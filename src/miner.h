
# ifndef MINER_RUN_H
# define MINER_RUN_H

#include <functional>
#include <algorithm>
#include "../alter/tools.h"
#include "../alter/generate.h"
#include <random>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace map_evaluator {
	template<typename ArrType>
    std::function<int(int, int)> simple(ArrType **dp, const ArrType **mp, const int row, const int col, const int k)
	{
        if (k <= 1) {
            return [row, col] (int x, int y) mutable -> int {
                static std::mt19937 mt_rand(time(nullptr));
                switch (((x == row - 1) << 1) | (y == col - 1)) {
                case 0: {
                    return mt_rand() & 1;
                }
                case 1: {
                    // 右侧没有了, 下走
                    return 0;
                }
                case 2: {
                    // 下侧没有了, 右走
                    return 1;
                }
                case 3: {
                    return -1;
                }
                default:
                    // ????
                    return -2;
                }
            };
        }

        return [dp, mp, row, col, k] (int x, int y) mutable -> int {
			switch (((x == row - 1) << 1) | (y == col - 1)) {
            case 0: {
                for (int i = std::min(x + k - 1, row - 1); i >= x; i--) {
                    int j = y + k - (i - x + 1);
                    if (j >= col) {
                        continue;
                    }
                    dp[i][j] = mp[i][j];
                }
                dp[std::min(k - 1, row - 1)][std::min(k - 1, col - 1)]
                    =mp[std::min(k - 1, row - 1)][std::min(k - 1, col - 1)];
                if (x + k - 1 >= row) {
                    for (int j = std::min(y + k - (row - 1 - x + 1) - 1, col - 2); j >= y; j--) {
                        dp[row - 1][j] = mp[row - 1][j] + dp[row - 1][j + 1];
                    }
                }

                if (y + k - 1 >= col) {
                    for (int i = std::min(y + k - (col - 1 - x + 1) - 1, row - 2); i >= x; i--) {
                        dp[i][col - 1] = mp[i][col - 1] + dp[i + 1][col - 1];
                    }
                }

                for (int i = std::min(x + k - 1, row - 2); i >= x; i--) {
                    for (int j = std::min(y + k - (i - x + 1) - 1, col - 2); j >= y; j--) {
                        dp[i][j] = std::max(dp[i][j + 1], dp[i + 1][j]) + mp[i][j];
                    }
                }
                // 1->右走, 0 -> 下走
                return dp[x][y + 1] > dp[x + 1][y];
            }
            case 1: {
                // 右侧没有了, 下走
                return 0;
            }
            case 2: {
                // 下侧没有了, 右走
                return 1;
            }
            case 3: {
                return -1;
            }
            default:
                // ????
                return -2;
            }
		};
	}

    template<typename ArrType>
    std::function<int(int, int)> image(ArrType **dp, const ArrType **mp, const int row, const int col, const int k, const double c)
	{
        if (k <= 1) {
            return [row, col] (int x, int y) mutable -> int {
                static std::mt19937 mt_rand(time(nullptr));
                switch (((x == row - 1) << 1) | (y == col - 1)) {
                case 0: {
                    return mt_rand() & 1;
                }
                case 1: {
                    // 右侧没有了, 下走
                    return 0;
                }
                case 2: {
                    // 下侧没有了, 右走
                    return 1;
                }
                case 3: {
                    return -1;
                }
                default:
                    // ????
                    return -2;
                }
            };
        }

        return [dp, mp, row, col, k, c] (int x, int y) mutable -> int {
			switch (((x == row - 1) << 1) | (y == col - 1)) {
            case 0: {
                dp[x][y] = mp[x][y];
                for(int i = x + 1; i < row; i++) {
                    if (i < k) {
                        dp[i][y] = mp[i][y];
                    } else {
                        dp[i][y] = ArrType(c * dp[i - 1][y]);
                    }
                }
                for(int i = y + 1; i < col; i++) {
                    if (i < k) {
                        dp[x][i] = mp[x][i];
                    } else {
                        dp[x][i] = ArrType(c * dp[x][i - 1]);
                    }
                }
                for (int i = x + 1; i < row; i++) {
                    for (int j = y + 1; j < col; j++) {
                        if (abs(x - i) + abs(y - j) < k) {
                            dp[i][j] = mp[i][j];
                        } else {
                            dp[i][j] = ArrType(c * (dp[i][j - 1] + dp[i][j - 1]));
                        }
                    }
                }
                for (int i = row - 2; i >= x; i--) {
                    dp[i][col - 1] = dp[i + 1][col - 1] + dp[i][col - 1];
                }
                for (int i = col - 2; i >= y; i--) {
                    dp[row - 1][i] = dp[row - 1][i + 1] + dp[row - 1][i];
                }
                for (int i = row - 2; i >= x; i--) {
                    for (int j = col - 2; j >= y; j--) {
                        dp[i][j] = std::max(dp[i][j + 1], dp[i + 1][j]) + dp[i][j];
                    }
                }
                // 1->右走, 0 -> 下走
                return dp[x][y + 1] > dp[x + 1][y];
            }
            case 1: {
                // 右侧没有了, 下走
                return 0;
            }
            case 2: {
                // 下侧没有了, 右走
                return 1;
            }
            case 3: {
                return -1;
            }
            default:
                // ????
                return -2;
            }
		};
	}

}


namespace god_run {

    template<typename ArrType>
    void solve(ArrType &ret, ArrType **dp, ArrType **sig, const ArrType **mp, const int row, const int col)
    {
        clear_two_dimensional_space<ArrType>(sig, row, col);
        dp[row - 1][col - 1] = mp[row - 1][col - 1];
        for (int i = row - 2; i >= 0; i--) {
            dp[i][col - 1] = dp[i + 1][col - 1] + mp[i][col - 1];
        }
        for (int i = col - 2; i >= 0; i--) {
            dp[row - 1][i] = dp[row - 1][i + 1] + mp[row - 1][i];
        }
        for (int i = row - 2; i >= 0; i--) {
            for (int j = col - 2; j >= 0; j--) {
                dp[i][j] = std::max(dp[i][j + 1], dp[i + 1][j]) + mp[i][j];
            }
        }
        ret = dp[0][0];
        for (int curx = 0, cury = 0;;) {
            if (curx == row - 1) {
                while (cury < col) {
                    sig[curx][cury] = 1;
                    cury++;
                }
                break;
            }
            if (cury == col - 1) {
                while (curx < row) {
                    sig[curx][cury] = 1;
                    curx++;
                }
                break;
            }
            if (dp[curx + 1][cury] > dp[curx][cury + 1]) {
                curx++;
            } else {
                cury++;
            }
            sig[curx][cury] = 1;
        }
        return;
    }

    template<typename ArrType>
    void solve_absent(
        ArrType &ret, ArrType **dp, ArrType **sig,
        const ArrType **mp, const ArrType **ab_mp, const int row, const int col)
    {
        clear_two_dimensional_space<ArrType>(sig, row, col);
        dp[row - 1][col - 1] = ab_mp[row - 1][col - 1];
        for (int i = row - 2; i >= 0; i--) {
            dp[i][col - 1] = dp[i + 1][col - 1] + ab_mp[i][col - 1];
        }
        for (int i = col - 2; i >= 0; i--) {
            dp[row - 1][i] = dp[row - 1][i + 1] + ab_mp[row - 1][i];
        }
        for (int i = row - 2; i >= 0; i--) {
            for (int j = col - 2; j >= 0; j--) {
                dp[i][j] = std::max(dp[i][j + 1], dp[i + 1][j]) + ab_mp[i][j];
            }
        }
        ret = 0;
        for (int curx = 0, cury = 0;;) {
            if (curx == row - 1) {
                while (cury < col) {
                    sig[curx][cury] = 1;
                    ret = ret + mp[curx][cury];
                    cury++;
                }
                break;
            }
            if (cury == col - 1) {
                while (curx < row) {
                    sig[curx][cury] = 1;
                    ret = ret + mp[curx][cury];
                    curx++;
                }
                break;
            }
            if (dp[curx + 1][cury] > dp[curx][cury + 1]) {
                curx++;
            } else {
                cury++;
            }
            ret = ret + mp[curx][cury];
            sig[curx][cury] = 1;
        }
        return;
    }

    template<typename ArrType>
    std::function<void()> bind_variables(
        ArrType *ret, ArrType **dp, ArrType **sig, const ArrType **mp, const int row, const int col
    ) {
        return [ret, dp, sig, mp, row, col] ()  mutable -> void {
            solve(*ret, dp, sig, mp, row, col);
        };
    }

    template<typename ArrType>
    std::function<void()> bind_variables_absent(
        ArrType *ret, ArrType **dp, ArrType **sig,
        const ArrType **mp, const ArrType **ab_mp, const int row, const int col
    ) {
        return [ret, dp, sig, mp, ab_mp, row, col] ()  mutable -> void {
            solve_absent(*ret, dp, sig, mp, ab_mp, row, col);
        };
    }
}

namespace snake_run {

    template<typename ArrType>
	void solve(ArrType &ret, ArrType **dp, ArrType **sig, const ArrType **mp, const int row, const int col, const int k)
	{
        clear_two_dimensional_space<ArrType>(sig, row, col);
        auto evaluator = map_evaluator::simple<ArrType>(dp, mp, row, col, k);
        
        ret = mp[0][0];
        sig[0][0] = 1;
        for (int curx = 0, cury = 0;;) {
        	switch(evaluator(curx, cury)) {
                case 1:{
                    cury++;
                    break;
                }
                case 0:{
                    curx++;
                    break;
                }
                default:{
                    return;
                    break;
                }
            }
            ret += mp[curx][cury];
            sig[curx][cury] = 1;
        }
	}

    template<typename ArrType>
    std::function<void()> bind_variables(
        ArrType *ret, ArrType **dp, ArrType **sig, const ArrType **mp, const int row, const int col, const int k
    ) {
        return [ret, dp, sig, mp, row, col, k] () mutable -> void {
            solve(*ret, dp, sig, mp, row, col, k);
        };
    }
}

namespace image_run {
    // c应该小于 0.7
    template<typename ArrType>
	void solve(
        ArrType &ret, ArrType **dp, ArrType **sig,
        const ArrType **mp, const int row, const int col, const int k, const double c
    ) {
        clear_two_dimensional_space<ArrType>(sig, row, col);
        auto evaluator = map_evaluator::image<ArrType>(dp, mp, row, col, k, c);
        ret = mp[0][0];
        sig[0][0] = 1;
        for (int curx = 0, cury = 0;;) {
            switch(evaluator(curx, cury)) {
                case 1:{
                    cury++;
                    break;
                }
                case 0:{
                    curx++;
                    break;
                }
                default:{
                    return ;
                }
            }
            ret += mp[curx][cury];
            sig[curx][cury] = 1;
        }
	}

    template<typename ArrType>
    std::function<void()> bind_variables(
        ArrType *ret, ArrType **dp, ArrType **sig,
        const ArrType **mp, const int row, const int col, const int k, const double c
    ) {
        return [ret, dp, sig, mp, row, col, k, c] () mutable -> void {
            solve(*ret, dp, sig, mp, row, col, k, c);
        };
    }
}

# endif