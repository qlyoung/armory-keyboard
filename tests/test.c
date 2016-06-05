/**
 * Unit tests for kybdutil.c
 */

#define DEFAULT_LAYOUT "test.layout"

#include "unity.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "kybdutil.h"
#include "layouts.h"
#include "unicode.h"

char *report;
struct Layout *lo;

void setUp() {
  report = calloc((size_t) 8, (size_t) 1);
  lo = load_layout(fopen(DEFAULT_LAYOUT, "r"));
  set_layout(lo);
}

void tearDown() {
  free(report);
  destroy_layout(lo);
}


// make sure layout file actually loaded
void test_layout_file_loaded() {
  TEST_ASSERT_NOT_NULL(lo);
}

// test if set_layout has not been called, fail always
void test_all_fail_when_no_layout_set() {
  int index = 0;
  char a = 'a';
  uint32_t codepoint = getCodepoint(&a, &index);

  set_layout(NULL);

  TEST_ASSERT_EQUAL(-1, make_hid_report_arr(report, 0, 1, &codepoint));
  TEST_ASSERT_EQUAL(-1, make_hid_report(report, 0, 1, codepoint));
}

/** basic parameter testing*/
void test_make_hid_report_arr_nullargs_fails() {
  int result = make_hid_report_arr(report, 0, 1, NULL);
  TEST_ASSERT_EQUAL_INT(-1, result);
  result = make_hid_report_arr(report, 1, 1, NULL);
  TEST_ASSERT_EQUAL_INT(-1, result);
}

void test_make_hid_report_one_char() {
  // test report generation for all codepoints in layout
  for (int i = 0; i < lo->size; i++) {
    // clear report
    memset(report, 0x00, (size_t) 8);

    // retrieve expected values
    char id  = lo->map[i]->id;
    char mod = lo->map[i]->mod;

    // get codepoint to encode
    uint32_t codepoint = lo->map[i]->ch;

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

void test_make_hid_report_two_chars() {
  // test report generation for all codepoints in layout
  for (int i = 0; i < lo->size; i++) {
    // clear report
    memset(report, 0x00, (size_t) 8);

    // retrieve expected values
    char id  = lo->map[i]->id;
    char mod = lo->map[i]->mod;

    // get codepoint to encode
    uint32_t codepoint = lo->map[i]->ch;

    make_hid_report(report, 0, 2, codepoint, codepoint);
    TEST_ASSERT_EQUAL(mod,  report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00, report[1]); // reserved
    TEST_ASSERT_EQUAL(id,   report[2]); // usage id
    TEST_ASSERT_EQUAL(id,   report[3]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[4]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[5]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[6]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[7]); // usage id
  }
}


void test_make_hid_report_three_chars() {
  // test report generation for all codepoints in layout
  for (int i = 0; i < lo->size; i++) {
    // clear report
    memset(report, 0x00, (size_t) 8);

    // retrieve expected values
    char id  = lo->map[i]->id;
    char mod = lo->map[i]->mod;

    // get codepoint to encode
    uint32_t codepoint = lo->map[i]->ch;

    make_hid_report(report, 0, 3, codepoint, codepoint, codepoint);
    TEST_ASSERT_EQUAL(mod,  report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00, report[1]); // reserved
    TEST_ASSERT_EQUAL(id,   report[2]); // usage id
    TEST_ASSERT_EQUAL(id,   report[3]); // usage id
    TEST_ASSERT_EQUAL(id,   report[4]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[5]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[6]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[7]); // usage id
  }
}

void test_make_hid_report_four_chars() {
  // test report generation for all codepoints in layout
  for (int i = 0; i < lo->size; i++) {
    // clear report
    memset(report, 0x00, (size_t) 8);

    // retrieve expected values
    char id  = lo->map[i]->id;
    char mod = lo->map[i]->mod;

    // get codepoint to encode
    uint32_t codepoint = lo->map[i]->ch;

    make_hid_report(report, 0, 4, codepoint, codepoint, codepoint, codepoint);
    TEST_ASSERT_EQUAL(mod,  report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00, report[1]); // reserved
    TEST_ASSERT_EQUAL(id,   report[2]); // usage id
    TEST_ASSERT_EQUAL(id,   report[3]); // usage id
    TEST_ASSERT_EQUAL(id,   report[4]); // usage id
    TEST_ASSERT_EQUAL(id,   report[5]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[6]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[7]); // usage id
  }
}

void test_make_hid_report_five_chars() {
  // test report generation for all codepoints in layout
  for (int i = 0; i < lo->size; i++) {
    // clear report
    memset(report, 0x00, (size_t) 8);

    // retrieve expected values
    char id  = lo->map[i]->id;
    char mod = lo->map[i]->mod;

    // get codepoint to encode
    uint32_t codepoint = lo->map[i]->ch;

    make_hid_report(report, 0, 5, codepoint, codepoint, codepoint, codepoint, codepoint);
    TEST_ASSERT_EQUAL(mod,  report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00, report[1]); // reserved
    TEST_ASSERT_EQUAL(id,   report[2]); // usage id
    TEST_ASSERT_EQUAL(id,   report[3]); // usage id
    TEST_ASSERT_EQUAL(id,   report[4]); // usage id
    TEST_ASSERT_EQUAL(id,   report[5]); // usage id
    TEST_ASSERT_EQUAL(id,   report[6]); // usage id
    TEST_ASSERT_EQUAL(0x00, report[7]); // usage id
  }
}

void test_make_hid_report_six_chars() {
  // test report generation for all codepoints in layout
  for (int i = 0; i < lo->size; i++) {
    // clear report
    memset(report, 0x00, (size_t) 8);

    // retrieve expected values
    char id  = lo->map[i]->id;
    char mod = lo->map[i]->mod;

    // get codepoint to encode
    uint32_t codepoint = lo->map[i]->ch;

    make_hid_report(report, 0, 6, codepoint, codepoint, codepoint, codepoint, codepoint, codepoint);
    TEST_ASSERT_EQUAL(mod,  report[0]); // modifier
    TEST_ASSERT_EQUAL(0x00, report[1]); // reserved
    TEST_ASSERT_EQUAL(id,   report[2]); // usage id
    TEST_ASSERT_EQUAL(id,   report[3]); // usage id
    TEST_ASSERT_EQUAL(id,   report[4]); // usage id
    TEST_ASSERT_EQUAL(id,   report[5]); // usage id
    TEST_ASSERT_EQUAL(id,   report[6]); // usage id
    TEST_ASSERT_EQUAL(id,   report[7]); // usage id
  }
}


int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_layout_file_loaded);
  RUN_TEST(test_all_fail_when_no_layout_set);
  RUN_TEST(test_make_hid_report_arr_nullargs_fails);
  RUN_TEST(test_make_hid_report_one_char);
  RUN_TEST(test_make_hid_report_two_chars);
  RUN_TEST(test_make_hid_report_three_chars);
  RUN_TEST(test_make_hid_report_four_chars);
  RUN_TEST(test_make_hid_report_five_chars);
  RUN_TEST(test_make_hid_report_six_chars);
  return UNITY_END();
}
