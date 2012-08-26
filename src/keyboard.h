/***************************************************************************
                          keyboard.h  -  Keyboard decoding
                             -------------------
    begin                : Thur Dec 7 2000
    copyright            : (C) 2000 by Simon White
    email                : s_a_white@email.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"

#ifdef _WIN32
#   include <conio.h>
#else
    int _kbhit (void);
#endif

enum
{
    A_NONE = 0,

    // Standard Commands
    A_PREFIX,
    A_SKIP,
    A_END_LIST,
    A_INVALID,

    // Custom Commands
    A_LEFT_ARROW,
    A_RIGHT_ARROW,
    A_UP_ARROW,
    A_DOWN_ARROW,
    A_HOME,
    A_END,
    A_PAUSED,
    A_QUIT,

    /* Debug */
    A_TOGGLE_VOICE1,
    A_TOGGLE_VOICE2,
    A_TOGGLE_VOICE3,
    A_TOGGLE_VOICE4,
    A_TOGGLE_VOICE5,
    A_TOGGLE_VOICE6,
    A_TOGGLE_FILTER
};

int  keyboard_decode      ();
void keyboard_enable_raw  ();
void keyboard_disable_raw ();
