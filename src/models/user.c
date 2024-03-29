#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <cmongo/collections.h>
#include <cmongo/crud.h>
#include <cmongo/model.h>

#include "models/user.h"

static CMongoModel *users_model = NULL;

static void user_doc_parse (
	void *user_ptr, const bson_t *user_doc
);

unsigned int users_model_init (void) {

	unsigned int retval = 1;

	users_model = cmongo_model_create (USERS_COLL_NAME);
	if (users_model) {
		cmongo_model_set_parser (users_model, user_doc_parse);

		retval = 0;
	}

	return retval;

}

void users_model_end (void) {

	cmongo_model_delete (users_model);

}

void *user_new (void) {

	User *user = (User *) malloc (sizeof (User));
	if (user) {
		(void) memset (user, 0, sizeof (User));
	}

	return user;

}

void user_delete (void *user_ptr) {

	if (user_ptr) free (user_ptr);

}

void user_print (const User *user) {

	if (user) {
		(void) printf ("User: \n");

		if (user->email_len)
			(void) printf ("\temail: %s\n", user->email);
		
		(void) printf ("\tiat: %ld\n", user->iat);

		(void) printf ("\tid: %s\n", user->id);
		
		if (user->name_len)
			(void) printf ("\tname: %s\n", user->name);

		if (user->username_len)
			(void) printf ("\tusername: %s\n", user->username);
		
		(void) printf ("\trole: %s\n", user->role);
	}

}

// parses a bson doc into a user model
static void user_doc_parse (
	void *user_ptr, const bson_t *user_doc
) {

	User *user = (User *) user_ptr;

	bson_iter_t iter = { 0 };
	if (bson_iter_init (&iter, user_doc)) {
		char *key = NULL;
		bson_value_t *value = NULL;
		while (bson_iter_next (&iter)) {
			key = (char *) bson_iter_key (&iter);
			value = (bson_value_t *) bson_iter_value (&iter);

			if (!strcmp (key, "_id")) {
				bson_oid_copy (&value->value.v_oid, &user->oid);
				bson_oid_to_string (&user->oid, user->id);
			}

			else if (!strcmp (key, "role")) {
				bson_oid_copy (&value->value.v_oid, &user->role_oid);
			}

			else if (!strcmp (key, "name") && value->value.v_utf8.str) {
				(void) strncpy (
					user->name,
					value->value.v_utf8.str,
					USER_NAME_SIZE - 1
				);

				user->name_len = (unsigned int) strlen (user->name);
			}

			else if (!strcmp (key, "email") && value->value.v_utf8.str) {
				(void) strncpy (
					user->email,
					value->value.v_utf8.str,
					USER_EMAIL_SIZE - 1
				);

				user->email_len = (unsigned int) strlen (user->email);
			}

			else if (!strcmp (key, "username") && value->value.v_utf8.str) {
				(void) strncpy (
					user->username,
					value->value.v_utf8.str,
					USER_USERNAME_SIZE - 1
				);

				user->username_len = (unsigned int) strlen (user->username);
			}

			else if (!strcmp (key, "password") && value->value.v_utf8.str) {
				(void) strncpy (
					user->password,
					value->value.v_utf8.str,
					USER_PASSWORD_SIZE - 1
				);

				user->password_len = (unsigned int) strlen (user->password);
			}

			else if (!strcmp (key, "first_time")) {
				user->first_time = value->value.v_bool;
			}

			else if (!strcmp (key, "date")) {
				user->date = value->value.v_datetime / 1000;
			}
		}
	}

}

static bson_t *user_query_oid (const bson_oid_t *oid) {

	bson_t *query = bson_new ();
	if (query) {
		(void) bson_append_oid (query, "_id", -1, oid);
	}

	return query;

}

unsigned int user_get_by_oid (
	User *user, const bson_oid_t *oid, const bson_t *query_opts
) {

	unsigned int retval = 1;

	if (user && oid) {
		bson_t *user_query = bson_new ();
		if (user_query) {
			(void) bson_append_oid (user_query, "_id", -1, oid);
			retval = mongo_find_one_with_opts (
				users_model,
				user_query, query_opts,
				user
			);
		}
	}

	return retval;

}

static bson_t *user_query_email (const char *email) {

	bson_t *query = bson_new ();
	if (query) {
		(void) bson_append_utf8 (query, "email", -1, email, -1);
	}

	return query;

}

bool user_check_by_email (const char *email) {

	return mongo_check (users_model, user_query_email (email));

}

unsigned int user_get_by_email (
	User *user, const char *email, const bson_t *query_opts
) {

	unsigned int retval = 1;

	if (user && email) {
		bson_t *user_query = bson_new ();
		if (user_query) {
			(void) bson_append_utf8 (user_query, "email", -1, email, -1);
			retval = mongo_find_one_with_opts (
				users_model,
				user_query, query_opts,
				user
			);
		}
	}

	return retval;

}

static bson_t *user_bson_create (const User *user) {

	bson_t *doc = bson_new ();
	if (doc) {
		(void) bson_append_oid (doc, "_id", -1, &user->oid);

		(void) bson_append_utf8 (doc, "name", -1, user->name, -1);
		(void) bson_append_utf8 (doc, "email", -1, user->email, -1);
		(void) bson_append_utf8 (doc, "username", -1, user->username, -1);
		(void) bson_append_utf8 (doc, "password", -1, user->password, -1);

		(void) bson_append_oid (doc, "role", -1, &user->role_oid);

		(void) bson_append_bool (doc, "first_time", -1, user->first_time);

		(void) bson_append_date_time (doc, "date", -1, user->date * 1000);
	}

	return doc;

}

unsigned int user_insert_one (const User *user) {

	return mongo_insert_one (
		users_model,
		user_bson_create (user)
	);

}

// adds one to user's values count
static bson_t *user_create_update_values (void) {

	bson_t *doc = bson_new ();
	if (doc) {
		bson_t inc_doc = BSON_INITIALIZER;
		(void) bson_append_document_begin (doc, "$inc", -1, &inc_doc);
		(void) bson_append_int32 (&inc_doc, "valuesCount", -1, 1);
		(void) bson_append_document_end (doc, &inc_doc);
	}

	return doc;

}

unsigned int user_add_value (const User *user) {

	return mongo_update_one (
		users_model,
		user_query_oid (&user->oid),
		user_create_update_values ()
	);

}
