#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "kybdutil.h"

#define DEFAULT_HID_DEVICE "/dev/hidg0"

/** Displays error message and exit()'s if fatal is true */
void err(char *message, bool fatal) {
  printf("%s\n", message);
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
    err("Cannot write to HID device", true);
  // send empty key
  memset(report, 0x0, 8);
  if (write(device, report, sizeof(report)) != sizeof(report))
    err("Cannot write to HID device", true);
}

// State variables for parser
long defdelay = 0;

/**
 * Parses and executes a DuckyScript.
 */
void parse(FILE* scriptfile, int fd) {
  char report[8];
  char line[500];

  // check for default delay setting
  fgets(line, sizeof(line), scriptfile);
  char *command = strtok(line, " ");
  if (!strcmp(command, "DEFAULT_DELAY") || !strcmp(command, "DEFAULTDELAY")) {
    if (sscanf(strtok(NULL, " "), "%ld", &defdelay) == 0)
      err("Malformed line: 1", false);
  }
  else {
    // reset file pointer
    rewind(scriptfile);
  }

  // loop over line
  int linenum = 0;
  while (fgets(line, sizeof(line), scriptfile)) {
    printf("%s", line);
    linenum++;
    // get command
    char* command = strtok(line, " \n");
    // if the line is blank or a comment, skip it
    if (command == NULL || strcmp(command, "REM") == 0) continue;

    // sleep for default delay
    millisleep(defdelay);
    // clear HID report
    memset(report, 0x0, sizeof(report));

    if (!strcmp(command, "DELAY")) {
      // read delay
      int delay = 0;
      if (sscanf(strtok(NULL, " \n"), "%d", &delay) == 0) {
        char *msg = calloc(strlen(line) + 18, 1);
        sprintf(msg, "Malformed line: %s", line);
        err(msg, false);
      }
      // delay
      millisleep(delay);
      continue;
    }
    else if (!strcmp(command, "STRING")) {
      // read string
      char *str = strtok(NULL, "\n");
      // send each character one by one
      for (int i = 0; i < strlen(str); i++) {
        // convert character to HID report
        make_hid_report(report, &str[i], 1);
        sendreport(report, fd);
      }
      continue;
    }
    else if (!strcmp(command, "GUI") || !strcmp(command, "WINDOWS")) {
      // check for single char
      char *c = strtok(NULL, " \n");
      // build report string size 3
      char *repstr = calloc((size_t) 3, (size_t) 1);
      repstr[0] = '\\';
      repstr[1] = 'g';
      if (c != NULL) {
        repstr[2] = *c;
        make_hid_report(report, repstr, 3);
      }
      else
        make_hid_report(report, repstr, 2);
      // send report
      sendreport(report, fd);
      free(repstr);
      continue;
    }
    else if (!strcmp(command, "MENU") || !strcmp(command, "APP")) {
      make_hid_report(report, "\\m", 2);
      sendreport(report, fd);
      continue;
    }
    else if (!strcmp(command, "SHIFT")) {

      continue;
    }
    else if (!strcmp(command, "ENTER")) {
      // build report
      make_hid_report(report, "\\n", 2);
      sendreport(report, fd);
      continue;
    }

  }

}

int dev_fd = -1;

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
  if ((dev_fd = open(dev_filename, O_RDWR, 0666)) == -1) {
      perror(dev_filename);
      return 3;
  }

  // read the file
  FILE* infile = fopen(argv[1], "rb");

  parse(infile, dev_fd);

  return 0;
}
