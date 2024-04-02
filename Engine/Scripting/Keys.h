#ifndef Keys_h_
#define Keys_h_

typedef enum
{
    Keys_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
     /* @{ */

    Keys_A = 4,
    Keys_B = 5,
    Keys_C = 6,
    Keys_D = 7,
    Keys_E = 8,
    Keys_F = 9,
    Keys_G = 10,
    Keys_H = 11,
    Keys_I = 12,
    Keys_J = 13,
    Keys_K = 14,
    Keys_L = 15,
    Keys_M = 16,
    Keys_N = 17,
    Keys_O = 18,
    Keys_P = 19,
    Keys_Q = 20,
    Keys_R = 21,
    Keys_S = 22,
    Keys_T = 23,
    Keys_U = 24,
    Keys_V = 25,
    Keys_W = 26,
    Keys_X = 27,
    Keys_Y = 28,
    Keys_Z = 29,

    Keys_1 = 30,
    Keys_2 = 31,
    Keys_3 = 32,
    Keys_4 = 33,
    Keys_5 = 34,
    Keys_6 = 35,
    Keys_7 = 36,
    Keys_8 = 37,
    Keys_9 = 38,
    Keys_0 = 39,

    Keys_RETURN = 40,
    Keys_ESCAPE = 41,
    Keys_BACKSPACE = 42,
    Keys_TAB = 43,
    Keys_SPACE = 44,

    Keys_MINUS = 45,
    Keys_EQUALS = 46,
    Keys_LEFTBRACKET = 47,
    Keys_RIGHTBRACKET = 48,
    Keys_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    Keys_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate Keys_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    Keys_SEMICOLON = 51,
    Keys_APOSTROPHE = 52,
    Keys_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    Keys_COMMA = 54,
    Keys_PERIOD = 55,
    Keys_SLASH = 56,

    Keys_CAPSLOCK = 57,

    Keys_F1 = 58,
    Keys_F2 = 59,
    Keys_F3 = 60,
    Keys_F4 = 61,
    Keys_F5 = 62,
    Keys_F6 = 63,
    Keys_F7 = 64,
    Keys_F8 = 65,
    Keys_F9 = 66,
    Keys_F10 = 67,
    Keys_F11 = 68,
    Keys_F12 = 69,

    Keys_PRINTSCREEN = 70,
    Keys_SCROLLLOCK = 71,
    Keys_PAUSE = 72,
    Keys_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    Keys_HOME = 74,
    Keys_PAGEUP = 75,
    Keys_DELETE = 76,
    Keys_END = 77,
    Keys_PAGEDOWN = 78,
    Keys_RIGHT = 79,
    Keys_LEFT = 80,
    Keys_DOWN = 81,
    Keys_UP = 82,

    Keys_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    Keys_KP_DIVIDE = 84,
    Keys_KP_MULTIPLY = 85,
    Keys_KP_MINUS = 86,
    Keys_KP_PLUS = 87,
    Keys_KP_ENTER = 88,
    Keys_KP_1 = 89,
    Keys_KP_2 = 90,
    Keys_KP_3 = 91,
    Keys_KP_4 = 92,
    Keys_KP_5 = 93,
    Keys_KP_6 = 94,
    Keys_KP_7 = 95,
    Keys_KP_8 = 96,
    Keys_KP_9 = 97,
    Keys_KP_0 = 98,
    Keys_KP_PERIOD = 99,

    Keys_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    Keys_APPLICATION = 101, /**< windows contextual menu, compose */
    Keys_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    Keys_KP_EQUALS = 103,
    Keys_F13 = 104,
    Keys_F14 = 105,
    Keys_F15 = 106,
    Keys_F16 = 107,
    Keys_F17 = 108,
    Keys_F18 = 109,
    Keys_F19 = 110,
    Keys_F20 = 111,
    Keys_F21 = 112,
    Keys_F22 = 113,
    Keys_F23 = 114,
    Keys_F24 = 115,
    Keys_EXECUTE = 116,
    Keys_HELP = 117,
    Keys_MENU = 118,
    Keys_SELECT = 119,
    Keys_STOP = 120,
    Keys_AGAIN = 121,   /**< redo */
    Keys_UNDO = 122,
    Keys_CUT = 123,
    Keys_COPY = 124,
    Keys_PASTE = 125,
    Keys_FIND = 126,
    Keys_MUTE = 127,
    Keys_VOLUMEUP = 128,
    Keys_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     Keys_LOCKINGCAPSLOCK = 130,  */
    /*     Keys_LOCKINGNUMLOCK = 131, */
    /*     Keys_LOCKINGSCROLLLOCK = 132, */
    Keys_KP_COMMA = 133,
    Keys_KP_EQUALSAS400 = 134,

    Keys_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    Keys_INTERNATIONAL2 = 136,
    Keys_INTERNATIONAL3 = 137, /**< Yen */
    Keys_INTERNATIONAL4 = 138,
    Keys_INTERNATIONAL5 = 139,
    Keys_INTERNATIONAL6 = 140,
    Keys_INTERNATIONAL7 = 141,
    Keys_INTERNATIONAL8 = 142,
    Keys_INTERNATIONAL9 = 143,
    Keys_LANG1 = 144, /**< Hangul/English toggle */
    Keys_LANG2 = 145, /**< Hanja conversion */
    Keys_LANG3 = 146, /**< Katakana */
    Keys_LANG4 = 147, /**< Hiragana */
    Keys_LANG5 = 148, /**< Zenkaku/Hankaku */
    Keys_LANG6 = 149, /**< reserved */
    Keys_LANG7 = 150, /**< reserved */
    Keys_LANG8 = 151, /**< reserved */
    Keys_LANG9 = 152, /**< reserved */

    Keys_ALTERASE = 153, /**< Erase-Eaze */
    Keys_SYSREQ = 154,
    Keys_CANCEL = 155,
    Keys_CLEAR = 156,
    Keys_PRIOR = 157,
    Keys_RETURN2 = 158,
    Keys_SEPARATOR = 159,
    Keys_OUT = 160,
    Keys_OPER = 161,
    Keys_CLEARAGAIN = 162,
    Keys_CRSEL = 163,
    Keys_EXSEL = 164,

    Keys_KP_00 = 176,
    Keys_KP_000 = 177,
    Keys_THOUSANDSSEPARATOR = 178,
    Keys_DECIMALSEPARATOR = 179,
    Keys_CURRENCYUNIT = 180,
    Keys_CURRENCYSUBUNIT = 181,
    Keys_KP_LEFTPAREN = 182,
    Keys_KP_RIGHTPAREN = 183,
    Keys_KP_LEFTBRACE = 184,
    Keys_KP_RIGHTBRACE = 185,
    Keys_KP_TAB = 186,
    Keys_KP_BACKSPACE = 187,
    Keys_KP_A = 188,
    Keys_KP_B = 189,
    Keys_KP_C = 190,
    Keys_KP_D = 191,
    Keys_KP_E = 192,
    Keys_KP_F = 193,
    Keys_KP_XOR = 194,
    Keys_KP_POWER = 195,
    Keys_KP_PERCENT = 196,
    Keys_KP_LESS = 197,
    Keys_KP_GREATER = 198,
    Keys_KP_AMPERSAND = 199,
    Keys_KP_DBLAMPERSAND = 200,
    Keys_KP_VERTICALBAR = 201,
    Keys_KP_DBLVERTICALBAR = 202,
    Keys_KP_COLON = 203,
    Keys_KP_HASH = 204,
    Keys_KP_SPACE = 205,
    Keys_KP_AT = 206,
    Keys_KP_EXCLAM = 207,
    Keys_KP_MEMSTORE = 208,
    Keys_KP_MEMRECALL = 209,
    Keys_KP_MEMCLEAR = 210,
    Keys_KP_MEMADD = 211,
    Keys_KP_MEMSUBTRACT = 212,
    Keys_KP_MEMMULTIPLY = 213,
    Keys_KP_MEMDIVIDE = 214,
    Keys_KP_PLUSMINUS = 215,
    Keys_KP_CLEAR = 216,
    Keys_KP_CLEARENTRY = 217,
    Keys_KP_BINARY = 218,
    Keys_KP_OCTAL = 219,
    Keys_KP_DECIMAL = 220,
    Keys_KP_HEXADECIMAL = 221,

    Keys_LCTRL = 224,
    Keys_LSHIFT = 225,
    Keys_LALT = 226, /**< alt, option */
    Keys_LGUI = 227, /**< windows, command (apple), meta */
    Keys_RCTRL = 228,
    Keys_RSHIFT = 229,
    Keys_RALT = 230, /**< alt gr, option */
    Keys_RGUI = 231, /**< windows, command (apple), meta */

    Keys_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

                                 /* @} *//* Usage page 0x07 */

                                 /**
                                  *  \name Usage page 0x0C
                                  *
                                  *  These values are mapped from usage page 0x0C (USB consumer page).
                                  */
                                  /* @{ */

    Keys_AUDIONEXT = 258,
    Keys_AUDIOPREV = 259,
    Keys_AUDIOSTOP = 260,
    Keys_AUDIOPLAY = 261,
    Keys_AUDIOMUTE = 262,
    Keys_MEDIASELECT = 263,
    Keys_WWW = 264,
    Keys_MAIL = 265,
    Keys_CALCULATOR = 266,
    Keys_COMPUTER = 267,
    Keys_AC_SEARCH = 268,
    Keys_AC_HOME = 269,
    Keys_AC_BACK = 270,
    Keys_AC_FORWARD = 271,
    Keys_AC_STOP = 272,
    Keys_AC_REFRESH = 273,
    Keys_AC_BOOKMARKS = 274,

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
     /* @{ */

    Keys_BRIGHTNESSDOWN = 275,
    Keys_BRIGHTNESSUP = 276,
    Keys_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    Keys_KBDILLUMTOGGLE = 278,
    Keys_KBDILLUMDOWN = 279,
    Keys_KBDILLUMUP = 280,
    Keys_EJECT = 281,
    Keys_SLEEP = 282,

    Keys_APP1 = 283,
    Keys_APP2 = 284,

    /* @} *//* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
     /* @{ */

    Keys_AUDIOREWIND = 285,
    Keys_AUDIOFASTFORWARD = 286,

    /* @} *//* Usage page 0x0C (additional media keys) */

    /* Add any other keys here. */

    SDL_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} Keys;

#endif /* Keys_h_ */

/* vi: set ts=4 sw=4 expandtab: */
