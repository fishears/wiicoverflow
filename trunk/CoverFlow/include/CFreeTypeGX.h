#ifndef CFREETYPEGX
#define CFREETYPEGX

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

#include <gccore.h>

typedef void CFreeTypeGX;

#ifdef __cplusplus
extern "C" {
#endif

CFreeTypeGX * CFreeTypeGX_new();

void CFreeTypeGX_LoadFont(const CFreeTypeGX *ftgx,uint8_t* fontBuffer, FT_Long bufferSize, FT_UInt pointSize, bool cacheAll);

void CFreeTypeGX_DrawText(const CFreeTypeGX *ftgx,uint16_t x, uint16_t y, wchar_t *text, GXColor color, uint16_t textStyling);

wchar_t* CFreeTypeGX_charToWideChar(const CFreeTypeGX *ftgx, char* p);
#ifdef __cplusplus
}
#endif

#endif //CFREETYPEGX
