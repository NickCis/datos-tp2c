#include "libres_bloques.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int Libres_read(char* path, size_t *len, unsigned int** arr){
	FILE* fd_libres;
	if( (fd_libres = fopen(path, "rb"))){//Abrio el archivo
		//TODO: controlar errores de lectura
		fread((void*) len, 1, sizeof(size_t), fd_libres);
		*arr = (unsigned int*) calloc(*len, sizeof(unsigned int));
		fread((void*) *arr, sizeof(unsigned int), *len, fd_libres);
		fclose(fd_libres);
	}else{
		*len = 0;
		*arr = NULL;
		return 1;
	}
	return 0;
}

int Libres_write(char* path, size_t len, unsigned int* arr){
	FILE* fd_libres = fopen(path, "wb");
	if(fd_libres == NULL) // Error
		printf("Error loco escribiendo archivo de libres. Errno: %d\n", errno);

	fwrite(&len, sizeof(size_t), 1, fd_libres);
	fwrite(arr, sizeof(unsigned int), len, fd_libres);
	fclose(fd_libres);
	return 0;
}

int Libre_agregar(size_t* len, unsigned int **arr, unsigned int l){
	(*len)++;
	*arr = (unsigned int*) realloc(*arr, (*len) * sizeof(unsigned int));
	(*arr)[(*len)-1] = l;
	return 0;
}

int Libre_pop(size_t* len, unsigned int *arr, unsigned int* l){
	if(! (*len))
		return 0;

	(*len)--;
	*l = arr[(*arr)];
	return 1;
}

