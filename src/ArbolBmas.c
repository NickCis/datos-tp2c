#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define DEFAULT_ORDER 4


#define MIN_ORDER 3
#define MAX_ORDER 20


typedef struct record {
	int value;
} record;


typedef struct node {
	void ** pointers;
	int * keys;
	struct node * parent;
	bool is_leaf;
	int num_keys;
	struct node * next; 
} node;


int order = DEFAULT_ORDER;


node * queue = NULL;


bool verbose_output = false;


void usage_1( void );
void usage_2( void );
void usage_3( void );
void enqueue( node * new_node );
node * dequeue( void );
int height( node * root );
int path_to_root( node * root, node * child );
void print_leaves( node * root );
void print_tree( node * root );
void find_and_print(node * root, int key, bool verbose); 
void find_and_print_range(node * root, int range1, int range2, bool verbose); 
int find_range( node * root, int key_start, int key_end, bool verbose,
		int returned_keys[], void * returned_pointers[]); 
node * find_leaf( node * root, int key, bool verbose );
record * find( node * root, int key, bool verbose );
int cut( int length );


record * make_record(int value);
node * make_node( void );
node * make_leaf( void );
int get_left_index(node * parent, node * left);
node * insert_into_leaf( node * leaf, int key, record * pointer );
node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer);
node * insert_into_node(node * root, node * parent, 
		int left_index, int key, node * right);
node * insert_into_node_after_splitting(node * root, node * parent, int left_index, 
		int key, node * right);
node * insert_into_parent(node * root, node * left, int key, node * right);
node * insert_into_new_root(node * left, int key, node * right);
node * start_new_tree(int key, record * pointer);
node * insert( node * root, int key, int value );



int get_neighbor_index( node * n );
node * adjust_root(node * root);
node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime);
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, 
		int k_prime_index, int k_prime);
node * delete_entry( node * root, node * n, int key, void * pointer );
node * delete( node * root, int key );


void usage_1( void ) {
	printf(" Árbol B+ de orden %d.\n", order);
	printf("Para construir un árbol B+ de diferente orden, empiece de nuevo e ingrese el orden\n");
	printf("como un argumento entero:  ArbolBmas <orden>  ");
	printf("(%d <= orden <= %d).\n", MIN_ORDER, MAX_ORDER);
	printf("Para leer los datos desde un archivo de entrada,\n");
	printf("empiece de nuevo e ingrese ");
	printf(" el orden seguido del nombre del archivo:\n");
	printf("ArbolBmas <orden> <nombre> \n");
}


void usage_2( void ) {
	printf("Introduzca cualquiera de las siguientes opciones :\n");
	printf("\ti <k>  -- Insetar <k> (an integer) tanto como llave y valor).\n");
	printf("\tf <k>  -- Encontrar el valor menor que la llave <k>.\n");
	printf("\tp <k> --  Imprimir el camino desde la raíz  hasta la llave k y su valor asociado.\n");
	printf("\tr <k1> <k2> -- Imprimir las llaves y los valores que se encuentran es este rango "
			"[<k1>, <k2>\n");
	printf("\td <k>  -- Eliminar llave <k> y su valor asociado.\n");
	printf("\tx -- Destruye todo el árbol y comienza de nuevo con un árbol vacío del  mismo orden.\n");
	printf("\tt -- Imprimir el árbol B+.\n");
	printf("\tl -- Imprimir las llaves de las hojas.\n");
	printf("\tq -- Salir. (o use Ctl-D.)\n");
	printf("\t? -- Imprimir mensaje de ayuda.\n");
}


void usage_3( void ) {
	printf("Uso: ./ArbolBmas [<orden>]\n");
	printf("\twhere %d <= orden <= %d .\n", MIN_ORDER, MAX_ORDER);
}



void enqueue( node * new_node ) {
	node * c;
	if (queue == NULL) {
		queue = new_node;
		queue->next = NULL;
	}
	else {
		c = queue;
		while(c->next != NULL) {
			c = c->next;
		}
		c->next = new_node;
		new_node->next = NULL;
	}
}



node * dequeue( void ) {
	node * n = queue;
	queue = queue->next;
	n->next = NULL;
	return n;
}



void print_leaves( node * root ) {
	int i;
	node * c = root;
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	while (!c->is_leaf)
		c = c->pointers[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
			if (verbose_output)
				printf("%lx ", (unsigned long)c->pointers[i]);
			printf("%d ", c->keys[i]);
		}
		if (verbose_output)
			printf("%lx ", (unsigned long)c->pointers[order - 1]);
		if (c->pointers[order - 1] != NULL) {
			printf(" | ");
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	printf("\n");
}


int height( node * root ) {
	int h = 0;
	node * c = root;
	while (!c->is_leaf) {
		c = c->pointers[0];
		h++;
	}
	return h;
}



int path_to_root( node * root, node * child ) {
	int length = 0;
	node * c = child;
	while (c != root) {
		c = c->parent;
		length++;
	}
	return length;
}



void print_tree( node * root ) {

	node * n = NULL;
	int i = 0;
	int rank = 0;
	int new_rank = 0;

	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	queue = NULL;
	enqueue(root);
	while( queue != NULL ) {
		n = dequeue();
		if (n->parent != NULL && n == n->parent->pointers[0]) {
			new_rank = path_to_root( root, n );
			if (new_rank != rank) {
				rank = new_rank;
				printf("\n");
			}
		}
		if (verbose_output) 
			printf("(%lx)", (unsigned long)n);
		for (i = 0; i < n->num_keys; i++) {
			if (verbose_output)
				printf("%lx ", (unsigned long)n->pointers[i]);
			printf("%d ", n->keys[i]);
		}
		if (!n->is_leaf)
			for (i = 0; i <= n->num_keys; i++)
				enqueue(n->pointers[i]);
		if (verbose_output) {
			if (n->is_leaf) 
				printf("%lx ", (unsigned long)n->pointers[order - 1]);
			else
				printf("%lx ", (unsigned long)n->pointers[n->num_keys]);
		}
		printf("| ");
	}
	printf("\n");
}


void find_and_print(node * root, int key, bool verbose) {
	record * r = find(root, key, verbose);
	if (r == NULL)
		printf("Record not found under key %d.\n", key);
	else 
		printf("Record at %lx -- key %d, value %d.\n",
				(unsigned long)r, key, r->value);
}


void find_and_print_range( node * root, int key_start, int key_end,
		bool verbose ) {
	int i;
	int array_size = key_end - key_start + 1;
	int returned_keys[array_size];
	void * returned_pointers[array_size];
	int num_found = find_range( root, key_start, key_end, verbose,
			returned_keys, returned_pointers );
	if (!num_found)
		printf("Ninguno encontrado.\n");
	else {
		for (i = 0; i < num_found; i++)
			printf("Key: %d   Ubicación: %lx  Valor: %d\n",
					returned_keys[i],
					(unsigned long)returned_pointers[i],
					((record *)
					 returned_pointers[i])->value);
	}
}



int find_range( node * root, int key_start, int key_end, bool verbose,
		int returned_keys[], void * returned_pointers[]) {
	int i, num_found;
	num_found = 0;
	node * n = find_leaf( root, key_start, verbose );
	if (n == NULL) return 0;
	for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++) ;
	if (i == n->num_keys) return 0;
	while (n != NULL) {
		for ( ; i < n->num_keys && n->keys[i] <= key_end; i++) {
			returned_keys[num_found] = n->keys[i];
			returned_pointers[num_found] = n->pointers[i];
			num_found++;
		}
		n = n->pointers[order - 1];
		i = 0;
	}
	return num_found;
}



node * find_leaf( node * root, int key, bool verbose ) {
	int i = 0;
	node * c = root;
	if (c == NULL) {
		if (verbose) 
			printf("Árbol Vacío.\n");
		return c;
	}
	while (!c->is_leaf) {
		if (verbose) {
			printf("[");
			for (i = 0; i < c->num_keys - 1; i++)
				printf("%d ", c->keys[i]);
			printf("%d] ", c->keys[i]);
		}
		i = 0;
		while (i < c->num_keys) {
			if (key >= c->keys[i]) i++;
			else break;
		}
		if (verbose)
			printf("%d ->\n", i);
		c = (node *)c->pointers[i];
	}
	if (verbose) {
		printf("Hoja [");
		for (i = 0; i < c->num_keys - 1; i++)
			printf("%d ", c->keys[i]);
		printf("%d] ->\n", c->keys[i]);
	}
	return c;
}


record * find( node * root, int key, bool verbose ) {
	int i = 0;
	node * c = find_leaf( root, key, verbose );
	if (c == NULL) return NULL;
	for (i = 0; i < c->num_keys; i++)
		if (c->keys[i] == key) break;
	if (i == c->num_keys) 
		return NULL;
	else
		return (record *)c->pointers[i];
}


int cut( int length ) {
	if (length % 2 == 0)
		return length/2;
	else
		return length/2 + 1;
}


record * make_record(int value) {
	record * new_record = (record *)malloc(sizeof(record));
	if (new_record == NULL) {
		perror("Registro creado.");
		exit(EXIT_FAILURE);
	}
	else {
		new_record->value = value;
	}
	return new_record;
}



node * make_node( void ) {
	node * new_node;
	new_node = malloc(sizeof(node));
	if (new_node == NULL) {
		perror("Nodo creado.");
		exit(EXIT_FAILURE);
	}
	new_node->keys = malloc( (order - 1) * sizeof(int) );
	if (new_node->keys == NULL) {
		perror("New node keys array.");
		exit(EXIT_FAILURE);
	}
	new_node->pointers = malloc( order * sizeof(void *) );
	if (new_node->pointers == NULL) {
		perror("New node pointers array.");
		exit(EXIT_FAILURE);
	}
	new_node->is_leaf = false;
	new_node->num_keys = 0;
	new_node->parent = NULL;
	new_node->next = NULL;
	return new_node;
}


node * make_leaf( void ) {
	node * leaf = make_node();
	leaf->is_leaf = true;
	return leaf;
}


int get_left_index(node * parent, node * left) {

	int left_index = 0;
	while (left_index <= parent->num_keys && 
			parent->pointers[left_index] != left)
		left_index++;
	return left_index;
}


node * insert_into_leaf( node * leaf, int key, record * pointer ) {

	int i, insertion_point;

	insertion_point = 0;
	while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
		insertion_point++;

	for (i = leaf->num_keys; i > insertion_point; i--) {
		leaf->keys[i] = leaf->keys[i - 1];
		leaf->pointers[i] = leaf->pointers[i - 1];
	}
	leaf->keys[insertion_point] = key;
	leaf->pointers[insertion_point] = pointer;
	leaf->num_keys++;
	return leaf;
}


node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer) {

	node * new_leaf;
	int * temp_keys;
	void ** temp_pointers;
	int insertion_index, split, new_key, i, j;

	new_leaf = make_leaf();

	temp_keys = malloc( order * sizeof(int) );
	if (temp_keys == NULL) {
		perror("Temporary keys array.");
		exit(EXIT_FAILURE);
	}

	temp_pointers = malloc( order * sizeof(void *) );
	if (temp_pointers == NULL) {
		perror("Temporary pointers array.");
		exit(EXIT_FAILURE);
	}

	insertion_index = 0;
	while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
		insertion_index++;

	for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
		if (j == insertion_index) j++;
		temp_keys[j] = leaf->keys[i];
		temp_pointers[j] = leaf->pointers[i];
	}

	temp_keys[insertion_index] = key;
	temp_pointers[insertion_index] = pointer;

	leaf->num_keys = 0;

	split = cut(order - 1);

	for (i = 0; i < split; i++) {
		leaf->pointers[i] = temp_pointers[i];
		leaf->keys[i] = temp_keys[i];
		leaf->num_keys++;
	}

	for (i = split, j = 0; i < order; i++, j++) {
		new_leaf->pointers[j] = temp_pointers[i];
		new_leaf->keys[j] = temp_keys[i];
		new_leaf->num_keys++;
	}

	free(temp_pointers);
	free(temp_keys);

	new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
	leaf->pointers[order - 1] = new_leaf;

	for (i = leaf->num_keys; i < order - 1; i++)
		leaf->pointers[i] = NULL;
	for (i = new_leaf->num_keys; i < order - 1; i++)
		new_leaf->pointers[i] = NULL;

	new_leaf->parent = leaf->parent;
	new_key = new_leaf->keys[0];

	return insert_into_parent(root, leaf, new_key, new_leaf);
}


node * insert_into_node(node * root, node * n, 
		int left_index, int key, node * right) {
	int i;

	for (i = n->num_keys; i > left_index; i--) {
		n->pointers[i + 1] = n->pointers[i];
		n->keys[i] = n->keys[i - 1];
	}
	n->pointers[left_index + 1] = right;
	n->keys[left_index] = key;
	n->num_keys++;
	return root;
}


node * insert_into_node_after_splitting(node * root, node * old_node, int left_index, 
		int key, node * right) {

	int i, j, split, k_prime;
	node * new_node, * child;
	int * temp_keys;
	node ** temp_pointers;

	temp_pointers = malloc( (order + 1) * sizeof(node *) );
	if (temp_pointers == NULL) {
		perror("Temporary pointers array for splitting nodes.");
		exit(EXIT_FAILURE);
	}
	temp_keys = malloc( order * sizeof(int) );
	if (temp_keys == NULL) {
		perror("Temporary keys array for splitting nodes.");
		exit(EXIT_FAILURE);
	}

	for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
		if (j == left_index + 1) j++;
		temp_pointers[j] = old_node->pointers[i];
	}

	for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
		if (j == left_index) j++;
		temp_keys[j] = old_node->keys[i];
	}

	temp_pointers[left_index + 1] = right;
	temp_keys[left_index] = key;

 
	split = cut(order);
	new_node = make_node();
	old_node->num_keys = 0;
	for (i = 0; i < split - 1; i++) {
		old_node->pointers[i] = temp_pointers[i];
		old_node->keys[i] = temp_keys[i];
		old_node->num_keys++;
	}
	old_node->pointers[i] = temp_pointers[i];
	k_prime = temp_keys[split - 1];
	for (++i, j = 0; i < order; i++, j++) {
		new_node->pointers[j] = temp_pointers[i];
		new_node->keys[j] = temp_keys[i];
		new_node->num_keys++;
	}
	new_node->pointers[j] = temp_pointers[i];
	free(temp_pointers);
	free(temp_keys);
	new_node->parent = old_node->parent;
	for (i = 0; i <= new_node->num_keys; i++) {
		child = new_node->pointers[i];
		child->parent = new_node;
	}

	return insert_into_parent(root, old_node, k_prime, new_node);
}



node * insert_into_parent(node * root, node * left, int key, node * right) {

	int left_index;
	node * parent;

	parent = left->parent;


	if (parent == NULL)
		return insert_into_new_root(left, key, right);


	left_index = get_left_index(parent, left);



	if (parent->num_keys < order - 1)
		return insert_into_node(root, parent, left_index, key, right);

	return insert_into_node_after_splitting(root, parent, left_index, key, right);
}



node * insert_into_new_root(node * left, int key, node * right) {

	node * root = make_node();
	root->keys[0] = key;
	root->pointers[0] = left;
	root->pointers[1] = right;
	root->num_keys++;
	root->parent = NULL;
	left->parent = root;
	right->parent = root;
	return root;
}


node * start_new_tree(int key, record * pointer) {

	node * root = make_leaf();
	root->keys[0] = key;
	root->pointers[0] = pointer;
	root->pointers[order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;
	return root;
}


node * insert( node * root, int key, int value ) {

	record * pointer;
	node * leaf;


	if (find(root, key, false) != NULL)
		return root;

	pointer = make_record(value);

	if (root == NULL) 
		return start_new_tree(key, pointer);


	leaf = find_leaf(root, key, false);



	if (leaf->num_keys < order - 1) {
		leaf = insert_into_leaf(leaf, key, pointer);
		return root;
	}


	return insert_into_leaf_after_splitting(root, leaf, key, pointer);
}


int get_neighbor_index( node * n ) {

	int i;


	for (i = 0; i <= n->parent->num_keys; i++)
		if (n->parent->pointers[i] == n)
			return i - 1;


	printf("Search for nonexistent pointer to node in parent.\n");
	printf("Nodo:  %#lx\n", (unsigned long)n);
	exit(EXIT_FAILURE);
}


node * remove_entry_from_node(node * n, int key, node * pointer) {

	int i, num_pointers;

		i = 0;
	while (n->keys[i] != key)
		i++;
	for (++i; i < n->num_keys; i++)
		n->keys[i - 1] = n->keys[i];


	num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
	i = 0;
	while (n->pointers[i] != pointer)
		i++;
	for (++i; i < num_pointers; i++)
		n->pointers[i - 1] = n->pointers[i];


	n->num_keys--;

	if (n->is_leaf)
		for (i = n->num_keys; i < order - 1; i++)
			n->pointers[i] = NULL;
	else
		for (i = n->num_keys + 1; i < order; i++)
			n->pointers[i] = NULL;

	return n;
}


node * adjust_root(node * root) {

	node * new_root;

	if (root->num_keys > 0)
		return root;


	if (!root->is_leaf) {
		new_root = root->pointers[0];
		new_root->parent = NULL;
	}


	else
		new_root = NULL;

	free(root->keys);
	free(root->pointers);
	free(root);

	return new_root;
}

node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime) {

	int i, j, neighbor_insertion_index, n_start, n_end, new_k_prime;
	node * tmp;
	bool split;


	if (neighbor_index == -1) {
		tmp = n;
		n = neighbor;
		neighbor = tmp;
	}


	neighbor_insertion_index = neighbor->num_keys;


	split = false;


	if (!n->is_leaf) {

		neighbor->keys[neighbor_insertion_index] = k_prime;
		neighbor->num_keys++;

		n_end = n->num_keys;

		n_start = 0; 
		if (n->num_keys + neighbor->num_keys >= order) {
			split = true;
			n_end = cut(order) - 2;
		}

		for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			neighbor->num_keys++;
			n->num_keys--;
			n_start++;
		}

		neighbor->pointers[i] = n->pointers[j];

		if (split) {
			new_k_prime = n->keys[n_start];
			for (i = 0, j = n_start + 1; i < n->num_keys; i++, j++) {
				n->keys[i] = n->keys[j];
				n->pointers[i] = n->pointers[j];
			}
			n->pointers[i] = n->pointers[j];
			n->num_keys--;
		}


		for (i = 0; i < neighbor->num_keys + 1; i++) {
			tmp = (node *)neighbor->pointers[i];
			tmp->parent = neighbor;
		}
	}

	else {
		for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++) {
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			neighbor->num_keys++;
		}
		neighbor->pointers[order - 1] = n->pointers[order - 1];
	}

	if (!split) {
		root = delete_entry(root, n->parent, k_prime, n);
		free(n->keys);
		free(n->pointers);
		free(n); 
	}
	else
		for (i = 0; i < n->parent->num_keys; i++)
			if (n->parent->pointers[i + 1] == n) {
				n->parent->keys[i] = new_k_prime;
				break;
			}

	return root;

}


node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, 
		int k_prime_index, int k_prime) {  

	int i;
	node * tmp;

	if (neighbor_index != -1) {
		if (!n->is_leaf)
			n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
		for (i = n->num_keys; i > 0; i--) {
			n->keys[i] = n->keys[i - 1];
			n->pointers[i] = n->pointers[i - 1];
		}
		if (!n->is_leaf) {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys];
			tmp = (node *)n->pointers[0];
			tmp->parent = n;
			neighbor->pointers[neighbor->num_keys] = NULL;
			n->keys[0] = k_prime;
			n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
		}
		else {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
			neighbor->pointers[neighbor->num_keys - 1] = NULL;
			n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
			n->parent->keys[k_prime_index] = n->keys[0];
		}
	}

	else {  
		if (n->is_leaf) {
			n->keys[n->num_keys] = neighbor->keys[0];
			n->pointers[n->num_keys] = neighbor->pointers[0];
			n->parent->keys[k_prime_index] = neighbor->keys[1];
		}
		else {
			n->keys[n->num_keys] = k_prime;
			n->pointers[n->num_keys + 1] = neighbor->pointers[0];
			tmp = (node *)n->pointers[n->num_keys + 1];
			tmp->parent = n;
			n->parent->keys[k_prime_index] = neighbor->keys[0];
		}
		for (i = 0; i < neighbor->num_keys; i++) {
			neighbor->keys[i] = neighbor->keys[i + 1];
			neighbor->pointers[i] = neighbor->pointers[i + 1];
		}
		if (!n->is_leaf)
			neighbor->pointers[i] = neighbor->pointers[i + 1];
	}

	n->num_keys++;
	neighbor->num_keys--;

	return root;
}


node * delete_entry( node * root, node * n, int key, void * pointer ) {

	int min_keys;
	node * neighbor;
	int neighbor_index;
	int k_prime_index, k_prime;
	int capacity;

	n = remove_entry_from_node(n, key, pointer);

	if (n == root) 
		return adjust_root(root);


	min_keys = n->is_leaf ? cut(order - 1) : cut(order) - 1;

	if (n->num_keys >= min_keys)
		return root;

	neighbor_index = get_neighbor_index( n );
	k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
	k_prime = n->parent->keys[k_prime_index];
	neighbor = neighbor_index == -1 ? n->parent->pointers[1] : 
		n->parent->pointers[neighbor_index];

	capacity = n->is_leaf ? order : order - 1;


	if (neighbor->num_keys + n->num_keys < capacity)
		return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);

	else
		return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}

node * delete(node * root, int key) {

	node * key_leaf;
	record * key_record;

	key_record = find(root, key, false);
	key_leaf = find_leaf(root, key, false);
	if (key_record != NULL && key_leaf != NULL) {
		root = delete_entry(root, key_leaf, key, key_record);
		free(key_record);
	}
	return root;
}


void destroy_tree_nodes(node * root) {
	int i;
	if (root->is_leaf)
		for (i = 0; i < root->num_keys; i++)
			free(root->pointers[i]);
	else
		for (i = 0; i < root->num_keys + 1; i++)
			destroy_tree_nodes(root->pointers[i]);
	free(root->pointers);
	free(root->keys);
	free(root);
}


node * destroy_tree(node * root) {
	destroy_tree_nodes(root);
	return NULL;
}


// MAIN

int main( int argc, char ** argv ) {

	char * input_file;
	FILE * fp;
	node * root;
	int input, range2;
	char instruction;
	char license_part;

	root = NULL;
	verbose_output = false;

	if (argc > 1) {
		order = atoi(argv[1]);
		if (order < MIN_ORDER || order > MAX_ORDER) {
			fprintf(stderr, "Orden Inválido: %d .\n\n", order);
			usage_3();
			exit(EXIT_FAILURE);
		}
	}

	usage_1();  
	usage_2();

	if (argc > 2) {
		input_file = argv[2];
		fp = fopen(input_file, "r");
		if (fp == NULL) {
			perror("Fallo en la apertura del archivo.");
			exit(EXIT_FAILURE);
		}
		while (!feof(fp)) {
			fscanf(fp, "%d\n", &input);
			root = insert(root, input, input);
		}
		fclose(fp);
		print_tree(root);
	}

	printf("> ");
	while (scanf("%c", &instruction) != EOF) {
		switch (instruction) {
		case 'd':
			scanf("%d", &input);
			root = delete(root, input);
			print_tree(root);
			break;
		case 'i':
			scanf("%d", &input);
			root = insert(root, input, input);
			print_tree(root);
			break;
		case 'f':
		case 'p':
			scanf("%d", &input);
			find_and_print(root, input, instruction == 'p');
			break;
		case 'r':
			scanf("%d %d", &input, &range2);
			if (input > range2) {
				int tmp = range2;
				range2 = input;
				input = tmp;
			}
			find_and_print_range(root, input, range2, instruction == 'p');
			break;
		case 'l':
			print_leaves(root);
			break;
		case 'q':
			while (getchar() != (int)'\n');
			return EXIT_SUCCESS;
		
		case 't':
			print_tree(root);
			break;

		case 'x':
			if (root)
				root = destroy_tree(root);
			print_tree(root);
			break;
		default:
			usage_2();
			break;
		}
		while (getchar() != (int)'\n');
		printf("> ");
	}
	printf("\n");

	return EXIT_SUCCESS;
}
