#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/types/string.h>

#include <cerver/collections/dlist.h>

#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>
#include <cerver/http/json/json.h>

#include <cerver/utils/utils.h>
#include <cerver/utils/log.h>

#include <cmongo/crud.h>

#include "test.h"

#include "controllers/roles.h"
#include "controllers/users.h"

#include "models/user.h"

#define USER_FIELD_LEN			128

// GET /api/users
void users_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (users_works, http_receive);

}

static void users_input_parse_json (
	json_t *json_body,
	const char **name,
	const char **username,
	const char **email,
	const char **password,
	const char **confirm
) {

	// get values from json to create a new transaction
	char *string = NULL;
	const char *key = NULL;
	json_t *value = NULL;
	if (json_typeof (json_body) == JSON_OBJECT) {
		json_object_foreach (json_body, key, value) {
			if (!strcmp (key, "name")) {
				string = (char *) json_string_value (value);
				if (strlen (string)) {
					*name = string;
					#ifdef ERMIRY_DEBUG
					(void) printf ("name: \"%s\"\n", *name);
					#endif
				}
			}

			else if (!strcmp (key, "username")) {
				string = (char *) json_string_value (value);
				if (strlen (string)) {
					*username = string;
					#ifdef ERMIRY_DEBUG
					(void) printf ("username: \"%s\"\n", *username);
					#endif
				}
			}

			else if (!strcmp (key, "email")) {
				string = (char *) json_string_value (value);
				if (strlen (string)) {
					*email = string;
					#ifdef ERMIRY_DEBUG
					(void) printf ("email: \"%s\"\n", *email);
					#endif
				}
			}

			else if (!strcmp (key, "password")) {
				string = (char *) json_string_value (value);
				if (strlen (string)) {
					*password = string;
					#ifdef ERMIRY_DEBUG
					(void) printf ("password: \"%s\"\n", *password);
					#endif
				}
			}

			else if (!strcmp (key, "confirm")) {
				string = (char *) json_string_value (value);
				if (strlen (string)) {
					*confirm = string;
					#ifdef ERMIRY_DEBUG
					(void) printf ("confirm: \"%s\"\n", *confirm);
					#endif
				}
			}
		}
	}

}

static void users_bad_input (
	const HttpReceive *http_receive,
	const char *name,
	const char *username,
	const char *email,
	const char *password,
	const char *confirm,
	bool login
) {

	char name_error[USER_FIELD_LEN] = { "null" };
	char username_error[USER_FIELD_LEN] = { "null" };
	char email_error[USER_FIELD_LEN] = { "null" };
	char password_error[USER_FIELD_LEN] = { "null" };
	char confirm_error[USER_FIELD_LEN] = { "null" };

	if (!name && !login) (void) strncpy (name_error, "Name field is required!", USER_FIELD_LEN);
	if (!username && !login) (void) strncpy (username_error, "Username field is required!", USER_FIELD_LEN);
	if (!email) (void) strncpy (email_error, "Email field is required!", USER_FIELD_LEN);
	if (!password) (void) strncpy (password_error, "Password field is required!", USER_FIELD_LEN);
	if (!confirm && !login) (void) strncpy (confirm_error, "Password confirm field is required!", USER_FIELD_LEN);

	if (password && confirm && !login) {
		if (strcmp (password, confirm)) {
			(void) strncpy (confirm_error, "Passwords do not match!", USER_FIELD_LEN);
		}
	}

	char *json = NULL;

	if (login) {
		json = c_string_create (
			"{\"email\": \"%s\", \"password\": \"%s\"}",
			email_error, password_error
		);
	}

	else {
		json = c_string_create (
			"{\"name\": \"%s\", \"username\": \"%s\", \"email\": \"%s\", \"password\": \"%s\", \"confirm\": \"%s\"}",
			name_error, username_error, email_error, password_error, confirm_error
		);
	}

	if (json) {
		HttpResponse *res = http_response_create (400, json, strlen (json));
		if (res) {
			http_response_compile (res);
			// http_response_print (res);
			(void) http_response_send (res, http_receive);
			http_respponse_delete (res);
		}

		free (json);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

}

static bool users_register_handler_validate_input (
	const HttpReceive *http_receive,
	User *user_values,
	const char *name,
	const char *username,
	const char *email,
	const char *password,
	const char *confirm
) {
	
	bool valid = false;

	if (name && username && email && password && confirm) {
		if (!strcmp (password, confirm)) {
			(void) strncpy (user_values->name, name, USER_NAME_LEN);
			(void) strncpy (user_values->username, username, USER_USERNAME_LEN);
			(void) strncpy (user_values->email, email, USER_EMAIL_LEN);
			(void) strncpy (user_values->password, password, USER_PASSWORD_LEN);

			valid = true;
		}
	}

	if (!valid) {
		#ifdef ERMIRY_DEBUG
		cerver_log_warning ("Missing user values!");
		#endif

		users_bad_input (
			http_receive,
			name, username, email, password, confirm,
			false
		);
	}

	return valid;

}

static bool users_login_handler_validate_input (
	const HttpReceive *http_receive,
	User *user_values,
	const char *email,
	const char *password
) {

	bool retval = false;

	if (email && password) {
		(void) strncpy (user_values->email, email, USER_EMAIL_LEN);
		(void) strncpy (user_values->password, password, USER_PASSWORD_LEN);

		retval = true;
	}

	else {
		#ifdef ERMIRY_DEBUG
		cerver_log_warning ("Missing user values!");
		#endif

		users_bad_input (
			http_receive,
			NULL, NULL, email, password, NULL,
			true
		);
	}

	return retval;

}

static bool users_validate_input (
	const HttpReceive *http_receive,
	const String *request_body,
	User *user_values, bool login
) {

	bool retval = false;

	if (request_body) {
		const char *name = NULL;
		const char *username = NULL;
		const char *email = NULL;
		const char *password = NULL;
		const char *confirm = NULL;

		json_error_t json_error =  { 0 };
		json_t *json_body = json_loads (request_body->str, 0, &json_error);
		if (json_body) {
			users_input_parse_json (
				json_body,
				&name,
				&username,
				&email,
				&password,
				&confirm
			);

			retval = login ?
				users_login_handler_validate_input (
					http_receive, user_values,
					email, password
				) :
				users_register_handler_validate_input (
					http_receive, user_values,
					name, username, email, password, confirm
				);

			json_decref (json_body);
		}

		else {
			cerver_log_error (
				"json_loads () - json error on line %d: %s\n", 
				json_error.line, json_error.text
			);

			(void) http_response_send (server_error, http_receive);
		}
	}

	else {
		cerver_log_error ("Missing request body to login user!");

		(void) http_response_send (server_error, http_receive);
	}

	return retval;

}

// generate and send back token
static void users_generate_and_send_token (
	const HttpReceive *http_receive,
	const User *user, const char *role_name
) {

	bson_oid_to_string (&user->oid, (char *) user->id);

	DoubleList *payload = dlist_init (key_value_pair_delete, NULL);
	(void) dlist_insert_at_end_unsafe (payload, key_value_pair_create ("email", user->email));
	(void) dlist_insert_at_end_unsafe (payload, key_value_pair_create ("id", user->id));
	(void) dlist_insert_at_end_unsafe (payload, key_value_pair_create ("name", user->name));
	(void) dlist_insert_at_end_unsafe (payload, key_value_pair_create ("role", role_name));
	(void) dlist_insert_at_end_unsafe (payload, key_value_pair_create ("username", user->username));

	// generate & send back auth token
	char *token = http_cerver_auth_generate_jwt (
		(HttpCerver *) http_receive->cr->cerver->cerver_data, payload
	);

	if (token) {
		char *bearer = c_string_create ("Bearer %s", token);
		if (bearer) {
			char *json = c_string_create ("{\"token\": \"%s\"}", bearer);
			if (json) {
				HttpResponse *res = http_response_create (200, json, strlen (json));
				if (res) {
					http_response_compile (res);
					// http_response_print (res);
					(void) http_response_send (res, http_receive);
					http_respponse_delete (res);
				}

				free (json);
			}

			else {
				(void) http_response_send (server_error, http_receive);
			}

			free (bearer);
		}

		else {
			(void) http_response_send (server_error, http_receive);
		}

		free (token);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

	dlist_delete (payload);

}

static u8 users_register_handler_save_user (
	const HttpReceive *http_receive,
	User *user
) {

	u8 retval = 1;

	if (!mongo_insert_one (
		users_collection,
		user_bson_create (user)
	)) {
		retval = 0;
	}

	else {
		cerver_log_error ("Failed to save new user!");
		(void) http_response_send (server_error, http_receive);
	}

	return retval;

}

// POST /api/users/register
void users_register_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user_values = test_user_get ();

	if (users_validate_input (
		http_receive,
		request->body,
		user_values,
		false
	)) {
		if (!test_user_check_by_email (http_receive, user_values->email)) {
			User *user = test_user_create (
				user_values->name,
				user_values->username,
				user_values->email,
				user_values->password,
				&common_role->oid
			);
			if (user) {
				if (!users_register_handler_save_user (http_receive, user)) {
					cerver_log_success (
						"User %s has created an account!", user_values->email
					);

					// return token upon success
					users_generate_and_send_token (
						http_receive, user, common_role->name
					);
				}

				test_user_delete (user);
			}

			else {
				#ifdef ERMIRY_DEBUG
				cerver_log_error ("Failed to create user!");
				#endif
				(void) http_response_send (server_error, http_receive);
			}
		}
	}

	test_user_delete (user_values);

}

// POST /api/users/login
void users_login_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user_values = test_user_get ();

	if (users_validate_input (
		http_receive,
		request->body,
		user_values,
		true
	)) {
		User *user = test_user_get_by_email (user_values->email);
		if (user) {
			#ifdef ERMIRY_DEBUG
			char oid_buffer[32] = { 0 };
			bson_oid_to_string (&user->oid, oid_buffer);
			#endif

			if (!strcmp (user->password, user_values->password)) {
				#ifdef ERMIRY_DEBUG
				cerver_log_success ("User %s login -> success", oid_buffer);
				#endif

				// generate and send token back to the user
				users_generate_and_send_token (
					http_receive,
					user, test_roles_get_by_oid (&user->role_oid)
				);
			}

			else {
				#ifdef ERMIRY_DEBUG
				cerver_log_error ("User %s login -> wrong password", oid_buffer);
				#endif

				(void) http_response_send (wrong_password, http_receive);
			}

			test_user_delete (user);
		}

		else {
			#ifdef ERMIRY_DEBUG
			cerver_log_warning ("User not found!");
			#endif
			(void) http_response_send (user_not_found, http_receive);
		}
	}

	test_user_delete (user_values);

}