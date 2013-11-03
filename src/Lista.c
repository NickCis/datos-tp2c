#include "lista.h"


//estructura nodo.
typedef struct nodo{

        void* valor;

        struct nodo* prox;

}nodo_t;

//estructura lista enlazada.
struct lista{

        nodo_t* primero;

        nodo_t* ultimo;

        size_t tamanio;

};

//estructura iterador de lista enlazada.
struct lista_iter{

        nodo_t* actual;

        nodo_t* anterior;

};


//Crea un nodo que contiene un dato recibido por parametro. Devuelve el nodo
//o NULL si no se lo pudo crear.
nodo_t* nodo_crear(void* dato){

        nodo_t* n=(nodo_t*)malloc(sizeof(nodo_t));

        if(!n) return NULL;

        n->valor=dato;

        n->prox=NULL;

        return n;

}

//Crea una lista enlazada vací­a. Devuelve la lista o NULL si no la pudo crear.

lista_t* lista_crear(void){

        lista_t* l=(lista_t*)malloc(sizeof(lista_t));

        if(!l) return NULL;

        l->primero=l->ultimo=NULL;
       
        l->tamanio=0;

        return l;

}

//Devuelve verdadero si la lista se encuentra vacía, en caso contrario o no se le pasara
//la lista por parámetro devuelve falso.
bool lista_esta_vacia(const lista_t* l){

        if(!l) return false;

        return (l->tamanio==0)? true:false;

}

//Inserta el elemento recibido por parámetro al principio de la lista. Devuelve verdadero si se pudo realizar la inserción
// caso contrario devolverá falso. Si la lista pasada por parámetro o el elemento no existen, o no
//se pudo crear el nodo para contener al elemento también se devolverá falso.
bool lista_insertar_primero(lista_t* l, void* dato){

        if(!l || !dato) return false;

        nodo_t* n=nodo_crear(dato);

        if(!n) return false;

        if(lista_esta_vacia(l)) l->primero=l->ultimo=n;
       
        else{

                n->prox=l->primero;

                l->primero=n;

        }

        (l->tamanio)++;

        return true;

}

//Inserta el elemento recibido por parámetro al final de la lista. Devuelve verdadero si se pudo insertar
//correctamente o falso en su defecto. Si la lista pasada por parámetro o elelemento no existen, o no
//se pudo crear el nodo para contener al elemento también se devolverá falso.
bool lista_insertar_ultimo(lista_t* l, void* dato){

        if(!l || !dato) return false;

        if(lista_esta_vacia(l)) return lista_insertar_primero(l,dato);

        nodo_t* n=nodo_crear(dato);

        if(!n) return false;

        (l->ultimo)->prox=n;

        l->ultimo=n;

        (l->tamanio)++;

        return true;

}

//Borra el primer elemento de una lista y devuelve su valor. Devuelve NULL si
//no se le pasó la lista por parametro.
void *lista_borrar_primero(lista_t* l){

        if(!l || lista_esta_vacia(l)) return NULL;

        void* valor=(l->primero)->valor;

        nodo_t* aux=(l->primero)->prox;

        free(l->primero);

        l->primero=aux;

        (l->tamanio)--;

        return valor;

}

//Devuelve el valor del primer elemento de la lista. Devuelve NULL si no se le paso
//la lista por parámetro o si la lista no contiene ningún dato.
void *lista_ver_primero(const lista_t* l){

        if(!l || lista_esta_vacia(l)) return NULL;

        return (l->primero)->valor;

}

//Devuelve el tamanio de la lista.
size_t lista_tamanio(const lista_t* l){

        if(!l) return 0;

        return l->tamanio;

}

//Destruye todos los nodos de lista y recibe una función para borrar los datos
//de cada nodo. Si no se le pasa función, no los destruye. Si no se le pasa
//la lista por parámetro no hace nada. Libera el espacio de memoria ocupado
//por la lista.
void lista_destruir(lista_t* l, void destruir_dato(void *)){

        if(!l) return;

        nodo_t* actual=l->primero;

        nodo_t* aux;

        while(actual){

                aux=actual->prox;

                if(destruir_dato) destruir_dato(actual->valor);

                free(actual);

                actual=aux;

        }

        free(l);

}


//Iteración de la lista enlazada recibida por parámetro. Si no se le pasa
//la lista, o no se pudo crear el iterador, devuelve NULL. En caso contrario, devolverá
//el iterador.
lista_iter_t* lista_iter_crear(const lista_t* list){
       
        if(!list) return NULL;

        lista_iter_t* iterador=(lista_iter_t*)malloc(sizeof(lista_iter_t));

        if(!iterador) return NULL;

        iterador->actual=list->primero;

        iterador->anterior=NULL;

        return iterador;

}

//El iterador avanza una posición sobre la lista. Si está al final de la lista, o no se le
//pasa la lista por parámetro, devuelve falso. En caso contrario, devuelve verdadero y se posiciona
//en el siguiente nodo de la lista.
bool lista_iter_avanzar(lista_iter_t* iterador){

        if(!iterador || lista_iter_al_final(iterador) ) return false;

        iterador->anterior=iterador->actual;

        iterador->actual=(iterador->actual)->prox;

        return true;

}

//El iterador avanza una posición sobre la lista. Si está al final de la lista, o no se le
//pasa la lista por parámetro, devuelve falso. En caso contrario, devuelve verdadero y se posiciona
//en el siguiente nodo de la lista.
void* lista_iter_ver_actual(const lista_iter_t* iterador){

        if(!iterador || !iterador->actual) return NULL;

        return (iterador->actual)->valor;

}

//Verifica si el iterador está posicionado sobre el final de la lista. Devuelve falso
//si no lo está o si no se le paso iterador por parámetro y verdadero si lo está.
bool lista_iter_al_final(const lista_iter_t* iterador){

        if( (!iterador) || (iterador->actual) ) return false;

        return true;

}

//Destruye el iterador, si es que se lo pasó por parámetro.
void lista_iter_destruir(lista_iter_t* iterador){

        if(!iterador) return;

        free(iterador);

}

//Inserta un elemento en la posición actual del iterador. Si no se le pasa dato o iterador o
//lista por parametró o no se pudo crear el nodo para contener el dato, devuelve false. En caso
//contrario, devuelve true e inserta el elemento.
bool lista_insertar(lista_t* list, lista_iter_t* iterador, void *dato){

        if(!dato || !list || !iterador) return false;

        nodo_t* insert=nodo_crear(dato);

        if( !insert ) return false;

        if(lista_esta_vacia(list)){
               
                list->primero=list->ultimo=insert;
               
                iterador->actual=insert;
               
                (list->tamanio)++;
               
        }else if(lista_iter_al_final(iterador)){
               
                (iterador->anterior)->prox=insert;
               
                iterador->actual=list->ultimo=insert;
               
                (list->tamanio)++;
               
        }else if( (iterador->actual)==(list->primero) ){
               
                insert->prox=iterador->actual;
               
                list->primero=insert;
               
                iterador->actual=insert;
               
                (list->tamanio)++;
               
        }else{
               
                (iterador->anterior)->prox=insert;
               
                insert->prox=iterador->actual;
               
                iterador->actual=insert;
               
                (list->tamanio)++;
       
        }              

        return true;

}

//Borra el elemento en la posición actual del iterador. Si no se le pasa iterador o
//lista por parametró o la lista esta vacía, devuelve NULL. En caso
//contrario, borra el elemento de la lista y lo devuelve.
void* lista_borrar(lista_t* list, lista_iter_t* iterador){

        if( !list || !iterador || lista_esta_vacia(list) || lista_iter_al_final(iterador) ) return NULL;
       
        void* valor=(iterador->actual)->valor;
       
        nodo_t* aux=iterador->actual;
       
        if(lista_tamanio(list)==1){
               
                list->ultimo=list->primero=iterador->actual=iterador->anterior=NULL;

                list->tamanio=0;
               
        }else if(iterador->actual==list->primero){
               
                iterador->actual=iterador->actual->prox;
               
                (list->tamanio)--;
               
                list->primero=iterador->actual;
               
        }else{
               
                iterador->actual=(iterador->actual)->prox;
               
                (iterador->anterior)->prox=iterador->actual;
               
                (list->tamanio)--;
               
        }
               
        free(aux);
       
        return valor;
       
}
