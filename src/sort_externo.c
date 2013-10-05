#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sort_externo.h"
#include "heapsort.h"
//#include "ordenador.h"
#include "archivo_bloque.h"
//#include "archivo_registro.h"

#define DEF_TEMP_NAME "sort_externo.tmp.%d.dat"
#define DEF_TEMP_ARR_SIZE (50)

typedef struct {
	uint8_t *buf;
	size_t size;
	TSortExternFunction cmp;
} TSortHelper;

int sort_helper_cmp(void *hlp1, void* hlp2){
	TSortHelper *h1 = hlp1, *h2 = hlp2;

	// Para facilitar el uso del array, ordeno de mayor a menor y duermo los registros al final (por eso el -1)
	return -1 * (h1->cmp((void*) h1->buf, h1->size, (void*) h2->buf, h2->size));
}

int replacememt_selection(char* in_file, char* tmp_name, size_t block_size, size_t array_size, TSortExternFunction cmp){
	TArchivo *in, *arch_tmp;
	void** tmp_array;
	unsigned int pasada = 0;
	uint8_t *buf;
	size_t buf_size=0, largo_actual = 0, offset = 0;;
	char path[255] = {0};
	//TArchivoReg* arch_tmp;
	TSortHelper *last = NULL;

	if(!in_file || !tmp_name || !block_size || !array_size)
		return 1;

	tmp_array = (void**) calloc(array_size, sizeof(void*));

	sprintf(path, tmp_name, pasada);
	//arch_tmp = ArchivoReg_crear(path);
	arch_tmp = Archivo_crear(path, block_size);

	in = Archivo_crear(in_file, block_size);
	Archivo_bloque_leer(in);

	while( (buf = Archivo_get_buf(in, &buf_size)) ){
		TSortHelper* tmp_sort = (TSortHelper*) calloc(1, sizeof(TSortHelper));
		tmp_sort->size = buf_size;
		tmp_sort->buf = buf;
		tmp_sort->cmp = cmp;

		if(largo_actual >= array_size - offset){
			if(last){
				free(last->buf);
				free(last);
			}

			// El orden que se genera es de mayor a menor
			heapsort(tmp_array, largo_actual, &sort_helper_cmp);
			last = tmp_array[largo_actual-1];
			//ArchivoReg_escribir(arch_tmp, last->buf, last->size);
			Archivo_agregar_buf(arch_tmp, last->buf, last->size);
			tmp_array[largo_actual-1] = NULL;
			largo_actual--;
		}

		if(last != NULL && cmp(last->buf, last->size, tmp_sort->buf, tmp_sort->size) < 0 ){
			//El ultimo que se escribio en archivo es mayor al que se quiere escribir
			offset++;
			tmp_array[array_size-offset] = tmp_sort;

			if(offset == array_size){
				//Ya esta todo dormido. Creo archivo nuevo
				//ArchivoReg_destruir(arch_tmp);

				Archivo_flush(arch_tmp);
				Archivo_destruir(arch_tmp);

				sprintf(path, tmp_name, ++pasada);
				//arch_tmp = ArchivoReg_crear(path);
				arch_tmp = Archivo_crear(path, block_size);

				offset = 0;
				largo_actual = array_size;
			}
		}else
			tmp_array[largo_actual++] = tmp_sort;

	}

	Archivo_destruir(in);

	free(last->buf);
	free(last);

	// Escribir las cosas que qedaron en el buffer a disco
	if(largo_actual){
		size_t largo = largo_actual;
		heapsort(tmp_array, largo, &sort_helper_cmp);
		while(largo){
			last = tmp_array[--largo];
			//ArchivoReg_escribir(arch_tmp, last->buf, last->size);
			Archivo_agregar_buf(arch_tmp, last->buf, last->size);
			free(last->buf);
			free(last);
		}
	}

	//ArchivoReg_destruir(arch_tmp);
	Archivo_flush(arch_tmp);
	Archivo_destruir(arch_tmp);

	// Escribo los que quedaron dormidos en un nuevo archivo
	if(offset){
		//void **offset_array = tmp_array + sizeof(void*) * largo_actual;
		void **offset_array = tmp_array + largo_actual;
		sprintf(path, tmp_name, ++pasada);
		//arch_tmp = ArchivoReg_crear(path);
		arch_tmp = Archivo_crear(path, block_size);

		heapsort(offset_array, offset, &sort_helper_cmp);

		while(offset){
			last = offset_array[--offset];
			//ArchivoReg_escribir(arch_tmp, last->buf, last->size);
			Archivo_agregar_buf(arch_tmp, last->buf, last->size);
			free(last->buf);
			free(last);
		}

		//ArchivoReg_destruir(arch_tmp);
		Archivo_flush(arch_tmp);
		Archivo_destruir(arch_tmp);
	}

	free(tmp_array);

	return pasada;
}

int sort_externo(size_t size, char* in_file, char* out_file, TSortExternFunction cmp){
	int pasada =0;

	if(!size || !in_file || !out_file)
		return 1;

	if( ! (pasada = replacememt_selection(in_file, DEF_TEMP_NAME, size, DEF_TEMP_ARR_SIZE, cmp)) ){
		// El archivo ya estaba ordenado
		char path[255] = {0};
		sprintf(path, DEF_TEMP_NAME, pasada);
		return rename(path, out_file);
	}


	return merge_externo(out_file, DEF_TEMP_NAME, size, DEF_TEMP_ARR_SIZE, 0, pasada, cmp);
}

typedef struct {
	uint8_t *buf;
	size_t size;
	TSortExternFunction cmp;
	TArchivo* arch;
} TMergeHelper;

int merge_helper_cmp(void *hlp1, void* hlp2){
	TMergeHelper *h1 = hlp1, *h2 = hlp2;

	// Para facilitar el uso del array, ordeno de mayor a menor y duermo los registros al final (por eso el -1)
	return -1 * (h1->cmp((void*) h1->buf, h1->size, (void*) h2->buf, h2->size));
}

int merge_externo(char* out_path, char* tmp_name, size_t block_size, size_t array_size, size_t desde, size_t hasta, TSortExternFunction cmp){
	TMergeHelper** tmp_array;
	size_t i=0, es_ultimo = 0, real_array_size = array_size;
	char path[255] = {0};
	TArchivo* out_file = NULL;

	if(!out_path || !tmp_name || !block_size || !array_size || hasta <= desde)
		return 1;

	if(hasta - desde +1 <= array_size){
		real_array_size = array_size = hasta-desde+1;
		es_ultimo = 1;
	}
	printf("Mergeo desde %d hasta %d con array: %d\n", desde, hasta, array_size);

	tmp_array = (TMergeHelper**) calloc(array_size, sizeof(void*));
	for(i=0; i < array_size; i++){
		tmp_array[i] = (TMergeHelper*) calloc(1, sizeof(TMergeHelper));
		tmp_array[i]->cmp = cmp;

		sprintf(path, tmp_name, desde+i);
		tmp_array[i]->arch = Archivo_crear(path, block_size);
		if(Archivo_bloque_leer(tmp_array[i]->arch)){
			Archivo_destruir(tmp_array[i]->arch);
			array_size--;
			i--;
			continue;
		}

		Archivo_close(tmp_array[i]->arch);

		if(!(tmp_array[i]->buf = Archivo_get_bloque_buf(tmp_array[i]->arch, &(tmp_array[i]->size)))){
			Archivo_destruir(tmp_array[i]->arch);
			array_size--;
			i--;
			continue;
		}
	}

	desde += array_size;
	sprintf(path, tmp_name, ++hasta);
	out_file = Archivo_crear((es_ultimo)? out_path : path, block_size);

	while(1){ //Cabeza mode: on
		heapsort((void**) tmp_array, array_size, &merge_helper_cmp);
		Archivo_agregar_buf(out_file, tmp_array[array_size-1]->buf, tmp_array[array_size-1]->size);
		free(tmp_array[array_size-1]->buf);

		if(! (tmp_array[array_size-1]->buf = Archivo_get_bloque_buf(
						tmp_array[array_size-1]->arch,
						&(tmp_array[array_size-1]->size)
		)) ){ // NULL leo siguiente bloque
			Archivo_open(tmp_array[array_size-1]->arch);
			if(Archivo_bloque_leer(tmp_array[array_size-1]->arch)){
				//Se termino el archivo.
				Archivo_destruir(tmp_array[array_size-1]->arch);

				if(! (--array_size)) // No hay mas archivos
					break;
			}else{
				Archivo_close(tmp_array[array_size-1]->arch);
				if(! (tmp_array[array_size-1]->buf = Archivo_get_bloque_buf(
					tmp_array[array_size-1]->arch,
					&(tmp_array[array_size-1]->size)
				)))
					printf("Error muy loco, si pasa esto algo esta andando MUY mal\n");
			}
		}
	}
	Archivo_flush(out_file);
	Archivo_destruir(out_file);

	for(i=0; i < real_array_size; i++)
		free(tmp_array[i]);
	free(tmp_array);

	if(es_ultimo)
		return 0;

	return merge_externo(out_path, tmp_name, block_size, real_array_size, desde, hasta, cmp);

}

