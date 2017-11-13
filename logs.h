#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "vector_dinamico.h"
#include "lista.h"
#include "strutil.h"
#include "timeutil.h"

/*
Las lineas del log tienen estos campos:
		ip 
		fecha y hora
		metodo
		url

y estan ordenadas por fecha y hora

en el tp dice "no es necesario memorizar las entradas entre distintos archivos log"

*/

struct ip;
struct url;

typedef struct horario{
	int n_req_2s; // numero de peticiones en 2 segundos, si es 5 o mayor es DoS
	//vector_t* horarios; // se guarda temporalmente algunos horarios para buscar DoS
	time_t hora;
}horario_t;


typedef struct ip{
	char* ip_str; // la ip en string "192.168.0.1"   -> hara falta?
	horario_t* horario; 
}ip_t;

typedef struct url{
	char* ruta; 
	size_t cantidad;
}url_t;

//revisa la sintaxis de una ip en formato str
bool validar_ip(char * ip_str);

//detecta sobrecargas de solicitudes desde una ip
bool ip_es_dos(ip_t* ip, char* h_actual);


//recibe una ip en string y devuelve un puntero a una ip, se puede inicializar con un horario
ip_t* nueva_ip(char* ip_str, char* horario);

//recibe una ruta_str y crea una url_t
url_t * nueva_url(char* url_str);

//compara dos ips segun sus enteros
int cmp_ips (const void* ip1, const void * ip2);

//compara dos ips segun sus enteros
int cmp_ips_abb (const char* ip1, const char * ip2);

//compara dos ips segun sus enteros, inversa. (Para reporte DoS)
int cmp_ips_inversa (const void* ip1, const void * ip2);


//compara dos urls segun su campo cantidad
int cmp_url(const void* url1, const void* url2);

// destruye un url_t
void dest_url(void* url);
// destruye un ip_t
void dest_ip(void * ip);

#endif


