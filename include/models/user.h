#ifndef _MODELS_USER_H_
#define _MODELS_USER_H_

#include <stdbool.h>

#include <time.h>

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#define USERS_COLL_NAME				"users"

#define USER_ID_SIZE				32
#define USER_NAME_SIZE				128
#define USER_EMAIL_SIZE				128
#define USER_USERNAME_SIZE			128
#define USER_PASSWORD_SIZE			128
#define USER_ROLE_SIZE				64
#define USER_DATE_BUFFER_SIZE		128

extern unsigned int users_model_init (void);

extern void users_model_end (void);

typedef struct User {

	// user's unique id
	bson_oid_t oid;
	char id[USER_ID_SIZE];

	// main user values
	unsigned int name_len;
	char name[USER_NAME_SIZE];

	unsigned int email_len;
	char email[USER_EMAIL_SIZE];

	unsigned int username_len;
	char username[USER_USERNAME_SIZE];

	unsigned int password_len;
	char password[USER_PASSWORD_SIZE];

	bson_oid_t role_oid;
	char role[USER_ROLE_SIZE];

	bool first_time;

	// when the user was added
	time_t date;
	char date_str[USER_DATE_BUFFER_SIZE];

	// used to validate JWT expiration
	time_t iat;

} User;

extern void *user_new (void);

extern void user_delete (void *user_ptr);

extern void user_print (const User *user);

extern unsigned int user_get_by_oid (
	User *user,
	const bson_oid_t *oid, const bson_t *query_opts
);

extern bool user_check_by_email (
	const char *email
);

extern unsigned int user_get_by_email (
	User *user, const char *email, const bson_t *query_opts
);

extern unsigned int user_insert_one (const User *user);

extern unsigned int user_add_value (const User *user);

#endif