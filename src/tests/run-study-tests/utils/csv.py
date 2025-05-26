import pandas


def read_csv(file_name, rows_to_skip):
    def cols(col_name):
        # Ignore columns with an empty name, labelled "Unammed.N" by pandas
        return (col_name not in ['Area', 'system', 'annual', 'monthly', 'weekly', 'daily', 'hourly']) and "Unnamed" not in col_name

    return pandas.read_csv(file_name,
                        skiprows=rows_to_skip,
                        sep='\t',
                        usecols=cols,
                        low_memory=False,
                        dtype=float)