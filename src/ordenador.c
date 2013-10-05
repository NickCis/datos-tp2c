#include<stdio.h>

#include "ordenador.h"

typedef struct TNodo {
	void* buf;
	struct TNodo *sig;
} TNodo;

struct TOrdenador {
	TNodo * primero;
	TSortFunction cmp;
};


int _Ordenador_agregar(TOrdenador* this, TNodo* prev, TNodo* nodo, void *buf);

TOrdenador* Ordenador_crear(TSortFunction func){
	TOrdenador* this = (TOrdenador*) calloc(1, sizeof(TOrdenador));

	this->cmp = func;
	return this;
}

int Ordenador_agregar(TOrdenador* this, void *buf){
	if(!this)
		return 1;

	if(!this->primero){
		this->primero = (TNodo*) calloc(1, sizeof(TNodo));
		this->primero->buf = buf;
		return 0;
	}

	return _Ordenador_agregar(this, NULL, this->primero, buf);
}

int _Ordenador_agregar(TOrdenador* this, TNodo* prev, TNodo* nodo, void *buf){
	int cmp;

	if(!nodo)
		return 1;

	cmp = this->cmp((void*) nodo->buf, (void*) buf);
	if(cmp < 0){ //Menor que nodo
		TNodo* tmp = (TNodo*) calloc(1, sizeof(TNodo));
		tmp->buf = buf;
		tmp->sig = nodo;
		if(! prev)
			this->primero = tmp;
		else
			prev->sig = tmp;
		return 0;
	}

	if(! nodo->sig){
		nodo->sig = (TNodo*) calloc(1, sizeof(TNodo));
		nodo->sig->buf = buf;
		return 0;
	}

	return _Ordenador_agregar(this, nodo, nodo->sig, buf);
}

void* Ordenador_quitar(TOrdenador* this){
	void* buf;
	TNodo* sig;
	if(! this || ! this->primero)
		return NULL;

	buf = this->primero->buf;
	sig = this->primero->sig;
	free(this->primero);
	this->primero = sig;

	return buf;
}

int Ordenador_destruir(TOrdenador* this){
	if(!this)
		return 1;

	//Supongo que ya se saco todo;
	if(this->primero)
		printf("Faltaron liberar cosas del ordenador\n");
	free(this);
	return 0;
}
