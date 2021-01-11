#ifndef _MODELS_VALUE_H_
#define _MODELS_VALUE_H_

#include <time.h>

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#include <cerver/types/types.h>

#define	VALUE_ID_LEN			32
#define VALUE_NAME_LEN			256
#define VALUE_DATA_LEN			1024

extern mongoc_collection_t *values_collection;

// opens handle to value collection
extern unsigned int values_collection_get (void);

extern void values_collection_close (void);

typedef struct Value {

	// value's unique id
	bson_oid_t oid;
	char id[VALUE_ID_LEN];

	// reference to the owner of this value
	bson_oid_t user_oid;

	char name[VALUE_NAME_LEN];
	char data[VALUE_DATA_LEN];

	// when the value was created
	time_t date;

} Value;

extern void *value_new (void);

extern void value_delete (void *value_ptr);

extern void value_print (Value *value);

extern bson_t *value_query_oid (const bson_oid_t *oid);

extern const bson_t *value_find_by_oid (
	const bson_oid_t *oid, const bson_t *query_opts
);

extern u8 value_get_by_oid (
	Value *trans, const bson_oid_t *oid, const bson_t *query_opts
);

extern const bson_t *value_find_by_oid_and_user (
	const bson_oid_t *oid, const bson_oid_t *user_oid,
	const bson_t *query_opts
);

extern u8 value_get_by_oid_and_user (
	Value *trans,
	const bson_oid_t *oid, const bson_oid_t *user_oid,
	const bson_t *query_opts
);

extern bson_t *value_to_bson (Value *trans);

extern bson_t *value_update_bson (Value *trans);

// get all the values that are related to a user
extern mongoc_cursor_t *values_get_all_by_user (
	const bson_oid_t *user_oid, const bson_t *opts
);

#endif