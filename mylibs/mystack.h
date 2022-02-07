#ifndef _MYSTACK_H
#define _MYSTACK_H

typedef struct entry {
    /* i is the step index of current entry. 
    For example, the begining input x would have i = 1, f(x) has i = 2, f(f(x)) has i = 3,...
    */
    unsigned int i; 

    /*The byte array named "value"*/
    unsigned char* value; 

    int byte_len; //len (in bytes) of the "value"

    /*total number of elements in the stack, count from this entry to the end of stack*/
    unsigned long long stack_count; 
    
    struct entry* next_entry;
} entry; //sizeof(entry) = 40

entry* new_entry();
void clear_entry();
void print_stack();
int add_new_entry_to_stack();
void solve_with_stack();
void solve_with_multi_stacks();
#endif