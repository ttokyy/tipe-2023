#include "linked_list.h"

l_list create_l_node (int x, int y, int spec){
	// retourne une liste chaînée réduite à une cellule contenant les champs x, y et spec
	l_list l = (l_list) malloc (sizeof(struct l_node_s));
	l->x = x;
	l->y = y;
	l->spec = spec;
	return l;
}

bool is_empty_l_list (l_list l){
	return l == NULL;
}

void free_l_list (l_list l){
	// libère l'espace mémoire alloué à la liste l
	while (l != NULL){
		l_list temp = l;
		l = l->next;
		free (temp);
	}
}

void l_insert (l_list* pl, int x, int y, int spec){
	l_list new_nd = create_l_node (x, y, spec);
	new_nd->next = *pl;
	*pl = new_nd;
}