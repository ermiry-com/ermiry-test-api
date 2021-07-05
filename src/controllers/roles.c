#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cmongo/crud.h>
#include <cmongo/select.h>

#include <cerver/collections/dlist.h>

#include <cerver/utils/log.h>

#include "models/role.h"

static DoubleList *roles = NULL;

const Role *common_role = NULL;

const Role *test_roles_get_by_name (
	const char *role_name
);

static unsigned int test_roles_init_get_roles (void) {

	unsigned int retval = 1;

	CMongoSelect *select = cmongo_select_new ();
	cmongo_select_insert_field (select, "name");
	cmongo_select_insert_field (select, "actions");

	roles = roles_get_all (select);
	if (roles) {
		cerver_log_success ("Got roles from db!");

		retval = 0;
	}

	else {
		cerver_log_error ("Failed to get roles from db!");
	}

	cmongo_select_delete (select);

	return retval;

}

static unsigned int test_roles_init_get_common (void) {

	unsigned int retval = 1;

	Role role_query = { 0 };
	(void) strncpy (role_query.name, "common", ROLE_NAME_SIZE - 1);

	common_role = (const Role *) dlist_search (
		roles, &role_query, role_comparator_by_name
	);

	if (common_role) {
		retval = 0;
	}

	else {
		cerver_log_error ("Failed to get common role!");
	}

	return retval;

}

unsigned int test_roles_init (void) {

	unsigned int retval = 1;

	roles = dlist_init (role_delete, NULL);
	if (!test_roles_init_get_roles ()) {
		if (!test_roles_init_get_common ()) {
			retval = 0;
		}
	}

	return retval;

}

void test_roles_end (void) {

	dlist_delete (roles);

}

const Role *test_roles_get_by_oid (
	const bson_oid_t *role_oid
) {

	const Role *retval = NULL;

	if (role_oid) {
		for (ListElement *le = dlist_start (roles); le; le = le->next) {
			if (!bson_oid_compare (&((Role *) le->data)->oid, role_oid)) {
				retval = ((Role *) le->data);
				break;
			}
		}
	}

	return retval;

}

const Role *test_roles_get_by_name (
	const char *role_name
) {

	const Role *retval = NULL;

	if (role_name) {
		for (ListElement *le = dlist_start (roles); le; le = le->next) {
			if (!strcmp (((Role *) le->data)->name, role_name)) {
				retval = ((Role *) le->data);
				break;
			}
		}
	}

	return retval;

}

const char *test_roles_name_get_by_oid (
	const bson_oid_t *role_oid
) {

	const char *retval = NULL;

	const Role *role = test_roles_get_by_oid (role_oid);
	if (role) {
		retval = role->name;
	}

	return retval;

}

bool test_role_search_and_check_action (
	const char *role_name, const char *action_name
) {

	bool retval = false;

	if (role_name && action_name) {
		Role *role = NULL;
		for (ListElement *le = dlist_start (roles); le; le = le->next) {
			role = (Role *) le->data;
			if (!strcmp (role->name, role_name)) {
				// search action in role
				for (unsigned int idx = 0; idx < role->n_actions; idx++) {
					if (!strcmp (role->actions[idx], action_name)) {
						retval = true;
						break;
					}
				}

				break;
			}
		}
	}

	return retval;

}
