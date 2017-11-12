#include "vector_dinamico.h"


// Funciones del alumno.






void* vector_obtener(vector_t* vector, size_t pos){
	
	 if( pos >= vector->tam) return NULL;
    return vector->datos[pos];

}

bool vector_guardar(vector_t* vector, size_t pos, void* valor){
	
	if (vector -> tam <= pos) return false;
	
	vector -> datos[pos] = valor;
	
	return true;
}

size_t vector_obtener_tamanio(vector_t* vector){
	return vector -> tam;
}

void vector_destruir(vector_t* vector, void destruir_dato(void*)){
	
	
	if( !vector )
		return;
	
	size_t tam= vector_obtener_tamanio(vector);
	
	if (destruir_dato != NULL){
	
		for(size_t i=0; i<tam; i++){
			destruir_dato(vector->datos[i]);
		}
	}
	free(vector->datos);
	free(vector);

}

// ...


// Funciones implementadas por la catedra.

vector_t* vector_crear(size_t tam) {
    vector_t* vector = malloc(sizeof(vector_t));

    if (vector == NULL) {
        return NULL;
    }
    vector->datos = malloc(tam * sizeof(void*));

    if (tam > 0 && vector->datos == NULL) {
        free(vector);
        return NULL;
    }
    vector->tam = tam;
    return vector;
}

bool vector_redimensionar(vector_t* vector, size_t tam_nuevo) {
    void** datos_nuevo = realloc(vector->datos, tam_nuevo * sizeof(void*));

    // Cuando tam_nuevo es 0, es correcto si se devuelve NULL.
    // En toda otra situación significa que falló el realloc.
    if (tam_nuevo > 0 && datos_nuevo == NULL) {
        return false;
    }

    vector->datos = datos_nuevo;
    vector->tam = tam_nuevo;
    return true;
}


