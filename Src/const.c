/*********************************************************
 * This code has been completely revamped for the use of *
 * VtR Role Play MU* Requiem by Muse. To use this code   *
 * provide credit in the greeting or motd.               *
 ********************************************************/

/***************************************************************************
 *  God Wars Mud improvements copyright (C) 1994, 1995, 1996 by Richard    *
 *  Woolcock.  This mud is NOT to be copied in whole or in part, or to be  *
 *  run without the permission of Richard Woolcock.  Nobody else has       *
 *  permission to authorise the use of this code.                          *
 ***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"

const int points_distribution[3] = { 8, 7, 6 };

const int abilities_points_distribution[3] = { 11, 7, 4 };

const int ability_insight_levels[MAX_TRAINABLE_ABILITY + 1] = { 0, 1, 3, 6, 9, 18, 30 };

const char *spec_names[MAX_SPECS][25] =
{
  {"cast_adept"},
  {"wildmob"},
  {"lumberjack"}
};


const char *clan_names[MAX_CLANS][25] =
{
{"Nosferatu"},
{"Beast"},
{"Ventrue"},
{"Daeva"},
{"Heaven"},
{"Darkling"},
{"Gangrel"},
{"Mekhet"},
{"Elemental"},
{"Fairest"},
{"Ogre"},
{"Ghoul"},
{"Wizened"},
{"The-Pack"},
{"Akhud"},
{"Clarity"},
{"Lilith"},
{"Glamour"},
{"Irraka"},
{"Ithaeur"},
{"Elodoth"},
{"Cahalith"},
{"Rahu"} };

const char *attrib_names[3][3][25] = {
	{{"Strength"}, {"Dexterity"}, {"Stamina"}},
	{{"Presence"}, {"Manipulation"}, {"Composure"}},
	{{"Resolve"}, {"Intelligence"}, {"Wits"}}
};

const char *ability_names[3][8][25] = {
	{{"Athletics"}, {"Brawl"}, {"Firearms"}, {"Larceny"}, {"Stealth"}, {"Survival"}, {"Weaponry"}, {"Drive"}},
	{{"AnimalKen"}, {"Empathy"}, {"Expression"}, {"Intimidation"}, {"Socialize"}, {"Streetwise"}, {"Persuasion"}, {"Subterfuge"}},
	{{"Academics"}, {"Computer"}, {"Crafts"}, {"Investigation"}, {"Medicine"}, {"Occult"}, {"Politics"}, {"Science"}}
};

const char *ww_breeds[BREED_MAX + 1][25] = { {"Human"}, {"Lupus"}, {"Metis"} };

const char *ww_organizations[ORGANIZATION_MAX + 1][25] = { {"Black Furies"}, {"Bone Gnawers"}, {"Children of Gaia"}, {"Fianna"}, {"Get of Fenris"},
{"Glass Walkers"}, {"Red Talons"}, {"Silent Striders"}, {"Shadow Lords"}, {"Silver Fangs"}, {"Stargazers"}, {"Uktena"}, {"Wendigos"}
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
 // First column = level of drunkenness. v1/v0 = multiplies level of drunkenness. 
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "water",			  "clear",	  {  0, 1, 10 }	},  /*  0 */
    { "beer",			  "amber",	  {  3, 2,  5 }	},
    { "wine",			  "rose",	  {  4, 2,  5 }	},
    { "ale",			  "brown",	  {  3, 2,  5 }	},
    { "stout",		      "dark",	  {  4, 2,  5 }	},

    { "whiskey",		  "golden",	  { 14, 1,  4 }	},  /*  5 */
    { "brandy",		      "red",	  { 12, 1,  8 }	},
    { "rum",              "brownish", { 12, 0,  0 }	},
    { "gin",              "clearish", { 10, 3,  3 }	},
    { "liquor",           "iridescent",  {  9, 4, -8 }	},

    { "milk",			  "white",	  {  0, 3,  6 }	},  /* 10 */
    { "tea",			  "tan",	  {  0, 1,  6 }	},
    { "coffee",			  "black",	  {  0, 1,  6 }	},
    { "blood",			  "red",	  {  0, 0,  5 }	},
    { "salt water",		  "clear",	  {  0, 1, -2 }	},

    { "cola",			  "caramel",	  {  0, 1,  5 }	}   /* 15 */
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
	"remove page",		4,
	spell_remove_page,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(564),	 10,	12,
	"",			"!RemovePage!"
    },
	
    {
	"insert page",		4,
	spell_insert_page,	TAR_IGNORE,		POS_STANDING,
	NULL,			SLOT(553),	 10,	12,
	"",			"!InsertPage!"
    },
	 
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
	"mandarin", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_mandarin, SLOT (0), 0, 24,
	 "mandarin", "!mandarin!"},
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
	 "fae", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_fae, SLOT (0), 0, 24,
	 "fae", "!fae!"},
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
	 "cantonese", 7,
	 spell_null, TAR_IGNORE, POS_STANDING,
	 &gsn_cantonese, SLOT (0), 0, 24,
	 "cantonese", "!cantonese!"},
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

const struct weather_type weather_table[MAX_WEATHER] =
{
   { "There is a cold rain trickling down.",						},
   { "There is a warm rain trickling down.",						},
   { "Giant drops of rain are pouring from the sky.",					},
   { "Ring sized drops of rain are falling from the sky.",				},
   { "Soft warm rain is slowly coming down.",                   			},
   { "Hand sized drops of rain are falling from the sky.",      			},
   { "Boot sized drops of rain are falling from the sky.",      			},
   { "A clear azure sky blankets the land.",                    	        	},
   { "The pristine blue of the sky above spreads, unmarred by a single cloud.", 	},
   { "An overcast sheet of clouds heavily blankets the sky.",   			},
   { "Wisps of clouds delicately curl and drift above.",		    		},
   { "Small pellets of ice fall from the sky and bounce as they hit the ground.",	},
   { "Balls of hail blanket the ground covering the landscape.",			},
   { "A large thunderstorm approaches you from a distance.",   				},
   { "A deep roar of thunder causes the very earth to tremble.",			},
   { "Flashes of light stab and illuminate the sky.",                   		},
   { "Waves of ice fall conintually from the sky.", 					},
   { "Shards of ice mercilessly batter the land.", 					},
   { "Soft flakes of snow drift from above, covering the land in a wash of white.",	},
   { "Large snowflakes swirl and drift, obscuring the nearby landscape.", 		},
   { "Large drifts of snow cover the ground blown out by driving winds.", 		},
   { "Blinding snow drives into the land, carried on howling gusts.",			},
   { "A misty haze covers the horizon.",						},
   { "Rolling fog claims the land clinging to the ground.",				},
   { "A bolt of lightning pierces the sky as a large storm approaches.", 		},
   { "Violent flashes of light rent the dark clouds.",					},
   { "A warm breeze flows through the land blanketing it with heat.",   		},
   { "A cold breeze flows through the land blanketing it with frost.",			},
   { "Gusts of wind sweep across the ground, pushing against any resistance.",		},
   { "Pressing winds push against the land, sighing mournfully with their passage.",	}, 
};   

const struct rain_type rain_table[ MAX_RAIN ] =
{
   { "There is a cold rain trickling down.",  		},
   { "There is a warm rain trickling down.",  		},
   { "Giant drops of rain are pouring from the sky.", 	},
   { "Ring sized drops of rain are falling from the sky.",    },
   { "Soft warm rain is slowly coming down.", 	 	},
   { "Hand sized drops of rain are falling from the sky.",    },
   { "Boot sized drops of rain are falling from the sky.",    }
};


const struct cloud_type cloud_table[ MAX_CLOUD ] =
{
   { "An overcast sheet of clouds heavily blankets the sky.", },
   { "Wisps of clouds delicately curl and drift above.", }
};

const struct cloudy_description cloudy_table[ MAX_CLOUDY ] =
{
   { "A clear azure sky blankets the land.",	},
   { "The pristine blue of the sky above spreads, unmarred by a single cloud.." } 
};

const struct hailstorm_description hailstorm_table[ MAX_HAILSTORM ] =
{
   { "Small pellets of ice fall from the sky and bounce as they hit the ground.",		},
   { "Balls of hail blanket the ground covering the landscape.", }
};

const struct thunderstorm_description thunderstorm_table[ MAX_THUNDERSTORM ] =
{
   { "A large thunderstorm approaches you from a distance.",	},
   { "A deep roar of thunder causes the very earth to tremble.", 	},
   { "Flashes of light stab and illuminate the sky.",               }
};

const struct icestorm_description icestorm_table[ MAX_ICESTORM ] =
{
   { "Waves of ice fall continually from the sky.",	},
   { "Shards of ice mercilessly batter the land.", }
};

const struct snow_description snow_table[ MAX_SNOW ] =
{
   { "Soft flakes of snow drift from above, covering the land in a wash of white.", },
   { "Large snowflakes swirl and drift, obscuring the nearby landscape.", }
};

const struct blizzard_description blizzard_table[ MAX_BLIZZARD ] =
{
   { "Large drifts of snow cover the ground blown out by driving winds.", },
   { "Blinding snow drives into the land, carried on howling gusts." }
};

const struct foggy_description foggy_table[ MAX_FOGGY ] =
{
   { "A misty haze covers the horizon.",	},
   { "Rolling fog claims the land clinging to the ground.", }
};

const struct lightning_description lightning_table[ MAX_LIGHTNING ] =
{
   { "A bolt of lightning pierces the sky as a large storm approaches.",  },
   { "Violent flashes of light rent the dark clouds.",			    }
};

const struct breeze_description breeze_table[ MAX_BREEZE ] =
{
   { "A warm breeze flows through the land blanketing it with heat.",		},
   { "A cold breeze flows through the land blanketing it with frost.",		}
};

const struct wind_description wind_table[ MAX_WIND ] =
{
   { "Gusts of wind sweep across the ground, pushing against any resistance.", },
   { "Pressing winds push against the land, sighing mournfully with their passage.", }
};

const struct noble_titles noble_rank_table[ MAX_NOBLE ] =
{   
    {
         {"Trespasser ",   "Trespasser ",    "Trespasser "   }
    },
    {
         {"Trespasser ",   "Trespasser ",    "Trespasser "   }
    },
    {
         {"Acknowledged Independent "    ,"Acknowledged Independent "    ,"Acknowledged Independent "      }
    },
    {
         {"Social Outcast "     ,"Social Outcast "    ,"Social Outcast "     }
    },
    {
         {"Loyal Subject "     ,"Loyal Subject "    ,"Loyal Subject "     }
    },
    {
         {"Harpy "  ,"Harpy " ,"Harpy "  }
    },
    {
         {"Chamberlain "      ,"Chamberlain "     ,"Chamberlain "      }
    },
    {
         {"Newcomer "  ,"Newcomer " ,"Newcomer "}
    },
    {
         {"Citizen "  ,"Citizen " ,"Citizen "  }
    },
    {
         {"Recruiter "        ,"Recruiter "       ,"Recruiter "        }
    },
    {
         {"Lord of Elysian Valley "      ,"Lord of Elysian Valley "     ,"Lord of Elysian Valley "      }
    },
    {
         {"Lord of Lincoln Heights "      ,"Lord of Lincoln Heights "     , "Lady of Lincoln Heights "     }
    },
    {
         { "Lord of the Pacific Palisades " ,"Lord of the Pacific Palisades " ,"Lady of the Pacific Palisades " }                
    }, 
    {
         { "Lord Knight " ,"Lord Knight " ,"Lady Knight " }                
    },  
    {
         { "Lord Seneschal " ,"Lord Seneschal " ,"Lady Seneschal "  }                
    },   
    {
         { "Senatorial Aide " ,"Senatorial Aide " ,"Senatorial Aide "  }                
    },   
    {
         { "Senator of South Central " ,"Senator of South Central " ,"Senator of South Central "  }                
    },
    {
         { "Senator of Santa Monica " ,"Senator of Santa Monica " ,"Senator of Santa Monica "  }                
    },
    {
         { "Senator of Hollywood " ,"Senator of Hollywood " ,"Senator of Hollywood "  }                
    },
    {
         { "King of Los Angeles " ,"King of Los Angeles " ,"Queen of Los Angeles "  }                
    },
    {
         { "Head of State " ,"Head of State " ,"Head of State "  }                
    },
};
