#include "heap.h"
#include "vector_dinamico.h"
#include <stdlib.h>
#include <stdbool.h> 
#include <stddef.h>	  
#include <string.h>
#include <stdio.h>
#define TAM_INICIAL 50
#define DUP 2
#define DIV 2
#define OCUPADO 75
#define DESOCUPADO 25


/* Tipo utilizado para el heap. */
struct heap{
	
	size_t cantidad;
	vector_t* vec;
	cmp_func_t cmp;
	
};

bool redimensionar(heap_t * heap,size_t dup,size_t div);
void upheap(vector_t* vector, size_t pos, cmp_func_t cmp);
void downheap(vector_t* vector, size_t cant, size_t pos, cmp_func_t cmp);
void swap(void*vector[], size_t uno, size_t dos);


/* Función de heapsort genérica. Esta función ordena mediante heap_sort
 * un arreglo de punteros opacos, para lo cual requiere que se
 * le pase una función de comparación. Modifica el arreglo "in-place".
 * Nótese que esta función NO es formalmente parte del TAD Heap.
 */
void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
	//heap_t* sort = heap_crear_arr(elementos,cant,cmp);
	heap_t* sort = heap_crear(cmp);
	if (!sort) return;
	
	for(size_t i=0; i<cant; i++){
		heap_encolar(sort,elementos[i]);
	}	
	for (size_t i=cant;i>0;i--)
		elementos[i-1]= heap_desencolar(sort);
	
	heap_destruir(sort,NULL);
}

/*
 * Implementación de un TAD cola de prioridad, usando un max-heap.
 *
 * Notar que al ser un max-heap el elemento mas grande será el de mejor
 * prioridad. Si se desea un min-heap, alcanza con invertir la función de
 * comparación.
 */


/* Crea un heap. Recibe como único parámetro la función de comparación a
 * utilizar. Devuelve un puntero al heap, el cual debe ser destruido con
 * heap_destruir().
 */
heap_t *heap_crear(cmp_func_t cmp){
	
	if (!cmp) return NULL;
	heap_t* heap = calloc(1, sizeof(heap_t));
	if (!heap) return NULL;
	
	vector_t* vector = vector_crear(TAM_INICIAL+1);
		if (!vector) return NULL;
		
	for (size_t i = 0; i<=TAM_INICIAL; i++) vector_guardar(vector,i,NULL);
	heap->cmp = cmp;
	heap->vec = vector;
	heap->cantidad = 0;
	
	return heap;

}


void swap(void* vector[], size_t uno, size_t dos) {
	void* aux = NULL;
	aux = vector[uno];
	vector[uno] = vector[dos];
	vector[dos] = aux;
}

// redimensiona para arriba o para abajo, campo en 0 para no redimensionar
bool redimensionar(heap_t * heap,size_t dup,size_t div){
	
	int v_tam =  (int)vector_obtener_tamanio(heap->vec);
	size_t ocup = (OCUPADO*v_tam/100);
	size_t desocupado = (DESOCUPADO* v_tam/100);
	size_t heap_c = heap_cantidad(heap);
	
	
	if (div == 0){
	
		if ( heap_c >= ocup) 
			return 	vector_redimensionar(heap->vec, v_tam*dup);
		}

	if (dup == 0){
		 if ((v_tam > TAM_INICIAL) && (heap_c <= desocupado) ) 
			return	vector_redimensionar(heap->vec, v_tam/div);	
		}
	return true;
}



/*
 * Constructor alternativo del heap. Además de la función de comparación,
 * recibe un arreglo de valores con que inicializar el heap. Complejidad
 * O(n).
 *
 * Excepto por la complejidad, es equivalente a crear un heap vacío y encolar
 * los valores de uno en uno
*/
heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
	
	if (!cmp) return NULL;
	if (n == 0) return heap_crear(cmp);
	heap_t* heap = calloc(1, sizeof(heap_t));
	if (!heap) return NULL;
	
	vector_t* vector = vector_crear(n+1);
		if (!vector) return NULL;
				
		vector_guardar(vector,0,NULL);
		for (size_t i = 0; i<n;i++)
		vector_guardar(vector, i+1, arreglo[i]);
		
		
	heap->cmp = cmp;
	heap->cantidad = n;
	heap->vec = vector;
	
	for (size_t i = (n/2); i > 1; i--)
		downheap(heap->vec, n, i, cmp);
		
	return heap;
}

/* Elimina el heap, llamando a la función dada para cada elemento del mismo.
 * El puntero a la función puede ser NULL, en cuyo caso no se llamará.
 * Post: se llamó a la función indicada con cada elemento del heap. El heap
 * dejó de ser válido. */
void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	
	if (!heap) return;
	
	void * a_dst;
	while (!heap_esta_vacio(heap)){
		a_dst = heap_desencolar(heap);
		if (destruir_elemento) destruir_elemento(a_dst);
	}
	vector_destruir(heap->vec,NULL);
	free(heap);
	heap = NULL;
}

/* Devuelve la cantidad de elementos que hay en el heap. */
size_t heap_cantidad(const heap_t *heap){
	if(!heap) return 0;
	return  heap->cantidad;
}

/* Devuelve true si la cantidad de elementos que hay en el heap es 0, false en
 * caso contrario. */
bool heap_esta_vacio(const heap_t *heap){
	if(!heap) return true;
	return (heap->cantidad== 0);
}



void upheap(vector_t* vector, size_t pos, cmp_func_t cmp){
	size_t padre = pos/2;
	void* dato1 =	vector_obtener(vector, pos);
	void* dato2 = 	vector_obtener(vector, padre);
	int comp =0;
	if (dato1 && dato2) {
		comp = cmp(dato1, dato2);
		if (comp > 0) { 
			swap(vector->datos, pos, padre); 
			//printf("uphipiando");
			upheap(vector, padre, cmp);
		}
		else if (comp == 0) {
			vector_guardar(vector, pos, NULL);
		}
	}
}


/* Agrega un elemento al heap. El elemento no puede ser NULL.
 * Devuelve true si fue una operación exitosa, o false en caso de error.
 * Pre: el heap fue creado.
 * Post: se agregó un nuevo elemento al heap.
 */
bool heap_encolar(heap_t *heap, void *elem){
	
	if (!heap ) return false;
	
	heap->cantidad++;
	size_t pos =  heap->cantidad;
	
	bool guardado = vector_guardar(heap->vec,pos,elem);
	redimensionar(heap,DUP,0);
	if (pos>1 && guardado)upheap(heap->vec,heap->cantidad,heap->cmp);
	
	return guardado;
}

/* Devuelve el elemento con máxima prioridad. Si el heap esta vacío, devuelve
 * NULL.
 * Pre: el heap fue creado.
 */
void *heap_ver_max(const heap_t *heap){
	if (!heap) return NULL;
	return vector_obtener(heap->vec,1);
}

void downheap(vector_t* vector, size_t cant, size_t pos, cmp_func_t cmp) {
	if (pos > cant) return;
	
	size_t izq_pos = pos*2;
	size_t der_pos = pos*2 +1;
	size_t max = pos;
	void* dato1 = vector_obtener(vector, pos);
	void* dato2 = vector_obtener(vector,izq_pos);
	void* dato3 = vector_obtener(vector, der_pos);
	
	
	
	if (dato1 && dato2 && dato3) {
		if ( (izq_pos <= cant) && (cmp(dato1,dato2) < 0)) 
	 		max = izq_pos;
	 		
	 		dato1 = vector_obtener(vector, max);
	 		
	 		if ( (der_pos <= cant) && (cmp(dato1,dato3) < 0)) 
	 		max = der_pos;

		if (max != pos) {
			swap(vector->datos, pos, max);
			downheap(vector, cant, max, cmp);
		}
	}	

}


/* Elimina el elemento con máxima prioridad, y lo devuelve.
 * Si el heap esta vacío, devuelve NULL.
 * Pre: el heap fue creado.
 * Post: el elemento desencolado ya no se encuentra en el heap.
 */
void *heap_desencolar(heap_t *heap){
	
	if 	(!heap || heap_esta_vacio(heap)) return NULL;
	void* dato = NULL;
	size_t ultima = heap->cantidad;
	void* dato_ultima = NULL;
	
	dato = vector_obtener(heap->vec, 1);
	dato_ultima = vector_obtener(heap->vec,ultima);
	vector_guardar(heap->vec,1,dato_ultima);
	vector_guardar(heap->vec,ultima,NULL);
	heap->cantidad--;
	
	if (heap->cantidad > TAM_INICIAL)
	redimensionar(heap,0,DIV);
	
	if (heap->cantidad > 1) 
	downheap(heap->vec, heap->cantidad,1, heap->cmp);
	
	return dato;
	
}

void pruebas_heap_alumno(void);

