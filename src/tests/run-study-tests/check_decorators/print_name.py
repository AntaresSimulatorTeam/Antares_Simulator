def printNameDecorator(check):
    run = check.run
    def decorated_run(self):
        print("Running check : %s" % check.name(self))
        run(self)

    check.run = decorated_run
    return check