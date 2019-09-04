import sys

import pandas as pd


if (__name__ == "__main__"):
    prefix  = sys.argv[1]
    n       = int(sys.argv[2])
    smin    = int(sys.argv[3])
    smax    = int(sys.argv[4])
    outfile = "/tmp/_orm_tmp.csv";
    if (len(sys.argv) > 5):
        outfile = sys.argv[5]

    df = pd.DataFrame(index=range(smin,smax+1))
    df.index.name = "overlap"
    df["aed"]   = pd.np.nan
    df["alpha"] = pd.np.nan

    for s in range(smin, smax+1):
        filename = prefix + str(s) + ".csv"
        tmp = pd.read_csv(filename)
        df.at[s] = (tmp.distance.mean(), tmp.distance.mean()/n)
    
    df.to_csv(outfile)
        
