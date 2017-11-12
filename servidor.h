#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lista.h"
#include "vector_dinamico.h"
#include "logs.h"
#include "strutil.h"

typedef struct servidor{
	
	abb_t* abb_ips; // ips
	heap_t* mas_visitados;
	hash_t* hash_ip; // hash_ip_ocurrencias
	hash_t* hash_url; // hash_url_ocurrencias
	hash_t* hash_dos;// hash_esDoS
	bool iniciado;
	
}servidor_t;


//elimina el newline y/o el retroceso de carro de un str
void corte_str(char** str);

	
/*Crea la estructura principal del programa. En ella se cargan todas las peticiones de los logs*/
servidor_t * servidor_crear();

/*Valida un comando del programa ingresado por stdin, devuelve el numero de comando validado o su negativo
si hay fallas en sus parametros. 0 si no existe*/
int validar_comando(char** comando);

/*Ejecuta un comando del programa ( agregar_archivo x, ver_visitantes x y,
ver_mas_visitados x ). PRE: Debe ser valido en sintaxis*/
bool ejecutar_comandos(servidor_t * servidor,char** vcomandos, int a_ejecutar);

/*Destruye la estructura principal*/
bool servidor_destruir(servidor_t * servidor);

/*imprime OK por stdout*/
void oki(void);

/*Imprime que hubo un error en un comando del sistema, se especifica por parametro*/
void mensaje_error(char* comando);

#endif
