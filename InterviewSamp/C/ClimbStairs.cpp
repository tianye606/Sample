#include "stdafx.h"

int climbStairs(int n) {
    int prev = 0;
    int cur = 1;
    for (int i = 1; i <= n; ++i){
        int tmp = cur;
        cur += prev;
        prev = tmp;
    }
    return cur;
}