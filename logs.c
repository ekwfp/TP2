#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include "logs.h"



//revisa la sintaxis de una ip en formato str
bool validar_ip(char * ip_str){
	char** tmp_ip_str= split(ip_str,'.');
	if (!tmp_ip_str) return false;
	if(tmp_ip_str[4] != NULL) return false; // si hay mas de 4 vectores
	for(int i= 0; i<4; i++){
		if(!tmp_ip_str[i]) return false; //si hay menos de 4 vectores
		int val = atoi(tmp_ip_str[i]);
		if ((val <0) && (val > 255)) return false;
	}
	free_strv(tmp_ip_str);
	return true;
}


bool ip_es_dos(ip_t* ip){
	
	
}

//recibe una linea del log y la parte, se puede iniciar la lista de horarios con el segundo parametro
ip_t* nueva_ip(char* ip_str, char* horario){
	
	
	
	
}

//recibe una ruta_str y crea una url_t
url_t * nueva_url(char* url_str){
	
	
	
}

//compara dos ips segun sus enteros
int cmp_ips (const char * ip1, const char * ip2){
	
	
}

int cmp_ips_inversa (const char * ip1, const char * ip2){
	
}

// destruye un ip_t
void dest_ip(void *){
	
	
	
}



