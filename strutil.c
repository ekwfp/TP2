#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_VECTORES 257


char* chartostring(char sep);

/*
 * Devuelve en un arreglo dinámico terminado en NULL con todos los subsegmentos
 * de ‘str’ separados por el carácter ‘sep’. Tanto el arreglo devuelto como las
 * cadenas que contiene son allocadas dinámicamente.
 *
 * Quien llama a la función toma responsabilidad de la memoria dinámica del
 * arreglo devuelto. La función devuelve NULL si falló alguna llamada a
 * malloc(), o si ‘sep’ es '\0'.
 */
 char** split(const char* str, char sep){
	
	if (sep == '\0') return NULL;
	char * aux = strdup(str); 
	if (aux == NULL ) return NULL;
	
	char * p = aux;
	char* coma= aux;
	char *sepa =  chartostring(sep);
	size_t vectores = 0;
	
	
	char ** tabla = calloc(MAX_VECTORES,sizeof(char*));
	 if (tabla == NULL) { 
	 	free(tabla);
	 	return NULL;
	 } 
	 
	 
	 while ((coma= strpbrk(p, sepa))!=0){ 
	 
	  *coma= '\0';
	 tabla[vectores++]= strdup(p); /* coma ahora es null (detras de null esta
	  el resto de la cadena), entonces copia hasta aca */
	 p= coma+1;
	 
	}
	//if (vectores==0) tabla[vectores++] = strdup("");else 
	tabla[vectores++]=strdup(p);
	
	tabla[vectores++]=NULL; // el char** tiene que terminar en null
	
	free(aux);	free(sepa); // str auxiliar y separador auxiliar
		
	/*size_t i=0;	
	while(tabla[i] != NULL){	
		 fprintf(stderr, "vector numero : %i , vale %s \n", i, tabla[i]);
		 i++;
		 }
	fprintf(stderr, "vector numero : %i , vale %s \n", i, tabla[i]);*/
		
	char ** tabla_aux = (char**)realloc(tabla, ((vectores+1) * sizeof(char*))); //limo los bordes del vector inicial
	if (tabla_aux == NULL) return NULL;
	
	tabla=tabla_aux;

	return tabla;
	
}

/* Recibe un char y devuelve un string de largo 1 con el char, el puntero debe liberarse.
Devuelve null si hubo algun problema*/
char* chartostring(char sep) {
	char *sepa = (char*) malloc (2); 
	if (sepa == NULL) {
		free(sepa); return NULL;
	}
	sepa[0]= sep;
	sepa[1]='\0';
	return sepa;
}

/*Libera un arreglo dinámico de cadenas, y todas las cadenas que contiene.*/
void free_strv(char* strv[]){
	
	size_t i = 0, final = 0;
	
	while (final == 0) {
	if(strv[i] == NULL) final= 1;
	free(strv[i]);
	i++;
	}
	free(strv);
}




