#include <stdio.h>
#include "../src/arbolbmas.h"
#define ORDER 64
int main()  {
	TArbolBM* arbol; 
	long id, ptr;
	int  ret;
	int  i;

	int insertar, buscar;

	printf("Ingrese la cantidad de inserciones que se quiere hacer, dejar un espacio y la cantidad de busquedas\n");

	scanf("%d  %d", &insertar, &buscar);

	arbol = Arbol_crear("arbol_data.dat", ORDER);

	for(i = 0; i < insertar; i++){
		printf("Insertar id ptr:");

		scanf("%ld  %ld", &id, &ptr);
		printf("%ld  %ld", id, ptr);
		printf("\n");

		ret = Arbol_insertar(arbol, id, ptr);
		if (ret != 0)  
			printf("Error insertando!\n");
	}  


	for(i = 0; i < buscar; i++)
	{  
		printf("Buscar id::");
		scanf("%ld", &id);
		printf("%ld", id);
		printf("\n");
		ptr = Arbol_get(arbol, id);
		printf("Key value is %ld and Ptr value is %ld\n", id, ptr);
	}


	Arbol_destruir(arbol);

	printf("finish main\n");  


}
