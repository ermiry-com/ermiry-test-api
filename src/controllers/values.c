#include <stdlib.h>

#include <time.h>

#include <cerver/types/string.h>

#include <cerver/collections/pool.h>

#include <cerver/http/json/json.h>

#include <cerver/utils/log.h>

#include <cmongo/crud.h>
#include <cmongo/select.h>

#include "controllers/values.h"

#include "models/value.h"

Pool *value_pool = NULL;

const bson_t *value_no_user_query_opts = NULL;
CMongoSelect *value_no_user_select = NULL;

void test_value_delete (void *value_ptr);

static unsigned int test_value_init_pool (void) {

	unsigned int retval = 1;

	value_pool = pool_create (value_delete);
	if (value_pool) {
		pool_set_create (value_pool, value_new);
		pool_set_produce_if_empty (value_pool, true);
		if (!pool_init (value_pool, value_new, DEFAULT_VALUES_POOL_INIT)) {
			retval = 0;
		}

		else {
			cerver_log_error ("Failed to init values pool!");
		}
	}

	else {
		cerver_log_error ("Failed to create values pool!");
	}

	return retval;

}

static unsigned int test_value_init_query_opts (void) {

	unsigned int retval = 1;

	value_no_user_select = cmongo_select_new ();

	(void) cmongo_select_insert_field (value_no_user_select, cmongo_select_field_create ("name"));
	(void) cmongo_select_insert_field (value_no_user_select, cmongo_select_field_create ("data"));
	(void) cmongo_select_insert_field (value_no_user_select, cmongo_select_field_create ("date"));

	value_no_user_query_opts = mongo_find_generate_opts (value_no_user_select);

	if (value_no_user_query_opts) retval = 0;

	return retval;

}

unsigned int test_value_init (void) {

	unsigned int errors = 0;

	errors |= test_value_init_pool ();

	errors |= test_value_init_query_opts ();

	return errors;

}

void test_value_end (void) {

	bson_destroy ((bson_t *) value_no_user_query_opts);

	pool_delete (value_pool);
	value_pool = NULL;

}

Value *test_value_get_by_id_and_user (
	const String *value_id, const bson_oid_t *user_oid
) {

	Value *value = NULL;

	if (value_id) {
		value = (Value *) pool_pop (value_pool);
		if (value) {
			bson_oid_init_from_string (&value->oid, value_id->str);

			if (value_get_by_oid_and_user (
				value,
				&value->oid, user_oid,
				NULL
			)) {
				test_value_delete (value);
				value = NULL;
			}
		}
	}

	return value;

}

Value *test_value_create (
	const char *user_id,
	const char *name,
	const char *data
) {

	Value *value = (Value *) pool_pop (value_pool);
	if (value) {
		bson_oid_init (&value->oid, NULL);

		bson_oid_init_from_string (&value->user_oid, user_id);

		if (name) (void) strncpy (value->name, name, VALUE_NAME_LEN - 1);
		if (data) (void) strncpy (value->data, data, VALUE_DATA_LEN - 1);

		value->date = time (NULL);
	}

	return value;

}

void test_value_delete (void *value_ptr) {

	(void) memset (value_ptr, 0, sizeof (Value));
	(void) pool_push (value_pool, value_ptr);

}