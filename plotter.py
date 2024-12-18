import numpy as np
import matplotlib.pyplot as plt

# sizes
x = np.array([4, 8, 20, 100, 1000, 5000, 10000, 25000, 75000, 100000])  # Generate 100 points between 10^0.1 and 10^2

# naive
naive_alloc = np.array([
    7.23E-06,
    6.12E-06,
    8.22E-06,
    3.77E-05,
    1.79E-03,
    3.82E-02,
    1.48E-01,
    9.28E-01,
    8.09E+00,
    1.44E+01
])
naive_proc = np.array([
    1.48E-06,
    3.70E-06,
    1.84E-05,
    4.42E-04,
    3.85E-02,
    9.55E-01,
    3.81E+00,
    2.36E+01,
    2.09E+02,
    3.71E+02
])
naive_btrack = np.array([
    1.17E-06,
    1.05E-06,
    1.36E-06,
    3.29E-06,
    1.62E-05,
    7.41E-05,
    1.96E-04,
    6.40E-04,
    2.09E-03,
    2.80E-03
])
naive_tot = naive_alloc + naive_proc + naive_btrack 

# blocked
blocked_alloc = np.array([
    2.94E-06,
    3.06E-06,
    4.24E-06,
    2.51E-05,
    1.60E-03,
    3.74E-02,
    1.47E-01,
    9.18E-01,
    8.07E+00,
    1.44E+01
])
blocked_proc = np.array([
    1.88E-06,
    1.81E-06,
    2.69E-06,
    2.03E-05,
    1.35E-03,
    3.59E-02,
    1.48E-01,
    1.06E+00,
    1.38E+01,
    2.88E+01
])
blocked_btrack = np.array([
    3.60E-07,
    3.91E-07,
    1.20E-06,
    2.50E-06,
    9.69E-06,
    7.02E-05,
    2.24E-04,
    7.30E-04,
    2.07E-03,
    2.78E-03
])
blocked_tot = blocked_alloc + blocked_proc + blocked_btrack 

# antidiag
antidiag_alloc = np.array([
    3.24E-06,
    3.26E-06,
    4.45E-06,
    2.45E-05,
    1.69E-03,
    3.86E-02,
    1.48E-01,
    9.03E-01,
    8.03E+00,
    1.44E+01
])
antidiag_proc = np.array([
    1.80E-02,
    1.78E-02,
    1.76E-02,
    4.43E-02,
    6.11E-02,
    5.69E-02,
    1.48E-01,
    6.11E-01,
    3.97E+00,
    6.08E+00
])
antidiag_btrack = np.array([
    4.71E-07,
    5.01E-07,
    3.02E-06,
    5.97E-06,
    2.68E-05,
    1.24E-04,
    6.10E-04,
    1.20E-03,
    3.97E-03,
    5.24E-03
])
antidiag_tot = antidiag_alloc + antidiag_proc + antidiag_btrack 

# column
column_alloc = np.array([
    4.10E-06,
    3.06E-06,
    3.95E-06,
    2.52E-05,
    1.65E-03,
    3.85E-02,
    1.49E-01,
    9.02E-01,
    8.09E+00,
    1.44E+01
])
column_proc = np.array([
    1.90E-05,
    1.74E-05,
    1.67E-05,
    1.85E-04,
    2.48E-03,
    1.13E-02,
    3.12E-02,
    3.65E-01,
    4.01E+00,
    7.82E+00
])
column_btrack = np.array([
    3.90E-07,
    4.01E-07,
    1.34E-06,
    5.46E-06,
    2.70E-05,
    2.09E-04,
    3.51E-04,
    8.84E-04,
    4.54E-03,
    4.54E-03
])
column_tot = column_alloc + column_proc + column_btrack 

# cuda
cuda_alloc = np.array([
    1.05E+00,
    1.08E+00,
    1.05E+00,
    1.06E+00,
    1.03E+00,
    1.03E+00,
    1.06E+00,
    1.03E+00,
    1.07E+00,
    1.07E+00
])
cuda_proc = np.array([
    1.30E-04,
    1.97E-04,
    4.06E-04,
    1.74E-03,
    1.99E-02,
    1.15E-01,
    2.31E-01,
    5.28E-01,
    1.76E+00,
    2.76E+00
])
cuda_btrack = np.array([
    3.15E-06,
    3.57E-06,
    6.33E-06,
    3.30E-05,
    2.60E-03,
    6.01E-02,
    2.46E-01,
    1.50E+00,
    1.35E+01,
    2.40E+01
])
cuda_tot = cuda_alloc + cuda_proc + cuda_btrack 

# Create the blocked plot
plt.figure(figsize=(10, 5))
plt.loglog(x, naive_alloc / blocked_alloc, label='Allocation', color='blue', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_proc / blocked_proc, label='Processing', color='green', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_btrack / blocked_btrack, label='Backtrace', color='purple', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / blocked_tot, label='Total', color='red', linewidth=2, marker="o")
plt.axhline(y = 1, color = 'black', linestyle = (0, (5, 10))) 
plt.xlabel('DNA Size (bp)', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup of Smith-Waterman (Naive vs Blocked + Flags)', fontsize=14)
plt.legend()
plt.savefig('x_Blocked_Speedup.png', dpi=300, bbox_inches='tight')

# Create the antidiag plot
plt.figure(figsize=(10, 5))
plt.loglog(x, naive_alloc / antidiag_alloc, label='Allocation', color='blue', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_proc / antidiag_proc, label='Processing', color='green', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_btrack / antidiag_btrack, label='Backtrace', color='purple', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / antidiag_tot, label='Total', color='red', linewidth=2, marker="o")
plt.axhline(y = 1, color = 'black', linestyle = (0, (5, 10))) 
plt.xlabel('DNA Size (bp)', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup of Smith-Waterman (Naive vs OpenMP Anti-Diagonal)', fontsize=14)
plt.legend()
plt.savefig('x_AntiDiag_Speedup.png', dpi=300, bbox_inches='tight')

# Create the column plot
plt.figure(figsize=(10, 5))
plt.loglog(x, naive_alloc / column_alloc, label='Allocation', color='blue', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_proc / column_proc, label='Processing', color='green', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_btrack / column_btrack, label='Backtrace', color='purple', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / column_tot, label='Total', color='red', linewidth=2, marker="o")
plt.axhline(y = 1, color = 'black', linestyle = (0, (5, 10))) 
plt.xlabel('DNA Size (bp)', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup of Smith-Waterman (Naive vs OpenMP Column)', fontsize=14)
plt.legend()
plt.savefig('x_Column_Speedup.png', dpi=300, bbox_inches='tight')

# Create the cuda plot
plt.figure(figsize=(10, 5))
plt.loglog(x, naive_alloc / cuda_alloc, label='Allocation', color='blue', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_proc / cuda_proc, label='Processing', color='green', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_btrack / cuda_btrack, label='Backtrace', color='purple', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / cuda_tot, label='Total', color='red', linewidth=2, marker="o")
plt.axhline(y = 1, color = 'black', linestyle = (0, (5, 10))) 
#print(naive_proc / cuda_proc)
#print(max(naive_proc / cuda_proc))
#print(naive_tot / cuda_tot)
#print(max(naive_tot / cuda_tot))
plt.xlabel('DNA Size (bp)', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup of Smith-Waterman (Naive vs CUDA)', fontsize=14)
plt.legend()
plt.savefig('x_Cuda_Speedup.png', dpi=300, bbox_inches='tight')

# Create the comprehensive plot
plt.figure(figsize=(10, 5))
plt.loglog(x, naive_proc / blocked_proc, label='Blocked+Flags Processing', color='blue', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / blocked_tot, label='Blocked+Flags Total', color='blue', linewidth=1, marker="o")
plt.loglog(x, naive_proc / antidiag_proc, label='AntiDiag OpenMP Processing', color='red', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / antidiag_tot, label='AntiDiag OpenMP Total', color='red', linewidth=1, marker="o")
plt.loglog(x, naive_proc / column_proc, label='Column OpenMP Processing', color='purple', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / column_tot, label='Column OpenMP Total', color='purple', linewidth=1, marker="o")
plt.loglog(x, naive_proc / cuda_proc, label='CUDA Processing', color='green', linestyle='--', linewidth=1, marker="o")
plt.loglog(x, naive_tot / cuda_tot, label='CUDA Total', color='green', linewidth=1, marker="o")
plt.axhline(y = 1, color = 'black', linestyle = (0, (5, 10))) 
plt.xlabel('DNA Size (bp)', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Speedup of Smith-Waterman (Naive vs Optimized Implementations)', fontsize=14)
plt.legend()
plt.savefig('x_All_Speedup.png', dpi=300, bbox_inches='tight')
