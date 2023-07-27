#from matplotlib import pyplot as plt
import matplotlib
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42

from matplotlib import pyplot as plt
import numpy as np

#plt.style.use("fivethirtyeight")

labels = ['1-25%', '25-50%', '50-75%', '75-100%']

random_alg = [0.495543, 0.4986115, 0.4998865, 0.501127]

secretary_alg = [0.7786105, 0.728244, 0.7092455, 0.6872025]

vm = [0.8173205, 0.8134135, 0.841269, 0.850518]

rvm = [0.764642, 0.7441455, 0.7993805, 0.8348235]

hybrid_vm = [0.8700595, 0.864742, 0.862402, 0.862873]

hybrid_rvm = [0.7819675, 0.7595135, 0.7899015, 0.8288905]


##Line plots
optimal_vm = [1, 0.9999525, 0.9947175, 0.940099]

optimal_rvm = [1, 1, 0.9994385, 0.983438]

optimal_hybrid_vm = [0.88834, 0.8662965, 0.862567, 0.8630105]

optimal_hybrid_rvm = [0.8497845, 0.7842465, 0.82091, 0.881733]

optimal = [1, 1, 0.9991275, 0.9784395]



x = np.arange(len(labels))  # the label locations
width = 0.1  # the width of the bars

fig, ax = plt.subplots()

rects1 = ax.bar(x - 3 * width, random_alg, width, label='Random', edgecolor = 'k', color = 'gray')
rects2 = ax.bar(x - 2 * width, secretary_alg, width, label='MC-Secretary', edgecolor = 'k', color = 'wheat')
rects3 = ax.bar(x - width, vm, width, label='VM', edgecolor = 'k', color = 'peru')
rects4 = ax.bar(x, rvm, width, label='RVM', edgecolor = 'k', color = 'darkred')
rects5 = ax.bar(x + width, hybrid_vm, width, label='HYBRID-VM', edgecolor = 'k', color = 'seagreen')
rects6 = ax.bar(x + 2*width, hybrid_rvm, width, label='HYBRID-RVM', edgecolor = 'k', color = 'navy')

#rects7 = ax.bar(x + 3*width, optimal, width, label='Optimal', edgecolor = 'k', color = 'black')

plt.plot(x, optimal, marker = 'o',  label="Optimal", color = 'black', linewidth = 1)

# plt.plot(x, optimal_vm, marker = '*',  label="Optimal-VM", color = 'peru', linewidth = 1)
# plt.plot(x, optimal_rvm, marker = 'o',  label="Optimal-RVM", color = 'seagreen', linewidth = 1)
# plt.plot(x, optimal_hybrid_vm, marker = 'v',  label="Optimal-HYBRID-VM", color = 'darkred', linewidth = 1)
# plt.plot(x, optimal_hybrid_rvm, marker = 's',  label="Optimal-HYBRID-RVM", color = 'navy', linewidth = 1)



ax.set_ylabel('Mean Hit Ratio')
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