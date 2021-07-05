#ifndef _TEST_SERVICE_H_
#define _TEST_SERVICE_H_

struct _HttpResponse;

extern struct _HttpResponse *existing_value;
extern struct _HttpResponse *missing_values;

extern struct _HttpResponse *test_works;
extern struct _HttpResponse *current_version;

extern struct _HttpResponse *catch_all;

extern unsigned int test_service_init (void);

extern void test_service_end (void);

#endif