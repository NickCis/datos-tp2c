#include <stdio.h>
#include "heapsort.h"

//#define LENGTH 15
#define LENGTH 2

int cmp(void* n1, void* n2){
	int num1 = (int) n1,
		num2 = (int) n2;

	printf("num1 %d num2 %d\n", num1, num2);

	if(num1 > num2){
		return -1;
	}else if(num1 < num2){
		return 1;
	}

	return 0;
}

void print_arr(int* arr, size_t len){
	size_t i = 0;
	for(i=0; i < len; i++)
		printf("%d ", *(arr+i));

	putchar('\n');
}

int main() {
	size_t length = LENGTH;
	int arr[LENGTH] = {
		5,
		4,
//		7,
//		0,
//		-5,
//		8,
//		7,
//		512,
//		-321,
//		200,
//		1000,
//		-40,
//		123,
//		320,
//		-999
	};

	print_arr(arr, length);
	heapsort((void**)arr, length, &cmp);
	print_arr(arr, length);

	return 0;
}
