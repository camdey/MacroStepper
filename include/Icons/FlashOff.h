// 'flash-off_80', 80x80px
const unsigned char flashOff [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff,
	0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x0f, 0xc0,
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x07, 0xe0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x01,
	0xff, 0xff, 0x80, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x07, 0xff, 0xff, 0xf0, 0x3e, 0x00, 0x00,
	0x00, 0x01, 0xf0, 0x07, 0xc0, 0x03, 0xfc, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x00,
	0x3f, 0x07, 0xc0, 0x00, 0x00, 0x07, 0x83, 0x80, 0x00, 0x00, 0x0f, 0xc1, 0xe0, 0x00, 0x00, 0x0f,
	0x0f, 0x80, 0x00, 0x00, 0x01, 0xf0, 0xf0, 0x00, 0x00, 0x1e, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xf8,
	0x78, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x7f, 0xff, 0xf8, 0x3c, 0x3c, 0x00, 0x00, 0x78, 0x78, 0x00,
	0xff, 0xff, 0xfc, 0x1e, 0x1e, 0x00, 0x00, 0xf1, 0xf8, 0x01, 0xff, 0xff, 0xfe, 0x07, 0x0f, 0x00,
	0x00, 0xe1, 0xfc, 0x01, 0xc0, 0x00, 0x0e, 0x03, 0x87, 0x00, 0x01, 0xc3, 0x9e, 0x01, 0x80, 0x00,
	0x1c, 0x01, 0xc3, 0x80, 0x03, 0xc7, 0x8f, 0x03, 0x80, 0x00, 0x1c, 0x00, 0xe3, 0xc0, 0x03, 0x8f,
	0xc7, 0x83, 0x80, 0x00, 0x38, 0x00, 0x71, 0xc0, 0x07, 0x1f, 0xe3, 0xc7, 0x00, 0x00, 0x38, 0x00,
	0x78, 0xe0, 0x07, 0x1c, 0xf1, 0xe7, 0x00, 0x00, 0x70, 0x00, 0x38, 0xe0, 0x0e, 0x38, 0x78, 0xff,
	0x00, 0x00, 0x70, 0x00, 0x1c, 0x70, 0x0e, 0x38, 0x3c, 0x7e, 0x00, 0x00, 0x70, 0x00, 0x1c, 0x70,
	0x1c, 0x70, 0x1e, 0x3e, 0x00, 0x00, 0xe0, 0x00, 0x0e, 0x38, 0x1c, 0x70, 0x0f, 0x1e, 0x00, 0x00,
	0xe0, 0x00, 0x06, 0x38, 0x38, 0xe0, 0x07, 0x8f, 0x00, 0x01, 0xc0, 0x00, 0x07, 0x1c, 0x38, 0xe0,
	0x03, 0xc7, 0x80, 0x01, 0xc0, 0x00, 0x07, 0x1c, 0x31, 0xc0, 0x01, 0xe3, 0xc0, 0x03, 0x80, 0x00,
	0x03, 0x8c, 0x71, 0xc0, 0x00, 0xf1, 0xe0, 0x03, 0x80, 0x00, 0x03, 0x8e, 0x71, 0x80, 0x00, 0x78,
	0xf0, 0x07, 0x00, 0x00, 0x01, 0x8e, 0x73, 0x80, 0x00, 0x7c, 0x78, 0x07, 0x00, 0x00, 0x01, 0xce,
	0x63, 0x80, 0x00, 0x7e, 0x3c, 0x0e, 0x00, 0x00, 0x01, 0xc6, 0xe3, 0x80, 0x00, 0xff, 0x1e, 0x0f,
	0xff, 0x80, 0x01, 0xc7, 0xe3, 0x80, 0x00, 0xe7, 0x8f, 0x0f, 0xff, 0xc0, 0x01, 0xc7, 0xe3, 0x00,
	0x00, 0xe3, 0xc7, 0x87, 0xff, 0xe0, 0x00, 0xc7, 0xe3, 0x00, 0x01, 0xc1, 0xe3, 0xc0, 0x03, 0xc0,
	0x00, 0xc7, 0xe7, 0x00, 0x01, 0xc0, 0xf1, 0xe0, 0x07, 0x80, 0x00, 0xe7, 0xe7, 0x00, 0x01, 0x80,
	0x78, 0xf0, 0x0f, 0x00, 0x00, 0xe7, 0xe7, 0x00, 0x03, 0x80, 0x3c, 0x78, 0x1e, 0x00, 0x00, 0xe7,
	0xe7, 0x00, 0x03, 0x80, 0x1e, 0x3c, 0x3c, 0x00, 0x00, 0xe7, 0xe7, 0x00, 0x07, 0xff, 0x0f, 0x1e,
	0x78, 0x00, 0x00, 0xe7, 0xe7, 0x00, 0x07, 0xff, 0x87, 0x8f, 0xf0, 0x00, 0x00, 0xe7, 0xe3, 0x00,
	0x03, 0xff, 0x83, 0xc7, 0xe0, 0x00, 0x00, 0xc7, 0xe3, 0x00, 0x00, 0x03, 0x81, 0xe3, 0xc0, 0x00,
	0x00, 0xc7, 0xe3, 0x80, 0x00, 0x03, 0x80, 0xf1, 0xe0, 0x00, 0x01, 0xc7, 0xe3, 0x80, 0x00, 0x03,
	0x80, 0x78, 0xf0, 0x00, 0x01, 0xc7, 0x63, 0x80, 0x00, 0x03, 0x80, 0x3c, 0x78, 0x00, 0x01, 0xc6,
	0x73, 0x80, 0x00, 0x03, 0x00, 0x3e, 0x3c, 0x00, 0x01, 0xce, 0x71, 0x80, 0x00, 0x07, 0x00, 0x7f,
	0x1e, 0x00, 0x01, 0x8e, 0x71, 0xc0, 0x00, 0x07, 0x00, 0xf7, 0x8f, 0x00, 0x03, 0x8e, 0x31, 0xc0,
	0x00, 0x07, 0x01, 0xe3, 0xc7, 0x80, 0x03, 0x9c, 0x38, 0xe0, 0x00, 0x06, 0x03, 0xc1, 0xe3, 0xc0,
	0x07, 0x1c, 0x38, 0xe0, 0x00, 0x06, 0x07, 0x80, 0xf1, 0xe0, 0x07, 0x1c, 0x1c, 0x60, 0x00, 0x0e,
	0x0f, 0x00, 0x78, 0xf0, 0x0e, 0x38, 0x1c, 0x70, 0x00, 0x0e, 0x1c, 0x00, 0x3c, 0x78, 0x0e, 0x38,
	0x0e, 0x38, 0x00, 0x0e, 0x38, 0x00, 0x1e, 0x3c, 0x1c, 0x70, 0x0e, 0x38, 0x00, 0x0c, 0x70, 0x00,
	0x0f, 0x1e, 0x1c, 0x70, 0x07, 0x1c, 0x00, 0x0c, 0xe0, 0x00, 0x07, 0x8f, 0x38, 0xe0, 0x07, 0x1e,
	0x00, 0x1f, 0xc0, 0x00, 0x03, 0xc7, 0xf8, 0xe0, 0x03, 0x8e, 0x00, 0x1f, 0x80, 0x00, 0x01, 0xe3,
	0xf1, 0xc0, 0x03, 0xc7, 0x00, 0x1f, 0x00, 0x00, 0x00, 0xf1, 0xe3, 0xc0, 0x01, 0xc3, 0x80, 0x1e,
	0x00, 0x00, 0x00, 0x79, 0xc3, 0x80, 0x00, 0xe3, 0xc0, 0x0c, 0x00, 0x00, 0x00, 0x3f, 0x87, 0x00,
	0x00, 0xf1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x8f, 0x00, 0x00, 0x78, 0xf8, 0x00, 0x00, 0x00,
	0x00, 0x1e, 0x1e, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x1e,
	0x1f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x78, 0x00, 0x00, 0x0f, 0x0f, 0x80, 0x00, 0x00, 0x03, 0xf0,
	0xf0, 0x00, 0x00, 0x07, 0x83, 0xf0, 0x00, 0x00, 0x0f, 0xc1, 0xe0, 0x00, 0x00, 0x03, 0xe0, 0xfc,
	0x00, 0x00, 0x3f, 0x07, 0xc0, 0x00, 0x00, 0x01, 0xf0, 0x3f, 0xc0, 0x07, 0xfc, 0x0f, 0x80, 0x00,
	0x00, 0x00, 0x7c, 0x0f, 0xff, 0xff, 0xf0, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x01, 0xff, 0xff,
	0x80, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x07, 0xe0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00,
	0x03, 0xf0, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
	0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00
};
