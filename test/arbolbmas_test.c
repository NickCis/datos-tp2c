#include <stdio.h>
#include "../src/arbolbmas.h"
#define ORDER 64

int main()  {
	TArbolBM* arbol;
	long id, ptr;
	int ret;
	int i;
	int insertar, buscar, eliminar;

	printf("Ingrese la cantidad de inserciones que se quiere hacer, dejar un espacio y la cantidad de busquedas\n");
	scanf("%d %d %d", &insertar, &buscar, &eliminar);

	arbol = Arbol_crear("arbol_data.dat", ORDER);

	for(i = 0; i < insertar; i++){
		printf("Insertar id ptr: ");
		scanf("%ld  %ld", &id, &ptr);
		printf("%ld  %ld\n", id, ptr);

		ret = Arbol_insertar(arbol, id, ptr);
		if (ret != 0)
			printf("Error insertando!\n");
	}


	for(i = 0; i < buscar; i++)
	{
		printf("Buscar id: ");
		scanf("%ld", &id);
		printf("%ld\n", id);

		if(Arbol_get(arbol, id, &ptr))
			printf("Elemento id: %ld no existe\n", id);
		else
			printf("#%ld = %ld\n", id, ptr);
	}

	for(i=0; i < eliminar; i++){
		printf("Eliminar id: ");
		scanf("%ld", &id);
		printf("%ld\n", id);

		ret = Arbol_remover(arbol, id);
		if (ret != 0)
			printf("Error eliminando!\n");
	}


	Arbol_destruir(arbol);
	return 0;
}
