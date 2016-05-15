/**
 * Keycode tables and english names for special characters.
 */

// table of escape codes
#define ALT         'a'
#define BACKSPACE   'b'
#define CONTROL     'c'
#define DELETE      'd'
#define ESCAPE      'e'
#define END         'f'
#define GUI         'g'
#define HOME        'h'
#define INSERT      'i'
#define MENU        'm'
#define ENTER       'n'
#define SHIFT       's'
#define TAB         't'
#define LARROW      'l'
#define RARROW      'r'
#define UARROW      'u'
#define DARROW      'j'
#define PAGEUP      'z'
#define PAGEDOWN    'x'
#define SPACE       'p'

int make_hid_report(char *, int, int, ...);
int make_hid_report_arr(char *, int, int, char *);
