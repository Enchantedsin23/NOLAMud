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
#include "merits.h"


char *const where_name[] = {
	"#y<#gused as light#y>      #n",
	"#y<#gworn on face#y>       #n",
	"#y<#gworn on head#y>       #n",
	"#y<#gworn around neck#y>   #n",
	"#y<#gworn around neck#y>   #n",
	"#y<#gworn on body#y>       #n",
	"#y<#gworn around body#y>   #n",
	"#y<#gworn on wrist#y>      #n",
	"#y<#gworn on wrist#y>      #n",
	"#y<#gworn on hands#y>      #n",
	"#y<#gworn on finger#y>     #n",
	"#y<#gworn on finger#y>     #n",
	"#y<#gheld, right hand#y>   #n",
	"#y<#gheld, left hand#y>    #n",
	"#y<#gtattooed#y>           #n",
	"#y<#gworn over shoulder#y> #n",
	"#y<#gworn about waist#y>   #n",
	"#y<#gworn on legs#y>       #n",
	"#y<#gworn on feet#y>       #n",
	"#y<#gin left pocket#y>     #n",
	"#y<#gin right pocket#y>    #n",
	"#y<#gthrough chest#y>      #n",
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

void werewolf_score args(( CHAR_DATA *ch, char * argument));
void vamp_score args(( CHAR_DATA *ch, char * argument));
void shifter_score args(( CHAR_DATA *ch, char * argument));
void mortal_score args(( CHAR_DATA *ch, char * argument));
void ensorcelled_score args(( CHAR_DATA *ch, char * argument));
void geist_score args(( CHAR_DATA *ch, char * argument));

int most_login = 10;
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
		sprintf (buf, "%s has loose garments draped around %s body.\n\r", HE_SHE (ch), HIS_HER (ch));
		stc (buf, tch);
		return;
	}

	fBool = FALSE;

	p1 = NULL;
	p2 = NULL;

	if ((obj = get_eq_char (ch, WEAR_LIGHT)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s is holding %s in %s hand, shining light around the area.\n\r", HE_SHE (ch), format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
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
				if (IS_CHANGELING (ch) && get_breed (ch, BREED_METIS) > 3)
				{
					if (get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
						sprintf (buf, ", with %s covering %s hands, venom dripping silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch), HIS_HER (ch));
					else
						sprintf (buf, ", with %s covering %s hands, gleaming silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch), HIS_HER (ch));
					strcat (descr, buf);
				}
				else
				{
					if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
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
				if (IS_CHANGELING (ch) && get_breed (ch, BREED_METIS) > 3)
				{
					if (get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
						sprintf (buf, " %s %s worn on %s hands and venom dripping silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch), HIS_HER (ch));
					else
						sprintf (buf, " %s %s worn on %s hands and gleaming silver talons extending from %s fingers", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch), HIS_HER (ch));
					strcat (descr, buf);
				}
				else
				{
					if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
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
			if (IS_CHANGELING (ch) && get_breed (ch, BREED_METIS) > 3)
			{
				if (get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
					sprintf (buf, " and %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				else
					sprintf (buf, " and %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
			}
			else
			{
				if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
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
			if (IS_CHANGELING (ch) && get_breed (ch, BREED_METIS) > 3)
			{
				if (get_organization (ch, ORGANIZATION_BLACK_FURIES) > 1)
					sprintf (buf, " %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				else
					sprintf (buf, " %s has venom-dripping silver talons extending from %s fingers", HE_SHE (ch), HIS_HER (ch));
				strcat (descr, buf);
			}

			if (!IS_NPC (ch) && IS_MORE (ch, MORE_HAND_FLAME))
			{
				sprintf (buf, ", which seem to be covered in flame.");
				strcat (descr, buf);
			}
			else
				strcat (descr, " ");

		}
	}
	else if (descr[0] != '\0')
		strcat (descr, "  ");
	else
		strcat (descr, "");
	fBool = FALSE;



	if ((obj = get_eq_char (ch, WEAR_HEAD)) != NULL && can_see_obj (tch, obj))
	{


		sprintf (buf, "On %s head is %s.", HIS_HER (ch), format_obj_to_char2 (obj, ch, TRUE));
		strcat (descr, buf);


	}

	if (get_eq_char (ch, WEAR_HEAD) == NULL && IS_VAMPIRE (ch) && (get_disc (ch, DISC_FORGE) > 0 || get_disc (ch, DISC_FLEETINGSPRING) > 2))
	{

		if (IS_MORE (ch, MORE_FORGE))
		{
			if (ch->pcdata->forge > 0)
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
			sprintf (buf, "bare chest ");
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

	if ((obj = get_eq_char (ch, WEAR_TATTOO)) != NULL && can_see_obj (tch, obj))
	{

		sprintf (buf, "%s %s inked onto %s body.", format_obj_to_char2 (obj, ch, TRUE), is_are (format_obj_to_char2 (obj, ch, TRUE)), HIS_HER (ch));
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
		sprintf (buf, "%s fits around %s waist.", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
		strcat (descr, buf);
	}

	if ((obj = get_eq_char (ch, WEAR_SHOULDER)) != NULL && can_see_obj (tch, obj))
	{
		sprintf (buf, "%s is slung over %s's shoulder.", format_obj_to_char2 (obj, ch, TRUE), HIM_HER (ch));
		strcat (descr, buf);
	}


	if (get_eq_char (ch, WEAR_ABOUT) == NULL)
	{
		if (((obj = get_eq_char (ch, WEAR_SCABBARD_L)) != NULL && can_see_obj (tch, obj)) && ((obj2 = get_eq_char (ch, WEAR_SCABBARD_R)) != NULL && can_see_obj (tch, obj2)) && !str_cmp ((p1 = str_dup (format_obj_to_char2 (obj, ch, TRUE))), (p2 = str_dup (format_obj_to_char2 (obj2, ch, TRUE)))))
		{
			sprintf (buf, "Two %s are pocketed.", pluralize (STR2 (obj, short_descr)));
			strcat (descr, buf);
		}
		else
		{
			if ((obj = get_eq_char (ch, WEAR_SCABBARD_L)) != NULL && can_see_obj (tch, obj))
			{
				sprintf (buf, "In %s left pocket, is %s", HIS_HER (ch), format_obj_to_char2 (obj, ch, TRUE));
				strcat (descr, buf);
				fBool = TRUE;
			}

			if ((obj = get_eq_char (ch, WEAR_SCABBARD_R)) != NULL && can_see_obj (tch, obj))
			{
				if (fBool)
					sprintf (buf, " and %s in %s right pocket.", format_obj_to_char2 (obj, ch, TRUE), HIS_HER (ch));
				else
					sprintf (buf, "In %s right pocket %s.", HIS_HER (ch), format_obj_to_char2 (obj, ch, TRUE));
				strcat (descr, buf);
			}
			else if (fBool)
				strcat (descr, ".  ");

		}
	}

	if (!IS_NPC (ch))
	{
		if (!IS_NPC (ch) && (IS_CHANGELING (ch))  && ch->pcdata->wolfform[1] > FORM_HUMAN)
		{
			if (IS_MORE (ch, MORE_BRISTLES))
			
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

	if (!IS_NPC (ch) && IS_VAMPIRE (ch) && IS_OBJ_STAT (obj, ITEM_VANISH) && get_disc (ch, DISC_CELERITY) > 0)
		strcat (pre, "#w(illusion)#n ");
	if (!IS_NPC (ch) && IS_CHANGELING (ch) && IS_OBJ_STAT (obj, ITEM_VANISH) && get_organization (ch, ORGANIZATION_STARGAZERS) > 2)
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
		strcat (buf, "#e(#rInvis#e)#n ");
	if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && !IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && IS_OBJ_STAT (obj, ITEM_ANTI_EVIL))
		strcat (buf, "#e(#BB#blue #BA#bura#e)#n ");
	else if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT (obj, ITEM_ANTI_EVIL))
		strcat (buf, "#e(#RR#red #RA#rura#e)#n ");
	else if (IS_AFFECTED (ch, AFF_DETECT_EVIL) && IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && !IS_OBJ_STAT (obj, ITEM_ANTI_NEUTRAL) && IS_OBJ_STAT (obj, ITEM_ANTI_EVIL))
		strcat (buf, "#e(#yY#oellow #yA#oura#e)#n ");
	if (IS_AFFECTED (ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT (obj, ITEM_MAGIC))
		strcat (buf, "#e(#BM#bagical#e)#n ");
	if (IS_OBJ_STAT (obj, ITEM_GLOW))
		strcat (buf, "#e(#nlight#e)#n ");
	if (IS_OBJ_STAT (obj, ITEM_HUM))
		strcat (buf, "#n");
	if (IS_OBJ_STAT (obj, ITEM_SHADOWPLANE) && obj->in_room != NULL && !IS_AFFECTED (ch, AFF_SHADOWPLANE))
		strcat (buf, "#e(#wU#bmbra#e)#n ");
	if (!IS_OBJ_STAT (obj, ITEM_SHADOWPLANE) && obj->in_room != NULL && IS_AFFECTED (ch, AFF_SHADOWPLANE))
		strcat (buf, "#e(#wN#normal #wp#nlane#e)#n ");
	if (!IS_NPC (ch) && IS_VAMPIRE (ch) && IS_OBJ_STAT (obj, ITEM_VANISH) && get_disc (ch, DISC_CELERITY) > 0)
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


int get_animalism (CHAR_DATA * ch)
{
	if (IS_NPC (ch))
		return 0;

	/* Old werewolves are fast, but Abominations have to burn animalism */
	if (ch->class == CLASS_CHANGELING && ch->pcdata->rank > get_disc (ch, DISC_ANIMALISM))
		return ch->pcdata->rank;

	if (ch->pcdata->animalism > 0 || (!IS_VAMPIRE (ch) && !IS_GHOUL (ch)))
		return get_disc (ch, DISC_ANIMALISM);

	return 0;
}


int get_disc (CHAR_DATA * ch, int disc)
{
	int max = 10;
	int total_disc;
	int min_disc = 0;
	if (disc < DISC_DOMINATE || disc > DISC_MAX)
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
	if (disc == DISC_THEBANSORCERY)
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
	if (disc < DISC_DOMINATE || disc > DISC_MAX)
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
	if (IS_CHANGELING (ch))
	{
		max = 40;
		if (disc < BREED_HUMAN || disc > BREED_METIS)
		{
			bug ("Get_breed: Out of breed range.", 0);
			return 0;
		}
	}

	if (IS_NPC (ch))
		return 0;
	if ((IS_SKINDANCER (ch)) && ch->pcdata->bloodlined[0] != disc)
		max = 3;
	if (ch->pcdata->breed[disc] > max)
		return max;
	else if (ch->pcdata->breed[disc] < 0)
		return 0;
	return ch->pcdata->breed[disc];
}


int get_organization (CHAR_DATA * ch, int disc)
{
	int max;
	if (IS_CHANGELING (ch))
	{
		max = 40;
		if (disc < ORGANIZATION_BLACK_FURIES || disc > ORGANIZATION_WENDIGOS)
		{
			bug ("Get_organization: Out of organization range.", 0);
			return 0;
		}
	}

	if (IS_NPC (ch))
		return 0;
	if (IS_SKINDANCER (ch) && ch->pcdata->bloodlined[2] != disc)
		max = 3;
	if (ch->pcdata->organizations[disc] > max)
		return max;
	else if (ch->pcdata->organizations[disc] < 0)
		return 0;
	return ch->pcdata->organizations[disc];
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char_total (OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing, bool showVehicles )
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
		if ( ch->vehicle != NULL )
			continue;
		if ( !showVehicles && obj->item_type == ITEM_VEHICLE )
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


void show_list_to_char (OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing)
{
	show_list_to_char_total( list, ch, fShort, fShowNothing, TRUE );
}

void show_char_to_char_0 (CHAR_DATA * victim, CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char buf4[MAX_STRING_LENGTH];
	char mount2[MAX_STRING_LENGTH];
	char specialstring[MAX_STRING_LENGTH];
	char covenantstat[MAX_STRING_LENGTH];
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
	
	if (IS_MORE3(victim, MORE3_OBFUS3) && (victim->pcdata->powers[DISC_OBFUSCATE] > ch->pcdata->powers[DISC_AUSPEX]) && !IS_IMMORTAL (ch) && IS_MORE2 (victim, MORE2_RPFLAG))
		return;
	
	if (IS_IMMORTAL(ch))
	{
		strcat(buf, "#y(#g");
		strcat(buf, victim->name);
		strcat(buf, "#y) #n");	
	}
	/* end obfus code */
	
	if (!IS_NPC (victim) && victim->desc == NULL)
		strcat (buf, "#R(LD)#n ");
	if (IS_SET (victim->extra2, EXTRA2_AFK))
                     strcat (buf, "#o(#yAFK#y)#n ");
	if (IS_MORE (victim, MORE_SPIRIT))
		strcat (buf, "#g(Sprt)#n ");
	if ((!IS_MORE2 (victim, MORE2_RPFLAG)) && (!IS_SET (victim->extra2, EXTRA2_AFK)))
			strcat (buf, "#n");
	if (IS_MORE3(victim, MORE3_OBFUS3))
		strcat (buf, "#c(Hidden)#n ");
	if ((IS_MORE2 (victim, MORE2_HEIGHTENSENSES)) && IS_STORYTELLER(ch))
		strcat (buf, "#c(#CHeightened Senses#c)#n ");
	if (IS_MORE3(victim, MORE3_OBFUS1))
		strcat (buf, "#c(Hidden)#n ");
	if ((IS_SET (victim->act, PLR_INCOG)) && (IS_IMMORTAL (victim)))
		strcat (buf, "(Incognito) ");
	if (IS_SET (victim->act, PLR_WIZINVIS) && IS_IMMORTAL (victim))
		strcat (buf, "(WizInvis) ");

	
	if ((ch->abilities[MENTAL][KNO_POLITICS] > 0) && (ch->side[0] == victim->side[0]) && (strlen (victim->side) > 3) && (IS_MORE2 (victim, MORE2_RPFLAG)) && (victim->pcdata->covenantstatus > 0)){
		
		char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_POLITICS, 8);
		
		if (char_success < 0){
		shownstatus = rand () % 1500 + 1;
		sprintf(covenantstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
		strcat(buf, covenantstat);	
		}
		
		if (char_success == 0){

		}
		
		if (char_success == 1){
		shownstatus = rand () % 300 + victim->pcdata->covenantstatus - 150;
			
			if (shownstatus < 0){
					shownstatus = 0;
				}
			sprintf(covenantstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, covenantstat);
		}	
		
		if (char_success == 2){
		
		shownstatus = rand () % 150 + victim->pcdata->covenantstatus - 75;
	
			if (shownstatus < 0){
					shownstatus = 0;
				}
			sprintf(covenantstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, covenantstat);
		
		}
		
		if (char_success == 3){
			
			

			shownstatus = rand () % 80 + victim->pcdata->covenantstatus - 40;

			if (shownstatus < 0){
					shownstatus = 0;
				}		
			sprintf(covenantstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, covenantstat);
		}
		
		
		if (char_success == 4){
		
		shownstatus = rand () % 20 + victim->pcdata->covenantstatus - 10;
		
	
		if (shownstatus < 0){
					shownstatus = 0;
				}
			sprintf(covenantstat, "#G(#eStatus#G:#e%d#G)#n ", shownstatus);
			strcat(buf, covenantstat);	
		}
		
		
		if (char_success > 4){
			sprintf(covenantstat, "#G(#eStatus#G:#e%d#G)#n ", victim->pcdata->covenantstatus); 
			strcat(buf, covenantstat);
		}

	}
	
	if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_AFFECTED (victim, AFF_SHADOWPLANE))
		strcat (buf, "#r(Normal plane)#n ");
	else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_AFFECTED (victim, AFF_SHADOWPLANE))
		strcat (buf, "#e(Umbra)#n ");
			
	
	if (!IS_NPC (ch) || ((victim->pcdata->wolfform[1] == FORM_DALU)) || ((victim->pcdata->wolfform[1] == FORM_GAURU)) || ((victim->pcdata->wolfform[1] == FORM_URSHUL)) || ((victim->pcdata->wolfform[1] == FORM_URHAN)) || (IS_AFFECTED (victim, AFF_POLYMORPH)))
		{
			/* Remember Code */
			remembered_char = memory_search_real_name(ch->memory, victim->name);
			if (remembered_char != NULL)
			{
				strcat (buf, "#y(#g");
				strcat (buf, remembered_char->remembered_name);
				strcat (buf, "#y) #n");
			}
			/* End Remember Code */
		}

	if (!IS_NPC (ch) && (IS_VAMPIRE (ch) || IS_GHOUL (ch))
		 && get_disc (ch, DISC_MAJESTY) > 0)
	{
		switch (victim->class)
		{
		default:
			break;
		case CLASS_VAMPIRE:
			strcat (buf, "(#RVamp#n) ");
			break;
		case CLASS_CHANGELING:
			strcat (buf, "(#RWC#n) ");
			break;
		}
	}

	
	if (victim->affected_by == AFF_POLYMORPH)
	{
		strcat (buf, "");
	}	
	
	else if (IS_CHANGELING (victim) && IS_SET (victim->act, PLR_WOLFMAN) && !IS_POLYAFF (victim, POLY_ZULO) && IS_CHANGELING (ch))
	{
		/* Remember Code */
		remembered_char = memory_search_real_name(ch->memory, victim->name);
		if (remembered_char != NULL)
		{
			strcat (buf, "#y(#g");
			strcat (buf, remembered_char->remembered_name);
			strcat (buf, "#y) #n");
		}
		/* End Remember Code */
	}

	if (IS_AFFECTED (victim, AFF_PERFUME))
	{
		if (!IS_NPC (victim) && victim->sex == SEX_FEMALE) 
			sprintf (buf4, "\n\r#g She carries a lovely scent of feminine perfume.#n");
		else if (!IS_NPC (victim) && victim->sex == SEX_MALE)
			sprintf (buf4, "\n\r#g He carries an intoxicating, musky scent of cologne.#n");
		else
			sprintf (buf4, "\n\r#g They carry an indeterminate scent of body wash.#n");
		strcat (buf3, buf4);
	}
	
	if (IS_AFFECTED (victim, AFF_DIRTY))
	{
		if (!IS_NPC (victim) && victim->sex == SEX_FEMALE) 
			sprintf (buf4, "\n\r#g She is covered in reeking filth and grime.#n");
		else if (!IS_NPC (victim) && victim->sex == SEX_MALE)
			sprintf (buf4, "\n\r#g He is covered in reeking filth and grime.#n");
		else
			sprintf (buf4, "\n\r#g They are covered in reeking filth and grime.#n");
		strcat (buf3, buf4);
	}
	
	// Pregnancy check
	if (IS_EXTRA(victim, EXTRA_PREGNANT))
	{
			sprintf (buf4, "\n\r#g She is starting to show a small, round belly.#n");
		strcat (buf3, buf4);
	}
	
	// Nudity check
	if (((get_eq_char (victim, WEAR_LEGS)) == NULL) && ((get_eq_char (victim, WEAR_BODY)) == NULL) && ((get_eq_char (victim, WEAR_FEET)) == NULL) && ((get_eq_char (victim, WEAR_ABOUT)) == NULL))
	{
		if (!IS_NPC (victim) && victim->sex == SEX_FEMALE) 
			sprintf (buf4, "\n\r#g She is completely naked.#n");
		else if (!IS_NPC (victim) && victim->sex == SEX_MALE)
			sprintf (buf4, "\n\r#g He is completely naked.#n");
		else if (IS_NPC (victim))
			sprintf (buf4, "\n\r#n");
		else
			sprintf (buf4, "\n\r#g They are completely naked.#n");
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
			vname = str_dup (victim->name);

		if (IS_NPC (emb))
			ename = str_dup (emb->short_descr);
		else if (IS_AFFECTED (emb, AFF_POLYMORPH))
			ename = str_dup (emb->morph);
		else
			ename = str_dup (emb->name);

		if (ch == victim)
			sprintf (buf4, "\n\r...You are intensely focused on %s.", ename);
		else if (ch == emb)
			sprintf (buf4, "\n\r...%s is intensely focused on you.", vname);
		else
			sprintf (buf4, "\n\r...%s is intensely focused on %s.", vname, ename);
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
			vname = str_dup (victim->name);

		if (IS_NPC (emb))
			ename = str_dup (emb->short_descr);
		else if (IS_AFFECTED (emb, AFF_POLYMORPH))
			ename = str_dup (emb->morph);
		else
			ename = str_dup (emb->name);

		if (ch == victim)
			sprintf (buf4, "\n\r...You are being intensely focused on by %s.", ename);
		else if (ch == emb)
			sprintf (buf4, "\n\r...%s is your intense, emotional focus.", vname);
		else
			sprintf (buf4, "\n\r...%s is being intensely focused on by %s.", vname, ename);
		strcat (buf3, buf4);
	}

	if (victim->position == POS_STANDING && victim->long_descr[0] != '\0' && strlen (victim->long_descr) > 3 && (mount = victim->mount) == NULL && victim->stance[0] < 1)
	{
		strcpy (buf4, victim->long_descr);
		buf4[strlen (buf4) - 2] = '\0';
		strcat (buf, "#o");
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
			strcat (buf, "#y(#g");
			strcat (buf, remembered_char->remembered_name);
			strcat (buf, "#y) #n");
		}
		/* End Remember Code */

    /* Changeling short desc */		
		if ( IS_CHANGELING(ch) && IS_CHANGELING(victim) )
		   {
           strcat (buf, victim->pcdata->miendesc);
           }
	    else if ( IS_ENSORCELLED(ch) && IS_CHANGELING(victim) )
		   {
           strcat (buf, victim->pcdata->miendesc);
           }
		else 
		   {
		   strcat (buf, victim->pcdata->roomdesc);
	        }
	    }
     /* End short desc */

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
				strcat (buf, "thin air?");
			else if (victim->fighting == ch)
				strcat (buf, "YOU!");
			else if (victim->in_room == victim->fighting->in_room)
			{
				strcat (buf, PERS (victim->fighting, ch));
				strcat (buf, ".");
			}
			else
				strcat (buf, "someone who left?");
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
			strcat (buf, " is DEAD!");
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
				sprintf (buf4, " is sitting on %s.", victim->sat->short_descr);
				strcat (buf, buf4);
			}
			else
				strcat (buf, " is sitting here.");
			break;

		case POS_STANDING:

			if (!IS_NPC (victim) && (victim->pcdata->doing != NULL))
			{
				if ((victim->pcdata->roomdesc != NULL) && (strlen (victim->pcdata->roomdesc) > 3) && (!IS_AFFECTED (victim, AFF_POLYMORPH))){
					}
					else {
						strcat (buf, "");
					} 
				strcat (buf, " ");
				strcat (buf, victim->pcdata->doing);
				strcat (buf, " ");
			}
			else
			{
				if (!IS_POLYAFF (victim, POLY_FISH) && !IS_IMMORTAL (victim) && !IS_NPC (victim))
					strcat (buf, " is here.");
				else if ((IS_POLYAFF (victim, POLY_FISH) || IS_IMMORTAL (victim)))
					strcat (buf, " is here.");
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
					strcat (buf, "#n");
					//if (victim->pcdata->aura != NULL){
					//	strcat (buf, "#n");
					//	strcat (buf, victim->pcdata->aura);
					//}
					//else {
					//	strcat (buf, "#n");
					//} 
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
				strcat (buf, "someone who left?");
			break;
		}
	}

	if (!IS_NPC(victim) && victim->pcdata->show_social == TRUE && IS_VAMPIRE (victim) && !str_cmp (victim->bloodline, "Unset"))
		sprintf(specialstring, " #b(#BPres#b:#B0#b)#n");
	else if (!IS_NPC(victim) && victim->pcdata->show_social == TRUE)
		sprintf(specialstring, " #b(#BPres#b:#B%d#b)#n", 
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
	OBJ_DATA *obj;
	int iWear;
	bool found;



	if ((((!IS_MORE3(ch, MORE3_OBFUS1)) && (!IS_MORE3(ch, MORE3_OBFUS3))) && ((!IS_MORE3(victim, MORE3_OBFUS1)) && (!IS_MORE3(victim, MORE3_OBFUS3)))) && (can_see (victim, ch)))
	
	{
		
		act ("", ch, NULL, victim, TO_VICT); // $n looks at you.
		act ("", ch, NULL, victim, TO_NOTVICT); // $n looks at $N.
	}

	if (!IS_NPC(victim) && victim->pcdata->showtempdesc == TRUE)
	{
		send_to_char(victim->pcdata->tempdesc, ch);
		send_to_char("\r", ch);
	}
	
	/* Changeling Test */
	 else if ( IS_CHANGELING(ch) && IS_CHANGELING(victim) ) 
	{
		send_to_char(victim->pcdata->mlongdesc, ch);
		send_to_char("\r", ch);
	}
	/* End test */
	
	/* Ensorcelled Test */
	 else if ( IS_ENSORCELLED(ch) && IS_CHANGELING(victim) ) 
	{
		send_to_char(victim->pcdata->mlongdesc, ch);
		send_to_char("\r", ch);
	}
	/* End test */

	else if (!IS_NPC (victim) && IS_WEREWOLF (victim) && victim->pcdata->wolfform[1] != FORM_HUMAN)
	{
		if (victim->pcdata->urhandesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_URHAN)
		{
			send_to_char (victim->pcdata->urhandesc, ch);
			send_to_char ("\r", ch);
		}
		else if (victim->pcdata->gaurudesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_GAURU)
		{
			send_to_char (victim->pcdata->gaurudesc, ch);
			send_to_char ("\r", ch);
		}
		else if (victim->pcdata->urshuldesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_URSHUL)
		{
			send_to_char (victim->pcdata->urshuldesc, ch);
			send_to_char ("\r", ch);
		}
		else if (victim->pcdata->daludesc[0] != '\0' && victim->pcdata->wolfform[1] == FORM_DALU)
		{
			send_to_char (victim->pcdata->daludesc, ch);
			send_to_char ("\r", ch);
		}	
		else
			act ("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
	}


	else
	{
		if (victim->description[0] != '\0')
		{
			send_to_char (victim->description, ch);
			send_to_char ("\r", ch);
		}
		else
		{
			act ("You see nothing special about $M.", ch, NULL, victim, TO_CHAR);
		}
	}

	found = FALSE;
	//new
	if (!IS_NPC (ch) && !IS_NPC (victim))
	//{
	//	send_to_char ("\r", ch);
	//	show_equipment (victim, ch);
	//}
	//else
	{
		for (iWear = 0; iWear < MAX_WEAR; iWear++)
		{
			if ((obj = get_eq_char (victim, iWear)) != NULL && can_see_obj (ch, obj))
			{
				if (!found)
				{
					char buf[MAX_STRING_LENGTH];
					send_to_char ("\n\r", ch);
					sprintf(buf, "Currently wearing:\n\r");
					send_to_char (buf, ch);
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

	if ( can_see_2( ch, rch, FALSE ) )
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

    if ( IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R) )
    {
	send_to_char( "You have no eyes to see with!\n\r", ch );
	return FALSE;
    }

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
	char buf1[MAX_STRING_LENGTH];
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
	ROOM_INDEX_DATA *location;
	//OBJ_DATA *vehicle;
	//OBJ_DATA *vehicle_next;
	//ROOM_INDEX_DATA *pRoomIndex;
	char *pdesc;
	int door;
	//bool found;
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
			if (!IS_CHANGELING (ch) || get_organization (ch, ORGANIZATION_UKTENA) < 1)
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

	// Look around hook
if (!str_cmp (arg1, "around") && ch->in_room != NULL)
     { location = ch->in_room; 
       if (location->extra_descr != NULL) 
            {
             strcpy (buf1, "Looking around, you see the following items of interest:#n\n\r #n"); 
             EXTRA_DESCR_DATA *ed;
             for (ed = location->extra_descr; ed; ed = ed->next) 
             {
             strcat (buf1, ed->keyword);
             if (ed->next)
             strcat (buf1, " "); 
             }
             strcat (buf1, "\n\r"); 

//Send the text to the character 
             send_to_char(buf1, ch);
			 return;
             } else {
             send_to_char("Looking around, you see nothing of special interest.", ch);
             return;
            }
           } 
	
	if (arg1[0] == '\0' || !str_cmp (arg1, "auto"))
	{
		/* 'look' or 'look auto' */
		if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
			act ("$p", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
		else if (IS_AFFECTED (ch, AFF_SHADOWPLANE)) //will be removed
			send_to_char ("#gThe#G H#gedge#n\n\r", ch);   //will be removed
		else
		{
			if ( ch->vehicle != NULL )
				sprintf (buf, "%s\n\r", ch->vehicle->short_descr);
			else
				sprintf (buf, "#G%s#n\n\r", ch->in_room->name);

			/* shows immortals the room flags */
			if (IS_IMMORTAL (ch) && IS_SET (ch->act, PLR_ROOMFLAGS))
			{
				sprintf (buf2, " #w(#1vnum#n: #G%d#w) [#1Sector#n:%s #1Room Flags#n:%s]#n", ch->in_room->vnum, room_sector (ch->in_room->sector_type), room_bit_name (ch->in_room->room_flags));
				strcat (buf, buf2);
			}
		
			send_to_char (buf, ch);
		}

		if (!IS_OUTDOORS( ch ))
		{
	    pdesc = get_extra_descr ("@temp", ch->in_room->extra_descr);
	          if (pdesc != NULL)
	          {
		      ch_printf (ch, "%s", pdesc);
	          }
		}
		else
		{
			show_weather(ch);
		}
		
		if (!IS_NPC (ch) && IS_CHANGELING (ch) && IS_SET (ch->in_room->room_flags, ROOM_HEDGEGATE))
		{
			send_to_char ("#pThe area is alive with waves of tingling glamour.#n\n\r", ch);
		}
		
		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_CLOTHES))
		{
			send_to_char ("#oWares are folded, stacked and displayed for purchase.#n\n\r", ch);
		}
		
		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_BAR))
		{
			send_to_char ("#oThe sickly-sour smell of alcohol is prevalent.#n\n\r", ch);
		}
		
		if (!IS_NPC (ch) && IS_ENSORCELLED (ch) && IS_SET (ch->in_room->room_flags, ROOM_HEDGEGATE))
		{
			send_to_char ("#pThe area is alive with waves of tingling glamour.#n\n\r", ch);
		}

		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_STOREROOM)) // Storeroom was gmarket

		{
			send_to_char ("#n", ch);
		}
		if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_ESCAPE))
		{
			send_to_char ("#n", ch);
		}

		if (ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT && !IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL)
		{
			act ("You are inside $p.", ch, ch->pcdata->chobj->in_obj, NULL, TO_CHAR);
			show_list_to_char (ch->pcdata->chobj->in_obj->contains, ch, FALSE, FALSE);
		}
		else if ((arg1[0] == '\0' || !str_cmp (arg1, "auto")) && IS_AFFECTED (ch, AFF_SHADOWPLANE)){
		/* Hacked Umbra Code 
			
			if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_HEDGEGATE)){
				send_to_char ("Great care has been taken to make sure no piece of foliage is out of place nor any unknown visitor allowed.  It seems to be a sort of thicket and is very green surrounding the caern itself.  A few spirits imbued in the caern itself dance about the area offering even the  surrounding grounds a peaceful serenity.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}

			else if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_STOREROOM)){
				send_to_char ("This place is obviously holy in a sense and seems well guarded by both spirit and feral alike.  Like the area surrounding it, it is calm and its construction is precise and spares no attention to detail.  A ring is made in the ground and a few pebbles decorate that circle that holds any offereings made to the spirits overhead. It seems as if this is where the natural world begins and the spirit world ends.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			
			else if (!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_ESCAPE)){
				send_to_char ("Darkness seems ever prevalent here, the feeling of the unnatural floats about as much as the minions of the wyrm mostly unseen by the naked eye.  It seems as if this room is everything a sane person would stay way from and beg for Gaia's strength to get by it.  Those who seek to cleanse this area would certainly have trouble doing so and probally end up in pain for their deeds.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) &&  (ch->in_room->sector_type == SECT_MOUNTAIN)){
				send_to_char ("Deep into the damp ground lies a passage way of connected caves and within the view is glorious with its high ceilings and deep pools of pure water.  Little seems to have been in here and it seems mostly peaceful only hosting a few less hostile spirits.\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_CITY)){
				send_to_char ("Feral seem not to be welcome as spirits dance back and forth giving them glares or even worse threatening their very presence.  All structures within this city are a pale grey, boring and looming and the air is chilled here.  There is no smell here only of the stale air within the walls of the city and weaver spirits seem to dance around its inner walls spreading their webs.\n\r\n\r", ch);
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
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_HEDGE)){
				send_to_char (" Rolling hills of short grasses and small shrubs line this part of the umbra.  It seems empty and more silent than it should and in the distance are seen the countryside of the humans beginning to taint this place.  The hills are large enough to provide protection for anyone needing it or a safe place to rest.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_INSIDE)){
				send_to_char ("Within these walls one can find small comfort in this city either hiding from the spirits glaring gaze or from anything else that seems to hunt those who come here.  It is relativly empty of any belongings and is made of simple wood sporting nothing of interest.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_OOC)){
				send_to_char ("Little to nothing grows here except for the ever growing amount of sand on the ground that seems to multiply with each step.  Water is scarce and the sky above is an errie red-orange color.  Emptyness seems to prevail here and nothing more can be said of the bland landscape of the desert.\n\r\n\r", ch);
				do_exits (ch, "auto");
			}
			else {
				send_to_char ("Colors and spirits seem to swirl about in a dance not meant for most eyes to even dream about.  Stepping further into the umbra shall bring spirits of both friend and foe and challenge even the mighty of strength testing them all.\n\r\n\r", ch);
				do_exits (ch, "auto");		
			}*/	
		}
		
		
		else if ((!IS_NPC (ch) && !IS_SET (ch->act, PLR_BRIEF)) && (arg1[0] == '\0' || !str_cmp (arg1, "auto")))
		{	
			
			     
			if (ch->plane == 0)
			sprintf(buf, "#n  %s",ch->in_room->description);
		if ( ch->vehicle != NULL ) sprintf(buf, "%s\n\r", format_string(ch->vehicle->inside_desc));
			
		
		send_to_char(buf, ch);
		
		
		if (!IS_NPC (ch) && IS_SET (ch->act, PLR_AUTOEXIT))
			do_exits (ch, "auto");
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
	if ( !str_cmp(arg1, "out") && ch->vehicle != NULL ) {
		OBJ_DATA *vehicle = ch->vehicle;
		ch->vehicle = NULL;
		do_look(ch, "");
		ch->vehicle = vehicle;
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

		if ( ( (IS_MORE3(victim, MORE3_OBFUS1)) || (IS_MORE3(victim, MORE3_OBFUS3)) ) && (victim->pcdata->powers[DISC_OBFUSCATE] > ch->pcdata->powers[DISC_AUSPEX]) && !IS_IMMORTAL (ch))
		
		{
			send_to_char ("You do not see that here.\n\r", ch);
			return;
		}
	
		else if ( IS_IMMORTAL(ch) || ( ch->vehicle == victim->vehicle ) ) {
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
			if ( obj->item_type == ITEM_VEHICLE ) {
				if ( ch->vehicle != obj )
					send_to_char (format_string(obj->outside_desc), ch);
				else
					send_to_char (format_string(obj->inside_desc), ch);
			} else 
				send_to_char (obj->description, ch);
			send_to_char ("#n", ch);
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
			else {
				if ( obj->item_type == ITEM_VEHICLE ) {
					if ( ch->vehicle != obj )
						send_to_char (format_string(obj->outside_desc), ch);
					else
						send_to_char (format_string(obj->inside_desc), ch);
				} else 
					send_to_char (obj->description, ch);
			}
			send_to_char ("#n", ch);
			return;
		}
	}

	for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
	{
		if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && obj->chobj != NULL && obj->chobj == ch)
			continue;
		if (can_see_obj (ch, obj))
		{
			if ( obj->item_type == ITEM_VEHICLE ) {
				if ( ch->vehicle != obj )
					send_to_char (format_string(obj->outside_desc), ch);
				else
					send_to_char (format_string(obj->inside_desc), ch);
				send_to_char ("#n", ch);
				return;
			}

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
			send_to_char ("#n", ch);
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

	strcpy (buf, fAuto ? "\n\r#g[ #nExits:" : "#nObvious exits:\n\r");

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
                                 sprintf( buf + strlen(buf), "#w %-5s#n(#eclosed#n)",
                                 dir_name[door]);
                          }
			else
			  {
			        strcat (buf, "#w ");
                                strcat (buf, dir_name[door]);
                          }
                        }
			else
			{
				if (!IS_NPC(ch) && IS_IMMORTAL(ch))
				{
					sprintf (buf + strlen (buf), "#w%-5s - (%d) %s\n\r", capitalize (dir_name[door]), pexit->to_room->vnum, pexit->to_room->name);
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
		strcat (buf, "#g ]#n\n\r");

	send_to_char (buf, ch);
	return;
}

void do_score (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

    if ( get_trust( ch ) != ch->level )
    {
	sprintf( buf, "You are trusted at Staff level %d.",
	    get_trust( ch ) );
	send_to_char( buf, ch );
    }
	
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

	if (IS_WEREWOLF(ch))
	{
		werewolf_score(ch, argument);
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
	
	if (IS_ENSORCELLED(ch))
	{
		ensorcelled_score(ch, argument);
		return;
	}
	
		if (IS_GEIST(ch))
	{
		geist_score(ch, argument);
		return;
	}
	
	mortal_score(ch, argument);
	
	    if (IS_EXTRA(ch, EXTRA_PREGNANT))
    {
    send_to_char("#n                                                 You are pregnant with a child.\n\r",ch);
	}
	
	return;
}

void werewolf_score ( CHAR_DATA *ch, char * argument)
{
   char buf[MAX_STRING_LENGTH*2];

   sprintf(buf, "\n\r"
"#o   .-----------------------------------------------------------------------.#n\n\r"
"#o  /             #GNew Orleans by Night#y:#G A World of Darkness Story#o             \\#n\n\r"
"#o /---------------------------------------------------------------------------\\#n\n\r"
      "#o/#G  Name  #y:#e %-19s  #GSurname#y:#e %-36s #o\\#n\n\r"
      "#o|#G  Virtue#y:#e %-16s      #GVice#y:#e %-38s #o|#n\n\r"
      "#o|#G  Primal Urge#y:#e %-61d #o|#n\n\r"
"#o|-----------------------------------------------------------------------------|#n\n\r"
"#o#C   #GAuspice#y :#e %-7s  #GTribe#y :#e %-7s     #GLodge #y:#e %-16s \n\r"
"#o|-----------------------------------------------------------------------------|#n\n\r"
   "#o}---------------------------------#y/#wA#nttributes#y/#o--------------------------------{#n\n\r"
   "#o}--------/#gP#Ghysical#o/-----------------/#gS#Gocial#o/----------------/#gM#Gental#o/----------{#n\n\r"
   "#o| #wStrength#e____%9s #o|  #wPresence#e______%9s #o|  #wResolve#e_______%9s #o|#n\n\r"
   "#o| #wDexterity#e___%9s #o|  #wManipulation#e__%9s #o|  #wIntelligence#e__%9s #o|#n\n\r"
   "#o| #wStamina#e_____%9s #o|  #wComposure#e_____%9s #o|  #wWits#e__________%9s #o|#n\n\r"
   "#o}-----------------------------------#y/#wS#nkills#y/#o----------------------------------{#n\n\r"
   "#o}--------/#gP#Ghysical#o/-----------------/#gS#Gocial#o/----------------/#gM#Gental#o/----------{#n\n\r"
   "#o| #wAthletics#e___%5s #o|  #wAnimal Ken#e____%5s #o|  #wAcademics#e_____%5s #o|#n\n\r"
   "#o| #wBrawl#e_______%5s #o|  #wEmpathy#e_______%5s #o|  #wComputer#e______%5s #o|#n\n\r"
   "#o| #wFirearms#e____%5s #o|  #wExpression#e____%5s #o|  #wCrafts#e________%5s #o|#n\n\r"
   "#o| #wLarceny#e_____%5s #o|  #wIntimidation#e__%5s #o|  #wInvestigation#e_%5s #o|#n\n\r"
   "#o| #wStealth#e_____%5s #o|  #wSocialize#e_____%5s #o|  #wMedicine#e______%5s #o|#n\n\r"
   "#o| #wSurvival#e____%5s #o|  #wStreetwise#e____%5s #o|  #wOccult#e________%5s #o|#n\n\r"
   "#o| #wWeaponry#e____%5s #o|  #wPersuasion#e____%5s #o|  #wPolitics#e______%5s #o|#n\n\r"
   "#o| #wDrive#e_______%5s #o|  #wSubterfuge#e____%5s #o|  #wScience#e_______%5s #o|#n\n\r"
   "#o}--------------------------------#y/#wA#ndvantages#y/#o---------------------------------{#n\n\r"
   "#o|                             #o==== #gR#Genown#o ====                                |#n\n\r",
   
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
      strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice, ch->vamppot,       
      (ch->pcdata->clan != -1) ? clan_names[ch->pcdata->clan][0] : "None", strlen (ch->side) < 2 ? "None" :
      ch->bloodline, 
      strlen (ch->lodge) < 2 ? "None" : ch->lodge,
      
      show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
      show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
      show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->bloodline, "Unset")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),

      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE])
      );
      send_to_char(buf, ch);

      if (ch->pcdata->powers[DISC_CUNNING] > 0)
      {
		sprintf (buf, "#o|                        #gC#Gunning#e___________%-8s#n                          #o|#n\n\r", show_dot(ch->pcdata->powers[DISC_CUNNING]));
		send_to_char(buf, ch);
      }
      if (ch->pcdata->powers[DISC_GLORY] > 0)
      {
		sprintf (buf, "#o|                        #gG#Glory#e_____________%-8s#n                          #o|#n\n\r", show_dot(ch->pcdata->powers[DISC_GLORY]));
		send_to_char(buf, ch);
      }
      if (ch->pcdata->powers[DISC_HONOR] > 0)
      {
		sprintf (buf, "#o|                        #gH#Gonor#e_____________%-8s#n                          #o|#n\n\r", show_dot(ch->pcdata->powers[DISC_HONOR]));
		send_to_char(buf, ch);
      }
      if (ch->pcdata->powers[DISC_PURITY] > 0)
      {
		sprintf (buf, "#o|                        #gP#Gurity#e____________%-8s#n                          #o|#n\n\r", show_dot(ch->pcdata->powers[DISC_PURITY]));
		send_to_char(buf, ch);
		  }
      if (ch->pcdata->powers[DISC_WISDOM] > 0)
      {
		sprintf (buf, "#o|                        #gW#Gisdom#e____________%-8s#n                          #o|#n\n\r", show_dot(ch->pcdata->powers[DISC_WISDOM]));
		send_to_char(buf, ch);
      }
      

      send_to_char("#o|                                                                             #o|#n\n\r", ch);      
      
   sprintf(buf, 
   "#o}-------------------------------#y/#wO#nther Traits#y/#o--------------------------------{#n\n\r"
   "#o|    #o=====#wW#nillpower#o=====                              ==#wD#namage Total#o: #o%d#o===#n#o    |#n\n\r"
   "#o|    #n%19s                       #o        Agg   Lethal  Bash     #o|#n\n\r"
   "#o|   #n%21s                      #y         %d      %d       %d#o      |#n\n\r"
   "#o|                             #o=====#cH#Carmony#o=====#o                               |#n\n\r"
   "#o|#n                            %19s                         #o     |#n\n\r"
      "#o|-----------------------------------------------------------------------------|#n\n\r"
      "#o\\    #GXP#y:#e%-4d     #GHours#y:#e %-5d      #GIC#y:#e %-5d         #gW#Gerewolf#y:#e The#g F#Gorsaken#o   /#n\n\r"
      "#o \\---------------------------------------------------------------------------/#n\n\r"
      "#g              Enter '#Gwerestats#g' for your Werewolf form modifiers.#n\n\r",
  
      (ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
      show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), 
      show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]), 
      ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
      show_spot(ch->pcdata->clant),
      
      ch->exp,((get_age (ch) - 17) * 2), get_hoursIC(ch)
//	  ch->pcdata->harmony_bank
    );
    
    send_to_char(buf, ch);
    return;
}


void mortal_score( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH*2];
   
   if IS_SET (ch->act, PLR_BLIND)
		{
      sprintf(buf, "\n\r"
      "Mortal Score Sheet\n\r\n\r"
      "Name: %s\n\r"
      "Surname: %s\n\r"
      "Virtue: %s\n\r"
      "Vice: %s\n\r"
      "Partner: %s\n\r\n\r"
      "Morality: %d\n\r\n\r"
      "#wPhysical Attributes:#n "
      "Strength %d, Dexterity %d, Stamina %d\n\r"
      "#wSocial Attributes:#n "
      "Presence %d, Manipulation %d, Composure %d\n\r"
      "#wMental Attributes:#n "
      "Resolve %d, Intelligence %d, Wits %d\n\r\n\r"
      "#wPhysical Skills:#n \n\r"
      "Athletics %d, Brawl %d, Firearms %d, Larceny %d, Stealth %d, Survival %d,\n\rWeaponry %d, Drive %d \n\r"
      "#wSocial Skills:#n \n\r"
      "Animal Ken %d, Empathy %d, Expression %d, Intimidation %d, Socialize %d, \n\rStreetwise %d, Persuasion %d, Subterfuge %d \n\r"
      "#wMental Skills:#n \n\r"
      "Academics %d, Computer %d, Crafts %d, Investigation %d, Medicine %d, Occult %d,\n\rPolitics %d, Science %d \n\r\n\r"
      "Damage Totals: \n\r"
      "%d Aggravated, %d Lethal, %d Bashing \n\r\n\r"
      "Current XP: %d\n\r"
      "Hours Played: %d\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
	  strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice,
	  strlen (ch->pcdata->marriage) < 2 ? "Single" : ch->pcdata->marriage,
	  ch->pcdata->clant,
	  ch->attributes[ATTRIB_STR], ch->attributes[ATTRIB_DEX], ch->attributes[ATTRIB_STA],
	  ch->attributes[ATTRIB_CHA], ch->attributes[ATTRIB_MAN], ch->attributes[ATTRIB_APP],
	  ch->attributes[ATTRIB_PER], ch->attributes[ATTRIB_INT], ch->attributes[ATTRIB_WIT],
	  ch->abilities[PHYSICAL][TAL_ATHLETICS],ch->abilities[PHYSICAL][TAL_BRAWL],ch->abilities[PHYSICAL][TAL_FIREARMS],ch->abilities[PHYSICAL][TAL_LARCENY],ch->abilities[PHYSICAL][TAL_STEALTH],ch->abilities[PHYSICAL][TAL_SURVIVAL],ch->abilities[PHYSICAL][TAL_WEAPONRY],ch->abilities[PHYSICAL][TAL_DRIVE],
	  ch->abilities[SOCIAL][SKI_ANIMALKEN],ch->abilities[SOCIAL][SKI_EMPATHY],ch->abilities[SOCIAL][SKI_EXPRESSION],ch->abilities[SOCIAL][SKI_INTIMIDATION],ch->abilities[SOCIAL][SKI_SOCIALIZE],ch->abilities[SOCIAL][SKI_STREETWISE],ch->abilities[SOCIAL][SKI_PERSUASION],ch->abilities[SOCIAL][SKI_SUBTERFUGE],
	  ch->abilities[MENTAL][KNO_ACADEMICS],ch->abilities[MENTAL][KNO_COMPUTER],ch->abilities[MENTAL][KNO_CRAFTS],ch->abilities[MENTAL][KNO_INVESTIGATION],ch->abilities[MENTAL][KNO_MEDICINE],ch->abilities[MENTAL][KNO_OCCULT],ch->abilities[MENTAL][KNO_POLITICS],ch->abilities[MENTAL][KNO_SCIENCE],
	  ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
	  ch->exp, ((get_age (ch) - 17) * 2)
      );
      send_to_char(buf, ch);
      return;
      }
	else

   sprintf(buf, "\n\r"
"#g.-----------------------------------------------------------------------------.#n\n\r"
"#g|#y I#onnocence#y L#oost                                                       #yM#oortal #g|#n\n\r"
"#g|-----------------------------------------------------------------------------|#n\n\r"
      "#g|  #nName   #y:#n %-19s #nSurname#y:#n %-36s #g|#n\n\r"
      "#g|  #nVirtue #y:#n %-16s       #nVice#y:#n %-36s #g|#n\n\r"
	  "#g|  #nPartner#y:#n %-65s #g|#n\n\r"
      "#g)--------#G(#yP#ohysical#G)#g-----------------#G(#yS#oocial#G)#g----------------#G(#yM#oental#G)#g----------(#n\n\r"
      "#g| #nStrength#e....%9s #g|  #nPresence#e......%9s #g|  #nResolve#e.......%9s #g|#n\n\r"
      "#g| #nDexterity#e...%9s #g|  #nManipulation#e..%9s #g|  #nIntelligence#e..%9s #g|#n\n\r"
      "#g| #nStamina#e.....%9s #g|  #nComposure#e.....%9s #g|  #nWits#e..........%9s #g|#n\n\r"
      "#g)-----------------------------------#G(#yS#okills#G)#g----------------------------------(#n\n\r"
      "#g| #nAthletics#e...%5s #g|  #nAnimal Ken#e....%5s #g|  #nAcademics#e.....%5s #g|#n\n\r"
      "#g| #nBrawl#e.......%5s #g|  #nEmpathy#e.......%5s #g|  #nComputer#e......%5s #g|#n\n\r"
      "#g| #nFirearms#e....%5s #g|  #nExpression#e....%5s #g|  #nCrafts#e........%5s #g|#n\n\r"
      "#g| #nLarceny#e.....%5s #g|  #nIntimidation#e..%5s #g|  #nInvestigation#e.%5s #g|#n\n\r"
      "#g| #nStealth#e.....%5s #g|  #nSocialize#e.....%5s #g|  #nMedicine#e......%5s #g|#n\n\r"
      "#g| #nSurvival#e....%5s #g|  #nStreetwise#e....%5s #g|  #nOccult#e........%5s #g|#n\n\r"
      "#g| #nWeaponry#e....%5s #g|  #nPersuasion#e....%5s #g|  #nPolitics#e......%5s #g|#n\n\r"
      "#g| #nDrive#e.......%5s #g|  #nSubterfuge#e....%5s #g|  #nScience#e.......%5s #g|#n\n\r"
      "#g)-----------------------------------#G(#yP#oools#G)#g-----------------------------------(#n\n\r"
      "#g|                                                                             |#n\n\r"
      "#g|         #yW#oillpower#y:               M#oorality#y:            D#oamage#y T#ootal#y: #n%d       #g|\n\r"
      "#g|#n    %19s      %19s      #nAgg   Lethal  Bash     #g|#n\n\r"
      "#g|#n   %21s                               %d      %d      %d #g      |#n\n\r"
      "#g|                                                                             |#n\n\r"
      "#g|-----------------------------------------------------------------------------|#n\n\r"
      "#g| #nXP#y:#n %-56d Hours#y:#n %-7d #g|#n\n\r"
      "#g'-----------------------------------------------------------------------------'#n\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
      strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice,
	  strlen (ch->pcdata->marriage) < 2 ? "Single" : ch->pcdata->marriage,
      show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
      show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
      show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->bloodline, "Unset")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),

      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE]),
      (ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
      show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_spot(ch->pcdata->clant),
      show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]),
      ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
      ch->exp, ((get_age (ch) - 17) * 2)
   );
   send_to_char(buf, ch);
   return;
}

void ensorcelled_score( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH*2];
   
   if IS_SET (ch->act, PLR_BLIND)
		{
      sprintf(buf, "\n\r"
      "Ensorcelled Score Sheet\n\r\n\r"
      "Name: %s\n\r"
      "Surname: %s\n\r"
      "Virtue: %s\n\r"
      "Vice: %s\n\r"
      "Partner: %s\n\r\n\r"
      "Morality: %d\n\r\n\r"
      "#wPhysical Attributes:#n "
      "Strength %d, Dexterity %d, Stamina %d\n\r"
      "#wSocial Attributes:#n "
      "Presence %d, Manipulation %d, Composure %d\n\r"
      "#wMental Attributes:#n "
      "Resolve %d, Intelligence %d, Wits %d\n\r\n\r"
      "#wPhysical Skills:#n \n\r"
      "Athletics %d, Brawl %d, Firearms %d, Larceny %d, Stealth %d, Survival %d,\n\rWeaponry %d, Drive %d \n\r"
      "#wSocial Skills:#n \n\r"
      "Animal Ken %d, Empathy %d, Expression %d, Intimidation %d, Socialize %d, \n\rStreetwise %d, Persuasion %d, Subterfuge %d \n\r"
      "#wMental Skills:#n \n\r"
      "Academics %d, Computer %d, Crafts %d, Investigation %d, Medicine %d, Occult %d,\n\rPolitics %d, Science %d \n\r\n\r"
      "Damage Totals: \n\r"
      "%d Aggravated, %d Lethal, %d Bashing \n\r\n\r"
      "Current XP: %d\n\r"
      "Hours Played: %d\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
	  strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice,
	  strlen (ch->pcdata->marriage) < 2 ? "Single" : ch->pcdata->marriage,
	  ch->pcdata->clant,
	  ch->attributes[ATTRIB_STR], ch->attributes[ATTRIB_DEX], ch->attributes[ATTRIB_STA],
	  ch->attributes[ATTRIB_CHA], ch->attributes[ATTRIB_MAN], ch->attributes[ATTRIB_APP],
	  ch->attributes[ATTRIB_PER], ch->attributes[ATTRIB_INT], ch->attributes[ATTRIB_WIT],
	  ch->abilities[PHYSICAL][TAL_ATHLETICS],ch->abilities[PHYSICAL][TAL_BRAWL],ch->abilities[PHYSICAL][TAL_FIREARMS],ch->abilities[PHYSICAL][TAL_LARCENY],ch->abilities[PHYSICAL][TAL_STEALTH],ch->abilities[PHYSICAL][TAL_SURVIVAL],ch->abilities[PHYSICAL][TAL_WEAPONRY],ch->abilities[PHYSICAL][TAL_DRIVE],
	  ch->abilities[SOCIAL][SKI_ANIMALKEN],ch->abilities[SOCIAL][SKI_EMPATHY],ch->abilities[SOCIAL][SKI_EXPRESSION],ch->abilities[SOCIAL][SKI_INTIMIDATION],ch->abilities[SOCIAL][SKI_SOCIALIZE],ch->abilities[SOCIAL][SKI_STREETWISE],ch->abilities[SOCIAL][SKI_PERSUASION],ch->abilities[SOCIAL][SKI_SUBTERFUGE],
	  ch->abilities[MENTAL][KNO_ACADEMICS],ch->abilities[MENTAL][KNO_COMPUTER],ch->abilities[MENTAL][KNO_CRAFTS],ch->abilities[MENTAL][KNO_INVESTIGATION],ch->abilities[MENTAL][KNO_MEDICINE],ch->abilities[MENTAL][KNO_OCCULT],ch->abilities[MENTAL][KNO_POLITICS],ch->abilities[MENTAL][KNO_SCIENCE],
	  ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
	  ch->exp, ((get_age (ch) - 17) * 2)
      );
      send_to_char(buf, ch);
      return;
      }
	else

   sprintf(buf, "\n\r"
"#g.-----------------------------------------------------------------------------.#n\n\r"
"#g|#y I#onnocence#y L#oost                                                  #yE#onsorcelled#g |#n\n\r"
"#g|-----------------------------------------------------------------------------|#n\n\r"
      "#g|  #nName  #y:#n %-19s  #nSurname#y:#n %-36s #g|#n\n\r"
      "#g|  #nVirtue#y:#n %-16s        #nVice#y:#n %-36s #g|#n\n\r"
	  "#g|  #nPartner#y:#n %-65s #g|#n\n\r"
      "#g)--------#G(#yP#ohysical#G)#g-----------------#G(#yS#oocial#G)#g----------------#G(#yM#oental#G)#g----------(#n\n\r"
      "#g| #nStrength#e....%9s #g|  #nPresence#e......%9s #g|  #nResolve#e.......%9s #g|#n\n\r"
      "#g| #nDexterity#e...%9s #g|  #nManipulation#e..%9s #g|  #nIntelligence#e..%9s #g|#n\n\r"
      "#g| #nStamina#e.....%9s #g|  #nComposure#e.....%9s #g|  #nWits#e..........%9s #g|#n\n\r"
      "#g)-----------------------------------#G(#yS#okills#G)#g----------------------------------(#n\n\r"
      "#g| #nAthletics#e...%5s #g|  #nAnimal Ken#e....%5s #g|  #nAcademics#e.....%5s #g|#n\n\r"
      "#g| #nBrawl#e.......%5s #g|  #nEmpathy#e.......%5s #g|  #nComputer#e......%5s #g|#n\n\r"
      "#g| #nFirearms#e....%5s #g|  #nExpression#e....%5s #g|  #nCrafts#e........%5s #g|#n\n\r"
      "#g| #nLarceny#e.....%5s #g|  #nIntimidation#e..%5s #g|  #nInvestigation#e.%5s #g|#n\n\r"
      "#g| #nStealth#e.....%5s #g|  #nSocialize#e.....%5s #g|  #nMedicine#e......%5s #g|#n\n\r"
      "#g| #nSurvival#e....%5s #g|  #nStreetwise#e....%5s #g|  #nOccult#e........%5s #g|#n\n\r"
      "#g| #nWeaponry#e....%5s #g|  #nPersuasion#e....%5s #g|  #nPolitics#e......%5s #g|#n\n\r"
      "#g| #nDrive#e.......%5s #g|  #nSubterfuge#e....%5s #g|  #nScience#e.......%5s #g|#n\n\r"
      "#g)-----------------------------------#G(#yP#oools#G)#g-----------------------------------(#n\n\r"
      "#g|                                                                             |#n\n\r"
      "#g|         #yW#oillpower#y:               M#oorality#y:            D#oamage#y T#ootal#y: #n%d       #g|\n\r"
      "#g|#n    %19s      %19s      #nAgg   Lethal  Bash     #g|#n\n\r"
      "#g|#n   %21s                               %d      %d      %d #g      |#n\n\r"
      "#g|                                                                             |#n\n\r"
      "#g|-----------------------------------------------------------------------------|#n\n\r"
      "#g| #nXP#y:#n %-56d Hours#y:#n %-7d #g|#n\n\r"
      "#g'-----------------------------------------------------------------------------'#n\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
      strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice,
	  strlen (ch->pcdata->marriage) < 2 ? "Single" : ch->pcdata->marriage,
      show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
      show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
      show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->bloodline, "Unset")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),

      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE]),
      (ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
      show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_spot(ch->pcdata->clant),
      show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]),
      ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
      ch->exp, ((get_age (ch) - 17) * 2)
   );
   send_to_char(buf, ch);
   return;
}

void geist_score( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH*2];

   sprintf(buf, "\n\r"
"#b   .-----------------------------------------------------------------------.#n\n\r"
"#b  /             #eNew Orleans by Night#b:#e A World of Darkness Story#b             \\#n\n\r"
"#b /---------------------------------------------------------------------------\\#n\n\r"
      "#b/#c  Name  #b:#e %-19s  #cTitle#b:#e %-38s #b\\#n\n\r"
      "#b|#c  Virtue#b:#e %-16s      #cVice#b:#e %-38s #b|#n\n\r"
      "#b|#c  Psyche#b:#e %-15d      #cPlasm#b:#e %-29s                    #b|#n\n\r"
"#b|-----------------------------------------------------------------------------|#n\n\r"
"#b#c    Archetype#b:#e %-2s                   #cThreshold#b:#e   %-2s\n\r"
"#b|-----------------------------------------------------------------------------|#n\n\r"
      "#b}---------------------------------#e/#cAttributes#e/#b--------------------------------{#n\n\r"
      "#b}--------#e/#bPhysical#e/#b-----------------#e/#bSocial#e/#b----------------#e/#bMental#e/#b----------{#n\n\r"
      "#b| #eStrength#c____%9s #b|  #ePresence#c______%9s #b|  #eResolve#c_______%9s #b|#n\n\r"
      "#b| #eDexterity#c___%9s #b|  #eManipulation#c__%9s #b|  #eIntelligence#c__%9s #b|#n\n\r"
      "#b| #eStamina#c_____%9s #b|  #eComposure#c_____%9s #b|  #eWits#c__________%9s #b|#n\n\r"
      "#b}-----------------------------------#e/#cSkills#e/#b----------------------------------{#n\n\r"
      "#b}--------#e/#bP#bhysical#e/#b-----------------#e/#bSocial#e/#b----------------#e/#bM#bental#e/#b----------{#n\n\r"
      "#b| #eAthletics#c___%5s #b|  #eAnimal Ken#c____%5s #b|  #eAcademics#c_____%5s #b|#n\n\r"
      "#b| #eBrawl#c_______%5s #b|  #eEmpathy#c_______%5s #b|  #eComputer#c______%5s #b|#n\n\r"
      "#b| #eFirearms#c____%5s #b|  #eExpression#c____%5s #b|  #eCrafts#c________%5s #b|#n\n\r"
      "#b| #eLarceny#c_____%5s #b|  #eIntimidation#c__%5s #b|  #eInvestigation#c_%5s #b|#n\n\r"
      "#b| #eStealth#c_____%5s #b|  #eSocialize#c_____%5s #b|  #eMedicine#c______%5s #b|#n\n\r"
      "#b| #eSurvival#c____%5s #b|  #eStreetwise#c____%5s #b|  #eOccult#c________%5s #b|#n\n\r"
      "#b| #eWeaponry#c____%5s #b|  #ePersuasion#c____%5s #b|  #ePolitics#c______%5s #b|#n\n\r"
      "#b| #eDrive#c_______%5s #b|  #eSubterfuge#c____%5s #b|  #eScience#c_______%5s #b|#n\n\r"
      "#b}-------------------------------#e/#cOther Traits#e/#b--------------------------------{#n\n\r"
      "#b|                                                                             |#n\n\r"
      "#b|         #cWillpower#b:             #c   Synergy#b:            #cDamage Total#b: #c%d#n       #b|\n\r"
      "#b|#e    %19s      %19s      #nAgg   Lethal  Bash     #b|#n\n\r"
      "#b|#e   %21s     #w                          #b%d      %d      %d #b      |#n\n\r"
      "#b|                                                                             |#n\n\r"
      "#b|-----------------------------------------------------------------------------|#n\n\r"
      "#b\\    #cXP#b:#e%-4d     #cHours#b:#e %-5d      #cIC#b:#e %-5d            Geist: Sin-Eaters    #b /#n\n\r"
      "#b \\---------------------------------------------------------------------------/#n\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->title : "None"),
      strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice, ch->vamppot, show_spot(ch->pcdata->plasm),
      strlen (ch->kithname) < 2 ? "None" : ch->kithname, strlen (ch->courtname) < 2 ? "None" : ch->courtname,
      show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
      show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
      show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->bloodline, "Unset")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),

      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE]),
      (ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
      show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_spot(ch->pcdata->clant),
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
"#r   .-----------------------------------------------------------------------.#n\n\r"
"#r  /             #RNew Orleans by Night: A World of Darkness Story#r             \\#n\n\r"
"#r /---------------------------------------------------------------------------\\#n\n\r"
      "#r/  Name  #R:#n %-19s  #rTitle#R:#n %-38s #r\\#n\n\r"
      "#r|  Virtue#R:#n %-16s      #rVice#R:#n %-38s #r|#n\n\r"
	  "#r|  Kindred Title/Rank#R:#n %-54s #r|#n\n\r"
      "#r|-----------------------------------------------------------------------------#r|#n\n\r"
"  #n #eBloodline#R:#n %-4s    #eClan#R:#n %-7s    #eCovenant#R:#n %-4s    #ePotency#R:#n %-4d#n\n\r"
"#r}---------------------------------#R/#wAttributes#R/#r--------------------------------{#n\n\r"
      "#r}--------#e/#RPhysical#e/#r-----------------#e/#RSocial#e/#r----------------#e/#RMental#e/#r----------{#n\n\r"
      "#r| #eStrength#w____%9s #r|  #ePresence#w______%9s #r|  #eResolve#w_______%9s #r|#n\n\r"
      "#r| #eDexterity#w___%9s #r|  #eManipulation#w__%9s #r|  #eIntelligence#w__%9s #r|#n\n\r"
      "#r| #eStamina#w_____%9s #r|  #eComposure#w_____%9s #r|  #eWits#w__________%9s #r|#n\n\r"
      "#r}-----------------------------------#R/#wSkills#R/#r----------------------------------{#n\n\r"
      "#r}--------#e/#RPhysical#e/#r-----------------#e/#RSocial#e/#r----------------#e/#RMental#e/#r----------{#n\n\r"
      "#r| #eAthletics#w___%5s #r|  #eAnimal Ken#w____%5s #r|  #eAcademics#w_____%5s #r|#n\n\r"
      "#r| #eBrawl#w_______%5s #r|  #eEmpathy#w_______%5s #r|  #eComputer#w______%5s #r|#n\n\r"
      "#r| #eFirearms#w____%5s #r|  #eExpression#w____%5s #r|  #eCrafts#w________%5s #r|#n\n\r"
      "#r| #eLarceny#w_____%5s #r|  #eIntimidation#w__%5s #r|  #eInvestigation#w_%5s #r|#n\n\r"
      "#r| #eStealth#w_____%5s #r|  #eSocialize#w_____%5s #r|  #eMedicine#w______%5s #r|#n\n\r"
      "#r| #eSurvival#w____%5s #r|  #eStreetwise#w____%5s #r|  #eOccult#w________%5s #r|#n\n\r"
      "#r| #eWeaponry#w____%5s #r|  #ePersuasion#w____%5s #r|  #ePolitics#w______%5s #r|#n\n\r"
      "#r| #eDrive#w_______%5s #r|  #eSubterfuge#w____%5s #r|  #eScience#w_______%5s #r|#n\n\r"
      "#r}--------------------------------#R/#wAdvantages#R/#r---------------------------------{\n\r"
      "#r|                            #e====#RDisciplines#e====   #r                           |\n\r",
 ch->name, 
(ch->pcdata) ? ch->pcdata->title : "None",
 strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice, noble_rank_table[ch->pcdata->noble].title_of_rank[ch->sex],
 ch->bloodline, (ch->pcdata->clan != -1) ? clan_names[ch->pcdata->clan][0] : 
"None", strlen (ch->side) < 2 ? "None" : ch->side, ch->vamppot,
 show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
 show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
 show_dot(ch->attributes[ATTRIB_STA]), show_dot((!IS_NPC (ch) && !str_cmp (ch->bloodline, "Unset")) ? 0 : ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),
  
      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE]));
 send_to_char(buf, ch);
col  = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "#eNone#w___________");
				break;
			case DISC_DOMINATE:
				strcpy (disc, "#eDominate#w_______");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "#eAuspex#w_________");
				break;
			case DISC_MAJESTY:
				strcpy (disc, "#eMajesty________#w");
				break;
			case DISC_NIGHTMARE:
				strcpy (disc, "#eNightmare#w______");
				break;
			case DISC_ANIMALISM:
				strcpy (disc, "#eAnimalism#w______");
				break;
			case DISC_CELERITY:
				strcpy (disc, "#eCelerity#w_______");
				break;
			case DISC_RESILIENCE:
				strcpy (disc, "#eResilience#w_____");
				break;
			case DISC_HOURS:
				strcpy (disc, "#eHours#w__________");
				break;
			case DISC_MOON:
				strcpy (disc, "#eMoon#w___________");
				break;
			case DISC_VIGOR:
				strcpy (disc, "#eVigor#w__________");
				break;
			case DISC_FLEETINGWINTER:
				strcpy (disc, "#eFleeting Winter#w");
				break;
			case DISC_MIRROR:
				strcpy (disc, "#eMirror#w_________");
				break;
			case DISC_ANIMATION:
				strcpy (disc, "#eAnimation#w______");
				break;
			case DISC_COMMUNION:
				strcpy (disc, "#eCommunion#w______");
				break;
			case DISC_ETERNALSUMMER:
				strcpy (disc, "#eEternal Summer#w__");
				break;
			case DISC_FORGE:
				strcpy (disc, "#eForge#w__________");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "#eObfuscate#w______");
				break;
			case DISC_DARKNESS:
				strcpy (disc, "#eDarkness#w_______");
				break;
			case DISC_ARTIFICE:
				strcpy (disc, "#eArtifice#w_______");
				break;
			//case DISC_ETERNALSPRING:
			//	strcpy (disc, "#eEternal Spring#w_");
			//	break;
			case DISC_PROTEAN:
				strcpy (disc, "#eProtean#w________");
				break;
			case DISC_FLEETINGSUMMER:
				strcpy (disc, "#eFleeting Summer#w");
				break;
			case DISC_REFLECTIONS:
				strcpy (disc, "#eReflections#w____");
				break;
			case DISC_FLEETINGAUTUMN:
				strcpy (disc, "#eFleeting Autumn#w");
				break;
			case DISC_SHADEANDSPIRIT:
				strcpy (disc, "#eShade & Spirit#w_");
				break;
			case DISC_STONE:
				strcpy (disc, "#eStone#w__________");
				break;
			case DISC_THEBANSORCERY:
				strcpy (disc, "#eTheban Sorcery#w_");
				break;
			case DISC_SEPARATION:
				strcpy (disc, "#eSeparation#w_____");
				break;
			case DISC_FLEETINGSPRING:
				strcpy (disc, "#eFleeting Spring#w");
				break;
			case DISC_VAINGLORY:
				strcpy (disc, "#eVainglory#w______");
				break;
			case DISC_WILD:
				strcpy (disc, "#eWild#w___________");
				break;
			case DISC_TENURE:
				strcpy (disc, "#eTenure#w_________");
				break;
			case DISC_PHAGIA:
				strcpy (disc, "#ePhagia#w_________");
				break;
			//case DISC_ETERNALSUMMER:
			//	strcpy (disc, "#eEternal Summer#w_");
			//	break;
			case DISC_ETERNALWINTER:
				strcpy (disc, "#eEternal Winter#w__");
				break;
			}
			if (ch->class == CLASS_VAMPIRE && (ch->pcdata->bloodlined[0] == sn || ch->pcdata->bloodlined[1] == sn || ch->pcdata->bloodlined[2] == sn))
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
"#r|#R      City Status              Covenant Status             Clan Status       #r|\n\r"
"#r|#e       %9s                  %9s                 %9s        #r|#n\n\r"
"#r|#e                                                                             #r|#n\n\r",  
show_spot_5(ch->pcdata->virtues[VIRTUE_CITYSTATUS]), show_spot_5(ch->pcdata->virtues[VIRTUE_COVSTATUS]), show_spot_5(ch->pcdata->virtues[VIRTUE_CLANSTATUS]));
send_to_char(buf,ch);
sprintf(buf,
"#r}-------------------------------#R/#wOther Traits#R/#r--------------------------------{\n\r");
send_to_char(buf,ch);
sprintf(buf,
"#r|         #nWillpower              #n  Humanity#e             #nDamage Total#e: #R%d#n       #r|\n\r"
"#r|#w    %19s      %19s      #eAgg   Lethal  Bash     #r|#n\n\r"
"#r|#w   %21s     #w                          #R%d      %d      %d #r      |#n\n\r"
"#r|                             #e====#RBlood Pool#e====                              #r|\n\r"
"#r|                   #w%40s                #r|#n\n\r"
"#r|                  #w%40s               #r|#n\n\r",
(ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_spot(ch->pcdata->clant),
show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]),
ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
show_spot_20(ch->blood[BLOOD_POOL]),
show_slash_20(ch->blood[BLOOD_CURRENT]));
send_to_char(buf, ch);
sprintf(buf,
"#r|-----------------------------------------------------------------------------|#n\n\r"
"#r\\#n #RXP#r:#R %-4d   #RHours#r:#R %-5d #RIC#r:#R %-5d  #r                                 #RVampire#r /\n\r",
ch->exp, ((get_age (ch) - 17) * 2), get_hoursIC(ch));
send_to_char(buf,ch);
sprintf(buf,
"#r '---------------------------------------------------------------------------'#n\n\r"
"\n\r");
send_to_char(buf, ch);
return;
}



void shifter_score( CHAR_DATA *ch, char *argument )
{
char buf[MAX_STRING_LENGTH*2];
int col = 0;
int sn;
char disc[30];
bool second = FALSE;

   if IS_SET (ch->act, PLR_BLIND)
		{
      sprintf(buf, "\n\r"
      "Changeling Score Sheet\n\r\n\r"
      "Name: %s\n\r"
      "Surname: %s\n\r"
      "Virtue: %s\n\r"
      "Vice: %s\n\r"
      "Partner: %s\n\r"
	  "Kith: %s\n\r"
	  "Seeming: %s\n\r"
	  "Court: %s\n\r"
	  "Motley: %s\n\r\n\r"
	  
	  "Court Mantle: %d\n\r"
	  "Clarity: %d\n\r"
	  "Glamour: %d/%d\n\r\n\r"
	  
      "#wPhysical Attributes:#n "
      "Strength %d, Dexterity %d, Stamina %d\n\r"
      "#wSocial Attributes:#n "
      "Presence %d, Manipulation %d, Composure %d\n\r"
      "#wMental Attributes:#n "
      "Resolve %d, Intelligence %d, Wits %d\n\r\n\r"
      "#wPhysical Skills:#n \n\r"
      "Athletics %d, Brawl %d, Firearms %d, Larceny %d, Stealth %d, Survival %d,\n\rWeaponry %d, Drive %d \n\r"
      "#wSocial Skills:#n \n\r"
      "Animal Ken %d, Empathy %d, Expression %d, Intimidation %d, Socialize %d, \n\rStreetwise %d, Persuasion %d, Subterfuge %d \n\r"
      "#wMental Skills:#n \n\r"
      "Academics %d, Computer %d, Crafts %d, Investigation %d, Medicine %d, Occult %d,\n\rPolitics %d, Science %d \n\r\n\r"
	  
      "Damage Totals: \n\r"
      "%d Aggravated, %d Lethal, %d Bashing \n\r\n\r"
      "Current XP: %d\n\r"
      "Hours Played: %d\n\r\n\r"
	  "#wContracts:#n\n\r",
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
	  strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice,
	  strlen (ch->pcdata->marriage) < 2 ? "Single" : ch->pcdata->marriage,
	  strlen (ch->kithname) < 2 ? "None" : ch->kithname, (ch->pcdata->clan != -1) ? clan_names[ch->pcdata->clan][0] : "None",
	  strlen (ch->courtname) < 2 ? "None" : ch->courtname,
	  strlen (ch->pcdata->coterie) < 2 ? "None" : ch->pcdata->coterie,
	  ch->pcdata->virtues[VIRTUE_MANTLE],
	  ch->pcdata->clarity[1],
	  ch->blood[BLOOD_POOL],ch->blood[BLOOD_CURRENT],
	  ch->attributes[ATTRIB_STR], ch->attributes[ATTRIB_DEX], ch->attributes[ATTRIB_STA],
	  ch->attributes[ATTRIB_CHA], ch->attributes[ATTRIB_MAN], ch->attributes[ATTRIB_APP],
	  ch->attributes[ATTRIB_PER], ch->attributes[ATTRIB_INT], ch->attributes[ATTRIB_WIT],
	  ch->abilities[PHYSICAL][TAL_ATHLETICS],ch->abilities[PHYSICAL][TAL_BRAWL],ch->abilities[PHYSICAL][TAL_FIREARMS],ch->abilities[PHYSICAL][TAL_LARCENY],ch->abilities[PHYSICAL][TAL_STEALTH],ch->abilities[PHYSICAL][TAL_SURVIVAL],ch->abilities[PHYSICAL][TAL_WEAPONRY],ch->abilities[PHYSICAL][TAL_DRIVE],
	  ch->abilities[SOCIAL][SKI_ANIMALKEN],ch->abilities[SOCIAL][SKI_EMPATHY],ch->abilities[SOCIAL][SKI_EXPRESSION],ch->abilities[SOCIAL][SKI_INTIMIDATION],ch->abilities[SOCIAL][SKI_SOCIALIZE],ch->abilities[SOCIAL][SKI_STREETWISE],ch->abilities[SOCIAL][SKI_PERSUASION],ch->abilities[SOCIAL][SKI_SUBTERFUGE],
	  ch->abilities[MENTAL][KNO_ACADEMICS],ch->abilities[MENTAL][KNO_COMPUTER],ch->abilities[MENTAL][KNO_CRAFTS],ch->abilities[MENTAL][KNO_INVESTIGATION],ch->abilities[MENTAL][KNO_MEDICINE],ch->abilities[MENTAL][KNO_OCCULT],ch->abilities[MENTAL][KNO_POLITICS],ch->abilities[MENTAL][KNO_SCIENCE],
	  ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
	  ch->exp, ((get_age (ch) - 17) * 2));
      send_to_char(buf, ch);
	  
col  = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "#nNone ");
				break;
			case DISC_DOMINATE:
				strcpy (disc, "#nDominate ");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "#nAuspex ");
				break;
			case DISC_MAJESTY:
				strcpy (disc, "#nMajesty ");
				break;
			case DISC_NIGHTMARE:
				strcpy (disc, "#nNightmare ");
				break;
			case DISC_ANIMALISM:
				strcpy (disc, "#nAnimalism ");
				break;
			case DISC_CELERITY:
				strcpy (disc, "#nCelerity ");
				break;
			case DISC_RESILIENCE:
				strcpy (disc, "#nResilience ");
				break;
			case DISC_HOURS:
				strcpy (disc, "#nHours ");
				break;
			case DISC_MOON:
				strcpy (disc, "#nMoon ");
				break;
			case DISC_VIGOR:
				strcpy (disc, "#nVigor ");
				break;
			case DISC_FLEETINGWINTER:
				strcpy (disc, "#nFleeting Winter");
				break;
			case DISC_MIRROR:
				strcpy (disc, "#nMirror ");
				break;
			case DISC_ANIMATION:
				strcpy (disc, "#nAnimation ");
				break;
			case DISC_COMMUNION:
				strcpy (disc, "#nCommunion ");
				break;
			case DISC_FORGE:
				strcpy (disc, "#nForge ");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "#nObfuscate ");
				break;
			case DISC_DARKNESS:
				strcpy (disc, "#nDarkness ");
				break;
			case DISC_ARTIFICE:
				strcpy (disc, "#nArtifice ");
				break;
			case DISC_ETERNALSPRING:
				strcpy (disc, "#nEternal Spring ");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "#nProtean ");
				break;
			case DISC_FLEETINGSUMMER:
				strcpy (disc, "#nFleeting Summer ");
				break;
			case DISC_REFLECTIONS:
				strcpy (disc, "#nReflections ");
				break;
			case DISC_FLEETINGAUTUMN:
				strcpy (disc, "#nFleeting Autumn ");
				break;
			case DISC_SHADEANDSPIRIT:
				strcpy (disc, "#nShade & Spirit ");
				break;
			case DISC_STONE:
				strcpy (disc, "#nStone ");
				break;
			case DISC_THEBANSORCERY:
				strcpy (disc, "#nTheban Sorcery ");
				break;
			case DISC_SEPARATION:
				strcpy (disc, "#nSeparation ");
				break;
			case DISC_FLEETINGSPRING:
				strcpy (disc, "#nFleeting Spring ");
				break;
			case DISC_VAINGLORY:
				strcpy (disc, "#nVainglory ");
				break;
			case DISC_WILD:
				strcpy (disc, "#nWild ");
				break;
			case DISC_TENURE:
				strcpy (disc, "#nTenure ");
				break;
			case DISC_PHAGIA:
				strcpy (disc, "#nPhagia ");
				break;
			case DISC_ETERNALSUMMER:
				strcpy (disc, "#nEternal Summer ");
				break;
			case DISC_ETERNALWINTER:
				strcpy (disc, "#nEternal Winter ");
				break;
			}
			if (ch->class == CLASS_CHANGELING && (ch->pcdata->bloodlined[0] == sn || ch->pcdata->bloodlined[1] == sn || ch->pcdata->bloodlined[2] == sn))
				capitalize (disc);
			if (!col)
			{
			sprintf (buf, "%s%d", disc, (get_disc(ch, sn)));
			send_to_char (buf, ch);
			col++;
			second = FALSE;
			}
			else
			{
			sprintf (buf, ", %s%d\n\r", disc, (get_disc(ch, sn)));
			send_to_char (buf, ch);
			 col--;
			second = TRUE;
		        }

	}
if (!second)
send_to_char("#n \n\r", ch);

//send_to_char(buf, ch);
return;
}
	else

sprintf(buf,
"\n\r"
"#g.-----------------------------------------------------------------------------.#n\n\r"
"#g|#y I#onnocence#y L#oost                                                         C#y:#otL #g|#n\n\r"
"#g|-----------------------------------------------------------------------------|#n\n\r"
"#g|#n  Name  #y :#n %-22s #nWyrd#y   :#n %-34d#g|#n\n\r"
"#g|#n  Virtue#y :#n %-22s #nVice#y   :#n %-34s#g|#n\n\r"
"#g|#n  Kith#y   :#n %-22s #nSeeming#y:#n %-34s#g|#n\n\r"
"#g|  #nPartner#y:#n %-22s #nMotley#y :#n %-34s#g|#n\n\r"
"#g|-----------------------------------------------------------------------------|#n\n\r"
"#g)--------#G(#yP#ohysical#G)#g-----------------#G(#yS#oocial#G)#g----------------#G(#yM#oental#G)#g----------(#n\n\r"
"#g| #nStrength#e....%9s #g|  #nPresence#e......%9s #g|  #nResolve#e.......%9s #g|#n\n\r"
"#g| #nDexterity#e...%9s #g|  #nManipulation#e..%9s #g|  #nIntelligence#e..%9s #g|#n\n\r"
"#g| #nStamina#e.....%9s #g|  #nComposure#e.....%9s #g|  #nWits#e..........%9s #g|#n\n\r"
"#g)-----------------------------------#G(#yS#okills#G)#g----------------------------------(#n\n\r"
"#g| #nAthletics#e...%5s #g|  #nAnimal Ken#e....%5s #g|  #nAcademics#e.....%5s #g|#n\n\r"
"#g| #nBrawl#e.......%5s #g|  #nEmpathy#e.......%5s #g|  #nComputer#e......%5s #g|#n\n\r"
"#g| #nFirearms#e....%5s #g|  #nExpression#e....%5s #g|  #nCrafts#e........%5s #g|#n\n\r"
"#g| #nLarceny#e.....%5s #g|  #nIntimidation#e..%5s #g|  #nInvestigation#e.%5s #g|#n\n\r"
"#g| #nStealth#e.....%5s #g|  #nSocialize#e.....%5s #g|  #nMedicine#e......%5s #g|#n\n\r"
"#g| #nSurvival#e....%5s #g|  #nStreetwise#e....%5s #g|  #nOccult#e........%5s #g|#n\n\r"
"#g| #nWeaponry#e....%5s #g|  #nPersuasion#e....%5s #g|  #nPolitics#e......%5s #g|#n\n\r"
"#g| #nDrive#e.......%5s #g|  #nSubterfuge#e....%5s #g|  #nScience#e.......%5s #g|#n\n\r"
"#g)-----------------------------------------------------------------------------(\n\r"
"#g|#n Court#y:#n %-50s Mantle#y:#n %-29s #g|#n\n\r"
"#g)---------------------------------#G(#yC#oontracts#G)#g---------------------------------(#n\n\r",  
 
 ch->name, ch->vamppot,
 strlen (ch->virtue) < 2 ? "None" : ch->virtue, strlen (ch->vice) < 2 ? "None" : ch->vice,
 strlen (ch->kithname) < 2 ? "None" : ch->kithname, (ch->pcdata->clan != -1) ? clan_names[ch->pcdata->clan][0] : "None",
 strlen (ch->pcdata->marriage) < 2 ? "Single" : ch->pcdata->marriage,
 strlen (ch->pcdata->coterie) < 2 ? "None" : ch->pcdata->coterie,
 show_dot(ch->attributes[ATTRIB_STR]), show_dot(ch->attributes[ATTRIB_CHA]), show_dot(ch->attributes[ATTRIB_PER]),
 show_dot(ch->attributes[ATTRIB_DEX]), show_dot(ch->attributes[ATTRIB_MAN]), show_dot(ch->attributes[ATTRIB_INT]),
 show_dot(ch->attributes[ATTRIB_STA]), show_dot(ch->attributes[ATTRIB_APP]), show_dot(ch->attributes[ATTRIB_WIT]),
  
      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE]),
	  strlen (ch->courtname) < 2 ? "None" : ch->courtname,
      show_spot_5(ch->pcdata->virtues[VIRTUE_MANTLE]));
	 
 send_to_char(buf, ch);
col  = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "#nNone#e...........");
				break;
			case DISC_DOMINATE:
				strcpy (disc, "#nDominate#e.......");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "#nAuspex#e.........");
				break;
			case DISC_MAJESTY:
				strcpy (disc, "#nMajesty#e........");
				break;
			case DISC_NIGHTMARE:
				strcpy (disc, "#nNightmare#e......");
				break;
			case DISC_ANIMALISM:
				strcpy (disc, "#nAnimalism#e......");
				break;
			case DISC_CELERITY:
				strcpy (disc, "#nCelerity......#e.");
				break;
			case DISC_RESILIENCE:
				strcpy (disc, "#nResilience#e.....");
				break;
			case DISC_HOURS:
				strcpy (disc, "#nHours#e..........");
				break;
			case DISC_MOON:
				strcpy (disc, "#nMoon#e...........");
				break;
			case DISC_VIGOR:
				strcpy (disc, "#nVigor#e..........");
				break;
			case DISC_FLEETINGWINTER:
				strcpy (disc, "#nFleeting Winter#e");
				break;
			case DISC_MIRROR:
				strcpy (disc, "#nMirror#e.........");
				break;
			case DISC_ANIMATION:
				strcpy (disc, "#nAnimation#e......");
				break;
			case DISC_COMMUNION:
				strcpy (disc, "#nCommunion#e......");
				break;
			case DISC_FORGE:
				strcpy (disc, "#nForge#e..........");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "#nObfuscate#e......");
				break;
			case DISC_DARKNESS:
				strcpy (disc, "#nDarkness#e.......");
				break;
			case DISC_ARTIFICE:
				strcpy (disc, "#nArtifice#e.......");
				break;
			case DISC_ETERNALSPRING:
				strcpy (disc, "#nEternal Spring#e.");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "#nProtean#e........");
				break;
			case DISC_FLEETINGSUMMER:
				strcpy (disc, "#nFleeting Summer#e");
				break;
			case DISC_REFLECTIONS:
				strcpy (disc, "#nReflections#e....");
				break;
			case DISC_FLEETINGAUTUMN:
				strcpy (disc, "#nFleeting Autumn#e");
				break;
			case DISC_SHADEANDSPIRIT:
				strcpy (disc, "#nShade & Spirit#e.");
				break;
			case DISC_STONE:
				strcpy (disc, "#nStone#e..........");
				break;
			case DISC_THEBANSORCERY:
				strcpy (disc, "#nTheban Sorcery#e.");
				break;
			case DISC_SEPARATION:
				strcpy (disc, "#nSeparation#e.....");
				break;
			case DISC_FLEETINGSPRING:
				strcpy (disc, "#nFleeting Spring#e");
				break;
			case DISC_VAINGLORY:
				strcpy (disc, "#nVainglory#e......");
				break;
			case DISC_WILD:
				strcpy (disc, "#nWild#e...........");
				break;
			case DISC_TENURE:
				strcpy (disc, "#nTenure#e.........");
				break;
			case DISC_PHAGIA:
				strcpy (disc, "#nPhagia#e.........");
				break;
			case DISC_ETERNALSUMMER:
				strcpy (disc, "#nEternal Summer#e.");
				break;
			case DISC_ETERNALWINTER:
				strcpy (disc, "#nEternal Winter#e.");
				break;
			}
			if (ch->class == CLASS_CHANGELING && (ch->pcdata->bloodlined[0] == sn || ch->pcdata->bloodlined[1] == sn || ch->pcdata->bloodlined[2] == sn))
				capitalize (disc);
			if (!col)
			{
			send_to_char("#g|             ", ch);
			sprintf (buf, "%s.%9s#n #g|#n   ", disc, show_dot (get_disc(ch, sn)));
			send_to_char (buf, ch);
			col++;
			second = FALSE;
			}
			else
			{
			sprintf (buf, "%s.%9s#n         #g|#n\n\r", disc, show_dot (get_disc(ch, sn)));
			send_to_char (buf, ch);
			 col--;
			second = TRUE;
		        }

	}
if (!second)
send_to_char("                                  #g|#n\n\r", ch);

if (IS_CHANGELING(ch))
{
sprintf(buf,
"#g)------------------------------------#G(#yP#oools#G)#g----------------------------------(\n\r"
"#g|              =====#yW#oillpower#g=====          ==#yD#oamage#y T#ootal#y: #n%d#g===#n              #g|#n\n\r"
"#g|              #n%19s           Agg   Lethal  Bash               #g|#n\n\r" 
"#g|             #n%21s           #n%d      %d      %d                 #g|#n\n\r"       
"#g|                                                                            #g |\n\r"
"#g|                              #g=====#yG#olamour#g=====                              #g|#n\n\r"
"#g|                   #w%40s                #g|#n\n\r"
"#g|                  #w%40s               #g|#n\n\r"
"#g|                             ======#yC#olarity#g======                             #g|#n\n\r"
"#g|                             #n%19s                             #g|#n\n\r",
(ch->pcdata->aggdamage + ch->pcdata->bashingdamage + ch->pcdata->lethaldamage),
show_spot(ch->pcdata->willpower[WILLPOWER_MAX]), show_slash(ch->pcdata->willpower[WILLPOWER_CURRENT]), 
ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage,
show_spot_20(ch->blood[BLOOD_POOL]),show_slash_20(ch->blood[BLOOD_CURRENT]), show_spot(ch->pcdata->clarity[1]));
send_to_char(buf,ch);
}


sprintf(buf,
"#g|-----------------------------------------------------------------------------|#n\n\r"
"#g| #nXP#y:#n %-56d Hours#y: #n%-7d #g|#n\n\r",
ch->exp, ((get_age (ch) - 17) * 2));
send_to_char(buf,ch);
sprintf(buf,
"#g'-----------------------------------------------------------------------------'#n\n\r"
"#n                 See #y'#nchanstats#y'#n for common Changeling rolls.\n\r"
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
 sprintf(buf, ".........");
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

char * display_extra(CHAR_DATA *ch) {
	EXTRA *extra;
	EXTRA *catagory;
	bool stop = FALSE;
	char *hold[50];
	int i;
	char buf[MSL * 2];
	char *temp2;
	char temp[MSL];
	temp[0] = '\0';
	buf[0] = '\0';
	temp2 = str_dup("");
	for (i = 0; i < 20; i++)
		hold[i] = str_dup("");

	if (IS_NPC(ch))
		return NULL;
	strcat(buf,
			"#g.---------------------( #wMerits and Specialties #g)--------------------.#n\n\r");

	strcat(buf,
			"#g|                                                                   #g|#n\n\r#g|    (#w Merits#g )                                                     |#n\n\r");

	for (i = 0; i < 10; i++) {
		if (ch->pcdata->merits[i] != NULL) {
			sprintf(temp, "#g|    * #n%-60s #g|#n\r\n", ch->pcdata->merits[i]);
			strcat(buf, temp);
		}

	}
	sprintf(temp,
			"#g|                                                                   |#n\r\n");
	strcat(buf, temp);

	strcat(buf,
			"#g|    (#w Specialties#g )                                                |#n\n\r");
	for (i = 0; i < 10; i++) {
		if (ch->pcdata->spec[i] != NULL) {
			sprintf(temp, "#g|    * #n%-60s #g|#n\r\n", ch->pcdata->spec[i]);
			strcat(buf, temp);
		}

	}
	strcat(buf,
			"#g|                                                                   |#n\n\r#g)-----------------------------( #wDetails #g)---------------------------(#n\n\r#g|                                                                   #g|#n\n\r");

	strcat(buf,
			"#g|    (#w Fighting Styles#g )                                            |#m\n\r");

	for (i = 0; i < 10; i++) {
		if (ch->pcdata->style[i] != NULL) {
			sprintf(temp, "#g|    * #n%-60s #g|#n\r\n", ch->pcdata->style[i]);
			strcat(buf, temp);
		}

	}
	sprintf(temp,
			"#g|                                                                   |#n\r\n");
	strcat(buf, temp);

	strcat(buf,
			"#g|    (#w Equipment#g )                                                  |#n\n\r");
	for (i = 0; i < 10; i++) {
		if (ch->pcdata->enote[i] != NULL) {
			sprintf(temp, "#g|    * #n%-60s #g|#n\r\n", ch->pcdata->enote[i]);
			strcat(buf, temp);
		}

	}

	sprintf(temp,
			"#g|                                                                   |#n\r\n");
	strcat(buf, temp);

	strcat(buf,
			"#g|    (#w Pledges#g )                                                    |#n\n\r");
	for (i = 0; i < 10; i++) {
		if (ch->pcdata->pnote[i] != NULL) {
			sprintf(temp, "#g|    * #n%-60s #g|#n\r\n", ch->pcdata->pnote[i]);
			strcat(buf, temp);
		}

	}

	if (!(extra = ch->pcdata->extra)) {
		strcat(buf,
				"#g|                                                                   |#n\n\r#g'-------------------------------------------------------------------'#n\n\r"); //None
		temp2 = str_dup(buf);
		return temp2;
	}

	strcat(buf,
			"#g|                                                                   |#n\n\r#g)-----------------------------( #wAddenda #g)---------------------------(#n\n\r");
	for (catagory = ch->pcdata->extra; catagory; catagory = catagory->prev) {
		for (i = 0; i < 10; i++) {
			if (!str_cmp(catagory->catagory, hold[i]))
				break;

			if (!str_cmp(hold[i], "")) {
				stop = TRUE;
				break;
			}

		}
		if (stop) {
			hold[i] = str_dup(catagory->catagory);
			sprintf(temp, "\n\r#g     (#n %s#g )#n\n\r", catagory->catagory);
			strcat(buf, temp);
			stop = FALSE;
		}
		if (catagory->rating != -1)
			sprintf(temp, "     #g* #n%s#G %d#n\n\r", catagory->field,
					catagory->rating);
		else
			sprintf(temp, "     #g* #n%s#n\n\r", catagory->field);
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
	 
	sprintf( buf, "#gThe year is #G2017#g.#n\n\r");	 

	  send_to_char( buf, ch );
	 sprintf( buf,"\n\r#GInnocence Lost#g started up at #G%s#n\r#gThe system time is#G %s#n\r",
	str_boot_time,(char *) ctime( &current_time ));
	 send_to_char( buf, ch );

	 return;
}

void do_help( CHAR_DATA *ch, char *arg )
{
    HELP_DATA *help;
    HELP_DATA *help_short = NULL;
    char help_short_key [MAX_INPUT_LENGTH];
    char helplist [MAX_STRING_LENGTH]; /* it could overflow... */
    char key [MAX_INPUT_LENGTH];
    char *keylist;
    bool found = FALSE;
    bool many = FALSE;

    if ( *arg == '\0' )
        arg = "summary"; /* I suggest summaryxxx or so to prevent shortcuts */

    helplist[0] = '\0';
    
    for ( help = help_first; help; help = help->next )
    {
        if ( help->level > get_trust( ch ) )
            continue;

        /* is_name sucks ;) */
        
        for ( keylist = help->keyword; ; )
        {
            keylist = one_argument( keylist, key );
            if ( key[0] == '\0' || !str_prefix( arg, key ) )
                break;
        }
        
        if ( key[0] == '\0' )
            continue;
        
        if ( !str_cmp( arg, key ) )
        {
        /* the keyword is exactly like argument */
            if ( found )
                send_to_char( "\n\r#g--------------------------------------------------------------------------------#n\n\r\n\r", ch );

            if ( help->level >= 0 && str_cmp( arg, "imotd" ) )
            {
                send_to_char( help->keyword, ch );
                send_to_char( "\n\r", ch );
            }

            if ( help->text[0] == '.' )
                send_to_char( help->text+1, ch );
            else
                send_to_char( help->text  , ch );
            found = TRUE;
        }
        else
        {
        /* short */
            if ( !help_short && !many )
            {
                help_short = help;
                strcpy( help_short_key, key );
            }
            else
            {
                if ( help_short )
                {
                    strcat( helplist, help_short_key );
                    strcat( helplist, "\n\r" );
                    help_short = NULL;
                }
                strcat( helplist, key );
                strcat( helplist, "\n\r" );
                many = TRUE;
            }
        }
    }

    if ( !found && !many && !help_short )
    {
        send_to_char( "#y[#gHelpSys#y]#n No help on that word.\n\r", ch );
        return;
    }
    if ( found )
    {
        if ( help_short )
        {
            /* I suggest making this line green */
            send_to_char( "\n\r#y[#gHelpSys#y]#n The argument is also a prefix of keyword:\n\r", ch );
            send_to_char( help_short_key, ch );
            send_to_char( "\n\r", ch );
        }
        else if ( many )
        {
            /* colour this line too */
            send_to_char( "\n\r#y[#gHelpSys#y]#n The argument is also a prefix of keywords:\n\r", ch );
            send_to_char( helplist, ch );
        }
    }
    else if ( help_short )
    {
        if ( help_short->level >= 0 && str_cmp( help_short_key, "imotd" ) )
        {
            send_to_char( help_short->keyword, ch );
            send_to_char( "\n\r", ch );
        }

        if ( help_short->text[0] == '.' )
            send_to_char( help_short->text+1, ch );
        else
            send_to_char( help_short->text  , ch );
    }
    else if ( many )
    {
        /* This line also should be coloured */
        send_to_char( "#y[#gHelpSys#y]#n The word you entered is a prefix of help keywords:\n\r", ch );
        send_to_char( helplist, ch );
    }

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
			strcat (class, "#e     [#CA#GF#PK#e] #n");
		else if (IS_SET (wch->extra2, EXTRA2_DND))
			strcat (class, "#e     [#RDND#e] #n");

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

				strcat (class, "#sPlayer#n");
				break;
				}
				break;
			case MAX_LEVEL - 10:
				strcat (class, "#wMortal#n");
				break;
			case MAX_LEVEL - 11:
				strcat (class, "#yNewbie#n");
				break;
			case MAX_LEVEL - 12:
				strcat (class, "#yNewbie#n");
				break;
			}
		/*
		 * Format it up.
		 */
		if (IS_VAMPIRE (wch))
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}
		else if (IS_GHOUL (wch))
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}
		else if (IS_CHANGELING (wch))
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}
		else if (IS_WEREWOLF (wch))
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}
	  else if (IS_GEIST (wch))
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}
		else if (IS_ENSORCELLED (wch))
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}

		else
		{
			strcpy (openb, "#n");
			strcpy (closeb, "#n");
		}
		{

			if (IS_GHOUL (wch))
				sprintf (kav, " %s#n%s", openb, closeb);
			else if (IS_VAMPIRE (wch) && wch->vamppot == 1)
				sprintf (kav, " %s#n%s", openb, closeb);
			else if (IS_CHANGELING (wch) && wch->vamppot == 1)
				sprintf (kav, " %s#n%s", openb, closeb);
			/* Geist */
			else if (IS_GEIST (wch) && wch->vamppot == 0)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 1)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 2)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
		  else if (IS_GEIST (wch) && wch->vamppot == 3)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 4)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 5)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 6)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 7)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 8)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			else if (IS_GEIST (wch) && wch->vamppot == 9)
				sprintf (kav, " %s#cSin-Eater#n%s", openb, closeb);
			/* End Geist */
			/* Ensorcelled */
		  else if (IS_ENSORCELLED (wch) && wch->vamppot == 0)
				sprintf (kav, " %s#c(Ensorcelled)#n%s", openb, closeb);
			/* End Ensorcelled */
			else if (IS_VAMPIRE (wch))
			{
				bool use_age = FALSE;
				char bloodlinename[80];

				if (strlen (wch->bloodline) < 2)
					use_age = TRUE;
				else if (wch->vamppot == 2)
					sprintf (kav, " %s%s", openb, closeb);
				else if (IS_EXTRA (wch, EXTRA_PRINCE))
					sprintf (kav, " %s%s", openb, closeb);
				else
					use_age = TRUE;
				if (use_age)
				{
					if (strlen (wch->bloodline) < 2)
						strcpy (bloodlinename, "Vampire");
					else
						strcpy (bloodlinename, wch->bloodline);
					switch (wch->vamppot)
					{
					default:
						sprintf (kav, " %s#e%s#n%s", openb, bloodlinename, closeb);
						break;
					case 8:
						sprintf (kav, " %s#e%s#n%s", openb, bloodlinename, closeb);
						break;
					case 9:
						sprintf (kav, " %s#e%s Vampire#n%s", openb, bloodlinename, closeb);
						break;
					case 10:
						sprintf (kav, " %s#b%s Vampire#n%s", openb, bloodlinename, closeb);
						break;
					case 11:
						sprintf (kav, " %s#b%s Antediluvian#n%s", openb, bloodlinename, closeb);
						break;
					case 12:
						sprintf (kav, " %s#b%s Vampire#n%s", openb, bloodlinename, closeb);
						break;
					case 13:
						sprintf (kav, " %s#b The First#n%s", openb, closeb);
						break;
					}
				}
			}
			else if (IS_CHANGELING (wch))
			{
				if (strlen (wch->bloodline) > 1)
				{
					if (wch->vamppot == 2)
						sprintf (kav, " %s%s#n%s", openb, wch->bloodline, closeb);
					else if (IS_EXTRA (wch, EXTRA_PRINCE))
						sprintf (kav, " %s%s#n%s", openb, wch->bloodline, closeb);
					else
						sprintf (kav, " %s%s#n%s", openb, wch->bloodline, closeb);
				}
				else
					sprintf (kav, " %s#n%s", openb, closeb);
			}
			else if (IS_WEREWOLF (wch))
                        {
                                if (strlen (wch->bloodline) > 1)
                                {
                                        if (wch->vamppot == 2)
                                                sprintf (kav, " %s#G%s Chief#c%s", openb, wch->bloodline, closeb);
                                        else if (IS_EXTRA (wch, EXTRA_PRINCE))
                                                sprintf (kav, " %s#G%s Shaman#c%s", openb, wch->bloodline, closeb);
                                        else
                                                sprintf (kav, " %s#G%s#n%s", openb, wch->bloodline, closeb);
                                }
                                else
                                        sprintf (kav, " %s#G%s#n%s", openb, wch->bloodline, closeb);
                        }

			else
				sprintf (kav, "  %s#B%s#n%s", openb, wch->bloodline, closeb);
		}

		
		
		if (IS_SET (wch->extra2, EXTRA2_AFK))
		    strcpy (sex_text, "#n");
		else if (wch->level < 6 && wch->pcdata->pretitle[0] != '\0')
			strcpy(sex_text, wch->pcdata->pretitle);
		else if (wch->sex == SEX_FEMALE)
			strcpy (sex_text, "#n");
		else if (wch->sex == SEX_MALE)
			strcpy (sex_text, "#n");    
		else
			strcpy (sex_text, "#n");

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

		if ((IS_MORE2 (wch, MORE2_RPFLAG))) 
			strcpy (rpflag, "#c(IC)#n");
		else
			strcpy (rpflag, " ");

		if (IS_MORE2 (wch, MORE2_RPFLAG))
			wholistinchar++;

		//Construct title
		if ((!IS_IMMORTAL(wch) && !strstr(wch->pcdata->title, wch->name)))
			sprintf(title, "#n   %s %s %s", wch->name, wch->pcdata->surname, wch->pcdata->title);
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
				sprintf (buf_mor + strlen (buf_mor), " %-8s#n %s%s %s\n\r", sex_text, title, kav, rpflag);
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
				sprintf (buf_mor + strlen (buf_mor), " %-8s#n %s %s\n\r", sex_text, title, rpflag);
			}
		}
	}
send_to_char ("#n                                     #g._-_,  #n         ... ...#y           '  _  '#n\r\n", ch);
send_to_char ("#w  Innocence Lost                   #g-=_#o* #g=-  #n...  ... :.: : :#y          - #o (_)#y  -#n\r\n", ch);
send_to_char ("#g  Los Angeles by Night             #o,'/''#g\\   #n:  ''  : ... : :,:   .::.#n\r\n", ch);
send_to_char ("#n                                  #o//        #n: :'': : : : : :.'   :  :#e'#y  '   '#n", ch);
send_to_char ("\n\r#g-#w Currently Online #g--------------#o//#g---------#n:.:__:.:_:.:_:.:':#g--#n':..:#g-----------\r\n\r\n", ch);
	send_to_char (buf_mor, ch);
	send_to_char ("#n\r\n", ch);
	send_to_char (buf_imm, ch);
	if (nTotal > most_login)
        most_login = nTotal;

	sprintf (buf, "\r\n#g--------------------------------------------------------------------------------#n\n\r#g  There are #n%d#g people online, and #n%d#g since the last reboot.#n\n\r", nTotal, most_login );
	send_to_char (buf, ch);
	return;
}



void do_inventory (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *vehicle;
	OBJ_DATA *vehicle_next;
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

		case ITEM_VEHICLE:
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
			for (vehicle = ch->in_room->contents; vehicle != NULL; vehicle = vehicle_next)
			{
				vehicle_next = vehicle->next_content;
				if ((obj->value[0] == vehicle->value[3]) && (obj->value[3] == vehicle->value[0]))
				{
					found = TRUE;
					if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_SET (vehicle->extra_flags, ITEM_SHADOWPLANE))
					{
						REMOVE_BIT (ch->affected_by, AFF_SHADOWPLANE);
						do_look (ch, "auto");
						SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
						break;
					}
					else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_SET (vehicle->extra_flags, ITEM_SHADOWPLANE))
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
	send_to_char ("You are carrying:\n\r\n\r", ch);
	show_list_to_char_total (ch->carrying, ch, TRUE, TRUE, FALSE);
	return;
}

void do_worth (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	
	sprintf (buf, "#gYou are carrying #G%d #gdollars.#n\n\r", ch->pounds);
	send_to_char (buf, ch);
}

void do_equipment (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	int iWear;
	bool found;

	send_to_char ("#nYou are using:#n\n\r", ch);
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
                sprintf(buf, "#eYour auras are currently#o:#w %s\n\r", ch->pcdata->aura);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#eYour auras have been cleared.#n\r\n", ch);
                free_string (ch->pcdata->aura);
                ch->pcdata->aura = NULL;
                return;
        }
        else if (strlen (argument) > MAX_AURA_LENGTH)
        {
                send_to_char ("#eLine too long.#n\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->aura = str_dup (argument);
                sprintf (buf, "#eYour auras are now '%s'#n\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}

/* Finger element - Surname */

void do_surname (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

	if ((!IS_NPC (ch) && !IS_SET (ch->in_room->room_flags, ROOM_CHARGEN)))
	{
	send_to_char ("You cannot set your surname outside of CharGen.\n\r", ch);
	return;
	}
	
    if (!strcmp(argument,"clear"))
    {
        send_to_char ("#y[#gCharGen#y]#n Your surname has been cleared.#n\r\n", ch);
        free_string (ch->pcdata->surname);
        ch->pcdata->surname = NULL;
        return;
    }
	
	if ((ch->pcdata->surname != NULL))
	{
		send_to_char ("#y[#gCharGen#y]#n You already have a surname.\n\r", ch);
		return;
	}
	
    if (argument[0] == '\0')
    {
        sprintf(buf, "#y[#gCharGen#y]#n Your surname is #w%s#n.#n\n\r", ch->pcdata->surname);
        send_to_char(buf, ch);
        return;
    }
	
    if (strlen (argument) > MAX_SURNAME_LENGTH)
    {
        send_to_char ("#y[#gCharGen#y]#n That is too long for a surname.\n\r", ch);
        return;
    }

    else
	{
		ch->pcdata->surname = str_dup (argument);
        sprintf (buf, "#y[#gCharGen#y]#n Surname set to #w%s#n.\r\n", argument);
        send_to_char (buf, ch);
        return;
	}
}
/* End of Surname test */

/* Finger element - Wiki Page */

void do_wiki (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#g[#G Your Wiki Page is: %s #g]#n\n\r", ch->pcdata->wiki);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#g[#G Your Wiki Page has been cleared. #g]#n\r\n", ch);
                free_string (ch->pcdata->wiki);
                ch->pcdata->wiki = NULL;
                return;
        }
        else if (strlen (argument) > MAX_WIKI_LENGTH)
        {
                send_to_char ("#eLine too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->wiki = str_dup (argument);
                sprintf (buf, "#g[#G Your Wiki Page: %s #g]#n\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}
/* End of Wiki Page finger element */

/* Finger element - RP Preferences */

void do_pref (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#r[#R Your RP Preferences are: %s #r]#n\n\r", ch->pcdata->pref);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#r[#R Your RP Preferences have been cleared. #r]#n\r\n", ch);
                free_string (ch->pcdata->pref);
                ch->pcdata->pref = NULL;
                return;
        }
        else if (strlen (argument) > MAX_PREF_LENGTH)
        {
                send_to_char ("#eLine too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->pref = str_dup (argument);
                sprintf (buf, "#r[#R Your RP Preferences: %s #r]#n\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}
/* End of RP Preferences finger element */

/* Finger element - Reputation */

void do_reputation (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#p[#P Your IC Reputation is: %s #p]#n\n\r", ch->pcdata->reputation);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#p[#P Your Reputation has been deleted. #p]#n\r\n", ch);
                free_string (ch->pcdata->reputation);
                ch->pcdata->reputation = NULL;
                return;
        }
        else if (strlen (argument) > MAX_REPUTATION_LENGTH)
        {
                send_to_char ("#eLine too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->reputation = str_dup (argument);
                sprintf (buf, "#p[#P Your Reputation: %s #p]#n\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}
/* End of Reputation finger element */

void do_voice (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;

        if (argument[0] == '\0')
        {
                sprintf(buf, "#eYour voice is currently#o:#w %s\n\r", ch->pcdata->voice);
                send_to_char(buf, ch);
                return;
        }
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#eYour voice has been cleared.\r\n", ch);
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
                sprintf (buf, "#eYour voice is now '#w%s#e'\r\n", argument);
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
                sprintf(buf, "Your room description is:#w %s#n\n\r", ch->pcdata->roomdesc);
                send_to_char(buf, ch);
                return;
        }
        /*
        else if (!strcmp(argument,"clear"))
        {
                send_to_char ("#eYour room description has been cleared.\r\n", ch);
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
                sprintf (buf, "You set your roomdesc to: #w%s#n\r\n", argument);
                send_to_char (buf, ch);
                return;
	}
}

void do_miendesc (CHAR_DATA * ch, char *argument)
{
        char buf[MAX_STRING_LENGTH];

        if (IS_NPC (ch))
                return;
       	if (!IS_CHANGELING(ch))
	      {
	     	send_to_char ("Only Changelings may set a short description for Mien.", ch);
	     	return;
	      }

        if (argument[0] == '\0')
        {
                sprintf(buf, "Your Mien short description is:#w %s#n\n\r", ch->pcdata->miendesc);
                send_to_char(buf, ch);
                return;
        }
        else if (strlen (argument) > MAX_MIENDESC_LENGTH)
        {
                send_to_char ("Line too long.\n\r", ch);
                return;
        }

        else
	{
		ch->pcdata->miendesc = str_dup (argument);
                sprintf (buf, "Your set your Mien short description to: #w%s#n\r\n", argument);
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
		sprintf(buf, "#nYour doing/action is:#w %s#e (Clear it with 'doing clear')#n\n\r", ch->pcdata->doing);
		send_to_char(buf, ch);
		return;
	}
	else if (!strcmp(argument,"clear"))
	{
		send_to_char ("Your doing/action has been cleared.\r\n", ch);
                free_string (ch->pcdata->doing);
                ch->pcdata->doing = NULL;
		return;
	}
	else
	{
		ch->pcdata->doing = str_dup (argument);
		sprintf (buf, "You set your doing/action to: #w%s#e (Clear it with 'doing clear')#n\r\n", argument);
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

		
		send_to_char ("#rChannels#R:#                                                 #n\n\r", ch);
		
		send_to_char (!IS_SET  (ch->deaf, CHANNEL_ADVICE) ? "#s +ADVICE" : "#s -advice", ch);

		send_to_char (!IS_SET (ch->deaf, CHANNEL_OMOTE) ? "#s +OMOTE" : "#s -omote", ch);		

		send_to_char (!IS_SET (ch->deaf, CHANNEL_OSAY) ? "#s +OSAY" : "#s -osay", ch);

		send_to_char (!IS_SET (ch->deaf, CHANNEL_TELL) ? "#s +TELL" : "#s -tell", ch);

		send_to_char (!IS_SET (ch->deaf, CHANNEL_INFO) ? "#s +INFO" : "#s -info", ch);

		send_to_char(!IS_SET  (ch->deaf, CHANNEL_CELL) ? "#s +CELL"  : "#s -cell", ch); 
		
		send_to_char(!IS_SET  (ch->deaf, CHANNEL_SHOUT) ? "#s +SHOUT" : "#s -shout", ch);
		
		send_to_char(!IS_SET  (ch->deaf, CHANNEL_YELL) ? "#s +YELL" : "#s -yell", ch);
		
		send_to_char(!IS_SET  (ch->deaf, CHANNEL_HINT) ? "#s +HINT" : "#s -hint", ch);

		if (IS_IMMORTAL (ch))
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_STORYINFORM) ? "#w +STORYINFORM" : "#w -storyinform", ch);
		}

		if (IS_STORYTELLER (ch))
		{
			send_to_char (!IS_SET (ch->deaf, CHANNEL_STORY) ? "#B +STORY" : "#B -story", ch);
			send_to_char (!IS_SET (ch->deaf, CHANNEL_SMOTE) ? "#B +SMOTE" : "#B -smote", ch);
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


		send_to_char ("\n\r\n\r#rNote#R:#n -all or +all will set or clear all your channels", ch);

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
		else if (!str_cmp (arg + 1, "advice"))
			bit = CHANNEL_ADVICE;
		else if (IS_IMMORTAL (ch) && !str_cmp (arg + 1, "storyinform"))
			bit = CHANNEL_STORYINFORM;
		else if (IS_STORYTELLER (ch) && !str_cmp (arg + 1, "story"))
			bit = CHANNEL_STORY;
	    else if (IS_IMMORTAL (ch) && !str_cmp (arg + 1, "smote"))
			bit = CHANNEL_SMOTE;
		else if (!str_cmp (arg + 1, "osay"))
			bit = CHANNEL_OSAY;
 		else if (!str_cmp (arg + 1, "omote"))
            bit = CHANNEL_OMOTE;
		else if (IS_IMMORTAL (ch) && !str_cmp (arg + 1, "immtalk"))
			bit = CHANNEL_IMMTALK;
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
		else if (!str_cmp (arg + 1, "howl"))
			bit = CHANNEL_HOWL;
		else if (!str_cmp (arg + 1, "hint"))
			bit = CHANNEL_HINT;
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
				REMOVE_BIT (ch->deaf, CHANNEL_ADVICE);
				REMOVE_BIT (ch->deaf, CHANNEL_OMOTE);
				REMOVE_BIT (ch->deaf, CHANNEL_OSAY);
				REMOVE_BIT (ch->deaf, CHANNEL_INFO);
				REMOVE_BIT (ch->deaf, CHANNEL_TELL);
				REMOVE_BIT (ch->deaf, CHANNEL_CELL);
				REMOVE_BIT (ch->deaf, CHANNEL_SHOUT);
				REMOVE_BIT (ch->deaf, CHANNEL_YELL);
                REMOVE_BIT (ch->deaf, CHANNEL_HOWL);
				REMOVE_BIT (ch->deaf, CHANNEL_HINT);
				if (IS_IMMORTAL (ch))
				{
					REMOVE_BIT (ch->deaf, CHANNEL_STORYINFORM);
					REMOVE_BIT (ch->deaf, CHANNEL_IMMTALK);
				}
				if (IS_STORYTELLER (ch))
				{
					REMOVE_BIT (ch->deaf, CHANNEL_LOG_DICE);
					REMOVE_BIT (ch->deaf, CHANNEL_STORY);
					REMOVE_BIT (ch->deaf, CHANNEL_SMOTE);
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
				SET_BIT (ch->deaf, CHANNEL_ADVICE);
				SET_BIT (ch->deaf, CHANNEL_OMOTE);
				SET_BIT (ch->deaf, CHANNEL_OSAY);
				SET_BIT (ch->deaf, CHANNEL_INFO);
				SET_BIT (ch->deaf, CHANNEL_TELL);
				SET_BIT (ch->deaf, CHANNEL_CELL);
				SET_BIT (ch->deaf, CHANNEL_SHOUT);
				SET_BIT (ch->deaf, CHANNEL_YELL);
				SET_BIT (ch->deaf, CHANNEL_HOWL);
				SET_BIT (ch->deaf, CHANNEL_HINT);
				if (IS_IMMORTAL (ch))
				{
					SET_BIT (ch->deaf, CHANNEL_STORYINFORM);
					SET_BIT (ch->deaf, CHANNEL_IMMTALK);
				}
				if (IS_STORYTELLER (ch))
				{
					SET_BIT (ch->deaf, CHANNEL_STORY);
					SET_BIT (ch->deaf, CHANNEL_SMOTE);
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

			send_to_char ("#eOk.\n\r", ch);
			return;
		}
		else
		{
			send_to_char ("#eSet or clear which channel?\n\r", ch);
			return;
		}

		if (fClear)
			REMOVE_BIT (ch->deaf, bit);
		else
			SET_BIT (ch->deaf, bit);

		send_to_char ("#eOk.\n\r", ch);
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
		send_to_char ("#g[#n Keyword  #g]#n Option\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_ANSI) ? "#g[#w+ANSI     #g]#n You have ansi colour #Con#n.\n\r" : "#g[#w-ansi     #g]#n You have ansi colour #eoff#n.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_AUTOEXIT) ? "#g[#w+AUTOEXIT #g]#n Seeing exits automatically is #Con#n.\n\r" : "#g[#w-autoexit #g]#n Seeing exits automatically is #eoff#n.\n\r", ch);
		
		send_to_char (IS_SET (ch->act, PLR_BLIND) ? "#g[#w+BLIND    #g]#n Vision disabled score sheet is #Con#n.\n\r" : "#g[#w-blind    #g]#n Vision disabled score sheet is #eoff#n.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_BLANK) ? "#g[#w+BLANK    #g]#n The blank line before your prompt is #Con#n.\n\r" : "#g[#w-blank    #g]#n The blank line before your prompt is #eoff#n.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_BRIEF) ? "#g[#w+BRIEF    #g]#n Brief descriptions are #Con#n.\n\r" : "#g[#w-brief    #g]#n Brief descriptions are #eoff#n.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_COMBINE) ? "#g[#w+COMBINE  #g]#n Object lists in combined format.\n\r" : "#g[#w-combine  #g]#n Object lists in single format.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_PROMPT) ? "#g[#w+PROMPT   #g]#n Prompt is turned #Con#n.\n\r" : "#g[#w-prompt   #g]#n Prompt is turned #eoff#n.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_TELNET_GA) ? "#g[#w+TELNETGA #g]#n Telnet GA sequence is #Con#n.\n\r" : "#g[#w-telnetga #g]#n Telnet GA sequence is #eoff#n.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_SILENCE) ? "#g[#w+SILENCE  #g]#n You are silenced.\n\r" : "", ch);

		send_to_char (!IS_SET (ch->act, PLR_NO_EMOTE) ? "" : "#g[#w-emote    #g]#n You can't emote.\n\r", ch);

		send_to_char (!IS_SET (ch->act, PLR_NO_TELL) ? "" : "#g[#w-tell     #g]#n You can't use 'tell'.\n\r", ch);

		send_to_char (IS_SET (ch->act, PLR_EXTRAS) ? "#g[#w+EXTRA    #g]#n Extra fields under score #Con#n.\n\r" : "#g[#w-extra    #g]#n Extra fields under score #eoff#n.\n\r", ch);

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
		else if (!str_cmp (arg + 1, "blind"))
			bit = PLR_BLIND;
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

// Blind score sheet

void do_blind (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_BLIND)) 
		do_config (ch, "-blind");
	else
		do_config (ch, "+blind");
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

	sprintf (buf, "You weigh %d pounds and are worth %d pounds.\n\r", obj->weight, obj->cost);
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

	case ITEM_PHONE:
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
			send_to_char ("But you already have customized prompt on!\n\r", ch);
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
			send_to_char ("But you already have customized prompt off!\n\r", ch);
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

	if (IS_NPC (ch))
		return;

	if (IS_SET (ch->extra2, EXTRA2_AFK))
	{
		send_to_char ("#gYou are already AFK.#n.\n\r", ch);
		return;
	}

	if (!IS_SET (ch->extra2, EXTRA2_AFK))
	{
		send_to_char ("#gYou're AFK. Come back soon! \n\r", ch);
		SET_BIT (ch->extra2, EXTRA2_AFK);
		return;
	}

	return;
}


/*void do_clan (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int i = 0;
	int clan = -1;

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (ch->pcdata->clan != -1)
	{
		sprintf (buf, "You follow the clan of %s.\n\r", clan_names[ch->pcdata->clan][0]);
		send_to_char (buf, ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose a clan:\n\r", ch);
		strcpy (buf, clan_names[0][0]);
		for (i = 1; i < MAX_CLANS; i++)
		{
			strcat (buf, " ");
			strcat (buf, clan_names[i][0]);
		}
		strcat (buf, "\n\r");
		send_to_char (buf, ch);
		return;
	}

	for (i = 0; i < MAX_CLANS; i++)
	{
		if (!str_cmp (arg, clan_names[i][0]))
		{
			clan = i;
			break;
		}
	}

	if (clan == -1)
	{
		send_to_char ("Unknown Clan, Try Again\n\r", ch);
		return;
	}

	if (clan == CLAN_NOSFERATU)
	{
		if (!str_cmp (ch->bloodline, "toreador"))
		{
			send_to_char ("Your bloodline cannot choose Via Bestiae.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Bestiae.\n\r", ch);
	}

	if (clan == CLAN_BEAST && !str_cmp (ch->bloodline, "Assamite"))
	{
		send_to_char ("You now seek Golconda Via Sanguinius.\n\r", ch);
	}
	else if (clan == CLAN_BEAST)
	{
		send_to_char ("Only Assamite can follow Via Sanguinius.\n\r", ch);
		return;
	}

	if (clan == CLAN_ELEMENTAL && ( !str_cmp (ch->bloodline, "Cappadocian") ||
		!str_cmp (ch->bloodline, "Tzimisce") ) )
	{
		send_to_char ("You now seek Golconda Via Ossium.\n\r", ch);
	}
	else if (clan == CLAN_ELEMENTAL)
	{
		send_to_char ("Only Cappadocians can follow Via Ossium.\n\r", ch);
		return;
	}
	if (clan == CLAN_OGRE)
	{
		send_to_char ("You now seek Golconda Via Noctis.\n\r", ch);
	}
	if (clan == CLAN_GHOUL && ( !str_cmp (ch->bloodline, "Tzimisce") ) )
	{
		send_to_char ("You now seek Golconda Via Mutationis.\n\r", ch);
	}
	else if (clan == CLAN_GHOUL)
	{
		send_to_char ("Only Tzimisce can follow Via Mutationis.\n\r", ch);
		return;
	}

	if (clan == CLAN_VENTRUE)
	{
		send_to_char ("You now seek Golconda Via Regalis.\n\r", ch);
	}

	if (clan == CLAN_FAIREST)
	{
		send_to_char( "You now seek Goldconda Via Einherjar.\n\r",ch);
	}
	if (clan == CLAN_DAEVA)
	{
		if (!str_cmp (ch->bloodline, "salubri"))
		{
			send_to_char ("Your bloodline cannot choose Via Peccati.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Peccati.\n\r", ch);
	}
	if (clan == CLAN_DARKLING)
	{
		if (!str_cmp (ch->bloodline, "baali"))
		{
			send_to_char ("Your bloodline cannot choose Via Caeli.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Caeli.\n\r", ch);
	}
	if (clan == CLAN_HUMANITY)
	{
		if (!str_cmp (ch->bloodline, "baali"))
		{
			send_to_char ("Your bloodline cannot choose Via Humanitatis.\n\r", ch);
			return;
		}
		send_to_char ("You now seek Golconda Via Humanitatis.\n\r", ch);
	}
	if (clan == CLAN_GANGREL && !str_cmp (ch->bloodline, "Ravnos"))
	{
		send_to_char ("You now seek Golconda Via Paradocis.\n\r", ch);
	}
	else if (clan == CLAN_GANGREL)
	{
		send_to_char ("Only Ravnos can follow Via Paradocis.\n\r", ch);
		return;
	}

	if (clan == CLAN_MEKHET)
	{
		send_to_char ("You now seek Golconda Via Reflections.\n\r", ch);
	}


	ch->pcdata->clan = clan;
	ch->pcdata->clant = 1;
	sprintf (buf, "Your Seeming is set to %s.", clan_names[clan][0]);
	send_to_char (buf, ch);
	return;
}*/



void do_urhandesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_WEREWOLF(ch))
	{
		send_to_char ("Only werewolves may use this", ch);
		return;
	}

	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->urhandesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->urhandesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your Urhan description is:\n\r", ch);
		send_to_char (ch->pcdata->urhandesc ? ch->pcdata->urhandesc : "(None).\n\r", ch);
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
		send_to_char ("Your temporary description is:\n\r", ch);
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

void do_mlongdesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_CHANGELING(ch))
	{
		send_to_char ("Only Changelings may set a long description for Mien.", ch);
		return;
	}

	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->mlongdesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->mlongdesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your long Mien description is:\n\r", ch);
		send_to_char (ch->pcdata->mlongdesc ? ch->pcdata->mlongdesc : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Description Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   mlongdesc show\n\r", ch);
		send_to_char ("          mlongdesc edit\n\r", ch);
		send_to_char ("          mlongdesc append\n\r", ch);
	}
	return;
}

void do_gaurudesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_WEREWOLF(ch))
	{
		send_to_char ("Only werewolves may use this", ch);
		return;
	}

	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->gaurudesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->gaurudesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your Gauru description is:\n\r", ch);
		send_to_char (ch->pcdata->gaurudesc ? ch->pcdata->gaurudesc : "(None).\n\r", ch);
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


void do_urshuldesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_WEREWOLF(ch))
	{
		send_to_char ("Only werewolves may use this", ch);
		return;
	}


	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->urshuldesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->urshuldesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your Urshul description is:\n\r", ch);
		send_to_char (ch->pcdata->urshuldesc ? ch->pcdata->urshuldesc : "(None).\n\r", ch);
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



void do_daludesc (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (!IS_WEREWOLF(ch))
	{
		send_to_char ("Only werewolves may use this", ch);
		return;
	}


	if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->daludesc);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->daludesc);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your Dalu description is:\n\r", ch);
		send_to_char (ch->pcdata->daludesc ? ch->pcdata->daludesc : "(None).\n\r", ch);
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

	ch->pcdata->po = i;
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
		send_to_char ("#pYour actions are now #nO#eut#e of#n C#eharacter#p.#n\n\r", ch);
		REMOVE_BIT (ch->more2, MORE2_RPFLAG);
		if (IS_AFFECTED (ch, AFF_POLYMORPH))
			sprintf (buf, "#w%s#e is now #pOut of Character.#n\n\r", ch->morph);
		else
			sprintf (buf, "#w%s#e is now #pOut of Character.#n\n\r", ch->name);
		do_info (ch, buf);
		ch->playedIC += (int) (current_time - ch->rpflag_time);
		return;
	}
	send_to_char ("#pYou are now #nI#en#n C#eharacter#p, any actions you make will be taken as such.#n\n\r", ch);
	SET_BIT (ch->more2, MORE2_RPFLAG);
	
	if (IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (buf, "#w%s#e is now#p In Character.#n\n\r", ch->morph);
	else
		sprintf (buf, "#w%s#e is now #pIn Character.#n\n\r", ch->name);
	do_info (ch, buf);
	ch->rpflag_time = current_time;
	return;
}

/*
void do_hours (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	sprintf(buf, "#bHours online#e: #n%d#n\n\r", (get_age(ch) - 17) * 2);
	send_to_char(buf, ch);
	sprintf(buf, "#bHours IC#e: #n%d#n\n\r", get_hoursIC(ch));
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
	send_to_char ("\n\r#nPlaces with characters who have toggled#w RPVIS#n on:", ch);
	send_to_char ("\n\r#GWho              Where#n", ch);
	send_to_char ("\n\r#g=================================================#n\r\n", ch);
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
		if ((IS_MORE3(victim, MORE3_OBFUS3)) && !IS_STORYTELLER(ch))
			continue;
		if ((IS_MORE2 (victim, MORE2_RPVIS)) && !IS_STORYTELLER(ch))
			continue;
		
				if (!str_cmp(ch->name, victim->name))
				{
					sprintf(buf, "#w%-16s #n%10s\r\n", ch->name, ch->in_room->name);
					send_to_char(buf, ch);
				}
				else
				{
					temp_mem = memory_search_real_name(ch->memory, victim->name);

					if (IS_STORYTELLER(ch))
						{
						sprintf(buf, "#w%-16s #n%10s\r\n", victim->name, victim->in_room->name);
						send_to_char(buf, ch);
					}
					
					else if (IS_MORE2 (victim, MORE2_RPVIS))
					{
						sprintf(buf, "#eHidden           Unknown Location#n\r\n");
						send_to_char(buf, ch);
					}
					
					else if	(temp_mem == NULL)
					{
						sprintf(buf, "#w%-16s #n%10s\r\n", victim->name, victim->in_room->name);
						send_to_char(buf, ch);
					}
					else
					{
						sprintf(buf, "#w%-16s #n%10s\r\n", temp_mem->remembered_name, victim->in_room->name);
						send_to_char(buf, ch);
					}
//					sprintf (buf, "#w%s #n- #eRoom#o:#b %10s\n\r",
//					victim->name, victim->in_room->name);
//					send_to_char (buf, ch);
//					rpTotal++;
				}
				rpTotal++;
			
	}	
	if (IS_NPC (ch))
	return;
	send_to_char ("#g=================================================#n\r\n", ch);
}

void do_rpvis (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_MORE2 (ch, MORE2_RPVIS))
	{
		send_to_char ("#eYou are now #Cvisible#e on the #nW#where#nRP#e list.#n\n\r", ch);
		REMOVE_BIT (ch->more2, MORE2_RPVIS);
		return;
	}
	send_to_char ("#eYou are #Cinvisible#e on the #nW#where#nRP#e list.#n\n\r", ch);
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
	if (!IS_CHANGELING (victim))
	{
		send_to_char ("Only on Changelings", ch);
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

void do_smoke1 (CHAR_DATA * ch, char *argument)
{
	
	char buf[MAX_STRING_LENGTH];
	
	if (IS_NPC (ch))
		return;
	/* Still need if Changeling and if disc is right level*/
	if (!IS_CHANGELING (ch))
	{
		send_to_char ("#eYou have to be a Changeling to use this.#n\n\r", ch);
		return;
	}
		
	if (ch->pcdata->powers[DISC_OBFUSCATE] < 5)
	{
		send_to_char ("#eYou must have Smoke at level 5 to use this.#n\n\r", ch);
		return;
	}
		
	if (IS_MORE3 (ch, MORE3_OBFUS1))
	{
		send_to_char ("#P(#pSmoke Dectivated#P) #eYou are no longer invisible.#n\n\r", ch);
		REMOVE_BIT (ch->more3, MORE3_OBFUS1);
		sprintf( buf, "#e$n appears suddenly from the shadows.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
		return;
	}
	send_to_char ("#P(#pSmoke Activated#P) #eYou are now invisible.#n\n\r", ch);
	SET_BIT (ch->more3, MORE3_OBFUS1);
	sprintf( buf, "#e$n moves to the shadows and disappears.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
	return;
}

void do_smoke3 (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	
	if (IS_NPC (ch))
		return;
	/* Still need if Changeling and if Contract is right level*/
	if (!IS_CHANGELING (ch))
	{
		send_to_char ("#eYou must be a Changeling to use #CS#cmoke#e Contracts#e.#n\n\r", ch);
		return;
	}
		
/*	if (ch->pcdata->powers[DISC_OBFUSCATE] < 5)
	{
		send_to_char ("#eYou are not skilled enough in #CS#cmoke#e to do that.#n\n\r", ch);
		return;
	}*/
		
	if (IS_MORE3 (ch, MORE3_OBFUS3))
	{
		send_to_char ("#C(#cLightshy Deactivated#C) #eYou are no longer invisible.#n\n\r", ch);
		REMOVE_BIT (ch->more3, MORE3_OBFUS3);
		sprintf( buf, "#e$n appears suddenly from the shadows.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
		return;
	}
	send_to_char ("#C(#cLightshy Activated#C) #eYou can now move about invisible.#n\n\r", ch);
	SET_BIT (ch->more3, MORE3_OBFUS3);
	sprintf( buf, "#e$n moves to the shadows and disappears.#n\n\r" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
	return;
}

void do_v (CHAR_DATA * ch, char *argument)
{
	send_to_char ("#r8===>#n\n\r", ch);
	return;
}

void do_wpburn (CHAR_DATA * ch, char *argument)
{
char buf[MAX_STRING_LENGTH];
	if (ch->pcdata->willpower[WILLPOWER_CURRENT] > 1){
		send_to_char ("#eYou burn #n1#o w#yillpower#e.#n\n\r", ch);
		sprintf( buf, "#n$n#e has burned a #ow#yillpower#e.#n" );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pcdata->willpower[WILLPOWER_CURRENT] -= 0;	
			return;
			}
	send_to_char ("#RYou dont have enough willpower to burn.\n\r", ch);
	return;
}

void do_fishslap (CHAR_DATA * ch, char *argument)
	{
	char arg1[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	argument = one_argument (argument, arg1);
	
	if (IS_NPC (ch))	
		return;
	
	if (str_cmp (ch->name,"Merry")){
		send_to_char ("#cOnly #CMerry#c has access to the fishslap command!#n\n\r", ch);
		return;
	}
	
	if (arg1[0] == '\0')
	{
		send_to_char ("#CSyntax#c:#C fishslap playername#n\n\r", ch);
		
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("#cThey have to be around for you to fishslap them. Sorry, Lady.#n\n\r", ch);
		return;
	}
	
		send_to_char ("#GMerry firmly slaps you across the face with a trout!#n\n\r", victim);
		for (d = descriptor_list; d != NULL; d = d->next)
			{
				if (d->connected == CON_PLAYING)
				{
					sprintf(buf, "#GMerry firmly slaps %s with a trout!#n\n\r", arg1);
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
	
    if ( !IS_SET(ch->in_room->room_flags, ROOM_CLOTHES))
	{
	send_to_char("You cannot buy anything here.\n\r", ch);
    return;
	}
	
	if (arg1[0] == '\0')
		{
            send_to_char ("Use \"purchase list\" to see what you can buy.\n\r", ch);
			return;
		}
	
	if (!strcmp (arg1, "list"))
		{
			send_to_char ("#g[#Gkeyword#g]      [#GItem#g]#n\n\r", ch);
			send_to_char ("#g------------------------------------------------------------------------Clothes-#n\n\r", ch);
			send_to_char ("   jeans #g[#G 30#g]#n a pair of blue jeans with some noticeable wear on the knees\n\r", ch);
			send_to_char ("   shirt #g[#G 11#g]#n a simple white tee shirt with a single pocket on the breast\n\r", ch);
			send_to_char ("   shoes #g[#G 20#g]#n a pair of generic white sneakers tying up with long lacings\n\r", ch);
			send_to_char ("  gloves #g[#G  9#g]#n a pair of plain work gloves with light ribbing on the cuffs\n\r", ch);
			send_to_char ("     cap #g[#G  9#g]#n a common english flatcap with a fading, herringbone pattern\n\r", ch);
			send_to_char ("    belt #g[#G 15#g]#n a plain black leather belt snugly fastened around the waist\n\r", ch);
			send_to_char ("    coat #g[#G 45#g]#n an unremarkable black woolen peacoat with big round buttons\n\r", ch);
			send_to_char ("     bag #g[#G 20#g]#n a spacious canvas messenger bag with a broad shoulder strap\n\r", ch);
			send_to_char ("#g--------------------------------------------------------------------Electronics-#n\n\r", ch);
			send_to_char ("   phone #g[#G250#g]#n a generic cell phone protected within a gray Otter Box case\n\r", ch);
			send_to_char ("  tablet #g[#G300#g]#n a slim touch-screen tablet with phone and wireless capacity\n\r", ch);
			send_to_char ("#g------------------------------------------------------------------------Jewelry-#n\n\r", ch);
			send_to_char ("necklace #g[#G 50#g]#n a necklace of delicate silver links clasped behind the nape\n\r", ch);
			send_to_char ("    ring #g[#G 45#g]#n a slender ring of silver fitting securely around the finger\n\r", ch);
			send_to_char ("   watch #g[#G 35#g]#n a simple wristwatch with some scuffs upon the face and band\n\r", ch);
			send_to_char ("#g--------------------------------------------------------------------------Books-#n\n\r", ch);
			send_to_char ("notebook #g[#G  8#g]#n a lightweight spiral-bound notebook with a dark green cover\n\r", ch);
			send_to_char ("   paper #g[#G  1#g]#n a single sheet of clean writing paper with three slim holes\n\r", ch);
			send_to_char (" perfume #g[#G 40#g]#n a dainty bottle of perfume with a sophisticated, rosy scent\n\r", ch);
			send_to_char (" cologne #g[#G 40#g]#n a squared bottle of cologne with a refined, masculine scent\n\r", ch);
			send_to_char ("#g--------------------------------------------------------------------Accessories-#n\n\r", ch);
			send_to_char ("  condom #g[#G  2#g]#n a single Trojan condom wrapped in sleek foil-like packaging\n\r", ch);
			send_to_char ("    mask #g[#G 14#g]#n a generic domino mask made of cheaply patterned faux-velvet\n\r", ch);
			send_to_char ("  tattoo #g[#G 55#g]#n a plain, black-inked tattoo worked in slim sinuous patterns\n\r", ch);
			send_to_char ("umbrella #g[#G 17#g]#n a large black umbrella situated on a curving ashwood handle\n\r", ch);
			send_to_char ("#g--------------------------------------------------------------------------------#n\n\r", ch);
			return;
		}
		
	if (!strcmp (arg1, "perfume")){
			if ( ch->pounds < 40 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20179 ), 1), ch );
			send_to_char("You purchase a bottle of perfume.\n\r", ch);
			sprintf (buf, "%s purchases a bottle of perfume.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pounds -= 40;
			return;		
		}
		
	if (!strcmp (arg1, "condom")){
			if ( ch->pounds < 2 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 46 ), 1), ch );
			send_to_char("You purchase a single-packaged condom.\n\r", ch);
			sprintf (buf, "%s purchases a single-packaged condom.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pounds -= 2;
			return;		
		}
		
	if (!strcmp (arg1, "cologne")){
			if ( ch->pounds < 40 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20180 ), 1), ch );
			send_to_char("You purchase a bottle of cologne.\n\r", ch);
			sprintf (buf, "%s purchases a bottle of cologne.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pounds -= 40;
			return;		
		}
		
	if (!strcmp (arg1, "notebook")){
			if ( ch->pounds < 8 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 41 ), 1), ch );
			send_to_char("You purchase a notebook.\n\r", ch);
			sprintf (buf, "%s purchases a notebook.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 8;
			return;		
		}

	if (!strcmp (arg1, "paper")){
			if ( ch->pounds < 1 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 42 ), 1), ch );
			send_to_char("You purchase a sheet of paper.\n\r", ch);
			sprintf (buf, "%s purchases a sheet of paper.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 1;
			return;		
		}
			
	if (!strcmp (arg1, "shoes")){
			if ( ch->pounds < 20 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3358 ), 1), ch );
			send_to_char("You purchase a pair of shoes.\n\r", ch);
			sprintf (buf, "%s purchases a pair of shoes.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 20;
			return;		
		}
		
	if (!strcmp (arg1, "phone")){
			if ( ch->pounds < 250 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 40 ), 1), ch );
			send_to_char("You purchase a new cell phone.\n\r", ch);
			sprintf (buf, "%s purchases a new cell phone.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 250;
			return;		
		}
		
	if (!strcmp (arg1, "tablet")){
			if ( ch->pounds < 300 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 43 ), 1), ch );
			send_to_char("You purchase a touch-screen tablet.\n\r", ch);
			sprintf (buf, "%s purchases a touch-screen tablet.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 300;
			return;		
		}

	if (!strcmp (arg1, "jeans")){
			if ( ch->pounds < 30 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 1011 ), 1), ch );
			send_to_char("You purchase a pair of blue jeans.\n\r", ch);
			sprintf (buf, "%s purchases a pair of blue jeans.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 30;
			return;		
		}
		
	if (!strcmp (arg1, "shirt")){
			if ( ch->pounds < 11 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 1008 ), 1), ch );
			send_to_char("You purchase a plain white tee shirt.\n\r", ch);
			sprintf (buf, "%s purchases a plain white tee shirt.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 11;
			return;		
		}
		
	if (!strcmp (arg1, "gloves")){
			if ( ch->pounds < 9 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 1016 ), 1), ch );
			send_to_char("You purchase a pair of work gloves.\n\r", ch);
			sprintf (buf, "%s purchases a pair of work gloves.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 9;
			return;		
		}
	if (!strcmp (arg1, "cap")){
			if ( ch->pounds < 9 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3706 ), 1), ch );
			send_to_char("You purchase a simple flatcap.\n\r", ch);
			sprintf (buf, "%s purchases a simple flatcap.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 9;
			return;		
		}	
	if (!strcmp (arg1, "belt")){
			if ( ch->pounds < 15 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3362 ), 1), ch );
			send_to_char("You purchase a leather belt.\n\r", ch);
			sprintf (buf, "%s purchases a leather belt.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 15;
			return;		
		}
	if (!strcmp (arg1, "bag")){
			if ( ch->pounds < 20 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3032 ), 1), ch );
			send_to_char("You purchase a messenger bag.\n\r", ch);
			sprintf (buf, "%s purchases a messenger bag.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 20;
			return;		
		}
		
	if (!strcmp (arg1, "coat")){
			if ( ch->pounds < 45 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3361 ), 1), ch );
			send_to_char("You purchase a woolen peacoat.\n\r", ch);
			sprintf (buf, "%s purchases a woolen peacoat.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 45;
			return;		
		}
		
	if (!strcmp (arg1, "necklace")){
			if ( ch->pounds < 50 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 9563 ), 1), ch );
			send_to_char("You purchase a necklace.\n\r", ch);
			sprintf (buf, "%s purchases a necklace.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 50;
			return;		
		}

	if (!strcmp (arg1, "ring")){
			if ( ch->pounds < 45 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3364 ), 1), ch );
			send_to_char("You purchase a ring.\n\r", ch);
			sprintf (buf, "%s purchases a ring.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 45;
			return;		
		}
		
	if (!strcmp (arg1, "tattoo")){
			if ( ch->pounds < 55 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 1010 ), 1), ch );
			send_to_char("You purchase a design for a tattoo.\n\r", ch);
			sprintf (buf, "%s purchases a design for a tattoo.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 55;
			return;
	   }
	   
	if (!strcmp (arg1, "mask")){
			if ( ch->pounds < 14 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 1012 ), 1), ch );
			send_to_char("You purchase a domino mask.\n\r", ch);
			sprintf (buf, "%s purchases a domino mask.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 14;
			return;
	   }

	if (!strcmp (arg1, "umbrella")){
			if ( ch->pounds < 17 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 1002 ), 1), ch );
			send_to_char("You purchase an umbrella.\n\r", ch);
			sprintf (buf, "%s purchases an umbrella.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 17;
			return;		
		}

	if (!strcmp (arg1, "watch")){
			if ( ch->pounds < 35 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 3363 ), 1), ch );
			send_to_char("You purchase a watch.\n\r", ch);
			sprintf (buf, "%s purchases a watch.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 35;
			return;
	   }
			
	send_to_char ("There is nothing for sale here by that name.\n\r", ch);
}


void do_order (CHAR_DATA * ch, char *argument)
	{
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	argument = one_argument (argument, arg1);
	
	if (IS_NPC (ch))	
		return;
	
    if ( !IS_SET(ch->in_room->room_flags, ROOM_BAR))
	{
	send_to_char("You cannot order anything here.\n\r", ch);
    return;
	}
	
	if (arg1[0] == '\0')
		{
            send_to_char ("Use \"order list\" to see what kinds of drinks are available.\n\r", ch);
			return;
		}
	
	if (!strcmp (arg1, "list"))
		{
			send_to_char ("#o[#ykeyword#o]      [#yDrink#o]#n\n\r", ch);
			send_to_char ("#o---------------------------------------------------------------------------Beer-#n\n\r", ch);
			send_to_char ("  amstel#o [#y  3#o]#n a pint glass of crisp Amstel beer with a thick, frothy head\n\r", ch);
			send_to_char ("guinness#o [#y  4#o]#n a tall bottle of dry Irish stout with Guinness on the label\n\r", ch);
			send_to_char ("   leffe#o [#y  5#o]#n a heavy chalice glass of potent, honey-colored Leffe Blonde\n\r", ch);
			send_to_char ("#o---------------------------------------------------------------------------Wine-#n\n\r", ch);
			send_to_char (" barossa#o [#y  4#o]#n a light glass flute of full-bodied, red Barossa Valley wine\n\r", ch);
			send_to_char ("larchago#o [#y  4#o]#n a slender fluted glass of vibrant white Larchago rioja wine\n\r", ch);
			send_to_char (" miradou#o [#y  4#o]#n a slender glass flute of pink, refreshing Miradou rose wine\n\r", ch);
			send_to_char ("#o-------------------------------------------------------------------------Liquor-#n\n\r", ch);
			send_to_char (" whiskey#o [#y  3#o]#n a chilled shooter glass of fragrant, golden-colored whiskey\n\r", ch);
			send_to_char ("  brandy#o [#y  4#o]#n a curvaceous snifter three-quarters filled with warm brandy\n\r", ch);
			send_to_char ("     rum#o [#y  2#o]#n a short sugar-rimmed shot glass of heavily spiced brown rum\n\r", ch);
			send_to_char (" martini#o [#y  7#o]#n a tall martini glass garnished with an olive on a toothpick\n\r", ch);
			send_to_char ("#o---------------------------------------------------------------------------Soft-#n\n\r", ch);
			send_to_char ("    coke#o [#y  1#o]#n a tall, chilled glass of Coca-Cola served over chips of ice\n\r", ch);
			send_to_char ("   water#o [#y  1#o]#n a clean glass of clear, cold water served over chips of ice\n\r", ch);
			if ((!IS_NPC (ch) && IS_CHANGELING (ch) && IS_SET (ch->in_room->room_flags, ROOM_CBAR) && IS_SET (ch->in_room->room_flags, ROOM_BAR)) || (IS_ENSORCELLED (ch)))
			{
			send_to_char ("#o---------------------------------------------------------------------Flanagan's-#n\n\r", ch);
			send_to_char ("   blush#o [#y  6#o]#n a glass of light, hoppy ale flavored with pink blushberries\n\r", ch); // Spring - Hum flag - Ale
			//send_to_char ("   dream#o [#y  6#o]#n a glass of golden-coloured, refreshing Dream-a-Drupe lambic\n\r", ch); // Summer - Hum flag - Beer
			//send_to_char ("  ertwen#o [#y  6#o]#n a glass of hearty, toasted bock flavored with Ertwen leaves\n\r", ch); // Fall - Hum flag - Stout
			//send_to_char ("  murmur#o [#y  6#o]#n a frosted glass of rich, dark stout with a tea-like flavour\n\r", ch); // Winter - Hum flag - Stout
			send_to_char ("  chuchu#o [#y  6#o]#n a chalice pint glass filled with black, animate Chu Chu Gin\n\r", ch); // Magic flag - Gin
			send_to_char ("  cousin#o [#y  6#o]#n a glass flute filled with an intoxicating iridescent liquor\n\r", ch); // Magic flag - Liquor
			send_to_char ("    coco#o [#y  6#o]#n a warmed snifter of dark, heavy rum threaded with Cocorange\n\r", ch); // Magic flag -- Rum
			send_to_char ("  nettle#o [#y  6#o]#n a salt-dusted shooter glass of sharply crisp Coupnettle gin\n\r", ch); // Magic flag -- Gin
			send_to_char ("   gourd#o [#y  6#o]#n a pumpkin and Headgourd whiskey served with a leafy garnish\n\r", ch); // Magic flag -- Whiskey
			}
			send_to_char ("#o--------------------------------------------------------------------------------#n\n\r", ch);
			return;
		}

	if (ch->pcdata->canbuy < 1 && !IS_STORYTELLER(ch)){
			send_to_char ("Your Resources are temporarily tapped out.#n\n\r", ch);
			return;
		}

		// Changeling drinks
		
	if (!strcmp (arg1, "blush")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20170 ), 1), ch );
			send_to_char("You order a glass of Blushberry ale.\n\r", ch);
			sprintf (buf, "%s orders a glass of Blushberry ale.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}

	if (!strcmp (arg1, "dream")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20171 ), 1), ch );
			send_to_char("You order a glass of Dream-a-Drupe lambic.\n\r", ch);
			sprintf (buf, "%s orders a glass of Dream-a-Drupe lambic.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "ertwen")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20172 ), 1), ch );
			send_to_char("You order a glass of Ertwen bock.\n\r", ch);
			sprintf (buf, "%s orders a glass of Ertwen bock.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "murmur")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20173 ), 1), ch );
			send_to_char("You order a glass of Murmurleaf tea-stout.\n\r", ch);
			sprintf (buf, "%s orders a glass of Murmurleaf tea-stout.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "chuchu")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20174 ), 1), ch );
			send_to_char("You order a pint of animate Chu Chu Gin.\n\r", ch);
			sprintf (buf, "%s orders a pint of animate Chu Chu Gin.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "cousin")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20175 ), 1), ch );
			send_to_char("You order a flute of iridescent liquor.\n\r", ch);
			sprintf (buf, "%s orders a flute of iridescent liquor.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "coco")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20176 ), 1), ch );
			send_to_char("You order a snifter of fragrant Cocorange rum.\n\r", ch);
			sprintf (buf, "%s orders a snifter of fragrant Cocorange rum.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "nettle")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20177 ), 1), ch );
			send_to_char("You order a shooter of Coupnettle gin.\n\r", ch);
			sprintf (buf, "%s orders a shooter of Coupnettle gin.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
	if (!strcmp (arg1, "gourd")){
			if ( ch->pounds < 6 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20178 ), 1), ch );
			send_to_char("You order a shot of Headgourd whiskey.\n\r", ch);
			sprintf (buf, "%s orders a shot of Headgourd whiskey.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 6;
			return;		
		}
		
		// End changeling drinks
		
		
	if (!strcmp (arg1, "whiskey")){
			if ( ch->pounds < 3 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20166 ), 1), ch );
			send_to_char("You order a shooter of whiskey.\n\r", ch);
			sprintf (buf, "%s orders a shooter of whiskey.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 3;
			return;		
		}	

	if (!strcmp (arg1, "brandy")){
			if ( ch->pounds < 4 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20167 ), 1), ch );
			send_to_char("You order a warmed snifter of brandy.\n\r", ch);
			sprintf (buf, "%s orders a warmed snifter of brandy.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 4;
			return;		
		}	

	if (!strcmp (arg1, "rum")){
			if ( ch->pounds < 2 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20168 ), 1), ch );
			send_to_char("You order a shot of spiced rum.\n\r", ch);
			sprintf (buf, "%s orders a shot of spiced rum.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 2;
			return;		
		}	

	if (!strcmp (arg1, "martini")){
			if ( ch->pounds < 7 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20169 ), 1), ch );
			send_to_char("You order an olive-garnished martini.\n\r", ch);
			sprintf (buf, "%s orders an olive-garnished martini.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 7;
			return;		
		}			

	
	if (!strcmp (arg1, "amstel")){
			if ( ch->pounds < 3 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20158 ), 1), ch );
			send_to_char("You order a pint of Amstel.\n\r", ch);
			sprintf (buf, "%s orders a pint of Amstel.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 3;
			return;		
		}
		
	if (!strcmp (arg1, "guinness")){
			if ( ch->pounds < 4 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20159 ), 1), ch );
			send_to_char("You order a bottle of Guinness.\n\r", ch);
			sprintf (buf, "%s orders a bottle of Guinness.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 4;
			return;		
		}
		
	if (!strcmp (arg1, "leffe")){
			if ( ch->pounds < 5 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20165 ), 1), ch );
			send_to_char("You order a glass of Leffe Blonde.\n\r", ch);
			sprintf (buf, "%s orders a glass of Leffe Blonde.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 5;
			return;		
		}
		
	if (!strcmp (arg1, "barossa")){
			if ( ch->pounds < 4 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20160 ), 1), ch );
			send_to_char("You order a glass of Barossa Valley red.\n\r", ch);
			sprintf (buf, "%s orders a glass of Barossa Valley red.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 4;
			return;		
		}
		
	if (!strcmp (arg1, "larchago")){
			if ( ch->pounds < 4 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20161 ), 1), ch );
			send_to_char("You order a glass of Larchago white wine.\n\r", ch);
			sprintf (buf, "%s orders a glass of Larchago white wine.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 4;
			return;		
		}
		
	if (!strcmp (arg1, "miradou")){
			if ( ch->pounds < 4 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20163 ), 1), ch );
			send_to_char("You order a glass of Miradou rose wine.\n\r", ch);
			sprintf (buf, "%s orders a glass of Miradou rose wine.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 4;
			return;		
		}

	if (!strcmp (arg1, "coke")){
			if ( ch->pounds < 1 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20162 ), 1), ch );
			send_to_char("You order a glass of Coca-Cola.\n\r", ch);
			sprintf (buf, "%s orders a glass of Coca-Cola.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 1;
			return;		
		}

	if (!strcmp (arg1, "water")){
			if ( ch->pounds < 1 )
	        {
            send_to_char("You cannot afford to purchase that.\n\r", ch);
	        return;
	        }
			obj_to_char( create_object(get_obj_index( 20164 ), 1), ch );
			send_to_char("You order a glass of ice water.\n\r", ch);
			sprintf (buf, "%s orders a glass of ice water.\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
            ch->pounds -= 1;
			return;		
		}		
	
	send_to_char ("You can't order that here.\n\r", ch);
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
		send_to_char("#c[#C #c(#CStatusCode#c) :#C Only vampires may influence Covenant Status. #c]#n\n\r", ch);
		return;
	}
	
	if (arg1[0] == '\0')
	{
		send_to_char ("#c[#C #c(#CStatusCode#c) : #CSyntax#c#C status #c<#Cname#c>#C +#c/#C- #c]#n\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL || (victim->in_room != ch->in_room))
	{
		send_to_char ("#c[#C #c(#CStatusCode#c) : #CThat character isn't present. #c]#n\n\r", ch);
		return;
	}

		if (ch->side[0] != victim->side[0]){
		send_to_char("#c[#C #c(#CStatusCode#c) : #CYou can only influence the Status of vampires in your Covenant. #c]#n\n\r", ch);
		return;
	}
	

	
	if ((ch->pcdata->statusvotecounter < 4) && !IS_STORYTELLER(ch)){
		send_to_char ("#c[#C #c(#CStatusCode#c) : #CYou can not currently cast a status vote. #c]#n\n\r", ch);
		send_to_char ("#c[#C #c(#CStatusCode#c) : #CYou get one status vote every 4 hours IC. #c]#n\n\r", ch);
		return;
	}
	
	if (victim == ch)
	{
		send_to_char ("#c[#C #c(#CStatusCode#c) : #CYou cannot influence your own Status with the automated system. #c]#n\n\r", ch);
		return;
	}
	
/* eh? 	*/
	
	if(has_merit(ch, "prince")){
		politicdiff -= 1;
	}	
	
  if(has_merit(victim, "prince")){
		politicdiff += 1;
	}
		
	if(has_merit(ch, "court")){
		politicdiff -= 4;
	}
		
  if(has_merit(victim, "court")){
		politicdiff += 4;
	}
	
	if(has_merit(ch, "harpy")){
		politicdiff -= 1;
	}
		
  if(has_merit(victim, "harpy")){
		politicdiff += 1;
	}
	
  if(has_merit(ch, "notary")){
		politicdiff -= 1;
	}
		
  if(has_merit(victim, "notary")){
		politicdiff += 1;
	}
	
	/* end eh? */
		if(ch->abilities[MENTAL][KNO_POLITICS] > 0){
		
		politicsucc = diceroll(ch, ATTRIB_MAN, MENTAL, KNO_POLITICS, 8);

	if (politicsucc < 0){
		ch->pcdata->covenantstatus -= 35;
		statusmod = 0;
		
	}
	
	if (politicsucc == 0)
		statusmod = 0;
	
	if (politicsucc == 1)
		statusmod = .5;
		
	if (politicsucc == 2)
		statusmod = 1;
		
	if (politicsucc == 3)
		statusmod = 1.25;
		
	if (politicsucc == 4)
		statusmod = 2.25;
		
	if (politicsucc >= 5)
		statusmod = 3.75;
		
	}

	else{
	send_to_char("#c[#C #c(#CStatusCode#c) : #CYou must have at least one dot of Politics to influence Status ratings. #c]#n\n\r", ch);
	return;
}
	
	
		
		
	
	if (!str_cmp (arg2, "+")){	
		statusdiff = ch->pcdata->covenantstatus - victim->pcdata->covenantstatus;
		
		

		if (statusdiff <= 49){
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 50) && (statusdiff <= 99)){
			statuschange = (2 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		
		if ((statusdiff >= 100) && (statusdiff <= 199)){
			statuschange = (5 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 200) && (statusdiff <= 249)){
			statuschange = (10 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 250) && (statusdiff <= 299)){
			statuschange = (20 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 300) && (statusdiff <= 349)){
			statuschange = (40 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 350) && (statusdiff <= 399)){
			statuschange = (60 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 400) && (statusdiff <= 449)){
			statuschange = (100 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 450) && (statusdiff <= 499)){
			statuschange = (110 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 500) && (statusdiff <= 549)){
			statuschange = (120 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 550) && (statusdiff <= 599)){
			statuschange = (130 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if (statusdiff >= 600){
			statuschange = (140 * statusmod);
			victim->pcdata->covenantstatus += statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to better their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
	}
	
	
	else if (!str_cmp (arg2, "-")){
		statusdiff = ch->pcdata->covenantstatus - victim->pcdata->covenantstatus;
		
	if (statusdiff <= -50){
			
				send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
			
		if ((statusdiff >= -49) && (statusdiff <= 19)){
			statuschange = (2 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 20) && (statusdiff <= 99)){
			statuschange = (5 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		
		if ((statusdiff >= 100) && (statusdiff <= 199)){
			statuschange = (10 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if ((statusdiff >= 200) && (statusdiff <= 249)){
			statuschange = (20 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 250) && (statusdiff <= 299)){
			statuschange = (40 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 300) && (statusdiff <= 349)){
			statuschange = (60 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
				if ((statusdiff >= 350) && (statusdiff <= 399)){
			statuschange = (100 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 400) && (statusdiff <= 449)){
			statuschange = (110 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 450) && (statusdiff <= 499)){
			statuschange = (120 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 500) && (statusdiff <= 549)){
			statuschange = (130 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}

		if ((statusdiff >= 550) && (statusdiff <= 599)){
			statuschange = (140 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
		
		if (statusdiff >= 600){
			statuschange = (150 * statusmod);
			victim->pcdata->covenantstatus -= statuschange;
			send_to_char("#c[#C #c(#CStatusCode#c) :#C You have attempted to worsen their reputation. #c]#n\n\r", ch);
			ch->pcdata->statusvotecounter = 0;
		}
	
	}

	else{
		send_to_char("#c[#C #c(#CStatusCode#c) :#CDo you want to increase #c(#C+#c)#C or decrease#c (#C-#c)#C the subject's Status? #c]#n\n\r", ch);
		return;
	}
	
}

/*void do_moon_phase (CHAR_DATA * ch, char *argument)
{

  char buf[MAX_STRING_LENGTH];


  if (weather_info.moon_phase == MOON_NEW_MOON)
    {
      send_to_char
	("The moon is current in the 'New moon' Phase\n\r\n\r",
	 ch);
      send_to_char ("#e         _..._           \n\r", ch);
      send_to_char ("#e       .:::::::.         \n\r", ch);
      send_to_char ("#e      :::::::::::        \n\r", ch);
      send_to_char ("#e      :::::::::::        \n\r", ch);
      send_to_char ("#e      ':::::::::'        \n\r", ch);
      send_to_char ("#e        '':::''          \n\r\n\r", ch);
      return;
    }


  else if (weather_info.moon_phase == MOON_WAXING_CRESCENT)
    {
      send_to_char
	("The moon is current in the 'Waxing Crescent' Phase\n\r\n\r",
	 ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .::::. '.		\n\r", ch);
      send_to_char ("#e     :::::::.  : 	\n\r", ch);
      send_to_char ("#e     ::::::::  :  	\n\r", ch);
      send_to_char ("#e     '::::::' .'	    \n\r", ch);
      send_to_char ("#e      ''::'-'		\n\r\n\r", ch);
      return;

    }


  else if (weather_info.moon_phase == MOON_FIRST_QUARTER)
    {
      sprintf (buf,
	       "The moon is currently in the 'Waxing first quarter' Phase.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .::::  '.		\n\r", ch);
      send_to_char ("#e     ::::::    :  	\n\r", ch);
      send_to_char ("#e     ::::::    : 	\n\r", ch);
      send_to_char ("#e     ':::::   .' 	\n\r", ch);
      send_to_char ("#e       ''::.-'		\n\r\n\r", ch);
      return;
    }


  else if (weather_info.moon_phase == MOON_WAXING_GIBBOUS)
    {
      sprintf (buf,
	       "The moon is currently in the 'Waxing Gibbous' Phase.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .::'   '.		\n\r", ch);
      send_to_char ("#e     :::       : 	\n\r", ch);
      send_to_char ("#e     :::       : 	\n\r", ch);
      send_to_char ("#e     '::.     .' 	\n\r", ch);
      send_to_char ("#e       '':..-'   	\n\r\n\r", ch);
      return;
    }


  else if (weather_info.moon_phase == MOON_FULL_MOON)
    {
      sprintf (buf, "The moon is currently Full.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .'     '.		\n\r", ch);
      send_to_char ("#e     :         : 	\n\r", ch);
      send_to_char ("#e     :         : 	\n\r", ch);
      send_to_char ("#e     '.       .' 	\n\r", ch);
      send_to_char ("#e       '-...-'		\n\r\n\r", ch);
      return;
    }


  else if (weather_info.moon_phase == MOON_WANING_GIBBOUS)
    {
      sprintf (buf,
	       "The moon is currently in the 'Waning Gibbous' Phase.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .'   '::.		\n\r", ch);
      send_to_char ("#e     :       ::: 	\n\r", ch);
      send_to_char ("#e     :       ::: 	\n\r", ch);
      send_to_char ("#e     '.     .::' 	\n\r", ch);
      send_to_char ("#e       '-..:''		\n\r\n\r", ch);
      return;
    }

  else if (weather_info.moon_phase == MOON_LAST_QUARTER)
    {
      sprintf (buf,
	       "The moon is currently in the 'Last Quarter' Phase.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .'  ::::.		\n\r", ch);
      send_to_char ("#e     :    :::::: 	\n\r", ch);
      send_to_char ("#e     :    :::::: 	\n\r", ch);
      send_to_char ("#e     '.   :::::' 	\n\r", ch);
      send_to_char ("#e       '-.::''		\n\r\n\r", ch);
      return;
    }

  else if (weather_info.moon_phase == MOON_WANING_CRESCENT)
    {
      sprintf (buf,
	       "The moon is currently in the 'Waning Crescent' Phase.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .' .::::.		\n\r", ch);
      send_to_char ("#e     :  :::::::: 	\n\r", ch);
      send_to_char ("#e     :  :::::::: 	\n\r", ch);
      send_to_char ("#e     '. '::::::' 	\n\r", ch);
      send_to_char ("#e       '-.::''		\n\r\n\r", ch);
      return;
    }

  else
    {
      sprintf (buf,
	       "The moon is currently in the 'Waning Crescent' Phase.\n\r");
      send_to_char (buf, ch);
      send_to_char ("#e        _..._		\n\r", ch);
      send_to_char ("#e      .' .::::.		\n\r", ch);
      send_to_char ("#e     :  :::::::: 	\n\r", ch);
      send_to_char ("#e     :  :::::::: 	\n\r", ch);
      send_to_char ("#e     '. '::::::' 	\n\r", ch);
      send_to_char ("#e       '-.::''		\n\r\n\r", ch);
      return;
    }

  return;
}*/

void do_report (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    sprintf (buf,
             "#g[Report]#n You have #R%d#n aggravated, #R%d#n lethal, and #R%d#n bashing damage.\n\r",
             ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage);

    send_to_char (buf, ch);

    sprintf (buf,
             "#g[Report]#n $n has #R%d#n aggravated, #R%d#n lethal, and #R%d#n bashing damage.\n\r",
             ch->pcdata->aggdamage, ch->pcdata->lethaldamage, ch->pcdata->bashingdamage);

    act (buf, ch, NULL, NULL, TO_ROOM);

    return;
}

void do_affects(CHAR_DATA *ch, char *argument) {
	char buf[MAX_INPUT_LENGTH];
	send_to_char("#gYou are affected by:#n\n\r", ch);
	{

		// Damage check

		if ((ch->pcdata->aggdamage) > 0) {
			send_to_char(
					"#y* #wAggravated Wound:#n A raw and indescribable pain emanates from the source of your injury. #g(Heal 1 per week)#n\n\r",
					ch);
		}

		if ((ch->pcdata->lethaldamage) > 0) {
			send_to_char(
					"#y* #wLethal Wound:#n A sharp, raw pain ripples and throbs through lacerations and deep bruises. #g(Heal 1 every 2 days)#n\n\r",
					ch);
		}

		if ((ch->pcdata->bashingdamage) > 0) {
			send_to_char(
					"#y* #wBashing Wound:#n A throbbing but ultimately minor soreness pulses through bruised flesh. #g(Heal 1 per day)#n\n\r",
					ch);
		}

		// End Damage

		// Perfume check
		if (IS_AFFECTED(ch, AFF_PERFUME)) {
			{
				if (!IS_NPC(ch) && ch->sex == SEX_FEMALE)
					send_to_char(
							"#y* #wScent:#n Your skin smells of lovely, feminine perfume.#n\n\r",
							ch);
				else if (!IS_NPC(ch) && ch->sex == SEX_MALE)
					send_to_char(
							"#y* #wScent:#n Your skin smells of intoxicating, musky cologne.#n\n\r",
							ch);
				else
					send_to_char(
							"#y* #wScent:#n Your skin smells of clean body wash.#n\n\r",
							ch);
			}
		}
		
		// Dirty check
		
		if (IS_AFFECTED(ch, AFF_DIRTY)) {
			{
				if (!IS_NPC(ch) && ch->sex == SEX_FEMALE)
					send_to_char(
							"#y* #wDirty:#n You are covered in filth and grime, reeking to high heaven.#n\n\r",
							ch);
				else if (!IS_NPC(ch) && ch->sex == SEX_MALE)
					send_to_char(
							"#y* #wDirty:#n You are covered in filth and grime, reeking to high heaven.#n\n\r",
							ch);
				else
					send_to_char(
							"#y* #wDirty:#n You are covered in filth and grime, reeking to high heaven.#n\n\r",
							ch);
			}
		}
		
		// Condom check
		if (IS_AFFECTED(ch, AFF_CONTRACEPTION)) {
			{
				if (!IS_NPC(ch) && ch->sex == SEX_FEMALE)
					send_to_char(
							"#y* #wSafe(r) Sex:#n You are taking preventative measures against pregnancy.#n\n\r",
							ch);
				else if (!IS_NPC(ch) && ch->sex == SEX_MALE)
					send_to_char(
							"#y* #wSafe(r) Sex:#n You are taking preventative measures against pregnancy.#n\n\r",
							ch);
				else
					send_to_char(
							"#y* #wSafe(r) Sex:#n You are taking preventative measures against pregnancy.#n\n\r",
							ch);
			}
		}
		
		// Pregnancy check if (IS_EXTRA(ch, EXTRA_MARRIED))
		if (!IS_NPC(ch) && ch->sex == SEX_FEMALE && IS_EXTRA(ch, EXTRA_PREGNANT)) {
					send_to_char(
							"#y* #wPregnant:#n You suffer intermittent bouts of nausea, soreness, and a steadily developing baby-bump. #n",
							ch);
					sprintf (buf, "#g(Conceived on %-24s by: %s.)#n\n\r", ch->pcdata->conception, ch->pcdata->cparents);
	                send_to_char (buf, ch);
		            }
		
		// Labor check
		if (!IS_NPC(ch) && ch->sex == SEX_FEMALE && IS_EXTRA(ch, EXTRA_LABOUR)) {
					send_to_char(
							"#y* #wLabor:#n Body-wracking, inconceivable pain contracts through your abdomen. The baby will be here soon.#n\n\r",
							ch);
		}

		// Nudity check
		if (((get_eq_char(ch, WEAR_LEGS)) == NULL)
				&& ((get_eq_char(ch, WEAR_BODY)) == NULL)
				&& ((get_eq_char(ch, WEAR_FEET)) == NULL)
				&& ((get_eq_char(ch, WEAR_ABOUT)) == NULL)) {
			send_to_char("#Y* #wNaked:#n You are completely naked.#n\n\r", ch);
			return;
		}

		// Drunkenness checks
		if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 2
				&& ch->pcdata->condition[COND_DRUNK] < 11) {
			send_to_char(
					"#y* #wTipsy:#n Your extremities feel warm and fuzzy; you're pleasantly intoxicated.\n\r",
					ch);
			return;
		}

		if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10) {
			send_to_char(
					"#y* #wDrunk:#n The world is spinning and your inhibitions are wholly absent.\n\r",
					ch);
			return;
		}
		
	}
	
}

void do_contracts( CHAR_DATA *ch, char *argument )
{
    if (IS_NPC(ch)) return;

    if (!IS_CHANGELING(ch) )
    {
	send_to_char("Huh?\n\r",ch);
	return;
    }

	send_to_char( "#n Contract                                                               Commands\n\r",ch);
    send_to_char( "#g--------------------------------------------------------------------------------#n\n\r", ch );
    send_to_char( "#n  Dream 2                                                             dreamweave#n\n\r", ch );
	send_to_char( "#n Mirror 2                                                               skinmask#n\n\r", ch );
	send_to_char( "#n  Smoke 5                                                               lightshy#n\n\r", ch );
    send_to_char( "#g--------------------------------------------------------------------------------#n\n\r", ch );
    return;
}

/* Bio test */

void do_biography (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	else if (!strcmp (argument, "edit"))
		string_edit (ch, &ch->pcdata->biography);
	else if (!strcmp (argument, "append"))
		string_append (ch, &ch->pcdata->biography);
	else if (!strcmp (argument, "show"))
	{
		send_to_char ("Your character biography is:\n\r", ch);
		send_to_char (ch->pcdata->biography ? ch->pcdata->biography : "(None).\n\r", ch);
	}
	else
	{
		send_to_char ("Biography Options: show, edit, append\n\r", ch);
		send_to_char ("Syntax:   biography show\n\r", ch);
		send_to_char ("          biography edit\n\r", ch);
		send_to_char ("          biography append\n\r", ch);
	}
	return;
}

/* End bio test */

void do_skills ( CHAR_DATA *ch, char * argument)
{
   char buf[MAX_STRING_LENGTH*2];
   
sprintf(buf, "\n\r"
   "#g--------------------------------------------------------------------------------#n\r\n"
   "#w %s %s's Skills#n\r\n"
   "#g--------------------------------------------------------------------------------#n\r\n"
   "#g| Physical |------------| Social |-----------------| Mental |-------------------#n\n\r"
   "#g|#n Athletics#e...%5s #g|#n  Animal Ken#e....%5s #g|  #nAcademics#e......%5s #g|#n\n\r"
   "#g|#n Brawl#e.......%5s #g|#n  Empathy#e.......%5s #g|  #nComputer#e.......%5s #g|#n\n\r"
   "#g|#n Firearms#e....%5s #g|#n  Expression#e....%5s #g|  #nCrafts#e.........%5s #g|#n\n\r"
   "#g|#n Larceny#e.....%5s #g|#n  Intimidation#e..%5s #g|  #nInvestigation#e..%5s #g|#n\n\r"
   "#g|#n Stealth#e.....%5s #g|#n  Socialize#e.....%5s #g|  #nMedicine#e.......%5s #g|#n\n\r"
   "#g|#n Survival#e....%5s #g|#n  Streetwise#e....%5s #g|  #nOccult#e.........%5s #g|#n\n\r"
   "#g|#n Weaponry#e....%5s #g|#n  Persuasion#e....%5s #g|  #nPolitics#e.......%5s #g|#n\n\r"
   "#g|#n Drive#e.......%5s #g|#n  Subterfuge#e....%5s #g|  #nScience#e........%5s #g|#n\n\r"
   "#g--------------------------------------------------------------------------------#n\r\n",
      ch->name, ((ch->pcdata) ? ch->pcdata->surname : "None"),
      show_dot(ch->abilities[PHYSICAL][TAL_ATHLETICS]),  show_dot(ch->abilities[SOCIAL][SKI_ANIMALKEN]),  show_dot(ch->abilities[MENTAL][KNO_ACADEMICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_BRAWL]),  show_dot(ch->abilities[SOCIAL][SKI_EMPATHY]),  show_dot(ch->abilities[MENTAL][KNO_COMPUTER]), 
      show_dot(ch->abilities[PHYSICAL][TAL_FIREARMS]),  show_dot(ch->abilities[SOCIAL][SKI_EXPRESSION]),  show_dot(ch->abilities[MENTAL][KNO_CRAFTS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_LARCENY]),  show_dot(ch->abilities[SOCIAL][SKI_INTIMIDATION]),  show_dot(ch->abilities[MENTAL][KNO_INVESTIGATION]), 
      show_dot(ch->abilities[PHYSICAL][TAL_STEALTH]),  show_dot(ch->abilities[SOCIAL][SKI_SOCIALIZE]),  show_dot(ch->abilities[MENTAL][KNO_MEDICINE]), 
      show_dot(ch->abilities[PHYSICAL][TAL_SURVIVAL]),  show_dot(ch->abilities[SOCIAL][SKI_STREETWISE]),  show_dot(ch->abilities[MENTAL][KNO_OCCULT]), 
      show_dot(ch->abilities[PHYSICAL][TAL_WEAPONRY]),  show_dot(ch->abilities[SOCIAL][SKI_PERSUASION]),  show_dot(ch->abilities[MENTAL][KNO_POLITICS]), 
      show_dot(ch->abilities[PHYSICAL][TAL_DRIVE]),  show_dot(ch->abilities[SOCIAL][SKI_SUBTERFUGE]),  show_dot(ch->abilities[MENTAL][KNO_SCIENCE])
      );
      send_to_char(buf, ch);
}

void do_merits ( CHAR_DATA *ch, char * argument)
{
	
	char buf[MAX_STRING_LENGTH*2];
    int col = 0;
    int sn;
    char merit[30];
    bool second = FALSE;
   
    if (IS_NPC(ch)) return;

    send_to_char("#g--------------------------------------------------------------------------------#n\n\r", ch);
    send_to_char("#n Merits and Fighting Styles#n\n\r", ch);
    send_to_char("#g--------------------------------------------------------------------------------#n\n\r", ch);

    col  = 0;
		for (sn = 0; sn <= MERIT_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (merit, "None#e.................");
				break;
			case MERIT_AREAOFEXPERTISE:
				strcpy (merit, "Area of Expertise#e....");
				break;
			case MERIT_COMMONSENSE:
				strcpy (merit, "Common Sense#e.........");
				break;
			case MERIT_DANGERSENSE:
				strcpy (merit, "Danger Sense#e.........");
				break;
			case MERIT_DIRECTIONSENSE:
				strcpy (merit, "Direction Sense#e......");
				break;
			case MERIT_EIDETICMEMORY:
				strcpy (merit, "Eidetic Memory#e.......");
				break;
			case MERIT_ENCYCLOPEDICKNOWLEDGE:
				strcpy (merit, "Encylopedic Knowledge#e");
				break;
			case MERIT_EYEFORTHESTRANGE:
				strcpy (merit, "Eye for the Strange#e..");
				break;
			case MERIT_FASTREFLEXES:
				strcpy (merit, "Fast Reflexes#e........");
				break;
			case MERIT_GOODTIMEMANAGEMENT:
				strcpy (merit, "Good Time Management#e.");
				break;
			case MERIT_HOLISTICAWARENESS:
				strcpy (merit, "Holistic Awareness#e...");
				break;
			case MERIT_INDOMITABLE:
				strcpy (merit, "Indomitable#e..........");
				break;
			case MERIT_INTERDISCIPLINARY:
				strcpy (merit, "Interdisciplinary#e....");
				break;
			case MERIT_INVESTIGATIVEA:
				strcpy (merit, "Investigative Aide#e...");
				break;
			case MERIT_INVESTIGATIVEP:
				strcpy (merit, "Investigative Prodigy#e");
				break;
			case MERIT_LANGUAGE:
				strcpy (merit, "Language#e.............");
				break;
			case MERIT_LIBRARY:
				strcpy (merit, "Library#e..............");
				break;
			case MERIT_ADVANCEDLIBRARY:
				strcpy (merit, "Advanced Library#e.....");
				break;
			case MERIT_MEDITATIVEMIND:
				strcpy (merit, "Meditative Mind#e......");
				break;
			case MERIT_MULTILINGUAL:
				strcpy (merit, "Multi-lingual#e........");
				break;
			case MERIT_PATIENT:
				strcpy (merit, "Patient#e..............");
				break;
			case MERIT_TOLERANCEFORBIO:
				strcpy (merit, "Tolerance for Biology#e");
				break;
			case MERIT_TRAINEDOBSERVER:
				strcpy (merit, "Trained Observer#e.....");
				break;
			case MERIT_VICERIDDEN:
				strcpy (merit, "Vice Ridden#e..........");
				break;
			case MERIT_VIRTUOUS:
				strcpy (merit, "Virtuous#e.............");
				break;
			case MERIT_AMBIDEXTROUS:
				strcpy (merit, "Ambidextrous#e.........");
				break;
			case MERIT_AUTOMATIVEGENIUS:
				strcpy (merit, "Automative Genius#e....");
				break;
			case MERIT_CRACKDRIVER:
				strcpy (merit, "Crack Driver#e.........");
				break;
			case MERIT_DEMOLISHER:
				strcpy (merit, "Demolisher#e...........");
				break;
			case MERIT_DOUBLEJOINTED:
				strcpy (merit, "Double-Jointed#e.......");
				break;
			case MERIT_FLEETOFFOOT:
				strcpy (merit, "Fleet of Foot#e........");
				break;
			case MERIT_GIANT:
				strcpy (merit, "Giant#e................");
				break;
			case MERIT_HARDY:
				strcpy (merit, "Hardy#e................");
				break;
			case MERIT_GREYHOUND:
				strcpy (merit, "Greyhound#e............");
				break;
			case MERIT_IRONSTAMINA:
				strcpy (merit, "Iron Stamina#e.........");
				break;
			case MERIT_QUICKDRAW:
				strcpy (merit, "Quickdraw#e............");
				break;
			case MERIT_RELENTLESS:
				strcpy (merit, "Relentless#e...........");
				break;
			case MERIT_SEIZINGTHEEDGE:
				strcpy (merit, "Seizing the Edge#e.....");
				break;
			case MERIT_SLEIGHTOFHAND:
				strcpy (merit, "Sleight of Hand#e......");
				break;
			case MERIT_SMALLFRAMED:
				strcpy (merit, "Small-Framed#e.........");
				break;
			case MERIT_ALLIES:
				strcpy (merit, "Allies#e...............");
				break;
			case MERIT_ALTERNATEIDENTITY:
				strcpy (merit, "Alternate Identity#e...");
				break;
			case MERIT_ANONYMITY:
				strcpy (merit, "Anonymity#e............");
				break;				
			case MERIT_BARFLY:
				strcpy (merit, "Barfly#e...............");
				break;
			case MERIT_CLOSEDBOOK:
				strcpy (merit, "Closed Book#e..........");
				break;
            case MERIT_CONTACTS:
                strcpy (merit, "Contacts#e.............");
                break;
            case MERIT_FAME:
                strcpy (merit, "Fame#e.................");
                break;
            case MERIT_FIXER:
                strcpy (merit, "Fixer#e................");
                break;
            case MERIT_HOBBYISTCLIQUE:
                strcpy (merit, "Hobbyist Clique#e......");
                break;
            case MERIT_INSPIRING:
                strcpy (merit, "Inspiring#e............");
                break;
            case MERIT_IRONWILL:
                strcpy (merit, "Iron Will#e............");
                break;
            case MERIT_MENTOR:
                strcpy (merit, "Mentor#e...............");
                break;
            case MERIT_PUSHER:
                strcpy (merit, "Pusher#e...............");
                break;
            case MERIT_RESOURCES:
                strcpy (merit, "Resources#e............");
                break;
            case MERIT_RETAINER:
                strcpy (merit, "Retainer#e.............");
                break;
            case MERIT_SAFEPLACE:
                strcpy (merit, "Safe Place#e...........");
                break;
            case MERIT_SMALLUNITTACTICS:
                strcpy (merit, "Small Unit Tactics#e...");
                break;
            case MERIT_SPINDOCTOR:
                strcpy (merit, "Spin Doctor#e..........");
                break;
            case MERIT_STAFF:
                strcpy (merit, "Staff#e................");
                break;
            case MERIT_STATUS:
                strcpy (merit, "Status#e...............");
                break;
            case MERIT_STRIKINGLOOKS:
                strcpy (merit, "Striking Looks#e.......");
                break;
            case MERIT_SYMPATHETIC:
                strcpy (merit, "Sympathetic#e..........");
                break;
            case MERIT_TABLETURNER:
                strcpy (merit, "Table Turner#e.........");
                break;
            case MERIT_TAKESONETOKNOWONE:
                strcpy (merit, "Takes One To Know One#e");
                break;
            case MERIT_TASTE:
                strcpy (merit, "Taste#e................");
                break;
            case MERIT_TRUEFRIEND:
                strcpy (merit, "True Friend#e..........");
                break;
            case MERIT_UNTOUCHABLE:
                strcpy (merit, "Untouchable#e..........");
                break;
            case MERIT_RUNECASTER:
                strcpy (merit, "Runecaster#e...........");
                break;
            case MERIT_CHEAPSHOT:
                strcpy (merit, "Cheap Shot#e...........");
                break;
            case MERIT_CHOKEHOLD:
                strcpy (merit, "Choke Hold#e...........");
                break;
            case MERIT_DEFENSIVECOMBAT:
                strcpy (merit, "Defensive Combat#e.....");
                break;
            case MERIT_FIGHTINGFINESSE:
                strcpy (merit, "Fighting Finesse#e.....");
                break;
            case MERIT_IRONSKIN:
                strcpy (merit, "Iron Skin#e............");
                break;
            case MERIT_KILLERINSTINCT:
                strcpy (merit, "Killer Instinct#e......");
                break;
            case MERIT_SHIV:
                strcpy (merit, "Shiv#e.................");
                break;
            case MERIT_SUBDUINGSTRIKES:
                strcpy (merit, "Subduing Strikes#e.....");
                break;
            case MERIT_CLEARSIGHTED:
                strcpy (merit, "Clear Sighted#e........");
                break;
            case MERIT_WEAKENEDBOND:
                strcpy (merit, "Weakened Bond#e........");
                break;
            case MERIT_PRODUCER:
                strcpy (merit, "Producer#e.............");
                break;
            case MERIT_BEASTWHISPERS:
                strcpy (merit, "Beast Whispers#e.......");
                break;
            case MERIT_BELOVED:
                strcpy (merit, "Beloved#e..............");
                break;
            case MERIT_PROTECTED:
                strcpy (merit, "Protected#e............");
                break;
            case MERIT_ANIMALSPEECH:
                strcpy (merit, "Animal Speech#e........");
                break;
            case MERIT_AURAREADING:
                strcpy (merit, "Aura Reading#e.........");
                break;
            case MERIT_AUTOMATICWRITING:
                strcpy (merit, "Automatic Writing#e....");
                break;
            case MERIT_BIOKINESIS:
                strcpy (merit, "Biokinesis#e...........");
                break;
            case MERIT_CITYWALKER:
                strcpy (merit, "City Walker#e..........");
                break;
            case MERIT_CLAIRVOYANCE:
                strcpy (merit, "Clairvoyance#e.........");
                break;
            case MERIT_CURSED:
                strcpy (merit, "Cursed#e...............");
                 break;
            case MERIT_LAYINGONHANDS:
                strcpy (merit, "Laying On Hands#e......");
                break;
            case MERIT_MEDIUM:
                strcpy (merit, "Medium#e...............");
                break;
            case MERIT_MINDOFAMADMAN:
                strcpy (merit, "Mind Of A Madman#e.....");
                break;
            case MERIT_OMENSENSITIVITY:
                strcpy (merit, "Omen Sensitivity#e.....");
                break;
            case MERIT_NUMBINGTOUCH:
                strcpy (merit, "Numbing Touch#e........");
                break;
            case MERIT_PSYCHOKINESIS:
                strcpy (merit, "Psychokinesis#e........");
                break;
            case MERIT_PSYCHOMETRY:
                strcpy (merit, "Psychometry#e..........");
                break;
            case MERIT_TELEKINESIS:
                strcpy (merit, "Telekinesis#e..........");
                break;
            case MERIT_TELEPATHY:
                strcpy (merit, "Telepathy#e............");
                break;
            case MERIT_THIEFOFFATE:
                strcpy (merit, "Thief Of Fate#e........");
                break;
            case MERIT_UNSEENSENSE:
                strcpy (merit, "Unseen Sense#e.........");
                break;
				
			}
			capitalize (merit);
			if (!col)
			{
			sprintf (buf, "#n %s.%9s#n                ", merit, show_dot (get_merit(ch, sn)));
			send_to_char (buf, ch);
			col++;
			//second = FALSE;
			}
			else
			{
			sprintf (buf, "%s.%9s#n         #n\n\r", merit, show_dot (get_merit(ch, sn)));
			send_to_char (buf, ch);
			 col--;
			second = TRUE;
		        }
	}
    send_to_char("\n\r#g--------------------------------------------------------------------------------#n\n\r", ch);
}
