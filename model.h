/* view.h -- #defines for view options
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 * 
 * Copyright (C) Darren Provine, 2009-2023, All Rights Reserved
 */

/* VIEW OPTIONS
 *
 * test -----------------------+
 * time     -----------------+ |
 * snake ------------------+ | |
 * up -------------------+ | | |
 *                       | | | |
 * down -----------+     | | | |
 * language -----+ |     | | | |
 * bounce -----+ | |     | | | |
 * dvd ------V V V V     V V V V
 *           0 0 0 0     0 0 0 0
 */

 #include <sys/time.h>

 #define  TEST_MODE        0x01
 #define  TIME_MODE        0x02
 #define  SNAKE_MODE       0x04
 #define  UP_MODE          0x08
 #define  DOWN_MODE        0x10
 #define  LANGUAGE_MODE    0x20
 #define  BOUNCE_MODE      0x40
 #define  DVD_MODE         0x80
 
 
 // set packed bits for what you want
 void set_view_properties( int );
 int get_view_properties( void );
