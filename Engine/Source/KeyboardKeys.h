#ifndef KeyboardKeys_h_
#define KeyboardKeys_h_



typedef enum KeyboardKeys
{
    KeyboardKeys_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
     /* @{ */

    KeyboardKeys_A = 4,
    KeyboardKeys_B = 5,
    KeyboardKeys_C = 6,
    KeyboardKeys_D = 7,
    KeyboardKeys_E = 8,
    KeyboardKeys_F = 9,
    KeyboardKeys_G = 10,
    KeyboardKeys_H = 11,
    KeyboardKeys_I = 12,
    KeyboardKeys_J = 13,
    KeyboardKeys_K = 14,
    KeyboardKeys_L = 15,
    KeyboardKeys_M = 16,
    KeyboardKeys_N = 17,
    KeyboardKeys_O = 18,
    KeyboardKeys_P = 19,
    KeyboardKeys_Q = 20,
    KeyboardKeys_R = 21,
    KeyboardKeys_S = 22,
    KeyboardKeys_T = 23,
    KeyboardKeys_U = 24,
    KeyboardKeys_V = 25,
    KeyboardKeys_W = 26,
    KeyboardKeys_X = 27,
    KeyboardKeys_Y = 28,
    KeyboardKeys_Z = 29,

    KeyboardKeys_1 = 30,
    KeyboardKeys_2 = 31,
    KeyboardKeys_3 = 32,
    KeyboardKeys_4 = 33,
    KeyboardKeys_5 = 34,
    KeyboardKeys_6 = 35,
    KeyboardKeys_7 = 36,
    KeyboardKeys_8 = 37,
    KeyboardKeys_9 = 38,
    KeyboardKeys_0 = 39,

    KeyboardKeys_RETURN = 40,
    KeyboardKeys_ESCAPE = 41,
    KeyboardKeys_BACKSPACE = 42,
    KeyboardKeys_TAB = 43,
    KeyboardKeys_SPACE = 44,

    KeyboardKeys_MINUS = 45,
    KeyboardKeys_EQUALS = 46,
    KeyboardKeys_LEFTBRACKET = 47,
    KeyboardKeys_RIGHTBRACKET = 48,
    KeyboardKeys_BACKSLASH = 49, /**< Located at the lower left of the return
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
    KeyboardKeys_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate KeyboardKeys_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    KeyboardKeys_SEMICOLON = 51,
    KeyboardKeys_APOSTROPHE = 52,
    KeyboardKeys_GRAVE = 53, /**< Located in the top left corner (on both ANSI
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
    KeyboardKeys_COMMA = 54,
    KeyboardKeys_PERIOD = 55,
    KeyboardKeys_SLASH = 56,

    KeyboardKeys_CAPSLOCK = 57,

    KeyboardKeys_F1 = 58,
    KeyboardKeys_F2 = 59,
    KeyboardKeys_F3 = 60,
    KeyboardKeys_F4 = 61,
    KeyboardKeys_F5 = 62,
    KeyboardKeys_F6 = 63,
    KeyboardKeys_F7 = 64,
    KeyboardKeys_F8 = 65,
    KeyboardKeys_F9 = 66,
    KeyboardKeys_F10 = 67,
    KeyboardKeys_F11 = 68,
    KeyboardKeys_F12 = 69,

    KeyboardKeys_PRINTSCREEN = 70,
    KeyboardKeys_SCROLLLOCK = 71,
    KeyboardKeys_PAUSE = 72,
    KeyboardKeys_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    KeyboardKeys_HOME = 74,
    KeyboardKeys_PAGEUP = 75,
    KeyboardKeys_DELETE = 76,
    KeyboardKeys_END = 77,
    KeyboardKeys_PAGEDOWN = 78,
    KeyboardKeys_RIGHT = 79,
    KeyboardKeys_LEFT = 80,
    KeyboardKeys_DOWN = 81,
    KeyboardKeys_UP = 82,

    KeyboardKeys_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    KeyboardKeys_KP_DIVIDE = 84,
    KeyboardKeys_KP_MULTIPLY = 85,
    KeyboardKeys_KP_MINUS = 86,
    KeyboardKeys_KP_PLUS = 87,
    KeyboardKeys_KP_ENTER = 88,
    KeyboardKeys_KP_1 = 89,
    KeyboardKeys_KP_2 = 90,
    KeyboardKeys_KP_3 = 91,
    KeyboardKeys_KP_4 = 92,
    KeyboardKeys_KP_5 = 93,
    KeyboardKeys_KP_6 = 94,
    KeyboardKeys_KP_7 = 95,
    KeyboardKeys_KP_8 = 96,
    KeyboardKeys_KP_9 = 97,
    KeyboardKeys_KP_0 = 98,
    KeyboardKeys_KP_PERIOD = 99,

    KeyboardKeys_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    KeyboardKeys_APPLICATION = 101, /**< windows contextual menu, compose */
    KeyboardKeys_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    KeyboardKeys_KP_EQUALS = 103,
    KeyboardKeys_F13 = 104,
    KeyboardKeys_F14 = 105,
    KeyboardKeys_F15 = 106,
    KeyboardKeys_F16 = 107,
    KeyboardKeys_F17 = 108,
    KeyboardKeys_F18 = 109,
    KeyboardKeys_F19 = 110,
    KeyboardKeys_F20 = 111,
    KeyboardKeys_F21 = 112,
    KeyboardKeys_F22 = 113,
    KeyboardKeys_F23 = 114,
    KeyboardKeys_F24 = 115,
    KeyboardKeys_EXECUTE = 116,
    KeyboardKeys_HELP = 117,
    KeyboardKeys_MENU = 118,
    KeyboardKeys_SELECT = 119,
    KeyboardKeys_STOP = 120,
    KeyboardKeys_AGAIN = 121,   /**< redo */
    KeyboardKeys_UNDO = 122,
    KeyboardKeys_CUT = 123,
    KeyboardKeys_COPY = 124,
    KeyboardKeys_PASTE = 125,
    KeyboardKeys_FIND = 126,
    KeyboardKeys_MUTE = 127,
    KeyboardKeys_VOLUMEUP = 128,
    KeyboardKeys_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     KeyboardKeys_LOCKINGCAPSLOCK = 130,  */
    /*     KeyboardKeys_LOCKINGNUMLOCK = 131, */
    /*     KeyboardKeys_LOCKINGSCROLLLOCK = 132, */
    KeyboardKeys_KP_COMMA = 133,
    KeyboardKeys_KP_EQUALSAS400 = 134,

    KeyboardKeys_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    KeyboardKeys_INTERNATIONAL2 = 136,
    KeyboardKeys_INTERNATIONAL3 = 137, /**< Yen */
    KeyboardKeys_INTERNATIONAL4 = 138,
    KeyboardKeys_INTERNATIONAL5 = 139,
    KeyboardKeys_INTERNATIONAL6 = 140,
    KeyboardKeys_INTERNATIONAL7 = 141,
    KeyboardKeys_INTERNATIONAL8 = 142,
    KeyboardKeys_INTERNATIONAL9 = 143,
    KeyboardKeys_LANG1 = 144, /**< Hangul/English toggle */
    KeyboardKeys_LANG2 = 145, /**< Hanja conversion */
    KeyboardKeys_LANG3 = 146, /**< Katakana */
    KeyboardKeys_LANG4 = 147, /**< Hiragana */
    KeyboardKeys_LANG5 = 148, /**< Zenkaku/Hankaku */
    KeyboardKeys_LANG6 = 149, /**< reserved */
    KeyboardKeys_LANG7 = 150, /**< reserved */
    KeyboardKeys_LANG8 = 151, /**< reserved */
    KeyboardKeys_LANG9 = 152, /**< reserved */

    KeyboardKeys_ALTERASE = 153, /**< Erase-Eaze */
    KeyboardKeys_SYSREQ = 154,
    KeyboardKeys_CANCEL = 155,
    KeyboardKeys_CLEAR = 156,
    KeyboardKeys_PRIOR = 157,
    KeyboardKeys_RETURN2 = 158,
    KeyboardKeys_SEPARATOR = 159,
    KeyboardKeys_OUT = 160,
    KeyboardKeys_OPER = 161,
    KeyboardKeys_CLEARAGAIN = 162,
    KeyboardKeys_CRSEL = 163,
    KeyboardKeys_EXSEL = 164,

    KeyboardKeys_KP_00 = 176,
    KeyboardKeys_KP_000 = 177,
    KeyboardKeys_THOUSANDSSEPARATOR = 178,
    KeyboardKeys_DECIMALSEPARATOR = 179,
    KeyboardKeys_CURRENCYUNIT = 180,
    KeyboardKeys_CURRENCYSUBUNIT = 181,
    KeyboardKeys_KP_LEFTPAREN = 182,
    KeyboardKeys_KP_RIGHTPAREN = 183,
    KeyboardKeys_KP_LEFTBRACE = 184,
    KeyboardKeys_KP_RIGHTBRACE = 185,
    KeyboardKeys_KP_TAB = 186,
    KeyboardKeys_KP_BACKSPACE = 187,
    KeyboardKeys_KP_A = 188,
    KeyboardKeys_KP_B = 189,
    KeyboardKeys_KP_C = 190,
    KeyboardKeys_KP_D = 191,
    KeyboardKeys_KP_E = 192,
    KeyboardKeys_KP_F = 193,
    KeyboardKeys_KP_XOR = 194,
    KeyboardKeys_KP_POWER = 195,
    KeyboardKeys_KP_PERCENT = 196,
    KeyboardKeys_KP_LESS = 197,
    KeyboardKeys_KP_GREATER = 198,
    KeyboardKeys_KP_AMPERSAND = 199,
    KeyboardKeys_KP_DBLAMPERSAND = 200,
    KeyboardKeys_KP_VERTICALBAR = 201,
    KeyboardKeys_KP_DBLVERTICALBAR = 202,
    KeyboardKeys_KP_COLON = 203,
    KeyboardKeys_KP_HASH = 204,
    KeyboardKeys_KP_SPACE = 205,
    KeyboardKeys_KP_AT = 206,
    KeyboardKeys_KP_EXCLAM = 207,
    KeyboardKeys_KP_MEMSTORE = 208,
    KeyboardKeys_KP_MEMRECALL = 209,
    KeyboardKeys_KP_MEMCLEAR = 210,
    KeyboardKeys_KP_MEMADD = 211,
    KeyboardKeys_KP_MEMSUBTRACT = 212,
    KeyboardKeys_KP_MEMMULTIPLY = 213,
    KeyboardKeys_KP_MEMDIVIDE = 214,
    KeyboardKeys_KP_PLUSMINUS = 215,
    KeyboardKeys_KP_CLEAR = 216,
    KeyboardKeys_KP_CLEARENTRY = 217,
    KeyboardKeys_KP_BINARY = 218,
    KeyboardKeys_KP_OCTAL = 219,
    KeyboardKeys_KP_DECIMAL = 220,
    KeyboardKeys_KP_HEXADECIMAL = 221,

    KeyboardKeys_LCTRL = 224,
    KeyboardKeys_LSHIFT = 225,
    KeyboardKeys_LALT = 226, /**< alt, option */
    KeyboardKeys_LGUI = 227, /**< windows, command (apple), meta */
    KeyboardKeys_RCTRL = 228,
    KeyboardKeys_RSHIFT = 229,
    KeyboardKeys_RALT = 230, /**< alt gr, option */
    KeyboardKeys_RGUI = 231, /**< windows, command (apple), meta */

    KeyboardKeys_MODE = 257,    /**< I'm not sure if this is really not covered
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

    KeyboardKeys_AUDIONEXT = 258,
    KeyboardKeys_AUDIOPREV = 259,
    KeyboardKeys_AUDIOSTOP = 260,
    KeyboardKeys_AUDIOPLAY = 261,
    KeyboardKeys_AUDIOMUTE = 262,
    KeyboardKeys_MEDIASELECT = 263,
    KeyboardKeys_WWW = 264,
    KeyboardKeys_MAIL = 265,
    KeyboardKeys_CALCULATOR = 266,
    KeyboardKeys_COMPUTER = 267,
    KeyboardKeys_AC_SEARCH = 268,
    KeyboardKeys_AC_HOME = 269,
    KeyboardKeys_AC_BACK = 270,
    KeyboardKeys_AC_FORWARD = 271,
    KeyboardKeys_AC_STOP = 272,
    KeyboardKeys_AC_REFRESH = 273,
    KeyboardKeys_AC_BOOKMARKS = 274,

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
     /* @{ */

    KeyboardKeys_BRIGHTNESSDOWN = 275,
    KeyboardKeys_BRIGHTNESSUP = 276,
    KeyboardKeys_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    KeyboardKeys_KBDILLUMTOGGLE = 278,
    KeyboardKeys_KBDILLUMDOWN = 279,
    KeyboardKeys_KBDILLUMUP = 280,
    KeyboardKeys_EJECT = 281,
    KeyboardKeys_SLEEP = 282,

    KeyboardKeys_APP1 = 283,
    KeyboardKeys_APP2 = 284,

    /* @} *//* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
     /* @{ */

    KeyboardKeys_AUDIOREWIND = 285,
    KeyboardKeys_AUDIOFASTFORWARD = 286,

    /* @} *//* Usage page 0x0C (additional media keys) */

    /* Add any other keys here. */

        KeyboardKeys_NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} KeyboardKeys;

#endif /* KeyboardKeys_h_ */