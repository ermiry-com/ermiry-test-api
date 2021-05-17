#ifndef _TEST_H_
#define _TEST_H_

#include <stdbool.h>

#include <cerver/types/string.h>

#include "runtime.h"

#define DEFAULT_PORT					"5001"

struct _HttpCerver;

extern struct _HttpCerver *http_cerver;

extern RuntimeType RUNTIME;

extern unsigned int PORT;

extern unsigned int CERVER_RECEIVE_BUFFER_SIZE;
extern unsigned int CERVER_TH_THREADS;
extern unsigned int CERVER_CONNECTION_QUEUE;

extern const String *PRIV_KEY;
extern const String *PUB_KEY;

extern bool ENABLE_USERS_ROUTES;

// inits test main values
extern unsigned int test_init (void);

// ends test main values
extern unsigned int test_end (void);

#endif