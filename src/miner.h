
# ifdef MINER_RUN_H
# define MINER_RUN_H

#include <functional>
#include <algorithm>

namespace map_evaluator {
	template<typename ArrType>
	std::function<int(int, int)> simple(const ArrType **mp, int row, int col, int k)
	{
		int dp[k + 1][k + 1];
		return [mp, row, col, k] (int x, int y) {
			int tc;
			for (int i = min(x + k - 1, row - 2); i >= x; i--) {
				for (int j = min(y + k - (i - x - k + 2), col - 2); j >= y; j--) {
					dp[i][j] = 
				}
			}
			return 
		}
	}
}

namespace snake_run {

	void solve()
	{

	}
}

# endif