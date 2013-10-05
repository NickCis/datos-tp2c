#include <errno.h>
#include <string.h>
#include "archivo_bloque.h"
#include "bloque.h"

struct TArchivo {
	size_t size_bloque;
	size_t cant_bloque;
	int reg_in_bloque;
	FILE* fd;
	TBloque *bloque;
	long size_file, file_pos;
	char* path;
};

TArchivo* Archivo_crear(char *path, size_t size){
	TArchivo* this = (TArchivo*) calloc(1, sizeof(TArchivo));

	this->size_bloque = size;

	if( (this->fd = fopen(path, "r+b")) == NULL){//Error
		int err = errno;
		if(err == ENOENT) // Archivo no existe
			this->fd = fopen(path, "w+b");
		else{
			printf("Error loco con el archivo. Errno: %d\n", err);
			free(this);
			return NULL;
		}
	}

	this->path = (char*) calloc(strlen(path) +1, sizeof(char));
	strcpy(this->path, path);

	fseek(this->fd, 0, SEEK_END);
	this->size_file = ftell(this->fd);
	fseek(this->fd, 0, SEEK_SET);

	this->cant_bloque = (size_t) this->size_file / this->size_bloque;
	
	this->bloque = Bloque_crear(this->size_bloque);

	return this;
}


int Archivo_bloque_seek(TArchivo* this, unsigned int n, int whence){
	if(!this)
		return 1;

	return fseek(this->fd, this->size_bloque * n, whence);
}

int Archivo_bloque_leer(TArchivo* this){
	if(!this)
		return 1;

	if(this->bloque)
		Bloque_destruir(this->bloque);

	this->reg_in_bloque = 0;

	this->bloque = Bloque_crear(this->size_bloque);
	return Bloque_leer(this->bloque, this->fd);
}

uint8_t* Archivo_get_buf(TArchivo* this, size_t* size){
	uint8_t* buf = NULL;

	if(!this || !this->bloque)
		return NULL;

	if(! (buf = Archivo_get_bloque_buf(this, size))){
		if(Archivo_bloque_leer(this))
			return NULL;
		return Archivo_get_bloque_buf(this, size);
	}

	return buf;
}

uint8_t* Archivo_get_bloque_buf(TArchivo* this, size_t* size){
	uint8_t* buf = NULL;

	if(!this || !this->bloque)
		return NULL;

	if(! (buf = Bloque_get_buf(this->bloque, this->reg_in_bloque ++, size))){ // Termino el bloque
		return NULL;
	}

	return buf;
}

int Archivo_agregar_buf(TArchivo* this, uint8_t* buff, size_t size){
	if(!this || !this->bloque)
		return 1;

	if(Bloque_agregar_buf(this->bloque, buff, size)){ //No entra
		if(Bloque_escribir(this->bloque, this->fd))
			return 1;
		if(Bloque_destruir(this->bloque))
			return 1;
		if(! (this->bloque = Bloque_crear(this->size_bloque)))
			return 1;
		return Bloque_agregar_buf(this->bloque, buff, size); //No entra en bloque vacio
	}

	return 0;
}

int Archivo_flush(TArchivo* this){
	if(!this || !this->bloque)
		return 1;

	if(Bloque_escribir(this->bloque, this->fd))
		return 1;

	return 0;
}

int Archivo_close(TArchivo* this){
	if(!this)
		return 1;

	this->file_pos = ftell(this->fd);
	printf("Cierro archivo %lu\n", this->file_pos);
	fclose(this->fd);

	this->fd = NULL;

	return 0;
}

int Archivo_open(TArchivo* this){
	if(!this || this->fd)
		return 1;

	// TODO: controlar errores
	this->fd = fopen(this->path, "r+b");
	printf("Abro archivo %lu\n", this->file_pos);
	fseek(this->fd, this->file_pos, SEEK_SET);

	return 0;
}

int Archivo_destruir(TArchivo* this){
	if(!this)
		return 1;

	if(this->fd)
		fclose(this->fd);
	if(this->bloque)
		Bloque_destruir(this->bloque);
	if(this->path)
		free(this->path);

	free(this);
	return 0;
}


