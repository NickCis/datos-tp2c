#ifndef LISTA_H
#define LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* PRIMITIVAS LISTA ENLAZADA */

typedef struct lista lista_t;

typedef struct lista_iter lista_iter_t;

//Crea una lista enlazada vacía. Devuelve la lista o NULL si no la pudo crear.
lista_t *lista_crear();

//Devuelve verdadero si la lista se encuentra vacía, en caso contrario o no se le pasara
//la lista por parámetro devuelve falso.
bool lista_esta_vacia(const lista_t *lista);

//Inserta el elemento recibido por parámetro al principio de la lista. Devuelve verdadero si se pudo realizar la inserción
// caso contrario devolverá falso. Si la lista pasada por parámetro o el elemento no existen, o no
//se pudo crear el nodo para contener al elemento también se devolverá falso.
bool lista_insertar_primero(lista_t *lista, void *dato);

//Inserta el elemento recibido por parámetro al final de la lista. Devuelve verdadero si se pudo insertar
//correctamente o falso en su defecto. Si la lista pasada por parámetro o elelemento no existen, o no
//se pudo crear el nodo para contener al elemento también se devolverá falso.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Borra el primer elemento de una lista y devuelve su valor. Devuelve NULL si no se le pasó la lista por parametro.
void *lista_borrar_primero(lista_t *lista);

//Devuelve el valor del primer elemento de la lista. Devuelve NULL si no se le paso
//la lista por parámetro o si la lista no contiene ningún dato.
void *lista_ver_primero(const lista_t *lista);

//Devuelve el tamanio de la lista.
size_t lista_tamanio(const lista_t *lista);

//Destruye todos los nodos de lista y recibe una función para borrar los datos
//de cada nodo. Si no se le pasa función, no los destruye. Si no se le pasa
//la lista por parámetro no hace nada. Libera el espacio de memoria ocupado
//por la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));


//Iteración de la lista enlazada recibida por parámetro. Si no se le pasa
//la lista, o no se pudo crear el iterador, devuelve NULL. En caso contrario, devolverá
//el iterador.
lista_iter_t *lista_iter_crear(const lista_t *lista);

//El iterador avanza una posición sobre la lista. Si está al final de la lista, o no se le
//pasa la lista por parámetro, devuelve falso. En caso contrario, devuelve verdadero y se posiciona
//en el siguiente nodo de la lista.
bool lista_iter_avanzar(lista_iter_t *iter);

//El iterador avanza una posición sobre la lista. Si está al final de la lista, o no se le
//pasa la lista por parámetro, devuelve falso. En caso contrario, devuelve verdadero y se posiciona
//en el siguiente nodo de la lista.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Verifica si el iterador está posicionado sobre el final de la lista. Devuelve falso
//si no lo está o si no se le paso iterador por parámetro y verdadero si lo está.
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador, si es que se lo pasó por parámetro.
void lista_iter_destruir(lista_iter_t *iter);


//Inserta un elemento en la posición actual del iterador. Si no se le pasa dato o iterador o
//lista por parametró o no se pudo crear el nodo para contener el dato, devuelve false. En caso
//contrario, devuelve true e inserta el elemento.
bool lista_insertar(lista_t *lista, lista_iter_t *iter, void *dato);

//Borra el elemento en la posición actual del iterador. Si no se le pasa iterador o
//lista por parametró o la lista esta vacía, devuelve NULL. En caso
//contrario, borra el elemento de la lista y lo devuelve.
void *lista_borrar(lista_t *lista, lista_iter_t *iter);

#endif // LISTA_H
