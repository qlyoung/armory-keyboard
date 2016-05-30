#include "unity.h"
#include <stdlib.h>
#include <ctype.h>
#include "kybdutil.h"

char *report;
char *nullChar;
char oneChar;

void setUp() {
  report = calloc((size_t) 8, (size_t) 1);
  oneChar = 'a';
  nullChar = NULL;
}

void tearDown() {
  free(report);
}

/** basic parameter testing*/
void test_make_hid_report_nullargs_fails() {
  int result = make_hid_report_arr(report, 0, 1, nullChar);
  TEST_ASSERT_EQUAL_INT(-1, result);

  result = make_hid_report_arr(report, 1, 1, nullChar);
  TEST_ASSERT_EQUAL_INT(-1, result);
}

/** single lowercase char report testing, a-z */
void test_make_hid_report_one_lowercase_alpha() {
  for (char c = 'a'; c <= 'z'; c++) {
    make_hid_report(report, 0, 1, c);
    char expected = c - ('a' - 4);
    TEST_ASSERT_EQUAL(0x00, report[0]);
    TEST_ASSERT_EQUAL(0x00, report[1]);
    TEST_ASSERT_EQUAL(expected, report[2]);
    TEST_ASSERT_EQUAL(0x00, report[3]);
    TEST_ASSERT_EQUAL(0x00, report[4]);
    TEST_ASSERT_EQUAL(0x00, report[5]);
    TEST_ASSERT_EQUAL(0x00, report[6]);
    TEST_ASSERT_EQUAL(0x00, report[7]);
  }
}

/** single uppercase char report testing, a-z */
void test_make_hid_report_one_uppercase_alpha() {
  for (char c = 'A'; c <= 'Z'; c++) {
    make_hid_report(report, 0, 1, c);
    char expected = tolower(c) - ('a' - 4);
    TEST_ASSERT_EQUAL(0x22, report[0]);
    TEST_ASSERT_EQUAL(0x00, report[1]);
    TEST_ASSERT_EQUAL(expected, report[2]);
    TEST_ASSERT_EQUAL(0x00, report[3]);
    TEST_ASSERT_EQUAL(0x00, report[4]);
    TEST_ASSERT_EQUAL(0x00, report[5]);
    TEST_ASSERT_EQUAL(0x00, report[6]);
    TEST_ASSERT_EQUAL(0x00, report[7]);
  }
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
  RUN_TEST(test_make_hid_report_nullargs_fails);
  RUN_TEST(test_make_hid_report_one_lowercase_alpha);
  RUN_TEST(test_make_hid_report_one_uppercase_alpha);
  RUN_TEST(test_make_hid_report_one_numeric);
  RUN_TEST(test_make_hid_report_one_symbolic);
  RUN_TEST(test_make_hid_report_one_escape);
  return UNITY_END();
}
