#include "hash.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#define CANTIDAD_POR_DEFECTO  41
#define FACTOR_DE_REDIMENSION 10
#define COTA_MINIMA 0.25
#define COTA_MAXIMA 0.75

struct hash{
	lista_t** tabla;
    size_t cantidad;
    size_t capacidad;
    hash_destruir_dato_t destruir_dato;
};

typedef struct hash_campo {
    char *clave;
    void *valor;
}hash_campo_t;

struct hash_iter {
	const hash_t* hash;
	lista_iter_t* lista_iter;
	size_t posicion;
};


size_t funcion_hashing(const char* clave,size_t capacidad) {
	size_t hash = 5381;
	int c;
	while ((c = *clave++))hash = ((hash << 5) + hash) + (size_t)c; 
	return hash % capacidad;
}


/* ******************************************************************
 *           PRIMITIVAS DEL AUXILIAR HASH CAMPO
 * *****************************************************************/
hash_campo_t* hash_campo_crear(char* clave,void* dato) {
	hash_campo_t* nuevo_hash_campo = malloc(sizeof(hash_campo_t));
	if (!nuevo_hash_campo) return NULL;
	nuevo_hash_campo -> clave = clave;
	nuevo_hash_campo -> valor = dato;
	return nuevo_hash_campo;
}

bool agregar_campo(lista_iter_t* iter_lista,const char* clave,void* dato) {
	char* copia_clave = malloc(sizeof(char)*(strlen(clave)+1));
	strcpy(copia_clave,clave);
	hash_campo_t* nuevo_campo = hash_campo_crear(copia_clave,dato);
	if (!nuevo_campo) return false;
	bool insertado_ok = lista_iter_insertar(iter_lista,nuevo_campo); 
	return insertado_ok; 
}

lista_iter_t* buscar_campo(lista_t* lista,const char* clave) {
	lista_iter_t* iter = lista_iter_crear(lista);
	while (!lista_iter_al_final(iter)) {
		hash_campo_t* campo = lista_iter_ver_actual(iter);
		if (strcmp(campo->clave, clave) == 0) {
			break;
		}
		lista_iter_avanzar(iter);
	}
	return iter;
}


void hash_campo_borrar(hash_campo_t* campo, hash_destruir_dato_t destruir_dato){
	if(destruir_dato) destruir_dato(campo->valor);
	free(campo->clave);
	free(campo);
}

/* ******************************************************************
 *               AUXILIARES DE PRIMITIVAS DEL HASH
 * *****************************************************************/
float factor_de_carga(hash_t* hash){
	return (float) (hash->cantidad / hash->capacidad);
}

lista_t** crear_tabla(hash_t* hash, size_t capacidad_nueva){
	lista_t** nueva_tabla = malloc(sizeof(lista_t*)*capacidad_nueva);
	if(!nueva_tabla) return NULL;
	for(size_t i = 0; i< capacidad_nueva; i++){
		nueva_tabla[i] = lista_crear();
		if(!nueva_tabla[i])return NULL;
	}
	return nueva_tabla;
}

void destruir_tabla(hash_t* hash){
	for(size_t i = 0; i < hash -> capacidad ; i++){
		lista_t* lista_a_destruir = hash -> tabla[i];
		if(!lista_a_destruir)continue;
		lista_iter_t * iter = lista_iter_crear(lista_a_destruir);
		while(!lista_iter_al_final(iter)){
			hash_campo_t* campo_actual = lista_iter_borrar(iter);  
			hash_campo_borrar(campo_actual, hash->destruir_dato);
		}
		lista_iter_destruir(iter);
		lista_destruir(lista_a_destruir, NULL);
	}
	free(hash->tabla);
}

bool re_hasheo(hash_t* hash, lista_t** nueva_tabla, size_t capacidad_nueva){
	for(size_t i = 0; i< hash->capacidad; i++){
		lista_t* lista_a_trasladar = hash -> tabla[i];
		if(lista_esta_vacia(lista_a_trasladar)){
			lista_destruir(lista_a_trasladar, NULL);
			continue;
		}
		lista_iter_t* iter = lista_iter_crear(lista_a_trasladar);
		while(!lista_iter_al_final(iter)){
			hash_campo_t* campo = lista_iter_borrar(iter); 
			size_t pos_nueva = (size_t) funcion_hashing(campo->clave, capacidad_nueva); 
			bool status = lista_insertar_primero(nueva_tabla[pos_nueva],  campo); 
			if(status == false) return false;
		}
		lista_iter_destruir(iter);
		lista_destruir(lista_a_trasladar, free);
	}
	return true;
}

bool hash_redimensionar(hash_t* hash, size_t capacidad_nueva){
	lista_t** nueva_tabla = crear_tabla(hash, capacidad_nueva);
	if(!nueva_tabla)return false;
	if(!re_hasheo(hash, nueva_tabla, capacidad_nueva))return false;
	free(hash->tabla);
	hash->tabla = nueva_tabla;
	hash->capacidad = capacidad_nueva;
	return true;
		
}


bool supera_minimo_aceptado(size_t capacidad){
	return capacidad / FACTOR_DE_REDIMENSION >= CANTIDAD_POR_DEFECTO;
}

lista_iter_t* encontrar_clave(const hash_t* hash, const char* clave){
	size_t pos_guardado = (size_t) funcion_hashing(clave, hash->capacidad);
	lista_t* lista_elegida = hash -> tabla[pos_guardado];
	lista_iter_t* iter = buscar_campo(lista_elegida,clave);
	return iter;
}


/* ******************************************************************
 *                        PRIMITIVAS DEL HASH
 * *****************************************************************/

hash_t* hash_crear(hash_destruir_dato_t destruir_dato) {
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;
	lista_t** tabla = crear_tabla(hash, CANTIDAD_POR_DEFECTO);
	if(!tabla) {
		free(hash);
		return NULL;
	}

	hash->tabla = tabla;
	hash->cantidad = 0;
	hash->capacidad = CANTIDAD_POR_DEFECTO;
	hash->destruir_dato = destruir_dato;
	return hash;
}

bool hash_guardar(hash_t* hash, const char* clave, void* dato) {
	if (factor_de_carga(hash) >= COTA_MAXIMA) {
		size_t nueva_capacidad = hash->capacidad * FACTOR_DE_REDIMENSION;
		if (!hash_redimensionar(hash, nueva_capacidad)) return false;
	}
	lista_iter_t* iter = encontrar_clave(hash, clave);
	hash_campo_t* campo = lista_iter_ver_actual(iter);
	if (campo) {
		if(hash->destruir_dato) hash->destruir_dato(campo->valor);
		campo -> valor = dato;
		lista_iter_destruir(iter);
		return true;
	}
	bool agregado_ok = agregar_campo(iter,clave,dato);
	if (agregado_ok) hash -> cantidad++;
	lista_iter_destruir(iter);
	return agregado_ok;
}

void* hash_borrar(hash_t* hash, const char* clave) {
	if (factor_de_carga(hash) <= COTA_MINIMA && supera_minimo_aceptado(hash->capacidad)) {
		size_t nueva_capacidad = hash->capacidad / FACTOR_DE_REDIMENSION;
		if (!hash_redimensionar(hash, nueva_capacidad)) return NULL;
	}
	lista_iter_t* iter = encontrar_clave(hash, clave);
	hash_campo_t* campo = lista_iter_borrar(iter);
	lista_iter_destruir(iter);
	if (!campo) {
		return NULL;
	}
	hash -> cantidad --;
	void* dato = campo -> valor;
	hash_campo_borrar(campo, NULL);
	return dato;
}

void* hash_obtener(const hash_t *hash, const char *clave) {
	lista_iter_t* iter = encontrar_clave(hash, clave);
	hash_campo_t* campo = lista_iter_ver_actual(iter);
	lista_iter_destruir(iter);
	if (campo) return campo -> valor;
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave) { 
	lista_iter_t* iter = encontrar_clave(hash, clave);
	hash_campo_t* campo = lista_iter_ver_actual(iter);
	lista_iter_destruir(iter);
	if (!campo) return false;
	return true;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}
void hash_destruir(hash_t *hash){
	destruir_tabla(hash);
	free(hash);
}



/* ******************************************************************
 *              PRIMITIVAS AUXILIARES DEL ITERADOR DEL HASH
 * *****************************************************************/
bool encontrar_pos_no_vacia(const hash_t* hash, size_t* posicion_iter , lista_iter_t** iterador_de_lista){
	for(size_t i = *posicion_iter; i < hash->capacidad ; i++) {
		if(!lista_esta_vacia(hash->tabla[i])){
			*posicion_iter = i;
			*iterador_de_lista = lista_iter_crear(hash->tabla[i]);
			return true;
		}
	}
	*iterador_de_lista = NULL;
	return false;
}

/* ******************************************************************
 *               PRIMITIVAS DEL ITERADOR DEL HASH
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t* hash) {
	if(!hash) return NULL;
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if(!hash_iter) return NULL;
	hash_iter->hash = hash;
	hash_iter->posicion = 0;
	encontrar_pos_no_vacia(hash, &hash_iter->posicion, &hash_iter->lista_iter);
	return hash_iter;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
	if(!iter && !iter->lista_iter) return false;
	if(hash_iter_al_final(iter))return false;
	lista_iter_avanzar(iter->lista_iter);
	if(!lista_iter_al_final(iter->lista_iter))return true;
	lista_iter_destruir(iter->lista_iter);
	iter->posicion++;
	return encontrar_pos_no_vacia(iter->hash, &iter->posicion, &iter->lista_iter);
}

const char* hash_iter_ver_actual(const hash_iter_t* iter) { 
	if(hash_iter_al_final(iter))return NULL;
	hash_campo_t* campo = lista_iter_ver_actual(iter->lista_iter);
	return campo->clave;
}

bool hash_iter_al_final(const hash_iter_t *iter) {
	if(!iter) return true;
	if(iter->lista_iter)return lista_esta_vacia(iter->hash->tabla[iter->posicion]);
	else return true;
}

void hash_iter_destruir(hash_iter_t* iter){
	if(!iter) return;
	if(iter->lista_iter)lista_iter_destruir(iter->lista_iter);
	free(iter);
}

