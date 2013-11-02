#include <string.h>
#include "bloque.h"

typedef struct {
	size_t size_write;
	uint8_t buf[];
} TBloqueBuf;

struct TBloque {
	size_t size;
	size_t size_reg;
	uint8_t* regs;
	TBloqueBuf* bloque;
};

TBloque* Bloque_crear(size_t size){
	TBloque* this = (TBloque*) calloc(1, sizeof(TBloque));

	this->size = size;
	this->bloque = (TBloqueBuf*) calloc(1, this->size);
	return this;
}

TBloque* BloqueFijo_crear(size_t size, size_t size_reg){
	TBloque* this = (TBloque*) calloc(1, sizeof(TBloque));

	this->size = size;
	this->size_reg = size_reg;
	this->bloque = (TBloqueBuf*) calloc(1, this->size);
	return this;
}

int Bloque_leer(TBloque* this, FILE* fd){
	size_t read = 0;
	if(!this)
		return 1;

	read = fread((void*) this->bloque, 1, this->size, fd);
	if(read != this->size )
		return -1;

	return 0;
}

int Bloque_escribir(TBloque* this, FILE* fd){
	size_t write = 0;
	if(!this)
		return 1;

	write = fwrite(this->bloque, 1, this->size, fd);
	if(write != this->size)
		return 1;

	return 0;
}

int Bloque_libre(TBloque* this, size_t size){
	size_t write_size;
	if(!this)
		return 1;

	//Tamano escrito + tamano header + size de lo qie se va a escribir
	write_size = this->bloque->size_write + sizeof(TBloqueBuf) + sizeof(size_t); 
	if(write_size >= this->size)
		return 2;

	if(size > this->size - write_size) //No entra
		return 2;

	return 0;
}

int BloqueFijo_libre(TBloque* this){
	if(!this)
		return 1;
	return Bloque_libre(this, this->size_reg+sizeof(size_t));
}

int Bloque_lleno(TBloque* this){
	if(!this)
		return 0;

	size_t max_size = BLOQUE_PORCENTAGE * this->size / 100;
	if(this->bloque->size_write >= max_size)
		return 1;
	return 0;
}

int Bloque_agregar_buf(TBloque* this, uint8_t* buff, size_t size){
	if(!this)
		return 1;

	if(Bloque_libre(this, size)) //No entra
		return 2;

	memcpy(this->bloque->buf+this->bloque->size_write, &size, sizeof(size_t));
	this->bloque->size_write += sizeof(size_t);

	memcpy(this->bloque->buf+this->bloque->size_write, buff, size);
	this->bloque->size_write += size;

	return 0;
}

int BloqueFijo_agregar_buf(TBloque* this, uint8_t* buff){
	if(!this)
		return 1;

	if(BloqueFijo_libre(this)) //No entra
		return 2;

	memcpy(this->bloque->buf+this->bloque->size_write, buff, this->size_reg);
	this->bloque->size_write += this->size_reg;

	return 0;
}



/** Solo para bloques de registros de longitud variable
 */
uint8_t* _Bloque_get_buf(TBloque* this, int n, size_t* size){
	uint8_t* buff = NULL;
	size_t pos = 0;
	size_t *tam = &pos;

	if(!this)
		return NULL;

	do {
		pos += *tam;
		tam = (size_t*) (this->bloque->buf + pos);
		pos +=  sizeof(size_t);
	} while( n-- > 0 && pos < this->bloque->size_write);

	if(pos > this->bloque->size_write)
		return NULL;

	*size = *tam;
	buff = (uint8_t*) malloc(*tam);
	memcpy(buff, this->bloque->buf+pos, *tam);

	return buff;
}

/** Solo para bloques de registros de longitud fija
 */
uint8_t* _BloqueFijo_get_buf(TBloque* this, int n, size_t* size){
	uint8_t* buff = NULL;
	size_t pos = 0;

	if(!this)
		return NULL;

	*size = this->size_reg;
	pos = n * this->size_reg;

	if(pos > this->bloque->size_write)
		return NULL;

	buff = (uint8_t*) malloc(this->size_reg);
	memcpy(buff, this->bloque->buf+pos, this->size_reg);

	return buff;
}

uint8_t* Bloque_get_buf(TBloque* this, int n, size_t* size){
	if(!this)
		return NULL;

	if(this->size_reg)
		return _BloqueFijo_get_buf(this, n, size);
	else
		return _Bloque_get_buf(this, n, size);
}

int Bloque_destruir(TBloque* this){
	if(!this)
		return 1;

	free(this->bloque);
	free(this);

	return 0;
}
