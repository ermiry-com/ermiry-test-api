#ifndef _TEST_ROUTES_SERVICE_H_
#define _TEST_ROUTES_SERVICE_H_

// GET /api/test
extern void test_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/test/version
extern void test_version_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/test/auth
extern void test_auth_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif