/* LEDots.h -- header file for dot matrix clock library file
 *
 * Darren Provine, 14 March 2010
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <ncurses.h>
#include <term.h>
#include <stdlib.h>

#ifndef BYTE
typedef unsigned char byte;
#define BYTE
#endif

#ifndef KEYBITS
typedef unsigned char keybits;
#define KEYBITS
#endif


// display object methods
typedef struct {
    void (*set_byte)(int, byte);  // set a byte of display-mapped memory
    void (*update)(void);      // redraw screen from bits in memory
    void (*set_title)(char *);    // set the title bar
    void (*set_key)(int, char *); // set the text on a key
    void (*read_input)(void);     // wait for user to click or type

    int (*set_callback)( void(*f)(keybits) ); // sets input callback function
} scroll_display;

void start_display(void); // creates display object in memory
void end_display(void);   // cleans up memory

scroll_display *get_panel(void); // return pointer to display object in memory
