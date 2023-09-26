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


char *const dir_name[] = {
	"north", "east", "south", "west", "up", "down"
};

const sh_int rev_dir[] = {
	2, 3, 0, 1, 5, 4
};

const sh_int movement_loss[SECT_MAX] = {
	1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6
};



/*
 * Local functions.
 */
int find_door args ((CHAR_DATA * ch, char *arg));
bool has_key args ((CHAR_DATA * ch, int key));
void show_page args ((CHAR_DATA * ch, OBJ_DATA * book, int pnum, bool pagefalse));
void show_runes args ((CHAR_DATA * ch, OBJ_DATA * page, bool endline));
bool are_runes args ((OBJ_DATA * page));



void move_char (CHAR_DATA * ch, int door)
{
	CHAR_DATA *fch;
	CHAR_DATA *fch_next;
	CHAR_DATA *mount;
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char mount2[MAX_INPUT_LENGTH];
	char leave[20];
	int revdoor;
	int vnum = 0;
	int getroom = ch->room;
	

	if (door < 0 || door > 5)
	{
		bug ("Do_move: bad door %d.", door);
		return;
	}

	if (ch->embraced != ARE_NONE)
	{
		send_to_char ("You are unable to break your embrace.\n\r", ch);
		return;
	}

	in_room = ch->in_room;
	if ((pexit = in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
	{
		send_to_char ("Alas, you cannot go that way.\n\r", ch);
		return;
	}

	if (IS_SET (ch->extra2, EXTRA2_JUSTDIED))
	{
		REMOVE_BIT (ch->extra2, EXTRA2_JUSTDIED);
	}

	if (IS_SET (pexit->exit_info, EX_WALL_WATER))
	{
		send_to_char ("You are unable to pass the wall of water.\n\r", ch);
		return;
	}

	if (IS_SET (pexit->exit_info, EX_WALL_GRANITE))
	{
		send_to_char ("You are unable to pass the wall of granite.\n\r", ch);
		return;
	}

	if (IS_SET (pexit->exit_info, EX_WALL_FLAME))
	{
		int sn, spelltype;
		int ch_hp = ch->hit;
		send_to_char ("You step through the wall of searing flames!\n\r", ch);
		act ("$n gingerly steps through the wall of searing flames!", ch, NULL, NULL, TO_ROOM);
		if ((sn = skill_lookup ("fire breath")) >= 0)
		{
			spelltype = skill_table[sn].target;
			(*skill_table[sn].spell_fun) (sn, 25, ch, ch);
		}
		if (ch == NULL || ch->position == POS_DEAD)
			return;
		if (ch->hit < ch_hp)
		{
			act ("$n's flesh smolders as $e passes through the wall of searing flames!", ch, NULL, NULL, TO_ROOM);
			send_to_char ("Your flesh smolders as you pass through the wall of searing flames!\n\r", ch);
			if (number_percent () <= 5)
				SET_BIT (ch->affected_by, AFF_FLAMING);
		}
	}

	if (IS_MORE2 (ch, MORE2_DEMENTATION4))
	{
		send_to_char ("You look around confused..what were you doing again?\n\r", ch);
		return;
	}


	if (IS_SET (pexit->exit_info, EX_WARD_GHOULS) && IS_GHOUL (ch))
	{
		send_to_char ("Something prevents you from moving that way.\n\r", ch);
		return;
	}

	if (IS_SET (pexit->exit_info, EX_WARD_LUPINES) && IS_WEREWOLF (ch))
	{
		send_to_char ("Something prevents you from moving that way.\n\r", ch);
		return;
	}

	if (IS_SET (pexit->exit_info, EX_WARD_KINDRED) && IS_VAMPIRE (ch))
	{
		send_to_char ("Something prevents you from moving that way.\n\r", ch);
		return;
	}

	vnum = in_room->vnum;

	if (IS_SET (pexit->exit_info, EX_CLOSED) && pexit->keyword != NULL && !IS_AFFECTED (ch, AFF_PASS_DOOR) && !IS_AFFECTED (ch, AFF_ETHEREAL) && !IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_STORYTELLER(ch))
	{
		int eff_str = get_curr_str (ch) * 3;
		if (!IS_NPC (ch) && (IS_VAMPIRE (ch) || IS_GHOUL (ch)) && get_disc (ch, DISC_POTENCE) > 0)
			eff_str += (get_disc (ch, DISC_POTENCE) * 10);

		if (eff_str >= 100)
		{
			act ("You smash open the $d.", ch, NULL, pexit->keyword, TO_CHAR);
			act ("$n smashes open the $d.", ch, NULL, pexit->keyword, TO_ROOM);
			REMOVE_BIT (pexit->exit_info, EX_CLOSED);
		}
		else if (!IS_NPC (ch) && IS_WEREWOLF (ch) && ch->pcdata->disc[TOTEM_BOAR] > 0)
		{
			act ("You smash open the $d.", ch, NULL, pexit->keyword, TO_CHAR);
			act ("$n smashes open the $d.", ch, NULL, pexit->keyword, TO_ROOM);
			REMOVE_BIT (pexit->exit_info, EX_CLOSED);
		}
		else
		{
			act ("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
			return;
		}
	}

	if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL && in_room == ch->master->in_room)
	{
		send_to_char ("What?  And leave your beloved master?\n\r", ch);
		return;
	}

	if (IS_NPC (ch) && (mount = ch->mount) != NULL && IS_SET (ch->mounted, IS_MOUNT))
	{
		send_to_char ("You better wait for instructions from your rider.\n\r", ch);
		return;
	}

	if (room_is_private (to_room))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}

	if (!IS_NPC (ch))
	{
		int move = 0;
		if (in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR)
		{
			if (!IS_AFFECTED (ch, AFF_FLYING) && ((!IS_NPC (ch) && !IS_VAMPAFF (ch, VAM_FLYING)) && !IS_NPC (ch) && (!IS_NPC (ch))) && !((mount = ch->mount) != NULL && IS_SET (ch->mounted, IS_RIDING) && IS_AFFECTED (mount, AFF_FLYING)))
			{
				send_to_char ("You can't fly.\n\r", ch);
				return;
			}
		}

		if (in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		{
			OBJ_DATA *obj;
			bool found;

			/*
			 * Look for a boat.
			 */
			found = FALSE;
			if (!IS_NPC (ch))
			{
				if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_STARGAZERS) > 0)
					found = TRUE;
				else if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_UKTENA) > 1)
					found = TRUE;
				else if (IS_VAMPAFF (ch, VAM_FLYING))
					found = TRUE;
				else if (IS_POLYAFF (ch, POLY_SERPENT))
					found = TRUE;
				 else if (!IS_NPC(ch) && (IS_VAMPIRE(ch) || IS_GHOUL(ch)) && get_disc(ch,DISC_CELERITY) > 8)
					 found = TRUE;
			}
			if (IS_AFFECTED (ch, AFF_FLYING))
				found = TRUE;
			else if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
				found = TRUE;
			else if (IS_POLYAFF (ch, POLY_FISH))
				found = TRUE;
			else if (ch->abilities[TALENTS][TAL_ATHLETICS] > 1)
				found = TRUE;
			else if ((mount = ch->mount) != NULL && IS_SET (ch->mounted, IS_RIDING) && IS_AFFECTED (mount, AFF_FLYING))
				found = TRUE;
			if (!found)
			{
				for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
				{
					if (obj->item_type == ITEM_BOAT)
					{
						found = TRUE;
						break;
					}
				}
				if (!found)
				{
					send_to_char ("You need a boat to go there.\n\r", ch);
					return;
				}
			}
		}
		else if (!IS_AFFECTED (ch, AFF_FLYING) && IS_POLYAFF (ch, POLY_FISH))
		{
			bool from_ok = FALSE;
			bool to_ok = FALSE;
			if (in_room->sector_type == SECT_WATER_NOSWIM)
				from_ok = TRUE;
			if (in_room->sector_type == SECT_WATER_SWIM)
				from_ok = TRUE;
			if (to_room->sector_type == SECT_UNDERWATER)
				to_ok = TRUE;
			if (to_room->sector_type == SECT_WATER_NOSWIM)
				to_ok = TRUE;
			if (to_room->sector_type == SECT_WATER_SWIM)
				to_ok = TRUE;
			if (!from_ok || !to_ok)
			{
				if (in_room->sector_type != SECT_UNDERWATER)
				{
					act ("$n wriggles helplessly on the floor.", ch, NULL, NULL, TO_ROOM);
					send_to_char ("You cannot cross land in this form.\n\r", ch);
				}
				else
					send_to_char ("You cannot cross land in this form.\n\r", ch);
				return;
			}
		}

		if (IS_HERO (ch))
			move = 0;
		else
			move = movement_loss[UMIN (SECT_MAX - 1, in_room->sector_type)] + movement_loss[UMIN (SECT_MAX - 1, to_room->sector_type)];

		if (IS_SET (ch->mounted, IS_RIDING) && ch->move < move)
		{
			send_to_char ("You are too exhausted.\n\r", ch);
			return;
		}

		WAIT_STATE (ch, 1);
		if (!IS_SET (ch->mounted, IS_RIDING))
			ch->move -= move;
	}

	/* Check for mount message  KaVir */
	if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
	{
		if (IS_NPC (mount))
			sprintf (mount2, " on %s.", mount->short_descr);
		else
			sprintf (mount2, " on %s.", mount->name);
	}
	else
		sprintf (mount2, ".");

	if (IS_NPC (ch) && ch->pIndexData->vnum == MOB_VNUM_EYE)
		strcpy (leave, "rolls");
	else if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH) && ch->pcdata->obj_vnum == OBJ_VNUM_SPILT_BLOOD)
		strcpy (leave, "pours");
	else if (IS_EXTRA (ch, EXTRA_OSWITCH))
		strcpy (leave, "rolls");
	else if (IS_AFFECTED (ch, AFF_ETHEREAL))
		strcpy (leave, "floats");
	else if (in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		strcpy (leave, "swims");
	else if (IS_SET (ch->polyaff, POLY_SERPENT))
		strcpy (leave, "slithers");
	else if (IS_SET (ch->polyaff, POLY_WOLF))
		strcpy (leave, "stalks");
	else if (IS_SET (ch->polyaff, POLY_FROG))
		strcpy (leave, "hops");
	else if (IS_SET (ch->polyaff, POLY_FISH))
		strcpy (leave, "swims");
	else if (in_room->sector_type == SECT_WATER_SWIM || to_room->sector_type == SECT_WATER_SWIM)
		strcpy (leave, "wades");
	else
		strcpy (leave, "walks");

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *victim;

		if ((victim = d->character) == NULL)
			continue;

		if (ch->in_room == NULL || victim->in_room == NULL)
			continue;

		if (ch == victim || ch->in_room != victim->in_room)
			continue;

		if (d->connected != CON_PLAYING)
			continue;

		if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_AFFECTED (ch, AFF_SNEAK) && !IS_MORE3(ch, MORE3_OBFUS2) && !IS_VAMPAFF (ch, VAM_EARTHMELDED) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)) && can_see (victim, ch))
		{
			if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED (mount, AFF_FLYING)) || IS_AFFECTED (ch, AFF_FLYING) || (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_FLYING)))
				sprintf (poly, "%s flies $T%s", ch->morph, mount2);
			else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
				sprintf (poly, "%s rides $T%s", ch->morph, mount2);
			else
				sprintf (poly, "%s %s $T%s", ch->morph, leave, mount2);
			act (poly, victim, NULL, dir_name[door], TO_CHAR);
		}
		else if (!IS_VAMPAFF (ch, VAM_EARTHMELDED) && !IS_AFFECTED (ch, AFF_SNEAK) && !IS_MORE3(ch, MORE3_OBFUS2) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)) && can_see (victim, ch))
		{
			if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED (mount, AFF_FLYING)) || IS_AFFECTED (ch, AFF_FLYING) || (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_FLYING)))
				sprintf (poly, "$n flies %s%s", dir_name[door], mount2);
			else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
				sprintf (poly, "$n rides %s%s", dir_name[door], mount2);
			else
				sprintf (poly, "$n %s %s%s", leave, dir_name[door], mount2);
			act (poly, ch, NULL, victim, TO_VICT);
		}
	}

	ch->room = getroom;
	char_from_room (ch);
	char_to_room (ch, to_room);

	if (door == 0)
	{
		revdoor = 2;
		sprintf (buf, "the south");
	}
	else if (door == 1)
	{
		revdoor = 3;
		sprintf (buf, "the west");
	}
	else if (door == 2)
	{
		revdoor = 0;
		sprintf (buf, "the north");
	}
	else if (door == 3)
	{
		revdoor = 1;
		sprintf (buf, "the east");
	}
	else if (door == 4)
	{
		revdoor = 5;
		sprintf (buf, "below");
	}
	else
	{
		revdoor = 4;
		sprintf (buf, "above");
	}


	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *victim;

		if ((victim = d->character) == NULL)
			continue;

		if (ch->in_room == NULL || victim->in_room == NULL)
			continue;

		if (ch == victim || ch->in_room != victim->in_room)
			continue;

		if (d->connected != CON_PLAYING)
			continue;

		if (!IS_NPC (ch) && !IS_AFFECTED (ch, AFF_SNEAK) && IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_EARTHMELDED) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)) && can_see (victim, ch))
		{
			if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED (mount, AFF_FLYING)) || IS_AFFECTED (ch, AFF_FLYING) || (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_FLYING)))
				sprintf (poly, "%s flies in from %s%s", ch->morph, buf, mount2);
			else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
				sprintf (poly, "%s rides in from %s%s", ch->morph, buf, mount2);
			else
				sprintf (poly, "%s %s in from %s%s", ch->morph, leave, buf, mount2);
			act (poly, ch, NULL, victim, TO_VICT);
		}
		else if (!IS_AFFECTED (ch, AFF_SNEAK) && can_see (victim, ch) && !IS_MORE3(ch, MORE3_OBFUS2) && !IS_VAMPAFF (ch, VAM_EARTHMELDED) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)))
		{
			if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED (mount, AFF_FLYING)) || IS_AFFECTED (ch, AFF_FLYING) || (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_FLYING)))
				sprintf (poly, "$n flies in from %s%s", buf, mount2);
			
			else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
				sprintf (poly, "$n rides in from %s%s", buf, mount2);
			else
				sprintf (poly, "$n %s in from %s%s", leave, buf, mount2);
			act (poly, ch, NULL, victim, TO_VICT);
		}
	}
	do_look (ch, "auto");

	for (fch = in_room->people; fch != NULL; fch = fch_next)
	{
		fch_next = fch->next_in_room;
		if ((mount = fch->mount) != NULL && mount == ch && IS_SET (fch->mounted, IS_MOUNT))
		{
			act ("$N digs $S heels into you.", fch, NULL, ch, TO_CHAR);
			char_from_room (fch);
			char_to_room (fch, ch->in_room);
		}

		if (fch->master == ch && fch->position == POS_STANDING && fch->in_room != ch->in_room && !IS_VAMPAFF (fch, VAM_EARTHMELDED) && !IS_EXTRA (fch, EXTRA_OSWITCH))
		{
			act ("You follow $N.", fch, NULL, ch, TO_CHAR);
			move_char (fch, door);
		}
	}

	mprog_entry_trigger (ch);	//mob programs
	mprog_greet_trigger (ch);	//mob programs

	room_text (ch, ">ENTER<");
	return;
}



void do_north (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (IS_AFFECTED (ch, AFF_WEBBED))
	{
		send_to_char ("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_SHADOW))
	{
		send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_JAIL))
	{
		send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
		return;
	}
	in_room = ch->in_room;
	move_char (ch, DIR_NORTH);

	return;
}



void do_east (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (IS_AFFECTED (ch, AFF_WEBBED))
	{
		send_to_char ("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_SHADOW))
	{
		send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_JAIL))
	{
		send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
		return;
	}
	in_room = ch->in_room;
	move_char (ch, DIR_EAST);

	return;
}



void do_south (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (IS_AFFECTED (ch, AFF_WEBBED))
	{
		send_to_char ("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_SHADOW))
	{
		send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_JAIL))
	{
		send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
		return;
	}
	in_room = ch->in_room;
	move_char (ch, DIR_SOUTH);

	return;
}



void do_west (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (IS_AFFECTED (ch, AFF_WEBBED))
	{
		send_to_char ("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_SHADOW))
	{
		send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_JAIL))
	{
		send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
		return;
	}
	in_room = ch->in_room;
	move_char (ch, DIR_WEST);

	return;
}



void do_up (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (IS_AFFECTED (ch, AFF_WEBBED))
	{
		send_to_char ("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_SHADOW))
	{
		send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_JAIL))
	{
		send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
		return;
	}
	in_room = ch->in_room;
	move_char (ch, DIR_UP);

	return;
}



void do_down (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (IS_AFFECTED (ch, AFF_WEBBED))
	{
		send_to_char ("You are unable to move with all this sticky webbing on.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_SHADOW))
	{
		send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (ch, AFF_JAIL))
	{
		send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
		return;
	}
	in_room = ch->in_room;
	move_char (ch, DIR_DOWN);

	return;
}



int find_door (CHAR_DATA * ch, char *arg)
{
	EXIT_DATA *pexit;
	int door;

	if (!str_cmp (arg, "n") || !str_cmp (arg, "north"))
		door = 0;
	else if (!str_cmp (arg, "e") || !str_cmp (arg, "east"))
		door = 1;
	else if (!str_cmp (arg, "s") || !str_cmp (arg, "south"))
		door = 2;
	else if (!str_cmp (arg, "w") || !str_cmp (arg, "west"))
		door = 3;
	else if (!str_cmp (arg, "u") || !str_cmp (arg, "up"))
		door = 4;
	else if (!str_cmp (arg, "d") || !str_cmp (arg, "down"))
		door = 5;
	else
	{
		for (door = 0; door <= 5; door++)
		{
			if ((pexit = ch->in_room->exit[door]) != NULL && IS_SET (pexit->exit_info, EX_ISDOOR) && pexit->keyword != NULL && is_name (arg, pexit->keyword))
				return door;
		}
		act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
		return -1;
	}

	if ((pexit = ch->in_room->exit[door]) == NULL)
	{
		act ("I see no door $T here.", ch, NULL, arg, TO_CHAR);
		return -1;
	}

	if (!IS_SET (pexit->exit_info, EX_ISDOOR))
	{
		send_to_char ("You can't do that.\n\r", ch);
		return -1;
	}

	return door;
}

/* Designed for the portal spell, but can also have other uses...KaVir
 * V0 = Where the portal will take you.
 * V1 = Number of uses (0 is infinate).
 * V2 = if 2, cannot be entered.
 * V3 = The room the portal is currently in.
 */
void do_enter (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *portal;
	OBJ_DATA *portal_next;
	CHAR_DATA *mount;
	bool found;

	argument = one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Enter what?\n\r", ch);
		return;
	}

	obj = get_obj_list (ch, arg, ch->in_room->contents);
	if (obj == NULL)
	{
		act ("I see no $T here.", ch, NULL, arg, TO_CHAR);
		return;
	}
	if (obj->item_type != ITEM_PORTAL)
	{
		act ("You cannot enter that.", ch, NULL, arg, TO_CHAR);
		return;
	}

	if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_SET (obj->extra_flags, ITEM_SHADOWPLANE))
	{
		send_to_char ("You are too insubstantual.\n\r", ch);
		return;
	}
	else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_SET (obj->extra_flags, ITEM_SHADOWPLANE))
	{
		send_to_char ("It is too insubstantual.\n\r", ch);
		return;
	}

	if (obj->value[2] == 2 || obj->value[2] == 3)
	{
		act ("It seems to be closed.", ch, NULL, arg, TO_CHAR);
		return;
	}

	pRoomIndex = get_room_index (obj->value[0]);
	location = ch->in_room;

	if (pRoomIndex == NULL)
	{
		act ("You are unable to enter.", ch, NULL, arg, TO_CHAR);
		return;
	}

	act ("You step into $p.", ch, obj, NULL, TO_CHAR);
	if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (poly, "%s steps into $p.", ch->morph);
	else
		sprintf (poly, "$n steps into $p.");
	act (poly, ch, obj, NULL, TO_ROOM);
	char_from_room (ch);
	char_to_room (ch, pRoomIndex);
	if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (poly, "%s steps out of $p.", ch->morph);
	else
		sprintf (poly, "$n steps out of $p.");
	act (poly, ch, obj, NULL, TO_ROOM);
	char_from_room (ch);
	char_to_room (ch, location);

	if (obj->value[1] != 0)
	{
		obj->value[1] = obj->value[1] - 1;
		if (obj->value[1] < 1)
		{
			act ("$p vanishes.", ch, obj, NULL, TO_CHAR);
			act ("$p vanishes.", ch, obj, NULL, TO_ROOM);
			obj->timer = 1;
		}
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
				break;
			}
			else if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_SET (portal->extra_flags, ITEM_SHADOWPLANE))
			{
				SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
				break;
			}
			if (portal->value[1] != 0)
			{
				portal->value[1] = portal->value[1] - 1;
				if (portal->value[1] < 1)
				{
					act ("$p vanishes.", ch, portal, NULL, TO_CHAR);
					act ("$p vanishes.", ch, portal, NULL, TO_ROOM);
					portal->timer = 1;
				}
			}
		}
	}
	do_look (ch, "auto");
	if ((mount = ch->mount) == NULL)
		return;
	char_from_room (mount);
	char_to_room (mount, ch->in_room);
	return;
}


void do_open (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int door;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Open what?\n\r", ch);
		return;
	}
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
		
	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* 'open object' */
		if (obj->item_type != ITEM_CONTAINER && obj->item_type != ITEM_BOOK)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's already open.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSEABLE) && obj->item_type != ITEM_BOOK)
		{
			send_to_char ("You can't do that.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's locked.\n\r", ch);
			return;
		}

		REMOVE_BIT (obj->value[1], CONT_CLOSED);
		send_to_char ("Ok.\n\r", ch);
		act ("$n opens $p.", ch, obj, NULL, TO_ROOM);
		return;
	}

	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'open door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's already open.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's locked.\n\r", ch);
			return;
		}

		REMOVE_BIT (pexit->exit_info, EX_CLOSED);
		act ("$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM);
		send_to_char ("Ok.\n\r", ch);

		/* open the other side */
		if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->to_room == ch->in_room)
		{
			CHAR_DATA *rch;

			REMOVE_BIT (pexit_rev->exit_info, EX_CLOSED);
			for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
				act ("The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR);
		}
	}

	return;
}



void do_close (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int door;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Close what?\n\r", ch);
		return;
	}
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);

	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* 'close object' */
		if (obj->item_type != ITEM_CONTAINER && obj->item_type != ITEM_BOOK)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's already closed.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSEABLE) && obj->item_type != ITEM_BOOK)
		{
			send_to_char ("You can't do that.\n\r", ch);
			return;
		}

		SET_BIT (obj->value[1], CONT_CLOSED);
		if (obj->item_type == ITEM_BOOK)
			obj->value[2] = 0;
		send_to_char ("Ok.\n\r", ch);
		act ("$n closes $p.", ch, obj, NULL, TO_ROOM);
		return;
	}

	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'close door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if (IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's already closed.\n\r", ch);
			return;
		}

		SET_BIT (pexit->exit_info, EX_CLOSED);
		act ("$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM);
		send_to_char ("Ok.\n\r", ch);

		/* close the other side */
		if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			CHAR_DATA *rch;

			SET_BIT (pexit_rev->exit_info, EX_CLOSED);
			for (rch = to_room->people; rch != NULL; rch = rch->next_in_room)
				act ("The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR);
		}
	}

	return;
}



void do_turn (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int value = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: Turn <book> <forward/back>.\n\r", ch);
		return;
	}

	if (is_number (arg2))
		value = atoi (arg2);

	if ((obj = get_obj_here (ch, arg1)) == NULL)
	{
		send_to_char ("You don't have that book.\n\r", ch);
		return;
	}

	if (obj->item_type != ITEM_BOOK)
	{
		send_to_char ("That's not a book.\n\r", ch);
		return;
	}
	if (IS_SET (obj->value[1], CONT_CLOSED))
	{
		send_to_char ("First you should open it.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "f") || !str_cmp (arg2, "forward"))
	{
		if (obj->value[2] >= obj->value[3])
		{
			send_to_char ("But you are already at the end of the book.\n\r", ch);
			return;
		}
		obj->value[2] += 1;
		act ("You flip forward a page in $p.", ch, obj, NULL, TO_CHAR);
		act ("$n flips forward a page in $p.", ch, obj, NULL, TO_ROOM);
	}
	else if (!str_cmp (arg2, "b") || !str_cmp (arg2, "backward"))
	{
		if (obj->value[2] <= 0)
		{
			send_to_char ("But you are already at the beginning of the book.\n\r", ch);
			return;
		}
		obj->value[2] -= 1;
		act ("You flip backward a page in $p.", ch, obj, NULL, TO_CHAR);
		act ("$n flips backward a page in $p.", ch, obj, NULL, TO_ROOM);
	}
	else if (is_number (arg2) && value >= 0 && value <= obj->value[3])
	{
		if (value == obj->value[2])
		{
			act ("$p is already open at that page.", ch, obj, NULL, TO_CHAR);
			return;
		}
		else if (value < obj->value[2])
		{
			act ("You flip backwards through $p.", ch, obj, NULL, TO_CHAR);
			act ("$n flips backwards through $p.", ch, obj, NULL, TO_ROOM);
		}
		else
		{
			act ("You flip forwards through $p.", ch, obj, NULL, TO_CHAR);
			act ("$n flips forwards through $p.", ch, obj, NULL, TO_ROOM);
		}
		obj->value[2] = value;
	}
	else
		send_to_char ("Do you wish to turn forward or backward a page?\n\r", ch);
	return;
}

void do_read (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("What do you wish to read?\n\r", ch);
		return;
	}

	if ((obj = get_obj_here (ch, arg)) == NULL)
	{
		send_to_char ("You don't have that book.\n\r", ch);
		return;
	}

	if (obj->item_type == ITEM_PAGE)
	{
		if (!str_cmp (obj->victpoweruse, "(null)"))
			sprintf (buf, "Untitled page.\n\r");
		else
			sprintf (buf, "%s.\n\r", obj->victpoweruse);
		buf[0] = UPPER (buf[0]);
		send_to_char (buf, ch);
		if (obj->chpoweruse == NULL || obj->chpoweruse == '\0' || !str_cmp (obj->chpoweruse, "(null)"))
		{
			if (!are_runes (obj))
				send_to_char ("This page is blank.\n\r", ch);
			else
				send_to_char ("This page is blank.\n\r", ch);
			return;
		}
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char (obj->chpoweruse, ch);
		send_to_char ("\n\r--------------------------------------------------------------------------------\n\r", ch);

		return;
	}

	if (obj->item_type != ITEM_BOOK)
	{
		send_to_char ("That's not a book.\n\r", ch);
		return;
	}
	if (IS_SET (obj->value[1], CONT_CLOSED))
	{
		if (!str_cmp (obj->victpoweruse, "(null)"))
			sprintf (buf, "The book is untitled.\n\r");
		else
			sprintf (buf, "The book is titled '%s'.\n\r", obj->victpoweruse);
		buf[0] = UPPER (buf[0]);
		send_to_char (buf, ch);
		return;
	}

	if (obj->value[2] == 0)
	{
		int page;

		send_to_char ("Index page.\n\r", ch);
		if (obj->value[3] <= 0)
		{
			send_to_char ("<No pages>\n\r", ch);
			return;
		}
		for (page = 1; page <= obj->value[3]; page++)
		{
			sprintf (buf, "Page %d:", page);
			send_to_char (buf, ch);
			show_page (ch, obj, page, TRUE);
		}
	}
	else
	{
		sprintf (buf, "Page %d:", obj->value[2]);
		send_to_char (buf, ch);
		show_page (ch, obj, obj->value[2], FALSE);
	}
	return;
}

void show_page (CHAR_DATA * ch, OBJ_DATA * book, int pnum, bool pagefalse)
{
	OBJ_DATA *page;
	OBJ_DATA *page_next;
	char buf[MAX_STRING_LENGTH];
	bool found = FALSE;

	for (page = book->contains; page != NULL; page = page_next)
	{
		page_next = page->next_content;
		if (page->value[0] == pnum)
		{
			found = TRUE;
			if (!str_cmp (page->victpoweruse, "(null)"))
				sprintf (buf, "Untitled page.\n\r");
			else
				sprintf (buf, "%s.\n\r", page->victpoweruse);
			buf[0] = UPPER (buf[0]);
			send_to_char (buf, ch);
			if (!pagefalse)
			{
				if (page->chpoweruse == NULL || page->chpoweruse == '\0' || !str_cmp (page->chpoweruse, "(null)"))
				{
					if (!are_runes (page))
						send_to_char ("This page is blank.\n\r", ch);
					else
						send_to_char ("This page is blank.\n\r", ch);
					return;
				}
				send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
				send_to_char (page->chpoweruse, ch);
				send_to_char ("\n\r--------------------------------------------------------------------------------\n\r", ch);
			}
		}
	}
	if (!found)
		send_to_char ("This page has been torn out.\n\r", ch);
	return;
}

void show_runes (CHAR_DATA * ch, OBJ_DATA * page, bool endline)
{
	if (page->value[1] + page->value[2] + page->value[3] < 1)
		return;
	send_to_char ("This page contains the following symbols:\n\r", ch);
	send_to_char ("Runes:", ch);
	if (page->value[1] > 0)
	{
		if (IS_SET (page->value[1], RUNE_FIRE))
			send_to_char (" Fire", ch);
		if (IS_SET (page->value[1], RUNE_AIR))
			send_to_char (" Air", ch);
		if (IS_SET (page->value[1], RUNE_EARTH))
			send_to_char (" Earth", ch);
		if (IS_SET (page->value[1], RUNE_WATER))
			send_to_char (" Water", ch);
		if (IS_SET (page->value[1], RUNE_DARK))
			send_to_char (" Dark", ch);
		if (IS_SET (page->value[1], RUNE_LIGHT))
			send_to_char (" Light", ch);
		if (IS_SET (page->value[1], RUNE_LIFE))
			send_to_char (" Life", ch);
		if (IS_SET (page->value[1], RUNE_DEATH))
			send_to_char (" Death", ch);
		if (IS_SET (page->value[1], RUNE_MIND))
			send_to_char (" Mind", ch);
		if (IS_SET (page->value[1], RUNE_SPIRIT))
			send_to_char (" Spirit", ch);
	}
	else
		send_to_char (" None", ch);
	send_to_char (".\n\r", ch);
	send_to_char ("Glyphs:", ch);
	if (page->value[2] > 0)
	{
		if (IS_SET (page->value[2], GLYPH_CREATION))
			send_to_char (" Creation", ch);
		if (IS_SET (page->value[2], GLYPH_DESTRUCTION))
			send_to_char (" Destruction", ch);
		if (IS_SET (page->value[2], GLYPH_SUMMONING))
			send_to_char (" Summoning", ch);
		if (IS_SET (page->value[2], GLYPH_TRANSFORMATION))
			send_to_char (" Transformation", ch);
		if (IS_SET (page->value[2], GLYPH_TRANSPORTATION))
			send_to_char (" Transportation", ch);
		if (IS_SET (page->value[2], GLYPH_ENHANCEMENT))
			send_to_char (" Enhancement", ch);
		if (IS_SET (page->value[2], GLYPH_REDUCTION))
			send_to_char (" Reduction", ch);
		if (IS_SET (page->value[2], GLYPH_CONTROL))
			send_to_char (" Control", ch);
		if (IS_SET (page->value[2], GLYPH_PROTECTION))
			send_to_char (" Protection", ch);
		if (IS_SET (page->value[2], GLYPH_INFORMATION))
			send_to_char (" Information", ch);
	}
	else
		send_to_char (" None", ch);
	send_to_char (".\n\r", ch);
	send_to_char ("Sigils:", ch);
	if (page->value[3] > 0)
	{
		if (IS_SET (page->value[3], SIGIL_SELF))
			send_to_char (" Self", ch);
		if (IS_SET (page->value[3], SIGIL_TARGETING))
			send_to_char (" Targeting", ch);
		if (IS_SET (page->value[3], SIGIL_AREA))
			send_to_char (" Area", ch);
		if (IS_SET (page->value[3], SIGIL_OBJECT))
			send_to_char (" Object", ch);
	}
	else
		send_to_char (" None", ch);
	send_to_char (".\n\r", ch);
	if (endline)
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
	return;
}

bool are_runes (OBJ_DATA * page)
{
	if (page->value[1] + page->value[2] + page->value[3] < 1)
		return FALSE;
	return TRUE;
}

bool has_key (CHAR_DATA * ch, int key)
{
	OBJ_DATA *obj;

	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->pIndexData->vnum == key)
			return TRUE;
	}

	return FALSE;
}



void do_lock (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int door;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Lock what?\n\r", ch);
		return;
	}

	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* 'lock object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char ("It can't be locked.\n\r", ch);
			return;
		}
		if (!has_key (ch, obj->value[2]))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's already locked.\n\r", ch);
			return;
		}

		SET_BIT (obj->value[1], CONT_LOCKED);
		send_to_char ("*Click*\n\r", ch);
		act ("$n locks $p.", ch, obj, NULL, TO_ROOM);
		return;
	}

	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'lock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char ("It can't be locked.\n\r", ch);
			return;
		}
		if (!has_key (ch, pexit->key))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's already locked.\n\r", ch);
			return;
		}

		SET_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("The door locks with a satisfying *click*.\n\r", ch);
		act ("$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

		/* lock the other side */
		if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != 0 && pexit_rev->to_room == ch->in_room)
		{
			SET_BIT (pexit_rev->exit_info, EX_LOCKED);
		}
	}

	return;
}



void do_unlock (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int door;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Unlock what?\n\r", ch);
		return;
	}

	if ((obj = get_obj_here (ch, arg)) != NULL)
	{
		/* 'unlock object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char ("That's not a container.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char ("It can't be unlocked.\n\r", ch);
			return;
		}
		if (!has_key (ch, obj->value[2]))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's already unlocked.\n\r", ch);
			return;
		}

		REMOVE_BIT (obj->value[1], CONT_LOCKED);
		send_to_char ("*Click*\n\r", ch);
		act ("$n unlocks $p.", ch, obj, NULL, TO_ROOM);
		return;
	}

	if ((door = find_door (ch, arg)) >= 0)
	{
		/* 'unlock door' */
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = ch->in_room->exit[door];
		if (!IS_SET (pexit->exit_info, EX_CLOSED))
		{
			send_to_char ("It's not closed.\n\r", ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char ("It can't be unlocked.\n\r", ch);
			return;
		}
		if (!has_key (ch, pexit->key))
		{
			send_to_char ("You lack the key.\n\r", ch);
			return;
		}
		if (!IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("It's already unlocked.\n\r", ch);
			return;
		}

		REMOVE_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("*Click*\n\r", ch);
		act ("$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM);

		/* unlock the other side */
		if ((to_room = pexit->to_room) != NULL && (pexit_rev = to_room->exit[rev_dir[door]]) != NULL && pexit_rev->to_room == ch->in_room)
		{
			REMOVE_BIT (pexit_rev->exit_info, EX_LOCKED);
		}
	}

	return;
}



void do_knock(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int door, from_door;
    EXIT_DATA *pexit;
	 ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    char buf[MSL];
 
    if( argument[0] == '\0')
    {
        send_to_char("Knock in which direction?\n\r",ch);
        return;
    }
    if( LOWER(argument[0]) == 'n')
    {
      door = DIR_NORTH;
      from_door = DIR_SOUTH;
    }
    else
    if( LOWER(argument[0]) == 'e')
    {
      door = DIR_EAST;
      from_door = DIR_WEST;
    }
    else
    if( LOWER(argument[0]) == 'w')
    {
      door = DIR_WEST;
      from_door = DIR_EAST;
    }
    else
    if( LOWER(argument[0]) == 's')
    {
      door = DIR_SOUTH;
      from_door = DIR_NORTH;
    }
    else
    if( LOWER(argument[0]) == 'u')
    {
       door = DIR_UP;
       from_door = DIR_DOWN;
    }
	 else
    if( LOWER(argument[0]) == 'd')
    {
      door = DIR_DOWN;
      from_door = DIR_UP;
    }
    else
    { 
       send_to_char("Knock in which direction?\n\r",ch);
       return;
    }

    in_room = ch->in_room;
    if( ( pexit = in_room->exit[door]) == NULL)
    {
        send_to_char("There is no door in that direction.\n\r",ch);
        return;
    }

    if( !IS_SET(pexit->exit_info, EX_CLOSED) )
    {
        send_to_char("There is no closed door in that direction.\n\r",ch);
        return;
    }

    act("$n knocks on the $T door.\n\r", ch,NULL,dir_name[door],TO_ROOM);
    send_to_char("You knock on the door.\n\r",ch);

    to_room = pexit->to_room;
    for( vch = to_room->people; vch != NULL; vch = vch_next )
    {
       if (vch->position < POS_SITTING)
       continue;
       vch_next = vch->next_in_room;
       sprintf(buf,"There is a knock on the door.\n\r");
       send_to_char(buf,vch);
    }
    
	 return;
}


void do_stand (CHAR_DATA * ch, char *argument)
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		if (IS_AFFECTED (ch, AFF_SLEEP))
		{
			send_to_char ("You can't wake up!\n\r", ch);
			return;
		}

		if (IS_MORE2 (ch, MORE2_COMA))
		{
			send_to_char ("You can't wake up...your in a coma!\n\r", ch);
			return;
		}

		send_to_char ("You wake and stand up.\n\r", ch);
		act ("$n wakes and stands up.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_STANDING;
		break;

	case POS_RESTING:
	case POS_SITTING:
		send_to_char ("You stand up.\n\r", ch);
		act ("$n stands up.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_STANDING;
		break;

	case POS_MEDITATING:

		send_to_char ("You uncross your legs and stand up.\n\r", ch);
		act ("$n uncrosses $s legs and stands up.", ch, NULL, NULL, TO_ROOM);

		ch->position = POS_STANDING;
		break;

	case POS_STANDING:
		send_to_char ("You are already standing.\n\r", ch);
		break;

	case POS_FIGHTING:
		send_to_char ("You are already fighting!\n\r", ch);
		break;
	}

	if (ch->sat != NULL && !CAN_SIT (ch))
	{
		act ("You get off $p.", ch, ch->sat, NULL, TO_CHAR);
		act ("$n gets off $p.", ch, ch->sat, NULL, TO_ROOM);
		ch->sat->sat = NULL;
		ch->sat = NULL;
	}
	return;
}



void do_rest (CHAR_DATA * ch, char *argument)
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char ("You are already sleeping.\n\r", ch);
		break;

	case POS_RESTING:
		send_to_char ("You are already resting.\n\r", ch);
		break;

	case POS_MEDITATING:
	case POS_SITTING:
	case POS_STANDING:
		send_to_char ("You rest.\n\r", ch);
		act ("$n rests.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_RESTING;
		break;

	case POS_FIGHTING:
		send_to_char ("You are already fighting!\n\r", ch);
		break;
	}
	return;
}



void do_sit (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	bool sit_obj = FALSE;

	one_argument (argument, arg);

	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char ("You are already sleeping.\n\r", ch);
		break;

	case POS_RESTING:
		send_to_char ("You sit up.\n\r", ch);
		act ("$n sits up.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SITTING;
		break;

	case POS_MEDITATING:
		send_to_char ("You are already meditating.\n\r", ch);
		break;

	case POS_SITTING:
		send_to_char ("You are already sitting.\n\r", ch);
		break;

	case POS_STANDING:
		if (arg[0] == '\0')
		{
			send_to_char ("You sit down.\n\r", ch);
			act ("$n sits down.", ch, NULL, NULL, TO_ROOM);
			ch->position = POS_SITTING;
		}
		else
			sit_obj = TRUE;
		break;

	case POS_FIGHTING:
		send_to_char ("You are already fighting!\n\r", ch);
		break;
	}

	if (!sit_obj)
		return;

	/* Furniture value 0: 1 means you can sit on it.
	 */

	if ((obj = get_obj_here (ch, arg)) == NULL)
	{
		send_to_char ("You cannot see that here.\n\r", ch);
		return;
	}
	if (obj->item_type != ITEM_FURNITURE || obj->value[0] != 1 || CAN_WEAR (obj, ITEM_TAKE) || obj->in_room == NULL)
	{
		send_to_char ("You cannot sit on that.\n\r", ch);
		return;
	}
	if (ch->sat != NULL)
	{
		send_to_char ("But you are already sitting down!\n\r", ch);
		return;
	}
	if (obj->sat != NULL)
	{
		send_to_char ("There is already someone sat on it.\n\r", ch);
		return;
	}
	ch->sat = obj;
	obj->sat = ch;
	ch->position = POS_SITTING;
	act ("You sit down on $p.", ch, obj, NULL, TO_CHAR);
	act ("$n sits down on $p.", ch, obj, NULL, TO_ROOM);
	return;
}



void do_wake (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		do_stand (ch, argument);
		return;
	}

	if (!IS_AWAKE (ch))
	{
		send_to_char ("You are asleep yourself!\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_AWAKE (victim))
	{
		act ("$N is already awake.", ch, NULL, victim, TO_CHAR);
		return;
	}
	if (IS_MORE2 (victim, MORE2_COMA))
	{
		act ("You can't wake $M!", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (IS_AFFECTED (victim, AFF_SLEEP))
	{
		act ("You can't wake $M!", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (victim->position < POS_SLEEPING)
	{
		act ("$E doesn't respond!", ch, NULL, victim, TO_CHAR);
		return;
	}

	act ("You wake $M.", ch, NULL, victim, TO_CHAR);
	act ("$n wakes you.", ch, NULL, victim, TO_VICT);
	victim->position = POS_RESTING;
	if (victim->sat != NULL && !CAN_SIT (victim))
	{
		victim->sat->sat = NULL;
		victim->sat = NULL;
	}
	return;
}


void do_train (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	sh_int *pAbility;
	char *pOutput;
	long cost;
	bool last = TRUE;
	int i;
	int j;
	int col = 0;
	int max_train_trait = 0;
	int max_train_ability = MAX_TRAINABLE_ABILITY - 1;
	int temp_num = 0;
	int temp_num2 = 0;

	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;

	cost = 1;

	if (ch->class == CLASS_VAMPIRE)
		max_train_ability = MAX_TRAINABLE_ABILITY;

	if (ch->class == CLASS_VAMPIRE)
	{
		switch (ch->vampgen)
		{
		case 1:
			max_train_trait = 10;
			break;
		case 2:
			max_train_trait = 10;
			break;
		case 3:
			max_train_trait = 10;
			break;
		case 4:
			max_train_trait = 9;
			break;
		case 5:
			max_train_trait = 8;
			break;
		case 6:
			max_train_trait = 7;
			break;
		case 7:
			max_train_trait = 6;
			break;
		default:
			max_train_trait = 5;
			break;
		}
	}
	else
		max_train_trait = 5;

	if (!str_cmp (arg1, "willpower"))
	{
		cost = (ch->pcdata->willpower[WILLPOWER_MAX]) * 100;
		pAbility = &ch->pcdata->willpower[WILLPOWER_MAX];
		pOutput = "willpower";
	}
/*
	else if (!str_cmp (arg1, "conscience"))
	{
		cost = (ch->pcdata->virtues[VIRTUE_CONSCIENCE]) * 2;
		pAbility = &ch->pcdata->virtues[VIRTUE_CONSCIENCE];
		pOutput = "conscience";
	}
	
	else if (!str_cmp (arg1, "faith"))
	{
		cost = (ch->pcdata->virtues[VIRTUE_FAITH]) * 2;
		pAbility = &ch->pcdata->virtues[VIRTUE_FAITH];
		pOutput = "faith";
	}

	else if (!str_cmp (arg1, "wisdom"))
	{
		cost = (ch->pcdata->virtues[VIRTUE_WISDOM]) * 2;
		pAbility = &ch->pcdata->virtues[VIRTUE_WISDOM];
		pOutput = "wisdom";
	}

	else if (!str_cmp (arg1, "zeal"))
	{
		cost = (ch->pcdata->virtues[VIRTUE_ZEAL]) * 2;
		pAbility = &ch->pcdata->virtues[VIRTUE_ZEAL];
		pOutput = "zeal";
	}

	else if (!str_cmp (arg1, "selfcontrol"))
        {
                cost = (ch->pcdata->virtues[VIRTUE_SELFCONTROL]) * 2;
                pAbility = &ch->pcdata->virtues[VIRTUE_SELFCONTROL];
                pOutput = "self control";
        }

	else if (!str_cmp (arg1, "courage"))
        {
		 cost = (ch->pcdata->virtues[VIRTUE_COURAGE]) * 2;
		 pAbility = &ch->pcdata->virtues[VIRTUE_COURAGE];
		 pOutput = "courage";
	}

	else if (!str_cmp (arg1, "road"))
	{
		cost = (ch->pcdata->roadt) * 2;
		pAbility = &ch->pcdata->roadt;
		pOutput = "road";
	}

	else if (!str_cmp (arg1, "rage"))
	{

		if (ch->pcdata->rage[1] >= 10)
		{
			send_to_char("#GYou're rage is already at max.#n\n\r", ch);
			return;
		}
		cost = ch->pcdata->rage[1];
		if (cost > ch->exp || ch->exp < 1)
		{
			send_to_char ("#gYou don't have enough exp.#n\n\r", ch);
			return;
		}

		ch->exp -= (cost < 1 ? 1 : cost);
		ch->pcdata->rage[0] += 1;
		ch->pcdata->rage[1] += 1;
		send_to_char("#GYour rage increases!#n\n\r",ch);
		return;
	}

	else if (!str_cmp (arg1, "gnosis"))
	{
		if (ch->pcdata->gnosis[1] >= 10 || ch->pcdata->gnosis[0] >= 10)
		{
			send_to_char("#BYour gnosis is already at max.#n\n\r", ch);
			return;
		}

		cost = ch->pcdata->gnosis[1] * 2;

		if (cost > ch->exp || ch->exp < 1)
		{
			send_to_char ("#CYou don't have enough exp.#c\n\r", ch);
			return;
		}

		ch->exp -= (cost < 1 ? 1 : cost);
		ch->pcdata->gnosis[0] += 1;
		ch->pcdata->gnosis[1] += 1;
		send_to_char("#BYour gnosis increases!#n\n\r",ch);
		return;
	}

	else if (!str_cmp (arg1, "strength"))
	{
		cost = (ch->attributes[ATTRIB_STR]) * 4;
		pAbility = &ch->attributes[ATTRIB_STR];
		pOutput = "strength";
	}

	else if (!str_cmp (arg1, "dexterity"))
	{
		cost = (ch->attributes[ATTRIB_DEX]) * 4;
		pAbility = &ch->attributes[ATTRIB_DEX];
		pOutput = "dexterity";
	}

	else if (!str_cmp (arg1, "stamina"))
	{
		cost = (ch->attributes[ATTRIB_STA]) * 4;
		pAbility = &ch->attributes[ATTRIB_STA];
		pOutput = "stamina";
	}

	else if (!str_cmp (arg1, "charisma"))
	{
		cost = (ch->attributes[ATTRIB_CHA]) * 4;
		pAbility = &ch->attributes[ATTRIB_CHA];
		pOutput = "charisma";
	}

	else if (!str_cmp (arg1, "manipulation"))
	{
		cost = (ch->attributes[ATTRIB_MAN]) * 4;
		pAbility = &ch->attributes[ATTRIB_MAN];
		pOutput = "manipulation";
	}

	else if (!str_cmp (arg1, "perception"))
	{
		cost = (ch->attributes[ATTRIB_PER]) * 4;
		pAbility = &ch->attributes[ATTRIB_PER];
		pOutput = "perception";
	}

	else if (!str_cmp (arg1, "intelligence"))
	{
		cost = (ch->attributes[ATTRIB_INT]) * 4;
		pAbility = &ch->attributes[ATTRIB_INT];
		pOutput = "intelligence";
	}

	else if (!str_cmp (arg1, "wits"))
	{
		cost = (ch->attributes[ATTRIB_WIT]) * 4;
		pAbility = &ch->attributes[ATTRIB_WIT];
		pOutput = "wits";
	}

	else if (!str_cmp (arg1, "expression"))
	{
		if (ch->abilities[TALENTS][TAL_EXPRESSION] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_EXPRESSION]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_EXPRESSION];
		pOutput = "expression";
	}

	else if (!str_cmp (arg1, "alertness"))
	{
		if (ch->abilities[TALENTS][TAL_ALERTNESS] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_ALERTNESS]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_ALERTNESS];
		pOutput = "alertness";
	}

	else if (!str_cmp (arg1, "athletics"))
	{
		if (ch->abilities[TALENTS][TAL_ATHLETICS] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_ATHLETICS]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_ATHLETICS];
		pOutput = "athletics";
	}

	else if (!str_cmp (arg1, "brawl"))
	{
		if (ch->abilities[TALENTS][TAL_BRAWL] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_BRAWL]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_BRAWL];
		pOutput = "brawl";
	}

	else if (!str_cmp (arg1, "dodge"))
	{
		if (ch->abilities[TALENTS][TAL_DODGE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_DODGE]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_DODGE];
		pOutput = "dodge";
	}

	else if (!str_cmp (arg1, "empathy"))
	{
		if (ch->abilities[TALENTS][TAL_EMPATHY] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_EMPATHY]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_EMPATHY];
		pOutput = "empathy";
	}

	else if (!str_cmp (arg1, "intimidation"))
	{
		if (ch->abilities[TALENTS][TAL_INTIMIDATION] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_INTIMIDATION]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_INTIMIDATION];
		pOutput = "intimidation";
	}

	else if (!str_cmp (arg1, "legerdemain"))
	{
		if (ch->abilities[TALENTS][TAL_LEGERDEMAIN] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_LEGERDEMAIN]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_LEGERDEMAIN];
		pOutput = "legerdemain";
	}

	else if (!str_cmp (arg1, "leadership"))
	{
		if (ch->abilities[TALENTS][TAL_LEADERSHIP] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_LEADERSHIP]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_LEADERSHIP];
		pOutput = "leadership";
	}

	else if (!str_cmp (arg1, "subterfuge"))
	{
		if (ch->abilities[TALENTS][TAL_SUBTERFUGE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[TALENTS][TAL_SUBTERFUGE]) * 2;
		pAbility = &ch->abilities[TALENTS][TAL_SUBTERFUGE];
		pOutput = "subterfuge";
	}

	else if (!str_cmp (arg1, "animalken"))
	{
		if (ch->abilities[SKILLS][SKI_ANIMALKEN] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_ANIMALKEN]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_ANIMALKEN];
		pOutput = "animalken";
	}

	else if (!str_cmp (arg1, "archery"))
	{
		if (ch->abilities[SKILLS][SKI_ARCHERY] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_ARCHERY]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_ARCHERY];
		pOutput = "archery";
	}

	else if (!str_cmp (arg1, "crafts"))
	{
		if (ch->abilities[SKILLS][SKI_CRAFTS] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_CRAFTS]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_CRAFTS];
		pOutput = "crafts";
	}

	else if (!str_cmp (arg1, "etiquette"))
	{
		if (ch->abilities[SKILLS][SKI_ETIQUETTE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_ETIQUETTE]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_ETIQUETTE];
		pOutput = "etiquette";
	}

	else if (!str_cmp (arg1, "commerce"))
	{
		if (ch->abilities[SKILLS][SKI_COMMERCE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_COMMERCE]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_COMMERCE];
		pOutput = "commerce";
	}

	else if (!str_cmp (arg1, "melee"))
	{
		if (ch->abilities[SKILLS][SKI_MELEE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_MELEE]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_MELEE];
		pOutput = "melee";
	}

	else if (!str_cmp (arg1, "performance"))
	{
		if (ch->abilities[SKILLS][SKI_PERFORMANCE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_PERFORMANCE]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_PERFORMANCE];
		pOutput = "performance";
	}
*/
/*else if (!str_cmp (arg1, "ride"))
	{
		if (ch->abilities[SKILLS][SKI_RIDE] == 0)
			cost = 90;
		else
			cost = (ch->abilities[SKILLS][SKI_RIDE]) * 200;
		pAbility = &ch->abilities[SKILLS][SKI_RIDE];
		pOutput = "ride";
	}
*/
/*
	else if (!str_cmp (arg1, "stealth"))
	{
		if (ch->abilities[SKILLS][SKI_STEALTH] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_STEALTH]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_STEALTH];
		pOutput = "stealth";
	}

	else if (!str_cmp (arg1, "survival") 
       || !str_cmp (arg1, "primal urge") || !str_cmp(arg1, "primal-urge"))
	{
		if (ch->abilities[SKILLS][SKI_SURVIVAL] == 0)
			cost = 3;
		else
			cost = (ch->abilities[SKILLS][SKI_SURVIVAL]) * 2;
		pAbility = &ch->abilities[SKILLS][SKI_SURVIVAL];
		if (IS_SHIFTER(ch))
		pOutput = "primal urge";
		else
		pOutput = "survival";
	}

	else if (!str_cmp (arg1, "academics"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_ACADEMICS] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_ACADEMICS]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_ACADEMICS];
		pOutput = "academics";
	}

	else if (!str_cmp (arg1, "hearthwisdom"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_HEARTHWISDOM] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_HEARTHWISDOM]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_HEARTHWISDOM];
		pOutput = "hearthwisdom";
	}

	else if (!str_cmp (arg1, "investigation"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_INVESTIGATION] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_INVESTIGATION]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_INVESTIGATION];
		pOutput = "investigation";
	}

	else if (!str_cmp (arg1, "law"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_LAW] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_LAW]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_LAW];
		pOutput = "law";
	}

	else if (!str_cmp (arg1, "linguistics"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_LINGUISTICS] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_LINGUISTICS]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_LINGUISTICS];
		pOutput = "linguistics";
	}

	else if (!str_cmp (arg1, "medicine"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_MEDICINE] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_MEDICINE]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_MEDICINE];
		pOutput = "medicine";
	}

	else if (!str_cmp (arg1, "occult") || !str_cmp(arg1, "rituals"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_OCCULT] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_OCCULT]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_OCCULT];
		if (IS_SHIFTER(ch))
		pOutput = "rituals";
		else
		pOutput = "occult";
	}

	else if (!str_cmp (arg1, "politics"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_POLITICS] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_POLITICS]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_POLITICS];
		pOutput = "politics";
	}

	else if (!str_cmp (arg1, "theology"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_THEOLOGY] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_THEOLOGY]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_THEOLOGY];
		pOutput = "theology";
	}

	else if (!str_cmp (arg1, "seneschal"))
	{
		if (ch->abilities[KNOWLEDGES][KNO_SENESCHAL] == 0)
			cost = 3;
		else
			cost = (ch->abilities[KNOWLEDGES][KNO_SENESCHAL]) * 2;
		pAbility = &ch->abilities[KNOWLEDGES][KNO_SENESCHAL];
		pOutput = "seneschal";
	}
*/
	else
	{
		sprintf (buf, "#cYou can train the following Attributes, Abilities, etc.#n\n\r");
		send_to_char (buf, ch);

		send_to_char ("#CAttributes#B:#n\n\r", ch);
		for (i = 0; i < MAX_ATTRIB; i++)
		{
			if (ch->attributes[i] < max_train_trait)
			{
				temp_num2 = i;
				while (temp_num2 >= 3)
				{
					temp_num++;
					temp_num2 = temp_num2 - 3;
				} 
				sprintf (buf, "#c%-21s# #C%2d#c ", attrib_names[temp_num][temp_num2][0], (ch->attributes[i]) * 4);
				send_to_char (buf, ch);
				if (++col % 3 == 0)
					send_to_char ("\n\r", ch);
				temp_num = temp_num2 = 0;
			}
		}
		while (col % 3 != 0)
		{
			send_to_char ("                          ", ch);
			col++;
			if (col % 3 == 0)
				send_to_char ("\n\r", ch);
		}

		col = 0;
		send_to_char ("#CAbilities#B:#n\n\r", ch);
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 10; j++)
			{
				if (ch->abilities[i][j] < max_train_ability) // && (ch->pcdata->abilities_points[i][j] >= ability_insight_levels[ch->abilities[i][j]]))
				{
					if (ch->abilities[i][j] == 0)
						sprintf (buf, "#c%-21s #C%2d#c ", ability_names[i][j][0], 3);
					else
						sprintf (buf, "#c%-21s#C %2d#c ", ability_names[i][j][0], (ch->abilities[i][j]) * 2);
					send_to_char (buf, ch);
					if (++col % 3 == 0)
						send_to_char ("\n\r", ch);
				}
			}
		}
		while (col % 3 != 0)
		{
			send_to_char ("                          ", ch);
			col++;
			if (col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
		col = 0;
		send_to_char ("#COther Traits#B:#n#n\n\r", ch);
		if (ch->pcdata->willpower[WILLPOWER_MAX] < 10)
		{
			sprintf (buf, "#cWillpower             #C%2d#c ", (ch->pcdata->willpower[WILLPOWER_MAX]) * 1);
			send_to_char (buf, ch);
		}
		else
			send_to_char ("                          ", ch);
		if (ch->class == CLASS_VAMPIRE || ch->class == CLASS_INQUISITOR) //new code
		{
			if (ch->pcdata->roadt < 10)
			{
				sprintf (buf, "#cRoad                  #C%2d#c ", (ch->pcdata->roadt) * 2);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("                          ", ch);
		}
		else if (ch->class != CLASS_WEREWOLF)
			send_to_char ("                          ", ch);
		if (IS_SHIFTER(ch))
		{
			if (ch->pcdata->rage[1] < 10)
			{
				sprintf (buf, "#cRage                  #C%2d#c ", ch->pcdata->rage[1]);
				send_to_char(buf,ch);
			}
			else
				send_to_char ("                          ", ch);

			if (ch->pcdata->gnosis[1] < 10)
			{
				sprintf (buf, "\n\r#cGnosis                #C%2d#c \n\r", (ch->pcdata->gnosis[1] * 2));
				send_to_char(buf,ch);
			}
			else
				send_to_char ("\n\r                          #n\n\r", ch);
		}
		else
			send_to_char ("\n\r                          #n\n\r", ch);
		if (ch->class == CLASS_VAMPIRE)
		{
			if (ch->pcdata->virtues[VIRTUE_CONSCIENCE] < 10)
			{
				sprintf (buf, "#cConscience            #C%2d#c ", (ch->pcdata->virtues[VIRTUE_CONSCIENCE] * 2));
				send_to_char(buf,ch);
			}
			else
				send_to_char("                          ", ch);
			if (ch->pcdata->virtues[VIRTUE_SELFCONTROL] < 10)
			{
				sprintf (buf, "#cSelf Control          #C%2d#c ", (ch->pcdata->virtues[VIRTUE_SELFCONTROL] * 2));
				send_to_char(buf,ch);
			}
			else
				send_to_char("                          ", ch);
			if (ch->pcdata->virtues[VIRTUE_COURAGE] < 10)
			{
				sprintf (buf, "#cCourage               #C%2d#c \n\r", (ch->pcdata->virtues[VIRTUE_COURAGE] * 2));
				send_to_char(buf,ch);
			}
			else			
				send_to_char("                          #n\n\r", ch);
		}
		
		if (ch->class == CLASS_INQUISITOR) //new code
		{
			if (ch->pcdata->virtues[VIRTUE_ZEAL] < 10)
			{
				sprintf (buf, "#cZeal                  #C%2d#c ", (ch->pcdata->virtues[VIRTUE_ZEAL] * 2));
				send_to_char(buf, ch);
			}
			else
				send_to_char("                          ", ch);

			if (ch->pcdata->virtues[VIRTUE_FAITH] < 10)
			{
				sprintf (buf, "#cFaith                 #C%2d#c ", (ch->pcdata->virtues[VIRTUE_FAITH] * 2));
				send_to_char(buf, ch);
			}
			else
				send_to_char("                          ", ch);
				
			if (ch->pcdata->virtues[VIRTUE_WISDOM] < 10)
			{
				sprintf (buf, "#cWisdom                #C%2d#c ", (ch->pcdata->virtues[VIRTUE_WISDOM] * 2));
				send_to_char(buf, ch);
			}
			else
				send_to_char("                          ", ch);
				
			if (ch->pcdata->virtues[VIRTUE_CONSCIENCE] < 10)
			{
				sprintf (buf, "#cConscience            #C%2d#c ", (ch->pcdata->virtues[VIRTUE_CONSCIENCE] * 2));
				send_to_char(buf,ch);
			}
			else
				send_to_char("                          ", ch);
				
			if (ch->pcdata->virtues[VIRTUE_SELFCONTROL] < 10)
			{
				sprintf (buf, "#cSelf Control          #C%2d#c ", (ch->pcdata->virtues[VIRTUE_SELFCONTROL] * 2));
				send_to_char(buf,ch);
			}
			else
				send_to_char("                          ", ch);
				
			if (ch->pcdata->virtues[VIRTUE_COURAGE] < 10)
			{
				sprintf (buf, "#cCourage               #C%2d#c \n\r", (ch->pcdata->virtues[VIRTUE_COURAGE] * 2));
				send_to_char(buf,ch);
			}
			else			
				send_to_char("                          #n\n\r", ch);
		}


		send_to_char ("#c................#n\n\r", ch);
		sprintf (buf, "#cYou have #C%2d#c xp available for training.\n\r", ch->exp);
		send_to_char (buf, ch);
		return;
	}


	if (ch->pcdata->willpower[WILLPOWER_MAX] >= 10 && !str_cmp (arg1, "willpower"))
	{
		if (last)
			act ("#cYour $T is already at maximum.", ch, NULL, pOutput, TO_CHAR);
		return;
	}

	if ((*pAbility >= 10) && (!str_cmp (arg1, "road")))
	{
		if (last)
			 act ("#cYour $T is already at maximum.",  ch, NULL, pOutput, TO_CHAR);
		return;
	}

	if ((*pAbility >= 5) && (!str_cmp (arg1, "courage") || !str_cmp (arg1, "selfcontrol") || !str_cmp (arg1, "conscience") || !str_cmp (arg1, "wisdom") || !str_cmp (arg1, "faith") || !str_cmp (arg1, "zeal")))
	{
		if (last)
			act ("#cYour $T is already at maximum.",  ch, NULL, pOutput, TO_CHAR);
		return;
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if ((!str_cmp (arg1, ability_names[i][j][0])) && (ch->abilities[i][j] >= max_train_ability))
			{
				if (last)
					send_to_char ("#cYou already know too much about the ability.\n\r", ch);
				return;
			}


			if ((!str_cmp (arg1, ability_names[i][j][0])) && (ch->pcdata->abilities_points[i][j] < ability_insight_levels[ch->abilities[i][j]]))
			{
				if (last)
					send_to_char ("#cYou don't have enough insight into that skill to raise it.n\r", ch);
				return;
			}

		}
	}

	if (cost > ch->exp || ch->exp < 1)
	{
		if (last)
			send_to_char ("#cYou don't have enough xp.\n\r", ch);
		return;
	}

	ch->exp -= (cost < 1 ? 1 : cost);
	*pAbility += 1;
	if (last)
		act ("#CYou train $T.#n", ch, NULL, pOutput, TO_CHAR);
	return;
}

