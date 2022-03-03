from check_on_results.check_general import check_interface

class reservoir_levels(check_interface):
    def need_output_results(self):
        return False

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)