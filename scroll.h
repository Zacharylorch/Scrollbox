/* scroll.h -- header for the scroll box
 *
 * Darren Provine, 17 Nov 2012
 */

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include "LEDots.h"

/* defined in "scroll.c" */
void tick(int);

/* defined in "model.c" */
int get_delay();
void set_delay();
void start_timer();
void setup(char *);
char *display_string();

char *get_scrollmessage(void);

/* defined in "view.c" */
void show(char *);


/* debug flag */

extern int debug;

#include "view.h"
