/* scroll.c -- scrolling display
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 *
 * Copyright (C) Darren Provine, 2011-2023, All Rights Reserved
 */

#include <time.h>
#include <stdio.h>
#include <limits.h>

// sleep needs this
#include <unistd.h>

#include "scroll.h"
#include "view.h"

// debug flag - changed via -D option
int debug = 0;

char saved_message[256];

static char bugaddress[]="lorchz74@elvis.rowan.edu";

// version -- say which version this is and exit
// (note simple output; you could also print the rcsid too)
void version()
{
    fprintf(stderr, "scroll version 1\n");
    exit(0);
}

// usage -- print brief summary and instructions
void usage(char *progname)
{
    fprintf(stderr, "This program shows a scrolling box.\n");
    fprintf(stderr, "Usage: %s [-dDvh] text\n", progname);
    fprintf(stderr,
            "  -d # : set scroll delay to # milliseconds (default=1000)\n");
    fprintf(stderr,
            "  -D   : turn on debug messages (more Ds = more debugging)\n");
    fprintf(stderr, "  -v   : show version information\n");
    fprintf(stderr, "  -h   : this help message\n");
    fprintf(stderr, "Use quotes for multi-word messages, 'like this'.\n");
    fprintf(stderr, "report bugs to %s \n", bugaddress);
    exit (0);
}

void exit_scroll()
{
    end_display();
    exit(0);
}

int test_mode_end = INT_MAX;
int time_mode_end = INT_MAX;

// keyboard and mouse handling
void process_key(keybits KeyCode)
{
    int row, col;
    int view_props;

    const int DELAY_INTERVAL = 100;
    const int DELAY_MIN      = 100;
    const int DELAY_MAX      = 5000;

    if ( KeyCode & 0x80 ) { // mouse click
        row = (KeyCode & 0x70) >> 4;
        col = (KeyCode & 0x0f);

        if (row == 0)
        {
            switch(col)
            {
                case 0: 
                    if (get_delay() - DELAY_INTERVAL > DELAY_MIN)
                    {
                        set_delay(get_delay() - DELAY_INTERVAL);
                        start_timer();
                    }
                    break;
                case 1:
                    if (get_delay() + DELAY_INTERVAL < DELAY_MAX)
                    {
                        set_delay(get_delay() + DELAY_INTERVAL);
                        start_timer();
                    }
                    break;
                case 2:
                    view_props = get_view_properties();
                    view_props |= ( TIME_MODE );
                    set_view_properties (view_props);
                    strncpy(saved_message, get_scrollmessage(), sizeof(saved_message));
                    time_mode_end = time(NULL) + 10;
                    break;
                case 3:
                    view_props = get_view_properties();
                    view_props |= ( TEST_MODE );
                    set_view_properties (view_props);
                    test_mode_end = time(NULL) + 5;
                    break;
                case 4:
                    exit_scroll();
                    break;
            }
        }
        else if (row == 1)
        {
            switch(col)
            {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
            }
        }

    } else { // keyboard press
        switch(KeyCode) {
            case 'q': exit_scroll(); break;
        }
    }
}

int main(int argc, char *argv[])
{
    int            initial_delay = 300;
    int            letter; // option char, if any
    scroll_display *panel;

    // note: C does automatic concatenation of long strings
    char title[81] =
               "----------------------------"
               "   Zach, Noel, Tess   "
               "----------------------------";

    
    // if (get_view_properties() & TEST_MODE)
    // {
    //     title[81] =
    //         "----------------------------"
    //         "         Test         "
    //         "----------------------------";
    // }

    // if (get_view_properties() & TIME_MODE)
    // {
    //     title[81] =
    //         "----------------------------"
    //         "         Time         "
    //         "----------------------------";
    // }

    // loop through all the options; getopt() can handle together or apart
    while ( ( letter = getopt(argc, argv, "d:Dvh")) != -1 ) {
        switch (letter) {
            case 'd':  initial_delay = atoi(optarg); break;
            case 'D':  debug++;                      break;
            case 'v':  version();                    break;
            case 'h':  usage(argv[0]);               break;

            case '?':  // unknown flag; fall through to next case
            default:   // shouldn't happen, but Just In Case
                       // note that getopt() warns about the unknown flag
                  fprintf(stderr, "run \"%s -h\" for help\n", argv[0]);
                  exit(1);
        }
    }

    // optind is the first argument after options are processed
    // if there aren't any, the user didn't give a message to scroll
    if (optind == argc) {
        usage(argv[0]);
    }

    // put the information from the command line into the module
    setup(argv[optind]);

    // set up the view
    start_display();
    panel=get_panel();
    assert(panel != NULL);
    panel->set_callback(process_key);
    panel->set_title(title);

    // turn on some keys in row 2
    panel->set_key(0, "snake");
    panel->set_key(1, "up");
    panel->set_key(2, "lang");
    panel->set_key(3, "inverted");
    panel->set_key(4, "foobar");

    // start the model running
    set_delay(initial_delay);
    start_timer();

    // for (int i = 0; i < 90; i++) {
    //     panel->set_byte(i, 0xFF);
    // }
    // panel->update();

    // while (1) {
    //     panel->read_input();
    // }

    // wait for the model to signal controller
    while ( 1 ) {
        panel->read_input();
    }

    end_display();

    return 0;
}


/* This is the function the model uses to signal the controller
 * that there's work to do.
 */
void tick(int sig)
{
    char   *segment;
    time_t now = time( NULL );
    int view_props = get_view_properties();
    
    /* get the information from model about what chars to show
     * and how far over to slide them */
    segment = display_string();

    static int last_time_mode = 0;

    if ((view_props & TIME_MODE) && !last_time_mode)
    {
        strncpy(saved_message, get_scrollmessage(), sizeof(saved_message)); // Save original
        setup(make_timestring());  // Set up the time text ONCE
        last_time_mode = 1;
    }

    if (!(view_props & TIME_MODE) && last_time_mode)
    {
        setup(saved_message);      // Restore original text
        last_time_mode = 0;
    }
    
    if (debug >= 3) 
    {
        fprintf(stderr, "display_string() returned |%s|\r\n", segment);
        sleep(2);
    }

    if ( now > test_mode_end ) 
    {
        view_props = get_view_properties();
        view_props &= (~TEST_MODE);
        set_view_properties(view_props);
    }

    if ( now > time_mode_end ) 
    {
        view_props = get_view_properties();
        view_props &= (~TIME_MODE);
        set_view_properties(view_props);
        setup(saved_message);
    }

    if (get_view_properties() & TEST_MODE)
    {
        show(NULL);  // triggers show() to run its TEST_MODE logic
        return;             // don't continue to scroll
    }

    /* Send those chars to the view. */
    show(segment);
}

