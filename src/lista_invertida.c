#include <string.h>
#include "lista_invertida.h"
#include "libres_bloques.h"
#include "archivo_bloque.h"


struct TListaInvertida {
	size_t block_size;
	char* path;
	char* path_baja;
	TArchivo* arch_bloques;
	unsigned int* libres;
	size_t libres_len;
	unsigned int cur_lista;
	unsigned int cur_next;
	unsigned int cur_first;
	char read;
};

typedef struct {
	unsigned int size;
	uint8_t* buf;
} _Reg;

TListaInvertida* ListaInvertida_crear(char* path, char* path_baja, size_t block_size){
	TListaInvertida* this;

	if(!path || !path_baja || !block_size)
		return NULL;

	this = (TListaInvertida*) calloc(1, sizeof(TListaInvertida));

	this->cur_lista = 0;

	this->block_size = block_size;
	this->path = strcpy(calloc(1, strlen(path)+1), path);
	this->path_baja = strcpy(calloc(1, strlen(path_baja)+1), path_baja);

	this->arch_bloques = Archivo_crear_adm(this->path, NULL, this->block_size);

	Libres_read(path_baja, &(this->libres_len), &(this->libres));

	return this;
}

unsigned int ListaInvertida_new(TListaInvertida* this){
	//TODO: checkear errores
	unsigned int block_num;
	unsigned int next = 0;

	if(! Libre_pop(&(this->libres_len), this->libres, &block_num))
		block_num = Archivo_cant_bloque(this->arch_bloques);

	Archivo_bloque_seek(this->arch_bloques, block_num, SEEK_SET);

	Archivo_bloque_new(this->arch_bloques);
	Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) &next, sizeof(size_t));
	Archivo_flush(this->arch_bloques);

	Archivo_bloque_seek(this->arch_bloques, block_num, SEEK_SET);

	this->read = 0;
	this->cur_lista = block_num;

	return block_num;
}

int ListaInvertida_erase(TListaInvertida* this){
	size_t aux;
	unsigned int next;
	uint8_t* buf;
	if(!this)
		return 1;

	next = this->cur_first;
	do{
		//TODO:
		//Libre_agregar(&(this->libres_len), &(this->libres), next);
		Archivo_bloque_seek(this->arch_bloques, next, SEEK_SET);
		Archivo_bloque_leer(this->arch_bloques);
		buf = Archivo_get_bloque_buf(this->arch_bloques, &aux);
		Archivo_bloque_seek(this->arch_bloques, next, SEEK_SET);
		next = *( (unsigned int*) buf);
		free(buf);
		Archivo_bloque_new(this->arch_bloques);
		Archivo_flush(this->arch_bloques);
	} while(next);

	return 0;
}

int ListaInvertida_set(TListaInvertida* this, unsigned int ref){
	if(!this)
		return 1;

	this->read = 0;
	this->cur_next = 0;
	this->cur_first = this->cur_lista = ref;
	if(Archivo_bloque_seek(this->arch_bloques, ref, SEEK_SET)){
		printf("error seek \n");
	}
	return 0;
}

uint8_t* ListaInvertida_get(TListaInvertida* this, size_t* size){
	uint8_t * reg;
	if(!this)
		return NULL;

	if(this->read == 0){
		this->read = 1;
		if(Archivo_bloque_leer(this->arch_bloques)){
			printf("error leyendo\n");
		}
		reg = Archivo_get_bloque_buf(this->arch_bloques, size);

		if(!reg)
			return NULL;

		this->cur_next = * ((unsigned int*) reg);
		free(reg);
	}

	reg = Archivo_get_bloque_buf(this->arch_bloques, size);

	if(reg == NULL && this->cur_next){
		unsigned int first = this->cur_first;
		Archivo_bloque_seek(this->arch_bloques, this->cur_next, SEEK_SET);
		this->read = 0;
		this->cur_first = first;
		return ListaInvertida_get(this, size);
	}

	return reg;
}

int ListaInvertida_agregar(TListaInvertida* this, uint8_t* ele, size_t size){
	// TODO: checkear errores
	if(!this)
		return 1;

	if(this->read == 0){
		uint8_t* aux;
		size_t aux_s;
		this->read = 1;
		if(Archivo_bloque_leer(this->arch_bloques)){
			printf("error leyendo\n");
		}
		aux = Archivo_get_bloque_buf(this->arch_bloques, &aux_s);

		if(!aux)
			printf("error leyendo next\n");
		
		this->cur_next = * ((unsigned int*) aux);
		free(aux);

		Archivo_bloque_seek(this->arch_bloques, this->cur_lista, SEEK_SET);
	}

	if(Archivo_bloque_agregar_buf(this->arch_bloques, ele, size)){// se lleno
		Archivo_flush(this->arch_bloques);
		if(this->cur_next == 0){
			unsigned int block_num;
			uint8_t * aux;
			size_t aux_s;
			if(! Libre_pop(&(this->libres_len), this->libres, &block_num))
				block_num = Archivo_cant_bloque(this->arch_bloques);

			Archivo_bloque_seek(this->arch_bloques, this->cur_lista, SEEK_SET);

			Archivo_bloque_leer(this->arch_bloques);
			aux = Archivo_get_bloque_buf(this->arch_bloques, &aux_s);
			free(aux);

			// TODO: reemplazar porlista
			_Reg aux_r[256] = {{0}};

			int i=0;
			while( ( aux_r[i].buf = Archivo_get_bloque_buf(this->arch_bloques, &(aux_r[i].size)))){
				i++;
			}

			Archivo_bloque_seek(this->arch_bloques, this->cur_lista, SEEK_SET);
			Archivo_bloque_new(this->arch_bloques);
			Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) &block_num, sizeof(unsigned int));

			int k;
			for(k=0; k < i; k++){
				Archivo_bloque_agregar_buf(this->arch_bloques, aux_r[k].buf, aux_r[k].size);
				free(aux_r[k].buf);
			}

			Archivo_flush(this->arch_bloques);

			Archivo_bloque_seek(this->arch_bloques, block_num, SEEK_SET);
			Archivo_bloque_new(this->arch_bloques);
			this->cur_lista = block_num;
			block_num = 0;

			Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) &block_num, sizeof(unsigned int));
			Archivo_bloque_agregar_buf(this->arch_bloques, ele, size);
			this->cur_next = 0;
		}else{
			unsigned int first = this->cur_first;
			ListaInvertida_set(this, this->cur_next);
			this->cur_first = first;
			return ListaInvertida_agregar(this, ele, size);
		}
	}

	return 0;
}

int ListaInvertida_escribir(TListaInvertida* this){
	if(!this)
		return 1;

	return Archivo_flush(this->arch_bloques);
}

int ListaInvertida_destruir(TListaInvertida* this){
	if(!this)
		return 1;

	//ListaInvertida_escribir(this);
	Libres_write(this->path_baja, this->libres_len, this->libres);
	free(this->libres);
	free(this->path);
	free(this->path_baja);

	Archivo_destruir(this->arch_bloques);
	free(this);

	return 0;
}
