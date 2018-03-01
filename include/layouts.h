#ifndef LAYOUTS_H
#define LAYOUTS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Escape codepoints for common keys */
#define ALT 1
#define BACKSPACE 2
#define CONTROL 3
#define DELETE 4
#define ESCAPE 5
#define END 6
#define GUI 7
#define HOME 8
#define INSERT 9
#define DARROW 10
#define UARROW 11
#define LARROW 12
#define RARROW 13
#define ENTER 14
#define SPACE 15
#define PRNTSCRN 16
#define SCRLLCK 17
#define MENU 18
#define SHIFT 19
#define TAB 20
#define CAPSLOCK 21
#define PAUSE 22
#define NUMLOCK 23
#define PAGEDOWN 24
#define PAGEUP 25
#define CLEAR 26
#define F1 27
#define F2 28
#define F3 29
#define F4 30
#define F5 31
#define F6 32
#define F7 33
#define F8 34
#define F9 35
#define F10 36
#define F11 37
#define F12 38
#define ESCAPE_END 0

/**
 * Structure to hold a Unicode character and the
 * HID Usage ID + modifier bitfield necessary to
 * produce it for a given layout
 */
struct Keycode {
	// Unicode code point for character
	uint32_t ch;
	// character's usage ID
	unsigned char id;
	// modifier byte for character
	unsigned char mod;
};

/**
 * Structure to hold the name of the layout and
 * keycode mappings for all characters that the
 * layout supports
 */
struct Layout {
	// number of mappings
	int size;
	// all keycode mappings for layout
	struct Keycode **map;
};

/**
 * Load the layout with the specified name.
 *
 * @param[in] layoutfile layout file opened for reading
 * @return pointer to layout, NULL on error
 */
struct Layout *load_layout(FILE *layoutfile);

/**
 * Frees all memory used by a layout
 * @param[in] layout layout to destroy
 */
void destroy_layout(struct Layout *layout);

/**
 * Finds the keycode mapping for the character specified by
 * the given Unicode codepoint.
 *
 * @param[in] codepoint the codepoint of the character to map
 * @param[in] layout the layout to look up the mapping in
 * @param[in] escape whether the passed codepoint is a predefined escape code
 * @return pointer to the mapping in the layout, or NULL no mapping was found
 */
const struct Keycode *map_codepoint(uint32_t codepoint, struct Layout *layout,
				    bool escape);

#endif
