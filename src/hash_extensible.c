#include "hash_extensible.h"
#include <string.h>
#include <errno.h>

#include "archivo_bloque.h"

struct THashExtensible{
	size_t block_size;
	char* path;
	char* path_tabla;
	size_t tabla_len;
	char* path_baja;
	TDispersionFunct funct;
	unsigned int* tabla;
	TArchivo* arch_bloques;
	THashId get_id;
	unsigned int* libres;
	size_t libres_len;
};

typedef struct {
	uint8_t* ele;
	size_t size;
} Registro;

/*void printTabla(THashExtensible* this){
	size_t i=0;
	printf("tabla: ");
	for(i=0; i< this->tabla_len; i++){
		printf(" '%d' ", this->tabla[i]);
	}

	printf("\n");
}*/

/** Lee la tabla del archivo.
 */
int _readTabla(THashExtensible* this);
/** Escribe tabla a archivo
 */
int _writeTabla(THashExtensible* this);
/** Duplica tabla
 */
int _duplicarTabla(THashExtensible* this);

/** Hace el reemplazo circular en la tabla de dispersion desde pos, saltando salto y poniendo el valor.
 * @param pos: posicion donde arranca
 * @param salto: tama~no del salto
 * @param valor: valor que escribe en las posiciones
 */
int _reemplazoCircular(THashExtensible* this, unsigned int pos, unsigned int salto, unsigned int valor);
/** Lee archivo de bloques libres
 */
int _readLibres(THashExtensible* this);
/** Escribe archivo de bloques libres
 */
int _writeLibres(THashExtensible* this);
/**Agrega numero de bloque a bloques libres.
 */
int _agregarLibre(THashExtensible* this, unsigned int l);
/** Devuelve verdadero si hay un bloque libre, lo saca de la lsita y lo devuelve en l
 * @param l[out]: numero de bloque libre;
 * @return 0 -> no hay, 1 si hay
 */
int _popLibre(THashExtensible* this, unsigned int *l);

THashExtensible* HashExtensible_crear(char* path, char* path_tabla, char* path_baja, size_t block_size, TDispersionFunct funct, THashId get_id){
	THashExtensible* this;

	if(!path || !path_tabla || !path_baja || !block_size || !funct || !get_id)
		return NULL;

	this = (THashExtensible*) calloc(1, sizeof(THashExtensible));
	this->path = strcpy(calloc(1, strlen(path)+1), path);
	this->path_tabla = strcpy(calloc(1, strlen(path_tabla)+1), path_tabla);
	this->path_baja = strcpy(calloc(1, strlen(path_baja)+1), path_baja);

	this->block_size = block_size;
	this->funct = funct;
	this->get_id = get_id;

	this->arch_bloques = Archivo_crear_adm(this->path, NULL, this->block_size);
	_readTabla(this);
	_readLibres(this);

	return this;
}

int HashExtensible_insertar(THashExtensible* this, uint8_t* ele, size_t ele_size){
	if(!this)
		return 1;

	unsigned int id = this->get_id(ele, ele_size);
	unsigned int pos = this->funct(id, this->tabla_len);
	unsigned int block_n = this->tabla[pos];

	{ // Compruebo que no exista ya un elemento con el id
		size_t tmp_size;
		uint8_t *tmp_buf;
		if((tmp_buf = HashExtensible_get(this, id, &tmp_size))){
			free(tmp_buf);
			return 1;
		}
	}

	if(Archivo_bloque_seek(this->arch_bloques, block_n, SEEK_SET))
		printf("ERROR sekeando..\n");

	if(Archivo_bloque_leer(this->arch_bloques))
		printf("error leyendo\n");

	if(Archivo_bloque_seek(this->arch_bloques, block_n, SEEK_SET))
		printf("ERROR sekeando2..\n");

	if(Archivo_bloque_agregar_buf(this->arch_bloques, ele, ele_size)){ // No entra
		//TODO: controlar errores de todo!

		size_t size;
		unsigned int * tam_disp = (unsigned int*) Archivo_bloque_get_buf(this->arch_bloques, 0, &size);

		//if(!tam_disp)
		//	printf("tam_disp es nulo!\n");

		unsigned int new_tam_disp = *tam_disp * 2;

		// TODO: usar una lista y no esta cabeceada
		Registro regs[256] = {{0}};
		int i=0;
		while( ( regs[i].ele = Archivo_bloque_get_buf(this->arch_bloques, i+1, &(regs[i].size)) ) ){
			i++;
		}

		Archivo_bloque_new(this->arch_bloques); // Pongo un nuevo bloque en memoria

		// Agrego nuevo tama~no de dispersion
		Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) &new_tam_disp, sizeof(unsigned int));
		Archivo_flush(this->arch_bloques); // Fuerzo escritura a disco

		// Agrego nuevo bloque
		unsigned int new_bloque_num;
		if(_popLibre(this, &new_bloque_num)){
			Archivo_bloque_seek(this->arch_bloques, new_bloque_num, SEEK_SET); // Seekeo al bloque libre
		}else{
			Archivo_bloque_seek(this->arch_bloques, 0, SEEK_END); // Seekeo al final
			new_bloque_num = Archivo_get_cur_bloque(this->arch_bloques);
		}

		Archivo_bloque_new(this->arch_bloques); // Pongo un nuevo bloque en memoria

		// Agrego tama~no de dispersion
		Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) &new_tam_disp, sizeof(unsigned int));

		Archivo_flush(this->arch_bloques); // Fuerzo escritura a disco

		if(*tam_disp == this->tabla_len){
			_duplicarTabla(this);
			this->tabla[pos] = new_bloque_num;
		}else{
			/*this->tabla[pos] = new_bloque_num;
			int new_pos;
			for(new_pos = pos; new_pos < this->tabla_len ; new_pos+= new_tam_disp)
				this->tabla[new_pos] = new_bloque_num;

			for(new_pos = pos; new_pos >= 0 ; new_pos-= new_tam_disp)
				this->tabla[new_pos] = new_bloque_num;*/

			_reemplazoCircular(this, pos, new_tam_disp, new_bloque_num);
		}

		// Redisperso
		int j =0;
		for(j=0; j < i; j++){
			HashExtensible_insertar(this, regs[j].ele, regs[j].size);
			free(regs[j].ele);
		}

		free(tam_disp);
		// Se vuelve a intentar insertar
		// TODO: poner condicion para que no entre en loop infinito
		return HashExtensible_insertar(this, ele, ele_size);
	}else{
		Archivo_flush(this->arch_bloques);
	}

	return 0;
}

uint8_t* HashExtensible_get(THashExtensible* this, unsigned int id, size_t *size){
	if(!this)
		return NULL;

	unsigned int pos = this->funct(id, this->tabla_len);
	unsigned int block_n = this->tabla[pos];

	if(Archivo_bloque_seek(this->arch_bloques, block_n, SEEK_SET)){
		printf("ERROR sekeando..\n");
		return NULL;
	}

	if(Archivo_bloque_leer(this->arch_bloques)){
		printf("error leyendo\n");
		return NULL;
	}

	uint8_t* ele;
	// Arranco desde el 1, por que el 0 es la tama~no de dispercion del bloque
	int i = 1;
	while( (ele = Archivo_bloque_get_buf(this->arch_bloques, i++, size)) ){
		if(id == this->get_id(ele, *size))
			return ele;
		free(ele);
	}

	return NULL;
}

uint8_t* HashExtensible_del(THashExtensible* this, unsigned int id, size_t *size){
	if(!this)
		return NULL;

	unsigned int pos = this->funct(id, this->tabla_len);
	unsigned int block_n = this->tabla[pos];

	if(Archivo_bloque_seek(this->arch_bloques, block_n, SEEK_SET)){
		printf("ERROR sekeando..\n");
		return NULL;
	}

	if(Archivo_bloque_leer(this->arch_bloques)){
		printf("error leyendo\n");
		return NULL;
	}

	unsigned int * tam_disp = (unsigned int*) Archivo_bloque_get_buf(this->arch_bloques, 0, size);
	int existe_id = 0;

	// TODO: usar una lista y no esta cabeceada
	Registro regs[256] = {{0}};
	int i=0;
	while( ( regs[i].ele = Archivo_bloque_get_buf(this->arch_bloques, i+1, &(regs[i].size)) ) ){
		if(this->get_id(regs[i].ele, regs[i].size) == id)
			existe_id = 1;
		i++;
	}

	if(! existe_id){
		while(i > 0){
			free(regs[--i].ele);
		}
		*size = 0;
		return NULL;
	}

	// Seekeo al ppcio del bloque (el leer me mueve el fd)
	if(Archivo_bloque_seek(this->arch_bloques, block_n, SEEK_SET)){
		printf("ERROR sekeando..\n");
		return NULL;
	}

	Archivo_bloque_new(this->arch_bloques); //Pongo en memoria bloque nuevo

	// Agrego tama~no de dispersion
	Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) tam_disp, sizeof(unsigned int));

	uint8_t *retval = NULL;
	if(i>1){ // No es el unico elemento en el bloque
		printf("No es unico elemento en bloque\n");
		for(existe_id=0 ; existe_id < i ; existe_id++){
			if(this->get_id(regs[existe_id].ele, regs[existe_id].size) != id){
				Archivo_bloque_agregar_buf(this->arch_bloques, regs[existe_id].ele, regs[existe_id].size);
				free(regs[existe_id].ele);
			}else{
				retval = regs[existe_id].ele;
				*size = regs[existe_id].size;
			}
		}
	}else{
		retval = regs[0].ele;
		*size = regs[0].size;

		int first_offset = pos - (*tam_disp)/2;
		if(first_offset < 0)
			first_offset += this->tabla_len;

		int second_offset = pos + (*tam_disp)/2;
		if(second_offset >= this->tabla_len)
			second_offset -= this->tabla_len;

		if(this->tabla[first_offset] == this->tabla[second_offset]){ // Existe bloque de reemplazo!
			// Seekeo el bloque reemplazo
			if(Archivo_bloque_seek(this->arch_bloques, this->tabla[first_offset], SEEK_SET))
				return NULL;

			if(Archivo_bloque_leer(this->arch_bloques)) // Lo leo
				return NULL;

			if(Archivo_bloque_seek(this->arch_bloques, this->tabla[first_offset], SEEK_SET)) // Dejo el fd al ppcio del bloque
				return NULL;

			unsigned int * tam_disp_rem = (unsigned int*) Archivo_bloque_get_buf(this->arch_bloques, 0, size);

			// Recorro circularmente desde busqueda a saltos de TD de bloque reemplazo, poniendo el reemplazo
			_reemplazoCircular(this, pos, *tam_disp_rem, this->tabla[first_offset]);

			// TODO: usar una lista y no esta cabeceada
			Registro regs[256] = {{0}};
			int i=0;
			while( ( regs[i].ele = Archivo_bloque_get_buf(this->arch_bloques, i+1, &(regs[i].size)) ) ){
				i++;
			}

			Archivo_bloque_new(this->arch_bloques); // Pongo un nuevo bloque en memoria
			*tam_disp_rem = (*tam_disp_rem) /2; // se divide en dos el TD de b reemplazo

			Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) tam_disp_rem, sizeof(unsigned int));
			free(tam_disp_rem);

			int j =0;
			for(j=0; j < i; j++){
				Archivo_bloque_agregar_buf(this->arch_bloques, regs[j].ele, regs[j].size);
				free(regs[j].ele);
			}

			// Si las dos mitades de la tabla son iguales, trunco
			int son_iguales = 1;
			for(j=0; j < this->tabla_len /2; j++){
				if(this->tabla[j] != this->tabla[j+this->tabla_len/2]){
					son_iguales = 0;
					break;
				}
			}
			if(son_iguales){
				this->tabla_len /= 2;
				this->tabla = (unsigned int*) realloc(this->tabla, this->tabla_len * sizeof(unsigned int));
			}

			_agregarLibre(this, block_n);
		}
	}

	Archivo_flush(this->arch_bloques);

	free(tam_disp);
	return retval;
}

int HashExtensible_destruir(THashExtensible* this){
	if(!this)
		return 1;

	_writeTabla(this);
	_writeLibres(this);

	free(this->path);
	free(this->path_tabla);
	free(this->path_baja);
	free(this->libres);
	free(this->tabla);
	Archivo_destruir(this->arch_bloques);

	free(this);

	return 0;
}

int _readTabla(THashExtensible* this){
	FILE* fd_tabla;
	if( (fd_tabla = fopen(this->path_tabla, "rb"))){//Abrio el archivo
		//TODO: controlar errores de lectura
		fread((void*) &(this->tabla_len), 1, sizeof(size_t), fd_tabla);
		this->tabla = (unsigned int*) calloc(this->tabla_len, sizeof(unsigned int));
		fread((void*) this->tabla, sizeof(unsigned int), this->tabla_len, fd_tabla);
		fclose(fd_tabla);
	}else{
		this->tabla_len = 1;
		this->tabla = (unsigned int*) calloc(this->tabla_len, sizeof(unsigned int));

		// Agrego bloque 0
		unsigned int tam_disp = 1;
		//Seekeo al ppcio
		Archivo_bloque_seek(this->arch_bloques, 0, SEEK_SET);
		//Pongo en memoria bloque nuevo
		Archivo_bloque_new(this->arch_bloques);
		//Agrego tam de dispercion al bloque
		Archivo_bloque_agregar_buf(this->arch_bloques, (uint8_t*) &tam_disp, sizeof(unsigned int));
		// Fuerzo escritura a disco
		Archivo_flush(this->arch_bloques);
		// Seeko al ppcio
		Archivo_bloque_seek(this->arch_bloques, 0, SEEK_SET);
	}
	return 0;
}

int _writeTabla(THashExtensible* this){
	FILE* fd_tabla = fopen(this->path_tabla, "w+b");
	if(fd_tabla == NULL) // Error
		printf("Error loco escribiendo archivo de tabla. Errno: %d\n", errno);

	fwrite(&(this->tabla_len), sizeof(size_t), 1, fd_tabla);
	fwrite(this->tabla, sizeof(unsigned int), this->tabla_len, fd_tabla);
	fclose(fd_tabla);
	return 0;
}

int _duplicarTabla(THashExtensible* this){
	this->tabla = realloc(this->tabla, this->tabla_len*2 * sizeof(unsigned int));
	memcpy(this->tabla + this->tabla_len, this->tabla, this->tabla_len * sizeof(unsigned int));
	this->tabla_len *= 2;

	return 0;
}

int _reemplazoCircular(THashExtensible* this, unsigned int pos, unsigned int salto, unsigned int valor){
	this->tabla[pos] = valor;
	int new_pos;
	for(new_pos = pos; new_pos < this->tabla_len ; new_pos+= salto)
		this->tabla[new_pos] = valor;

	for(new_pos = pos; new_pos >= 0 ; new_pos-= salto)
		this->tabla[new_pos] = valor;

	return 0;
}

unsigned int HashDispersionModulo(unsigned int id, unsigned int tam_tabla){
	return id % tam_tabla;
}

int _readLibres(THashExtensible* this){
	FILE* fd_libres;
	if( (fd_libres = fopen(this->path_baja, "rb"))){//Abrio el archivo
		//TODO: controlar errores de lectura
		fread((void*) &(this->libres_len), 1, sizeof(size_t), fd_libres);
		this->libres = (unsigned int*) calloc(this->libres_len, sizeof(unsigned int));
		fread((void*) this->libres, sizeof(unsigned int), this->libres_len, fd_libres);
		fclose(fd_libres);
	}else{
		this->libres_len = 0;
		this->libres = NULL;
	}
	return 0;
}

int _writeLibres(THashExtensible* this){
	FILE* fd_libres = fopen(this->path_baja, "w+b");
	if(fd_libres == NULL) // Error
		printf("Error loco escribiendo archivo de libres. Errno: %d\n", errno);

	fwrite(&(this->libres_len), sizeof(size_t), 1, fd_libres);
	fwrite(this->libres, sizeof(unsigned int), this->libres_len, fd_libres);
	fclose(fd_libres);
	return 0;
}

int _agregarLibre(THashExtensible* this, unsigned int l){
	this->libres_len++;
	this->libres = (unsigned int*) realloc(this->libres, this->libres_len * sizeof(unsigned int));
	this->libres[this->libres_len-1] = l;
	return 0;
}

int _popLibre(THashExtensible* this, unsigned int *l){
	if(! this->libres_len)
		return 0;

	this->libres_len--;
	*l = this->libres[this->libres_len];
	return 1;
}
