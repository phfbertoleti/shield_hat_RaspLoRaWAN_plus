/* Header file com as imagens a serem exibidas no display */
/* Importante: todas as imagens tem resolução 128x64 */
#ifndef IMAGENS_DEFS_H
#define IMAGENS_DEFS_H

const unsigned char lorawan_logo[] PROGMEM=
{
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7c, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x70, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x01, 0xfc, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1e, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x18, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xf0, 0x00, 0x20, 0x0f, 0xfc, 0x00, 0x00, 0x3c, 0x0f, 0x03, 0xe3, 0xe3, 0xc0, 0xf0, 0x00, 
0x00, 0xf0, 0x03, 0xfe, 0x0f, 0xff, 0x00, 0x00, 0x3c, 0x0f, 0x07, 0xc3, 0xe3, 0xe0, 0xf2, 0x00, 
0x00, 0xf0, 0x07, 0x03, 0x0f, 0xff, 0x80, 0x00, 0x3c, 0x1f, 0x07, 0xc7, 0xe3, 0xe0, 0xf0, 0x00, 
0x00, 0xf0, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x3c, 0x1f, 0x0f, 0x87, 0xe3, 0xf0, 0xf0, 0x00, 
0x00, 0xf0, 0x00, 0x00, 0x0f, 0x03, 0xc0, 0x00, 0x3c, 0x3f, 0x0f, 0x8f, 0xe3, 0xf0, 0xf0, 0x00, 
0x00, 0xf0, 0x01, 0xfc, 0x0f, 0x03, 0xc3, 0xfc, 0x3c, 0x7f, 0x1f, 0x1f, 0xe3, 0xf8, 0xf0, 0x00, 
0x00, 0xf0, 0x03, 0xfe, 0x0f, 0x03, 0xc7, 0xfe, 0x3c, 0x7f, 0x3e, 0x1f, 0xe3, 0xf8, 0xf0, 0x00, 
0x00, 0xf0, 0x07, 0xff, 0x0f, 0x03, 0xc7, 0x9e, 0x3c, 0xff, 0x3e, 0x3f, 0xe3, 0xfc, 0xf0, 0x00, 
0x00, 0xf0, 0x07, 0x8f, 0x8f, 0x07, 0xc7, 0x0e, 0x3d, 0xff, 0x7c, 0x3d, 0xe3, 0xfe, 0xf0, 0x00, 
0x00, 0xf0, 0x0f, 0x07, 0x8f, 0xff, 0xc0, 0x0e, 0x3d, 0xff, 0x7c, 0x79, 0xe3, 0xfe, 0xf0, 0x00, 
0x00, 0xf0, 0x0f, 0x03, 0x8f, 0xff, 0x80, 0x7e, 0x3f, 0xff, 0xf8, 0xf9, 0xe3, 0xff, 0xf0, 0x00, 
0x00, 0xf0, 0x0f, 0x03, 0x8f, 0xff, 0x03, 0xfe, 0x3f, 0xef, 0xf0, 0xf1, 0xe3, 0xdf, 0xf0, 0x00, 
0x00, 0xf0, 0x0f, 0x03, 0x8f, 0xfe, 0x07, 0xfe, 0x3f, 0xcf, 0xf1, 0xf3, 0xe3, 0xcf, 0xf0, 0x00, 
0x00, 0xf0, 0x0f, 0x03, 0x8f, 0x1f, 0x0f, 0x8e, 0x3f, 0xcf, 0xe3, 0xff, 0xe3, 0xcf, 0xf0, 0x00, 
0x00, 0xf0, 0x0f, 0x07, 0x8f, 0x0f, 0x0f, 0x0e, 0x3f, 0x8f, 0xe3, 0xff, 0xe3, 0xc7, 0xf0, 0x00, 
0x00, 0xff, 0xcf, 0x87, 0x8f, 0x0f, 0x8f, 0x1e, 0x3f, 0x0f, 0xc7, 0xff, 0xe3, 0xc7, 0xf0, 0x00, 
0x00, 0xff, 0xe7, 0xcf, 0x0f, 0x07, 0x8f, 0x3e, 0x3f, 0x0f, 0x87, 0xff, 0xe3, 0xc3, 0xf0, 0x00, 
0x00, 0xff, 0xe3, 0xff, 0x0f, 0x07, 0xcf, 0xfe, 0x3e, 0x0f, 0x8f, 0x81, 0xe3, 0xc3, 0xf0, 0x00, 
0x00, 0xff, 0xe1, 0xfe, 0x0f, 0x03, 0xc7, 0xfe, 0x3e, 0x0f, 0x1f, 0x01, 0xe3, 0xc1, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1c, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x0f, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x60, 0x78, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x78, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#endif