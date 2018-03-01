/*
 * Utility functions for generating USB HID keyboard reports.
 *
 * Authors:
 *  Quentin Young (qlyoung AT qlyoung.net)
 *  Collin Mulliner (collin AT mulliner.org)
 */

#include "kybdutil.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

struct Layout *layout;

void set_layout(struct Layout *lo)
{
	layout = lo;
}

int make_hid_report_arr(char *report, int numescape, int argc,
			uint32_t *codepoints)
{
	int index = 2;

	if (codepoints == NULL)
		return -1;

	for (int i = 0; i < argc && i < 6; i++) {
		assert(index < 8);
		uint32_t input = codepoints[i];
		const struct Keycode *match =
			map_codepoint(input, layout, i < numescape);
		if (match == NULL)
			return -1;
		if (match->id != 0x00)
			report[index++] = match->id;
		report[0] |= match->mod;
	}

	return 0;
}

int make_hid_report(char *report, int numescape, int argc, ...)
{
	va_list cplist;
	uint32_t codepoints[6];

	if (layout == NULL)
		return -1;

	va_start(cplist, argc);
	for (int i = 0; i < argc && i < 6; i++)
		codepoints[i] = (uint32_t)va_arg(cplist, int);
	va_end(cplist);

	return make_hid_report_arr(report, numescape, argc, codepoints);
}
