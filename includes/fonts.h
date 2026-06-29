#ifndef FONTS_H
#define FONTS_H

#include "stdint.h"

#define PSF1_FONT_MAGIC 0x0436

/*
	For PSF1 glyph width is always = 8 bits
	and glyph height = characterSize
*/
typedef struct {
	u16_t	magic;			// Magic bytes for identification.
	u8_t	fontMode;		// PSF font mode.
	u8_t	characterSize;	// PSF character size.
}			PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

extern unsigned char font_data[];

#endif // FONTS_H
