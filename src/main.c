#include <stdio.h>
#include <string.h>
#include "stdin_io.h"
#include "usuarios.h"

void conectarse();
void crear_usuario();
void menu_conectado_usuario(TUsuario* user);
void menu_conectado_provedor(TUsuario* user);
void menu_conectado_admin(TUsuario* user);
int modificar_datos(TUsuario* user);

int borrar_usuario(unsigned int dni);

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
	printf("Conectado satisfactoriamente. Bienvenido '%s'\n", Usuario_get_nombre(user));

	switch(Usuario_get_tipo(user)){
		case 'u':
			menu_conectado_usuario(user);
			break;
		case 'p':
			menu_conectado_provedor(user);
			break;
		case 'a':
			menu_conectado_admin(user);
			break;
		default:
			break;
	}
	Usuario_free(user);
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

void menu_conectado_usuario(TUsuario* user){
	while(1){
		printf("Que desea hacer?\n");
		printf("1 - Modificar sus datos\n");
		printf("2 - Darse de baja\n");
		printf("3 - Buscar Servicio\n");
		printf("s - Salir\n");
		printf("Seleccione opcion\n");
		char opt = read_opt();
		if(opt == 's')
			break;
		switch(opt){
			case '1':
				modificar_datos(user);
				break;

			case '2':
				if(! borrar_usuario(Usuario_get_dni(user)))
					return;
				break;

			case '3':
				printf("TODO\n");
				break;

			default:
				break;
		}
	}
}

int borrar_usuario(unsigned int dni){
	printf("Se borrara su usuario, esta seguro?(s/n)\n");
	char c = read_opt();
	if(c != 's')
		return 1;
	TUsuario* user = Usuario_del(dni);
	free(user);
	return 0;
}

int modificar_datos(TUsuario* user){
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

	printf("Ingrese nombre:\n");
	read_str(nombre, 100);
	Usuario_set_nombre(user, nombre);

	printf("Ingrese apellido:\n");
	read_str(apellido, 100);
	Usuario_set_apellido(user, apellido);

	printf("Ingrese cantidad de mails:\n");
	c_mail = (int) get_dni();
	Usuario_set_mail_c(user, c_mail);
	int i = 0;
	for(i=0; i < c_mail ; i++){
		printf("Ingrese mail n %d:\n", i+1);
		read_str(mails[i], 300);
		Usuario_set_mail(user, i, mails[i]);
	}

	printf("Ingrese contrase~na:\n");
	read_str(pass, 32);
	Usuario_set_password(user, pass);

	printf("Ingrese provincia:\n");
	read_str(prov, 100);
	Usuario_set_provincia(user, prov);

	//printf("Se creara usuario:\n");
	//printf("\t* Dni: '%d'\n", dni);
	//printf("\t* Nombre: '%s'\n", nombre);
	//printf("\t* Apellido: '%s'\n", apellido);
	//printf("\t* Cantidad de mails: '%d'\n", c_mail);
	//for (i=0; i < c_mail ; i++){
	//	printf("\t* Mail %d: '%s'\n", i+1, mails[i]);
	//}
	//printf("\t* Contrase~na: '%s'\n", pass);
	//printf("\t* Provincia: '%s'\n", prov);
	//printf("Esta seguro?(s/n)\n");
	//if(read_opt() != 's'){
	//	printf("Abortando...\n");
	//	return;
	//}

	Usuario_store(user);
	return 0;
}

void menu_conectado_provedor(TUsuario* user){
	printf("TODO:\n");
}
void menu_conectado_admin(TUsuario* user){
	printf("TODO:\n");
}
