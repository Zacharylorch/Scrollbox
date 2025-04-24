/* view.h -- #defines for view options
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 * 
 * Copyright (C) Darren Provine, 2009-2023, All Rights Reserved
 */

/* VIEW OPTIONS
 *
 * fast (default is 24-hour) --+
 * reserved -----------------+ |
 * slow -------------------+ | |
 * test -----------------+ | | |
 *                       | | | |
 * snake ----------+     | | | |
 * horizontal ---+ |     | | | |
 * spanish   --+ | |     | | | |
 * inverted -V V V V     V V V V
 *           0 0 0 0     0 0 0 0
 */

 #include <sys/time.h>

 #define  FAST_MODE        0x01
 #define  SLOW_MODE        0x04
 #define  TEST_MODE        0x08
 #define  SNAKE_MODE       0x10
 #define  HORIZONTAL_MODE  0x20
 #define  SPANISH_MODE     0x40
 #define  INVERTED_MODE    0x80
 
 
 // set packed bits for what you want
 void set_view_properties( int );
 int get_view_properties( void );
 
 void show(struct tm *);
