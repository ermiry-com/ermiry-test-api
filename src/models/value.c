#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <cerver/types/types.h>

#include <cerver/utils/log.h>

#include <cmongo/collections.h>
#include <cmongo/crud.h>

#include "models/value.h"

#define VALUES_COLL_NAME         				"values"

mongoc_collection_t *values_collection = NULL;

// opens handle to value collection
unsigned int values_collection_get (void) {

	unsigned int retval = 1;

	values_collection = mongo_collection_get (VALUES_COLL_NAME);
	if (values_collection) {
		cerver_log_debug ("Opened handle to values collection!");
		retval = 0;
	}

	else {
		cerver_log_error ("Failed to get handle to values collection!");
	}

	return retval;

}

void values_collection_close (void) {

	if (values_collection) mongoc_collection_destroy (values_collection);

}

void *value_new (void) {

	Value *value = (Value *) malloc (sizeof (Value));
	if (value) {
		(void) memset (value, 0, sizeof (Value));
	}

	return value;

}

void value_delete (void *value_ptr) {

	if (value_ptr) free (value_ptr);

}

void value_print (Value *value) {

	if (value) {
		char buffer[128] = { 0 };
		bson_oid_to_string (&value->oid, buffer);
		(void) printf ("id: %s\n", buffer);

		(void) printf ("name: %s\n", value->name);
		(void) printf ("data: %s\n", value->data);

		(void) strftime (buffer, 128, "%d/%m/%y - %T", gmtime (&value->date));
		(void) printf ("date: %s GMT\n", buffer);
	}

}

static void value_doc_parse (Value *val, const bson_t *value_doc) {

	bson_iter_t iter = { 0 };
	if (bson_iter_init (&iter, value_doc)) {
		char *key = NULL;
		bson_value_t *value = NULL;
		while (bson_iter_next (&iter)) {
			key = (char *) bson_iter_key (&iter);
			value = (bson_value_t *) bson_iter_value (&iter);

			if (!strcmp (key, "_id")) {
				bson_oid_copy (&value->value.v_oid, &val->oid);
				bson_oid_init_from_string (&val->oid, val->id);
			}

			else if (!strcmp (key, "user"))
				bson_oid_copy (&value->value.v_oid, &val->user_oid);

			else if (!strcmp (key, "name") && value->value.v_utf8.str) {
				(void) strncpy (
					val->name,
					value->value.v_utf8.str,
					VALUE_NAME_LEN - 1
				);
			}

			else if (!strcmp (key, "data") && value->value.v_utf8.str) {
				(void) strncpy (
					val->data,
					value->value.v_utf8.str,
					VALUE_DATA_LEN - 1
				);
			}

			else if (!strcmp (key, "date")) 
				val->date = (time_t) bson_iter_date_time (&iter) / 1000;
		}
	}

}

bson_t *value_query_oid (const bson_oid_t *oid) {

	bson_t *query = NULL;

	if (oid) {
		query = bson_new ();
		if (query) {
			(void) bson_append_oid (query, "_id", -1, oid);
		}
	}

	return query;

}

const bson_t *value_find_by_oid (
	const bson_oid_t *oid, const bson_t *query_opts
) {

	const bson_t *retval = NULL;

	bson_t *value_query = bson_new ();
	if (value_query) {
		(void) bson_append_oid (value_query, "_id", -1, oid);
		retval = mongo_find_one_with_opts (
			values_collection, value_query, query_opts
		);
	}

	return retval;

}

u8 value_get_by_oid (
	Value *value, const bson_oid_t *oid, const bson_t *query_opts
) {

	u8 retval = 1;

	if (value) {
		const bson_t *value_doc = value_find_by_oid (oid, query_opts);
		if (value_doc) {
			value_doc_parse (value, value_doc);
			bson_destroy ((bson_t *) value_doc);

			retval = 0;
		}
	}

	return retval;

}

const bson_t *value_find_by_oid_and_user (
	const bson_oid_t *oid, const bson_oid_t *user_oid,
	const bson_t *query_opts
) {

	const bson_t *retval = NULL;

	bson_t *value_query = bson_new ();
	if (value_query) {
		(void) bson_append_oid (value_query, "_id", -1, oid);
		(void) bson_append_oid (value_query, "user", -1, user_oid);

		retval = mongo_find_one_with_opts (
			values_collection, value_query, query_opts
		);
	}

	return retval;

}

u8 value_get_by_oid_and_user (
	Value *value,
	const bson_oid_t *oid, const bson_oid_t *user_oid,
	const bson_t *query_opts
) {

	u8 retval = 1;

	if (value) {
		const bson_t *value_doc = value_find_by_oid_and_user (
			oid, user_oid, query_opts
		);

		if (value_doc) {
			value_doc_parse (value, value_doc);
			bson_destroy ((bson_t *) value_doc);

			retval = 0;
		}
	}

	return retval;

}

bson_t *value_to_bson (Value *value) {

    bson_t *doc = NULL;

    if (value) {
        doc = bson_new ();
        if (doc) {
            (void) bson_append_oid (doc, "_id", -1, &value->oid);

			(void) bson_append_oid (doc, "user", -1, &value->user_oid);

			(void) bson_append_utf8 (doc, "name", -1, value->name, -1);
			(void) bson_append_utf8 (doc, "data", -1, value->data, -1);
			(void) bson_append_date_time (doc, "date", -1, value->date * 1000);
        }
    }

    return doc;

}

bson_t *value_update_bson (Value *value) {

	bson_t *doc = NULL;

    if (value) {
        doc = bson_new ();
        if (doc) {
			bson_t set_doc = { 0 };
			(void) bson_append_document_begin (doc, "$set", -1, &set_doc);
			(void) bson_append_utf8 (&set_doc, "name", -1, value->name, -1);
			(void) bson_append_utf8 (&set_doc, "data", -1, value->data, -1);
			(void) bson_append_document_end (doc, &set_doc);
        }
    }

    return doc;

}

// get all the values that are related to a user
mongoc_cursor_t *values_get_all_by_user (
	const bson_oid_t *user_oid, const bson_t *opts
) {

	mongoc_cursor_t *retval = NULL;

	if (user_oid && opts) {
		bson_t *query = bson_new ();
		if (query) {
			(void) bson_append_oid (query, "user", -1, user_oid);

			retval = mongo_find_all_cursor_with_opts (
				values_collection,
				query, opts
			);
		}
	}

	return retval;

}