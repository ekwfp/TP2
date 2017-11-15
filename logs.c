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

size_t longitud_lista(lista_t* lista){
	lista_iter_t* iter = lista_iter_crear(lista);
	size_t longitud = 0;
	while (!lista_iter_al_final(iter)){
		longitud++;
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return longitud;
}

bool ip_es_dos(ip_t* ip, char* h_actual){

	//Con la funcion que teniamos antes, nos comiamos muchos casos, ponele que tenias un log con la ip en los segundos 1,2,2,3,3,3 La funcion que teniamos no tiraba dos
	//Lo que hice fue modicar la estructura ip, y le puse una lista. A esa lista la voy a ir llenando
	// con los strings de los horarios (en memoria dinamica).

	if (!ip) return false;
	time_t time_actual = char_a_time(h_actual); //creo el time_t para poder hacer el diff
	lista_t* horarios = ip->horarios;
	char* str_guardado = strdup(h_actual); //aca pido memoria para el string
	double interv = INTERVALO;
	while (!lista_esta_vacia(horarios)){ //itero hasta que la lista este vacia NOTA: Si entra en el while, y se vuelve vacia, significa que no pudo entrar en el if de la diferencia, entonces
		// significa que los horarios de antes no son "compatibles" con el nuevo horario (o sea, la diferencia de los horarios anteriores es mayor a 2)
		char* hora_str = (char*)(lista_ver_primero(horarios));
		time_t hora = char_a_time(hora_str); //creo el time_t para poder hacer el diff
		double diff  = difftime(time_actual,hora);
		if (diff < interv){
			lista_insertar_ultimo(horarios,str_guardado); //si estoy dentro del intervalo de segundos, inserto el str
			if(longitud_lista(horarios) >= MAX_INTENTOS){ //si llego a 5, devuelvo true
				return true; //si ya van 5 es dos
			}	
			return false;
		
		}
		free(lista_borrar_primero(horarios));
	}

	lista_insertar_primero(horarios,str_guardado); 

	return false;
}

/*horario_t * nuevo_horario(char* h_str){
	horario_t* horario = calloc(1,sizeof(horario_t));
	if(!horario) return NULL;
	char* hora = strdup(h_str);
	if(h_str){
	 	horario->hora = hora;
	 }
	return horario;
}
*/

//recibe una linea del log y la parte, se puede iniciar la lista de horarios con el segundo parametro
ip_t* nueva_ip(char* ip_str, char* horario){
	ip_t* ip = calloc(1,sizeof(ip_t));
	if(!ip) return NULL;
	ip->ip_str = strdup(ip_str);
	if(!ip->ip_str) {
		free(ip);
		return NULL;
	}
	lista_t* horarios = lista_crear();
	if (!horarios){
		free(ip->ip_str);
		free(ip);
		return NULL;
	}
	ip->horarios = horarios;
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
void destruir_horarios (void* horario){
	//horario_t* horario_p = (horario_t*)horario;
	//free(horario_p->hora);
	free(horario);
}
// destruye un ip_t
void dest_ip(void * ip){
	 if (!ip) return;
	 ip_t* temp = (ip_t*) ip;
	 free(temp->ip_str);
	 lista_destruir(temp->horarios,destruir_horarios);
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


