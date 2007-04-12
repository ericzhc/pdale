#include "stdlib.h"
#include "GUI.H"

const GUI_COLOR ColorsLogoCompressed[] = {
	0xBFBFBF,0xFFFFFF,0xB5B5B5,0x000000
	,0xFF004C,0xB5002B,0x888888,0xCF0038
	,0xCFCFCF,0xC0C0C0
};
const GUI_LOGPALETTE PalLogoCompressed = {
	10,/* number of entries */
	0, /* No transparency */
	&ColorsLogoCompressed[0]
};
const unsigned char acLogoCompressed[] = {
	/* RLE: 270 Pixels @ 000,000*/ 254, 0x00, 16, 0x00,
	/* RLE: 268 Pixels @ 001,001*/ 254, 0x01, 14, 0x01,
	/* RLE: 001 Pixels @ 000,002*/ 1, 0x00,
	/* RLE: 267 Pixels @ 001,002*/ 254, 0x01, 13, 0x01,
	/* ABS: 002 Pixels @ 268,002*/ 0, 2, 0x20,
	/* ABS: 002 Pixels @ 268,073*/ 0, 2, 0x20,
	/* RLE: 267 Pixels @ 001,074*/ 254, 0x01, 13, 0x01,
	/* ABS: 003 Pixels @ 268,074*/ 0, 3, 0x20, 0x10,
	/* RLE: 267 Pixels @ 002,075*/ 254, 0x02, 13, 0x02,
	0}; /* 4702 for 20444 pixels */
	const GUI_BITMAP bmLogoCompressed = {
		269, /* XSize */
		76, /* YSize */
		135, /* BytesPerLine */
		GUI_COMPRESS_RLE4, /* BitsPerPixel */
		acLogoCompressed, /* Pointer to picture data (indices) */
		&PalLogoCompressed /* Pointer to palette */
		,GUI_DRAW_RLE4
	};