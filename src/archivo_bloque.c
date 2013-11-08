#include <errno.h>
#include <string.h>
#include "archivo_bloque.h"
#include "bloque.h"

struct TArchivo {
	size_t size_bloque;
	size_t size_reg;
	size_t cant_bloque;
	size_t cur_bloque; // -> Va a indicar el numero de bloque en donde esta parado fd en el archivo
	int reg_in_bloque;
	FILE* fd;
	TBloque *bloque;
	long size_file, file_pos;
	char* path;
	char* path_adm;
	uint8_t* mapa_bits;
	size_t mapa_bits_size;
};

/** Escribe el flag de control de memoria del bloque actual
 * funcion de uso interno.
 */
void escribir_info_mapa(TArchivo* this){
	if(this->mapa_bits){
		size_t pos = this->cur_bloque / 8;
		if(pos >= this->mapa_bits_size){
			//this->mapa_bits_size++;
			this->mapa_bits_size = pos+1;
			this->mapa_bits = (uint8_t*) realloc(this->mapa_bits, this->mapa_bits_size);
		}

		//printf("bloque: '%d' mapa de bits pos: '%d' offset: '%d' bloque lleno: '%d' \n", this->cur_bloque, pos, (this->cur_bloque % 8), (Bloque_lleno(this->bloque)));

		int flag = (Bloque_lleno(this->bloque)) << (this->cur_bloque % 8);
		flag |= this->mapa_bits[pos];
		this->mapa_bits[pos] = flag;
	}
}

TArchivo* Archivo_crear(char *path, size_t size){
	return Archivo_crear_adm(path, NULL, size);
}

TArchivo* ArchivoFijo_crear(char *path, size_t size, size_t size_reg){
	return ArchivoFijo_crear_adm(path, NULL, size, size_reg);
}

TArchivo* Archivo_crear_adm(char *path, char *path_adm , size_t size){
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

	if(path_adm){
		this->path_adm = (char*) calloc(strlen(path_adm) +1, sizeof(char));
		strcpy(this->path_adm, path_adm);
		this->mapa_bits_size = this->cant_bloque / 8;
		if(this->cant_bloque % 8)
			this->mapa_bits_size++;

		this->mapa_bits = (uint8_t*) calloc(1, this->mapa_bits_size);

		FILE* fd_mapa;
		if( (fd_mapa = fopen(path_adm, "r+b"))){//Abrio el archivo
			fread((void*) this->mapa_bits, 1, this->mapa_bits_size, fd_mapa);
			fclose(fd_mapa);
		}

	}

	return this;
}

TArchivo* ArchivoFijo_crear_adm(char *path, char *path_adm , size_t size, size_t size_reg){
	TArchivo* this = Archivo_crear_adm(path, path_adm, size);
	if(this){
		this->size_reg = size_reg;
		Archivo_bloque_new(this);
	}

	return this;
}


int Archivo_bloque_seek(TArchivo* this, unsigned int n, int whence){
	if(!this)
		return 1;

	switch(whence){
		case SEEK_SET:
			this->cur_bloque = n;
			break;

		case SEEK_CUR:
			this->cur_bloque += n;
			break;

		case SEEK_END:
			this->cur_bloque = this->cant_bloque - n;
			break;

		default:
			break;
	}

	//if(this->bloque)
	//	Bloque_destruir(this->bloque);

	//this->bloque = Bloque_crear(this->size_bloque);

	return fseek(this->fd, this->size_bloque * n, whence);
}

int Archivo_bloque_new(TArchivo* this){
	if(!this)
		return 1;

	if(this->bloque)
		Bloque_destruir(this->bloque);

	if(this->size_reg)
		this->bloque = BloqueFijo_crear(this->size_bloque, this->size_reg);
	else
		this->bloque = Bloque_crear(this->size_bloque);

	if(this->bloque)
		return 0;

	return 1;
}

int Archivo_bloque_leer(TArchivo* this){
	if(!this)
		return 1;

	//if(this->bloque)
	//	Bloque_destruir(this->bloque);

	this->reg_in_bloque = 0;
	this->cur_bloque++;

	//this->bloque = Bloque_crear(this->size_bloque);
	Archivo_bloque_new(this);
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

uint8_t* Archivo_bloque_get_buf(TArchivo* this, int n, size_t* size){
	if(!this || !this->bloque)
		return NULL;

	return Bloque_get_buf(this->bloque, n, size);
}

int Archivo_agregar_buf(TArchivo* this, uint8_t* buff, size_t size){
	if(!this || !this->bloque)
		return 1;

	if(Bloque_agregar_buf(this->bloque, buff, size)){ //No entra
		if(Bloque_escribir(this->bloque, this->fd))
			return 1;

		escribir_info_mapa(this);

		this->cur_bloque++;
		//if(Bloque_destruir(this->bloque))
		//	return 1;
		//if(! (this->bloque = Bloque_crear(this->size_bloque)))
		//	return 1;
		if(Archivo_bloque_new(this))
			return 1;

		return Bloque_agregar_buf(this->bloque, buff, size); //No entra en bloque vacio
	}

	return 0;
}

int ArchivoFijo_agregar_buf(TArchivo* this, uint8_t* buff){
	if(!this || !this->bloque)
		return 1;

	if(BloqueFijo_agregar_buf(this->bloque, buff)){ //No entra
		if(Bloque_escribir(this->bloque, this->fd))
			return 1;

		escribir_info_mapa(this);

		this->cur_bloque++;
		//if(Bloque_destruir(this->bloque))
		//	return 1;
		//if(! (this->bloque = Bloque_crear(this->size_bloque)))
		//	return 1;
		if(Archivo_bloque_new(this))
			return 1;
		return BloqueFijo_agregar_buf(this->bloque, buff); //No entra en bloque vacio
	}

	return 0;
}

int Archivo_bloque_agregar_buf(TArchivo* this, uint8_t* buff, size_t size){
	return Bloque_agregar_buf(this->bloque, buff, size);
}

int ArchivoFijo_bloque_agregar_buf(TArchivo* this, uint8_t* buff){
	return BloqueFijo_agregar_buf(this->bloque, buff);
}

int Archivo_bloque_libre(TArchivo* this, size_t size){
	return Bloque_libre(this->bloque, size);
}

int ArchivoFijo_bloque_libre(TArchivo* this){
	return BloqueFijo_libre(this->bloque);
}

int Archivo_flush(TArchivo* this){
	if(!this || !this->bloque)
		return 1;

	escribir_info_mapa(this);

	this->cur_bloque++;
	if(this->cur_bloque > this->cant_bloque)
		this->cant_bloque = this->cur_bloque;

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

	if(this->path_adm){
		FILE* fd_adm = fopen(this->path_adm, "w+b");
		if(fd_adm == NULL) // Error
			printf("Error loco escribiendo archivo de mapa de bits. Errno: %d\n", errno);

		//printf("tama~no de mapa de bits '%d'\n", this->mapa_bits_size);
		fwrite(this->mapa_bits, 1, this->mapa_bits_size, fd_adm);
		fclose(fd_adm);
	}
	if(this->mapa_bits)
		free(this->mapa_bits);
	if(this->fd)
		fclose(this->fd);
	if(this->bloque)
		Bloque_destruir(this->bloque);
	if(this->path)
		free(this->path);
	if(this->path_adm)
		free(this->path_adm);

	free(this);
	return 0;
}

int Archivo_libre(TArchivo* this, size_t n_bloque){
	if(!this)
		return 0;

	size_t pos = n_bloque / 8;

	if(pos > this->mapa_bits_size){
		printf("NUMERO BLOQUE MAYOR A TAMA~NO MAPA\n");
		return 0;
	}

	int flag = 1 << (n_bloque % 8);
	return (flag &= this->mapa_bits[pos]) ? 1 : 0;
}

int Archivo_get_cur_bloque(TArchivo* this){
	return this->cur_bloque;
}

size_t Archivo_cant_bloque(TArchivo* this){
	if(!this)
		return 0;
	return this->cant_bloque;
}
