#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md5.h"
#include "mystack.h"
#include "mybrent.h" //For using findmu function if needed

entry* new_entry(unsigned int i, int l, ull count, unsigned char* data) {
    
    entry* E = (entry*)malloc(sizeof(entry));
    E->i = i;
    E->stack_count = count;
    E->byte_len = l;
    E->value = malloc(l);
    memcpy(E->value, data, l);
    E->next_entry = NULL;

    return E;
}

void clear_entry(entry *E) {
    free(E->value);
    E->next_entry = NULL;
    free(E);
}

/*
Testing purposes
*/
void print_stack(entry* top) {
    if (!top) {
        printf("NULL\n");
        return;
    }
    entry* curr = top;
    printf("top -> ");
    while (curr) {
        printf("(%d, ", curr->i);
        print_hex(curr->value, curr->byte_len);
        printf(") -> ");
        curr = curr->next_entry;
    }
    printf("NULL\n");
}


/*
The purpose of this function is to insert a new entry into a stack (in form of a linked list)
such that the top element would always have the greatest value compared to all other elements
Return 0 if we don't find any element having the same value with the new_entry
Return the ***step distance*** between 2 entries if we find out an element having the same value with the new_entry
*/
int add_new_entry_to_stack(entry** top, unsigned int i, int l, unsigned char* new_data) {
    
    /* Check if top is NULL, we create a new entry from given parameters,
    then we point top to the new entry we've created. Return 0
    */
    if (!(*top)) {
        entry* new_E = new_entry(i, l, 1, new_data);
        new_E->next_entry = *top;
        *top = new_E;
        return 0;
    }

    /*If top is NOT NULL
    We search for the latest entry which has the value smaller than or equals to current new_data
    */
    entry* curr = *top;
    entry* prev = NULL;
    while(curr && memcmp(new_data, curr->value, l) < 0) {
        prev = curr;
        curr = curr->next_entry;
    }

    /*If we find out an entry having the same value with new_data, 
    then Return the step distance between 2 entries
    */
    if (curr && memcmp(new_data, curr->value, l) == 0) {
        return i - curr->i;
    }

    /*The entry "curr" is the maximum entry which has the value smaller than our new_data
    */
    entry *new_E;
    if (curr)
        /*If "curr" is NOT NULL, assume that "curr" has stack_count of x, then create a new entry having stack_count of x+1*/
        new_E = new_entry(i, l, curr->stack_count + 1, new_data);
    else 
        /*If "curr" is NULL, the stack now has only 1 element, then the stack_count of new entry is 1 */
        new_E = new_entry(i, l, 1, new_data);

    /*
    Free unused entries
    */
    entry* temp1 = *top;
    entry* temp2 = NULL;
    while (temp1 != curr) {
        temp2 = temp1->next_entry;
        clear_entry(temp1);
        temp1 = temp2;
    }

    /*Point the ToS to new entry*/
    (*top) = new_E;
    new_E->next_entry = curr;

    return 0;
}

/*
Find collision of a given function (MD5) by using ONE STACK techniques, from a given input
Calculate the mu, lambda, total function calls and the max size of the stack
*/
void solve_with_stack(ull* mu, ull* lambda, ull *f_calls, ull *stack_size, unsigned char* input, int TEST_MU) {
    *lambda = 1;
    *mu = 0;
    *f_calls = 0;

    int l = strlen(input);

    unsigned char* x = malloc(l);
    unsigned char* y = malloc(l);

    memcpy(x, input, l);

    struct entry* top = NULL;

    ull check = 0;
    ull step = 1;
    *stack_size = 0;

    /*
    Do the loop while we are not finding out the collision
    */
    while (check == 0) {
        /*
        Take as input an char array "x"
        The output of md5_small is stored in "y"
        */
        md5_small(y, l, 8*l, x, l);

        /*
        Increase f_calls once we call md5_small
        */
        *f_calls += 1;

        /*
        check = 0 if we don't find out a collision when adding a new entry into the stack
        check > 0 if a collision is found, end loop
        */
        check = add_new_entry_to_stack(&top, step, l, y);
        
        /*
        Update the max size of the stack
        */
        *stack_size = top->stack_count > *stack_size ? top->stack_count : *stack_size;

        /*
        Copy value of y to x
        The entry "x" would be the input for next loop
        */
        memcpy(x, y, l);
        step++;
    }
    /*
    The value of lambda would be the value of "check" = the step distance
    between 2 entries when we found out the collision
    */
    *lambda = check;

    if (TEST_MU)
        find_mu(mu, lambda, input);
}



/*
Find collision of a given function (MD5) by using MULTIPLES STACK techniques, from a given input
Calculate the mu, lambda, total function calls and the max size of all stacks 
*/
void solve_with_multi_stacks(ull* mu, ull* lambda, ull *f_calls, ull *max_total_stack_size, unsigned char* input, int num_stacks, int TEST_MU) {
    *lambda = 1;
    *mu = 0;
    *f_calls = 0;

    int l = strlen(input);

    unsigned char* x = malloc(l);
    unsigned char* y = malloc(l);

    memcpy(x, input, l);

    /*
    Allocate the memory for multiple stacks,
    each stack is pointed by an entry top[i]
    */
    struct entry** top = malloc(num_stacks*sizeof(entry*));
    for (int i = 0; i < num_stacks; i++)
        top[i] = NULL;

    ull check = 0;
    ull count = 1;
    *max_total_stack_size = 0;
    ull temp_max = 0;
    int index;

    /*
    Do the loop while we are not finding out the collision
    */
    while (check == 0) {

        /*
        Take as input an char array "x"
        The output of md5_small is stored in "y"
        */
        md5_small(y, l, 8*l, x, l);

        /*
        Increase f_calls once we call md5_small
        */
        *f_calls += 1;

        /*
        We want to know which stack would be used in this loop
        by calculating the index 
        */
        index = (int)y[l-1] % num_stacks;
        
        /*
        We add new entry to the stack[index] and update the total size of all stacks
        */
        if (top[index] != NULL)
            temp_max -= top[index]->stack_count;
        check = add_new_entry_to_stack(&top[index], count, l, y);
        if (top[index] != NULL)
            temp_max += top[index]->stack_count;
        
        /*
        Update the max total size of all stacks accross the loops
        */
        *max_total_stack_size = temp_max > *max_total_stack_size ? temp_max : *max_total_stack_size;

        memcpy(x, y, l);
        count++;
    }
    /*
    The value of lambda would be the value of "check" = the step distance
    between 2 entries when we found out the collision
    */
    *lambda = check;

    if (TEST_MU)
        find_mu(mu, lambda, input);
}
