#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "vector_dinamico.h"
#include "lista.h"
#include "strutil.h"

struct ip;
struct url;

/*
Las lineas del log tienen estos campos:
		ip 
		fecha y hora
		metodo
		url

y estan ordenadas por fecha y hora

en el tp dice "no es necesario memorizar las entradas entre distintos archivos log"

*/

typedef struct horario{
	int n_req_2s; // numero de peticiones en 2 segundos, si es 5 o mayor es DoS
	//vector_t* horarios; // se guarda temporalmente algunos horarios para buscar DoS
	time_t** horarios;
}horario_t;


typedef struct ip{
	int** ip_int; // un vector de 4 enteros {192,168,0,1}
	char* ip_str; // la ip en string "192.168.0.1"   -> hara falta?
	horario_t* horario; 
}ip_t;

typedef struct url{
	char* ruta; 
	size_t * cantidad;
}url_t;

//revisa la sintaxis de una ip en formato str
bool validar_ip(char * ip_str);

bool ip_es_dos(ip_t* ip);


//recibe una linea del log y la parte, se puede iniciar la lista de horarios con el segundo parametro
ip_t* nueva_ip(char* ip_str, char* horario);

//recibe una ruta_str y crea una url_t
url_t * nueva_url(char* url_str);

//compara dos ips segun sus enteros
int cmp_ips (const char * ip1, const char * ip2);

//compara dos ips segun sus enteros, inversa. (Para reporte DoS)
int cmp_ips_inversa (const char * ip1, const char * ip2);


// destruye un ip_t
void dest_ip(void *);





#endif


