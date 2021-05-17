#ifndef _MODELS_VALUE_H_
#define _MODELS_VALUE_H_

#include <time.h>

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#include <cerver/types/types.h>

#define VALUES_COLL_NAME        	"values"

#define VALUE_ID_SIZE			32
#define VALUE_NAME_SIZE			256
#define VALUE_DATA_SIZE			512

extern unsigned int values_model_init (void);

extern void values_model_end (void);

typedef struct Value {

	// value's unique id
	bson_oid_t oid;
	char id[VALUE_ID_SIZE];

	// reference to the owner of this value
	bson_oid_t user_oid;

	char name[VALUE_NAME_SIZE];
	char data[VALUE_DATA_SIZE];

	// when the value was created
	time_t date;

} Value;

extern void *value_new (void);

extern void value_delete (void *value_ptr);

extern void value_print (Value *value);

extern bson_t *value_query_oid (const bson_oid_t *oid);

extern bson_t *value_query_by_oid_and_user (
	const bson_oid_t *oid, const bson_oid_t *user_oid
);

extern u8 value_get_by_oid (
	Value *value, const bson_oid_t *oid, const bson_t *query_opts
);

extern u8 value_get_by_oid_and_user (
	Value *value,
	const bson_oid_t *oid, const bson_oid_t *user_oid,
	const bson_t *query_opts
);

extern u8 value_get_by_oid_and_user_to_json (
	const bson_oid_t *oid, const bson_oid_t *user_oid,
	const bson_t *query_opts,
	char **json, size_t *json_len
);

// get all the values that are related to a user
extern mongoc_cursor_t *values_get_all_by_user (
	const bson_oid_t *user_oid, const bson_t *opts
);

extern unsigned int values_get_all_by_user_to_json (
	const bson_oid_t *user_oid, const bson_t *opts,
	char **json, size_t *json_len
);

extern unsigned int value_insert_one (const Value *value);

extern unsigned int value_update_one (const Value *value);

extern unsigned int value_delete_one_by_oid_and_user (
	const bson_oid_t *oid, const bson_oid_t *user_oid
);

#endif