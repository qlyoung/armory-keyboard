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

struct Layout *layout;

void set_layout(struct Layout *lo) {
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

    const struct Keycode *match = map_codepoint(input, layout, ic < numescape);
    if (match == NULL) return -1;
    if (match->id != 0x00)
      report[index++] = match->id;
    report[0] |= match->mod;
  }
  va_end(chars);

  return 0;
}

int make_hid_report_arr(char *report, int numescape, int argc, uint32_t *codepoints) {
  // nullcheck
  if (codepoints == NULL) return -1;

  // i'm so sorry
  switch (argc) {
    case 1:
      return make_hid_report(report, numescape, argc, codepoints[0]);
    case 2:
      return make_hid_report(report, numescape, argc, codepoints[0], codepoints[1]);
    case 3:
      return make_hid_report(report, numescape, argc, codepoints[0], codepoints[1], codepoints[2]);
    case 4:
      return make_hid_report(report, numescape, argc, codepoints[0], codepoints[1], codepoints[2], codepoints[3]);
    case 5:
      return make_hid_report(report, numescape, argc, codepoints[0], codepoints[1], codepoints[2], codepoints[3], codepoints[4]);
    case 6:
    default:
      return make_hid_report(report, numescape, argc, codepoints[0], codepoints[1], codepoints[2], codepoints[3], codepoints[4], codepoints[5]);
  }
}

