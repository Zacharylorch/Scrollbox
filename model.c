/* model.c -- Data and Timing for the Scroll Box
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 *
 * Copyright (C) Darren Provine, 2011-2023, All Rights Reserved
 */

#include "scroll.h"

/* the message that we're scrolling */
char scrollmessage[256];

/* pad scrollmessage with 9 spaces left and right */
void setup(char *text)
{
    snprintf(scrollmessage, 255, "         %s         ", text);

    if (debug >= 2) 
    {
        fprintf(stderr, "text is: |%s|\r\n", scrollmessage);
        sleep(1);
    }
}

static int startpos = 0;  // or pixel_offset if you're using fine scrolling

/* 'viewport' is declared outside the function, so the pointer will
 * still be valid when it's returned */
char viewport[9];

char *display_string()
{
    if (startpos > strlen(scrollmessage) - 9)
    {
        startpos = 0;
    }

    // copy 9 characters from scrollmessage
    strncpy(viewport, &(scrollmessage[startpos]), 9);

    if (debug >= 4) 
    {
        fprintf(stderr, "viewport: |%s|\r\n", viewport);
        sleep(1);
    }

    if (!(get_view_properties() & TEST_MODE)) 
    {
        startpos += 1;
    }

    return &viewport[0];
}


// NOTE: delay is in milliseconds!
int delay;

int get_delay()
{
    return delay;
}

void set_delay(int new_delay)
{
    delay = new_delay;
}

char *get_scrollmessage()
{
    return scrollmessage;
}

/* Set up an interval timer for our scroll box.
 * This is part of the model; as with the clock, it's what actually
 * measures real time passing.
 * When the interval is over, notify tick(), which is part of the
 * controller for this program.
 */
void start_timer()
{
    struct itimerval interval; // interval object
    struct sigaction new_action, old_action;  // signal actions
    int    full_seconds, micro_seconds;
    
    /* See the manual entry for signal.h(3HEAD) for a list of all
     * signals. */

    // blank out the signal mask
    sigemptyset( &new_action.sa_mask );
    // clear flags (our application is pretty simple)
    new_action.sa_flags = 0;
    // set tick() as the signal handler when we get the timer signal.
    new_action.sa_handler = tick;
 
    /* sigaction takes a pointer as an argument, so you have to
     * allocate space.  The declaration of new_action() up above
     * has no star - that's the actual object, not just a pointer.
     */
    if ( sigaction(SIGALRM, &new_action, &old_action) == -1 ) {
        perror("Could not set new handler for SIGALRM");
        exit(1);
    }

    /* set interval to the specified delay value. */
    full_seconds  = delay / 1000;
    micro_seconds = (delay - (full_seconds * 1000) ) * 1000;
    interval.it_value.tv_sec = full_seconds;
    interval.it_value.tv_usec = micro_seconds;
    interval.it_interval = interval.it_value;

    /* set a timer for the real time clock using the interval object */
    /* NOTE: takes a pointer, so no * in our declaration of "interval */
    setitimer(ITIMER_REAL, &interval, NULL);
}
