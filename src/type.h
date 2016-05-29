#include <stdio.h>

#define DEFAULT_HID_DEVICE "/dev/hidg0"

#define ERR_USAGE "usage: ./type <script> [/dev/hidgX]"
#define ERR_INVALID_TOKEN "Invalid token, skipping line"
#define ERR_CANNOT_WRITE_HID "Error writing HID report"
#define ERR_CANNOT_OPEN_OUTFILE "Error opening output file"
#define ERR_CANNOT_OPEN_INFILE "Error opening script file"

/**
 * Writes the HID report follows by an empty report to the
 * specified file.
 * @param[in] report the 8-byte HID report
 * @param[in] file file stream to write report to
 */
void write_report(char *report, FILE *file);

/**
 * Maps ArmoryDuckyScript escape token to corresponding escape character
 * @param[in] token pointer to null-terminated token
 * @return the escape character or 0x00 if no mapping was found
 */
char map_escape(char *token);

/**
 * Parses and executes an ArmoryDuckyScript, writing generated
 * HID reports to the file descriptor specified.
 * @param[in] scriptfile FILE pointer to script file
 * @param[in] file file stream to write generated reports to
 */
void parse(FILE *scriptfile, FILE *file);
