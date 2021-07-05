#ifndef _TEST_ROUTES_VALUES_H_
#define _TEST_ROUTES_VALUES_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/values
// get all the authenticated user's values
extern void test_values_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// POST /api/values
// a user has requested to create a new value
extern void test_value_create_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/values/:id/info
// returns information about an existing value that belongs to a user
extern void test_value_get_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// PUT /api/values/:id/update
// a user wants to update an existing value
extern void test_value_update_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// DELETE /api/values/:id/remove
// deletes an existing user's value
extern void test_value_delete_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif