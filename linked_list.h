#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct l_node_s {
	int x;
	int y;
	int spec;
	struct l_node_s* next;
};
typedef struct l_node_s* l_list;

l_list create_l_node (int x, int y, int spec);
bool is_empty_l_list (l_list l);
void free_l_list (l_list l);
void l_insert (l_list* pl, int x, int y, int spec);

#endif