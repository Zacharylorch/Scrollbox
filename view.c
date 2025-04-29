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

int view_props = 0x00;
int toggle_checker = 0;

void set_view_properties(int viewbits)
{
    view_props = viewbits;
}

int get_view_properties()
{
    return view_props;
}

void do_test(int flip)
{
    scroll_display *panel = get_panel();

    for (int byte = 0; byte < 90; byte++)
    {
        if ((byte % 2 == 0 && !flip) || (byte % 2 != 0 && flip))
        {
            panel->set_byte(byte, 0xAA);
        }
        else
        {
            panel->set_byte(byte, 0x55);
        }
    }
}

void up_down_mode(char *text)
{
    
}

#define MAX_TIMESTR 20 // big enough for any valid data
// make_timestring
// returns a string formatted from the "dateinfo" object.
char *make_timestring()
{
    time_t      now;
    struct tm   *dateinfo;

    /* get current time into "struct tm" object */
    (void) time(&now);
    dateinfo = localtime( &now );

    char *timeformat = "(no format)"; // see strftime(3)

    timeformat = "%l:%M:%S %P";

    // make the timestring and return it
    static char timestring[MAX_TIMESTR];
    strftime(timestring, MAX_TIMESTR, timeformat, dateinfo);
    return timestring;
}

void show(char *text)
{
    int position; // character position 0 to 8
    int line;     // line 0 to 9
    scroll_display *panel;
    int c1, c2;
    int pixel_offset;

    panel = get_panel();
    assert(panel != NULL);

    if (view_props & TEST_MODE)
    {
        toggle_checker = !toggle_checker;
        do_test(toggle_checker);
        panel->update();
        return;
    }

    if ((view_props & UP_MODE) || (view_props & DOWN_MODE))
    {
        up_down_mode(text);
        panel->update();
        return;
    }

    assert(strlen(text) == 10); // 9 visible + 1 lookahead

    pixel_offset = get_pixel_offset(); // <--- get pixel shift

    if (debug) 
    {
        fprintf(stderr, " text:|%s| pixel_offset:%d\n", text, pixel_offset);
        fflush(stderr);
    }

    for (position = 0; position < 9; position++)
    {
        char ch1 = text[position];
        char ch2 = text[position + 1];

        // If outside text, assume space (' ')
        if (!isprint(ch1)) ch1 = ' ';
        if (!isprint(ch2)) ch2 = ' ';

        c1 = ch1 - 0x20;
        c2 = ch2 - 0x20;

        for (line = 0; line < 10; line++)
        {
            byte left = (*glyph[c1])[line];
            byte right = (*glyph[c2])[line];

            byte merged = (left << pixel_offset) | (right >> (8 - pixel_offset));

            panel->set_byte(position + line * 9, merged);
        }
    }

    if (debug) 
    {
        fprintf(stderr, "-\n");
        fflush(stderr);
    }

    panel->update();
}