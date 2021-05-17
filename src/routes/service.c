#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/types/types.h>

#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include <cerver/utils/utils.h>
#include <cerver/utils/log.h>

#include "test.h"

#include "models/user.h"

#include "controllers/service.h"

// GET /api/test
void test_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (test_works, http_receive);

}

// GET /api/test/version
void test_version_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (current_version, http_receive);

}

// GET /api/test/auth
void test_auth_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	User *user = (User *) request->decoded_data;

	if (user) {
		#ifdef ERMIRY_DEBUG
		user_print (user);
		#endif

		(void) http_response_send (oki_doki, http_receive);
	}

	else {
		(void) http_response_send (bad_user_error, http_receive);
	}

}

// GET *
void test_catch_all_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	http_response_send (catch_all, http_receive);

}
