#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include "logs.h"
#include "timeutil.h"
#define MAX_INTENTOS 5
#define INTERVALO 2



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

bool ip_es_dos(ip_t* ip, char* h_actual){
	if (!ip) return false;
	horario_t * horas = ip->horario;
	time_t time_actual = char_a_time(h_actual);
	
	if( horas->n_req_2s == 0) {
		ip->horario->hora = time_actual;
		ip->horario->n_req_2s++;
		return false;
	}
	double diff  = difftime(time_actual,horas->hora);
	double interv = INTERVALO;
	//si la diferencia entre la primer hora es <2'' 
	if (diff < interv) {
		ip->horario->n_req_2s++; 
		if(ip->horario->n_req_2s >= MAX_INTENTOS) return true; //si ya van 5 es dos
		return false;
	}
	//descarto posibilidad 
	ip->horario->hora = time_actual;
	ip->horario->n_req_2s = 1;
	return false;
}

horario_t * nuevo_horario(char* h_str){
	horario_t* horario = calloc(1,sizeof(horario_t));
	if(!horario) return NULL;
	horario->n_req_2s = 0;
	if(h_str){
	 	horario->hora = char_a_time(h_str);
	 	horario->n_req_2s++;
	 }
	return horario;
}


//recibe una linea del log y la parte, se puede iniciar la lista de horarios con el segundo parametro
ip_t* nueva_ip(char* ip_str, char* horario){
	ip_t* ip = calloc(1,sizeof(ip_t));
	if(!ip) return NULL;
	ip->ip_str = strdup(ip_str);
	if(!ip->ip_str) {
		free(ip);
		return NULL;
	}
	ip->horario = nuevo_horario(horario);
	if(!horario) {
		free(ip->ip_str);
		free(ip); 
		return NULL;
	}
	return ip;
}

//recibe una ruta_str y crea una url_t
url_t * nueva_url(char* url_str){
	char * dir = strdup(url_str);
	if(!dir) return NULL;
	url_t* url = calloc(1,sizeof(url_t));
	if(!url) {
		free(dir);
		return NULL;	
	}
	url->ruta = dir;
	url->cantidad = 1;
	return url;
}

//compara dos ips segun sus enteros
int cmp_ips_abb (const char * ip1, const char * ip2){
	char** ipn1 = split(ip1,'.');
	char** ipn2 = split(ip2,'.');
	for (int i = 0; i < 4; i++) {
		if ( atoi(ipn1[i]) > atoi(ipn2[i]) ) {
			free_strv(ipn1);
			free_strv(ipn2);
			return 1;
		}
		if ( atoi(ipn1[i]) < atoi(ipn2[i]) ) {
			free_strv(ipn1);
			free_strv(ipn2);
			return -1;
		}
	}
	free_strv(ipn1);
	free_strv(ipn2);
	return 0;
}

int cmp_ips (const void * ip1, const void * ip2){
	char** ipn1 = split((char*)ip1,'.');
	char** ipn2 = split((char*)ip2,'.');
	for (int i = 0; i < 4; i++) {
		if ( atoi(ipn1[i]) > atoi(ipn2[i]) ) {
			free_strv(ipn1);
			free_strv(ipn2);
			return 1;
		}
		if ( atoi(ipn1[i]) < atoi(ipn2[i]) ) {
			free_strv(ipn1);
			free_strv(ipn2);
			return -1;
		}
	}
	free_strv(ipn1);
	free_strv(ipn2);
	return 0;
}

int cmp_ips_inversa (const void * ip1, const void * ip2){
	char** ipn1 = split((char*)ip1,'.');
	char** ipn2 = split((char*)ip2,'.');
	for (int i = 0; i < 4; i++) {
		if ( atoi(ipn1[i]) > atoi(ipn2[i]) ) {
			free_strv(ipn1);
			free_strv(ipn2);
			return -1;
		}
		if ( atoi(ipn1[i]) < atoi(ipn2[i]) ) {
			free_strv(ipn1);
			free_strv(ipn2);
			return 1;
		}
	}
	free_strv(ipn1);
	free_strv(ipn2);
	return 0;
}

// destruye un ip_t
void dest_ip(void * ip){
	 if (!ip) return;
	 ip_t* temp = (ip_t*) ip;
	 free(temp->ip_str);
	 free(temp->horario);
	 free(temp);
}

int cmp_url(const void* url1, const void* url2){
	url_t* temp1 = (url_t*) url1;
	url_t* temp2 = (url_t*) url2;
	if ( (!temp2) || ( temp1->cantidad > temp2->cantidad))  	return 1;
	if ( temp1->cantidad < temp2->cantidad) 	return -1;
	return 0;
}

void dest_url(void* url){
	if (!url) return;
	url_t* temp = (url_t*) url;
	free(temp->ruta);
	free(temp);
}


