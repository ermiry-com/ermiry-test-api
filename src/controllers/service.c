#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/http/response.h>

#include <cerver/utils/utils.h>

#include "version.h"

HttpResponse *missing_values = NULL;

HttpResponse *test_works = NULL;
HttpResponse *current_version = NULL;

HttpResponse *catch_all = NULL;

unsigned int test_service_init (void) {

	unsigned int retval = 1;

	missing_values = http_response_json_key_value (
		HTTP_STATUS_BAD_REQUEST, "error", "Missing values!"
	);

	test_works = http_response_json_key_value (
		HTTP_STATUS_OK, "msg", "Test API works!"
	);

	char *status = c_string_create (
		"%s - %s", TEST_VERSION_NAME, TEST_VERSION_DATE
	);

	if (status) {
		current_version = http_response_json_key_value (
			HTTP_STATUS_OK, "version", status
		);

		free (status);
	}

	catch_all = http_response_json_key_value (
		HTTP_STATUS_OK, "msg", "Test API Service!"
	);

	if (
		missing_values
		&& test_works && current_version
		&& catch_all
	) retval = 0;

	return retval;

}

void test_service_end (void) {

	http_response_delete (missing_values);

	http_response_delete (test_works);
	http_response_delete (current_version);

	http_response_delete (catch_all);

}
