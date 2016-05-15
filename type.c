#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "kybdutil.h"

#define DEFAULT_HID_DEVICE "/dev/hidg0"
#define ERR_INVALID_TOKEN "Invalid token, skipping line."
#define ERR_INVALID_HIDDEV "Bad HID device."

/** Displays error message and exits if fatal is true */
void err(char *message, bool fatal) {
  fprintf(stderr, "[!] %s\n", message);
  if (fatal)
    exit(1);
}

/** Sleeps for the specified number of milliseconds */
void millisleep(long milliseconds) {
  // convert millis to seconds and nanos
  long seconds = (long) (milliseconds / 1000L);
  long nanos = (milliseconds - seconds * 1000) * 1000000;
  struct timespec ts = { .tv_sec = (size_t) seconds, .tv_nsec = nanos };
  nanosleep(&ts, NULL);
}

/** Sends the HID report to the specified character device */
void sendreport(char* report, int device) {
  // send key
  if (write(device, report, sizeof(report)) != sizeof(report))
    err(ERR_INVALID_HIDDEV, true);
  // send empty key
  memset(report, 0x0, 8);
  if (write(device, report, sizeof(report)) != sizeof(report))
    err(ERR_INVALID_HIDDEV, true);
}

/* State variables for parser */
long defdelay = 0;

/* maps DuckyScript special keywords to their char values */
char map_escape(char* token) {
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
  else if (!strcmp(token, "GUI"))
    return GUI;
  else if (!strcmp(token, "WINDOWS"))
    return GUI;
  else if (!strcmp(token, "HOME"))
    return HOME;
  else if (!strcmp(token, "INSERT"))
    return INSERT;
  else if (!strcmp(token, "DOWNARROW"))
    return DARROW;
  else if (!strcmp(token, "UPARROW"))
    return UARROW;
  else if (!strcmp(token, "LEFTARROW"))
    return LARROW;
  else if (!strcmp(token, "RIGHTARROW"))
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
  else
    return 0;
}


/**
 * Parses and executes an ArmoryDuckyScript.
 * @param scriptfile FILE pointer to script file
 * @param fd file descriptor for /dev/hidgX device
 */
void parse(FILE* scriptfile, int fd) {
  char report[8];
  char line[500];

  // check for default delay setting
  fgets(line, sizeof(line), scriptfile);
  char *command = strtok(line, " ");
  if (!strcmp(command, "DEFAULT_DELAY") || !strcmp(command, "DEFAULTDELAY")) {
    if (sscanf(strtok(NULL, " "), "%ld", &defdelay) == 0)
      err(ERR_INVALID_TOKEN, false);
  }
  else // reset file pointer
    rewind(scriptfile);

  // loop over line
  while (fgets(line, sizeof(line), scriptfile)) {
    // echo script line if it's not empty
    if (strlen(line) > 1)
      printf("%s", line);
    // get command
    char* command = strtok(line, " \n");
    // if the line is blank or a comment, skip it
    if (command == NULL || !strcmp(command, "REM") || !strcmp(command, "#")) continue;
    // clear HID report
    memset(report, 0x0, sizeof(report));

    // if cascade for control commands
    if (!strcmp(command, "DELAY")) {
      // read delay
      int delay = 0;
      if (sscanf(strtok(NULL, " \n"), "%d", &delay) == 0) {
        err(ERR_INVALID_TOKEN, false);
        continue;
      }

      // execute delay
      millisleep(delay);
    }
    else if (!strcmp(command, "STRING")) {
      // read string param
      char *str = strtok(NULL, "\n");
      // if no string, err
      if (str == NULL) {
        err(ERR_INVALID_TOKEN, false);
        continue;
      }
      // send each character one by one
      for (int i = 0; i < strlen(str); i++) {
        // convert character to HID report
        make_hid_report(report, 0, 1, str[i]);
        sendreport(report, fd);
      }
    }
    // because duckyscript was created by skids
    else if (!strcmp(command, "SIMUL")) {
      // parse up to six arguments to be sent simultaneously
      char simuls[6];
      char *param = NULL;
      bool escapes_done = false, invalid = false;
      int i = 0, num_escapes = 0;
      for (; i < sizeof(simuls); i++) {
        // get next space or newline-delimited argument
        param = strtok(NULL, " \n");
        // if there are no more tokens, die
        if (param == NULL) break;
        // if it's a single char, just pass it along
        if (strlen(param) == 1) {
          simuls[i] = *param;
          escapes_done = true;
        }
        else {
          // if escapes are already done, error
          if (escapes_done) {
            invalid = true;
            break;
          };
          // try to map it as a syntax element
          char esc = map_escape(param);
          if (esc == 0) {
            invalid = true;
            break;
          }
          // write special
          simuls[i] = esc;
          num_escapes++;
        }
      }
      // if broke loop because of error, skip line
      if (invalid) {
        err(ERR_INVALID_TOKEN, false);
        continue;
      }
      // generate report
      make_hid_report_arr(report, num_escapes, i, simuls);
      sendreport(report, fd);
    }
    // if it wasn't anything else, try to map token to an escape
    else {
      char esc = map_escape(command);
      // if it's not a valid escape, print err and skip line
      if (esc == 0) {
        err(ERR_INVALID_TOKEN, false);
        continue;
      }
      make_hid_report(report, 1, 1, esc);
      sendreport(report, fd);
    }

    // sleep for default delay
    millisleep(defdelay);
  }
}

int main(int argc, char** argv) {
  // sanity check on argument count
  if (argc < 2) {
    printf("usage: %s <file> [/dev/hidgX]\n", argv[0]);
    return 0;
  }

  // device file that we will write HID reports to
  char* dev_filename = DEFAULT_HID_DEVICE;

  // use the user's device file if they gave us one
  if (argc > 2)
      dev_filename = argv[2];

  // attempt to open device file
  int dev_fd;
  if ((dev_fd = open(dev_filename, O_RDWR, 0666)) == -1) {
      perror(dev_filename);
      return 3;
  }

  // open and parse file
  FILE* infile = fopen(argv[1], "rb");
  parse(infile, dev_fd);

  return 0;
}
