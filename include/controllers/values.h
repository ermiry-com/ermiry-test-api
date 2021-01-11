#ifndef _TEST_TRANSACTIONS_H_
#define _TEST_TRANSACTIONS_H_

#include <bson/bson.h>

#include <cerver/types/string.h>

#include <cerver/collections/pool.h>

#include <cmongo/select.h>

#include "models/value.h"

#define DEFAULT_VALUES_POOL_INIT			32

extern Pool *value_pool;

extern const bson_t *value_no_user_query_opts;
extern CMongoSelect *value_no_user_select;

extern unsigned int test_value_init (void);

extern void test_value_end (void);

extern Value *test_value_get_by_id_and_user (
	const String *value_id, const bson_oid_t *user_oid
);

extern Value *test_value_create (
	const char *user_id,
	const char *name,
	const char *data
);

extern void test_value_delete (void *value_ptr);

#endif