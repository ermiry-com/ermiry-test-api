#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <signal.h>

#include <cerver/cerver.h>
#include <cerver/version.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>

#include <cerver/utils/log.h>

#include "test.h"
#include "version.h"

#include "controllers/users.h"

#include "routes/service.h"
#include "routes/users.h"

static Cerver *test_api = NULL;

void end (int dummy) {
	
	if (test_api) {
		cerver_stats_print (test_api, false, false);
		cerver_log_msg ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) test_api->cerver_data);
		cerver_log_line_break ();
		cerver_teardown (test_api);
	}

	(void) test_end ();

	cerver_end ();

	exit (0);

}

static void test_set_service_routes (HttpCerver *http_cerver) {

	/* register top level route */
	// GET /api/test
	HttpRoute *test_route = http_route_create (REQUEST_METHOD_GET, "api/test", test_handler);
	http_cerver_route_register (http_cerver, test_route);

	/* register test children routes */
	// GET api/test/version
	HttpRoute *test_version_route = http_route_create (REQUEST_METHOD_GET, "version", test_version_handler);
	http_route_child_add (test_route, test_version_route);

	// GET api/test/auth
	HttpRoute *test_auth_route = http_route_create (REQUEST_METHOD_GET, "auth", test_auth_handler);
	http_route_set_auth (test_auth_route, HTTP_ROUTE_AUTH_TYPE_BEARER);
	http_route_set_decode_data (test_auth_route, test_user_parse_from_json, test_user_delete);
	http_route_child_add (test_route, test_auth_route);

}

static void test_set_users_routes (HttpCerver *http_cerver) {

	/* register top level route */
	// GET /api/users
	HttpRoute *users_route = http_route_create (REQUEST_METHOD_GET, "api/users", users_handler);
	http_cerver_route_register (http_cerver, users_route);

	/* register users children routes */
	// POST api/users/login
	HttpRoute *users_login_route = http_route_create (REQUEST_METHOD_POST, "login", users_login_handler);
	http_route_child_add (users_route, users_login_route);

	// POST api/users/register
	HttpRoute *users_register_route = http_route_create (REQUEST_METHOD_POST, "register", users_register_handler);
	http_route_child_add (users_route, users_register_route);

}

static void start (void) {

	test_api = cerver_create (
		CERVER_TYPE_WEB,
		"test-api",
		PORT,
		PROTOCOL_TCP,
		false,
		CERVER_CONNECTION_QUEUE
	);

	if (test_api) {
		/*** cerver configuration ***/
		cerver_set_receive_buffer_size (test_api, CERVER_RECEIVE_BUFFER_SIZE);
		cerver_set_thpool_n_threads (test_api, CERVER_TH_THREADS);
		cerver_set_handler_type (test_api, CERVER_HANDLER_TYPE_THREADS);

		cerver_set_reusable_address_flags (test_api, true);

		/*** web cerver configuration ***/
		HttpCerver *http_cerver = (HttpCerver *) test_api->cerver_data;

		http_cerver_auth_set_jwt_algorithm (http_cerver, JWT_ALG_RS256);
		http_cerver_auth_set_jwt_priv_key_filename (http_cerver, PRIV_KEY->str);
		http_cerver_auth_set_jwt_pub_key_filename (http_cerver, PUB_KEY->str);

		test_set_service_routes (http_cerver);

		test_set_users_routes (http_cerver);

		// return not found on any mismatch
		http_cerver_set_not_found_handler (http_cerver);

		if (cerver_start (test_api)) {
			cerver_log_error (
				"Failed to start %s!",
				test_api->info->name->str
			);

			cerver_delete (test_api);
		}
	}

	else {
		cerver_log_error ("Failed to create cerver!");

		cerver_delete (test_api);
	}

}

int main (int argc, char const **argv) {

	srand ((unsigned int) time (NULL));

	// register to the quit signal
	(void) signal (SIGINT, end);
	(void) signal (SIGTERM, end);

	// to prevent SIGPIPE when writting to socket
	(void) signal (SIGPIPE, SIG_IGN);

	cerver_init ();

	cerver_version_print_full ();

	cerver_log_line_break ();
	test_version_print_full ();
	cerver_log_line_break ();

	if (!test_init ()) {
		start ();
	}

	else {
		cerver_log_error ("Failed to init test!");
	}

	(void) test_end ();

	cerver_end ();

	return 0;

}