#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include "servidor.h"
#define AGREGAR "agregar_archivo"
#define VISITANTES "ver_visitantes"
#define MAS_VISITADOS "ver_mas_visitados"

// servidor

//destruye las estructuras internas del servidor
void exit_servidor(int errores, servidor_t* srv, abb_t* abb, heap_t* heap, hash_t* url, hash_t* ip);

//vacia los hashes dos y los horarios de ip
void vaciar_hashes(servidor_t* servidor);

// Recibe un hash con ips y las imprime por pantalla en order creciente
void reportar_dos(hash_t* hash_dos);

// Recibe un heap y reordena sus prioridades
void actualizar_visitados(servidor_t* servidor);

// recibe un archivo por comando y lo procesa
bool agregar_archivo(servidor_t* servidor, char** vcomandos);

// muestra las ip que hicieron alguna peticion, dentro de un rango dado
bool ver_visitantes(servidor_t* servidor, char** vcomandos);

// muestra n urls mas visitadas
bool ver_mas_visitados(servidor_t* servidor, char** vcomandos);

//corta una linea ya sea LF o CRLF		
void corte_str(char** linea){
	char* corte = strchr(*linea, 10); //\n
	char* corte2 = strchr(*linea, 13);//\v
	if (corte) *corte=0;
	if (corte2) *corte2=0;
}

void mensaje_error(char* comando){
	fprintf(stderr, "Error en comando %s\n", comando);
}

void oki(void){
	fprintf(stdout, "OK\n");
}

//destruye las estructuras internas del servidor en caso de error al crear
void exit_servidor(int errores, servidor_t* srv, abb_t* abb, heap_t* heap, hash_t* url, hash_t* ip){
	switch(errores){
		case 5: hash_destruir(ip);
		case 4: hash_destruir(url);
		case 3: heap_destruir(heap,NULL);
		case 2: abb_destruir(abb);
		case 1: free(srv);
	}
}

//crea estructura vacia
servidor_t* servidor_crear(){
	//reservo memoria
	servidor_t * servidor=  (servidor_t*) calloc (1,sizeof(servidor_t));
	if (servidor==NULL) return NULL;
	abb_t* ips = abb_crear(&cmp_ips_abb, NULL);
	if (!ips){	
		exit_servidor(1, servidor, NULL,NULL,NULL,NULL); 
		return NULL; 
	}
	heap_t* mas_visitados = heap_crear(&cmp_url);
	if (!mas_visitados){	
		exit_servidor(2, servidor, ips,NULL,NULL,NULL);
		return NULL; 
	}
	hash_t* hash_url = hash_crear(&dest_url);
	if (!hash_url){
		exit_servidor(3, servidor, ips,mas_visitados,NULL,NULL);
	 	return NULL; 
	}
	hash_t* hash_ip = hash_crear(&dest_ip);
	if (!hash_ip){	
		exit_servidor(4, servidor, ips,mas_visitados,hash_url,NULL); 
		return NULL; 
	}
	hash_t* hash_dos = hash_crear(NULL);
	if (!hash_dos){	
		exit_servidor(5, servidor, ips,mas_visitados,hash_url,hash_ip);
		return NULL; 
	}
	//asigno
	servidor->abb_ips = ips;
	servidor->mas_visitados = mas_visitados;
	servidor->hash_url = hash_url;
	servidor->hash_ip = hash_ip;
	servidor->hash_dos = hash_dos;
	servidor->iniciado = false;
		
	return servidor;
}

int validar_comando(char** tmp_v){ 
	//devuelve el comando a ejecutar
	char* comando[] = {	AGREGAR, VISITANTES, MAS_VISITADOS};
	int j=0, i = 0;	
	if(tmp_v && tmp_v[0]!=NULL){
		int valido= -1;
		for (j=0; j<3; j++)			
			if ((strcmp (tmp_v[i], comando[j])) == 0) {valido= j; break;} //busca el comando a validar
		switch (valido){
			case 0: //caso de agregar 
			//	if ((tmp_v[2] == NULL) && (tmp_v[1]) ) return 1; // si hay un y solo un parametro
				if ((tmp_v[1] != NULL) && (tmp_v[2] == NULL)) return 1;
				return -1;
				
			case 1: //visitantes
				if ( (tmp_v[1] != NULL) && (tmp_v[2] != NULL) && (tmp_v[3] == NULL )){ // si hay dos y solo dos parametros
					bool ip1 = validar_ip(tmp_v[1]);
					bool ip2 = validar_ip(tmp_v[2]);
					if (ip1 && ip2) return 2;
				}
				return -2;
				
			case 2: //ver mas visitados
				if((tmp_v[1] != NULL) && (tmp_v[2] == NULL )){ // si hay un y solo un parametro
					int val = atoi(tmp_v[1]);
					if( val> 0 ) return 3; 
				}
				return -3;
				
			default: 
				return 0; 
		}
	} 
	return 0; 
}

bool ejecutar_comandos(servidor_t * servidor, char ** vcomandos, int a_ejecutar){  
//el servidor, la lista de comandos, el numero de comando(devuelto por la validacion)
	bool ok = true;
	switch (a_ejecutar){
		case 1: //agregar
			ok = agregar_archivo(servidor, vcomandos);
			break;
		case 2: //ver visitantes
			ok = ver_visitantes(servidor, vcomandos);
			oki();
			break;
		case 3: //ver mas visitados
			ok = ver_mas_visitados(servidor, vcomandos);
			oki();
			break;
		default: 
			ok = false;
	}
	return ok;
}

void servidor_destruir(servidor_t * servidor){
	if(!servidor) return;
	hash_destruir(servidor->hash_ip);
	hash_destruir(servidor->hash_dos);
	heap_destruir(servidor->mas_visitados,NULL);
	hash_destruir(servidor->hash_url);
	abb_destruir(servidor->abb_ips);
	free(servidor);
}

//////////////////////////////////////////////////////////////////////////////////
/////////// PROCESOS

/////////// AGREGAR_ARCHIVO

bool procesar_log(FILE * log, servidor_t* servidor){
	char* linea = NULL; size_t capacidad = 0; ssize_t leidos; //combo getline
	while((leidos = getline(&linea,&capacidad,log)) > 0){ // mientras lineas
		corte_str(&linea);
		char** campos = split(linea,'\t'); // separo la linea en 4 campos
    	ip_t* ip = NULL;
    	url_t * url = NULL;
    	char * ip_leida = campos[0]; // ip de la linea log
    	char * time_leido = campos [1]; // fecha y hora de la linea log
    	char * url_leida = campos[3]; // url de la linea log
    	if( !hash_pertenece(servidor->hash_dos,ip_leida)){ 
		// si no es DoS lo proceso, sino no hace falta porque 1) ya es dos => esta en el abb
	    	if (hash_pertenece(servidor->hash_ip, ip_leida)){ //si existe agrego horario 
	    		ip= hash_obtener(servidor->hash_ip, ip_leida);				
	    		if(ip_es_dos(ip, time_leido)){
	    			hash_guardar(servidor->hash_dos, ip_leida, NULL); // solo hace falta listar
	    		}
			}
			else{ // sino creo nueva ip
				ip = nueva_ip(ip_leida,time_leido);
				abb_guardar(servidor->abb_ips,ip_leida, ip); // guardo la ip, si ya existe no pasa nada.
				hash_guardar(servidor->hash_ip, ip_leida, ip); // la anoto en el hash
			}
		}
    	if (hash_pertenece(servidor->hash_url, url_leida)){ //si existe suma
    		url = hash_obtener(servidor->hash_url,url_leida);
    		url->cantidad++;
		} 
		else{ // sino crea con cantidad en 1  y la mete en el heap
		// esto permite asegurar que no se van a encolar repetidos en el heap
			url = nueva_url(url_leida);
	    	hash_guardar(servidor->hash_url,url_leida, url );
	    	heap_encolar(servidor->mas_visitados, url);
		}
        free_strv(campos); 
    }
	free(linea); 
	return true; 
}

//vacia los hashes dos y los horarios de ip
void vaciar_hashes(servidor_t* servidor){
	hash_iter_t* iter = hash_iter_crear(servidor->hash_ip);
	if(!iter) return;
	const char* ip_str = NULL;
	ip_t* ip_actual = NULL;
	while(!hash_iter_al_final(iter)){
		ip_str = hash_iter_ver_actual(iter);
		ip_actual = hash_obtener(servidor->hash_ip, ip_str);
		ip_actual->horario->n_req_2s = 0;
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	
	// reseteo del hash de DoS
	hash_destruir(servidor->hash_dos);
	hash_t * hash_dos = hash_crear(NULL);
	if (!hash_dos) return;
	servidor->hash_dos = hash_dos;
}


bool agregar_archivo(servidor_t * servidor, char** vcomandos){
	FILE * log = fopen(vcomandos[1], "rt");
	if (!log){
		mensaje_error(AGREGAR);
		return false;	
	}
	if (servidor->iniciado) vaciar_hashes(servidor); //vacio la lista de DoS del log anterior
	servidor->iniciado = true;
	if (!procesar_log(log, servidor)) return false;
	actualizar_visitados(servidor);
	reportar_dos(servidor->hash_dos);
	fclose(log);
	oki();
	return true; 
}

// Recibe un hash con ips y las imprime por pantalla en order creciente
void reportar_dos(hash_t* hash_dos){
	hash_iter_t* iter = hash_iter_crear(hash_dos);
	if (!iter) return;
	char* ip_actual = NULL;
	heap_t* heap = heap_crear(cmp_ips_inversa);
	if (!heap) return;
	while(!hash_iter_al_final(iter)){
		ip_actual = strdup(hash_iter_ver_actual(iter));
		heap_encolar(heap, ip_actual);
		hash_iter_avanzar(iter);
	}
	while(!heap_esta_vacio(heap)){
		char * imprimir = heap_desencolar(heap);
		fprintf(stdout,"DoS: %s\n", imprimir);
		free(imprimir);
	}
	hash_iter_destruir(iter);
	heap_destruir(heap, NULL);
}

/////////// VER MAS VISITADOS

void actualizar_visitados(servidor_t* servidor){
	if (!servidor) return;
	if (!servidor->mas_visitados) return;
	heap_reorganizar(servidor->mas_visitados);
}


bool ver_mas_visitados(servidor_t * servidor, char** vcomandos){
	if(!servidor || !servidor->mas_visitados) return false;
	lista_t * desencolados  = lista_crear();
	if(!desencolados) return false;
	size_t n = heap_cantidad(servidor->mas_visitados);
	int mostrar = atoi(vcomandos[1]);
	if (mostrar > n) mostrar = (int)n;
	char s2[] = "\u00E1";
	fprintf(stdout,"Sitios m%ss visitados:\n", s2);
	while(mostrar>0){
		url_t *url = (url_t*) heap_desencolar(servidor->mas_visitados);
		if(url == NULL){
			mostrar--;
			continue;
		}
		lista_insertar_ultimo(desencolados,url);
		fprintf(stdout,"\t%s - %zu\n", url->ruta, url->cantidad);
		mostrar--;
	}
	while(!lista_esta_vacia(desencolados)){
		heap_encolar(servidor->mas_visitados, lista_borrar_primero(desencolados));					
	}
	lista_destruir(desencolados,NULL);
	return true;
}

/////////// VER VISITANTES

void imprimir_ip(void* ip){
	if (!ip) return;
	char* ip_s = (char*) ip;
	fprintf(stdout,"\t%s\n", ip_s);
}

bool ver_visitantes(servidor_t * servidor, char** vcomandos){
	if(!servidor || !servidor->abb_ips) return false;
	fprintf(stdout,"Visitantes:\n");
 	abb_inorden_intervalo(servidor->abb_ips,vcomandos[1],vcomandos[2],&imprimir_ip);
 	return true;
}

