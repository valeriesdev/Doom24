#include "queue.h"
#include "dmath.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_node(node_t* node, node_t data) {
	node->sector = data.sector;
	node->via = data.via;
	node->a = data.a;
	node->b = data.b;
	node->c = data.c;
	node->d = data.d;
	node->next = NULL;
}

void enqueue(node_t** head, node_t data) {
	if (*head == NULL) {
		*head = malloc(sizeof(node_t));
		set_node(*head, data);
		return;
	}
	node_t* current = *head;
	while (current->next != NULL) current = current->next;

	node_t* new_node = malloc(sizeof(node_t));
	set_node(new_node, data);
	current->next = new_node;
}

node_t* dequeue(node_t** head) {
	if (*head == NULL) return;

	node_t* new_node = malloc(sizeof(node_t));
	ASSERT(new_node != NULL, "MEMORY FAILURE");
	memcpy(new_node, *head, sizeof(node_t));

	if ((*head)->next == NULL) {
		free(*head);
		*head = NULL;
		return new_node;
	}

	*head = (*head)->next;
	return new_node;
}