#ifndef _MODELS_ACTION_H_
#define _MODELS_ACTION_H_

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#define ACTIONS_COLL_NAME  				"actions"

#define ACTION_NAME_SIZE				128
#define ACTION_DESCRIPTION_SIZE			256

extern unsigned int actions_model_init (void);

extern void actions_model_end (void);

struct _RoleAction {

	bson_oid_t oid;

	char name[ACTION_NAME_SIZE];
	char description[ACTION_DESCRIPTION_SIZE];

};

typedef struct _RoleAction RoleAction;

extern RoleAction *action_new (void);

extern void action_delete (void *action_ptr);

extern RoleAction *action_create (
	const char *name, const char *description
);

extern void action_print (const RoleAction *action);

// gets an action form the db by its name
extern RoleAction *action_get_by_name (
	const char *name
);

#endif