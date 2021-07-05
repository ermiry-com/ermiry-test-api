#include <stdlib.h>
#include <stdio.h>

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#include <cmongo/collections.h>
#include <cmongo/crud.h>
#include <cmongo/model.h>

#include "models/action.h"

static CMongoModel *actions_model = NULL;

static void action_doc_parse (
	void *action_ptr, const bson_t *action_doc
);

unsigned int actions_model_init (void) {

	unsigned int retval = 1;

	actions_model = cmongo_model_create (ACTIONS_COLL_NAME);
	if (actions_model) {
		cmongo_model_set_parser (actions_model, action_doc_parse);

		retval = 0;
	}

	return retval;

}

void actions_model_end (void) {

	cmongo_model_delete (actions_model);

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

void action_print (const RoleAction *action) {

	if (action) {
		(void) printf ("Action: \n");
		(void) printf ("\tName: %s\n", action->name);
		(void) printf ("\tDescription: %s\n", action->description);
	}

}

static void action_doc_parse (
	void *action_ptr, const bson_t *action_doc
) {

	RoleAction *action = (RoleAction *) action_ptr;

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
					ACTION_NAME_SIZE - 1
				);
			}

			else if (!strcmp (key, "description") && value->value.v_utf8.str) {
				(void) strncpy (
					action->description,
					value->value.v_utf8.str,
					ACTION_DESCRIPTION_SIZE - 1
				);
			}
		}
	}

}

// gets an action form the db by its name
RoleAction *action_get_by_name (const char *name) {

	RoleAction *action = NULL;

	if (name) {
		action = action_new ();

		bson_t *action_query = bson_new ();
		if (action_query) {
			(void) bson_append_utf8 (action_query, "name", -1, name, -1);
			if (mongo_find_one (
				actions_model,
				action_query, NULL,
				action
			)) {
				action_delete (action);
				action = NULL;
			}
		}
	}

	return action;

}
