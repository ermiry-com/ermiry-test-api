#include <stdlib.h>

#include <time.h>

#include <cerver/types/string.h>

#include <cerver/http/response.h>
#include <cerver/http/json/json.h>

#include <cerver/collections/pool.h>

#include <cerver/utils/log.h>

#include <cmongo/crud.h>
#include <cmongo/select.h>

#include "errors.h"

#include "models/value.h"
#include "models/user.h"

#include "controllers/values.h"

Pool *values_pool = NULL;

const bson_t *value_no_user_query_opts = NULL;
static CMongoSelect *value_no_user_select = NULL;

HttpResponse *no_user_values = NULL;
HttpResponse *no_user_value = NULL;

HttpResponse *value_created_success = NULL;
HttpResponse *value_created_bad = NULL;
HttpResponse *value_deleted_success = NULL;
HttpResponse *value_deleted_bad = NULL;

void test_value_return (void *value_ptr);

static unsigned int test_values_init_pool (void) {

	unsigned int retval = 1;

	values_pool = pool_create (value_delete);
	if (values_pool) {
		pool_set_create (values_pool, value_new);
		pool_set_produce_if_empty (values_pool, true);
		if (!pool_init (values_pool, value_new, DEFAULT_VALUES_POOL_INIT)) {
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

static unsigned int test_values_init_query_opts (void) {

	unsigned int retval = 1;

	value_no_user_select = cmongo_select_new ();
	(void) cmongo_select_insert_field (value_no_user_select, "name");
	(void) cmongo_select_insert_field (value_no_user_select, "data");
	(void) cmongo_select_insert_field (value_no_user_select, "color");
	(void) cmongo_select_insert_field (value_no_user_select, "date");

	value_no_user_query_opts = mongo_find_generate_opts (value_no_user_select);

	if (value_no_user_query_opts) retval = 0;

	return retval;

}

static unsigned int test_values_init_responses (void) {

	unsigned int retval = 1;

	no_user_values = http_response_json_key_value (
		HTTP_STATUS_NOT_FOUND, "msg", "Failed to get user's values"
	);

	no_user_value = http_response_json_key_value (
		HTTP_STATUS_NOT_FOUND, "msg", "User's value was not found"
	);

	value_created_success = http_response_json_key_value (
		HTTP_STATUS_OK, "oki", "doki"
	);

	value_created_bad = http_response_json_key_value (
		HTTP_STATUS_BAD_REQUEST, "error", "Failed to create value!"
	);

	value_deleted_success = http_response_json_key_value (
		HTTP_STATUS_OK, "oki", "doki"
	);

	value_deleted_bad = http_response_json_key_value (
		HTTP_STATUS_BAD_REQUEST, "error", "Failed to delete value!"
	);

	if (
		no_user_values && no_user_value
		&& value_created_success && value_created_bad
		&& value_deleted_success && value_deleted_bad
	) retval = 0;

	return retval;

}

unsigned int test_values_init (void) {

	unsigned int errors = 0;

	errors |= test_values_init_pool ();

	errors |= test_values_init_query_opts ();

	errors |= test_values_init_responses ();

	return errors;

}

void test_values_end (void) {

	cmongo_select_delete (value_no_user_select);
	bson_destroy ((bson_t *) value_no_user_query_opts);

	pool_delete (values_pool);
	values_pool = NULL;

	http_response_delete (no_user_values);
	http_response_delete (no_user_value);

	http_response_delete (value_created_success);
	http_response_delete (value_created_bad);
	http_response_delete (value_deleted_success);
	http_response_delete (value_deleted_bad);

}

unsigned int test_values_get_all_by_user (
	const bson_oid_t *user_oid,
	char **json, size_t *json_len
) {

	return values_get_all_by_user_to_json (
		user_oid, value_no_user_query_opts,
		json, json_len
	);

}

Value *test_value_get_by_id_and_user (
	const String *value_id, const bson_oid_t *user_oid
) {

	Value *value = NULL;

	if (value_id) {
		value = (Value *) pool_pop (values_pool);
		if (value) {
			bson_oid_init_from_string (&value->oid, value_id->str);

			if (value_get_by_oid_and_user (
				value,
				&value->oid, user_oid,
				NULL
			)) {
				test_value_return (value);
				value = NULL;
			}
		}
	}

	return value;

}

u8 test_value_get_by_id_and_user_to_json (
	const char *value_id, const bson_oid_t *user_oid,
	const bson_t *query_opts,
	char **json, size_t *json_len
) {

	u8 retval = 1;

	if (value_id) {
		bson_oid_t value_oid = { 0 };
		bson_oid_init_from_string (&value_oid, value_id);

		retval = value_get_by_oid_and_user_to_json (
			&value_oid, user_oid,
			query_opts,
			json, json_len
		);
	}

	return retval;

}

static Value *test_value_create_actual (
	const char *user_id,
	const char *name, const char *data,
	const char *color
) {

	Value *value = (Value *) pool_pop (values_pool);
	if (value) {
		bson_oid_init (&value->oid, NULL);

		bson_oid_init_from_string (&value->user_oid, user_id);

		if (name) (void) strncpy (value->name, name, VALUE_NAME_SIZE - 1);
		if (data) (void) strncpy (value->data, data, VALUE_DATA_SIZE - 1);
		
		value->date = time (NULL);
	}

	return value;

}

static void test_value_parse_json (
	json_t *json_body,
	const char **name,
	const char **data,
	const char **color
) {

	// get values from json to create a new value
	const char *key = NULL;
	json_t *value = NULL;
	if (json_typeof (json_body) == JSON_OBJECT) {
		json_object_foreach (json_body, key, value) {
			if (!strcmp (key, "name")) {
				*name = json_string_value (value);
				#ifdef ERMIRY_DEBUG
				(void) printf ("name: \"%s\"\n", *name);
				#endif
			}

			else if (!strcmp (key, "data")) {
				*data = json_string_value (value);
				#ifdef ERMIRY_DEBUG
				(void) printf ("data: \"%s\"\n", *data);
				#endif
			}
		}
	}

}

static TestError test_value_create_parse_json (
	Value **value,
	const char *user_id, const String *request_body
) {

	TestError error = TEST_ERROR_NONE;

	const char *name = NULL;
	const char *data = NULL;
	const char *color = NULL;

	json_error_t json_error =  { 0 };
	json_t *json_body = json_loads (request_body->str, 0, &json_error);
	if (json_body) {
		test_value_parse_json (
			json_body,
			&name,
			&data,
			&color
		);

		*value = test_value_create_actual (
			user_id,
			name, data,
			color
		);

		json_decref (json_body);
	}

	else {
		cerver_log_error (
			"json_loads () - json error on line %d: %s\n", 
			json_error.line, json_error.text
		);

		error = TEST_ERROR_BAD_REQUEST;
	}

	return error;

}

TestError test_value_create (
	const User *user, const String *request_body
) {

	TestError error = TEST_ERROR_NONE;

	if (request_body) {
		Value *value = NULL;

		error = test_value_create_parse_json (
			&value,
			user->id, request_body
		);

		if (error == TEST_ERROR_NONE) {
			#ifdef ERMIRY_DEBUG
			value_print (value);
			#endif

			if (!value_insert_one (
				value
			)) {
				// update users values
				(void) user_add_value (user);
			}

			else {
				error = TEST_ERROR_SERVER_ERROR;
			}
			
			test_value_return (value);
		}
	}

	else {
		#ifdef ERMIRY_DEBUG
		cerver_log_error ("Missing request body to create value!");
		#endif

		error = TEST_ERROR_BAD_REQUEST;
	}

	return error;

}

static TestError test_value_update_parse_json (
	Value *value, const String *request_body
) {

	TestError error = TEST_ERROR_NONE;

	const char *name = NULL;
	const char *data = NULL;
	const char *color = NULL;

	json_error_t json_error =  { 0 };
	json_t *json_body = json_loads (request_body->str, 0, &json_error);
	if (json_body) {
		test_value_parse_json (
			json_body,
			&name,
			&data,
			&color
		);

		if (name) (void) strncpy (value->name, name, VALUE_NAME_SIZE - 1);
		if (data) (void) strncpy (value->data, data, VALUE_DATA_SIZE - 1);

		json_decref (json_body);
	}

	else {
		#ifdef ERMIRY_DEBUG
		cerver_log_error (
			"json_loads () - json error on line %d: %s\n", 
			json_error.line, json_error.text
		);
		#endif

		error = TEST_ERROR_BAD_REQUEST;
	}

	return error;

}

TestError test_value_update (
	const User *user, const String *value_id,
	const String *request_body
) {

	TestError error = TEST_ERROR_NONE;

	if (request_body) {
		Value *value = test_value_get_by_id_and_user (
			value_id, &user->oid
		);

		if (value) {
			// get update values
			if (test_value_update_parse_json (
				value, request_body
			) == TEST_ERROR_NONE) {
				// update the value in the db
				if (value_update_one (value)) {
					error = TEST_ERROR_SERVER_ERROR;
				}
			}

			test_value_return (value);
		}

		else {
			#ifdef ERMIRY_DEBUG
			cerver_log_error ("Failed to get matching value!");
			#endif

			error = TEST_ERROR_NOT_FOUND;
		}
	}

	else {
		#ifdef ERMIRY_DEBUG
		cerver_log_error ("Missing request body to update value!");
		#endif

		error = TEST_ERROR_BAD_REQUEST;
	}

	return error;

}

// TODO: handle things that reference the requested value
TestError test_value_delete (
	const User *user, const String *value_id
) {

	TestError error = TEST_ERROR_NONE;

	bson_oid_t oid = { 0 };
	bson_oid_init_from_string (&oid, value_id->str);

	if (!value_delete_one_by_oid_and_user (
		&oid, &user->oid
	)) {
		#ifdef ERMIRY_DEBUG
		cerver_log_debug ("Deleted value %s", value_id->str);
		#endif
	}

	else {
		error = TEST_ERROR_BAD_REQUEST;
	}

	return error;

}

void test_value_return (void *value_ptr) {

	(void) memset (value_ptr, 0, sizeof (Value));
	(void) pool_push (values_pool, value_ptr);

}