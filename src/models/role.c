#include <stdlib.h>
#include <stdio.h>

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#include <cerver/types/types.h>

#include <cerver/utils/log.h>

#include <cmongo/collections.h>
#include <cmongo/crud.h>
#include <cmongo/select.h>

#include "models/role.h"

#define ROLES_COLL_NAME  				"roles"

mongoc_collection_t *roles_collection = NULL;

unsigned int roles_collection_get (void) {

	unsigned int retval = 1;

	roles_collection = mongo_collection_get (ROLES_COLL_NAME);
	if (roles_collection) {
		retval = 0;
	}

	else {
		cerver_log_error ("Failed to get handle to roles collection!");
	}

	return retval;

}

void roles_collection_close (void) {

	if (roles_collection) mongoc_collection_destroy (roles_collection);

}


Role *role_new (void) {

	Role *role = (Role *) malloc (sizeof (Role));
	if (role) {
		(void) memset (role, 0, sizeof (Role));
	}

	return role;

}

void role_delete (void *role_ptr) {

	if (role_ptr) free (role_ptr);

}

Role *role_create (const char *name) {

	Role *role = role_new ();
	if (role) {
		if (name) {
			(void) strncpy (role->name, name, ROLE_NAME_LEN);
		}
	}

	return role;

}

void role_print (Role *role) {

	if (role) {
		(void) printf ("Name: %s\n", role->name);
		if (role->n_actions) {
			(void) printf ("Actions: \n");
			for (unsigned int i = 0; i < role->n_actions; i++) {
				(void) printf ("\t%s\n", role->actions[i]);
			}
		}

		else {
			(void) printf ("No actions!\n");
		}
	}

}

// creates a role bson with all role parameters
bson_t *role_bson_create (Role *role) {

	bson_t *doc = NULL;

	if (role) {
		doc = bson_new ();
		if (doc) {
			bson_oid_init (&role->oid, NULL);
			(void) bson_append_oid (doc, "_id", -1, &role->oid);

			(void) bson_append_utf8 (doc, "name", -1, role->name, -1);
		
			bson_t actions_array = { 0 };
			(void) bson_append_array_begin (doc, "actions", -1, &actions_array);
			if (role->n_actions) {
				char buf[16] = { 0 };
				const char *key = NULL;
				size_t keylen = 0;

				for (unsigned int i = 0; i < role->n_actions; i++) {
					keylen = bson_uint32_to_string (i, &key, buf, sizeof (buf));
					(void) bson_append_utf8 (
						&actions_array, key, (int) keylen, role->actions[i], -1
					);
				}
			}
			(void) bson_append_array_end (doc, &actions_array);
		}
	}

	return doc;

}

static void role_doc_parse_actions (
	Role *role, bson_iter_t *iter
) {

	const u8 *data = NULL;
	u32 len = 0;

	bson_iter_array (iter, &len, &data);

	bson_t *actions_array = bson_new_from_data (data, len);
	if (actions_array) {
		bson_iter_t array_iter = { 0 };
		if (bson_iter_init (&array_iter, actions_array)) {
			while (bson_iter_next (&array_iter)) {
				// const char *key = bson_iter_key (&array_iter);
				const bson_value_t *value = bson_iter_value (&array_iter);

				(void) strncpy (
					role->actions[role->n_actions],
					value->value.v_utf8.str,
					ROLE_ACTION_LEN - 1
				);

				role->n_actions += 1;
			}
		}

		bson_destroy (actions_array);
	}

}

Role *role_doc_parse (const bson_t *role_doc) {

	Role *role = NULL;

	if (role_doc) {
		role = role_create (NULL);

		bson_iter_t iter = { 0 };
		if (bson_iter_init (&iter, role_doc)) {
			while (bson_iter_next (&iter)) {
				const char *key = bson_iter_key (&iter);
				const bson_value_t *value = bson_iter_value (&iter);

				if (!strcmp (key, "_id")) {
					bson_oid_copy (&value->value.v_oid, &role->oid);
				}

				else if (!strcmp (key, "name") && value->value.v_utf8.str) 
					(void) strncpy (
						role->name, value->value.v_utf8.str, ROLE_NAME_LEN - 1
					);

				else if (!strcmp (key, "actions")) {
					role_doc_parse_actions (role, &iter);
				}
			}
		}
	}

	return role;

}

static const bson_t *role_find_by_oid (
	const bson_oid_t *oid, bool actions
) {

	const bson_t *retval = NULL;

	bson_t *role_query = bson_new ();
	if (role_query) {
		(void) bson_append_oid (role_query, "_id", -1, oid);

		CMongoSelect *select = NULL;
		if (!actions) {
			select = cmongo_select_new ();
			(void) cmongo_select_insert_field (
				select,
				cmongo_select_field_create ("name")
			);
		}

		retval = mongo_find_one (roles_collection, role_query, select);

		cmongo_select_delete (select);
	}

	return retval;

}

// gets a role by its oid from the db
// option to select if you want actions or not
Role *role_get_by_oid (
	const bson_oid_t *oid, bool actions
) {

	Role *role = NULL;

	if (oid) {
		const bson_t *role_doc = role_find_by_oid (oid, actions);
		if (role_doc) {
			role = role_doc_parse (role_doc);
			bson_destroy ((bson_t *) role_doc);
		}
	}

	return role;

}

// get a role doc from the db by name
static const bson_t *role_find_by_name (
	const char *name, bool actions
) {

	const bson_t *retval = NULL;

	bson_t *role_query = bson_new ();
	if (role_query) {
		(void) bson_append_utf8 (role_query, "name", -1, name, -1);

		CMongoSelect *select = NULL;
		if (!actions) {
			select = cmongo_select_new ();
			(void) cmongo_select_insert_field (
				select,
				cmongo_select_field_create ("name")
			);
		}

		retval = mongo_find_one (roles_collection, role_query, select);

		cmongo_select_delete (select);
	}

	return retval;    

}

// gets a role form the db by its name
// option to select if you want actions or not
Role *role_get_by_name (const char *name, bool actions) {

	Role *role = NULL;

	if (name) {
		const bson_t *role_doc = role_find_by_name (name, actions);
		if (role_doc) {
			role = role_doc_parse (role_doc);
			// bson_destroy ((bson_t *) role_doc);
		}
	}

	return role;

}

bson_t *role_bson_create_oid_query (const bson_oid_t *oid) {

	bson_t *role_query = NULL;

	if (oid) {
		role_query = bson_new ();
		if (role_query) {
			(void) bson_append_oid (role_query, "_id", -1, oid);
		}
	}

	return role_query;

}

bson_t *role_bson_create_name_query (const char *name) {

	bson_t *role_query = NULL;

	if (name) {
		role_query = bson_new ();
		if (role_query) {
			(void) bson_append_utf8 (role_query, "name", -1, name, -1);
		}
	}

	return role_query;

}

bson_t *role_bson_create_update (Role *role) {

	bson_t *doc = NULL;

	if (role) {
		doc = bson_new ();
		if (doc) {
			bson_t set_doc = { 0 };
			(void) bson_append_document_begin (doc, "$set", -1, &set_doc);

			(void) bson_append_utf8 (&set_doc, "name", -1, role->name, -1);

			bson_t actions_array = { 0 };
			(void) bson_append_array_begin (&set_doc, "actions", -1, &actions_array);
			if (role->n_actions) {
				char buf[16] = { 0 };
				const char *key = NULL;
				size_t keylen = 0;

				for (unsigned int i = 0; i < role->n_actions; i++) {
					keylen = bson_uint32_to_string (i, &key, buf, sizeof (buf));
					(void) bson_append_utf8 (
						&actions_array, key, (int) keylen, role->actions[i], -1
					);
				}
			}
			(void) bson_append_array_end (&set_doc, &actions_array);
			
			(void) bson_append_document_end (doc, &set_doc);
		} 
	}

	return doc;

}