import matplotlib.pyplot as plt
import numpy as np

# 实验数据（来自您的排序实验输出）
n = np.array([10, 100, 1000, 2000, 5000, 10000, 100000])
bubble = np.array([45, 4950, 499500, 1999000, 12497500, 49995000, 4999950000])
merge = np.array([20, 548, 8699, 19405, 55281, 120547, 1536257])
quick = np.array([24, 636, 10741, 25386, 71926, 159356, 1987161])

# 创建图形
plt.figure(figsize=(8, 6))

# 绘制三条曲线（双对数坐标）
plt.loglog(n, bubble, 'o-', label='Bubble Sort', linewidth=2, markersize=6)
plt.loglog(n, merge, 's-', label='Merge Sort', linewidth=2, markersize=6)
plt.loglog(n, quick, '^-', label='Quick Sort', linewidth=2, markersize=6)

# 添加参考线 O(n²) 和 O(n log n) 用于对比
# 将参考线缩放到与数据可比较的范围
n_fit = np.logspace(1, 5, 100)
theoretical_n2 = 0.5 * n_fit**2   # 冒泡的理论曲线（仅示意）
theoretical_nlogn = n_fit * np.log2(n_fit) * 2  # 合并/快速的理论曲线

plt.loglog(n_fit, theoretical_n2, 'k--', alpha=0.5, label=r'$O(n^2)$ reference')
plt.loglog(n_fit, theoretical_nlogn, 'k:', alpha=0.5, label=r'$O(n \log n)$ reference')

# 图表装饰
plt.xlabel('Input size n (log scale)', fontsize=12)
plt.ylabel('Number of comparisons (log scale)', fontsize=12)
plt.title('Comparison Counts vs. Input Size (log-log scale)', fontsize=14)
plt.grid(True, which='both', linestyle='--', alpha=0.7)
plt.legend(loc='upper left', fontsize=10)

# 保存图片（高分辨率）
plt.tight_layout()
plt.savefig('sorting_comparisons.png', dpi=300)
plt.show()

print("图片已保存为 sorting_comparisons.png")