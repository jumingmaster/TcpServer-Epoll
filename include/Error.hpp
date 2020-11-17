#pragma once
#include <pthread.h>

class Error {

        friend void createKey (void);

public:
        enum Err 
        {
                ALLOCATE_ERROR = 1, 

        };        

        int *error;

        int getError (void) { return *error; }
        void setError (int err) { *error = err; }
        

private:
        static pthread_key_t key;
        static pthread_once_t key_once;
        void initError (void);
};