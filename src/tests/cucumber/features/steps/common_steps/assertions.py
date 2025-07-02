# Custom assertions

def assert_double_close(expected, actual, relative_tolerance, message_prefix="Compared"):
    assert abs((actual - expected) / max(1e-6, abs(expected))) <= relative_tolerance, \
     f"{message_prefix} values are not close: expected = {expected} ; actual = {actual}"