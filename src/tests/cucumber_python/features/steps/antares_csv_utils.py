import pandas


def read_csv(file_name):
    ignore_rows = [0,1,2,3,5,6]
    return pandas.read_csv(file_name,
                        skiprows=ignore_rows,
                        sep='\t',
                        low_memory=False)