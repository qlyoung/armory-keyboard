/** A few brief helpers for UTF-8 */
#ifndef UNICODE_H
#define UNICODE_H

#include <stdint.h>

/**
 * Reads a single UTF-8 code sequence from the string specified
 * at the given index into the string and updates the index to
 * point to the byte immediately after the sequence.
 * @param[in] string the UTF-8 encoded string to read from
 * @param[in] index pointer to index into string
 * @return Unicode codepoint for the character
 */
uint32_t getUTF8Char(char *string, int *index);

#endif
