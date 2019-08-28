import sys

import pandas as pd

if (__name__ == "__main__"):
    df = pd.read_csv(sys.argv[1])
    n = int(sys.argv[2])
    print("Mean: ", df.distance.mean() / n)
