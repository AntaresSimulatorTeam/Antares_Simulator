# Custom assertions

def assert_double_close(expected, actual, relative_tolerance):
    assert abs((actual - expected) / max(1e-6, expected)) <= relative_tolerance, \
        f"Values are not close: expected = {expected} ; actual = {actual}"