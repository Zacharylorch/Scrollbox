/* view.h -- #defines for view options
 *
 * Zachary Lorch, Noel Abastillas, Tess Erickson, 24 April 2025
 * 
 * Copyright (C) Darren Provine, 2009-2023, All Rights Reserved
 */

/* VIEW OPTIONS
 *
 * test (default is 24-hour) --+
 * reserved -----------------+ |
 * snake ------------------+ | |
 * horizontal -----------+ | | |
 *                       | | | |
 * spanish --------+     | | | |
 * inverted -----+ |     | | | |
 * x ----------+ | |     | | | |
 * x --------V V V V     V V V V
 *           0 0 0 0     0 0 0 0
 */

 #include <sys/time.h>

 #define  TEST_MODE        0x01
 #define  SNAKE_MODE       0x04
 #define  HORIZONTAL_MODE  0x08
 #define  SPANISH_MODE     0x10
 #define  INVERTED_MODE    0x20
 //#define                   0x40
 //#define                   0x80
 
 
 // set packed bits for what you want
 void set_view_properties( int );
 int get_view_properties( void );
