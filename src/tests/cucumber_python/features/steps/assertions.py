def assert_double_close(expected, actual, relative_tolerance):
    assert abs((actual - expected) / expected) <= relative_tolerance