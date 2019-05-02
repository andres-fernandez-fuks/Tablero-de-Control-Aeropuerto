/* ******************************************************************
 * Corrector: Camila Dvorkin
 * Alumno: Nicolás Alberto Ramirez Scarfiello
 * Padrón: 102090
 * *****************************************************************/
#include "pila.h"
#include <stdlib.h>
#include <stdio.h>

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};
bool pila_redimensionar(pila_t *pila, size_t capacidad_nueva);
#define TAM_INICIAL 10
/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/
pila_t* pila_crear(void){
	pila_t* pila = malloc(sizeof(pila_t));
	
	if (pila == NULL) return NULL;
	pila->cantidad = 0;
	pila->capacidad = TAM_INICIAL;
	pila->datos = malloc(pila->capacidad * sizeof(void*));
		
	if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
	return pila;
}

void pila_destruir(pila_t *pila) {
	free(pila->datos);
	free(pila);
}

bool pila_esta_vacia(const pila_t *pila) {
	if(pila->cantidad == 0) return true;
	return false;
}

bool pila_apilar(pila_t *pila, void* valor) {
	bool status = true;
	if (pila->capacidad == pila->cantidad)  status = pila_redimensionar(pila , pila->cantidad * 2);
	if (status != true) return false;

	pila->datos[pila->cantidad] = valor;
	(pila->cantidad)++;
	return true;
}

void* pila_ver_tope(const pila_t *pila) {
	if (pila_esta_vacia(pila)) return NULL;
	return pila->datos[pila->cantidad-1];
}

void* pila_desapilar(pila_t *pila) {
	if (pila_esta_vacia(pila)) return NULL;
	void* elemento = pila_ver_tope(pila);
	(pila->cantidad)--;
	if ((pila->capacidad > 4*pila->cantidad)&&(pila->cantidad > 2 * TAM_INICIAL))  pila_redimensionar(pila , pila->capacidad / 2);
	return elemento;
}

bool pila_redimensionar(pila_t *pila, size_t capacidad_nueva) {
    void** datos_nuevo = realloc(pila->datos, capacidad_nueva * sizeof(void*));
    if (datos_nuevo == NULL) return false;
    pila->datos = datos_nuevo;
    pila->capacidad = capacidad_nueva;
    return true;
}
// ...
