#include "abb.h"
#include "pila.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


typedef struct abb_nodo {
	char* clave;
	void* dato;
	struct abb_nodo* izq;
	struct abb_nodo* der;
}abb_nodo_t;

struct abb {
	abb_nodo_t* raiz;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
	size_t cantidad;
};

struct abb_iter {
	const abb_t* abb;
	pila_t* pila;
};



// PRIMITIVAS DEL ABB_NODO //


abb_nodo_t* abb_nodo_crear(const char* clave,void* dato) {
	abb_nodo_t* abb_nodo = malloc(sizeof(abb_nodo_t));
	if (!abb_nodo) return NULL;
	char* copia_clave = malloc(sizeof(char)*strlen(clave)+1);
	if (!copia_clave) {
		free(abb_nodo);
		return NULL;
	}
	strcpy(copia_clave,clave);
	abb_nodo -> clave = copia_clave;
	abb_nodo -> dato = dato;
	abb_nodo -> izq = NULL;
	abb_nodo -> der = NULL;
	return abb_nodo;
}

void abb_nodo_destruir(abb_nodo_t* abb_nodo,abb_destruir_dato_t destruir_dato) {
	if (!abb_nodo) return;
	free(abb_nodo->clave);
	if (destruir_dato) destruir_dato(abb_nodo->dato);
	free(abb_nodo);
}


// AUXILIARES DE PRIMITIVAS DEL ABB //


size_t tipo_de_nodo(abb_nodo_t* nodo) {
	/* Devuelve el tipo de nodo actual:
	   0 - Hoja
	   1 - Padre parcial (sólo hijo derecho)
	   2 - Padre parcial (sólo hijo izquierdo)
	   3 - Padre completo
	*/
	if (!nodo || (!nodo->izq && !nodo->der)) return 0;
	if (!nodo->izq) return 1;
	if (!nodo->der) return 2;
	return 3;
}

abb_nodo_t* _buscar_padre_nodo(abb_nodo_t* nodo,abb_nodo_t* padre,const abb_t* arbol, const char* clave,bool busqueda_exacta){
	/* 
	Devuelve el padre del nodo que corresponda según la función que utilice esta función
	Recibe un bool busqueda: true en las funciones ABB_BORRAR/PERTENECE/OBTENER, false en la función ABB_GUARDAR
	*/
	if (!nodo) {
		if (busqueda_exacta) return NULL;
		return padre;
	}
	if (arbol->cmp(nodo->clave,clave) == 0) return padre;
	else if (!busqueda_exacta && arbol->cmp(nodo->clave,clave) > 0 && !nodo->izq) return nodo;
	else if (!busqueda_exacta && arbol->cmp(nodo->clave,clave) < 0 && !nodo->der) return nodo;
	else if (busqueda_exacta && nodo->izq && arbol->cmp(nodo->izq->clave,clave) == 0) return nodo;
	else if (busqueda_exacta && nodo->der && arbol->cmp(nodo->der->clave,clave) == 0) return nodo;
	if (arbol->cmp(nodo->clave,clave) < 0) return _buscar_padre_nodo(nodo->der,nodo, arbol, clave,busqueda_exacta);
	return _buscar_padre_nodo(nodo->izq,nodo, arbol, clave,busqueda_exacta);
}

bool abb_guardar_raiz(abb_t* arbol,const char* clave,void* dato) {
	// Sólo se ejecuta si el Árbol no tiene raíz. Crea y agrega la raíz del Árbol.
	abb_nodo_t* abb_nodo = abb_nodo_crear(clave,dato);
	if (!abb_nodo) return false;
	arbol->raiz = abb_nodo;
	arbol->cantidad++;
	return true;
}

abb_nodo_t* conseguir_hijo(abb_nodo_t* padre,const char* clave,abb_comparar_clave_t cmp) {
	if (padre->izq && cmp(padre->izq->clave,clave)==0) return padre->izq;
	return padre->der;
}

bool _abb_guardar_repetido(abb_t* arbol,void* dato,const char* clave) {
	abb_nodo_t* padre = _buscar_padre_nodo(arbol->raiz,NULL,arbol,clave,false);
	abb_nodo_t* hijo;
	if (!padre) hijo = arbol->raiz;
	else hijo = conseguir_hijo(padre,clave,arbol->cmp);
	if (arbol->destruir_dato) arbol->destruir_dato(hijo->dato);
	hijo->dato = dato;
	return true;

}

bool _abb_guardar_nuevo(abb_t* arbol,void* dato,const char* clave) {
	abb_nodo_t* nuevo_nodo = abb_nodo_crear(clave,dato);
	if (!nuevo_nodo) return false;
	abb_nodo_t* padre = _buscar_padre_nodo(arbol->raiz,NULL,arbol,clave,false);
	if (arbol->cmp(padre->clave,clave) > 0) padre->izq = nuevo_nodo;
	else padre->der = nuevo_nodo; 
	arbol->cantidad++;
	return true;
}

bool _abb_guardar(abb_t* arbol,abb_nodo_t* actual,const char* clave,void* dato) {
	if (abb_pertenece(arbol,clave)) return _abb_guardar_repetido(arbol,dato,clave);
	return _abb_guardar_nuevo(arbol,dato,clave);
}


void _abb_destruir(abb_nodo_t* abb_nodo,abb_destruir_dato_t destruir_dato) {
	if (!abb_nodo) return;
	if (!abb_nodo->izq && !abb_nodo->der) {
		abb_nodo_destruir(abb_nodo,destruir_dato);
		return;
	}
	_abb_destruir(abb_nodo->izq,destruir_dato);
	_abb_destruir(abb_nodo->der,destruir_dato);
	abb_nodo_destruir(abb_nodo,destruir_dato);
	return;
}

void swappear_nodos(abb_nodo_t* actual, abb_nodo_t* hijo) {
	// Recibe dos nodos e intercambia sus claves y valores
	char* aux_clave = actual->clave;
	void* aux_dato = actual->dato;
	actual->clave = hijo->clave;
	actual->dato = hijo->dato;
	hijo->clave = aux_clave;
	hijo->dato = aux_dato;
}

abb_nodo_t* buscar_nodo_a_swappear(abb_nodo_t* abb_nodo) {
	// Función auxiliar para destruir_padre_completo: devuelve el menor de sus nietos izquierdo (o su hijo derecho, de no tener)
	if (!abb_nodo->izq) return abb_nodo;
	return buscar_nodo_a_swappear(abb_nodo->izq);
}

abb_nodo_t* buscar_padre_nodo_a_swappear(abb_nodo_t* abb_nodo,abb_nodo_t* nodo_a_swappear) {
	// Recibe un nodo seleccionado para swappear, y devuelve su padre
	if (!abb_nodo) return NULL;
	if (abb_nodo->izq == nodo_a_swappear) return abb_nodo;
	return buscar_padre_nodo_a_swappear(abb_nodo->izq,nodo_a_swappear);
}

void* destruir_hoja(abb_t* arbol,abb_nodo_t* actual, abb_nodo_t* padre) {
	// Auxiliar para _abb_borrar: recibe un nodo padre, su hijo, intercambia su clave y valor, y destruye al hijo
	void* dato = actual -> dato;
	if (!padre) arbol -> raiz = NULL;
	else {
		if (actual == padre->izq) padre->izq = NULL;
		else padre->der = NULL;
	}
	abb_nodo_destruir(actual,NULL);
	return dato;
}

void* destruir_padre_parcial(abb_nodo_t* actual,abb_nodo_t* padre,bool tiene_hijo_izquierdo) {
	// Auxiliar para _abb_borrar: recibe un padre, su hijo, intercambia su clave,valor, modifica los hijos y destruye al hijo original
	void* dato = actual->dato;
	abb_nodo_t* nodo_a_swappear;
	if (tiene_hijo_izquierdo) nodo_a_swappear = actual->izq;
	else nodo_a_swappear = actual->der;
	swappear_nodos(actual,nodo_a_swappear);
	actual->izq = nodo_a_swappear -> izq;
	actual->der = nodo_a_swappear -> der;
	abb_nodo_destruir(nodo_a_swappear,NULL);
	return dato;
}


void* destruir_padre_completo(abb_nodo_t* actual,abb_nodo_t* padre) {
	// Auxiliar para _abb_borrar: recibe un padre, su hijo, busca un nodo a swappear, intercambia valor,clave,hijos y lo destruye
	void* dato = actual->dato;
	abb_nodo_t* nodo_a_swappear = buscar_nodo_a_swappear(actual->der);
	if (nodo_a_swappear == actual->der) {
		swappear_nodos(actual,nodo_a_swappear);
		actual->der = nodo_a_swappear->der;
		}
	else {
		abb_nodo_t* padre_nodo_a_swappear = buscar_padre_nodo_a_swappear(actual->der,nodo_a_swappear);
		swappear_nodos(actual,nodo_a_swappear);
		padre_nodo_a_swappear->izq = nodo_a_swappear->der;
		
	}
	abb_nodo_destruir(nodo_a_swappear,NULL);
	return dato;
}


void* _abb_borrar(abb_t* arbol,abb_nodo_t* actual,const char* clave,bool borrar_raiz) {
	abb_nodo_t* padre;
	abb_nodo_t* hijo;
	if (borrar_raiz) {
		padre = NULL;
		hijo = arbol->raiz;
	}
	else {
		padre = _buscar_padre_nodo(arbol->raiz,NULL,arbol,clave,true);
		if (!padre) return NULL;
		hijo = conseguir_hijo(padre,clave,arbol->cmp);
	}
	size_t tipo_nodo = tipo_de_nodo(hijo);
	abb_nodo_t* resultado;
	switch(tipo_nodo) {
		case 0:
			resultado = destruir_hoja(arbol,hijo,padre);
			break;
		case 1:
			resultado = destruir_padre_parcial(hijo,padre,false);
			break;
		case 2:
			resultado = destruir_padre_parcial(hijo,padre,true);
			break;
		case 3:
			resultado = destruir_padre_completo(hijo,padre);
			break;
	}
	arbol->cantidad--;
	return resultado;
}


// PRIMITIVAS DEL ABB //


abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
	abb_t* abb = malloc(sizeof(abb_t));
	if (!abb) return NULL;
	abb -> raiz = NULL;
	abb -> cmp = cmp;
	abb -> destruir_dato = destruir_dato;
	abb -> cantidad = 0;
	return abb;
}

bool abb_guardar(abb_t* arbol, const char* clave, void* dato) {
	if (!arbol) return false;
	bool guardado_ok;
	if (!arbol->raiz) guardado_ok = abb_guardar_raiz(arbol,clave,dato);
	else guardado_ok = _abb_guardar(arbol,arbol->raiz,clave,dato);
	return guardado_ok;
}


void* abb_borrar(abb_t* arbol, const char* clave) {
	if (!arbol || !arbol->raiz) return NULL;
	void* dato;
	if (arbol->cmp(arbol->raiz->clave,clave) == 0) dato = _abb_borrar(arbol,arbol->raiz,clave,true);
	else dato = _abb_borrar(arbol,arbol->raiz,clave,false);
	return dato;
}


void* abb_obtener(const abb_t* arbol, const char* clave) {
	if (!arbol || !arbol->raiz) return NULL;
	if (arbol->cmp(arbol->raiz->clave,clave) == 0) return arbol->raiz->dato;
	abb_nodo_t* padre = _buscar_padre_nodo(arbol->raiz,NULL,arbol,clave,true);
	if(!padre) return NULL;
	abb_nodo_t* hijo = conseguir_hijo(padre,clave,arbol->cmp);
	return hijo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char* clave) {
	if (!arbol || !arbol->raiz) return false;
	if (arbol->cmp(arbol->raiz->clave,clave) == 0) return true;
	abb_nodo_t* padre = _buscar_padre_nodo(arbol->raiz,NULL,arbol,clave,true);
	if(!padre) return false;
	return true;
}

size_t abb_cantidad(abb_t *arbol) {
	return arbol -> cantidad;
}

void abb_destruir(abb_t *arbol) {
	_abb_destruir(arbol->raiz,arbol->destruir_dato);
	free(arbol);
}

// ITERADOR INTERNO //


bool _abb_in_order(abb_nodo_t* abb_nodo, bool visitar(const char*, void*, void*), void *extra) {
	if (!abb_nodo) return true;
	if(!_abb_in_order(abb_nodo->izq,visitar,extra) || !visitar(abb_nodo->clave,abb_nodo->dato,extra) || !_abb_in_order(abb_nodo->der,visitar,extra))return false;
	return true;
}


void abb_in_order(abb_t *arbol, bool visitar(const char*, void*, void*), void *extra) {
	_abb_in_order(arbol->raiz,visitar,extra);
}


// AUXILIARES DEL ITERADOR EXTERNO //

bool posterior_a_hasta(abb_nodo_t* nodo,const char* ref_hasta,abb_comparar_clave_t cmp) {
	if (ref_hasta && cmp(nodo->clave,ref_hasta) > 0) return true;
	return false;
}

bool anterior_a_desde(abb_nodo_t* nodo,const char* ref_desde,abb_comparar_clave_t cmp) {
	if (ref_desde && cmp(nodo->clave,ref_desde) < 0) return true;
	return false;
}

bool esta_en_rango(abb_nodo_t* nodo,const char* ref_desde,const char* ref_hasta,abb_comparar_clave_t cmp) {
	return (!anterior_a_desde(nodo,ref_desde,cmp) && !posterior_a_hasta(nodo,ref_hasta,cmp));
}

void apilar_nodos(abb_iter_t* iter,abb_nodo_t* nodo,const char* ref_desde,const char* ref_hasta) {
	if (!nodo) return;
	if (!posterior_a_hasta(nodo,ref_desde,iter->abb->cmp)) apilar_nodos(iter,nodo->der,ref_desde,ref_hasta);
	if (esta_en_rango(nodo,ref_desde,ref_hasta,iter->abb->cmp)) pila_apilar(iter->pila,nodo);
	if (!anterior_a_desde(nodo,ref_desde,iter->abb->cmp)) apilar_nodos(iter,nodo->izq,ref_desde,ref_hasta);
	
}


// PRIMITIVAS DEL ITERADOR EXTERNO //

abb_iter_t* abb_iter_in_crear(const abb_t *arbol,const char* ref_desde,const char* ref_hasta) {
	abb_iter_t* abb_iter_in = malloc(sizeof(abb_iter_t));
	if (!abb_iter_in) return NULL;
	abb_iter_in -> abb = arbol;
	abb_iter_in -> pila = pila_crear();
	if (!abb_iter_in -> pila) {
		free(abb_iter_in);
		return NULL;
	}
	if (arbol->raiz) {
		apilar_nodos(abb_iter_in,arbol->raiz,ref_desde,ref_hasta);
	}
	return abb_iter_in;
}

bool abb_iter_in_avanzar(abb_iter_t* iter) {
	if (pila_esta_vacia(iter->pila)) return false;
	abb_nodo_t* desapilado = pila_desapilar(iter->pila);
	if (desapilado->der) {
		apilar_nodos(iter,desapilado->der,iter->ref_desde,iter->ref_hasta);
	}
	return true;
}

const char* abb_iter_in_ver_actual(const abb_iter_t* iter) {
	if (pila_esta_vacia(iter->pila)) return NULL;
	abb_nodo_t* desapilado = pila_ver_tope(iter->pila);
	return desapilado->clave;
}

bool abb_iter_in_al_final(const abb_iter_t* iter) {
	return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter) {
	pila_destruir(iter->pila);
	free(iter);
}
