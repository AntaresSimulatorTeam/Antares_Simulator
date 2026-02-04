import math
from pathlib import Path

from check_decorators.print_name import printNameDecorator
from check_on_results.check_general import check_interface
from utils.find_reference import find_reference_folder
from utils.find_output import find_dated_output_folder, find_simulation_folder
from utils.assertions import check
from check_on_results.mps_utils import *

tol = 1e-12
@printNameDecorator
class compare_mps_files(check_interface):
    def __init__(self, study_path):
        super().__init__(study_path)
        self.study_path = study_path
        self.ref_folder = None
        self.dated_output_folder = None

    def find_folders_to_compare(self):
        # Retrieve the full path of 'reference' folder
        self.ref_folder = Path(find_reference_folder(self.study_path))

        # Folder of results (of which content is compared to content of reference folder)
        # ... of form yyyymmdd-hhmm<mode< (ex : 20230105-0944eco)
        self.dated_output_folder = Path(find_dated_output_folder(self.study_path))

    def run(self):
        self.find_folders_to_compare()

        ref_mps_files = list(self.ref_folder.glob('*.mps'))
        assert ref_mps_files, "Couldn't find reference MPS files"

        mps_files = list(self.dated_output_folder.glob('**/*.mps'))
        assert mps_files, "Couldn't find actual output MPS files"

        list_of_pairs = [(mps_ref, mps) for mps_ref in ref_mps_files for mps in mps_files if mps_ref.name == mps.name]
        for pair in list_of_pairs:
            ref_mps = pair[0]
            out_mps = pair[1]

            if not self.compare_files(pair, ref_mps, out_mps):
                print(f"Difference between files {pair[0]} and {pair[1]}")
                self.compare_models(pair, str(ref_mps), str(out_mps))

    def compare_models(self, pair, ref_mps, out_mps):
        ref_model = load_problem(ref_mps)
        out_model = load_problem(out_mps)
        

        # problem type
        self.checkProblemType(pair, ref_model, out_model)
        #compare nb variables
        self.compare_variable_numbers(pair, ref_model, out_model)

        #compare nb constraints
        self.compare_constraint_numbers(pair, ref_model, out_model)
        
        #compare variable
        # 
        self.compare_variables(pair, ref_model, out_model)

     
        # Compare constraint bounds
        self.compare_constraint_properties(pair, ref_model, out_model)
        # Compare constraint matrix
        self.compare_matrices(pair, ref_model, out_model)

    def checkProblemType(self, pair, ref_model, out_model):
        ref_is_lp = self.is_lp(ref_model)
        out_is_lp = self.is_lp(out_model)
        check(ref_is_lp == out_is_lp,
              f"Difference in problem type (MIP/LP) between files {pair[0]} and {pair[1]}")

    def is_lp(self, model):
        is_lp = True
        for var in model.get_variables():
            if var.is_integral:
                is_lp = False
                break
        
        return is_lp
    def check_numbers(self, left, right, msg:str):
        if math.isinf(left):
            check(math.isinf(right), msg)
        else:
            check(abs(left-right)< tol, msg)

    def compare_variables(self, pair, ref_model, out_model):
        ref_vars = extract_variables(ref_model)
        out_vars = extract_variables(out_model)
        for ref_var, out_var in zip(ref_vars, out_vars):
            check(ref_var['name'] == out_var['name'],
                  f"Difference in variable name {ref_var['name']} != {out_var['name']} between files {pair[0]} and {pair[1]}")
            check(ref_var['is_integral'] == out_var['is_integral'],
                  f"Difference in type of variable {ref_var['is_integral']} != {out_var['is_integral']} between files {pair[0]} and {pair[1]}")
            self.check_numbers(ref_var['xmin'] , out_var['xmin'] ,
                  f"Difference in lower bound of variable {ref_var['name']}: {ref_var['xmin']} != {out_var['xmin']} between files {pair[0]} and {pair[1]}")
            self.check_numbers(ref_var['xmax'] , out_var['xmax'],
                  f"Difference in upper bound of variable {ref_var['name']}: {ref_var['xmax']} != {out_var['xmax']} between files {pair[0]} and {pair[1]}")
            self.check_numbers(ref_var['cost'] , out_var['cost'] ,
                  f"Difference in cost of variable {ref_var['name']}: {ref_var['cost']} != {out_var['cost']} between files {pair[0]} and {pair[1]}")
    
    



    def compare_constraint_properties(self, pair, ref_model, out_model):
                # get_constraint_bounds Returns dict{ constr_name : (sense, rhs) }
        ref_constr_bounds = get_constraint_bounds(ref_model)
        out_constr_bounds = get_constraint_bounds(out_model)
        check(ref_constr_bounds.keys() == out_constr_bounds.keys(),
      f"Constraint sets differ between files {pair[0]} and {pair[1]}")

        for name, (ref_sense, ref_rhs) in ref_constr_bounds.items():
            out_sense, out_rhs = out_constr_bounds[name]

            self.check_numbers(ref_sense , out_sense,
                f"Difference in constraint sense for {name}")

            self.check_numbers(ref_rhs , out_rhs,
                f"Difference in constraint rhs for {name}: {ref_rhs} != {out_rhs}, between files {pair[0]} and {pair[1]}")

    def compare_matrices(self, pair, ref_model, out_model):
        # Returns { constraint_name : { variable_name : coefficient } }
        ref_constr_matrix = get_constraint_matrix(ref_model)
        out_constr_matrix = get_constraint_matrix(out_model)

        # Check same constraint set
        check(ref_constr_matrix.keys() == out_constr_matrix.keys(),
            f"Difference in constraint set between files {pair[0]} and {pair[1]}")

        for constr_name, ref_row in ref_constr_matrix.items():
            out_row = out_constr_matrix[constr_name]

            # Check same variable set for this constraint
            check(ref_row.keys() == out_row.keys(),
                f"Difference in variables for constraint {constr_name} "
                f"between files {pair[0]} and {pair[1]}")

            # Check coefficients
            for var_name, ref_coef in ref_row.items():
                out_coef = out_row[var_name]

                self.check_numbers(ref_coef , out_coef,
                    f"Difference in coefficient for constraint {constr_name}, "
                    f"variable {var_name} between files {pair[0]} and {pair[1]}")

    def compare_constraint_numbers(self, pair, ref_model, out_model):
        ref_nb_constrs = ref_model.num_constraints
        out_nb_constrs = out_model.num_constraints
        check(ref_nb_constrs == out_nb_constrs,
              f"Difference in number of constraints between files {pair[0]} and {pair[1]}")

    def compare_variable_numbers(self, pair, ref_model, out_model):
        ref_nb_vars = ref_model.num_variables
        out_nb_vars = out_model.num_variables
        check(ref_nb_vars == out_nb_vars,
              f"Difference in number of variables between files {pair[0]} and {pair[1]}")

        

    def compare_files(self, pair, ref_mps, out_mps):
        ref_content = open(ref_mps).read()
        output_content = open(out_mps).read()
        return ref_content == output_content


    def name(self):
        return "mps compare"