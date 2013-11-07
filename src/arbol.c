// http://forums.codeguru.com/showthread.php?453150-B-Tree-C-Implementation
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "arbolbmas.h"

#define ORDER 64
#define BLKSIZE (ORDER*8)
#define BKTPTRS (ORDER*2 - 1)
#define SEEK_SET  0

struct TArbolBM {
	int File;
	char *path;
	//int DupKeys;
	long RootBlk;
	long AllocBlk;
	long CurBlk;
	short CurKey;
	short CurPtr;
};

typedef struct {
	short LeafNode;
	short KeyCount;
	long Keys[ORDER - 1];
	long Ptrs[ORDER];
} TNodo;

typedef struct {
	long RootBlock;
	long AllocBlock;
	char Junk[BLKSIZE - 2*sizeof(long)]; // Para que ocupe justo un bloque
} HeadType;

static void ReadBlock(int File, long Block, int Size, void *Addr) {
	if(lseek(File, Block * Size, SEEK_SET) == -1 ||
			read(File, Addr, Size) == -1)
		printf("ReadBlock Error, File %d, Block %d, Size %d\n",
			File, Block, Size);
}

static void WriteBlock(int File, long Block, int Size, void *Addr) {
	if(lseek(File, Block * Size, SEEK_SET) == -1 ||
			write(File, Addr, Size) == -1)
		printf("WriteBlock Error, File %d, Block %d, Size %d\n",
			File, Block, Size);
}

static void ReadHead(TArbolBM *arbol) {
	HeadType HeadBlock;
	ReadBlock(arbol->File, 0, BLKSIZE, &HeadBlock);
	arbol->RootBlk = HeadBlock.RootBlock;
	arbol->AllocBlk = HeadBlock.AllocBlock;
}

static void WriteHead(TArbolBM *arbol) {
	HeadType HeadBlock;
	HeadBlock.RootBlock = arbol->RootBlk;
	HeadBlock.AllocBlock = arbol->AllocBlk;
	WriteBlock(arbol->File, 0, BLKSIZE, &HeadBlock);
}

TArbolBM* Arbol_crear(char *path) {
	int j;
	TArbolBM * this = (TArbolBM*) calloc(1, sizeof(TArbolBM));

	this->path = strcpy(malloc(strlen(path)+1), path);
	this->CurBlk = -1;

	if((this->File = open(path, O_RDWR)) >= 0) {
		ReadHead(this);
	} else
		if((this->File = open(path, O_RDWR|O_CREAT, 0644)) >= 0) { 
			TNodo Node;
			this->RootBlk = 1;
			this->AllocBlk = 2;
			WriteHead(this);
			Node.LeafNode = 1;
			Node.KeyCount = 0;
			for(j = 0; j < ORDER-1; j++)
			{ 
				Node.Ptrs[j] = -1; 
				Node.Keys[j] = 0; 
			}  
			Node.Ptrs[ORDER - 1] = -1;
			WriteBlock(this->File, 1, BLKSIZE, &Node);
		} else {
			perror("OpenIndex");
			free(this);
			this = NULL;
		}
	return this;
}

void Arbol_destruir(TArbolBM* this){
	WriteHead(this);
	close(this->File);
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

static void CheckBucket(TArbolBM* arbol, TNodo *Node, long *Key, long *Ptr) {
	*Ptr = -1;
	if(arbol->CurBlk >= 0) {
		long NextPtr;
		*Key = Node->Keys[arbol->CurKey-1];
		*Ptr = Node->Ptrs[arbol->CurKey];

		if( NextPtr < 0) {
			arbol->CurPtr = 0;
			arbol->CurKey++;
			if(arbol->CurKey > Node->KeyCount) {
				arbol->CurBlk = Node->Ptrs[0];
				arbol->CurKey = 1;
			}
		}
	}
}

long Arbol_get(TArbolBM* this, long id) {
	TNodo Node;
	long Ptr;
	this->CurBlk = this->RootBlk; // Seteo bloque correinte como raiz
	for(;;) {
		ReadBlock(this->File, this->CurBlk, BLKSIZE, &Node);
		this->CurKey = FindKey(&Node, id);

		if(Node.LeafNode)
			break;

		this->CurBlk = Node.Ptrs[this->CurKey];
	}
	this->CurPtr = 0;

	if(this->CurKey == 0)
		this->CurBlk = -1;

	CheckBucket(this, &Node, &id, &Ptr);

	return Ptr;
}

static int InsertKey(TArbolBM* arbol, TNodo *Node, int KIdx, long *Key, long *Ptr) {
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
		//*Ptr = Alloc(Index)++;
		*Ptr = arbol->AllocBlk++;
		if(Node->LeafNode) {  /* insert in sequential linked list */
			NNode.Ptrs[0] = Node->Ptrs[0];
			Node->Ptrs[0] = *Ptr;
		}
		//WriteBlock(File(Index), *Ptr, BLKSIZE, &NNode);
		WriteBlock(arbol->File, *Ptr, BLKSIZE, &NNode);
		//WriteHead(Index);
		WriteHead(arbol);
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

	ReadBlock(this->File, Block, BLKSIZE, &nodo);
	KIdx = FindKey(&nodo, *Key);
	EqualKey = KIdx && nodo.Keys[KIdx-1] == *Key;

	if(!nodo.LeafNode)
		Split = RecInsert(this, nodo.Ptrs[KIdx], Key, Ptr, error);

	if(Split || nodo.LeafNode && !EqualKey) {
		Split = InsertKey(this, &nodo, KIdx, Key, Ptr);
		WriteBlock(this->File, Block, BLKSIZE, &nodo);
	} else if(nodo.LeafNode) {
		*error = -1;
	}
	return Split;
}

int Arbol_insertar(TArbolBM* this, long Key, long Ptr){
	int Split;
	int error = 0;

	Split = RecInsert(this, this->RootBlk, &Key, &Ptr, &error);

	if(Split) {
		TNodo Node;
		Node.LeafNode = 0;
		Node.KeyCount = 1;
		Node.Keys[0] = Key;
		Node.Ptrs[1] = Ptr;
		Node.Ptrs[0] = this->RootBlk;
		this->RootBlk = this->AllocBlk++;
		WriteBlock(this->File, this->RootBlk, BLKSIZE, &Node);
		WriteHead(this);
	}
	this->CurBlk = -1;
	return error;
}

int main()  {

	printf("BLKSIZE: '%d' sizeof(TNodo): '%d'\n", BLKSIZE, sizeof(TNodo));

	/********************************************************************/
	/* The main section of code should be replaced.                     */
	/* It just illustrates how keys and addresses can be inserted       */
	/* into a B+ tree and then retrieved                                */
	/********************************************************************/

	char Xname[8] = "arb.dat";
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

	XEntryNum = Arbol_crear(Xname);

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
