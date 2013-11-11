#include <stdio.h>
#include <string.h>
#include "stdin_io.h"
#include "usuarios.h"
#include "servicios.h"
#include "consultas.h"
#include "cotizaciones.h"
#include "categorias.h"
#include <time.h>

void conectarse();
void crear_usuario(char t_u);
void crear_categoria();
void menu_conectado_usuario(TUsuario* user);
void menu_conectado_provedor(TUsuario* user);
void menu_conectado_admin(TUsuario* user);
int modificar_datos(TUsuario* user);
void crear_servicio(TUsuario* user);
void borrar_servicio(TUsuario* user);
void buscar_servicio(TUsuario* user);
void imprimir_servicio(TServicio* serv);
void imprimir_consulta(TConsulta* con);
void menu_pos_busqueda_de_servicio(TUsuario* user, unsigned int idserv);
void nueva_consulta(TUsuario* user, TServicio* serv);
void nueva_cotizacion(TUsuario* user, TServicio* serv);
void list_categorias();
void list_usuarios(char t_u);
void get_time(char* fecha, char*hora);

int borrar_usuario(unsigned int dni);
void contestar_consulta(unsigned int id_c);

int main(int argc, char* argv[]){
	Usuarios_init();
	Servicios_init();
	Consultas_init();
	Cotizaciones_init();
	Categorias_init();
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
				crear_usuario('u');
				break;

			default:
				break;
		}
	}

	Usuarios_end();
	Servicios_end();
	Consultas_end();
	Cotizaciones_end();
	Categorias_end();
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

void crear_usuario(char t_u){
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
	//char t_u = 'u';
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
		printf("5 - Listar sus consultas\n");
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

			case '5':{
				size_t len;
				unsigned int* cons = Consulta_from_dni(Usuario_get_dni(user), &len);
				size_t i;
				for(i=0; i < len; i++){
					TConsulta* con = Consulta_from_id(cons[i]);
					imprimir_consulta(con);
					Consulta_free(con);
				}
				free(cons);
				break;
			}

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
		printf("4 - Por Palabra clave (descripcion servicio)\n");
		printf("5 - Por Palabra clave (descripcion categoria)\n");
		printf("6 - Por Palabra clave (consulta)\n");
		printf("s - Salir\n");
		char opt = read_opt();

		if(opt == 's')
			break;

		switch(opt){
			case '1':{
				list_usuarios('p');
				printf("Ingrese id de provedor:\n");
				unsigned dni_p = get_dni();
				if(!dni_p){
					printf("Dni invalido\n");
					break;
				}

				unsigned int id_p;
				TServicio* serv;
				while( (serv = Servicio_from_dni_prov(dni_p, &id_p)) ){
					imprimir_servicio(serv);
					Servicio_free(serv);
				}

				menu_pos_busqueda_de_servicio(user, 0);
				break;
			}

			case '2':{
				printf("Ingrese id servicio:\n");
				unsigned int id = get_dni();
				TServicio* serv = Servicio_from_id(id);
				imprimir_servicio(serv);
				menu_pos_busqueda_de_servicio(user, Servicio_get_id(serv));
				Servicio_free(serv);
				break;
			}

			case '3':{
				list_categorias();
				printf("Ingrese id de categoria:\n");
				unsigned int id_cat = get_dni();
				size_t len;
				unsigned int* servs = Servicio_from_categoria(id_cat, &len);
				size_t i;
				for(i=0; i < len; i++){
					TServicio* serv = Servicio_from_id(servs[i]);
					imprimir_servicio(serv);
					Servicio_free(serv);
				}
				free(servs);
				menu_pos_busqueda_de_servicio(user, 0);
				break;
			}

			case '4':{
				unsigned int* servs;
				size_t len;
				char t[256];
				size_t i;

				printf("Ingrese palabra:\n");
				read_str(t, 255);

				servs = Servicio_buscar(t, &len);
				if(!servs){
					printf("no se encontro\n");
					break;
				}
				for(i=0; i < len; i++){
					TServicio* serv = Servicio_from_id(servs[i]);
					imprimir_servicio(serv);
					Servicio_free(serv);
				}
				free(servs);
				menu_pos_busqueda_de_servicio(user, 0);
				break;
			}

			case '5':{
				unsigned int* cats;
				size_t len;
				char t[256];
				size_t i;
				TCategoria* cat;

				printf("Ingrese palabra:\n");
				read_str(t, 255);

				cats = Categorias_buscar(t, &len);
				if(!cats){
					printf("No se encontro\n");
					break;
				}
				for(i=0; i < len; i++){
					cat = Categoria_from_id(cats[i]);
					printf("Categoria #%d\n", Categoria_get_id(cat));
					printf("\tNombre: '%s'\n", Categoria_get_nombre(cat));
					printf("\tDesc: '%s'\n", Categoria_get_descripcion(cat));
					Categoria_free(cat);
				}
				free(cats);
				break;
			}

			case '6':{
				unsigned int* cons;
				size_t len;
				char t[256];
				size_t i;

				printf("Ingrese palabra:\n");
				read_str(t, 255);

				cons = Consulta_buscar(t, &len);
				if(!cons){
					printf("no se encontro\n");
					break;
				}
				for(i=0; i < len; i++){
					TConsulta* con = Consulta_from_id(cons[i]);
					printf("Consulta #%d\n", Consulta_get_id(con));
					printf("\tServicio #%d\n", Consulta_get_id_serv(con));
					printf("\tDni: %d\n", Consulta_get_dni(con));
					printf("\tConsulta: '%s'\n", Consulta_get_consulta(con));
					Consulta_free(con);
				}
				free(cons);
				menu_pos_busqueda_de_servicio(user, 0);
				break;
			}

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
	char fecha[9];
	char hora[5];
	printf("Ingrese Consulta:\n");
	read_str(consulta, 300);

	get_time(fecha, hora);

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

	size_t len;
	unsigned int* cons = Consulta_from_serv(Servicio_get_id(serv), &len);
	size_t i;
	if(cons){
		printf(" --- Consultas --- \n");
		for(i=0; i < len; i++){
			TConsulta* con = Consulta_from_id(cons[i]);
			imprimir_consulta(con);
			Consulta_free(con);
		}
		free(cons);
		printf(" ---------- \n");
	}
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
				contestar_consulta(0);
				break;
			}
			case '6':{
				list_categorias();
				printf("Ingrese id de servicio:\n");
				unsigned int id_s = get_dni();
				if(!id_s){
					printf("Id invalid\n");
					break;
				}

				TServicio* serv = Servicio_from_id(id_s);
				if(!serv){
					printf("Servicio inexistente\n");
					break;
				}

				if(Servicio_get_dni_p(serv) != Usuario_get_dni(user)){
					printf("El servicio no es tuyo!\n");
					break;
				}

				Servicio_free(serv);

				printf("Ingrese id de categoria:\n");
				unsigned int id_c = get_dni();
				if(!id_c){
					printf("Id invalid\n");
					break;
				}

				TCategoria* cat = Categoria_from_id(id_c);
				if(!cat){
					printf("Categoria inexistente\n");
					break;
				}

				Categoria_free(cat);

				Servicio_agregar_categoria(id_s, id_c);

				break;
			}

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
	while(1){
		printf("Que desea hacer?\n");
		if(Usuario_get_tipo(user) == 's'){
			printf("1 - Crear administrador\n");
			printf("2 - Dar de baja administrador\n");
		}
		printf("3 - Crear nueva categoria\n");
		printf("4 - Crear nuevas categorias (alta masiva)\n");
		printf("5 - Modificar categoria\n");
		printf("6 - Dar de baja categoria\n");
		printf("7 - Moderar preguntas\n");
		printf("s - Salir\n");
		printf("Seleccione opcion\n");
		char opt = read_opt();
		if(opt == 's')
			break;
		if((opt == '1' || opt == '2') && Usuario_get_tipo(user) != 's')
			opt = 0;
		switch(opt){
			case '1':
				crear_usuario('a');
				break;
			case '2':{
				list_usuarios('a');

				printf("Ingrese Id para dar de baja:\n");
				unsigned int dni = get_dni();
				if(dni == 0 || dni == Usuario_get_dni(user)){
					printf("Dni invalido\n");
					break;
				}
				borrar_usuario(dni);
				break;
			}
			case '3':
				crear_categoria();
				break;
			case '4':{
				printf("TODO:\n");
				break;
			}
			case '5':
				list_categorias();
				printf("TODO:\n");
				break;
			case '6':{
				list_categorias();
				printf("Ingrese id a eliminar:\n");
				unsigned int id_c = get_dni();
				if(!id_c){
					printf("Id invalida\n");
					break;
				}
				TCategoria* cat = Categoria_del(id_c);
				if(cat){
					printf("Categoria borrada correctamente\n");
					Categoria_free(cat);
				}else{
					printf("Error borrando Categoria\n");
				}
				break;
			}
			case '7':{
				printf("Ingrese id de consulta:\n");
				unsigned int id_c = get_dni();
				if(!id_c){
					printf("Id invalido\n");
					return;
				}

				TConsulta* cons = Consulta_from_id(id_c);
				if(!cons){
					printf("Consulta inexistente\n");
					return;
				}

				imprimir_consulta(cons);
				printf("Desea ocultarla?(0 no, 1 si)\n");
				unsigned int o = get_dni();
				char oc = o ? 1 : 0;
				Consulta_set_oculta(cons, oc);
				Consulta_store(cons);
				Consulta_free(cons);

				break;
			}

			default:
				break;
		}
	}
}

void crear_categoria(){
	char nombre[101];
	char descripcion[301];

	printf("Crear nueva categoria\n");
	printf("Ingrese nombre:\n");
	read_str(nombre, 100);

	printf("Ingrese descripcion:\n");
	read_str(descripcion, 100);

	TCategoria* cat = Categoria_new(nombre, descripcion);
	if( ! cat ){
		printf("Error\n");
		return;
	}

	Categoria_free(cat);

	printf("Categoria creada Satisfactoriamente\n");
	return;
}
void list_categorias(){
	TCategoria* cat;
	unsigned int id_p= 0;
	while( (cat = Categoria_all(&id_p)) ){
		printf("Categoria #%d\n", Categoria_get_id(cat));
		printf("\tNombre: '%s'\n", Categoria_get_nombre(cat));
		printf("\tDesc: '%s'\n", Categoria_get_descripcion(cat));
		Categoria_free(cat);
	}
}

void list_usuarios(char t_u){
	size_t len;
	size_t i;
	unsigned int* ids = Usuario_from_t_u(t_u, &len);
	for(i=0; i < len; i++){
		TUsuario* user = Usuario_from_dni(ids[i]);
		if(user){
			printf("Usuario #%d\n", Usuario_get_dni(user));
			printf("\tNombre: '%s'\n", Usuario_get_nombre(user));
			printf("\tApellido: '%s'\n", Usuario_get_apellido(user));
			free(user);
		}
	}
	free(ids);
}

void get_time(char* fecha, char*hora){
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime(fecha, 9, "%Y%m%d", timeinfo);
	strftime(hora, 5, "%H%M", timeinfo);
}

void imprimir_consulta(TConsulta* con){
	if(!Consulta_get_oculta(con))
		return;
	printf("Consulta #%d\n", Consulta_get_id(con));
	printf("\tServicio #%d\n", Consulta_get_id_serv(con));
	printf("\tDni #%d\n", Consulta_get_dni(con));
	printf("\tConsulta: '%s'\n", Consulta_get_consulta(con));
	printf("\tFecha: %s\n", Consulta_get_fecha(con));
	printf("\tHora: %s\n", Consulta_get_hora(con));
	if(Consulta_get_hay_rta(con)){
		printf("\tRta: '%s'\n", Consulta_get_rta(con));
		printf("\tFecha: %s\n", Consulta_get_rta_fecha(con));
		printf("\tHora: %s\n", Consulta_get_rta_hora(con));
	}else{
		printf("\tNo hay rta\n");
	}
}

void contestar_consulta(unsigned int id_c){
	// TODO: validar qe servicio sea del provedor
	if(!id_c){
		printf("Ingrese id de consulta a contestar:\n");
		id_c = get_dni();
	}
	if(!id_c){
		printf("Id invalido\n");
		return;
	}

	TConsulta* cons = Consulta_from_id(id_c);
	if(!cons){
		printf("Consulta inexistente\n");
		return;
	}
	char fecha[9];
	char hora[5];
	char rta[301];

	get_time(fecha, hora);

	printf("Ingrese Rta:\n");
	read_str(rta, 300);
	Consulta_set_hay_rta(cons, 1);
	Consulta_set_rta(cons, rta);
	Consulta_set_rta_fecha(cons, fecha);
	Consulta_set_rta_hora(cons, hora);

	Consulta_store(cons);
	Consulta_free(cons);
}
