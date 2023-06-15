/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "biddle.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//Biddle character structure
enum
{
  Biddle_ArcMain_Idle0,
  Biddle_ArcMain_Idle1,
  Biddle_ArcMain_Idle2,
  Biddle_ArcMain_Left0,
  Biddle_ArcMain_Left1,
  Biddle_ArcMain_Left2,
  Biddle_ArcMain_Left3,
  Biddle_ArcMain_Left4,
  Biddle_ArcMain_Down0,
  Biddle_ArcMain_Down1,
  Biddle_ArcMain_Down2,
  Biddle_ArcMain_Down3,
  Biddle_ArcMain_Down4,
  Biddle_ArcMain_Up0,
  Biddle_ArcMain_Up1,
  Biddle_ArcMain_Up2,
  Biddle_ArcMain_Right0,
  Biddle_ArcMain_Right1,
  Biddle_ArcMain_Right2,
  Biddle_ArcMain_Right3,
	
	Biddle_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[Biddle_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_Biddle;

//Biddle character definitions
static const CharFrame char_biddle_frame[] = {
  {Biddle_ArcMain_Idle0, {  0,  0,122,189}, {149,134}}, //0 idle 1
  {Biddle_ArcMain_Idle0, {122,  0,125,188}, {152,133}}, //1 idle 2
  {Biddle_ArcMain_Idle1, {  0,  0,126,189}, {153,135}}, //2 idle 3
  {Biddle_ArcMain_Idle1, {126,  0,125,192}, {152,138}}, //3 idle 4
  {Biddle_ArcMain_Idle2, {  0,  0,124,192}, {152,138}}, //4 idle 5

  {Biddle_ArcMain_Left0, {  0,  0,131,176}, {162,122}}, //5 left 1
  {Biddle_ArcMain_Left1, {  0,  0,130,174}, {162,120}}, //6 left 2
  {Biddle_ArcMain_Left2, {  0,  0,129,174}, {162,120}}, //7 left 3
  {Biddle_ArcMain_Left3, {  0,  0,129,174}, {162,120}}, //8 left 4
  {Biddle_ArcMain_Left4, {  0,  0,129,175}, {162,120}}, //9 left 5

  {Biddle_ArcMain_Down0, {  0,  0,134,151}, {158, 97}}, //10 down 1
  {Biddle_ArcMain_Down1, {  0,  0,136,149}, {158, 94}}, //11 down 2
  {Biddle_ArcMain_Down2, {  0,  0,136,148}, {157, 93}}, //12 down 3
  {Biddle_ArcMain_Down3, {  0,  0,137,148}, {158, 93}}, //13 down 4
  {Biddle_ArcMain_Down4, {  0,  0,136,148}, {158, 93}}, //14 down 5

  {Biddle_ArcMain_Up0, {  0,  0,126,180}, {161,125}}, //15 up 1
  {Biddle_ArcMain_Up0, {126,  0,124,184}, {159,129}}, //16 up 2
  {Biddle_ArcMain_Up1, {  0,  0,124,186}, {158,131}}, //17 up 3
  {Biddle_ArcMain_Up1, {124,  0,124,186}, {158,131}}, //18 up 4
  {Biddle_ArcMain_Up2, {  0,  0,124,186}, {158,131}}, //19 up 5

  {Biddle_ArcMain_Right0, {  0,  0,124,189}, {149,134}}, //20 right 1
  {Biddle_ArcMain_Right0, {124,  0,128,187}, {151,132}}, //21 right 2
  {Biddle_ArcMain_Right1, {  0,  0,128,187}, {151,132}}, //22 right 3
  {Biddle_ArcMain_Right2, {  0,  0,128,187}, {151,132}}, //23 right 4
  {Biddle_ArcMain_Right3, {  0,  0,128,187}, {151,132}}, //24 right 5
};

static const Animation char_biddle_anim[CharAnim_Max] = {
	{2, (const u8[]){ 0, 1, 2, 3, 4, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 5, 6, 7, 8, 9, 9, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 10, 11, 12, 13, 14, 14, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 15, 16, 17, 18, 19, 19, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){ 20, 21, 22, 23, 24, 24, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//Biddle character functions
void Char_Biddle_SetFrame(void *user, u8 frame)
{
	Char_Biddle *this = (Char_Biddle*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_biddle_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_Biddle_Tick(Character *character)
{
	Char_Biddle *this = (Char_Biddle*)character;
	
	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_Biddle_SetFrame);
	if ((stage.song_step >= 1428) && (stage.song_step <= 1439))
		Character_DrawBlend(character, &this->tex, &char_biddle_frame[this->frame]);
	else
		Character_Draw(character, &this->tex, &char_biddle_frame[this->frame]);
}

void Char_Biddle_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_Biddle_Free(Character *character)
{
	Char_Biddle *this = (Char_Biddle*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_Biddle_New(fixed_t x, fixed_t y)
{
	//Allocate biddle object
	Char_Biddle *this = Mem_Alloc(sizeof(Char_Biddle));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_Biddle_New] Failed to allocate biddle object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_Biddle_Tick;
	this->character.set_anim = Char_Biddle_SetAnim;
	this->character.free = Char_Biddle_Free;
	
	Animatable_Init(&this->character.animatable, char_biddle_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 3;

	//health bar color
	this->character.health_bar = 0xFFFFFFFF;
	
	this->character.focus_x = FIXED_DEC(65,1);
	this->character.focus_y = FIXED_DEC(-115,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	this->character.zoom_save = this->character.focus_zoom;
	this->character.size = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\PLAYER\\BIDDLE.ARC;1");
	
	const char **pathp = (const char *[]){
  "idle0.tim",
  "idle1.tim",
  "idle2.tim",
  "left0.tim",
  "left1.tim",
  "left2.tim",
  "left3.tim",
  "left4.tim",
  "down0.tim",
  "down1.tim",
  "down2.tim",
  "down3.tim",
  "down4.tim",
  "up0.tim",
  "up1.tim",
  "up2.tim",
  "right0.tim",
  "right1.tim",
  "right2.tim",
  "right3.tim",
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
