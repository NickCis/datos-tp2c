// http://forums.codeguru.com/showthread.php?453150-B-Tree-C-Implementation
// gcc arbol.c archivo_bloque.c bloque.c -o arbol
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "arbolbmas.h"
#include "archivo_bloque.h"

#define ORDER 64
#define BLKSIZE (ORDER*8)
#define BKTPTRS (ORDER*2 - 1)
#define SEEK_SET  0

struct TArbolBM {
	//int File;
	TArchivo* arch;
	char *path;
	long root_bloque;
	long alloc_bloque;
	long corriente_bloque;
	short corriente_llave;
	short corriente_ptr;
	TArbolId get_id;
	size_t block_size;
};

typedef struct {
	short LeafNode;
	short KeyCount;
	long Keys[ORDER - 1];
	long Ptrs[ORDER];
} TNodo;

/*static void ReadBlock(int File, long Block, int Size, void *Addr) {
	if(lseek(File, Block * Size, SEEK_SET) == -1 ||
			read(File, Addr, Size) == -1)
		printf("ReadBlock Error, File %d, Block %d, Size %d\n",
			File, Block, Size);
}*/

static void _leerNodo(TArbolBM * this, size_t bloque, void* data, size_t * size) {
	// TODO: checkear errores
	size_t size_read = 0;
	Archivo_bloque_seek(this->arch, bloque, SEEK_SET);
	Archivo_bloque_leer(this->arch);
	uint8_t *aux = Archivo_get_bloque_buf(this->arch, &size_read);
	memcpy(data, aux, size_read);
	*size = size_read;
	free(aux);
}

/*static void WriteBlock(int File, long Block, int Size, void *Addr) {
	if(lseek(File, Block * Size, SEEK_SET) == -1 ||
			write(File, Addr, Size) == -1)
		printf("WriteBlock Error, File %d, Block %d, Size %d\n",
			File, Block, Size);
}*/

static void _escribirNodo(TArbolBM * this, size_t bloque, void* data, size_t size) {
	// TODO: checkear errores
	Archivo_bloque_seek(this->arch, bloque, SEEK_SET);
	Archivo_bloque_new(this->arch);
	Archivo_bloque_agregar_buf(this->arch, data, size);
	Archivo_flush(this->arch);

}

static void _leerCabecera(TArbolBM *this) {
	// TODO: checkear errores
	size_t size;

	Archivo_bloque_seek(this->arch, 0, SEEK_SET);
	Archivo_bloque_leer(this->arch);

	long *aux = Archivo_get_bloque_buf(this->arch, &size);
	this->root_bloque = *aux;
	free(aux);

	aux = Archivo_get_bloque_buf(this->arch, &size);
	this->alloc_bloque = *aux;
	free(aux);
}

static void _escribirCabecera(TArbolBM *this) {
	// TODO: checkear errores
	Archivo_bloque_seek(this->arch, 0, SEEK_SET);
	Archivo_bloque_new(this->arch);
	Archivo_bloque_agregar_buf(this->arch, &(this->root_bloque), sizeof(long));
	Archivo_bloque_agregar_buf(this->arch, &(this->alloc_bloque), sizeof(long));
	Archivo_flush(this->arch);
}

TArbolBM* Arbol_crear(char *path, size_t block_size, TArbolId get_id){
	int j;
	TArbolBM * this = (TArbolBM*) calloc(1, sizeof(TArbolBM));

	this->path = strcpy(malloc(strlen(path)+1), path);
	this->corriente_bloque = -1;
	this->get_id = get_id;
	this->block_size = block_size;

	this->arch = Archivo_crear(path, block_size);

	//if((this->File = open(path, O_RDWR)) >= 0) {
	if(Archivo_cant_bloque(this->arch)){
		_leerCabecera(this);
	} else{
		TNodo Node;
		this->root_bloque = 1;
		this->alloc_bloque = 2;
		_escribirCabecera(this);
		Node.LeafNode = 1;
		Node.KeyCount = 0;
		for(j = 0; j < ORDER-1; j++)
		{ 
			Node.Ptrs[j] = -1; 
			Node.Keys[j] = 0; 
		}  
		Node.Ptrs[ORDER - 1] = -1;
		//WriteBlock(this->File, 1, BLKSIZE, &Node);
		_escribirNodo(this, 1, &Node, sizeof(TNodo));

	}
	return this;
}

void Arbol_destruir(TArbolBM* this){
	// TODO: cerrar arch
	_escribirCabecera(this);
	Archivo_destruir(this->arch);
	free(this->path);
	free(this);
}

/** Devuelve indice de id (en array).
 */
static int FindKey(TNodo *nodo, long id) {
	int k;
	for(k = 0; k < nodo->KeyCount; k++)
		if(nodo->Keys[k] > id)
			break;

	return k;
}

static void CheckBucket(TArbolBM* this, TNodo *Node, long *Key, long *Ptr) {
	*Ptr = -1;
	if(this->corriente_bloque >= 0) {
		long NextPtr;
		*Key = Node->Keys[this->corriente_llave-1];
		*Ptr = Node->Ptrs[this->corriente_llave];

		if( NextPtr < 0) {
			this->corriente_ptr = 0;
			this->corriente_llave++;
			if(this->corriente_llave > Node->KeyCount) {
				this->corriente_bloque = Node->Ptrs[0];
				this->corriente_llave = 1;
			}
		}
	}
}

long Arbol_get(TArbolBM* this, long id) {
	TNodo Node;
	long Ptr;
	this->corriente_bloque = this->root_bloque; // Seteo bloque correinte como raiz
	for(;;) {
		size_t size = sizeof(TNodo);
		_leerNodo(this, this->corriente_bloque, &Node, &size);
		this->corriente_llave = FindKey(&Node, id);

		if(Node.LeafNode)
			break;

		this->corriente_bloque = Node.Ptrs[this->corriente_llave];
	}
	this->corriente_ptr = 0;

	if(this->corriente_llave == 0)
		this->corriente_bloque = -1;

	CheckBucket(this, &Node, &id, &Ptr);

	return Ptr;
}

static int InsertKey(TArbolBM* this, TNodo *Node, int KIdx, long *Key, long *Ptr) {
	long Keys[ORDER], Ptrs[ORDER+1];
	int Count, Count1, Count2, k;
	Count = Node->KeyCount + 1;
	Count1 = Count < ORDER ? Count : ORDER/2;
	Count2 = Count - Count1;
	for(k = ORDER/2; k < KIdx; k++) {
		Keys[k] = Node->Keys[k];
		Ptrs[k+1] = Node->Ptrs[k+1];
	}
	Keys[KIdx] = *Key;
	Ptrs[KIdx+1] = *Ptr;
	for(k = KIdx; k < Node->KeyCount; k++) {
		Keys[k+1] = Node->Keys[k];
		Ptrs[k+2] = Node->Ptrs[k+1];
	}
	for(k = KIdx; k < Count1; k++) {
		Node->Keys[k] = Keys[k];
		Node->Ptrs[k+1] = Ptrs[k+1];
	}
	Node->KeyCount = Count1;
	if(Count2) {
		int s, d;
		TNodo NNode;
		NNode.LeafNode = Node->LeafNode;
		Count2 -= !Node->LeafNode;
		for(s = ORDER/2 + !Node->LeafNode, d = 0; d < Count2; s++, d++) {
			NNode.Keys[d] = Keys[s];
			NNode.Ptrs[d] = Ptrs[s];
		}
		NNode.Ptrs[d] = Ptrs[s];
		NNode.KeyCount = Count2;
		*Key = Keys[ORDER/2];
		*Ptr = this->alloc_bloque++;
		if(Node->LeafNode) {  /* insert in sequential linked list */
			NNode.Ptrs[0] = Node->Ptrs[0];
			Node->Ptrs[0] = *Ptr;
		}
		//WriteBlock(arbol->File, *Ptr, BLKSIZE, &NNode);
		_escribirNodo(this, *Ptr, &NNode, sizeof(TNodo));
		//WriteHead(Index);
		_escribirCabecera(this);
	}
	return Count2;
}

/** 
 *
 * @param this[in]: instancia de arbol
 * @param Block[in]: numero de bloque
 * @param *Key
 */
static int RecInsert(TArbolBM* this, long Block, long *Key, long *Ptr, int *error) {
	TNodo nodo;
	int KIdx, Split = 0;
	int EqualKey;
	size_t size = sizeof(TNodo);

	//ReadBlock(this->File, Block, BLKSIZE, &nodo);
	_leerNodo(this, Block, &nodo, &size);
	KIdx = FindKey(&nodo, *Key);
	EqualKey = KIdx && nodo.Keys[KIdx-1] == *Key;

	if(!nodo.LeafNode)
		Split = RecInsert(this, nodo.Ptrs[KIdx], Key, Ptr, error);

	if(Split || nodo.LeafNode && !EqualKey) {
		Split = InsertKey(this, &nodo, KIdx, Key, Ptr);
		//WriteBlock(this->File, Block, BLKSIZE, &nodo);
		_escribirNodo(this, Block, &nodo, BLKSIZE);
	} else if(nodo.LeafNode) {
		*error = -1;
	}
	return Split;
}

int Arbol_insertar(TArbolBM* this, long Key, long Ptr){
	int Split;
	int error = 0;

	Split = RecInsert(this, this->root_bloque, &Key, &Ptr, &error);

	if(Split) {
		TNodo Node;
		Node.LeafNode = 0;
		Node.KeyCount = 1;
		Node.Keys[0] = Key;
		Node.Ptrs[1] = Ptr;
		Node.Ptrs[0] = this->root_bloque;
		this->root_bloque = this->alloc_bloque++;
		_escribirNodo(this, this->root_bloque, &Node, sizeof(TNodo));
		_escribirCabecera(this);
	}
	this->corriente_bloque = -1;
	return error;
}

int main()  {

	printf("BLKSIZE: '%d' sizeof(TNodo): '%d'\n", BLKSIZE, sizeof(TNodo));

	/********************************************************************/
	/* The main section of code should be replaced.                     */
	/* It just illustrates how keys and addresses can be inserted       */
	/* into a B+ tree and then retrieved                                */
	/********************************************************************/

	//char Xname[8] = "arb.dat";
	TArbolBM* XEntryNum; 
	long XKey, XPtr;
	int  RetnCode;
	int  i,j;

	int NumInsert, NumLookup;

	printf("start main\n");


	/********************************************************************/
	/* read number of keys to insert, the number of keys  to search for */
	/* and whether duplicates are allowed in the index                  */
	/********************************************************************/

	scanf("%d  %d",&NumInsert, &NumLookup);

	XEntryNum = Arbol_crear("arbol_data.dat", 1024, NULL);

	for(i = 0; i < NumInsert; i++){
		printf("Insert KEY, PTR:");
		/* read key and address  to insert into the B+ tree  */

		scanf("%d  %d",&XKey, &XPtr);
		printf("%d  %d",XKey, XPtr);
		printf("\n");

		RetnCode = Arbol_insertar(XEntryNum, XKey, XPtr);
		if (RetnCode != 0)  
			printf("Error During Key Insertion\n");
	}  


	for(i = 0; i < NumLookup; i++)
	{  

		/* read key to lookup in the B+ tree  */
		printf("Search KEY:");
		scanf("%d",&XKey);
		printf("%d",XKey);
		printf("\n");
		XPtr = Arbol_get(XEntryNum, XKey);
		printf("Key value is %d and Ptr value is %d\n", XKey, XPtr);
	}


	Arbol_destruir(XEntryNum);

	printf("finish main\n");  


}
