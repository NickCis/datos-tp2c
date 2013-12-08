#include <stdio.h>
#include <string.h>
#include "stdin_io.h"
#include "usuarios.h"
#include "cifrador_hill.h"

int main(int argc, char* argv[]){
	Usuarios_init();
	unsigned int dni = 0;
	char nombre[101];
	char apellido[101];
	int c_mail = 0;
	char mail1[301];
	char mail2[301];
	char mail3[301];
	char* mails[3] = {
		mail1,
		mail2,
		mail3
	};
	char pass[33] = {0};
	char pass_n[33] = {0};
	char prov[101];
	char t_u = 's';
	printf("Crear administrador del sistema\n");
	printf("Ingrese dni:\n");
	while( (dni = get_dni()) == 0){
		printf("Dni invalido\n");
		printf("Ingrese dni:\n");
	}

	printf("Ingrese nombre:\n");
	read_str(nombre, 100);

	printf("Ingrese apellido:\n");
	read_str(apellido, 100);

	printf("Ingrese cantidad de mails:\n");
	c_mail = (int) get_dni();
	int i = 0;
	for(i=0; i < c_mail ; i++){
		printf("Ingrese mail n %d:\n", i+1);
		read_str(mails[i], 300);
	}

	do {
		printf("Ingrese contrase~na: (9 caracteres, mayusculas, numeros, _,.?-)\n");
		read_str(pass, 32);
	}while(validar_clave(pass));

	printf("Ingrese provincia:\n");
	read_str(prov, 100);

	printf("Se creara usuario:\n");
	printf("\t* Dni: '%d'\n", dni);
	printf("\t* Nombre: '%s'\n", nombre);
	printf("\t* Apellido: '%s'\n", apellido);
	printf("\t* Cantidad de mails: '%d'\n", c_mail);
	for (i=0; i < c_mail ; i++){
		printf("\t* Mail %d: '%s'\n", i+1, mails[i]);
	}
	normalizar(pass, pass_n);
	printf("\t* Contrase~na: '%s'\n", pass_n);
	printf("\t* Provincia: '%s'\n", prov);
	printf("Esta seguro?(s/n)\n");
	if(read_opt() != 's'){
		printf("Abortando...\n");
		return 1;
	}

	TUsuario* user = Usuario_new(
		dni,
		nombre,
		apellido,
		c_mail,
		mails,
		pass,
		prov,
		t_u
	);

	if(! user ){
		printf("Error\n");
		return 1;
	}

	Usuario_free(user);
	printf("Usuario creado Satisfactoriamente\n");
	Usuarios_end();
	return 0;
}
