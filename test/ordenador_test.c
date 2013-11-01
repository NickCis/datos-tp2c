#include <stdio.h>
#include "../src/ordenador.h"

//typedef struct TOrdenador TOrdenador;
//TOrdenador* Ordenador_crear(TSortFunction func);
//int Ordenador_agregar(TOrdenador* this, void *buf);
//void* Ordenador_quitar(TOrdenador* this);
//int Ordenador_destruir();

int cmp(void* a, void* b){
	int *c = (int*) a, *d = (int*) b;
	if(*c < *d)
		return 1;
	else if(*c > *d)
		return -1;

	return 0;
}
int main(int argc, char* argv[]){
	int enteros[] = {
		32,
		1,
		10,
		24,
		2,
		5,
		99,
		124,
		2,
		1,
		0,
		-12,
		4561,
		-19
	}, cant = 14, i=0;

	TOrdenador* ord = Ordenador_crear(&cmp);

	for(i=0; i < cant; i++){
		Ordenador_agregar(ord, (void*) &(enteros[i]));
	}

	for(i=0; i < cant; i++){
		int* n = (int*) Ordenador_quitar(ord);
		printf("%d) %d\n", i, *n);
	}

	Ordenador_destruir(ord);

	return 0;
}
