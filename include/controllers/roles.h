#ifndef _TEST_ROLES_H_
#define _TEST_ROLES_H_

#include <stdbool.h>

#include <bson/bson.h>

#include "models/role.h"

extern const Role *common_role;

extern unsigned int test_roles_init (void);

extern void test_roles_end (void);

extern const Role *test_roles_get_by_oid (
	const bson_oid_t *role_oid
);

extern const Role *test_roles_get_by_name (
	const char *role_name
);

extern const char *test_roles_name_get_by_oid (
	const bson_oid_t *role_oid
);

extern bool test_role_search_and_check_action (
	const char *role_name, const char *action_name
);

#endif