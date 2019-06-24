// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t Arimo_Regular_30Bitmaps[] PROGMEM = {

	// Bitmap Data:
	0x00, // ' '
	0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x03,0xFE, // '!'
	0xE3,0xF1,0xF8,0xDC,0x66,0x33,0x18, // '"'
	0x03,0x06,0x01,0x83,0x00,0x81,0x00,0x40,0x80,0x60,0xC0,0x30,0x61,0xFF,0xFE,0xFF,0xFF,0x04,0x08,0x06,0x0C,0x03,0x06,0x01,0x03,0x00,0x81,0x07,0xFF,0xFB,0xFF,0xFC,0x30,0x60,0x10,0x30,0x08,0x10,0x0C,0x18,0x06,0x0C,0x03,0x06,0x00, // '#'
	0x00,0xC0,0x00,0x60,0x01,0xFC,0x03,0xFF,0x83,0xCD,0xE3,0x86,0x39,0xC3,0x1C,0xE1,0x80,0x70,0xC0,0x1C,0x60,0x07,0xF0,0x01,0xFF,0x00,0x1F,0xE0,0x06,0xF8,0x03,0x1C,0x01,0x86,0x60,0xC3,0xB0,0x61,0x9C,0x31,0xC7,0x99,0xE1,0xFF,0xE0,0x3F,0xC0,0x03,0x00,0x01,0x80, // '$'
	0x3E,0x00,0x30,0x3F,0x80,0x38,0x18,0x60,0x18,0x18,0x30,0x18,0x0C,0x18,0x1C,0x06,0x0C,0x0C,0x03,0x06,0x0C,0x01,0x83,0x0E,0x00,0xE1,0x86,0x00,0x30,0xC6,0x1F,0x1F,0xC7,0x1F,0xC3,0xC3,0x18,0x60,0x03,0x0C,0x38,0x03,0x86,0x0C,0x01,0x87,0x06,0x01,0x81,0x83,0x01,0xC0,0xC1,0x80,0xC0,0x61,0xC0,0xC0,0x30,0xC0,0xE0,0x0F,0xE0,0x60,0x03,0xC0, // '%'
	0x03,0xF0,0x00,0xFF,0x00,0x38,0x70,0x06,0x06,0x00,0xC0,0xC0,0x18,0x38,0x03,0x0E,0x00,0x73,0x80,0x07,0xE0,0x01,0xF0,0x00,0x7C,0x04,0x3D,0xC1,0xC7,0x1C,0x31,0xC1,0x86,0x38,0x39,0x87,0x03,0xB0,0xE0,0x3C,0x1C,0x03,0x81,0xC1,0xF8,0x1F,0xFB,0xF0,0xFC,0x3E, // '&'
	0xFF,0xF0, // '''
	0x06,0x0E,0x1C,0x18,0x38,0x30,0x70,0x60,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0x60,0x70,0x30,0x38,0x18,0x1C,0x0E,0x06, // '('
	0x60,0x70,0x38,0x18,0x1C,0x0C,0x0E,0x06,0x06,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x06,0x06,0x0E,0x0C,0x1C,0x18,0x38,0x70,0x60, // ')'
	0x0C,0x01,0x80,0x30,0x66,0x6F,0xFC,0x7E,0x07,0x80,0xF0,0x33,0x0E,0x70,0x84,0x00, // '*'
	0x03,0x00,0x0C,0x00,0x30,0x00,0xC0,0x03,0x00,0x0C,0x0F,0xFF,0xFF,0xFF,0x03,0x00,0x0C,0x00,0x30,0x00,0xC0,0x03,0x00,0x0C,0x00, // '+'
	0xFF,0xA5,0xB0, // ','
	0xFF,0xFF, // '-'
	0xFC, // '.'
	0x03,0x07,0x06,0x06,0x06,0x0E,0x0C,0x0C,0x0C,0x1C,0x18,0x18,0x38,0x38,0x30,0x30,0x70,0x60,0x60,0x60,0xE0,0xC0, // '/'
	0x07,0xC0,0x3F,0xE0,0xF1,0xE3,0x80,0xC7,0x01,0xCC,0x01,0xB8,0x03,0x70,0x07,0xE0,0x0F,0xC0,0x1F,0x80,0x3F,0x00,0x7E,0x00,0xFC,0x01,0xD8,0x03,0x30,0x06,0x70,0x1C,0xE0,0x30,0xF1,0xE0,0xFF,0x80,0x7C,0x00, // '0'
	0x07,0x00,0x78,0x0F,0xC0,0xE6,0x06,0x30,0x21,0x80,0x0C,0x00,0x60,0x03,0x00,0x18,0x00,0xC0,0x06,0x00,0x30,0x01,0x80,0x0C,0x00,0x60,0x03,0x00,0x18,0x00,0xC1,0xFF,0xFF,0xFF,0x80, // '1'
	0x0F,0xC1,0xFF,0x1E,0x3D,0xC0,0x7E,0x03,0xA0,0x1C,0x00,0xE0,0x07,0x00,0x30,0x03,0x80,0x38,0x07,0x80,0x78,0x07,0x00,0x70,0x07,0x00,0x70,0x07,0x00,0x38,0x01,0xFF,0xFF,0xFF,0x80, // '2'
	0x0F,0xE0,0x3F,0xE0,0xF1,0xE3,0x80,0xE6,0x01,0xC4,0x03,0x80,0x07,0x00,0x0E,0x00,0x78,0x0F,0xC0,0x1F,0x80,0x03,0xC0,0x01,0xC0,0x01,0x80,0x03,0xF0,0x07,0xE0,0x0E,0xE0,0x39,0xE0,0xF1,0xFF,0xC0,0xFE,0x00, // '3'
	0x00,0x70,0x01,0xE0,0x03,0xC0,0x0D,0x80,0x3B,0x00,0x66,0x01,0x8C,0x07,0x18,0x1C,0x30,0x30,0x60,0xE0,0xC3,0x81,0x86,0x03,0x1C,0x06,0x3F,0xFF,0xFF,0xFF,0x00,0x30,0x00,0x60,0x00,0xC0,0x01,0x80,0x03,0x00, // '4'
	0x7F,0xFC,0xFF,0xF9,0xC0,0x03,0x80,0x07,0x00,0x0E,0x00,0x1C,0x00,0x3B,0xF0,0x7F,0xF0,0xF0,0xF1,0xC0,0x70,0x00,0xE0,0x00,0xE0,0x01,0xC0,0x03,0x80,0x07,0xE0,0x1C,0xE0,0x39,0xE1,0xE1,0xFF,0x80,0xFC,0x00, // '5'
	0x0F,0xC0,0x7F,0x83,0x87,0x1C,0x0E,0x60,0x23,0x80,0x0E,0x00,0x30,0x00,0xC7,0xC3,0x7F,0xCF,0x07,0xB8,0x0E,0xE0,0x1B,0x00,0x7C,0x01,0xF8,0x07,0xE0,0x19,0xC0,0xE7,0x87,0x0F,0xFC,0x0F,0xC0, // '6'
	0xFF,0xFF,0xFF,0xC0,0x06,0x00,0x70,0x07,0x00,0x30,0x03,0x80,0x18,0x01,0xC0,0x1C,0x00,0xE0,0x06,0x00,0x70,0x03,0x00,0x38,0x01,0xC0,0x0E,0x00,0x60,0x03,0x00,0x38,0x01,0xC0,0x00, // '7'
	0x0F,0xE0,0x3F,0xE0,0xE0,0xE3,0x80,0xE7,0x01,0xCE,0x03,0x9C,0x07,0x38,0x0E,0x38,0x38,0x3F,0xE0,0x7F,0xC1,0xC1,0xC7,0x01,0xCC,0x01,0xB8,0x03,0xF0,0x07,0xE0,0x0E,0xE0,0x39,0xE0,0xF1,0xFF,0xC0,0xFE,0x00, // '8'
	0x0F,0xC0,0x3F,0xE0,0xF1,0xE3,0x81,0xC7,0x01,0xCC,0x03,0xB8,0x03,0x70,0x06,0x60,0x0C,0xE0,0x3D,0xC0,0x79,0xC3,0xF1,0xFE,0xC1,0xF1,0x80,0x03,0x00,0x0E,0x20,0x18,0xE0,0x70,0xE1,0xC1,0xFF,0x00,0xFC,0x00, // '9'
	0xFC,0x00,0x00,0x3F, // ':'
	0xFF,0x80,0x00,0x00,0x01,0xFF,0x4B,0x60, // ';'
	0x00,0x06,0x00,0x3C,0x03,0xF0,0x1F,0x01,0xF8,0x0F,0xC0,0x3C,0x00,0x60,0x00,0xF0,0x00,0xFC,0x00,0x7E,0x00,0x1F,0x00,0x0F,0xC0,0x03,0xC0,0x01,0x80, // '<'
	0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xF0, // '='
	0x80,0x01,0xC0,0x03,0xF0,0x00,0xF8,0x00,0x7E,0x00,0x1F,0x00,0x0F,0x80,0x07,0x00,0x3E,0x03,0xF0,0x1F,0x80,0xF8,0x0F,0xC0,0x1C,0x00,0x20,0x00,0x00, // '>'
	0x07,0xE0,0x3F,0xF0,0xF9,0xF3,0x80,0xE6,0x00,0xFC,0x01,0xC0,0x03,0x80,0x07,0x00,0x1C,0x00,0x70,0x03,0xC0,0x0F,0x00,0x38,0x00,0x70,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x18,0x00,0x30,0x00, // '?'
	0x00,0x3F,0xC0,0x00,0x7F,0xFC,0x00,0xF0,0x0F,0x00,0xE0,0x01,0xC0,0xE0,0x00,0x70,0xE0,0x00,0x18,0xE0,0x78,0xC6,0x60,0xFF,0x63,0x70,0xE1,0xF1,0xB0,0xE0,0x78,0xD8,0x60,0x18,0x6C,0x70,0x0C,0x3E,0x30,0x0E,0x1E,0x18,0x07,0x0F,0x0C,0x03,0x07,0x86,0x03,0x87,0xE3,0x83,0xC3,0x31,0xC3,0x63,0x98,0x7F,0x3F,0x8C,0x1F,0x0F,0x03,0x00,0x00,0x01,0xC0,0x00,0x00,0x70,0x00,0x60,0x1E,0x01,0xF0,0x03,0xFF,0xE0,0x00,0x7F,0x80,0x00, // '@'
	0x00,0xE0,0x00,0x3E,0x00,0x06,0xC0,0x00,0xD8,0x00,0x3B,0x80,0x07,0x30,0x00,0xC7,0x00,0x38,0xE0,0x07,0x0C,0x01,0xC1,0xC0,0x38,0x38,0x06,0x03,0x01,0xC0,0x70,0x3F,0xFE,0x0F,0xFF,0xE1,0xC0,0x1C,0x38,0x03,0x8E,0x00,0x39,0xC0,0x07,0x38,0x00,0x6E,0x00,0x0E, // 'A'
	0xFF,0xF0,0xFF,0xFC,0xE0,0x3E,0xE0,0x0E,0xE0,0x0E,0xE0,0x0E,0xE0,0x0E,0xE0,0x0E,0xE0,0x1C,0xFF,0xF8,0xFF,0xF8,0xE0,0x1E,0xE0,0x07,0xE0,0x07,0xE0,0x03,0xE0,0x03,0xE0,0x07,0xE0,0x07,0xE0,0x1E,0xFF,0xFC,0xFF,0xF0, // 'B'
	0x03,0xF8,0x01,0xFF,0xE0,0x7C,0x3E,0x1C,0x01,0xE7,0x00,0x1D,0xE0,0x01,0xB8,0x00,0x07,0x00,0x00,0xE0,0x00,0x18,0x00,0x03,0x00,0x00,0x60,0x00,0x0E,0x00,0x01,0xC0,0x00,0x38,0x00,0x37,0x80,0x07,0x70,0x01,0xC7,0x80,0x70,0x7C,0x7E,0x07,0xFF,0x00,0x3F,0x80, // 'C'
	0xFF,0xC0,0x3F,0xFE,0x0C,0x07,0xC3,0x00,0x38,0xC0,0x07,0x30,0x00,0xEC,0x00,0x3B,0x00,0x0E,0xC0,0x01,0xB0,0x00,0x7C,0x00,0x1F,0x00,0x06,0xC0,0x01,0xB0,0x00,0xEC,0x00,0x3B,0x00,0x1E,0xC0,0x07,0x30,0x07,0x8C,0x07,0xC3,0xFF,0xE0,0xFF,0xE0,0x00, // 'D'
	0xFF,0xFF,0x7F,0xFF,0xB8,0x00,0x1C,0x00,0x0E,0x00,0x07,0x00,0x03,0x80,0x01,0xC0,0x00,0xE0,0x00,0x7F,0xFF,0x3F,0xFF,0x9C,0x00,0x0E,0x00,0x07,0x00,0x03,0x80,0x01,0xC0,0x00,0xE0,0x00,0x70,0x00,0x38,0x00,0x1F,0xFF,0xFF,0xFF,0xF8, // 'E'
	0xFF,0xFF,0xFF,0xFF,0x80,0x07,0x00,0x0E,0x00,0x1C,0x00,0x38,0x00,0x70,0x00,0xE0,0x01,0xC0,0x03,0xFF,0xF7,0xFF,0xEE,0x00,0x1C,0x00,0x38,0x00,0x70,0x00,0xE0,0x01,0xC0,0x03,0x80,0x07,0x00,0x0E,0x00,0x00, // 'F'
	0x01,0xFC,0x00,0x7F,0xF0,0x1F,0x8F,0xC3,0xC0,0x1C,0x38,0x00,0xE7,0x00,0x0C,0x70,0x00,0x07,0x00,0x00,0x60,0x00,0x0E,0x00,0x00,0xE0,0x1F,0xF6,0x01,0xFF,0x60,0x00,0x77,0x00,0x07,0x70,0x00,0x77,0x80,0x07,0x38,0x00,0x71,0xC0,0x0F,0x1F,0x87,0xE0,0x7F,0xF8,0x01,0xFE,0x00, // 'G'
	0xC0,0x03,0xE0,0x01,0xF0,0x00,0xF8,0x00,0x7C,0x00,0x3E,0x00,0x1F,0x00,0x0F,0x80,0x07,0xC0,0x03,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x7C,0x00,0x3E,0x00,0x1F,0x00,0x0F,0x80,0x07,0xC0,0x03,0xE0,0x01,0xF0,0x00,0xF8,0x00,0x7C,0x00,0x38, // 'H'
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE, // 'I'
	0x03,0xF8,0x1F,0xC0,0x0E,0x00,0x70,0x03,0x80,0x1C,0x00,0xE0,0x07,0x00,0x38,0x01,0xC0,0x0E,0x00,0x70,0x03,0x80,0x1C,0x00,0xFC,0x07,0x70,0x3B,0x83,0x8E,0x3C,0x7F,0xC0,0xF8,0x00, // 'J'
	0xE0,0x07,0x38,0x03,0x8E,0x01,0xC3,0x80,0xE0,0xE0,0x70,0x38,0x38,0x0E,0x1C,0x03,0x8E,0x00,0xE7,0x00,0x3B,0xC0,0x0F,0xF8,0x03,0xEF,0x00,0xF1,0xE0,0x38,0x38,0x0E,0x07,0x03,0x80,0xE0,0xE0,0x1C,0x38,0x07,0x8E,0x00,0xF3,0x80,0x1C,0xE0,0x03,0x80, // 'K'
	0xC0,0x06,0x00,0x30,0x01,0x80,0x0C,0x00,0x60,0x03,0x00,0x18,0x00,0xC0,0x06,0x00,0x30,0x01,0x80,0x0C,0x00,0x60,0x03,0x00,0x18,0x00,0xC0,0x06,0x00,0x30,0x01,0xFF,0xFF,0xFF,0x80, // 'L'
	0xF0,0x00,0x7F,0xC0,0x07,0xFE,0x00,0x3F,0xF0,0x01,0xFF,0xC0,0x1F,0xF6,0x00,0xDF,0xB8,0x06,0xFD,0xC0,0x77,0xE6,0x03,0x3F,0x38,0x39,0xF8,0xC1,0x8F,0xC6,0x0C,0x7E,0x38,0xE3,0xF0,0xC6,0x1F,0x87,0x30,0xFC,0x3B,0x87,0xE0,0xD8,0x3F,0x06,0xC1,0xF8,0x1C,0x0F,0xC0,0xE0,0x7E,0x07,0x03,0x80, // 'M'
	0xE0,0x03,0xF8,0x01,0xFC,0x00,0xFF,0x00,0x7D,0xC0,0x3E,0xE0,0x1F,0x38,0x0F,0x8C,0x07,0xC7,0x03,0xE1,0xC1,0xF0,0x60,0xF8,0x38,0x7C,0x0E,0x3E,0x07,0x1F,0x01,0xCF,0x80,0x77,0xC0,0x3B,0xE0,0x0F,0xF0,0x03,0xF8,0x01,0xFC,0x00,0x78, // 'N'
	0x01,0xFC,0x00,0x3F,0xF8,0x07,0xE3,0xF0,0x78,0x03,0x83,0x80,0x0E,0x38,0x00,0x39,0xC0,0x01,0xCE,0x00,0x0E,0xE0,0x00,0x37,0x00,0x01,0xF8,0x00,0x0F,0xC0,0x00,0x76,0x00,0x03,0x38,0x00,0x39,0xC0,0x01,0xCE,0x00,0x0E,0x38,0x00,0xE1,0xE0,0x0E,0x07,0xE3,0xF0,0x0F,0xFE,0x00,0x1F,0xC0,0x00, // 'O'
	0xFF,0xF0,0xFF,0xFC,0xE0,0x1E,0xE0,0x07,0xE0,0x07,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x07,0xE0,0x07,0xE0,0x1E,0xFF,0xFC,0xFF,0xF8,0xE0,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x00,0xE0,0x00, // 'P'
	0x01,0xFC,0x00,0x3F,0xF8,0x07,0xE3,0xF0,0x78,0x03,0x83,0x80,0x0E,0x38,0x00,0x39,0xC0,0x01,0xCE,0x00,0x0E,0xE0,0x00,0x37,0x00,0x01,0xF8,0x00,0x0F,0xC0,0x00,0x76,0x00,0x03,0x38,0x00,0x39,0xC0,0x01,0xCE,0x00,0x0E,0x38,0x00,0xE1,0xE0,0x0E,0x07,0xC1,0xF0,0x0F,0xFE,0x00,0x1F,0xC0,0x00,0x1C,0x00,0x00,0xE0,0x00,0x07,0x80,0x00,0x1E,0x00,0x00,0x7E,0x00,0x01,0xF0, // 'Q'
	0xFF,0xF8,0x3F,0xFF,0x8C,0x01,0xF3,0x00,0x1C,0xC0,0x03,0xB0,0x00,0xEC,0x00,0x3B,0x00,0x0E,0xC0,0x07,0x30,0x03,0xCF,0xFF,0xE3,0xFF,0xE0,0xC0,0x70,0x30,0x0E,0x0C,0x03,0x83,0x00,0x70,0xC0,0x0E,0x30,0x03,0x8C,0x00,0x73,0x00,0x1E,0xC0,0x03,0x80, // 'R'
	0x07,0xF8,0x07,0xFF,0x83,0xC0,0xF0,0xE0,0x1E,0x70,0x03,0x9C,0x00,0x07,0x00,0x00,0xE0,0x00,0x3F,0x00,0x07,0xFC,0x00,0x7F,0xE0,0x01,0xFC,0x00,0x07,0x80,0x00,0xE0,0x00,0x1C,0x80,0x07,0x70,0x01,0x9C,0x00,0xE3,0xE0,0xF0,0x7F,0xF8,0x07,0xF8,0x00, // 'S'
	0xFF,0xFF,0xFF,0xFF,0xC0,0x70,0x00,0x38,0x00,0x1C,0x00,0x0E,0x00,0x07,0x00,0x03,0x80,0x01,0xC0,0x00,0xE0,0x00,0x70,0x00,0x38,0x00,0x1C,0x00,0x0E,0x00,0x07,0x00,0x03,0x80,0x01,0xC0,0x00,0xE0,0x00,0x70,0x00,0x38,0x00,0x1C,0x00, // 'T'
	0xE0,0x01,0xF8,0x00,0x7E,0x00,0x1F,0x80,0x07,0xE0,0x01,0xF8,0x00,0x7E,0x00,0x1F,0x80,0x07,0xE0,0x01,0xF8,0x00,0x7E,0x00,0x1F,0x80,0x07,0xE0,0x01,0xF8,0x00,0x7E,0x00,0x1D,0xC0,0x0E,0x70,0x03,0x9E,0x01,0xC3,0xE3,0xF0,0x7F,0xF0,0x07,0xF0,0x00, // 'U'
	0xE0,0x00,0xEE,0x00,0x39,0xC0,0x07,0x38,0x00,0xE3,0x80,0x38,0x70,0x07,0x06,0x00,0xC0,0xE0,0x38,0x1C,0x07,0x01,0x80,0xC0,0x38,0x38,0x07,0x07,0x00,0x71,0xC0,0x0E,0x38,0x00,0xC6,0x00,0x1D,0xC0,0x03,0xB8,0x00,0x36,0x00,0x06,0xC0,0x00,0xF8,0x00,0x0E,0x00, // 'V'
	0xE0,0x03,0x80,0x1D,0xC0,0x1E,0x00,0x67,0x00,0x78,0x03,0x9C,0x01,0xE0,0x0E,0x30,0x0F,0xC0,0x38,0xE0,0x33,0x01,0xC3,0x80,0xCC,0x07,0x0E,0x07,0x38,0x1C,0x1C,0x1C,0xE0,0xE0,0x70,0x61,0x83,0x81,0xC1,0x86,0x0E,0x03,0x8E,0x1C,0x30,0x0E,0x30,0x31,0xC0,0x38,0xC0,0xC7,0x00,0x67,0x03,0x98,0x01,0xDC,0x0E,0xE0,0x07,0x60,0x1B,0x80,0x0D,0x80,0x6C,0x00,0x3E,0x01,0xF0,0x00,0xF0,0x03,0xC0,0x01,0xC0,0x0F,0x00, // 'W'
	0x70,0x07,0xB8,0x03,0x8E,0x03,0x83,0x81,0xC1,0xC1,0xC0,0x71,0xC0,0x18,0xC0,0x0E,0xE0,0x03,0xE0,0x00,0xE0,0x00,0x70,0x00,0x7C,0x00,0x77,0x00,0x39,0x80,0x38,0xE0,0x38,0x38,0x1C,0x0C,0x1C,0x07,0x1C,0x01,0xCE,0x00,0xEE,0x00,0x38, // 'X'
	0xE0,0x03,0xB8,0x03,0x9C,0x01,0xC7,0x01,0xC3,0x80,0xE0,0xE0,0xE0,0x38,0x60,0x1C,0x70,0x07,0x70,0x03,0xB8,0x00,0xF8,0x00,0x38,0x00,0x1C,0x00,0x0E,0x00,0x07,0x00,0x03,0x80,0x01,0xC0,0x00,0xE0,0x00,0x70,0x00,0x38,0x00,0x1C,0x00, // 'Y'
	0x7F,0xFE,0x7F,0xFE,0x00,0x0E,0x00,0x1C,0x00,0x1C,0x00,0x38,0x00,0x70,0x00,0xE0,0x00,0xE0,0x01,0xC0,0x03,0x80,0x07,0x00,0x07,0x00,0x0E,0x00,0x1C,0x00,0x38,0x00,0x38,0x00,0x70,0x00,0xE0,0x00,0xFF,0xFF,0xFF,0xFF, // 'Z'
	0xFF,0xFE,0x38,0xE3,0x8E,0x38,0xE3,0x8E,0x38,0xE3,0x8E,0x38,0xE3,0x8E,0x38,0xE3,0x8E,0x38,0xE3,0x8F,0xFF, // '['
	0xC0,0xE0,0x60,0x60,0x60,0x70,0x30,0x30,0x30,0x38,0x18,0x18,0x18,0x0C,0x0C,0x0C,0x0E,0x06,0x06,0x06,0x07,0x03, // '\'
	0xFF,0xF1,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x7F,0xFF, // ']'
	0x07,0x80,0x1E,0x00,0xFC,0x03,0x30,0x1C,0xE0,0x61,0x83,0x87,0x0C,0x0C,0x70,0x39,0x80,0x6E,0x01,0xC0, // '^'
	0xFF,0xFF,0xFF,0xFF,0xC0, // '_'
	0xE1,0xC3,0x06,0x0C, // '`'
	0x0F,0xC0,0x7F,0xC1,0xE1,0xC3,0x81,0xC0,0x03,0x80,0x07,0x03,0xFE,0x1F,0xFC,0x70,0x39,0xC0,0x73,0x80,0xE7,0x03,0xCE,0x07,0x8E,0x1F,0x1F,0xEF,0x9F,0x0F, // 'a'
	0xE0,0x03,0x80,0x0E,0x00,0x38,0x00,0xE0,0x03,0x80,0x0E,0x7C,0x3B,0xFC,0xF8,0x7B,0xC0,0xEE,0x01,0xB8,0x07,0xE0,0x1F,0x80,0x7E,0x01,0xF8,0x07,0xE0,0x1F,0x80,0x6F,0x03,0xBE,0x1E,0xEF,0xF3,0x9F,0x00, // 'b'
	0x0F,0xC0,0xFF,0x0E,0x1C,0xE0,0x76,0x03,0xF0,0x03,0x80,0x1C,0x00,0xE0,0x07,0x00,0x18,0x00,0xE0,0x77,0x03,0x9C,0x38,0x7F,0x81,0xF8, // 'c'
	0x00,0x1C,0x00,0x70,0x01,0xC0,0x07,0x00,0x1C,0x00,0x70,0xF9,0xCF,0xF7,0x78,0x7D,0xC0,0xF6,0x01,0xF8,0x07,0xE0,0x1F,0x80,0x7E,0x01,0xF8,0x07,0xE0,0x1D,0x80,0x77,0x03,0xDE,0x1F,0x3F,0xDC,0x3E,0x70, // 'd'
	0x0F,0xC0,0x7F,0x83,0x87,0x1C,0x0E,0x60,0x3B,0x80,0x6E,0x01,0xFF,0xFF,0xFF,0xFF,0x80,0x0E,0x00,0x18,0x00,0x70,0x18,0xE0,0xE3,0xFF,0x03,0xF0, // 'e'
	0x0F,0x1F,0x1C,0x18,0x18,0x18,0xFF,0xFF,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18, // 'f'
	0x0F,0x9C,0xFF,0x77,0x87,0xDC,0x0F,0x60,0x1F,0x80,0x7E,0x01,0xF8,0x07,0xE0,0x1F,0x80,0x7E,0x01,0xD8,0x07,0x70,0x3D,0xE1,0xF3,0xFD,0xC7,0xE7,0x00,0x1C,0x00,0x77,0x01,0x9E,0x1E,0x3F,0xF0,0x3F,0x00, // 'g'
	0xE0,0x07,0x00,0x38,0x01,0xC0,0x0E,0x00,0x70,0x03,0x9F,0x9D,0xFE,0xF8,0x7F,0x81,0xF8,0x0F,0xC0,0x7E,0x03,0xF0,0x1F,0x80,0xFC,0x07,0xE0,0x3F,0x01,0xF8,0x0F,0xC0,0x7E,0x03,0xF0,0x1C, // 'h'
	0xFF,0x80,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0, // 'i'
	0x1C,0x71,0xC0,0x00,0x01,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x71,0xC7,0x1C,0x7F,0xBC, // 'j'
	0xE0,0x07,0x00,0x38,0x01,0xC0,0x0E,0x00,0x70,0x03,0x81,0xDC,0x1C,0xE1,0xC7,0x1C,0x38,0xE1,0xCE,0x0E,0xE0,0x7F,0x03,0xF8,0x1E,0xE0,0xE3,0x87,0x0E,0x38,0x71,0xC1,0xCE,0x07,0x70,0x1C, // 'k'
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC0, // 'l'
	0xE7,0xC3,0xE7,0x7F,0x3F,0xBE,0x3B,0x1F,0xE0,0xF0,0x7E,0x07,0x03,0xF0,0x38,0x1F,0x81,0xC0,0xFC,0x0E,0x07,0xE0,0x70,0x3F,0x03,0x81,0xF8,0x1C,0x0F,0xC0,0xE0,0x7E,0x07,0x03,0xF0,0x38,0x1F,0x81,0xC0,0xFC,0x0E,0x07, // 'm'
	0xE7,0xE7,0x7F,0xBE,0x1F,0xE0,0x7E,0x03,0xF0,0x1F,0x80,0xFC,0x07,0xE0,0x3F,0x01,0xF8,0x0F,0xC0,0x7E,0x03,0xF0,0x1F,0x80,0xFC,0x07, // 'n'
	0x0F,0xC0,0xFF,0xC7,0x87,0x9C,0x0E,0x60,0x1B,0x80,0x7E,0x01,0xF8,0x07,0xE0,0x1F,0x80,0x7E,0x01,0xD8,0x06,0x70,0x38,0xE1,0xC3,0xFF,0x03,0xF0, // 'o'
	0xE7,0xC3,0xBF,0xCF,0x87,0xBC,0x0E,0xE0,0x1B,0x80,0x7E,0x01,0xF8,0x07,0xE0,0x1F,0x80,0x7E,0x01,0xF8,0x06,0xF0,0x3B,0xE1,0xEF,0xFF,0x39,0xF0,0xE0,0x03,0x80,0x0E,0x00,0x38,0x00,0xE0,0x03,0x80,0x00, // 'p'
	0x0F,0x9C,0xFF,0x77,0x87,0xDC,0x0F,0x60,0x1F,0x80,0x7E,0x01,0xF8,0x07,0xE0,0x1F,0x80,0x7E,0x01,0xD8,0x07,0x70,0x3D,0xE1,0xF3,0xFD,0xC3,0xE7,0x00,0x1C,0x00,0x70,0x01,0xC0,0x07,0x00,0x1C,0x00,0x70, // 'q'
	0xCF,0xFF,0xFD,0xF0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0,0xE0, // 'r'
	0x1F,0xC1,0xFF,0x1C,0x1C,0xC0,0x7E,0x00,0x38,0x01,0xF0,0x07,0xF8,0x07,0xF0,0x07,0x80,0x0E,0x00,0x7E,0x03,0xF8,0x39,0xFF,0xC3,0xF8, // 's'
	0x18,0x18,0x18,0x38,0xFF,0xFF,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x38,0x1F,0x1F, // 't'
	0xE0,0x3F,0x01,0xF8,0x0F,0xC0,0x7E,0x03,0xF0,0x1F,0x80,0xFC,0x07,0xE0,0x3F,0x01,0xF8,0x0F,0xC0,0x7E,0x07,0xF8,0x7D,0xFE,0xE7,0xE7, // 'u'
	0xE0,0x0E,0xC0,0x19,0xC0,0x73,0x80,0xE3,0x01,0x87,0x07,0x0E,0x0E,0x0C,0x38,0x1C,0x70,0x18,0xC0,0x33,0x80,0x77,0x00,0x6C,0x00,0xD8,0x01,0xF0,0x01,0xC0, // 'v'
	0xE0,0x70,0x3F,0x03,0x81,0xD8,0x36,0x0C,0xC1,0xB0,0x67,0x0D,0x87,0x38,0x6C,0x38,0xC7,0x61,0x86,0x31,0x8C,0x39,0x8C,0xE1,0xCC,0x66,0x06,0xE3,0x30,0x36,0x0D,0x81,0xB0,0x7C,0x07,0x83,0xC0,0x3C,0x1E,0x01,0xC0,0x70, // 'w'
	0x70,0x38,0xC0,0xC3,0x87,0x06,0x38,0x1C,0xE0,0x3F,0x00,0x78,0x01,0xE0,0x07,0x80,0x3F,0x00,0xCC,0x07,0x38,0x38,0x60,0xE1,0xC7,0x03,0x98,0x0E, // 'x'
	0xE0,0x0E,0xC0,0x19,0xC0,0x73,0x80,0xE3,0x81,0x87,0x07,0x06,0x0C,0x0E,0x18,0x1C,0x70,0x18,0xC0,0x39,0x80,0x37,0x00,0x6C,0x00,0xF8,0x00,0xE0,0x01,0xC0,0x03,0x80,0x06,0x00,0x1C,0x00,0x70,0x07,0xC0,0x0F,0x00,0x00, // 'y'
	0x7F,0xF3,0xFF,0x80,0x1C,0x01,0xC0,0x1C,0x01,0xE0,0x0E,0x00,0xE0,0x0E,0x00,0x70,0x07,0x00,0x70,0x07,0x00,0x38,0x03,0xFF,0xFF,0xFF, // 'z'
	0x0F,0x1F,0x1C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x38,0x70,0xE0,0xF0,0x38,0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1C,0x1F,0x0F, // '{'
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // '|'
	0xE0,0xF8,0x38,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x1C,0x0E,0x07,0x0F,0x1C,0x1C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x38,0xF8,0xE0 // '}'
};
const GFXglyph Arimo_Regular_30Glyphs[] PROGMEM = {
// bitmapOffset, width, height, xAdvance, xOffset, yOffset
	  {     0,   1,   1,   9,    0,    0 }, // ' '
	  {     1,   3,  21,  10,    3,  -21 }, // '!'
	  {     9,   9,   6,  12,    1,  -21 }, // '"'
	  {    16,  17,  21,  18,    0,  -21 }, // '#'
	  {    61,  17,  24,  18,    0,  -22 }, // '$'
	  {   112,  25,  21,  28,    1,  -21 }, // '%'
	  {   178,  19,  21,  21,    1,  -21 }, // '&'
	  {   228,   2,   6,   7,    2,  -21 }, // '''
	  {   230,   8,  28,  11,    2,  -22 }, // '('
	  {   258,   8,  28,  11,    0,  -22 }, // ')'
	  {   286,  11,  11,  13,    1,  -21 }, // '*'
	  {   302,  14,  14,  19,    2,  -17 }, // '+'
	  {   327,   3,   7,   9,    3,   -3 }, // ','
	  {   330,   8,   2,  11,    1,   -9 }, // '-'
	  {   332,   2,   3,   9,    3,   -3 }, // '.'
	  {   333,   8,  22,   9,    0,  -22 }, // '/'
	  {   355,  15,  21,  18,    1,  -21 }, // '0'
	  {   395,  13,  21,  18,    2,  -21 }, // '1'
	  {   430,  13,  21,  18,    2,  -21 }, // '2'
	  {   465,  15,  21,  18,    1,  -21 }, // '3'
	  {   505,  15,  21,  18,    1,  -21 }, // '4'
	  {   545,  15,  21,  18,    1,  -21 }, // '5'
	  {   585,  14,  21,  18,    2,  -21 }, // '6'
	  {   622,  13,  21,  18,    2,  -21 }, // '7'
	  {   657,  15,  21,  18,    1,  -21 }, // '8'
	  {   697,  15,  21,  18,    1,  -21 }, // '9'
	  {   737,   2,  16,   9,    3,  -16 }, // ':'
	  {   741,   3,  20,   9,    3,  -16 }, // ';'
	  {   749,  15,  15,  19,    2,  -17 }, // '<'
	  {   778,  14,  10,  19,    2,  -15 }, // '='
	  {   796,  15,  15,  19,    2,  -17 }, // '>'
	  {   825,  15,  21,  18,    1,  -21 }, // '?'
	  {   865,  25,  26,  31,    2,  -22 }, // '@'
	  {   947,  19,  21,  20,    0,  -21 }, // 'A'
	  {   997,  16,  21,  21,    2,  -21 }, // 'B'
	  {  1039,  19,  21,  23,    2,  -21 }, // 'C'
	  {  1089,  18,  21,  23,    3,  -21 }, // 'D'
	  {  1137,  17,  21,  21,    2,  -21 }, // 'E'
	  {  1182,  15,  21,  19,    2,  -21 }, // 'F'
	  {  1222,  20,  21,  24,    1,  -21 }, // 'G'
	  {  1275,  17,  21,  23,    3,  -21 }, // 'H'
	  {  1320,   3,  21,   8,    2,  -21 }, // 'I'
	  {  1328,  13,  21,  16,    0,  -21 }, // 'J'
	  {  1363,  18,  21,  21,    2,  -21 }, // 'K'
	  {  1411,  13,  21,  18,    3,  -21 }, // 'L'
	  {  1446,  21,  21,  26,    2,  -21 }, // 'M'
	  {  1502,  17,  21,  23,    3,  -21 }, // 'N'
	  {  1547,  21,  21,  24,    1,  -21 }, // 'O'
	  {  1603,  16,  21,  21,    2,  -21 }, // 'P'
	  {  1645,  21,  27,  24,    1,  -21 }, // 'Q'
	  {  1716,  18,  21,  23,    3,  -21 }, // 'R'
	  {  1764,  18,  21,  21,    1,  -21 }, // 'S'
	  {  1812,  17,  21,  20,    1,  -21 }, // 'T'
	  {  1857,  18,  21,  23,    2,  -21 }, // 'U'
	  {  1905,  19,  21,  20,    0,  -21 }, // 'V'
	  {  1955,  30,  21,  31,    0,  -21 }, // 'W'
	  {  2034,  17,  21,  20,    1,  -21 }, // 'X'
	  {  2079,  17,  21,  20,    1,  -21 }, // 'Y'
	  {  2124,  16,  21,  19,    1,  -21 }, // 'Z'
	  {  2166,   6,  28,   9,    2,  -22 }, // '['
	  {  2187,   8,  22,   9,    0,  -22 }, // '\'
	  {  2209,   6,  28,   9,    0,  -22 }, // ']'
	  {  2230,  14,  11,  15,    0,  -21 }, // '^'
	  {  2250,  17,   2,  18,    0,    4 }, // '_'
	  {  2255,   6,   5,  11,    2,  -23 }, // '`'
	  {  2259,  15,  16,  17,    1,  -16 }, // 'a'
	  {  2289,  14,  22,  18,    2,  -22 }, // 'b'
	  {  2328,  13,  16,  16,    1,  -16 }, // 'c'
	  {  2354,  14,  22,  18,    1,  -22 }, // 'd'
	  {  2393,  14,  16,  17,    1,  -16 }, // 'e'
	  {  2421,   8,  22,   9,    0,  -22 }, // 'f'
	  {  2443,  14,  22,  18,    1,  -16 }, // 'g'
	  {  2482,  13,  22,  18,    2,  -22 }, // 'h'
	  {  2518,   3,  22,   8,    2,  -22 }, // 'i'
	  {  2527,   6,  28,   8,   -1,  -22 }, // 'j'
	  {  2548,  13,  22,  16,    2,  -22 }, // 'k'
	  {  2584,   3,  22,   8,    2,  -22 }, // 'l'
	  {  2593,  21,  16,  26,    2,  -16 }, // 'm'
	  {  2635,  13,  16,  18,    2,  -16 }, // 'n'
	  {  2661,  14,  16,  17,    1,  -16 }, // 'o'
	  {  2689,  14,  22,  18,    2,  -16 }, // 'p'
	  {  2728,  14,  22,  18,    1,  -16 }, // 'q'
	  {  2767,   8,  16,  11,    2,  -16 }, // 'r'
	  {  2783,  13,  16,  16,    1,  -16 }, // 's'
	  {  2809,   8,  20,   9,    0,  -20 }, // 't'
	  {  2829,  13,  16,  18,    2,  -16 }, // 'u'
	  {  2855,  15,  16,  16,    0,  -16 }, // 'v'
	  {  2885,  21,  16,  22,    0,  -16 }, // 'w'
	  {  2927,  14,  16,  15,    0,  -16 }, // 'x'
	  {  2955,  15,  22,  16,    0,  -16 }, // 'y'
	  {  2997,  13,  16,  16,    1,  -16 }, // 'z'
	  {  3023,   8,  28,  11,    1,  -22 }, // '{'
	  {  3051,   2,  28,   9,    3,  -22 }, // '|'
	  {  3058,   8,  28,  11,    1,  -22 } // '}'
};
const GFXfont Arimo_Regular_30 PROGMEM = {
(uint8_t  *)Arimo_Regular_30Bitmaps,(GFXglyph *)Arimo_Regular_30Glyphs,0x20, 0x7E, 36};