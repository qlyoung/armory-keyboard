/**
 * Unit tests for kybdutil.c
 */

#define DEFAULT_LAYOUT "test.layout"

#include "unity.h"
#include <stdlib.h>
#include <ctype.h>
#include "kybdutil.h"
#include "layouts.h"
#include "unicode.h"

char *report;
struct Layout *layout;

void setUp() {
  report = calloc((size_t) 8, (size_t) 1);
  layout = load_layout(fopen(DEFAULT_LAYOUT, "r"));
}

void tearDown() {
  free(report);
  destroy_layout(layout);
}


// make sure layout file actually loaded
void test_layout_file_loaded() {
  TEST_ASSERT_NOT_NULL(layout);
}

/** basic parameter testing*/
void test_make_hid_report_arr_nullargs_fails() {
  int result = make_hid_report_arr(report, 0, 1, NULL);
  TEST_ASSERT_EQUAL_INT(-1, result);
  result = make_hid_report_arr(report, 1, 1, NULL);
  TEST_ASSERT_EQUAL_INT(-1, result);
}

/** single lowercase ascii hid report testing, a-z */
void test_make_hid_report_one_lowercase_ascii_alpha() {
  for (char c = 'a'; c <= 'z'; c++) {

    // not strictly necessary, but this is correct API usage
    int index = 0;
    uint32_t codepoint = getCodepoint(&c, &index);

    // lookup expected values in layout
    const struct Keycode *expected = map_codepoint(codepoint, layout, false);
    char id  = expected->id;
    char mod = expected->mod;

    make_hid_report(report, 0, 1, codepoint);
    TEST_ASSERT_EQUAL(mod,      report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00,     report[1]); // reserved
    TEST_ASSERT_EQUAL(id,       report[2]); // usage id
    TEST_ASSERT_EQUAL(0x00,     report[3]); // usage id
    TEST_ASSERT_EQUAL(0x00,     report[4]); // usage id
    TEST_ASSERT_EQUAL(0x00,     report[5]); // usage id
    TEST_ASSERT_EQUAL(0x00,     report[6]); // usage id
    TEST_ASSERT_EQUAL(0x00,     report[7]); // usage id
  }
}

/** single uppercase char report testing, a-z */
void test_make_hid_report_one_uppercase_ascii_alpha() {
  for (char c = 'A'; c <= 'Z'; c++) {

    // not strictly necessary, but this is correct API usage
    int index = 0;
    uint32_t codepoint = getCodepoint(&c, &index);

    // lookup expected values in layout
    const struct Keycode *expected = map_codepoint(codepoint, layout, false);
    char id  = expected->id;
    char mod = expected->mod;

    // validate report
    make_hid_report(report, 0, 1, codepoint);
    TEST_ASSERT_EQUAL(mod,  report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00, report[1]); // reserved
    TEST_ASSERT_EQUAL(id,   report[2]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[3]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[4]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[5]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[6]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[7]); // usage id
  }
}

void test_make_hid_report_one_unicode_alpha() {
  TEST_IGNORE();
}

void test_make_hid_report_one_numeric() {
  TEST_IGNORE();
}

void test_make_hid_report_one_symbolic() {
  TEST_IGNORE();
}

void test_make_hid_report_one_escape() {
  TEST_IGNORE();
}


int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_layout_file_loaded);
  RUN_TEST(test_make_hid_report_arr_nullargs_fails);
  RUN_TEST(test_make_hid_report_one_lowercase_ascii_alpha);
  RUN_TEST(test_make_hid_report_one_uppercase_ascii_alpha);
  RUN_TEST(test_make_hid_report_one_numeric);
  RUN_TEST(test_make_hid_report_one_symbolic);
  RUN_TEST(test_make_hid_report_one_escape);
  return UNITY_END();
}
