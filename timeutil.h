#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <stddef.h>
#include <time.h>
#define TIME_FORMAT "%FT%T%z"

//Dada una cadena en formato ISO-8601 devuelve una variable de tipo time_t
//que representa un instante en el tiempo.
time_t char_a_time(const char* iso8601);

#endif  // TIMEUTIL_H
