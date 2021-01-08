#include <stdlib.h>
#include <stdio.h>

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#include <cerver/utils/log.h>

#include <cmongo/collections.h>
#include <cmongo/crud.h>

#include "models/action.h"

#define ACTIONS_COLL_NAME  				"actions"

mongoc_collection_t *actions_collection = NULL;

unsigned int actions_collection_get (void) {

	unsigned int retval = 1;

	actions_collection = mongo_collection_get (ACTIONS_COLL_NAME);
	if (actions_collection) {
		retval = 0;
	}

	else {
		cerver_log_error ("Failed to get handle to actions collection!");
	}

	return retval;

}

void actions_collection_close (void) {

	if (actions_collection) mongoc_collection_destroy (actions_collection);

}

RoleAction *action_new (void) {

	RoleAction *action = (RoleAction *) malloc (sizeof (RoleAction));
	if (action) {
		(void) memset (action, 0, sizeof (RoleAction));
	}

	return action;

}

void action_delete (void *action_ptr) {

	if (action_ptr) free (action_ptr);

}

RoleAction *action_create (
	const char *name, const char *description
) {

	RoleAction *action = action_new ();
	if (action) {
		if (name) {
			(void) strncpy (
				action->name, name, ACTION_NAME_LEN - 1
			);
		}

		if (description) {
			(void) strncpy (
				action->description, description, ACTION_DESCRIPTION_LEN - 1
			);
		}
	}

	return action;

}

void action_print (RoleAction *action) {

	if (action) {
		(void) printf ("Name: %s\n", action->name);
		(void) printf ("Description: %s\n", action->description);
	}

}

// creates a action bson with all action parameters
bson_t *action_bson_create (RoleAction *action) {

	bson_t *doc = NULL;

	if (action) {
		doc = bson_new ();
		if (doc) {
			bson_oid_init (&action->oid, NULL);
			(void) bson_append_oid (doc, "_id", -1, &action->oid);

			(void) bson_append_utf8 (
				doc, "name", -1, action->name, -1
			);

			(void) bson_append_utf8 (
				doc, "description", -1, action->description, -1
			);
		}
	}

	return doc;

}

RoleAction *action_doc_parse (
	const bson_t *action_doc
) {

	RoleAction *action = NULL;

	if (action_doc) {
		action = action_new ();

		bson_iter_t iter = { 0 };
		if (bson_iter_init (&iter, action_doc)) {
			while (bson_iter_next (&iter)) {
				const char *key = bson_iter_key (&iter);
				const bson_value_t *value = bson_iter_value (&iter);

				if (!strcmp (key, "_id")) {
					bson_oid_copy (&value->value.v_oid, &action->oid);
				}

				else if (!strcmp (key, "name") && value->value.v_utf8.str) {
					(void) strncpy (
						action->name,
						value->value.v_utf8.str,
						ACTION_NAME_LEN - 1
					);
				}

				else if (!strcmp (key, "description") && value->value.v_utf8.str) {
					(void) strncpy (
						action->description,
						value->value.v_utf8.str,
						ACTION_DESCRIPTION_LEN - 1
					);
				}
			}
		}
	}

	return action;

}

// get an action doc from the db by name
static const bson_t *action_find_by_name (
	const char *name
) {

	const bson_t *retval = NULL;

	bson_t *action_query = bson_new ();
	if (action_query) {
		(void) bson_append_utf8 (action_query, "name", -1, name, -1);

		retval = mongo_find_one (actions_collection, action_query, NULL);
	}

	return retval;

}

// gets an action form the db by its name
RoleAction *action_get_by_name (const char *name) {

	RoleAction *action = NULL;

	if (name) {
		const bson_t *action_doc = action_find_by_name (name);
		if (action_doc) {
			action = action_doc_parse (action_doc);
			bson_destroy ((bson_t *) action_doc);
		}
	}

	return action;

}

bson_t *action_bson_create_name_query (const char *name) {

	bson_t *action_query = NULL;

	if (name) {
		action_query = bson_new ();
		if (action_query) {
			bson_append_utf8 (action_query, "name", -1, name, -1);
		}
	}

	return action_query;

}

bson_t *action_bson_create_update (
	const char *name, const char *description
) {

	bson_t *doc = NULL;

	if (name && description) {
		doc = bson_new ();
		if (doc) {
			bson_t set_doc = { 0 };
			(void) bson_append_document_begin (doc, "$set", -1, &set_doc);

			(void) bson_append_utf8 (&set_doc, "name", -1, name, -1);
			(void) bson_append_utf8 (&set_doc, "description", -1, description, -1);

			(void) bson_append_document_end (doc, &set_doc);
		}
	}

	return doc;

}