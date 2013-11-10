#include <errno.h>
#include "archivo_registro.h"

struct TArchivoReg {
	FILE* fd;
};

TArchivoReg* ArchivoReg_crear(char* path){
	TArchivoReg* this = (TArchivoReg*) calloc(1, sizeof(TArchivoReg));

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

	return this;
}

uint8_t* ArchivoReg_leer(TArchivoReg* this, size_t* size){
	uint8_t *buf = NULL;

	if(!this)
		return NULL;

	if(fread(size, 1, sizeof(size_t), this->fd) != sizeof(size_t))
		return NULL;

	buf = (uint8_t*) calloc(1, *size);

	if(fread(buf, 1, *size, this->fd) != *size){
		free(buf);
		return NULL;
	}

	return buf;
}


int ArchivoReg_escribir(TArchivoReg* this, uint8_t* buff, size_t size){
	if(!this)
		return 1;

	if(fwrite(&size, 1, sizeof(size_t), this->fd) != sizeof(size_t))
		return 2;

	if(fwrite(buff, 1, size, this->fd) != size)
		return 3;

	return 0;
}


int ArchivoReg_destruir(TArchivoReg* this){
	if(!this)
		return 1;

	fclose(this->fd);
	free(this);

	return 0;
}

int ArchivoReg_seek_end(TArchivoReg* this){
	if(!this)
		return 1;
	fseek(this->fd, 0, SEEK_END);
	return 0;
}

int ArchivoReg_seek_start(TArchivoReg* this){
	if(!this)
		return 1;
	fseek(this->fd, 0, SEEK_SET);
	return 0;
}
