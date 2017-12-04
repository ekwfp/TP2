#define _POSIX_C_SOURCE 200809L 
#define _XOPEN_SOURCE 500

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "lista.h"
#include "vector_dinamico.h"
#include "heap.h"
#include "hash.h"
#include "abb.h"
#include "strutil.h"
#include "timeutil.h"
#include "servidor.h"

/*
main - funcionamiento:
	crea el servidor
	(*)lee stdin linea por linea
	la separa en comandos
	los valida, si !validos -> finaliza
	ejecuta(validado)
	avanza linea
	vuelve a (*)
	
Modificaciones a los TDA
	vector dinamico:
		- vector_obtener devuelve void*
	abb:
		- nueva primitiva: abb_inorden_intervalo
	heap:
		- nueva primitiva: heap_reorganizar	
*/

int main(int argc,char* argv[]){
	if (argc!= 1) {	fprintf(stderr,"Error en comando %s\n", argv[0]); return 1; }
    servidor_t* servidor = servidor_crear();//estructura principal
	if (servidor == NULL){ servidor_destruir(servidor); return 3; }//!memoria
    int exe = 1;
    char** vcomando = NULL; 	
	while (exe>0){ // mientras se ejecuten cosas validas
		char* linea = NULL;
		size_t capacidad = 0; ssize_t leidos; //combo getline
		leidos = getline(&linea,&capacidad,stdin); 
		if (leidos < 1){ //si EOF va a finalizar
			free(linea);
			break;
	    }
	 	corte_str(&linea);
		vcomando= split(linea,' '); //separo stdin
		if (vcomando==NULL) { //problema spliteando
        //	free_strv(vcomando);
        	free(linea);
        	break;
		}
      	exe = validar_comando(vcomando);
      	if(exe < 1) { //si no es valido
      		mensaje_error(vcomando[0]); //muestro mensaje
      		free_strv(vcomando);
			free(linea);
      		break; // voy a finalizar
		}
		ejecutar_comandos(servidor,vcomando, exe);	//el servidor, la lista de comandos, el numero de comando(devuelto por la validacion)
		free_strv(vcomando);
		free(linea);
	}
    servidor_destruir(servidor);	
	return 0;
}
