#ifndef UNICODE_H
#define UNICODE_H

#include <stdint.h>

/**
 * Reads a single UTF-8 code sequence from a string and returns
 * its 32-bit Unicode codepoint.
 *
 * @param[in] string the UTF-8 encoded string to read from
 * @param[out] index pointer to integer specifying the index of
 *  the byte starting the UTF-8 code sequence. Upon return this
 *  value will be the index of the byte immediately following
 *  the code sequence.
 * @return Unicode codepoint for the character
 */
uint32_t getCodepoint(char *string, int *index);

#endif
