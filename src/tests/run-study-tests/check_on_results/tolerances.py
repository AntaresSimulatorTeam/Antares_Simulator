import sys

def trim_digit_after_last_dot(s : str) -> str:
    '''
    Remove trailing digit after a dot.

    Examples
    >> trim_after_last_dot("OP.1")
       "OP"
    >> trim_after_last_dot("Hello !")
       "Hello !"
    >> trim_after_last_dot("Hello.")
       "Hello."
    >> trim_after_last_dot('OP. COST')
       OP. COST
    '''
    t = s.split('.')
    if t[-1].isdigit():
        return '.'.join(t[:-1])
    else:
        return s

class Tolerances():
    RTOL = {}
    ATOL = {}
    default_abs_tol = 0
    default_rel_tol = 1e-4

    def absolute(self, col_name):
        trimmed_col_name = trim_digit_after_last_dot(col_name)
        if trimmed_col_name in self.ATOL:
            return self.ATOL[trimmed_col_name]
        return self.default_abs_tol

    def relative(self, col_name):
        trimmed_col_name = trim_digit_after_last_dot(col_name)
        if trimmed_col_name in self.RTOL:
            return self.RTOL[trimmed_col_name]
        return self.default_rel_tol

    def set_relative(self, var_collection, value):
        for var in var_collection:
            self.RTOL[var] = value

    def set_absolute(self, var_collection, value):
        for var in var_collection:
            self.ATOL[var] = value

class Linux_tolerances(Tolerances):
    def __init__(self):
        self.RTOL = {"CO2 EMIS.": 1e-3, "FLOW LIN.": 1e-3, "UCAP LIN.": 1e-3, "H. INFL": 1e-3, "H. STOR": 1e-3,
                           "H. OVFL": 1e-3, "OV. COST": 1e-3, "LIGNITE": 1e-3, "CONG. FEE (ABS.)": 1e-3, "sb": 1e-3,
                           "MARG. COST": 1e-3, "DTG MRG": 1e-3, "BALANCE": 1e-3, "BASE": 1e-3, "MRG. PRICE": 1e-3,
                           "OP. COST": 1e-3, "SEMI BASE": 1e-3, "COAL": 1e-3, "MAX MRG": 1e-3, "UNSP. ENRG": 1e-3}
        self.ATOL = {"CO2 EMIS.": 1, "CONG. FEE (ALG.)": 1, "FLOW LIN.": 1, "UCAP LIN.": 1, "peak": 1, "PEAK": 1,
                           "H. INFL": 1, "H. STOR": 1, "HURDLE COST": 1, "H. OVFL": 1, "LOAD": 1, "CONG. FEE (ABS.)": 1,
                           "sb": 1, "MISC. DTG": 1, "DTG MRG": 1, "BALANCE": 1, "BASE": 1, "OP. COST": 1,
                           "SEMI BASE": 1, "COAL": 1, "p": 1, "MAX MRG": 1, "UNSP. ENRG": 1, "SOLAR": 1, "b": 1,
                           "NODU": 1, "H. ROR": 1}

class Win_tolerances(Tolerances):
    def __init__(self):
        pass


def get_tolerances():
    if sys.platform == "linux":
        return Linux_tolerances()
    elif sys.platform == "win32":
        return Win_tolerances()
    else:
        raise_assertion("Unknown OS")