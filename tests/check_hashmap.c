#include <stdlib.h>
#include <check.h>
#include "../src/collections.h"

START_TEST(test_hashmap_create)
{
    struct HashMap *map = hashmap_new();
    hashmap_free(map);
}
END_TEST

Suite *hashmap_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("hashmap");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_hashmap_create);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = hashmap_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}