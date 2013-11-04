#ifndef ArbolBmas_H
#define ArbolBmas_H
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

//Primer mensaje de usuario
void usage_1( void );

//Segundo mensaje de usuario
void usage_2( void );

// Breve descripción de uso
void usage_3( void );

//Función de ayuda para imprimir arbol de salida
void enqueue( node * new_node );

//Función de ayuda para imprimir arbol de salida
node * dequeue( void );

// Imprime la fila inferior de llaves
void print_leaves( node * root );

//Función que calcula la altura de un arbol desde la raiz hasta cualquier hoja
int height( node * root );

//Función que calcula la longitud desde cualquier nodo de la raiz
int path_to_root( node * root, node * child );

// Imprime el arbol con las llaves en cada nodo y el símbolo'|' para separar los nodos 
void print_tree( node * root );

// Busca el registro con una determinada llave y la imprime
void find_and_print(node * root, int key, bool verbose); 

//Busca e imprime las llaves en un rango determinado
void find_and_print_range(node * root, int range1, int range2, bool verbose); 

// Encuentra llaves y sus punteros, si está presente, en el rango especificado por key_start y key_end, inclusive. Coloca estos en los arrays Returned_keys y returned_pointers y devuelve el número de entradas encontradas.
int find_range( node * root, int key_start, int key_end, bool verbose,
		int returned_keys[], void * returned_pointers[]);


// Rastrea la ruta desde la raíz a una hoja, buscando por clave. Devuelve la hoja que contiene la clave dada.
node * find_leaf( node * root, int key, bool verbose );


// Encuentra y devuelve el registro a la que una llave se refiere
record * find( node * root, int key, bool verbose );

//Busca el lugar apropiado para dividri un nodo que es demasiado grande
int cut( int length );

// Crea un nuevo registro para almacenar la llave
record * make_record(int value);

// Crea un nuevo nodo ya sea nodo hoja o interno
node * make_node( void );

//Crea una nueva hoja
node * make_leaf( void );

// Función utilizada en insert_into_parent. Para buscar el índice del puntero del padre.
int get_left_index(node * parent, node * left);

// Inserta un nuevo puntero  a un registro y su correspondiente llave en una hoja. Devuelve la hoja actualizada
node * insert_into_leaf( node * leaf, int key, record * pointer );

//Inserta una nueva clave y puntero a un nuevo registro en una hoja de modo que exceda de la orden del árbol, haciendo que la hoja se divida en dos.
node * insert_into_leaf_after_splitting(node * root, node * leaf, int key, record * pointer);

// Inserta una nueva clave y puntero a un nodo en un nodo en el que éstos pueden adaptarse sin violar las propiedades del árbol B +.
node * insert_into_node(node * root, node * parent, 
		int left_index, int key, node * right);

//Inserta un duplicado de la llave y el puntero a un nodo en un nodo, haciendo que el tamaño del nodo supere el orden, razón por la cual el nodo se dividió en dos.
node * insert_into_node_after_splitting(node * root, node * parent, int left_index, 
		int key, node * right);

//Inserta un nodo (hoja o nodo interno) en el árbol B +. Devuelve la raíz del árbol después de la inserción.
node * insert_into_parent(node * root, node * left, int key, node * right);

//Crea una nueva raíz de dos subárboles e inserta la clave adecuada en la nueva raíz.
node * insert_into_new_root(node * left, int key, node * right);

//Primera inserción
node * start_new_tree(int key, record * pointer);

//Inserta una clave y un valor asociado en el árbol B +, manteniendo las propiedades del árbol B +.
node * insert( node * root, int key, int value );


//Recupera el índice del vecino más cercano de un nodo (hermano) a la izquierda si es que existe. Si no es así (el nodo es el hijo más a la izquierda), devuelve -1 para indicar este caso especial.
int get_neighbor_index( node * n );

//Ajusta la raíz
node * adjust_root(node * root);

// Balanceo del nodo
node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime);

//Redistribución entre nodos
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index, 
		int k_prime_index, int k_prime);

// Elimina una entrada del arbol B+
node * delete_entry( node * root, node * n, int key, void * pointer );

// Elimina
node * delete( node * root, int key );

// Elimina la entrada del nodo y actualiza los punteros
node * remove_entry_from_node(node * n, int key, node * pointer);

// Destruye nodo del arbol
void destroy_tree_nodes(node * root);

// Destruye el arbol
node * destroy_tree(node * root);

#endif // ArbolBmas_H
