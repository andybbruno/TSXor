import os
import glob
import pandas as pd
from pylab import *
import matplotlib.pyplot as plt
import random

path = 'res_lz_tz'

v = 0
i = 0
fig, axs = plt.subplots(nrows=7, ncols=2, sharex=True)

random.seed(0)
all_colors = list(plt.cm.colors.cnames.keys())
c = random.choices(all_colors, k=8)

pathlist = sort(os.listdir(path))

for filename in pathlist:
    if(filename != '.DS_Store'):
        print(filename)
        df = pd.read_csv(path+'/'+filename)

        x = df['i']
        y = df['freq']

        axs[v, i].bar(x, y, color=c, edgecolor='lightgray')
        axs[v, i].ticklabel_format(style="sci", scilimits=(0, 0), axis="y")

        title = filename.split('.')[0].split('_')[0] + " " + filename.split('.')[0].split('_')[1]
        axs[v, i].set_title(title)
        axs[v, i].grid(axis='x', linestyle=':')

        if i == 0:
            i = i + 1
        elif i == 1:
            v = v + 1
            i = 0

subplots_adjust(left=0.3, bottom=0.05, right=0.7,
                top=0.95, wspace=None, hspace=0.6)

plt.xticks(x)
plt.show()
# plt.savefig('OFFSET_IMG/' + filename + '.png', dpi=300)
# plt.close()