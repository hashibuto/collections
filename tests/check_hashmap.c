#include <stdlib.h>
#include <check.h>
#include "../src/collections.h"

START_TEST(test_hashmap_create_and_destroy)
{
    struct HashMap *map = hashmap_new();
    hashmap_free(map);
}
END_TEST

START_TEST(test_hashmap_add_and_remove_string_elements)
{
    struct HashMap *map = hashmap_new();
    hashmap_put_str(map, "key_001", "hello_key_001");
    // hashmap_put_str(map, "key_002", "hello_key_002");
    // ck_assert_int_eq(map->size, 2);
    // char *value = hashmap_get(map, "key_001");
    // ck_assert_ptr_nonnull(value);
    // ck_assert_str_eq(value, "hello_key_001");
    hashmap_free(map);
}
END_TEST

Suite *hashmap_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("hashmap");
    tc_core = tcase_create("core");

    tcase_add_test(tc_core, test_hashmap_create_and_destroy);
    tcase_add_test(tc_core, test_hashmap_add_and_remove_string_elements);
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