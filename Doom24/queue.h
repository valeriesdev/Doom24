#pragma once

#include "dmath.h"
#include <math.h>
#include <stdio.h>
typedef struct node {
	int sector;
	uint8_t via;
	v2 a;
	v2 b;
	v2 c;
	v2 d;
	struct node* next;
} node_t; 

#define NULL_NODE ((node_t) { -1, -1, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, NULL })

void enqueue(node_t** head, node_t data);
node_t* dequeue(node_t** head);
void set_node(node_t* node, node_t data);