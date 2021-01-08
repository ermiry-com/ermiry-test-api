#ifndef _TEST_CONTROLLERS_ROLES_H_
#define _TEST_CONTROLLERS_ROLES_H_

#include <bson/bson.h>

#include "models/role.h"

extern const Role *common_role;

extern unsigned int test_roles_init (void);

extern void test_roles_end (void);

extern const char *test_roles_get_by_oid (
	const bson_oid_t *role_oid
);

#endif