#define _XOPEN_SOURCE 500
#include "abb.h"
#include "lista.h"
#include <stdlib.h>
#define _SVID_SOURCE
#include <string.h>

#define apilar(lista,dato) lista_insertar_primero(lista,dato)
#define desapilar(lista) lista_borrar_primero(lista)

struct abb {
	abb_t* izq;
	abb_t* der;
	char* clave;
	void* dato;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir_dato;
};


abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	if (!cmp) return NULL;
	abb_t* arbol = calloc(1,sizeof(abb_t));
	if (!arbol) return NULL;
	arbol->cmp = cmp;
	arbol->destruir_dato = destruir_dato;
	arbol->clave = NULL;
	arbol->dato = NULL;
	arbol->izq = NULL;
	arbol->der = NULL;
	return arbol;
}

bool crear_abb_nodo(abb_t** arbol,const char *clave, void *dato, abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	
	
	*arbol = abb_crear(cmp,destruir_dato);
	if(!(*arbol)) return false;
	(*arbol)->clave = strdup(clave);
	if (!(*arbol)->clave) return false;
	(*arbol)->dato = dato;
	return true;	
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
	if (!arbol) return false;
	if (arbol != NULL && !arbol->clave){
		arbol->clave = strdup(clave);
		arbol->dato = dato;
	}
	else {
		int cmp = arbol->cmp(arbol->clave,clave);
		if (cmp == 0 ) {
			if(arbol->destruir_dato) arbol->destruir_dato(arbol->dato);
			arbol->dato = dato;
			return true;
		} 
		if (cmp > 0){
			if (arbol->izq) return abb_guardar(arbol->izq,clave,dato);
			return crear_abb_nodo(&arbol->izq, clave, dato, arbol->cmp, arbol->destruir_dato);
		}
			if (arbol->der)	return abb_guardar(arbol->der,clave,dato);
			return crear_abb_nodo(&arbol->der, clave, dato, arbol->cmp, arbol->destruir_dato);
	}
	return true;
}

void *abb_obtener(const abb_t *arbol, const char *clave){
	if (!arbol || !arbol->clave) return NULL;
	int cmp = arbol->cmp(arbol->clave,clave);
	if (cmp == 0 )	return arbol->dato;
	if (cmp > 0){
		if (arbol->izq) {
			return abb_obtener(arbol->izq,clave);
		}
			return NULL;
	}
		if (arbol->der){
			return abb_obtener(arbol->der,clave);
		}
			return NULL;
}

bool abb_pertenece(const abb_t *arbol, const char *clave){
	if (arbol == NULL) return false;
	if (arbol->clave == NULL) return false;
	int cmp = arbol->cmp(arbol->clave,clave);
	if (cmp == 0 )	return true;
	if (cmp > 0){
		if (arbol->izq) return abb_pertenece(arbol->izq,clave);
		return false;
	}
	if (arbol->der)	return abb_pertenece(arbol->der,clave);
	return false;
}

size_t abb_cantidad(abb_t *arbol){
	if (arbol == NULL || !arbol->clave)	return 0;
	return abb_cantidad(arbol->izq) + abb_cantidad(arbol->der) +1;
}



//busca el nodo mas chico o el mas grande
abb_t* abb_min_max(abb_t* min, abb_t* max){
	if(min){
		if (!min->izq) return min;
		return abb_min_max(min->izq,NULL);
	}
	if(max){
		if (!max->der) return max;
		return abb_min_max(NULL,max->der);
	}
	return NULL;
}

void _abb_borrar(abb_t *arbol, const char *clave, void** dato){
	if (!arbol || !arbol->clave) {*dato = NULL; return;}
	void* swap_dato = NULL;
	abb_t* swap;
	char* swap_clave;
	int cmp = (arbol->cmp(clave, arbol->clave));

 	if (cmp == 0){
		*dato = arbol->dato;
			
		//if (!arbol->izq && !arbol->der && arbol->clave){
		if ((!arbol->izq && !arbol->der && arbol->clave) || ((arbol->izq && !arbol->izq->clave) && (arbol->der && !arbol->der->clave))){
		//if (abb_cantidad(arbol) == 1){
			arbol->dato=NULL; 
			free(arbol->clave);
			arbol->clave = NULL;
			if (arbol->izq != NULL){
				abb_destruir(arbol->izq);
				arbol->izq = NULL;
			}
			if (arbol->der != NULL){
				abb_destruir(arbol->der);
				arbol->der = NULL;
			}	
		} 
		else{
			if (arbol->izq && arbol->izq->clave) {
				swap = abb_min_max(NULL,arbol->izq);
					if (!swap->clave) {*dato = NULL; return;}
				
				swap_clave = strdup(swap->clave);
				
				_abb_borrar(arbol->izq, swap->clave, &swap_dato);
				if (!arbol->izq->clave){
					abb_destruir(arbol->izq);
					arbol->izq = NULL;
				}
			} if (arbol->der && arbol->der->clave) {
				swap = abb_min_max(arbol->der,NULL);
				if (!swap->clave){*dato = NULL; return;}
				swap_clave = strdup(swap->clave);
				_abb_borrar(arbol->der, swap->clave, &swap_dato);
				if (!arbol->der->clave){
				//if (abb_cantidad(arbol->der) == 0){
					abb_destruir(arbol->der);
					arbol->der = NULL;
				}
			}
			free(arbol->clave);
			arbol->clave = swap_clave;
			arbol->dato = swap_dato;
		}
	} 
	else if (cmp < 0){
        if (arbol->izq)
        _abb_borrar(arbol->izq,clave, &(*dato));
        else {*dato = NULL; return;}
    }
	else{
        if (arbol->der) 
        _abb_borrar(arbol->der,clave,&(*dato));
        else {*dato = NULL; return;}
    }
	return;
}


void *abb_borrar(abb_t *arbol, const char *clave){
	if(!arbol || !arbol->clave) return NULL;
	void * dato = NULL;
	_abb_borrar(arbol,clave,&dato);
	return dato;
}

void abb_destruir(abb_t *arbol){
	if (!arbol) return;
	abb_destruir(arbol->izq);
	abb_destruir(arbol->der);
	if (arbol->destruir_dato)	arbol->destruir_dato(arbol->dato);
	if (arbol->clave)
	free(arbol->clave);
	free(arbol);
}


//ITERADOR INTERNO
void abb_in_order(abb_t * arbol, bool (*visitar)(const char *, void *, void *), void *extra){
	if (arbol == NULL) return; 
	if (visitar == NULL) return;
	abb_in_order(arbol->izq,visitar,extra);
	if(!visitar(arbol->clave,arbol->dato,extra)) return;
	abb_in_order(arbol->der,visitar,extra);	
}

//ITERADOR EXTERNO
void listar_izquierdos(const abb_t * arbol, lista_t* iter);

struct abb_iter{
	lista_t* pila; 
};


void listar_izquierdos(const abb_t * arbol, lista_t* iter){
	if (arbol == NULL) return;
	if(arbol->izq == NULL) return;
	apilar(iter,(void*)arbol->izq);
	listar_izquierdos(arbol->izq, iter);	
}

//pre: arbol creado
abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	//if  (arbol){
	abb_iter_t* abb_iter = calloc(1,sizeof(abb_iter_t));
	if (!abb_iter)  return NULL;
	lista_t* pila = lista_crear();
	if (!pila) return NULL;
	abb_iter->pila = pila;
	if(arbol->clave) {
		//lista_insertar_primero(pila, arbol);
		apilar(pila,(void*)arbol);
		listar_izquierdos(arbol, abb_iter->pila);
	}
	return abb_iter;
//	}
}

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter)) return false;
	abb_t* desapilado = (abb_t*)desapilar(iter->pila);
	if (!desapilado) return false; 
	if (desapilado->der) {
		apilar(iter->pila, desapilado->der);
		listar_izquierdos(desapilado->der,iter->pila);
	}
	return true;
}


const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (abb_iter_in_al_final(iter))  return NULL;
	return ((abb_t*)lista_ver_primero(iter->pila))->clave;	
}

bool abb_iter_in_al_final(const abb_iter_t *iter){
	return lista_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
	lista_destruir(iter->pila,NULL);
	free(iter);
}
