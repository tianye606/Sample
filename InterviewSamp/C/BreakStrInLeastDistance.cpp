#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <array>

int find_max(int freq[], bool excep[]) {
    int max_i = -1;
    int max = -1;
    for (char c = 'a'; c <= 'z'; c++) {
        if (!excep[c] && freq[c] > 0 && freq[c] > max) {
            max = freq[c];
            max_i = c;
        }
    }
    return max_i;
}

void BreakStrInLeastDistance(char* str, int d, char ans[]) {
    int n = strlen(str);
    int freq[256] = { 0 };
    for (int i = 0; i < n; i++)
        freq[str[i]]++;

    int lastInsertedPos[256];
    std::fill_n(lastInsertedPos, 256, -1);

    for (int i = 0; i < n; i++) {
        bool excep[256] = { false };
        bool done = false;
        while (!done) {
            int j = find_max(freq, excep);
            if (j == -1) {
                std::cout << "Error!\n";
                return;
            }
            excep[j] = true;
            if (lastInsertedPos[j] < 0 || i - lastInsertedPos[j] >= d) {
                ans[i] = j;
                freq[j]--;
                lastInsertedPos[j] = i;
                done = true;
            }
        }
    }
    ans[n] = '\0';
}