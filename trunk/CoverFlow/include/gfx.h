#ifndef _GFX_H_
#define _GFX_H_

#include "coverflow.h"
#include "button.h"
#include "titles.h"

#define FLIP_SPEED  0.016

char* _title;
char* _msg;

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
extern const u8 reset_png[];
extern const u8 reset_hover_png[];
extern const u8 settings_png[];
extern const u8 settings_hover_png[];
extern const u8 toggle_on_png[];
extern const u8 toggle_off_png[];
extern const u8 plus_button_png[];
extern const u8 plus_button_hover_png[];
extern const u8 minus_button_png[];
extern const u8 minus_button_hover_png[];
extern const u8 covers_png[];
extern const u8 covers_hover_png[];
extern const u8 titles_png[];
extern const u8 titles_hover_png[];
extern const u8 theme_white_png[];
extern const u8 theme_black_png[];
extern const u8 bg_options_screen_no_transparency_png[];
extern const u8 star_on_png[];
extern const u8 star_off_png[];

/*Home Menu Images*/
extern const u8 wiiMenu_png[];
extern const u8 wiiMenu_hover_png[];
extern const u8 loader_png[];
extern const u8 loader_hover_png[];
extern const u8 wiimote_png[];
extern const u8 homeTop_png[];
extern const u8 homeTop_hover_png[];
extern const u8 homeBottom_png[];
extern const u8 homeBottom_hover_png[];
extern const u8 battery_png[];
extern const u8 battery_bar_png[];
extern const u8 battery_dead_png[];

GRRLIB_texImg cover_texture;
GRRLIB_texImg back_texture;
GRRLIB_texImg empty_texture;
GRRLIB_texImg no_disc_texture;
GRRLIB_texImg current_cover_texture;
GRRLIB_texImg loader_main_texture;
GRRLIB_texImg progress_texture;
GRRLIB_texImg menu_bg_texture;
GRRLIB_texImg slidebar_texture;			// black theme slider
GRRLIB_texImg slidebar_white_texture;	// white theme slider
GRRLIB_texImg font_texture;				// little old font
GRRLIB_texImg font_title;				// title font
GRRLIB_texImg font_title_small;			// title font small
GRRLIB_texImg load_bg_texture;
GRRLIB_texImg ambientlight_texture;
GRRLIB_texImg ambientlight_white_texture;
GRRLIB_texImg battery;
GRRLIB_texImg battery_dead;
GRRLIB_texImg battery_bar;

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
Button resetButton;
Button settingsButton;
Button toggleOnButton;
Button toggleOffButton;
Button gsettingsButton;
Button gbackButton;
Button glangupButton;
Button glangdownButton;
Button gvidupButton;
Button gviddownButton;
Button ghookupButton;
Button ghookdownButton;
Button gvidtvonButton;
Button gvidtvoffButton;
Button gcheatonButton;
Button gcheatoffButton;

Button coverTextOnButton;
Button coverTextOffButton;

Button langupButton;
Button langdownButton;
Button vidupButton;
Button viddownButton;
Button hookupButton;
Button hookdownButton;
Button vidtvonButton;
Button vidtvoffButton;
Button titlesButton;
Button coversButton;
Button graphicsButton;
Button themeWhiteButton;
Button themeBlackButton;
Button quickstartOnButton;
Button quickstartOffButton;
Button rumbleOnButton;
Button rumbleOffButton;
Button musicOnButton;
Button musicOffButton;
Button bookmarkOnButton;
Button bookmarkOffButton;

/*gfx buttons*/
Button spacingupButton;
Button spacingdownButton;
Button angleupButton;
Button angledownButton;
Button zoomupButton;
Button zoomdownButton;
Button falloffupButton;
Button falloffdownButton;
Button windowupButton;
Button windowdownButton;

/*Home Menu Buttons*/
Button homeMenuTopButton;
Button homeMenuBottomButton;
Button wiimoteButton;
Button loaderButton;
Button wiiMenuButton;

void LoadTextures();
void DrawBufferedCover(int i, float loc, float angle, float falloff );
void Paint_Progress(float v, char* msg);
void Paint_Progress_Generic(int v, int max, char* msg);
void Init_Buttons();
void GRRLIB_Cover(float pos, int texture_id);
void GetWiimoteData();
void DrawCursor(int type, f32 xpos, f32 ypos, float degrees, float scaleX, f32 scaleY, u32 color);
void DrawSlider(int theme_id);	   // takes a SETTING_theme, but defaults to black

void draw_covers();
void draw_game_title(int index, float textSize);
void draw_selected();

//Work for new Load Animation
int draw_selected_two(bool load, bool hover);

float change_scale_without_containing(float val, float in_min, float in_max, float out_min, float out_max);
float change_scale(float val, float in_min, float in_max, float out_min, float out_max);
int WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b);

void ShowProgress (s32 done, s32 total);
int ProgressWindow(wbfs_t *hdd, char* title, char* msg);
void game_settings_menu();
void freeResources();

// Cover rollover event handlers
bool CoverHoverCenter();
bool CoverHoverRight();
bool CoverHoverLeft();

#endif

