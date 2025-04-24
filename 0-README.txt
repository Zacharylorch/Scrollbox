SCROLL BOX

As with the clock, we have a file 'LED-layout.txt', which explains which
bits go where to turn on LEDs.  The manufacturer of the display has
also sent us a file 'LEDots.h', which has prototypes and declarations
needed for our compiler, and 'LEDots.o', which has all the object code.
(They did not send us the C source files, because they consider that to
be a trade secret.)

We have a stub scroll box that displays a message only once, and has no
other functions.  We need to finish it.


WHAT YOU NEED TO DO:

1) Figure out how to display a message more than once.

   Note that the stringscroll program has a loop; this display_string()
   function will NOT have a loop.  Each time it's called, it advances
   one position in the string and return the characters it gets, and
   that's all it does.

   The loop for the finished scrollbox is provided by the 'while ( 1 )'
   in main() and the timer, which combine to call display_string()
   repeatedly.  display_string() has the body of the loop, but doesn't
   have a loop of its own.


2) Add code to handle keystrokes and mouse clicks.  You can make the
   box scroll faster/slower by just using set_delay() to lower/raise
   the value of the 'delay' variable in the model.  Put limits: don't
   let delay drop to less than 100, or rise to more than 5000.

   You can make up whatever you want for 'time' and 'test', as long
   as one shows the time for 5 seconds and the other tests all the
   LEDs in one way or another.


3) Get extra credit if you want.

a) Add smooth scrolling.

   Right now, the scrollbox jumps one letter at a time, because it
   copies in full bytes.  A more attractive way for it to work would
   be to step the display one pixel at a time, instead of in blocks.

   This doesn't take much code, but it does take some figuring out.
   It may help to make diagrams and picture how the bits have to be
   shifted and then combined.


b) Adding some things for the second row of buttons to do.

   You can use the second-row keys for whatever.  Previous students
   have created buttons to show the date.  Others have done inverse
   video, vertical scrolling for short messages, mirror-mode where the
   letters are flipped and scroll the other direction so it looks
   right in a mirror.

   If you have an idea but aren't sure how to do it, or whether
   it'll be too hard, talk to me and I'll give suggestions.


c) Add your own option flags:

   Maybe '-c' for 'just be a clock', and then set up the bottom row
   of buttons with AM/PM and 24hr buttons, and even re-use the '-o'
   offset code you wrote for the clock project.

   One student made a clock that counted for 10 seconds, and then
   showed the date for five seconds, and then went back to the time.
   (The date was shown with an abbreviation for the month, as 'Dec 12'.)

   One pair made the time infinitely scroll across the display
   ('... 10:22:14 ... 10:22:15 ... 10:22:16 ...'), but they couldn't
   get it synced so that the seconds never changed while scrolling.


d) Other stuff

   The title bar is 80 characters; so you can put in not only your
   name, but you could put the date in the upper-right.  You could use
   the sysinfo(2) system call to put the total number of running
   processes, or the system load, or some other such information in
   the title bar.

   You could even put the date/time in the title bar continually,
   updating that every second and/or minute while the message scrolls
   by.  (You could add an option flag for whether you want seconds.)

   A combination would be an option flag so that it shows the time
   every minute or so after it gets to the end of the scrolling
   message.


NOTE ON IMPLEMENTATION:

As with the clock, one way to make these features work is to create a
variable in 'model.c' and/or 'view.c' and make up some constants, and
use get/set functions the way the view does in the clock.

#define MODE_NORMAL  0x01
#define MODE_CLOCK   0x02
#define MODE_DATE    0x04
#define MODE_TEST    0x08

Then your Controller module, would have a switch statement where
you check which mode you're in and act accordingly.


***      WARNING WARNING WARNING     ***
***
***   If you add a 'view.h' file, be sure you do 'git add view.h',
***   or the new file won't be included with your project.


Notes:

 1) For modes which are supposed to be temporary, you can add
    a counter for those and decrement it each time.


 2) Anything about the text of the message is in the model.  Show the
    time, or switching to a different message, or anything about the
    text to be displayed, is something the model should do.  'What it
    says' is in the model.


 3) Anything about the way it looks is in the view.  Going to inverse
    video, or putting the characters in backwards and reversing
    direction, is something the view handles.  'How it looks' is in
    the view.

    The 'test' mode, in which we test all the LEDs, is also about
    the view.


 4) Some things require coordination: if you want to do smooth
    scrolling, for example, you'll to update the screen more often.
    That will affect the limits of your fast/slow button and how big
    the steps should be.


If you need to change show() to take extra parameters, feel free to do
so.  Be sure to update the prototype in scroll.h, or the compiler will
fuss at you.
