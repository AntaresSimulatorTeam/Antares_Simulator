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
