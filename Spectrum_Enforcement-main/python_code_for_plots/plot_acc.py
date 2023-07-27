import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

from matplotlib import pyplot as plt
import numpy as np

#plt.style.use("fivethirtyeight")

labels = ['1-25%', '25-50%', '50-75%', '75-100%']

random_alg = [0.6629765, 0.632515, 0.621629, 0.609124]

secretary_alg = [0.8994245, 0.9210075, 0.906838, 0.8695355]

secretary_rr = [0.832145, 0.783212, 0.73215, 0.6910405]

vm = [0.90036, 0.90619, 0.8548265, 0.7869295]

rvm = [0.9002685, 0.91528, 0.8633105, 0.787044]

hybrid_vm = [0.9286405, 0.931837, 0.9265685, 0.8855995]

hybrid_rvm = [0.9100275, 0.9183895, 0.9177635, 0.8799815]





x = np.arange(len(labels))  # the label locations
width = 0.1  # the width of the bars

fig, ax = plt.subplots()

rects1 = ax.bar(x - 4 * width, random_alg, width, label='Random', edgecolor = 'k', color = 'gray')
rects2 = ax.bar(x - 3 * width, secretary_rr, width, label='MC-Secretary-RR', edgecolor = 'k', color = 'rosybrown')
rects3 = ax.bar(x - 2 * width, secretary_alg, width, label='MC-Secretary', edgecolor = 'k', color = 'wheat')
rects4 = ax.bar(x - width, vm, width, label='VM', edgecolor = 'k', color = 'peru')
rects5 = ax.bar(x, rvm, width, label='RVM', edgecolor = 'k', color = 'darkred')
rects6 = ax.bar(x + width, hybrid_vm, width, label='HYBRID-VM', edgecolor = 'k', color = 'seagreen')
rects7 = ax.bar(x + 2*width, hybrid_rvm, width, label='HYBRID-RVM', edgecolor = 'k', color = 'navy')





ax.set_ylabel('Mean Accuracy')
ax.set_xlabel('Range of k (% of volunteers to be selected)')
#ax.set_title('Scores by group and gender')
ax.set_xticks(x)
ax.set_xticklabels(labels)

# Put a legend to the right of the current axis
#ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15),fancybox=True, ncol=4)
ax.tick_params(axis="y", direction='in', length=8)

ax2 = ax.secondary_yaxis("right")
ax2.tick_params(axis="y", direction="out", length=8)

ax.set_ylim([0,1.0])
fig.tight_layout()

#ax.grid()
plt.show()

#ages_x = [25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35]

#dev_y = [38496, 42000, 46752, 49320, 53200,
        # 56000, 62316, 64928, 67317, 68748, 73752]
				 
#plt.plot(ages_x, dev_y, color="#444444", label="All Devs")

# py_dev_y = [45372, 48876, 53850, 57287, 63016,
#             65998, 70003, 70000, 71496, 75370, 83640]
# plt.plot(ages_x, py_dev_y, color="#008fd5", label="Python")

# js_dev_y = [37810, 43515, 46823, 49293, 53437,
#             56373, 62375, 66674, 68745, 68746, 74583]
# plt.plot(ages_x, js_dev_y, color="#e5ae38", label="JavaScript")

# plt.legend()

# plt.title("Median Salary (USD) by Age")
# plt.xlabel("Range of k")
# plt.ylabel("Mean Hit Ratio")

# plt.tight_layout()

# plt.show()