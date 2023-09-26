/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
 *  God Wars Mud improvements copyright (C) 1994, 1995, 1996 by Richard    *
 *  Woolcock.  This mud is NOT to be copied in whole or in part, or to be  *
 *  run without the permission of Richard Woolcock.  Nobody else has       *
 *  permission to authorise the use of this code.                          *
 ***************************************************************************/

/************************************************************
 * This code has been modified for the Twilight Reconquista *
 * Anyone may use it if they wish 			    *
 * We ask only for credit in the login or MOTD              *
 ************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"

const int points_distribution[3] = { 10, 8, 6 };

const int abilities_points_distribution[3] = { 13, 9, 5 };

const int ability_insight_levels[MAX_TRAINABLE_ABILITY + 1] = { 0, 1, 3, 6, 9, 18, 30 };

const char *spec_names[MAX_SPECS][25] =
{
  {"cast_adept"},
  {"wildmob"},
  {"lumberjack"}
};


const char *road_names[MAX_ROADS][25] =
{
{"Beast"},
{"Blood"},
{"Kings"},
{"Sin"},
{"Heaven"},
{"Humanity"},
{"Paradox"},
{"Serpent"},
{"Bones"},
{"Slain"},
{"Night"},
{"Metamorphisis"} };

const char *attrib_names[3][3][25] = {
	{{"Strength"}, {"Dexterity"}, {"Stamina"}},
	{{"Charisma"}, {"Manipulation"}, {"Appearance"}},
	{{"Perception"}, {"Intelligence"}, {"Wits"}}
};

const char *ability_names[3][10][25] = {
	{{"Expression"}, {"Alertness"}, {"Athletics"}, {"Brawl"}, {"Dodge"}, {"Empathy"}, {"Intimidation"}, {"Streetwise"}, {"Leadership"}, {"Subterfuge"}},
	{{"AnimalKen"}, {"Firearms"}, {"Crafts"}, {"Etiquette"}, {"Security"}, {"Melee"}, {"Performance"}, {"Drive"}, {"Stealth"}, {"Survival"}},
	{{"Academics"}, {"Computer"}, {"Investigation"}, {"Law"}, {"Linguistics"}, {"Medicine"}, {"Occult"}, {"Politics"}, {"Science"}, {"Finance"}}
};

const char *ww_breeds[BREED_MAX + 1][25] = { {"Homid"}, {"Lupus"}, {"Metis"} };

const char *ww_auspices[AUSPICE_MAX + 1][25] = { {"Ragabash"}, {"Theurge"}, {"Philodox"}, {"Galliard"}, {"Ahroun"} };

const char *ww_tribes[TRIBE_MAX + 1][25] = { {"Black Furies"}, {"Bone Gnawers"}, {"Children of Gaia"}, {"Fianna"}, {"Get of Fenris"},
{"Glass Walkers"}, {"Red Talons"}, {"Silent Strides"}, {"Shadow Lords"}, {"Silver Fangs"}, {"Stargazers"}, {"Uktena"}, {"Wendigos"}
};


/*
 * Attribute bonus tables.
 */
const struct str_att_bonus_type str_bonus[12] = {
	{0, 0, 0},		/* 0 */
	{1, 80, 5},		/* 1 */
	{10, 200, 10},		/* 2 */
	{30, 500, 20},		/* 3 */
	{50, 800, 30},		/* 4 */
	{100, 1300, 45},	/* 5 */
	{150, 1600, 60},	/* 6 */
	{200, 1900, 80},	/* 7 */
	{350, 2200, 100},	/* 8 */
	{400, 2500, 120},	/* 9 */
	{450, 2800, 140},	/* 10 */
	{500, 3100, 150}	/* 11 */
};


const struct dex_att_bonus_type dex_bonus[12] = {
	{0, 0},			/* 0 */
	{1, 50},		/* 1 */
	{10, 0},		/* 2 */
	{30, -50},		/* 3 */
	{50, -150},		/* 4 */
	{100, -250},		/* 5 */
	{200, -350},		/* 6 */
	{300, -450},		/* 7 */
	{400, -550},		/* 8 */
	{500, -650},		/* 9 */
	{600, -750},		/* 10 */
	{700, -850}		/* 11 */
};

/*
 * Liquid properties.
 * Used in world.obj.
 */
const struct liq_type liq_table[LIQ_MAX] = {
	{"water", "clear", {0, 1, 10}},	/*  0 */
	{"beer", "amber", {3, 2, 5}},
	{"wine", "rose", {5, 2, 5}},
	{"ale", "brown", {2, 2, 5}},
	{"dark ale", "dark", {1, 2, 5}},

	{"whisky", "golden", {6, 1, 4}},	/*  5 */
	{"lemonade", "pink", {0, 1, 8}},
	{"firebreather", "boiling", {10, 0, 0}},
	{"local specialty", "everclear", {3, 3, 3}},
	{"slime mold juice", "green", {0, 4, -8}},

	{"milk", "white", {0, 3, 6}},	/* 10 */
	{"tea", "tan", {0, 1, 6}},
	{"coffee", "black", {0, 1, 6}},
	{"blood", "red", {0, 0, 5}},
	{"salt water", "clear", {0, 1, -2}},

	{"cola", "cherry", {0, 1, 5}}	/* 15 */
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n

const struct skill_type skill_table[MAX_SKILL] = {

	{
	 "reserved", 99,
	 0, TAR_IGNORE, POS_STANDING,
	 NULL, SLOT (0), 0, 0,
	 "", ""},

	{
	 "peek", 4,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_peek, SLOT (0), 0, 0,
	 "", "!Peek!"},

	{
	 "purple sorcery", 99,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 NULL, SLOT (554), 100, 12,
	 "", "The purple spell on you fades away."},

	{
	 "red sorcery", 99,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 NULL, SLOT (555), 100, 12,
	 "", "The red spell on you fades away."},

	{
	 "blue sorcery", 99,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 NULL, SLOT (556), 100, 12,
	 "", "The blue spell on you fades away."},

	{
	 "green sorcery", 99,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 NULL, SLOT (557), 100, 12,
	 "", "The green spell on you fades away."},

	{
	 "yellow sorcery", 99,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 NULL, SLOT (558), 100, 12,
	 "", "The yellow spell on you fades away."},

	{
	 "strength", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (568), 100, 12,
	 "", "Your supernatural strength fades."},

	{
	 "dexterity", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (569), 100, 12,
	 "", "Your supernatural dexterity fades."},

	{
	 "stamina", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (570), 100, 12,
	 "", "Your supernatural stamina fades."},

	{
	 "blood potency", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 &gsn_potency, SLOT (571), 100, 12,
	 "", "Your blood returns to its original potency."},

	{
	 "weakness", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (573), 100, 12,
	 "", "You feel stronger."},

	{
	 "slowness", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (574), 100, 12,
	 "", "You feel faster."},

	{
	 "infirmity", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (575), 100, 12,
	 "", "You feel healthier."},

	{
	 "flame bolt", 99,
	 spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
	 NULL, SLOT (578), 20, 12,
	 "flame bolt", "!FlameBolt!"},

	{
	 "infernal flame bolt", 99,
	 spell_null, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
	 NULL, SLOT (580), 20, 12,
	 "infernal flame bolt", "!InfernalFlameBolt!"},

	{
	 "levitation", 99,
	 spell_null, TAR_CHAR_SELF, POS_STANDING,
	 NULL, SLOT (93), 10, 12,
	 "poison", "!Levitation!"},

	{
	 "english", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_english, SLOT (0), 0, 24,
	 "english", "!english!"},
	{
	 "german", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_german, SLOT (0), 0, 24,
	 "german", "!german!"},
	{
	"chinese", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_chinese, SLOT (0), 0, 24,
	 "chinese", "!chinese!"},
	{
	 "japanese", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_japanese, SLOT (0), 0, 24,
	 "japanese", "!japanese!"},
	{
	 "russian", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_russian, SLOT (0), 0, 24,
	 "russian", "!russian!"},
	{
	 "arabic", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_arabic, SLOT (0), 0, 24,
	 "arabic", "!arabic!"},
	{
	 "latin", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_latin, SLOT (0), 0, 24,
	 "latin", "!latin!"},
	{
	 "italian", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_italian, SLOT (0), 0, 24,
	 "italian", "!italian!"},
	{
	 "spanish", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_spanish, SLOT (0), 0, 24,
	 "spanish", "!spanish!"},
	{
	 "hebrew", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_hebrew, SLOT (0), 0, 24,
	 "hebrew", "!hebrew!"},
	{
	 "gaelic", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_gaelic, SLOT (0), 0, 24,
	 "gaelic", "!gaelic!"},
	{
	 "indonesian", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_indonesian, SLOT (0), 0, 24,
	 "indonesian", "!indonesian!"},
	{
	 "greek", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_greek, SLOT (0), 0, 24,
	 "greek", "!greek!"},
	{
	 "slavic", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_slavic, SLOT (0), 0, 24,
	 "slavic", "!slavic!"},
	{
	 "french", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_french, SLOT (0), 0, 24,
	 "french", "!french!"},
	{
	 "garou", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_garou, SLOT (0), 0, 24,
	 "garou", "!garou!"},
	{
	 "korean", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_korean, SLOT (0), 0, 24,
	 "korean", "!korean!"},
	{
	 "vietnamese", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_vietnamese, SLOT (0), 0, 24,
	 "vietnamese", "!vietnamese!"},
	{
	 "swahili", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_swahili, SLOT (0), 0, 24,
	 "swahili", "!swahili!"},
	{
	 "tagalog", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_tagalog, SLOT (0), 0, 24,
	 "tagalog", "!tagalog!"},
	{
	 "hindu", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_hindu, SLOT (0), 0, 24,
	 "hindu", "!hindu!"}
};
