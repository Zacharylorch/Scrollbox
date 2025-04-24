/* view.c -- display for scrolling box
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 *
 * Copyright (C) Darren Provine, 2011-2023, All Rights Reserved
 */

#include "scroll.h"
#include "ctype.h"
#include "Chars/All_Chars.h"
#include "view.h"

void show(char *text)
{
    int       position; // where in the string are we?
    int       line;     // which line are we on?
    scroll_display *panel;    // where is the panel's memory?
    int       c;        // what character are we doing?
    int       index;    // where is the character in the glyph[] array?

    assert(strlen(text) == 9); // make sure we got enough chars

    panel = get_panel();
    assert(panel != NULL);     // probably should be an 'if'.

    if (debug) {
        fprintf(stderr, " text:|%s|", text);
        fflush(stderr);
    }

    for (position = 0; position < 9; position ++ ) {

        // Characters start at ' ', which is ASCII 0x20
        // but is in the array at location 0.
        c = (int) text[position];

        // make sure the character is okay
        if ( isprint(c) ) {
            index = c - 0x20;
        } else {
            index = 'X'- 0x20;
        }
        if (debug) {
            fprintf(stderr, "-%c", c);
            fflush(stderr);
        }

        for (line=0; line < 10; line++) {
            panel->set_byte(position + line * 9, (*glyph[index])[line]);
        }
    }
    if (debug) {
        fprintf(stderr, "-\r");
        fflush(stderr);
    }
    panel->update();
}

