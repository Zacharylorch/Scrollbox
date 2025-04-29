/* model.c -- Data and Timing for the Scroll Box
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 *
 * Copyright (C) Darren Provine, 2011-2023, All Rights Reserved
 */

#include "scroll.h"

/* the message that we're scrolling */
char scrollmessage[256];
char raw_message[256];
bool original_entered;

static int pixel_offset = 0;
static int start_char = 0;

/* pad scrollmessage with 9 spaces left and right */
void setup(char *text)
{
    snprintf(scrollmessage, 255, "          %s          ", text);

    if (!original_entered)
    {
        strncpy(raw_message, text, sizeof(raw_message));
        original_entered = true;
    }
    
    start_char = 0;
    pixel_offset = 0;

    if (debug >= 2) 
    {
        fprintf(stderr, "text is: |%s|\r\n", scrollmessage);
        sleep(1);
    }
}

char viewport[11]; // 10 characters + 1 null terminator

char *display_string()
{
    int len = strlen(scrollmessage);

    if (pixel_offset >= 8)
    {
        pixel_offset = 0;
        start_char++;

        if (start_char >= len - 10)
        {
            start_char = 0;
        }
    }

    // Build viewport character-by-character, wrapping around manually
    for (int i = 0; i < 10; i++)
    {
        viewport[i] = scrollmessage[(start_char + i) % len];
    }

    viewport[10] = '\0'; // null terminate

    if (!(get_view_properties() & TEST_MODE))
    {
        pixel_offset++;
    }

    return viewport;
}

int get_pixel_offset()
{
    return pixel_offset;
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

char *get_raw_message()
{
    return raw_message;
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
