/*
 * stack.h
 *
 *  Created on: 6 Aug 2019
 *      Author: Yarden
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"

#ifndef STACK_H_
#define STACK_H_


typedef struct Stack {
    List *list;
    ListNode *top;
}Stack;

struct Stack* createStack();

/*
 * checks whether or not stack is empty
 */
int isEmpty(struct Stack *stack);

/*
 * add data to the stack, on the top
 */
void push(struct Stack *stack, void* data);

/*
 * get the top of the stack, and remove it from it
 */
void *pop (struct Stack *stack);

/*
 * get the top of the stach
 */
void *peek(struct Stack *stack);

#endif /* STACK_H_ */
