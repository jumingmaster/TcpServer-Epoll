#include "Error.hpp"
#include <stdlib.h>

pthread_key_t Error::key;
pthread_once_t Error::key_once = PTHREAD_ONCE_INIT;


void createKey (void) {
        pthread_key_create (&Error::key, NULL);
}

void Error::initError (void) {

        pthread_once (&key_once, createKey);

        if ( (error == pthread_getspecific (key) ) == NULL) {

                error = (int *)malloc (sizeof (int));
                pthread_setspecific (key, error);
        }
}

