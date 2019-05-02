/* ******************************************************************
 * Corrector: Camila Dvorkin
 * Alumno: Nicolás Alberto Ramirez Scarfiello
 * Padrón: 102090
 * *****************************************************************/
#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al principio de la lista. 
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra al 
// principio de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. 
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Elimina el elemento al principio de la lista. 
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se eliminó el primer elemento de la lista.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Obtiene el largo de la lista.
// Pre: la lista fue creada.
// Post: se devolvió el largo de la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crea un iterador para un TDA lista.
// Pre: la lista fue creada.
// Post: devuelve el iterador de una lista.
lista_iter_t *lista_iter_crear(lista_t *lista);


// Avanza el iterador de la lista.
// Pre: la lista fue creada, no esta vacia, y su iterador fue creado.
// Post: el iterador ahora apunta a la siguiente posición.
bool lista_iter_avanzar(lista_iter_t *iter);

// Obtiene el valor al que apunta el iterador de la lista.
// Pre: la lista fue creada, no esta vacia, y su iterador fue creado.
// Post: se devolvió el valor del elemento al que que esta apuntando el iterador 
// de la lista.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve verdadero o falso, según si el iterador esta o no apuntando al final
// de la lista.
// Pre: la lista y su iterador fueron creados
bool lista_iter_al_final(const lista_iter_t *iter);

// Destruye el iterador.
// Pre: el iterador de la lista fue creado
void lista_iter_destruir(lista_iter_t *iter);

// Agrega un nuevo elemento en la posicion a la que apunta el iterador de la lista . 
// Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, dato se encuentra en la posición
// a la que apunta el iterador de la lista.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Elimina el elemento en la posicion a la que apunta el iterador de la lista . 
// Devuelve NULL en caso de error.
// Pre: la lista fue creada.
// Post: se eliminó el elemento al que apuntaba el iterador, el iterador ahora apunta
// a la posición siguiente.
void *lista_iter_borrar(lista_iter_t *iter);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.

void pruebas_lista_alumno(void);
#endif // COLA_H
