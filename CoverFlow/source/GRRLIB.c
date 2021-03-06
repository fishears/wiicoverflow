/*===========================================
        GRRLIB 4.0.0
        Code     : NoNameNo
        Additional Code : Crayon
        GX hints : RedShade

Download and Help Forum : http://grrlib.santo.fr
===========================================*/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include "pngu/pngu.h"
#include "GRRLIB.h"
#include <fat.h>
#include "settings.h"
#include "core/video.h"
#include "TrackedMemoryManager.h"

#define DEFAULT_FIFO_SIZE (256 * 1024)

extern const u8 matte_black_png[];
extern const u8 matte_grey_png[];
extern const u8 case_right_png[];
extern const u8 case_3d_shadow_png[];
GRRLIB_texImg   matteBlackTexture;
GRRLIB_texImg   matteGreyTexture;
GRRLIB_texImg   rightTexture;
GRRLIB_texImg   case3DShadowTexture;
GXTexObj        matteBlackTex;
GXTexObj        matteGreyTex;
GXTexObj        rightSideTex;
GXTexObj        case3DShadowTex;

extern s_settings settings;

u32 fb = 0;
static void *xfb[2] = { NULL, NULL};
GXRModeObj *rmode;
void *gp_fifo = NULL;

Mtx view;

extern bool fatInitDefault (void);

/**
 * Clear screen with a specific color.
 * @param color the color to use to fill the screen.
 */
inline void GRRLIB_FillScreen(u32 color) {
    GRRLIB_Rectangle(-40, -40, 680, 520, color, 1);
}

/**
 * Draw a dot.
 * @param x specifies the x-coordinate of the dot.
 * @param y specifies the y-coordinate of the dot.
 * @param color the color of the dot.
 */
inline void GRRLIB_Plot(f32 x, f32 y, u32 color) {
    guVector v[] = {{x,y,0.0f}};

    GRRLIB_NPlot(v, color, 1);
}

/**
 *
 * @param v
 * @param color
 * @param n
 */
void GRRLIB_NPlot(guVector v[], u32 color, long n) {
    GRRLIB_GXEngine(v, color, n, GX_POINTS);
}

/**
 * Draw a line.
 * @param x1 start point for line for the x coordinate.
 * @param y1 start point for line for the y coordinate.
 * @param x2 end point for line for the x coordinate.
 * @param y2 end point for line for the x coordinate.
 * @param color line color.
 */
inline void GRRLIB_Line(f32 x1, f32 y1, f32 x2, f32 y2, u32 color) {
    guVector v[] = {{x1,y1,0.0f}, {x2,y2,0.0f}};

    GRRLIB_NGone(v, color, 2);
}

/**
 * Draw a rectangle.
 * @param x specifies the x-coordinate of the upper-left corner of the rectangle.
 * @param y specifies the y-coordinate of the upper-left corner of the rectangle.
 * @param width the width of the rectangle.
 * @param height the height of the rectangle.
 * @param color the color of the rectangle.
 * @param filled true to fill the rectangle with a color.
 */
inline void GRRLIB_Rectangle(f32 x, f32 y, f32 width, f32 height, u32 color, u8 filled) {
    f32 x2 = x+width;
    f32 y2 = y+height;
    guVector v[] = {{x,y,0.0f}, {x2,y,0.0f}, {x2,y2,0.0f}, {x,y2,0.0f}, {x,y,0.0f}};

    if(!filled) {
        GRRLIB_NGone(v, color, 5);
    }
    else{
        GRRLIB_NGoneFilled(v, color, 4);
    }
}

/**
 *
 * @param v
 * @param color
 * @param n
 */
void GRRLIB_NGone(guVector v[], u32 color, long n) {
    GRRLIB_GXEngine(v, color, n, GX_LINESTRIP);
}

/**
 *
 * @param v
 * @param color
 * @param n
 */
void GRRLIB_NGoneFilled(guVector v[], u32 color, long n) {
    GRRLIB_GXEngine(v, color, n, GX_TRIANGLEFAN);
}

/**
 *
 * @param tex
 * @param tilew
 * @param tileh
 * @param tilestart
 */
void GRRLIB_InitTileSet(struct GRRLIB_texImg *tex, unsigned int tilew, unsigned int tileh, unsigned int tilestart) {
    tex->tilew = tilew;
    tex->tileh = tileh;
    tex->nbtilew = tex->w / tilew;
    tex->nbtileh = tex->h / tileh;
    tex->tilestart = tilestart;
}

/**
 * Load a texture from a buffer.
 * @param my_png the PNG buffer to load.
 * @return A GRRLIB_texImg structure filled with PNG informations.
 */
void GRRLIB_LoadTexturePNG(GRRLIB_texImg * my_texture, const unsigned char my_png[]) {
    PNGUPROP imgProp;
    IMGCTX ctx;
	void * textureAddress;

    ctx = PNGU_SelectImageFromBuffer(my_png);
    PNGU_GetImageProperties (ctx, &imgProp);
	textureAddress= CFMemAlign (32, imgProp.imgWidth * imgProp.imgHeight * 4);
	if (textureAddress>0)
	{
		my_texture->data = textureAddress;
		PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, my_texture->data, 255);
		PNGU_ReleaseImageContext (ctx);
		my_texture->w = imgProp.imgWidth;
		my_texture->h = imgProp.imgHeight;
		GRRLIB_FlushTex(*my_texture);
	}
	else // out of memory
	{
		// it would be nice to return a no memory image, but there is no memory
		// and there'd be issues when free was called (i.e. it'd crash)
		// at least no someone will notice a missing image (if it doesn't crash -
		// won't find out till memory runs out).  Still better than nothing
		my_texture->data=NULL;
		my_texture->w=0;
		my_texture->h=0;
	}
}

/**
 * Load a texture from a buffer into fixed memory location.
 * @param my_png the PNG buffer to load.
 * @param textureAddress the destination address.
 * @return A GRRLIB_texImg structure filled with PNG informations.
 */
void GRRLIB_LoadTexturePNGToMemory(GRRLIB_texImg * my_texture, const unsigned char my_png[], void * textureAddress) {
    PNGUPROP imgProp;
    IMGCTX ctx;

    ctx = PNGU_SelectImageFromBuffer(my_png);
    PNGU_GetImageProperties (ctx, &imgProp);
    my_texture->data = textureAddress;
    PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, my_texture->data, 255);
    PNGU_ReleaseImageContext (ctx);
    my_texture->w = imgProp.imgWidth;
    my_texture->h = imgProp.imgHeight;
    GRRLIB_FlushTex(*my_texture);
}

/**
 * Load a texture from a buffer into fixed memory location.
 * @param my_png the PNG buffer to load.
 * @param textureAddress the destination address.
 * @return A GRRLIB_texImg structure filled with PNG informations.
 */
void GRRLIB_LoadTexturePNGToMemorySized(GRRLIB_texImg * my_texture, const unsigned char my_png[], void * textureAddress,int Size) {
    PNGUPROP imgProp;
    IMGCTX ctx;

    ctx = PNGU_SelectImageFromBuffer(my_png);
    PNGU_GetImageProperties (ctx, &imgProp);
    my_texture->data = 0;
	if (Size != imgProp.imgWidth * imgProp.imgHeight * 4) return;
    my_texture->data = textureAddress;
    PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, my_texture->data, 255);
    PNGU_ReleaseImageContext (ctx);
    my_texture->w = imgProp.imgWidth;
    my_texture->h = imgProp.imgHeight;
    GRRLIB_FlushTex(*my_texture);
}

/**
 * Convert a raw bmp (RGB, no alpha) to 4x4RGBA.
 * @author DrTwox
 * @param src
 * @param dst
 * @param width
 * @param height
*/
void GRRLIB_RawTo4x4RGBA(const unsigned char *src, void *dst, const unsigned int width, const unsigned int height) {
    unsigned int block;
    unsigned int i;
    unsigned int c;
    unsigned int ar;
    unsigned int gb;
    unsigned char *p = (unsigned char*)dst;

    for (block = 0; block < height; block += 4) {
        for (i = 0; i < width; i += 4) {
            /* Alpha and Red */
            for (c = 0; c < 4; ++c) {
                for (ar = 0; ar < 4; ++ar) {
                    /* Alpha pixels */
                    *p++ = 255;
                    /* Red pixels */
                    *p++ = src[((i + ar) + ((block + c) * width)) * 3];
                }
            }

            /* Green and Blue */
            for (c = 0; c < 4; ++c) {
                for (gb = 0; gb < 4; ++gb) {
                    /* Green pixels */
                    *p++ = src[(((i + gb) + ((block + c) * width)) * 3) + 1];
                    /* Blue pixels */
                    *p++ = src[(((i + gb) + ((block + c) * width)) * 3) + 2];
                }
            }
        } /* i */
    } /* block */
}

/**
 * Print formatted output.
 * @param xpos
 * @param ypos
 * @param tex
 * @param color
 * @param zoom
 * @param text
 * @param ... Optional arguments.
 */
void GRRLIB_PrintBMF(f32 xpos, f32 ypos, GRRLIB_bytemapFont bmf, f32 zoom, const char *text, ...) {
    unsigned int i, j, x, y, n, size;
    char tmp[1024];

    va_list argp;
    va_start(argp, text);
    size = vsprintf(tmp, text, argp);
    va_end(argp);

	GRRLIB_texImg tex_BMfont;
    GRRLIB_CreateEmptyTexture(&tex_BMfont,800, 600);


    for(i=0; i<size; i++) {
        for(j=0; j<bmf.nbChar; j++) {
            if(tmp[i] == bmf.charDef[j].character) {
                n=0;
                for(y=0; y<bmf.charDef[j].height; y++) {
                    for(x=0; x<bmf.charDef[j].width; x++) {
                        if(bmf.charDef[j].data[n]) {
                            GRRLIB_SetPixelTotexImg(xpos + x + bmf.charDef[j].relx, ypos + y + bmf.charDef[j].rely,
                                tex_BMfont, bmf.palette[bmf.charDef[j].data[n]]);
                            //GRRLIB_Plot(xpos + x + bmf.charDef[j].relx, ypos + y + bmf.charDef[j].rely,
                            //    bmf.palette[bmf.charDef[j].data[n]]);
                        }
                        n++;
                    }
                }
                xpos += bmf.charDef[j].shift + bmf.addSpace;
                break;
            }
        }
    }
    GRRLIB_FlushTex(tex_BMfont);

    GRRLIB_DrawImg(0, 0, tex_BMfont, 0, 1, 1, 0xFFFFFFFF);

    CFFree(tex_BMfont.data);
}

/**
 * Load a ByteMap font structure from a buffer.
 * @param my_bmf the ByteMap font buffer to load.
 * @return A GRRLIB_bytemapFont structure filled with BMF informations.
 */
GRRLIB_bytemapFont GRRLIB_LoadBMF(const unsigned char my_bmf[]) {
    GRRLIB_bytemapFont fontArray;
    int i, j = 1;
    u8 lineheight, usedcolors, highestcolor, nbPalette;
    short int sizeover, sizeunder, sizeinner, numcolpal;
    u16 nbPixels;

    // Initialize everything to zero
    memset(&fontArray, 0, sizeof(fontArray));

    if(my_bmf[0]==0xE1 && my_bmf[1]==0xE6 && my_bmf[2]==0xD5 && my_bmf[3]==0x1A) {
        fontArray.version = my_bmf[4];
        lineheight = my_bmf[5];
        sizeover = my_bmf[6];
        sizeunder = my_bmf[7];
        fontArray.addSpace = my_bmf[8];
        sizeinner = my_bmf[9];
        usedcolors = my_bmf[10];
        highestcolor = my_bmf[11];
        nbPalette = my_bmf[16];
        numcolpal = 3 * nbPalette;
        fontArray.palette = (u32 *)CFCalloc(nbPalette + 1, sizeof(u32));
        for(i=0; i < numcolpal; i+=3) {
            fontArray.palette[j++] = ((((my_bmf[i+17]<<2)+3)<<24) | (((my_bmf[i+18]<<2)+3)<<16) | (((my_bmf[i+19]<<2)+3)<<8) | 0xFF);
        }
        j = my_bmf[17 + numcolpal];
        fontArray.name = (char *)CFCalloc(j + 1, sizeof(char));
        memcpy(fontArray.name, &my_bmf[18 + numcolpal], j);
        j = 18 + numcolpal + j;
        fontArray.nbChar = (my_bmf[j] | my_bmf[j+1]<<8);
        fontArray.charDef = (GRRLIB_bytemapChar *)CFCalloc(fontArray.nbChar, sizeof(GRRLIB_bytemapChar));
        j++;
        for(i=0; i < fontArray.nbChar; i++) {
            fontArray.charDef[i].character = my_bmf[++j];
            fontArray.charDef[i].width = my_bmf[++j];
            fontArray.charDef[i].height = my_bmf[++j];
            fontArray.charDef[i].relx = my_bmf[++j];
            fontArray.charDef[i].rely = my_bmf[++j];
            fontArray.charDef[i].shift = my_bmf[++j];
            nbPixels = fontArray.charDef[i].width * fontArray.charDef[i].height;
            fontArray.charDef[i].data = CFMalloc(nbPixels);
            if(nbPixels && fontArray.charDef[i].data) {
                memcpy(fontArray.charDef[i].data, &my_bmf[++j], nbPixels);
                j += (nbPixels - 1);
            }
        }
    }
    return fontArray;
}

/**
 * Free memory.
 * @param bmf a GRRLIB_bytemapFont structure.
 */
void GRRLIB_FreeBMF(GRRLIB_bytemapFont bmf)
{
    unsigned int i;

    for(i=0; i<bmf.nbChar; i++) {
        CFFree(bmf.charDef[i].data);
    }
    CFFree(bmf.charDef);
    CFFree(bmf.palette);
    CFFree(bmf.name);
}

/**
 * Load a texture from a buffer.
 * @param my_img the JPEG or PNG buffer to load.
 * @return A GRRLIB_texImg structure filled with imgage informations.
 */
void GRRLIB_LoadTexture(GRRLIB_texImg * my_texture, const unsigned char my_img[]) {

    if(my_img[0]==0xFF && my_img[1]==0xD8 && my_img[2]==0xFF) {
	    /*Invalid PNG... return empty texture*/
        GRRLIB_CreateEmptyTexture(my_texture,12,12);
    }
    else {
        GRRLIB_LoadTexturePNG(my_texture,my_img);
    }
}

/**
 * Create an empty texture.
 * @param w width of the new texture to create.
 * @param h height of the new texture to create.
 * @return A GRRLIB_texImg structure newly created.
 */
void GRRLIB_CreateEmptyTexture(GRRLIB_texImg * my_texture,unsigned int w, unsigned int h) {
    unsigned int x, y;


    my_texture->data = CFMemAlign (32, h * w * 4);
    my_texture->w = w;
    my_texture->h = h;
    // Initialize the texture
    for(y=0; y<h; y++) {
        for(x=0; x<w; x++) {
            GRRLIB_SetPixelTotexImg(x, y, *my_texture, 0x00000000);
        }
    }
    GRRLIB_FlushTex(*my_texture);
}

/**
 * Create an empty texture.
 * @param w width of the new texture to create.
 * @param h height of the new texture to create.
 * @return A GRRLIB_texImg structure newly created.
 */
void GRRLIB_DuplicateTexture(GRRLIB_texImg * destination_texture, GRRLIB_texImg tex, unsigned int w, unsigned int h) {
    unsigned int x, y;

    destination_texture->data = CFMemAlign (32, h * w * 4);
    destination_texture->w = w;
    destination_texture->h = h;
    // Initialize the texture
    for(y=0; y<h; y++) {
        for(x=0; x<w; x++) {
            GRRLIB_SetPixelTotexImg(x, y, *destination_texture, GRRLIB_GetPixelFromtexImg(x, y, tex));
        }
    }
    GRRLIB_FlushTex(*destination_texture);
}

/**
 * Draw a texture.
 * @param xpos specifies the x-coordinate of the upper-left corner.
 * @param ypos specifies the y-coordinate of the upper-left corner.
 * @param tex texture to draw.
 * @param degrees angle of rotation.
 * @param scaleX
 * @param scaleY
 * @param color
 */
inline void GRRLIB_DrawImg(f32 xpos, f32 ypos, GRRLIB_texImg tex, float degrees, float scaleX, f32 scaleY, u32 color ) {
    GXTexObj texObj;
    u16 width, height;
    Mtx m, m1, m2, mv;

	if (tex.data==0) return;

    GX_InitTexObj(&texObj, tex.data, tex.w, tex.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    //GX_InitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    GX_LoadTexObj(&texObj, GX_TEXMAP0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

    width = tex.w * 0.5;
    height = tex.h * 0.5;
    guMtxIdentity (m1);
    guMtxScaleApply(m1, m1, scaleX, scaleY, 1.0);
    guVector axis = (guVector) {0, 0, 1 };
    guMtxRotAxisDeg (m2, &axis, degrees);
    guMtxConcat(m2, m1, m);

    guMtxTransApply(m, m, xpos+width, ypos+height, 0);
    guMtxConcat (GXmodelView2D, m, mv);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(-width, -height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(0, 0);

    GX_Position3f32(width, -height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(1, 0);

    GX_Position3f32(width, height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(1, 1);

    GX_Position3f32(-width, height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(0, 1);
    GX_End();
    GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);
}

inline void GRRLIB_DrawImgReflection(f32 xpos, f32 ypos, GRRLIB_texImg tex, float degrees, float scaleX, f32 scaleY, float dist)
{
    GXTexObj texObj;
    u16 width, height;
    Mtx m, m1, m2, mv;

	if (tex.data==0) return;

    GX_InitTexObj(&texObj, tex.data, tex.w, tex.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_InitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    GX_LoadTexObj(&texObj, GX_TEXMAP0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

    width = tex.w * 0.5;
    height = tex.h * 0.5 * dist;
    guMtxIdentity (m1);
    guMtxScaleApply(m1, m1, scaleX, scaleY, 1.0);
    guVector axis = (guVector) {0, 0, 1 };
    guMtxRotAxisDeg (m2, &axis, degrees);
    guMtxConcat(m2, m1, m);

    guMtxTransApply(m, m, xpos+width, ypos+height, 0);
    guMtxConcat (GXmodelView2D, m, mv);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(-width, -height, 0);
    GX_Color1u32(0xffffff55);
    GX_TexCoord2f32(0, 1);

    GX_Position3f32(width, -height, 0);
    GX_Color1u32(0xffffff55);
    GX_TexCoord2f32(1, 1);

    GX_Position3f32(width, height, 0);
    GX_Color1u32(0xffffff00);
    GX_TexCoord2f32(1, 0);

    GX_Position3f32(-width, height, 0);
    GX_Color1u32(0xffffff00);
    GX_TexCoord2f32(0, 0);
    GX_End();
    GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);
}

inline void GRRLIB_DrawFlatCoverImg(f32 xpos, f32 ypos, GRRLIB_texImg tex, float degrees, float scaleX, f32 scaleY, u32 color ) {
    GXTexObj texObj;
    u16 width, height;
    Mtx m, m1, m2, mv;

	if (tex.data==0) return;

	float right = .527;

	if(tex.w == 160 && tex.h == 224)
		right = 0;

    GX_InitTexObj(&texObj, tex.data, tex.w, tex.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    //GX_InitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    GX_LoadTexObj(&texObj, GX_TEXMAP0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

    width = 160 * 0.5;
    height = 224 * 0.5;

    guMtxIdentity (m1);
    guMtxScaleApply(m1, m1, scaleX, scaleY, 1.0);
    guVector axis = (guVector) {0, 0, 1 };
    guMtxRotAxisDeg (m2, &axis, degrees);
    guMtxConcat(m2, m1, m);

    guMtxTransApply(m, m, xpos+width, ypos+height, 0);
    guMtxConcat (GXmodelView2D, m, mv);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(-width, -height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(right, 0);

    GX_Position3f32(width, -height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(1, 0);

    GX_Position3f32(width, height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(1, 1);

    GX_Position3f32(-width, height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(right, 1);
    GX_End();
    GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);
}

/**
 * Draw a texture.
 * @param xpos specifies the x-coordinate of the upper-left corner.
 * @param ypos specifies the y-coordinate of the upper-left corner.
 * @param tex texture to draw.
 * @param degrees angle of rotation.
 * @param scaleX
 * @param scaleY
 * @param color
 */
inline void GRRLIB_DrawCoverImg(f32 loc, f32 zpos, GRRLIB_texImg tex, float degrees, float scale, u32 color, float falloff, int theme)
{

	if (tex.data==0) return;

	GRRLIB_3D_Init();

    GXTexObj texObj;
    u16 width, height;

    Mtx m, mv;

    GX_InitTexObj(&texObj, tex.data, tex.w, tex.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);

	bool use3d = settings.covers3d;

	if(use3d && (tex.w == 160 && tex.h == 224))
	{
		use3d = false;
		//force to 2d cover since 3d wasn't found, but 2d was
	}

	if(!(use3d))
	{
		width = tex.w * 0.01 * scale;
		height = tex.h * 0.01 * scale;
	}
	else
	{
		width = 160 * 0.01 * scale;
		height = 224 * 0.01 * scale;
	}

    guMtxIdentity (m);
    guMtxScaleApply(m, m, 1.0, 1.0, 10.0);

	guMtxTransApply(m, m, 0, 0, -1*zpos);

    guVector axis = (guVector) {0, 1, 0 };
    guMtxRotAxisDeg (m, &axis, degrees);

	if(scale > 1)
		guMtxTransApply(m, m, loc, 0, 6.0f+falloff + -1*zpos);
	else
		guMtxTransApply(m, m, loc, 0, 8.0f+falloff + -1*zpos);

    guMtxConcat (view, m, mv);

	GX_LoadTexObj(&texObj, GX_TEXMAP0);
    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);

	float thickness = 0.15;
	if (settings.covers3dThickness) // true = flat 3d covers
		thickness = 0.0001;
	float left = .470;
	float right = .527;

	if(use3d)
	{
		// Draw Back cover
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(-width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 0);

		GX_Position3f32(width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 1);
		GX_End();

		 //Draw right side
		GX_LoadTexObj(&rightSideTex, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(-width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(-width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(-width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 0);
		GX_End();

		// Draw Left side
		GX_LoadTexObj(&texObj, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 1);

		GX_Position3f32(width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 0);

		GX_Position3f32(width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 0);

		GX_Position3f32(width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 1);
		GX_End();

		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		// Draw the Front Cover
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 1);

		GX_Position3f32(width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 0);

		GX_Position3f32(-width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 0);
		GX_End();

		// Draw the 3D shadow mask
		// Load the texture
		GX_LoadTexObj(&case3DShadowTex, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);
		//Draw over the back cover
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(-width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 0);

		GX_Position3f32(width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 1);
		GX_End();

		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		//Draw over the left spine
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(width, -height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 1);

		GX_Position3f32(width, height, thickness);
		GX_Color1u32(color);
		GX_TexCoord2f32(left, 0);

		GX_Position3f32(width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 0);

		GX_Position3f32(width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 1);
		GX_End();

		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		//Draw over the front cover
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 1);

		GX_Position3f32(width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(right, 0);

		GX_Position3f32(-width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 0);
		GX_End();
	}
	else
	{
		// Draw the Front Cover in 2D
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(width, -height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(-width, height, 0);
		GX_Color1u32(color);
		GX_TexCoord2f32(1, 0);
		GX_End();

		if(settings.covers3d)
		{ //Only do this when in 3d, but no 3d cover

			// Draw Back cover
			GX_LoadPosMtxImm (mv, GX_PNMTX0);

			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-width, -height, .001);
			GX_Color1u32(color);
			GX_TexCoord2f32(0, 1);

			GX_Position3f32(-width, height, .001);
			GX_Color1u32(color);
			GX_TexCoord2f32(0, 0);

			GX_Position3f32(width, height, .001);
			GX_Color1u32(color);
			GX_TexCoord2f32(1, 0);

			GX_Position3f32(width, -height, .001);
			GX_Color1u32(color);
			GX_TexCoord2f32(1, 1);
			GX_End();
		}
	}

	float dist = 2.02;

	//Reflections
	if(use3d)
	{
		dist = 2.02;
		// Draw Back cover reflection matte
		if(theme) // white
			GX_LoadTexObj(&matteGreyTex, GX_TEXMAP0);
		else
			GX_LoadTexObj(&matteBlackTex, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(-width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(left, 1);

		GX_Position3f32(width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(left, 0);
		GX_End();

		// Draw Back cover reflection
		GX_LoadTexObj(&texObj, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(-width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF60);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF60);
		GX_TexCoord2f32(left, 1);

		GX_Position3f32(width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(left, 0);
		GX_End();

		// Draw Right side reflection matte
		if(theme) // white
			GX_LoadTexObj(&matteGreyTex, GX_TEXMAP0);
		else
			GX_LoadTexObj(&matteBlackTex, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(1, 0);

		GX_Position3f32(-width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(-width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(-width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(0, 0);
		GX_End();

		// Draw Right side reflection
		GX_LoadTexObj(&rightSideTex, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(1, 0);

		GX_Position3f32(-width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFF70);
		GX_TexCoord2f32(1, 1);

		GX_Position3f32(-width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF70);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(-width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(0, 0);
		GX_End();

		// Draw Left side reflection matte
		if(theme) // white
			GX_LoadTexObj(&matteGreyTex, GX_TEXMAP0);
		else
			GX_LoadTexObj(&matteBlackTex, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(left, 0);

		GX_Position3f32(width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(left, 1);

		GX_Position3f32(width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(right, 1);

		GX_Position3f32(width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFFFF);
		GX_TexCoord2f32(right, 0);
		GX_End();

		// Draw Left side reflection
		GX_LoadTexObj(&texObj, GX_TEXMAP0);
		GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
		GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
		GX_LoadPosMtxImm (mv, GX_PNMTX0);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(width, -height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(left, 0);

		GX_Position3f32(width, height-height*dist, thickness);
		GX_Color1u32(0xFFFFFF70);
		GX_TexCoord2f32(left, 1);

		GX_Position3f32(width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFF70);
		GX_TexCoord2f32(right, 1);

		GX_Position3f32(width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(right, 0);
		GX_End();
	}
	else
	{

		if(settings.covers3d)
		{ //Only do this when in 3d, but no 3d cover

			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-width, -height-height*dist, .001);
			GX_Color1u32(0xFFFFFFFF);
			GX_TexCoord2f32(0, 0);

			GX_Position3f32(-width, height-height*dist, .001);
			GX_Color1u32(0xFFFFFFFF);
			GX_TexCoord2f32(0, 1);

			GX_Position3f32(width, height-height*dist, .001);
			GX_Color1u32(0xFFFFFFFF);
			GX_TexCoord2f32(1, 1);

			GX_Position3f32(width, -height-height*dist, .001);
			GX_Color1u32(0xFFFFFFFF);
			GX_TexCoord2f32(1, 0);
			GX_End();
		}
	}

	// Draw Reflection Matte
	if(theme) // white
		GX_LoadTexObj(&matteGreyTex, GX_TEXMAP0);
	else
		GX_LoadTexObj(&matteBlackTex, GX_TEXMAP0);
    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(-width, -height-height*dist, 0);
    GX_Color1u32(0xFFFFFFFF);
    GX_TexCoord2f32(1, 0);

    GX_Position3f32(width, -height-height*dist, 0);
    GX_Color1u32(0xFFFFFFFF);
    GX_TexCoord2f32(0, 0);

    GX_Position3f32(width, height-height*dist, 0);
    GX_Color1u32(0xFFFFFFFF);
    GX_TexCoord2f32(0, 1);

    GX_Position3f32(-width, height-height*dist, 0);
    GX_Color1u32(0xFFFFFFFF);
    GX_TexCoord2f32(1, 1);
    GX_End();

    GX_LoadTexObj(&texObj, GX_TEXMAP0);
    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);

	if(use3d)
	{

		// Draw Front Cover reflection

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(1, 0);

		GX_Position3f32(width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(right, 0);

		GX_Position3f32(width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFF60);
		GX_TexCoord2f32(right, 1);

		GX_Position3f32(-width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFF60);
		GX_TexCoord2f32(1, 1);
		GX_End();
	}
	else
	{
		// Draw Front Cover reflection
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		GX_Position3f32(-width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(1, 0);

		GX_Position3f32(width, -height-height*dist, 0);
		GX_Color1u32(0xFFFFFF00);
		GX_TexCoord2f32(0, 0);

		GX_Position3f32(width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFF60);
		GX_TexCoord2f32(0, 1);

		GX_Position3f32(-width, height-height*dist, 0);
		GX_Color1u32(0xFFFFFF60);
		GX_TexCoord2f32(1, 1);
		GX_End();
	}

    GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);
    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);

	GRRLIB_2D_Init();
}
/**
 * Draw a tile.
 * @param xpos specifies the x-coordinate of the upper-left corner.
 * @param ypos specifies the y-coordinate of the upper-left corner.
 * @param tex texture to draw.
 * @param degrees angle of rotation.
 * @param scaleX
 * @param scaleY
 * @param color
 * @param frame
 */
inline void GRRLIB_DrawTile(f32 xpos, f32 ypos, GRRLIB_texImg tex, float degrees, float scaleX, f32 scaleY, u32 color, int frame) {
    GXTexObj texObj;
    f32 width, height;
    Mtx m, m1, m2, mv;

	if (tex.data==0) return;

    // Frame Correction by spiffen
    f32 FRAME_CORR = 0.001f;
    f32 s1 = (((frame%tex.nbtilew))/(f32)tex.nbtilew)+(FRAME_CORR/tex.w);
    f32 s2 = (((frame%tex.nbtilew)+1)/(f32)tex.nbtilew)-(FRAME_CORR/tex.w);
    f32 t1 = (((int)(frame/tex.nbtilew))/(f32)tex.nbtileh)+(FRAME_CORR/tex.h);
    f32 t2 = (((int)(frame/tex.nbtilew)+1)/(f32)tex.nbtileh)-(FRAME_CORR/tex.h);

    GX_InitTexObj(&texObj, tex.data, tex.tilew*tex.nbtilew, tex.tileh*tex.nbtileh, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_InitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
    GX_LoadTexObj(&texObj, GX_TEXMAP0);

    GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
    GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

    width = tex.tilew * 0.5f;
    height = tex.tileh * 0.5f;
    guMtxIdentity (m1);
    guMtxScaleApply(m1, m1, scaleX, scaleY, 1.0f);
    guVector axis = (guVector) {0, 0, 1 };
    guMtxRotAxisDeg (m2, &axis, degrees);
    guMtxConcat(m2, m1, m);
    guMtxTransApply(m, m, xpos+width, ypos+height, 0);
    guMtxConcat (GXmodelView2D, m, mv);
    GX_LoadPosMtxImm (mv, GX_PNMTX0);
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(-width, -height, 0);
    GX_Color1u32(color);
    GX_TexCoord2f32(s1, t1);

    GX_Position3f32(width, -height,  0);
    GX_Color1u32(color);
    GX_TexCoord2f32(s2, t1);

    GX_Position3f32(width, height,  0);
    GX_Color1u32(color);
    GX_TexCoord2f32(s2, t2);

    GX_Position3f32(-width, height,  0);
    GX_Color1u32(color);
    GX_TexCoord2f32(s1, t2);
    GX_End();

    GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);



    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);
}

/**
 * Print formatted output.
 * @param xpos
 * @param ypos
 * @param tex
 * @param color
 * @param zoom
 * @param text
 * @param ... Optional arguments.
 */
void GRRLIB_Printf(f32 xpos, f32 ypos, GRRLIB_texImg tex, u32 color, f32 zoom, const char *text, ...) {
	// This is provided as legacy support for drawing text via the GRRBLIB_Printf... should be using CFreeTypeGX_DrawText now for TTF
    int size;
    char tmp[1024];
    va_list argp;
    va_start(argp, text);
    size = vsprintf(tmp, text, argp);
    va_end(argp);
	CFreeTypeGX_DrawText(ttf16pt, xpos, ypos,  tmp, (GXColor){0xff, 0xff, 0xff, 0xff}, FTGX_NULL);
}

/**
 * Determines whether the specified point lies within the specified rectangle.
 * @param hotx specifies the x-coordinate of the upper-left corner of the rectangle.
 * @param hoty specifies the y-coordinate of the upper-left corner of the rectangle.
 * @param hotw the width of the rectangle.
 * @param hoth the height of the rectangle.
 * @param wpadx specifies the x-coordinate of the point.
 * @param wpady specifies the y-coordinate of the point.
 * @return If the specified point lies within the rectangle, the return value is true otherwise it's false.
 */
bool GRRLIB_PtInRect(int hotx, int hoty, int hotw, int hoth, int wpadx, int wpady) {
    return(((wpadx>=hotx) & (wpadx<=(hotx+hotw))) & ((wpady>=hoty) & (wpady<=(hoty+hoth))));
}

/**
 * Determines whether a specified rectangle lies within another rectangle.
 */
bool GRRLIB_RectInRect(int rect1x, int rect1y, int rect1w, int rect1h, int rect2x, int rect2y, int rect2w, int rect2h) {
    return ((rect1x >= rect2x) && (rect1y >= rect2y) &&
        (rect1x+rect1w <= rect2x+rect2w) && (rect1y+rect1h <= rect2y+rect2h));
}

/**
 * Determines whether a part of a specified rectangle lies on another rectangle.
 */
bool GRRLIB_RectOnRect(int rect1x, int rect1y, int rect1w, int rect1h, int rect2x, int rect2y, int rect2w, int rect2h) {
    return (GRRLIB_PtInRect(rect1x, rect1y, rect1w, rect1h, rect2x, rect2y) ||
        GRRLIB_PtInRect(rect1x, rect1y, rect1w, rect1h, rect2x+rect2w, rect2y) ||
        GRRLIB_PtInRect(rect1x, rect1y, rect1w, rect1h, rect2x+rect2w, rect2y+rect2h) ||
        GRRLIB_PtInRect(rect1x, rect1y, rect1w, rect1h, rect2x, rect2y+rect2h));
}

/**
 * Return the color value of a pixel from a GRRLIB_texImg.
 * @param x specifies the x-coordinate of the pixel in the texture.
 * @param y specifies the y-coordinate of the pixel in the texture.
 * @param tex texture to get the color from.
 * @return The color of a pixel.
 */
u32 GRRLIB_GetPixelFromtexImg(int x, int y, GRRLIB_texImg tex) {
    u8 *truc = (u8*)tex.data;
    u8 r, g, b, a;
    u32 offset;

    offset = (((y >> 2)<<4)*tex.w) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) << 1); // Fuckin equation found by NoNameNo ;)

    a=*(truc+offset);
    r=*(truc+offset+1);
    g=*(truc+offset+32);
    b=*(truc+offset+33);

    return((r<<24) | (g<<16) | (b<<8) | a);
}

/**
 * Set the color value of a pixel to a GRRLIB_texImg.
 * @see GRRLIB_FlushTex
 * @param x specifies the x-coordinate of the pixel in the texture.
 * @param y specifies the y-coordinate of the pixel in the texture.
 * @param tex texture to set the color to.
 * @param color the color of the pixel.
 */
void GRRLIB_SetPixelTotexImg(int x, int y, GRRLIB_texImg tex, u32 color) {
    u8 *truc = (u8*)tex.data;
    u32 offset;

    offset = (((y >> 2)<<4)*tex.w) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) <<1); // Fuckin equation found by NoNameNo ;)

    *(truc+offset)=color & 0xFF;
    *(truc+offset+1)=(color>>24) & 0xFF;
    *(truc+offset+32)=(color>>16) & 0xFF;
    *(truc+offset+33)=(color>>8) & 0xFF;
}

/**
 * Writes the contents of a texture in the data cache down to main memory.
 * For performance the CPU holds a data cache where modifications are stored before they get written down to mainmemory.
 * @param tex the texture to flush.
 */
void GRRLIB_FlushTex(GRRLIB_texImg tex)
{
    DCFlushRange(tex.data, tex.w * tex.h * 4);
}

/**
 * Change a texture to gray scale.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture grayscaled destination.
 */
void GRRLIB_BMFX_Grayscale(GRRLIB_texImg texsrc, GRRLIB_texImg texdest) {
    unsigned int x, y;
    u8 gray;
    u32 color;

    for(y=0; y<texsrc.h; y++) {
        for(x=0; x<texsrc.w; x++) {
            color = GRRLIB_GetPixelFromtexImg(x, y, texsrc);

            gray = (((color >> 24 & 0xFF)*77 + (color >> 16 & 0xFF)*150 + (color >> 8 & 0xFF)*28) / (255));

            GRRLIB_SetPixelTotexImg(x, y, texdest,
                ((gray << 24) | (gray << 16) | (gray << 8) | (color & 0xFF)));
        }
    }
}

/**
 * Invert colors of the texture.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture destination.
 */
void GRRLIB_BMFX_Invert(GRRLIB_texImg texsrc, GRRLIB_texImg texdest) {
    unsigned int x, y;
    u32 color;

    for(y=0; y<texsrc.h; y++) {
        for(x=0; x<texsrc.w; x++) {
            color = GRRLIB_GetPixelFromtexImg(x, y, texsrc);

            GRRLIB_SetPixelTotexImg(x, y, texdest,
                ((0xFFFFFF - (color >> 8 & 0xFFFFFF)) << 8)  | (color & 0xFF));
        }
    }
}

/**
 * Flip texture horizontal.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture destination.
 */
void GRRLIB_BMFX_FlipH(GRRLIB_texImg texsrc, GRRLIB_texImg texdest) {
    unsigned int x, y, txtWidth = texsrc.w - 1;

    for(y=0; y<texsrc.h; y++) {
        for(x=0; x<texsrc.w; x++) {
            GRRLIB_SetPixelTotexImg(txtWidth - x, y, texdest,
                GRRLIB_GetPixelFromtexImg(x, y, texsrc));
        }
    }
}

/**
 * Flip texture vertical.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture destination.
 */
void GRRLIB_BMFX_FlipV(GRRLIB_texImg texsrc, GRRLIB_texImg texdest) {
    unsigned int x, y, texHeight = texsrc.h - 1;

    for(y=0; y<texsrc.h; y++) {
        for(x=0; x<texsrc.w; x++) {
            GRRLIB_SetPixelTotexImg(x, texHeight - y, texdest,
                GRRLIB_GetPixelFromtexImg(x, y, texsrc));
        }
    }
}

/**
 * Blur a texture.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture destination.
 * @param factor the blur factor.
 */
void GRRLIB_BMFX_Blur(GRRLIB_texImg texsrc, GRRLIB_texImg texdest, int factor) {
    int numba = (1+(factor<<1))*(1+(factor<<1));

    int x, y;
    int k, l;
    int tmp=0;
    int newr, newg, newb, newa;
    u32 colours[numba];
    u32 thiscol;

    for (x = 0; x < texsrc.w; x++) {
        for (y = 0; y < texsrc.h; y++) {
            newr = 0;
            newg = 0;
            newb = 0;
            newa = 0;

            tmp=0;
            thiscol = GRRLIB_GetPixelFromtexImg(x, y, texsrc);

            for (k = x - factor; k <= x + factor; k++) {
                for (l = y - factor; l <= y + factor; l++) {
                    if (k < 0) { colours[tmp] = thiscol; }
                    else if (k >= texsrc.w) { colours[tmp] = thiscol; }
                    else if (l < 0) { colours[tmp] = thiscol; }
                    else if (l >= texsrc.h) { colours[tmp] = thiscol; }
                    else{ colours[tmp] = GRRLIB_GetPixelFromtexImg(k, l, texsrc); }
                    tmp++;
                }
            }

            for (tmp = 0; tmp < numba; tmp++) {
                newr += (colours[tmp] >> 24) & 0xFF;
                newg += (colours[tmp] >> 16) & 0xFF;
                newb += (colours[tmp] >> 8) & 0xFF;
                newa += colours[tmp] & 0xFF;
            }

            newr /= numba;
            newg /= numba;
            newb /= numba;
            newa /= numba;

            GRRLIB_SetPixelTotexImg(x, y, texdest, (newr<<24) | (newg<<16) | (newb<<8) | newa);
        }
    }
}

/**
 * A texture effect.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture destination.
 * @param factor The factor level of the effect.
 */
void GRRLIB_BMFX_Pixelate(GRRLIB_texImg texsrc, GRRLIB_texImg texdest, int factor) {
    unsigned int x, y;
    unsigned int xx, yy;
    u32 rgb;

    for(x=0; x<texsrc.w-1-factor; x+= factor) {
        for(y=0; y<texsrc.h-1-factor; y+=factor) {
            rgb=GRRLIB_GetPixelFromtexImg(x, y, texsrc);
                for(xx=x; xx<x+factor; xx++) {
                    for(yy=y; yy<y+factor; yy++) {
                        GRRLIB_SetPixelTotexImg(xx, yy, texdest, rgb);
                    }
                }
        }
    }
}

/**
 * A texture effect.
 * @see GRRLIB_FlushTex
 * @param texsrc the texture source.
 * @param texdest the texture destination.
 * @param factor The factor level of the effect.
 */
void GRRLIB_BMFX_Scatter(GRRLIB_texImg texsrc, GRRLIB_texImg texdest, int factor) {
    unsigned int x, y;
    int val1, val2;
    u32 val3, val4;
    int factorx2 = factor*2;

    for(y=0; y<texsrc.h; y++) {
        for(x=0; x<texsrc.w; x++) {
            val1 = x + (int) (factorx2 * (rand() / (RAND_MAX + 1.0))) - factor;
            val2 = y + (int) (factorx2 * (rand() / (RAND_MAX + 1.0))) - factor;

            if((val1 >= texsrc.w) || (val1 <0) || (val2 >= texsrc.h) || (val2 <0)) {
            }
            else {
                val3 = GRRLIB_GetPixelFromtexImg(x, y, texsrc);
                val4 = GRRLIB_GetPixelFromtexImg(val1, val2, texsrc);
                GRRLIB_SetPixelTotexImg(x, y, texdest, val4);
                GRRLIB_SetPixelTotexImg(val1, val2, texdest, val3);
            }
        }
    }
}

/**
 *
 * @param v
 * @param color
 * @param n
 * @param fmt
 */
void GRRLIB_GXEngine(guVector v[], u32 color, long n, u8 fmt) {
    int i;

    GX_Begin(fmt, GX_VTXFMT0, n);
    for(i=0; i<n; i++) {
        GX_Position3f32(v[i].x, v[i].y,  v[i].z);
        GX_Color1u32(color);
    }
    GX_End();
}

/**
 * Initialize GRRLIB. Call this at the beginning your code.
 * @see GRRLIB_Exit
 */
void GRRLIB_Init() {
    f32 yscale;
    u32 xfbHeight;

    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    if(rmode == NULL)
        return;

    /* Widescreen patch by CashMan's Productions (http://www.CashMan-Productions.fr.nf) */
    if (CONF_GetAspectRatio() == CONF_ASPECT_16_9)
    {
        rmode->viWidth = 678;
        rmode->viXOrigin = (VI_MAX_WIDTH_NTSC - 678)/2;
    }

    VIDEO_Configure (rmode);
    xfb[0] = (u32 *)MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfb[1] = (u32 *)MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    if(xfb[0] == NULL || xfb[1] == NULL)
        return;

    VIDEO_SetNextFramebuffer(xfb[fb]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode&VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    gp_fifo = (u8 *) CFMemAlign(32, DEFAULT_FIFO_SIZE);
    if(gp_fifo == NULL)
        return;
    memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
    GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);

	GRRLIB_SetBGColor(0); //default to black

    // other gx setup
    yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
    GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

    if (rmode->aa)
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    GX_SetDispCopyGamma(GX_GM_1_0);


    // setup the vertex descriptor
    // tells the flipper to expect direct data
    GX_ClearVtxDesc();
    GX_InvVtxCache ();
    GX_InvalidateTexAll();

    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);


    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);

    GX_SetNumChans(1);
    GX_SetNumTexGens(1);
    GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    guMtxIdentity(GXmodelView2D);
    guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -50.0F);
    GX_LoadPosMtxImm(GXmodelView2D, GX_PNMTX0);

	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);

	GRRLIB_2D_Init();

	GRRLIB_LoadTexture(&matteBlackTexture,matte_black_png);
	GRRLIB_LoadTexture(&matteGreyTexture,matte_grey_png);
	GRRLIB_LoadTexture(&rightTexture,case_right_png);
	GRRLIB_LoadTexture(&case3DShadowTexture,case_3d_shadow_png);
    GX_InitTexObj(&matteBlackTex, matteBlackTexture.data, matteBlackTexture.w, matteBlackTexture.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_InitTexObj(&matteGreyTex, matteGreyTexture.data, matteGreyTexture.w, matteGreyTexture.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&rightSideTex, rightTexture.data, rightTexture.w, rightTexture.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_InitTexObj(&case3DShadowTex, case3DShadowTexture.data, case3DShadowTexture.w, case3DShadowTexture.h, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);

	//Video_ManualSet(xfb[0], rmode);

}

void GRRLIB_SetBGColor(int theme)
{
	if (theme) // white theme
	{
		// clears the bg to color and clears the z buffer
		GXColor background = { 0xCF, 0xCF, 0xCF, 0xFF };
		GX_SetCopyClear (background, GX_MAX_Z24);  // I don't know why it flashes to black here briefly...??
		GRRLIB_Render();

	}
	else // black or default
	{
		// clears the bg to color and clears the z buffer
		GXColor background = { 0x00, 0x00, 0x00, 0xFF };
		GX_SetCopyClear (background, GX_MAX_Z24);
		GRRLIB_Render();
	}

}

void GRRLIB_3D_Init()
{
    Mtx44 perspective;

	guVector cam = {settings.coverCamX, settings.coverCamY, settings.coverZoom},
			up = {0.0F, 1.0F, 0.0F},
		  look = {0.0F, 0.0F, 1.0F};
	guLookAt(view, &cam, &up, &look);

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
    //f32 w = rmode->viWidth;
    //f32 h = rmode->viHeight;
	//guPerspective(perspective, 45, 1.0, 0.1F, 300.0F);
	if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
		guPerspective(perspective, 45, 1.15F, 0.1F, 300.0F);
	else
		guPerspective(perspective, 45, 1.0, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
}

void GRRLIB_2D_Init()
{
    Mtx44 perspective;

    guOrtho(perspective,0, 479, 0, 639, 0, 300.0F);
    GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
}

/**
 * Call this function after drawing.
 */
void GRRLIB_Render() {
    GX_DrawDone ();

    fb ^= 1;        // flip framebuffer
    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(xfb[fb], GX_TRUE);
    VIDEO_SetNextFramebuffer(xfb[fb]);
    VIDEO_Flush();
    VIDEO_WaitVSync();
}

/**
 * Call this before exiting your application.
 */
void GRRLIB_Exit() {

	// Free custom GRRLIB textures

	CFFree(rightTexture.data);
	CFFree(matteBlackTexture.data);
	CFFree(matteGreyTexture.data);

    GX_Flush();
    GX_AbortFrame();

    if(xfb[0] != NULL) {
	//	VIDEO_ClearFrameBuffer(rmode, xfb[0], 0x000000);
        CFFree(MEM_K1_TO_K0(xfb[0]));
        xfb[0] = NULL;
    }
    if(xfb[1] != NULL) {
	//	VIDEO_ClearFrameBuffer(rmode, xfb[1], 0x000000);
        CFFree(MEM_K1_TO_K0(xfb[1]));
        xfb[1] = NULL;
    }
    if(gp_fifo != NULL) {
        CFFree(gp_fifo);
        gp_fifo = NULL;
    }
}

/**
 * Make a PNG screenshot on the SD card.
 * @param File Name of the file to write.
 * @return True if every thing worked, false otherwise.
 */
bool GRRLIB_ScrShot(const char* File)
{
    IMGCTX pngContext;
    int ErrorCode = -1;

    //if(fatInitDefault() && (pngContext = PNGU_SelectImageFromDevice(File)))
	if((pngContext = PNGU_SelectImageFromDevice(File)))
    {
        ErrorCode = PNGU_EncodeFromYCbYCr(pngContext, 640, 480, xfb[fb], 0);
        PNGU_ReleaseImageContext(pngContext);
    }
    return !ErrorCode;
}
