#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include "timeutil.h"
#include <time.h>
#define TIME_FORMAT "%FT%T%z"


//Dada una cadena en formato ISO-8601 devuelve una variable de tipo time_t
//que representa un instante en el tiempo.
time_t char_a_time(const char* iso8601){
    struct tm bktime = { 0 };
    strptime(iso8601, TIME_FORMAT, &bktime);
    return mktime(&bktime);
}

//double difftime(time_t time1, time_t time0);
