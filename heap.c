#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define CAPACIDAD_DEFAULT 100


struct heap {
	cmp_func_t cmp;
	void** arreglo;
	size_t cantidad;
	size_t capacidad;
};


// FUNCIONES AUXILIARES HEAP //


size_t posicion_padre(size_t pos_hijo) {
	return (pos_hijo-1)/2;
}

size_t posicion_hijo_izq(size_t pos_padre) {
	return pos_padre*2+1;
}

size_t posicion_hijo_der(size_t pos_padre) {
	return pos_padre*2+2;
}


void upheap(void** arreglo,size_t pos_actual,cmp_func_t cmp) {
	// "Sube" el elemento dentro del heap a la posición correcta para mantener la invariante de la estructura
	if (pos_actual <= 0) return;
	void* elemento = arreglo[pos_actual];
	size_t pos_padre = posicion_padre(pos_actual);
	void* padre = arreglo[pos_padre];
	if (cmp(elemento,padre) <= 0) return;
	arreglo[pos_actual] = padre;
	arreglo[pos_padre] = elemento;
	pos_actual = pos_padre;
	upheap(arreglo,pos_actual,cmp);
	return;
}

void swap(void** arreglo,size_t pos_padre, size_t pos_hijo) {
	// Intercambia dos punteros de un arreglo
	void* aux = arreglo[pos_padre];
	arreglo[pos_padre] = arreglo[pos_hijo];
	arreglo[pos_hijo] = aux;
}

size_t _downheap(void** arreglo,size_t pos_padre,size_t pos_hijo_izq,size_t pos_hijo_der,size_t cant,cmp_func_t cmp) {
	// Evalúa el intercambio de un elemento del arreglo cuando éste tiene dos hijos
	void* padre = arreglo[pos_padre];
	void* hijo_izq = arreglo[pos_hijo_izq];
	if (pos_hijo_der >= cant) {
		if (cmp(padre,hijo_izq) >= 0) return pos_padre;
		swap(arreglo,pos_padre,pos_hijo_izq);
		return pos_hijo_izq;
	}
	void* hijo_der = arreglo[pos_hijo_der];
	if (cmp(padre,hijo_izq) >= 0 && cmp(padre,hijo_der) >= 0) return pos_padre;
	if (cmp(hijo_izq,hijo_der) >= 0) {
		swap(arreglo,pos_padre,pos_hijo_izq);
		return pos_hijo_izq;
	}
	else {
		swap(arreglo,pos_padre,pos_hijo_der);
		return pos_hijo_der;
	}
}

void downheap(void** arreglo,size_t cantidad,size_t pos_actual,cmp_func_t cmp) {
	// "Baja" el elemento dentro del heap a la posición correcta para mantener la invariante de la estructura
	if(pos_actual >= cantidad) return;
	size_t pos_hijo_izq = posicion_hijo_izq(pos_actual);
	if (pos_hijo_izq >= cantidad) return;
	size_t pos_hijo_der = posicion_hijo_der(pos_actual);
	size_t nueva_pos = _downheap(arreglo,pos_actual,pos_hijo_izq,pos_hijo_der,cantidad,cmp);
	if (nueva_pos == pos_actual) return;
	downheap(arreglo,cantidad,nueva_pos,cmp);
}

void heapify(void* arreglo[],size_t cantidad,size_t pos_actual,cmp_func_t cmp) {
	// Ordena un heap hasta que se cumpla en todos sus elementos la invariante de la estructura
	downheap(arreglo,cantidad,pos_actual,cmp);
	if (pos_actual == 0) return;
	heapify(arreglo,cantidad,pos_actual-1,cmp);
}


void _heap_sort(void* arreglo[],size_t cantidad,size_t pos_ult_relativo,cmp_func_t cmp) {
	// Función auxiliar iterativa de heap_sort
	if (pos_ult_relativo == 0) return;
	swap(arreglo,0,pos_ult_relativo);
	downheap(arreglo,pos_ult_relativo,0,cmp);
	_heap_sort(arreglo,cantidad,pos_ult_relativo-1,cmp);
}

void heap_sort(void* arreglo[], size_t cantidad, cmp_func_t cmp) {
	if (cantidad == 0) return;
	heapify(arreglo,cantidad,cantidad/2,cmp);
	_heap_sort(arreglo,cantidad,cantidad-1,cmp);
}


void determinar_redimension_heap(heap_t* heap) {
	// Determina si un heap debe o no redimensionarse, y de ser así, lo hace
	size_t nueva_capacidad = 0;
	if (heap->cantidad == heap->capacidad) nueva_capacidad = heap->capacidad*2;
	else if (heap->cantidad == heap->capacidad/4) {
		if (heap->capacidad/2 > CAPACIDAD_DEFAULT) nueva_capacidad = heap->capacidad/2;
	}
	if (nueva_capacidad != 0) {
		heap->arreglo = realloc(heap->arreglo,nueva_capacidad*sizeof(void*));
		heap->capacidad = nueva_capacidad;
	}
}

void copiar_arreglo(void** arreglo_orig,size_t cant,void** arreglo_copia) {
	// Copia los elementos del arreglo original al arreglo copia
	for (size_t i = 0; i < cant; i++) {
		arreglo_copia[i] = arreglo_orig[i];
	}
}

void _heap_crear(heap_t* heap,size_t cant_inicial,cmp_func_t cmp) {
	heap->cantidad = cant_inicial;
	heap->capacidad = cant_inicial + CAPACIDAD_DEFAULT;
	heap->cmp = cmp;
	heap->arreglo = malloc(sizeof(void*)*heap->capacidad);
}



// PRIMITIVAS DEL HEAP //


heap_t* heap_crear(cmp_func_t cmp) {
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;
	_heap_crear(heap,0,cmp);
	return heap;
}

heap_t* heap_crear_arr(void* arreglo[], size_t n, cmp_func_t cmp) {
	heap_t* heap = malloc(sizeof(heap_t));
	if (!heap) return NULL;
	_heap_crear(heap,n,cmp);
	copiar_arreglo(arreglo,n,heap->arreglo);
	heapify(heap->arreglo,heap->cantidad,heap->cantidad-1,cmp);
	return heap;
}

void heap_destruir(heap_t* heap, void destruir_elemento(void* e)) {
	if (destruir_elemento) {
		for (size_t i = 0; i < heap->cantidad; i++) {
		destruir_elemento(heap->arreglo[i]);
		}
	}
	free(heap->arreglo);
	free(heap);
}

size_t heap_cantidad(const heap_t* heap) {
	return heap->cantidad;
}

bool heap_esta_vacio(const heap_t* heap) {
	return heap->cantidad == 0;
}

bool heap_encolar(heap_t* heap, void* elem) {
	if (!elem) return false;
	heap -> arreglo[heap->cantidad] = elem;
	heap->cantidad++;
	if (heap->cantidad > 1) upheap(heap->arreglo,heap->cantidad-1,heap->cmp);
	determinar_redimension_heap(heap);
	return true;
}

void* heap_ver_max(const heap_t *heap) {
	if (heap_esta_vacio(heap)) return NULL;
	return heap->arreglo[0];
}

void* heap_desencolar(heap_t* heap) {
	if (heap_esta_vacio(heap)) return NULL;
	size_t n = heap->cantidad;
	void* elem_a_devolver = heap->arreglo[0];
	heap->arreglo[0] = heap->arreglo[n-1];
	heap->arreglo[n-1] = NULL;
	heap->cantidad--;
	if (heap-> cantidad > 0) downheap(heap->arreglo,heap->cantidad,0,heap->cmp);
	determinar_redimension_heap(heap);
	return elem_a_devolver;
}





