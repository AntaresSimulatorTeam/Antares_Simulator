from check_on_results.check_general import check_interface


class output_compare(check_interface):
    def need_output_results(self):
        return True

    def run(self):
        # To be done
        print("running check : %s" % self.__class__.__name__)




