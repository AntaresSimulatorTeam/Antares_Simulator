import sympy as sp
import re


def parse_linear_expression(expression: str):
    locals_ = {
        "ceil": sp.ceiling,
        "floor": sp.floor,
        "max": sp.Max,
        "min": sp.Min,
    }

    # 1️⃣ Find dotted variable names
    dotted_vars = re.findall(
        r"[A-Za-z_]\w*(?:\.[A-Za-z_]\w*)+", expression
    )

    # 2️⃣ Build replacement maps
    name_to_sym = {}
    sym_to_name = {}

    rewritten_expr = expression
    for i, name in enumerate(sorted(set(dotted_vars), key=len, reverse=True)):
        sym = f"__v{i}"
        name_to_sym[name] = sym
        sym_to_name[sym] = name
        rewritten_expr = rewritten_expr.replace(name, sym)

    # 3️⃣ Register symbols
    for sym in sym_to_name:
        locals_[sym] = sp.Symbol(sym)

    # 4️⃣ Parse
    expr = sp.sympify(rewritten_expr, locals=locals_)
    expr = sp.expand(expr)

    coeffs: dict[str, float] = {}

    # 5️⃣ Extract coefficients
    for var in expr.free_symbols:
        # if not expr.is_linear(var):
        #     raise ValueError(f"Expression is not linear in {var}")

        coeff = sp.simplify(expr.coeff(var))

        if coeff.free_symbols:
            raise ValueError(f"Non-scalar coefficient for {var}")

        value = float(coeff)
        if value != 0.0:
            original_name = sym_to_name.get(str(var), str(var))
            coeffs[original_name] = value

    # 6️⃣ Reject constant term
    remainder = expr
    for var in expr.free_symbols:
        remainder -= expr.coeff(var) * var

    if remainder != 0:
        raise ValueError(f"Constant term detected: {remainder}")

    return coeffs
