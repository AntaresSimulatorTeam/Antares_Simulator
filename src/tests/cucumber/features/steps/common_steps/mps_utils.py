import pulp


def load_problem(path):
    return pulp.LpProblem.fromMPS(path)[1]


def get_objective_coeffs(model):
    return {v.name: coef for v, coef in model.objective.items()}


def get_constraint_bounds(model):
    info = {}
    for name, c in model.constraints.items():
        sense = c.sense  # -1: <=, 0: =, 1: >=
        rhs = -c.constant  # PuLP stores RHS as negative constant
        info[name] = (sense, rhs)
    return info


def get_constraint_matrix(model):
    mat = {}
    for cname, c in model.constraints.items():
        mat[cname] = {v.name: coef for v, coef in c.items()}
    return mat


def extract_variables(model):
    """
    Returns a list of dicts:
    [{'name': varname, 'xmin': lowBound, 'xmax': upBound, 'cost': objective_coef}, ...]
    """
    obj = get_objective_coeffs(model)
    vars_table = []
    for v in model.variables():
        vars_table.append({
            'name': v.name,
            'xmin': v.lowBound,
            'xmax': v.upBound,
            'cost': obj.get(v.name, 0)
        })
    return vars_table
