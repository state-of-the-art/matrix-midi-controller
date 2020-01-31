/**
 * State of the Art Matrix Midi Controller
 * 
 * License: Apache v2.0
 * Site: https://state-of-the-art.io/projects/matrix-midi-controller/
 */
 // Small font 4x4
#ifndef FONT_H
#define FONT_H

#define FONT_WIDTH 4
#define FONT_HEIGHT 4

const int font_data[] = {
  // 0 0x00
  0b1110,
  0b1010,
  0b1010,
  0b1110,
  // 1 0x04
  0b0100,
  0b1100,
  0b0100,
  0b1110,
  // 2 0x08
  0b1110,
  0b0010,
  0b0100,
  0b1110,
  // 3 0x0c
  0b1110,
  0b0110,
  0b0110,
  0b1110,
  // 4 0x10
  0b0010,
  0b0110,
  0b1110,
  0b0010,
  // 5 0x14
  0b1110,
  0b1100,
  0b0110,
  0b1110,
  // 6 0x18
  0b0010,
  0b0100,
  0b1110,
  0b1110,
  // 7 0x1c
  0b1110,
  0b0010,
  0b0100,
  0b0100,
  // 8 0x20
  0b1110,
  0b1110,
  0b1010,
  0b1110,
  // 9 0x24
  0b0110,
  0b1010,
  0b0110,
  0b0010,
  // a 0x28
  0b0110,
  0b1010,
  0b1110,
  0b0000,
  // b 0x2c
  0b1000,
  0b1110,
  0b1110,
  0b0000,
  // c 0x30
  0b1110,
  0b1000,
  0b1110,
  0b0000,
  // d 0x34
  0b0010,
  0b1110,
  0b1110,
  0b0000,
  // e 0x38
  0b1110,
  0b1100,
  0b1110,
  0b0000,
  // f 0x3c
  0b1110,
  0b1100,
  0b1000,
  0b0000,
  // g 0x40
  0b1110,
  0b1110,
  0b0010,
  0b1110,
  // h 0x44
  0b1010,
  0b1110,
  0b1010,
  0b0000,
  // i 0x48
  0b0100,
  0b0100,
  0b0100,
  0b0000,
  // j 0x4c
  0b0010,
  0b1010,
  0b1110,
  0b0000,
  // k 0x50
  0b1010,
  0b1100,
  0b1010,
  0b0000,
  // l 0x54
  0b1000,
  0b1000,
  0b1110,
  0b0000,
  // m 0x58
  0b1110,
  0b1110,
  0b1010,
  0b0000,
  // n 0x5c
  0b1100,
  0b1010,
  0b1010,
  0b0000,
  // o 0x60
  0b1110,
  0b1010,
  0b1110,
  0b0000,
  // p 0x64
  0b1110,
  0b1110,
  0b1000,
  0b0000,
  // q 0x68
  0b1110,
  0b1110,
  0b0010,
  0b0010,
  // r 0x6c
  0b1110,
  0b1010,
  0b1000,
  0b0000,
  // s 0x70
  0b0110,
  0b0100,
  0b1100,
  0b0000,
  // t 0x74
  0b1110,
  0b0100,
  0b0100,
  0b0000,
  // u 0x78
  0b1010,
  0b1010,
  0b1110,
  0b0000,
  // v 0x7c
  0b1010,
  0b1010,
  0b0100,
  0b0000,
  // w 0x80
  0b1010,
  0b1110,
  0b1110,
  0b0000,
  // x 0x84
  0b1010,
  0b0100,
  0b1010,
  0b0000,
  // y 0x88
  0b1010,
  0b0100,
  0b0100,
  0b0000,
  // z 0x8c
  0b1100,
  0b0100,
  0b0110,
  0b0000,
  // " " 0x90
  0b0000,
  0b0000,
  0b0000,
  0b0000,
  // _ 0x94
  0b0000,
  0b0000,
  0b1110,
  0b0000,
  // . 0x98
  0b0000,
  0b0000,
  0b0100,
  0b0000,
  // , 0x9c
  0b0000,
  0b0000,
  0b0100,
  0b0100,
};

const int * fontCharGet(char c) {
  if( c >= 48 && c <= 57 ) // 0 - 9
    return &font_data[(c-48)*4];
  else if( c >= 65 && c <= 90 ) // A - Z
    return &font_data[(c-55)*4];
  else if( c >= 97 && c <= 122 ) // a - z
    return &font_data[(c-87)*4];
  else if( c == 32 ) // " "
    return &font_data[0x90];
  else if( c == 46 ) // "."
    return &font_data[0x98];
  else if( c == 44 ) // ","
    return &font_data[0x9c];
  else // "_"
    return &font_data[0x94];
}

#endif // FONT_H
