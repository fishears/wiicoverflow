#ifndef _GFX_H_
#define _GFX_H

#include "coverflow.h"
#include "button.h"

extern const u8 add_button_png[];
extern const u8 add_button_hover_png[];
extern const u8 slide_png[];
extern const u8 slide_hover_png[];
extern const u8 load_png[];
extern const u8 load_hover_png[];
extern const u8 back_png[];
extern const u8 back_hover_png[];
extern const u8 ok_png[];
extern const u8 ok_hover_png[];
extern const u8 cancel_png[];
extern const u8 cancel_hover_png[];
extern const u8 yes_png[];
extern const u8 yes_hover_png[];
extern const u8 no_png[];
extern const u8 no_hover_png[];
extern const u8 cheatson_png[];
extern const u8 cheatson_hover_png[];
extern const u8 cheatsoff_png[];
extern const u8 cheatsoff_hover_png[];
extern const u8 delete_png[];
extern const u8 delete_hover_png[];
extern const u8 settings_png[];
extern const u8 settings_hover_png[];

GRRLIB_texImg cover_texture;
GRRLIB_texImg back_texture;
GRRLIB_texImg empty_texture;
GRRLIB_texImg no_disc_texture;
GRRLIB_texImg current_cover_texture;
GRRLIB_texImg text_font1;
GRRLIB_texImg helvetica;
GRRLIB_texImg loader_main_texture;
GRRLIB_texImg progress_texture;
GRRLIB_texImg gradient_texture;
GRRLIB_texImg menu_bg_texture;
GRRLIB_texImg slide_bar_texture;
GRRLIB_texImg usb_error_texture;
GRRLIB_texImg tex_BMfont5;

Button addButton;
Button slideButton;
Button okButton;
Button backButton;
Button cancelButton;
Button loadButton;
Button cheatonButton;
Button cheatoffButton;
Button yesButton;
Button noButton;
Button deleteButton;
Button settingsButton;

void DrawBufferedCover(int i, float loc, float angle);
void Paint_Progress(float v, char* msg);
void Init_Buttons();
void Hover_Buttons();
void DrawSlider();
void GRRLIB_Cover(float pos, int texture_id);
//void draw_game_title(int index);
void draw_covers();
float change_scale_without_containing(float val, float in_min, float in_max, float out_min, float out_max);
float change_scale(float val, float in_min, float in_max, float out_min, float out_max);

#endif
