import os
import glob
import pandas as pd
from pylab import *
import matplotlib.pyplot as plt
import random


path = 'res_times'


random.seed(4)
all_colors = list(plt.cm.colors.cnames.keys())
c = random.choices(all_colors, k=8)

v = 0
i = 0
fig, axs = plt.subplots(nrows=4, ncols=2, sharex=True)

pathlist = sort(os.listdir(path))

for filename in pathlist:
    if(filename != '.DS_Store'):
        print(filename)
        df = pd.read_csv(path+'/'+filename)

        x = df['Case']
        y = df['Percentage']

        axs[v,i].bar(x, y, color=c, edgecolor='lightgray')
        axs[v,i].set_ylim([0, 120])

        # axs[v].ticklabel_format(style="sci", scilimits=(0, 0), axis="y")

        if filename == "Z.txt":
            title = "AVERAGE"
        else:
            title = filename.split('.')[0]

        axs[v,i].set_title(title)
        axs[v,i].grid(axis='x', linestyle=':')

        rects = axs[v,i].patches
        labels = ["{:.2f}".format(a) for a in y]

        for rect, label in zip(rects, labels):
            height = rect.get_height()
            axs[v,i].text(rect.get_x() + rect.get_width() / 2, height, label,
                    ha='center', va='bottom')

        if i == 0:
            i = i + 1
        elif i == 1:
            v = v + 1
            i = 0


subplots_adjust(left=0.3, bottom=0.05, right=0.7,
                top=0.95, wspace=None, hspace=0.6)

# plt.xticks(x)

fig.text(0.26, 0.5, 'PERCENTAGE', va='center', rotation='vertical',fontsize=12)
plt.show()
# plt.savefig('OFFSET_IMG/' + filename + '.png', dpi=300)
# plt.close()
