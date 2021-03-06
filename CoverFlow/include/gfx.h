#ifndef _GFX_H_
#define _GFX_H_

#include "coverflow.h"
#include "button.h"
#include "titles.h"

char* _title;
char* _msg;

extern const u8 slide_png[];
extern const u8 slide_hover_png[];
extern const u8 star_on_png[];
extern const u8 star_off_png[];

/*Home Menu Images*/
extern const u8 menu_home_button_png[];
extern const u8 menu_home_button_over_png[];
extern const u8 menu_home_wiimote_png[];
extern const u8 menu_home_top_png[];
extern const u8 menu_home_top_over_png[];
extern const u8 menu_home_bottom_png[];
extern const u8 menu_home_bottom_over_png[];
extern const u8 battery_png[];
extern const u8 battery_bar_png[];
extern const u8 battery_dead_png[];
extern const u8 battery_bar_red_png[];

// Settings Menu stuff
extern const u8 button_bar_h28w104_black_png[];			// Basic UI button in black
extern const u8 button_bar_h28w104_white_png[];			// Basic UI button in white
extern const u8 button_bar_h28w104_toggle_left_png[];	// Two state white/black button
extern const u8 button_bar_h28w104_toggle_right_png[];  // Two state black/white button
extern const u8 button_minus_png[];						// Left side of -/+ combo button
extern const u8 button_minus_over_png[];				// Left side hover
extern const u8 button_plus_png[];						// Right side of -/+ combo button
extern const u8 button_plus_over_png[];					// Right side hover
extern const u8 button_round_add_png[];					// Round button - add
extern const u8 button_round_add_over_png[];			// Round button - add over
extern const u8 button_round_gear_png[];				// Round button - gear
extern const u8 button_round_gear_over_png[];			// Round button - gear over
extern const u8 button_round_delete_png[];				// Round button - delete
extern const u8 button_round_delete_over_png[];			// Round button - delete over
extern const u8 button_round_lock_png[];				// Round button - lock
extern const u8 button_round_lock_over_png[];			// Round button - lock over
extern const u8 button_round_unlock_png[];				// Round button - unlock
extern const u8 button_round_unlock_over_png[];			// Round button - unlock over
extern const u8 dialog_box_titlebar_png[];				// Basic UI Dialog panel Titlebar
extern const u8 dialog_box_titlebar_long_png[];			// Basic UI Dialog panel Titlebar long version
extern const u8 dialog_box_icon_png[];					// Icon for dialog panel
extern const u8 menu_graphics_wireframe_png[];			// Wireframe of the covers used on the Graphics Settings panel
extern const u8 menu_graphics_box1_png[];				// Control box 1
extern const u8 menu_graphics_box2_png[];				// Control box 2
extern const u8 menu_settings_box_png[];				// Control box 3 - Used on Settings Panel
extern const u8 menu_button_png[];						// Large menu button on settings top header
extern const u8 menu_button_over_png[];					// Large menu hover
extern const u8 menu_header_png[];						// Header bar for settings screens
extern const u8 menu_logo_png[];						// CoverFloader logo
extern const u8 menu_panel_png[];						// Settings screen background panel (TODO replace with GRRLIB rectangle)

extern const u8 button_left_png[];						// Left side of </> combo button
extern const u8 button_left_over_png[];					// Left side hover
extern const u8 button_right_png[];						// Right side of </> combo button
extern const u8 button_right_over_png[];				// Right side hover

extern const u8 flag_br_png[];							// Flags...
extern const u8 flag_da_png[];
extern const u8 flag_de_png[];
extern const u8 flag_fr_png[];
extern const u8 flag_it_png[];
extern const u8 flag_us_png[];
extern const u8 flag_es_png[];		// spanish
extern const u8 flag_ct_png[];		// catalan
extern const u8 flag_fi_png[];		// finnish
extern const u8 flag_MyLang_png[];	// MyLang
extern const u8 flag_dk_png[];		// danish
extern const u8 flag_tr_png[];		// turkish
extern const u8 flag_hu_png[];		// hungarian
extern const u8 button_cheat_on_png[];  //for cheat manager
extern const u8 button_cheat_off_png[];  //for cheat manager
extern const u8 button_cheat_edit_png[];
extern const u8 button_cheat_edit_over_png[];
extern const u8 hdspace_png[];
extern const u8 button_round_info_png[];
extern const u8 button_round_info_over_png[];
extern const u8 button_char_up_png[];
extern const u8 button_char_up_over_png[];
extern const u8 button_char_down_png[];
extern const u8 button_char_down_over_png[];

#ifdef NEWS_READER
extern const u8 button_round_news_png[];
extern const u8 button_round_news_over_png[];
extern const u8 button_round_news_blue_png[];
Button3 newsButton;
Button  checkNewsButton;
Button updateAppButton;
#endif


#ifdef OSK
extern const u8 kb_key_png[];
extern const u8 kb_function_png[];
extern const u8 kb_space_png[];
Button kb_key[4][11];
Button kb_function[4];
Button kb_space;
Button kb_OK;
Button kb_ESC;
#endif


extern const u8 button_edit_png[];
extern const u8 button_edit_over_png[];
Button button_edit[9];

GRRLIB_texImg menu_button_texture;
GRRLIB_texImg menu_button_over_texture;
GRRLIB_texImg menu_header_texture;
GRRLIB_texImg menu_header_vflip_texture;
GRRLIB_texImg menu_logo_texture;
GRRLIB_texImg dialog_box_titlebar_texture;
GRRLIB_texImg dialog_box_titlebar_long_texture;
GRRLIB_texImg dialog_box_icon_texture;
GRRLIB_texImg menu_graphics_wireframe_texture;
GRRLIB_texImg menu_graphics_box1_texture;
GRRLIB_texImg menu_graphics_box2_texture;
Button menuSettingsButton;
Button menuGraphicsButton;
Button menuLanguagesButton;
Button menuLogoButton;
Button menuAdvancedButton;

/*
GRRLIB_texImg flag_br_texture; // Brazil flag
GRRLIB_texImg flag_da_texture; // Netherlands Flag
GRRLIB_texImg flag_de_texture; // Germany Flag
GRRLIB_texImg flag_fr_texture; // France Flag
GRRLIB_texImg flag_it_texture; // Italy Flag
GRRLIB_texImg flag_tr_texture; // Turkey Flag
GRRLIB_texImg flag_us_texture; // US Flag
GRRLIB_texImg flag_es_texture; // Spain Flag
GRRLIB_texImg flag_ct_texture; // Catalan Flag
GRRLIB_texImg flag_fi_texture; // Finnland Flag
GRRLIB_texImg flag_MyLang_texture; // MyLang Flag
GRRLIB_texImg flag_dk_texture; // Denmark Flag
GRRLIB_texImg flag_hu_texture; // Hungary Flag
*/
//GRRLIB_texImg button_round_info_texture;
//GRRLIB_texImg button_round_info_over_texture;


Button flagBRButton;
Button flagDAButton;
Button flagDEButton;
Button flagFRButton;
Button flagITButton;
Button flagTRButton;
Button flagUSButton;
Button flagESButton;
Button flagCTButton;
Button flagFIButton;
Button flagMyLangButton;
Button flagDKButton;
Button flagHUButton;

Button infoButton;

//cheat manager buttons
Button cheatEnabled[10];
Button cheatDisabled[10];
Button cheatEditButton[10];
Button manageCheatsButton;
Button pageUpButton;
Button pageDownButton;
Button cheatDoneButton;
Button selectAllButton;
Button deselectAllButton;
Button charUpButton[8];
Button charDownButton[8];
Button varEditButton[10];
Button varDoneButton;

GRRLIB_texImg cover_texture;
GRRLIB_texImg cover_texture_3d;
GRRLIB_texImg empty_texture;
GRRLIB_texImg no_disc_texture;
GRRLIB_texImg current_cover_texture;
GRRLIB_texImg progress_step_texture;
GRRLIB_texImg progress_bar_texture;
GRRLIB_texImg slidebar_texture;			// black theme slider
GRRLIB_texImg slidebar_white_texture;	// white theme slider
GRRLIB_texImg ambientlight_texture;
GRRLIB_texImg ambientlight_white_texture;
GRRLIB_texImg battery;
GRRLIB_texImg battery_dead;
GRRLIB_texImg battery_bar;
GRRLIB_texImg battery_bar_red;
GRRLIB_texImg hdspace_texture;

Button addButton;
Button slideButton;
Button okButton;
Button backButton;
Button cancelButton;
Button loadButton;
Button editGameIDButton;
Button lockButton;
Button unlockButton;
Button cheatonButton;
Button cheatoffButton;
Button yesButton;
Button noButton;
Button deleteButton;
Button settingsButton;
Button toggleOnButton;
Button toggleOffButton;
Button loseronButton;
Button loseroffButton;

Button infoLeftButton;
Button infoRightButton;

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
Button gfixupButton;
Button gfixdownButton;
Button gAltDolupButton;
Button gAltDoldownButton;
Button gciosupButton;
Button gciosdownButton;
Button gblockOnButton;
Button gblockOffButton;

Button gDOLdownButton;
Button gDOLupButton;


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
Button barstepupButton;
Button barstepdownButton;

Button bookmarkOnButton;
Button bookmarkOffButton;
Button cheatDownButton;
Button sysciosupButton;
Button sysciosdownButton;

Button covers3dThinButton;
Button covers3dFatButton;
Button covers3dOnButton;
Button covers3dOffButton;
Button hidescrollOnButton;
Button hidescrollOffButton;

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

Button falldistupButton;
Button falldistdownButton;
Button coverzoomupButton;
Button coverzoomdownButton;

Button resetButton;
Button restoreButton;

/*Home Menu Buttons*/
Button homeMenuTopButton;
Button homeMenuBottomButton;
Button wiimoteButton;
Button loaderButton;
Button wiiMenuButton;
void MemInfo();
void ResetBuffer();
void LoadFonts();
void LoadTextures();
void DrawBufferedCover(int i, float loc, float zpos, float angle, float falloff );
void Paint_Progress(float v, char* msg);
void Paint_Progress_Generic(int v, int max, char* msg);
void Paint_Progress_FadeToBG();
void Paint_Progress_FadeInStart();
void Init_Buttons();
void Label_Buttons();
void GRRLIB_Cover(float pos, int texture_id);
void GetWiimoteData();
void DrawCursor(int type, f32 xpos, f32 ypos, float degrees, float scaleX, f32 scaleY, u32 color);
void DrawSlider(int yPos, int theme_id);	   // takes a SETTING_theme, but defaults to black
int  DrawLoadGameDialog(bool load, bool hover);
void DrawCoverFlyInStart();
void draw_covers();
void draw_game_title(int yPos, int index, float textSize);


float change_scale_without_containing(float val, float in_min, float in_max, float out_min, float out_max);
float change_scale(float val, float in_min, float in_max, float out_min, float out_max);
bool  WindowPrompt(char* title, char* txt, struct Button* choice_a, struct Button* choice_b);
bool  WindowPromptInstall(char* id, char* title, char* txt, struct Button* choice_a, struct Button* choice_b, int option);

void loadProgressStep();
void ShowProgress (s32 done, s32 total);
int  ProgressWindow(wbfs_t *hdd, char* title, char* msg);
void freeResources();

// Cover rollover event handlers
bool CoverHoverCenter();
bool CoverHoverRight();
bool CoverHoverLeft();
void MemInfo_2();

#endif

