#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdio.h>


// ******************************************************************
//                  DEFINICION DE LOS TIPOS DE DATOS
// ******************************************************************

// La lista está planteada como una lista de punteros genéricos.

struct lista;
typedef struct lista lista_t;
typedef struct lista_iter lista_iter_t;


// ******************************************************************
//                    PRIMITIVAS DE LA LISTA
// ******************************************************************


// Crea una lista.
// Post: Da una lista vacia.
lista_t *lista_crear(void);

// Devuelve verdadero si no tiene elementos, falso en caso contrario.
// Pre: La lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al comienzo de la lista. Si hay error devuelve false.
// Pre: La lista fue creada.
// Post: se agrego un nuevo elemento a la lista en la primera posicion.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Si hay error devuelve false.
// Pre: La lista fue creada.
// Post: se agrego un nuevo elemento a la lista en la ultima posicion.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Se devuelve el valor del primer elemento de la lista y se lo quita de la misma.
// Si la lista esta vacia devuelve NULL.
// Pre: la lista fue creada.
// Post: Se devuelve el elemento eliminado, o null si estaba vacia.
void *lista_borrar_primero(lista_t *lista);

// Devuelve null si esta vacia, si tiene elementos devuelve el primero sin quitarlo de la lista.
// Pre: La lista fue creada.
// Post: Devuelve el primer elemento cuando la lista no esta vacia. 
void *lista_ver_primero(const lista_t *lista);


// Devuelve null si esta vacia, si tiene elementos devuelve el primero sin quitarlo de la lista.
// Pre: La lista fue creada.
// Post: Devuelve el primer elemento cuando la lista no esta vacia. 
void *lista_ver_ultimo(const lista_t *lista);

// Devuelve la cantidad de elementos de la lista.
// Pre: La lista fue creada.
// Post:  Devuelve la cantidad de elementos.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la funcionn destruir_dato por parametro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una funcionn capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

// Destruye la lista dentro de una direccion de la tabla hash. Si se recibe la funcionn destruir_dato por parametro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una funcionn capaz de destruir
// los pares (valor, clave) de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los pares de elementos de la lista.
void lista_destruir_hash(lista_t *lista, void* hash, void destruir_dato(void *, void*));

// Deja la lista en estado vacio. Si se recibe la funcionn destruir_dato por parametro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una funcionn capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.

void lista_vaciar(lista_t *lista, void destruir_dato(void *));



// ******************************************************************
//                    PRIMITIVAS DE ITERADOR INTERNO
// ******************************************************************


// Itera a traves de la lista hasta que la funcion visitar devuelva false o el iterador llegue al final
// Pre: La lista fue creada. extra es un puntero apuntando a algo accesible.
void lista_iterar(lista_t *lista, bool (*visitar)(void *dato, void *extra), void *extra);


// ******************************************************************
//                    PRIMITIVAS DEL ITERADOR
// ******************************************************************




// Crea un iterador apuntando al inicio de la lista.
// Pre: Lista creada.
// Post: El iterador fue creado al inicio de la lista.
lista_iter_t *lista_iter_crear(lista_t *lista);



// El iterador avanza una posicion en la lista.
// Pre: Lista e iterador creados.
// Post: Devuelve true si se pudo avanzar o false si hubo un problema.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve el dato del nodo de la lista donde actualmente se encuentra el iterador.
// Pre: Lista e iterador creados.
// Post: Devuelve el puntero al dato.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve true si el iterador esta al final de la lista o false.
// Pre: Lista e iterador creados.
bool lista_iter_al_final(const lista_iter_t *iter);


// Destruye el iterador.
// Pre: Iterador creado.
// Post: Iterador destruido.
void lista_iter_destruir(lista_iter_t *iter);

// En la posicion en la que se encuentra el iterador se inserta un elemento en la lista.
// Pre: Lista e iterador creados.
// Post: Devuelve true si se pudo insertar el elemento, o false de haber un problema.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra de la lista el elemento al que apunta el iterador y devuelve su dato.
// Devuelve Null si estaba vacia.
// Pre: Lista e iterador creados.
// Post: El iterador queda apuntado a la siguiente posicion.
void *lista_iter_borrar(lista_iter_t *iter);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

void pruebas_lista_alumno(void);

#endif // LISTA_H
