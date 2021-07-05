#include <cerver/handler.h>

#include <cerver/http/http.h>
#include <cerver/http/response.h>

#include "errors.h"
#include "test.h"

#include "controllers/service.h"

const char *test_error_to_string (const TestError type) {

	switch (type) {
		#define XX(num, name, string) case TEST_ERROR_##name: return #string;
		TEST_ERROR_MAP(XX)
		#undef XX
	}

	return test_error_to_string (TEST_ERROR_NONE);

}

void test_error_send_response (
	const TestError error,
	const HttpReceive *http_receive
) {

	switch (error) {
		case TEST_ERROR_NONE: break;

		case TEST_ERROR_BAD_REQUEST:
			(void) http_response_send (bad_request_error, http_receive);
			break;

		case TEST_ERROR_MISSING_VALUES:
			(void) http_response_send (missing_values, http_receive);
			break;

		case TEST_ERROR_BAD_USER:
			(void) http_response_send (bad_user_error, http_receive);
			break;

		case TEST_ERROR_NOT_FOUND:
			(void) http_response_send (not_found_error, http_receive);
			break;

		case TEST_ERROR_SERVER_ERROR:
			(void) http_response_send (server_error, http_receive);
			break;

		default: break;
	}

}