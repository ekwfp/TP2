#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stdlib.h>


//Tipos
typedef struct abb abb_t;
typedef struct abb_iter abb_iter_t;
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);



// Primitivas

// Crea un nuevo ABB. Recibe una funcion que compara las claves de los nodos,
// y otra que destruye el dato que contienen. 
// Post: da un puntero a un nuevo ABB. NULL si falla.
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guarda un dato en el ABB. Si no se encuentra la clave, se crea un nodo, sino
// reasigna el dato del nodo. False si falla.
// Pre: ABB.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra un nodo con la clave y devuelve el dato almacenado.
// Null si no existe.
// Pre: ABB.
void *abb_borrar(abb_t *arbol, const char *clave);

// Devuelve el dato almacenado en el nodo con la clave pasada.
// NULL si no existe.
// Pre: ABB.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Busca un nodo con la clave pasada. Informa si existe o no.
// Pre: ABB.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de nodos.
// Pre: ABB.
size_t abb_cantidad(abb_t *arbol);

// Destruye el ABB y los datos con la funcion.
// Pre: ABB.
// Post: ABB destruido.
void abb_destruir(abb_t *arbol);


// RECORRIDO INORDEN


// ITERADOR INTERNO

// Itera inorden usando visitar() en todos los nodos.
// Pre: ABB
// Post: Funcion visitar en los nodos.
void abb_in_order(abb_t *arbol, bool (*visitar)(const char *, void *, void *), void *extra);



//ITERADOR EXTERNO

// Crea el iterador.
// Pre: ABB
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza el iterador. False si no se pudo.
// Pre: Iterador.
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve clave actual o NULL si error.
// Pre: Iterador.
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Informa si el iterador se encuentra al final.
// Pre: Iterador
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador.
// Pre: Iterador.
void abb_iter_in_destruir(abb_iter_t* iter);


#endif
