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
#include <time.h>
#elif defined(WIN32)
#include <sys/types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include <time.h>
#if !defined(WIN32)
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include "version.h"


char *const where_name[] = {
	"#w[#eLight         #w]#n ",
	"#w[#eOn Finger     #w]#n ",
	"#w[#eOn Finger     #w]#n ",
	"#w[#eAround Neck   #w]#n ",
	"#w[#eAround Neck   #w]#n ",
	"#w[#eOn Body       #w]#n ",
	"#w[#eOn Head       #w]#n ",
	"#w[#eOn Legs       #w]#n ",
	"#w[#eOn Feet       #w]#n ",
	"#w[#eOn Hands      #w]#n ",
	"#w[#eOn Arms       #w]#n ",
	"#w[#eAs Shield     #w]#n ",
	"#w[#eAround Body   #w]#n ",
	"#w[#eAround Waist  #w]#n ",
	"#w[#eAround Wrist  #w]#n ",
	"#w[#eAround Wrist  #w]#n ",
	"#w[#eRight Hand    #w]#n ",
	"#w[#eLeft Hand     #w]#n ",
	"#w[#eOn Face       #w]#n ",
	"#w[#eLeft Scabbard #w]#n ",
	"#w[#eRight Scabbard#w]#n ",
	"#w[#eThrough Chest #w]#n ",
};



/*
 * Local functions.
 */
void show_equipment (CHAR_DATA * ch, CHAR_DATA * tch);	/* MereK */
char *format_obj_to_char args ((OBJ_DATA * obj, CHAR_DATA * ch, bool fShort));
void show_char_to_char_0 args ((CHAR_DATA * victim, CHAR_DATA * ch));
void show_char_to_char_1 args ((CHAR_DATA * victim, CHAR_DATA * ch));
char *format_obj_to_char2 (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort);
void show_char_to_char args ((CHAR_DATA * list, CHAR_DATA * ch));
bool check_blind args ((CHAR_DATA * ch));

void obj_score args ((CHAR_DATA * ch, OBJ_DATA * obj));
void pretty_proc args ((char *buf, char *word));
void desc_pretty args ((CHAR_DATA * ch, int start, int lines));
#define PLINE 74		/* Lines to word wrap */

char *show_dot args(( int dot) );
char *show_spot args(( int spot) );
char *show_slash args(( int slash) );
char *show_slash_20 args(( int slash) );
char *show_spot_5 args(( int slash) );
char *show_spot_20 args(( int slash) );

void inquis_score args(( CHAR_DATA *ch, char * argument));
void vamp_score args(( CHAR_DATA *ch, char * argument));
void shifter_score args(( CHAR_DATA *ch, char * argument));
void mortal_score args(( CHAR_DATA *ch, char * argument));

int most_login = 20;
int most_rp = 12;

char *smash_article (char *text)
{
	char *arg;
	char buf[MAX_STRING_LENGTH];
	static char buf2[MAX_STRING_LENGTH];

	one_argument (text, buf);

	if (!str_cmp (buf, "the") || !str_cmp (buf, "an") || !str_cmp (buf, "a"))
	{
		arg = one_argument (text, buf);
		sprintf (buf2, "%s", arg);
	}
	else
		strcpy (buf2, text);

	return buf2;
}


/*
 * Sees if last char is 's' and returns 'is' or 'are' pending.
 */
char *is_are (char *text)
{
	while (*text != '\0')
	{
		text++;
	}

	text--;

	if (LOWER (*text) == 's' && LOWER (*text - 1) != 's')
		return "are";
	else
		return "is";
}

/*
 * New version of pluralize, more verbose
 */
char *pluralize( char *argument )
{
   static char buf[MAX_STRING_LENGTH];
   char *v;
   char *v2;
   char xbuf[MAX_STRING_LENGTH];

   sprintf( buf, "%s", smash_article( argument ) );
   v = strstr( buf, " of " );
   v2 = strstr( buf, " with " );


   if( ( v2 && !v )
    || ( v2 && v && strlen( v2 ) > strlen( v ) ) )
   {
      v = v2;
   }

   xbuf[0] ='\0';
   if( v != NULL )
   {
      sprintf( xbuf, "%s", v );
      buf[strlen( buf ) - strlen( v )] = '\0';
   }

   if( LOWER( buf[strlen( buf ) - 1] ) == 'f' )
   {
      if( LOWER( buf[strlen( buf ) - 2] ) == 'f' )
      {
         buf[strlen( buf ) -2] = 'v';
         buf[strlen( buf ) -1] = 'e';
         strcat( buf, "s" );
      }
      else
      {
         buf[strlen( buf ) -1] = 'v';
         strcat( buf, "es" );
      }
   }
   else if( LOWER( buf[strlen( buf ) - 1] ) == 'h'
    && LOWER( buf[strlen( buf ) - 2] ) == 't' )
   {
      strcat( buf, "es" );
   }
   else if( LOWER( buf[strlen( buf ) - 1] ) == 'h'
    && LOWER( buf[strlen( buf ) - 2] ) == 's' )
   {
      strcat( buf, "es" );
   }
   else if( LOWER( buf[strlen( buf ) - 1] ) == 'h'
    && LOWER( buf[strlen( buf ) - 2] ) == 'c' )
   {
      strcat( buf, "es" );
   }
   else if( LOWER( buf[strlen( buf ) - 1] ) == 'x' )
   {
      strcat( buf, "es" );
   }
   else if( LOWER( buf[strlen( buf ) - 1]) == 's' )
   {
      if( LOWER( buf[strlen( buf ) - 2] ) == 'u'
       && !IS_VOWEL( LOWER( buf[strlen( buf ) - 3] ) ) )
      {
         buf[strlen( buf ) - 2 ] = 'i';
         buf[strlen( buf ) - 1] = '\0';
      }
      else
         strcat( buf, "es" );
   }
   else
      strcat( buf, "s" );

   strcat( buf, xbuf );
   return buf;
}


/*
 * Shows the generated object description to a character.
 * (Once again I must say, Thanks to KINO for this AWESOME idea)
 *                                   - Enigma
 *
 * This code is copyrighted by Locke of NiMUD.  This is *NOT*
 * original code by Enigma.
 */
void show_equipment (CHAR_DATA * ch, CHAR_DATA * tch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	char buf[MAX_STRING_LENGTH];
	char descr[MAX_STRING_LENGTH];
	char *finalstr;
	bool fBool;
	int oType;
	char *p1;
	char *p2;

	buf[0] = '\0';
	descr[0] = '\0';

	if (IS_NPC (ch))
		return;

	if (IS_AFFECTED (ch, AFF_HIDE))
	{
		sprintf (buf, "%s has loose garments are draped around %s body.\n\r", HE_SHE (ch), HIS_HER (ch));
		stc (buf, tch);
		return;
	}

	fBool = FALSE;

	p1 = NULL;
	p2 = NULL;

	if ((obj = get_eq_char (ch, WEAR_LIGHT)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s is holding %s high above %s head shining the area brightly\n\r", HE_SHE (ch), format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
		strcat (descr, buf);
	}


	if (((obj = get_eq_char (ch, WEAR_FINGER_L)) != NULL && can_see_obj (tch, obj)) && ((obj2 = get_eq_char (ch, WEAR_FINGER_R)) != NULL && can_see_obj (tch, obj2)) && !strcmp ((p1 = str_dup (format_obj_to_char2 (obj, ch, TRUE))), (p2 = str_dup (format_obj_to_char2 (obj2, ch, TRUE)))))
	{
		sprintf (buf, "%s is wearing two %s on %s fingers", HE_SHE (ch), pluralize (STR2 (obj, short_descr)), HIS_HER (ch));
		strcat (descr, buf);
	}
	else
	{
		if ((obj = get_eq_char (ch, WEAR_FINGER_L)) != NULL && can_see_obj (tch, obj))
		{
			sprintf (buf, "On %s left hand, %s has placed %s", HIS_HER (ch), HE_SHE (ch), PERSO (obj, ch));
			strcat (descr, buf);
			strcat (descr, ". ");
			fBool = TRUE;
		}

		if ((obj = get_eq_char (ch, WEAR_FINGER_R)) != NULL && can_see_obj (tch, obj))
		{
			if (fBool)
				sprintf (buf, ", while %s encircles a finger on %s right ", PERSO (obj, ch), HIS_HER (ch));
			else
				sprintf (buf, "On %s right hand %s %s", HIS_HER (ch), is_are (PERSO (obj, ch)), PERSO (obj, ch));
			strcat (descr, buf);
			strcat (descr, ". ");
		}
	}

	free_string (p1);
	free_string (p2);
	p1 = NULL;
	p2 = NULL;

	//checks for various claws
	if ((obj = get_eq_char (ch, WEAR_HANDS)) != NULL && can_see_obj (tch, obj))
	{
		if (descr[0] != '\0')
		{

			if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_CLAWS))
			{
				if (IS_WEREWOLF (ch) && get_auspice (ch, AUSPICE_AHROUN) > 3)
				{
					if (get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
						sprintf (buf, ", with %s covering %s hands, venom dripping silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch), HIS_HER (ch));
					else
						sprintf (buf, ", with %s covering %s hands, gleaming silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch), HIS_HER (ch));
					strcat (descr, buf);
				}
				else
				{
					if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
						sprintf (buf, ", with %s covering %s hands, venom-dripping talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch), HIS_HER (ch));
					else
						sprintf (buf, ", with %s covering %s hands, razor sharp talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch), HIS_HER (ch));
					strcat (descr, buf);
				}
			}
			else
			{
				sprintf (buf, ", with %s covering %s hands", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
				strcat (descr, buf);
			}

			if (!IS_NPC (ch) && IS_MORE (ch, MORE_HAND_FLAME))
			{
				sprintf (buf, ", which seem to be covered in flame.");
				strcat (descr, buf);
			}
			else
				strcat (descr, ". ");

		}
		else
		{

			if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_CLAWS))
			{
				if (IS_WEREWOLF (ch) && get_auspice (ch, AUSPICE_AHROUN) > 3)
				{
					if (get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
						sprintf (buf, " %s %s worn on %s hands and venom dripping silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch), HIS_HER (ch));
					else
						sprintf (buf, " %s %s worn on %s hands and gleaming silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch), HIS_HER (ch));
					strcat (descr, buf);
				}
				else
				{
					if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
						sprintf (buf, " %s %s worn on %s hands and venom-dripping talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch), HIS_HER (ch));
					else
						sprintf (buf, " %s %s worn on %s hands and razor sharp talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch), HIS_HER (ch));
					strcat (descr, buf);
				}
			}
			else
			{
				sprintf (buf, "%s %s worn on %s hands", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch));
				strcat (descr, buf);
			}
			if (!IS_NPC (ch) && IS_MORE (ch, MORE_HAND_FLAME))
			{
				sprintf (buf, ", which seem to be covered in flame.");
				strcat (descr, buf);
			}
			else
				strcat (descr, ". ");

		}

	}
	//checks for various claws
	else if (get_eq_char (ch, WEAR_HANDS) == NULL && descr[0] != '\0')
	{
		if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_CLAWS))
		{
			if (IS_WEREWOLF (ch) && get_auspice (ch, AUSPICE_AHROUN) > 3)
			{
				if (get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
					sprintf (buf, " and %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				else
					sprintf (buf, " and %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
			}
			else
			{
				if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
					sprintf (buf, " and %s has venom-dripping talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				else
					sprintf (buf, " and %s has razor sharp talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
			}

			strcat (descr, buf);

			if (!IS_NPC (ch) && IS_MORE (ch, MORE_HAND_FLAME))
			{
				sprintf (buf, ", which seem to be covered in flame.");
				strcat (descr, buf);
			}
			else
				strcat (descr, ". ");

		}
	}
	else if (get_eq_char (ch, WEAR_HANDS) == NULL && descr[0] == '\0')
	{
		if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_CLAWS))
		{
			//check for normal claws
			if (IS_WEREWOLF (ch) && get_auspice (ch, AUSPICE_AHROUN) > 3)
			{
				if (get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
					sprintf (buf, " %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				else
					sprintf (buf, " %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				strcat (descr, buf);
			}
			else
			{
				if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_BLACK_FURIES) > 1)
					sprintf (buf, " %s has venom-dripping talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				else
					sprintf (buf, " %s has razor sharp talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				strcat (descr, buf);
			}

			if (!IS_NPC (ch) && IS_MORE (ch, MORE_HAND_FLAME))
			{
				sprintf (buf, ", which seem to be covered in flame.");
				strcat (descr, buf);
			}
			else
				strcat (descr, ". ");

		}
	}
	else if (descr[0] != '\0')
		strcat (descr, ".  ");
	else
		strcat (descr, "");
	fBool = FALSE;



	if ((obj = get_eq_char (ch, WEAR_HEAD)) != NULL && can_see_obj (tch, obj))
	{


		sprintf (buf, "On %s head is a %s.", HIS_HER (ch), format_obj_to_char2 (obj, ch, TRUE));
		strcat (descr, buf);


	}

	if (get_eq_char (ch, WEAR_HEAD) == NULL && IS_VAMPIRE (ch) && (get_disc (ch, DISC_OBEAH) > 0 || get_disc (ch, DISC_VALEREN) > 2))
	{

		if (IS_MORE (ch, MORE_OBEAH))
		{
			if (ch->pcdata->obeah > 0)
			{
				sprintf (buf, "%s third eye is glowing brightly in the centre of %s forehead, ", HIS_HER (ch), HIS_HER (ch));
				strcat (descr, buf);
			}
			else
			{
				sprintf (buf, "%s has a third eye is open in the middle of %s forehead, ", HE_SHE (ch), HIS_HER (ch));
				strcat (descr, buf);
			}
		}
		else
		{
			sprintf (buf, "%s has a strange slit mark in the centre of %s forehead, ", HE_SHE (ch), HIS_HER (ch));
			strcat (descr, buf);
		}
	}


	if (((obj = get_eq_char (ch, WEAR_NECK_1)) != NULL && can_see_obj (tch, obj)) && ((obj2 = get_eq_char (ch, WEAR_NECK_2)) != NULL && can_see_obj (tch, obj2)) && !str_cmp ((p1 = str_dup (format_obj_to_char2 (obj, ch, TRUE))), (p2 = str_dup (format_obj_to_char2 (obj2, ch, TRUE)))))
	{
		sprintf (buf, "Two %s are clasped around %s neck.", pluralize (STR2 (obj, short_descr)), HIS_HER (ch));
		strcat (descr, buf);
	}
	else
	{
		if ((obj = get_eq_char (ch, WEAR_NECK_1)) != NULL && can_see_obj (tch, obj))
		{
			if (get_eq_char (ch, WEAR_NECK_2) == NULL)
				sprintf (buf, "%s %s worn about %s neck.", PERSO (obj, ch), is_are (PERSO (obj, ch)), HIS_HER (ch));
			else
				sprintf (buf, "%s and ", PERSO (obj, ch));
			strcat (descr, buf);
			fBool = TRUE;
		}

		if ((obj = get_eq_char (ch, WEAR_NECK_2)) != NULL && can_see_obj (tch, obj))
		{
			if (fBool)
				sprintf (buf, "%s are worn about %s neck.", PERSO (obj, ch), HIS_HER (ch));
			else
				sprintf (buf, "%s %s worn about %s neck.", PERSO (obj, ch), is_are (PERSO (obj, ch)), HIS_HER (ch));
			strcat (descr, buf);
		}


	}

	free_string (p1);
	free_string (p2);
	p1 = NULL;
	p2 = NULL;

	fBool = FALSE;

	if ((obj = get_eq_char (ch, WEAR_ABOUT)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s %s worn over %s ", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch));
		strcat (descr, buf);
		if ((obj = get_eq_char (ch, WEAR_BODY)) != NULL && can_see_obj (tch, obj))
		{
			sprintf (buf, "%s. ", smash_article (format_obj_to_char2 (obj, ch, TRUE)));
			strcat (descr, buf);
		}
		else
		{
			sprintf (buf, "naked chest ");
			strcat (descr, buf);
		}
		
		strcat (descr, ".  ");
	}
	else if ((obj = get_eq_char (ch, WEAR_BODY)) != NULL && can_see_obj (tch, obj) && get_eq_char (ch, WEAR_ABOUT) == NULL)
	{
		sprintf (buf, "%s is wearing %s on %s body", HE_SHE (ch), format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
		strcat (descr, buf);

		strcat (descr, ".  ");
	}

	if ((obj = get_eq_char (ch, WEAR_ARMS)) != NULL && can_see_obj (tch, obj))
	{

		sprintf (buf, "%s %s on %s arms.", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch));
		strcat (descr, buf);
	}

	if ((obj2 = get_eq_char (ch, WEAR_LEGS)) != NULL && can_see_obj (tch, obj2))
	{
		sprintf (buf, "%s is wearing %s on %s legs.", HE_SHE (ch), format_obj_to_char2 (obj2, ch, TRUE), HIS_HER (ch));
		strcat (descr, buf);
	}

	if ((obj2 = get_eq_char (ch, WEAR_FACE)) != NULL && can_see_obj (tch, obj2))
	{

		sprintf (buf, "%s has %s on %s face.", HE_SHE (ch), format_obj_to_char2 (obj2, ch, TRUE), HIS_HER (ch));
		strcat (descr, buf);

	}

	if ((obj = get_eq_char (ch, WEAR_FEET)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s wears %s as footwear.", HE_SHE (ch), format_obj_to_char2 (obj, ch, TRUE));
		strcat (descr, buf);
	}

	if (descr[0] != '\0')
	{
		send_to_char ("   ", tch);

		finalstr = format_string (str_dup (descr));
		send_to_char (finalstr, tch);
		free_string (finalstr);

		descr[0] = '\0';
	}

	if (((obj = get_eq_char (ch, WEAR_WRIST_L)) != NULL && can_see_obj (tch, obj)) && ((obj2 = get_eq_char (ch, WEAR_WRIST_R)) != NULL && can_see_obj (tch, obj2)) && !str_cmp ((p1 = str_dup (format_obj_to_char2 (obj, ch, TRUE))), (p2 = str_dup (format_obj_to_char2 (obj2, ch, TRUE)))))
	{
		sprintf (buf, "%s are on each wrist.", pluralize (STR2 (obj, short_descr)));
		strcat (descr, buf);
	}
	else
	{
		if ((obj = get_eq_char (ch, WEAR_WRIST_L)) != NULL && can_see_obj (tch, obj))
		{
			sprintf (buf, "Encircling %s left wrist, %s %s", HIS_HER (ch), is_are (format_obj_to_char2 (obj, ch, TRUE)), format_obj_to_char2 (obj, ch, TRUE));
			strcat (descr, buf);
			fBool = TRUE;
		}

		if ((obj = get_eq_char (ch, WEAR_WRIST_R)) != NULL && can_see_obj (tch, obj))
		{
			if (fBool)
				sprintf (buf, " and %s on %s right.", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
			else
				sprintf (buf, "Encircling %s right wrist %s %s.", HIS_HER (ch), is_are (format_obj_to_char2 (obj, ch, TRUE)), format_obj_to_char2 (obj, ch, TRUE));
			strcat (descr, buf);
		}
		else if (fBool)
			strcat (descr, ".  ");
	}

	free_string (p1);
	free_string (p2);
	p1 = NULL;
	p2 = NULL;

	fBool = FALSE;

	obj = get_eq_char (ch, WEAR_WIELD);
	if (!obj)
		obj = get_eq_char (ch, WEAR_HOLD);
	obj2 = get_eq_char (ch, WEAR_HOLD);
	if (!obj)
		obj = get_eq_char (ch, WEAR_WIELD);


	if (obj != NULL && can_see_obj (tch, obj) && obj2 != NULL && can_see_obj (tch, obj2) && (obj->wear_loc == obj2->wear_loc - 1) 
          && !str_cmp ((p1 = str_dup (format_obj_to_char2 (obj, ch, TRUE))), (p2 = str_dup (format_obj_to_char2 (obj2, ch, TRUE)))))
	{
		sprintf (buf, "Two %s are %s in %s hands.", pluralize 
(STR2 (obj, short_descr)), obj->wear_loc == WEAR_WIELD ? "held" : "held", HIS_HER (ch));
		strcat (descr, buf);
	}
	else
	{
		oType = ITEM_TRASH;
		if (obj != NULL && can_see_obj (tch, obj))
		{
			sprintf (buf, "%s %s %s in one hand", 
format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), obj->wear_loc == WEAR_WIELD ? "held" : "held");
			strcat (descr, buf);
			fBool = TRUE;
		}

		if (obj2 != NULL && can_see_obj (tch, obj2))
		{
			if (fBool)
			{
				if (obj->wear_loc == (obj2->wear_loc - 1))
					sprintf (buf, ", with %s in the other.", format_obj_to_char2 (obj2, ch, TRUE));
				else
					sprintf (buf, ", and %s %s %s in the other.  ", format_obj_to_char2 (obj2, ch, TRUE), is_are (format_obj_to_char2 (obj2, ch, TRUE)), obj2->wear_loc == WEAR_HOLD ? "wielded" : "held");
			}
			else
			{
				sprintf (buf, "%s %s being %s.  ", format_obj_to_char2 (obj2, ch, TRUE), is_are (format_obj_to_char2 (obj2, ch, TRUE)), obj2->wear_loc == WEAR_HOLD ? "wielded" : "held");
			}
			strcat (descr, buf);
		}
		else if (fBool)
			strcat (descr, ".  ");
	}

	free_string (p1);
	free_string (p2);
	p1 = NULL;
	p2 = NULL;

	fBool = FALSE;
	if ((obj = get_eq_char (ch, WEAR_WAIST)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s fits snugly around %s waist.", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
		strcat (descr, buf);
	}

	if ((obj = get_eq_char (ch, WEAR_SHIELD)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s serves to protect %s from attacks.", format_obj_to_char2 (obj, ch, TRUE), HIM_HER (ch));
		strcat (descr, buf);
	}


	if (get_eq_char (ch, WEAR_ABOUT) == NULL)
	{
		if (((obj = get_eq_char (ch, WEAR_SCABBARD_L)) != NULL && can_see_obj (tch, obj)) && ((obj2 = get_eq_char (ch, WEAR_SCABBARD_R)) != NULL && can_see_obj (tch, obj2)) && !str_cmp ((p1 = str_dup (format_obj_to_char2 (obj, ch, TRUE))), (p2 = str_dup (format_obj_to_char2 (obj2, ch, TRUE)))))
		{
			sprintf (buf, "Two %s are sheathed.", pluralize (STR2 (obj, short_descr)));
			strcat (descr, buf);
		}
		else
		{
			if ((obj = get_eq_char (ch, WEAR_SCABBARD_L)) != NULL && can_see_obj (tch, obj))
			{
				sprintf (buf, "In %s left sheath, is %s", HIS_HER (ch), format_obj_to_char2 (obj, ch, TRUE));
				strcat (descr, buf);
				fBool = TRUE;
			}

			if ((obj = get_eq_char (ch, WEAR_SCABBARD_R)) != NULL && can_see_obj (tch, obj))
			{
				if (fBool)
					sprintf (buf, " and %s in %s right sheath.", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
				else
					sprintf (buf, "In %s right sheath %s.", HIS_HER (ch), format_obj_to_char2 (obj, ch, TRUE));
				strcat (descr, buf);
			}
			else if (fBool)
				strcat (descr, ".  ");

		}
	}

	if (!IS_NPC (ch))
	{
		if (!IS_NPC (ch) && (IS_WEREWOLF (ch))  && ch->pcdata->wolfform[1] > FORM_HOMID)
		{
			if (IS_MORE (ch, MORE_BRISTLES))
			{
				sprintf (buf, "body covered in thick fur and quill-like spines");
				strcat (descr, buf);
			}
			else
			{
				sprintf (buf, "body is covered in a thick fur");
				strcat (descr, buf);
			}
			
				strcat (descr, ".");
		}

 		if(IS_SET(ch->polyaff, POLY_WOLF) && IS_VAMPIRE(ch))
		{
			sprintf( buf, "body is covered in a thick fur");
			strcat(descr, buf );
		}

		if (IS_VAMPAFF (ch, VAM_FANGS))
		{
			sprintf (buf, " %s has two very sharp canines extended in %s upper mouth.", 
			capitalize(HE_SHE (ch)), HIS_HER (ch));
			strcat (descr, buf);
		}
	}


	if (descr[0] != '\0')
	{
		send_to_char ("   ", tch);

		finalstr = format_string (str_dup (descr));
		send_to_char (finalstr, tch);
		free_string (finalstr);
	}

	return;
}




/*
 * New object format for the description based on equipment..
 *
 *    .. Enigma
 */
char *format_obj_to_char2 (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
	static char buf[MAX_STRING_LENGTH];
	char pre[MAX_STRING_LENGTH];

	buf[0] = '\0';

	pre[0] = '\0';

	if (IS_OBJ_STAT (obj, ITEM_INVIS))
		strcat (pre, "(invisible) ");

	if (!IS_NPC (ch) && IS_VAMPIRE (ch) && IS_OBJ_STAT (obj, ITEM_VANISH) && get_disc (ch, DISC_CHIMERSTRY) > 0)
		strcat (pre, "#w(illusion)#n ");
	if (!IS_NPC (ch) && IS_WEREWOLF (ch) && IS_OBJ_STAT (obj, ITEM_VANISH) && get_tribe (ch, TRIBE_STARGAZERS) > 2)
		strcat (pre, "#w(illusion)#n ");

	if (pre[0] != '\0')
	{
		sprintf (buf, "%s %s%s", IS_VOWEL (pre[0]) ? "an" : "a", pre, smash_article (obj->short_descr));
	}
	else
		strcat (buf, obj->short_descr);

	return buf;
}

char *format_obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
	static char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	if (obj->points > 0)
		strcat (buf, "(Legendary) ");
	
	if (IS_OBJ_STAT (obj, ITEM_INVIS))
		strcat (buf, "#e(#CI#cnvis#e)#n ");
	if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && !IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && IS_OBJ_STAT (obj, ITEM_ANTI_EVIL))
		strcat (buf, "#e(#BB#blue #BA#bura#e)#n ");
	else if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT (obj, ITEM_ANTI_EVIL))
		strcat (buf, "#e(#RR#red #RA#rura#e)#n ");
	else if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT (obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT (obj, ITEM_ANTI_EVIL))
		strcat (buf, "#e(#yY#oellow #yA#oura#e)#n ");
	if (IS_AFFECTED (ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT (obj, ITEM_MAGIC))
		strcat (buf, "#e(#BM#bagical#e)#n ");
	if (IS_OBJ_STAT (obj, ITEM_GLOW))
		strcat (buf, "#r(#RBurning#r)#n ");
	if (IS_OBJ_STAT (obj, ITEM_HUM))
		strcat (buf, "#e(#CH#cum#e)#n ");
	if (IS_OBJ_STAT (obj, ITEM_SHADOWPLANE) && obj->in_room != NULL && !IS_AFFECTED (ch, AFF_SHADOWPLANE))
		strcat (buf, "#e(#wU#bmbra#e)#n ");
	if (!IS_OBJ_STAT (obj, ITEM_SHADOWPLANE) && obj->in_room != NULL && IS_AFFECTED (ch, AFF_SHADOWPLANE))
		strcat (buf, "#e(#wN#normal #wp#nlane#e)#n ");
	if (!IS_NPC (ch) && IS_VAMPIRE (ch) && IS_OBJ_STAT (obj, ITEM_VANISH) && get_disc (ch, DISC_CHIMERSTRY) > 0)
		strcat (buf, "#e(#CI#cllusion#e)#n ");

	if (fShort)
	{
		if (obj->short_descr != NULL)
			strcat (buf, obj->short_descr);
		if (obj->condition < 99)
			strcat (buf, " #r(#ys#rc#yr#ra#yt#rc#yh#re#yd#r)#n");
		else if (obj->condition < 60)
			strcat (buf, " #r(#yb#re#yb#rt#r)#n");
		else if (obj->condition < 30)
			strcat (buf, " #r(#yd#ra#ym#ra#yg#re#yd#r)#n");

	}
	else
		strcat (buf, obj->description);		

	return buf;
}


int get_path2 (CHAR_DATA * ch, int disc)
{
	int max = 5;
	if (disc < PATH_FLAME || disc > PATH_MAX_ALLOWED)
	{
		bug ("Get_path: Out of path range.", 0);
		return 0;
	}
	if (IS_NPC (ch))
		return 0;
	if (ch->pcdata->thaum[disc] > max)
		return max;
	else if (ch->pcdata->thaum[disc] < 0)
		return 0;
	return ch->pcdata->thaum[disc];
}


int get_celerity (CHAR_DATA * ch)
{
	if (IS_NPC (ch))
		return 0;

	/* Old werewolves are fast, but Abominations have to burn Celerity */
	if (ch->class == CLASS_WEREWOLF && ch->pcdata->rank > get_disc (ch, DISC_CELERITY))
		return ch->pcdata->rank;

	if (ch->pcdata->celerity > 0 || (!IS_VAMPIRE (ch) && !IS_GHOUL (ch)))
		return get_disc (ch, DISC_CELERITY);

	return 0;
}


int get_disc (CHAR_DATA * ch, int disc)
{
	int max = 10;
	int total_disc;
	int min_disc = 0;
	if (disc < DISC_ANIMALISM || disc > DISC_MAX)
	{
		bug ("Get_disc: Out of discipline range.", 0);
		return 0;
	}
	if (IS_NPC (ch))
		return 0;
	if (ch->pcdata->powers_set[disc] > 0)
		min_disc = ch->pcdata->powers_set[disc];

	if (ch->class != CLASS_VAMPIRE)
		max = 10;
	if (disc == DISC_THAUMATURGY)
		max = 5;

	if (min_disc > max)
		min_disc = max;

	if (ch->pcdata->powers_set[disc] > min_disc)
		min_disc = ch->pcdata->powers_set[disc];

	if (ch->pcdata->powers_mod[disc] < 1)
	{
		if (ch->pcdata->powers[disc] > min_disc)
		{
			if (ch->pcdata->powers[disc] > max)
				return max;
			else
				return ch->pcdata->powers[disc];
		}
		else
			return min_disc;
	}

	if (ch->pcdata->powers[disc] < 1)
		total_disc = 0;
	else
		total_disc = ch->pcdata->powers[disc];
	if (ch->pcdata->powers_mod[disc] > 0)
		total_disc += ch->pcdata->powers_mod[disc];
	if (total_disc > max)
		total_disc = max;
	if (total_disc < min_disc)
		return min_disc;
	return total_disc;
}

int get_truedisc (CHAR_DATA * ch, int disc)
{
	int total_disc;
	int min_disc = 0;
	if (disc < DISC_ANIMALISM || disc > DISC_MAX)
	{
		bug ("Get_truedisc: Out of discipline range.", 0);
		return 0;
	}
	if (IS_NPC (ch))
		return 0;
	if (ch->pcdata->powers_set[disc] > 0)
		min_disc = ch->pcdata->powers_set[disc];

	if (ch->pcdata->powers_mod[disc] < 1)
	{
		if (ch->pcdata->powers[disc] > min_disc)
			return ch->pcdata->powers[disc];
		else
			return min_disc;
	}

	if (ch->pcdata->powers[disc] < 1)
		total_disc = 0;
	else
		total_disc = ch->pcdata->powers[disc];
	if (ch->pcdata->powers_mod[disc] > 0)
		total_disc += ch->pcdata->powers_mod[disc];
	if (total_disc < min_disc)
		return min_disc;
	return total_disc;
}

int get_breed (CHAR_DATA * ch, int disc)
{
	int max;
	if (IS_WEREWOLF (ch))
	{
		max = 40;
		if (disc < BREED_HOMID || disc > BREED_METIS)
		{
			bug ("Get_breed: Out of breed range.", 0);
			return 0;
		}
	}

	if (IS_NPC (ch))
		return 0;
	if ((IS_SKINDANCER (ch)) && ch->pcdata->cland[0] != disc)
		max = 3;
	if (ch->pcdata->breed[disc] > max)
		return max;
	else if (ch->pcdata->breed[disc] < 0)
		return 0;
	return ch->pcdata->breed[disc];
}

int get_auspice (CHAR_DATA * ch, int disc)
{
	int max = 40;
	if (disc < AUSPICE_RAGABASH || disc > AUSPICE_AHROUN)
	{
		bug ("Get_auspice: Out of auspice range.", 0);
		return 0;
	}
	if (IS_NPC (ch))
		return 0;
	if ((IS_SKINDANCER (ch)) && ch->pcdata->cland[1] != disc)
		max = 3;
	if (ch->pcdata->auspice[disc] > max)
		return max;
	else if (ch->pcdata->auspice[disc] < 0)
		return 0;
	return ch->pcdata->auspice[disc];
}

int get_tribe (CHAR_DATA * ch, int disc)
{
	int max;
	if (IS_WEREWOLF (ch))
	{
		max = 40;
		if (disc < TRIBE_BLACK_FURIES || disc > TRIBE_WENDIGOS)
		{
			bug ("Get_tribe: Out of tribe range.", 0);
			return 0;
		}
	}

	if (IS_NPC (ch))
		return 0;
	if (IS_SKINDANCER (ch) && ch->pcdata->cland[2] != disc)
		max = 3;
	if (ch->pcdata->tribes[disc] > max)
		return max;
	else if (ch->pcdata->tribes[disc] < 0)
		return 0;
	return ch->pcdata->tribes[disc];
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char (OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
	char buf[MAX_STRING_LENGTH];
	char **prgpstrShow;
	int *prgnShow;
	char *pstrShow;
	OBJ_DATA *obj;
	int nShow;
	int iShow;
	int count;
	bool fCombine;

	if (ch->desc == NULL)
		return;

	/*
	 * Alloc space for output lines.
	 */
	count = 0;
	for (obj = list; obj != NULL; obj = obj->next_content)
		count++;
	if (count > 8000)
	{
		send_to_char ("There are too many items to display.\n\r", ch);
		return;
	}

	prgpstrShow = alloc_mem (count * sizeof (char *));
	prgnShow = alloc_mem (count * sizeof (int));
	nShow = 0;

	/*
	 * Format the list of objects.
	 */
	for (obj = list; obj != NULL; obj = obj->next_content)
	{
		if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
			continue;
		if (obj->description != NULL && strlen (obj->description) < 2)
			continue;
		if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
		{
			pstrShow = format_obj_to_char (obj, ch, fShort);
			fCombine = FALSE;

			if (IS_NPC (ch) || IS_SET (ch->act, PLR_COMBINE))
			{
				/*
				 * Look for duplicates, case sensitive.
				 * Matches tend to be near end so run loop backwords.
				 */
				for (iShow = nShow - 1; iShow >= 0; iShow--)
				{
					if (!strcmp (prgpstrShow[iShow], pstrShow))
					{
						prgnShow[iShow]++;
						fCombine = TRUE;
						break;
					}
				}
			}

			/*
			 * Couldn't combine, or didn't want to.
			 */
			if (!fCombine)
			{
				prgpstrShow[nShow] = str_dup (pstrShow);
				prgnShow[nShow] = 1;
				nShow++;
			}
		}
	}

	/*
	 * Output the formatted list.
	 */

	for (iShow = 0; iShow < nShow; iShow++)
	{
		if (prgpstrShow[iShow][0] == '\0')
		{
			free_string (prgpstrShow[iShow]);
			continue;
		}
		if (IS_NPC (ch) || IS_SET (ch->act, PLR_COMBINE))
		{
			if (prgnShow[iShow] != 1)
			{
				sprintf (buf, "(%2d) ", prgnShow[iShow]);
				send_to_char (buf, ch);
			}
			else
			{
				send_to_char ("     ", ch);
			}
		}
		send_to_char (prgpstrShow[iShow], ch);
		send_to_char ("\n\r", ch);
		free_string (prgpstrShow[iShow]);
	}

	if (fShowNothing && nShow == 0)
	{
		if (IS_NPC (ch) || IS_SET (ch->act, PLR_COMBINE))
			send_to_char ("     ", ch);
		send_to_char ("Nothing.\n\r", ch);
	}

	/*
	 * Clean up.
	 */
	free_mem (prgpstrShow, count * sizeof (char *));
	free_mem (prgnShow, count * sizeof (int));

	return;
}



void show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char buf4[MAX_STRING_LENGTH];
	char mount2[MAX_STRING_LENGTH];
	char specialstring[MAX_STRING_LENGTH];
	char sectstat[MAX_STRING_LENGTH];
	char *vname;
	char *ename;
	int char_success;
	int shownstatus;
	CHAR_DATA *mount;
	MEMORY_DATA * remembered_char = NULL;



	buf[0] = '\0';
	buf2[0] = '\0';
	buf3[0] = '\0';
	buf4[0] = '\0';

	if (!IS_NPC (victim) && victim->pcdata->chobj != NULL)
		return;

	if ((mount = victim->mount) != NULL && IS_SET (victim->mounted, IS_MOUNT))
		return;

	/* Begin Obfus Code */
	if (IS_MORE3(victim, MORE3_OBFUS1) && (victim->pcdata->powers[DISC_OBFUSCATE] > ch->pcdata->powers[DISC_AUSPEX]) && !IS_IMMORTAL (ch) && IS_MORE2 (victim, MORE2_RPFLAG))
		return;
	
	if (IS_MORE3(victim, MORE3_OBFUS2) && (victim->pcdata->powers[DISC_OBFUSCATE] > ch->pcdata->powers[DISC_AUSPEX]) && !IS_IMMORTAL (ch) && IS_MORE2 (victim, MORE2_RPFLAG))
		return;
	
	if (IS_IMMORTAL(ch))
	{
		strcat(buf, "#o(#c");
		strcat(buf, victim->name);
		strcat(buf, "#o)#n");	
	}
	/* end obfus code */
	
	if (!IS_NPC (victim) && victim->desc == NULL)
		strcat (buf, "#y(LD)#n ");
	if (IS_SET (victim->extra2, EXTRA2_AFK))
                     strcat (buf, "#e(#CAFK#e)#n ");
	if (IS_MORE (victim, MORE_SPIRIT))
		strcat (buf, "#g(Sprt)#n ");
	if ((!IS_MORE2 (victim, MORE2_RPFLAG)) && (!IS_SET (victim->extra2, EXTRA2_AFK)))
			strcat (buf, "#e<#ROOC#e>#n#n ");
	if ((IS_MORE2 (victim, MORE2_RPFLAG)) && (IS_MORE3(victim, MORE3_OBFUS2)))
		strcat (buf, "#e(#cObfus#e)#n ");
	if ((IS_MORE2 (victim, MORE2_HEIGHTENSENSES)) && IS_STORYTELLER(ch))
		strcat (buf, "#b(#cHeightened Senses#b)#n ");
	if (IS_MORE3(victim, MORE3_OBFUS1))
		strcat (buf, "#e(#cObfus#e)#n ");
	if ((IS_SET (victim->act, PLR_INCOG)) && (IS_IMMORTAL (victim)))
		strcat (buf, "(Incognito) ");
	if (IS_SET (victim->act, PLR_WIZINVIS) && IS_IMMORTAL (victim))
		strcat (buf, "(WizInvis) ");

	
	if ((ch->abilities[KNOWLEDGES][KNO_POLITICS] > 0) && (ch->side[0] == victim->side[0]) && (strlen (victim->side) > 3) && (IS_MORE2 (victim, MORE2_RPFLAG)) && (victim->pcdata->sectstatus > 0)){
		
		char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_POLITICS, 6);
		
		if (char_success < 0){
		shownstatus = rand () % 1500 + 1;
		sprintf(sectstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
		strcat(buf, sectstat);	
		}
		
		if (char_success == 0){

		}
		
		if (char_success == 1){
		shownstatus = rand () % 300 + victim->pcdata->sectstatus - 150;
			
			if (shownstatus < 0){
					shownstatus = 0;
				}
			sprintf(sectstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, sectstat);
		}	
		
		if (char_success == 2){
		
		shownstatus = rand () % 150 + victim->pcdata->sectstatus - 75;
	
			if (shownstatus < 0){
					shownstatus = 0;
				}
			sprintf(sectstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, sectstat);
		
		}
		
		if (char_success == 3){
			
			

			shownstatus = rand () % 80 + victim->pcdata->sectstatus - 40;

			if (shownstatus < 0){
					shownstatus = 0;
				}		
			sprintf(sectstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, sectstat);
		}
		
		
		if (char_success == 4){
		
		shownstatus = rand () % 20 + victim->pcdata->sectstatus - 10;
		
	
		if (shownstatus < 0){
					shownstatus = 0;
				}
			sprintf(sectstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, sectstat);	
		}
		
		
		if (char_success > 4){
			sprintf(sectstat, "#G(#eStatus#G:#e%d#G)#n ", victim->pcdata->sectstatus); 
			strcat(buf, sectstat);
		}

	}
	
	
	
	
//		if ((victim->pcdata->roomdesc != NULL) && (strlen (victim->pcdata->roomdesc) > 3) && (IS_MORE2 (victim, MORE2_RPFLAG)) && (!IS_AFFECTED (victim, AFF_POLYMORPH)))
//		strcat (buf, "#B(#e");
	
	
	
	if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_AFFECTED (victim, AFF_SHADOWPLANE))
		strcat (buf, "#r(Normal plane)#n ");
	else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_AFFECTED (victim, AFF_SHADOWPLANE))
		strcat (buf, "#e(Umbra)#n ");
			
	


	if (((victim->pcdata->wolfform[1] == FORM_GLABRO)) || ((victim->pcdata->wolfform[1] == FORM_CRINOS)) || ((victim->pcdata->wolfform[1] == FORM_HISPO)) || ((victim->pcdata->wolfform[1] == FORM_LUPUS)) || (IS_AFFECTED (victim, AFF_POLYMORPH)))
		{
			/* Remember Code */
			remembered_char = memory_search_real_name(ch->memory, victim->name);
			if (remembered_char != NULL)
			{
				strcat (buf, "#B(#e");
				strcat (buf, remembered_char->remembered_name);
				strcat (buf, "#B)#n ");
			}
			/* End Remember Code */
		}

	if (!IS_NPC (ch) && (IS_VAMPIRE (ch) || IS_GHOUL (ch))
		 && get_disc (ch, DISC_ABOMBWE) > 0)
	{
		switch (victim->class)
		{
		default:
			break;
		case CLASS_VAMPIRE:
			strcat (buf, "(#RVamp#n) ");
			break;
		case CLASS_WEREWOLF:
			strcat (buf, "(#RWW#n) ");
			break;
		}
	}

	
	if (victim->affected_by == AFF_POLYMORPH)
	{
		strcat (buf, "");
	}	
	
	else if (IS_WEREWOLF (victim) && IS_SET (victim->act, PLR_WOLFMAN) && !IS_POLYAFF (victim, POLY_ZULO) && IS_WEREWOLF (ch))
	{
		/* Remember Code */
		remembered_char = memory_search_real_name(ch->memory, victim->name);
		if (remembered_char != NULL)
		{
			strcat (buf, "#B(#e");
			strcat (buf, remembered_char->remembered_name);
			strcat (buf, "#B)#n ");
		}
		/* End Remember Code */
	}
	else if (!IS_NPC (ch) && !IS_NPC (victim) && IS_VAMPIRE (victim) && IS_VAMPAFF (victim, VAM_DISGUISED) && !IS_EXTRA (victim, EXTRA_VICISSITUDE))
	{
		if ((IS_VAMPIRE (ch) && get_disc (ch, DISC_AUSPEX) > get_disc (victim, DISC_CHIMERSTRY)) || (IS_WEREWOLF (ch) && get_auspice (ch, AUSPICE_PHILODOX) > 1))
		{
			if (!IS_EXTRA (victim, EXTRA_VICISSITUDE) && !IS_SET (victim->act, PLR_WOLFMAN))
			{
				/* Remember Code */
				remembered_char = memory_search_real_name(ch->memory, victim->name);
				if (remembered_char != NULL)
				{
					//strcat (buf, "#B(#e");
					//strcat (buf, remembered_char->remembered_name);
					//strcat (buf, "#B)#n ");
				}
				/* End Remember Code */
			}
		}
	}

	if (IS_AFFECTED (victim, AFF_WEBBED))
	{
		if (IS_NPC (victim))
			sprintf (buf4, "\n\r...%s is coated in a sticky web.", victim->short_descr);
		else if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH))
			sprintf (buf4, "\n\r...%s is coated in a sticky web.", victim->morph);
		else
			sprintf (buf4, "\n\r...%s is coated in a sticky web.", victim->pcdata->roomdesc);
		strcat (buf3, buf4);
	}

	if (IS_AFFECTED (victim, AFF_SHADOW))
	{
		if (IS_NPC (victim))
			sprintf (buf4, "\n\r...%s is entrapped by tendrils of darkness.", victim->short_descr);
		else if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH))
			sprintf (buf4, "\n\r...%s is entrapped by tendrils of darkness.", victim->morph);
		else
			sprintf (buf4, "\n\r...%s is entrapped by tendrils of darkness.", victim->pcdata->roomdesc);
		strcat (buf3, buf4);
	}

	if (IS_AFFECTED (victim, AFF_JAIL))
	{
		if (IS_NPC (victim))
			sprintf (buf4, "\n\r...%s is surrounded with bands of water.", victim->short_descr);
		else if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH))
			sprintf (buf4, "\n\r...%s is surrounded with bands of water.", victim->morph);
		else
			sprintf (buf4, "\n\r...%s is surrounded with bands of water.", victim->pcdata->roomdesc);
		strcat (buf3, buf4);
	}

	if (victim->embraced == ARE_EMBRACING && victim->embrace != NULL)
	{
		CHAR_DATA *emb = victim->embrace;
		if (IS_NPC (victim))
			vname = str_dup (victim->short_descr);
		else if (IS_AFFECTED (victim, AFF_POLYMORPH))
			vname = str_dup (victim->morph);
		else
			vname = str_dup (victim->pcdata->roomdesc);

		if (IS_NPC (emb))
			ename = str_dup (emb->short_descr);
		else if (IS_AFFECTED (emb, AFF_POLYMORPH))
			ename = str_dup (emb->morph);
		else
			ename = str_dup (emb->pcdata->roomdesc);

		if (ch == victim)
			sprintf (buf4, "\n\r...You are holding %s in a close embrace.", ename);
		else if (ch == emb)
			sprintf (buf4, "\n\r...%s is holding you in a close embrace.", vname);
		else
			sprintf (buf4, "\n\r...%s is holding %s in a close embrace.", vname, ename);
		strcat (buf3, buf4);
	}
	else if (victim->embraced == ARE_EMBRACED && victim->embrace != NULL)
	{
		CHAR_DATA *emb = victim->embrace;
		if (IS_NPC (victim))
			vname = str_dup (victim->short_descr);
		else if (IS_AFFECTED (victim, AFF_POLYMORPH))
			vname = str_dup (victim->morph);
		else
			vname = str_dup (victim->pcdata->roomdesc);

		if (IS_NPC (emb))
			ename = str_dup (emb->short_descr);
		else if (IS_AFFECTED (emb, AFF_POLYMORPH))
			ename = str_dup (emb->morph);
		else
			ename = str_dup (emb->pcdata->roomdesc);

		if (ch == victim)
			sprintf (buf4, "\n\r...You are being held by %s in a close embrace.", ename);
		else if (ch == emb)
			sprintf (buf4, "\n\r...%s is being held by you in a close embrace.", vname);
		else
			sprintf (buf4, "\n\r...%s is being held by %s in a close embrace.", vname, ename);
		strcat (buf3, buf4);
	}

	if (victim->position == POS_STANDING && victim->long_descr[0] != '\0' && strlen (victim->long_descr) > 3 && (mount = victim->mount) == NULL && victim->stance[0] < 1)
	{
		strcpy (buf4, victim->long_descr);
		buf4[strlen (buf4) - 2] = '\0';
		strcat (buf, "#c");
		strcat (buf, buf4);
		strcat (buf, buf2);
		strcat (buf, buf3);

		strcat (buf, "\n\r");
		buf[0] = UPPER (buf[0]);
		send_to_char (buf, ch);

		if (!IS_NPC (ch) && IS_SET (ch->act, PLR_BRIEF))
			return;

		if (IS_NPC (ch) || !IS_SET (ch->act, PLR_BRIEF))
		{
			if (IS_ITEMAFF (victim, ITEMA_SHOCKSHIELD))
				act ("...$N is surrounded by a crackling shield of lightning.", ch, NULL, victim, TO_CHAR);
			if (IS_ITEMAFF (victim, ITEMA_FIRESHIELD))
				act ("...$N is surrounded by a burning shield of fire.", ch, NULL, victim, TO_CHAR);
			if (IS_ITEMAFF (victim, ITEMA_ICESHIELD))
				act ("...$N is surrounded by a shimmering shield of ice.", ch, NULL, victim, TO_CHAR);
			if (IS_ITEMAFF (victim, ITEMA_ACIDSHIELD))
				act ("...$N is surrounded by a bubbling shield of acid.", ch, NULL, victim, TO_CHAR);
			if (IS_ITEMAFF (victim, ITEMA_CHAOSSHIELD))
				act ("...$N is surrounded by a swirling shield of chaos.", ch, NULL, victim, TO_CHAR);
			if (IS_ITEMAFF (victim, ITEMA_REFLECT))
				act ("...$N is surrounded by a flickering shield of darkness.", ch, NULL, victim, TO_CHAR);
			if (IS_ITEMAFF (victim, ITEMA_SUNSHIELD))
				act ("...$N is surrounded by a glowing halo of sunlight.", ch, NULL, victim, TO_CHAR);
		}

		return;
	}
	else if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH))
		strcat (buf, victim->morph);
	else
	{
		/* Remember Code */
		remembered_char = memory_search_real_name(ch->memory, victim->name);
		if (remembered_char != NULL)
		{
			strcat (buf, "#B(#e");
			strcat (buf, remembered_char->remembered_name);
			strcat (buf, "#B)#n ");
		}
		/* End Remember Code */
		strcat (buf, victim->pcdata->roomdesc);
	}

	if ((mount = victim->mount) != NULL && victim->mounted == IS_RIDING)
	{
		if (IS_NPC (mount))
			sprintf (mount2, " is here riding %s", mount->short_descr);
		else
			sprintf (mount2, " is here riding %s", mount->name);
		strcat (buf, mount2);
		if (victim->position == POS_FIGHTING)
		{
			strcat (buf, ", fighting ");
			if (victim->fighting == NULL)
				strcat (buf, "thin air??");
			else if (victim->fighting == ch)
				strcat (buf, "YOU!");
			else if (victim->in_room == victim->fighting->in_room)
			{
				strcat (buf, PERS (victim->fighting, ch));
				strcat (buf, ".");
			}
			else
				strcat (buf, "somone who left??");
		}
		else
			strcat (buf, ".");
	}
	else if (victim->position == POS_STANDING && IS_AFFECTED (victim, AFF_FLYING))
		strcat (buf, " is hovering here.");
	else if (victim->position == POS_STANDING && (!IS_NPC (victim) && (IS_VAMPAFF (victim, VAM_FLYING))))
		strcat (buf, " is hovering here.");
	else
	{
		switch (victim->position)
		{
		case POS_DEAD:
			strcat (buf, " is DEAD!!");
			break;
		case POS_MORTAL:
			strcat (buf, " is lying here mortally wounded.");
			break;
		case POS_INCAP:
			strcat (buf, " is lying here incapacitated.");
			break;
		case POS_STUNNED:
			strcat (buf, " is lying here stunned.");
			break;
		case POS_SLEEPING:
			if (victim->sat != NULL)
			{
				sprintf (buf4, " is sleeping on %s here.", victim->sat->short_descr);
				strcat (buf, buf4);
			}
			else
				strcat (buf, " is sleeping here.");
			break;
		case POS_RESTING:
			if (victim->sat != NULL)
			{
				sprintf (buf4, " is resting on %s here.", victim->sat->short_descr);
				strcat (buf, buf4);
			}
			else
				strcat (buf, " is resting here.");
			break;
		case POS_MEDITATING:
			if (victim->sat != NULL)
			{
				sprintf (buf4, " is meditating on %s here.", victim->sat->short_descr);
				strcat (buf, buf4);
			}
			else
				strcat (buf, " is meditating here.");
			break;

		case POS_SITTING:
			if (victim->sat != NULL)
			{
				sprintf (buf4, " is sitting on %s here.", victim->sat->short_descr);
				strcat (buf, buf4);
			}
			else
				strcat (buf, " is sitting here.");
			break;

		case POS_STANDING:
			
                        if (!IS_NPC (victim) && (!IS_MORE2 (victim, MORE2_RPFLAG)))
			{
				strcat (buf, " is here oocly.");
			}

			else if (!IS_NPC (victim) && (victim->pcdata->doing != NULL))
			{
				if ((victim->pcdata->roomdesc != NULL) && (strlen (victim->pcdata->roomdesc) > 3) && (!IS_AFFECTED (victim, AFF_POLYMORPH))){
					//	strcat (buf, "#B)#n ");
				//		strcat (buf, victim->pcdata->roomdesc);
					}
					else {
						strcat (buf, "");
					} 
				strcat (buf, " ");
				strcat (buf, victim->pcdata->doing);
				strcat (buf, " ");
				if (victim->pcdata->aura != NULL){
					strcat (buf, " ");
					strcat (buf, victim->pcdata->aura);
				}
				else {
					strcat (buf, "");
				} 
			}
			else
			{
				if ((victim->in_room->room_flags == ROOM_UNDER_WATER) && !IS_POLYAFF (victim, POLY_FISH) && !IS_IMMORTAL (victim) && !IS_NPC (victim))
					strcat (buf, " is drowning here.");
				else if ((victim->in_room->room_flags == ROOM_UNDER_WATER) && (IS_POLYAFF (victim, POLY_FISH) || IS_IMMORTAL (victim)))
					strcat (buf, " is here swimming underwater.");
				else
				{
					if ((victim->pcdata->roomdesc != NULL) && (strlen (victim->pcdata->roomdesc) > 3) && (!IS_AFFECTED (victim, AFF_POLYMORPH))){
						//strcat (buf, "#B)#n ");
						//strcat (buf, victim->pcdata->roomdesc);
					}
					else {
						strcat (buf, "");
					} 
						
					strcat (buf, " is here.");
					strcat (buf, " ");
					if (victim->pcdata->aura != NULL){
						strcat (buf, " ");
						strcat (buf, victim->pcdata->aura);
					}
					else {
						strcat (buf, "");
					} 
				}
			}
			break;
		case POS_FIGHTING:
			strcat (buf, " is here, fighting ");
			if (victim->fighting == NULL)
				strcat (buf, "thin air??");
			else if (victim->fighting == ch)
				strcat (buf, "YOU!");
			else if (victim->in_room == victim->fighting->in_room)
			{
				strcat (buf, PERS (victim->fighting, ch));
				strcat (buf, ".");
			}
			else
				strcat (buf, "somone who left??");
			break;
		}
	}

	if (!IS_NPC(victim) && victim->pcdata->show_social == TRUE && IS_VAMPIRE (victim) && !str_cmp (victim->clan, "Nosferatu"))
		sprintf(specialstring, " #b(#BApp#b:#B0#b)#n");
	else if (!IS_NPC(victim) && victim->pcdata->show_social == TRUE)
		sprintf(specialstring, " #b(#BApp#b:#B%d#b)#n", 
	get_curr_app(victim));


	if (buf2[0] != '\0') {
		if (!IS_NPC(victim) && victim->pcdata->show_social == TRUE)
			strcat (buf, specialstring);
		strcat (buf, "\n\r");
	}
	strcat (buf, buf2);
	strcat (buf, buf3);
	if (!IS_NPC(victim) && victim->pcdata->show_social == TRUE)
		strcat(buf, specialstring);
	strcat (buf, "\n\r");
	buf[0] = UPPER (buf[0]);
	send_to_char (buf, ch);

	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_BRIEF))
		return;

	if (IS_ITEMAFF (victim, ITEMA_SHOCKSHIELD))
		act ("...$N is surrounded by a crackling shield of lightning.", ch, NULL, victim, TO_CHAR);
	if (IS_ITEMAFF (victim, ITEMA_FIRESHIELD))
		act ("...$N is surrounded by a burning shield of fire.", ch, NULL, victim, TO_CHAR);
	if (IS_ITEMAFF (victim, ITEMA_ICESHIELD))
		act ("...$N is surrounded by a shimmering shield of ice.", ch, NULL, victim, TO_CHAR);
	if (IS_ITEMAFF (victim, ITEMA_ACIDSHIELD))
		act ("...$N is surrounded by a bubbling shield of acid.", ch, NULL, victim, TO_CHAR);
	if (IS_ITEMAFF (victim, ITEMA_CHAOSSHIELD))
		act ("...$N is surrounded by a swirling shield of chaos.", ch, NULL, victim, TO_CHAR);
	if (IS_ITEMAFF (victim, ITEMA_REFLECT))
		act ("...$N is surrounded by a flickering shield of darkness.", ch, NULL, victim, TO_CHAR);
	if (IS_ITEMAFF (victim, ITEMA_SUNSHIELD))
		act ("...$N is surrounded by a glowing halo of sunlight.", ch, NULL, victim, TO_CHAR);
	return;
}


void show_char_to_char_1 (CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	int iWear;
	int percent;
	bool found;



	if ((((!IS_MORE3(ch, MORE3_OBFUS1)) && (!IS_MORE3(ch, MORE3_OBFUS2))) && ((!IS_MORE3(victim, MORE3_OBFUS1)) && (!IS_MORE3(victim, MORE3_OBFUS2)))) && (can_see (victim, ch)))
	
	{
		
		act ("$n looks at you.", ch, NULL, victim, TO_VICT);
		act ("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
	}

	if (!IS_NPC(victim) && victim->pcdata->showtempdesc == TRUE)
	{
		send_to_char(victim->pcdata->tempdesc, ch);
		send_to_char("\n\r", ch);
	}

	else if (!IS_NPC (victim) && IS_SHIFTER (victim) && victim->pcdata->wolfform[1] != FORM_HOMID)
	{
		if (victim->pcdata->lupusdesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_LUPUS)
		{
			send_to_char (victim->pcdata->lupusdesc, ch);
			send_to_char ("\n\r", ch);
		}
		else if (victim->pcdata->crinosdesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_CRINOS)
		{
			send_to_char (victim->pcdata->crinosdesc, ch);
			send_to_char ("\n\r", ch);
		}
		else if (victim->pcdata->hispodesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_HISPO)
		{
			send_to_char (victim->pcdata->hispodesc, ch);
			send_to_char ("\n\r", ch);
		}
		else if (victim->pcdata->glabrodesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_GLABRO)
		{
			send_to_char (victim->pcdata->glabrodesc, ch);
			send_to_char ("\n\r", ch);
		}
		else
			act ("You see nothing special about $M ", ch, NULL, victim, TO_CHAR);
	}


	else
	{
		if (victim->description[0] != '\0')
		{
			send_to_char (victim->description, ch);
			send_to_char ("\n\r", ch);
		}
		else
		{
			act ("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
		}
	}
	if (victim->max_hit > 0)
		percent = (100 * victim->hit) / victim->max_hit;
	else
		percent = -1;

	strcpy (buf, PERS (victim, ch));

	if (percent >= 100)
		strcat (buf, " is in perfect health.\n\r");
	else if (percent >= 90)
		strcat (buf, " is slightly scratched.\n\r");
	else if (percent >= 80)
		strcat (buf, " has a few bruises.\n\r");
	else if (percent >= 70)
		strcat (buf, " has some cuts.\n\r");
	else if (percent >= 60)
		strcat (buf, " has several wounds.\n\r");
	else if (percent >= 50)
		strcat (buf, " has many nasty wounds.\n\r");
	else if (percent >= 40)
		strcat (buf, " is bleeding freely.\n\r");
	else if (percent >= 30)
		strcat (buf, " is covered in blood.\n\r");
	else if (percent >= 20)
		strcat (buf, " is leaking guts.\n\r");
	else if (percent >= 10)
		strcat (buf, " is almost dead.\n\r");
	else
		strcat (buf, " is DYING.\n\r");

	buf[0] = UPPER (buf[0]);
	send_to_char (buf, ch);

	found = FALSE;
	//new
	if (!IS_NPC (ch) && !IS_NPC (victim))
	{
		send_to_char ("\n\r", ch);
		show_equipment (victim, ch);
	}
	else
	{
		for (iWear = 0; iWear < MAX_WEAR; iWear++)
		{
			if ((obj = get_eq_char (victim, iWear)) != NULL && can_see_obj (ch, obj))
			{
				if (!found)
				{
					send_to_char ("\n\r", ch);
					act ("$N is using:", ch, NULL, victim, TO_CHAR);
					found = TRUE;
				}
				send_to_char (where_name[iWear], ch);
				if (IS_NPC (ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj != obj)
				{
					send_to_char (format_obj_to_char (obj, ch, TRUE), ch);
					send_to_char ("\n\r", ch);
				}
				else
					send_to_char ("you\n\r", ch);
			}
		}
	}

	if (!IS_NPC (ch) && (IS_IMMORTAL (ch) || IS_STORYTELLER (ch) || IS_SHOPKEEPER (ch)) && victim != ch)
	{
		send_to_char ("\n\rYou peek at the inventory:\n\r", ch);
		show_list_to_char (victim->carrying, ch, TRUE, TRUE);
	}

	return;
}


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
	 {
	if ( rch == ch )
		 continue;

	if ( IS_IMMORTAL(ch) || IS_IMMORTAL(rch) )
	{
		 if ( !can_see( ch, rch ) ) continue;

	}
	else
	{
		 if ( !IS_NPC(rch)
		 && ( IS_SET(rch->act, PLR_WIZINVIS)
		 &&   !IS_SET(ch->act, PLR_HOLYLIGHT)))
		continue;
	}

	if ( !IS_NPC(rch) )
	{
		 if (IS_EXTRA(rch,EXTRA_OSWITCH)) continue;
	}

	if ( can_see( ch, rch ) )
	    show_char_to_char_0( rch, ch );
	else if ( room_is_dark( ch->in_room ) && !IS_VAMPAFF( rch, VAM_EARTHMELDED)
	&&      ( IS_AFFECTED(rch, AFF_INFRARED) || IS_VAMPAFF(rch, VAM_NIGHTSIGHT)) )
	{
		send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
	}
    }

    return;
} 


bool check_blind (CHAR_DATA * ch)
{
	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
		return TRUE;

	if (IS_ITEMAFF (ch, ITEMA_VISION))
		return TRUE;

	if (IS_AFFECTED (ch, AFF_BLIND) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT))
	{
		send_to_char ("You can't see a thing!\n\r", ch);
		return FALSE;
	}

	return TRUE;
}



void do_look (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	EXIT_DATA *pexit;
	CHAR_DATA *victim;
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *wizard;
	OBJ_DATA *obj;
	OBJ_DATA *portal;
	OBJ_DATA *portal_next;
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	char *pdesc;
	int door;
	bool found;
	bool found_char;

	if (ch->desc == NULL && (wizard = ch->wizard) == NULL)
		return;

	if (ch->in_room == NULL)
		return;

	if (ch->position == POS_SLEEPING)
	{
		send_to_char ("You can't see anything, you're sleeping!\n\r", ch);
		return;
	}

	if (!check_blind (ch))
		return;

	if (!IS_NPC (ch) && ch->in_room != NULL && IS_SET (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS) && !IS_ITEMAFF (ch, ITEMA_VISION) && !IS_IMMORTAL (ch))
	{
		if (!IS_VAMPIRE (ch) || !IS_MORE2 (ch, MORE2_NIGHTEYES))
		{
			if (!IS_WEREWOLF (ch) || get_tribe (ch, TRIBE_UKTENA) < 1)
			{
				if (IS_NPC (ch))
				{
					send_to_char ("It is pitch black ... \n\r", ch);
					return;
				}
			}
		}
	}

	if (!IS_NPC (ch) && !IS_SET (ch->act, PLR_HOLYLIGHT) && !IS_ITEMAFF (ch, ITEMA_VISION) && !IS_VAMPAFF (ch, VAM_NIGHTSIGHT) && !IS_MORE2 (ch, MORE2_HEIGHTENSENSES) && !IS_MORE2 (ch, MORE2_NIGHTEYES) && !IS_AFFECTED (ch, AFF_SHADOWPLANE) && !(ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL) && room_is_dark (ch->in_room))
	{
		send_to_char ("It is pitch black ... \n\r", ch);
		show_char_to_char (ch->in_room->people, ch);
		return;
	}
	if (!room_is_dark (ch->in_room) && (IS_VAMPIRE (ch) || IS_GHOUL (ch)) && IS_MORE2 (ch, MORE2_NIGHTEYES))
	{
		send_to_char ("AHH! Its too bright in here to use Eyes of the Night \n\r", ch);
		return;
	}

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);


	if (arg1[0] == '\0' || !str_cmp (arg1, "auto"))
	{
		/* 'look' or 'look auto' */
		if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
			act ("$p", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
		else if (IS_AFFECTED (ch, AFF_SHADOWPLANE)) //will be removed
			send_to_char ("#BTh#be#B Um#Gb#br#Ba#n\n\r", ch);   //will be removed
		else
		{
			sprintf (buf, "%s\n\r", ch->in_room->name);

			/* shows immortals the room flags */
			if (IS_IMMORTAL (ch) && IS_SET (ch->act, PLR_ROOMFLAGS))
			{
				sprintf (buf2, " #C(#1vnum#n: %d) [#1Sector#n:%s #1Room Flags#n:%s]#n", ch->in_room->vnum, room_sector (ch->in_room->sector_type), room_bit_name (ch->in_room->room_flags));
				strcat (buf, buf2);
			}
			strcat (buf, "\r\n");
			send_to_char (buf, ch);
		}

		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_SMALLCAERN))
		{
			send_to_char ("#CThe room tingles with power.#n\n\r", ch);
		}

		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_LARGECAERN))

		{
			send_to_char ("#CThe room buzzes with elemental power.#n\n\r", ch);
		}
		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_DEFILEDCAERN))
		{
			send_to_char ("#CThe room smells of corruption#n\n\r", ch);
		}

		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_UNDER_WATER))
		{
			send_to_char ("#b\n\rYou are underwater swimming#n\n\r", ch);
		}

		if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
		{
			act ("You are inside $p.", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
			show_list_to_char (ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE);
		}
		else if ((arg1[0] == '\0' || !str_cmp (arg1, "auto")) && IS_AFFECTED (ch, AFF_SHADOWPLANE)){
		/* Hacked Umbra Code */	
			
			if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_SMALLCAERN)){
				send_to_char ("Great care has been taken to make sure no piece of foliage is out of place nor any unknown visitor allowed.  It seems to be a sort of thicket and is very green surrounding the caern itself.  A few spirits imbued in the caern itself dance about the area offering even the  surrounding grounds a peaceful serenity.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}

			else if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_LARGECAERN)){
				send_to_char ("This place is obviously holy in a sense and seems well guarded by both spirit and garou alike.  Like the area surrounding it, it is calm and its construction is precise and spares no attention to detail.  A ring is made in the ground and a few pebbles decorate that circle that holds any offereings made to the spirits overhead. It seems as if this is where the natural world begins and the spirit world ends.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			
			else if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_DEFILEDCAERN)){
				send_to_char ("Darkness seems ever previlant here, the feeling of the unnatural floats about as much as the minions of the wyrm mostly unseen by the naked eye.  It seems as if this room is everything a sane person would stay way from and beg for Gaia's strength to get by it.  Those who seek to cleanse this area would certainly have trouble doing so and probally end up in pain for their deeds.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) &&  (ch->in_room->sector_type == SECT_MOUNTAIN)){
				send_to_char ("Deep into the damp ground lies a passage way of connected caves and within the view is glorious with its high ceilings and deep pools of pure water.  Little seems to have been in here and it seems mostly peaceful only hosting a few less hostile spirits.\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_CITY)){
				send_to_char ("Garou seem not to be welcome as spirits dance back and forth giving them glares or even worse threatening their very presence.  All structures within this city are a pale grey, boring and looming and the air is chilled here.  There is no smell here only of the stale air within the walls of the city and weaver spirits seem to dance around its inner walls spreading their webs.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_FIELD)){
				send_to_char ("Looking around the horizon one can only see tall grasses with few trees about this field.  The grass is a bright green and looks healthy despite the unusual coloring of the sky and the spiritual energy here seems to pulse in vibrant colors as a few spirits obviously frenquent the place.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_FOREST)){
				send_to_char ("Huge tightly packed trees are about the area blocking almost all light from entering the forest.  Once entering, it seems cooler and darkness falls in its clutches.  It is harder to walk through and one must be wary judging their path for at every turn something may arise.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_HILLS)){
				send_to_char (" Rolling hills of short grasses and small shrubs line this part of the umbra.  It seems empty and more silent than it should and in the distance are seen the countryside of the humans beginning to taint this place.  The hills are large enough to provide protection for anyone needing it or a safe place to rest.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_INSIDE)){
				send_to_char ("Within these walls one can find small comfort in this city either hiding from the spirits glaring gaze or from anything else that seems to hunt those who come here.  It is relativly empty of any belongings and is made of simple wood sporting nothing of interest.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_DESERT)){
				send_to_char ("Little to nothing grows here except for the ever growing amount of sand on the ground that seems to multiply with each step.  Water is scarce and the sky above is an errie red-orange color.  Emptyness seems to prevail here and nothing more can be said of the bland landscape of the desert.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else {
				send_to_char ("Colors and spirits seem to swirl about in a dance not meant for most eyes to even dream about.  Stepping further into the umbra shall bring spirits of both friend and foe and challenge even the mighty of strength testing them all.\n\r\n\r", ch);
				do_exits (ch, "auto");		
			}
		}
		
		
		else if ((!IS_NPC (ch) && !IS_SET (ch->act, PLR_BRIEF)) && (arg1[0] == '\0' || !str_cmp (arg1, "auto")))
		{

			
			/*Modified Show Weather Code
		if (IS_OUTSIDE (ch)){
		if (weather_info.mmhg <= 965)
			strcat(buf, "Lightning flashes in the sky as the heavy rain forms puddles on the ground.");
			
		else if (weather_info.mmhg <= 980)
			strcat(buf, "You can hear a faint roar of thunder as the rain falls in a steady torrent.\n\r");
		else if (weather_info.mmhg <= 990)
			strcat(buf, "A light, but steady rain falls, forming into puddles on the ground.\n\r");
		else if (weather_info.mmhg <= 1000)
			strcat(buf, "The air is foggy and damp with drizzle.\n\r");
		else if (weather_info.mmhg <= 1020){
			strcat(buf, "The sky is shrouded in dark clouds. From time to time you can hear far \n\r");
			strcat(buf, "distant thunder.\n\r");
		}
		else if (weather_info.mmhg <= 1030)
			strcat(buf, "Only small patches of sky can be seen through the thick clouds.\n\r");
		else if (weather_info.mmhg <= 1040)
			strcat(buf, "The open sky is broken up by large, steel grey clouds.\n\r");
		else if (weather_info.mmhg <= 1050)
			strcat(buf, "The sky is clear, except for a few light, almost transparent clouds.\n\r");
		else
			strcat(buf, "The sky shows pristine and clear overhead.\n\r");
		}	
	else{
		if (weather_info.mmhg <= 965){
			strcat(buf, "Thunder crashes, and the sound of heavy rain can be heard pounding \n\r");
			strcat(buf, "overhead.\n\r");
		}
		else if (weather_info.mmhg <= 980)
			strcat(buf, "Every few moments, thunder cracks, and the drumming of rain is audible.\n\r");
		else if (weather_info.mmhg <= 990)
			strcat(buf, "You hear a light rain falling from outside.\n\r");
		else if (weather_info.mmhg <= 1000)
			strcat(buf, "\n\r");
		
		}	
			
			     */
			if (ch->plane == 0)
			sprintf(buf, "%s",ch->in_room->description);
		
			
		
		send_to_char(buf, ch);
		
		
		if (!IS_NPC (ch) && IS_SET (ch->act, PLR_AUTOEXIT))
			do_exits (ch, "auto");
		
			send_to_char ("\n\r", ch);
		/*	if (ch->in_room->blood == 100)
				sprintf (buf, "You notice that the room is completely drenched in blood.\n\r");
			else if (ch->in_room->blood > 75)
				sprintf (buf, "You notice that there is a very large amount of blood around the room.\n\r");
			else if (ch->in_room->blood > 50)
				sprintf (buf, "You notice that there is a large quantity of blood around the room.\n\r");
			else if (ch->in_room->blood > 25)
				sprintf (buf, "You notice a fair amount of blood on the floor.\n\r");
			else if (ch->in_room->blood > 10)
				sprintf (buf, "You notice several blood stains on the floor.\n\r");
			else if (ch->in_room->blood > 5)
				sprintf (buf, "You notice a few blood stains on the floor.\n\r");
			else if (ch->in_room->blood > 2)
				sprintf (buf, "You notice a couple of blood stains on the floor.\n\r");
			else if (ch->in_room->blood > 0)
				sprintf (buf, "You notice a few drops of blood on the floor.\n\r");
			else
				sprintf (buf, "You notice nothing special in the room.\n\r");
			if (ch->in_room->blood > 0)
				send_to_char (buf, ch); */
		}

		show_list_to_char (ch->in_room->contents, ch, FALSE, FALSE);
		show_char_to_char (ch->in_room->people, ch);
		return;
	}

	if (!str_cmp (arg1, "i") || !str_cmp (arg1, "in"))
	{
		/* 'look in' */
		if (arg2[0] == '\0')
		{
			send_to_char ("Look in what?\n\r", ch);
			return;
		}

		if ((obj = get_obj_here (ch, arg2)) == NULL)
		{
			send_to_char ("You do not see that here.\n\r", ch);
			return;
		}

		switch (obj->item_type)
		{
		default:
			send_to_char ("That is not a container.\n\r", ch);
			break;

		case ITEM_PORTAL:
			pRoomIndex = get_room_index (obj->value[0]);
			location = ch->in_room;
			if (pRoomIndex == NULL)
			{
				send_to_char ("It doesn't seem to lead anywhere.\n\r", ch);
				return;
			}
			if (obj->value[2] == 1 || obj->value[2] == 3)
			{
				send_to_char ("It seems to be closed.\n\r", ch);
				return;
			}
			char_from_room (ch);
			char_to_room (ch, pRoomIndex);

			found = FALSE;
			for (portal = ch->in_room->contents; portal != NULL; portal = portal_next)
			{
				portal_next = portal->next_content;
				if ((obj->value[0] == portal->value[3]) && (obj->value[3] == portal->value[0]))
				{
					found = TRUE;
					if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_SET (portal->extra_flags, ITEM_SHADOWPLANE))
					{
						REMOVE_BIT (ch->affected_by, AFF_SHADOWPLANE);
						do_look (ch, "auto");
						SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_SET (portal->extra_flags, ITEM_SHADOWPLANE))
					{
						SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
						do_look (ch, "auto");
						REMOVE_BIT (ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else
					{
						do_look (ch, "auto");
						break;
					}
				}
			}
			char_from_room (ch);
			char_to_room (ch, location);
			break;

		case ITEM_DRINK_CON:
			if (obj->value[1] <= 0)
			{
				send_to_char ("It is empty.\n\r", ch);
				break;
			}

			if (obj->value[1] < obj->value[0] / 5)
				sprintf (buf, "There is a little %s liquid left in it.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0] / 4)
				sprintf (buf, "It contains a small about of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0] / 3)
				sprintf (buf, "It's about a third full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0] / 2)
				sprintf (buf, "It's about half full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0])
				sprintf (buf, "It is almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] == obj->value[0])
				sprintf (buf, "It's completely full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else
				sprintf (buf, "Somehow it is MORE than full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			send_to_char (buf, ch);
			break;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			if (IS_SET (obj->value[1], CONT_CLOSED))
			{
				send_to_char ("It is closed.\n\r", ch);
				break;
			}

			act ("$p contains:", ch, obj, NULL, TO_CHAR);
			show_list_to_char (obj->contains, ch, TRUE, TRUE);
			break;
		}
		return;
	}
	if (IS_IMMORTAL(ch) && (victim = get_char_room(ch, arg1)) != NULL)
	{
		found_char = 1;
	}
	else if ((victim = get_char_room_from_alias(ch, arg1)) != NULL)
	{
		found_char = 1;
	}
	else if ((victim = get_char_room_from_desc(ch, arg1)) != NULL)
	{
		found_char = 1;
	}
	else
		found_char = 0;
//	if ((victim = get_char_room_from_desc(ch, arg1)) != NULL || (victim = get_char_room_from_alias(ch, arg1)) != NULL)
//	if ((victim = get_char_room (ch, arg1)) != NULL)
	if (found_char)
	{
		
		
		if ( ( (IS_MORE3(victim, MORE3_OBFUS1)) || (IS_MORE3(victim, MORE3_OBFUS2)) ) && (victim->pcdata->powers[DISC_OBFUSCATE] > ch->pcdata->powers[DISC_AUSPEX]) && !IS_IMMORTAL (ch))
		
		
		
		
		{
			send_to_char ("You do not see that here.\n\r", ch);
			return;
		}
	
		else{
			show_char_to_char_1 (victim, ch);
			return;
		}
	}
	for (vch = char_list; vch != NULL; vch = vch_next)
	{
		vch_next = vch->next;
		if (vch->in_room == NULL)
			continue;
		if (vch->in_room == ch->in_room)
		{
			if (!IS_NPC (vch) && !str_cmp (arg1, vch->morph))
			{
				show_char_to_char_1 (vch, ch);
				return;
			}
			continue;
		}
	}

	if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
	{
		obj = get_obj_in_obj (ch, arg1);
		if (obj != NULL)
		{
			send_to_char (obj->description, ch);
			send_to_char ("\n\r", ch);
			return;
		}
	}

	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
			continue;
		if (can_see_obj (ch, obj))
		{
			pdesc = get_extra_descr (arg1, obj->extra_descr);
			if (pdesc != NULL)
			{
				send_to_char (pdesc, ch);
				return;
			}

			pdesc = get_extra_descr (arg1, obj->pIndexData->extra_descr);
			if (pdesc != NULL)
			{
				send_to_char (pdesc, ch);
				return;
			}
		}

		if (is_name (arg1, obj->name))
		{
			if (obj->item_type == ITEM_CONTRACT)
			{
				if (obj->value[0] == CONTRACT_NONE)
					send_to_char ("#R&------------------------ DAEMONIC CONTRACT -----------------------&\n\r", ch);
				else
					send_to_char ("#R&------------------- ACCEPTED DAEMONIC CONTRACT -------------------&\n\r", ch);
				sprintf (buf3, "#R| Contractor: #e%-10s#R                       Soul Point Cost: #e%2d#R |\n\r", obj->questowner, obj->value[1]);
				send_to_char (buf3, ch);
				send_to_char ("#R&------------------------------------------------------------------&\n\r", ch);
				send_to_char (obj->description, ch);
				send_to_char ("\n\r#R&------------------------------------------------------------------&\n\r", ch);
			}
			else
				send_to_char (obj->description, ch);
			send_to_char ("\n\r", ch);
			return;
		}
	}

	for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
	{
		if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
			continue;
		if (can_see_obj (ch, obj))
		{
			pdesc = get_extra_descr (arg1, obj->extra_descr);
			if (pdesc != NULL)
			{
				send_to_char (pdesc, ch);
				return;
			}

			pdesc = get_extra_descr (arg1, obj->pIndexData->extra_descr);
			if (pdesc != NULL)
			{
				send_to_char (pdesc, ch);
				return;
			}
		}

		if (is_name (arg1, obj->name))
		{
			send_to_char (obj->description, ch);
			send_to_char ("\n\r", ch);
			return;
		}
	}

	pdesc = get_extra_descr (arg1, ch->in_room->extra_descr);
	if (pdesc != NULL)
	{
		send_to_char (pdesc, ch);
		return;
	}

	if (!str_cmp (arg1, "n") || !str_cmp (arg1, "north"))
		door = 0;
	else if (!str_cmp (arg1, "e") || !str_cmp (arg1, "east"))
		door = 1;
	else if (!str_cmp (arg1, "s") || !str_cmp (arg1, "south"))
		door = 2;
	else if (!str_cmp (arg1, "w") || !str_cmp (arg1, "west"))
		door = 3;
	else if (!str_cmp (arg1, "u") || !str_cmp (arg1, "up"))
		door = 4;
	else if (!str_cmp (arg1, "d") || !str_cmp (arg1, "down"))
		door = 5;
	else
	{
		send_to_char ("You do not see that here.\n\r", ch);
		return;
	}

	/* 'look direction' */
	if ((pexit = ch->in_room->exit[door]) == NULL)
	{
		send_to_char ("Nothing special there.\n\r", ch);
		return;
	}

	return;
}



void do_examine (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Examine what?\n\r", ch);
		return;
	}

	do_look (ch, arg);

	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		if (obj->condition >= 100)
			sprintf (buf, "You notice that %s is in perfect condition.\n\r", obj->short_descr);
		else if (obj->condition >= 75)
			sprintf (buf, "You notice that %s is in good condition.\n\r", obj->short_descr);
		else if (obj->condition >= 50)
			sprintf (buf, "You notice that %s is in average condition.\n\r", obj->short_descr);
		else if (obj->condition >= 25)
			sprintf (buf, "You notice that %s is in poor condition.\n\r", obj->short_descr);
		else
			sprintf (buf, "You notice that %s is in awful condition.\n\r", obj->short_descr);
		send_to_char (buf, ch);
		switch (obj->item_type)
		{
		default:
			break;

		case ITEM_DRINK_CON:
		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			send_to_char ("When you look inside, you see:\n\r", ch);
			sprintf (buf, "in %s", arg);
			do_look (ch, buf);
		}
	}

	return;
}



/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits (CHAR_DATA * ch, char *argument)
{
	extern char *const dir_name[];
	char buf[MAX_STRING_LENGTH];
	EXIT_DATA *pexit;
	bool found;
	bool fAuto;
	int door;

	buf[0] = '\0';
	fAuto = !str_cmp (argument, "auto");

	if (!check_blind (ch))
		return;

	strcpy (buf, fAuto ? "#c[#CExits#c:#B" : "#CObvious exits#c:#B\n\r");

	found = FALSE;
	for (door = 0; door <= 5; door++)
	{
		if ((pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL)
		{
			found = TRUE;
                    if (fAuto)
                        {
	                if ( IS_SET(pexit->exit_info, EX_CLOSED) )
                          {
                                 sprintf( buf + strlen(buf), "#C %-5s(closed)",
                                 dir_name[door]);
                          }
			else
			  {
			        strcat (buf, "#C ");
                                strcat (buf, dir_name[door]);
                          }
                        }
			else
			{
				if (!IS_NPC(ch) && IS_IMMORTAL(ch))
				{
					sprintf (buf + strlen (buf), "%-5s - (%d) %s\n\r", capitalize (dir_name[door]), pexit->to_room->vnum, pexit->to_room->name);
				}
				else if (IS_SET (world_affects, WORLD_FOG))
				{
					sprintf (buf + strlen (buf), "%-5s - Too foggy to tell.\n\r", capitalize (dir_name[door]));
				}
				else if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_NIGHTSIGHT))
				{
					sprintf (buf + strlen (buf), "%-5s - %s\n\r", capitalize (dir_name[door]), pexit->to_room->name);
				}
				else
					sprintf (buf + strlen (buf), "%-5s - %s\n\r", capitalize (dir_name[door]), room_is_dark (pexit->to_room) ? "Too dark to tell" : pexit->to_room->name);
			}
		}
	}

	if (!found)
		strcat (buf, fAuto ? " none" : "None.\n\r");

	if (fAuto)
		strcat (buf, "#c]\n\r");

	send_to_char (buf, ch);
	return;
}

void do_score (CHAR_DATA * ch, char *argument)
{

	char arg[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH))
	{
		obj_score (ch, ch->pcdata->chobj);
		return;
	}

	if (arg[0] != '\0' && !str_cmp( arg, "extra" ) )
	{
		page_to_char( display_extra(ch), ch);
		/*if (ch->desc->snoop_by != NULL)
		 send_to_char( "\n\r.", ch ); */
		send_to_char( "\n\r", ch );
		return;
	}

	if (IS_INQUISITOR(ch))
	{
		inquis_score(ch, argument);
		return;
	}

	if (IS_VAMPIRE(ch) || IS_GHOUL(ch))
	{
		vamp_score(ch, argument);
		return;
	}
	
	if (IS_SHIFTER(ch))
	{
		shifter_score(ch, argument);
		return;
	}
	
	
	mortal_score(ch, argument);
	return;
}

void inquis_score ( CHAR_DATA *ch, char * argument)
{
   char buf[MAX_STRING_LENGTH*2];

   sprintf(buf, "\n\r"
   "#w}-----------------------------------------------------------------------------{#n\n\r"
   "#w|  #BName  #w:#B %-19s  Title#w:#B %-38s #w|#n\n\r"
   "#w|  #BNature#w:#B %-16s  Demeanor#w:#B %-38s#w |#n\n\r"
   "#w|  #BOrder #w:#B %-16s   Impulse#w:#B %-38s#w |#n\n\r"
   "#w}---------------------------------#C/#BAttributes#C/#w--------------------------------{#n\n\r"
   "#w}--------#C/#cPhysical#C/#w-----------------#C/#cSocial#C/#w----------------#C/#cMental#C/#w----------{#n\n\r"
   "#w| #BStrength#w____%9s |  #BCharisma#w______%9s |  #BPerception#w____%9s |#n\n\r"
   "#w| #BDexterity#w___%9s |  #BManipulation#w__%9s |  #BIntelligence#w__%9s |#n\n\r"
   "#w| #BStamina#w_____%9s |  #BAppearance#w____%9s |  #BWits#w__________%9s |#n\n\r"
   "#w}----------------------------------#C/#BAbilities#C/#w--------------------------------{#n\n\r"
   "#w}-------#C/#cTalents#C/#w-------------------#C/#cSkills#C/#w---------------#C/#cKnowledges#C/#w-------{#n\n\r"
   "#w| #BAlertness#w___%5s |  #BAnimal Ken#w____%5s |  #BAcademics#w_____%5s |#n\n\r"
   "#w| #BAthletics#w___%5s |  #BFirearms#w______%5s |  #BComputer#w______%5s |#n\n\r"
   "#w| #BBrawl#w_______%5s |  #BSecurity#w______%5s |  #BInvestigation#w_%5s |#n\n\r"
   "#w| #BDodge#w_______%5s |  #BCrafts#w________%5s |  #BLaw#w___________%5s |#n\n\r"
   "#w| #BEmpathy#w_____%5s |  #BEtiquette#w_____%5s |  #BLinguistics#w___%5s |#n\n\r"
   "#w| #BExpression#w__%5s |  #BMelee#w_________%5s |  #BMedicine#w______%5s |#n\n\r"
   "#w| #BIntimidation#w%5s |  #BPerformance#w___%5s |  #BOccult#w________%5s |#n\n\r"
   "#w| #BLeadership#w__%5s |  #BDrive#w_________%5s |  #BPolitics#w______%5s |#n\n\r"
   "#w| #BStreetwise#w__%5s |  #BStealth#w_______%5s |  #BFinance#w_______%5s |#n\n\r"
   "#w| #BSubterfuge#w__%5s |  #BSurvival#w______%5s |  #BScience#w_______%5s |#n\n\r"
   "#w}--------------------------------#C/#BAdvantages#C/#w---------------------------------{#n\n\r"
   "#w|                            #c==== #CHoly Arts#c ====#w                              |#n\n\r",
   
      ch->name, ((ch->pcdata) ? ch->pcdata->title : "None"),
      strlen (ch->nature) < 2 ? "None" : ch->nature, strlen (ch->demeanor) < 2 ? "None" : ch->demeanor,         
      strlen (ch->order) < 2 ? "None" : ch->order, strlen (ch->impulse) < 2 ? "None" : ch->impulse,
      
      show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
      show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
      show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->clan, "Nosferatu")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),

      show_dot(ch->abilities[TALENTS][TAL_ALERTNESS]),  show_dot(ch->abilities[SKILLS][SKI_ANIMALKEN]),  show_dot(ch->abilities[KNOWLEDGES][KNO_ACADEMICS]), 
      show_dot(ch->abilities[TALENTS][TAL_ATHLETICS]),  show_dot(ch->abilities[SKILLS][SKI_FIREARMS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_COMPUTER]), 
      show_dot(ch->abilities[TALENTS][TAL_BRAWL]),  show_dot(ch->abilities[SKILLS][SKI_SECURITY]),  show_dot(ch->abilities[KNOWLEDGES][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[TALENTS][TAL_DODGE]),  show_dot(ch->abilities[SKILLS][SKI_CRAFTS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LAW]), 
      show_dot(ch->abilities[TALENTS][TAL_EMPATHY]),  show_dot(ch->abilities[SKILLS][SKI_ETIQUETTE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LINGUISTICS]), 
      show_dot(ch->abilities[TALENTS][TAL_EXPRESSION]),  show_dot(ch->abilities[SKILLS][SKI_MELEE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_MEDICINE]), 
      show_dot(ch->abilities[TALENTS][TAL_INTIMIDATION]),  show_dot(ch->abilities[SKILLS][SKI_PERFORMANCE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_OCCULT]), 
      show_dot(ch->abilities[TALENTS][TAL_LEADERSHIP]),  show_dot(ch->abilities[SKILLS][SKI_DRIVE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_POLITICS]), 
      show_dot(ch->abilities[TALENTS][TAL_STREETWISE]),  show_dot(ch->abilities[SKILLS][SKI_STEALTH]),  show_dot(ch->abilities[KNOWLEDGES][KNO_FINANCE]), 
      show_dot(ch->abilities[TALENTS][TAL_SUBTERFUGE]),  show_dot(ch->abilities[SKILLS][SKI_SURVIVAL]),  show_dot(ch->abilities[KNOWLEDGES][KNO_SCIENCE])
      );
      send_to_char(buf, ch);

      if (ch->pcdata->powers[DISC_SOPHIA_PATRIS] > 0)
      {
		sprintf (buf, "#w|                        #BSophia Patris#w_____%-8s#n                          #w|#n\n\r", show_dot(ch->pcdata->powers[DISC_SOPHIA_PATRIS]));
		send_to_char(buf, ch);
      }
      if (ch->pcdata->powers[DISC_SEDULITAS_FILIUS] > 0)
      {
		sprintf (buf, "#w|                        #BSedulitas Filius#w__%-8s#n                          #w|#n\n\r", show_dot(ch->pcdata->powers[DISC_SEDULITAS_FILIUS]));
		send_to_char(buf, ch);
      }
      if (ch->pcdata->powers[DISC_BONI_SPIRITUS] > 0)
      {
		sprintf (buf, "#w|                        #BBoni Spiritus#w_____%-8s#n                          #w|#n\n\r", show_dot(ch->pcdata->powers[DISC_BONI_SPIRITUS]));
		send_to_char(buf, ch);
      }

      send_to_char("#w|                                                                             #w|#n\n\r", ch);      
      
   sprintf(buf, 
   "#w|                                                                             |#n\n\r"
   "#w|       #CConscience               #CSelf-Control                 #CCourage#w         |#n\n\r"
   "#w|       #e%9s                 %9s                  %9s#w        |#n\n\r"
   "#w|         Faith                    Wisdom                      Zeal           |#n\n\r"
   "#w|       #e%9s                 %9s                  %9s#w        |#n\n\r"
   "#w}-------------------------------#C/#BOther Traits#C/#w--------------------------------{#n\n\r"
   "#w|    #b=====#BWillpower#b=====          ===#wPiety#b===          #w==#CDamage Total#C: #R%d#w===#n#w   |#n\n\r"
   "#w|    #e%19s      %19s       #wAgg   Lethal  Bash #w   |#n\n\r"
   "#w|   #e%21s                                #B%d      %d      %d#w      |#n\n\r"
   "#w|                             #B====#CConviction#B=====#w                             |#n\n\r"
   "#w|#e                            %19s                           #w |#n\n\r"
   "#w|                                                                             |#n\n\r"
   "#w|   #CXP#B:%-4d        #CHours#B:#C%-5d #CIC#C:%-5d              #B  Streetlight Manifesto  #w|#n\n\r"
   "#w|   #wConviction Bank#B:#w%-4d                                                      #w|#n\n\r"
   "#w}-----------------------------------------------------------------------------{#n\n\r",
  

      show_spot_5(ch->pcdata->virtues[VIRTUE_CONSCIENCE]), show_spot_5(ch->pcdata->virtues[VIRTUE_SELFCONTROL]), show_spot_5(ch->pcdata->virtues[VIRTUE_COURAGE]),
      show_spot_5(ch->pcdata->virtues[VIRTUE_FAITH]), show_spot_5(ch->pcdata->virtues[VIRTUE_WISDOM]), show_spot_5(ch->pcdata->virtues[VIRTUE_ZEAL]),
      (ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
      show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_spot(ch->pcdata->roadt), 
      show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]), 
      ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
      show_slash(ch->pcdata->conviction[CONVICTION_CURRENT]),
      
      ch->exp,((get_age (ch) - 17) * 2), get_hoursIC(ch), 
	  ch->pcdata->conviction_bank
    );
    
    send_to_char(buf, ch);
    return;
}


void mortal_score( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH*2];

   sprintf(buf, "\n\r"
      "#B}-----------------------------------------------------------------------------{#n\n\r"
      "#B|  #cName  #C:#n %-19s  #cTitle#C:#n %-38s #B|#n\n\r"
      "#B|  #cNature#C:#n %-16s  #cDemeanor#C:#n %-38s #B|#n\n\r"
      "#B}---------------------------------#c/#wAttributes#c/#B--------------------------------{#n\n\r"
      "#B}--------#c/#CPhysical#c/#B-----------------#c/#CSocial#c/#B----------------#c/#CMental#c/#B----------{#n\n\r"
      "#B| #cStrength#w____%9s #B|  #cCharisma#w______%9s #B|  #cPerception#w____%9s #B|#n\n\r"
      "#B| #cDexterity#w___%9s #B|  #cManipulation#w__%9s #B|  #cIntelligence#w__%9s #B|#n\n\r"
      "#B| #cStamina#w_____%9s #B|  #cAppearance#w____%9s #B|  #cWits#w__________%9s #B|#n\n\r"
      "#B}----------------------------------#c/#wAbilities#c/#B--------------------------------{#n\n\r"
      "#B}-------#c/#CTalents#c/#B-------------------#c/#CSkills#c/#B---------------#c/#CKnowledges#c/#B-------{#n\n\r"
      "#B| #cAlertness#w___%5s #B|  #cAnimal Ken#w____%5s #B|  #cAcademics#w_____%5s #B|#n\n\r"
      "#B| #cAthletics#w___%5s #B|  #cFirearms#w______%5s #B|  #cComputer#w______%5s #B|#n\n\r"
      "#B| #cBrawl#w_______%5s #B|  #cSecurity#w______%5s #B|  #cInvestigation#w_%5s #B|#n\n\r"
      "#B| #cDodge#w_______%5s #B|  #cCrafts#w________%5s #B|  #cLaw#w___________%5s #B|#n\n\r"
      "#B| #cEmpathy#w_____%5s #B|  #cEtiquette#w_____%5s #B|  #cLinguistics#w___%5s #B|#n\n\r"
      "#B| #cExpression#w__%5s #B|  #cMelee#w_________%5s #B|  #cMedicine#w______%5s #B|#n\n\r"
      "#B| #cIntimidation#w%5s #B|  #cPerformance#w___%5s #B|  #cOccult#w________%5s #B|#n\n\r"
      "#B| #cLeadership#w__%5s #B|  #cDrive#w_________%5s #B|  #cPolitics#w______%5s #B|#n\n\r"
      "#B| #cStreetwise#w__%5s #B|  #cStealth#w_______%5s #B|  #cFinance#w_______%5s #B|#n\n\r"
      "#B| #cSubterfuge#w__%5s #B|  #cSurvival#w______%5s #B|  #cScience#w_______%5s #B|#n\n\r"
      "#B}-------------------------------#c/#wOther Traits#c/#B--------------------------------{#n\n\r"
      "#B|       #w=#CConscience#w=            =#CSelf-Control#w=                =#CCourage#w=       #B|#n\n\r"
      "#B|         #e%9s                %9s                  %9s#B       |#n\n\r"
      "#B|                                                                             |#n\n\r"
      "#B|     #c=====#wWillpower#c=====         ===#BPiety#c===          #c==#wDamage Total#c: #R%d#c===#n   #B|#n\n\r"
      "#B|     #w%19s     #C%5s       #cAgg   Lethal  Bash    #B|#n\n\r"
      "#B|    #C%21s        #c===========            #B%d      %d      %d      #B|#n\n\r"
      "#B|                                                                             |#n\n\r"
      "#B|    #cXP#C:#n%-4d     #cHours#C:#n %-5d #cIC#C: #n%-5d            #b  Streetlight Manifesto    #B|#n\n\r"
      "#B}-----------------------------------------------------------------------------{#n\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->title : "None"),
      strlen (ch->nature) < 2 ? "None" : ch->nature, strlen (ch->demeanor) < 2 ? "None" : ch->demeanor,
      show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
      show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
      show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->clan, "Nosferatu")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),

      show_dot(ch->abilities[TALENTS][TAL_ALERTNESS]),  show_dot(ch->abilities[SKILLS][SKI_ANIMALKEN]),  show_dot(ch->abilities[KNOWLEDGES][KNO_ACADEMICS]), 
      show_dot(ch->abilities[TALENTS][TAL_ATHLETICS]),  show_dot(ch->abilities[SKILLS][SKI_FIREARMS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_COMPUTER]), 
      show_dot(ch->abilities[TALENTS][TAL_BRAWL]),  show_dot(ch->abilities[SKILLS][SKI_SECURITY]),  show_dot(ch->abilities[KNOWLEDGES][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[TALENTS][TAL_DODGE]),  show_dot(ch->abilities[SKILLS][SKI_CRAFTS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LAW]), 
      show_dot(ch->abilities[TALENTS][TAL_EMPATHY]),  show_dot(ch->abilities[SKILLS][SKI_ETIQUETTE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LINGUISTICS]), 
      show_dot(ch->abilities[TALENTS][TAL_EXPRESSION]),  show_dot(ch->abilities[SKILLS][SKI_MELEE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_MEDICINE]), 
      show_dot(ch->abilities[TALENTS][TAL_INTIMIDATION]),  show_dot(ch->abilities[SKILLS][SKI_PERFORMANCE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_OCCULT]), 
      show_dot(ch->abilities[TALENTS][TAL_LEADERSHIP]),  show_dot(ch->abilities[SKILLS][SKI_DRIVE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_POLITICS]), 
      show_dot(ch->abilities[TALENTS][TAL_STREETWISE]),  show_dot(ch->abilities[SKILLS][SKI_STEALTH]),  show_dot(ch->abilities[KNOWLEDGES][KNO_FINANCE]), 
      show_dot(ch->abilities[TALENTS][TAL_SUBTERFUGE]),  show_dot(ch->abilities[SKILLS][SKI_SURVIVAL]),  show_dot(ch->abilities[KNOWLEDGES][KNO_SCIENCE]),
      show_spot_5(ch->pcdata->virtues[VIRTUE_CONSCIENCE]), show_spot_5(ch->pcdata->virtues[VIRTUE_SELFCONTROL]), show_spot_5(ch->pcdata->virtues[VIRTUE_COURAGE]),
      (ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
      show_spot(ch->pcdata->willpower[WILLPOWER_MAX]),show_spot(ch->pcdata->roadt), 
      show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]),
      ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
      ch->exp, ((get_age (ch) - 17) * 2), get_hoursIC(ch)
   );
   send_to_char(buf, ch);
   return;
}


void vamp_score( CHAR_DATA *ch, char *argument )
{
char buf[MAX_STRING_LENGTH*2];
int col = 0;
int sn;
char disc[30];
bool second = FALSE;
sprintf(buf,
"\n\r"
"#r}-----------------------------------------------------------------------------{#n\n\r"
      "#r|  #cName  #R:#n %-19s  #cTitle#R:#n %-38s #r|#n\n\r"
      "#r|  #cNature#R:#n %-16s  #cDemeanor#R:#n %-38s #r|#n\n\r"
"#r|#n  #cClan  #R:#c %-18s    #cPath#R:#c %-13s #cSect#R:#c %-13s#cGen#R:#c%-2d#r|#n\n\r"
"#r}---------------------------------#R/#PAttributes#R/#r--------------------------------{#n\n\r"
      "#r}--------#e/#RPhysical#e/#r-----------------#e/#RSocial#e/#r----------------#e/#RMental#e/#r----------{#n\n\r"
      "#r| #pStrength#w____%9s #r|  #pCharisma#w______%9s #r|  #pPerception#w____%9s #r|#n\n\r"
      "#r| #pDexterity#w___%9s #r|  #pManipulation#w__%9s #r|  #pIntelligence#w__%9s #r|#n\n\r"
      "#r| #pStamina#w_____%9s #r|  #pAppearance#w____%9s #r|  #pWits#w__________%9s #r|#n\n\r"
      "#r}----------------------------------#R/#PAbilities#R/#r--------------------------------{#n\n\r"
      "#r}-------#e/#RTalents#e/#r-------------------#e/#RSkills#e/#r---------------#e/#RKnowledges#e/#r-------{#n\n\r"
      "#r| #pAlertness#w___%5s #r|  #pAnimal Ken#w____%5s #r|  #pAcademics#w_____%5s #r|#n\n\r"
      "#r| #pAthletics#w___%5s #r|  #pFirearms#w______%5s #r|  #pComputer#w______%5s #r|#n\n\r"
      "#r| #pBrawl#w_______%5s #r|  #pSecurity#w______%5s #r|  #pInvestigation#w_%5s #r|#n\n\r"
      "#r| #pDodge#w_______%5s #r|  #pCrafts#w________%5s #r|  #pLaw#w___________%5s #r|#n\n\r"
      "#r| #pEmpathy#w_____%5s #r|  #pEtiquette#w_____%5s #r|  #pLinguistics#w___%5s #r|#n\n\r"
      "#r| #pExpression#w__%5s #r|  #pMelee#w_________%5s #r|  #pMedicine#w______%5s #r|#n\n\r"
      "#r| #pIntimidation#w%5s #r|  #pPerformance#w___%5s #r|  #pOccult#w________%5s #r|#n\n\r"
      "#r| #pLeadership#w__%5s #r|  #pDrive#w_________%5s #r|  #pPolitics#w______%5s #r|#n\n\r"
      "#r| #pStreetwise#w__%5s #r|  #pStealth#w_______%5s #r|  #pFinance#w_______%5s #r|#n\n\r"
      "#r| #pSubterfuge#w__%5s #r|  #pSurvival#w______%5s #r|  #pScience#w_______%5s #r|#n\n\r"
"#r}--------------------------------#R/#PAdvantages#R/#r---------------------------------{\n\r"
"#r|                            #e====#RDisciplines#e====   #r                           |\n\r",
 ch->name, 
(ch->pcdata) ? ch->pcdata->title : "None",
 strlen (ch->nature) < 2 ? "None" : ch->nature, strlen (ch->demeanor) < 2 ? "None" : ch->demeanor,
 ch->clan, (ch->pcdata->road != -1) ? road_names[ch->pcdata->road][0] : 
"None", strlen (ch->side) < 2 ? "None" : ch->side, ch->vampgen,
 show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
 show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
 show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->clan, "Nosferatu")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),
  
 show_dot(ch->abilities[TALENTS][TAL_ALERTNESS]),  show_dot(ch->abilities[SKILLS][SKI_ANIMALKEN]),  show_dot(ch->abilities[KNOWLEDGES][KNO_ACADEMICS]), 
 show_dot(ch->abilities[TALENTS][TAL_ATHLETICS]),  show_dot(ch->abilities[SKILLS][SKI_FIREARMS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_COMPUTER]), 
 show_dot(ch->abilities[TALENTS][TAL_BRAWL]),  show_dot(ch->abilities[SKILLS][SKI_SECURITY]),  show_dot(ch->abilities[KNOWLEDGES][KNO_INVESTIGATION]), 
 show_dot(ch->abilities[TALENTS][TAL_DODGE]),  show_dot(ch->abilities[SKILLS][SKI_CRAFTS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LAW]), 
 show_dot(ch->abilities[TALENTS][TAL_EMPATHY]),  show_dot(ch->abilities[SKILLS][SKI_ETIQUETTE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LINGUISTICS]), 
 show_dot(ch->abilities[TALENTS][TAL_EXPRESSION]),  show_dot(ch->abilities[SKILLS][SKI_MELEE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_MEDICINE]), 
 show_dot(ch->abilities[TALENTS][TAL_INTIMIDATION]),  show_dot(ch->abilities[SKILLS][SKI_PERFORMANCE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_OCCULT]), 
 show_dot(ch->abilities[TALENTS][TAL_LEADERSHIP]),  show_dot(ch->abilities[SKILLS][SKI_DRIVE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_POLITICS]), 
 show_dot(ch->abilities[TALENTS][TAL_STREETWISE]),  show_dot(ch->abilities[SKILLS][SKI_STEALTH]),  show_dot(ch->abilities[KNOWLEDGES][KNO_FINANCE]), 
 show_dot(ch->abilities[TALENTS][TAL_SUBTERFUGE]),  show_dot(ch->abilities[SKILLS][SKI_SURVIVAL]),  show_dot(ch->abilities[KNOWLEDGES][KNO_SCIENCE]));
 send_to_char(buf, ch);
col  = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "#pNone#w___________");
				break;
			case DISC_ANIMALISM:
				strcpy (disc, "#pAnimalism#w______");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "#pAuspex#w_________");
				break;
			case DISC_ABOMBWE:
				strcpy (disc, "#pAbombwe#w________");
				break;
			case DISC_BARDO:
				strcpy (disc, "#pBardo#w__________");
				break;
			case DISC_CELERITY:
				strcpy (disc, "#pCelerity#w_______");
				break;
			case DISC_CHIMERSTRY:
				strcpy (disc, "#pChimerstry#w_____");
				break;
			case DISC_DAIMOINON:
				strcpy (disc, "#pDaimoinon#w______");
				break;
			case DISC_DEIMOS:
				strcpy (disc, "#pDeimos#w_________");
				break;
			case DISC_DEMENTATION:
				strcpy (disc, "#pDementation#w____");
				break;
			case DISC_DOMINATE:
				strcpy (disc, "#pDominate#w_______");
				break;
			case DISC_FORTITUDE:
				strcpy (disc, "#pFortitude#w______");
				break;
			case DISC_MA:
				strcpy (disc, "#pMa#w_____________");
				break;
			case DISC_MELPOMINEE:
				strcpy (disc, "#pMelpominee#w_____");
				break;
			case DISC_MYTHERCERIA:
				strcpy (disc, "#pMytherceria#w____");
				break;
			case DISC_MORTIS:
				strcpy (disc, "#pMortis#w_________");
				break;
			case DISC_NECROMANCY:
				strcpy (disc, "#pNecromancy#w_____");
				break;
			case DISC_OBEAH:
				strcpy (disc, "#pObeah#w__________");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "#pObfuscate#w______");
				break;
			case DISC_OBTENEBRATION:
				strcpy (disc, "#pObtenebration#w__");
				break;
			case DISC_POTENCE:
				strcpy (disc, "#pPotence#w________");
				break;
			case DISC_PRESENCE:
				strcpy (disc, "#pPresence#w_______");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "#pProtean#w________");
				break;
			case DISC_QUIETUS:
				strcpy (disc, "#pQuietus#w________");
				break;
			case DISC_SERPENTIS:
				strcpy (disc, "#pSerpentis#w______");
				break;
			case DISC_SPIRITUS:
				strcpy (disc, "#pSpiritus#w_______");
				break;
			case DISC_TEMPORIS:
				strcpy (disc, "#pTemporis#w_______");
				break;
			case DISC_THANATOSIS:
				strcpy (disc, "#pThanatosis#w_____");
				break;
			case DISC_THAUMATURGY:
				strcpy (disc, "#pThaumaturgy#w____");
				break;
			case DISC_VICISSITUDE:
				strcpy (disc, "#pVicissitude#w____");
				break;
			case DISC_VALEREN:
				strcpy (disc, "#pValeren#w________");
				break;
			case DISC_GROTESQUOUS:
				strcpy (disc, "#pGrotesquous#w____");
				break;
			case DISC_NIHILISTICS:
				strcpy (disc, "#pNihilistics#w____");
				break;
			case DISC_MALEFICIA:
				strcpy (disc, "#pMaleficia#w______");
				break;
			case DISC_MYTHERCIA:
				strcpy (disc, "#pMythercia#w______");
				break;
			case DISC_STIGA:
				strcpy (disc, "#pStiga#w__________");
				break;
			case DISC_VISCERATIKA:
				strcpy (disc, "#pVisceratika#w____");
				break;
			}
			if (ch->class == CLASS_VAMPIRE && (ch->pcdata->cland[0] == sn || ch->pcdata->cland[1] == sn || ch->pcdata->cland[2] == sn))
				capitalize (disc);
			if (!col)
			{
			send_to_char("#r|             ", ch);
			sprintf (buf, "%s_%9s#n #r|#n   ", disc, show_dot (get_disc(ch, sn)));
			send_to_char (buf, ch);
			col++;
			second = FALSE;
			}
			else
			{
			sprintf (buf, "%s_%9s#n         #r|#n\n\r", disc, show_dot (get_disc(ch, sn)));
			send_to_char (buf, ch);
			 col--;
			second = TRUE;
		        }

	}
if (!second)
send_to_char("                                  #r|#n\n\r", ch);
sprintf(buf,
"#r|                                                                             |\n\r"
"#r| #eConscience/Conviction     #eSelf-Control/Instinct             #eCourage#r         |\n\r"
"#r|#p       %9s                  %9s                 %9s        #r|#n\n\r",  
show_spot_5(ch->pcdata->virtues[VIRTUE_CONSCIENCE]), show_spot_5(ch->pcdata->virtues[VIRTUE_SELFCONTROL]), show_spot_5(ch->pcdata->virtues[VIRTUE_COURAGE]));
send_to_char(buf,ch);
sprintf(buf,
"#r}-------------------------------#R/#POther Traits#R/#r--------------------------------{\n\r");
send_to_char(buf,ch);
sprintf(buf,
"#r|    #e=====#RWillpower#e=====           #e===#RRoad#e===         #e==#pDamage Total#e: #R%d#e===#n    #r|\n\r"
"#r|#p    %19s      %19s      #eAgg   Lethal  Bash     #r|#n\n\r"
"#r|#p   %21s     #p===================       #R%d      %d      %d #r      |#n\n\r"
"#r|                             #e====#RBlood Pool#e====                              #r|\n\r"
"#r|                   #R%40s                #r|#n\n\r"
"#r|                  #R%40s               #r|#n\n\r",
(ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_spot(ch->pcdata->roadt),
show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]),
ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
show_spot_20(ch->blood[BLOOD_POOL]),
show_slash_20(ch->blood[BLOOD_CURRENT]));
send_to_char(buf, ch);
sprintf(buf,
"#r|#n #RStatus#r: #R%-4d  #PXP#p: %-4d   #PHours#p: %-5d #PIC#p: %-5d #p  Streetlight Manifesto  #r   |\n\r",
ch->pcdata->sectstatus, ch->exp, ((get_age (ch) - 17) * 2), get_hoursIC(ch));
send_to_char(buf,ch);
sprintf(buf,
"#r}-----------------------------------------------------------------------------{#n\n\r"
"\n\r");
send_to_char(buf, ch);
return;
}



void shifter_score( CHAR_DATA *ch, char *argument )
{
char buf[MAX_STRING_LENGTH*2];
sprintf(buf,
"\n\r"
"#g}-----------------------------------------------------------------------------{#n\n\r"
"#g|  #cName  #G:#n %-19s  #cTitle#G:#n %-38s #g|#n\n\r"
"#g|  #cNature#G:#n %-16s  #cDemeanor#G:#n %-38s #g|#n\n\r"
"#g|  #cTribe #G:#n %-18s    #cSect#G: %-13s            #cRank#G: %-2d      #g|\n\r"
"#g}---------------------------------#c/#GAttributes#c/#g--------------------------------{#n\n\r"
"#g}--------#c/#CPhysical#c/#g-----------------#c/#CSocial#c/#g----------------#c/#CMental#c/#g----------{#n\n\r"
"#g| #cStrength#w____%9s #g|  #cCharisma#w______%9s #g|  #cPerception#w____%9s #g|#n\n\r"
"#g| #cDexterity#w___%9s #g|  #cManipulation#w__%9s #g|  #cIntelligence#w__%9s #g|#n\n\r"
"#g| #cStamina#w_____%9s #g|  #cAppearance#w____%9s #g|  #cWits#w__________%9s #g|#n\n\r"
"#g}----------------------------------#c/#GAbilities#c/#g--------------------------------{#n\n\r"
"#g}-------#c/#CTalents#c/#g-------------------#c/#CSkills#c/#g---------------#c/#CKnowledges#c/#g-------{#n\n\r"
"#g| #cAlertness#w___%5s #g|  #cAnimal Ken#w____%5s #g|  #cAcademics#w_____%5s #g|#n\n\r"
"#g| #cAthletics#w___%5s #g|  #cFirearms#w______%5s #g|  #cComputer#w______%5s #g|#n\n\r"
"#g| #cBrawl#w_______%5s #g|  #cSecurity#w______%5s #g|  #cInvestigation#w_%5s #g|#n\n\r"
"#g| #cDodge#w_______%5s #g|  #cCrafts#w________%5s #g|  #cLaw#w___________%5s #g|#n\n\r"
"#g| #cEmpathy#w_____%5s #g|  #cEtiquette#w_____%5s #g|  #cLinguistics#w___%5s #g|#n\n\r"
"#g| #cExpression#w__%5s #g|  #cMelee#w_________%5s #g|  #cMedicine#w______%5s #g|#n\n\r"
"#g| #cIntimidation#w%5s #g|  #cPerformance#w___%5s #g|  #cOccult#w________%5s #g|#n\n\r"
"#g| #cLeadership#w__%5s #g|  #cDrive#w_________%5s #g|  #cPolitics#w______%5s #g|#n\n\r"
"#g| #cStreetwise#w__%5s #g|  #cStealth#w_______%5s #g|  #cFinance#w_______%5s #g|#n\n\r"
"#g| #cSubterfuge#w__%5s #g|  #cSurvival#w______%5s #g|  #cScience#w_______%5s #g|#n\n\r",

 ch->name, (ch->pcdata) ? ch->pcdata->title : "None",
 strlen (ch->nature) < 2 ? "None" : ch->nature, strlen (ch->demeanor) < 2 ? "None" : ch->demeanor,
 ch->clan, strlen (ch->side) < 2 ? "None" : ch->side, ch->pcdata->rank,
 show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
 show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
 show_dot(ch->attributes[ATTRIB_STA]), show_dot(ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),
  
 show_dot(ch->abilities[TALENTS][TAL_ALERTNESS]),  show_dot(ch->abilities[SKILLS][SKI_ANIMALKEN]),  show_dot(ch->abilities[KNOWLEDGES][KNO_ACADEMICS]), 
 show_dot(ch->abilities[TALENTS][TAL_ATHLETICS]),  show_dot(ch->abilities[SKILLS][SKI_FIREARMS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_COMPUTER]), 
 show_dot(ch->abilities[TALENTS][TAL_BRAWL]),  show_dot(ch->abilities[SKILLS][SKI_SECURITY]),  show_dot(ch->abilities[KNOWLEDGES][KNO_INVESTIGATION]), 
 show_dot(ch->abilities[TALENTS][TAL_DODGE]),  show_dot(ch->abilities[SKILLS][SKI_CRAFTS]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LAW]), 
 show_dot(ch->abilities[TALENTS][TAL_EMPATHY]),  show_dot(ch->abilities[SKILLS][SKI_ETIQUETTE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_LINGUISTICS]), 
 show_dot(ch->abilities[TALENTS][TAL_EXPRESSION]),  show_dot(ch->abilities[SKILLS][SKI_MELEE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_MEDICINE]), 
 show_dot(ch->abilities[TALENTS][TAL_INTIMIDATION]),  show_dot(ch->abilities[SKILLS][SKI_PERFORMANCE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_OCCULT]), 
 show_dot(ch->abilities[TALENTS][TAL_LEADERSHIP]),  show_dot(ch->abilities[SKILLS][SKI_DRIVE]),  show_dot(ch->abilities[KNOWLEDGES][KNO_POLITICS]), 
 show_dot(ch->abilities[TALENTS][TAL_STREETWISE]),  show_dot(ch->abilities[SKILLS][SKI_STEALTH]),  show_dot(ch->abilities[KNOWLEDGES][KNO_FINANCE]), 
 show_dot(ch->abilities[TALENTS][TAL_SUBTERFUGE]),  show_dot(ch->abilities[SKILLS][SKI_SURVIVAL]),  show_dot(ch->abilities[KNOWLEDGES][KNO_SCIENCE]));
 send_to_char(buf, ch);

if (IS_WEREWOLF(ch))
{
sprintf(buf,
"#g}-------------------------------#c/#GOther Traits#c/#g--------------------------------{\n\r"
"#g|     #o=====#wWillpower#o=====                             #o==#GDamage Total#o: #R%d#o===#n    #g|#n\n\r"
"#g|     #w%19s                              #oAgg   Lethal  Bash     #g|#n\n\r" 
"#g|    #w%21s                              #y%d      %d      %d       #g|#n\n\r"       
"#g|                                                                            #g |\n\r"
"#g|     #o=======#yRage#o========                            =======#GGnosis#o========    #g|#n\n\r"
"#g|     #w%19s                             #w%19s     #g|#n\n\r" 
"#g|    #w%21s                           #w%21s    #g|#n\n\r"    
"#g|     #o=======#wGlory#o=======     =======#wHonor#o=======     ======#wWisdom#o=======     #g|\n\r"
"#g|#n     #w%19s     #w%19s     #w%19s     #g|#n\n\r"
"#g|         #oTemp#y:#G %d                  #oTemp#y:#G %d                  #oTemp#y:#G %d           #g|\n\r",
(ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]), 
ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
show_spot(ch->pcdata->rage[1]), show_spot(ch->pcdata->gnosis[1]), show_slash(ch->pcdata->rage[0]), 
show_slash(ch->pcdata->gnosis[0]), show_spot(ch->pcdata->renown[PERMGLORY]), show_spot(ch->pcdata->renown[PERMHONOR]), 
show_spot(ch->pcdata->renown[PERMWISDOM]), ch->pcdata->renown[TEMPGLORY], ch->pcdata->renown[TEMPHONOR], ch->pcdata->renown[TEMPWISDOM]);
send_to_char(buf, ch);
}


sprintf(buf,
"#g|#n   #GXP#g: #G%-4d   #GHours#g: #G%-5d #GIC#g: #G%-5d             #g  Streetlight Manifesto  #g   |\n\r",
ch->exp, ((get_age (ch) - 17) * 2), get_hoursIC(ch));
send_to_char(buf,ch);
sprintf(buf,
"#g}-----------------------------------------------------------------------------{#n\n\r"
"\n\r");
send_to_char(buf, ch);
return;
}



char *show_dot(int dot)
{
char buf[MSL];
char *return_buf;
int i = 0;

buf[0] = '\0';

if (dot < 0)
dot = 0;
 sprintf(buf, "_________");
 if (dot <= 9 && dot > 0)
 {
   for (i = 0; i < dot; i++)
   {
     buf[8-i] = '*';
   }
   buf[9] = '\0';
 }
 else if (dot == 0)
 {
   buf[8] = 'x';
 }
 else
 {
  char buf2[MSL];
  buf2[0] = '\0';
   for (i = 0; i < dot; i++)
   {
     buf[5-i] = '*';
   }
   sprintf(buf2, "%2d", (dot - 5));
   buf[6] = '-';
   buf[7] = buf2[0];
   buf[8] = buf2[1];
   buf[9] = '\0';
 }
 
 return_buf = str_dup(buf);
return return_buf;
}

char *show_spot(int spot)
{
char buf[MSL];
char *buf2;
int i = 0;
int spacer = 2;
buf[0] = '\0';

if (spot > 10)
spot = 10;
if (spot < 0)
spot = 0;
sprintf(buf, "#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n");
for (i = 0; i < spot ; i++)
{
 buf[(i+spacer)] = '*';
 spacer+=5;
}
 buf2 = str_dup(buf);
return buf2;
}


char *show_spot_5(int spot)
{
char buf[MSL];
char *buf2;
int i = 0;
int spacer = 2;
buf[0] = '\0';


if (spot > 5)
spot = 5;
if (spot < 0)
spot = 0;
sprintf(buf, "#eO#n-#eO#n-#eO#n-#eO#n-#eO#n");
for (i = 0; i < spot ; i++)
{
 buf[(i+spacer)] = '*';
 spacer+=5;
}
 buf2 = str_dup(buf);
return buf2;
}


char *show_spot_20(int spot)
{
char buf[MSL];
char *buf2;
int i = 0;
int spacer = 2;
buf[0] = '\0';


if (spot < 0)
spot = 0;

if (spot <= 20)
{
 sprintf(buf, 
"#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n   ");
 for (i = 0; i < spot ; i++)
 {
  buf[(i+spacer)] = '*';
  spacer+=5;
 }
}
else
{
 sprintf(buf, 
"#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n-#eO#n+%-2d", (spot - 20));
 spot = 20;
 for (i = 0; i < spot ; i++)
 {
  buf[(i+spacer)] = '*';
  spacer+=5;
 }
}
 buf2 = str_dup(buf);
return buf2;

}

char *show_slash_20(int slash)
{
char buf[MSL];
char *buf2;
int i = 0;
int spacer = 3;

buf[0] = '\0';

if (slash < 0)
slash = 0;
if (slash <= 20)
{
 sprintf(buf, 
"[#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]   ");
 for (i = 0; i < slash; i++)
 {
  buf[(i+spacer)] = '/';
  spacer+=5;
 }
}
else
{
 sprintf(buf, 
"[#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]+%-2d", (slash - 20));
 slash = 20;
 for (i = 0; i < slash; i++)
 {
  buf[(i+spacer)] = '/';
  spacer+=5;
 }
}
 buf2 = str_dup(buf);
return buf2;
}


char *show_slash(int slash)
{
char buf[MSL];
char *buf2;
int i = 0;
int spacer = 3;

buf[0] = '\0';

if (slash > 10)
slash = 10;
if (slash < 0)
slash = 0;
sprintf(buf, "[#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]#eO#n]");
for (i = 0; i < slash; i++)
{
 buf[(i+spacer)] = '/';
 spacer+=5;
}

 buf2 = str_dup(buf);
return buf2;
}

char * display_extra (CHAR_DATA *ch)
{
EXTRA *extra;
EXTRA *catagory;
bool stop = FALSE;
char *hold[50];
int i;
char buf[MSL*2];
char *temp2;
char temp[MSL];
temp[0] = '\0';
buf[0] = '\0';
temp2 = str_dup("");
for (i = 0; i < 20; i++)
 hold[i] = str_dup("");

if (IS_NPC(ch))
return NULL;
strcat(buf, "#cBackgrounds, Merits, Flaws and Notes#n\n\r\n\r");


	strcat(buf, "#c  -#CMerits#c-#n\n\r");
		
		for (i = 0; i < 10; i++)
			{
			if (ch->pcdata->merits[i] != NULL){
				sprintf(temp, "     #C- #c%s\r\n", ch->pcdata->merits[i]);
				strcat(buf, temp);
				}
				
			}
	sprintf(temp, "\r\n");
				strcat(buf, temp);
	
	strcat(buf, "  #c-#CFlaws#c-#n\n\r");
			for (i = 0; i < 10; i++){
			if (ch->pcdata->flaws[i] != NULL){
				sprintf(temp, "     #C- #c%s\r\n", ch->pcdata->flaws[i]);
				strcat(buf, temp);
				}
				
			
			}
		
	sprintf(temp, "\r\n");
				strcat(buf, temp);


if (!(extra =ch->pcdata->extra))
  {
strcat(buf, "                    None\n\r"); 
temp2 = str_dup(buf);
return temp2;
  }



for (catagory = ch->pcdata->extra; catagory; catagory = catagory->prev)
{
  for (i=0; i < 50; i++)
  {
    if (!str_cmp(catagory->catagory, hold[i]))
	  break;
	
    if (!str_cmp(hold[i], ""))
	{
	  stop = TRUE;
	  break;
	}
    
  }
if (stop)
{
    hold[i] = str_dup(catagory->catagory);
  sprintf(temp,  "\n\r     -#C%s#n\n\r", catagory->catagory);
  strcat(buf, temp);
stop = FALSE;
}
	if (catagory->rating != -1)
	  sprintf(temp, "     #c%s #C%d#n\n\r", catagory->field,catagory->rating);
	else
	  sprintf(temp, "     #c%s#n\n\r", catagory->field);
	  strcat(buf, temp);

}



temp2 = str_dup(buf);
return temp2;

}


char *const day_name[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday"
};

char *const month_name[] = {
	"January", "February", "March", "April",
	"May", "June", "July", "August", "September",
	"October", "November", "December", "the Dark Shades", "the Shadows",
	"the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void do_time( CHAR_DATA *ch, char *argument )
{
	 extern char str_boot_time[];
	 char buf[MAX_STRING_LENGTH];
	 char descr [ MAX_INPUT_LENGTH ];
	 char arg [ MAX_INPUT_LENGTH ];
	 char * suf;
	 int    day;

	 argument = one_argument( argument, arg );
	 day     = time_info.day + 1;

	 if ( day > 4 && day <  20 ) suf = "th";
	 else if ( day % 10 ==  1       ) suf = "st";
	 else if ( day % 10 ==  2       ) suf = "nd";
	 else if ( day % 10 ==  3       ) suf = "rd";
	 else                             suf = "th";


	 switch ( time_info.hour )
	 {
		  case  1:
		  case  2: sprintf( descr, "late night"       ); break;
		  case  3:
		  case  4: sprintf( descr, "early morning"    ); break;
		  case  5: sprintf( descr, "just before dawn" ); break;
		  case  6: sprintf( descr, "dawn"             ); break;
		  case  7:
		  case  8: sprintf( descr, "early morning"    ); break;
		  case  9:
		  case 10:
		  case 11: sprintf( descr, "morning"          ); break;
		  case 12: sprintf( descr, "midday"           ); break;
		  case 13:
		  case 15: sprintf( descr, "afternoon"        ); break;
		  case 14: sprintf( descr, "mid-afternoon"    ); break;
		  case 16:
		  case 17: sprintf( descr, "evening"          ); break;
		  case 18: sprintf( descr, "twilight"         ); break;
		  case 19: sprintf( descr, "dusk"             ); break;
		  case 20:
		  case 21:
		  case 22: sprintf( descr, "night"            ); break;
		  case 23: sprintf( descr, "late at night"    ); break;
		  case  0: sprintf( descr, "midnight"         ); break;
	 }
	 
	sprintf( buf, "Victorian Age\n\r");	 

	  send_to_char( buf, ch );
	 sprintf( buf,"\n\rStreetlight Manifesto started up at %s\rThe system time is %s\r",
	str_boot_time,(char *) ctime( &current_time ));
	 send_to_char( buf, ch );

	 return;
}





void do_help (CHAR_DATA * ch, char *argument)
{
	char argall[MAX_INPUT_LENGTH];
	char argone[MAX_INPUT_LENGTH];
	HELP_DATA *pHelp;

	if (argument[0] == '\0')
		argument = "summary";

	/*
	 * Reuben's work.  Adding search string capability to help.
	 *
	 */
	if ( argument[0] == '*' )
	{
		char buf[MAX_STRING_LENGTH];
	 	bool fFound = FALSE;

		argument++;
		if ( *argument == '\0' ) return;
		
		send_to_char( "Search completed.\n\r", ch );
		sprintf( buf, "Related Help Topics:\n\r" );
		for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
		{
			if ( pHelp->level > get_trust( ch ) )
			  continue;

			if ( strstr( pHelp->text, argument ) )
			{
			  fFound = TRUE;
			  strcat( buf, pHelp->keyword );
			  strcat( buf, "\n\r" );
			}
		}

		send_to_char( buf, ch );
		if ( !fFound ) send_to_char( "No related information.\n\r", ch );
		return;
	}

	/*
	 * Tricky argument handling so 'help a b' doesn't match a.
	 */
	argall[0] = '\0';
	while (argument[0] != '\0')
	{
		argument = one_argument (argument, argone);
		if (argall[0] != '\0')
			strcat (argall, " ");
		strcat (argall, argone);
	}

	for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
	{
		if (pHelp->level > get_trust (ch))
			continue;

		if (is_name (argall, pHelp->keyword))
		{
			if (pHelp->level >= 0 && str_cmp (argall, "imotd"))
			{
				send_to_char (pHelp->keyword, ch);
				send_to_char ("\n\r", ch);
			}

			/*
			 * Strip leading '.' to allow initial blanks.
			 */
			if (is_name ("LOGOUT", pHelp->keyword))
			{
				send_to_char( pHelp->text, ch);
				return;
			}

			if (pHelp->text[0] == '.')
				page_to_char (pHelp->text + 1, ch );
			else
				page_to_char (pHelp->text, ch);
			return;
		}
	}

	send_to_char ("No help on that word.\n\r", ch);
	return;
}



/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char buf_imm[MAX_STRING_LENGTH];
	char buf_mor[MAX_STRING_LENGTH];
	char kav[MAX_STRING_LENGTH];
	char arg[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	char sex_text[MAX_STRING_LENGTH];
	char invis_info[MAX_STRING_LENGTH];
        char builderflag[MSL];
	char rpflag[MAX_STRING_LENGTH];
	char incog_info[MAX_STRING_LENGTH];
	int char_level;
	char title[MAX_STRING_LENGTH];
	//int i;
	char openb[20];
	char closeb[20];
	DESCRIPTOR_DATA *d;
	int iLevelLower;
	int iLevelUpper;
	int nNumber;
	int nMatch;
	int nTotal;
	int wholistinchar;
	
	bool fClassRestrict;
	bool fImmortalOnly;
	bool isName;
	int loop = 0;

	smash_tilde (argument);

	one_argument (argument, arg);

	if (arg[0] != '\0')
	{
		sprintf (arg2, "|%s*", argument);
		while (arg2[loop++] != '\0')
		{
			if (arg2[loop] == ' ')
				arg2[loop] = '*';
		}
	}

	if (IS_NPC (ch))
		return;

	/*
	 * Set default arguments.
	 */
	iLevelLower = 0;
	iLevelUpper = MAX_LEVEL;
	fClassRestrict = FALSE;
	fImmortalOnly = FALSE;
	isName = FALSE;

	/*
	 * Parse arguments.
	 */
	nNumber = 0;
	for (;;)
	{
		if (arg[0] == '\0')
			break;

		if (is_number (arg))
		{
			send_to_char ("Enter 'Avatar' for level 3's, or 'God' for level 4's and 5's.\n\r", ch);
			return;
		}
		else
		{
			/*
			 * Look for classes to turn on.
			 */
			arg[3] = '\0';
			if (!str_cmp (arg, "imm") || !str_cmp (arg, "ava"))
			{
				fClassRestrict = TRUE;
				break;
			}
			else if (!str_cmp (arg, "god") || !str_cmp (arg, "imp"))
			{
				fImmortalOnly = TRUE;
				break;
			}
			else
			{
				isName = TRUE;
				break;
			}
		}
	}

	
	/*
	 * Now show matching chars.
	 */
	nMatch = 0;
	nTotal = 0;
	wholistinchar = 0;
	buf_imm[0] = '\0';
	buf_mor[0] = '\0';
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *wch;
		char class[MAX_STRING_LENGTH];

		/*
		 * Check for match against restrictions.
		 * Don't use trust as that exposes trusted mortals.
		 */
		if (d->connected != CON_PLAYING)
			continue;
		
		
		if (IS_IMMORTAL (d->character) && !can_see (ch, d->character))
			continue;

		nTotal++;
		
		if (!can_see (ch, d->character))
			continue;

		wch = (d->original != NULL) ? d->original : d->character;
		if ((wch->level < iLevelLower || wch->level > iLevelUpper) || (fImmortalOnly && wch->level < LEVEL_IMMORTAL) || (fClassRestrict && wch->level != LEVEL_HERO))
			continue;

		if (arg2[0] != '\0' && isName && !IS_NPC (wch))
		{
			if (!is_in (wch->name, arg2) && !is_in (wch->pcdata->title, arg2))
				continue;
		}

		nMatch++;
		char_level = wch->level;

		/*
		 * Figure out what to print for class.
		 */
		class[0] = '\0';

		if (IS_SET (wch->extra2, EXTRA2_AFK))
			strcat (class, "#c[#CAFK#c]#n      ");
		else if (IS_SET (wch->extra2, EXTRA2_DND))
			strcat (class, "#e[#RDND#e]#n      ");

		else
			switch (wch->level)
			{
			default:
				break;
				/*Begin Special Titles*/
			
					

		
				

			
			case MAX_LEVEL - 0:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 1:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 2:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 3:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 4:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 5:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 6:
				strcat (class, wch->pcdata->pretitle);
				break;
			case MAX_LEVEL - 7:
			case MAX_LEVEL - 8:
			case MAX_LEVEL - 9:
				switch (wch->race)
				{
				default:

					strcat (class, "#sPlayer#n     ");
					break;
				}
				break;
			case MAX_LEVEL - 10:
				strcat (class, "#wMortal#n     ");
				break;
			case MAX_LEVEL - 11:
				strcat (class, "#yNewbie#n     ");
				break;
			case MAX_LEVEL - 12:
				strcat (class, "#yNewbie#n     ");
				break;
			}
		/*
		 * Format it up.
		 */
		if (IS_VAMPIRE (wch))
		{
			strcpy (openb, "#R<#n");
			strcpy (closeb, "#R>#n");
		}
		else if (IS_GHOUL (wch))
		{
			strcpy (openb, "#R<#n");
			strcpy (closeb, "#R>#n");
		}
		else if (IS_WEREWOLF (wch))
		{
			strcpy (openb, "#G(#n");
			strcpy (closeb, "#G)#n");
		}
		else if (IS_INQUISITOR (wch))
		{
			strcpy (openb, "#w[#n");
			strcpy (closeb, "#w]#n");
		}

		else
		{
			strcpy (openb, "#b[#n");
			strcpy (closeb, "#b]#n");
		}
		{

			if (IS_GHOUL (wch))
				sprintf (kav, " %s#RGhoul#n%s", openb, closeb);
			else if (IS_VAMPIRE (wch) && wch->vampgen == 1)
				sprintf (kav, " %s#bMaster Vampire#n%s", openb, closeb);
			else if (IS_WEREWOLF (wch) && wch->vampgen == 1)
				sprintf (kav, " %s#cMaster Werewolf#n%s", openb, closeb);
			else if (IS_VAMPIRE (wch))
			{
				bool use_age = FALSE;
				char clanname[80];

				if (strlen (wch->clan) < 2)
					use_age = TRUE;
				else if (wch->vampgen == 2)
					sprintf (kav, " %s#bAntediluvian of %s#n%s", openb, wch->clan, closeb);
				else if (IS_EXTRA (wch, EXTRA_PRINCE))
					sprintf (kav, " %s#b%s Prince#n%s", openb, wch->clan, closeb);
				else
					use_age = TRUE;
				if (use_age)
				{
					if (strlen (wch->clan) < 2)
						strcpy (clanname, "Caitiff");
					else
						strcpy (clanname, wch->clan);
					switch (wch->vampgen)
					{
					default:
						sprintf (kav, " %s#e%s#n%s", openb, clanname, closeb);
						break;
					case 6:
						sprintf (kav, " %s#e%s#n%s", openb, clanname, closeb);
						break;
					case 5:
						sprintf (kav, " %s#e%s Elder#n%s", openb, clanname, closeb);
						break;
					case 4:
						sprintf (kav, " %s#b%s Methuselah#n%s", openb, clanname, closeb);
						break;
					case 3:
						sprintf (kav, " %s#b%s Antediluvian#n%s", openb, clanname, closeb);
						break;
					case 2:
						sprintf (kav, " %s#b%s Master Vampire#n%s", openb, clanname, closeb);
						break;
					case 1:
						sprintf (kav, " %s#b The First#n%s", openb, closeb);
						break;
					}
				}
			}
			else if (IS_WEREWOLF (wch))
			{
				if (strlen (wch->clan) > 1)
				{
					if (wch->vampgen == 2)
						sprintf (kav, " %s#c%s Chief#c%s", openb, wch->clan, closeb);
					else if (IS_EXTRA (wch, EXTRA_PRINCE))
						sprintf (kav, " %s#c%s Shaman#c%s", openb, wch->clan, closeb);
					else
						sprintf (kav, " %s#c%s#n%s", openb, wch->clan, closeb);
				}
				else
					sprintf (kav, " %s#cRonin#n%s", openb, closeb);
			}
			else if (IS_INQUISITOR (wch))
                        {
                                if (strlen (wch->clan) > 1)
                                {
                                        if (wch->vampgen == 2)
                                                sprintf (kav, " %s#C%s Chief#c%s", openb, wch->clan, closeb);
                                        else if (IS_EXTRA (wch, EXTRA_PRINCE))
                                                sprintf (kav, " %s#C%s Shaman#c%s", openb, wch->clan, closeb);
                                        else
                                                sprintf (kav, " %s#C%s#n%s", openb, wch->clan, closeb);
                                }
                                else
                                        sprintf (kav, " %s#C%s#n%s", openb, wch->clan, closeb);
                        }

			else
				sprintf (kav, "  %s#B%s#n%s", openb, wch->clan, closeb);
		}

		
		
		if (IS_SET (wch->extra2, EXTRA2_AFK))
		  strcpy (sex_text, "#c[#CAFK#c]#n ");
		else if (wch->level < 6 && wch->pcdata->pretitle[0] != '\0')
			strcpy(sex_text, wch->pcdata->pretitle);
		else if (wch->sex == SEX_FEMALE)
			strcpy (sex_text, "#pFemale#n");
		else if (wch->sex == SEX_MALE)
			strcpy (sex_text, "#bMale  #n");    
		else
			strcpy (sex_text, "#cNone  #n");

		if ((IS_SET (wch->act, PLR_INCOG)) && (IS_STORYTELLER(wch)))
			strcpy (invis_info, "#e[Incog]#n");
		else
			strcpy (invis_info, " ");
		if (IS_SET (wch->act, PLR_WIZINVIS) && IS_STORYTELLER(wch))
			strcpy (incog_info, "#e[Invis]#n");
		else
			strcpy (incog_info, " ");
		if (wch->desc->editor != ED_NONE)
			strcpy (builderflag, "#G(OLC)#n");
		else
			strcpy(builderflag, " ");

		if ((IS_MORE2 (wch, MORE2_RPFLAG)) && (IS_STORYTELLER(ch))) 
			strcpy (rpflag, "#c(#BIC#c)#n");
		else
			strcpy (rpflag, " ");

		if (IS_MORE2 (wch, MORE2_RPFLAG))
			wholistinchar++;

		//Construct title
		if ((!IS_IMMORTAL(wch) && !strstr(wch->pcdata->title, wch->name)))
			sprintf(title, "%s %s", wch->name, wch->pcdata->title);
		else
			sprintf(title, wch->pcdata->title);
			
		// Gobbo Who Change
		if (ch->level > 5)	//if character is an immortal
		{
			if (char_level > 5)	//watching another immortals stats
			{
				sprintf (buf_imm + strlen (buf_imm), "%-13s%s%s%s%s %s%s\n\r", class, title, kav, incog_info, invis_info, rpflag, builderflag);
			}
			else	// else watching a mortals stats
			{
				sprintf (buf_mor + strlen (buf_mor), " %-8s#b|#c %s%s %s\n\r", sex_text, title, kav, rpflag);
			}
		}
		else		//is NOT an immortal
		{
			if (char_level > 5)	//mortal looking at imm
			{
				sprintf (buf_imm + strlen (buf_imm), "%-13s%s%s%s %s\n\r", class, title, incog_info, invis_info, rpflag);
			}
			else	//mortal looking at mortal
			{
				sprintf (buf_mor + strlen (buf_mor), " %-8s#b|#c %s %s\n\r", sex_text, title, rpflag);
			}
		}
	}
	
	send_to_char ("\n\r#n              #g   __                      #e       _", ch);
	send_to_char ("\n\r#n              #g  ( #G_/_ _ _ _//'_ /_/ #w /|/|#e _   '(_ _ _ _/_", ch);
	send_to_char ("\n\r#b__________ #C-#c+#C- #g__)#G// (-(- /(/(//)/ #w /   |#e(//)/ / (-_) /() #C-#c+#C-#b _________", ch);
	send_to_char ("\n\r#G                            _/ ", ch);
	send_to_char ("\n\r#n #C-#c+#C-#w V#eisible#w S#etaff#C -#c+#C-#n \r\n", ch);
	
	send_to_char (buf_imm, ch);
	send_to_char ("\n\r#C -#c+#C-#w O#enline #wC#eharacters#C -#c+#C-#n \r\n", ch);
	send_to_char (buf_mor, ch);
	if (nTotal > most_login)
        most_login = nTotal;

	sprintf (buf, "\n\r#ePlayers#b:#w %d#n       #eMost this boot#b:#w %d       #eIn Character#b: #w%d#n\n\r", nTotal, most_login, wholistinchar );
	send_to_char (buf, ch);
	return;
}



void do_inventory (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *portal;
	OBJ_DATA *portal_next;
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	bool found;

	if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH))
	{
		if (!IS_NPC (ch) && (obj = ch->pcdata->chobj) == NULL)
		{
			send_to_char ("You are not a container.\n\r", ch);
			return;
		}
		switch (obj->item_type)
		{
		default:
			send_to_char ("You are not a container.\n\r", ch);
			break;

		case ITEM_PORTAL:
			pRoomIndex = get_room_index (obj->value[0]);
			location = ch->in_room;
			if (pRoomIndex == NULL)
			{
				send_to_char ("You don't seem to lead anywhere.\n\r", ch);
				return;
			}
			char_from_room (ch);
			char_to_room (ch, pRoomIndex);

			found = FALSE;
			for (portal = ch->in_room->contents; portal != NULL; portal = portal_next)
			{
				portal_next = portal->next_content;
				if ((obj->value[0] == portal->value[3]) && (obj->value[3] == portal->value[0]))
				{
					found = TRUE;
					if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_SET (portal->extra_flags, ITEM_SHADOWPLANE))
					{
						REMOVE_BIT (ch->affected_by, AFF_SHADOWPLANE);
						do_look (ch, "auto");
						SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_SET (portal->extra_flags, ITEM_SHADOWPLANE))
					{
						SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
						do_look (ch, "auto");
						REMOVE_BIT (ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else
					{
						do_look (ch, "auto");
						break;
					}
				}
			}
			char_from_room (ch);
			char_to_room (ch, location);
			break;

		case ITEM_DRINK_CON:
			if (obj->value[1] <= 0)
			{
				send_to_char ("You are empty.\n\r", ch);
				break;
			}
			if (obj->value[1] < obj->value[0] / 5)
				sprintf (buf, "There is a little %s liquid left in you.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0] / 4)
				sprintf (buf, "You contain a small about of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0] / 3)
				sprintf (buf, "You're about a third full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0] / 2)
				sprintf (buf, "You're about half full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] < obj->value[0])
				sprintf (buf, "You are almost full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else if (obj->value[1] == obj->value[0])
				sprintf (buf, "You're completely full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			else
				sprintf (buf, "Somehow you are MORE than full of %s liquid.\n\r", liq_table[obj->value[2]].liq_color);
			send_to_char (buf, ch);
			break;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
			act ("$p contain:", ch, obj, NULL, TO_CHAR);
			show_list_to_char (obj->contains, ch, TRUE, TRUE);
			break;
		}
		return;
	}
	send_to_char ("You are carrying:\n\r", ch);
	show_list_to_char (ch->carrying, ch, TRUE, TRUE);
	return;
}



void do_equipment (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	int iWear;
	bool found;

	send_to_char ("You are using:\n\r", ch);
	found = FALSE;
	for (iWear = 0; iWear < MAX_WEAR; iWear++)
	{
		if ((obj = get_eq_char (ch, iWear)) == NULL)
			continue;

		send_to_char (where_name[iWear], ch);
		if (can_see_obj (ch, obj))
		{
			send_to_char (format_obj_to_char (obj, ch, TRUE), ch);
			send_to_char ("\n\r", ch);
		}
		else
		{
			send_to_char ("something.\n\r", ch);
		}
		found = TRUE;
	}

	if (!found)
		send_to_char ("Nothing.\n\r", ch);

	return;
}






void do_credits (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "diku");
	return;
}



void set_title (CHAR_DATA * ch, char *title)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC (ch))
	{
		bug ("Set_title: NPC.", 0);
		return;
	}

	strcpy (buf, title);

	free_string (ch->pcdata->title);
	ch->pcdata->title = str_dup (buf);
	return;
}

void set_pretitle (CHAR_DATA * ch, char *pretitle)
{
	char buf[MAX_STRING_LENGTH];
	smash_tilde(pretitle);
	strcpy(buf, pretitle);
	free_string(ch->pcdata->pretitle);
	ch->pcdata->pretitle = str_dup (buf);
	return;
}



void do_title (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (argument[0] == '\0')
	{
		send_to_char ("Change your title to what?\n\r", ch);
		return;
	}

	if (IS_IMMORTAL(ch)){
	if (strlen (argument) > 75)
		argument[75] = '\0';
	}
	else
	{
	if (strlen(argument) > 62)
		argument[45] = '\0';
	}

	smash_tilde (argument);
	set_title (ch, argument);
	send_to_char ("Ok.\n\r", ch);
}

void do_pretitle (CHAR_DATA * ch, char *argument)
{	
	CHAR_DATA  * vch;
	char arg[MAX_INPUT_LENGTH];
	if (!IS_STORYTELLER(ch))
	{
		if (strlen_noansi(argument) > 8)
			send_to_char("Pretitle too long.", ch);
		else
		{
			set_pretitle(ch, argument);
			send_to_char("#eBooyah.#n\n\r", ch);
		} 
	}
	else
	{
		argument = one_argument(argument, arg);
		if (arg[0] == '\0')
			send_to_char("Syntax: pretitle <player> <pretitle>\n\r", ch);
		else if ((vch = get_char_world (ch, arg)) == NULL)
			send_to_char ("They aren't logged on.\n\r", ch);
		else if ((strlen_noansi(argument) > 13 && IS_IMMORTAL(vch)) 
			  || (strlen_noansi(argument) > 8 && !IS_IMMORTAL(vch)))
			send_to_char("Pretitle too long.\n\r", ch);
		else
		{
			set_pretitle(vch, argument);
			send_to_char("Ok.\n\r", ch);
			send_to_char("#eYour pretitle has been ganked. Booyah#n\n\r", vch);
		}
	}
	return;
}

void do_email (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (argument[0] == '\0')
	{
		send_to_char( "Yes, but what is it?\n\r", ch );
		return;
	}

	if (strlen (argument) > 50)
		argument[50] = '\0';

	smash_tilde (argument);
	free_string (ch->pcdata->email);
	ch->pcdata->email = str_dup (argument);
	send_to_char ("Ok.\n\r", ch);
}



void do_description (CHAR_DATA * ch, char *argument)
{

	if (!strcmp (argument, "edit"))
	{
		string_edit (ch, &ch->description);
	}
	else if (!strcmp (argument, "append"))
	{
		string_append (ch, &ch->description);
	}
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your description is:\n\r", ch);
		send_to_char (ch->description ? ch->description : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   description show\n\r", ch);
		send_to_char ("          description edit\n\r", ch);
		send_to_char ("          description append\n\r", ch);
	}
	return;
}




void desc_pretty (CHAR_DATA * ch, int start, int lines)
{
	char buf[MAX_STRING_LENGTH];
	char wordbuf[MAX_INPUT_LENGTH];
	char *p, *bp, *wp;
	int i, inword;
	/* find starting line to pretty-ify */
	for (i = 1, p = ch->description, bp = buf; *p != 0 && i < start; p++)
	{
		*bp++ = *p;
		if (*p == '\r')
			i++;
	}
	*bp = 0;
	/* now build pretty lines from raw ones */
	pretty_proc (bp, NULL);	/* init pretty processor */
	for (i = inword = 0, wp = wordbuf; i < lines && *p != 0; p++)
	{
		if (*p == ' ')
		{
			if (inword)
			{
				inword = 0;
				*wp = 0;
				pretty_proc (NULL, wordbuf);
				wp = wordbuf;
			}
			*wp++ = ' ';
		}
		else if (*p == '\r')
		{
			i++;	/* inc line count */
			if (inword)
			{
				inword = 0;
				*wp = 0;
				pretty_proc (NULL, wordbuf);
				wp = wordbuf;
				if (p[1] == '\n' || p[1] == ' ' || p[1] == 0)
					pretty_proc (NULL, "\n\r");
				else
					pretty_proc (NULL, " ");
			}
			else
			{
				pretty_proc (NULL, "\n\r");
				wp = wordbuf;
			}
		}
		else if (*p == '\n')
			continue;
		else
		{
			inword = 1;
			*wp++ = *p;
		}
	}
	/* and append any leftover lines directly */
	strcat (buf, p);
	/* and swap in the new editted description */
	free_string (ch->description);
	ch->description = str_dup (buf);
	send_to_char ("Ok.\n\r", ch);
}

void pretty_proc (char *buf, char *word)
{
	static char *pbuf;
	static int index;
	int i;
	/* special cue to do inits */
	if (word == NULL)
	{
		pbuf = buf;
		index = 0;
		return;
	}
	/* forced newline */
	if (!str_cmp ("\n\r", word))
	{
		/* strip trailing spaces */
		for (i = strlen (pbuf) - 1; i >= 0 && pbuf[i] == ' '; i--)
			pbuf[i] = 0;
		strcat (pbuf, word);
		index = 0;
		return;
	}
	/* see if it's a soft space */
	if (!str_cmp (" ", word))
	{
		if (index == 0)
			return;
	}
	/* add a word */
	if (strlen (word) > PLINE)
		word[PLINE] = 0;
	if (strlen (word) + index > PLINE)
	{
		/* strip trailing spaces */
		for (i = strlen (pbuf) - 1; i >= 0 && pbuf[i] == ' '; i--)
			pbuf[i] = 0;
		strcat (pbuf, "\n\r");
		index = 0;
		while (*word == ' ')
			word++;
	}
	strcat (pbuf, word);
	index += strlen (word);
}

void do_showsocial (CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if (arg[0] == '\0') {
		send_to_char("Do you want to turn social stat display on or off?\n\r", ch);
		return;
	}

	if (!strcmp(arg,"on")) {
		ch->pcdata->show_social = TRUE;
		send_to_char("You will now display social attributes\n\r",ch);
		return;
	}

	if (!strcmp(arg,"off")) {
		ch->pcdata->show_social = FALSE;
		send_to_char("You will no longer display social attributes\n\r",ch);
		return;
	}
	send_to_char("Do you want to turn social stat display on or off?\n\r",ch);
	return;
}

/* aura code */

void do_aura (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#cYour auras are currently: %s\n\r", ch->pcdata->aura);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#cYour auras have been cleared.\r\n", ch);
                free_string (ch->pcdata->aura);
                ch->pcdata->aura = NULL;
                return;
        }
        else if (strlen (argument) > MAX_AURA_LENGTH)
        {
                send_to_char ("#cLine too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->aura = str_dup (argument);
                sprintf (buf, "#cYour auras are now '%s'\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}

void do_voice (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#cYour voice is currently: %s\n\r", ch->pcdata->voice);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#cYour voice has been cleared.\r\n", ch);
                free_string (ch->pcdata->voice);
                ch->pcdata->voice = NULL;
                return;
        }
        else if (strlen (argument) > MAX_VOICE_LENGTH)
        {
                send_to_char ("Line too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->voice = str_dup (argument);
                sprintf (buf, "#cYour voice is now '%s'\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}


void do_roomdesc (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#cYour room description is currently: %s\n\r", ch->pcdata->roomdesc);
                send_to_char(buf, ch);
                return;
        }
        /*
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#cYour room description has been cleared.\r\n", ch);
                free_string (ch->pcdata->roomdesc);
                ch->pcdata->roomdesc = NULL;
                return;
        }
        */
        else if (strlen (argument) > MAX_ROOMDESC_LENGTH)
        {
                send_to_char ("Line too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->roomdesc = str_dup (argument);
                sprintf (buf, "#cYour roomdesc is now '%s'\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}


void do_doing (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC (ch))
		return;

	if (argument[0] == '\0')
	{
		sprintf(buf, "#cYour doing is currently: %s\n\r", ch->pcdata->doing);
		send_to_char(buf, ch);
		return;
	}
	else if (!strcmp(argument,"clear"))
	{
		send_to_char ("#cYour doing has been cleared.\r\n", ch);
                free_string (ch->pcdata->doing);
                ch->pcdata->doing = NULL;
		return;
	}
	else
	{
		ch->pcdata->doing = str_dup (argument);
		sprintf (buf, "#cYou doing is now '%s'\r\n", argument);
		send_to_char (buf, ch);
		return;
	}
}




void do_socials (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	int iSocial;
	int col;

	col = 0;

	for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
	{
		sprintf (buf, "%-12s", social_table[iSocial].name);
		send_to_char (buf, ch);
		if (++col % 6 == 0)
			send_to_char ("\n\r", ch);
	}

	if (col % 6 != 0)
		send_to_char ("\n\r", ch);

	return;
}


void do_channels (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		if (!IS_NPC (ch) && IS_SET (ch->act, PLR_SILENCE))
		{
			send_to_char ("You are silenced.\n\r", ch);
			return;
		}

		
		send_to_char ("#CChannels#B:#                                                 #n\n\r", ch);
	
		send_to_char (!IS_SET (ch->deaf, CHANNEL_OOC) ? "#s +OOC" : "#s -ooc", ch);
		if (IS_WEREWOLF (ch))
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_HOWL) ? "#s +HOWL" : "#s -howl", ch);
		}

		send_to_char (!IS_SET (ch->deaf, CHANNEL_OMOTE) ? "#s +OMOTE" : "#s -omote", ch);		

		send_to_char (!IS_SET (ch->deaf, CHANNEL_OSAY) ? "#s +OSAY" : "#s -osay", ch);

		send_to_char (!IS_SET (ch->deaf, CHANNEL_TELL) ? "#s +TELL" : "#s -tell", ch);

		send_to_char (!IS_SET (ch->deaf, CHANNEL_INFO) ? "#s +INFO" : "#s -info", ch);

		send_to_char(!IS_SET  (ch->deaf, CHANNEL_CELL) ? "#s +CELL"  : "#s -cell", ch); 
		
		send_to_char(!IS_SET  (ch->deaf, CHANNEL_SHOUT) ? "#s +RADIO" : "#s -radio", ch);

		if (IS_IMMORTAL (ch))
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_STORYINFORM) ? "#w +STORYINFORM" : "#w -storyinform", ch);
		}

		if (IS_STORYTELLER (ch))
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_STORY) ? "#B +STORY" : "#B -story", ch);
		}

		if (get_trust (ch) >= LEVEL_BUILDER)
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_IMMTALK) ? "#P +IMMTALK" : "#P -immtalk", ch);
		}

		if (get_trust (ch) >= LEVEL_BUILDER)
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG) ? "#G +LOG" : "#G -log", ch);
		}


		if(IS_STORYTELLER(ch))
		{
			send_to_char ("\n\r", ch);
			send_to_char ("#R-------------------------------------------------------------#n\n\r", ch);
			send_to_char ("#R| #nLogs:#R                                                     |#n\n\r", ch);
			send_to_char ("#R-------------------------------------------------------------#n\n\r", ch);
			send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG_STORYTELLER) ? " +LOG_STORYTELLER" : " -log_storyteller", ch);
			if (ch->level >= LEVEL_BUILDER )
			{
				send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG_DICE) ? " +LOG_DICE" : " -log_dice", ch);
				send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG_NORMAL) ? " +LOG_NORMAL" : " -log_normal", ch);
				send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG_CODER) ? " +LOG_CODER" : " -log_coder", ch);
				send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG_CONNECT) ? " +LOG_CONNECT" : " -log_connect", ch);
				send_to_char (!IS_SET (ch->deaf, CHANNEL_LOG_PLAYER) ? " +LOG_PLAYER" : " -log_player", ch);
			}
		}


		send_to_char ("\n\r\n\r#BNote#C:#B -all or +all will set or clear all your channels", ch);

		send_to_char (".\n\r", ch);
	}
	else
	{
		bool fClear;
		int bit;

		if (arg[0] == '+')
			fClear = TRUE;
		else if (arg[0] == '-')
			fClear = FALSE;
		else
		{
			send_to_char ("Channels -channel or +channel?\n\r", ch);
			return;
		}
		if (!str_cmp (arg + 1, "ooc"))
			bit = CHANNEL_OOC;
		else if (IS_IMMORTAL (ch) && !str_cmp (arg, "-trans"))
		{
		   SET_BIT(ch->act, PLR_NOTRANS);
		   send_to_char( "Done.\n\r", ch );
		   return;
		}
		else if (IS_IMMORTAL (ch) && !str_cmp (arg, "+trans"))
		{
		   REMOVE_BIT(ch->act, PLR_NOTRANS);
	           send_to_char( "Removed.\n\r", ch);
		   return;
		}
		else if (IS_IMMORTAL (ch) && !str_cmp (arg + 1, "storyinform"))
			bit = CHANNEL_STORYINFORM;
		else if (IS_STORYTELLER (ch) && !str_cmp (arg + 1, "story"))
			bit = CHANNEL_STORY;
		else if (!str_cmp (arg + 1, "osay"))
			bit = CHANNEL_OSAY;
 		 else if (!str_cmp (arg + 1, "omote"))
            bit = CHANNEL_OMOTE;
		else if (IS_IMMORTAL (ch) && !str_cmp (arg + 1, "immtalk"))
			bit = CHANNEL_IMMTALK;
		else if (!str_cmp (arg + 1, "howl"))
			bit = CHANNEL_HOWL;
		else if (IS_BUILDER(ch) && !str_cmp (arg + 1, "log_dice"))
			bit = CHANNEL_LOG_DICE;
		else if (IS_BUILDER(ch) && !str_cmp (arg + 1, "log"))
			bit = CHANNEL_LOG;
		else if (IS_BUILDER (ch) && !str_cmp (arg + 1, "log_normal"))
			bit = CHANNEL_LOG_NORMAL;
		else if (IS_BUILDER(ch) && !str_cmp (arg + 1, "log_coder"))
			bit = CHANNEL_LOG_CODER;
		else if (IS_BUILDER(ch) && !str_cmp (arg + 1, "log_connect"))
			bit = CHANNEL_LOG_CONNECT;
		else if (IS_IMP(ch) && !str_cmp (arg + 1, "log_player"))
			bit = CHANNEL_LOG_PLAYER;
		else if (IS_STORYTELLER(ch) && !str_cmp (arg + 1, "log_storyteller"))
			bit = CHANNEL_LOG_STORYTELLER;
		else if ((IS_SET (ch->act, PLR_CHAMPION) || IS_STORYTELLER (ch)) && !str_cmp (arg + 1, "pray"))
			bit = CHANNEL_PRAY;
		else if (!str_cmp (arg + 1, "info"))
			bit = CHANNEL_INFO;
		else if (!str_cmp (arg + 1, "tell"))
			bit = CHANNEL_TELL;
		else if (!str_cmp (arg + 1, "radio"))
			bit = CHANNEL_SHOUT;
		else if (!str_cmp (arg + 1, "cell"))
		{
			if (fClear)
				do_cell(ch, "off");
			else 
				do_cell(ch, "on");
			return;
		}
		else if (!str_cmp (arg + 1, "all"))
		{
			if (fClear)
			{
				REMOVE_BIT (ch->deaf, CHANNEL_OOC);
				REMOVE_BIT (ch->deaf, CHANNEL_OMOTE);
				REMOVE_BIT (ch->deaf, CHANNEL_OSAY);
				REMOVE_BIT (ch->deaf, CHANNEL_INFO);
				REMOVE_BIT (ch->deaf, CHANNEL_TELL);
				REMOVE_BIT (ch->deaf, CHANNEL_CELL);
				REMOVE_BIT (ch->deaf, CHANNEL_SHOUT);
				if (IS_WEREWOLF (ch) || IS_STORYTELLER (ch))
					REMOVE_BIT (ch->deaf, CHANNEL_HOWL);
				if (IS_IMMORTAL (ch))
				{
					REMOVE_BIT (ch->deaf, CHANNEL_STORYINFORM);
					REMOVE_BIT (ch->deaf, CHANNEL_IMMTALK);
				}
				if (IS_STORYTELLER (ch))
				{
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_DICE);
					REMOVE_BIT (ch->deaf, CHANNEL_STORY);
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_STORYTELLER);
				}
				if (IS_IMP(ch))
				{
					REMOVE_BIT (ch->deaf, CHANNEL_LOG);
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_NORMAL);
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_CODER);
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_CONNECT);
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_PLAYER);
				}
			}
			else
			{
				SET_BIT (ch->deaf, CHANNEL_OOC);
				SET_BIT (ch->deaf, CHANNEL_OMOTE);
				SET_BIT (ch->deaf, CHANNEL_OSAY);
				SET_BIT (ch->deaf, CHANNEL_INFO);
				SET_BIT (ch->deaf, CHANNEL_TELL);
				SET_BIT (ch->deaf, CHANNEL_CELL);
				SET_BIT (ch->deaf, CHANNEL_SHOUT);
				if (IS_WEREWOLF (ch) || IS_STORYTELLER (ch))
					SET_BIT (ch->deaf, CHANNEL_HOWL);
				if (IS_IMMORTAL (ch))
				{
					SET_BIT (ch->deaf, CHANNEL_STORYINFORM);
					SET_BIT (ch->deaf, CHANNEL_IMMTALK);
				}
				if (IS_STORYTELLER (ch))
				{
					SET_BIT (ch->deaf, CHANNEL_STORY);
					SET_BIT (ch->deaf, CHANNEL_LOG_STORYTELLER);
				}
				if (IS_BUILDER(ch))
				{
					SET_BIT (ch->deaf, CHANNEL_LOG_DICE);
				}
				if (IS_IMP(ch))
				{
					SET_BIT (ch->deaf, CHANNEL_LOG);
					SET_BIT (ch->deaf, CHANNEL_LOG_NORMAL);
					SET_BIT (ch->deaf, CHANNEL_LOG_CODER);
					SET_BIT (ch->deaf, CHANNEL_LOG_CONNECT);
					SET_BIT (ch->deaf, CHANNEL_LOG_PLAYER);
				}
			}

			send_to_char ("#cOk.\n\r", ch);
			return;
		}
		else
		{
			send_to_char ("#cSet or clear which channel?\n\r", ch);
			return;
		}

		if (fClear)
			REMOVE_BIT (ch->deaf, bit);
		else
			SET_BIT (ch->deaf, bit);

		send_to_char ("#cOk.\n\r", ch);
	}

	return;
}



/*
 * Contributed by Grodyn.
 */
void do_config (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	if (IS_NPC (ch))
		return;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("[ Keyword  ] Option\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_ANSI) ? "[+ANSI     ] You have ansi colour on.\n\r" : "[-ansi     ] You have ansi colour off.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_AUTOEXIT) ? "[+AUTOEXIT ] You automatically see exits.\n\r" : "[-autoexit ] You don't automatically see exits.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_BLANK) ? "[+BLANK    ] You have a blank line before your prompt.\n\r" : "[-blank    ] You have no blank line before your prompt.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_BRIEF) ? "[+BRIEF    ] You see brief descriptions.\n\r" : "[-brief    ] You see long descriptions.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_COMBINE) ? "[+COMBINE  ] You see object lists in combined format.\n\r" : "[-combine  ] You see object lists in single format.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_PROMPT) ? "[+PROMPT   ] You have a prompt.\n\r" : "[-prompt   ] You don't have a prompt.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_TELNET_GA) ? "[+TELNETGA ] You receive a telnet GA sequence.\n\r" : "[-telnetga ] You don't receive a telnet GA sequence.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_SILENCE) ? "[+SILENCE  ] You are silenced.\n\r" : "", ch);

		send_to_char (!IS_SET (ch->act, PLR_NO_EMOTE) ? "" : "[-emote    ] You can't emote.\n\r", ch);

		send_to_char (!IS_SET (ch->act, PLR_NO_TELL) ? "" : "[-tell     ] You can't use 'tell'.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_EXTRAS) ? "[+EXTRA    ] You see extra fields under score.\n\r" : "[-extra    ] You can't see extra fields.\n\r", ch);

	}
	else
	{
		bool fSet;
		int bit;

		if (arg[0] == '+')
			fSet = TRUE;
		else if (arg[0] == '-')
			fSet = FALSE;
		else
		{
			send_to_char ("Config -option or +option?\n\r", ch);
			return;
		}

		if (!str_cmp (arg + 1, "ansi"))
			bit = PLR_ANSI;
		else if (!str_cmp (arg + 1, "autoexit"))
			bit = PLR_AUTOEXIT;
		else if (!str_cmp (arg + 1, "blank"))
			bit = PLR_BLANK;
		else if (!str_cmp (arg + 1, "brief"))
			bit = PLR_BRIEF;
		else if (!str_cmp (arg + 1, "combine"))
			bit = PLR_COMBINE;
		else if (!str_cmp (arg + 1, "prompt"))
			bit = PLR_PROMPT;
		else if (!str_cmp (arg + 1, "telnetga"))
			bit = PLR_TELNET_GA;
		else if (!str_cmp (arg + 1, "extra"))
			bit = PLR_EXTRAS;
		else
		{
			send_to_char ("Config which option?\n\r", ch);
			return;
		}

		if (fSet)
			SET_BIT (ch->act, bit);
		else
			REMOVE_BIT (ch->act, bit);

		send_to_char ("Ok.\n\r", ch);
	}

	return;
}

void do_ansi (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_ANSI))
		do_config (ch, "-ansi");
	else
		do_config (ch, "+ansi");
	return;
}

void do_autoexit (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_AUTOEXIT))
		do_config (ch, "-autoexit");
	else
		do_config (ch, "+autoexit");
	return;
}



void do_blank (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_BLANK))
		do_config (ch, "-blank");
	else
		do_config (ch, "+blank");
	return;
}


void do_brief (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_BRIEF))
		do_config (ch, "-brief");
	else
		do_config (ch, "+brief");
	return;
}



void obj_score (CHAR_DATA * ch, OBJ_DATA * obj)
{
	char buf[MAX_STRING_LENGTH];
	AFFECT_DATA *paf;
	int itemtype;

	sprintf (buf, "You are %s.\n\r", obj->short_descr);
	send_to_char (buf, ch);

	sprintf (buf, "Type %s, Extra flags %s.\n\r", item_type_name (obj->item_type), extra_bit_name (obj->extra_flags));
	send_to_char (buf, ch);

	sprintf (buf, "You weigh %d pounds and are worth %d gold coins.\n\r", obj->weight, obj->cost);
	send_to_char (buf, ch);

	if (obj->questmaker != NULL && strlen (obj->questmaker) > 1 && obj->questowner != NULL && strlen (obj->questowner) > 1)
	{
		sprintf (buf, "You were created by %s, and are owned by %s.\n\r", obj->questmaker, obj->questowner);
		send_to_char (buf, ch);
	}
	else if (obj->questmaker != NULL && strlen (obj->questmaker) > 1)
	{
		sprintf (buf, "You were created by %s.\n\r", obj->questmaker);
		send_to_char (buf, ch);
	}
	else if (obj->questowner != NULL && strlen (obj->questowner) > 1)
	{
		sprintf (buf, "You are owned by %s.\n\r", obj->questowner);
		send_to_char (buf, ch);
	}

	switch (obj->item_type)
	{
	case ITEM_SCROLL:
	case ITEM_POTION:
		sprintf (buf, "You contain level %d spells of:", obj->value[0]);
		send_to_char (buf, ch);

		if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL)
		{
			send_to_char (" '", ch);
			send_to_char (skill_table[obj->value[1]].name, ch);
			send_to_char ("'", ch);
		}

		if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL)
		{
			send_to_char (" '", ch);
			send_to_char (skill_table[obj->value[2]].name, ch);
			send_to_char ("'", ch);
		}

		if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
		{
			send_to_char (" '", ch);
			send_to_char (skill_table[obj->value[3]].name, ch);
			send_to_char ("'", ch);
		}

		send_to_char (".\n\r", ch);
		break;

	case ITEM_QUEST:
		sprintf (buf, "Your quest point value is %d.\n\r", obj->value[0]);
		send_to_char (buf, ch);
		break;

	case ITEM_WAND:
	case ITEM_STAFF:
		sprintf (buf, "You have %d(%d) charges of level %d", obj->value[1], obj->value[2], obj->value[0]);
		send_to_char (buf, ch);

		if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
		{
			send_to_char (" '", ch);
			send_to_char (skill_table[obj->value[3]].name, ch);
			send_to_char ("'", ch);
		}
		send_to_char (".\n\r", ch);
		break;

	case ITEM_WEAPON:
		sprintf (buf, "You inflict %d to %d damage in combat (average %d).\n\r", obj->value[1], obj->value[2], (obj->value[1] + obj->value[2]) / 2);
		send_to_char (buf, ch);

		if (obj->value[0] >= 1000)
			itemtype = obj->value[0] - ((obj->value[0] / 1000) * 1000);
		else
			itemtype = obj->value[0];

		if (itemtype > 0)
		{
			if (obj->level < 10)
				sprintf (buf, "You are a minor spell weapon.\n\r");
			else if (obj->level < 20)
				sprintf (buf, "You are a lesser spell weapon.\n\r");
			else if (obj->level < 30)
				sprintf (buf, "You are an average spell weapon.\n\r");
			else if (obj->level < 40)
				sprintf (buf, "You are a greater spell weapon.\n\r");
			else if (obj->level < 50)
				sprintf (buf, "You are a major spell weapon.\n\r");
			else
				sprintf (buf, "You are a supreme spell weapon.\n\r");
			send_to_char (buf, ch);
		}

		if (itemtype == 1)
			sprintf (buf, "You are dripping with corrosive acid.\n\r");
		else if (itemtype == 4)
			sprintf (buf, "You radiate an aura of darkness.\n\r");
		else if (itemtype == 30)
			sprintf (buf, "You are the bane of all evil.\n\r");
		else if (itemtype == 34)
			sprintf (buf, "You drink the souls of your victims.\n\r");
		else if (itemtype == 37)
			sprintf (buf, "You have been tempered in hellfire.\n\r");
		else if (itemtype == 48)
			sprintf (buf, "You crackle with sparks of lightning.\n\r");
		else if (itemtype == 53)
			sprintf (buf, "You are dripping with a dark poison.\n\r");
		else if (itemtype > 0)
			sprintf (buf, "You have been imbued with the power of %s.\n\r", skill_table[itemtype].name);
		if (itemtype > 0)
			send_to_char (buf, ch);

		if (obj->value[0] >= 1000)
			itemtype = obj->value[0] / 1000;
		else
			break;

		if (itemtype == 4 || itemtype == 1)
			sprintf (buf, "You radiate an aura of darkness.\n\r");
		else if (itemtype == 27 || itemtype == 2)
			sprintf (buf, "You allow your wielder to see invisible things.\n\r");
		else if (itemtype == 39 || itemtype == 3)
			sprintf (buf, "You grant your wielder the power of flight.\n\r");
		else if (itemtype == 45 || itemtype == 4)
			sprintf (buf, "You allow your wielder to see in the dark.\n\r");
		else if (itemtype == 46 || itemtype == 5)
			sprintf (buf, "You render your wielder invisible to the human eye.\n\r");
		else if (itemtype == 52 || itemtype == 6)
			sprintf (buf, "You allow your wielder to walk through solid doors.\n\r");
		else if (itemtype == 54 || itemtype == 7)
			sprintf (buf, "You protect your wielder from evil.\n\r");
		else if (itemtype == 57 || itemtype == 8)
			sprintf (buf, "You protect your wielder in combat.\n\r");
		else if (itemtype == 9)
			sprintf (buf, "You allow your wielder to walk in complete silence.\n\r");
		else if (itemtype == 10)
			sprintf (buf, "You surround your wielder with a shield of lightning.\n\r");
		else if (itemtype == 11)
			sprintf (buf, "You surround your wielder with a shield of fire.\n\r");
		else if (itemtype == 12)
			sprintf (buf, "You surround your wielder with a shield of ice.\n\r");
		else if (itemtype == 13)
			sprintf (buf, "You surround your wielder with a shield of acid.\n\r");
		else if (itemtype == 14)
			sprintf (buf, "You protect your wielder from attacks from DarkBlade clan guardians.\n\r");
		else if (itemtype == 15)
			sprintf (buf, "You surround your wielder with a shield of chaos.\n\r");
		else if (itemtype == 16)
			sprintf (buf, "You regenerate the wounds of your wielder.\n\r");
		else if (itemtype == 17)
			sprintf (buf, "You enable your wielder to move at supernatural speed.\n\r");
		else if (itemtype == 18)
			sprintf (buf, "You can slice through armour without difficulty.\n\r");
		else if (itemtype == 19)
			sprintf (buf, "You protect your wielder from player attacks.\n\r");
		else if (itemtype == 20)
			sprintf (buf, "You surround your wielder with a shield of darkness.\n\r");
		else if (itemtype == 21)
			sprintf (buf, "You grant your wielder superior protection.\n\r");
		else if (itemtype == 22)
			sprintf (buf, "You grant your wielder supernatural vision.\n\r");
		else if (itemtype == 23)
			sprintf (buf, "You make your wielder fleet-footed.\n\r");
		else if (itemtype == 24)
			sprintf (buf, "You conceal your wielder from sight.\n\r");
		else if (itemtype == 25)
			sprintf (buf, "You invoke the power of your wielders beast.\n\r");
		else
			sprintf (buf, "You are bugged...please report it.\n\r");
		if (itemtype > 0)
			send_to_char (buf, ch);
		break;

	case ITEM_ARMOR:
		sprintf (buf, "Your armor class is %d.\n\r", obj->value[0]);
		send_to_char (buf, ch);
		if (obj->value[3] < 1)
			break;
		if (obj->value[3] == 4 || obj->value[3] == 1)
			sprintf (buf, "You radiate an aura of darkness.\n\r");
		else if (obj->value[3] == 27 || obj->value[3] == 2)
			sprintf (buf, "You allow your wearer to see invisible things.\n\r");
		else if (obj->value[3] == 39 || obj->value[3] == 3)
			sprintf (buf, "You grant your wearer the power of flight.\n\r");
		else if (obj->value[3] == 45 || obj->value[3] == 4)
			sprintf (buf, "You allow your wearer to see in the dark.\n\r");
		else if (obj->value[3] == 46 || obj->value[3] == 5)
			sprintf (buf, "You render your wearer invisible to the human eye.\n\r");
		else if (obj->value[3] == 52 || obj->value[3] == 6)
			sprintf (buf, "You allow your wearer to walk through solid doors.\n\r");
		else if (obj->value[3] == 54 || obj->value[3] == 7)
			sprintf (buf, "You protect your wearer from evil.\n\r");
		else if (obj->value[3] == 57 || obj->value[3] == 8)
			sprintf (buf, "You protect your wearer in combat.\n\r");
		else if (obj->value[3] == 9)
			sprintf (buf, "You allow your wearer to walk in complete silence.\n\r");
		else if (obj->value[3] == 10)
			sprintf (buf, "You surround your wearer with a shield of lightning.\n\r");
		else if (obj->value[3] == 11)
			sprintf (buf, "You surround your wearer with a shield of fire.\n\r");
		else if (obj->value[3] == 12)
			sprintf (buf, "You surround your wearer with a shield of ice.\n\r");
		else if (obj->value[3] == 13)
			sprintf (buf, "You surround your wearer with a shield of acid.\n\r");
		else if (obj->value[3] == 14)
			sprintf (buf, "You protect your wearer from attacks from DarkBlade clan guardians.\n\r");
		else if (obj->value[3] == 15)
			sprintf (buf, "You surround your wielder with a shield of chaos.\n\r");
		else if (obj->value[3] == 16)
			sprintf (buf, "You regenerate the wounds of your wielder.\n\r");
		else if (obj->value[3] == 17)
			sprintf (buf, "You enable your wearer to move at supernatural speed.\n\r");
		else if (obj->value[3] == 18)
			sprintf (buf, "You can slice through armour without difficulty.\n\r");
		else if (obj->value[3] == 19)
			sprintf (buf, "You protect your wearer from player attacks.\n\r");
		else if (obj->value[3] == 20)
			sprintf (buf, "You surround your wearer with a shield of darkness.\n\r");
		else if (obj->value[3] == 21)
			sprintf (buf, "You grant your wearer superior protection.\n\r");
		else if (obj->value[3] == 22)
			sprintf (buf, "You grant your wearer supernatural vision.\n\r");
		else if (obj->value[3] == 23)
			sprintf (buf, "You make your wearer fleet-footed.\n\r");
		else if (obj->value[3] == 24)
			sprintf (buf, "You conceal your wearer from sight.\n\r");
		else if (obj->value[3] == 25)
			sprintf (buf, "You invoke the power of your wearers beast.\n\r");
		else
			sprintf (buf, "You are bugged...please report it.\n\r");
		if (obj->value[3] > 0)
			send_to_char (buf, ch);
		break;
	}

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	{
		if (paf->location != APPLY_NONE && paf->modifier != 0)
		{
			sprintf (buf, "You %s %d.\n\r", affect_loc_name (paf->location), paf->modifier);
			send_to_char (buf, ch);
		}
	}

	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		if (paf->location != APPLY_NONE && paf->modifier != 0)
		{
			sprintf (buf, "You %s %d.\n\r", affect_loc_name (paf->location), paf->modifier);
			send_to_char (buf, ch);
		}
	}
	return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_prompt (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';
	if (IS_NPC (ch))
		return;

	if (argument[0] == '\0')
	{
		do_help (ch, "prompt");
		return;
	}

	if (!strcmp (argument, "on"))
	{
		if (IS_EXTRA (ch, EXTRA_PROMPT))
			send_to_char ("But you already have customised prompt on!\n\r", ch);
		else
		{
			send_to_char ("Ok.\n\r", ch);
			SET_BIT (ch->extra, EXTRA_PROMPT);
		}
		return;
	}
	else if (!strcmp (argument, "off"))
	{
		if (!IS_EXTRA (ch, EXTRA_PROMPT))
			send_to_char ("But you already have customised prompt off!\n\r", ch);
		else
		{
			send_to_char ("Ok.\n\r", ch);
			REMOVE_BIT (ch->extra, EXTRA_PROMPT);
		}
		return;
	}
	else if (!strcmp (argument, "clear"))
	{
		free_string (ch->prompt);
		ch->prompt = str_dup ("");
		return;
	}
	else
	{
		if (strlen (argument) > 50)
			argument[50] = '\0';
		smash_tilde (argument);
		strcat (buf, argument);
	}

	free_string (ch->prompt);
	ch->prompt = str_dup (buf);
	send_to_char ("Ok.\n\r", ch);
	return;
}

/* Do_prompt from Morgenes from Aldara Mud */
void do_cprompt (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';
	if (IS_NPC (ch))
		return;

	if (argument[0] == '\0')
	{
		do_help (ch, "cprompt");
		return;
	}

	if (!strcmp (argument, "clear"))
	{
		free_string (ch->cprompt);
		ch->cprompt = str_dup ("");
		return;
	}
	else
	{
		if (strlen (argument) > 50)
			argument[50] = '\0';
		smash_tilde (argument);
		strcat (buf, argument);
	}

	free_string (ch->cprompt);
	ch->cprompt = str_dup (buf);
	send_to_char ("Ok.\n\r", ch);
	return;
}

void do_peek (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (arg[0] == '\0')
	{
		send_to_char ("Peek at whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They are not here.\n\r", ch);
		return;
	}

	if (number_percent () <= ch->pcdata->learned[gsn_peek])
	{
		send_to_char ("You peek at their inventory:\n\r", ch);
		show_list_to_char (victim->carrying, ch, TRUE, TRUE);
	}
	else
	{
		send_to_char ("You peek at their inventory:\n\r", ch);
		send_to_char ("     Nothing.\n\r", ch);
	}
	WAIT_STATE (ch, 12);
	return;
}


void do_afk (CHAR_DATA * ch, char *argument)
{

	char buf[MAX_STRING_LENGTH];

	if (IS_NPC (ch))
		return;

	if (IS_SET (ch->extra2, EXTRA2_AFK))
	{
		send_to_char ("#cWelcome back#n.\n\r", ch);
		//if (strlen(ch->pcdata->roomdesc) > 1)
			sprintf (buf, "#C%s#e is no longer AFK.#n\n\r", ch->name);
		//else
			//sprintf(buf, "#CSome new person#e is no longer AFK.#n\n\r");
		if (!IS_SET(ch->act, PLR_WIZINVIS))
			do_info (ch, buf);
		REMOVE_BIT (ch->extra2, EXTRA2_AFK);
		return;
	}
	if (IS_MORE2 (ch, MORE2_RPFLAG))
	do_rpflag(ch, argument);

	if (!IS_SET (ch->extra2, EXTRA2_AFK))
	{
		send_to_char ("#cYou're AFK. Come back soon! \n\r", ch);
		//if (strlen(ch->pcdata->roomdesc) > 1)
			sprintf (buf, "#C%s#e is now AFK.", ch->name);
		//else
			//sprintf(buf, "#CSome new person#e is now AFK.");
		if (!IS_SET(ch->act, PLR_WIZINVIS))
			do_info (ch, buf);
		SET_BIT (ch->extra2, EXTRA2_AFK);
		return;
	}

	return;
}


void do_road (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int i = 0;
	int road = -1;

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (ch->pcdata->road != -1)
	{
		sprintf (buf, "You follow the road of %s.\n\r", road_names[ch->pcdata->road][0]);
		send_to_char (buf, ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose a road:\n\r", ch);
		strcpy (buf, road_names[0][0]);
		for (i = 1; i < MAX_ROADS; i++)
		{
			strcat (buf, " ");
			strcat (buf, road_names[i][0]);
		}
		strcat (buf, "\n\r");
		send_to_char (buf, ch);
		return;
	}

	for (i = 0; i < MAX_ROADS; i++)
	{
		if (!str_cmp (arg, road_names[i][0]))
		{
			road = i;
			break;
		}
	}

	if (road == -1)
	{
		send_to_char ("Unknown Road, Try Again\n\r", ch);
		return;
	}

	if (road == ROAD_BEAST)
	{
		if (!str_cmp (ch->clan, "toreador"))
		{
			send_to_char ("Your clan cannot choose Via Bestiae.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Bestiae.\n\r", ch);
	}

	if (road == ROAD_BLOOD && !str_cmp (ch->clan, "Assamite"))
	{
		send_to_char ("You now seek Golconda Via Sanguinius.\n\r", ch);
	}
	else if (road == ROAD_BLOOD)
	{
		send_to_char ("Only Assamite can follow Via Sanguinius.\n\r", ch);
		return;
	}

	if (road == ROAD_BONES && ( !str_cmp (ch->clan, "Cappadocian") ||
		!str_cmp (ch->clan, "Tzimisce") ) )
	{
		send_to_char ("You now seek Golconda Via Ossium.\n\r", ch);
	}
	else if (road == ROAD_BONES)
	{
		send_to_char ("Only Cappadocians can follow Via Ossium.\n\r", ch);
		return;
	}
	if (road == ROAD_NIGHT)
	{
		send_to_char ("You now seek Golconda Via Noctis.\n\r", ch);
	}
	if (road == ROAD_METAMORPHOSIS && ( !str_cmp (ch->clan, "Tzimisce") ) )
	{
		send_to_char ("You now seek Golconda Via Mutationis.\n\r", ch);
	}
	else if (road == ROAD_METAMORPHOSIS)
	{
		send_to_char ("Only Tzimisce can follow Via Mutationis.\n\r", ch);
		return;
	}

	if (road == ROAD_KING)
	{
		send_to_char ("You now seek Golconda Via Regalis.\n\r", ch);
	}

	if (road == ROAD_SLAIN)
	{
		send_to_char( "You now seek Goldconda Via Einherjar.\n\r",ch);
	}
	if (road == ROAD_SIN)
	{
		if (!str_cmp (ch->clan, "salubri"))
		{
			send_to_char ("Your clan cannot choose Via Peccati.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Peccati.\n\r", ch);
	}
	if (road == ROAD_HEAVEN)
	{
		if (!str_cmp (ch->clan, "baali"))
		{
			send_to_char ("Your clan cannot choose Via Caeli.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Caeli.\n\r", ch);
	}
	if (road == ROAD_HUMANITY)
	{
		if (!str_cmp (ch->clan, "baali"))
		{
			send_to_char ("Your clan cannot choose Via Humanitatis.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Humanitatis.\n\r", ch);
	}
	if (road == ROAD_PARADOX && !str_cmp (ch->clan, "Ravnos"))
	{
		send_to_char ("You now seek Golconda Via Paradocis.\n\r", ch);
	}
	else if (road == ROAD_PARADOX)
	{
		send_to_char ("Only Ravnos can follow Via Paradocis.\n\r", ch);
		return;
	}

	if (road == ROAD_SERPENT)
	{
		send_to_char ("You now seek Golconda Via Serpentis.\n\r", ch);
	}


	ch->pcdata->road = road;
	ch->pcdata->roadt = 1;
	sprintf (buf, "You now follow the Road of %s!", road_names[road][0]);
	send_to_char (buf, ch);
	return;
}



void do_lupusdesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_SHIFTER(ch))
	{
		send_to_char ("Only shifters may use this", ch);
		return;
	}

	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->lupusdesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->lupusdesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your lupus description is:\n\r", ch);
		send_to_char (ch->pcdata->lupusdesc ? ch->pcdata->lupusdesc : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   description show\n\r", ch);
		send_to_char ("          description edit\n\r", ch);
		send_to_char ("          description append\n\r", ch);
	}
	return;
}

void do_tempdesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	
	if (!strcmp (argument, "on"))
	{
		send_to_char ("You will now show your temporary description.\n\r",ch);
		ch->pcdata->showtempdesc = TRUE;
	}
	else if (!strcmp (argument, "off"))
	{
		send_to_char ("You will no longer show your temporary description.\n\r",ch);
		ch->pcdata->showtempdesc = FALSE;
	}
	else if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->tempdesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->tempdesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your crinos description is:\n\r", ch);
		send_to_char (ch->pcdata->tempdesc ? ch->pcdata->tempdesc : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   description show\n\r", ch);
		send_to_char ("          description edit\n\r", ch);
		send_to_char ("          description append\n\r", ch);
		send_to_char ("          description on\n\r", ch);
		send_to_char ("          description off\n\r", ch);
	}
	return;
}

void do_crinosdesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_SHIFTER(ch))
	{
		send_to_char ("Only shifters may use this", ch);
		return;
	}

	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->crinosdesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->crinosdesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your crinos description is:\n\r", ch);
		send_to_char (ch->pcdata->crinosdesc ? ch->pcdata->crinosdesc : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   description show\n\r", ch);
		send_to_char ("          description edit\n\r", ch);
		send_to_char ("          description append\n\r", ch);
	}
	return;
}


void do_hispodesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_SHIFTER(ch))
	{
		send_to_char ("Only shifters may use this", ch);
		return;
	}


	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->hispodesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->hispodesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your hispo description is:\n\r", ch);
		send_to_char (ch->pcdata->hispodesc ? ch->pcdata->hispodesc : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   description show\n\r", ch);
		send_to_char ("          description edit\n\r", ch);
		send_to_char ("          description append\n\r", ch);
	}
	return;
}



void do_glabrodesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_SHIFTER(ch))
	{
		send_to_char ("Only shifters may use this", ch);
		return;
	}


	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->glabrodesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->glabrodesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your glabro description is:\n\r", ch);
		send_to_char (ch->pcdata->glabrodesc ? ch->pcdata->glabrodesc : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   description show\n\r", ch);
		send_to_char ("          description edit\n\r", ch);
		send_to_char ("          description append\n\r", ch);
	}
	return;
}


void do_vision (CHAR_DATA * ch, char *argument)
{
	int i;

	if (!ch || IS_NPC (ch))
		return;

	if (!is_number (argument))
	{
		send_to_char ("This command requirse a number.\n\r", ch);
		return;
	}

	i = atoi (argument);

	if (i < 3 || i > 10)
	{
		send_to_char ("Mortal's vision range is only settable from 3 to 10.\n\r", ch);
		return;
	}

	if (i % 2 == 0)
	{
		send_to_char ("You must set vision in odd increments.\n\r", ch);
		return;
	}

	ch->pcdata->vision = i;
	do_look (ch, "auto");
}


void do_roomflags (CHAR_DATA * ch, char *argument)
{

	if (IS_NPC (ch))
		return;

	if (IS_SET (ch->act, PLR_ROOMFLAGS))
	{
		REMOVE_BIT (ch->act, PLR_ROOMFLAGS);
		send_to_char ("Roomflag info removed.\n\r", ch);
	}
	else
	{
		SET_BIT (ch->act, PLR_ROOMFLAGS);
		send_to_char ("Roomflag info added.\n\r", ch);
	}


	do_look (ch, "auto");
	return;
}


void do_rpflag (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	
	if (IS_NPC (ch))
		return;
	if (IS_MORE2 (ch, MORE2_RPFLAG))
	{
		send_to_char ("#cYour actions are no longer In Character.#n\n\r", ch);
		REMOVE_BIT (ch->more2, MORE2_RPFLAG);
		if (IS_AFFECTED (ch, AFF_POLYMORPH))
			sprintf (buf, "#C%s#c is no longer In Character.#n\n\r", ch->morph);
		else
			sprintf (buf, "#C%s#c is no longer In Character.#n\n\r", ch->pcdata->roomdesc);
		do_info (ch, buf);
		ch->playedIC += (int) (current_time - ch->rpflag_time);
		return;
	}
	send_to_char ("#CYou are now In Character, any actions you make will be taken as such.#n\n\r", ch);
	SET_BIT (ch->more2, MORE2_RPFLAG);
	
	if (IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (buf, "#C%s#c is now In Character.#n\n\r", ch->morph);
	else
		sprintf (buf, "#C%s#c is now In Character.#n\n\r", ch->pcdata->roomdesc);
	do_info (ch, buf);
	ch->rpflag_time = current_time;
	return;
}

/*
void do_hours (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	sprintf(buf, "#cHours online#C: #p%d#n\n\r", (get_age(ch) - 17) * 2);
	send_to_char(buf, ch);
	sprintf(buf, "#cHours IC#C: #p%d#n\n\r", get_hoursIC(ch));
	send_to_char(buf, ch);
	return;
}
*/

/* Whererp Code Begin */
void do_whererp (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	MEMORY_DATA * temp_mem;
	int rpTotal = 0;

	if (IS_NPC (ch))
		return;
	send_to_char ("\n\r#B______#C/#PCurrently Roleplaying#C/#B________________________________#n\r\n", ch);
	for (d = descriptor_list; d != NULL; d = d->next)
	{

		if (d->connected != CON_PLAYING)
			continue;
		if ((victim = d->character) == NULL)
			continue;
		if (IS_NPC (victim) || victim->in_room == NULL)
			continue;
		if (!IS_NPC (victim) && ( ch->trust < victim->trust ) )
			continue;
		if (!can_see (ch, d->character))	
			continue;
		
		if ((IS_MORE3(victim, MORE3_OBFUS1)) && !IS_STORYTELLER(ch))
			continue;
		if ((IS_MORE3(victim, MORE3_OBFUS2)) && !IS_STORYTELLER(ch))
			continue;
		//if ((IS_MORE2 (victim, MORE2_RPVIS)) && !IS_STORYTELLER(ch))
			//continue;
		
		
					
		if (IS_MORE2 (victim, MORE2_RPFLAG)) 
			{
//				if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH) && !IS_SHIFTER(victim)){
//					sprintf (buf, "#C%12s #B- #cRoom#C:#c %10s\n\r",
//					victim->morph, victim->in_room->name);
//					send_to_char (buf, ch);
//					rpTotal++;
//				}
//				else{
				if (!str_cmp(ch->name, victim->name))
				{
					sprintf(buf, "#C%12s #B- #cRoom#C:#c %10s\r\n", ch->name, ch->in_room->name);
					send_to_char(buf, ch);
				}
				else
				{
					temp_mem = memory_search_real_name(ch->memory, victim->name);

					if (IS_STORYTELLER(ch))
						{
						sprintf(buf, "#C%12s #B- #cRoom#C:#c %10s\r\n", victim->name, victim->in_room->name);
						send_to_char(buf, ch);
					}
					
					else if (IS_MORE2 (victim, MORE2_RPVIS))
					{
						sprintf(buf, "#CSomeone\r\n");
						send_to_char(buf, ch);
					}
					
					else if	(temp_mem == NULL)
					{
						sprintf(buf, "#C%.29s #B- #cRoom#C:#c %10s\r\n", victim->pcdata->roomdesc, victim->in_room->name);
						send_to_char(buf, ch);
					}
					else
					{
						sprintf(buf, "#C%29s #B- #cRoom#C:#c %10s\r\n", temp_mem->remembered_name, victim->in_room->name);
						send_to_char(buf, ch);
					}
//					sprintf (buf, "#C%12s #B- #cRoom#C:#c %10s\n\r",
//					victim->name, victim->in_room->name);
//					send_to_char (buf, ch);
//					rpTotal++;
				}
				rpTotal++;
				
			}
	
	}	
if (rpTotal > most_rp)
        most_rp = rpTotal;
sprintf (buf, "\n\r#PTotal Roleplaying#C: #B%d      #PMost this Boot#C: #B%d#n\n\r", rpTotal, most_rp);
	send_to_char (buf, ch);
	return;
}

void do_rpvis (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_MORE2 (ch, MORE2_RPVIS))
	{
		send_to_char ("#CYou are now visable on the WhereRP list.#n\n\r", ch);
		REMOVE_BIT (ch->more2, MORE2_RPVIS);
		return;
	}
	send_to_char ("#CYou are no longer visable on the WhereRP list.#n\n\r", ch);
	SET_BIT (ch->more2, MORE2_RPVIS);
	return;
}

/* Where RP code end */

void do_giftlist (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);


	if (IS_NPC (ch))
		return;


	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They must be in the room.\n\r", ch);
		return;
	}
	if (arg[0] == '\0')
	{
		send_to_char ("Who do want to add gifts to?\n\r", ch);
		return;
	}
	if (!IS_WEREWOLF (victim))
	{
		send_to_char ("Only on Werewolves", ch);
		return;
	}

	if (argument[0] != '\0')
	{
		buf[0] = '\0';
		smash_tilde (argument);
		if (argument[0] == '+')
		{
			if (victim->pcdata->giftlist != NULL)
				strcat (buf, victim->pcdata->giftlist);
			argument++;
			while (isspace (*argument))
				argument++;
		}

		if (strlen (buf) + strlen (argument) >= MAX_STRING_LENGTH - 2)
		{
			send_to_char ("Gift list getting way to long...\n\r", ch);
			return;
		}

		strcat (buf, argument);
		strcat (buf, "\n\r");
		free_string (victim->pcdata->giftlist);
		victim->pcdata->giftlist = str_dup (buf);
	}
	send_to_char ("Gift list now has:\n\r", ch);
	send_to_char (victim->pcdata->giftlist ? victim->pcdata->giftlist : "Nothing.\n\r", ch);


	send_to_char ("Your gift list has:\n\r", victim);
	send_to_char (victim->pcdata->giftlist ? victim->pcdata->giftlist : "Nothing.\n\r", victim);
	return;
}

const char *versionToString ( int version )
{
	static char buf[64];

	sprintf ( buf, "%d.%02d.%02d", version / 10000,
		( version - ( ( 10000 * ( version / 10000 ) ) ) ) / 100,
		version % 100 );

	return buf;
}

void do_version ( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];

	sprintf ( buf, "%s: Version %s %s\n\r", MUD_NAME,
		versionToString ( VERSION ), RELEASE );
	send_to_char( buf, ch );
	sprintf ( buf, "Build number : %d\n\r", LINKCOUNT );
	send_to_char( buf, ch );
	send_to_char( COMPILE_TIME, ch );
}

void do_obfuscate1 (CHAR_DATA * ch, char *argument)
{
	
	char buf[MAX_STRING_LENGTH];
	
	if (IS_NPC (ch))
		return;
	/* Still Need if vampire and if disc is right level*/
	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("#cYou have to be a vampire to use obfuscate.#n\n\r", ch);
		return;
	}
		
	if (ch->pcdata->powers[DISC_OBFUSCATE] < 1)
	{
		send_to_char ("#cYou must have obfuscate to cloak yourself in shadows.#n\n\r", ch);
		return;
	}
		
	if (IS_MORE3 (ch, MORE3_OBFUS1))
	{
		send_to_char ("#cYou are no longer cloaked in shadows.#n\n\r", ch);
		REMOVE_BIT (ch->more3, MORE3_OBFUS1);
		sprintf( buf, "#C$n#c appears suddenly from the shadows.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
		return;
	}
	send_to_char ("#CYou are now cloaked in shadows.#n\n\r", ch);
	SET_BIT (ch->more3, MORE3_OBFUS1);
	sprintf( buf, "#C$n#c moves to the shadows and disappears.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
	return;
}

void do_obfuscate2 (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	
	if (IS_NPC (ch))
		return;
	/* Still Need if vampire and if disc is right level*/
	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("#cYou have to be a vampire to use obfuscate.#n\n\r", ch);
		return;
	}
		
	if (ch->pcdata->powers[DISC_OBFUSCATE] < 2)
	{
		send_to_char ("#cYou are not yet skilled enough in Obfuscate to do this.#n\n\r", ch);
		return;
	}
		
	if (IS_MORE3 (ch, MORE3_OBFUS2))
	{
		send_to_char ("#cYou are no longer moving unseen.#n\n\r", ch);
		REMOVE_BIT (ch->more3, MORE3_OBFUS2);
		sprintf( buf, "#C$n#c appears suddenly from the shadows.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
		return;
	}
	send_to_char ("#CYou can now move about unseen.#n\n\r", ch);
	SET_BIT (ch->more3, MORE3_OBFUS2);
	sprintf( buf, "#C$n#c moves to the shadows and disappears.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
	return;
}

void do_briansucks (CHAR_DATA * ch, char *argument)
{
	send_to_char ("#sGoto TrySB.net for the best dialup internet access!\n\r", ch);
	return;
}

void do_wpburn (CHAR_DATA * ch, char *argument)
{
char buf[MAX_STRING_LENGTH];
	if (ch->pcdata->willpower[WILLPOWER_CURRENT] > 1){
		send_to_char ("#cYou burn 1 willpower for a success on your NEXT roll.\n\r", ch);
		sprintf( buf, "#C$n#c has burned a willpower for an autosuccess on their #Cnext#c roll.#n" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pcdata->willpower[WILLPOWER_CURRENT] -= 1;	
			return;
			}
	send_to_char ("#cYou dont have enough willpower to burn.\n\r", ch);
	return;
}

void do_bolt (CHAR_DATA * ch, char *argument)
	{
	char arg1[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	argument = one_argument (argument, arg1);
	
	if (IS_NPC (ch))	
		return;
	
	if (str_cmp (ch->name,"Joli")){
		send_to_char ("#cOnly Joli has access to the bolt command!#n\n\r", ch);
		return;
	}
	
	if (arg1[0] == '\0')
	{
		send_to_char ("#cSyntax: bolt playername#n\n\r", ch);
		
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("#cThey have to be around for you to bolt them. Sorry, Lady.#n\n\r", ch);
		return;
	}
	
		send_to_char ("#yJoli has zapped you with a Bolt of Lightning. Stop pissing her off, or it will get worse.#n\n\r", victim);
		for (d = descriptor_list; d != NULL; d = d->next)
			{
				if (d->connected == CON_PLAYING)
				{
					sprintf(buf, "#yJoli has zapped %s with a bolt of lightning for being a douche.#n\n\r", arg1);
					send_to_char (buf, d->character);
					
				}
			}
		return;
		}


void do_purchase (CHAR_DATA * ch, char *argument)
	{
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	argument = one_argument (argument, arg1);
	
	if (IS_NPC (ch))	
		return;
	
	if (arg1[0] == '\0')
		{
			send_to_char ("#cSyntax#C:#c purchase <item>#n\n\r", ch);
			send_to_char ("#cItems availible: pants, held, shirt, boots, gloves, hat, belt, jacket, necklace, ring, bracelet or bag.#n\n\r", ch);
			sprintf(buf, "#cYou can currently purchase #C%d#c items.#n\n\r", ch->pcdata->canbuy);
			send_to_char(buf, ch);
			return;
		}

	if (ch->pcdata->canbuy < 1 && !IS_STORYTELLER(ch)){
			send_to_char ("#cYou're currently unable to purchase items. You can purchase one item every four hours.#n\n\r", ch);
			return;
		}
			
		
	if (!strcmp (arg1, "boots")){
			obj_to_char( create_object(get_obj_index( 3358 ), 1), ch );
			send_to_char("#cYou purchased some boots. Restring them at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased some footwear.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}

	if (!strcmp (arg1, "pants")){
			obj_to_char( create_object(get_obj_index( 1011 ), 1), ch );
			send_to_char("#cYou purchased some pants. Restring them at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased some pants.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
		
	if (!strcmp (arg1, "shirt")){
			obj_to_char( create_object(get_obj_index( 1008 ), 1), ch );
			send_to_char("#cYou purchased a shirt. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a shirt.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
		
	if (!strcmp (arg1, "gloves")){
			obj_to_char( create_object(get_obj_index( 1016 ), 1), ch );
			send_to_char("#cYou purchased some gloves. Restring them at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased some gloves.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
	if (!strcmp (arg1, "hat")){
			obj_to_char( create_object(get_obj_index( 3706 ), 1), ch );
			send_to_char("#cYou purchased a hat. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a hat.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}	

	if (!strcmp (arg1, "necklace")){
			obj_to_char( create_object(get_obj_index( 9563 ), 1), ch );
			send_to_char("#cYou purchased a necklace. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a necklace.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}

	if (!strcmp (arg1, "ring")){
			obj_to_char( create_object(get_obj_index( 3364 ), 1), ch );
			send_to_char("#cYou purchased a ring. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a ring.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
	if (!strcmp (arg1, "belt")){
			obj_to_char( create_object(get_obj_index( 3362 ), 1), ch );
			send_to_char("#cYou purchased a belt. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a belt.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
	if (!strcmp (arg1, "bag")){
			obj_to_char( create_object(get_obj_index( 3032 ), 1), ch );
			send_to_char("#cYou purchased a bag. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a bag.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
	if (!strcmp (arg1, "belt")){
			obj_to_char( create_object(get_obj_index( 3362 ), 1), ch );
			send_to_char("#cYou purchased a belt. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a belt.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
	if (!strcmp (arg1, "held")){
			obj_to_char( create_object(get_obj_index( 1002 ), 1), ch );
			send_to_char("#cYou purchased a held item. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a held item.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}
		
	if (!strcmp (arg1, "jacket")){
			obj_to_char( create_object(get_obj_index( 3361 ), 1), ch );
			send_to_char("#cYou purchased a jacket. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a jacket.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;		
		}

	if (!strcmp (arg1, "bracelet")){
			obj_to_char( create_object(get_obj_index( 3363 ), 1), ch );
			send_to_char("#cYou purchased a bracelet. Restring it at your leisure.#n\n\r", ch);
			sprintf (buf, "#C%s#c just purchased a bracelet.#n \n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			if (!IS_STORYTELLER(ch)){
				ch->pcdata->canbuy -= 1;
				}
			return;
	   }
			
	send_to_char ("#cItems availible: pants, held, shirt, boots, gloves, hat, belt, jacket, necklace, ring, bracelet or bag.#n\n\r", ch);
	sprintf(buf, "#cYou can currently purchase #C%d#c items.#n\n\r", ch->pcdata->canbuy);
	send_to_char(buf, ch);
}


void do_status (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char arg2[MAX_INPUT_LENGTH];
	int statusdiff;
	int politicsucc = 0;
	int politicdiff = 6;
	int statuschange = 0;
	float statusmod = 0;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	
	if (!IS_VAMPIRE(ch)){
		send_to_char("#cHuh??#n\n\r", ch);
		return;
	}
	
	if (arg1[0] == '\0')
	{
		send_to_char ("#cSyntax: status name +/-\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL || (victim->in_room != ch->in_room))
	{
		send_to_char ("#cThey aren't here.\n\r", ch);
		return;
	}

		if (ch->side[0] != victim->side[0]){
		send_to_char("#cYou can only alter the status of those in your own sect.#n\n\r", ch);
		return;
	}
	

	
	if ((ch->pcdata->statusvotecounter < 4) && !IS_STORYTELLER(ch)){
		send_to_char ("#cYou can not currently cast a status vote.\n\r", ch);
		send_to_char ("#cYou get one status vote every 4 hours IC.\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("#cSorry, but you can't alter your own status.#n\n\r", ch);
		return;
	}
	
/* eh? 	*/
	
	if(has_merit(ch, "primogen")){
		politicdiff -= 1;
	}	
	
  if(has_merit(victim, "primogen")){
		politicdiff += 1;
	}
		
	if(has_merit(ch, "court")){
		politicdiff -= 2;
	}
		
  if(has_merit(victim, "court")){
		politicdiff += 2;
	}
	
	if(has_merit(ch, "harpy")){
		politicdiff -= 1;
	}
		
  if(has_merit(victim, "harpy")){
		politicdiff += 1;
	}
	
	/* end eh? */
		if(ch->abilities[KNOWLEDGES][KNO_POLITICS] > 0){
		
		politicsucc = diceroll(ch, ATTRIB_MAN, KNOWLEDGES, KNO_POLITICS, 6);

	if (politicsucc < 0){
		ch->pcdata->sectstatus -= 35;
		statusmod = 0;
		
	}
	
	if (politicsucc == 0)
		statusmod = 0;
	
	if (politicsucc == 1)
		statusmod = .5;
		
	if (politicsucc == 2)
		statusmod = .75;
		
	if (politicsucc == 3)
		statusmod = 1;
		
	if (politicsucc == 4)
		statusmod = 1.25;
		
	if (politicsucc >= 5)
		statusmod = 1.75;
		
	}

	else{
	send_to_char("#cYou must have a dot of Politics to alter someones status#n\n\r", ch);
	return;
}
	
	
		
		
	
	if (!str_cmp (arg2, "+")){	
		statusdiff = ch->pcdata->sectstatus - victim->pcdata->sectstatus;
		
		

		if (statusdiff <= 49){
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 50) && (statusdiff <= 99)){
			statuschange = (2 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		
		if ((statusdiff >= 100) && (statusdiff <= 199)){
			statuschange = (5 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 200) && (statusdiff <= 249)){
			statuschange = (7 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 250) && (statusdiff <= 299)){
			statuschange = (10 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 300) && (statusdiff <= 349)){
			statuschange = (20 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 350) && (statusdiff <= 399)){
			statuschange = (25 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 400) && (statusdiff <= 449)){
			statuschange = (30 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 450) && (statusdiff <= 499)){
			statuschange = (35 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 500) && (statusdiff <= 549)){
			statuschange = (40 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 550) && (statusdiff <= 599)){
			statuschange = (45 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if (statusdiff >= 600){
			statuschange = (50 * statusmod);
			victim->pcdata->sectstatus += statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
	}
	
	
	else if (!str_cmp (arg2, "-")){
		statusdiff = ch->pcdata->sectstatus - victim->pcdata->sectstatus;
		
	if (statusdiff <= -50){
			
				send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
			
		if ((statusdiff >= -49) && (statusdiff <= 19)){
			statuschange = (1 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 20) && (statusdiff <= 99)){
			statuschange = (4 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		
		if ((statusdiff >= 100) && (statusdiff <= 199)){
			statuschange = (8 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 200) && (statusdiff <= 249)){
			statuschange = (12 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 250) && (statusdiff <= 299)){
			statuschange = (15 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 300) && (statusdiff <= 349)){
			statuschange = (23 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 350) && (statusdiff <= 399)){
			statuschange = (29 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 400) && (statusdiff <= 449)){
			statuschange = (34 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 450) && (statusdiff <= 499)){
			statuschange = (39 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 500) && (statusdiff <= 549)){
			statuschange = (45 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 550) && (statusdiff <= 599)){
			statuschange = (50 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if (statusdiff >= 600){
			statuschange = (60 * statusmod);
			victim->pcdata->sectstatus -= statuschange;
			send_to_char("#cStatus vote cast.\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
	
	}

	else{
		send_to_char("#cYou must specify if you want to add to (#C+#c) or subtract from (#C-#c) their status.#n\n\r", ch);
		return;
	}
	
}
