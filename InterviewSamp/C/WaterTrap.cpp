#include "stdafx.h"

int WaterTrap(int A[], int n) {
    int max = 0;
    for (int i = 0; i < n; i++)
    if (A[i] > A[max]) max = i;

    int water = 0;

    //If current bar is higher than pick, that means it may trap more water between it and the max bar 
    // and so update pick. Or current bar is lower than pick (the bar before it) and max bar (the bar after it), 
    // and it forms a pool to keep water, so update the total water with the delta of peak and the current bar hight
    for (int i = 0, peak = 0; i < max; i++)
    {
        if (A[i] > peak) peak = A[i];
        else water += peak - A[i];
    }
    for (int i = n - 1, top = 0; i > max; i--)
    {
        if (A[i] > top) top = A[i];
        else water += top - A[i];
    }
    return water;
}