import matplotlib.pyplot as plt
import numpy as np
import subprocess

# test on DIM_SIZE=1000
simple_time = float(subprocess.check_output(['bin/matmul', 'simple', '1000', '1']))
optimized_time = float(subprocess.check_output(['bin/matmul', 'optimized', '1000', '1']))
multithread_times = []

MAX_THREADS = 20

for i in range(1, MAX_THREADS + 1):
    output = subprocess.check_output(['bin/matmul', 'multi-threaded', '1000', str(i)])
    multithread_times.append(float(output.splitlines()[-1]))

# print(simple_time)
# print(optimized_time)
# print(multithread_times)

plt.figure(figsize=((14, 8)))
plt.plot(np.arange(1, MAX_THREADS + 1), multithread_times, 
        label="multithreaded optimized algorithm")
plt.plot(np.arange(1, MAX_THREADS + 1), [simple_time for i in range(MAX_THREADS)],
        label="simple algorithm")
plt.plot(np.arange(1, MAX_THREADS + 1), [optimized_time for i in range(MAX_THREADS)],
        label="optimized (sibgle thread) algorithm")
plt.xlabel("Number of threads")
plt.ylabel("Time (seconds)")
plt.legend()
plt.title("Time of matrix multiplication for different algorithms (DIM_SIZE=1000)")
plt.grid(True)
plt.savefig("graphs/times.png")

plt.figure(figsize=((14, 8)))
plt.plot(np.arange(1, MAX_THREADS + 1), simple_time / np.array(multithread_times),
        label="acceleration")
plt.xlabel("Number of threads")
plt.ylabel("Acceleration")
plt.legend()
plt.title("Acceleration of multi-threaded algorithm over simple algorithm")
plt.grid(True)
plt.savefig("graphs/accelerations.png")