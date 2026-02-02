from ortools.linear_solver.python import model_builder


def load_problem(path: str):
    """
    Loads an MPS model using OR-Tools.
    """
    model = model_builder.ModelBuilder()
    model.import_from_mps_file(path)
    return model


def get_objective_coeffs(model):
    """
    Returns { varname : objective_coefficient }
    """
    coeffs = {}
    obj_expr = model.objective_expression()  # LinearExpr / FlatExpr
    for var, coef in zip(obj_expr.vars, obj_expr.coeffs):
        coeffs[var.name] = coef

    return coeffs


def get_constraint_bounds(model):
    """
    Returns { constr_name : (sense, rhs) }
    sense = -1 (<=), 0 (=), +1 (>=)
    rhs   = float
    """
    info = {}

    for c in model.get_linear_constraints():
        lb = c.lower_bound
        ub = c.upper_bound

        if lb == ub:
            sense = 0  # equality
            rhs = ub
        elif lb != float('-inf') and ub == float('inf'):
            sense = 1  # >=
            rhs = lb
        elif ub != float('inf') and lb == float('-inf'):
            sense = -1  # <=
            rhs = ub
        else:
            sense = -1
            rhs = ub

        info[c.name] = (sense, rhs)

    return info


def get_constraint_matrix(model):
    """
    Returns { constraint_name : { variable_name : coefficient } }
    """
    A = {}

    for c, expr in zip(model.get_linear_constraints(), model.get_linear_constraint_expressions()):
        row = {}
        for var, coef in zip(expr.vars, expr.coeffs):
            if coef != 0:
                row[var.name] = coef
        A[c.name] = row

    return A


def extract_variables(model: model_builder.ModelBuilder):
    """
    Returns a list of:
    { 'name': varname, 'is_integral': is_integral, 'xmin': lb, 'xmax': ub, 'cost': objective_coef }
    """
    obj = get_objective_coeffs(model)
    vars_table = []

    for var in model.get_variables():
        vars_table.append({
            'name': var.name,
            'is_integral': var.is_integral,
            'xmin': var.lower_bound,
            'xmax': var.upper_bound,
            'cost': obj.get(var.name, 0.0),
        })

    return vars_table
