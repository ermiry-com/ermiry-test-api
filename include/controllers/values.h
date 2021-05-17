#ifndef _TEST_TRANSACTIONS_H_
#define _TEST_TRANSACTIONS_H_

#include <bson/bson.h>

#include <cerver/collections/pool.h>

#include "errors.h"

#include "models/value.h"
#include "models/user.h"

#define DEFAULT_VALUES_POOL_INIT			32

struct _HttpResponse;

extern Pool *values_pool;

extern const bson_t *value_no_user_query_opts;

extern struct _HttpResponse *no_user_values;
extern struct _HttpResponse *no_user_value;

extern struct _HttpResponse *value_created_success;
extern struct _HttpResponse *value_created_bad;
extern struct _HttpResponse *value_deleted_success;
extern struct _HttpResponse *value_deleted_bad;

extern unsigned int test_values_init (void);

extern void test_values_end (void);

extern unsigned int test_values_get_all_by_user (
	const bson_oid_t *user_oid,
	char **json, size_t *json_len
);

extern Value *test_value_get_by_id_and_user (
	const String *value_id, const bson_oid_t *user_oid
);

extern u8 test_value_get_by_id_and_user_to_json (
	const char *value_id, const bson_oid_t *user_oid,
	const bson_t *query_opts,
	char **json, size_t *json_len
);

extern TestError test_value_create (
	const User *user, const String *request_body
);

extern TestError test_value_update (
	const User *user, const String *value_id,
	const String *request_body
);

extern TestError test_value_delete (
	const User *user, const String *value_id
);

extern void test_value_return (void *value_ptr);

#endif