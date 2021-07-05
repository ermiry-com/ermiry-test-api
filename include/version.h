#ifndef _TEST_VERSION_H_
#define _TEST_VERSION_H_

#define TEST_VERSION					"0.2"
#define TEST_VERSION_NAME				"Version 0.2"
#define TEST_VERSION_DATE				"28/05/2021"
#define TEST_VERSION_TIME				"04:40 CST"
#define TEST_VERSION_AUTHOR				"Erick Salas"

// print full pocket version information
extern void test_version_print_full (void);

// print the version id
extern void test_version_print_version_id (void);

// print the version name
extern void test_version_print_version_name (void);

#endif