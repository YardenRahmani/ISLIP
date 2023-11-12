import subprocess
import os
import statistics
import matplotlib.pyplot as plt

def clean():
    os.remove("temp.txt")
    for i in range(1,18):
        os.remove("out_Script"+str(i)+".txt")
        os.remove(str(i)+".log")

def draw_time_graphs(p):
    print("Creating time statistics graphs")
    runMean = []
    runMedian = []
    runMax = []
    runStdv = []
    for i in range(1,18):
        with open("out_script"+str(i) + ".txt", "r") as outFile:
           lines = outFile.read().split("\n")
           packages_time = []
           curMax = 0
           for line in lines[0:(len(lines)-1)]:
                tokens = line.split()
                package_time = float(tokens[3]) - float(tokens[0])
                if package_time > curMax :
                    curMax = package_time
                packages_time.append(package_time)
        runMax.append(curMax)
        runMean.append(statistics.mean(packages_time))
        runMedian.append(statistics.median(packages_time))
        runStdv.append(statistics.stdev(packages_time))
    fig, ax1 = plt.subplots()
    ax1.plot(p[0:8], runMean[0:8], label = 'mean')
    ax1.plot(p[0:8], runMedian[0:8], label = 'median')
    ax1.plot(p[0:8], runMax[0:8], label = 'max')
    ax1.plot(p[0:8], runStdv[0:8], label = 'stdv')
    ax1.set_xlabel('p')
    ax1.set_ylabel('Time steps')
    ax1.set_title('Run 1-8 package in queue time statistics')
    ax1.grid(True)
    ax1.legend()
    fig, ax2 = plt.subplots()
    ax2.plot(p[8:11], runMean[8:11], label = 'mean')
    ax2.plot(p[8:11], runMedian[8:11], label = 'median')
    ax2.plot(p[8:11], runMax[8:11], label = 'max')
    ax2.plot(p[8:11], runStdv[8:11], label = 'stdv')
    ax2.set_xlabel('p')
    ax2.set_ylabel('Time steps')
    ax2.set_title('Run 9-11 package in queue time statistics')
    ax2.grid(True)
    ax2.legend()
    fig, ax3 = plt.subplots()
    ax3.plot(p[11:14], runMean[11:14], label = 'mean')
    ax3.plot(p[11:14], runMedian[11:14], label = 'median')
    ax3.plot(p[11:14], runMax[11:14], label = 'max')
    ax3.plot(p[11:14], runStdv[11:14], label = 'stdv')
    ax3.set_xlabel('p')
    ax3.set_ylabel('Time steps')
    ax3.set_title('Run 12-14 package in queue time statistics')
    ax3.grid(True)
    ax3.legend()
    fig, ax4 = plt.subplots()
    ax4.plot(p[14:17], runMean[14:17], label = 'mean')
    ax4.plot(p[14:17], runMedian[14:17], label = 'median')
    ax4.plot(p[14:17], runMax[14:17], label = 'max')
    ax4.plot(p[14:17], runStdv[14:17], label = 'stdv')
    ax4.set_xlabel('p')
    ax4.set_ylabel('Time steps')
    ax4.set_title('Run 15-17 package in queue time statistics')
    ax4.grid(True)
    ax4.legend()
    
def draw_size_graphs(p):
    print("Creating size statistics graphs")
    runMean = []
    runMax = []
    runStdv = []
    for i in range(1,18):
        with open(str(i)+".log", "r") as logFile:
            lines = logFile.read().split("\n")
            queueSize = []
            curMax = 0
            for line in lines[0:(len(lines)-1)]:
                readValue = int((line.split())[3])
                if readValue > curMax:
                    curMax = readValue
                queueSize.append(readValue)
        runMean.append(statistics.mean(queueSize))
        runMax.append(curMax)
        runStdv.append(statistics.stdev(queueSize))
    fig, ax1 = plt.subplots()
    ax1.plot(p[0:8], runMean[0:8], label = 'mean')
    ax1.plot(p[0:8], runMax[0:8], label = 'max')
    ax1.plot(p[0:8], runStdv[0:8], label = 'stdv')
    ax1.set_xlabel('p')
    ax1.set_ylabel('Queue size')
    ax1.set_title('Run 1-8 queue size statistics')
    ax1.grid(True)
    ax1.legend()
    fig, ax2 = plt.subplots()
    ax2.plot(p[8:11], runMean[8:11], label = 'mean')
    ax2.plot(p[8:11], runMax[8:11], label = 'max')
    ax2.plot(p[8:11], runStdv[8:11], label = 'stdv')
    ax2.set_xlabel('p')
    ax2.set_ylabel('Queue size')
    ax2.set_title('Run 9-11 queue size statistics')
    ax2.grid(True)
    ax2.legend()
    fig, ax3 = plt.subplots()
    ax3.plot(p[11:14], runMean[11:14], label = 'mean')
    ax3.plot(p[11:14], runMax[11:14], label = 'max')
    ax3.plot(p[11:14], runStdv[11:14], label = 'stdv')
    ax3.set_xlabel('p')
    ax3.set_ylabel('Queue size')
    ax3.set_title('Run 12-14 queue size statistics')
    ax3.grid(True)
    ax3.legend()
    fig, ax4 = plt.subplots()
    ax4.plot(p[14:17], runMean[14:17], label = 'mean')
    ax4.plot(p[14:17], runMax[14:17], label = 'max')
    ax4.plot(p[14:17], runStdv[14:17], label = 'stdv')
    ax4.set_xlabel('p')
    ax4.set_ylabel('Queue size')
    ax4.set_title('Run 15-17 queue size statistics')
    ax4.grid(True)
    ax4.legend()
    clean()
    plt.show()

def run_simulations(p):
    for i in range(1,18):
        if i >= 15:
            N = 32
        else:
            N = 16
        if i >= 12 and i <= 14:
            k = 3
        else:
            k = 1
        print(f"Running run #{i}")
        with open("temp.txt", "w") as infile:
            if i >= 9 and i <= 11:
                subprocess.run(["tr_gen.exe", str(N), str(1000), str(190), str(p[i-1]), "-d"], stdout=infile)
            else:
                subprocess.run(["tr_gen.exe", str(N), str(1000), str(190), str(p[i-1])], stdout=infile)
        with open("temp.txt", "r") as infile:
            with open("out_Script" + str(i) +".txt", "w") as outfile:
                subprocess.run(["islip.exe", str(N), str(k), str(i)], stdin=infile, stdout = outfile)
        print(f"Finished run #{i}")

p = [0.6, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 0.99, 0.9, 0.95, 0.99, 0.9, 0.95, 0.99, 0.9, 0.95, 0.99]
run_simulations(p)
draw_time_graphs(p)
draw_size_graphs(p)