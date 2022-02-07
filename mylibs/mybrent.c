#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
#include "mybrent.h"

void print_hex(unsigned char* str, int l) {
    int i = 0;
    while(i < l) {
        printf("0x%02x ", str[i]);
        i++;
    }
    printf("\n");
}

/*
Find mu when knowing lambda from a given input
*/
void find_mu(ull *mu, ull *lambda, unsigned char* input) {
    int l = strlen(input);

    unsigned char* x = malloc(l);
    unsigned char* y = malloc(l);
    unsigned char* x2 = malloc(l);
    unsigned char* y2 = malloc(l);

    memcpy(x, input, l);
    memcpy(y, input, l);

    for (int i = 0; i < *lambda; i++) {
        md5_small(y, l, 8*l, y, l);
    }

    while (memcmp(x, y, l) != 0){
        md5_small(x2, l, 8*l, x, l);
        md5_small(y2, l, 8*l, y, l);
        *mu += 1;
        if (memcmp(x2, y2, l) == 0) {
            print_hex(x, l);
            print_hex(y, l);
        }
        memcpy(x, x2, l);
        memcpy(y, y2, l);
    }

}

/*
Find collision of a given function (MD5) from a given input
Caculate the mu, lambda and total function calls
*/
void brent(ull *mu, ull *lambda, ull *f_calls, unsigned char* input, int TEST_MU) 
{
    ull power = 1;
    *lambda = 1;
    *mu = 0;

    int l = strlen(input);

    unsigned char* x = malloc(l);
    unsigned char* y = malloc(l);

    /*
    Copy the input into array x,
    then calculate y = md5(x)
    */
    memcpy(x, input, l);
    md5_small(y, l, 8*l, x, l);
    *f_calls = 1;

    /*
    Do the loops while x != y
    */
    while (memcmp(x, y, l) != 0) {
        /*
        When "y" reaches the interval, double the "power" and reset lambda
        */
        if (power == *lambda) {
            memcpy(x, y, l); 
            power <<= 1;
            *lambda = 0;
        }
        /*
        Let "y" run inside the interval of "power" and try to find the collision
        */
        md5_small(y, l, 8*l, y, l);
        *f_calls += 1;
        *lambda += 1;
    }

    if (TEST_MU)
        find_mu(mu, lambda, input);
    
}

void partition_brent(ull *mu, ull *lambda, ull *f_calls, int numb_stack, unsigned char* input, int TEST_MU) 
{
    ull power[numb_stack];
    for (int i = 0; i < numb_stack; i++)
    {
        power[i] = 1;
    }

    *lambda = 0;
    *mu = 0;

    int l = strlen(input);

    ull counter[numb_stack];
    ull glob_count_x[numb_stack];
    ull glob_count;

    unsigned char** x = (unsigned char**) malloc(numb_stack * sizeof(char*));
    unsigned char* temp = malloc(l);

    for (int i = 0; i < numb_stack; i++)
    {
        counter[i] = 0;
        x[i] = malloc(l);
        memset(x[i], 0, l);
    }

    int r = (int) input[l-1] % numb_stack;
    memcpy(x[r], input, l);
    glob_count_x[r] = 0;

    md5_small(temp, l, 8*l, x[r], l);
    *f_calls = 1;
    r = (int)temp[l-1] % numb_stack;

    counter[r] = 1;
    glob_count = 1;
    

    while (memcmp(x[r], temp, l) != 0) {
        if (power[r] == counter[r]) {
            memcpy(x[r], temp, l);
            power[r] <<= 1;
            counter[r] = 0;
            glob_count_x[r] = glob_count;
        }

        md5_small(temp, l, 8*l, temp, l);
        r = (int)temp[l-1] % numb_stack;
        counter[r] += 1;
        glob_count += 1;

        *f_calls += 1;
    }
    *lambda = glob_count - glob_count_x[r];

    if (TEST_MU)
        find_mu(mu, lambda, input);
    
}