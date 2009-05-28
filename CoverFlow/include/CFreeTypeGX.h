#ifndef CFREETYPEGX
#define CFREETYPEGX

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include <gccore.h>

#define FTGX_NULL				0x0000
#define FTGX_JUSTIFY_LEFT		0x0001
#define FTGX_JUSTIFY_CENTER		0x0002
#define FTGX_JUSTIFY_RIGHT		0x0004
#define FTGX_ALIGN_TOP			0x0010
#define FTGX_ALIGN_MIDDLE		0x0020
#define FTGX_ALIGN_BOTTOM		0x0040

typedef void CFreeTypeGX;

#ifdef __cplusplus
extern "C" {
#endif

CFreeTypeGX * CFreeTypeGX_new();

void CFreeTypeGX_LoadFont(const CFreeTypeGX *ftgx,uint8_t* fontBuffer, FT_Long bufferSize, FT_UInt pointSize, bool cacheAll);

void CFreeTypeGX_DrawText(const CFreeTypeGX *ftgx,uint16_t x, uint16_t y, wchar_t *text, GXColor color, uint16_t textStyling);

void CFreeTypeGX_DrawTextWithShadow(const CFreeTypeGX *ftgx, uint16_t x, uint16_t y, wchar_t *text, GXColor color, GXColor shadowColor, uint16_t textStyling);

wchar_t* CFreeTypeGX_charToWideChar(const CFreeTypeGX *ftgx, char* p);

uint16_t CFreeTypeGX_getWidth(const CFreeTypeGX *ftgx, wchar_t *text);

uint16_t CFreeTypeGX_getHeight(const CFreeTypeGX *ftgx, wchar_t *text);

#ifdef __cplusplus
}
#endif

#endif //CFREETYPEGX
