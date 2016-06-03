#include "type.h"
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "kybdutil.h"
#include "layouts.h"
#include "unicode.h"

/**
 * Displays error message and optionally exits with
 * EXIT_FAILURE.
 *
 * @param message null-terminated error message
 * @param perr whether to use perror() to print the message
 * @param fatal whether this error should kill the program
 */
void err(const char *message, bool perr, bool fatal) {
  char *prefix = fatal ? "[X]" : "[!]";

  if (perr) {
    fprintf(stderr, "%s ", prefix);
    perror(message);
  }
  else {
    fprintf(stderr, "%s %s\n", prefix, message);
  }

  if (fatal)
    exit(EXIT_FAILURE);
}

/**
 * Sleeps in millisecond increments.
 *
 * @param milliseconds number of milliseconds to sleep for.
 */
void millisleep(long milliseconds) {
  // convert millis to seconds and nanos
  long seconds = (long) (milliseconds / 1000L);
  long nanos = (milliseconds - seconds * 1000) * 1000000;
  struct timespec ts = { .tv_sec = (size_t) seconds, .tv_nsec = nanos };
  nanosleep(&ts, NULL);
}

void write_report(char* report, FILE* file) {
  // send key
  if (fwrite(report, (size_t) 1, sizeof(report), file) != sizeof(report))
    err(ERR_CANNOT_WRITE_HID, false, true);

  fflush(file);

  // send empty key
  memset(report, 0x0, 8);

  if (fwrite(report, (size_t) 1, sizeof(report), file) != sizeof(report))
    err(ERR_CANNOT_WRITE_HID, false, true);

  fflush(file);
}

uint32_t map_escape(const char *token) {
  if (!strcmp(token, "ALT"))
    return ALT;
  else if (!strcmp(token, "BACKSPACE"))
    return BACKSPACE;
  else if (!strcmp(token, "CONTROL"))
    return CONTROL;
  else if (!strcmp(token, "CTRL"))
    return CONTROL;
  else if (!strcmp(token, "DELETE"))
    return DELETE;
  else if (!strcmp(token, "ESCAPE"))
    return ESCAPE;
  else if (!strcmp(token, "END"))
    return END;
  else if (!strcmp(token, "GUI") ||
           !strcmp(token, "WINDOWS"))
    return GUI;
  else if (!strcmp(token, "HOME"))
    return HOME;
  else if (!strcmp(token, "INSERT"))
    return INSERT;
  else if (!strcmp(token, "DOWNARROW") ||
           !strcmp(token, "DOWN"))
    return DARROW;
  else if (!strcmp(token, "UPARROW") ||
           !strcmp(token, "UP"))
    return UARROW;
  else if (!strcmp(token, "LEFTARROW") ||
           !strcmp(token, "LEFT"))
    return LARROW;
  else if (!strcmp(token, "RIGHTARROW") ||
           !strcmp(token, "RIGHT"))
    return RARROW;
  else if (!strcmp(token, "ENTER"))
    return ENTER;
  else if (!strcmp(token, "SPACE"))
    return SPACE;
  else if (!strcmp(token, "PRINTSCREEN"))
    return PRNTSCRN;
  else if (!strcmp(token, "SCROLLLOCK"))
    return SCRLLCK;
  else if (!strcmp(token, "MENU"))
    return MENU;
  else if (!strcmp(token, "APP"))
    return MENU;
  else if (!strcmp(token, "SHIFT"))
    return SHIFT;
  else if (!strcmp(token, "TAB"))
    return TAB;
  else if (!strcmp(token, "CAPSLOCK"))
    return CAPSLOCK;
  else if (!strcmp(token, "PAUSE"))
    return PAUSE;
  else if (!strcmp(token, "NUMLOCK"))
    return NUMLOCK;
  else if (!strcmp(token, "PAGEDOWN"))
    return PAGEDOWN;
  else if (!strcmp(token, "PAGEUP"))
    return PAGEUP;
  else if (!strcmp(token, "CLEAR"))
    return CLEAR;
  else if (!strcmp(token, "F1"))
    return F1;
  else if (!strcmp(token, "F2"))
    return F2;
  else if (!strcmp(token, "F3"))
    return F3;
  else if (!strcmp(token, "F4"))
    return F4;
  else if (!strcmp(token, "F5"))
    return F5;
  else if (!strcmp(token, "F6"))
    return F6;
  else if (!strcmp(token, "F7"))
    return F7;
  else if (!strcmp(token, "F8"))
    return F8;
  else if (!strcmp(token, "F9"))
    return F9;
  else if (!strcmp(token, "F10"))
    return F10;
  else if (!strcmp(token, "F11"))
    return F11;
  else if (!strcmp(token, "F12"))
    return F12;
  else
    return 0;
}

/* State variables for parser */
long defdelay = 0;

/**
 * Parses an ArmoryDuckyScript, generates HID reports
 * accordingly and writes them to the output file.
 *
 * @param scriptfile FILE pointer to script file
 * @param outfile FILE pointer to write generated reports to.
 */
void parse(FILE* scriptfile, FILE* file) {
  char report[8];
  char line[501];
  char *command;

  // loop over lines in file
  while (fgets(line, sizeof(line), scriptfile)) {

    if (strlen(line) > 1)
      printf("%s", line);

    command = strtok(line, " \n");
    if (command == NULL || !strcmp(command, "REM") || !strcmp(command, "#")) continue;

    // clear HID report
    memset(report, 0x0, sizeof(report));


    if (!strcmp(command, "DEFAULT_DELAY") || !strcmp(command, "DEFAULTDELAY")) {
      if (sscanf(strtok(NULL, " "), "%ld", &defdelay) == 0)
        err(ERR_INVALID_TOKEN, false, false);
        continue;
    }
    if (!strcmp(command, "DELAY")) {
      int delay = 0;
      if (sscanf(strtok(NULL, " \n"), "%d", &delay) == 0) {
        err(ERR_INVALID_TOKEN, false, false);
        continue;
      }

      millisleep(delay);
    }
    else if (!strcmp(command, "STRING")) {
      char *str = strtok(NULL, "\n");
      if (str == NULL) {
        err(ERR_INVALID_TOKEN, false, false);
        continue;
      }

      // send each character one by one
      int index = 0;
      while (index < strlen(str)) {

        // read next UTF-8 char
        uint32_t codepoint = getCodepoint(str, &index);

        if (make_hid_report(report, 0, 1, codepoint)) {
          char *prefix  = "No mapping for character:";
          char *message = malloc(strlen(prefix) + 16);
          sprintf(message, "%s %c (U+%04x)", prefix, codepoint, codepoint);
          err(message, false, false);
          free(message);
          continue;
        }

        write_report(report, file);
      }
    }
    else if (!strcmp(command, "SIMUL")) {
      // parse up to six arguments to be sent simultaneously
      uint32_t simuls[6];
      char *param = NULL;
      bool escapes_done = false, invalid = false;
      int i = 0, num_escapes = 0;

      for (; i < sizeof(simuls); i++) {
        param = strtok(NULL, " \n");
        if (param == NULL) break;

        int index = 0;
        uint32_t nextCodepoint = getCodepoint(param, &index);

        // if the token is a single character, save and move on
        if (index == strlen(param)) {
          simuls[i] = nextCodepoint;
          escapes_done = true;
        }
        // if it's not a single character, it should be an escape token
        else {
          char esc;
          if (escapes_done || (esc = map_escape(param)) == 0) {
            invalid = true;
            break;
          }
          // add to report and move on
          simuls[i] = esc;
          num_escapes++;
        }
      }

      // skip line if invalid token was encountered
      if (invalid) {
        err(ERR_INVALID_TOKEN, false, false);
        continue;
      }

      make_hid_report_arr(report, num_escapes, i, simuls);
      write_report(report, file);
    }
    // if it wasn't anything else, try to map token to an escape
    else {
      char esc = map_escape(command);
      if (esc == 0) {
        err(ERR_INVALID_TOKEN, false, false);
        continue;
      }
      make_hid_report(report, 1, 1, esc);
      write_report(report, file);
    }

    millisleep(defdelay);
  }
}


#ifndef TESTING
int main(int argc, char** argv) {
  // args
  FILE *outfile, *infile, *layoutfile;
  char *outfile_path = DEFAULT_OUTPUT_FILE;

  // sanity check on argument count
  if (argc < 3)
    err(ERR_USAGE, false, true);

  int optchar;
  while((optchar = getopt(argc, argv, "s:l:o:")) != -1) {
    switch (optchar) {
      case 's':
        // open script file
        infile = fopen(optarg, "rb");
        if (infile == NULL)
          err(ERR_CANNOT_OPEN_INFILE, true, true);
        break;
      case 'l':
        // open layout file
        layoutfile = fopen(optarg, "rb");
        if (layoutfile == NULL)
          err(ERR_CANNOT_OPEN_INFILE, true, true);
        break;
      case 'o':
        // get output file path
        outfile_path = optarg;
        break;
    }
  }

  // open output file
  outfile = fopen(outfile_path, "a");
  if (outfile == NULL)
    err(ERR_CANNOT_OPEN_OUTFILE, true, true);

  // load layout file
  struct Layout *layout = load_layout(layoutfile);
  if (layout == NULL)
    err(ERR_BAD_LAYOUTFILE, false, true);

  // set layout
  set_layout(layout);

  parse(infile, outfile);

  // free resources
  destroy_layout(layout);
  fclose(layoutfile);
  fclose(infile);
  fclose(outfile);


  return EXIT_SUCCESS;
}
#endif // TESTING
