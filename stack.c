/*
 * stack.c
 *
 *  Created on: 6 Aug 2019
 *      Author: Yarden
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"



struct Stack* createStack()
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->list = listInit();
    stack->top = NULL;
    return stack;
}

int isEmpty(struct Stack *stack)
{
	return stack->list->size==0;
}

void push(struct Stack *stack, void* data)
{
	listPushBack(stack->list, data);
	stack->top =stack->list->last;
}

void *pop (struct Stack *stack)
{
	ListNode *node = stack->list->last;
	void *data;
	if(node == NULL)
	    return NULL;
	data = node->data;

	if(listRemove(stack->list, (stack->list->size-1)) == NULL)
	    return NULL;

	stack->top=stack->list->last;
	return data;
}

void *peek(struct Stack *stack)
{
	return stack->top->data;
}
