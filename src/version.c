#include <cerver/utils/log.h>

#include "version.h"

// print full pocket version information
void test_version_print_full (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Test API Version: %s", TEST_VERSION_NAME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Release Date & time: %s - %s", TEST_VERSION_DATE, TEST_VERSION_TIME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Author: %s\n", TEST_VERSION_AUTHOR
	);


}

// print the version id
void test_version_print_version_id (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Test API Version ID: %s\n", TEST_VERSION
	);

}

// print the version name
void test_version_print_version_name (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Test API Version: %s\n", TEST_VERSION_NAME
	);

}