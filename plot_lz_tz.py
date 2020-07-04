import os
import glob
import pandas as pd
from pylab import *
import matplotlib.pyplot as plt

path = 'res_lz_tz'


v = 0
i = 0
fig, axs = plt.subplots(nrows=7, ncols=2, sharex=True)


pathlist = sort(os.listdir(path))

for filename in pathlist:
    if(filename != '.DS_Store'):
        print(filename)
        df = pd.read_csv(path+'/'+filename)

        x = df['i']
        y = df['freq']

        axs[v, i].bar(x, y)
        axs[v, i].set_title(filename)

        if i == 0:
            i = i + 1
        elif i == 1:
            v = v + 1
            i = 0


subplots_adjust(left=0.05, bottom=0.05, right=0.95,
                top=0.95, wspace=None, hspace=0.4)

plt.show()
# plt.savefig('OFFSET_IMG/' + filename + '.png', dpi=300)
# plt.close()
