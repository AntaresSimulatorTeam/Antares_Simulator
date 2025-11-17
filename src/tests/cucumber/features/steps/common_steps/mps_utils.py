import pulp

def load_problem(path):
    return pulp.LpProblem.fromMPS(path)[1]

def get_var_bounds(model):
    return {
        v.name: (v.lowBound, v.upBound)
        for v in model.variables()
    }

def get_objective_coeffs(model):
     return {v.name: coef for v, coef in model.objective.items()}

def get_constraint_bounds(model):
    info = {}
    for name, c in model.constraints.items():
        sense = c.sense     # -1: <=, 0: =, 1: >=
        rhs = -c.constant   # PuLP stores RHS as negative constant
        info[name] = (sense, rhs)
    return info

def get_constraint_matrix(model):
    mat = {}
    for cname, c in model.constraints.items():
        mat[cname] = {v.name: coef for v, coef in c.items()}
    return mat

