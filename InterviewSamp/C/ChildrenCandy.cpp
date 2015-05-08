#include "stdafx.h"
#include "Header.h"

int candy(int ratings[], int len) {
    int* increment = new int[len];
    // Scan from left to right and then right to left
    for (int i = 1, inc = 1; i < len; i++) {
        if (ratings[i] > ratings[i - 1])
            increment[i] = max(inc++, increment[i]);
        else
            inc = 1;
    }
    for (int i = len - 2, inc = 1; i >= 0; i--) {
        if (ratings[i] > ratings[i + 1])
            increment[i] = max(inc++, increment[i]);
        else
            inc = 1;
    }
    // At least n candy since everyone must have one candy
    int candies=len;
    for (int i = 0; i < len; i++)
    {
        candies = candies + increment[i];
    }
    delete[] increment;
    return candies;
}