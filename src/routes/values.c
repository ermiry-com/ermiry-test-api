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

#include "test.h"

#include "controllers/values.h"
#include "controllers/users.h"

// GET /api/test/values
// get all the authenticated user's values
void test_values_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;
	if (user) {
		size_t json_len = 0;
		char *json = NULL;

		if (!test_values_get_all_by_user (
			&user->oid,
			&json, &json_len
		)) {
			if (json) {
				(void) http_response_json_custom_reference_send (
					http_receive,
					HTTP_STATUS_OK,
					json, json_len
				);

				free (json);
			}

			else {
				(void) http_response_send (no_user_values, http_receive);
			}
		}

		else {
			(void) http_response_send (no_user_values, http_receive);
		}		
	}

	else {
		(void) http_response_send (bad_user_error, http_receive);
	}

}

// POST /api/test/values
// a user has requested to create a new value
void test_value_create_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;
	if (user) {
		TestError error = test_value_create (
			user, request->body
		);

		switch (error) {
			case TEST_ERROR_NONE: {
				// return success to user
				(void) http_response_send (
					value_created_success,
					http_receive
				);
			} break;

			default: {
				test_error_send_response (error, http_receive);
			} break;
		}
	}

	else {
		(void) http_response_send (bad_user_error, http_receive);
	}

}

// GET /api/test/values/:id/info
// returns information about an existing value that belongs to a user
void test_value_get_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	const String *value_id = request->params[0];

	User *user = (User *) request->decoded_data;
	if (user) {
		if (value_id) {
			size_t json_len = 0;
			char *json = NULL;

			if (!test_value_get_by_id_and_user_to_json (
				value_id->str, &user->oid,
				value_no_user_query_opts,
				&json, &json_len
			)) {
				if (json) {
					(void) http_response_json_custom_reference_send (
						http_receive, HTTP_STATUS_OK, json, json_len
					);
					
					free (json);
				}

				else {
					(void) http_response_send (server_error, http_receive);
				}
			}

			else {
				(void) http_response_send (no_user_value, http_receive);
			}
		}
	}

	else {
		(void) http_response_send (bad_user_error, http_receive);
	}

}

// PUT /api/test/values/:id/update
// a user wants to update an existing value
void test_value_update_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;
	if (user) {
		TestError error = test_value_update (
			user, request->params[0], request->body
		);

		switch (error) {
			case TEST_ERROR_NONE: {
				(void) http_response_send (oki_doki, http_receive);
			} break;

			default: {
				test_error_send_response (error, http_receive);
			} break;
		}
	}

	else {
		(void) http_response_send (bad_user_error, http_receive);
	}

}

// DELETE /api/test/values/:id/remove
// deletes an existing user's value
void test_value_delete_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	const String *value_id = request->params[0];

	User *user = (User *) request->decoded_data;
	if (user) {
		switch (test_value_delete (user, value_id)) {
			case TEST_ERROR_NONE:
				(void) http_response_send (value_deleted_success, http_receive);
				break;

			default:
				(void) http_response_send (value_deleted_bad, http_receive);
				break;
		}
	}

	else {
		(void) http_response_send (bad_user_error, http_receive);
	}

}