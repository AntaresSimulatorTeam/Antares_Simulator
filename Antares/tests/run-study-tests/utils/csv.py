import pandas


def read_csv(file_name):
    def cols(col_name):
        # Ignore columns with an empty name, labelled "Unammed.N" by pandas
        return (col_name not in ['Area', 'system', 'annual', 'monthly', 'weekly', 'daily', 'hourly']) and "Unnamed" not in col_name

    ignore_rows = [0,1,2,3,5,6]
    return pandas.read_csv(file_name,
                        skiprows=ignore_rows,
                        sep='\t',
                        usecols=cols,
                        low_memory=False,
                        dtype=float)