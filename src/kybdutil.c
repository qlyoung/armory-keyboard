/*
 * Utility functions for generating USB HID keyboard reports.
 *
 * Authors:
 *  Quentin Young (qlyoung AT qlyoung.net)
 *  Collin Mulliner (collin AT mulliner.org)
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include "kybdutil.h"

// some error codes
#define ERR_BAD_ESCAPE_CNT "Bad escape count"
#define ERR_BAD_CHAR_CNT "Bad character count"
#define ERR_UNKNOWN_ESCAPE "Unknown escape character"
#define ERR_UNKNOWN_CHAR "Keycode unknown for character"


/**
 * Keycode struct. Holds a character, its HID Usage ID, and
 * a bit field that indicates what bits of the modifier byte
 * must be set to produce it.
 *
 * Used to map keyboard characters to their keycodes.
 */
struct key_t {
  // literal character or escape code
  char k;
  // HID Usage ID / keycode for the character
  char c;
  // modifier bitfield for this character
  char mod;
};

/** Table of key_t for digital keys */
static struct key_t keys_num[] = {
  {.k = '0', .c = 0x27, .mod=0x00},
  {.k = '1', .c = 0x1e, .mod=0x00},
  {.k = '2', .c = 0x1f, .mod=0x00},
  {.k = '3', .c = 0x20, .mod=0x00},
  {.k = '4', .c = 0x21, .mod=0x00},
  {.k = '5', .c = 0x22, .mod=0x00},
  {.k = '6', .c = 0x23, .mod=0x00},
  {.k = '7', .c = 0x24, .mod=0x00},
  {.k = '8', .c = 0x25, .mod=0x00},
  {.k = '9', .c = 0x26, .mod=0x00},
};

/** Table of key_t for symbolic keys */
static struct key_t keys_symbol[] = {
  {.k = '!', .c = 0x1e, .mod=0x20},
  {.k = '@', .c = 0x1f, .mod=0x20},
  {.k = '#', .c = 0x20, .mod=0x20},
  {.k = '$', .c = 0x21, .mod=0x20},
  {.k = '%', .c = 0x22, .mod=0x20},
  {.k = '^', .c = 0x23, .mod=0x20},
  {.k = '&', .c = 0x24, .mod=0x20},
  {.k = '*', .c = 0x25, .mod=0x20},
  {.k = '(', .c = 0x26, .mod=0x20},
  {.k = ')', .c = 0x27, .mod=0x20},
  {.k = '-', .c = 0x2d, .mod=0x00},
  {.k = '_', .c = 0x2d, .mod=0x20},
  {.k = '+', .c = 0x2e, .mod=0x20},
  {.k = '=', .c = 0x2e, .mod=0x00},
  {.k = '[', .c = 0x2f, .mod=0x00},
  {.k = '{', .c = 0x2f, .mod=0x20},
  {.k = ']', .c = 0x30, .mod=0x00},
  {.k = '}', .c = 0x30, .mod=0x20},
  {.k ='\\', .c = 0x31, .mod=0x00},
  {.k = '|', .c = 0x31, .mod=0x20},
  {.k = ';', .c = 0x33, .mod=0x00},
  {.k = ':', .c = 0x33, .mod=0x20},
  {.k ='\'', .c = 0x34, .mod=0x00},
  {.k = '"', .c = 0x34, .mod=0x20},
  {.k = ',', .c = 0x36, .mod=0x00},
  {.k = '<', .c = 0x36, .mod=0x20},
  {.k = '.', .c = 0x37, .mod=0x00},
  {.k = '>', .c = 0x37, .mod=0x20},
  {.k = '/', .c = 0x38, .mod=0x00},
  {.k = '?', .c = 0x38, .mod=0x20},
  {.k = '`', .c = 0x35, .mod=0x00},
  {.k = '~', .c = 0x35, .mod=0x20},
  {.k = ' ', .c = 0x2c, .mod=0x00},
  {.k = 0x0, .c = 0x00, .mod=0x00}
};

/** Table of key_t for keys with escape characters */
static struct key_t keys_escape[] = {
  // escape codes
  {.k = ALT,      .c = 0x00, .mod=0x04}, // alt
  {.k = BACKSPACE,.c = 0x2A, .mod=0x00}, // backspace
  {.k = CONTROL,  .c = 0x00, .mod=0x01}, // ctrl
  {.k = DELETE,   .c = 0x4C, .mod=0x00}, // delete
  {.k = ESCAPE,   .c = 0x29, .mod=0x00}, // esc
  {.k = END,      .c = 0x4D, .mod=0x00}, // end
  {.k = GUI,      .c = 0x00, .mod=0x08}, // gui/win
  {.k = HOME,     .c = 0x4A, .mod=0x00}, // home
  {.k = INSERT,   .c = 0x49, .mod=0x00}, // insert
  {.k = DARROW,   .c = 0x51, .mod=0x00}, // down arrow
  {.k = UARROW,   .c = 0x52, .mod=0x00}, // up arrow
  {.k = LARROW,   .c = 0x50, .mod=0x00}, // left arrow
  {.k = RARROW,   .c = 0x4F, .mod=0x00}, // right arrow
  {.k = ENTER,    .c = 0x28, .mod=0x00}, // enter
  {.k = SPACE,    .c = 0x2C, .mod=0x00}, // space (helps with parsing)
  {.k = PRNTSCRN, .c = 0x46, .mod=0x00}, // printscreen
  {.k = SCRLLCK,  .c = 0x47, .mod=0x00}, // scroll lock
  {.k = MENU,     .c = 0x76, .mod=0x00}, // menu
  {.k = SHIFT,    .c = 0x00, .mod=0x02}, // shift
  {.k = TAB,      .c = 0x2B, .mod=0x00}, // tab
  {.k = CAPSLOCK, .c = 0x39, .mod=0x00}, // capslock
  {.k = PAUSE,    .c = 0x48, .mod=0x00}, // pause
  {.k = NUMLOCK,  .c = 0x53, .mod=0x00}, // numlock (keypad)
  {.k = PAGEDOWN, .c = 0x4E, .mod=0x00}, // page down
  {.k = PAGEUP,   .c = 0x4B, .mod=0x00}, // page up
  {.k = CLEAR,    .c = 0x9C, .mod=0x00}, // clear
  {.k = F1,       .c = 0x3A, .mod=0x00}, // F1
  {.k = F2,       .c = 0x3B, .mod=0x00}, // F2
  {.k = F3,       .c = 0x3C, .mod=0x00}, // F3
  {.k = F4,       .c = 0x3D, .mod=0x00}, // F4
  {.k = F5,       .c = 0x3E, .mod=0x00}, // F5
  {.k = F6,       .c = 0x3F, .mod=0x00}, // F6
  {.k = F7,       .c = 0x40, .mod=0x00}, // F7
  {.k = F8,       .c = 0x41, .mod=0x00}, // F8
  {.k = F9,       .c = 0x42, .mod=0x00}, // F9
  {.k = F10,      .c = 0x43, .mod=0x00}, // F10
  {.k = F11,      .c = 0x44, .mod=0x00}, // F11
  {.k = F12,      .c = 0x45, .mod=0x00}, // F12
  {.k = 0x0,      .c = 0x00, .mod=0x00}
};

/**
 * Searches an array of key_t[] looking for the key_t associated with
 * the given character.
 * @param[in] keychar the character to search for
 * @param[in] table the key_t array to search in
 * @return constant pointer to the associated key_t, or NULL if it was
 * not found
 */
const struct key_t *find_key(char keychar, struct key_t table[]) {
  // linear search; could be changed to bsearch with ordered keycodes
  for (int i = 0; table[i].k != 0; i++) {
    if (keychar == table[i].k)
      return &table[i];
  }
  return NULL;
}

int make_hid_report(char *report, int numescape, int argc, ...) {
  // sanity checks
  if (argc < 1) {
    fprintf(stderr, "[!] "ERR_BAD_CHAR_CNT": %d\n", argc);
    return -1;
  }
  if (numescape < 0) {
    fprintf(stderr, "[!] "ERR_BAD_ESCAPE_CNT": %d\n", numescape);
    return -1;
  }

  va_list chars;
  va_start(chars, argc);

  int index = 2;

  argc = argc > 6 ? 6 : argc;
  for (int ic = 0; ic < argc; ic++) {
    char input = (char) va_arg(chars, int);

    // if processing escapes, search for character in escape table
    if (ic < numescape) {
      const struct key_t * match = find_key(input, keys_escape);
      if (match != NULL) {
        report[index] = match->c;
        report[0] |= match->mod;
      }
      else {
        fprintf(stderr, ERR_UNKNOWN_ESCAPE": %c\n", input);
        return -1;
      }
    }
    else if (isalpha(input)) {
      // see HID Usage Tables page 0x07 for this conversion
      report[index] = tolower(input) - ('a' - 4);
      // if uppercase character, set l+r shift in modifier
      if (isupper(input))
        report[0] = 0x22;
    }
    else if (isdigit(input)) {
      report[index] = keys_num[input - '0'].c;
    }
    else if (ispunct(input)) {
      const struct key_t * match = find_key(input, keys_symbol);
      if (match != NULL) {
        report[index] = match->c;
        report[0] |= match->mod;
      }
      else {
        fprintf(stderr, "[!] "ERR_UNKNOWN_CHAR": %c\n", input);
        return -1;
      }
    }
    index++;
  }
  va_end(chars);

  return 0;
}

int make_hid_report_arr(char *report, int numescape, int argc, char* chars) {
  // sanity checks
  if (argc < 1) {
    fprintf(stderr, "[!] Insane character count (%d)\n", argc);
    return -1;
  }
  if (numescape < 0) {
    fprintf(stderr, "[!] Insane escape count (%d)\n", numescape);
    return -1;
  }

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

