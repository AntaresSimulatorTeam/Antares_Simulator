from os import environ
import pandas

def raise_assertion(message):
    test_name = environ.get('PYTEST_CURRENT_TEST').split('::')[-1].split(' ')[0]
    raise AssertionError("%s::%s" % (test_name, message))
