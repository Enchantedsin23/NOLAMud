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
#include <malloc.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


#if !defined(macintosh)
extern int _filbuf args ((FILE *));
#endif

#define END_MARKER	"END"

/* Local Functions */
void boot_db args ((bool fCopyOver));
void copyover_recover args (());
void boot_done args (());

//mobprogram stuff
int mprog_name_to_type args ((char *name));
MPROG_DATA *mprog_file_read args ((char *f, MPROG_DATA * mprg, MOB_INDEX_DATA * pMobIndex));
void load_mobprogs args ((FILE * fp));
void mprog_read_programs args ((FILE * fp, MOB_INDEX_DATA * pMobIndex));



/*
 * Globals.
 */
long int uptime;

HELP_DATA *help_first;
HELP_DATA *help_last;

SHOP_DATA *shop_first;
SHOP_DATA *shop_last;

CHAR_DATA *char_free;
EXTRA_DESCR_DATA *extra_descr_free;
EXTRA *extra_free;
NOTE_DATA *note_free;
OBJ_DATA *obj_free;
PC_DATA *pcdata_free;

char bug_buf[2 * MAX_INPUT_LENGTH];
CHAR_DATA *char_list;
char *help_greeting;
char log_buf[2 * MAX_INPUT_LENGTH];
OBJ_DATA *object_list;
TIME_INFO_DATA time_info;
WEATHER_DATA weather_info;
//MPROG_RESETS *mp_resets;
CHAR_DATA *first_fight = NULL;	/* First character to fight     */
CHAR_DATA *last_fight = NULL;	/* Last character to fight      */

sh_int gsn_peek;
sh_int gsn_polymorph;
sh_int gsn_potency;
sh_int gsn_jail;
sh_int gsn_blindness;
sh_int gsn_invis;

// language stuff
sh_int gsn_english;
sh_int gsn_german;
sh_int gsn_chinese;
sh_int gsn_russian;
sh_int gsn_arabic;
sh_int gsn_french;
sh_int gsn_latin;
sh_int gsn_spanish;
sh_int gsn_italian;
sh_int gsn_hebrew;
sh_int gsn_gaelic;
sh_int gsn_japanese;
sh_int gsn_greek;
sh_int gsn_slavic;
sh_int gsn_garou;
sh_int gsn_indonesian;
sh_int gsn_korean;
sh_int gsn_vietnamese;
sh_int gsn_swahili;
sh_int gsn_tagalog;
sh_int gsn_hindu;
/*
 * Locals.
 */
MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
char *string_hash[MAX_KEY_HASH];

AREA_DATA *area_first;
AREA_DATA *area_last;

char *string_space;
char *top_string;
char str_empty[1] = { '\0' };

int top_affect;
int top_area;
int top_rt;
int top_ed;
int top_exit;
int top_help;
int top_mob_index;
int top_obj_index;
int top_reset;
int top_room;
int top_shop;

int top_vnum_room;		/* OLC */
int top_vnum_mob;		/* OLC */
int top_vnum_obj;		/* OLC */



/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
// Max string used to be 1572864
#define			MAX_STRING	15728640
#define			MAX_PERM_BLOCK	131072
#define			MAX_MEM_LIST	11

void *rgFreeList[MAX_MEM_LIST];
const int rgSizeList[MAX_MEM_LIST] = {
	16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768 - 64
};

int nAllocString;
int sAllocString;
int nAllocPerm;
int sAllocPerm;



/*
 * Semi-locals.
 */
bool fBootDb;
FILE *fpArea;
char strArea[MAX_INPUT_LENGTH];
//extern int MAX_STRING;
//void init_string_space args ((void));


/*
 * Local booting procedures.
 */
void init_mm args ((void));

void load_area args ((FILE * fp));
void load_helps args ((FILE * fp));
void load_mobiles args ((FILE * fp));
void load_objects args ((FILE * fp));
void load_resets args ((FILE * fp));
void load_rooms args ((FILE * fp));
void load_shops args ((FILE * fp));
void load_rooms_v1 args ((FILE * fp));
void load_specials args ((FILE * fp));

void fix_exits args ((void));

void reset_area args ((AREA_DATA * pArea));

void new_load_area args ((FILE * fp));	/* OLC */



/*
 * Big mama top level function.
 */
void boot_db (bool fCopyOver)
{
	   FILE *bfile;
	   char *tmp;
	/*
	 * Init some data space stuff.
	 */
	{
		if ((string_space = calloc (1, MAX_STRING)) == NULL)
		{
			bug ("Boot_db: can't alloc %d string space.", MAX_STRING);
			exit (1);
		}
		top_string = string_space;
		fBootDb = TRUE;
	}

	/*
	 * Init random number generator.
	 */
	{
		init_mm ();
	}

	/*
	 * Set time and weather.
	 */
	{
		long lhour, lday, lmonth;

		lhour = (current_time - 650336715) / (PULSE_TICK / PULSE_PER_SECOND);
		uptime = current_time;
		time_info.hour = lhour % 24;
		lday = lhour / 24;
		time_info.day = lday % 31;
		lmonth = lday / 31;
		time_info.month = lmonth % 12;
		time_info.year = 1248;
		/* weather code */
		/*
		weather_info.time = "01:00";
                weather_info.wind_speed = 0;
                weather_info.wind_dir = "northeast";
                weather_info.clouds = "clear";
                weather_info.cloud_height = 0;
                weather_info.temperature = 20;
                weather_info.weathercond = NULL;
		*/
        /* old weather code */
		weather_info.change = 0;
		weather_info.mmhg = 960;
		if (time_info.month >= 7 && time_info.month <= 12)
			weather_info.mmhg += number_range (1, 50);
		else
			weather_info.mmhg += number_range (1, 80);

		if (weather_info.mmhg <= 965)
			weather_info.sky = SKY_LIGHTNING;
		else if (weather_info.mmhg <= 980)
			weather_info.sky = SKY_HEAVY_RAIN;
		else if (weather_info.mmhg <= 990)
			weather_info.sky = SKY_RAINING;
		else if (weather_info.mmhg <= 1000)
			weather_info.sky = SKY_DRIZZLE;
		else if (weather_info.mmhg <= 1020)
			weather_info.sky = SKY_CLOUDY;
		else if (weather_info.mmhg <= 1030)
			weather_info.sky = SKY_MOSTLY_CLOUDY;
		else if (weather_info.mmhg <= 1040)
			weather_info.sky = SKY_PARTLY_CLOUDY;
		else if (weather_info.mmhg <= 1050)
			weather_info.sky = SKY_SPARSE_CLOUDS;
		else
			weather_info.sky = SKY_CLOUDLESS;
	}
	

	/*
	 * Assign gsn's for skills which have them.
	 */
	{
		int sn;

		for (sn = 0; sn < MAX_SKILL; sn++)
		{
			if (skill_table[sn].pgsn != NULL)
				*skill_table[sn].pgsn = sn;
		}
	}

	/*
	 * Read in all the area files.
	 */
	{
		FILE *fpList;

		if ((fpList = fopen (AREA_LIST, "r")) == NULL)
		{
			perror (AREA_LIST);
			exit (1);
		}

		for (;;)
		{
			strcpy (strArea, fread_word (fpList));
			if (strArea[0] == '$')
				break;

			if (strArea[0] == '-')
			{
				fpArea = stdin;
			}
			else
			{
				if ((fpArea = fopen (strArea, "r")) == NULL)
				{
					perror (strArea);
					exit (1);
				}
			}

			for (;;)
			{
				char *word;

				if (fread_letter (fpArea) != '#')
				{
					bug ("Boot_db: # not found.", 0);
					exit (1);
				}

				word = fread_word (fpArea);

				if (word[0] == '$')
					break;
				else if (!str_cmp (word, "AREA"))
					load_area (fpArea);
				else if (!str_cmp (word, "HELPS"))
					load_helps (fpArea);
				else if (!str_cmp (word, "MOBILES"))
					load_mobiles (fpArea);
				else if (!str_cmp (word, "MOBPROGS"))
					load_mobprogs (fpArea);	//mobprogram stuff
				else if (!str_cmp (word, "OBJECTS"))
					load_objects (fpArea);
				else if (!str_cmp (word, "RESETS"))
					load_resets (fpArea);
				else if (!str_cmp (word, "ROOMS"))
					load_rooms (fpArea);
				else if (!str_cmp (word, "ROOM_V1"))
					load_rooms_v1 (fpArea);
				else if (!str_cmp (word, "SHOPS"))
					load_shops (fpArea);
				else if (!str_cmp (word, "SPECIALS"))
					load_specials (fpArea);
				else if (!str_cmp (word, "AREADATA"))
					new_load_area (fpArea);
				else	/* Above is OLC */
				{
					bug ("Boot_db: bad section name.", 0);
					exit (1);
				}
			}

			if (fpArea != stdin)
			{
				fflush (fpArea);
				fclose (fpArea);
			}
			fpArea = NULL;
		}
		fflush (fpList);
		fclose (fpList);
	}

	/*
	 * Fix up exits.
	 * Declare db booting over.
	 * Reset all areas once.
	 * Load up the notes file.
	 * Loads command disable list.
	 */
	{
		fix_exits ();
		fBootDb = FALSE;
//		boot_done ();
		area_update ();
		load_boards ();	/* Load all boards */
		save_notes ();
		load_disabled ();
		load_banlist ();
		MOBtrigger = TRUE;	//mob programs
		srand ((unsigned) time (NULL));	/* Seed Random Number Generator */
	}

	/*
	 * Read in the eban list.
	 */
	{

	   log_string( "Loading eban list.", 1 );
	   if( ( bfile = fopen( EBAN_FILE, "r" ) ) != NULL )
	   {
		tmp = fread_string( bfile );
		while( tmp && str_cmp( tmp, "End" ) )
		{
		   do_eban( NULL, tmp );
		   free_string( tmp );
		   tmp = fread_string( bfile );
		}
		free_string( tmp );
	   }
	   fclose( bfile );
	}

	if (fCopyOver)
		copyover_recover ();

	return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area (FILE * fp)
{
	AREA_DATA *pArea;

	pArea = alloc_perm (sizeof (*pArea));
	pArea->reset_first = NULL;
	pArea->reset_last = NULL;
	pArea->name = fread_string (fp);
	pArea->area_flags = AREA_LOADING;	/* OLC */
	pArea->security = 1;	/* OLC */
	pArea->builders = str_dup ("None");	/* OLC */
	pArea->lvnum = 0;	/* OLC */
	pArea->uvnum = 0;	/* OLC */
	pArea->vnum = 0;	/* OLC */
	pArea->filename = str_dup (strArea);	/* OLC */
	pArea->age = 15;
	pArea->nplayer = 0;
	pArea->x = -1;
	pArea->y = -1;
	pArea->start = 0;
	if (area_first == NULL)
		area_first = pArea;
	if (area_last != NULL)
	{
		area_last->next = pArea;
		REMOVE_BIT (area_last->area_flags, AREA_LOADING);	/* OLC */
	}
	area_last = pArea;
	pArea->next = NULL;

	top_area++;
	return;
}

/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
				if ( !str_cmp( word, literal ) )	\
                {                                   \
                    field  = value;                 \
                    fMatch = TRUE;                  \
                    break;                          \
				}

#define SKEY( string, field )                       \
                if ( !str_cmp( word, string ) )     \
                {                                   \
						  free_string( field );           \
                    field = fread_string( fp );     \
                    fMatch = TRUE;                  \
                    break;                          \
				}




/* OLC
 * I left the Repop field here because I though that newbie programmers like
 * myself would want to set up something small to begin with.  To get the
 * repop field to work, you will want to go through the code and add it to
 * memory recycling routines in BUILD.C and also in the UPDATE.C file to
 * display the message.  Have fun.  Erase this comment when you are done
 * trying this.  Mail me jdinkel@mines.colorado.edu if you have trouble.
 */

/*
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * End
 */
void new_load_area (FILE * fp)
{
	AREA_DATA *pArea;
	char *word;
	bool fMatch;

	pArea = alloc_perm (sizeof (*pArea));
	pArea->age = 15;
	pArea->nplayer = 0;
	pArea->filename = str_dup (strArea);
	pArea->vnum = top_area;
	pArea->name = str_dup ("New Area");
	pArea->builders = str_dup ("");
	pArea->security = 1;
	pArea->lvnum = 0;
	pArea->uvnum = 0;
	pArea->area_flags = 0;

	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;

		switch (UPPER (word[0]))
		{
		case 'N':
			SKEY ("Name", pArea->name);
			break;
		case 'S':
			KEY ("Security", pArea->security, fread_number (fp));
			break;
		case 'V':
			if (!str_cmp (word, "VNUMs"))
			{
				pArea->lvnum = fread_number (fp);
				pArea->uvnum = fread_number (fp);
			}
			break;
		case 'E':
			if (!str_cmp (word, "End"))
			{
				fMatch = TRUE;
				if (area_first == NULL)
					area_first = pArea;
				if (area_last != NULL)
					area_last->next = pArea;
				area_last = pArea;
				pArea->next = NULL;
				top_area++;
				return;
			}
			break;
		case 'B':
			SKEY ("Builders", pArea->builders);
			break;
		case 'R':
/*            SKEY( "Repop",  pArea->repop ); */
			if (!str_cmp (word, "Reset"))
			{
				fread_to_eol (fp);
				fMatch = TRUE;
			}
			break;
		}
	}

}



/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum (int vnum)
{
	if (area_last->lvnum == 0 || area_last->uvnum == 0)
		area_last->lvnum = area_last->uvnum = vnum;
	if (vnum != URANGE (area_last->lvnum, vnum, area_last->uvnum))
	{
		if (vnum < area_last->lvnum)
			area_last->lvnum = vnum;
		else
			area_last->uvnum = vnum;
	}
	return;
}



/*
 * Snarf a help section.
 */
void load_helps (FILE * fp)
{
	HELP_DATA *pHelp;

	for (;;)
	{
		pHelp = alloc_mem (sizeof (*pHelp));
		pHelp->level = fread_number (fp);
		pHelp->keyword = fread_string (fp);
		if (pHelp->keyword[0] == '$')
			break;
		pHelp->text = fread_string (fp);

		if (!str_cmp (pHelp->keyword, "greeting"))
			help_greeting = pHelp->text;

		if (help_first == NULL)
			help_first = pHelp;
		if (help_last != NULL)
			help_last->next = pHelp;

		help_last = pHelp;
		pHelp->next = NULL;
		top_help++;
	}
	log_string ("done reading help", CHANNEL_LOG_NORMAL);

	return;
}



/*
 * Snarf a mob section.
 */
void load_mobiles (FILE * fp)
{
	MOB_INDEX_DATA *pMobIndex;
	int i;
	int j;

	if (area_last == NULL)	/* OLC */
	{
		bug ("Load_mobiles: no #AREA seen yet.", 0);
		exit (1);
	}
	for (;;)
	{
		sh_int vnum;
		char letter;
		int iHash;

		letter = fread_letter (fp);
		if (letter != '#')
		{
			bug ("Load_mobiles: # not found.", 0);
			exit (1);
		}

		vnum = fread_number (fp);
		if (vnum == 0)
			break;

		fBootDb = FALSE;
		if (get_mob_index (vnum) != NULL)
		{
			bug ("Load_mobiles: vnum %d duplicated.", vnum);
			exit (1);
		}
		fBootDb = TRUE;

		pMobIndex = alloc_perm (sizeof (*pMobIndex));
		pMobIndex->vnum = vnum;
		pMobIndex->area = area_last;	/* OLC */
		pMobIndex->player_name = fread_string (fp);
		pMobIndex->short_descr = fread_string (fp);
		pMobIndex->long_descr = fread_string (fp);
		pMobIndex->description = fread_string (fp);

		pMobIndex->long_descr[0] = UPPER (pMobIndex->long_descr[0]);
		pMobIndex->description[0] = UPPER (pMobIndex->description[0]);

		pMobIndex->act = fread_number (fp) | ACT_IS_NPC;
		pMobIndex->affected_by = fread_number (fp);
//		pMobIndex->affected_by2 = fread_number (fp);
		pMobIndex->itemaffect = 0;
		pMobIndex->pShop = NULL;
		pMobIndex->alignment = fread_number (fp);
		letter = fread_letter (fp);
		pMobIndex->level = fread_number (fp);

		/*
		 * The unused stuff is for imps who want to use the old-style
		 * stats-in-files method.
		 */
		pMobIndex->hitroll = fread_number (fp);	/* Unused */
		pMobIndex->ac = fread_number (fp);	/* Unused */
		pMobIndex->hitnodice = fread_number (fp);	/* Unused */
		/* 'd'          */ fread_letter (fp);
		/* Unused */
		pMobIndex->hitsizedice = fread_number (fp);	/* Unused */
		/* '+'          */ fread_letter (fp);
		/* Unused */
		pMobIndex->hitplus = fread_number (fp);	/* Unused */
		pMobIndex->damnodice = fread_number (fp);	/* Unused */
		/* 'd'          */ fread_letter (fp);
		/* Unused */
		pMobIndex->damsizedice = fread_number (fp);	/* Unused */
		/* '+'          */ fread_letter (fp);
		/* Unused */
		pMobIndex->damplus = fread_number (fp);	/* Unused */
		pMobIndex->gold = fread_number (fp);	/* Unused */
		/* xp can't be used! */ fread_number (fp);
		/* Unused */
		/* position     */ fread_number (fp);
		/* Unused */
		/* start pos    */ fread_number (fp);
		/* Unused */

		/*
		 * Back to meaningful values.
		 */
		pMobIndex->sex = fread_number (fp);

		// Gobbo Trial **************/
		pMobIndex->attributes[ATTRIB_STR] = fread_number (fp);
		pMobIndex->attributes[ATTRIB_DEX] = fread_number (fp);
		pMobIndex->attributes[ATTRIB_STA] = fread_number (fp);


		pMobIndex->attributes[ATTRIB_CHA] = fread_number (fp);
		pMobIndex->attributes[ATTRIB_MAN] = fread_number (fp);
		pMobIndex->attributes[ATTRIB_APP] = fread_number (fp);

		pMobIndex->attributes[ATTRIB_PER] = fread_number (fp);
		pMobIndex->attributes[ATTRIB_INT] = fread_number (fp);
		pMobIndex->attributes[ATTRIB_WIT] = fread_number (fp);

		for (i = 0; i < 3; i++)
			for (j = 0; j < 10; j++)
				pMobIndex->abilities[i][j] = 0;

		pMobIndex->abilities[TALENTS][TAL_BRAWL] = fread_number (fp);
		pMobIndex->abilities[TALENTS][TAL_DODGE] = fread_number (fp);
		pMobIndex->abilities[SKILLS][SKI_MELEE] = fread_number (fp);

		// End Gobbo

		if (letter != 'S')
		{
			bug ("Load_mobiles: vnum %d non-S.", vnum);
			exit (1);
		}

//mob program stuff
		letter = fread_letter (fp);
		if (letter == '>')
		{
			ungetc (letter, fp);
			mprog_read_programs (fp, pMobIndex);
		}
		else
			ungetc (letter, fp);
//end mob prog


		iHash = vnum % MAX_KEY_HASH;
		pMobIndex->next = mob_index_hash[iHash];
		mob_index_hash[iHash] = pMobIndex;
		top_mob_index++;
		top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;	/* OLC */
		assign_area_vnum (vnum);	/* OLC */
	}

	return;
}



/*
 * Snarf an obj section.
 */
void load_objects (FILE * fp)
{
	OBJ_INDEX_DATA *pObjIndex;

	if (area_last == NULL)	/* OLC */
	{
		bug ("Load_objects: no #AREA seen yet.", 0);
		exit (1);
	}
	for (;;)
	{
		sh_int vnum;
		char letter;
		int iHash;

		letter = fread_letter (fp);
		if (letter != '#')
		{
			bug ("Load_objects: # not found.", 0);
			exit (1);
		}

		vnum = fread_number (fp);
		if (vnum == 0)
			break;

		fBootDb = FALSE;
		if (get_obj_index (vnum) != NULL)
		{
			bug ("Load_objects: vnum %d duplicated.", vnum);
			exit (1);
		}
		fBootDb = TRUE;

		pObjIndex = alloc_perm (sizeof (*pObjIndex));
		pObjIndex->vnum = vnum;
		pObjIndex->area = area_last;	/* OLC */
		pObjIndex->name = fread_string (fp);
		pObjIndex->short_descr = fread_string (fp);
		pObjIndex->description = fread_string (fp);
		/* Action description */ fread_string (fp);

		pObjIndex->short_descr[0] = LOWER (pObjIndex->short_descr[0]);
		pObjIndex->description[0] = UPPER (pObjIndex->description[0]);

		pObjIndex->item_type = fread_number (fp);
		pObjIndex->extra_flags = fread_number (fp);
		pObjIndex->wear_flags = fread_number (fp);
		pObjIndex->value[0] = fread_number (fp);
		pObjIndex->value[1] = fread_number (fp);
		pObjIndex->value[2] = fread_number (fp);
		pObjIndex->value[3] = fread_number (fp);
		pObjIndex->weight = fread_number (fp);
		pObjIndex->cost = fread_number (fp);	/* Unused */
		pObjIndex->affected = NULL;
		pObjIndex->extra_descr = NULL;
		pObjIndex->chpoweron = NULL;
		pObjIndex->chpoweroff = NULL;
		pObjIndex->chpoweruse = NULL;
		pObjIndex->victpoweron = NULL;
		pObjIndex->victpoweroff = NULL;
		pObjIndex->victpoweruse = NULL;
		pObjIndex->spectype = 0;
		pObjIndex->specpower = 0;
		/* Cost per day */ fread_number (fp);
/*
	if ( pObjIndex->item_type == ITEM_POTION )
	    SET_BIT(pObjIndex->extra_flags, ITEM_NODROP);
*/
		for (;;)
		{
			char letter;

			letter = fread_letter (fp);

			if (letter == 'A')
			{
				AFFECT_DATA *paf;

				paf = alloc_perm (sizeof (*paf));
				paf->type = -1;
				paf->duration = -1;
				paf->location = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->bitvector = 0;
				paf->next = pObjIndex->affected;
				pObjIndex->affected = paf;
				top_affect++;
			}

			else if (letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;

				ed = alloc_perm (sizeof (*ed));
				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = pObjIndex->extra_descr;
				pObjIndex->extra_descr = ed;
				top_ed++;
			}

			else if (letter == 'Q')
			{
				pObjIndex->chpoweron = fread_string (fp);
				pObjIndex->chpoweroff = fread_string (fp);
				pObjIndex->chpoweruse = fread_string (fp);
				pObjIndex->victpoweron = fread_string (fp);
				pObjIndex->victpoweroff = fread_string (fp);
				pObjIndex->victpoweruse = fread_string (fp);
				pObjIndex->spectype = fread_number (fp);
				pObjIndex->specpower = fread_number (fp);
			}

			else
			{
				ungetc (letter, fp);
				break;
			}
		}

		/*
		 * Translate spell "slot numbers" to internal "skill numbers."
		 */
		switch (pObjIndex->item_type)
		{
		case ITEM_PILL:
		case ITEM_POTION:
		case ITEM_SCROLL:
			pObjIndex->value[1] = slot_lookup (pObjIndex->value[1]);
			pObjIndex->value[2] = slot_lookup (pObjIndex->value[2]);
			pObjIndex->value[3] = slot_lookup (pObjIndex->value[3]);
			break;

		case ITEM_STAFF:
		case ITEM_WAND:
			pObjIndex->value[3] = slot_lookup (pObjIndex->value[3]);
			break;
		}

		iHash = vnum % MAX_KEY_HASH;
		pObjIndex->next = obj_index_hash[iHash];
		obj_index_hash[iHash] = pObjIndex;
		top_obj_index++;
		top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;	/* OLC */
		assign_area_vnum (vnum);	/* OLC */
	}

	return;
}



/*
 * Snarf a reset section.
 */
void load_resets (FILE * fp) /* OLC */
{
	RESET_DATA *pReset = NULL;
	int mvn = 0;

	if (area_last == NULL)
	{
		bug ("Load_resets: no #AREA seen yet.", 0);
		exit (1);
	}

	for (;;)
	{
	EXIT_DATA	*pexit;
		ROOM_INDEX_DATA *pRoomIndex;
		char letter;
		int arg1, arg2, arg3;

		if ((letter = fread_letter (fp)) == 'S')
			break;

		if (letter == '*')
		{
			fread_to_eol (fp);
			continue;
		}

		/* if_flag */ fread_number (fp);
		arg1 = fread_number (fp);
		arg2 = fread_number (fp);
		arg3 = (letter == 'G' || letter == 'R') ? 0 : fread_number (fp);
		fread_to_eol (fp);

		/*
		 * Validate parameters.
		 * We're calling the index functions for the side effect.
		 */
		switch (letter)
		{
		default:
			bug ("Load_resets: bad command '%c'.", letter);
			exit (1);
			break;

		case 'M':
			get_mob_index (arg1);
			get_room_index (arg3);

			pReset = alloc_perm (sizeof (*pReset));
			pReset->arg1 = arg1;
			pReset->arg2 = arg2;
			pReset->arg3 = arg3;
			pReset->command = letter;
			mvn = arg3;

			break;

		case 'O':
			get_obj_index (arg1);
			get_room_index (arg3);

			pReset = alloc_perm (sizeof (*pReset));
			pReset->arg1 = arg1;
			pReset->arg2 = arg2;
			pReset->arg3 = arg3;
			pReset->command = letter;

			break;

		case 'P':
			get_obj_index (arg1);
			get_obj_index (arg3);

			pReset = alloc_perm (sizeof (*pReset));
			pReset->arg1 = arg1;
			pReset->arg2 = arg2;
			pReset->arg3 = arg3;
			pReset->command = letter;

			break;

		case 'G':
		case 'E':
			get_room_index (mvn);
			get_obj_index (arg1);

			pReset = alloc_perm (sizeof (*pReset));
			pReset->arg1 = arg1;
			pReset->arg2 = arg2;
			pReset->arg3 = arg3;
			pReset->command = letter;

			break;

		case 'D':
	    pRoomIndex = get_room_index( arg1 );

	    if ( arg2 < 0
	    ||   arg2 > MAX_DIR
	    || ( pexit = pRoomIndex->exit[arg2] ) == NULL
               || !IS_SET( pexit->rs_flags, EX_ISDOOR ) )
	    {
		bug( "Load_resets: 'D': exit %d not door.", arg2 );
		exit( 1 );
	    }

	    switch ( arg3 )
	    {
		default:
		    bug( "Load_resets: 'D': bad 'locks': %d.", arg3 );
		    exit( 1 ); break;
		case 0: break;
		case 1: SET_BIT( pexit->rs_flags, EX_CLOSED ); break;
		case 2: SET_BIT( pexit->rs_flags, EX_CLOSED );
			SET_BIT( pexit->rs_flags, EX_LOCKED ); break;
	    }
			break;

		case 'R':
			pRoomIndex = get_room_index (arg1);

			if (arg2 < 0 || arg2 > MAX_DIR)
			{
				bug ("Load_resets: 'R': bad exit %d.", arg2);
				exit (1);
			}

			pReset = alloc_perm (sizeof (*pReset));
			pReset->arg1 = arg1;
			pReset->arg2 = arg2;
			pReset->arg3 = arg3;
			pReset->command = letter;

			break;
		}

		if (pReset)
		{
			if (area_last->reset_first == NULL)
				area_last->reset_first = pReset;
			if (area_last->reset_last != NULL)
				area_last->reset_last->next = pReset;

			area_last->reset_last = pReset;
			pReset->next = NULL;
			top_reset++;
		}
	}

	return;
}



/*
 * Snarf a room section.
 */
void load_rooms_v1 (FILE * fp)
{
	ROOM_INDEX_DATA *pRoomIndex;

	if (area_last == NULL)
	{
		bug ("Load_resets: no #AREA seen yet.", 0);	/* OLC */
		exit (1);
	}

	for (;;)
	{
		sh_int vnum;
		char letter;
		int door;
		int iHash;
		static int prev_top_area = -1;

		letter = fread_letter (fp);
		if (letter != '#')
		{
			bug ("Load_rooms: # not found.", 0);
			exit (1);
		}

		vnum = fread_number (fp);
		if (vnum == 0)
			break;
		if (prev_top_area != top_area)
		{
			prev_top_area = top_area;
			area_last->start = vnum;
		}


		fBootDb = FALSE;
		if (get_room_index (vnum) != NULL)
		{
			bug ("Load_rooms: vnum %d duplicated.", vnum);
			exit (1);
		}
		fBootDb = TRUE;

		pRoomIndex = alloc_perm (sizeof (*pRoomIndex));
		pRoomIndex->people = NULL;
		pRoomIndex->contents = NULL;
		pRoomIndex->to_obj = NULL;
		pRoomIndex->extra_descr = NULL;
		pRoomIndex->area = area_last;
		pRoomIndex->vnum = vnum;
		pRoomIndex->name = fread_string (fp);
		pRoomIndex->description = fread_string (fp);
		/* Area number */ fread_number (fp);
		pRoomIndex->room_flags = fread_number (fp);
		pRoomIndex->added_flags = 0;
		pRoomIndex->sector_type = fread_number (fp);
		pRoomIndex->version = 0;
		pRoomIndex->version = fread_number (fp);
		if (pRoomIndex->version >= 1)
		{
			pRoomIndex->x = fread_number (fp);
			pRoomIndex->y = fread_number (fp);
		}
		pRoomIndex->light = 0;
		pRoomIndex->blood = 0;
		pRoomIndex->passed = 0;
		pRoomIndex->roomtext = NULL;

		pRoomIndex->wood = 0;
		pRoomIndex->build = 0;
		pRoomIndex->ground = 0;

		pRoomIndex->room = 0;

		for (door = 0; door <= 4; door++)
		{
			pRoomIndex->track[door] = str_dup ("");
			pRoomIndex->track_dir[door] = 0;
		}
		for (door = 0; door <= 5; door++)
			pRoomIndex->exit[door] = NULL;

		for (;;)
		{
			letter = fread_letter (fp);

			if (letter == 'S')
				break;

			if (letter == 'D')
			{
				EXIT_DATA *pexit;
				int locks;

				door = fread_number (fp);
				if (door < 0 || door > 5)
				{
					bug ("Fread_rooms: vnum %d has bad door number.", vnum);
					exit (1);
				}

				pexit = alloc_perm (sizeof (*pexit));
				pexit->description = fread_string (fp);
				pexit->keyword = fread_string (fp);
				pexit->exit_info = 0;
				pexit->rs_flags = 0;	/* OLC */
				locks = fread_number (fp);
				pexit->key = fread_number (fp);
				pexit->vnum = fread_number (fp);

				switch (locks)
				{
				case 1:
					pexit->rs_flags = EX_ISDOOR;
					break;
				case 2:
					pexit->rs_flags = EX_ISDOOR | EX_PICKPROOF;
					break;
				}

				pRoomIndex->exit[door] = pexit;
				top_exit++;
			}
			else if (letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;

				ed = alloc_perm (sizeof (*ed));
				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = pRoomIndex->extra_descr;
				pRoomIndex->extra_descr = ed;
				top_ed++;
			}
			else if (letter == 'T')
			{
				ROOMTEXT_DATA *rt;

				rt = alloc_perm (sizeof (*rt));
				rt->input = fread_string (fp);
				rt->output = fread_string (fp);
				rt->choutput = fread_string (fp);
				rt->name = fread_string (fp);
				rt->type = fread_number (fp);
				rt->power = fread_number (fp);
				rt->mob = fread_number (fp);
				rt->next = pRoomIndex->roomtext;
				pRoomIndex->roomtext = rt;
				top_rt++;
			}
			else if (letter == 'V')
			{
				pRoomIndex->x = fread_number (fp);
				pRoomIndex->y = fread_number (fp);
				area_last->x = pRoomIndex->x;
				area_last->y = pRoomIndex->y;
			}
			else
			{
				bug ("Load_rooms: vnum %d has flag not 'DES'.", vnum);
				exit (1);
			}
		}

		iHash = vnum % MAX_KEY_HASH;
		pRoomIndex->next = room_index_hash[iHash];
		room_index_hash[iHash] = pRoomIndex;
		top_room++;
		top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;	/* 
										   OLC */
		assign_area_vnum (vnum);	/* OLC */
	}

	return;
}
void load_rooms (FILE * fp)
{
	ROOM_INDEX_DATA *pRoomIndex;

	if (area_last == NULL)
	{
		bug ("Load_resets: no #AREA seen yet.", 0);	/* OLC */
		exit (1);
	}

	for (;;)
	{
		sh_int vnum;
		char letter;
		int door;
		int iHash;
		static int prev_top_area = -1;

		letter = fread_letter (fp);
		if (letter != '#')
		{
			bug ("Load_rooms: # not found.", 0);
			exit (1);
		}

		vnum = fread_number (fp);
		if (vnum == 0)
			break;
		if (prev_top_area != top_area)
		{
			prev_top_area = top_area;
			area_last->start = vnum;
		}


		fBootDb = FALSE;
		if (get_room_index (vnum) != NULL)
		{
			bug ("Load_rooms: vnum %d duplicated.", vnum);
			exit (1);
		}
		fBootDb = TRUE;

		pRoomIndex = alloc_perm (sizeof (*pRoomIndex));
		pRoomIndex->people = NULL;
		pRoomIndex->contents = NULL;
		pRoomIndex->to_obj = NULL;
		pRoomIndex->extra_descr = NULL;
		pRoomIndex->area = area_last;
		pRoomIndex->vnum = vnum;
		pRoomIndex->name = fread_string (fp);
		pRoomIndex->description = fread_string (fp);
		/* Area number */ fread_number (fp);
		pRoomIndex->room_flags = fread_number (fp);
		pRoomIndex->added_flags = 0;
		pRoomIndex->sector_type = fread_number (fp);
		pRoomIndex->light = 0;
		pRoomIndex->blood = 0;
		pRoomIndex->passed = 0;
		pRoomIndex->roomtext = NULL;

		pRoomIndex->wood = 0;
		pRoomIndex->build = 0;
		pRoomIndex->ground = 0;
		pRoomIndex->version = 0;
		pRoomIndex->x = -1;
		pRoomIndex->y = -1;
		pRoomIndex->room = 0;

		for (door = 0; door <= 4; door++)
		{
			pRoomIndex->track[door] = str_dup ("");
			pRoomIndex->track_dir[door] = 0;
		}
		for (door = 0; door <= 5; door++)
			pRoomIndex->exit[door] = NULL;

		for (;;)
		{
			letter = fread_letter (fp);

			if (letter == 'S')
				break;

			if (letter == 'D')
			{
				EXIT_DATA *pexit;
				int locks;

				door = fread_number (fp);
				if (door < 0 || door > 5)
				{
					bug ("Fread_rooms: vnum %d has bad door number.", vnum);
					exit (1);
				}

				pexit = alloc_perm (sizeof (*pexit));
				pexit->description = fread_string (fp);
				pexit->keyword = fread_string (fp);
				pexit->exit_info = 0;
				pexit->rs_flags = 0;	/* OLC */
				locks = fread_number (fp);
				pexit->key = fread_number (fp);
				pexit->vnum = fread_number (fp);

				switch (locks)
				{
				case 1:
					pexit->exit_info = EX_ISDOOR;
					break;
				case 2:
					pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;
					break;
				}

				pRoomIndex->exit[door] = pexit;
				top_exit++;
			}
			else if (letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;

				ed = alloc_perm (sizeof (*ed));
				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = pRoomIndex->extra_descr;
				pRoomIndex->extra_descr = ed;
				top_ed++;
			}
			else if (letter == 'T')
			{
				ROOMTEXT_DATA *rt;

				rt = alloc_perm (sizeof (*rt));
				rt->input = fread_string (fp);
				rt->output = fread_string (fp);
				rt->choutput = fread_string (fp);
				rt->name = fread_string (fp);
				rt->type = fread_number (fp);
				rt->power = fread_number (fp);
				rt->mob = fread_number (fp);
				rt->next = pRoomIndex->roomtext;
				pRoomIndex->roomtext = rt;
				top_rt++;
			}
			else if (letter == 'V')
			{
				pRoomIndex->x = fread_number (fp);
				pRoomIndex->y = fread_number (fp);
				area_last->x = pRoomIndex->x;
				area_last->y = pRoomIndex->y;
			}
			else
			{
				bug ("Load_rooms: vnum %d has flag not 'DES'.", vnum);
				exit (1);
			}
		}

		iHash = vnum % MAX_KEY_HASH;
		pRoomIndex->next = room_index_hash[iHash];
		room_index_hash[iHash] = pRoomIndex;
		top_room++;
		top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;	/* OLC */
		assign_area_vnum (vnum);	/* OLC */
	}

	return;
}


/*
 * Snarf a shop section. I can probably junk this but I'll have to ask
 */
void load_shops (FILE * fp)
{
	SHOP_DATA *pShop;

	for (;;)
	{
		MOB_INDEX_DATA *pMobIndex;
		int iTrade;

		pShop = alloc_perm (sizeof (*pShop));
		pShop->keeper = fread_number (fp);
		if (pShop->keeper == 0)
			break;
		for (iTrade = 0; iTrade < MAX_TRADE; iTrade++)
			pShop->buy_type[iTrade] = fread_number (fp);
		pShop->profit_buy = fread_number (fp);
		pShop->profit_sell = fread_number (fp);
		pShop->open_hour = fread_number (fp);
		pShop->close_hour = fread_number (fp);
		fread_to_eol (fp);
		pMobIndex = get_mob_index (pShop->keeper);
		pMobIndex->pShop = pShop;

		if (shop_first == NULL)
			shop_first = pShop;
		if (shop_last != NULL)
			shop_last->next = pShop;

		shop_last = pShop;
		pShop->next = NULL;
		top_shop++;
	}

	return;
}



/*
 * Snarf spec proc declarations.
 */
void load_specials (FILE * fp)
{
	for (;;)
	{
		MOB_INDEX_DATA *pMobIndex;
		char letter;

		switch (letter = fread_letter (fp))
		{
		default:
			bug ("Load_specials: letter '%c' not *MS.", letter);
			exit (1);

		case 'S':
			return;

		case '*':
			break;

		case 'M':
			pMobIndex = get_mob_index (fread_number (fp));
			pMobIndex->spec_name = str_dup (fread_word (fp));	/* OLC */
			pMobIndex->spec_fun = spec_lookup (pMobIndex->spec_name);	/* OLC */
			if (pMobIndex->spec_fun == 0)
			{
				bug ("Load_specials: 'M': vnum %d.", pMobIndex->vnum);
				exit (1);
			}
			break;
		}

		fread_to_eol (fp);
	}
}



/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits (void)
{
	extern const sh_int rev_dir[];
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;
	int iHash;
	int door;

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next)
		{
			bool fexit;

			fexit = FALSE;
			for (door = 0; door <= 5; door++)
			{
				if ((pexit = pRoomIndex->exit[door]) != NULL)
				{
					fexit = TRUE;
					if (pexit->vnum <= 0)
						pexit->to_room = NULL;
					else
						pexit->to_room = get_room_index (pexit->vnum);
				}
			}

			if (!fexit)
				SET_BIT (pRoomIndex->room_flags, ROOM_NO_MOB);
		}
	}

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next)
		{
			for (door = 0; door <= 5; door++)
			{
				if ((pexit = pRoomIndex->exit[door]) != NULL && (to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->to_room != pRoomIndex)
				{
					sprintf (buf, "Fix_exits: %d:%d -> %d:%d -> %d.", pRoomIndex->vnum, door, to_room->vnum, rev_dir[door], (pexit_rev->to_room == NULL) ? 0 : pexit_rev->to_room->vnum);
/*		    bug( buf, 0 ); */
				}
			}
		}
	}

	return;
}



/*
 * Repopulate areas periodically.
 */
void area_update (void)
{
	AREA_DATA *pArea;

	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	{
		CHAR_DATA *pch;

		if (++pArea->age < 3)
			continue;

		/*
		 * Check for PC's.
		 */
		if (pArea->nplayer > 0 && pArea->age == 15 - 1)
		{
			for (pch = char_list; pch != NULL; pch = pch->next)
			{
				if (!IS_NPC (pch) && IS_AWAKE (pch) && pch->in_room != NULL && pch->in_room->area == pArea)
				{
				/*	send_to_char( "#bA howl can be 
heard in the distance, a lone call that rings of sorrow.#n\n\r", pch);				
				*/
				}

			}
		}

		/*
		 * Check age and reset.
		 * Note: Mud School resets every 3 minutes (not 15).
		 */
		if (pArea->nplayer == 0 || pArea->age >= 15)
		{
			ROOM_INDEX_DATA *pRoomIndex;

			reset_area (pArea);
			pArea->age = number_range (0, 3);
			pRoomIndex = get_room_index (ROOM_VNUM_SCHOOL);
			if (pRoomIndex != NULL && pArea == pRoomIndex->area)
				pArea->age = 15 - 3;
		}
	}

	return;
}



/*
 * Reset one area.
 */
void reset_area (AREA_DATA * pArea)
{
	CHAR_DATA *mob;
	RESET_DATA *pReset;
	ROOM_INDEX_DATA *pRoomIndex;	/* OLC */
	int iExit;		/* OLC */
	int iHash;		/* OLC */
	int level;
	bool last;

	if (pArea == NULL)
		return;

	mob = NULL;
	last = TRUE;
	level = 0;

	/*
	 * OLC
	 * New system saves resets as part of door structure.
	 */
	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next)
		{
			if (pRoomIndex->area == pArea)
				for (iExit = 0; iExit < MAX_DIR; iExit++)
				{
					EXIT_DATA *pExit;

					if ((pExit = pRoomIndex->exit[iExit]) && !IS_SET (pExit->exit_info, EX_BASHED))
					{
						pExit->exit_info = pExit->rs_flags;
						if ((pExit->to_room != NULL) && (pExit = pExit->to_room->exit[rev_dir[iExit]]) != NULL)
						{
							pExit->exit_info = pExit->rs_flags;
							/* nail the other side */
						}
					}
				}
		}
	}

	for (pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next)
	{
		OBJ_DATA *obj;
		OBJ_DATA *obj_to;
		MOB_INDEX_DATA *pMobIndex;
		OBJ_INDEX_DATA *pObjIndex;
		OBJ_INDEX_DATA *pObjToIndex;
		ROOM_INDEX_DATA *pRoomIndex;
		EXIT_DATA *pexit;
		switch (pReset->command)
		{
		default:
			bug ("Reset_area: bad command %c.", pReset->command);
			break;

		case 'M':
			if ((pMobIndex = get_mob_index (pReset->arg1)) == NULL)
			{
				bug ("Reset_area: 'M': bad vnum %d.", pReset->arg1);
				continue;
			}

			if ((pRoomIndex = get_room_index (pReset->arg3)) == NULL)
			{
				bug ("Reset_area: 'R': bad vnum %d.", pReset->arg3);
				continue;
			}

			level = URANGE (0, pMobIndex->level - 2, LEVEL_HERO);
			if (pMobIndex->count >= pReset->arg2)
			{
				last = FALSE;
				break;
			}

			mob = create_mobile (pMobIndex);

			/*
			 * Check for pet shop.
			 */
			{
				ROOM_INDEX_DATA *pRoomIndexPrev;

				pRoomIndexPrev = get_room_index (pRoomIndex->vnum - 1);
				if (pRoomIndexPrev != NULL && IS_SET (pRoomIndexPrev->room_flags, ROOM_PET_SHOP))
					SET_BIT (mob->act, ACT_PET);
			}

			if (room_is_dark (pRoomIndex))
				SET_BIT (mob->affected_by, AFF_INFRARED);

			char_to_room (mob, pRoomIndex);
			level = URANGE (0, mob->level - 2, LEVEL_HERO);
			last = TRUE;
			break;

		case 'O':
			if ((pObjIndex = get_obj_index (pReset->arg1)) == NULL)
			{
				bug ("Reset_area: 'O': bad vnum %d.", pReset->arg1);
				continue;
			}

			if ((pRoomIndex = get_room_index (pReset->arg3)) == NULL)
			{
				bug ("Reset_area: 'R': bad vnum %d.", pReset->arg3);
				continue;
			}

			if (pArea->nplayer > 0 || count_obj_list (pObjIndex, pRoomIndex->contents) > 0)
			{
				last = FALSE;
				break;
			}

			obj = create_object (pObjIndex, number_range (1, 50));
			obj->cost = 0;
			obj_to_room (obj, pRoomIndex);
			last = TRUE;
			break;

		case 'P':
			if ((pObjIndex = get_obj_index (pReset->arg1)) == NULL)
			{
				bug ("Reset_area: 'P': bad vnum %d.", pReset->arg1);
				continue;
			}

			if ((pObjToIndex = get_obj_index (pReset->arg3)) == NULL)
			{
				bug ("Reset_area: 'P': bad vnum %d.", pReset->arg3);
				continue;
			}

			if (pArea->nplayer > 0 || (obj_to = get_obj_type (pObjToIndex)) == NULL || obj_to->in_room == NULL || count_obj_list (pObjIndex, obj_to->contains) > 0)
			{
				last = FALSE;
				break;
			}

			obj = create_object (pObjIndex, number_range (1, 50));
			obj_to_obj (obj, obj_to);
			last = TRUE;
			break;

		case 'G':
		case 'E':
			if ((pObjIndex = get_obj_index (pReset->arg1)) == NULL)
			{
				bug ("Reset_area: 'E' or 'G': bad vnum %d.", pReset->arg1);
				continue;
			}

			if (!last)
				break;

			if (mob == NULL)
			{
				bug ("Reset_area: 'E' or 'G': null mob for vnum %d.", pReset->arg1);
				last = FALSE;
				break;
			}

			if (mob->pIndexData->pShop != NULL)
			{
				int olevel;

				switch (pObjIndex->item_type)
				{
				default:
					olevel = 1;
					break;
				case ITEM_PILL:
					olevel = number_range (0, 10);
					break;
				case ITEM_POTION:
					olevel = number_range (0, 10);
					break;
				case ITEM_SCROLL:
					olevel = number_range (5, 15);
					break;
				case ITEM_WAND:
					olevel = number_range (10, 20);
					break;
				case ITEM_STAFF:
					olevel = number_range (15, 25);
					break;
					// case ITEM_ARMOR:     olevel = number_range(  5, 15 ); break;
					// case ITEM_WEAPON:    olevel = number_range(  5, 15 ); break;
				case ITEM_ARMOR:
					olevel = 1;
					break;
				case ITEM_WEAPON:
					olevel = 1;
					break;
				}

				obj = create_object (pObjIndex, olevel);
				if (pReset->command == 'G')
					SET_BIT (obj->extra_flags, ITEM_INVENTORY);
			}
			else
			{
				obj = create_object (pObjIndex, number_range (1, 50));
			}
			obj_to_char (obj, mob);
			if (pReset->command == 'E')
				equip_char (mob, obj, pReset->arg3);
			last = TRUE;
			break;

		case 'D':

	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    if ( ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL )
		break;

	    switch ( pReset->arg3 )
	    {
	    case 0:
/*		REMOVE_BIT( pexit->rs_flags, EX_CLOSED );
		REMOVE_BIT( pexit->rs_flags, EX_LOCKED );
*/		break;

	    case 1:
		SET_BIT(    pexit->rs_flags, EX_CLOSED );
		REMOVE_BIT( pexit->rs_flags, EX_LOCKED );
		break;

	    case 2:
		SET_BIT(    pexit->rs_flags, EX_CLOSED );
		SET_BIT(    pexit->rs_flags, EX_LOCKED );
		break;
	    }

	    last = TRUE;

			break;

		case 'R':

	    if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
	    {
		bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
		continue;
	    }

	    {
		int d0;
		int d1;

		for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
		{
		    d1                   = number_range( d0, pReset->arg2-1 );
		    pexit                = pRoomIndex->exit[d0];
		    pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
		    pRoomIndex->exit[d1] = pexit;
		}
	    }

			break;
		}
	}

	return;
}



/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile (MOB_INDEX_DATA * pMobIndex)
{
	CHAR_DATA *mob;
	int tempvalue;
	int i;
	int j;

	if (pMobIndex == NULL)
	{
		bug ("Create_mobile: NULL pMobIndex.", 0);
		exit (1);
	}

	if (char_free == NULL)
	{
		mob = alloc_perm (sizeof (*mob));
	}
	else
	{
		mob = char_free;
		char_free = char_free->next;
	}

	clear_char (mob);
	mob->pIndexData = pMobIndex;
	mob->name = str_dup ("");
	mob->hunting = str_dup ("");
	mob->lord = str_dup ("");
	mob->clan = str_dup ("");
	mob->side = str_dup ("");
	mob->morph = str_dup ("");
	mob->createtime = str_dup ("");
	mob->lasttime = str_dup ("");
	mob->lasthost = str_dup ("");
	mob->powertype = str_dup ("");
	mob->poweraction = str_dup ("");
	mob->pload = str_dup ("");
	mob->prompt = str_dup ("");
	mob->cprompt = str_dup ("");

	mob->health_level = HEALTH_NORMAL;

	mob->attributes[ATTRIB_STR] = 1;
	mob->attributes[ATTRIB_DEX] = 1;
	mob->attributes[ATTRIB_STA] = 1;
	mob->attributes[ATTRIB_CHA] = 1;
	mob->attributes[ATTRIB_MAN] = 1;
	mob->attributes[ATTRIB_APP] = 1;
	mob->attributes[ATTRIB_PER] = 1;
	mob->attributes[ATTRIB_INT] = 1;
	mob->attributes[ATTRIB_WIT] = 1;

	mob->mod_attributes[ATTRIB_STR] = 0;
	mob->mod_attributes[ATTRIB_DEX] = 0;
	mob->mod_attributes[ATTRIB_STA] = 0;
	mob->mod_attributes[ATTRIB_CHA] = 0;
	mob->mod_attributes[ATTRIB_MAN] = 0;
	mob->mod_attributes[ATTRIB_APP] = 0;
	mob->mod_attributes[ATTRIB_PER] = 0;
	mob->mod_attributes[ATTRIB_INT] = 0;
	mob->mod_attributes[ATTRIB_WIT] = 0;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			mob->abilities[i][j] = 1;
		}
	}



	mob->name = str_dup(pMobIndex->player_name);
	mob->short_descr = str_dup(pMobIndex->short_descr);
	mob->long_descr = str_dup(pMobIndex->long_descr);
	mob->description = str_dup(pMobIndex->description);
	mob->spec_fun = pMobIndex->spec_fun;

	mob->home = 3001;
	mob->form = 0;
	mob->body = 32767;
	mob->level = pMobIndex->level;
	mob->act = pMobIndex->act;
/*
    if (IS_SET(mob->act, ACT_SKILLED))
*/


	mob->affected_by = pMobIndex->affected_by;
//
	 mob->affected_by2	= pMobIndex->affected_by2;
	mob->alignment = pMobIndex->alignment;
	mob->sex = pMobIndex->sex;

	mob->armor = interpolate (mob->level, 100, -100);

	tempvalue = mob->level;

	mob->max_hit = tempvalue;

	mob->hit = mob->max_hit;

	mob->hitroll = mob->level;
	mob->damroll = mob->level;

	/*
	 * Insert in list.
	 */
	mob->next = char_list;
	char_list = mob;
	pMobIndex->count++;
	return mob;
}



/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object (OBJ_INDEX_DATA * pObjIndex, int level)
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;

	if (pObjIndex == NULL)
	{
		bug ("Create_object: NULL pObjIndex.", 0);
//      exit( 1 );
		return NULL;
	}

	if (obj_free == NULL)
	{
		obj = alloc_perm (sizeof (*obj));
	}
	else
	{
		obj = obj_free;
		obj_free = obj_free->next;
	}

	*obj = obj_zero;
	obj->pIndexData = pObjIndex;
	obj->in_room = NULL;
	obj->to_room = NULL;
	obj->level = level;
	obj->wear_loc = -1;

	obj->name = str_dup(pObjIndex->name);
	obj->short_descr = str_dup(pObjIndex->short_descr);
	obj->description = str_dup(pObjIndex->description);

	if (pObjIndex->chpoweron != NULL)
	{
		obj->chpoweron = str_dup(pObjIndex->chpoweron);
		obj->chpoweroff = str_dup(pObjIndex->chpoweroff);
		obj->chpoweruse = str_dup(pObjIndex->chpoweruse);
		obj->victpoweron = str_dup(pObjIndex->victpoweron);
		obj->victpoweroff = str_dup(pObjIndex->victpoweroff);
		obj->victpoweruse = str_dup(pObjIndex->victpoweruse);
		obj->spectype = pObjIndex->spectype;
		obj->specpower = pObjIndex->specpower;
	}
	else
	{
		obj->chpoweron = str_dup ("(null)");
		obj->chpoweroff = str_dup ("(null)");
		obj->chpoweruse = str_dup ("(null)");
		obj->victpoweron = str_dup ("(null)");
		obj->victpoweroff = str_dup ("(null)");
		obj->victpoweruse = str_dup ("(null)");
		obj->spectype = 0;
		obj->specpower = 0;
	}
	obj->actcommands = str_dup ("");
	obj->questmaker = str_dup ("");
	obj->questowner = str_dup ("");

	obj->chobj = NULL;
	obj->sat = NULL;

	obj->quest = 0;
	obj->points = 0;

	obj->item_type = pObjIndex->item_type;
	obj->extra_flags = pObjIndex->extra_flags;
	obj->wear_flags = pObjIndex->wear_flags;
	obj->value[0] = pObjIndex->value[0];
	obj->value[1] = pObjIndex->value[1];
	obj->value[2] = pObjIndex->value[2];
	obj->value[3] = pObjIndex->value[3];
	obj->weight = pObjIndex->weight;
	obj->cost = pObjIndex->cost;

	if (obj->pIndexData->vnum >= 29500 && obj->pIndexData->vnum <= 29599)
	{
		obj->condition = 100;
		obj->toughness = 100;
		obj->resistance = 1;
		obj->level = 60;
		obj->cost = 1000000;
		switch (obj->pIndexData->vnum)
		{
		default:
			break;
		case 29512:
			break;
		}
	}
	else if (obj->pIndexData->vnum >= 29600 && obj->pIndexData->vnum <= 29699)
	{
		obj->condition = 100;
		obj->toughness = 100;
		obj->resistance = 1;
	}
	else
	{
		obj->condition = 100;
		obj->toughness = 5;
		obj->resistance = 25;
	}

	/*
	 * Mess with object properties.
	 */
	switch (obj->item_type)
	{
	default:
		bug ("Read_object: vnum %d bad type.", pObjIndex->vnum);
		break;

	case ITEM_LIGHT:
	case ITEM_TREASURE:
	case ITEM_FURNITURE:
	case ITEM_TRASH:
	case ITEM_CONTAINER:
	case ITEM_DRINK_CON:
	case ITEM_KEY:
	case ITEM_FOOD:
	case ITEM_BOAT:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_FOUNTAIN:
	case ITEM_PORTAL:
	case ITEM_EGG:
	case ITEM_WEAPON:
	case ITEM_ARMOR:
	case ITEM_VOODOO:
	case ITEM_STAKE:
	case ITEM_MISSILE:
	case ITEM_AMMO:
	case ITEM_QUEST:
	case ITEM_QUESTCARD:
	case ITEM_QUESTMACHINE:
	case ITEM_SYMBOL:
	case ITEM_BOOK:
	case ITEM_PAGE:
	case ITEM_TOOL:
	case ITEM_RESOURCE:
	case ITEM_CREATURE:
	case ITEM_WALL:
	case ITEM_CONTRACT:
		break;

	case ITEM_SCROLL:
		obj->value[0] = number_fuzzy (obj->value[0]);
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		obj->value[0] = number_fuzzy (obj->value[0]);
		obj->value[1] = number_fuzzy (obj->value[1]);
		obj->value[2] = obj->value[1];
		break;

	case ITEM_POTION:
	case ITEM_PILL:
		obj->value[0] = number_fuzzy (number_fuzzy (obj->value[0]));
		break;

	case ITEM_MONEY:
		obj->value[0] = obj->cost;
		break;
	}

	obj->next = object_list;
	object_list = obj;
	pObjIndex->count++;

	return obj;
}



/*
 * Clear a new character.
 */
void clear_char (CHAR_DATA * ch)
{
	static CHAR_DATA ch_zero;

	*ch = ch_zero;
	ch->name = &str_empty[0];
	ch->short_descr = &str_empty[0];
	ch->long_descr = &str_empty[0];
	ch->description = &str_empty[0];
	ch->lord = &str_empty[0];
	ch->clan = &str_empty[0];
	ch->side = &str_empty[0];
	ch->morph = &str_empty[0];
	ch->createtime = &str_empty[0];
	ch->lasthost = &str_empty[0];
	ch->lasttime = &str_empty[0];
	ch->powertype = &str_empty[0];
	ch->poweraction = &str_empty[0];
	ch->pload = &str_empty[0];
	ch->prompt = &str_empty[0];
	ch->cprompt = &str_empty[0];
	ch->hunting = &str_empty[0];
	ch->plane = 0;
	ch->logon = current_time;
	ch->armor = 100;
	ch->position = POS_STANDING;
	ch->hit = 7;
	ch->max_hit = 7;
	ch->mana = 1000;
	ch->max_mana = 1000;
	ch->move = 1000;
	ch->max_move = 1000;
	ch->master = NULL;
	ch->leader = NULL;
	ch->fighting = NULL;
	ch->mount = NULL;
	ch->wizard = NULL;
	ch->embrace = NULL;
	ch->sat = NULL;
	ch->next_fight = NULL;
	ch->prev_fight = NULL;
	ch->paradox[0] = 0;
	ch->paradox[1] = 0;
	ch->paradox[2] = 0;
	ch->damcap[0] = 1000;
	ch->damcap[1] = 0;
	ch->race = 0;
	ch->class = 0;
	ch->embraced = 0;
	ch->blood[0] = 10;
	ch->blood[1] = 10;
	ch->blood[2] = 1;
	ch->init = 0;
	ch->next_wilderness = NULL;
	ch->prev_wilderness = NULL;
	ch->x = 0;
	ch->y = 0;
	ch->home_x = 0;
	ch->home_y = 0;
	ch->room = 0;
	ch->state = 0;
	ch->action = 0;
	ch->quality = 0;
	return;
}



/*
 * Free a character.
 */
void free_extra (EXTRA *extra)
{
 if (!extra)
   return;

  free_string(extra->catagory);
  free_string(extra->field);
  free_string(extra->extra);

  extra->next = extra_free;
  extra_free = extra;

}
void free_char (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		extract_obj (obj);
	}

	for (paf = ch->affected; paf != NULL; paf = paf_next)
	{
		paf_next = paf->next;
		affect_remove (ch, paf);
	}

	free_string (ch->name);
	free_string (ch->short_descr);
	free_string (ch->long_descr);
	free_string (ch->description);
	free_string (ch->lord);
	free_string (ch->clan);
	free_string (ch->side);
	free_string (ch->nature);
	free_string (ch->demeanor);
	free_string (ch->order);
	free_string (ch->impulse);
	free_string (ch->morph);
	free_string (ch->createtime);
	free_string (ch->lasttime);
	free_string (ch->lasthost);
	free_string (ch->powertype);
	free_string (ch->poweraction);
	free_string (ch->pload);
	free_string (ch->prompt);
	free_string (ch->cprompt);
	free_string (ch->hunting);
	free_string (ch->ringtone);
	while(ch->memory != NULL)
	{
		remove_memory_node(ch, ch->memory);
	}

	if (ch->pcdata != NULL)
	{
		free_string (ch->pcdata->doing);
		free_string (ch->pcdata->aura);
		free_string (ch->pcdata->voice);
		free_string (ch->pcdata->roomdesc);
		free_string (ch->pcdata->pwd);
		free_string (ch->pcdata->bamfin);
		free_string (ch->pcdata->bamfout);
		free_string (ch->pcdata->title);
		free_string (ch->pcdata->pretitle);
		free_string (ch->pcdata->conception);
		free_string (ch->pcdata->parents);
		free_string (ch->pcdata->cparents);
		free_string (ch->pcdata->marriage);
		free_string (ch->pcdata->email);
		free_string (ch->pcdata->lupusdescshort);
		free_string (ch->pcdata->crinosdescshort);
		free_string (ch->pcdata->hispodescshort);
		free_string (ch->pcdata->glabrodescshort);
		free_string (ch->pcdata->crinosdesc);
		free_string (ch->pcdata->tempdesc);
		free_string (ch->pcdata->lupusdesc);
		free_string (ch->pcdata->glabrodesc);
		free_string (ch->pcdata->hispodesc);
		free_string (ch->pcdata->giftlist);
		free_string (ch->pcdata->conding);
		free_string (ch->pcdata->love);
		free_string (ch->pcdata->coterie);
		free_string (ch->pcdata->cot_time);
		free_string (ch->pcdata->cot_clans);
		free_string (ch->pcdata->cot_sides);
		free_string (ch->pcdata->leader);
		free_string (ch->pcdata->stancename);
		free_string (ch->pcdata->stancelist);
		free_string (ch->pcdata->stancework);
		ch->pcdata->next = pcdata_free;
		pcdata_free = ch->pcdata;
	}

	ch->next = char_free;
	char_free = ch;
	return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr (const char *name, EXTRA_DESCR_DATA * ed)
{
	for (; ed != NULL; ed = ed->next)
	{
		if (is_name (name, ed->keyword))
			return ed->description;
	}
	return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index (int vnum)
{
	MOB_INDEX_DATA *pMobIndex;

	for (pMobIndex = mob_index_hash[vnum % MAX_KEY_HASH]; pMobIndex != NULL; pMobIndex = pMobIndex->next)
	{
		if (pMobIndex->vnum == vnum)
			return pMobIndex;
	}

	if (fBootDb)
	{
		bug ("Get_mob_index: bad vnum %d.", vnum);
		exit (1);
	}

	return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index (int vnum)
{
	OBJ_INDEX_DATA *pObjIndex;

	for (pObjIndex = obj_index_hash[vnum % MAX_KEY_HASH]; pObjIndex != NULL; pObjIndex = pObjIndex->next)
	{
		if (pObjIndex->vnum == vnum)
			return pObjIndex;
	}

	if (fBootDb)
	{
		bug ("Get_obj_index: bad vnum %d.", vnum);
		exit (1);
	}

	return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index (int vnum)
{
	ROOM_INDEX_DATA *pRoomIndex;

	for (pRoomIndex = room_index_hash[vnum % MAX_KEY_HASH]; pRoomIndex != NULL; pRoomIndex = pRoomIndex->next)
	{
		if (pRoomIndex->vnum == vnum)
			return pRoomIndex;
	}

	if (fBootDb)
	{
		bug ("Get_room_index: bad vnum %d.", vnum);
		exit (1);
	}

	return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter (FILE * fp)
{
	char c;

	do
	{
		c = getc (fp);
	}
	while (isspace (c));

	return c;
}



/*
 * Read a number from a file.
 */
int fread_number (FILE * fp)
{
	int number;
	bool sign;
	char c;

	do
	{
		c = getc (fp);
	}
	while (isspace (c));

	number = 0;

	sign = FALSE;
	if (c == '+')
	{
		c = getc (fp);
	}
	else if (c == '-')
	{
		sign = TRUE;
		c = getc (fp);
	}

	if (!isdigit (c))
	{
		bug ("Fread_number: bad format.", 0);
		exit (1);
	}

	while (isdigit (c))
	{
		number = number * 10 + c - '0';
		c = getc (fp);
	}

	if (sign)
		number = 0 - number;

	if (c == '|')
		number += fread_number (fp);
	else if (c != ' ')
		ungetc (c, fp);

	return number;
}

/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 */
char *fread_string (FILE * fp)
{
	char *plast;
	char c;

	plast = top_string + sizeof (char *);
	if (plast > &string_space[MAX_STRING - MAX_STRING_LENGTH])
	{
		bug ("Fread_string: MAX_STRING %d exceeded.", MAX_STRING);
		exit (1);
	}

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do
	{
		c = getc (fp);
	}
	while (isspace (c));

	if ((*plast++ = c) == '~')
		return &str_empty[0];

	for (;;)
	{
		/*
		 * Back off the char type lookup,
		 *   it was too dirty for portability.
		 *   -- Furey
		 */
		switch (*plast = getc (fp))
		{
		default:
			plast++;
			break;

		case EOF:
			bug ("Fread_string: EOF", 0);
			exit (1);
			break;

		case '\n':
			plast++;
			*plast++ = '\r';
			break;

		case '\r':
			break;

		case '~':
			plast++;
			{
				union
				{
					char *pc;
					char rgc[sizeof (char *)];
				}
				u1;
				int ic;
				int iHash;
				char *pHash;
				char *pHashPrev;
				char *pString;

				plast[-1] = '\0';
				iHash = UMIN (MAX_KEY_HASH - 1, plast - 1 - top_string);
				for (pHash = string_hash[iHash]; pHash; pHash = pHashPrev)
				{
					for (ic = 0; ic < sizeof (char *); ic++)
						u1.rgc[ic] = pHash[ic];
					pHashPrev = u1.pc;
					pHash += sizeof (char *);

					if (top_string[sizeof (char *)] == pHash[0] && !strcmp (top_string + sizeof (char *) + 1, pHash + 1))
						return pHash;
				}

				if (fBootDb)
				{
					pString = top_string;
					top_string = plast;
					u1.pc = string_hash[iHash];
					for (ic = 0; ic < sizeof (char *); ic++)
						pString[ic] = u1.rgc[ic];
					string_hash[iHash] = pString;

					nAllocString += 1;
					sAllocString += top_string - pString;
					return pString + sizeof (char *);
				}
				else
				{
					return str_dup (top_string + sizeof (char *));
				}
			}
		}
	}
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol (FILE * fp)
{
	char c;

	do
	{
		c = getc (fp);
	}
	while (c != '\n' && c != '\r');

	do
	{
		c = getc (fp);
	}
	while (c == '\n' || c == '\r');

	ungetc (c, fp);
	return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word (FILE * fp)
{
	static char word[MAX_INPUT_LENGTH];
	char *pword;
	char cEnd;

	do
	{
		cEnd = getc (fp);
	}
	while (isspace (cEnd));

	if (cEnd == '\'' || cEnd == '"')
	{
		pword = word;
	}
	else
	{
		word[0] = cEnd;
		pword = word + 1;
		cEnd = ' ';
	}

	for (; pword < word + MAX_INPUT_LENGTH; pword++)
	{
		*pword = getc (fp);
		if (cEnd == ' ' ? isspace (*pword) : *pword == cEnd)
		{
			if (cEnd == ' ')
				ungetc (*pword, fp);
			*pword = '\0';
			return word;
		}
	}

	bug ("Fread_word: word too long.", 0);
	exit (1);
	return NULL;
}



/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem (int sMem)
{
	void *pMem;
	int iList;

	for (iList = 0; iList < MAX_MEM_LIST; iList++)
	{
		if (sMem <= rgSizeList[iList])
			break;
	}

	if (iList == MAX_MEM_LIST)
	{
		bug ("Alloc_mem: size %d too large.", sMem);
		exit (1);
	}

	if (rgFreeList[iList] == NULL)
	{
		pMem = alloc_perm (rgSizeList[iList]);
	}
	else
	{
		pMem = rgFreeList[iList];
		rgFreeList[iList] = *((void **) rgFreeList[iList]);
	}
	return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem (void *pMem, int sMem)
{
	int iList;
	for (iList = 0; iList < MAX_MEM_LIST; iList++)
	{
		if (sMem <= rgSizeList[iList])
			break;
	}

	if (iList == MAX_MEM_LIST)
	{
		bug ("Free_mem: size %d too large.", sMem);
		exit (1);
	}

	*((void **) pMem) = rgFreeList[iList];
	rgFreeList[iList] = pMem;
	return;
}



/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm (int sMem)
{
	static char *pMemPerm;
	static int iMemPerm;
	void *pMem;
	while (sMem % sizeof (long) != 0)
		sMem++;
	if (sMem > MAX_PERM_BLOCK)
	{
		bug ("Alloc_perm: %d too large.", sMem);
		exit (1);
	}

	if (pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK)
	{
		iMemPerm = 0;
		if ((pMemPerm = calloc (1, MAX_PERM_BLOCK)) == NULL)
		{
			perror ("Alloc_perm");
			exit (1);
		}
	}

	pMem = pMemPerm + iMemPerm;
	iMemPerm += sMem;
	nAllocPerm += 1;
	sAllocPerm += sMem;
	return pMem;
}

/*
 * Duplicate a string into dynamic memory.
 * Fread_strings are read-only and shared.
 */
char *str_dup (const char *str)
{
	char *str_new;

	if (str[0] == '\0')
		return &str_empty[0];

	if (str >= string_space && str < top_string)
		return (char *) str;

	str_new = alloc_mem (strlen (str) + 1);
	strcpy (str_new, str);
	return str_new;
}



/*
 * Free a string.
 * Null is legal here to simplify callers.
 * Read-only shared strings are not touched.
 */
void free_string (char *pstr)
{
	if (pstr == NULL || pstr == &str_empty[0] || (pstr >= string_space && pstr < top_string))
		return;

	free_mem (pstr, strlen (pstr) + 1);
	return;
}


void do_areas (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char addbuf[MAX_STRING_LENGTH];
	AREA_DATA *pArea;

	send_to_char( "#RAreas #bTwilight Reconquista#n.:\n\r", ch);
	sprintf(buf,"#w%-36s #e|#w %-30s#n\n\r", "Area Name", "Builders");
	for (pArea = area_first; pArea != NULL; pArea = pArea->next)
	{
		sprintf (addbuf, "#R[#w%-35s#R] #r(#w%-30s#r)#n\n\r", 
		  pArea->name, pArea->builders);
		strcat( buf, addbuf );
	}

	page_to_char (buf, ch);
	return;
}



void do_memory (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	sprintf (buf, "Affects %5d\n\r", top_affect);
	send_to_char (buf, ch);
	sprintf (buf, "Areas   %5d\n\r", top_area);
	send_to_char (buf, ch);
	sprintf (buf, "RmTxt   %5d\n\r", top_rt);
	send_to_char (buf, ch);
	sprintf (buf, "ExDes   %5d\n\r", top_ed);
	send_to_char (buf, ch);
	sprintf (buf, "Exits   %5d\n\r", top_exit);
	send_to_char (buf, ch);
	sprintf (buf, "Helps   %5d\n\r", top_help);
	send_to_char (buf, ch);
	sprintf (buf, "Mobs    %5d\n\r", top_mob_index);
	send_to_char (buf, ch);
	sprintf (buf, "Objs    %5d\n\r", top_obj_index);
	send_to_char (buf, ch);
	sprintf (buf, "Resets  %5d\n\r", top_reset);
	send_to_char (buf, ch);
	sprintf (buf, "Rooms   %5d\n\r", top_room);
	send_to_char (buf, ch);
	sprintf (buf, "Shops   %5d\n\r", top_shop);
	send_to_char (buf, ch);

	sprintf (buf, "Strings %5d strings of %7d bytes (max %d).\n\r", nAllocString, sAllocString, MAX_STRING);
	send_to_char (buf, ch);

	sprintf (buf, "Perms   %5d blocks  of %7d bytes.\n\r", nAllocPerm, sAllocPerm);
	send_to_char (buf, ch);

	return;
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy (int number)
{
	switch (number_bits (2))
	{
	case 0:
		number -= 1;
		break;
	case 3:
		number += 1;
		break;
	}

	return UMAX (1, number);
}



/*
 * Generate a random number.
 */
int number_range (int from, int to)
{
	int power;
	int number;

	if ((to = to - from + 1) <= 1)
		return from;

	for (power = 2; power < to; power <<= 1)
		;

	while ((number = number_mm () & (power - 1)) >= to)
		;

	return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent (void)
{
	int percent;

	while ((percent = number_mm () & (128 - 1)) > 99)
		;

	return 1 + percent;
}

int number_percent2 (void)
{
	return ((rand()%100)+1);
}



/*
 * Generate a random door.
 */
int number_door (void)
{
	int door;

	while ((door = number_mm () & (8 - 1)) > 5)
		;

	return door;
}



int number_bits (int width)
{
	return number_mm () & ((1 << width) - 1);
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static int rgiState[2 + 55];

void init_mm ()
{
	int *piState;
	int iState;

	piState = &rgiState[2];

	piState[-2] = 55 - 55;
	piState[-1] = 55 - 24;

	piState[0] = ((int) current_time) & ((1 << 30) - 1);
	piState[1] = 1;
	for (iState = 2; iState < 55; iState++)
	{
		piState[iState] = (piState[iState - 1] + piState[iState - 2]) & ((1 << 30) - 1);
	}
	return;
}



int number_mm (void)
{
	int *piState;
	int iState1;
	int iState2;
	int iRand;

	piState = &rgiState[2];
	iState1 = piState[-2];
	iState2 = piState[-1];
	iRand = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
	piState[iState1] = iRand;
	if (++iState1 == 55)
		iState1 = 0;
	if (++iState2 == 55)
		iState2 = 0;
	piState[-2] = iState1;
	piState[-1] = iState2;
	return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice (int number, int size)
{
	int idice;
	int sum;

	switch (size)
	{
	case 0:
		return 0;
	case 1:
		return number;
	}

	for (idice = 0, sum = 0; idice < number; idice++)
		sum += number_range (1, size);

	return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate (int level, int value_00, int value_32)
{
	return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde (char *str)
{
	for (; *str != '\0'; str++)
	{
		if (*str == '~')
			*str = '-';
	}

	return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp (const char *astr, const char *bstr)
{
	if (astr == NULL)
	{
		bug ("Str_cmp: null astr.", 0);
		return TRUE;
	}

	if (bstr == NULL)
	{
		bug ("Str_cmp: null bstr.", 0);
		return TRUE;
	}

	for (; *astr || *bstr; astr++, bstr++)
	{
		if (LOWER (*astr) != LOWER (*bstr))
			return TRUE;
	}

	return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix (const char *astr, const char *bstr)
{
	if (astr == NULL)
	{
		bug ("Strn_cmp: null astr.", 0);
		return TRUE;
	}

	if (bstr == NULL)
	{
		bug ("Strn_cmp: null bstr.", 0);
		return TRUE;
	}

	for (; *astr; astr++, bstr++)
	{
		if (LOWER (*astr) != LOWER (*bstr))
			return TRUE;
	}

	return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix (const char *astr, const char *bstr)
{
	int sstr1;
	int sstr2;
	int ichar;
	char c0;

	if ((c0 = LOWER (astr[0])) == '\0')
		return FALSE;

	sstr1 = strlen (astr);
	sstr2 = strlen (bstr);

	for (ichar = 0; ichar <= sstr2 - sstr1; ichar++)
	{
		if (c0 == LOWER (bstr[ichar]) && !str_prefix (astr, bstr + ichar))
			return FALSE;
	}

	return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix (const char *astr, const char *bstr)
{
	int sstr1;
	int sstr2;

	sstr1 = strlen (astr);
	sstr2 = strlen (bstr);
	if (sstr1 <= sstr2 && !str_cmp (astr, bstr + sstr2 - sstr1))
		return FALSE;
	else
		return TRUE;
}



/*
 * Returns an initial-capped string.
 */
char *capitalize (const char *str)
{
	static char strcap[MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strcap[i] = LOWER (str[i]);
	strcap[i] = '\0';
	strcap[0] = UPPER (strcap[0]);
	return strcap;
}


/*
 * Returns an completely capitalized string.
 */
char *upper_cap (const char *str)
{
	static char strcap[MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strcap[i] = UPPER (str[i]);
	strcap[i] = '\0';
	return strcap;
}



/*
 * Append a string to a file.
 */
void append_file (CHAR_DATA * ch, char *file, char *str)
{
	FILE *fp;

	if (IS_NPC (ch) || str[0] == '\0')
		return;

	fflush (fpReserve);
	fclose (fpReserve);

	if ((fp = fopen (file, "a")) == NULL)
	{
		perror (file);
		send_to_char ("Could not open the file!\n\r", ch);
	}
	else
	{
		fprintf (fp, "[%5d] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0, ch->name, str);
		fflush (fp);
		fclose (fp);
	}
	fpReserve = fopen (NULL_FILE, "r");
	return;
}



/*
 * Reports a bug.
 */
void bug (const char *str, int param)
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;

	if (fpArea != NULL)
	{
		int iLine;
		int iChar;

		if (fpArea == stdin)
		{
			iLine = 0;
		}
		else
		{
			iChar = ftell (fpArea);
			fseek (fpArea, 0, 0);
			for (iLine = 0; ftell (fpArea) < iChar; iLine++)
			{
				while (getc (fpArea) != '\n')
					;
			}
			fseek (fpArea, iChar, 0);
		}

		sprintf (buf, "[*****] FILE: %s LINE: %d", strArea, iLine);
		log_string (buf, CHANNEL_LOG_NORMAL);

		if ((fp = fopen ("shutdown.txt", "a")) != NULL)
		{
			fprintf (fp, "[*****] %s\n", buf);
			fflush (fp);
			fclose (fp);
		}
	}

	strcpy (buf, "[*****] BUG: ");
	sprintf (buf + strlen (buf), str, param);
	log_string (buf, CHANNEL_LOG_NORMAL);

	fflush (fpReserve);
	fclose (fpReserve);
	if ((fp = fopen (BUG_FILE, "a")) != NULL)
	{
		fprintf (fp, "%s\n", buf);
		fflush (fp);
		fclose (fp);
	}
	fpReserve = fopen (NULL_FILE, "r");

	return;
}


void log_string_quiet (const char *str, int bit)
{
	char *strtime;
	char logout[MAX_STRING_LENGTH];

	strtime = ctime (&current_time);
	strtime[strlen (strtime) -1] = '\0';
	fprintf( stderr, "%s :: %s\n", strtime, str);
	strcpy (logout, str);
	return;
}

void log_string (const char *str, int bit)
{
	char *strtime;
	char logout[MAX_STRING_LENGTH];

	strtime = ctime (&current_time);
	strtime[strlen (strtime) - 1] = '\0';
	fprintf (stderr, "%s :: %s\n", strtime, str);
	strcpy (logout, str);
	logchan (logout, bit);
	return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain (void)
{
	return;
}




//mob program stuff
/* the functions */

/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */

int mprog_name_to_type (char *name)
{
	if (!str_cmp (name, "in_file_prog"))
		return IN_FILE_PROG;
	if (!str_cmp (name, "act_prog"))
		return ACT_PROG;
	if (!str_cmp (name, "speech_prog"))
		return SPEECH_PROG;
	if (!str_cmp (name, "rand_prog"))
		return RAND_PROG;
	if (!str_cmp (name, "fight_prog"))
		return FIGHT_PROG;
	if (!str_cmp (name, "death_prog"))
		return DEATH_PROG;

	if (!str_cmp (name, "entry_prog"))
		return ENTRY_PROG;
	if (!str_cmp (name, "greet_prog"))
		return GREET_PROG;
	if (!str_cmp (name, "all_greet_prog"))
		return ALL_GREET_PROG;
	if (!str_cmp (name, "give_prog"))
		return GIVE_PROG;
	if (!str_cmp (name, "bribe_prog"))
		return BRIBE_PROG;
//	if (!str_cmp (name, "buy_prog"))
//		return BUY_PROG;


	return (ERROR_PROG);
}

/* This routine reads in scripts of MOBprograms from a file */

/* This routine reads in scripts of MOBprograms from a file */
MPROG_DATA *mprog_file_read (f, mprg, pMobIndex)
     char *f;
     MPROG_DATA *mprg;
     MOB_INDEX_DATA *pMobIndex;
{
	MPROG_DATA *mprg2;
	FILE *progfile;
	char letter;
	char buf[MAX_STRING_LENGTH];
	bool done = FALSE;
	char MOBProgfile[MAX_INPUT_LENGTH];
	MPROG_RESETS *temp;

	sprintf (MOBProgfile, "%s%s", MOB_DIR, f);
	pMobIndex->mobprog_filename = str_dup(f);
	temp = alloc_perm(sizeof(MPROG_RESETS));
	temp->vnum = pMobIndex->vnum;
	temp->filename = str_dup(f);
	temp->next = NULL;
	
	if (pMobIndex->area->mprogs == NULL)
		pMobIndex->area->mprogs = temp;
	else
	{
		temp->next = pMobIndex->area->mprogs;
		pMobIndex->area->mprogs = temp;
	}
	progfile = fopen (MOBProgfile, "r");
	if (!progfile)
	{
		sprintf(buf, "Mob: couldn't open mobprog file %s", MOBProgfile);
		bug (buf, 0);
		exit (1);
	}
	mprg2 = mprg;
	switch (letter = fread_letter (progfile))
	{
	case '>':
		break;
	case '|':
		bug ("empty mobprog file.", 0);
		exit (1);
		break;
	default:
		bug ("in mobprog file syntax error.", 0);
		exit (1);
		break;
	}
	while (!done)
	{
		mprg2->type = mprog_name_to_type (fread_word (progfile));
		switch (mprg2->type)
		{
		case ERROR_PROG:
			bug ("mobprog file type error", 0);
			exit (1);
			break;
		case IN_FILE_PROG:
			bug ("mprog file contains a call to file.", 0);
			exit (1);
			break;
		default:
			pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
			mprg2->arglist = fread_string (progfile);
			mprg2->comlist = fread_string (progfile);
			switch (letter = fread_letter (progfile))
			{
			case '>':
				mprg2->next = (MPROG_DATA *) alloc_perm (sizeof (MPROG_DATA));
				mprg2 = mprg2->next;
				mprg2->next = NULL;
				break;
			case '|':
				done = TRUE;
				break;
			default:
				bug ("in mobprog file syntax error.", 0);
				exit (1);
				break;
			}
			break;
		}
	}
	fclose (progfile);
	return mprg2;
}


/* Snarf a MOBprogram section from the area file.
 */
void load_mobprogs (fp)
     FILE *fp;
{
	char letter;
	MOB_INDEX_DATA *iMob;
	int value;
	MPROG_DATA *working;

	for (;;)
		switch (letter = fread_letter (fp))
		{
		default:
			bug ("Load_mobprogs: bad command '%c'.", letter);
			exit (1);
			break;
		case 'S':
		case 's':
			fread_to_eol (fp);
			return;
		case '*':
			fread_to_eol (fp);
			break;
		case 'M':
		case 'm':
			value = fread_number (fp);
			if ((iMob = get_mob_index (value)) == NULL)
			{
				bug ("Load_mobprogs: vnum %d doesnt exist", value);
				exit (1);
			}

			working = (MPROG_DATA *) alloc_perm (sizeof (MPROG_DATA));
			iMob->mobprogs = working;
			working = mprog_file_read (fread_word (fp), working, iMob);
			working->next = NULL;
			fread_to_eol (fp);
			break;
		}
}

/* This procedure is responsible for reading any in_file MOBprograms.
 */
void mprog_read_programs (fp, pMobIndex)
     FILE *fp;
     MOB_INDEX_DATA *pMobIndex;
{
	MPROG_DATA *mprg;
	bool done = FALSE;
	char letter;
	if ((letter = fread_letter (fp)) != '>')
	{
		bug ("Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum);
		exit (1);
	}
	pMobIndex->mobprogs = (MPROG_DATA *) alloc_perm (sizeof (MPROG_DATA));
	mprg = pMobIndex->mobprogs;
	while (!done)
	{
		mprg->type = mprog_name_to_type (fread_word (fp));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug ("Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum);
			exit (1);
			break;
		case IN_FILE_PROG:
			mprg = mprog_file_read (fread_string (fp), mprg, pMobIndex);
			fread_to_eol (fp);
			switch (letter = fread_letter (fp))
			{
			case '>':
				mprg->next = (MPROG_DATA *) alloc_perm (sizeof (MPROG_DATA));
				mprg = mprg->next;
				mprg->next = NULL;
				break;
			case '|':
				mprg->next = NULL;
				fread_to_eol (fp);
				done = TRUE;
				break;
			default:
				bug ("Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum);
				exit (1);
				break;
			}
			break;
		default:
			pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
			mprg->arglist = fread_string (fp);
			fread_to_eol (fp);
			mprg->comlist = fread_string (fp);
			fread_to_eol (fp);
			switch (letter = fread_letter (fp))
			{
			case '>':
				mprg->next = (MPROG_DATA *) alloc_perm (sizeof (MPROG_DATA));
				mprg = mprg->next;
				mprg->next = NULL;
				break;
			case '|':
				mprg->next = NULL;
				fread_to_eol (fp);
				done = TRUE;
				break;
			default:
				bug ("Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum);
				exit (1);
				break;
			}
			break;
		}
	}
}


