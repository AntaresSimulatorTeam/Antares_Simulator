# Custom assertions

def check_double_close(expected, actual, relative_tolerance, message_prefix="Compared"):
    if not (abs((actual - expected) / max(1e-6, abs(expected))) <= relative_tolerance):
        return f"{message_prefix} values are not close: expected = {expected} ; actual = {actual}"
    return None


def assert_double_close(expected, actual, relative_tolerance, message_prefix="Compared"):
    error = check_double_close(expected, actual, relative_tolerance, message_prefix)
    assert error is None, error
