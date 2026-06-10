import matplotlib.pyplot as plt
import numpy as np

# 数据来自表5 (仅提取 n 和相应的执行时间)
n_vals = [1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
          20000, 40000, 80000, 160000, 320000]

# 动态规划 (DP) 时间 (ms)  for C=10000 (有效数据到10000, 之后为0但为了作图只取非零段)
dp_c10000 = [18, 38, 55, 68, 81, 103, 115, 137, 150, 188]  # 对应 n=1000..10000
n_dp = n_vals[:10]  # [1000..10000]

# 贪心法 (Greedy) 时间 (ms)  for C=10000, C=100000, C=1000000
greedy_c10000 = [0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 5, 11, 23, 64]
greedy_c100000 = [0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 2, 6, 12, 24, 57]
greedy_c1000000 = [0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 3, 5, 12, 27, 52]

# 注意：贪心法时间有0值，在双对数坐标中0无法显示，故将0替换为一个小正数（如0.1）以便绘图，但原始0表示时间极短可忽略
# 更合理的做法是只绘制非零点，或者将0替换为1e-3。这里为了清晰，对贪心法中0值不绘制（使用掩码）
greedy_c10000 = np.array(greedy_c10000)
greedy_c100000 = np.array(greedy_c100000)
greedy_c1000000 = np.array(greedy_c1000000)

# 创建掩码：时间 > 0 的点才绘制
mask_g10000 = greedy_c10000 > 0
mask_g100000 = greedy_c100000 > 0
mask_g1000000 = greedy_c1000000 > 0

# 创建图形
plt.figure(figsize=(8, 6))

# 绘制动态规划曲线 (C=10000)
plt.loglog(n_dp, dp_c10000, 'o-', color='red', linewidth=2, markersize=6,
           label='DP, C=10000')

# 绘制贪心法曲线
plt.loglog(np.array(n_vals)[mask_g10000], greedy_c10000[mask_g10000], 's-',
           color='blue', linewidth=2, markersize=6, label='Greedy, C=10000')
plt.loglog(np.array(n_vals)[mask_g100000], greedy_c100000[mask_g100000], '^-',
           color='green', linewidth=2, markersize=6, label='Greedy, C=100000')
plt.loglog(np.array(n_vals)[mask_g1000000], greedy_c1000000[mask_g1000000], 'd-',
           color='orange', linewidth=2, markersize=6, label='Greedy, C=1000000')

# 图表装饰
plt.xlabel('Number of items n (log scale)', fontsize=12)
plt.ylabel('Execution time (ms, log scale)', fontsize=12)
plt.title('Knapsack Algorithm Performance (log-log scale)', fontsize=14)
plt.grid(True, which='both', linestyle='--', alpha=0.7)
plt.legend(loc='upper left', fontsize=10)

# 保存图片（高分辨率）
plt.tight_layout()
plt.savefig('knapsack_performance.png', dpi=300)
plt.show()

print("图片已保存为 knapsack_performance.png")