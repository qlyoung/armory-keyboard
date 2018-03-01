#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <stdio.h>

/** The default output device */
#define DEFAULT_OUTPUT_FILE "/dev/hidg0"

/** Error codes */
#define ERR_USAGE "usage: ./type -s <script> -l <layout> [-o /dev/hidgX]"
#define ERR_INVALID_TOKEN "Invalid token, skipping line"
#define ERR_NO_MAPPING "No mapping for character, skipping"
#define ERR_CANNOT_WRITE_HID "Error writing HID report"
#define ERR_CANNOT_OPEN_LAYOUTFILE "Error opening layout file"
#define ERR_CANNOT_OPEN_OUTFILE "Error opening output file"
#define ERR_CANNOT_OPEN_INFILE "Error opening script file"
#define ERR_BAD_LAYOUTFILE "Bad layout file"
#define ERR_BAD_UNICODE "Indecipherable UTF-8 byte sequence"

/**
 * Writes the HID report followed by an empty report to the
 * specified file.
 *
 * @param[in] report the 8-byte HID report
 * @param[in] file file stream to write report to
 */
void write_report(char *report, FILE *file);

/**
 * Maps ArmoryDuckyScript escape token to the corresponding escape
 * value. The returned value can subsequently be passed as an escape
 * codepoint to make_hid_report() to generate unprintable keycodes
 * such as modifiers and toggle keys.
 *
 * @param[in] token pointer to null-terminated string specifying
 *  the escape token to map
 * @return the escape character or 0x00 if no mapping was found
 */
uint32_t map_escape(const char *token);

/**
 * Parses and executes an ArmoryDuckyScript, writing generated
 * HID reports to the file descriptor specified.
 *
 * @param[in] scriptfile FILE pointer to script file
 * @param[in] outfile file stream to write generated reports to
 */
void parse(FILE *scriptfile, FILE *outfile);

#endif
