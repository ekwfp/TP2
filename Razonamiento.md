# TP2

## SERVIDOR

- La estructura principal es un abb donde se guardan las ip usando una funcion que compare sus enteros.

- Un hash_url con las claves = urls, y datos = cantidad de ocurrencias

- Un hash_ip con claves = ips, y datos = la ip_str y una sub_struct (horario) que consiste en un time_t y un contador de peticiones

- Un hash_esDoS donde se anotan las ips que son DoS.

- Un heap donde van las urls mas visitadas.



> La consigna dice que no hay que memorizar los DoS de los logs anteriores, asi que el hash_esDoS y el hash_ip_ocurrencias hay que vaciarlo por cada Agregar_archivo(log), también se podría crear otro servidor, pero tal vez traiga problemas de memoria


## IPS

Las lineas del log tienen estos campos:
	- ip 
	- fecha y hora
	- metodo
	- url

y estan ordenadas por fecha y hora


## PROCESOS Y LÓGICA


### Carga de archivo

1. Para cada linea del .log  (sabiendo que son todas validas)
   - Separar con `split('\t')` (en tabulaciones) 
   - Hacer un conteo de solicitudes para cada ip (en un hash)
   	
   		- >usar funciones de `timeutil` para calcular las diferencias, se calcula con corte de control, ya que se ingresan cronológicamente
   		- >si la ip ya pertenecía al hash no calcular tiempos
   		- Si es DoS (5 peticiones en menos de 2 segundos) guardar esa ip en el hash_esDoS

2. Vaciar/Rehacer hashes antes de cargar otro archivo
	

### Informar ataques DoS

1. Iterar todo el hash_esDoS
2. Encolar cada ip en un a)vector , b) heap
3. a) Heapsort, b) desencolar(minimos) || ir imprimiendo DoS <ip>


### Urls y más visitados

1. Estructura url_t pensada temporal: 
   - int cantidad
   - char * url
2. Mantener las urls en un heap de maximos
3. Desencolar n elementos pedidos, imprimirlos, encolarlos nuevamente
	- Este heap hay que mantenerlo hasta el final del programa
	
4. Implementar una forma de actualizarlo cada vez que se procese un archivo
	- Estaba pensando en encolar las nuevas urls(usando pertenece del hash), y usar una nueva primitiva `reorganizar(heap)` o algo asi 
	- >seguramente hay una forma mas facil

### Ver visitantes
1. Iterar Inorder (desde, hasta) el abb principal, imprimiendo las ips
   - Fede compartió un pseudocodigo de esto en el grupo creo, hay que ponerlo dentro del tda abb 
   
