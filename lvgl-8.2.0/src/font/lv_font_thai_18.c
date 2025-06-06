﻿/*******************************************************************************
 * Size: 18 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "../../lvgl.h"
#endif

#ifndef LV_FONT_THAI_18
#define LV_FONT_THAI_18 1
#endif

#if LV_FONT_THAI_18

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xaa, 0xaa, 0x8f,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x9, 0x82, 0x40, 0x91, 0xff, 0x11, 0x4, 0x81,
    0x23, 0xff, 0x32, 0x8, 0x82, 0x40,

    /* U+0024 "$" */
    0x10, 0x20, 0xf6, 0x89, 0x12, 0x34, 0x38, 0x1c,
    0x2c, 0x48, 0x99, 0x7f, 0x4, 0x8,

    /* U+0025 "%" */
    0x78, 0x26, 0x62, 0x21, 0x31, 0x9, 0xc, 0xd0,
    0x3d, 0x80, 0x9, 0xc0, 0x91, 0xc, 0x88, 0x44,
    0x44, 0x22, 0x60, 0xe0,

    /* U+0026 "&" */
    0x1e, 0x1, 0x98, 0x8, 0x40, 0x42, 0x21, 0x61,
    0x1e, 0x9, 0x8c, 0xd0, 0x34, 0x80, 0xe4, 0x6,
    0x10, 0x78, 0x7c, 0x70,

    /* U+0028 "(" */
    0x26, 0x44, 0x88, 0x88, 0x88, 0x84, 0x46, 0x20,

    /* U+0029 ")" */
    0x46, 0x22, 0x11, 0x11, 0x11, 0x12, 0x26, 0x40,

    /* U+002A "*" */
    0x20, 0x8f, 0xcc, 0x51, 0x20,

    /* U+002B "+" */
    0x8, 0x8, 0x8, 0xff, 0x8, 0x8, 0x8, 0x8,

    /* U+002C "," */
    0x69, 0x60,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x2, 0xc, 0x10, 0x60, 0x83, 0x4, 0x8, 0x30,
    0x41, 0x82, 0x4, 0x18, 0x20, 0x0,

    /* U+0030 "0" */
    0x3c, 0x62, 0x43, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0xc2, 0x42, 0x3c,

    /* U+0031 "1" */
    0x17, 0xd1, 0x11, 0x11, 0x11, 0x11,

    /* U+0032 "2" */
    0x7c, 0x8c, 0x8, 0x10, 0x20, 0x83, 0x18, 0x61,
    0x83, 0x7, 0xf0,

    /* U+0033 "3" */
    0x7c, 0x46, 0x2, 0x2, 0x4, 0x38, 0x6, 0x1,
    0x1, 0x1, 0x82, 0x7c,

    /* U+0034 "4" */
    0x2, 0x3, 0x3, 0x81, 0x41, 0x21, 0x91, 0x88,
    0x84, 0xff, 0x81, 0x0, 0x80, 0x40,

    /* U+0035 "5" */
    0x7e, 0x81, 0x2, 0x7, 0x80, 0x80, 0x81, 0x2,
    0x6, 0x17, 0xc0,

    /* U+0036 "6" */
    0x1e, 0x20, 0x40, 0x80, 0xbc, 0xc2, 0x81, 0x81,
    0x81, 0x81, 0x42, 0x3c,

    /* U+0037 "7" */
    0xff, 0x3, 0x6, 0x4, 0xc, 0x8, 0x8, 0x18,
    0x10, 0x30, 0x30, 0x30,

    /* U+0038 "8" */
    0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x42, 0x81,
    0x81, 0x81, 0x42, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x42, 0x81, 0x81, 0x81, 0x43, 0x3d, 0x1,
    0x1, 0x2, 0x84, 0xf8,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0x6c, 0x0, 0x3, 0x49, 0x40,

    /* U+003C "<" */
    0x3, 0xc, 0x30, 0xe0, 0x60, 0x18, 0x6, 0x1,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0xc0, 0x70, 0x18, 0x6, 0xe, 0x38, 0xe0, 0x80,

    /* U+003F "?" */
    0x7a, 0x30, 0x41, 0x8, 0x42, 0x8, 0x0, 0x3,
    0xc,

    /* U+0040 "@" */
    0xf, 0xc0, 0x60, 0xc2, 0x1, 0x90, 0xf2, 0xc4,
    0x46, 0x21, 0x18, 0x84, 0x62, 0x11, 0x88, 0x46,
    0x33, 0x24, 0x77, 0x18, 0x0, 0x30, 0x0, 0x3f,
    0x80,

    /* U+0041 "A" */
    0x6, 0x1, 0xc0, 0x2c, 0x4, 0x81, 0x90, 0x23,
    0xc, 0x21, 0xfe, 0x20, 0x4c, 0x9, 0x1, 0xe0,
    0x10,

    /* U+0042 "B" */
    0xfd, 0xe, 0xc, 0x18, 0x5f, 0x21, 0xc1, 0x83,
    0x6, 0x17, 0xc0,

    /* U+0043 "C" */
    0xf, 0xcc, 0x14, 0x1, 0x0, 0x80, 0x20, 0x8,
    0x2, 0x0, 0x80, 0x10, 0x3, 0x4, 0x7e,

    /* U+0044 "D" */
    0xfe, 0x20, 0xc8, 0xa, 0x3, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x80, 0xa0, 0x28, 0x33, 0xf0,

    /* U+0045 "E" */
    0xfe, 0x8, 0x20, 0x83, 0xf8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0x83, 0xf8, 0x20, 0x82, 0x8,
    0x20,

    /* U+0047 "G" */
    0xf, 0xcc, 0x14, 0x1, 0x0, 0x80, 0x20, 0x8,
    0x3e, 0x1, 0xc0, 0x50, 0x13, 0x4, 0x7e,

    /* U+0048 "H" */
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xf, 0xfe, 0x3,
    0x1, 0x80, 0xc0, 0x60, 0x30, 0x10,

    /* U+0049 "I" */
    0xff, 0xf0,

    /* U+004A "J" */
    0x11, 0x11, 0x11, 0x11, 0x11, 0x2e,

    /* U+004B "K" */
    0x82, 0x86, 0x8c, 0x98, 0xb0, 0xe0, 0xe0, 0xb0,
    0x98, 0x8c, 0x86, 0x83,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+004D "M" */
    0xc0, 0x78, 0xf, 0x83, 0xd8, 0xd9, 0x13, 0x36,
    0x62, 0x8c, 0x71, 0x84, 0x30, 0x6, 0x0, 0xc0,
    0x10,

    /* U+004E "N" */
    0xc0, 0x78, 0x1e, 0x6, 0xc1, 0x98, 0x63, 0x18,
    0xc6, 0x19, 0x83, 0x60, 0x78, 0x1e, 0x3,

    /* U+004F "O" */
    0x1f, 0x83, 0xc, 0x40, 0x2c, 0x3, 0x80, 0x18,
    0x1, 0x80, 0x18, 0x1, 0xc0, 0x34, 0x2, 0x30,
    0xc1, 0xf8,

    /* U+0050 "P" */
    0xf9, 0xe, 0xc, 0x18, 0x30, 0xbe, 0x40, 0x81,
    0x2, 0x4, 0x0,

    /* U+0051 "Q" */
    0x1f, 0x83, 0xc, 0x40, 0x2c, 0x3, 0x80, 0x18,
    0x1, 0x80, 0x18, 0x1, 0xc0, 0x34, 0x2, 0x30,
    0xc1, 0xf8, 0x3, 0x0, 0x18, 0x0, 0x70,

    /* U+0052 "R" */
    0xfc, 0x86, 0x82, 0x82, 0x82, 0x84, 0xf8, 0x8c,
    0x84, 0x86, 0x83, 0x83,

    /* U+0053 "S" */
    0x3e, 0x86, 0x4, 0xc, 0xe, 0x7, 0x3, 0x2,
    0x6, 0x1f, 0xc0,

    /* U+0054 "T" */
    0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+0055 "U" */
    0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x80, 0xc0, 0x50, 0x47, 0xc0,

    /* U+0056 "V" */
    0xc0, 0x68, 0x9, 0x83, 0x30, 0x62, 0x8, 0x63,
    0x4, 0x60, 0xc8, 0x1b, 0x1, 0x40, 0x38, 0x3,
    0x0,

    /* U+0057 "W" */
    0xc0, 0x81, 0x20, 0xe0, 0x98, 0x50, 0xcc, 0x28,
    0x62, 0x36, 0x21, 0x1b, 0x30, 0xc8, 0x98, 0x24,
    0x48, 0x16, 0x34, 0xe, 0xe, 0x7, 0x6, 0x1,
    0x83, 0x0,

    /* U+0058 "X" */
    0x60, 0xc8, 0x23, 0x18, 0x6c, 0x1a, 0x3, 0x80,
    0xe0, 0x68, 0x13, 0xc, 0x66, 0x9, 0x3,

    /* U+0059 "Y" */
    0xc1, 0xa0, 0x98, 0xc4, 0x43, 0x60, 0xa0, 0x70,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+005A "Z" */
    0xff, 0x80, 0x80, 0x80, 0xc0, 0x40, 0x40, 0x60,
    0x20, 0x20, 0x30, 0x10, 0x1f, 0xf0,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0x38,

    /* U+005C "\\" */
    0x81, 0x81, 0x3, 0x2, 0x4, 0xc, 0x8, 0x18,
    0x10, 0x20, 0x60, 0x40, 0xc0, 0x80,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x92, 0x78,

    /* U+005E "^" */
    0x18, 0x18, 0x3c, 0x24, 0x62, 0x42, 0xc1,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xc8, 0x80,

    /* U+0061 "a" */
    0x7c, 0x8c, 0x8, 0x77, 0xf0, 0x60, 0xc3, 0x7a,

    /* U+0062 "b" */
    0x80, 0x80, 0x80, 0x80, 0xbc, 0xc2, 0x81, 0x81,
    0x81, 0x81, 0x83, 0xc2, 0xbc,

    /* U+0063 "c" */
    0x3e, 0xc3, 0x4, 0x8, 0x10, 0x20, 0x21, 0x3e,

    /* U+0064 "d" */
    0x1, 0x1, 0x1, 0x1, 0x3d, 0x43, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x43, 0x3d,

    /* U+0065 "e" */
    0x3c, 0x42, 0xc1, 0x81, 0xff, 0x80, 0x80, 0x41,
    0x3e,

    /* U+0066 "f" */
    0x1c, 0x82, 0x8, 0xf8, 0x82, 0x8, 0x20, 0x82,
    0x8, 0x20,

    /* U+0067 "g" */
    0x3d, 0x43, 0xc1, 0x81, 0x81, 0x81, 0x81, 0x43,
    0x3d, 0x1, 0x1, 0x82, 0xfc,

    /* U+0068 "h" */
    0x81, 0x2, 0x4, 0xb, 0xd8, 0xe0, 0xc1, 0x83,
    0x6, 0xc, 0x18, 0x20,

    /* U+0069 "i" */
    0xcc, 0x0, 0x88, 0x88, 0x88, 0x88, 0x70,

    /* U+006A "j" */
    0x11, 0x0, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12,
    0xe0,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x8, 0xd3, 0x24, 0x50, 0xe1,
    0x62, 0x64, 0x68, 0x60,

    /* U+006C "l" */
    0x92, 0x49, 0x24, 0x92, 0x46,

    /* U+006D "m" */
    0xbc, 0xf6, 0x38, 0xe0, 0x83, 0x4, 0x18, 0x20,
    0xc1, 0x6, 0x8, 0x30, 0x41, 0x82, 0x8,

    /* U+006E "n" */
    0xbd, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x82,

    /* U+006F "o" */
    0x3e, 0x30, 0xb0, 0x30, 0x18, 0xc, 0x6, 0x2,
    0xc2, 0x3e, 0x0,

    /* U+0070 "p" */
    0xbc, 0xc2, 0x81, 0x81, 0x81, 0x81, 0x83, 0xc2,
    0xbc, 0x80, 0x80, 0x80, 0x80,

    /* U+0071 "q" */
    0x3d, 0x43, 0x81, 0x81, 0x81, 0x81, 0x81, 0x43,
    0x3d, 0x1, 0x1, 0x1, 0x1,

    /* U+0072 "r" */
    0xbe, 0x21, 0x8, 0x42, 0x10, 0x80,

    /* U+0073 "s" */
    0x7e, 0x18, 0x30, 0x78, 0x30, 0x61, 0xf8,

    /* U+0074 "t" */
    0x1, 0x9, 0xf2, 0x10, 0x84, 0x21, 0x8, 0x30,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xe3, 0x7a,

    /* U+0076 "v" */
    0xc1, 0x21, 0x98, 0xcc, 0x42, 0x61, 0xa0, 0x50,
    0x38, 0x18, 0x0,

    /* U+0077 "w" */
    0xc2, 0x1a, 0x18, 0x91, 0x44, 0xca, 0x62, 0x5a,
    0x14, 0x50, 0xe2, 0x87, 0x1c, 0x18, 0xc0,

    /* U+0078 "x" */
    0x43, 0x62, 0x24, 0x1c, 0x18, 0x1c, 0x24, 0x66,
    0x43,

    /* U+0079 "y" */
    0x83, 0xc3, 0x42, 0x66, 0x64, 0x24, 0x3c, 0x18,
    0x18, 0x10, 0x10, 0x20, 0xc0,

    /* U+007A "z" */
    0xfe, 0x8, 0x20, 0xc1, 0x4, 0x18, 0x20, 0xfe,

    /* U+007B "{" */
    0x29, 0x24, 0x94, 0x49, 0x24, 0x88,

    /* U+007C "|" */
    0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0x89, 0x24, 0x91, 0x49, 0x24, 0xa0,

    /* U+007E "~" */
    0x70, 0xce, 0x61, 0xc0,

    /* U+00B2 "²" */
    0x74, 0xc2, 0x31, 0x11, 0x1f,

    /* U+00B3 "³" */
    0xe1, 0x1e, 0x31, 0x3e,

    /* U+00D7 "×" */
    0x40, 0x62, 0x34, 0x18, 0x1c, 0x26, 0x43, 0x0,

    /* U+00E2 "â" */
    0x18, 0x59, 0x10, 0x7, 0xc8, 0xc0, 0x87, 0x7f,
    0x6, 0xc, 0x37, 0xa0,

    /* U+00E7 "ç" */
    0x3e, 0xc3, 0x4, 0x8, 0x10, 0x20, 0x21, 0x3e,
    0x20, 0x70, 0x21, 0x80,

    /* U+00EA "ê" */
    0x18, 0x24, 0x42, 0x0, 0x3c, 0x42, 0xc1, 0x81,
    0xff, 0x80, 0x80, 0x41, 0x3e,

    /* U+00F1 "ñ" */
    0x75, 0x38, 0x5, 0xec, 0x70, 0x60, 0xc1, 0x83,
    0x6, 0xc, 0x10,

    /* U+00FC "ü" */
    0x64, 0x0, 0x4, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0x7, 0x1b, 0xd0,

    /* U+0161 "š" */
    0x89, 0x42, 0x0, 0x7e, 0x18, 0x30, 0x78, 0x30,
    0x61, 0xf8,

    /* U+0E01 "ก" */
    0x3c, 0x42, 0x81, 0xe1, 0x41, 0x81, 0x81, 0x81,
    0x81,

    /* U+0E02 "ข" */
    0x3c, 0x51, 0x96, 0x25, 0x51, 0x68, 0x42, 0x10,
    0x84, 0x21, 0xf, 0x80,

    /* U+0E03 "ฃ" */
    0x2c, 0x55, 0x96, 0x65, 0x61, 0x68, 0x42, 0x10,
    0x84, 0x21, 0xf, 0x80,

    /* U+0E04 "ค" */
    0x3c, 0xc2, 0x81, 0x9d, 0xb5, 0x5d, 0x41, 0x41,
    0x41,

    /* U+0E05 "ฅ" */
    0x76, 0xd9, 0x81, 0x99, 0xb5, 0x55, 0x59, 0x41,
    0x41,

    /* U+0E06 "ฆ" */
    0x2c, 0x55, 0x96, 0x65, 0x61, 0x68, 0x42, 0x13,
    0xf4, 0xa7, 0x38, 0x40,

    /* U+0E07 "ง" */
    0xc, 0x50, 0xe1, 0xc5, 0x93, 0x47, 0xc,

    /* U+0E08 "จ" */
    0x79, 0x88, 0x8, 0x17, 0x2b, 0x5e, 0x83, 0x2,

    /* U+0E09 "ฉ" */
    0x3c, 0x10, 0x80, 0x11, 0x84, 0xa1, 0x18, 0x42,
    0xfc, 0xe5, 0x21, 0xc0,

    /* U+0E0A "ช" */
    0x0, 0x27, 0x89, 0x1a, 0x31, 0x25, 0x44, 0xd0,
    0x82, 0x10, 0x42, 0x8, 0x41, 0xf0,

    /* U+0E0B "ซ" */
    0x0, 0x25, 0x89, 0x5a, 0x33, 0x25, 0x84, 0xd0,
    0x82, 0x10, 0x42, 0x8, 0x41, 0xf0,

    /* U+0E0C "ฌ" */
    0x3c, 0x14, 0x61, 0x82, 0x1e, 0x21, 0x42, 0x18,
    0x21, 0xef, 0xda, 0xa7, 0xee, 0x10,

    /* U+0E0D "ญ" */
    0x3c, 0x14, 0x61, 0x82, 0x1e, 0x21, 0x42, 0x18,
    0x21, 0xe2, 0x1a, 0x21, 0xe3, 0xe0, 0x39, 0x3,
    0xb0, 0x3c,

    /* U+0E0E "ฎ" */
    0xf, 0x4, 0x22, 0x4, 0xe1, 0x10, 0x48, 0x12,
    0x5, 0x81, 0x60, 0x41, 0x11, 0xe4, 0xb7, 0x18,
    0xc0,

    /* U+0E0F "ฏ" */
    0xf, 0x8, 0xc8, 0x27, 0x13, 0x9, 0x5, 0x83,
    0x41, 0x6c, 0x9c, 0x4b, 0xe6, 0xd0,

    /* U+0E10 "ฐ" */
    0x3e, 0x81, 0x3, 0xe6, 0x2a, 0x54, 0xb9, 0x1c,
    0x1, 0xdd, 0xb6, 0xe0,

    /* U+0E11 "ฑ" */
    0x2c, 0x6a, 0xdd, 0x9a, 0xac, 0x96, 0x92, 0x14,
    0x42, 0x88, 0x61, 0xc, 0x20,

    /* U+0E12 "ฒ" */
    0x66, 0xe, 0xd8, 0x60, 0x43, 0x32, 0x1a, 0x90,
    0xac, 0x85, 0x7f, 0xae, 0xa7, 0x67, 0x8,

    /* U+0E13 "ณ" */
    0x3c, 0x10, 0x8c, 0x22, 0x8, 0x47, 0x10, 0x84,
    0x21, 0x10, 0x42, 0x38, 0xbf, 0x51, 0xca, 0xe2,
    0x1c,

    /* U+0E14 "ด" */
    0x3c, 0xc2, 0x81, 0x99, 0x95, 0x5d, 0x49, 0x71,
    0x61,

    /* U+0E15 "ต" */
    0x66, 0xdb, 0x81, 0x99, 0x95, 0x5d, 0x49, 0x71,
    0x61,

    /* U+0E16 "ถ" */
    0x3c, 0x42, 0x81, 0xe1, 0x41, 0x81, 0xe1, 0xa1,
    0xe1,

    /* U+0E17 "ท" */
    0x61, 0xd1, 0xd8, 0xa4, 0x92, 0x49, 0x44, 0xa2,
    0x61, 0x30, 0x80,

    /* U+0E18 "ธ" */
    0x7f, 0x2, 0x7, 0xe0, 0x30, 0x60, 0xc1, 0xfc,

    /* U+0E19 "น" */
    0x61, 0x28, 0x46, 0x10, 0x84, 0x21, 0x8, 0x42,
    0xfc, 0xe5, 0x21, 0xc0,

    /* U+0E1A "บ" */
    0x60, 0xd0, 0x58, 0x24, 0x12, 0x9, 0x4, 0x82,
    0x41, 0x3f, 0x0,

    /* U+0E1B "ป" */
    0x0, 0x80, 0x40, 0x2c, 0x1a, 0xb, 0x4, 0x82,
    0x41, 0x20, 0x90, 0x48, 0x27, 0xe0,

    /* U+0E1C "ผ" */
    0xe1, 0xa1, 0xe1, 0x81, 0x99, 0x9d, 0xa5, 0xc3,
    0x81,

    /* U+0E1D "ฝ" */
    0x1, 0x1, 0x1, 0xe1, 0xa1, 0xe1, 0x81, 0x99,
    0x9d, 0xa5, 0xc3, 0x81,

    /* U+0E1E "พ" */
    0x66, 0x69, 0x96, 0x64, 0xa5, 0x29, 0x4e, 0x73,
    0xc, 0xc3, 0x30, 0xc0,

    /* U+0E1F "ฟ" */
    0x0, 0x40, 0x10, 0x5, 0x99, 0xa6, 0x59, 0x92,
    0x94, 0xa5, 0x39, 0xcc, 0x33, 0xc, 0xc3,

    /* U+0E20 "ภ" */
    0xf, 0x4, 0x32, 0x4, 0xe1, 0x30, 0x48, 0x16,
    0x6, 0x81, 0x60, 0x40,

    /* U+0E21 "ม" */
    0x61, 0xa1, 0x61, 0x21, 0x21, 0x21, 0xfd, 0xa7,
    0xe1,

    /* U+0E22 "ย" */
    0xe3, 0x47, 0x8c, 0x1e, 0x38, 0x60, 0xc1, 0xfc,

    /* U+0E23 "ร" */
    0x7f, 0x83, 0xe0, 0x20, 0x40, 0x83, 0xa, 0xc,

    /* U+0E24 "ฤ" */
    0x3c, 0x42, 0x81, 0xe1, 0x41, 0x81, 0x81, 0xe1,
    0xe1, 0x1, 0x1, 0x1, 0x1,

    /* U+0E25 "ล" */
    0x79, 0x8, 0xb, 0xdc, 0x70, 0x70, 0xd1, 0xc2,

    /* U+0E26 "ฦ" */
    0xf, 0x4, 0x22, 0x4, 0xe1, 0x10, 0x48, 0x12,
    0x5, 0x81, 0x60, 0x40, 0x10, 0x4, 0x1, 0x0,
    0x40,

    /* U+0E27 "ว" */
    0x7d, 0x8c, 0x8, 0x10, 0x20, 0x41, 0x85, 0x6,

    /* U+0E28 "ศ" */
    0x3f, 0xc7, 0x81, 0x99, 0xb5, 0x55, 0x59, 0x41,
    0x41,

    /* U+0E29 "ษ" */
    0x60, 0x94, 0x11, 0x82, 0x97, 0x52, 0xac, 0x5f,
    0x8, 0x21, 0x4, 0x3f, 0x0,

    /* U+0E2A "ส" */
    0x7f, 0x1c, 0xb, 0xdc, 0x70, 0x70, 0xd1, 0xc2,

    /* U+0E2B "ห" */
    0x63, 0x53, 0x58, 0xc4, 0x72, 0x49, 0x44, 0xc2,
    0x61, 0x20, 0x80,

    /* U+0E2C "ฬ" */
    0x0, 0xd0, 0x17, 0x66, 0xca, 0x64, 0x66, 0x42,
    0x94, 0x29, 0x43, 0x9c, 0x30, 0xc3, 0xc, 0x30,
    0xc0,

    /* U+0E2D "อ" */
    0x7c, 0xc2, 0x1, 0x71, 0x51, 0x71, 0x41, 0x63,
    0x3e,

    /* U+0E2E "ฮ" */
    0x7d, 0x47, 0x7e, 0x62, 0xa2, 0xa2, 0x62, 0x42,
    0x3c,

    /* U+0E2F "ฯ" */
    0x2, 0xce, 0x1c, 0x57, 0x20, 0x40, 0x81, 0x2,

    /* U+0E30 "ะ" */
    0xe6, 0xaf, 0x0, 0x3, 0x9a, 0xbc,

    /* U+0E31 "ั" */
    0xe6, 0xaf, 0x0,

    /* U+0E32 "า" */
    0x7d, 0x8c, 0x8, 0x10, 0x20, 0x40, 0x81, 0x2,

    /* U+0E33 "ำ" */
    0xc0, 0x8, 0xc, 0x0, 0x3e, 0x8, 0xc0, 0x10,
    0x4, 0x1, 0x0, 0x40, 0x10, 0x4, 0x1,

    /* U+0E34 "ิ" */
    0x38, 0x89, 0xf8, 0x0,

    /* U+0E35 "ี" */
    0x7b, 0xf, 0xe8, 0x20,

    /* U+0E36 "ึ" */
    0x77, 0x1f, 0xf0, 0x0,

    /* U+0E37 "ื" */
    0x7f, 0xf, 0xe8, 0x20,

    /* U+0E38 "ุ" */
    0xf5,

    /* U+0E39 "ู" */
    0xce, 0x5c,

    /* U+0E3A "ฺ" */
    0xc0,

    /* U+0E3F "฿" */
    0x10, 0x10, 0xfc, 0x96, 0x92, 0x92, 0x94, 0xfc,
    0x92, 0x91, 0x91, 0x91, 0x92, 0xfc, 0x10, 0x10,

    /* U+0E40 "เ" */
    0x92, 0x49, 0x3d, 0xe0,

    /* U+0E41 "แ" */
    0x89, 0x12, 0x24, 0x48, 0x91, 0x3b, 0xd5, 0xee,

    /* U+0E42 "โ" */
    0x7e, 0x83, 0xc0, 0x40, 0x81, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x81, 0xc2, 0x87,

    /* U+0E43 "ใ" */
    0x3c, 0xcd, 0x52, 0xc6, 0x81, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x81, 0xc2, 0x87,

    /* U+0E44 "ไ" */
    0x99, 0x52, 0xa5, 0x46, 0x81, 0x2, 0x4, 0x8,
    0x10, 0x20, 0x40, 0x81, 0xc2, 0x87,

    /* U+0E45 "ๅ" */
    0x7d, 0x8c, 0x8, 0x10, 0x20, 0x40, 0x81, 0x2,
    0x4, 0x8, 0x10, 0x20,

    /* U+0E46 "ๆ" */
    0x9, 0xf8, 0x79, 0xa7, 0x90, 0x41, 0x4, 0x10,
    0x41, 0x4,

    /* U+0E47 "็" */
    0x1, 0xf8, 0x6f, 0x4c,

    /* U+0E48 "่" */
    0xe0,

    /* U+0E49 "้" */
    0x66, 0x96, 0xdc,

    /* U+0E4A "๊" */
    0x13, 0xb6, 0xaf, 0x60,

    /* U+0E4B "๋" */
    0x5d, 0x0,

    /* U+0E4C "์" */
    0x5, 0xfa, 0x87, 0x0,

    /* U+0E4D "ํ" */
    0xe7, 0x80,

    /* U+0E4E "๎" */
    0x6e, 0x70,

    /* U+0E4F "๏" */
    0x3c, 0x42, 0x81, 0x9d, 0x9d, 0x81, 0x42, 0x3c,

    /* U+0E50 "๐" */
    0x3c, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3c,

    /* U+0E51 "๑" */
    0x3e, 0x20, 0xa6, 0x32, 0x99, 0xce, 0x4d, 0xec,

    /* U+0E52 "๒" */
    0x80, 0x40, 0x20, 0x10, 0x9, 0xf4, 0xa6, 0xf3,
    0x35, 0x9c, 0xc0, 0x5f, 0xc0,

    /* U+0E53 "๓" */
    0x77, 0x64, 0xe2, 0x30, 0x1b, 0xf, 0x4d, 0xcc,

    /* U+0E54 "๔" */
    0x0, 0x80, 0x41, 0xc7, 0x4, 0x4, 0x3a, 0x3d,
    0x1e, 0xc6, 0x3f, 0xc0,

    /* U+0E55 "๕" */
    0x18, 0x96, 0xcb, 0xc7, 0x4, 0x4, 0x3a, 0x3b,
    0x1e, 0xc6, 0x3f, 0xc0,

    /* U+0E56 "๖" */
    0xc0, 0x40, 0x60, 0x3e, 0x3, 0x71, 0x51, 0x51,
    0x72, 0x3c,

    /* U+0E57 "๗" */
    0x0, 0x20, 0x4, 0x0, 0x80, 0x17, 0x63, 0x92,
    0x62, 0x4c, 0x9, 0xb9, 0x3d, 0x25, 0xe7, 0x0,

    /* U+0E58 "๘" */
    0x0, 0x80, 0x20, 0x70, 0xe0, 0x40, 0x20, 0x68,
    0x1a, 0x76, 0xf6, 0x9c, 0xe0,

    /* U+0E59 "๙" */
    0x0, 0x40, 0x97, 0xd7, 0x32, 0x8c, 0x21, 0xb,
    0xe3, 0xa8, 0x7b, 0x0,

    /* U+0E5A "๚" */
    0x2, 0xb3, 0x61, 0xb1, 0x57, 0x28, 0x14, 0xa,
    0x5, 0x2, 0x80,

    /* U+0E5B "๛" */
    0x78, 0x1, 0x80, 0x2, 0x0, 0x4, 0x35, 0x8,
    0x6a, 0x50, 0xeb, 0xb3, 0x50, 0x3c, 0x0,

    /* U+2018 "‘" */
    0x5e,

    /* U+2019 "’" */
    0x5a,

    /* U+201C "“" */
    0x4a, 0xf5, 0x20,

    /* U+201D "”" */
    0x5a, 0xa5, 0x20
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 79, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 82, .box_w = 2, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 113, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 6, .adv_w = 170, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 20, .adv_w = 155, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 34, .adv_w = 236, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 54, .adv_w = 230, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 87, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 82, .adv_w = 87, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 90, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 95, .adv_w = 197, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 103, .adv_w = 62, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 105, .adv_w = 115, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 106, .adv_w = 62, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 112, .box_w = 7, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 121, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 155, .box_w = 4, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 155, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 162, .adv_w = 155, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 176, .adv_w = 155, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 223, .adv_w = 155, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 235, .adv_w = 62, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 238, .adv_w = 62, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 243, .adv_w = 197, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 251, .adv_w = 197, .box_w = 8, .box_h = 5, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 256, .adv_w = 197, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 264, .adv_w = 129, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 273, .adv_w = 275, .box_w = 14, .box_h = 14, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 298, .adv_w = 186, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 315, .adv_w = 165, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 178, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 202, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 356, .adv_w = 146, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 141, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 198, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 389, .adv_w = 204, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 77, .box_w = 1, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 103, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 411, .adv_w = 167, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 423, .adv_w = 136, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 432, .adv_w = 236, .box_w = 11, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 449, .adv_w = 215, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 464, .adv_w = 217, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 482, .adv_w = 161, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 493, .adv_w = 217, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 516, .adv_w = 172, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 153, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 539, .adv_w = 151, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 553, .adv_w = 198, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 567, .adv_w = 179, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 269, .box_w = 17, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 610, .adv_w = 170, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 625, .adv_w = 159, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 639, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 653, .adv_w = 87, .box_w = 3, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 659, .adv_w = 109, .box_w = 7, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 673, .adv_w = 87, .box_w = 3, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 679, .adv_w = 197, .box_w = 8, .box_h = 7, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 686, .adv_w = 120, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 687, .adv_w = 77, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 10},
    {.bitmap_index = 689, .adv_w = 147, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 697, .adv_w = 169, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 710, .adv_w = 133, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 718, .adv_w = 170, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 731, .adv_w = 151, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 740, .adv_w = 90, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 750, .adv_w = 170, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 763, .adv_w = 163, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 775, .adv_w = 73, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 782, .adv_w = 70, .box_w = 4, .box_h = 17, .ofs_x = -2, .ofs_y = -4},
    {.bitmap_index = 791, .adv_w = 143, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 803, .adv_w = 73, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 808, .adv_w = 248, .box_w = 13, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 163, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 831, .adv_w = 169, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 842, .adv_w = 169, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 855, .adv_w = 170, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 868, .adv_w = 100, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 874, .adv_w = 122, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 881, .adv_w = 98, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 889, .adv_w = 163, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 897, .adv_w = 138, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 908, .adv_w = 208, .box_w = 13, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 923, .adv_w = 132, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 932, .adv_w = 139, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 945, .adv_w = 130, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 953, .adv_w = 87, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 959, .adv_w = 69, .box_w = 1, .box_h = 18, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 962, .adv_w = 87, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 968, .adv_w = 197, .box_w = 9, .box_h = 3, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 972, .adv_w = 105, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 977, .adv_w = 105, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 981, .adv_w = 197, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 989, .adv_w = 147, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1001, .adv_w = 133, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1013, .adv_w = 151, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1026, .adv_w = 163, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1037, .adv_w = 163, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1048, .adv_w = 122, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1058, .adv_w = 165, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1067, .adv_w = 174, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1079, .adv_w = 177, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1091, .adv_w = 166, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1100, .adv_w = 166, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1109, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1121, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1128, .adv_w = 147, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1136, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1148, .adv_w = 173, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1162, .adv_w = 175, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1176, .adv_w = 239, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1190, .adv_w = 238, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1208, .adv_w = 172, .box_w = 10, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1225, .adv_w = 172, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1239, .adv_w = 140, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1251, .adv_w = 195, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1264, .adv_w = 244, .box_w = 13, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1279, .adv_w = 257, .box_w = 15, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1296, .adv_w = 166, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1305, .adv_w = 166, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1314, .adv_w = 165, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1323, .adv_w = 170, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1334, .adv_w = 147, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1342, .adv_w = 171, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1354, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1365, .adv_w = 164, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1379, .adv_w = 172, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1388, .adv_w = 172, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1400, .adv_w = 192, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1412, .adv_w = 192, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1427, .adv_w = 172, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1439, .adv_w = 162, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1448, .adv_w = 151, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1456, .adv_w = 124, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1464, .adv_w = 165, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1477, .adv_w = 155, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1485, .adv_w = 172, .box_w = 10, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1502, .adv_w = 131, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1510, .adv_w = 166, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1519, .adv_w = 170, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1532, .adv_w = 155, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1540, .adv_w = 170, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1551, .adv_w = 193, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1568, .adv_w = 152, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1577, .adv_w = 155, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1586, .adv_w = 146, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1594, .adv_w = 98, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1600, .adv_w = 0, .box_w = 6, .box_h = 3, .ofs_x = -6, .ofs_y = 10},
    {.bitmap_index = 1603, .adv_w = 131, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1611, .adv_w = 131, .box_w = 10, .box_h = 12, .ofs_x = -3, .ofs_y = 0},
    {.bitmap_index = 1626, .adv_w = 0, .box_w = 7, .box_h = 4, .ofs_x = -9, .ofs_y = 9},
    {.bitmap_index = 1630, .adv_w = 0, .box_w = 7, .box_h = 4, .ofs_x = -8, .ofs_y = 9},
    {.bitmap_index = 1634, .adv_w = 0, .box_w = 7, .box_h = 4, .ofs_x = -8, .ofs_y = 9},
    {.bitmap_index = 1638, .adv_w = 0, .box_w = 7, .box_h = 4, .ofs_x = -9, .ofs_y = 9},
    {.bitmap_index = 1642, .adv_w = 0, .box_w = 2, .box_h = 4, .ofs_x = -3, .ofs_y = -5},
    {.bitmap_index = 1643, .adv_w = 0, .box_w = 5, .box_h = 3, .ofs_x = -6, .ofs_y = -4},
    {.bitmap_index = 1645, .adv_w = 0, .box_w = 2, .box_h = 1, .ofs_x = -3, .ofs_y = -2},
    {.bitmap_index = 1646, .adv_w = 168, .box_w = 8, .box_h = 16, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 1662, .adv_w = 77, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1666, .adv_w = 139, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1674, .adv_w = 118, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1688, .adv_w = 113, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1702, .adv_w = 115, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1716, .adv_w = 131, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 1728, .adv_w = 128, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1738, .adv_w = 0, .box_w = 6, .box_h = 5, .ofs_x = -8, .ofs_y = 9},
    {.bitmap_index = 1742, .adv_w = 0, .box_w = 1, .box_h = 3, .ofs_x = -2, .ofs_y = 10},
    {.bitmap_index = 1743, .adv_w = 0, .box_w = 6, .box_h = 4, .ofs_x = -7, .ofs_y = 9},
    {.bitmap_index = 1746, .adv_w = 0, .box_w = 7, .box_h = 4, .ofs_x = -7, .ofs_y = 10},
    {.bitmap_index = 1750, .adv_w = 0, .box_w = 3, .box_h = 3, .ofs_x = -3, .ofs_y = 10},
    {.bitmap_index = 1752, .adv_w = 0, .box_w = 7, .box_h = 4, .ofs_x = -6, .ofs_y = 9},
    {.bitmap_index = 1756, .adv_w = 0, .box_w = 3, .box_h = 3, .ofs_x = -3, .ofs_y = 9},
    {.bitmap_index = 1758, .adv_w = 0, .box_w = 3, .box_h = 4, .ofs_x = -4, .ofs_y = 9},
    {.bitmap_index = 1760, .adv_w = 156, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1768, .adv_w = 162, .box_w = 8, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1775, .adv_w = 177, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1783, .adv_w = 186, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1796, .adv_w = 171, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1804, .adv_w = 172, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1816, .adv_w = 172, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1828, .adv_w = 141, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1838, .adv_w = 210, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1854, .adv_w = 181, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1867, .adv_w = 178, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1879, .adv_w = 186, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1890, .adv_w = 256, .box_w = 15, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1905, .adv_w = 66, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 1906, .adv_w = 66, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 1907, .adv_w = 109, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 1910, .adv_w = 109, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 8}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_2[] = {
    0x0, 0x1, 0x25, 0x30, 0x35, 0x38, 0x3f, 0x4a,
    0xaf
};

static const uint8_t glyph_id_ofs_list_5[] = {
    0, 1, 0, 0, 2, 3
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 7, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 40, .range_length = 87, .glyph_id_start = 8,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 178, .range_length = 176, .glyph_id_start = 95,
        .unicode_list = unicode_list_2, .glyph_id_ofs_list = NULL, .list_length = 9, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 3585, .range_length = 58, .glyph_id_start = 104,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 3647, .range_length = 29, .glyph_id_start = 162,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 8216, .range_length = 6, .glyph_id_start = 191,
        .unicode_list = NULL, .glyph_id_ofs_list = glyph_id_ofs_list_5, .list_length = 6, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 6,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
lv_font_t lv_font_thai_18 = {
#else
lv_font_t lv_font_thai_18 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 21,          /*The maximum line height required by the font*/
    .base_line = 5,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_FONT_THAI_18*/

