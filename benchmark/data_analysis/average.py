import os
import glob
import pandas as pd

path = '/home/abruno/LZ-XOR/benchmark/results'
pathlist = sorted(os.listdir(path))

for filename in pathlist:
    # print(filename)
    df = pd.read_csv(path + '/' + filename)
    alg = filename.split('-')[0]
    dataset = filename.split('-')[1].split('.')[0]
    print(alg + ',' + dataset + ',' +
          str(round(df['compr_speed'].mean(),2)) + ',' + str(round(df['decompr_speed'].mean(),2)))
