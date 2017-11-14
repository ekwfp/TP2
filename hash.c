#define _XOPEN_SOURCE 500
#include "lista.h"
#include "vector_dinamico.h"
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAM_INICIAL  1650
#define DIV 2
#define MAX 70



struct hash { // Abierto
	vector_t* direcciones;
	size_t c_elementos;
    size_t tamanio;
	hash_destruir_dato_t destruir;
};

typedef struct nodo_hash {
	char* llave;
	void* dato;
} par_t;

struct hash_iter {
	const hash_t* hash;
	size_t dir_actual;
	size_t iteradas;
    lista_iter_t* act;
};


size_t hashing(const char* llave, size_t tamanio) { 
    long hash, i;
   	unsigned char* llavee= (unsigned char*)llave;
    size_t len = strlen(llave);
    for(hash = i = 0; i<len; ++i){
        hash += llavee[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return (size_t)hash%tamanio;
}


//Redimensiona el vector del hash a tam. 
//Pre: hash creado.
//Post: hash con nuevo tam:
bool hash_redimensionar(hash_t* hash, size_t tam);

//Itera hasta la proxima lista desde I. Devuelve la direccion donde hay lista, o -1.
//Guarda en buscador la direccion de la lista encontrada.
//Pre: iterador de hash creado.
int  hash_buscarlista(hash_iter_t* iter,  lista_iter_t** buscador, size_t i);

//destruye un elemento de tipo par
void destruir_listado_hash(void * dato, void* hash);


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = calloc(1, sizeof(hash_t));
	if (hash == NULL)	return NULL;
	vector_t* direcciones = vector_crear(TAM_INICIAL);
	if (direcciones == NULL){
		free (hash);
		return NULL;
	}
	for(size_t i=0; i<TAM_INICIAL; i++) direcciones->datos[i]=NULL;
	hash->direcciones = direcciones;
	hash->c_elementos = 0;
    hash->tamanio = TAM_INICIAL;
	hash->destruir = destruir_dato;
	return hash;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->c_elementos;
}

// recibe un iterador de lista y una clave
//post: devuelve true si pertenece la clave, el iterador ahora apunta a la clave
bool lista_iter_en_clave(lista_iter_t* iter, const char * clave){
	par_t* par_act;
	while (!lista_iter_al_final(iter)){
		par_act = lista_iter_ver_actual(iter);
		if (strcmp(par_act->llave, clave) == 0)	return true;
		lista_iter_avanzar(iter);
	}
	return false;
}

//recibe un hash y una clave y devuelve un iterador de lista apuntando al par-dato si lo encuentra
// si no lo encuentra devuelve NULL
lista_iter_t* crear_lista_iter_en_clave(const hash_t* hash, const char * clave, size_t *direccion){
	if (hash_cantidad(hash) == 0) return NULL;
	size_t dir = hashing(clave, hash->tamanio);
	if (!hash->direcciones->datos[dir])	return NULL;
	lista_iter_t* iter = lista_iter_crear(hash->direcciones->datos[dir]);
	if (!iter) return NULL;
	if (lista_iter_en_clave(iter, clave)) {
		*direccion = dir;
		return iter;
	}
	lista_iter_destruir(iter);
	*direccion = 0;
	return NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t dir= 0;
	lista_iter_t* iter = crear_lista_iter_en_clave(hash,clave,&dir);
	if (!iter) return false;
	lista_iter_destruir(iter);
	return true;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	size_t dir = hashing( clave, hash->tamanio);
    par_t* n_elemento =calloc(1,sizeof(par_t));
	n_elemento->llave = strdup(clave);
	n_elemento->dato = dato;
	if (!hash->direcciones->datos[dir]){ //si cae en la direccion por 1ra vez se crea la lista y se enlista el elemento
		hash->direcciones->datos[dir]= lista_crear();
		if (lista_insertar_ultimo(hash->direcciones->datos[dir], n_elemento)){
			hash->c_elementos++;
            size_t usado = (100*hash->c_elementos)/hash->tamanio; // 70%->redimension
	        if(usado>= MAX ){
           		if(!hash_redimensionar(hash,hash->tamanio*DIV)) return false;
			}
			return true;
		} 
	} 
	else{ // si cae en  la misma direccion verificar 2 cosas: si tiene la misma clave, reemplazar dato. sino, enlistar.
		size_t direc = 0;
		lista_iter_t* iter = crear_lista_iter_en_clave(hash,clave,&direc);
		if (direc != 0){
			par_t* par_act = lista_iter_ver_actual(iter);
			if (hash->destruir != NULL)
					hash->destruir(par_act->dato);
			par_act->dato = dato;
			free(n_elemento->llave);
			free(n_elemento);
			lista_iter_destruir(iter);
			return true;
		}
		lista_iter_destruir(iter);
		if (lista_insertar_ultimo(hash->direcciones->datos[dir], n_elemento)){
			hash->c_elementos++;
            size_t usado = (100*hash->c_elementos)/hash->tamanio; //70% ->redimension
	        if(usado>= MAX ){
                if(!hash_redimensionar(hash,hash->tamanio*DIV)) return false;
        	}
			return true;
		} 
	}
	free(n_elemento->llave);
	free(n_elemento);
	return false;
}


void *hash_borrar(hash_t *hash, const char *clave){
	size_t dir = 0;
	lista_iter_t* iter = crear_lista_iter_en_clave(hash,clave,&dir);
	if (!iter) return NULL;
	par_t* par = lista_iter_borrar(iter);
	lista_iter_destruir(iter);
	void* dato = par->dato;
	free(par->llave);
	free(par);
	hash->c_elementos--;
	if (lista_esta_vacia(hash->direcciones->datos[dir])){
		lista_destruir(hash->direcciones->datos[dir], NULL);
		hash->direcciones->datos[dir] = NULL;
	}
	if( (hash->tamanio) < (hash->c_elementos / (DIV*DIV)) ) // 25% -> redimension
		if( !hash_redimensionar(hash, hash->tamanio / DIV) ) return NULL;     
    return dato;      
}

void *hash_obtener(const hash_t *hash, const char *clave){
	void* dato = NULL;
	size_t dir= 0;
	lista_iter_t* iter = crear_lista_iter_en_clave(hash,clave,&dir);
	if (!iter) return NULL;
	dato =((par_t*)lista_iter_ver_actual(iter))->dato;
	lista_iter_destruir(iter);
	return dato;
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iter = calloc(1,sizeof(hash_iter_t));
	if (iter == NULL)	return NULL;
	iter->hash = hash;
	iter->act = NULL;
	iter->dir_actual = 0;
	iter->iteradas = 0;
	if (iter->hash->c_elementos != 0){		
		lista_iter_t* buscador = NULL;
		int i = hash_buscarlista(iter, &buscador, 0);
		if (i != -1){
	        iter->act = buscador;
			iter->dir_actual = (size_t)i;
			iter->iteradas++;
		} 
		else{
			free(iter);
			return NULL;
		}
	}
	return iter;
}

int hash_buscarlista(hash_iter_t* iter,  lista_iter_t** buscador, size_t i){
	size_t j = i;
	while ( ( *buscador == NULL ) && ( j < ( iter->hash->direcciones->tam ) ) ) {
		if (!iter->hash->direcciones->datos[j]){
            j++; 
			continue;
        }
		*buscador = lista_iter_crear(iter->hash->direcciones->datos[j]);
		if (*buscador == NULL)	return -1;
		if (lista_iter_al_final(*buscador)) {
			lista_iter_destruir(*buscador);
			*buscador = NULL;
		}
    }
	return (int)j;
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if (hash_iter_al_final(iter))	return false;
	if (lista_iter_al_final(iter->act)) {
		lista_iter_t* buscador = NULL;
		int i = hash_buscarlista(iter, &buscador, iter->dir_actual + 1);
		if (i != -1) {
			lista_iter_destruir(iter->act);
			iter->act = buscador;
			iter->dir_actual = (size_t)i;
			return true;
		} 
			return false;
	}
	if (lista_iter_avanzar(iter->act)){
		iter->iteradas++;
		if (lista_iter_al_final(iter->act)) {
			lista_iter_t* buscador = NULL;
			int i = hash_buscarlista(iter, &buscador, iter->dir_actual + 1);
			if (i != -1){
				lista_iter_destruir(iter->act);
				iter->act = buscador;
				iter->dir_actual = (size_t)i;
				return true;
			} 
			return false;
		} 
		return true;
	} 
	return false;
}

const char * hash_iter_ver_actual(const hash_iter_t *iter){
	if (!hash_iter_al_final(iter)) return ((par_t*)lista_iter_ver_actual(iter->act))->llave; 
    return NULL;
}

bool hash_iter_al_final(const hash_iter_t *iter){
	if ((iter->hash->c_elementos < iter->iteradas) || (iter->hash->c_elementos == 0)) return true;
	return false;
}

void hash_iter_destruir(hash_iter_t* iter){
	if (iter->act)	lista_iter_destruir(iter->act);
	free(iter);
}

void destruir_listado_hash(void * dato, void* hash){
	hash_t* hash_p = (hash_t*) hash;
	par_t* acceso_dato= (par_t*) dato;
	free(acceso_dato->llave);
	if (hash_p->destruir != NULL) hash_p->destruir(acceso_dato->dato);
	free(acceso_dato);
}

void hash_destruir(hash_t *hash){
	size_t i = 0;
	while ( i < hash->direcciones->tam) {
		if (hash->direcciones->datos[i]){
		 	lista_destruir_hash(hash->direcciones->datos[i], hash, &destruir_listado_hash); 
		}
		i++;
	}
	vector_destruir(hash->direcciones,NULL);
	free(hash);
}

bool hash_redimensionar(hash_t* hash, size_t tam){
	if (hash->c_elementos == 0)	return vector_redimensionar(hash->direcciones, tam);
	lista_t* lista_elementos = lista_crear();
    if (!lista_elementos) return false;
	hash_iter_t* iter = hash_iter_crear(hash);
    if (!iter) return false;
    size_t nueva_dir; //rehasheo
	size_t dir = iter->dir_actual;
	lista_iter_t* lista_iter = iter->act;
	lista_t* lista_newdir = NULL;
	par_t* par_act = NULL;
	while (!hash_iter_al_final(iter)){ //guarda temportalmente todos los elementos
		if (iter->act)	lista_insertar_ultimo(lista_elementos, lista_iter_ver_actual(iter->act));
		hash_iter_avanzar(iter);
		if (iter->act != lista_iter){
			lista_destruir(hash->direcciones->datos[dir], NULL);
			dir = iter->dir_actual;
			lista_iter = iter->act;
		}
	}
	hash_iter_destruir(iter);
	vector_redimensionar(hash->direcciones, tam);
    hash->tamanio= tam;
	for (size_t i=0; i<tam; i++)	hash->direcciones->datos[i] = NULL; // re-inicializa el vector
	while (!lista_esta_vacia(lista_elementos)){
		par_act = lista_borrar_primero(lista_elementos); //paso elemento al vector
		nueva_dir = hashing( par_act->llave, tam ); // reasigno direccion
		lista_newdir = hash->direcciones->datos[nueva_dir];
		if (!lista_newdir){
			lista_newdir = lista_crear();
			hash->direcciones->datos[nueva_dir] = lista_newdir;
		}
		lista_insertar_ultimo(lista_newdir, par_act);
	}
	lista_destruir(lista_elementos, NULL); //tmp
	return true;
}



