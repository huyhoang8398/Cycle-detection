#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "md5.h"
#include "floyd.h"
#include "mybrent.h"

void floyd(ull *mu, ull *lambda, ull* fcalls, unsigned char* input, int TEST) {
    int l = strlen(input);

    unsigned char* tortoise = malloc(l);
    unsigned char* hare = malloc(l);

    md5_small(tortoise, l, 8*l, input, l);
    md5_small(hare, l, 8*l, tortoise, l);
    *fcalls = 2;

    while(memcmp(tortoise, hare, l) != 0) {
        //tortoise takes 1 steps
        md5_small(tortoise, l, 8*l, tortoise, l);

        //hare takes 2 steps
        md5_small(hare, l, 8*l, hare, l);
        md5_small(hare, l, 8*l, hare, l);

        *fcalls += 3;
    }
    
    if (TEST) {
        *mu = 0;
        unsigned char* temp = malloc(l);
        memcpy(tortoise, input, l);
        while(memcmp(tortoise, hare, l) != 0) {
            md5_small(temp, l, 8*l, tortoise, l);
            md5_small(hare, l, 8*l, hare, l);
            if(memcmp(temp, hare, l) == 0) {
                print_hex(tortoise, l);
            }
            memcpy(tortoise, temp, l);
            *mu += 1;
        }

        *lambda = 1;
        md5_small(hare, l, 8*l, tortoise, l);
        while(memcmp(tortoise, hare, l) != 0) {
            md5_small(temp, l, 8*l, hare, l);
            if(memcmp(tortoise, temp, l) == 0) {
                print_hex(hare, l);
            }
            memcpy(hare, temp, l);
            *lambda += 1;
        }
    }
}