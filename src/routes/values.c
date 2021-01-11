#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/types/types.h>
#include <cerver/types/string.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>
#include <cerver/http/json/json.h>

#include <cerver/utils/utils.h>
#include <cerver/utils/log.h>

#include <cmongo/crud.h>

#include "errors.h"
#include "test.h"

#include "controllers/users.h"
#include "controllers/values.h"

#include "models/user.h"
#include "models/value.h"

static char *test_values_handler_generate_json (
	mongoc_cursor_t *value_cursor,
	size_t *json_len
) {

	char *retval = NULL;

	bson_t *doc = bson_new ();
	if (doc) {
		bson_t value_array = { 0 };
		(void) bson_append_array_begin (doc, "values", -1, &value_array);
		char buf[16] = { 0 };
		const char *key = NULL;
		size_t keylen = 0;

		int i = 0;
		const bson_t *value_doc = NULL;
		while (mongoc_cursor_next (value_cursor, &value_doc)) {
			keylen = bson_uint32_to_string (i, &key, buf, sizeof (buf));
			(void) bson_append_document (&value_array, key, (int) keylen, value_doc);

			bson_destroy ((bson_t *) value_doc);

			i++;
		}
		(void) bson_append_array_end (doc, &value_array);

		retval = bson_as_relaxed_extended_json (doc, json_len);
	}

	return retval;

}

// GET /api/values
// get all the authenticated user's values
void test_values_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;
	if (user) {
		mongoc_cursor_t *value_cursor = values_get_all_by_user (
			&user->oid, value_no_user_query_opts
		);

		if (value_cursor) {
			// convert them to json and send them back
			size_t json_len = 0;
			char *json = test_values_handler_generate_json (
				value_cursor, &json_len
			);

			if (json) {
				(void) http_response_json_custom_reference_send (
					http_receive,
					200,
					json, json_len
				);

				free (json);
			}

			else {
				(void) http_response_send (server_error, http_receive);
			}

			mongoc_cursor_destroy (value_cursor);
		}

		else {
			(void) http_response_send (bad_user, http_receive);
		}
	}

	else {
		(void) http_response_send (bad_user, http_receive);
	}

}

static void test_value_parse_json (
	json_t *json_body,
	const char **name,
	const char **data
) {

	// get values from json to create a new value
	const char *key = NULL;
	json_t *value = NULL;
	if (json_typeof (json_body) == JSON_OBJECT) {
		json_object_foreach (json_body, key, value) {
			if (!strcmp (key, "name")) {
				*name = json_string_value (value);
				(void) printf ("name: \"%s\"\n", *name);
			}

			if (!strcmp (key, "data")) {
				*data = json_string_value (value);
				(void) printf ("data: \"%s\"\n", *data);
			}
		}
	}

}

static TestError test_value_create_handler_internal (
	Value **value,
	const char *user_id, const String *request_body
) {

	TestError error = TEST_ERROR_NONE;

	if (request_body) {
		const char *name = NULL;
		const char *data = NULL;

		json_error_t json_error =  { 0 };
		json_t *json_body = json_loads (request_body->str, 0, &json_error);
		if (json_body) {
			test_value_parse_json (
				json_body,
				&name, &data
			);

			if (name && data) {
				*value = test_value_create (
					user_id,
					name, data
				);

				if (*value == NULL) error = TEST_ERROR_SERVER_ERROR;
			}

			else {
				error = TEST_ERROR_MISSING_VALUES;
			}

			json_decref (json_body);
		}

		else {
			cerver_log_error (
				"json_loads () - json error on line %d: %s\n", 
				json_error.line, json_error.text
			);

			error = TEST_ERROR_BAD_REQUEST;
		}
	}

	else {
		cerver_log_error ("Missing request body to create value!");

		error = TEST_ERROR_BAD_REQUEST;
	}

	return error;

}

// POST /api/test/values
// a user has requested to create a new value
void test_value_create_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;
	if (user) {
		Value *value = NULL;

		TestError error = test_value_create_handler_internal (
			&value,
			user->id, request->body
		);

		if (error == TEST_ERROR_NONE) {
			#ifdef ERMIRY_DEBUG
			value_print (value);
			#endif

			if (!mongo_insert_one (
				values_collection,
				value_to_bson (value)
			)) {
				// return success to user
				(void) http_response_send (
					oki_doki,
					http_receive
				);
			}

			else {
				(void) http_response_send (
					server_error,
					http_receive
				);
			}
			
			test_value_delete (value);
		}

		else {
			test_error_send_response (error, http_receive);
		}
	}

	else {
		(void) http_response_send (bad_user, http_receive);
	}

}

// GET /api/test/values/:id
// returns information about an existing value that belongs to a user
void test_value_get_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	const String *value_id = request->params[0];

	User *user = (User *) request->decoded_data;
	if (user) {
		Value *value = (Value *) pool_pop (value_pool);
		if (value) {
			bson_oid_init_from_string (&value->oid, value_id->str);
			bson_oid_init_from_string (&value->user_oid, user->id);

			const bson_t *value_bson = value_find_by_oid_and_user (
				&value->oid, &value->user_oid,
				value_no_user_query_opts
			);

			if (value_bson) {
				size_t json_len = 0;
				char *json = bson_as_relaxed_extended_json (value_bson, &json_len);
				if (json) {
					(void) http_response_json_custom_reference_send (
						http_receive, 200, json, json_len
					);

					free (json);
				}

				bson_destroy ((bson_t *) value_bson);
			}

			else {
				(void) http_response_send (bad_request, http_receive);
			}

			test_value_delete (value);
		}
	}

	else {
		(void) http_response_send (bad_user, http_receive);
	}

}

static u8 test_value_update_handler_internal (
	Value *value, const String *request_body
) {

	u8 retval = 1;

	if (request_body) {
		const char *name = NULL;
		const char *data = NULL;

		json_error_t error =  { 0 };
		json_t *json_body = json_loads (request_body->str, 0, &error);
		if (json_body) {
			test_value_parse_json (
				json_body,
				&name, &data
			);

			if (name) {
				(void) strncpy (
					value->name,
					name,
					VALUE_NAME_LEN - 1
				);
			}

			if (data) {
				(void) strncpy (
					value->data,
					data,
					VALUE_DATA_LEN - 1
				);
			}

			json_decref (json_body);

			retval = 0;
		}

		else {
			cerver_log_error (
				"json_loads () - json error on line %d: %s\n", 
				error.line, error.text
			);
		}
	}

	else {
		cerver_log_error ("Missing request body to update value!");
	}

	return retval;

}

// POST /api/values/:id/update
// a user wants to update an existing value
void test_value_update_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;
	if (user) {
		bson_oid_init_from_string (&user->oid, user->id);

		Value *value = test_value_get_by_id_and_user (
			request->params[0], &user->oid
		);

		if (value) {
			// get update values
			if (!test_value_update_handler_internal (
				value, request->body
			)) {
				// update the value in the db
				if (!mongo_update_one (
					values_collection,
					value_query_oid (&value->oid),
					value_update_bson (value)
				)) {
					(void) http_response_send (oki_doki, http_receive);
				}

				else {
					(void) http_response_send (server_error, http_receive);
				}
			}

			test_value_delete (value);
		}

		else {
			(void) http_response_send (bad_request, http_receive);
		}
	}

	else {
		(void) http_response_send (bad_user, http_receive);
	}

}

// DELETE /api/values/:id/remove
// deletes an existing user's value
void test_value_delete_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	const String *value_id = request->params[0];

	User *user = (User *) request->decoded_data;
	if (user) {
		bson_t *value_query = bson_new ();
		if (value_query) {
			bson_oid_t oid = { 0 };

			bson_oid_init_from_string (&oid, value_id->str);
			(void) bson_append_oid (value_query, "_id", -1, &oid);

			bson_oid_init_from_string (&oid, user->id);
			(void) bson_append_oid (value_query, "user", -1, &oid);

			if (!mongo_delete_one (values_collection, value_query)) {
				#ifdef ERMIRY_DEBUG
				cerver_log_debug ("Deleted value %s", value_id->str);
				#endif

				(void) http_response_send (oki_doki, http_receive);
			}

			else {
				(void) http_response_send (server_error, http_receive);
			}
		}

		else {
			(void) http_response_send (server_error, http_receive);
		}
	}

	else {
		(void) http_response_send (bad_user, http_receive);
	}

}