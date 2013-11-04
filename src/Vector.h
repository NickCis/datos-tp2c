#ifndef VECTOR_H
#define VECTOR_H
#include <stdbool.h>
#include <stddef.h>

typedef struct vector vector_t;

// Crea un vector de tamaño tam
vector_t* vector_crear(size_t tam);

// Destruye el vector(todos los elementos)
void vector_destruir(vector_t *vector,void destruir_dato (void*));

// Destruye el vector(los primeros cant elementos)
void vector_destruir_con_elementos(vector_t* vector, void destruir_dato(void*),size_t cant);

// Cambia el tamaño del vector y devuelve verdadero, en cambio, si el vector que dó intacto devuelve falso
bool vector_redimensionar(vector_t *vector, size_t nuevo_tam);

// Almacena en valor el dato guardado en la posición pos del vector
// Devuelve false si la posición es inválida (fuera del rango del vector, que va de 0 a tamaño-1)
void* vector_obtener(vector_t *vector, size_t pos);

// Almacena el valor en la posición pos
// Devuelve falso si la posición es inválida (fuera del rango del vector, que va de 0 a tamaño-1) y verdadero si se guardó el valor con éxito.
bool vector_guardar(vector_t *vector, size_t pos, void* valor);

// Devuelve el tamaño del vector
size_t vector_obtener_tamanio(vector_t *vector);

#endif
