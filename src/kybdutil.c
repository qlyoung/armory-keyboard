/*
 * Utility functions for generating USB HID keyboard reports.
 *
 * Authors:
 *  Quentin Young (qlyoung AT qlyoung.net)
 *  Collin Mulliner (collin AT mulliner.org)
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include "kybdutil.h"

layout_t *layout;

void set_layout(layout_t *lo) {
  layout = lo;
}

int make_hid_report(char *report, int numescape, int argc, ...) {
  va_list chars;
  va_start(chars, argc);

  int index = 2;

  for (int ic = 0; ic < argc; ic++) {
    // assert report index is in range
    assert(index < 7);

    uint32_t input = (uint32_t) va_arg(chars, int);

    // if processing escapes, search for character in escape table
    if (ic < numescape) {
      const keycode_t *match = map_codepoint(input, layout, true);
      if (match == NULL) return -1;
      if (match->id != 0x00)
        report[index++] = match->id;
      report[0] |= match->mod;
    }
    else {
      const keycode_t *match = map_codepoint(input, layout, false);
      if (match == NULL) return -1;
      report[index++] = match->id;
      report[0] |= match->mod;
    }
  }
  va_end(chars);

  return 0;
}

int make_hid_report_arr(char *report, int numescape, int argc, char *chars) {
  // nullcheck
  if (chars == NULL) return -1;

  // i'm so sorry
  switch (argc) {
    case 1:
      return make_hid_report(report, numescape, argc, chars[0]);
    case 2:
      return make_hid_report(report, numescape, argc, chars[0], chars[1]);
    case 3:
      return make_hid_report(report, numescape, argc, chars[0], chars[1], chars[2]);
    case 4:
      return make_hid_report(report, numescape, argc, chars[0], chars[1], chars[2], chars[3]);
    case 5:
      return make_hid_report(report, numescape, argc, chars[0], chars[1], chars[2], chars[3], chars[4]);
    case 6:
    default:
      return make_hid_report(report, numescape, argc, chars[0], chars[1], chars[2], chars[3], chars[4], chars[5]);
  }
}

