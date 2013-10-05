#include "heapsort.h"

void heapify(void** array, size_t size, TCompareFunction cmp, int dsd);

void heapsort(void** array, size_t size, TCompareFunction cmp){
	int i=0;

	for(i=0; i<size; i++){
		heapify(array, size, cmp, i);
	}
}

void heapify(void** array, size_t size, TCompareFunction cmp, int dsd){
	int i=0;

	for(i=dsd; i < size; i++){
		int parent = ( ( i- dsd -1)/2) + dsd;
		int j = i;

		while(parent >= dsd && cmp(*(array+parent), *(array+j)) < 0){
			void* aux = *(array+parent);
			*(array+parent) = *(array+j);
			*(array+j) = aux;

			j = parent;
			parent = (( parent - dsd -1)/2) + dsd;
		}
	}

}
