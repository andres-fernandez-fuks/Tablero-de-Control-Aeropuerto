/* ********************************************************(**********
 * Corrector: Camila Dvorkin
 * Alumno: Nicolás Alberto Ramirez Scarfiello
 * Padrón: 102090
 * *****************************************************************/
#define _POSIX_C_SOURCE 200809L
#include "strutil.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* *****************************************************************
 *                           FUNCIONES
 * *****************************************************************/

char** split(const char* str, char sep) {
	size_t len = strlen(str);
	size_t contador_de_sep = 0;
	for(size_t i = 0; i<len; i++){
		if(str[i]==sep) contador_de_sep++;
	}
	char** lista = malloc((contador_de_sep+2) * sizeof(char*));
	if(!lista) return NULL;

	char* new_cadena_aux;
	char* cadena_aux =  strdup("");
	char caracter[2] = ""; 
	size_t largo_cadena_aux;
	size_t numero_de_cadena = 0;

	for(size_t i = 0; i<len; i++){
		if(str[i]==sep) {
			lista[numero_de_cadena] = strdup(cadena_aux);
			free(cadena_aux);
			cadena_aux =  strdup("");
			numero_de_cadena++;
		}else{     
			caracter[0] = str[i];  
			largo_cadena_aux = strlen(cadena_aux);
			new_cadena_aux = realloc(cadena_aux, (sizeof(cadena_aux)*largo_cadena_aux)+sizeof(caracter)); 
			if(!new_cadena_aux) {
				free(lista);
				return NULL;
			}
			cadena_aux = new_cadena_aux;
			strcat(cadena_aux, caracter);
		}
	}
	lista[numero_de_cadena] = strdup(cadena_aux);
	free(cadena_aux);
	numero_de_cadena++;
	lista[numero_de_cadena] = NULL;
	return lista;	
}

char* join(char** strv, char sep) {
	size_t len = 0;
	size_t i = 0;
	char separador[2] = ""; 
	separador[0] = sep;
	while(strv[i]!=NULL){
		len += strlen(strv[i])+1;
		i++;
	}
	char* cadena_aux;
	if(len>0){
		cadena_aux = malloc(sizeof(char*) * len);	
		if(!cadena_aux ) return NULL;
	}else{ 
		cadena_aux = strdup("");
	}


	char* puntero_a_cadena = cadena_aux;
	for(size_t j = 0 ; j<i; j++){
		puntero_a_cadena = stpcpy(puntero_a_cadena, strv[j]);
		if(j+1!=i) puntero_a_cadena = stpcpy(puntero_a_cadena, separador);
	}
	if(len>0)cadena_aux [len-1]=  '\0';
	return  cadena_aux ;
}

void free_strv(char* strv[]) {
	size_t i = 0;
	while(strv[i]!=NULL){
		free(strv[i]);
		i++;
	}
	free(strv);
}
// ...
