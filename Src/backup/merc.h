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
 * Anyone may use it if they wish                           *
 * We ask only for credit in the login or MOTD              *
 ************************************************************/
 
 /***********************************************************************
 * This code has been stolen and hacked to shit by yet another coder    *
 * for use in A Streetlight Manifesto.                                  *
 *                                                                      *
 * To use it, I ask only that you scar                                  *
 * "KURT HAS A HUGE PENIS" into your chest with a knife;                * 
 * preferably an African one. :3                                        *
 ************************************************************************/
 
#include <malloc.h>
#define STATUS_LOOKUP       0   // New Descriptor, in lookup by default.
#define STATUS_DONE         1   // The lookup is done.
#define STATUS_WAIT         2   // Closed while in thread.
#define STATUS_CLOSED       3   // Closed, ready to be recycled.


/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )			( )
#define DECLARE_DO_FUN( fun )		void fun( )
#define DECLARE_SPEC_FUN( fun )		bool fun( )
#define DECLARE_SPELL_FUN( fun )	void fun( )
#else
#define args( list )			list
#define DECLARE_DO_FUN( fun )		DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )		SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )	SPELL_FUN fun
#endif

#define LINKCOUNT   15 //Used to be in build.h

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if	!defined(FALSE)
#define FALSE	 0
#endif

#if	!defined(TRUE)
#define TRUE	 1
#endif

#if	defined(_AIX)
#if	!defined(const)
#define const
#endif
typedef int sh_int;
typedef int bool;
#define unix
#else
typedef short int sh_int;
typedef unsigned char bool;
#endif



/*
 * Structure types.
 */
typedef struct  dummy_arg               DUMMY_ARG;
typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct ban_data BAN_DATA;
typedef struct immlist_data IMMLIST_DATA;
typedef struct report_data REPORT_DATA;
typedef struct char_data CHAR_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct note_data NOTE_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct extra_data EXTRA;
typedef struct reset_data RESET_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct weather_data WEATHER_DATA;
typedef struct editing_data EDITING_DATA;	/* OLC */
typedef struct mob_prog_data MPROG_DATA;
typedef struct mob_prog_act_list MPROG_ACT_LIST;
typedef struct mob_prog_resets MPROG_RESETS;

/*
 * Account data
 */
typedef struct eban_data EBAN_DATA;
typedef struct player_info PLAYER_INFO;


/*
 * Function types.
 */
typedef void DO_FUN args ((CHAR_DATA * ch, char *argument));
typedef bool SPEC_FUN args ((CHAR_DATA * ch));
typedef void SPELL_FUN args ((int sn, int level, CHAR_DATA * ch, void *vo));

/*
 * Recommend system.
 */
 typedef struct pose_recc POSE_RECC;
 typedef struct renown_recc RENOWN_RECC;

/*
 * New version stuff.
 */
#define MUD_NAME 	"A Streetlight Manifesto"
#define DEFAULT_PORT 9002
#define RELEASE		"Beta"
#define COMPILE_TIME	"Compiled on "	__DATE__ " at " __TIME__ ".\n\r"

const char *versionToString	args ( ( int version ) );

/*
 * String and memory management parameters.
 */
#define	MAX_KEY_HASH		 4096
#define MSL MAX_STRING_LENGTH
#define MAX_INPUT_LENGTH	 3000
#define MAX_STRING_LENGTH	 8192

#define MAX_ROOMDESC_LENGTH  100
#define MAX_VOICE_LENGTH     100
#define MAX_AURA_LENGTH      100

/*
 * Gobal Procedures
 */
void clear_stats args ((CHAR_DATA * ch));
void room_is_total_darkness args ((ROOM_INDEX_DATA * pRoomIndex));
void raw_kill args ((CHAR_DATA * victim, CHAR_DATA * ch));
void make_corpse args ((CHAR_DATA * ch, CHAR_DATA * maker));


#define OPTIMISED_PFILES            1	/* Set to 0 for full length pfiles */


#define MAX_SPECS               3

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */

#define MAX_SKILL		  201
#define MAX_LEVEL		   12
#define LEVEL_HERO		   (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 6)
#define LEVEL_STORYTELLER	   (MAX_LEVEL - 6)
#define LEVEL_SHOPKEEPER	   (MAX_LEVEL - 7)
#define LEVEL_MORTAL		   (MAX_LEVEL - 10)
#define LEVEL_AVATAR		   (MAX_LEVEL - 9)
#define LEVEL_APPRENTICE	   (MAX_LEVEL - 8)
#define LEVEL_MAGE		   (MAX_LEVEL - 7)

#define LEVEL_BUILDER		   (MAX_LEVEL - 5)
#define LEVEL_QUESTMAKER	   (MAX_LEVEL - 4)
#define LEVEL_ENFORCER		   (MAX_LEVEL - 3)
#define LEVEL_JUDGE		   (MAX_LEVEL - 2)
#define LEVEL_HIGHJUDGE		   (MAX_LEVEL - 1)
#define LEVEL_IMPLEMENTOR	   (MAX_LEVEL)

#define PULSE_PER_SECOND	    4
#define PULSE_GAIN_EXP		  ( 1 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE		  ( 1 )
#define PULSE_MOBILE		  ( 4 * PULSE_PER_SECOND)
#define PULSE_TICK		  (240 * PULSE_PER_SECOND)
#define PULSE_AREA		  (240 * PULSE_PER_SECOND)
#define PULSE_DAY		  (10800 * PULSE_PER_SECOND)
#define PULSE_WEEK		  (75600 * PULSE_PER_SECOND)


/*
 * Rotains Clan Table Structure
 */

struct dummy_arg
{
  DUMMY_ARG        *next;
  DESCRIPTOR_DATA  *d;
  char             *buf;
  sh_int           status;
};


#include "board.h"

/*
 * Site ban structure.
 */
struct ban_data
{
	BAN_DATA *next;
	char *name;
};

/*
 * E-mail ban structure.
 */
struct eban_data
{
	EBAN_DATA * next;
	char * name;
};

/*
 * immlist data structure.
 */
struct immlist_data
{
	IMMLIST_DATA *next;
	char *name;
	int level;
};

struct report_data
{
	REPORT_DATA *next;
	char *name;
	int value[4];
};



/*
 * Store types of char logged in.
 */
#define LOGIN_NONE		    0
#define LOGIN_NEWBIE		    1
#define LOGIN_MORTAL		    2
#define LOGIN_GOD		    4
#define LOGIN_VAMPIRE		    8
#define LOGIN_WEREWOLF		   16


/*
 * Time and weather stuff.
 */
#define SKY_CLOUDLESS		    0
#define SKY_CLOUDY		    1
#define SKY_RAINING		    2
#define SKY_LIGHTNING		    3
#define SKY_HEAVY_RAIN		    4
#define SKY_DRIZZLE		    5
#define SKY_MOSTLY_CLOUDY	    6
#define SKY_PARTLY_CLOUDY           7
#define SKY_SPARSE_CLOUDS           8

struct time_info_data
{
	int hour;
	int day;
	int month;
	int year;
};
struct weather_data
{
	int mmhg;
	int change;
	int sky;
	int sunlight;
	int time_of_day;
};
/*
struct weather_data
{
        char * time;
        int wind_speed;
        char * wind_dir;
        char * clouds;
        int cloud_height;
        int temperature;
        char * weathercond;
};
*/
/*
 * Connected state for a channel.
 */
#define CON_PLAYING			 0	/* IN: MOTD, OUT: BEGIN PLAY */
#define CON_GET_NAME			 1	/* IN: NONE, OUT: Enter name */
#define CON_GET_OLD_PASSWORD		 2	/* IN: Name, OUT: Show MOTD */
#define CON_CONFIRM_NEW_NAME		 3	/* IN: Sure, OUT: Password */
#define CON_GET_NEW_PASSWORD		 4	/* IN: Pass, OUT: Confirm */
#define CON_CONFIRM_NEW_PASSWORD	 5	/* IN: Pass, OUT: What sex */
#define CON_GET_NEW_SEX			 6	/* IN: Sex,  OUT: Start as vamp */
#define CON_GET_PRIMARY_STAT_AREA	7
#define CON_GET_SECONDARY_STAT_AREA	8
#define CON_GET_PRIMARY_STATS		9
#define CON_GET_SECONDARY_STATS		10
#define CON_GET_TERTIARY_STATS		11
#define CON_CONFIRM_STATS		12
#define CON_GET_PRIMARY_ABILITY_AREA	 13
#define CON_GET_SECONDARY_ABILITY_AREA	 14
#define CON_GET_ABILITIES		 15
#define CON_READ_MOTD			 16	/* IN: NONE, OUT: Play */
#define CON_COPYOVER_RECOVER		 17
#define CON_GET_NEW_ANSI                 18
#define CON_WIZ_INVIS		 24
#define CON_CREATEVAMPIRE		 25
#define CON_NOT_PLAYING			26
#define CON_GET_EMAIL		27
#define CON_GET_NEW_EMAIL	28
#define CON_MAIN_MENU		29
#define CON_DOCUMENT_MENU	30
#define CON_ACCEPT_DISCLAIMER	31

/*
 * Connected states for OLC.
 */
#define ED_NONE			       0
#define ED_AEDITOR                     1
#define ED_REDITOR                     2
#define ED_MEDITOR                     3
#define ED_OEDITOR                     4
#define ED_HEDITOR		       5
#define ED_MPEDITOR		       6
#define ED_SMPEDITOR		       7

/*
 * Online editing structure.
 */
struct editing_data		/* OLC */
{
	void *pIndex;
	int vnum;
	int type;
	int command;
	char *text;
	char **destination;
};

/*
 * Area flags. OLC
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1	/* Area has been modified. */
#define         AREA_ADDED      2	/* Area has been added to. */
#define         AREA_LOADING    4	/* Used for counting in db.c */


/*
 * Descriptor (channel) structure.
 */
struct descriptor_data
{
	DESCRIPTOR_DATA *next;
	DESCRIPTOR_DATA *snoop_by;
	CHAR_DATA *character;
	CHAR_DATA *original;
	char *host;
	sh_int descriptor;
	sh_int connected;
	bool fcommand;
	char inbuf[4 * MAX_INPUT_LENGTH];
	char incomm[MAX_INPUT_LENGTH];
	char inlast[MAX_INPUT_LENGTH];
	int repeat;
	char *outbuf;
	int outsize;
	int outtop;
	EDITING_DATA editing;	/* OLC */
	void *pEdit;		/* OLC */
	void *pEdit2;		/* OLC */
	char **pString;		/* OLC */
	char *tempString;	/* OLC */
	int editor;
	bool                 mxp;
	sh_int	lookup_status;
	char *		showstr_head;
	char *		showstr_point;
};


/*
 * TO types for act.
 */
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3
#define TO_MORTS        4



/*
 * Help table types.
 */
struct help_data
{
	HELP_DATA *next;
	sh_int level;
	char *keyword;
	char *text;
};

	

/*
 * Shop types.
 */
#define MAX_TRADE	 5

struct shop_data
{
	SHOP_DATA *next;	/* Next shop in list            */
	sh_int keeper;		/* Vnum of shop keeper mob      */
	sh_int buy_type[MAX_TRADE];	/* Item types shop will buy     */
	sh_int profit_buy;	/* Cost multiplier for buying   */
	sh_int profit_sell;	/* Cost multiplier for selling  */
	sh_int open_hour;	/* First opening hour           */
	sh_int close_hour;	/* First closing hour           */
};

struct note_data
{
	NOTE_DATA *next;
	BOARD_DATA *board;
	char *sender;
	char *date;
	char *to_list;
	char *subject;
	char *text;
	time_t date_stamp;
	time_t expire;
};

/*
 * An affect.
 */
struct affect_data
{
	AFFECT_DATA *next;
	sh_int type;
	sh_int duration;
	sh_int location;
	sh_int modifier;
	int bitvector;
};


/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_CITYGUARD	   3060
#define MOB_VNUM_VAMPIRE	   3404


/*
 * Classes.  KaVir.
 */
#define CLASS_NONE	      	   0
#define CLASS_VAMPIRE	      	   1
#define CLASS_WEREWOLF	      	   2
#define CLASS_INQUISITOR      	   4
#define CLASS_SKINDANCER      	   8
#define CLASS_GHOUL	           16


/*
 * Immunities, for players.  KaVir.
 */
#define IMM_XXXX	   1024	/* Immune to being summoned.            */
#define IMM_VAMPIRE	   4096	/* Allow yourself to become a vampire.  */
#define IMM_STEAL	1048576	/* Cannot have stuff stolen.            */
#define IMM_XXXXXXXXXX  16777216	/* Objects can't be transported to you. */


/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC		      1	/* Auto set for mobs    */
#define ACT_SENTINEL		      2	/* Stays in one room    */
#define ACT_SCAVENGER		      4	/* Picks up objects     */
#define ACT_AGGRESSIVE		     32	/* Attacks PC's         */
#define ACT_STAY_AREA		     64	/* Won't leave area     */
#define ACT_WIMPY		    128	/* Flees when hurt      */
#define ACT_PET			    256	/* Auto set for pets    */
#define ACT_TRAIN		    512	/* Can train PC's       */
#define ACT_PRACTICE		   1024	/* Can practice PC's    */
#define ACT_MOUNT		   2048	/* Can be mounted       */
#define ACT_NOPARTS		   4096	/* Dead = no body parts */
#define ACT_NOEXP		   8192	/* No exp for killing   */
#define ACT_DOG			  16384	/* Is a dog */
#define ACT_SKILLED		  32768	/* Stances */
#define ACT_COMMANDED		  65536	/* Can't attack newbies */
#define ACT_TINY		 131072
#define ACT_ANIMAL		 (ACT_TINY * 2)	//is an animal
#define ACT_NOQUIT		 (ACT_ANIMAL * 2)
#define ACT_MAX                  ACT_NOQUIT
/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND		      1
#define AFF_INVISIBLE		      2
#define AFF_DETECT_EVIL		      4
#define AFF_DETECT_INVIS	      8
#define AFF_DETECT_MAGIC	     16
#define AFF_DETECT_HIDDEN	     32
#define AFF_SHADOWPLANE		     64	/* Creatures in shadow plane - KaVir */
#define AFF_SANCTUARY		    128
#define AFF_FAERIE_FIRE		    256
#define AFF_INFRARED		    512
#define AFF_CURSE		   1024
#define AFF_FLAMING		   2048	/* For burning creatures - KaVir */
#define AFF_POISON		   4096
#define AFF_PROTECT		   8192
#define AFF_ETHEREAL		  16384	/* For ethereal creatures - KaVir */
#define AFF_SNEAK		  32768
#define AFF_HIDE		  65536
#define AFF_SLEEP		 131072
#define AFF_CHARM		 262144
#define AFF_FLYING		 524288
#define AFF_PASS_DOOR		1048576
#define AFF_POLYMORPH		2097152	/* For polymorphed creatures - KaVir */
#define AFF_SHADOWSIGHT		4194304	/* Can see between planes - KaVir */
#define AFF_WEBBED		8388608	/* Cannot move - KaVir */
#define AFF_CONTRACEPTION      16777216	/* Cannot get pregnant - KaVir */
#define AFF_SHADOW             33554432	/* For Arms of the Abyss - KaVir */
#define AFF_ROT                67108864	/* For Putrefaction - KaVir */
#define AFF_FEAR	      134217728	/* Fear affects - KaVir */
#define AFF_JAIL	      268435456	/* For Jail of Water - KaVir */
#define AFF_MAX               AFF_JAIL 


/* Thaum paths */
#define PATH_FLAME                    0	// Lure of Flames
#define PATH_WEATHER                  1	// Koldunic  path
#define PATH_CONJURING                2	// Thaum  path
#define PATH_WATER                    3	// Koldunic  path
#define PATH_SPIRIT                   4	// Koldunic  path
#define PATH_ELEMENTAL                5	// Thaum  path
#define PATH_BLOOD                    6	// Thaum path
#define PATH_DESTRUCTION                7	// Thaum  path
#define PATH_MORPHEUS              8	// Thaum  path
#define PATH_HEARTH                   9	// Thaum  path
#define PATH_BLOODCURSE            10	// Thaum  path
#define PATH_MARS                        11	// Thaum  path
#define PATH_DIONYSUS              12	// Thaum  path
#define PATH_EARTH                    13	// Koldunic  path
#define PATH_WIND                        14	// Koldunic  path
#define PATH_FIRE                        15	// Koldunic  path
#define PATH_MSPIRIT                        16	// Thaum  path
#define PATH_MAX_ALLOWED             19


// Thaum Paths
// Normal Thaum Paths
#define PATH_REGO_VITAE		0	
#define PATH_CREO_IGNEM		1
#define PATH_REGO_MOTUS		2
#define PATH_REGO_TEMPESTAS	3
#define PATH_REGO_AQUAM		4
#define PATH_REGO_ELEMENTUM	5
#define PATH_ALCHEMY		6
#define PATH_LEVINBOLT		7
#define PATH_SOULS_FEAR         8
#define PATH_WARDING		9
// Dark Thaum Paths
#define PATH_REGO_VENALIS		10	// Corruption
#define PATH_NEFAS		11	// Evil
#define PATH_REGO_DOLOR		12	// Pain
#define PATH_REGO_MANES		13	// Spirit
// Koldun Paths
#define PATH_AIR		16

#define MAX_THAUM_PATHS		18


// Types of Thaum

#define THAUM_TYPE_GENERAL	1
#define THAUM_TYPE_DARK		2
#define THAUM_TYPE_KOLDUN	4


/*
 * Bits for 'itemaffect'.
 * Used in #MOBILES.
 */
#define ITEMA_SHOCKSHIELD	      1
#define ITEMA_FIRESHIELD	      2
#define ITEMA_ICESHIELD		      4
#define ITEMA_ACIDSHIELD	      8
#define ITEMA_DBPASS                 16
#define ITEMA_CHAOSSHIELD            32
#define ITEMA_ARTIFACT               64
#define ITEMA_REGENERATE            128
#define ITEMA_SPEED                 256
#define ITEMA_VORPAL                512
#define ITEMA_PEACE                1024
#define ITEMA_RIGHT_SILVER         2048
#define ITEMA_LEFT_SILVER          4096
#define ITEMA_REFLECT              8192
#define ITEMA_RESISTANCE          16384
#define ITEMA_VISION              32768
#define ITEMA_STALKER             65536
#define ITEMA_VANISH             131072
#define ITEMA_RAGER              262144
#define ITEMA_HIGHLANDER         524288
#define ITEMA_SHADES            1048576
#define ITEMA_SUNSHIELD         2097152
#define ITEMA_RIGHT_AGG		4194304
#define ITEMA_LEFT_AGG		8388608

/*
 * Rune, Glyph and Sigil bits.
 */
#define RUNE_FIRE	      1
#define RUNE_AIR	      2
#define RUNE_EARTH	      4
#define RUNE_WATER	      8
#define RUNE_DARK	      16
#define RUNE_LIGHT	      32
#define RUNE_LIFE	      64
#define RUNE_DEATH	      128
#define RUNE_MIND	      256
#define RUNE_SPIRIT	      512
#define RUNE_MASTER	      1024
#define GLYPH_CREATION	      1
#define GLYPH_DESTRUCTION     2
#define GLYPH_SUMMONING	      4
#define GLYPH_TRANSFORMATION  8
#define GLYPH_TRANSPORTATION  16
#define GLYPH_ENHANCEMENT     32
#define GLYPH_REDUCTION	      64
#define GLYPH_CONTROL	      128
#define GLYPH_PROTECTION      256
#define GLYPH_INFORMATION     512
#define SIGIL_SELF	      1
#define SIGIL_TARGETING       2
#define SIGIL_AREA	      4
#define SIGIL_OBJECT	      8


/* Colour scale macros - added 12th Aug 1995 by Calamar */

#define NO_COLOUR	""	/* Blank */
#define GREY		"[0;1;30m"	/* Dark Grey */
#define D_RED		"[0;0;31m"	/* Dark Red */
#define L_RED		"[0;1;31m"	/* Light Red */
#define D_GREEN		"[0;0;32m"	/* Dark Green */
#define L_GREEN		"[0;1;32m"	/* Light Green */
#define BROWN		"[0;0;33m"	/* Brown */
#define YELLOW		"[0;1;33m"	/* Yellow */
#define D_BLUE		"[0;0;34m"	/* Dark Blue */
#define L_BLUE		"[0;1;34m"	/* Light Blue */
#define MAGENTA		"[0;0;35m"	/* Magenta */
#define PINK		"[0;1;35m"	/* Pink */
#define D_CYAN		"[0;0;36m"	/* Dark Cyan */
#define L_CYAN		"[0;1;36m"	/* Light Cyan */
#define NORMAL		"[0;0;37m"	/* Light Grey */
#define WHITE		"[0;0;38m"	/* White */

#define ADD_COLOUR(_player,_str,_col) { int swh_len = strlen(_str) + 20; \
					char swh_temp[swh_len]; \
if (!IS_NPC(_player) && IS_SET(_player->act, PLR_ANSI)) {swh_temp[0] = '\0'; \
strcpy(swh_temp, _col); strcat(swh_temp, _str); strcat(swh_temp, NORMAL); \
strcpy(_str, swh_temp);}}

#define SCALE_COLS 4

#define COL_SCALE(_swh_str,_swh_ch,_swh_curr,_swh_max) \
	ADD_COLOUR(_swh_ch, _swh_str, \
	(_swh_curr < 1) ? L_RED : \
	(_swh_curr < _swh_max) ? \
	scale[(SCALE_COLS * _swh_curr) / ((_swh_max > 0) \
	? _swh_max : 1)] : L_CYAN)

extern char *scale[SCALE_COLS];

extern CHAR_DATA Score_ch;	/* Score_ch */

/*
 * File bits.
 */
#define FILE_PLAYER		      0
#define FILE_PLAYER_BAK			1
/*
 * Bits for 'vampire'.
 * Used for player vampires.
 */
#define VAM_FANGS		      1	/* Fangs currently extended */
#define VAM_CLAWS		      2	/* Protean level 2, claws extended */
#define VAM_NIGHTSIGHT		      4	/* Protean level 1, nightsight on */
#define VAM_FLYING		      8	/* Protean bat form */
#define VAM_SONIC		     16	/* Protean bat form */
#define VAM_CHANGED		     32	/* Protean powers level 4 and 5 */
#define VAM_SPIRITEYES		     64	/* Path of Spirit level 2 */
#define VAM_OBFUSCATE		    128	/* Concealed from sight */

#define VAM_DISGUISED		  16384	/* For the Obfuscate disguise ability */

#define VAM_EVILEYE		 131072	/* Path of Spirit level 1 */

#define VAM_EARTHMELDED		 524288	/* Protean level 3, in the ground */


/*
 * Bits for disciplines.
 * Used for player vampires.
 */

#define RES_ANIMALISM		      1
#define RES_AUSPEX		      2
#define RES_DOMINATE		      4
#define RES_PRESENCE		      8
#define RES_TOGGLE		     16
#define RES_CELERITY		     32

#define WILL_ANIMALISM		      1
#define WILL_AUSPEX		      2
#define WILL_DOMINATE		      3
#define WILL_PRESENCE		      4

#define CLAN_FIRST		      0
#define CLAN_SECOND		      1
#define CLAN_THIRD		      2

#define DISC_ANIMALISM		      0
#define DISC_AUSPEX		      1
#define DISC_CELERITY		      2
#define DISC_CHIMERSTRY		      3
#define DISC_DAIMOINON		      4
#define DISC_DOMINATE		      5
#define DISC_FORTITUDE		      6
#define DISC_DEMENTATION	      7
#define DISC_NECROMANCY		      8
#define DISC_OBEAH		      9
#define DISC_OBFUSCATE		     10
#define DISC_OBTENEBRATION	     11
#define DISC_POTENCE		     12
#define DISC_PRESENCE		     13
#define DISC_PROTEAN		     14
#define DISC_QUIETUS		     15
#define DISC_SERPENTIS		     16
#define DISC_MORTIS		     17
#define DISC_THAUMATURGY	     18
#define DISC_VICISSITUDE	     19
#define DISC_ABOMBWE		     20
#define DISC_TEMPORIS	     	     21	// True Brujah
#define DISC_VALEREN 	     	     22	// Salubri Antitribu
#define DISC_DEIMOS        	     23	// Lamina,cappadocian bloodline
#define DISC_GROTESQUOUS   	     24	// Nictuku, Nossie Bloodline
#define DISC_NIHILISTICS   	     25	// Nagaraja, Euthantosis
#define DISC_MA   	     	     26	//Anda, Gangrel Bloodline
#define DISC_BARDO   	     	     27	// Children of Osiris,
#define DISC_MALEFICIA     	     28	// Infernalists
#define DISC_MYTHERCIA     	     29	// Kiasyd (lasombra fae bloodline)
#define DISC_SPIRITUS   	     30	// Arhimanes (gangrel bloodline)
#define DISC_STIGA   	     	     31	// Infernalists
#define DISC_MELPOMINEE   	     32	// Infernalists
#define DISC_THANATOSIS   	     33	// Infernalists
#define DISC_VISCERATIKA	     34 // Gargoyles
#define DISC_SOPHIA_PATRIS     	     35 // Inquisitors
#define DISC_SEDULITAS_FILIUS  	     36 // Inquisitors
#define DISC_BONI_SPIRITUS     	     37 // Inquisitors
#define DISC_MYTHERCERIA             38 // Kiasyd
#define DISC_MAX		     39


#define BREED_HOMID		      0
#define BREED_LUPUS		      1
#define BREED_METIS		      2
#define BREED_MAX		      2


#define AUSPICE_KAMSA		      0
#define AUSPICE_KAMAKSHI		      1
#define AUSPICE_KALI		      2
#define AUSPICE_KARTIKEYA		      3

#define TRIBE_COBRA		      0
#define TRIBE_COPPERHEAD		      1
#define TRIBE_ADDER		      2
#define TRIBE_RATTLESNAKE		      3


#define AUSPICE_RAGABASH	      0
#define AUSPICE_THEURGE		      1
#define AUSPICE_PHILODOX	      2
#define AUSPICE_GALLIARD	      3
#define AUSPICE_AHROUN		      4
#define AUSPICE_MAX				  4


#define TRIBE_BLACK_FURIES	      0
#define TRIBE_BONE_GNAWERS	      1
#define TRIBE_CHILDREN_OF_GAIA	  2
#define TRIBE_FIANNA		      3
#define TRIBE_GET_OF_FENRIS	      4
#define TRIBE_GLASS_WALKERS	      5
#define TRIBE_RED_TALONS	      6
#define TRIBE_SHADOW_LORDS	      7
#define TRIBE_SILENT_STRIDERS	  8
#define TRIBE_SILVER_FANGS	      9
#define TRIBE_STARGAZERS	      10
#define TRIBE_UKTENA		      11
#define TRIBE_WENDIGOS		      12
#define TRIBE_MAX				  12



#define FACTION_MYRIDON		      0
#define FACTION_VISKR		      1
#define FACTION_WYRSTA		      2

#define ASPECT_TENERE		      0
#define ASPECT_HATAR		      1
#define ASPECT_KUMOTI		      2



/*
 * World affect bits.
 */
#define WORLD_FOG		      1
#define WORLD_RAIN		      2
#define WORLD_LIGHTNING		      3
#define WORLD_NICE		      8	/* No swearing while frenzied */


/*
 * Bits for blood.
 * Used for player vampires.
 */
#define BLOOD_CURRENT		      0
#define BLOOD_POOL		      1
#define BLOOD_POTENCY		      2


/*
 * Bits for werewolf forms.
 */
#define FORM_HOMID		      1
#define FORM_GLABRO		      2
#define FORM_CRINOS		      3
#define FORM_HISPO		      4
#define FORM_LUPUS		      5



/*
 * Bits for 'polymorph'.
 * Used for players.

 */
#define POLY_BAT		      1
#define POLY_WOLF		      2
#define POLY_MIST		      4
#define POLY_SERPENT		      8
#define POLY_RAVEN		     16
#define POLY_FISH		     32
#define POLY_FROG		     64
#define POLY_ZULO		    128
#define POLY_SHADOW		    256
#define POLY_SPIRIT		    512
#define POLY_MASK		   1024	/* Vicissitude 3 */
#define POLY_TRUE		   2048	/* Chimerstry 6 */
#define POLY_HAGS		   4096	/* Thanatosis 1 */
#define POLY_CHANGELING		   8192	/* Vicissitude 1 */
#define POLY_FLESHCRAFT		  16384	/* Vicissitude 2 */
#define POLY_BONECRAFT		  32768	/* Vicissitude 3 */
#define POLY_WILDEBEAST		  (POLY_BONECRAFT * 2)	/* Abombwe */



/*
 * Languages.
 */
#define DIA_OLDE		      1
#define LANG_ENGLISH          8
#define LANG_GERMAN		      	16
#define LANG_VIETNAMESE		      	32
#define LANG_RUSSIAN		      	64
#define LANG_ARABIC		      	128
#define LANG_FRENCH		      	256
#define LANG_LATIN		      	512
#define LANG_SPANISH		      	1024
#define LANG_ITALIAN	      	2048
#define LANG_HEBREW         (LANG_ITALIAN * 2)
#define LANG_GAELIC         (LANG_HEBREW * 2)
#define LANG_INDONESIAN        (LANG_GAELIC * 2)
#define LANG_GREEK	        (LANG_INDONESIAN * 2)
#define LANG_SLAVIC         ( LANG_GREEK * 2)
#define LANG_GAROU         ( LANG_SLAVIC * 2)
#define LANG_KOREAN         ( LANG_GAROU * 2)
#define LANG_CHINESE	  ( LANG_KOREAN * 2)
#define LANG_SWAHILI      ( LANG_CHINESE * 2)
#define LANG_TAGALOG      ( LANG_SWAHILI * 2)
#define LANG_HINDU        ( LANG_TAGALOG * 2)
#define LANG_JAPANESE     ( LANG_HINDU * 2)

#define MAX_LANGUAGE   23	/* Number of languages, + 1 */

/*
 * Language stuff
 */

#define ENGLISH       0
#define GERMAN        1
#define FRENCH        2
#define CHINESE       3
#define RUSSIAN       4
#define ARABIC        5
#define LATIN         6
#define SPANISH       7
#define ITALIAN       8
#define HEBREW        9
#define GAELIC        10
#define TAGALOG      11
#define GREEK         12
#define SLAVIC        13
#define GAROU         14
#define JAPANESE      15
#define NORSE	      16
#define INDONESIAN    17
#define KOREAN        18
#define VIETNAMESE    19
#define SWAHILI       20
#define HINDU         21
/*
 * Score.
 */
#define SCORE_TOTAL_XP		      0
#define SCORE_TOTAL_LEVEL	      2



/*
 * Mounts
 */
#define IS_ON_FOOT		      0
#define IS_MOUNT		      1
#define IS_RIDING		      2
#define IS_CARRIED		      4
#define IS_CARRYING		      8



/*
 * Embrace positions.
 */
#define ARE_NONE		      0
#define ARE_EMBRACING		      1
#define ARE_EMBRACED		      2



/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		      0
#define SEX_MALE		      1
#define SEX_FEMALE		      2



/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MONEY_ONE	      2
#define OBJ_VNUM_MONEY_SOME	      3

#define OBJ_VNUM_CORPSE_NPC	     10
#define OBJ_VNUM_CORPSE_PC	     11
#define OBJ_VNUM_SEVERED_HEAD	     12
#define OBJ_VNUM_TORN_HEART	     13
#define OBJ_VNUM_SLICED_ARM	     14
#define OBJ_VNUM_SLICED_LEG	     15
#define OBJ_VNUM_FINAL_TURD	     16

#define OBJ_VNUM_MUSHROOM	     20
#define OBJ_VNUM_LIGHT_BALL	     21
#define OBJ_VNUM_SPRING		     22
#define OBJ_VNUM_BLOOD_SPRING	     23

#define OBJ_VNUM_SCHOOL_MACE	   3700
#define OBJ_VNUM_SCHOOL_DAGGER	   3701
#define OBJ_VNUM_SCHOOL_SWORD	   3702
#define OBJ_VNUM_SCHOOL_VEST	   3703
#define OBJ_VNUM_SCHOOL_SHIELD	   3704
#define OBJ_VNUM_SCHOOL_BANNER     3716

/* For KaVir's stuff */
#define OBJ_VNUM_SOULBLADE	  30000
#define OBJ_VNUM_PORTAL		  30001
#define OBJ_VNUM_EGG		  30002
#define OBJ_VNUM_EMPTY_EGG	  30003
#define OBJ_VNUM_SPILLED_ENTRAILS 30004
#define OBJ_VNUM_QUIVERING_BRAIN  30005
#define OBJ_VNUM_SQUIDGY_EYEBALL  30006
#define OBJ_VNUM_SPILT_BLOOD      30007
#define OBJ_VNUM_VOODOO_DOLL      30010
#define OBJ_VNUM_RIPPED_FACE      30012
#define OBJ_VNUM_TORN_WINDPIPE    30013
#define OBJ_VNUM_CRACKED_HEAD     30014
#define OBJ_VNUM_SLICED_EAR	  30025
#define OBJ_VNUM_SLICED_NOSE	  30026
#define OBJ_VNUM_KNOCKED_TOOTH	  30027
#define OBJ_VNUM_TORN_TONGUE	  30028
#define OBJ_VNUM_SEVERED_HAND	  30029
#define OBJ_VNUM_SEVERED_FOOT	  30030
#define OBJ_VNUM_SEVERED_THUMB	  30031
#define OBJ_VNUM_SEVERED_INDEX	  30032
#define OBJ_VNUM_SEVERED_MIDDLE	  30033
#define OBJ_VNUM_SEVERED_RING	  30034
#define OBJ_VNUM_SEVERED_LITTLE	  30035
#define OBJ_VNUM_SEVERED_TOE	  30036
#define OBJ_VNUM_PROTOPLASM	  30037
#define OBJ_VNUM_QUESTCARD	  30039
#define OBJ_VNUM_QUESTMACHINE	  30040
#define OBJ_VNUM_ASHES            30042
#define OBJ_VNUM_DAEMON_CONTRACT  30043
#define MOB_VNUM_HOUND		  30000
#define MOB_VNUM_GUARDIAN	  30001
#define MOB_VNUM_MOUNT		  30006
#define MOB_VNUM_FROG		  30007
#define MOB_VNUM_RAVEN		  30008
#define MOB_VNUM_CAT		  30009
#define MOB_VNUM_DOG		  30010
#define MOB_VNUM_EYE		  30012
#define MOB_VNUM_IEYE		  30012
#define MOB_VNUM_MOLERAT	  30013
#define MOB_VNUM_YAK		  30014
#define MOB_VNUM_ASTRAL		  30015
#define MOB_VNUM_CLONE		  30016



/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_PORTAL		     27
#define ITEM_EGG		     28
#define ITEM_VOODOO		     29
#define ITEM_STAKE		     30
#define ITEM_MISSILE		     31	/* Ammo vnum, cur, max, type */
#define ITEM_AMMO		     32	/* ???, dam min, dam max, type */
#define ITEM_QUEST		     33
#define ITEM_QUESTCARD		     34
#define ITEM_QUESTMACHINE	     35
#define ITEM_SYMBOL		     36
#define ITEM_BOOK		     37
#define ITEM_PAGE		     38
#define ITEM_TOOL		     39
#define ITEM_WALL		     40	/* For wall of water */
#define ITEM_WARD		     41	/* For ritual wards */
#define ITEM_RESOURCE		     42
#define ITEM_CREATURE		     43
#define ITEM_SKIN		     43
#define ITEM_CONTRACT		     44


/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW		      1
#define ITEM_HUM		      2
#define ITEM_THROWN		      4
#define ITEM_KEEP		      8
#define ITEM_VANISH		     16	/* Illusion created by Chimerstry */
#define ITEM_INVIS		     32
#define ITEM_MAGIC		     64
#define ITEM_NODROP		    128
#define ITEM_BLESS		    256
#define ITEM_ANTI_GOOD		    512
#define ITEM_ANTI_EVIL		   1024
#define ITEM_ANTI_NEUTRAL	   2048
#define ITEM_NOREMOVE		   4096
#define ITEM_INVENTORY		   8192
#define ITEM_LOYAL		  16384
#define ITEM_SHADOWPLANE	  32768



/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE		      1
#define ITEM_WEAR_FINGER	      2
#define ITEM_WEAR_NECK		      4
#define ITEM_WEAR_BODY		      8
#define ITEM_WEAR_HEAD		     16
#define ITEM_WEAR_LEGS		     32
#define ITEM_WEAR_FEET		     64
#define ITEM_WEAR_HANDS		    128
#define ITEM_WEAR_ARMS		    256
#define ITEM_WEAR_SHIELD	    512
#define ITEM_WEAR_ABOUT		   1024
#define ITEM_WEAR_WAIST		   2048
#define ITEM_WEAR_WRIST		   4096
#define ITEM_WIELD		   8192
#define ITEM_HOLD		  16384
#define ITEM_WEAR_FACE		  32768


/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE		      0
#define APPLY_STR		      1
#define APPLY_DEX		      2
#define APPLY_INT		      3
#define APPLY_WIS		      4
#define APPLY_CON		      5
#define APPLY_SEX		      6
#define APPLY_CLASS		      7
#define APPLY_LEVEL		      8
#define APPLY_AGE		      9
#define APPLY_HEIGHT		     10
#define APPLY_WEIGHT		     11
#define APPLY_MANA		     12
#define APPLY_HIT		     13
#define APPLY_MOVE		     14
#define APPLY_GOLD		     15
#define APPLY_EXP		     16
#define APPLY_AC		     17
#define APPLY_HITROLL		     18
#define APPLY_DAMROLL		     19
#define APPLY_SAVING_PARA	     20
#define APPLY_SAVING_ROD	     21
#define APPLY_SAVING_PETRI	     22
#define APPLY_SAVING_BREATH	     23
#define APPLY_SAVING_SPELL	     24
#define APPLY_POLY		     25
#define APPLY_GENERATION	     26
#define APPLY_POTENCE		     27
#define APPLY_CELERITY		     28
#define APPLY_FORTITUDE		     29
#define APPLY_BLOOD_MAX		     30
#define APPLY_BLOOD_POT		     31
#define APPLY_ANIMALISM		     32
#define APPLY_AUSPEX		     33
#define APPLY_CHIMERSTRY	     34
#define APPLY_DAIMOINON		     35
#define APPLY_DOMINATE		     36
#define APPLY_DEMENTATION	     37
#define APPLY_NECROMANCY	     38
#define APPLY_OBEAH		     39
#define APPLY_OBFUSCATE		     40
#define APPLY_OBTENEBRATION	     41
#define APPLY_PRESENCE		     42
#define APPLY_PROTEAN		     43
#define APPLY_QUIETUS		     44
#define APPLY_SERPENTIS		     45
#define APPLY_MORTIS		     46
#define APPLY_THAUMATURGY	     47
#define APPLY_VICISSITUDE	     48
#define APPLY_SET_ANIMALISM	     49	/* SET_ are set rather than added */
#define APPLY_SET_AUSPEX	     50
#define APPLY_SET_CELERITY	     51
#define APPLY_SET_CHIMERSTRY	     52
#define APPLY_SET_DAIMOINON	     53
#define APPLY_SET_DOMINATE	     54
#define APPLY_SET_FORTITUDE	     55
#define APPLY_SET_DEMENTATION	     56
#define APPLY_SET_NECROMANCY	     57
#define APPLY_SET_OBEAH		     58
#define APPLY_SET_OBFUSCATE	     59
#define APPLY_SET_OBTENEBRATION      60
#define APPLY_SET_POTENCE	     61
#define APPLY_SET_PRESENCE	     62
#define APPLY_SET_PROTEAN	     63
#define APPLY_SET_QUIETUS	     64
#define APPLY_SET_SERPENTIS	     65
#define APPLY_SET_MORTIS	     66
#define APPLY_SET_THAUMATURGY	     67
#define APPLY_SET_VICISSITUDE	     68



/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE		      1
#define CONT_PICKPROOF		      2
#define CONT_CLOSED		      4
#define CONT_LOCKED		      8



/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO		      2
#define ROOM_VNUM_OOC		     10
#define ROOM_VNUM_SCHOOL	   3700
#define ROOM_VNUM_HELL		  1
#define ROOM_VNUM_CRYPT		  1
#define ROOM_VNUM_DISCONNECTION	  1
#define ROOM_VNUM_IN_OBJECT	  1
#define ROOM_VNUM_HEAVEN	  1



/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK		      1
#define ROOM_NO_MOB		      4
#define ROOM_INDOORS		      8
#define ROOM_PRIVATE		    512
#define ROOM_SAFE		   1024
#define ROOM_SOLITARY		   2048
#define ROOM_PET_SHOP		   4096
#define ROOM_NO_RECALL		   8192
#define ROOM_NO_TELEPORT	  16384
#define ROOM_TOTAL_DARKNESS	  32768
#define ROOM_UNDER_WATER         65536
#define ROOM_SMALLCAERN         131072
#define ROOM_LARGECAERN        262144
#define ROOM_DEFILEDCAERN        524288
#define ROOM_FLAG_MAX		 524288


#define ROOM2_SILENCE		      1

/*
 * Room text flags (KaVir).
 * Used in #ROOMS.
 */
#define RT_LIGHTS		      1	/* Toggles lights on/off */
#define RT_SAY			      2	/* Use this if no others powers */
#define RT_ENTER		      4
#define RT_CAST			      8
#define RT_THROWOUT		     16	/* Erm...can't remember ;) */
#define RT_OBJECT		     32	/* Creates an object */
#define RT_MOBILE		     64	/* Creates a mobile */
#define RT_LIGHT		    128	/* Lights on ONLY */
#define RT_DARK			    256	/* Lights off ONLY */
#define RT_OPEN_LIFT		    512	/* Open lift */
#define RT_CLOSE_LIFT		   1024	/* Close lift */
#define RT_MOVE_LIFT		   2048	/* Move lift */
#define RT_SPELL		   4096	/* Cast a spell */
#define RT_PORTAL		   8192	/* Creates a one-way portal */
#define RT_TELEPORT		  16384	/* Teleport player to room */

#define RT_ACTION		  32768
#define RT_BLANK_1		  65536
#define RT_BLANK_2		 131072

#define RT_RETURN		1048576	/* Perform once */
#define RT_PERSONAL		2097152	/* Only shows message to char */
#define RT_TIMER		4194304	/* Sets object timer to 1 tick */



/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH		      0
#define DIR_EAST		      1
#define DIR_SOUTH		      2
#define DIR_WEST		      3
#define DIR_UP			      4
#define DIR_DOWN		      5
#define MAX_DIR			      6	/* OLC */



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR		      1
#define EX_CLOSED		      2
#define EX_LOCKED		      4
#define EX_BASHPROOF		      8	/* KaVir */
#define EX_BASHED		     16	/* KaVir */
#define EX_PICKPROOF		     32
#define EX_PASSPROOF		     64	/* KaVir */
#define EX_WALL_WATER		    128	/* KaVir - Path of Water power 4 */
#define EX_WARD_GHOULS		    256	/* KaVir - Ward vs Ghouls */
#define EX_WARD_LUPINES		    512	/* KaVir - Ward vs Lupines */
#define EX_WARD_KINDRED		   1024	/* KaVir - Ward vs Kindred */
#define EX_WARD_SPIRITS		   2048	/* KaVir - Ward vs Spirits */
#define EX_WALL_FLAME		   4096	/* KaVir - Path of Flame power 4 */
#define EX_WALL_GRANITE		   8192	/* KaVir - Philodox power 5 */
#define EXIT_FLAG_MAX		   8192


/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE		      0
#define SECT_CITY		      1
#define SECT_FIELD		      2
#define SECT_FOREST		      3
#define SECT_HILLS		      4
#define SECT_MOUNTAIN		      5
#define SECT_WATER_SWIM		      6
#define SECT_WATER_NOSWIM	      7
#define SECT_UNDERWATER		      8
#define SECT_AIR		      9
#define SECT_DESERT		     10
#define SECT_MAX		     11



/*
 * Equipment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE		     -1
#define WEAR_LIGHT		      0
#define WEAR_FINGER_L		      1
#define WEAR_FINGER_R		      2
#define WEAR_NECK_1		      3
#define WEAR_NECK_2		      4
#define WEAR_BODY		      5
#define WEAR_HEAD		      6
#define WEAR_LEGS		      7
#define WEAR_FEET		      8
#define WEAR_HANDS		      9
#define WEAR_ARMS		     10
#define WEAR_SHIELD		     11
#define WEAR_ABOUT		     12
#define WEAR_WAIST		     13
#define WEAR_WRIST_L		     14
#define WEAR_WRIST_R		     15
#define WEAR_WIELD		     16
#define WEAR_HOLD		     17
#define WEAR_FACE		     18
#define WEAR_SCABBARD_L		     19
#define WEAR_SCABBARD_R		     20
#define WEAR_STAKE		     21
#define MAX_WEAR		     22




/*
 * Locations for damage.
 */
#define LOC_ARM_L		      2
#define LOC_ARM_R		      3


/*
 * For Arms
 */
#define SLIT_WRIST		    8192


/*
 * For Bleeding
 */
#define BLEEDING_WRIST_L	    1024
#define BLEEDING_WRIST_R	    2048



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/


/*
 * Stats - KaVir.
 */
#define STAT_STR		      0	/* Strength */
#define STAT_INT		      1	/* Intelligence */
#define STAT_WIS		      2	/* Wisdom */
#define STAT_DEX		      3	/* Dexterity */
#define STAT_CON		      4	/* Constitution */

/*
 * Attributes - Brad
 */
#define MAX_ATTRIB			  9	/* Total Number of Attributes */
#define MAX_TRAINABLE_ATTRIB  10	/* Max trainable Score of Attributes */
#define MAX_CHAR_CREATION_ATTRIB	4

#define ATTRIB_STR			  0	/* Strength */
#define ATTRIB_DEX			  1	/* Dexterity */
#define ATTRIB_STA			  2	/* Stamina */

#define ATTRIB_CHA			  3	/* Charisma */
#define ATTRIB_MAN			  4	/* Manipulation */
#define ATTRIB_APP			  5	/* Appearance */

#define ATTRIB_PER			  6	/* Perception */
#define ATTRIB_INT			  7	/* Intelligence */
#define ATTRIB_WIT			  8	/* Wits */

#define MAX_TRAINABLE_ABILITY		6
#define MAX_CHAR_CREATION_ABILITY	3

/*
 * Talents - Kyrlin
 */
#define TALENTS				 0	/* Identifier in abilities array */
#define MAX_TAL			        10	/* Total Number of Taletns */
#define MAX_TRAINABLE_TAL                6	/* Max trainable Score of Talents */

#define TAL_EXPRESSION			  0	/* Acting */
#define TAL_ALERTNESS			  1	/* Alertness */
#define TAL_ATHLETICS			  2	/* Athletics */
#define TAL_BRAWL			  3	/* Brawl */
#define TAL_DODGE			  4	/* Dodge */
#define TAL_EMPATHY			  5	/* Empathy */
#define TAL_INTIMIDATION		  6	/* Intimidation */
#define TAL_STREETWISE			  7	/* Streetwise */
#define TAL_LEADERSHIP			  8	/* Leadership */
#define TAL_SUBTERFUGE			  9	/* Subterfuge */

/*
 * Skills - Kyrlin
 */
#define SKILLS				 1	/* Identifier in abilities array */
#define MAX_SKI			        10	/* Total Number of Skills */
#define MAX_TRAINABLE_SKI                6	/* Max trainable Score of Talents */

#define SKI_ANIMALKEN			  0	/* Animal Ken */
#define SKI_FIREARMS			  1	/* Boomstick */
#define SKI_CRAFTS			  2	/* Crafts */
#define SKI_ETIQUETTE			  3	/* Etiquette */
#define SKI_SECURITY			  4	/* Security */
#define SKI_MELEE			  5	/* Melee */
#define SKI_PERFORMANCE			  6	/* Performance */
#define SKI_DRIVE			  7	/* Drive */
#define SKI_STEALTH			  8	/* Stealth */
#define SKI_SURVIVAL			  9	/* Survival */

/*
 * Knowledges - Kyrlin
 */
#define KNOWLEDGES			 2	/* Identifier in abilities array */
#define MAX_KNO			        10	/* Total Number of Knowledges */
#define MAX_TRAINABLE_KNO                6	/* Max trainable Score of Knowledges */

#define KNO_ACADEMICS			  0	/* Academics */
#define KNO_COMPUTER     		  1	/* Computer */
#define KNO_INVESTIGATION		  2	/* Investigation */
#define KNO_LAW				  3	/* Law */
#define KNO_LINGUISTICS			  4	/* Linguistics */
#define KNO_MEDICINE			  5	/* Medicine */
#define KNO_OCCULT			  6	/* Occult */
#define KNO_POLITICS			  7	/* Politics */
#define KNO_SCIENCE			  8	/* Science */
#define KNO_FINANCE			  9	/* Finance */


struct str_att_bonus_type
{
	int todam;
	int carry;
	int wield;
};

struct dex_att_bonus_type
{
	int tohit;
	int defensive;
};

/*
 * Positions.
 */
#define POS_DEAD		      0
#define POS_MORTAL		      1
#define POS_INCAP		      2
#define POS_STUNNED		      3
#define POS_SLEEPING		      4
#define POS_MEDITATING		      5
#define POS_SITTING		      6
#define POS_RESTING		      7
#define POS_FIGHTING		      8
#define POS_STANDING		      9



/*
 * ACT bits for players.
 */
#define PLR_IS_NPC		      1	/* Don't EVER set.      */
#define PLR_CHAMPION		      4
#define PLR_AUTOEXIT		      8
#define PLR_VANISH		     16
#define PLR_MXP    32
#define PLR_BLANK		     64
#define PLR_BRIEF		    128
#define PLR_NOTRANS		    256
#define PLR_COMBINE		    512
#define PLR_PROMPT		   1024
#define PLR_TELNET_GA		   2048
#define PLR_HOLYLIGHT		   4096
#define PLR_WIZINVIS		   8192
#define PLR_ANSI		  16384
#define	PLR_SILENCE		  32768
#define PLR_NO_EMOTE		  65536
#define PLR_NO_TELL		 262144
#define PLR_LOG			 524288
#define PLR_BLUR		1048576
#define PLR_FREEZE		2097152
#define PLR_WOLFMAN		4194304
#define PLR_SNOOP_PROOF		8388608
#define PLR_ROOMFLAGS	       16777216
#define PLR_LAST_AGG	       33554432
#define PLR_STORYTELLER	       67108864
#define PLR_INCOG	      134217728
#define PLR_MAP           (2*     PLR_INCOG )
#define PLR_EXTRAS           (2*     PLR_MAP )

/*
 * EXTRA bits for players. (KaVir)
 */
#define EXTRA_SIRE		      1
#define EXTRA_PRINCE		      2
#define EXTRA_TRUSTED		      4
#define EXTRA_NEWPASS		      8
#define EXTRA_OSWITCH		     16
#define EXTRA_SWITCH		     32
#define EXTRA_FAKE_CON		     64
#define TIED_UP			    128
#define GAGGED			    256
#define BLINDFOLDED		    512
#define EXTRA_PSAY		   1024
#define EXTRA_DONE		   2048
#define EXTRA_EXP		   4096
#define EXTRA_PREGNANT		   8192
#define EXTRA_LABOUR		  16384
#define EXTRA_BORN		  32768
#define EXTRA_PROMPT		  65536
#define EXTRA_MARRIED		 131072
#define EXTRA_DYING		 262144	/* You are about to die! */
#define EXTRA_NON_NEWBIE	 524288
#define EXTRA_CALL_ALL		1048576
#define EXTRA_CAN_SEE		2097152	/* You can't see, but familiar can */
#define EXTRA_SHOPKEEPER	4194304	/* temp */
#define EXTRA_AWE		8388608	/* Everyone is in awe of you */
#define EXTRA_NO_ACTION	       16777216	/* You can't do anything */
#define EXTRA_ILLUSION	       33554432	/* Mob is really just an illusion */
#define EXTRA_VICISSITUDE      67108864	/* Cannot see through disguise */
#define EXTRA_INVIS_EMAIL     134217728	/* Cannot see email via finger */
#define EXTRA_AUTHORISED      268435456	/* Player is authorised */

/*
 * EXTRA2 bits for players. (Brad)
 */
#define EXTRA2_AFK			1	/* Away from keyboard */
#define EXTRA2_DND			2	/* Do no disturb --Imp Only */
#define EXTRA2_JUSTDIED			4	/* Stop aggro's from mort looping people */
#define EXTRA2_DAEMON_REGEN		8	/* Daemon's Regenerating */
#define EXTRA2_TORPORED			16	/* Vampire Torpor Flag */
#define EXTRA2_RESEARCH                 32      /* Thaumaturgy Research */

/*
 * MORE bits for players. (KaVir)
 */
#define MORE_NPC		      1	/* NPC - ie cannot improve */
#define MORE_ANTITRIBU		      2	/* Antitribu vampire */
#define MORE_MUMMIFIED		      4	/* Serpentis 3 */
#define MORE_DARKNESS		      8	/* Obtenebration 1 */
#define MORE_SILENCE		     16	/* Quietus 1 */
#define MORE_PLASMA		     32	/* Vicissitude 5 */
#define MORE_ASHES		     64	/* Thanatosis 3 */
#define MORE_GUARDIAN		    128	/* Necromancy 4 */
#define MORE_LOYAL		    256	/* Dominate 6 */
#define MORE_SINGING		    512	/* Singing atm */
#define MORE_LEADER		   1024	/* Coterie leader */
#define MORE_HAND_FLAME		   2048	/* Hand of Flame */
#define MORE_SPIRIT		   4096	/* You become a true spirit */
#define MORE_STAKED		   8192	/* You have been staked */
#define MORE_DEFLECTION		  16384	/* Deflection ritual in effect */
#define MORE_PROJECTION		  32768	/* Using astral projection */
#define MORE_CHAMPION		  65536	/* Coterie champion */
#define MORE_REINA		 131072	/* Song of Reina - Melpominee 2 */
#define MORE_EVILEYE		 262144	/* Evil Eye ON */
#define MORE_FLAMES		 524288	/* Flames of the nether world */
#define MORE_COURAGE		1048576	/* Courage - Presence 7 */
#define MORE_ANGER		2097152	/* Anger - Presence 8 */
#define MORE_ANIMAL_MASTER	4194304	/* Animals obey and dont attack you */
#define MORE_INFORM		8388608	/* Inform when say */
#define MORE_CALM	       16777216	/* Cancel's Anger, cannot attack */
#define MORE_BRISTLES	       33554432	/* Metis power level 5 */
#define MORE_OBEAH	       67108864	/* Obeah third eye is open */
#define MORE_NOPAIN	      134217728	/* Obeah 2, no pain penalties */
#define MORE_NEUTRAL	      268435456	/* Obeah 3, makes room safe room */
#define MORE_BABEL	      	536870912	/* Bastet Babels Curse */
#define MORE_QUICKSAND	      (MORE_BABEL *2)	/* WW quicksand */


/*
 * MORE2 bits for players. (MereK)
 */
#define MORE2_TEMPORIS		      	1	/* Temporis 5 */
#define MORE2_BLESSING		      	2	/* Valrean 4 */
#define MORE2_RPFLAG		      	   4
#define MORE2_BLIGHT		         	8
#define MORE2_CHISTOP		      	   16
#define MORE2_HEIGHTENSENSES    		      	32	/**/
#define MORE2_DIONYSIS    		      64	/* Nuwisha Umbra Danse 2 */
#define MORE2_PASSION    		     256	/* Dementation 1 */
#define MORE2_COMA    		     512	/* Dementation */
#define MORE2_DRAENORTENTICLES  		       1024	/* Caligo 4 */
#define MORE2_HAUNTED		       8192 /* Dementation  */
#define MORE2_DEMENTATION2  	  	16384	/* Dementation level 2, Mind Tricks */
#define MORE2_DEMENTATION4      	32768	/* Dementation level 4, Confusion  */
#define MORE2_DEMENTATION6	      65536	/* Dementation level 6, Deranged */
#define MORE2_DEMENTATION7  	  131072	/* Dementation level 7, Mind of a Child */
#define MORE2_DEMENTATION9  	  262144	/* Dementation level 9, Scourage */
#define MORE2_DEIMOSWHISPERS	  524288	/* Deimos level 1 */
#define MORE2_DEIMOSSANGUIN 	 1048576	/* Deimos level..4 i think */
#define MORE2_HORRIDREALITY	 2097152	/* Chimistry lvl 5, Horrid Reality */
#define 	MORE2_DRAENORCLOAK 4194304	/* Caligo 2 */
#define 	MORE2_ZOMBIECURSE	    8388608	/* Temporis level 3 */
#define 	MORE2_CRIMSONFURY        	 16777216	/* Animalism 8 */
#define 	MORE2_AGONY           33554432	/* Auspex....8? */
#define 	MORE2_SENSORYDEPRIVED        	 67108864	/* Chimistry 9 */
#define 	MORE2_RPVIS         	134217728
#define 	MORE2_NIGHTEYES         	268435456
#define 	MORE2_BLACKDEATH        	536870912


/*
 * MORE2 bits for players. (MereK)
 */
#define MORE3_THOTH		      	1	/* Bardo 2 */
#define MORE3_PARADOX		      	2	/* Bardo 4 */
#define MORE3_BLOODCURSE	      	   4	/* Assamite Quietus 8 */
#define MORE3_ESCTASY		         	8	/* Auspex 7 Tzimisce */
#define MORE3_DARKSIGHT		      	   16	/* Lasombra Obtenebration 7 */
#define MORE3_RESILIENCE    		      	32	/* Mortis 4 Fortitude 8  */
#define MORE3_CRIMSONFURY    		      64	/* Animalism 8 */
#define MORE3_SENSORYSHIELD    		     128	/* Fortitude 6 */
#define MORE3_MYTHICALFORM    		     256	/* Protean 8 */
#define MORE3_FLESHWOUND    		     512	/* Obfuscate 3 Fortitude 2 */
#define MORE3_OBSESSION  		       1024	/* Serpentis 7 */
#define MORE3_OBFUS1           2048	/* Obfus1*/
#define MORE3_OBFUS2	 	    4096	/* obfus2*/

//#define MORE3_PHOBIA           2048	
//#define MORE3_CORRUPTION	 	    4096	

#define MORE3_TEMPTATION		       8192	/* Serpentis 8 */
#define MORE3_COWALKER  	  131072	/* Temporis 4 */
#define MORE3_RAMBLEON  	  262144	/* Temporis 2 */
#define MORE3_TIMEPOCKET	  524288	/* Temporis 8 */
#define MORE3_TIMESENSE 	 1048576	/* Temporis 1 */
#define MORE3_ANUBIS	 2097152
#define 	MORE3_DEATHSWHISPER	    8388608
#define 	MORE3_MASQUE        	 16777216
#define 	MORE3_LETHARGY           33554432
#define 	MORE3_SPEED        	 67108864
#define 	MORE3_SWIM         	134217728



#define CRIME_C_OUTLAW		      1	/* Camarilla Outlaw */
#define CRIME_A_OUTLAW		      2	/* Anarch Outlaw */
#define CRIME_S_OUTLAW		      4	/* Sabbat Outlaw */
#define CRIME_I_OUTLAW		      8	/* Inconnu Outlaw */
#define CRIME_BLOODHUNT		     16	/* Camarilla Bloodhunted */
#define CRIME_DIABLERIE		     32	/* Committed Diablerie */
#define CRIME_MASQUERADE	     64	/* Breached the Masquerade */
#define CRIME_DOMAIN		    128	/* Disobeyed Prince */
#define CRIME_PROGENY		    256	/* Sired childe without permission */
#define CRIME_ACCOUNTING	    512	/* Childe broke tradition */
#define CRIME_HOSPITALITY	   1024	/* Not yet presented self to prince */
#define CRIME_DESTRUCTION	   2048	/* Killed a vampire in same sect */

#define DIAB_TIME		      0	/* Duration of diablerie aura */
#define DIAB_EVER		      1	/* Number of diableries committed */

#define LIE_TRUE		      0
#define LIE_FALSE		      1


/*
 * AGE Bits.
 */
#define AGE_CHILDE		      0
#define AGE_NEONATE		      1
#define AGE_ANCILLA		      2
#define AGE_ELDER		      3
#define AGE_METHUSELAH		      4
#define AGE_ANTEDILUVIAN	      5

#define AGE_PUP			      0
#define AGE_YOUTH		      1
#define AGE_ADULT		      2
#define AGE_ELDER		      3
#define AGE_ANCIENT		      4


/*
 * Status.
 */
#define STATUS_FEAR		      0
#define STATUS_PRESTIGE		      1
#define STATUS_NATURAL		      2



/*
 *  Totems for werewolves.
 */
#define TOTEM_MANTIS	      0
#define TOTEM_BEAR	      1
#define TOTEM_LYNX	      2
#define TOTEM_BOAR	      3
#define TOTEM_OWL	      4
#define TOTEM_SPIDER	      5
#define TOTEM_WOLF	      6
#define TOTEM_HAWK	      7
#define TOTEM_SILVER	     10



/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION		      4	/* Obsolete     */
#define PLR_CHAT		    256	/* Obsolete     */
#define PLR_NO_SHOUT		 131072	/* Obsolete     */
#endif



/*
 * Channel bits.
 */
#define	CHANNEL_OOC		      1
#define CHANNEL_STORYINFORM	      2
#define	CHANNEL_OMOTE		      4
#define	CHANNEL_IMMTALK		      8
#define	CHANNEL_STORY		     16
#define	CHANNEL_QUESTION	     32
#define	CHANNEL_SHOUT		     64 // radio channel
#define	CHANNEL_YELL		    128
#define	CHANNEL_HOWL		    512
#define	CHANNEL_LOG		   1024
#define	CHANNEL_PRAY		   2048
#define	CHANNEL_INFO		   4096
#define	CHANNEL_OSAY		   8192	/* Is out - Kyrlin */
#define	CHANNEL_TELL		  16384
#define CHANNEL_LOG_NORMAL	  32768
#define CHANNEL_LOG_CODER	  65536
#define CHANNEL_LOG_STORYTELLER  131072
#define CHANNEL_LOG_CONNECT	 262144
#define CHANNEL_LOG_PLAYER	 524288
#define CHANNEL_LOG_DICE 	 1048576
#define CHANNEL_CELL         2097152


//mob programs
struct mob_prog_act_list
{
	MPROG_ACT_LIST *next;
	char *buf;
	CHAR_DATA *ch;
	OBJ_DATA *obj;
	void *vo;
};


struct mob_prog_data
{
	MPROG_DATA *next;
	int type;
	char *arglist;
	char *comlist;
};

struct mob_prog_resets
{
	MPROG_RESETS *next;
	int vnum;
	char *filename;
};

bool MOBtrigger;

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           1
#define SPEECH_PROG        2
#define RAND_PROG          4
#define FIGHT_PROG         8
#define DEATH_PROG        16
#define ENTRY_PROG        32
#define GREET_PROG        64
#define ALL_GREET_PROG   128
#define GIVE_PROG        256
#define BRIBE_PROG       512
#define MAX_PROG	 512



/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct mob_index_data
{
	MOB_INDEX_DATA *next;
	SPEC_FUN *spec_fun;
	char *spec_name;	/* OLC */
	SHOP_DATA *pShop;
	AREA_DATA *area;	/* OLC */
	CHAR_DATA *mount;
	CHAR_DATA *wizard;
	CHAR_DATA *embrace;
	CHAR_DATA *next_fight;
	CHAR_DATA *prev_fight;
	OBJ_DATA *sat;
	char *mobprog_filename;
	MPROG_DATA *mobprogs;	//mprog
	int progtypes;		//mprog
	char *hunting;
	char *player_name;
	char *short_descr;
	char *long_descr;
	char *description;
	char *lord;
	char *clan;
	char *side;
	char *morph;
	char *createtime;
	char *lasttime;
	char *lasthost;
	char *powertype;
	char *poweraction;
	char *pload;
	char *prompt;
	char *cprompt;
	sh_int spectype;
	sh_int specpower;
	int loc_hp[7];
	sh_int vnum;
	sh_int count;
	sh_int killed;
	sh_int sex;
	sh_int mounted;
	sh_int embraced;
	sh_int home;
	sh_int init;
	int level;
	int immune;
	int polyaff;
	int vampaff;
	int itemaffect;
	int vamppass;
	int form;
	int body;
	int act;
	int extra;
	int more;
	int more2;
	int more3;
	int affected_by;
	int affected_by2;
	int hit;
	int max_hit;
	sh_int alignment;
	sh_int hitroll;		/* Unused */
	sh_int ac;		/* Unused */
	sh_int hitnodice;	/* Unused */
	sh_int hitsizedice;	/* Unused */
	sh_int hitplus;		/* Unused */
	sh_int damnodice;	/* Unused */
	sh_int damsizedice;	/* Unused */
	sh_int damplus;		/* Unused */
	sh_int x;
	sh_int y;
	sh_int home_x;
	sh_int home_y;
	sh_int room;
	sh_int state;
	sh_int action;
	sh_int quality;
	int gold;		/* Unused */
	// Gobbo Trial
	sh_int attributes[9];
	sh_int abilities[3][10];
};

typedef struct memory_node
{
        char * real_name;
        char * remembered_name;
        struct memory_node * next;
        struct memory_node * previous;
}MEMORY_DATA;

/*
 *pose/renown recommendation
 */
 
//Files
#define POSE_RECC_FILE "posereccs.lol"
#define RENOWN_RECC_FILE "renownreccs.lol"

//Renown reccomend types
#define HONOR_RECC 1
#define GLORY_RECC 2
#define WISDOM_RECC 3

//Linked list structs
struct pose_recc
{
	char *giver;
	char *receiver;
	char *text;
	POSE_RECC *next;
};

struct renown_recc
{
	char *giver;
	char *receiver;
	char *text;
	int type;
	int amount;
	RENOWN_RECC *next;
};
/* One character (PC or NPC).
 */
struct char_data
{
	MEMORY_DATA * memory;
	CHAR_DATA *next_wilderness;
	CHAR_DATA *prev_wilderness;
	CHAR_DATA *next;
	CHAR_DATA *next_in_room;
	CHAR_DATA *next_fight;
	CHAR_DATA *prev_fight;
	CHAR_DATA *master;
	CHAR_DATA *leader;
	CHAR_DATA *fighting;
	CHAR_DATA *reply;
	CHAR_DATA *mount;
	CHAR_DATA *wizard;
	CHAR_DATA *embrace;
	OBJ_DATA *sat;
	SPEC_FUN *spec_fun;
	MOB_INDEX_DATA *pIndexData;
	DESCRIPTOR_DATA *desc;
	AFFECT_DATA *affected;
	OBJ_DATA *carrying;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *was_in_room;
	PC_DATA *pcdata;
	MPROG_ACT_LIST *mpact;	//mprog
	int mpactnum;		//mprog
	char *hunting;
	char *name;
	char *short_descr;
	char *long_descr;
	char *description;
	char *lord;
	char *clan;
	char *side;
	char *morph;
	char *mem_name;
	char *mem_desc;
	char *createtime;
	char *lasttime;
	char *lasthost;
	char *poweraction;
	char *powertype;
	char *pload;
	char *prompt;
	char *cprompt;
	char *nature;
	char *demeanor;
	char *order;
	char *impulse;
	sh_int sex;
	sh_int race;
	int class;
	int immune;
	int polyaff;
	int vampaff;
	int itemaffect;
	int vamppass;
	int form;
	int body;
	sh_int vampgen;
	sh_int spectype;
	sh_int specpower;
	sh_int loc_hp[7];
	sh_int wpn[13];
	sh_int spl[5];
	sh_int cmbt[8];
	sh_int stance[16];
	sh_int autostance;
	sh_int blood[3];
	sh_int beast;
	sh_int truebeast;
	sh_int mounted;
	sh_int embraced;
	sh_int home;
	sh_int init;
	sh_int level;
	sh_int trust;
	int played;
	time_t logon;
	time_t save_time;
	sh_int timer;
	sh_int wait;
	int pkill;
	int pdeath;
	int mkill;
	int mdeath;
	sh_int agg;
	int hit;
	int max_hit;
	int mana;
	int max_mana;
	int move;
	int max_move;
	int gold;
	int exp;
	int act;
	int extra;
	int extra2;
	int more;
	int more2;
	int more3;
	int affected_by;
	int affected_by2;
	sh_int position;
	sh_int carry_weight;
	sh_int carry_number;
	sh_int saving_throw;
	sh_int alignment;
	sh_int hitroll;
	sh_int damroll;
	sh_int armor;
	sh_int wimpy;
	int deaf;
	sh_int paradox[3];
	sh_int damcap[2];
	sh_int attributes[9];
	sh_int mod_attributes[9];
	sh_int health_level;
	sh_int abilities[3][10];
	sh_int x;
	sh_int y;
	sh_int home_x;
	sh_int home_y;
	sh_int room;
	sh_int state;
	sh_int action;
	sh_int quality;
	int plane;
	sh_int quintessence;
	sh_int max_quintessence;
	bool ploaded;
	int playedIC;
	time_t rpflag_time;
	time_t last_pose;
	CHAR_DATA *onPhone; //target of cell command
	int phonestate;     //-1 if caller and unanswered, 0 if callee and unaswered, 1 if answered
	bool phonevibrate;  //false if vibrate mode, true if loud.
	char *ringtone;     //description of character's ringtone. 
};


/*
 * Data which only PC's have.
 */
struct pc_data
{
	PC_DATA *next;
	CHAR_DATA *familiar;
	CHAR_DATA *partner;
	CHAR_DATA *propose;
	OBJ_DATA *chobj;
	OBJ_DATA *memorised;
	char *pwd;
	bool confirm_delete;
	char *bamfin;
	char *bamfout;
	char *title;
	char *pretitle;
	char *lupusdescshort;
	char *crinosdescshort;
	char *hispodescshort;
	char *glabrodescshort;
	char *lupusdesc;
	char *hispodesc;
	char *glabrodesc;
	char *giftlist;
	char *crinosdesc;
	char *conception;
	char *parents;
	char *cparents;
	char *marriage;
	char *email;
	char *conding;
	char *love;
	char *coterie;
	char *cot_time;
	char *cot_clans;
	char *cot_sides;
	char *leader;
	char *soul_keeper;
	char *stancename;
	char *stancelist;
	char *stancework;
	int stancemove[5];
	sh_int perm_str;
	sh_int perm_int;
	sh_int perm_wis;
	sh_int perm_dex;
	sh_int perm_con;
	sh_int mod_str;
	sh_int mod_int;
	sh_int mod_wis;
	sh_int mod_dex;
	sh_int mod_con;
	int abilities_points[3][10];
	char stat_groups[3];
	char ability_groups[3];
	int denydate;
	int quest;
	sh_int wolf;
	sh_int rank;
	sh_int regenerate;
	sh_int demonic;
	sh_int atm;
	sh_int song;
	int necromancy;
	int obeah;
	int darkness;
	int temporis;
	int reina;
	int silence;
	int dementation;
	int devil;
	int bodymods;
	int bpoints;
	int dpoints;
	int gpoints;
	int rpoints;
	int wpoints;
	int spirit;
	int conjure;
	int celerity;
	int firstaid;
	int repair;
	int teach;
	int learn;
	int evileye;
	sh_int notes;
	sh_int followers;
	sh_int gnosis[2];
	sh_int rage[2];
	int absorb[6];
	int qstats[5];
	int language[2];
	sh_int stage[3];
	sh_int status[3];
	sh_int wolfform[2];
	int score[6];
	sh_int runes[4];
	sh_int disc[11];
	int research[2];
	int rituals[10];
	sh_int cland[3];
	int diableries[2];
	sh_int resist[5];
	sh_int breed[3];
	sh_int auspice[5];
	sh_int tribes[20];
  /* Thaum */
	sh_int thaum[20];
	int pthaum[MAX_THAUM_PATHS];
	sh_int thaum_type;
        sh_int thaum_prime;
	sh_int research_power;
	sh_int research_time;
        sh_int research_type;
        sh_int research_tick;
	sh_int powers[40];
	sh_int powers_mod[20];
	sh_int powers_set[20];
	sh_int renown[6];
	sh_int faith;
	sh_int conviction[2];
	sh_int conviction_bank;
	int powers_set_wear;
	int power[2];
	int genes[10];
	int cot_min[12];
	sh_int willpower[2];
	sh_int fake_skill;
	sh_int fake_stance;
	sh_int fake_hit;
	sh_int fake_dam;
	int fake_hp;
	int fake_mana;
	int fake_move;
	int fake_max_hp;
	int fake_max_mana;
	int fake_max_move;
	int fake_ac;
	int fake_stats[5];
	sh_int obj_vnum;
	sh_int majesty;
	sh_int condition[3];
	sh_int learned[MAX_SKILL];
	long rp_counter;
	sh_int exhaustion;
	sh_int exercise[4];
	sh_int lie;
	sh_int class;
	int security;		/* OLC */
	BOARD_DATA *board;	/* The current board */
	time_t last_note[MAX_BOARD];	/* last note for the boards */
	NOTE_DATA *in_progress;
	char *doing;
	char *aura;
	char *voice;
	char *roomdesc;
	bool show_social;
	int road;
	sh_int roadt;
	sh_int virtues[6];
	bool extended_char_creation_in_progress;
	sh_int soul;
	int daemon_powers;
	int infernal_ranking;

/* language stuff*/
	int language2[MAX_LANGUAGE];
	int speaking;
	int vision;		/*do_map squares */

	char *tempdesc;
	bool showtempdesc;
	EXTRA *extra;
	sh_int pagelen;
/* Experience checks*/
/*------------------------------------ */
	int votes;
	TIME_INFO_DATA last_vote_time;

/*Damage Stuff*/
	int aggdamage;
	int lethaldamage;
	int bashingdamage;

/* WP Regain & Blood Burn Counters */
	int wp_regain_counter;
	int blood_burn_counter;
	int auto_vote_counter;
	int canbuy;
	int canbuycounter;
	int canvotecounter;
	int sectstatus;
	int statusvotecounter;

/* Merits/Flaws Stuff */
	char ** merits; // Array of strings
	char ** flaws;	// Array of strings
/* End Merits/Flaws Stuff */
};

/*
 * Player Information - email level type stuff.
 */
struct player_info
{
   char *	name;
   char *	email;
   char *	password;
   char *	last_host;
   time_t	logon;
   int		flags;
   EBAN_DATA *	accept_sites;
   EBAN_DATA *	deny_sites;
   PLAYER_INFO * next;
};

/* Flags for player_info */
#define PINFO_NONE		0
#define PINFO_NO_NEW_CHARS 	1
#define PINFO_BAN		2
#define PINFO_GOD_ACCOUNT	4
#define PINFO_CAN_MULTIPLAY	8

struct extra_data
{
EXTRA *next;
EXTRA *prev;
char *extra;
char *catagory;
char *field;
int rating;
int research;
};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_MAX		16

struct liq_type
{
	char *liq_name;
	char *liq_color;
	sh_int liq_affect[3];
};



/*
 * Extra description data for a room or object.
 */
struct extra_descr_data
{
	EXTRA_DESCR_DATA *next;	/* Next in list                     */
	char *keyword;		/* Keyword in look/examine          */
	char *description;	/* What to see                      */
};



/*
 * Prototype for an object.
 */
struct obj_index_data
{
	OBJ_INDEX_DATA *next;
	EXTRA_DESCR_DATA *extra_descr;
	AFFECT_DATA *affected;
	AREA_DATA *area;	/* OLC */
	char *name;
	char *short_descr;
	char *description;
	char *chpoweron;
	char *chpoweroff;
	char *chpoweruse;
	char *victpoweron;
	char *victpoweroff;
	char *victpoweruse;
	char *actcommands;
	char *questmaker;
	char *questowner;
	sh_int vnum;
	sh_int item_type;
	sh_int extra_flags;
	sh_int wear_flags;
	sh_int count;
	sh_int weight;
	int spectype;
	int specpower;
	sh_int condition;
	sh_int toughness;
	sh_int resistance;
	int quest;
	sh_int points;
	int cost;		/* Unused */
	int value[4];
};



/*
 * One object.
 */
struct obj_data
{
	OBJ_DATA *next;
	OBJ_DATA *next_content;
	OBJ_DATA *contains;
	OBJ_DATA *in_obj;
	CHAR_DATA *carried_by;
	CHAR_DATA *chobj;
	CHAR_DATA *sat;
	EXTRA_DESCR_DATA *extra_descr;
	AFFECT_DATA *affected;
	OBJ_INDEX_DATA *pIndexData;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	char *name;
	char *short_descr;
	char *description;
	char *chpoweron;
	char *chpoweroff;
	char *chpoweruse;
	char *victpoweron;
	char *victpoweroff;
	char *victpoweruse;
	char *actcommands;
	char *questmaker;
	char *questowner;
	sh_int item_type;
	sh_int extra_flags;
	sh_int wear_flags;
	sh_int wear_loc;
	sh_int weight;
	int spectype;
	int specpower;
	sh_int condition;
	sh_int toughness;
	sh_int resistance;
	int quest;
	sh_int points;
	int cost;
	sh_int level;
	sh_int timer;
	int value[4];
};



/*
 * Exit data.
 */
struct exit_data
{
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *next;	/* OLC */
	sh_int vnum;
	sh_int exit_info;
	sh_int key;
	char *keyword;
	char *description;
	int rs_flags;		/* OLC */
};



/*
 * Room text checking data.
 */
typedef struct roomtext_data
{
	int type;
	int power;
	int mob;
	char *input;
	char *output;
	char *choutput;
	char *name;
	struct roomtext_data *next;
}
ROOMTEXT_DATA;



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile 
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
	RESET_DATA *next;
	char command;
	sh_int arg1;
	sh_int arg2;
	sh_int arg3;
};

/*
 * Area definition.
 */
struct area_data
{
	AREA_DATA *next;
	RESET_DATA *reset_first;
	RESET_DATA *reset_last;
	MPROG_RESETS *mprogs;
	char *name;
	sh_int age;
	sh_int nplayer;
	sh_int x;
	sh_int y;
	sh_int start;
	char *filename;		/* OLC */
	char *builders;		/* OLC *//* Listing of */
	int security;		/* OLC *//* Value 1-9  */
	int lvnum;		/* OLC *//* Lower vnum */
	int uvnum;		/* OLC *//* Upper vnum */
	int vnum;		/* OLC *//* Area vnum  */
	int area_flags;		/* OLC */
};



/*
 * Room type.
 */
struct room_index_data
{
	ROOM_INDEX_DATA *next;
	CHAR_DATA *people;
	OBJ_DATA *contents;
	OBJ_DATA *to_obj;
	EXTRA_DESCR_DATA *extra_descr;
	AREA_DATA *area;
	EXIT_DATA *exit[6];
	ROOMTEXT_DATA *roomtext;
	char *track[5];
	char *name;
	char *description;
	sh_int vnum;
	int room_flags;
	int added_flags;
	sh_int light;
	sh_int blood;
	sh_int passed;
	sh_int track_dir[5];
	sh_int sector_type;
	sh_int wood;
	sh_int build;
	sh_int ground;
	sh_int x;
	sh_int y;
	sh_int room;
	sh_int version;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000
#define TYPE_ADD_AGG                 3000
#define TYPE_AGG                     3000



/*
 *  Target types.
 */
#define TAR_IGNORE		    0
#define TAR_CHAR_OFFENSIVE	    1
#define TAR_CHAR_DEFENSIVE	    2
#define TAR_CHAR_SELF		    3
#define TAR_OBJ_INV		    4

#define PURPLE_MAGIC		    0
#define RED_MAGIC		    1
#define BLUE_MAGIC		    2
#define GREEN_MAGIC		    3
#define YELLOW_MAGIC		    4



/*
 * Skills include spells as a particular case.
 */
struct skill_type
{
	char *name;		/* Name of skill                */
	sh_int skill_level;	/* Level needed by class        */
	SPELL_FUN *spell_fun;	/* Spell pointer (for spells)   */
	sh_int target;		/* Legal targets                */
	sh_int minimum_position;	/* Position for caster / user   */
	sh_int *pgsn;		/* Pointer to associated gsn    */
	sh_int slot;		/* Slot for #OBJECT loading     */
	sh_int min_mana;	/* Minimum mana used            */
	sh_int beats;		/* Waiting time after use       */
	char *noun_damage;	/* Damage message               */
	char *msg_off;		/* Wear off message             */
};

extern          DUMMY_ARG         *     dummy_free;
extern          DUMMY_ARG         *     dummy_list;
extern long int uptime;
/*
 * These are skill_lookup return values for common skills and spells.
 */
extern sh_int gsn_backstab;
extern sh_int gsn_hide;
extern sh_int gsn_peek;
extern sh_int gsn_pick_lock;
extern sh_int gsn_sneak;
extern sh_int gsn_steal;

extern sh_int gsn_fastdraw;
extern sh_int gsn_berserk;
extern sh_int gsn_punch;
extern sh_int gsn_elbow;
extern sh_int gsn_headbutt;
extern sh_int gsn_sweep;
extern sh_int gsn_knee;
extern sh_int gsn_tongue;
extern sh_int gsn_tail;
extern sh_int gsn_spines;
extern sh_int gsn_hurl;
extern sh_int gsn_kick;
extern sh_int gsn_rescue;
extern sh_int gsn_track;
extern sh_int gsn_polymorph;
extern sh_int gsn_web;
extern sh_int gsn_potency;
extern sh_int gsn_demon;
extern sh_int gsn_firstaid;
extern sh_int gsn_repair;
extern sh_int gsn_spit;
extern sh_int gsn_tendrils;
extern sh_int gsn_rot;
extern sh_int gsn_jail;

extern sh_int gsn_blindness;
extern sh_int gsn_charm_person;
extern sh_int gsn_curse;
extern sh_int gsn_invis;
extern sh_int gsn_mass_invis;
extern sh_int gsn_poison;
extern sh_int gsn_sleep;
extern sh_int gsn_fear;

extern sh_int gsn_dark_poison;
extern sh_int gsn_paradox;
extern sh_int gsn_darkness;

extern sh_int gsn_blast;
extern sh_int gsn_spikes;

/* language stuff */
extern sh_int gsn_english;
extern sh_int gsn_german;
extern sh_int gsn_chinese;
extern sh_int gsn_japanese;
extern sh_int gsn_indonesian;
extern sh_int gsn_korean;
extern sh_int gsn_vietnamese;
extern sh_int gsn_swahili;
extern sh_int gsn_tagalog;
extern sh_int gsn_hindu;
extern sh_int gsn_animal;
extern sh_int gsn_russian;
extern sh_int gsn_arabic;
extern sh_int gsn_french;
extern sh_int gsn_latin;
extern sh_int gsn_spanish;
extern sh_int gsn_italian;
extern sh_int gsn_hebrew;
extern sh_int gsn_gaelic;
extern sh_int gsn_egyptian;
extern sh_int gsn_enochian;
extern sh_int gsn_greek;
extern sh_int gsn_slavic;
extern sh_int gsn_garou;
extern sh_int gsn_demon2;
extern sh_int gsn_bastet;
extern sh_int gsn_corax;
extern sh_int gsn_wyrm;
extern sh_int gsn_nagah;
extern sh_int gsn_gurahl;
extern sh_int gsn_norse;

/*
 * Utility macros.
 */
#define UMIN(a, b)		((a) < (b) ? (a) : (b))
#define UMAX(a, b)		((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)		((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define LOWER(c)		((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)		((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))	/* OLC */
#define STR(dat, field)         (( (dat)->field != NULL                    \
															? (dat)->field                \
															: (dat)->pIndexData->field ))	/* OLC */

#define MTD(str)                ((str == NULL || str[0] == '\0' ))
#define MTP(str)                ((str == NULL || *str   == '\0' ))
#define stc send_to_char

#define INTERPOLATE(min, max, percent)   ( min + (max - min / percent == 0 ? 1 : percent ) )
/* Ported stuff by MereK, used for Dynamic EQ descriptions */
#define HE_SHE(ch)           (((ch)->sex == SEX_MALE  ) ? "he"   :  \
									(  ((ch)->sex == SEX_FEMALE) ? "she"  : "it" ) )
#define HIS_HER(ch)          (((ch)->sex == SEX_MALE  ) ? "his"  :  \
									(  ((ch)->sex == SEX_FEMALE) ? "her"  : "its" ) )
#define HIM_HER(ch)          (((ch)->sex == SEX_MALE  ) ? "him"  :  \
									(  ((ch)->sex == SEX_FEMALE) ? "her"  : "it" ) )
#define MAL_FEM(ch)          (((ch)->sex == SEX_MALE  ) ? "male"  :  \
									(  ((ch)->sex == SEX_FEMALE) ? "female"  : "something" ) )

#define IS_FEMALE(ch)      ( (ch)->sex == SEX_FEMALE )
#define IS_MALE(ch)        ( (ch)->sex == SEX_MALE   )

#define STR2( obj, thing )       ( obj->pIndexData ? obj->thing : obj->thing )
//( obj->pIndexData ? obj->pIndexData->thing : obj->thing )
#define PERSO(obj, looker)      ( can_see_obj(looker, obj) ? STR(obj,short_descr) : "something" )
#define IS_VOWEL(c)             (c == 'A' || c == 'a' ||                     \
				 c == 'E' || c == 'e' ||                     \
				 c == 'I' || c == 'i' ||                     \
				 c == 'O' || c == 'o' ||                     \
				 c == 'U' || c == 'u'      )


/*
 * Character macros.
 */
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_ANIMAL(ch)		(IS_SET((ch)->act, ACT_ANIMAL) && IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMP(ch)		(get_trust(ch) >= LEVEL_IMPLEMENTOR)
#define IS_HIGHJUDGE(ch) (get_trust(ch) >= LEVEL_HIGHJUDGE)
#define IS_JUDGE(ch)		(get_trust(ch) >= LEVEL_JUDGE)
#define IS_IMMORTAL(ch)		(get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_STORYTELLER(ch)	(get_trust(ch) >= LEVEL_STORYTELLER)
#define IS_ENFORCER(ch)	(get_trust(ch) >= LEVEL_ENFORCER)
#define IS_BUILDER(ch)	(get_trust(ch) >= LEVEL_BUILDER)
#define IS_SHOPKEEPER(ch)	(get_trust(ch) >= LEVEL_SHOPKEEPER)
#define IS_HERO(ch)		(get_trust(ch) >= LEVEL_HERO)
#define CAN_SIT(ch)		((ch)->position >= 4 && (ch)->position <= 7)
#define CAN_PK(ch)		(get_trust(ch) >= 3 && get_trust(ch) <= 6)
#define IS_NEWBIE(ch)		((ch)->level <= 2)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)	(IS_SET((ch)->affected_by2, (sn)))
#define IS_SPEAKING(ch, sn)	(IS_SET((ch)->pcdata->language[0], (sn)))
#define CAN_SPEAK(ch, sn)	(IS_SET((ch)->pcdata->language[1], (sn)))
#define IS_ITEMAFF(ch, sn)	(IS_SET((ch)->itemaffect, (sn)))
#define IS_IMMUNE(ch, sn)	(IS_SET((ch)->immune, (sn)))
#define IS_WILLPOWER(ch, sn)	(IS_SET((ch)->pcdata->resist[0], (sn)))
#define IS_VAMPAFF(ch, sn)	(IS_SET((ch)->vampaff, (sn)))
#define IS_VAMPPASS(ch, sn)	(IS_SET((ch)->vamppass, (sn)))
#define IS_FORM(ch, sn)		(IS_SET((ch)->body, (sn)))
#define IS_POLYAFF(ch, sn)	(IS_SET((ch)->polyaff, (sn)))
#define IS_EXTRA(ch, sn)	(IS_SET((ch)->extra, (sn)))
#define IS_MORE(ch, sn)		(IS_SET((ch)->more, (sn)))
#define IS_MORE2(ch, sn)		(IS_SET((ch)->more2, (sn)))
#define IS_MORE3(ch, sn)		(IS_SET((ch)->more3, (sn)))
#define IS_MOD(ch, sn)		(IS_SET((ch)->pcdata->bodymods, (sn)))
#define IS_DEMPOWER(ch, sn)	(IS_SET((ch)->pcdata->disc[C_POWERS], (sn)))
#define IS_DEMAFF(ch, sn)	(IS_SET((ch)->pcdata->disc[C_CURRENT], (sn)))
#define IS_CLASS(ch, CLASS)     (IS_SET((ch)->class, CLASS))
#define IS_MORTAL(ch)     (IS_SET((ch)->class, CLASS_NONE))
#define IS_VAMPIRE(ch)		(IS_SET((ch)->class, CLASS_VAMPIRE))
#define IS_SUPERNATURAL(ch)		(IS_SET((ch)->class, CLASS_VAMPIRE) || IS_SET((ch)->class, CLASS_WEREWOLF))

#define IS_SHIFTER(ch)		(IS_SET((ch)->class, CLASS_WEREWOLF) ||  IS_SET((ch)->class, CLASS_SKINDANCER))

#define IS_GHOUL(ch)		(IS_SET((ch)->class, CLASS_GHOUL))
#define IS_WEREWOLF(ch)		(IS_SET((ch)->class, CLASS_WEREWOLF))
#define IS_SKINDANCER(ch)	(IS_SET((ch)->class, CLASS_SKINDANCER))
#define IS_INQUISITOR(ch)	(IS_SET((ch)->class, CLASS_INQUISITOR))


#define IS_HEAD(ch, sn)		(IS_SET((ch)->loc_hp[0], (sn)))
#define IS_BODY(ch, sn)		(IS_SET((ch)->loc_hp[1], (sn)))
#define IS_ARM_L(ch, sn)	(IS_SET((ch)->loc_hp[2], (sn)))
#define IS_ARM_R(ch, sn)	(IS_SET((ch)->loc_hp[3], (sn)))
#define IS_LEG_L(ch, sn)	(IS_SET((ch)->loc_hp[4], (sn)))
#define IS_LEG_R(ch, sn)	(IS_SET((ch)->loc_hp[5], (sn)))
#define IS_BLEEDING(ch, sn)	(IS_SET((ch)->loc_hp[6], (sn)))

#define IN_CLAN( ch )           (IS_NPC(ch)?0:ch->clan)
#define IS_PLAYING( d )         (d->connected==CON_PLAYING)

#define IS_GOOD(ch)		(ch->alignment >= 350)
#define IS_EVIL(ch)		(ch->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		(ch->position > POS_SLEEPING)
#define PC(ch,field)            ((ch)->pcdata->field)	/* OLC */
#define GET_PC(ch,field,nopc)   ((ch->pcdata != NULL ? ch->pcdata->field    \
                                                     : (nopc)) )	/* OLC */
#define GET_AC(ch)		((ch)->armor				    \
					 + ( IS_AWAKE(ch)			    \
				    ? dex_bonus[get_curr_dex(ch)].defensive   \
				    : 0 ))
#define GET_HITROLL(ch)		((ch)->hitroll+dex_bonus[get_curr_dex(ch)].tohit)
#define GET_DAMROLL(ch)		((ch)->damroll+str_bonus[get_curr_str(ch)].todam)

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
				    (ch)->in_room->room_flags,		    \
				    ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))



/*
 * Object Macros.
 */
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))

#define LEARNED(ch,sn)          ( (IS_NPC(ch) ? 0                   \
                                              : PC(ch,learned[sn])) )


/*
 * Description macros.
 */
		// old
#define PERS(ch, looker)	( can_see( looker, (ch) ) ?		\
				( IS_NPC(ch) ? (ch)->short_descr	\
				: ( IS_AFFECTED( (ch), AFF_POLYMORPH) ?   \
				(ch)->morph : (ch)->pcdata->roomdesc ) )		\
				: "someone" )
  /*
     #define PERS(ch, looker)   ( can_see( looker, (ch) ) ?             \
     ( IS_NPC(ch) ? (ch)->short_descr   \
     : ( IS_AFFECTED( (ch), AFF_POLYMORPH) ?\
     (ch)->name : (ch)->name ) )                \
     : IS_IMMORTAL(ch) ? \
     "An Immortal" :    \
     "someone" )
   */
/*
 * Structure for a command in the command lookup table.
 */
struct cmd_type
{
	char *const name;
	DO_FUN *do_fun;
	sh_int position;
	sh_int level;
	sh_int log;
};




/*
 * Structure for a social in the socials table.
 */
struct social_type
{
	char *const name;
	char *const char_no_arg;
	char *const others_no_arg;
	char *const char_found;
	char *const others_found;
	char *const vict_found;
	char *const char_auto;
	char *const others_auto;
};


/*
 * Structure for an N-social in the nsocials table.
 */

struct nsocial_type
{
	char *const name;
	char *const char_no_arg;
	char *const others_no_arg;
	char *const char_found;
	char *const others_found;
	char *const vict_found;
	char *const char_auto;
	char *const others_auto;
};


/*
 * Structure for an X-social in the xsocials table.
 */
struct xsocial_type
{
	char *const name;
	char *const char_no_arg;
	char *const others_no_arg;
	char *const char_found;
	char *const others_found;
	char *const vict_found;
	char *const char_auto;
	char *const others_auto;
	sh_int gender;
	sh_int stage;
	sh_int position;
	sh_int self;
	sh_int other;
	bool chance;
};



/*
 * Global constants.
 */

#define MAX_THAUM_POWERS 95
#define MAX_RITUAL_POWERS 29

extern const int points_distribution[3];
extern const int abilities_points_distribution[3];
extern const char *attrib_names[3][3][25];
extern const struct str_att_bonus_type str_bonus[12];
extern const struct dex_att_bonus_type dex_bonus[12];
//extern const struct wit_att_bonus_type wit_bonus[12];

extern const char *road_names[12][25];
extern const char *spec_names[MAX_SPECS][25];
extern const char *ww_breeds[BREED_MAX + 1][25];
extern const char *ww_auspices[AUSPICE_MAX + 1][25];
extern const char *ww_tribes[TRIBE_MAX + 1][25];
extern const char *ability_names[3][10][25];
extern const int ability_insight_levels[MAX_TRAINABLE_ABILITY + 1];

extern const struct cmd_type cmd_table[];
extern const struct liq_type liq_table[LIQ_MAX];
extern const struct skill_type skill_table[MAX_SKILL];
extern const struct social_type social_table[];
extern const struct nsocial_type nsocial_table[];
extern const struct xsocial_type xsocial_table[];
extern char *const dir_name[];	/* OLC */
extern const sh_int rev_dir[];	/* OLC */



/*
 * Global variables.
 */
extern HELP_DATA *help_first;
extern HELP_DATA *help_last;
extern SHOP_DATA *shop_first;
extern BAN_DATA *ban_list;
extern IMMLIST_DATA *imm_list;
extern REPORT_DATA *report_list;
extern CHAR_DATA *char_list;
extern DESCRIPTOR_DATA *descriptor_list;
extern OBJ_DATA *object_list;

extern AFFECT_DATA *affect_free;
extern BAN_DATA *ban_free;
extern CHAR_DATA *char_free;
extern DESCRIPTOR_DATA *descriptor_free;
extern EXTRA_DESCR_DATA *extra_descr_free;
extern ROOMTEXT_DATA *roomtext_free;
extern NOTE_DATA *note_free;
extern OBJ_DATA *obj_free;
extern PC_DATA *pcdata_free;
extern EXTRA *extra_free;
extern char bug_buf[];
extern time_t current_time;
extern bool fLogAll;
extern FILE *fpReserve;
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;

/*
 * Account globals
 */
extern EBAN_DATA	* eban_list;
extern EBAN_DATA	* eban_free;
extern PLAYER_INFO	* player_info_free;

/*
 * Extra global's by KaVir.
 */
extern CHAR_DATA *first_fight;
extern CHAR_DATA *last_fight;
extern char first_place[];
extern char second_place[];
extern char third_place[];
extern int score_1;
extern int score_2;
extern int score_3;
extern int world_affects;



/*
 * online creation externals  OLC
 */
extern AREA_DATA *area_first;
extern AREA_DATA *area_last;
extern SHOP_DATA *shop_last;

extern int top_affect;
extern int top_area;
extern int top_ed;
extern int top_exit;
extern int top_help;
extern int top_mob_index;
extern int top_obj_index;
extern int top_reset;
extern int top_room;
extern int top_shop;

extern int top_vnum_mob;
extern int top_vnum_obj;
extern int top_vnum_room;

extern char str_empty[1];

extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];



/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN (do_abilities);
DECLARE_DO_FUN (do_affects);
DECLARE_DO_FUN (do_afk);
DECLARE_DO_FUN (do_allow);
DECLARE_DO_FUN (do_anesthetic);
DECLARE_DO_FUN (do_animal);
DECLARE_DO_FUN (do_ansi);
DECLARE_DO_FUN (do_answer);
DECLARE_DO_FUN (do_areas);
DECLARE_DO_FUN (do_areset);
DECLARE_DO_FUN (do_artifact);
DECLARE_DO_FUN (do_ashes);
DECLARE_DO_FUN (do_at);
DECLARE_DO_FUN (do_autoexit);
DECLARE_DO_FUN (do_autoloot);
DECLARE_DO_FUN (do_autosave);
DECLARE_DO_FUN (do_awe);
DECLARE_DO_FUN (do_baal);
DECLARE_DO_FUN (do_bamfin);
DECLARE_DO_FUN (do_bamfout);
//Cell phone commands
DECLARE_DO_FUN (do_cell);
DECLARE_DO_FUN (do_call);
DECLARE_DO_FUN (do_answer);
DECLARE_DO_FUN (do_hangup);
DECLARE_DO_FUN (do_ringtone);
//end
DECLARE_DO_FUN (do_ban);
DECLARE_DO_FUN (do_bolt);
DECLARE_DO_FUN (do_beastlike);
DECLARE_DO_FUN (do_beastwithin);
DECLARE_DO_FUN (do_beckon);
DECLARE_DO_FUN (do_bind);
DECLARE_DO_FUN (do_birth);
DECLARE_DO_FUN (do_bite);
DECLARE_DO_FUN (do_blank);
DECLARE_DO_FUN (do_blindfold);
DECLARE_DO_FUN (do_bloodagony);
DECLARE_DO_FUN (do_bloodline);
DECLARE_DO_FUN (do_blur);
DECLARE_DO_FUN (do_board);
DECLARE_DO_FUN (do_bodywrack);
DECLARE_DO_FUN (do_bonecraft);
DECLARE_DO_FUN (do_brandish);
DECLARE_DO_FUN (do_brief);
DECLARE_DO_FUN (do_briansucks);
DECLARE_DO_FUN (do_bug);
DECLARE_DO_FUN (do_build);
DECLARE_DO_FUN (do_burn);
DECLARE_DO_FUN (do_burrow);
DECLARE_DO_FUN (do_buy);
//DECLARE_DO_FUN (do_call);
DECLARE_DO_FUN (do_camouflage);
DECLARE_DO_FUN (do_cchange);
DECLARE_DO_FUN (do_change);
DECLARE_DO_FUN (do_changelight);
DECLARE_DO_FUN (do_changeling);
DECLARE_DO_FUN (do_channels);
DECLARE_DO_FUN (do_chant);
DECLARE_DO_FUN (do_chop);
DECLARE_DO_FUN (do_claim);
DECLARE_DO_FUN (do_clandisc);
DECLARE_DO_FUN (do_clanname);
DECLARE_DO_FUN (do_nature);
DECLARE_DO_FUN (do_force_char_update);
DECLARE_DO_FUN (do_demeanor);
/* Merits/Flaws Stuff */
DECLARE_DO_FUN (do_give_merit);
DECLARE_DO_FUN (do_rem_merit);
DECLARE_DO_FUN (do_give_flaw);
DECLARE_DO_FUN (do_rem_flaw);
/* End Merits/Flaws Stuff */
/* Remember/Forget Stuff */
DECLARE_DO_FUN (do_remember);
DECLARE_DO_FUN (do_forget);
/* End Remember/Forget Stuff */
DECLARE_DO_FUN (do_status);
DECLARE_DO_FUN (do_setOrder);
DECLARE_DO_FUN (do_impulse);
DECLARE_DO_FUN (do_sectname);
DECLARE_DO_FUN (do_sirename);
DECLARE_DO_FUN (do_discset);
DECLARE_DO_FUN (do_clanset);
DECLARE_DO_FUN (do_got);
DECLARE_DO_FUN (do_setbreed);
DECLARE_DO_FUN (do_settribe);
DECLARE_DO_FUN (do_setauspice);
DECLARE_DO_FUN (do_nuketell);
DECLARE_DO_FUN (do_clap);
DECLARE_DO_FUN (do_claw);
DECLARE_DO_FUN (do_claws);
DECLARE_DO_FUN (do_clearclass);
DECLARE_DO_FUN (do_clearclas);
DECLARE_DO_FUN (do_clearstats);
DECLARE_DO_FUN (do_clist);
DECLARE_DO_FUN (do_cloneobject);
DECLARE_DO_FUN (do_close);
DECLARE_DO_FUN (do_command);
DECLARE_DO_FUN (do_sweetwhispers);
DECLARE_DO_FUN (do_commands);
DECLARE_DO_FUN (do_complete);
DECLARE_DO_FUN (do_conditioning);
DECLARE_DO_FUN (do_config);
DECLARE_DO_FUN (do_consent);
DECLARE_DO_FUN (do_control);
DECLARE_DO_FUN (do_copyover);
DECLARE_DO_FUN (do_coterie);
DECLARE_DO_FUN (do_cprompt);
DECLARE_DO_FUN (do_create);
DECLARE_DO_FUN (do_credits);
DECLARE_DO_FUN (do_ctalk);
DECLARE_DO_FUN (do_curse);
DECLARE_DO_FUN (do_darkheart);
DECLARE_DO_FUN (do_dazzle);
DECLARE_DO_FUN (do_delete);
DECLARE_DO_FUN (do_demondesc);
DECLARE_DO_FUN (do_deny);
DECLARE_DO_FUN (do_description);
DECLARE_DO_FUN (do_lupusdesc);
DECLARE_DO_FUN (do_crinosdesc);
DECLARE_DO_FUN (do_tempdesc);
DECLARE_DO_FUN (do_glabrodesc);
DECLARE_DO_FUN (do_giftlist);
DECLARE_DO_FUN (do_hispodesc);
DECLARE_DO_FUN (do_hedit);
DECLARE_DO_FUN (do_diagnose);
DECLARE_DO_FUN (do_dice);
DECLARE_DO_FUN (do_dig);
DECLARE_DO_FUN (do_disable);
DECLARE_DO_FUN (do_discipline);
DECLARE_DO_FUN (do_disconnect);
DECLARE_DO_FUN (do_dismount);
DECLARE_DO_FUN (do_dnd);
DECLARE_DO_FUN (do_doing);
DECLARE_DO_FUN (do_aura);
DECLARE_DO_FUN (do_down);
DECLARE_DO_FUN (do_draw);
DECLARE_DO_FUN (do_drink);
DECLARE_DO_FUN (do_drop);
DECLARE_DO_FUN (do_earthmeld);
DECLARE_DO_FUN (do_east);
DECLARE_DO_FUN (do_eat);
DECLARE_DO_FUN (do_eban);
DECLARE_DO_FUN (do_eallow);
DECLARE_DO_FUN (do_echo);
DECLARE_DO_FUN (do_empty);
DECLARE_DO_FUN (do_email);
DECLARE_DO_FUN (do_emote);
DECLARE_DO_FUN (do_recall);
DECLARE_DO_FUN (do_rpose);
DECLARE_DO_FUN (do_rpflag);
DECLARE_DO_FUN (do_rpvis);
DECLARE_DO_FUN (do_enter);
DECLARE_DO_FUN (do_embrace);
DECLARE_DO_FUN (do_entrancement);
DECLARE_DO_FUN (do_equipment);
DECLARE_DO_FUN (do_evileye);
DECLARE_DO_FUN (do_evolve);
DECLARE_DO_FUN (do_examine);
DECLARE_DO_FUN (do_exlist);
DECLARE_DO_FUN (do_exits);
DECLARE_DO_FUN (do_exp);
DECLARE_DO_FUN (do_fairescape);
DECLARE_DO_FUN (do_familiar);
DECLARE_DO_FUN (do_fangs);
DECLARE_DO_FUN (do_far);
DECLARE_DO_FUN (do_fatality);
DECLARE_DO_FUN (do_ifavour);
DECLARE_DO_FUN (do_favour);
DECLARE_DO_FUN (do_fcommand);
DECLARE_DO_FUN (do_fear);
DECLARE_DO_FUN (do_feed);
DECLARE_DO_FUN (do_fetishdoll);
DECLARE_DO_FUN (do_fill);
DECLARE_DO_FUN (do_finger);
DECLARE_DO_FUN (do_fish);
DECLARE_DO_FUN (do_flames);
DECLARE_DO_FUN (do_fleshbond);
DECLARE_DO_FUN (do_fleshcraft);
DECLARE_DO_FUN (do_flex);
DECLARE_DO_FUN (do_follow);
DECLARE_DO_FUN (do_force);
DECLARE_DO_FUN (do_forceauto);
DECLARE_DO_FUN (do_formillusion);
DECLARE_DO_FUN (do_nightshades);
DECLARE_DO_FUN (do_nuke);
DECLARE_DO_FUN (do_freeze);
DECLARE_DO_FUN (do_frenzy);
DECLARE_DO_FUN (do_gag);
DECLARE_DO_FUN (do_get);
DECLARE_DO_FUN (do_gift);
DECLARE_DO_FUN (do_gifts2);
DECLARE_DO_FUN (do_gifts3);
DECLARE_DO_FUN (do_gifts);
DECLARE_DO_FUN (do_give);
DECLARE_DO_FUN (do_givepower);
DECLARE_DO_FUN (do_givevote);
DECLARE_DO_FUN (do_goto);
DECLARE_DO_FUN (do_proll);
DECLARE_DO_FUN (do_pdice);
DECLARE_DO_FUN (do_wgoto);
DECLARE_DO_FUN (do_map);
DECLARE_DO_FUN (do_mpresets);
/* command to turn on mxp */
DECLARE_DO_FUN( do_mxp );
DECLARE_DO_FUN (do_masquerade);
DECLARE_DO_FUN (do_vision);
DECLARE_DO_FUN (do_help);
DECLARE_DO_FUN (do_hide);
DECLARE_DO_FUN (do_hoe);
DECLARE_DO_FUN (do_holylight);
DECLARE_DO_FUN (do_home);
DECLARE_DO_FUN (do_whome);
DECLARE_DO_FUN (do_horn);
DECLARE_DO_FUN (do_howl);
DECLARE_DO_FUN (do_huh);
DECLARE_DO_FUN (do_hunt);
DECLARE_DO_FUN (do_icy);
DECLARE_DO_FUN (do_idea);
DECLARE_DO_FUN (do_immtalk);
DECLARE_DO_FUN (do_infirmity);
DECLARE_DO_FUN (do_info);
DECLARE_DO_FUN (do_inform);
DECLARE_DO_FUN (do_introduce);
DECLARE_DO_FUN (do_inventory);
DECLARE_DO_FUN (do_invis);
DECLARE_DO_FUN (do_incog);
DECLARE_DO_FUN (do_invisibility);
DECLARE_DO_FUN (do_jail);
DECLARE_DO_FUN (do_knock);
DECLARE_DO_FUN (do_lamprey);
DECLARE_DO_FUN (do_gatemoon);
DECLARE_DO_FUN (do_lambent);
DECLARE_DO_FUN (do_leap);
DECLARE_DO_FUN (do_lick);
DECLARE_DO_FUN (do_list);
DECLARE_DO_FUN (do_landscape);
DECLARE_DO_FUN (do_languagelearn);
DECLARE_DO_FUN (do_lay);
DECLARE_DO_FUN (do_locate);
DECLARE_DO_FUN (do_lock);
DECLARE_DO_FUN (do_log);
DECLARE_DO_FUN (do_login);
DECLARE_DO_FUN (do_look);

DECLARE_DO_FUN (do_pcourage);
DECLARE_DO_FUN (do_plove);
DECLARE_DO_FUN (do_purchase);

DECLARE_DO_FUN (do_loyalty);
DECLARE_DO_FUN (do_majesty);
DECLARE_DO_FUN (do_mask);
DECLARE_DO_FUN (do_masssummons);
DECLARE_DO_FUN (do_mclear);
DECLARE_DO_FUN (do_meditate);
DECLARE_DO_FUN (do_memory_list);
DECLARE_DO_FUN (do_memory);
DECLARE_DO_FUN (do_memorize);
DECLARE_DO_FUN (do_mfind);
DECLARE_DO_FUN (do_mindblast);
DECLARE_DO_FUN (do_mine);
DECLARE_DO_FUN (do_mirrorimage);
DECLARE_DO_FUN (do_mload);
DECLARE_DO_FUN (do_mount);
DECLARE_DO_FUN (do_morph);
DECLARE_DO_FUN (do_mortalvamp);
DECLARE_DO_FUN (do_mother);
DECLARE_DO_FUN (do_move);
DECLARE_DO_FUN (do_mset);
DECLARE_DO_FUN (do_mstat);
DECLARE_DO_FUN (do_mwhere);
DECLARE_DO_FUN (do_neutral);
DECLARE_DO_FUN (do_nice);
DECLARE_DO_FUN (do_nightsight);
DECLARE_DO_FUN (do_noemote);
DECLARE_DO_FUN (do_north);
DECLARE_DO_FUN (do_note);
DECLARE_DO_FUN (do_notell);
DECLARE_DO_FUN (do_nsocials);
DECLARE_DO_FUN (do_obeah);
DECLARE_DO_FUN (do_obedience);
DECLARE_DO_FUN (do_oclone);
DECLARE_DO_FUN (do_ofind);
DECLARE_DO_FUN (do_fvlist);
DECLARE_DO_FUN (do_olist);
DECLARE_DO_FUN (do_rlist);
DECLARE_DO_FUN (do_roomflags);
DECLARE_DO_FUN (do_mlist);
DECLARE_DO_FUN (do_noquit);
/* New Obfuscate Code */
DECLARE_DO_FUN (do_obfuscate1);
DECLARE_DO_FUN (do_obfuscate2);
DECLARE_DO_FUN (do_obfuscate3);
DECLARE_DO_FUN (do_oload);
DECLARE_DO_FUN (do_ooc);
DECLARE_DO_FUN (do_omote);
DECLARE_DO_FUN (do_open);
DECLARE_DO_FUN (do_order);
DECLARE_DO_FUN (do_oreturn);
DECLARE_DO_FUN (do_osay);
DECLARE_DO_FUN (do_oset);
DECLARE_DO_FUN (do_ostat);
DECLARE_DO_FUN (do_oswitch);
DECLARE_DO_FUN (do_otransfer);
DECLARE_DO_FUN (do_panacea);
DECLARE_DO_FUN (do_paralyzing);
DECLARE_DO_FUN (do_password);
DECLARE_DO_FUN (do_peek);
DECLARE_DO_FUN (do_pick);
DECLARE_DO_FUN (do_pigeon);
DECLARE_DO_FUN (do_plasmaform);
DECLARE_DO_FUN (do_pload);
DECLARE_DO_FUN (do_plant);
DECLARE_DO_FUN (do_porcupine);
DECLARE_DO_FUN (do_fixcharacter);
DECLARE_DO_FUN (do_portal);
DECLARE_DO_FUN (do_possession);
DECLARE_DO_FUN (do_pour);
DECLARE_DO_FUN (do_practice);
DECLARE_DO_FUN (do_prestore);
DECLARE_DO_FUN (do_preturn);
DECLARE_DO_FUN (do_projection);
DECLARE_DO_FUN (do_prompt);
DECLARE_DO_FUN (do_prospect);
DECLARE_DO_FUN (do_protect);
DECLARE_DO_FUN (do_psay);
DECLARE_DO_FUN (do_pset);
DECLARE_DO_FUN (do_class);
DECLARE_DO_FUN (do_pstat);
DECLARE_DO_FUN (do_ptalk);
DECLARE_DO_FUN (do_purge);
DECLARE_DO_FUN (do_put);
DECLARE_DO_FUN (do_putrefaction);
DECLARE_DO_FUN (do_qset);
DECLARE_DO_FUN (do_qstat);
DECLARE_DO_FUN (do_question);
DECLARE_DO_FUN (do_qui);
DECLARE_DO_FUN (do_quit);
DECLARE_DO_FUN (do_radio);
DECLARE_DO_FUN (do_rage);
DECLARE_DO_FUN (do_renown);
DECLARE_DO_FUN (do_read);
DECLARE_DO_FUN (do_readaura);
DECLARE_DO_FUN (do_reboo);
DECLARE_DO_FUN (do_reboot);
DECLARE_DO_FUN (do_recall);
DECLARE_DO_FUN (do_recite);
//Recommend commands  (recc.c)
DECLARE_DO_FUN (do_reccomend);
DECLARE_DO_FUN (do_renown);
//end recommend commands
DECLARE_DO_FUN (do_regain);
DECLARE_DO_FUN (do_regenerate);
DECLARE_DO_FUN (do_relevel);
DECLARE_DO_FUN (do_remove);
DECLARE_DO_FUN (do_removepower);
DECLARE_DO_FUN (do_restring);
DECLARE_DO_FUN (do_rename);
DECLARE_DO_FUN (do_renew);
DECLARE_DO_FUN (do_rent);
DECLARE_DO_FUN (do_reply);
DECLARE_DO_FUN (do_reshape);
DECLARE_DO_FUN (do_rest);
DECLARE_DO_FUN (do_restore);
DECLARE_DO_FUN (do_roomdesc);
DECLARE_DO_FUN (do_brestore);
DECLARE_DO_FUN (do_vrestore);
DECLARE_DO_FUN (do_return);
DECLARE_DO_FUN (do_reward);
DECLARE_DO_FUN (do_road);
DECLARE_DO_FUN (do_roadclear);
DECLARE_DO_FUN (do_pathclear);
DECLARE_DO_FUN (do_ritual);
DECLARE_DO_FUN (do_showsocial);
DECLARE_DO_FUN (do_generation);
DECLARE_DO_FUN (do_roll);
DECLARE_DO_FUN (do_rset);
DECLARE_DO_FUN (do_rstat);
DECLARE_DO_FUN (do_sacrifice);
DECLARE_DO_FUN (do_safe);
DECLARE_DO_FUN (do_save);
DECLARE_DO_FUN (do_saw);
DECLARE_DO_FUN (do_say);
DECLARE_DO_FUN (do_score);
DECLARE_DO_FUN (do_scry);
DECLARE_DO_FUN (do_scythe);
DECLARE_DO_FUN (do_reseed);
DECLARE_DO_FUN (do_sell);
DECLARE_DO_FUN (do_serpent);
DECLARE_DO_FUN (do_set_damage);
DECLARE_DO_FUN (do_shadowbody);
DECLARE_DO_FUN (do_shadowplane);
DECLARE_DO_FUN (do_shadowsight);
DECLARE_DO_FUN (do_shadowstep);
DECLARE_DO_FUN (do_sharespirit);
DECLARE_DO_FUN (do_sheath);
DECLARE_DO_FUN (do_shell);
DECLARE_DO_FUN (do_shield);
DECLARE_DO_FUN (do_shift);
DECLARE_DO_FUN( do_short );
DECLARE_DO_FUN (do_shout);
DECLARE_DO_FUN (do_shutdow);
DECLARE_DO_FUN (do_shutdown);
DECLARE_DO_FUN (do_side);
DECLARE_DO_FUN (do_silence);
DECLARE_DO_FUN (do_silencedeath);
DECLARE_DO_FUN (do_sit);
DECLARE_DO_FUN (do_skill);
DECLARE_DO_FUN (do_sla);
DECLARE_DO_FUN (do_slay);
DECLARE_DO_FUN (do_sleep);
DECLARE_DO_FUN (do_slit);
DECLARE_DO_FUN (do_slookup);
DECLARE_DO_FUN (do_smite);
DECLARE_DO_FUN (do_soulmask);
DECLARE_DO_FUN (do_soultwist);
DECLARE_DO_FUN (do_speak);
DECLARE_DO_FUN (do_special);
DECLARE_DO_FUN (do_spell);
DECLARE_DO_FUN (do_spiriteyes);
DECLARE_DO_FUN (do_spiritform);
DECLARE_DO_FUN (do_spit);
DECLARE_DO_FUN (do_story);
DECLARE_DO_FUN (do_storyinform);
DECLARE_DO_FUN (do_string);
DECLARE_DO_FUN (do_sneak);
DECLARE_DO_FUN (do_snoop);
DECLARE_DO_FUN (do_socials);
DECLARE_DO_FUN (do_south);
DECLARE_DO_FUN (do_split);
DECLARE_DO_FUN (do_sset);
DECLARE_DO_FUN (do_stand);
DECLARE_DO_FUN (do_steal);
DECLARE_DO_FUN (do_succulence);
DECLARE_DO_FUN (do_summoning);
DECLARE_DO_FUN (do_switch);
DECLARE_DO_FUN (do_tail);
DECLARE_DO_FUN (do_teachdisc);
DECLARE_DO_FUN (do_teachgift);
DECLARE_DO_FUN (do_tell);
DECLARE_DO_FUN (do_tie);
DECLARE_DO_FUN (do_time);
DECLARE_DO_FUN (do_title);
DECLARE_DO_FUN (do_totems);
DECLARE_DO_FUN (do_track);
DECLARE_DO_FUN (do_tradition);
DECLARE_DO_FUN (do_train);
DECLARE_DO_FUN (do_transfer);
DECLARE_DO_FUN (do_tribe);
DECLARE_DO_FUN (do_tribegifts);
DECLARE_DO_FUN (do_trim);
DECLARE_DO_FUN (do_truedisguise);
DECLARE_DO_FUN (do_truesight);
DECLARE_DO_FUN (do_turn);
DECLARE_DO_FUN (do_typo);
DECLARE_DO_FUN (do_unburden);
DECLARE_DO_FUN (do_unlock);
DECLARE_DO_FUN (do_unpolymorph);
DECLARE_DO_FUN (do_untie);
DECLARE_DO_FUN (do_unveil);
DECLARE_DO_FUN (do_unwerewolf);
DECLARE_DO_FUN (do_up);
DECLARE_DO_FUN (do_upkeep);
DECLARE_DO_FUN (do_uptime);
DECLARE_DO_FUN (do_newupkeep);
DECLARE_DO_FUN (do_users);
DECLARE_DO_FUN (do_omni);
DECLARE_DO_FUN (do_pretitle);
DECLARE_DO_FUN (do_value);
DECLARE_DO_FUN (do_vampire);
DECLARE_DO_FUN (do_lambiant);
DECLARE_DO_FUN (do_vampirecreation);
DECLARE_DO_FUN (do_vanish);
DECLARE_DO_FUN (do_version);
DECLARE_DO_FUN (do_visible);
DECLARE_DO_FUN (do_voice);
DECLARE_DO_FUN (do_vote);
DECLARE_DO_FUN (do_weakness);
DECLARE_DO_FUN (do_web);
DECLARE_DO_FUN (do_wake);
DECLARE_DO_FUN (do_wall);
DECLARE_DO_FUN (do_wallgranite);
DECLARE_DO_FUN (do_wear);
DECLARE_DO_FUN (do_wearaffect);
DECLARE_DO_FUN (do_weather);
DECLARE_DO_FUN (do_werewolf);
DECLARE_DO_FUN (do_west);
DECLARE_DO_FUN (do_whererp);
DECLARE_DO_FUN (do_whisper);
DECLARE_DO_FUN (do_who);
DECLARE_DO_FUN (do_wings);
DECLARE_DO_FUN (do_wizhelp);
DECLARE_DO_FUN (do_wizlist);
DECLARE_DO_FUN (do_wizlock);
DECLARE_DO_FUN (do_wsave);
DECLARE_DO_FUN (do_wyld);
DECLARE_DO_FUN (do_xemote);
DECLARE_DO_FUN (do_xsocials);
DECLARE_DO_FUN (do_yell);
DECLARE_DO_FUN (do_zap);
DECLARE_DO_FUN (do_zuloform);
DECLARE_DO_FUN (do_path);
DECLARE_DO_FUN (do_dehydrate);
DECLARE_DO_FUN (do_powerlevels);
DECLARE_DO_FUN (do_conceal);
DECLARE_DO_FUN (do_shadowplay);
DECLARE_DO_FUN (do_resurrection);
DECLARE_DO_FUN (do_personalsafety);
DECLARE_DO_FUN (do_treat);
DECLARE_DO_FUN (do_vitaeblock);
DECLARE_DO_FUN (do_hinder);
DECLARE_DO_FUN (do_nightmaster);
//wpburn
DECLARE_DO_FUN (do_wpburn);
//done wpburn
DECLARE_DO_FUN (do_ahrimansdefense);
DECLARE_DO_FUN (do_scorpionstouch);
// spiritus
DECLARE_DO_FUN (do_wildebeast);
// demenation
DECLARE_DO_FUN (do_thoth);
DECLARE_DO_FUN (do_anubis);
DECLARE_DO_FUN (do_dawn);

// deimos
DECLARE_DO_FUN (do_unseen);
DECLARE_DO_FUN (do_mesmerise);
// mortis
DECLARE_DO_FUN (do_awake);
DECLARE_DO_FUN (do_plaguewind);
DECLARE_DO_FUN (do_blackdeath);
DECLARE_DO_FUN (do_deathswhisper);
DECLARE_DO_FUN (do_reaperspassing);
DECLARE_DO_FUN (do_blight);
DECLARE_DO_FUN (do_masque);
DECLARE_DO_FUN (do_quell);
DECLARE_DO_FUN (do_crimsonfury);
DECLARE_DO_FUN (do_senseemotion);
DECLARE_DO_FUN (do_agony);
DECLARE_DO_FUN (do_lovingagony);
DECLARE_DO_FUN (do_madness);
DECLARE_DO_FUN (do_sensorydeprive);
DECLARE_DO_FUN (do_faerieland);
DECLARE_DO_FUN (do_tranquility);
DECLARE_DO_FUN (do_cache);
DECLARE_DO_FUN (do_obvious);
DECLARE_DO_FUN (do_obvious);
DECLARE_DO_FUN (do_darkeyes);
DECLARE_DO_FUN (do_darksight);
DECLARE_DO_FUN (do_shadowslave);
DECLARE_DO_FUN (do_invokefrenzy);
DECLARE_DO_FUN (do_shapemastery);
DECLARE_DO_FUN (do_homunculus);
DECLARE_DO_FUN (do_acidblood);

/* bastet powers */

DECLARE_DO_FUN (do_bshift);
DECLARE_DO_FUN (do_perfect);
DECLARE_DO_FUN (do_surge);
DECLARE_DO_FUN (do_banishburning);
DECLARE_DO_FUN (do_dragonsroar);
DECLARE_DO_FUN (do_bayoushambler);
DECLARE_DO_FUN (do_thunderbolt);
DECLARE_DO_FUN (do_stonework);
DECLARE_DO_FUN (do_healingmeditation);
DECLARE_DO_FUN (do_betweenworlds);
DECLARE_DO_FUN (do_flight);
/* Bastet */

DECLARE_DO_FUN (do_bastetteach);
DECLARE_DO_FUN (do_bastetlearn);
DECLARE_DO_FUN (do_bastetgifts);


/* rokea  */
DECLARE_DO_FUN( do_rokeaselect); //selects
DECLARE_DO_FUN( do_rokeateach); //duh
DECLARE_DO_FUN( do_sshift);


/* kitsune */
DECLARE_DO_FUN(do_kitsunegifts);
DECLARE_DO_FUN(do_kitsunelearn);
DECLARE_DO_FUN(do_kitsuneteach);
DECLARE_DO_FUN(do_kitsunepowers);
DECLARE_DO_FUN(do_kshift);
DECLARE_DO_FUN(do_paperspy);
DECLARE_DO_FUN(do_bushhide);
DECLARE_DO_FUN(do_carrionclothes);
DECLARE_DO_FUN(do_callallies);
DECLARE_DO_FUN(do_sealofinari);
DECLARE_DO_FUN(do_spiritdrain);
DECLARE_DO_FUN(do_graspthebeyond);
DECLARE_DO_FUN(do_fanshadowrobe);
DECLARE_DO_FUN(do_shadowfanflower);
DECLARE_DO_FUN(do_moonfanfaceshadow);
DECLARE_DO_FUN(do_distractions);
DECLARE_DO_FUN(do_wdistractions);
DECLARE_DO_FUN(do_fabrication);
DECLARE_DO_FUN(do_paper);
DECLARE_DO_FUN(do_update_weather_file);




 //various things thats for vampire disciplines
DECLARE_DO_FUN (do_temporis);
DECLARE_DO_FUN (do_tpermanency);
DECLARE_DO_FUN (do_permanency);
DECLARE_DO_FUN (do_blissfulagony);
DECLARE_DO_FUN (do_burningtouch);
DECLARE_DO_FUN (do_endwatch);
DECLARE_DO_FUN (do_zcurse);

DECLARE_DO_FUN (do_cshift);
DECLARE_DO_FUN (do_nshift);
DECLARE_DO_FUN (do_cgifts);
DECLARE_DO_FUN (do_ngifts);
DECLARE_DO_FUN (do_gshift);
DECLARE_DO_FUN (do_ggifts);
DECLARE_DO_FUN (do_rshift);
DECLARE_DO_FUN (do_rgifts);
DECLARE_DO_FUN (do_mgifts);
DECLARE_DO_FUN (do_mshift);
DECLARE_DO_FUN (do_nwgifts);
DECLARE_DO_FUN (do_nwshift);
DECLARE_DO_FUN (do_agifts);
DECLARE_DO_FUN (do_ashift);
/*
 * Online creation commands  OLC
 */
DECLARE_DO_FUN (do_ashow);
DECLARE_DO_FUN (do_rshow);
DECLARE_DO_FUN (do_oshow);
DECLARE_DO_FUN (do_mshow);

DECLARE_DO_FUN (do_aedit);
DECLARE_DO_FUN (do_redit);
DECLARE_DO_FUN (do_oedit);
DECLARE_DO_FUN (do_medit);
DECLARE_DO_FUN (do_mpedit);
DECLARE_DO_FUN (do_mpgold);

DECLARE_DO_FUN (do_resets);

DECLARE_DO_FUN (do_asave);

DECLARE_DO_FUN (do_arealist);



//      Mob programs Place the following into merc.h with all the other DO_FUN's:

DECLARE_DO_FUN (do_mpasound);
DECLARE_DO_FUN (do_mpat);
DECLARE_DO_FUN (do_mpecho);
DECLARE_DO_FUN (do_mpechoaround);
DECLARE_DO_FUN (do_mpechoat);
DECLARE_DO_FUN (do_mpforce);
DECLARE_DO_FUN (do_mpgoto);
DECLARE_DO_FUN (do_mpjunk);
DECLARE_DO_FUN (do_mpkill);
DECLARE_DO_FUN (do_mpmload);
DECLARE_DO_FUN (do_mpoload);
DECLARE_DO_FUN (do_mppurge);
DECLARE_DO_FUN (do_mpstat);
DECLARE_DO_FUN (do_mptransfer);

DECLARE_SPELL_FUN (spell_null);

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if	defined(_AIX)
char *crypt args ((const char *key, const char *salt));
#endif

#if	defined(apollo)
int atoi args ((const char *string));
void *calloc args ((unsigned nelem, size_t size));
char *crypt args ((const char *key, const char *salt));
#endif

#if	defined(hpux)
char *crypt args ((const char *key, const char *salt));
#endif

#if	defined(interactive)
#endif

#if	defined(linux)
char *crypt args ((const char *key, const char *salt));
#endif

#if	defined(macintosh)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(MIPS_OS)
char *crypt args ((const char *key, const char *salt));
#endif

#if	defined(MSDOS)
#define NOCRYPT
#if	defined(unix)
#undef	unix
#endif
#endif

#if	defined(NeXT)
char *crypt args ((const char *key, const char *salt));
#endif

#if	defined(sequent)
char *crypt args ((const char *key, const char *salt));
int fclose args ((FILE * stream));
int fprintf args ((FILE * stream, const char *format, ...));
int fread args ((void *ptr, int size, int n, FILE * stream));
int fseek args ((FILE * stream, long offset, int ptrname));
void perror args ((const char *s));
int ungetc args ((int c, FILE * stream));
#endif

#if	defined(sun)
char *crypt args ((const char *key, const char *salt));
int fclose args ((FILE * stream));
int fprintf args ((FILE * stream, const char *format, ...));
#if 	defined(SYSV)
size_t fread args ((void *ptr, size_t size, size_t n, FILE * stream));
#else
int fread args ((void *ptr, int size, int n, FILE * stream));
#endif
int fseek args ((FILE * stream, long offset, int ptrname));
void perror args ((const char *s));
int ungetc args ((int c, FILE * stream));
#endif

#if	defined(ultrix)
char *crypt args ((const char *key, const char *salt));
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if	defined(NOCRYPT)
#define crypt(s1, s2)	(s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR	""	/* Player files                 */
#define NULL_FILE	"proto.are"	/* To reserve one stream        */
#define MOB_DIR		""	/* MOBProg files                */
#endif

#if defined(MSDOS)
#define PLAYER_DIR	""	/* Player files                 */
#define NULL_FILE	"nul"	/* To reserve one stream        */
#define MOB_DIR		""	/* MOBProg files                */
#endif

#if defined(unix)
#define MOB_DIR		"MOBProgs/"	/* MOBProg files                */
#define PLAYER_DIR	"../player/"	/* Player files                 */
#define NULL_FILE	"/dev/null"	/* To reserve one stream        */
#define LAST_COMMAND	"../last_command.txt" /* signal handler */
#endif

#define PLAYER_BAK_DIR  "../player.bak/"/* Player Backup files */
#define AREA_LIST	"area.lst"	/* List of areas                */
#define CLAN_LIST	"clan1.txt"	/* List of clans                */
#define ART_LIST	"art1.txt"	/* List of artifacts            */
#define HOME_AREA	"../area/homes.are"	/* Mages towers, etc        */

#define BUG_FILE	"bugs.txt"	/* For 'bug' and bug( )         */
#define IDEA_FILE	"ideas.txt"	/* For 'idea'                   */
#define TYPO_FILE	"typos.txt"	/* For 'typo'                   */
#define NOTE_FILE	"notes.txt"	/* For 'notes'                  */
#define SHUTDOWN_FILE	"shutdown.txt"	/* For 'shutdown'               */
#define EBAN_FILE	"banned_emails.txt" /* Email bans */


/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN

/* act_comm.c */
void add_follower args ((CHAR_DATA * ch, CHAR_DATA * master));
void stop_follower args ((CHAR_DATA * ch));
void die_follower args ((CHAR_DATA * ch));
bool is_same_group args ((CHAR_DATA * ach, CHAR_DATA * bch));
void room_text args ((CHAR_DATA * ch, char *argument));
char *strlower args ((char *ip));
void excessive_cpu args ((int blx));
bool check_parse_name args ((char *name));
bool is_in args ((char *, char *));
bool all_in args ((char *, char *));
void log_reward args ((CHAR_DATA * ch, int reward));
bool check_banned args ((DESCRIPTOR_DATA *dnew));
void recycle_descriptors args (());
void recycle_dummys args (());

/* act_info.c */
char * display_extra args ((CHAR_DATA *ch));
void set_title args ((CHAR_DATA * ch, char *title));
void show_list_to_char args ((OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing));
int char_str args ((CHAR_DATA * ch));
int char_dex args ((CHAR_DATA * ch));
int char_sta args ((CHAR_DATA * ch));
int char_cha args ((CHAR_DATA * ch));
int char_int args ((CHAR_DATA * ch));
int char_per args ((CHAR_DATA * ch));
int stat_cost args ((CHAR_DATA * ch, int whichstat));
int get_celerity args ((CHAR_DATA * ch));
int get_disc args ((CHAR_DATA * ch, int disc));
int get_truedisc args ((CHAR_DATA * ch, int disc));
int get_breed args ((CHAR_DATA * ch, int disc));
int get_auspice args ((CHAR_DATA * ch, int disc));
int get_tribe args ((CHAR_DATA * ch, int disc));

/* act_move.c */
void move_char args ((CHAR_DATA * ch, int door));
void open_lift args ((CHAR_DATA * ch));
void close_lift args ((CHAR_DATA * ch));
void move_lift args ((CHAR_DATA * ch, int to_room));
void move_door args ((CHAR_DATA * ch));
void thru_door args ((CHAR_DATA * ch, int doorexit));
void open_door args ((CHAR_DATA * ch, bool be_open));
bool is_open args ((CHAR_DATA * ch));
bool same_floor args ((CHAR_DATA * ch, int cmp_room));
void check_hunt args ((CHAR_DATA * ch));

/* act_obj.c */
bool is_ok_to_wear args ((CHAR_DATA * ch, bool wolf_ok, char *argument));
bool remove_obj args ((CHAR_DATA * ch, int iWear, bool fReplace));
void wear_obj args ((CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace));
bool fifty_objects args ((CHAR_DATA * ch));
bool hundred_objects args ((CHAR_DATA * ch, OBJ_DATA * container));
void set_disc args ((CHAR_DATA * ch));
void strip_disc args ((CHAR_DATA * ch));
void mortal_vamp args ((CHAR_DATA * ch));

/* aw.c */
void bind_char args ((CHAR_DATA * ch));
void logchan args ((char *argument, int bit));
void sysmes args ((char *argument));
void save_banlist args (());
void load_banlist args (());
ROOM_INDEX_DATA *find_location (CHAR_DATA * ch, char *arg);	/* OLC */
bool has_merit args ((CHAR_DATA * ch, char * merit));
bool has_flaw args ((CHAR_DATA * ch, char * flaw));
int find_merit args ((CHAR_DATA * ch, char * merit_name));
int find_flaw args ((CHAR_DATA * ch, char * flaw_name));

/* memory.c */
MEMORY_DATA * memory_search_real_name args ((MEMORY_DATA * memory_start, char * search_name));
void remove_memory_node args ((CHAR_DATA * ch, MEMORY_DATA * node_to_remove));
void add_memory_from_name args ((CHAR_DATA * ch, char * real_name, char * remembered_name));
CHAR_DATA * get_char_room_from_desc args ((CHAR_DATA * ch, char * argument));
CHAR_DATA * get_char_room_from_alias args ((CHAR_DATA * ch, char * argument));
CHAR_DATA * get_char_world_alias(CHAR_DATA * ch, char *argument);
CHAR_DATA * get_char_world_desc(CHAR_DATA * ch, char *argument);

/* recc.c */
void add_pose_recc args((char *giver, char *receiver, char *text));
void add_renown_recc args((char *giver, char *receiver, int amount, int type,  char *text));
int remove_pose_recc args((int index));
int remove_renown_recc args((int index));
POSE_RECC *get_pose_recc(int index);
RENOWN_RECC *get_renown_recc(int index);
void save_pose_reccs();
void save_renown_reccs();
void load_pose_reccs();
void load_renown_reccs();



/* system.c */
void hedit args ((CHAR_DATA * ch, char *argument));
void mpedit args ((CHAR_DATA * ch, char *argument));
void sing_mpedit args ((CHAR_DATA * ch, char *argument));

/* board.c */
void finish_note args ((BOARD_DATA * board, NOTE_DATA * note));
NOTE_DATA *new_note args (());

/* comm.c */
void init_char	args (( DESCRIPTOR_DATA *d ) );
void close_socket args ((DESCRIPTOR_DATA * dclose));
void close_socket2 args ((DESCRIPTOR_DATA * dclose));
void write_to_buffer args ((DESCRIPTOR_DATA * d, const char *txt, int length));
void send_to_char args ((const char *txt, CHAR_DATA * ch));
void ch_printf args ((CHAR_DATA * ch, char *fmt, ...));
void send_to_all_char args ((const char *text));
void act args ((const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type));
void act2 args ((const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type));
void kavitem args ((const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type));
void crashrecov args ((int));
void dump_last_command args (());
void    page_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void show_string args( (struct descriptor_data *d, char *input));

/* db.c */
MPROG_DATA *mprog_file_read args ((char *f, MPROG_DATA *mprg, MOB_INDEX_DATA *pMobIndex));
void boot_db args ((bool fCopyOver));
void area_update args ((void));
CD *create_mobile args ((MOB_INDEX_DATA * pMobIndex));
OD *create_object args ((OBJ_INDEX_DATA * pObjIndex, int level));
void clear_char args ((CHAR_DATA * ch));
void free_char args ((CHAR_DATA * ch));
void free_extra args((EXTRA *extra));
char *get_extra_descr args ((const char *name, EXTRA_DESCR_DATA * ed));
char *get_roomtext args ((const char *name, ROOMTEXT_DATA * rt));
MID *get_mob_index args ((int vnum));
OID *get_obj_index args ((int vnum));
RID *get_room_index args ((int vnum));
char fread_letter args ((FILE * fp));
int fread_number args ((FILE * fp));
char *fread_string args ((FILE * fp));
void fread_to_eol args ((FILE * fp));
char *fread_word args ((FILE * fp));
void *alloc_mem args ((int sMem));
void *alloc_perm args ((int sMem));
void free_mem args ((void *pMem, int sMem));
char *str_dup args ((const char *str));
void free_string args ((char *pstr));
int number_fuzzy args ((int number));
int number_range args ((int from, int to));
int number_percent args ((void));
int number_percent2 args ((void));
int number_door args ((void));
int number_bits args ((int width));
int number_mm args ((void));
int dice args ((int number, int size));
int interpolate args ((int level, int value_00, int value_32));
void smash_tilde args ((char *str));
bool str_cmp args ((const char *astr, const char *bstr));
bool str_prefix args ((const char *astr, const char *bstr));
bool str_infix args ((const char *astr, const char *bstr));
bool str_suffix args ((const char *astr, const char *bstr));
char *capitalize args ((const char *str));
char *upper_cap args ((const char *str));
void append_file args ((CHAR_DATA * ch, char *file, char *str));
void bug args ((const char *str, int param));
void log_string args ((const char *str, int bit));
void log_string_quiet args ((const char *str, int bit));
void save_eban_file args ((void));
void tail_chain args ((void));
void reset_area args ((AREA_DATA * pArea));	/* OLC */

/* fight.c */
void damage args ((CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt));
void update_pos args ((CHAR_DATA * victim));
void stop_fighting args ((CHAR_DATA * ch, bool fBoth));
bool is_safe args ((CHAR_DATA * ch, CHAR_DATA * victim));

/* handler.c */

char *room_bit_name args ((int room_flags));
char *exitflag_bit_name args ((int exit_flags));
char *room_sector args ((int sector));
int strlen_noansi args ((char *str));
int diceroll args ((CHAR_DATA * ch, int attribute, int ability_are, int ability, int difficulty));
int count_ability_levels args ((int area, CHAR_DATA * ch));
int get_trust args ((CHAR_DATA * ch));
int get_age args ((CHAR_DATA * ch));
int get_hours args ((CHAR_DATA * ch));
int get_hoursIC args ((CHAR_DATA * ch));
int get_curr_str args ((CHAR_DATA * ch));	// Get characters current strength
int get_curr_dex args ((CHAR_DATA * ch));	// Get characters current dexterity
int get_curr_sta args ((CHAR_DATA * ch));	// Get characters current stamina
int get_curr_cha args ((CHAR_DATA * ch));	// Get characters current charisma
int get_curr_man args ((CHAR_DATA * ch));	// Get characters current manipulation
int get_curr_app args ((CHAR_DATA * ch));	// Get characters current appearance
int get_curr_per args ((CHAR_DATA * ch));	// Get characters current perception
int get_curr_int args ((CHAR_DATA * ch));	// Get characters current intelligence
int get_curr_wit args ((CHAR_DATA * ch));	// Get characters current wits

int can_carry_n args ((CHAR_DATA * ch));
int can_carry_w args ((CHAR_DATA * ch));
bool is_name args ((const char *str, char *namelist));
bool is_names args ((char *name1, char *name2));

bool is_abbr args ((const char *str, char *namelist));
int loc_name args ((const char *str, char *namelist));
bool is_full_name args ((const char *str, char *namelist));
void affect_to_char args ((CHAR_DATA * ch, AFFECT_DATA * paf));
void affect_remove args ((CHAR_DATA * ch, AFFECT_DATA * paf));
void affect_strip args ((CHAR_DATA * ch, int sn));
bool is_affected args ((CHAR_DATA * ch, int sn));
void affect_join args ((CHAR_DATA * ch, AFFECT_DATA * paf));
void char_from_room args ((CHAR_DATA * ch));
void char_to_room args ((CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex));
void obj_to_char args ((OBJ_DATA * obj, CHAR_DATA * ch));
void obj_from_char args ((OBJ_DATA * obj));
int apply_ac args ((OBJ_DATA * obj, int iWear));
OD *get_eq_char args ((CHAR_DATA * ch, int iWear));
void equip_char args ((CHAR_DATA * ch, OBJ_DATA * obj, int iWear));
void unequip_char args ((CHAR_DATA * ch, OBJ_DATA * obj));
int count_obj_list args ((OBJ_INDEX_DATA * obj, OBJ_DATA * list));
void obj_from_room args ((OBJ_DATA * obj));
void obj_to_room args ((OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex));
void obj_to_obj args ((OBJ_DATA * obj, OBJ_DATA * obj_to));
void obj_from_obj args ((OBJ_DATA * obj));
void extract_obj args ((OBJ_DATA * obj));
void extract_char args ((CHAR_DATA * ch, bool fPull));
CD *get_char args ((CHAR_DATA * ch));
CD *get_char_room args ((CHAR_DATA * ch, char *argument));
CD *get_char_world args ((CHAR_DATA * ch, char *argument));
CD *get_char_world2 args ((CHAR_DATA * ch, char *argument));
CD *get_char_finger args ((CHAR_DATA * ch, char *argument));
OD *get_obj_type args ((OBJ_INDEX_DATA * pObjIndexData));
OD *get_obj_list args ((CHAR_DATA * ch, char *argument, OBJ_DATA * list));
OD *get_obj_in_obj args ((CHAR_DATA * ch, char *argument));
OD *get_obj_carry args ((CHAR_DATA * ch, char *argument));
OD *get_obj_wear args ((CHAR_DATA * ch, char *argument));
OD *get_obj_here args ((CHAR_DATA * ch, char *argument));
OD *get_obj_room args ((CHAR_DATA * ch, char *argument));
OD *get_obj_world args ((CHAR_DATA * ch, char *argument));
OD *get_obj_world2 args ((CHAR_DATA * ch, char *argument));
OD *create_money args ((int amount));
int get_obj_number args ((OBJ_DATA * obj));
int get_obj_weight args ((OBJ_DATA * obj));
bool room_is_dark args ((ROOM_INDEX_DATA * pRoomIndex));
bool room_has_shadows args ((ROOM_INDEX_DATA * pRoomIndex));
bool room_is_private args ((ROOM_INDEX_DATA * pRoomIndex));
bool can_see args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool can_sense args ((CHAR_DATA * ch, CHAR_DATA * victim));
bool can_see_obj args ((CHAR_DATA * ch, OBJ_DATA * obj));
bool can_drop_obj args ((CHAR_DATA * ch, OBJ_DATA * obj));
char *affect_loc_name args ((int location));
char *affect_bit_name args ((int vector));
void affect_modify args ((CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd));
int str_close args ((char *a, char *b));
void mage_format args ((char *arg));
int PERCENTAGE args ((int amount, int max));
bool skill_check args ((CHAR_DATA * ch, int sn, int modifier));

bool is_number_special args ((const char *arg));
int skill_lookup args ((const char *name));
int slot_lookup args ((int slot));
int sn_lookup args ((int sn));

/* interp.c */
void parse_rp args ((CHAR_DATA * ch, char *argument));
int count_words args ((char *string));
void interpret args ((CHAR_DATA * ch, char *argument));
bool is_number args ((char *arg));
int number_argument args ((char *argument, char *arg));
char *one_argument args ((char *argument, char *arg_first));
char *one_argument_retain_cap  args ((char *argument, char *arg_first));
void stage_update args ((CHAR_DATA * ch, CHAR_DATA * victim, int stage));
void load_disabled args (());

/* save.c */
void save_char_obj args ((CHAR_DATA * ch));
bool load_char_obj args ((DESCRIPTOR_DATA * d, char *name));
bool load_char_short args ((DESCRIPTOR_DATA * d, char *name));
void backup_char_obj args ((CHAR_DATA * ch));
/* special.c */
SF *spec_lookup args ((const char *name));

/* update.c */
void update_handler args ((void));
bool should_gain_exp args ((CHAR_DATA * ch));

/* kav_info.c */
void birth_date args ((CHAR_DATA * ch, bool is_self));
void other_age args ((CHAR_DATA * ch, int extra, bool is_preg, char *argument));
int years_old args ((CHAR_DATA * ch));
void clear_eq args ((CHAR_DATA * ch));
bool play_yet args ((CHAR_DATA * ch));
int date_to_int args ((char *argument));

/* kav_wiz.c */
void oset_affect args ((CHAR_DATA * ch, OBJ_DATA * obj, int value, int affect, bool is_quest));

/* clan.c */
void vamp_create args ((CHAR_DATA * ch, char *argument));
void werewolf_regen args ((CHAR_DATA * ch));
void part_reg args ((CHAR_DATA * ch));
void reg_mend args ((CHAR_DATA * ch));
void vamp_rage args ((CHAR_DATA * ch));
bool char_exists args ((int file_dir, char *argument));
OD *get_page args ((OBJ_DATA * book, int page_num));
void update_gen args ((CHAR_DATA * ch));
int true_generation args ((CHAR_DATA * ch, char *argument));
void power_ghoul args ((CHAR_DATA * ch, CHAR_DATA * victim));
char *disc_name args ((int disc_no));

/* string.c */
char *format_string args ((char *oldstring /*, bool fSpace */ ));	/* OLC */
void string_edit args ((CHAR_DATA * ch, char **pString));	/* OLC */
void string_add args ((CHAR_DATA * ch, char *argument));	/* OLC */
void string_append args ((CHAR_DATA * ch, char **pString));	/* OLC */
char *first_arg args ((char *argument, char *arg_first, bool fCase));	/* OLC */
char *string_replace args ((char *orig, char *old, char *new));	/* OLC */
char *string_proper args ((char *argument));	/* OLC */
char *string_unpad args ((char *argument));	/* OLC */
int arg_count args ((char *argument));	/* OLC */
char *substr args ((char *sourceString, int startPos, int endPos)); /* text stuff */
int getSubStrPos args ((char *sourceString, char *searchString)); /* text stuff */
int indexOf args ((char *sourceString, char argument)); /* text stuff */
int countArgument args ((char *sourceString, char argument)); /* text stuff */

/* weather.c */
void update_weather args ((void));

/* olc.c */
void aedit args ((CHAR_DATA * ch, char *argument));	/* OLC */
void redit args ((CHAR_DATA * ch, char *argument));	/* OLC */
void medit args ((CHAR_DATA * ch, char *argument));	/* OLC */
void oedit args ((CHAR_DATA * ch, char *argument));	/* OLC */
void add_reset args ((AREA_DATA * area, RESET_DATA * pReset, int index));	/* OLC */

/* bit.c */
char *room_bit_name args ((int room_flags));	/* OLC */
int room_name_bit args ((char *buf));	/* OLC */
int sector_number args ((char *argument));	/* OLC */
char *path_names (int path);
char *sector_name args ((int sect));	/* OLC */
int item_name_type args ((char *name));	/* OLC */
char *item_type_name args ((int item_type));	/* OLC */
int extra_name_bit args ((char *buf));	/* OLC */
char *extra_bit_name args ((int extra_flags));	/* OLC */
int wear_name_bit args ((char *buf));	/* OLC */
char *wear_bit_name args ((int wear));	/* OLC */
int act_name_bit args ((char *buf));	/* OLC */
int affect_name_bit args ((char *buf));	/* OLC */
int affect_name_loc args ((char *name));	/* OLC */
int wear_name_loc args ((char *buf));	/* OLC */
char *wear_loc_name args ((int wearloc));	/* OLC */
char *act_bit_name args ((int act));	/* OLC */
int get_weapon_type args ((char *arg));	/* OLC */
char *get_type_weapon args ((int arg));	/* OLC */
int get_container_flags args ((char *arg));	/* OLC */
char *get_flags_container args ((int arg));	/* OLC */
int get_liquid_type args ((char *arg));	/* OLC */
char *get_type_liquid args ((int arg));	/* OLC */


#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF

#define DISABLED_FILE "disabled.txt"	/* disabled commands */

typedef struct disabled_data DISABLED_DATA;

/* one disabled command */
struct disabled_data
{
	DISABLED_DATA *next;	/* pointer to next node */
	struct cmd_type const *command;	/* pointer to the command struct */
	char *disabled_by;	/* name of disabler */
	sh_int level;		/* level of disabler */
};
extern DISABLED_DATA *disabled_first;


#define BAN_FILE "ban.txt"	/* ban files */
#define IMMLIST_FILE "immlistfile.txt"	/* dynamic immlist file */
#define REWARD_FILE  "rewards.txt"	/* logging of autorewarder */

#define COPYOVER_FILE "copyover.data"

#define EXE_FILE "../src/asm"

#define CRASH_TEMP_FILE	"crashtmp.txt"


extern char *last_command;
extern int iDelete;


/* mxp.c */
void  mxp_to_char               ( CHAR_DATA *ch, char *txt, int mxp_style );
void  shutdown_mxp              ( DESCRIPTOR_DATA *d );
void  init_mxp                  ( DESCRIPTOR_DATA *d );

/* mxp macro */
#define USE_MXP(ch)             (IS_SET(ch->act, PLR_MXP) && ch->desc->mxp)



void do_yomihalucinations (CHAR_DATA * ch);
void do_whispersofthesoul (CHAR_DATA * ch);	//more2.c

void do_language args ((CHAR_DATA * ch, char *argument, int language));



//mob program global declarations
ROOM_INDEX_DATA *find_location args ((CHAR_DATA * ch, char *arg));

/* mob_prog.c */
#ifdef DUNNO_STRSTR
char *strstr args ((const char *s1, const char *s2));
#endif

char *mprog_type_to_name	args (( int type ));
int mprog_name_to_type		args (( char *name ));
void mprog_wordlist_check args ((char *arg, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void *vo, int type));
void mprog_percent_check args ((CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void *vo, int type));
void mprog_act_trigger args ((char *buf, CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj, void *vo));
void mprog_bribe_trigger args ((CHAR_DATA * mob, CHAR_DATA * ch, int amount));
void mprog_entry_trigger args ((CHAR_DATA * mob));
void mprog_give_trigger args ((CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj));
void mprog_greet_trigger args ((CHAR_DATA * mob));
void mprog_fight_trigger args ((CHAR_DATA * mob, CHAR_DATA * ch));
void mprog_death_trigger args ((CHAR_DATA * mob));
void mprog_random_trigger args ((CHAR_DATA * mob));
void mprog_speech_trigger args ((char *txt, CHAR_DATA * mob));
void mprog_buy_trigger (CHAR_DATA * mob, CHAR_DATA * ch);

void area_map (CHAR_DATA * ch);
int isname (const char *str, const char *namelist);
int string_contains_at args ((char *check, char *against));

/* thaum stuff */
int get_path2 (CHAR_DATA * ch, int disc);

#define HEALTH_NORMAL		7
#define HEALTH_BRUISED		6
#define HEALTH_HURT		5
#define HEALTH_INJURED		4
#define HEALTH_WOUNDED		3
#define HEALTH_MAULED		2
#define HEALTH_CRIPPLED		1
#define HEALTH_INCAPACITATED	0





/*
 * Tool types., also for wilderness..used for creation in thaum.c
 */
#define TOOL_PEN		      1
#define TOOL_PLIERS		      2
#define TOOL_SCALPEL		      3
#define TOOL_FISHING_ROD	      4
#define TOOL_SHOVEL		      5
#define TOOL_PICK		      6
#define TOOL_HAMMER		      7
#define TOOL_NAILS		      8
#define TOOL_SAW		      9
#define TOOL_HOE		     10
#define TOOL_SEEDS		     11
#define TOOL_SCYTHE		     12

#define MAX_ROADS		12
#define ROAD_BEAST		0
#define ROAD_BLOOD		1
#define ROAD_KING		2
#define ROAD_SIN		3
#define ROAD_HEAVEN		4
#define ROAD_HUMANITY		5
#define ROAD_PARADOX		6
#define ROAD_SERPENT		7
#define ROAD_BONES		8
#define ROAD_SLAIN		9
#define ROAD_NIGHT		10
#define ROAD_METAMORPHOSIS	11

#define MAX_CREATION_VIRTUE_POINTS	7
#define VIRTUE_CONSCIENCE	0
#define VIRTUE_SELFCONTROL	1
#define VIRTUE_COURAGE		2
#define VIRTUE_ZEAL		3
#define VIRTUE_FAITH		4
#define VIRTUE_WISDOM		5




#define MAX_PLANE		18



#define GAROU_GNOSIS	0
#define GAROU_MAXGNOSIS	1

#define GAROU_RAGE	0
#define GAROU_MAXRAGE	1

#define WILLPOWER_CURRENT	0
#define WILLPOWER_MAX		1
#define MAX_TRAINABLE_WILLPOWER	10

#define CONVICTION_CURRENT	 0
#define CONVICTION_MAX		 1
#define	MAX_TRAINABLE_CONVICTION 10

#define TEMPGLORY 0
#define PERMGLORY 1
#define TEMPHONOR 2
#define PERMHONOR 3
#define TEMPWISDOM 4
#define PERMWISDOM 5


#define RANK_PUP 0
#define RANK_CLIATH 1
#define RANK_FOSTERN 2
#define RANK_ADREN 3
#define RANK_ATHRO 4
#define RANK_ELDER 5

#define HOMID		      0
#define LUPUS		      1
#define METIS		      2
#define RAGABASH	      3
#define THEURGE		   4
#define PHILODOX	      5
#define GALLIARD	      6
#define AHROUN		      7
#define BLACK_FURIES	   8
#define BONE_GNAWERS	   9
#define CHILDREN_OF_GAIA	10
#define FIANNA		        11
#define GET_OF_FENRIS	  12
#define GLASS_WALKERS	  13
#define RED_TALONS	     14
#define SHADOW_LORDS	      15
#define SILENT_STRIDERS	  16
#define SILVER_FANGS	      17
#define STARGAZERS	      18
#define UKTENA		      19
#define WENDIGOS		      20
#define BLACKSPIRALDANCERS		      21


#define HOMID_FIREMASTER		      1
#define HOMID_PERSUASION		      2
#define HOMID_MANSMELL		      4
#define HOMID_WORLDSPEECH	      8
#define HOMID_STAREDOWN		   16
#define HOMID_DISQUIET	      32
#define HOMID_TONGUES	      64
#define HOMID_COCOON		    128
#define HOMID_SPIRITWARD		    256
#define HOMID_ASSIMILATION		    512
#define HOMID_PARTHEVEIL		   1024


#define LUPUS_HEIGHTEN		      1
#define LUPUS_LEAP		      2
#define LUPUS_SCENTOFSIGHT		      4
#define LUPUS_SENSEUNATURAL	      8
#define LUPUS_CATFEET		   16
#define LUPUS_SPIRITNAME	      32
#define LUPUS_MONKEYTAIL	      64
#define LUPUS_BEASTLIFE		    128
#define LUPUS_GNAW		    256
#define LUPUS_ELEMENTALGIFT		    512
#define LUPUS_GREATBEAST		   1024


#define METIS_SHED		      1
#define METIS_CREATEELEMENT		      2
#define METIS_SENSEWYRM		      4
#define METIS_BURROW	      8
#define METIS_HATRED		   16
#define METIS_CATSEYES	      32
#define METIS_MENTALSPEECH	      64
#define METIS_SPLINTERCLAW		    128
#define METIS_PORCUPINE		    256
#define METIS_WITHERLIMB		    512
#define METIS_MADNESS		   1024
#define METIS_TOTEMGIFT		   2048


#define AHROUN_FALLINGTOUCH		      1
#define AHROUN_INSPIRATION		      2
#define AHROUN_SALAMANDER		      4
#define AHROUN_SENSESILVER	      8
#define AHROUN_FRAY		   16
#define AHROUN_TRUEFEAR	      32
#define AHROUN_FURY	      64
#define AHROUN_SCLAWS		    128
#define AHROUN_HEALING		    256
#define AHROUN_SHATTER		    512
#define AHROUN_CLENCHED		   1024

#define AHROUN_FURNACE		   2048
#define AHROUN_HELIOS		   4096
#define AHROUN_WILL		   8192
#define AHROUN_RAZORCLAWS		  16384


#define GALLIARD_BEASTSPEECH		      1
#define GALLIARD_WYLD		      2
#define GALLIARD_MINDSPEAK		      4
#define GALLIARD_WYRM	      8
#define GALLIARD_DREAMSPEAK		   16
#define GALLIARD_MIMIC	      32
#define GALLIARD_COBRA	      64
#define GALLIARD_RAGE		    128
#define GALLIARD_WALKER		    256
#define GALLIARD_FIRELIGHT		    512
#define GALLIARD_SIREN		   1024
#define GALLIARD_FABRIC		   2048
#define GALLIARD_HEADGAMES		   4096
#define GALLIARD_VENGEANCE		   8192


#define PHILODOX_RESISTPAIN		      1
#define PHILODOX_TRUEFORM		      2
#define PHILODOX_TRUTH		      4
#define PHILODOX_DUTY	      8
#define PHILODOX_KING		   16
#define PHILODOX_PURPOSE	      32
#define PHILODOX_WEAKARM	      64
#define PHILODOX_ANCIENTWAYS		    128
#define PHILODOX_BALANCE		    256
#define PHILODOX_ROLLOVER		    512
#define PHILODOX_SCENTOFBEYOND		   1024
#define PHILODOX_TAKETRUEFORM		   2048
#define PHILODOX_GEAS		   4096
#define PHILODOX_WALL		   8192


#define THEURGE_MOTHERSTOUCH		      1
#define THEURGE_SENSEWYRM		      2
#define THEURGE_SPIRITSPEECH		      4
#define THEURGE_COMMANDSPIRIT	      8
#define THEURGE_NAMESPIRIT		   16
#define THEURGE_SIGHTFROMBEYOND	      32
#define THEURGE_UMBRALTETHER	      64
#define THEURGE_EXORCISM		    128
#define THEURGE_PULSE		    256
#define THEURGE_SPIRITPATH		    512
#define THEURGE_UMBRALSIGHT		   1024
#define THEURGE_GRASP		   2048
#define THEURGE_DRAIN		   4096
#define THEURGE_LOGIC		   8192
#define THEURGE_SHADOWPLAY		  16384
#define THEURGE_FERAL			  32768
#define THEURGE_MALLEABLE		  65536
#define THEURGE_VESSEL		 131072


#define RAGABASH_BLUR		      1
#define RAGABASH_SEAL		      2
#define RAGABASH_WATER		      4
#define RAGABASH_SNOW	      8
#define RAGABASH_BLISSFUL		   16
#define RAGABASH_PREY	      32
#define RAGABASH_FORGOTTEN	      64
#define RAGABASH_ALTER		    128
#define RAGABASH_TRUTH		    256
#define RAGABASH_BRIDGE		    512
#define RAGABASH_FLYFEET		   1024
#define RAGABASH_REYNARD		   2048
#define RAGABASH_LUNA		   4096
#define RAGABASH_WHELP		   8192
#define RAGABASH_MAGPIE		  16384
#define RAGABASH_VIOLATION			  32768
#define RAGABASH_MALLEABLE		  65536
#define RAGABASH_VESSEL		 131072


#define CONTRACT_NONE	0
#define CONTRACT_ACCEPTED	1

#define REWARD_COST	3000


#define PATH 0
#define RITUAL 1
