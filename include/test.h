#ifndef _TEST_H_
#define _TEST_H_

#include <cerver/types/string.h>

#include "runtime.h"

#define DEFAULT_PORT					"5001"

struct _HttpResponse;

extern RuntimeType RUNTIME;

extern unsigned int PORT;

extern unsigned int CERVER_RECEIVE_BUFFER_SIZE;
extern unsigned int CERVER_TH_THREADS;
extern unsigned int CERVER_CONNECTION_QUEUE;

extern const String *PRIV_KEY;
extern const String *PUB_KEY;

extern struct _HttpResponse *oki_doki;
extern struct _HttpResponse *bad_request;
extern struct _HttpResponse *server_error;
extern struct _HttpResponse *bad_user;
extern struct _HttpResponse *missing_values;

extern struct _HttpResponse *test_works;
extern struct _HttpResponse *current_version;

// inits test main values
extern unsigned int test_init (void);

// ends test main values
extern unsigned int test_end (void);

#endif