import sys
from os import listdir
from os.path import isfile, join
from collections import defaultdict

import pandas as pd

import matplotlib.pyplot as plt
import seaborn as sns
sns.set()

def print_usage():
    print("\nUsage:\n")
    print("\n\t olap_merge.py file_prefix m s_min s_max [out_file] \n")


if (__name__ == "__main__"):
    mypath = sys.argv[1]
    onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
    g_map = defaultdict(list)
    fpath = "ecoli_m128_w96_N10000.cfg.csv"

    for fpath in onlyfiles:
        parts = fpath.split("_")
        G = parts[0]
        N = int(parts[3][0:parts[3].find(".")][1:])
        m = int(parts[1][1:])
        w = int(parts[2][1:])

        t = (fpath, m, w, N)
        g_map[G].append(t)


    
    for k in g_map:
        ts = g_map[k]
        print("Genome: " + k)
        df = pd.DataFrame(columns=["AED","Alpha", "Overlap"])
        for t in ts:
            filename = mypath + t[0]
            m = t[1]
            w = t[2]
            tmp = pd.read_csv(filename)
            aed = tmp["distance"].mean()
            alpha = aed / m
            df = df.append({"AED" : aed, "Alpha" : alpha, "Overlap" : w}, ignore_index=True)
        df["Overlap"] = df["Overlap"].astype(pd.np.int64)
        df = df.sort_values(by="Overlap").set_index("Overlap")
        plt.plot(df.index, df["Alpha"], label=k)        
        print(df)

    plt.legend()
    plt.show()

    
    # if (len(sys.argv) < 5):
    #     print_usage()
    #     sys.exit(1)
    # prefix  = sys.argv[1]
    # n       = int(sys.argv[2])
    # smin    = int(sys.argv[3])
    # smax    = int(sys.argv[4])
    # outfile = "/tmp/_orm_tmp.csv";
    # if (len(sys.argv) > 5):
    #     outfile = sys.argv[5]

    # df = pd.DataFrame(index=range(smin,smax+1))
    # df.index.name = "overlap"
    # df["aed"]   = pd.np.nan
    # df["alpha"] = pd.np.nan

    # for s in range(smin, smax+1):
    #     filename = prefix + str(s) + ".csv"
    #     tmp = pd.read_csv(filename)
    #     df.at[s] = (tmp.distance.mean(), tmp.distance.mean()/n)
    
    # df.to_csv(outfile)
        
