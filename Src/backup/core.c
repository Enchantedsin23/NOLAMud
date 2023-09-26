
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/*
 * The primary output interface for formatted output.
 */
void act (const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	char fname[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	CHAR_DATA *to_old;

	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	CHAR_DATA *familiar = NULL;
	CHAR_DATA *wizard = NULL;

	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	const char *i;
	char *point;
	bool is_fam;

	bool is_ok;
	/*
	 * Discard null and zero-length messages.
	 */
	if (format == NULL || format[0] == '\0')
		return;

	if (ch->in_room == NULL)
		to = NULL;
	else
		to = ch->in_room->people;
	if (type == TO_VICT)
	{
		if (vch == NULL || vch->in_room == NULL)
		{
			bug ("Act: null vch with TO_VICT.", 0);
			return;
		}
		to = vch->in_room->people;
	}

	for (; to != NULL; to = to->next_in_room)
	{
		is_fam = FALSE;
		to_old = to;

		if (type == TO_CHAR && to != ch)
			continue;
		if (type == TO_VICT && (to != vch || to == ch))
			continue;
		if (type == TO_ROOM && (to == ch || (ch->plane != to->plane && !IS_STORYTELLER(to))))
			continue;
		if (type == TO_NOTVICT && (to == ch || to == vch))
			continue;
		if (type == TO_MORTS && IS_IMMORTAL(to))
			continue;
		if (to->desc == NULL && IS_NPC (to) && (wizard = to->wizard) != NULL)
		{
			if (!IS_NPC (wizard) && ((familiar = wizard->pcdata->familiar) != NULL) && familiar == to)
			{
				if (to->in_room == ch->in_room && wizard->in_room != to->in_room)
				{
					to = wizard;
					SET_BIT (to->extra, EXTRA_CAN_SEE);
					is_fam = TRUE;
				}
			}
		}

		if (to->desc == NULL || !IS_AWAKE (to))
		{
			if (is_fam)
			{
				REMOVE_BIT (to->extra, EXTRA_CAN_SEE);
				to = to_old;
			}

			continue;
		}

		if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_room != NULL && !IS_NPC (to) && to->pcdata->chobj != NULL && to->pcdata->chobj->in_room != NULL && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL && !IS_NPC (to) && to->pcdata->chobj != NULL && to->pcdata->chobj->in_obj != NULL && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if (!is_ok)
			{
				if (is_fam)
				{
					REMOVE_BIT (to->extra, EXTRA_CAN_SEE);
					to = to_old;
				}

				continue;
			}
		}

		point = buf;
		str = format;
		while (*str != '\0')
		{
			if (*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if (arg2 == NULL && *str >= 'A' && *str <= 'Z')
			{
				bug ("Act: missing arg2 for code %d.", *str);
				i = " <@@@> ";
			}
			else
			{
				switch (*str)
				{
				default:
					bug ("Act: bad code %d.", *str);
					i = " <@@@> ";
					break;
					/* Thx alex for 't' idea */
				case 't':
					i = (char *) arg1;
					break;
				case 'T':
					i = (char *) arg2;
					break;
				case 'n':
					i = PERS (ch, to);
					break;
				case 'N':
					i = PERS (vch, to);
					break;
				case 'e':
					i = he_she[URANGE (0, ch->sex, 2)];
					break;
				case 'E':
					i = he_she[URANGE (0, vch->sex, 2)];
					break;
				case 'm':
					i = him_her[URANGE (0, ch->sex, 2)];
					break;
				case 'M':
					i = him_her[URANGE (0, vch->sex, 2)];
					break;
				case 's':
					i = his_her[URANGE (0, ch->sex, 2)];
					break;
				case 'S':
					i = his_her[URANGE (0, vch->sex, 2)];
					break;

				case 'p':
					i = can_see_obj (to, obj1) ? ((obj1->chobj != NULL && obj1->chobj == to) ? "you" : obj1->short_descr) : "something";
					break;

				case 'P':
					i = can_see_obj (to, obj2) ? ((obj2->chobj != NULL && obj2->chobj == to) ? "you" : obj2->short_descr) : "something";
					break;

				case 'd':
					if (arg2 == NULL || ((char *) arg2)[0] == '\0')
					{
						i = "door";
					}
					else
					{
						one_argument ((char *) arg2, fname);
						i = fname;
					}
					break;
				}
			}

			++str;
			while ((*point = *i) != '\0')
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
		if (is_fam)
		{
			if (to->in_room != ch->in_room && familiar != NULL && familiar->in_room == ch->in_room)
				send_to_char ("[ ", to);
			else
			{
				REMOVE_BIT (to->extra, EXTRA_CAN_SEE);
				to = to_old;
				continue;
			}
		}

		buf[0] = UPPER (buf[0]);
		if (to->desc && (to->desc->connected == CON_PLAYING))
			write_to_buffer (to->desc, buf, point - buf);

		//mob programs
		if (MOBtrigger)
			mprog_act_trigger (buf, to, ch, obj1, vch);
		/* Added by Kahn */

		/* Next if statement */
		if (is_fam)
		{
			REMOVE_BIT (to->extra, EXTRA_CAN_SEE);
			to = to_old;
		}

	}
	MOBtrigger = TRUE;	//mob programs
	return;
}

void act2 (const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *to;
/*
    CHAR_DATA *to_old;
*/
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
/*
	 CHAR_DATA *familiar = NULL;
    CHAR_DATA *wizard = NULL;
*/
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA *) arg2;
	const char *str;
	const char *i;
	char *point;
/*
    bool is_fam;
*/
	bool is_ok;
	/*
	 * Discard null and zero-length messages.
	 */
	if (format == NULL || format[0] == '\0')
		return;

	to = ch->in_room->people;
	if (type == TO_VICT)
	{
		if (vch == NULL || vch->in_room == NULL)
		{
			bug ("Act: null vch with TO_VICT.", 0);
			return;
		}
		to = vch->in_room->people;
	}

	for (; to != NULL; to = to->next_in_room)
	{
/*
	is_fam = FALSE;
	to_old = to;
*/
		if (type == TO_CHAR && to != ch)
			continue;
		if (type == TO_VICT && (to != vch || to == ch))
			continue;
		if (type == TO_ROOM && to == ch)
			continue;
		if (type == TO_NOTVICT && (to == ch || to == vch))
			continue;
		if (type == TO_MORTS && IS_IMMORTAL(to))
			continue;
/*
	if ( to->desc == NULL && IS_NPC(to) && (wizard = to->wizard) != NULL )
	{
	    if (!IS_NPC(wizard) && ((familiar=wizard->pcdata->familiar) != NULL)
		&& familiar == to)
	    {
		if (to->in_room == ch->in_room && 
		    wizard->in_room != to->in_room)
		{
		    to = wizard;
		    is_fam = TRUE;
		}
	    }
	}
*/
		if (to->desc == NULL || !IS_AWAKE (to))
		{
/*
	    if (is_fam) to = to_old;
*/
			continue;
		}

		if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_room != NULL && !IS_NPC (to) && to->pcdata->chobj != NULL && to->pcdata->chobj->in_room != NULL && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL && !IS_NPC (to) && to->pcdata->chobj != NULL && to->pcdata->chobj->in_obj != NULL && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if (!is_ok)
			{
/*
		if (is_fam) to = to_old;
*/
				continue;
			}
		}

		point = buf;
		str = format;
		while (*str != '\0')
		{
			if (*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if (arg2 == NULL && *str >= 'A' && *str <= 'Z')
			{
/*
		bug( "Act: missing arg2 for code %d.", *str );
*/
				i = " <@@@> ";
			}
			else
			{
				switch (*str)
				{
				default:
					i = " ";
					break;
				case 'n':
					if (ch != NULL)
						i = PERS (ch, to);
					else
						i = " ";
					break;
				case 'N':
					if (vch != NULL)
						i = PERS (vch, to);
					else
						i = " ";
					break;
				case 'e':
					if (ch != NULL)
						i = he_she[URANGE (0, ch->sex, 2)];
					else
						i = " ";
					break;
				case 'E':
					if (vch != NULL)
						i = he_she[URANGE (0, vch->sex, 2)];
					else
						i = " ";
					break;
				case 'm':
					if (ch != NULL)
						i = him_her[URANGE (0, ch->sex, 2)];
					else
						i = " ";
					break;
				case 'M':
					if (vch != NULL)
						i = him_her[URANGE (0, vch->sex, 2)];
					else
						i = " ";
					break;
				case 's':
					if (ch != NULL)
						i = his_her[URANGE (0, ch->sex, 2)];
					else
						i = " ";
					break;
				case 'S':
					if (vch != NULL)
						i = his_her[URANGE (0, vch->sex, 2)];
					else
						i = " ";
					break;
				case 'p':
					if (obj1 != NULL)
					{
						i = can_see_obj (to, obj1) ? ((obj1->chobj != NULL && obj1->chobj == to) ? "you" : obj1->short_descr) : "something";
					}
					else
						i = " ";
					break;

				case 'P':
					if (obj2 != NULL)
					{
						i = can_see_obj (to, obj2) ? ((obj2->chobj != NULL && obj2->chobj == to) ? "you" : obj2->short_descr) : "something";
					}
					else
						i = " ";
					break;
				}
			}

			++str;
			while ((*point = *i) != '\0')
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
/*
	if (is_fam)
	{
		 if (to->in_room != ch->in_room && familiar != NULL &&
		familiar->in_room == ch->in_room)
		send_to_char("[ ", to);
		 else
		 {
		to = to_old;
		continue;
		 }
	}
*/
		buf[0] = UPPER (buf[0]);
		write_to_buffer (to->desc, buf, point - buf);

		if (MOBtrigger)	//mob programs
			mprog_act_trigger (buf, to, ch, obj1, vch);

/*
	if (is_fam) to = to_old;
*/
	}
	MOBtrigger = TRUE;	//mob programs

	return;
}



void kavitem (const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type)
{
	static char *const he_she[] = { "it", "he", "she" };
	static char *const him_her[] = { "it", "him", "her" };
	static char *const his_her[] = { "its", "his", "her" };

	char buf[MAX_STRING_LENGTH];
	char kav[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA *) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA *) arg1;
	const char *str;
	const char *i;
	char *point;
	bool is_ok;

	/*
	 * Discard null and zero-length messages.
	 */
	if (format == NULL || format[0] == '\0')
		return;

	to = ch->in_room->people;
	if (type == TO_VICT)
	{
		if (vch == NULL)
		{
			bug ("Act: null vch with TO_VICT.", 0);
			return;
		}
		to = vch->in_room->people;
	}

	for (; to != NULL; to = to->next_in_room)
	{
		if (to->desc == NULL || !IS_AWAKE (to))
			continue;

		if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		{
			is_ok = FALSE;

			if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_room != NULL && !IS_NPC (to) && to->pcdata->chobj != NULL && to->pcdata->chobj->in_room != NULL && ch->in_room == to->in_room)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if (!IS_NPC (ch) && ch->pcdata->chobj != NULL && ch->pcdata->chobj->in_obj != NULL && !IS_NPC (to) && to->pcdata->chobj != NULL && to->pcdata->chobj->in_obj != NULL && ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
				is_ok = TRUE;
			else
				is_ok = FALSE;

			if (!is_ok)
				continue;
		}
		if (type == TO_CHAR && to != ch)
			continue;
		if (type == TO_VICT && (to != vch || to == ch))
			continue;
		if (type == TO_ROOM && to == ch)
			continue;
		if (type == TO_NOTVICT && (to == ch || to == vch))
			continue;

		point = buf;
		str = format;
		while (*str != '\0')
		{
			if (*str != '$')
			{
				*point++ = *str++;
				continue;
			}
			++str;

			if (arg2 == NULL && *str >= 'A' && *str <= 'Z')
				i = "";
			else
			{
				switch (*str)
				{
				default:
					i = "";
					break;
				case 'n':
					i = PERS (ch, to);
					break;
				case 'e':
					i = he_she[URANGE (0, ch->sex, 2)];
					break;
				case 'm':
					i = him_her[URANGE (0, ch->sex, 2)];
					break;
				case 's':
					i = his_her[URANGE (0, ch->sex, 2)];
					break;
				case 'p':
					i = can_see_obj (to, obj1) ? ((obj1->chobj != NULL && obj1->chobj == to) ? "you" : obj1->short_descr) : "something";
					break;

				case 'o':
					if (obj1 != NULL)
						sprintf (kav, "%s's", obj1->short_descr);
					i = can_see_obj (to, obj1) ? ((obj1->chobj != NULL && obj1->chobj == to) ? "your" : kav) : "something's";
					break;

				}
			}

			++str;
			while ((*point = *i) != '\0')
				++point, ++i;
		}

		*point++ = '\n';
		*point++ = '\r';
		buf[0] = UPPER (buf[0]);
		write_to_buffer (to->desc, buf, point - buf);
		//mob programs
		if (MOBtrigger)
			mprog_act_trigger (buf, to, ch, obj1, vch);

	}
	MOBtrigger = TRUE;	//mob programs
	return;
}


bool is_safe (CHAR_DATA * ch, CHAR_DATA * victim)
{
	/* Ethereal people can only attack other ethereal people */
	if (ch->in_room != victim->in_room)
		return TRUE;
	if (!IS_NPC (ch) && get_trust (ch) == LEVEL_BUILDER)
	{
		send_to_char ("Don't fight, build.\n\r", ch);
		return TRUE;
	}
	if (!IS_NPC (victim) && get_trust (victim) == LEVEL_BUILDER)
	{
		send_to_char ("They build, not fight.\n\r", ch);
		return TRUE;
	}
	if (IS_MORE3 (ch, MORE3_THOTH))
	{
		send_to_char ("You cannot do this while under the Sign of Thoth!\n\r", ch);
		return TRUE;
	}
	if (IS_MORE3 (victim, MORE3_THOTH))
	{
		send_to_char ("They are protected by some strange magical field!\n\r", victim);
		return TRUE;
	}

	if (IS_AFFECTED (ch, AFF_ETHEREAL) && !IS_AFFECTED (victim, AFF_ETHEREAL))
	{
		send_to_char ("You cannot while ethereal.\n\r", ch);
		return TRUE;
	}
	if (!IS_AFFECTED (ch, AFF_ETHEREAL) && IS_AFFECTED (victim, AFF_ETHEREAL))
	{
		send_to_char ("You cannot fight an ethereal person.\n\r", ch);
		return TRUE;
	}
	if (IS_POLYAFF (ch, POLY_MIST) || IS_POLYAFF (victim, POLY_MIST))
	{
		send_to_char ("People in mist form may not enter combat.\n\r", ch);
		return TRUE;
	}
	if (IS_POLYAFF (ch, POLY_SHADOW) || IS_POLYAFF (victim, POLY_SHADOW))
	{
		send_to_char ("People in smoke form may not enter combat.\n\r", ch);
		return TRUE;
	}

	/* You cannot attack across planes */
	if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_AFFECTED (victim, AFF_SHADOWPLANE))
	{
		act ("You are too insubstantial!", ch, NULL, victim, TO_CHAR);
		return TRUE;
	}
	if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_AFFECTED (victim, AFF_SHADOWPLANE))
	{
		act ("$E is too insubstantial!", ch, NULL, victim, TO_CHAR);
		return TRUE;
	}

	if (ch->in_room == NULL || IS_SET (ch->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char ("Something in the air prevents you from attacking.\n\r", ch);
		return TRUE;
	}

	if (victim == NULL || victim->in_room == NULL)
		return TRUE;

	if (IS_SET (victim->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char ("Something in the air prevents you from attacking.\n\r", ch);
		return TRUE;
	}
	if (IS_EXTRA (ch, EXTRA_OSWITCH))
	{
		send_to_char ("Objects cannot fight!\n\r", ch);
		return TRUE;
	}
	else if (IS_EXTRA (victim, EXTRA_OSWITCH))
	{
		send_to_char ("You cannot attack an object.\n\r", ch);
		return TRUE;
	}

	if (IS_VAMPAFF (ch, VAM_EARTHMELDED) || IS_VAMPAFF (victim, VAM_EARTHMELDED))
		return TRUE;

	/* Thx Josh! */
	if (victim->fighting == ch)
		return FALSE;

	if (IS_NPC (ch) && IS_NEWBIE (victim) && IS_SET (ch->act, ACT_COMMANDED))
	{
		send_to_char ("Newbie killing of ANY form is forbidden.\n\r", ch);
		return TRUE;
	}
	if (IS_NPC (victim) && IS_NEWBIE (ch) && IS_SET (victim->act, ACT_COMMANDED))
	{
		send_to_char ("Newbies cannot attack player-controlled mobs.\n\r", ch);
		return TRUE;
	}
	if (IS_ANIMAL (ch) && IS_MORE (victim, MORE_ANIMAL_MASTER))
	{
		send_to_char ("You like them too much to attack them.\n\r", ch);
		return TRUE;
	}

	if (IS_ITEMAFF (ch, ITEMA_PEACE))
	{
		send_to_char ("You are unable to attack them.\n\r", ch);
		return TRUE;
	}

	if (IS_ITEMAFF (victim, ITEMA_PEACE))
	{
		if (IS_WEREWOLF (victim))
			send_to_char ("They look FAR too scary to attack!\n\r", ch);
		else
			send_to_char ("They look FAR too imposing to attack!\n\r", ch);
		return TRUE;
	}

	if (IS_NPC (ch) || IS_NPC (victim))
		return FALSE;

	if (ch->trust > LEVEL_BUILDER)
	{
		send_to_char ("Attacking an Immortal is never a good idea!\n\r", ch);
		return FALSE;
	}

	if (victim->trust > LEVEL_BUILDER)
	{
		send_to_char ("Attacking an Immortal is never a good idea!\n\r", ch);
		return FALSE;
	}

	if (!CAN_PK (ch) || !CAN_PK (victim))
	{
		send_to_char ("Mortals are not permitted to fight other players.\n\r", ch);
		return TRUE;
	}

	if (ch->desc == NULL || victim->desc == NULL)
	{
		if (victim->position != POS_FIGHTING && victim->position > POS_STUNNED)
		{
			send_to_char ("They are currently link dead.\n\r", ch);
			return TRUE;
		}
	}

	if (!IS_NPC (ch) && !IS_NPC (victim))
	{
		if (IS_NEWBIE (ch))
		{
			send_to_char ("You cannot attack players while you are a newbie.\n\r", ch);
			return TRUE;
		}
		else if (IS_NEWBIE (victim))
		{
			send_to_char ("You cannot attack newbies.\n\r", ch);
			return TRUE;
		}

		if (IS_MORE (ch, MORE_LOYAL) && strlen (ch->pcdata->conding) > 1 && !str_cmp (ch->pcdata->conding, victim->name))
		{
			send_to_char ("You refuse to attack your master!\n\r", ch);
			return TRUE;
		}
		else if (strlen (ch->pcdata->love) > 1 && !str_cmp (ch->pcdata->love, victim->name))
		{
			send_to_char ("How could you attack someone you love?!\n\r", ch);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * Set position of a victim.
 */
void update_pos (CHAR_DATA * victim)
{
	CHAR_DATA *mount;

	if (victim->hit > 0)
	{
		if (victim->position <= POS_STUNNED)
		{
			bool recover_chance = FALSE;
			victim->position = POS_STANDING;
			if (number_percent () > 90)
				recover_chance = TRUE;
			if (!IS_NPC (victim) && !IS_SET (victim->extra2, EXTRA2_DAEMON_REGEN) && !IS_SET (victim->extra2, EXTRA2_TORPORED))
			{
				if (victim->max_hit * 0.25 > victim->hit || !recover_chance)
				{
					act ("$n clambers back to $s feet.", victim, NULL, NULL, TO_ROOM);
					act ("You clamber back to your feet.", victim, NULL, NULL, TO_CHAR);
				}
				else
				{
					act ("$n flips back up to $s feet.", victim, NULL, NULL, TO_ROOM);
					act ("You flip back up to your feet.", victim, NULL, NULL, TO_CHAR);
				}
			}
		}
		return;
	}
	else if ((mount = victim->mount) != NULL)
	{
		if (victim->mounted == IS_MOUNT)
		{
			act ("$n rolls off $N.", mount, NULL, victim, TO_ROOM);
			act ("You roll off $N.", mount, NULL, victim, TO_CHAR);
		}
		else if (victim->mounted == IS_RIDING)
		{
			act ("$n falls off $N.", victim, NULL, mount, TO_ROOM);
			act ("You fall off $N.", victim, NULL, mount, TO_CHAR);
		}
		mount->mount = NULL;
		victim->mount = NULL;
		mount->mounted = IS_ON_FOOT;
		victim->mounted = IS_ON_FOOT;
	}

	if (IS_NPC (victim) || victim->hit <= -11)
	{
		victim->position = POS_DEAD;
		return;
	}

	if (victim->hit <= -6)
		victim->position = POS_MORTAL;
	else if (victim->hit <= -3)
		victim->position = POS_INCAP;
	else
		victim->position = POS_STUNNED;

	if (victim->sat != NULL && !CAN_SIT (victim))
	{
		victim->sat->sat = NULL;
		victim->sat = NULL;
	}
	return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse (CHAR_DATA * ch, CHAR_DATA * maker)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *corpse;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	char *name;

	if (IS_NPC (ch))
	{
		name = ch->short_descr;
		corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_NPC), 0);
		corpse->timer = number_range (60, 120);
		corpse->value[2] = ch->pIndexData->vnum;
		if (ch->gold > 0)
		{
			obj_to_obj (create_money (ch->gold), corpse);
			ch->gold = 0;
		}
	}
	else
	{
		name = ch->name;
		corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_PC), 0);
		corpse->timer = number_range (600, 1200);
		/* Why should players keep their gold? */
		if (ch->gold > 0)
		{
			obj = create_money (ch->gold);
			if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && (!IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
				SET_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
			obj_to_obj (obj, corpse);
			ch->gold = 0;
		}
	}

	free_string (corpse->questmaker);
	if (IS_NPC (maker))
		corpse->questmaker = str_dup (maker->short_descr);
	else
		corpse->questmaker = str_dup (maker->name);

	free_string (corpse->questowner);
	if (IS_NPC (ch))
		corpse->questowner = str_dup (ch->short_descr);
	else
		corpse->questowner = str_dup (ch->name);

	if (ch->in_room != NULL)
		corpse->specpower = ch->in_room->vnum;

	sprintf (buf, corpse->short_descr, name);
	free_string (corpse->short_descr);
	corpse->short_descr = str_dup (buf);

	sprintf (buf, corpse->description, name);
	free_string (corpse->description);
	corpse->description = str_dup (buf);

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;

		obj_from_char (obj);
		if (IS_SET (obj->extra_flags, ITEM_VANISH))
			extract_obj (obj);
		else
		{
			if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && (!IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
				SET_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
			obj_to_obj (obj, corpse);
		}
	}

	/* If you die in the shadowplane, your corpse should stay there */
	if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
		SET_BIT (corpse->extra_flags, ITEM_SHADOWPLANE);

	obj_to_room (corpse, ch->in_room);
	return;
}



void raw_kill (CHAR_DATA * victim, CHAR_DATA * ch)
{
	CHAR_DATA *mount;
	if (IS_NPC(victim))
	{
		mprog_death_trigger (victim);	//mob programs
	}
	victim->agg = 0;
	if (IS_NPC (victim) && !IS_NPC (ch))
		ch->mkill++;
	if (!IS_NPC (victim) && IS_NPC (ch))
		victim->mdeath++;
	if (!IS_NPC (victim) && !IS_NPC (ch) && ch != victim)
	{
		ch->pkill++;
		victim->pdeath++;
	}
	if (IS_NPC (victim) && (IS_EXTRA (victim, EXTRA_ILLUSION)))
		act ("$n slowly fades away to nothing.", victim, NULL, NULL, TO_ROOM);
	else if (IS_NPC (victim) && strlen (victim->powertype) > 1 && !str_cmp (victim->powertype, "zombie"))
		act ("$n crumbles to dust as $e hits the floor.", victim, NULL, NULL, TO_ROOM);
	else if (!IS_NPC (victim) || !IS_EXTRA (victim, EXTRA_ILLUSION))
	{
		make_corpse (victim, ch);
	}

	if ((mount = victim->mount) != NULL)
	{
		if (victim->mounted == IS_MOUNT)
		{
			act ("$n rolls off the corpse of $N.", mount, NULL, victim, TO_ROOM);
			act ("You roll off the corpse of $N.", mount, NULL, victim, TO_CHAR);
		}
		else if (victim->mounted == IS_RIDING)
		{
			act ("$n falls off $N.", victim, NULL, mount, TO_ROOM);
			act ("You fall off $N.", victim, NULL, mount, TO_CHAR);
		}
		mount->mount = NULL;
		victim->mount = NULL;
		mount->mounted = IS_ON_FOOT;
		victim->mounted = IS_ON_FOOT;
	}

	victim->position = POS_DEAD;

	if (IS_NPC (victim))
	{
		victim->pIndexData->killed++;
		char_from_room (victim);
		char_to_room (victim, get_room_index (ROOM_VNUM_DISCONNECTION));
		extract_char (victim, TRUE);
		return;
	}
	victim->hit = 1;
	REMOVE_BIT (victim->extra, EXTRA_DYING);
	extract_char (victim, FALSE);
	while (victim->affected)
		affect_remove (victim, victim->affected);
	if (IS_AFFECTED (victim, AFF_POLYMORPH) && IS_AFFECTED (victim, AFF_ETHEREAL))
	{
		victim->affected_by = AFF_POLYMORPH + AFF_ETHEREAL;
	}
	else if (IS_AFFECTED (victim, AFF_POLYMORPH))
		victim->affected_by = AFF_POLYMORPH;
	else if (IS_AFFECTED (victim, AFF_ETHEREAL))
		victim->affected_by = AFF_ETHEREAL;
	else
		victim->affected_by = 0;

	REMOVE_BIT (victim->extra, TIED_UP);
	REMOVE_BIT (victim->extra, GAGGED);
	REMOVE_BIT (victim->extra, BLINDFOLDED);
	REMOVE_BIT (victim->extra, EXTRA_PREGNANT);
	REMOVE_BIT (victim->extra, EXTRA_LABOUR);
	REMOVE_BIT (victim->extra, EXTRA_NO_ACTION);
	victim->pcdata->demonic = 0;

	victim->itemaffect = 0;
	victim->loc_hp[0] = 0;
	victim->loc_hp[1] = 0;
	victim->loc_hp[2] = 0;
	victim->loc_hp[3] = 0;
	victim->loc_hp[4] = 0;
	victim->loc_hp[5] = 0;
	victim->loc_hp[6] = 0;
	victim->blood[0] = victim->blood[1];
	victim->armor = 100;
	victim->position = POS_RESTING;
	victim->hit = UMAX (1, victim->hit);
	victim->mana = UMAX (1, victim->mana);
	victim->move = UMAX (1, victim->move);
	victim->hitroll = 0;
	victim->damroll = 0;
	victim->saving_throw = 0;
	victim->carry_weight = 0;
	victim->carry_number = 0;
	do_call (victim, "all");
	do_autosave (victim, "");
	return;
}


void do_sla (CHAR_DATA * ch, char *argument)
{
	send_to_char ("If you want to SLAY, spell it out.\n\r", ch);
	return;
}



void do_slay (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Slay whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (ch == victim)
	{
		send_to_char ("Suicide is a mortal sin.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim) && victim->level >= ch->level)
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	act ("You slay $M in cold blood!", ch, NULL, victim, TO_CHAR);
	act ("$n slays you in cold blood!", ch, NULL, victim, TO_VICT);
	act ("$n slays $N in cold blood!", ch, NULL, victim, TO_NOTVICT);

	victim->form = 18;
	raw_kill (victim, ch);

	return;
}
