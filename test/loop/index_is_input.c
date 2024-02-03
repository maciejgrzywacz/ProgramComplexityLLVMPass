#include <stdio.h>
#include <math.h>

int global_array[10] = {};

int foo(int a, int b, int limit){
    volatile int c = 9;             // volatile to prevent some optimizations
    for(; a < limit; a++) {
        b += 1;
        for(; b < limit; b++) {
            c += 1;
        }
    }

    int return_value;
    if (c > 36) {
        return_value = global_array[a % 10];
        return_value += c;
        printf("%d", return_value);
    }
    else {
        return_value = global_array[limit % 10];
        return_value = log(return_value);
    }

    return return_value;
}