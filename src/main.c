#include <stdio.h>
#include <string.h>
#include "usuarios.h"

void conectarse();
void crear_usuario();
/** Lee dni por stdin, si es un dni invalido devuelve 0;
 */
unsigned int get_dni();

/** Lee str desde stdin hasta max_len. str ya debe tener malloqueado max_len +1!
 */
void read_str(char* str, size_t max_len);

/** Lee char de stdin y saca sobrante
 */
char read_opt();

int main(int argc, char* argv[]){
	Usuarios_init();
	printf(" Paginas Doradas ** \n");
	while(1){
		printf("Que desea hacer?\n");
		printf("1 - Conectarse\n");
		printf("2 - Crear usuario\n");
		printf("s - Salir\n");
		printf("Seleccione opcion\n");
		char opt = read_opt();
		if(opt == 's')
			break;
		switch(opt){
			case '1':
				conectarse();
				break;

			case '2':
				crear_usuario();
				break;

			default:
				break;
		}
	}

	Usuarios_end();
}

unsigned int get_dni(){
	unsigned int dni = 0;
	char c;
	while((c = getchar()) != '\n'){
		switch(c){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				dni = dni * 10 + c - '0';
				break;
			default:
				while( getchar() != '\n'){}
				return 0;
		}
	}
	return dni;
}

void read_str(char *str, size_t max_len){
	char c;
	int i = 0;
	str[max_len] = 0;
	while((c = getchar()) != '\n'){
		if(i >= max_len)
			continue;
		str[i++] = c;
		str[i] = 0;
	}
}

void conectarse(){
	unsigned int dni = 0;
	char pass[33] = {0};
	printf("Ingrese dni:\n");
	if( !(dni = get_dni())){
		printf("Dni invalido\n");
		return;
	}

	printf("Ingrese contrase~na:\n");
	read_str(pass, 32);

	TUsuario* user = Usuario_from_dni(dni);
	if(!user){
		printf("Usuario '%d' inexistente\n", dni);
		return;
	}

	if(strcmp(pass, Usuario_get_password(user))){
		Usuario_free(user);
		printf("Contrase~na incorrecta!\n");
		return;
	}
	Usuario_free(user);
	printf("Login correcto =)\n");
}

void crear_usuario(){
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
	char pass[33];
	char prov[101];
	char t_u = 'u';
	printf("Crear un nuevo usuario\n");
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

	printf("Ingrese contrase~na:\n");
	read_str(pass, 32);

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
	printf("\t* Contrase~na: '%s'\n", pass);
	printf("\t* Provincia: '%s'\n", prov);
	printf("Esta seguro?(s/n)\n");
	if(read_opt() != 's'){
		printf("Abortando...\n");
		return;
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
		return;
	}

	Usuario_free(user);
	printf("Usuario creado Satisfactoriamente\n");
	return;
}

char read_opt(){
	char c = getchar();
	if(c != '\n'){
		while( getchar() != '\n');
	}
	return c;


}
