#include <stdio.h>
#include <string.h>
#include "stdin_io.h"
#include "usuarios.h"
#include "servicios.h"
#include "consultas.h"
#include "cotizaciones.h"

void conectarse();
void crear_usuario();
void menu_conectado_usuario(TUsuario* user);
void menu_conectado_provedor(TUsuario* user);
void menu_conectado_admin(TUsuario* user);
int modificar_datos(TUsuario* user);
void crear_servicio(TUsuario* user);
void borrar_servicio(TUsuario* user);
void buscar_servicio(TUsuario* user);
void imprimir_servicio(TServicio* serv);
void menu_pos_busqueda_de_servicio(TUsuario* user, unsigned int idserv);
void nueva_consulta(TUsuario* user, TServicio* serv);
void nueva_cotizacion(TUsuario* user, TServicio* serv);

int borrar_usuario(unsigned int dni);

int main(int argc, char* argv[]){
	Usuarios_init();
	Servicios_init();
	Consultas_init();
	Cotizaciones_init();
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
	Servicios_end();
	Consultas_end();
	Cotizaciones_end();
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
		case 's':
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
		printf("4 - Convertirse en provedor\n");
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
				buscar_servicio(user);
				break;

			case '4':
				Usuario_set_tipo(user, 'p');
				Usuario_store(user);
				return;
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

void buscar_servicio(TUsuario* user){
	while(1){
		printf("Buscar servicios\n");
		printf("Que desea hacer?\n");
		printf("1 - Por provedoor\n");
		printf("2 - Por servicio\n");
		printf("3 - Por Categoria\n");
		printf("4 - Por Palabra clave (en descripcion y consulta)\n");
		printf("s - Salir\n");
		char opt = read_opt();

		if(opt == 's')
			break;

		switch(opt){
			case '1':
				printf("TODO:\n");
				break;

			case '2':{
				printf("Ingrese id servicio:\n");
				unsigned int id = get_dni();
				TServicio* serv = Servicio_from_id(id);
				imprimir_servicio(serv);
				menu_pos_busqueda_de_servicio(user, Servicio_get_id(serv));
				Servicio_free(serv);
				break;
			}

			case '3':
				printf("TODO:\n");
				break;

			case '4':
				printf("TODO:\n");
				break;

			default:
				break;
		}
	}
}

void menu_pos_busqueda_de_servicio(TUsuario* user, unsigned int idserv){
	if(!idserv){
		printf("Ingrese id de servicio para realizar una accion:\n");
		idserv = get_dni();
	}
	TServicio* serv = Servicio_from_id(idserv);
	if(!serv){
		printf("Servicio #%d inexistente\n", idserv);
		return;
	}

	printf("Que desea hacer?\n");
	printf("1 - Realizar una consulta\n");
	printf("2 - Realizar un pedido de cotizacion\n");
	printf("s - Salir\n");
	char opt = read_opt();
	switch(opt){
		case '1':
			nueva_consulta(user, serv);
			break;
		case '2':
			nueva_cotizacion(user, serv);
			break;
		default:
			break;
	}
	Servicio_free(serv);
}

void nueva_consulta(TUsuario* user, TServicio* serv){
	char consulta[301];
	char fecha[9] = "06112013";
	char hora[5] = "0529";
	printf("Ingrese Consulta:\n");
	read_str(consulta, 300);

	TConsulta* cons = Consulta_new(
		Servicio_get_id(serv),
		Usuario_get_dni(user),
		consulta,
		fecha,
		hora
	);

	if(!cons){
		printf("Error creando consulta\n");
		return;
	}
	Consulta_free(cons);
	printf("Consulta creada satisfactoriamente\n");
}

void nueva_cotizacion(TUsuario* user, TServicio* serv){
	printf("TODO:\n");
}

void imprimir_servicio(TServicio* serv){
	if(! serv){
		printf("Servicio inexistente\n");
		return;
	}
	printf("* Servicio #%d\n", Servicio_get_id(serv));
	printf("\tdni_p: '%d'\n", Servicio_get_dni_p(serv));
	printf("\tnombre: '%s'\n", Servicio_get_nombre(serv));
	printf("\tdesc: '%s'\n", Servicio_get_descripcion(serv));
	printf("\ttipo: '%c'\n", Servicio_get_tipo(serv));
}

void menu_conectado_provedor(TUsuario* user){
	while(1){
		printf("Que desea hacer?\n");
		printf("1 - Modificar sus datos\n");
		printf("2 - Darse de baja\n");
		printf("3 - Crear Servicio\n");
		printf("4 - Dar de baja Servicio\n");
		printf("5 - Listar mis servicios\n");
		printf("6 - Asociar servicios a categorias\n");
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
				crear_servicio(user);
				break;

			case '4':
				borrar_servicio(user);
				break;

			case '5':{
				unsigned int id_p = 0;
				TServicio* serv;
				while( (serv = Servicio_from_dni_prov(Usuario_get_dni(user), &id_p)) ){
					imprimir_servicio(serv);
					Servicio_free(serv);
				}
				
				break;
			}
			case '6':
				printf("TODO\n");
				break;

			default:
				break;
		}
	}
}

void crear_servicio(TUsuario* user){
	char nombre[101];
	char desc[301];
	char tipo = 'g';
	printf("Crear un nuevo servicio\n");

	printf("Ingrese nombre:\n");
	read_str(nombre, 100);

	printf("Ingrese descripcion:\n");
	read_str(desc, 300);
	do{
		printf("Ingrese tipo (g -> gratuito, p -> precio fijo, s -> subasta)\n");
		tipo = read_opt();
	} while(tipo != 'g' && tipo != 'p' && tipo != 's');
	printf("Se creara usuario:\n");
	printf("\t* Nombre: '%s'\n", nombre);
	printf("\t* Descripcion: '%s'\n", desc);
	printf("\t* Tipo: '%c'\n", tipo);
	printf("Esta seguro?(s/n)\n");
	if(read_opt() != 's'){
		printf("Abortando...\n");
		return;
	}

	TServicio* serv = Servicio_new(
		Usuario_get_dni(user),
		nombre,
		desc,
		tipo
	);
	if(!serv){
		printf("Error creando el servicio\n");
	}

	Servicio_free(serv);
}

void borrar_servicio(TUsuario* user){
	unsigned int id;
	printf("Borrar un nuevo servicio\n");

	printf("Ingrese id:\n");
	id = get_dni();

	printf("Esta seguro?(s/n)\n");
	if(read_opt() != 's'){
		printf("Abortando...\n");
		return;
	}

	TServicio* serv = Servicio_from_id(id);
	if(!serv){
		printf("Servicio inexistente\n");
		return;
	}

	if(Usuario_get_dni(user) != Servicio_get_dni_p(serv)){
		printf("Intentaste borrar un servicio que no te pertenece\n");
		Servicio_free(serv);
		return;
	}
	
	Servicio_free(serv);
	serv = Servicio_del(id);
	if(! serv){
		printf("Error borrando el servicio\n");
		return;
	}

	Servicio_free(serv);
}

void menu_conectado_admin(TUsuario* user){
	printf("TODO:\n");
}
