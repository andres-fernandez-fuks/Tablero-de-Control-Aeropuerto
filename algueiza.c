#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"
#include "abb.h"
#include "lista.h"
#include "pila.h"
#include "strutil.h"
#include "heap.h"

typedef struct vuelo{
	char** datos;
	char* prioridad;
}vuelo_t;

/* ******************************************************************
 *           PRIMITIVAS DE LA ESTRUCTURA VUELO
 * *****************************************************************/

vuelo_t* crear_vuelo(char** datos_del_vuelo) {
	vuelo_t* vuelo= malloc(sizeof(vuelo_t));
	if(!vuelo)return NULL;
	char** datos= malloc(sizeof(lista_t*)*11);
	if(!datos){
		free(vuelo);
		return NULL;
	}
	size_t i = 0;
	while(datos_del_vuelo[i]){
		datos[i] = malloc(sizeof(char)*(strlen(datos_del_vuelo[i])+1));
		strcpy(datos[i], datos_del_vuelo[i]);
		i++;
	}
	datos[i] = NULL;
	vuelo->datos = datos;
	return vuelo;
}

void imprimir_vuelo(vuelo_t* vuelo){
	size_t i = 0;
	while(vuelo->datos[i]!=NULL){
		if (!vuelo->datos[i+1]) fprintf(stdout, "%s", vuelo->datos[i]);
		else fprintf(stdout, "%s ", vuelo->datos[i]);
		i++;
	}
	fprintf(stdout, "\n");
}

void vuelo_destruir(void* vuelo){
	vuelo_t* datos_del_vuelo = (vuelo_t*)vuelo;
	size_t i = 0;
	while(datos_del_vuelo->datos[i]!=NULL){
		free(datos_del_vuelo->datos[i]);
		i++;
	}
	free(datos_del_vuelo->datos);
	free(datos_del_vuelo);
}

/* ******************************************************************
 *           AUXILIARES PARA IMPRIMIR CADENAS(no esta en uso)
 * *****************************************************************/

// Los hice por que no sabia que siempre pedian imprimir el vuelo completo.
char* concatenar_codigo(char* horario_o_prioridad, char* codigo){
	char** lista_de_cadenas = malloc(sizeof(char*)*4);
	lista_de_cadenas[0] = horario_o_prioridad;
	lista_de_cadenas[1] = "-";
	lista_de_cadenas[2] = codigo;
	lista_de_cadenas[3] = NULL;
	char* resultado_codigo = join(lista_de_cadenas, ' ');
	free(lista_de_cadenas);
	return resultado_codigo;
}

char* obtener_codigo(char* horario_codigo){
	char** lista_de_cadenas = split(horario_codigo, ' ');
	char* codigo = malloc(sizeof(char*)*strlen(lista_de_cadenas[2])+sizeof(char));
	strcpy(codigo, lista_de_cadenas[2]);
	free_strv(lista_de_cadenas);
	return codigo;
}

void destruir_vuelo_repetido(hash_t* datos_de_vuelos, abb_t* horarios_de_vuelos,char* num_vuelo){
	vuelo_t* vuelo_ant = hash_obtener(datos_de_vuelos,num_vuelo);
	char** terminos_ant = vuelo_ant->datos;
	char* horario_cod_ant = concatenar_codigo(terminos_ant[6], terminos_ant[0]);
	free(abb_borrar(horarios_de_vuelos,horario_cod_ant));
	hash_borrar(datos_de_vuelos,num_vuelo);
	vuelo_destruir(vuelo_ant);
	free(horario_cod_ant);
}


/* ******************************************************************
 *           FUNCIONES PEDIDAS POR EL TP
 * *****************************************************************/

// Agregar archivo, ya reemplaza con la ultima aparicion de un vuelo en los archivos que se agregan, ya desempata por codigo de vuelo
int agregar_archivo(hash_t* datos_de_vuelos, abb_t* horarios_de_vuelos, char* ruta_archivo){
	FILE* archivo = fopen(ruta_archivo, "r");
	if(!archivo){
		return 1;
	}
	char* linea = NULL;
	size_t tam = 0; 
	while((getline(&linea, &tam, archivo)) > 0) {
		size_t len = strlen(linea);
		linea[len-1]='\0';
		char** terminos = split(linea, ',');
		vuelo_t* vuelo = crear_vuelo(terminos);
		char* horario_codigo = concatenar_codigo(terminos[6], terminos[0]);
		if (hash_pertenece(datos_de_vuelos,terminos[0])) {
			destruir_vuelo_repetido(datos_de_vuelos,horarios_de_vuelos,terminos[0]);
		}
		abb_guardar(horarios_de_vuelos, horario_codigo, horario_codigo);
		hash_guardar(datos_de_vuelos, terminos[0], vuelo);
		free_strv(terminos);
	}
	if(linea)free(linea);
	fclose(archivo);
	return 0;
}

bool info_vuelo(const hash_t* datos_de_vuelos, const char* codigo_de_vuelo){
	vuelo_t* vuelo = (vuelo_t*) hash_obtener(datos_de_vuelos, codigo_de_vuelo);
	if(!vuelo) return false;
	else imprimir_vuelo(vuelo);
	return true;
}

// ver tablero
int ver_tablero(const abb_t* horarios_de_vuelos, const hash_t* datos_de_vuelos, int k, const char* modo, const char* desde, const char* hasta){
	pila_t* pila = pila_crear();
	if (!pila) return 1;
	abb_iter_t* iter = abb_iter_in_crear(horarios_de_vuelos,desde,hasta);
	if (!iter) {
		pila_destruir(pila);
		return 1;
	}
	size_t contador = 0;
	while(!abb_iter_in_al_final(iter)){
		char* horario_codigo = (char*)abb_iter_in_ver_actual(iter);
		if (strcmp(modo, "asc")==0) {
			if(contador < k){
				printf("%s\n",(char*) abb_obtener(horarios_de_vuelos, horario_codigo));
				contador++;
			}
			else break;
		}
		else pila_apilar(pila, abb_obtener(horarios_de_vuelos, horario_codigo));
		abb_iter_in_avanzar(iter);
	}
	abb_iter_in_destruir(iter);
	contador = 0;
	if(strcmp(modo, "desc")==0){
		while (!pila_esta_vacia(pila) && contador<k ) {
			printf("%s\n",(char*) pila_desapilar(pila));
			contador++;
		}
	}
	pila_destruir(pila);
	return 0;
}

int obtener_prioridad_vuelo(const char* codigo) {
	char** cadenas = split(codigo,'-');
	int prioridad = atoi(cadenas[0]);
	free_strv(cadenas);
	return prioridad;
}

char* obtener_cadena_vuelo(const char* codigo) {
	char** cadenas = split(codigo,'-');
	char* copia_cadena = malloc(sizeof(char)*strlen(cadenas[1])+1);
	strcpy(copia_cadena,cadenas[1]);
	free_strv(cadenas);
	return copia_cadena;
}

int cmp_codigos_vuelo(const void* punt_1,const void* punt_2) {
	const char* cadena_1 = punt_1;
	const char* cadena_2 = punt_2;
	int prior_1 = obtener_prioridad_vuelo(cadena_1);
	int prior_2 = obtener_prioridad_vuelo(cadena_2);
	if (prior_1 < prior_2) return 1;
	else if (prior_2 < prior_1) return -1;
	char* cad_vuelo_1 = obtener_cadena_vuelo(cadena_1);
	char* cad_vuelo_2 = obtener_cadena_vuelo(cadena_2);
	int resultado = strcmp(cad_vuelo_1,cad_vuelo_2);
	free(cad_vuelo_1);
	free(cad_vuelo_2);
	return resultado;
}


int prioridad_vuelos(hash_t* datos_de_vuelos , int k){
	heap_t* prioridad_de_vuelos = heap_crear(cmp_codigos_vuelo);
	if (!prioridad_de_vuelos) return 1;
	hash_iter_t* iter = hash_iter_crear(datos_de_vuelos);
	if(!iter) {
		heap_destruir(prioridad_de_vuelos,NULL);
		return 1;
	}
	pila_t* pila_aux = pila_crear();
	if (!pila_aux) {
		heap_destruir(prioridad_de_vuelos,NULL);
		hash_iter_destruir(iter);
		return 1;
	}

	char* prioridad_codigo;
	while(!hash_iter_al_final(iter)){
		vuelo_t* vuelo = (vuelo_t*) hash_obtener(datos_de_vuelos, hash_iter_ver_actual(iter));
		prioridad_codigo = concatenar_codigo(vuelo->datos[5], vuelo->datos[0]);
		if (k > 0) {
			heap_encolar(prioridad_de_vuelos, prioridad_codigo);
		}
		else if (cmp_codigos_vuelo((void*) prioridad_codigo,heap_ver_max(prioridad_de_vuelos)) < 0) {
			free(heap_desencolar(prioridad_de_vuelos));
			heap_encolar(prioridad_de_vuelos,prioridad_codigo);
		}
		else free(prioridad_codigo);
		k--;
		hash_iter_avanzar(iter);
	}
	while(!heap_esta_vacio(prioridad_de_vuelos)){
		pila_apilar(pila_aux ,heap_desencolar(prioridad_de_vuelos));
	}
	while((!pila_esta_vacia(pila_aux))){
		prioridad_codigo = pila_desapilar(pila_aux);
		printf("%s\n",prioridad_codigo);
		free(prioridad_codigo);
	}
	hash_iter_destruir(iter);
	heap_destruir(prioridad_de_vuelos, free);
	pila_destruir(pila_aux);
	return 0;
}

// borrar, arreglado el tema de destruir_vuelo
int borrar(abb_t* horarios_de_vuelos, hash_t* datos_de_vuelos, const char* desde, const char* hasta){
	abb_iter_t* iter = abb_iter_in_crear(horarios_de_vuelos,desde,hasta);
	lista_t* lista_aux = lista_crear();
	if(!iter) return 1;
	char* codigo_de_vuelo = NULL;
	while(!abb_iter_in_al_final(iter)){
		if(strcmp((char*)abb_iter_in_ver_actual(iter), hasta)>0) break;
		if(strcmp((char*)abb_iter_in_ver_actual(iter), desde)>=0){
			codigo_de_vuelo = obtener_codigo((char*)abb_iter_in_ver_actual(iter));
			vuelo_t* vuelo = (vuelo_t*) hash_borrar(datos_de_vuelos, codigo_de_vuelo);
			lista_insertar_ultimo(lista_aux, (char*)abb_iter_in_ver_actual(iter));
			imprimir_vuelo(vuelo);
			free(codigo_de_vuelo);
			vuelo_destruir(vuelo);
		}
		abb_iter_in_avanzar(iter);
	}
	abb_iter_in_destruir(iter);
	while(!lista_esta_vacia(lista_aux)){
		free(abb_borrar(horarios_de_vuelos, lista_borrar_primero(lista_aux)));
	}
	lista_destruir(lista_aux, NULL);
	return 0;
}

/* ******************************************************************
 *           PRIMITIVAS DE LA INTERFAZ
 * *****************************************************************/

// Esta hecho asi solo por que empieza con agregar archivo, pero su existencia depende de la interfaz
FILE* validar_parametros(int argc, char *argv[]){
	FILE* archivo;
	if(argc<1 || argc >2){
		fprintf(stderr, "Cantidad de parametros erronea\n");
		return NULL;
	}
	if(argc == 1)archivo = stdin;
	else{
		char* ruta = argv[1];
		archivo = fopen(ruta, "r");
		if(archivo==NULL){
			fprintf(stderr, "No se pudo leer el archivo indicado\n");
			return NULL;
		}
	}
	return archivo;
}


/* ******************************************************************
 *        FUNCIONES DE VALIDACIÓN DE PARÁMETROS INGRESADOS
 * *****************************************************************/


bool validar_parametros_para_agregar_archivo(char** parametros, char** ruta){
	if(!parametros[1] || parametros[2]) return false;
	char* cadena_csv = ".csv";
	if (!strstr(parametros[1],cadena_csv)) return false;
	*ruta = malloc(sizeof(char*)*strlen(parametros[1])+sizeof(char));
	strcpy(*ruta, parametros[1]);
	return true;
}

bool es_numero(char* dato){
	size_t len = strlen(dato);
	for (int j = 0; j<len; j++){
		if(len>1 && j == 0 && dato[j] == '-') continue;
		if(isalpha(dato[j])||ispunct(dato[j]))return false;
	}
	return true;
}

bool validar_parametros_para_ver_tablero(char** parametros,int* k ,char** modo, char** desde, char** hasta){
	if(es_numero(parametros[1]) && atoi(parametros[1]) > 0) *k = atoi(parametros[1]);
	else return false;
	if((strcmp( parametros[2] , "asc")==0) || (strcmp( parametros[2] , "desc")==0)){
		*modo = malloc(sizeof(char*)*strlen(parametros[2])+sizeof(char));
		strcpy(*modo, parametros[2]);
	}else return false;
	if((parametros[3]) && (parametros[4])&& (strcmp(parametros[3],parametros[4])<0)){
		*desde = malloc(sizeof(char*)*strlen(parametros[3])+sizeof(char));
		strcpy(*desde, parametros[3]);
		*hasta = malloc(sizeof(char*)*strlen(parametros[4])+sizeof(char));
		strcpy(*hasta, parametros[4]);
	}else return false;
	return true;
}

bool validar_parametros_para_info_vuelo(char** parametros, char** codigo){
	if(parametros[1]){
		*codigo = malloc(sizeof(char*)*strlen(parametros[1])+sizeof(char));
		strcpy(*codigo, parametros[1]);
		return true;
	}
	return false;
}

bool validar_parametros_para_prioridad_vuelo(char** parametros,int* k ){
	if(es_numero(parametros[1]) && atoi(parametros[1]) > 0){
		*k = atoi(parametros[1]);
		return true;
	}else return false;
}

bool validar_parametros_para_borrar(char** parametros,char** desde, char** hasta){
	if((parametros[1]) && (parametros[2])&& (strcmp(parametros[1],parametros[2])<0)){
		*desde = malloc(sizeof(char*)*strlen(parametros[1])+sizeof(char));
		strcpy(*desde, parametros[1]);
		*hasta = malloc(sizeof(char*)*strlen(parametros[2])+sizeof(char));
		strcpy(*hasta, parametros[2]);
		return true;
	}
	return false;
}


void rutina_agregar_archivo(hash_t* datos_de_vuelos,abb_t* horarios_de_vuelos,char** terminos) {
	char* ruta = NULL;
	if (validar_parametros_para_agregar_archivo(terminos, &ruta)) {
		if (agregar_archivo(datos_de_vuelos, horarios_de_vuelos, ruta) == 1) fprintf(stderr, "Error en comando %s\n",terminos[0]);
		else {
			free(ruta);
			fprintf(stdout,"OK\n");
		}
	}
	else fprintf(stderr, "Error en comando %s\n",terminos[0]);
}

void rutina_ver_tablero(hash_t* datos_de_vuelos,abb_t* horarios_de_vuelos,char** terminos) {
	int k = 0;
	char* modo = NULL;
	char* desde = NULL;
	char* hasta = NULL;
	if(validar_parametros_para_ver_tablero(terminos,  &k, &modo, &desde, &hasta)){
		ver_tablero(horarios_de_vuelos, datos_de_vuelos,  k, modo, desde, hasta);
		free(modo);
		free(desde);
		free(hasta);
		fprintf(stdout,"OK\n");
	}
	else fprintf(stderr, "Error en comando %s\n",terminos[0]);
}

void rutina_info_vuelo(hash_t* datos_de_vuelos,char** terminos) {
	char* codigo = NULL;
	if(validar_parametros_para_info_vuelo(terminos, &codigo)){
		bool vuelo_ok = info_vuelo(datos_de_vuelos, codigo);
		if (!vuelo_ok) fprintf(stderr, "Error en comando %s\n",terminos[0]);
		else fprintf(stdout,"OK\n");
		free(codigo);
	}
	else fprintf(stderr, "Error en comando %s\n",terminos[0]);
}

void rutina_prioridad_vuelos(hash_t* datos_de_vuelos,char** terminos) {
	int k = 0;
	if(validar_parametros_para_prioridad_vuelo(terminos, &k)){
		prioridad_vuelos(datos_de_vuelos, k); 
		fprintf(stdout,"OK\n");
	}
	else fprintf(stderr, "Error en comando %s\n",terminos[0]);
}

void rutina_borrar_vuelos(hash_t* datos_de_vuelos,abb_t* horarios_de_vuelos,char** terminos) {
	char* desde = NULL;
	char* hasta = NULL;
	if(validar_parametros_para_borrar(terminos,&desde, &hasta)){
		borrar(horarios_de_vuelos, datos_de_vuelos,desde, hasta);
		free(desde);
		free(hasta);
		fprintf(stdout,"OK\n");
	}
	else fprintf(stderr, "Error en comando %s\n",terminos[0]);
}



int main(void){
	hash_t* datos_de_vuelos = hash_crear(vuelo_destruir);
	abb_t* horarios_de_vuelos = abb_crear(strcmp, free);
	char* linea = NULL;
	size_t tam = 0; 
	while((getline(&linea, &tam, stdin)) > 0) {
		size_t len = strlen(linea);
		linea[len-1]='\0';
		char** terminos = split(linea, ' ');
		if(strcmp( terminos[0] , "agregar_archivo")==0) {
			rutina_agregar_archivo(datos_de_vuelos,horarios_de_vuelos,terminos);
		}
		if(strcmp( terminos[0] , "ver_tablero")==0) {
			rutina_ver_tablero(datos_de_vuelos,horarios_de_vuelos,terminos);
		}
		if(strcmp( terminos[0] , "info_vuelo")==0) {
			rutina_info_vuelo(datos_de_vuelos,terminos);
		}
		if(strcmp( terminos[0] , "prioridad_vuelos")==0) {
			rutina_prioridad_vuelos(datos_de_vuelos,terminos);
		}	
		if(strcmp( terminos[0] , "borrar")==0) {
			rutina_borrar_vuelos(datos_de_vuelos,horarios_de_vuelos,terminos);
		}
		free_strv(terminos);
	}
	if(linea)free(linea);
	abb_destruir(horarios_de_vuelos);
	hash_destruir(datos_de_vuelos);
	return 0;
}
