/* scroll.c -- scrolling display
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 *
 * Copyright (C) Darren Provine, 2011-2023, All Rights Reserved
 */

#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

// sleep needs this
#include <unistd.h>

#include "scroll.h"
#include "view.h"

#define API_KEY "AIzaSyAQKA4pRSdByfuEK24i-Q2mZTgbH3vDFco"
#define TARGET_LANG "es"

// debug flag - changed via -D option
int debug = 0;

int language_menu = 0;
char *pending_translation_lang = NULL;
int translation_in_progress = 0;

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

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(1);
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t total = size * nmemb;
    char *new_ptr = realloc(s->ptr, s->len + total + 1);
    if (!new_ptr) return 0;
    s->ptr = new_ptr;
    memcpy(s->ptr + s->len, ptr, total);
    s->len += total;
    s->ptr[s->len] = '\0';
    return total;
}

char *parse_translated_text(const char *json)
{
    const char *start = strstr(json, "\"translatedText\":");
    if (!start) return NULL;

    // Move past the key
    start = strchr(start, '"');  // to the first quote of the key
    if (!start) return NULL;
    start = strchr(start + 1, '"');  // to the closing quote of "translatedText"
    if (!start) return NULL;
    start = strchr(start + 1, '"');  // to the opening quote of value
    if (!start) return NULL;
    start++;  // move past the opening quote of value

    const char *end = strchr(start, '"');
    if (!end) return NULL;

    size_t len = end - start;
    char *result = malloc(len + 1);
    if (!result) return NULL;

    strncpy(result, start, len);
    result[len] = '\0';

    return result;
}

void translate_and_set(const char *target_lang)
{
    CURL *curl;
    CURLcode res;
    struct string s;
    char post_data[1024];

    const char *original = get_raw_message();

    snprintf(post_data, sizeof(post_data),
        "{\"q\": \"%s\", \"target\": \"%s\", \"format\": \"text\"}",
        original, target_lang);

    init_string(&s);
    curl = curl_easy_init();

    if (curl)
    {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL,
            "https://translation.googleapis.com/language/translate/v2?key=" API_KEY);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res == CURLE_OK)
        {
            char *translated = parse_translated_text(s.ptr);
            if (translated)
            {
                setup(translated);
                free(translated);
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        free(s.ptr);
    }
}

int test_mode_end = INT_MAX;
int time_mode_end = INT_MAX;

// keyboard and mouse handling
void process_key(keybits KeyCode)
{
    int row, col;
    int view_props;

    const int DELAY_INTERVAL = 100;
    const int DELAY_MIN      = 0;
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
            scroll_display *panel = get_panel();

            if (language_menu)
            {
                switch (col)
                {
                    case 0: // Close
                        setup(get_raw_message()); 
                        language_menu = 0;
                        panel->set_key(0, "Snake");
                        panel->set_key(1, "Up");
                        panel->set_key(2, "Down");
                        panel->set_key(3, "Lang");
                        panel->set_key(4, "Bounce");
                        break;
                    case 1: 
                        setup(get_raw_message()); 
                        break;
                    case 2: 
                        pending_translation_lang = strdup("es");
                        translation_in_progress = 1;
                        break;
                    case 3: 
                        pending_translation_lang = strdup("it");
                        translation_in_progress = 1;
                        break;
                    case 4: 
                        pending_translation_lang = strdup("fr");
                        translation_in_progress = 1;
                        break;
                }
            }
            else
            {
                switch(col)
                {
                    case 0:
                        break;
                    case 1:
                        view_props = get_view_properties();
                        view_props |= ( UP_MODE );
                        set_view_properties (view_props);
                        break;
                    case 2:
                        view_props = get_view_properties();
                        view_props |= ( DOWN_MODE );
                        set_view_properties (view_props);
                        break;
                    case 3:
                        language_menu = 1;
                        strncpy(saved_message, get_scrollmessage(), sizeof(saved_message));
                        panel->set_key(0, "Close");
                        panel->set_key(1, "En");
                        panel->set_key(2, "Es");
                        panel->set_key(3, "It");
                        panel->set_key(4, "Fr");
                        break;
                    case 4:
                        break;
                }
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
    int            initial_delay = 200;
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
    panel->set_key(0, "Snake");
    panel->set_key(1, "Up");
    panel->set_key(2, "Down");
    panel->set_key(3, "Lang");
    panel->set_key(4, "Bounce");

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

    if (translation_in_progress && pending_translation_lang)
    {
        setup("Translating...");
        show(display_string());
        get_panel()->update();

        translate_and_set(pending_translation_lang); 
        free(pending_translation_lang);
        pending_translation_lang = NULL;
        translation_in_progress = 0;

        return; // Skip other view updates this tick
    }

    static int last_time_mode = 0;

    if ((view_props & TIME_MODE) && !last_time_mode)
    {
        strncpy(saved_message, get_scrollmessage(), sizeof(saved_message)); // Save original
        setup(make_timestring());  // Set up the time text ONCE
        last_time_mode = 1;
    }

    if (!(view_props & TIME_MODE) && last_time_mode)
    {
        setup(get_raw_message());      // Restore original text
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
    }

    if (get_view_properties() & TEST_MODE)
    {
        show(NULL);  // triggers show() to run its TEST_MODE logic
        return;             // don't continue to scroll
    }

    /* Send those chars to the view. */
    show(segment);
}

