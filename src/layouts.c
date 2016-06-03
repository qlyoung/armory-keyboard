/*
 * Functions for loading, destroying and using
 * keyboard layouts.
 */

#include "layouts.h"
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "unicode.h"

/** Table of keycode for unprintable keys */
static struct Keycode keys_escape[] = {
  {.ch = ALT,       .id = 0x00, .mod=0x04}, // alt
  {.ch = BACKSPACE, .id = 0x2A, .mod=0x00}, // backspace
  {.ch = CONTROL,   .id = 0x00, .mod=0x01}, // ctrl
  {.ch = DELETE,    .id = 0x4C, .mod=0x00}, // delete
  {.ch = ESCAPE,    .id = 0x29, .mod=0x00}, // esc
  {.ch = END,       .id = 0x4D, .mod=0x00}, // end
  {.ch = GUI,       .id = 0x00, .mod=0x08}, // gui/win
  {.ch = HOME,      .id = 0x4A, .mod=0x00}, // home
  {.ch = INSERT,    .id = 0x49, .mod=0x00}, // insert
  {.ch = DARROW,    .id = 0x51, .mod=0x00}, // down arrow
  {.ch = UARROW,    .id = 0x52, .mod=0x00}, // up arrow
  {.ch = LARROW,    .id = 0x50, .mod=0x00}, // left arrow
  {.ch = RARROW,    .id = 0x4F, .mod=0x00}, // right arrow
  {.ch = ENTER,     .id = 0x28, .mod=0x00}, // enter
  {.ch = SPACE,     .id = 0x2C, .mod=0x00}, // space (helps with parsing)
  {.ch = PRNTSCRN,  .id = 0x46, .mod=0x00}, // printscreen
  {.ch = SCRLLCK,   .id = 0x47, .mod=0x00}, // scroll lock
  {.ch = MENU,      .id = 0x76, .mod=0x00}, // menu
  {.ch = SHIFT,     .id = 0x00, .mod=0x02}, // shift
  {.ch = TAB,       .id = 0x2B, .mod=0x00}, // tab
  {.ch = CAPSLOCK,  .id = 0x39, .mod=0x00}, // capslock
  {.ch = PAUSE,     .id = 0x48, .mod=0x00}, // pause
  {.ch = NUMLOCK,   .id = 0x53, .mod=0x00}, // numlock (keypad)
  {.ch = PAGEDOWN,  .id = 0x4E, .mod=0x00}, // page down
  {.ch = PAGEUP,    .id = 0x4B, .mod=0x00}, // page up
  {.ch = CLEAR,     .id = 0x9C, .mod=0x00}, // clear
  {.ch = F1,        .id = 0x3A, .mod=0x00}, // F1
  {.ch = F2,        .id = 0x3B, .mod=0x00}, // F2
  {.ch = F3,        .id = 0x3C, .mod=0x00}, // F3
  {.ch = F4,        .id = 0x3D, .mod=0x00}, // F4
  {.ch = F5,        .id = 0x3E, .mod=0x00}, // F5
  {.ch = F6,        .id = 0x3F, .mod=0x00}, // F6
  {.ch = F7,        .id = 0x40, .mod=0x00}, // F7
  {.ch = F8,        .id = 0x41, .mod=0x00}, // F8
  {.ch = F9,        .id = 0x42, .mod=0x00}, // F9
  {.ch = F10,       .id = 0x43, .mod=0x00}, // F10
  {.ch = F11,       .id = 0x44, .mod=0x00}, // F11
  {.ch = F12,       .id = 0x45, .mod=0x00}, // F12
  {.ch = 0x0,       .id = 0x00, .mod=0x00}
};

struct Layout *load_layout(FILE* layoutfile) {

  char line[50];

  // start with a table size of 50
  int table_cap = 50;
  struct Layout *layout = malloc(sizeof(struct Layout));
  // initialize map with cap
  layout->size = 0;
  layout->map = malloc(table_cap * sizeof(struct Keycode *));

  // check if layout file
  fgets(line, sizeof(line), layoutfile);
  if (!strstr(line, "-*- layout"))
    return NULL;

  while (fgets(line, sizeof(line), layoutfile)) {
    if (line[0] == '\n') continue;
    struct Keycode *keydef = malloc(sizeof(struct Keycode));
    int index = 0;
    // get the character to produce
    keydef->ch = getCodepoint(line, &index);
    // skip three ascii chars (space, 0, X)
    index += 3;
    // read two hex values
    unsigned int id, mod;
    sscanf(line + index, "%x %x", &id, &mod);
    keydef->id  = (char) id;
    keydef->mod = (char) mod;

    // add mapping to table
    layout->map[layout->size++] = keydef;

    // resize if necessary
    if (layout->size == table_cap) {
      table_cap *= 2;
      layout->map = realloc(layout->map, table_cap * sizeof(struct Keycode *));
    }
  }

  return layout;

}

void destroy_layout(struct Layout *layout) {
  for (int i = 0; i < layout->size; i++) {
    free(layout->map[i]);
  }
  free(layout->map);
  free(layout);
}

const struct Keycode *map_codepoint(uint32_t codepoint, struct Layout *layout, bool escape) {
  if (escape) {
    // search known escapes
    for (int i = 0; keys_escape[i].ch != 0x00; i++) {
      if (codepoint == keys_escape[i].ch)
        return &keys_escape[i];
    }
  }
  else {
    // search layout
    for (int i = 0; i < layout->size; i++) {
      if (layout->map[i]->ch == codepoint)
        return layout->map[i];
    }
  }

  return NULL;
}
