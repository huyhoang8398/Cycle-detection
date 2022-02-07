#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "mylibs/md5.h"
#include "mylibs/floyd.h"
#include "mylibs/mybrent.h"
#include "mylibs/mystack.h"

#define ull unsigned long long

int myrandom(int start, int end) {
    return rand() % (start + 1 - end) + start;
}

void rand_input(unsigned char* input, int l) {
    memset(input, 0, l);
    for (int i = 0; i < l; i++) {
        input[i] = myrandom(1, 127); //ASCII 32 - 126: readable characters
    }
}


int main(int argc, char** argv)
{
    int TEST_TIME = 0, TEST_MU = 0, MIN_LEN = 6, MAX_LEN = 6;

    if (argc > 1) {
        TEST_TIME = atoi(argv[1]);
    }

    if (argc > 2) {
        TEST_MU = atoi(argv[2]);
    }

    if (argc > 3) {
        MIN_LEN = atoi(argv[3]);
    }

    if (argc > 4) {
        MAX_LEN = atoi(argv[4]);
    }

    time_t t;
    srand((unsigned) time(&t));

    struct timeval start, end0, end1, end2, end3;
    gettimeofday(&start, NULL);

    ull mu;
    ull lambda;
    ull f_calls;
    ull stack_size, max_stack_size_1 = 0, max_stack_size_2 = 0;

    double fcalls_1, fcalls_2, fcalls_3, fcalls_4, fcalls_5;

    long seconds = 0;
    int num_stacks = 100;
    unsigned char* input = malloc(8);

    int loops;
    int input_len;
    ull cardi;
    double sqrt_cardi;
    FILE *f1 = fopen("output.txt", "w");
    FILE *f2 = fopen("output_fcalls.txt", "w");
    FILE *f3 = fopen("output_max_stack.txt", "w");

    for (input_len = MIN_LEN; input_len <= MAX_LEN; input_len++) {
        loops = 0;
        fcalls_1 = fcalls_2 = fcalls_3 = fcalls_4 = fcalls_5 = 0;
        max_stack_size_1 = max_stack_size_2 = 0;
        sqrt_cardi = pow(2, input_len*8/2);
        
        printf("input len = %d\n", input_len);

        while (loops++ < 15) {
            rand_input(input, input_len);
            printf("Input = ");
            print_hex(input, input_len);
            
            lambda = 0;
            /* Solve with Floyd */
            floyd(&mu, &lambda, &f_calls, input, TEST_MU);
            fcalls_1 += f_calls;
            if (TEST_TIME) {
                gettimeofday(&end0, NULL);
                printf("mu = %lld, lambda = %lld, fcalls = %lld\n", mu, lambda, f_calls);
                seconds = (end0.tv_sec - start.tv_sec);
                printf("Time solving with floyd: %ld seconds\n\n", seconds);
            }
            lambda = 0;
            /*Solve with Brent*/
            brent(&mu, &lambda, &f_calls, input, TEST_MU);
            fcalls_2 += f_calls;
            if (TEST_TIME) {
                gettimeofday(&end1, NULL);
                printf("mu = %lld, lambda = %lld, fcalls = %lld\n", mu, lambda, f_calls);
                seconds = (end1.tv_sec - end0.tv_sec);
                printf("Time solving with brent: %ld seconds\n\n", seconds);
            }

            lambda = 0;
            /*Solve with partitioning brent*/
            partition_brent(&mu, &lambda, &f_calls, num_stacks, input, TEST_MU);
            fcalls_3 += f_calls;
            if (TEST_TIME) {
                gettimeofday(&end2, NULL);
                printf("mu = %lld, lambda = %lld, fcalls = %lld\n", mu, lambda, f_calls);
                seconds = (end2.tv_sec - end1.tv_sec);
                printf("Time solving with partitioning brent: %ld seconds\n\n", seconds);
            }

            lambda = 0;
            /*Solve with one stack*/
            solve_with_stack(&mu, &lambda, &f_calls, &stack_size, input, TEST_MU); 
            fcalls_4 += f_calls;
            max_stack_size_1 = max_stack_size_1 > stack_size ? max_stack_size_1 : stack_size;
            if (TEST_TIME) {
                gettimeofday(&end3, NULL);
                printf("mu = %lld, lambda = %lld, fcalls = %lld, stack_size_max = %lld\n", mu, lambda, f_calls, stack_size);
                seconds = (end3.tv_sec - end2.tv_sec);
                printf("Time solving with one stack:  %ld seconds\n\n", seconds);
            }
            
            lambda = 0;
            /*Solve with multiple stacks*/
            solve_with_multi_stacks(&mu, &lambda, &f_calls, &stack_size, input, num_stacks, TEST_MU); 
            fcalls_5 += f_calls;
            max_stack_size_2 = max_stack_size_2 > stack_size ? max_stack_size_2 : stack_size;
            if (TEST_TIME) {
                gettimeofday(&end0, NULL);
                printf("mu = %lld, lambda = %lld, fcalls = %lld, stack_size_max = %lld\n", mu, lambda, f_calls, stack_size);
                seconds = (end0.tv_sec - end3.tv_sec);
                printf("Time solving with %d stacks: %ld seconds\n", num_stacks, seconds);
                printf("___________________________________________\n\n");
            }
        }
        fprintf(f1, "%d %.3lf %.3lf %.3lf  %.3lf %.3lf \n", input_len*8, fcalls_1/loops/sqrt_cardi, fcalls_2/loops/sqrt_cardi, fcalls_3/loops/sqrt_cardi, fcalls_4/loops/sqrt_cardi, fcalls_5/loops/sqrt_cardi);
        fprintf(f2, "%d %.0lf %.0lf %.0lf %.0lf %.0lf \n", input_len*8, fcalls_1/loops, fcalls_2/loops, fcalls_3/loops, fcalls_4/loops, fcalls_5/loops);
        fprintf(f3, "%d %lld %lld \n", input_len*8, max_stack_size_1, max_stack_size_2);
    }
    fclose(f1);
    fclose(f2);
    fclose(f3);

    return 0;
}