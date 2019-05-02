/* ******************************************************************
 * Corrector: Camila Dvorkin
 * Alumno: Nicolás Alberto Ramirez Scarfiello
 * Padrón: 102090
 * *****************************************************************/
#include "lista.h"
#include <stdlib.h>
#include <stdio.h>
/* Definición del struct lista.
 */
typedef struct nodo {
	void* dato;
	struct nodo* prox;
}nodo_t;

struct lista {
	nodo_t* prim;
	nodo_t* ult;
	size_t largo;
};

struct  lista_iter {
	nodo_t* anterior;
	nodo_t* actual;
	lista_t* lista;
};

nodo_t* crear_nodo(void *valor);
void nodo_destruir(nodo_t *nodo);

/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

lista_t *lista_crear(void) {
	lista_t* lista = malloc(sizeof(lista_t));

	if (lista == NULL) return NULL;
	lista->prim = NULL;
	lista->ult = NULL;
	lista->largo = 0;
	return lista;
}

bool lista_esta_vacia(const lista_t *lista) {
	return (lista->prim == NULL);
}

bool lista_insertar_primero(lista_t *lista, void *dato) {
	nodo_t* nuevo_nodo = crear_nodo(dato);
	if (nuevo_nodo == NULL) return false;
	if(lista_esta_vacia(lista))lista->ult = nuevo_nodo;
	nuevo_nodo->prox = lista->prim;
	lista->prim = nuevo_nodo;
	lista->largo++;
	return  true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato) {
	nodo_t* nuevo_nodo = crear_nodo(dato);
	if (nuevo_nodo == NULL) return false;
	if(lista_esta_vacia(lista)) lista->prim = nuevo_nodo;
	else lista->ult->prox = nuevo_nodo;
	lista->ult = nuevo_nodo;
	lista->largo++;
	return  true;
}

void *lista_borrar_primero(lista_t *lista) {
	nodo_t* aux;
	if(lista_esta_vacia(lista)) return NULL;
	void* dato = lista->prim->dato;
	aux = lista->prim;
	lista->prim = lista->prim->prox;
	nodo_destruir(aux);
	if(lista->prim == NULL) lista->ult = NULL;
	lista->largo--;
	return dato;
}

void *lista_ver_primero(const lista_t *lista) {
	if (lista_esta_vacia(lista)) return NULL;
	return lista->prim->dato;
}

void *lista_ver_ultimo(const lista_t* lista) {
	if (lista_esta_vacia(lista)) return NULL;
	return lista->ult->dato;
}

size_t lista_largo(const lista_t *lista) {
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)) {
	nodo_t* aux;
	while (lista->prim){
		if (destruir_dato) destruir_dato(lista->prim->dato);
		aux = lista->prim;
		lista->prim = lista->prim->prox;
		nodo_destruir(aux);
	}
	free(lista);
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra) {
	nodo_t* actual = lista->prim;
	bool ok = true;
	while(actual){
		ok &= visitar(actual->dato , extra);
		if(!ok) break;
		actual = actual->prox;
	}
}

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
lista_iter_t *lista_iter_crear(lista_t *lista) {
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if (iter == NULL) return NULL;
	iter->anterior = NULL;
	iter->actual = lista->prim;
	iter->lista = lista;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter) {
	if (!iter->actual) return false;
	iter->anterior = iter->actual;
	iter->actual = iter->actual->prox;	
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter) {
	if (!iter->actual) return NULL;
	return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter) {
	return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t *iter) {
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato) {
	nodo_t* nuevo_nodo = crear_nodo(dato);
	if (nuevo_nodo == NULL) return false;
	if(lista_esta_vacia(iter->lista)) iter->lista->prim = nuevo_nodo;
	if(iter->anterior)iter->anterior->prox = nuevo_nodo;
	else iter->lista->prim = nuevo_nodo;
	if(!iter->actual)iter->lista->ult = nuevo_nodo;
	nuevo_nodo->prox = iter->actual;
	iter->actual = nuevo_nodo;
	iter->lista->largo++;
	return true;
}

void *lista_iter_borrar(lista_iter_t *iter) {
	nodo_t* aux;
	if(!iter->actual) return NULL;
	void* dato = iter->actual->dato;
	aux = iter->actual;
	iter->actual = iter->actual->prox;
	if(iter->anterior)iter->anterior->prox = iter->actual;
	else iter->lista->prim = iter->actual;
	if(!iter->actual)iter->lista->ult = iter->anterior;
	nodo_destruir(aux);
	iter->lista->largo--;
	return dato;
}

/* *****************************************************************
                      AUXILIARES DEL NODO
 * *****************************************************************/

nodo_t* crear_nodo(void* valor) { 
	nodo_t* nodo = malloc(sizeof(nodo_t)); 

	if (nodo == NULL) return NULL; 
	nodo->dato = valor; 
	nodo->prox = NULL; 
	return nodo; 
}

void nodo_destruir(nodo_t* nodo) {
	free(nodo);
}
// ...
