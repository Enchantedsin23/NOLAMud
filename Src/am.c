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
		send_to_char ("You can't go that way.\n\r", ch);
		return;
	}

	weather_move( ch, to_room, pexit );

    if ((to_room->sector_type != SECT_INSIDE ) && (to_room->room_flags != ROOM_INDOORS) && !IS_NPC( ch ) )
    weather_check( ch );

	if ((to_room->sector_type != SECT_SEWER ) && !IS_NPC( ch ) )
	{
		REMOVE_BIT (ch->affected_by, AFF_PERFUME);
		SET_BIT(ch->affected_by, AFF_DIRTY);
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
	
	if (IS_SET (pexit->exit_info, EX_CHANGELING) && !IS_SUPERNATURAL(ch)) // Changeling-only exits
	{
		send_to_char ("#cYou walk into a wall and find yourself momentarily confused.#n\n\r", ch);
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

	if (IS_MORE2 (ch, MORE2_MOON4))
	{
		send_to_char ("You look around confused..what were you doing again?\n\r", ch);
		return;
	}


	if (IS_SET (pexit->exit_info, EX_WARD_GHOULS) && IS_GHOUL (ch))
	{
		send_to_char ("Something prevents you from moving that way.\n\r", ch);
		return;
	}

	if (IS_SET (pexit->exit_info, EX_WARD_LUPINES) && IS_CHANGELING (ch))
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
		if (!IS_NPC (ch) && (IS_VAMPIRE (ch) || IS_GHOUL (ch)) && get_disc (ch, DISC_ARTIFICE) > 0)
			eff_str += (get_disc (ch, DISC_ARTIFICE) * 10);

		if (eff_str >= 100)
		{
			act ("You smash open the $d.", ch, NULL, pexit->keyword, TO_CHAR);
			act ("$n smashes open the $d.", ch, NULL, pexit->keyword, TO_ROOM);
			REMOVE_BIT (pexit->exit_info, EX_CLOSED);
		}
		else if (!IS_NPC (ch) && IS_CHANGELING (ch) && ch->pcdata->disc[TOTEM_BOAR] > 0)
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

/*	if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL && in_room == ch->master->in_room)
	{
		send_to_char ("What?  And leave your beloved master?\n\r", ch);
		return;
	}*/

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
	//	send_to_char ("Bug check -- doing cleared\r\n", ch);
                free_string (ch->pcdata->doing);
                ch->pcdata->doing = NULL;
	//	return;
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
				if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_STARGAZERS) > 0)
					found = TRUE;
				else if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_UKTENA) > 1)
					found = TRUE;
				else if (IS_VAMPAFF (ch, VAM_FLYING))
					found = TRUE;
				else if (IS_POLYAFF (ch, POLY_SERPENT))
					found = TRUE;
				 else if (!IS_NPC(ch) && (IS_VAMPIRE(ch) || IS_GHOUL(ch)) && get_disc(ch,DISC_ANIMALISM) > 8)
					 found = TRUE;
			}
			if (IS_AFFECTED (ch, AFF_FLYING))
				found = TRUE;
			else if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
				found = TRUE;
			else if (IS_POLYAFF (ch, POLY_FISH))
				found = TRUE;
			else if (ch->abilities[PHYSICAL][TAL_ATHLETICS] > 1)
				found = TRUE;
			else if ((mount = ch->mount) != NULL && IS_SET (ch->mounted, IS_RIDING) && IS_AFFECTED (mount, AFF_FLYING))
				found = TRUE;
			if (!found)
			{
				for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
				{
					if (obj->item_type == ITEM_CONDOM)
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
		strcpy (leave, "walks");
	else if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH) && ch->pcdata->obj_vnum == OBJ_VNUM_SPILT_BLOOD)
		strcpy (leave, "walks");
	else if (IS_EXTRA (ch, EXTRA_OSWITCH))
		strcpy (leave, "walks");
	else if (IS_AFFECTED (ch, AFF_ETHEREAL))
		strcpy (leave, "walks");
	else if (in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		strcpy (leave, "walks");
	else if (IS_SET (ch->polyaff, POLY_SERPENT))
		strcpy (leave, "walks");
	else if (IS_SET (ch->polyaff, POLY_WOLF))
		strcpy (leave, "walks");
	else if (IS_SET (ch->polyaff, POLY_FROG))
		strcpy (leave, "walks");
	else if (IS_SET (ch->polyaff, POLY_FISH))
		strcpy (leave, "walks");
	else if (in_room->sector_type == SECT_WATER_SWIM || to_room->sector_type == SECT_WATER_SWIM)
		strcpy (leave, "walks");
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

		if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_AFFECTED (ch, AFF_SNEAK) && !IS_MORE3(ch, MORE3_OBFUS3) && !IS_VAMPAFF (ch, VAM_EARTHMELDED) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)) && can_see (victim, ch))
		{
			if (((mount = ch->mount) != NULL && ch->mounted == IS_RIDING && IS_AFFECTED (mount, AFF_FLYING)) || IS_AFFECTED (ch, AFF_FLYING) || (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_FLYING)))
				sprintf (poly, "%s flies $T%s", ch->morph, mount2);
			else if ((mount = ch->mount) != NULL && ch->mounted == IS_RIDING)
				sprintf (poly, "%s rides $T%s", ch->morph, mount2);
			else
				sprintf (poly, "%s %s $T%s", ch->morph, leave, mount2);
			act (poly, victim, NULL, dir_name[door], TO_CHAR);
		}
		else if (!IS_VAMPAFF (ch, VAM_EARTHMELDED) && !IS_AFFECTED (ch, AFF_SNEAK) && !IS_MORE3(ch, MORE3_OBFUS3) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)) && can_see (victim, ch))
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
		else if (!IS_AFFECTED (ch, AFF_SNEAK) && can_see (victim, ch) && !IS_MORE3(ch, MORE3_OBFUS3) && !IS_VAMPAFF (ch, VAM_EARTHMELDED) && (IS_NPC (ch) || !IS_SET (ch->act, PLR_WIZINVIS)))
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

bool check_move (CHAR_DATA *ch) {
  
        if (IS_AFFECTED (ch, AFF_SHADOW))
        {
                send_to_char ("You are unable to move with these tendrils of darkness entrapping you.\n\r", ch);
                return FALSE;
        }       
        else if (IS_AFFECTED (ch, AFF_JAIL))
        {
                send_to_char ("You are unable to move with these bands of water entrapping you.\n\r", ch);
                return FALSE;
	}
        else if (ch->vehicle != NULL)
        {
                send_to_char ("#eWhile inside a vehicle, you must move using the 'drive' command.#n\n\r", ch);
                return FALSE;
        }   
	return TRUE;
}

void do_north (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (!check_move(ch))
		return;
	in_room = ch->in_room;
	move_char (ch, DIR_NORTH);

	return;
}

void do_east (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (!check_move(ch))
		return;
	in_room = ch->in_room;
	move_char (ch, DIR_EAST);

	return;
}

void do_south (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (!check_move(ch))
		return;
	in_room = ch->in_room;
	move_char (ch, DIR_SOUTH);

	return;
}

void do_west (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (!check_move(ch))
		return;
	in_room = ch->in_room;
	move_char (ch, DIR_WEST);

	return;
}

void do_up (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

  if (!check_move(ch))
	  return;
	in_room = ch->in_room;
	move_char (ch, DIR_UP);

	return;
}

void do_down (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if (!check_move(ch))
		return;
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
 
/* Vehicle code by Julius */
 
void do_enter (CHAR_DATA * ch, char *argument)
{
	ROOM_INDEX_DATA *pRoomIndex;
	ROOM_INDEX_DATA *location;
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	CHAR_DATA *mount;
	bool found;

	argument = one_argument (argument, arg);
	
	if ( ch->vehicle != NULL )
	{
	  send_to_char ("#gYou are already in a vehicle.#n\n\r", ch);
	  return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("#gEnter what?#n\n\r", ch);
		return;
	}

	obj = get_obj_list (ch, arg, ch->in_room->contents);
	if (obj == NULL)
	{
		act ("#gI see no $T here.#n", ch, NULL, arg, TO_CHAR);
		return;
	}
	if (obj->item_type != ITEM_VEHICLE)
	{
		act ("#gYou cannot enter that.#n", ch, NULL, arg, TO_CHAR);
		return;
	}

	if (obj->value[2] == 2 || obj->value[2] == 3)
	{
		act ("#gIt seems to be closed.#n", ch, NULL, arg, TO_CHAR);
		return;
	}

	pRoomIndex = get_room_index (obj->value[0]);
	location = ch->in_room;

	//This should be commented out since the pRoomIndex should be the same as the location
	/*
	if (pRoomIndex == NULL)
	{
		act ("You are unable to enter.", ch, NULL, arg, TO_CHAR);
		return;
	}
	*/

	//The following lines handle what people see when you enter/exit a vehicle
	act ("#gYou step into $p.#n", ch, obj, NULL, TO_CHAR);
	if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (poly, "#g%s steps into $p.#n", ch->morph);
	else
		sprintf (poly, "#g$n steps into $p.#n");
	act (poly, ch, obj, NULL, TO_ROOM);


        //The following two lines move a player from the originating room into the destination room.  This can be commented out as a vehicle's originating room and destination room are the same.
	//char_from_room (ch);
	//char_to_room (ch, pRoomIndex);
	if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (poly, "#g%s steps out of $p.#n", ch->morph);
	else
		sprintf (poly, "#g$n steps out of $p.#n");

        //The following two lines move a player from the originating room into the destination room.  This can be commented out as a vehicle's originating room and destination room are the same.
        /*
	act (poly, ch, obj, NULL, TO_ROOM);
	char_from_room (ch);
	char_to_room (ch, location);
	*/

        //The following block of code removes a portal after its uses are done.  Should be commented out
        /*
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
        */

        //The following two lines move a player from the originating room into the destination room.  This can be commented out as a vehicle's originating room and destination room are the same.
        /*
	char_from_room (ch);
	char_to_room (ch, pRoomIndex);
        */
	found = FALSE;


	//The Money bit: set the ch->vehicle flag
	die_follower( ch );
	ch->vehicle = obj;
	do_look (ch, "auto");
	if ((mount = ch->mount) == NULL)
		return;
	char_from_room (mount);
	char_to_room (mount, ch->in_room);
	return;
}


void do_leave (CHAR_DATA * ch, char *argument)
{
	char poly[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	bool found;

	if (ch->vehicle == NULL)
	{
		send_to_char ("#gYou aren't inside a vehicle.#n\n\r", ch);
		return;
	}

	obj = ch->vehicle;

	//The following lines handle what people see when you leave a vehicle
	act ("#gYou step out of $p.#n", ch, obj, NULL, TO_CHAR);
	if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		sprintf (poly, "#g%s steps out of $p.#n", ch->morph);
	else
		sprintf (poly, "#g$n steps out of $p.#n");
	act (poly, ch, obj, NULL, TO_ROOM);
	
	found = FALSE;


	//The Money bit: remove the ch->vehicle flag
	ch->vehicle = NULL;
	do_look (ch, "");
	return;
}

/* Vehicle code by Julius */

void do_drive (CHAR_DATA * ch, char *argument)
 {
      char arg[MAX_INPUT_LENGTH]; 
	char buf[MAX_STRING_LENGTH];
	char bufo[MAX_STRING_LENGTH];
	char bufi[MAX_STRING_LENGTH];
      OBJ_DATA *vehicle;
      int door = -1;
      EXIT_DATA *pexit;
      ROOM_INDEX_DATA *to_room;
      ROOM_INDEX_DATA *from_room;
      CHAR_DATA *fch, *fch_next;
 
      if ((vehicle = ch->vehicle) == NULL)
      {
           send_to_char ("#gYou can only drive while inside a vehicle.#n\n\r", ch);
           return;
      }
      one_argument (argument, arg); 
      if (arg[0] == '\0')
      {
           send_to_char ("#gSyntax#y:#G drive <direction>#g.#g Example#y:#G drive west#n\n\r", ch);
           return;
      }
      
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
 
      if ( door == -1 )
         {
                 send_to_char ("#gSyntax#y:#G drive <direction>#g. Example#y:#G drive west#n\n\r", ch);
                 return;
         }
         
 
      /* 'look direction' */
      if ((pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
         {                                                                                                                                                                                    
                 send_to_char ("#gYou can't go that way.#n\n\r", ch);                                                                                                                             
                 return;                                                                                                                                                                 
         }
         
       if ( to_room->sector_type != SECT_CITY ) {
         send_to_char ("#gYou can only drive in the city!#n\n\r", ch);
         return;
 
         }
  
	obj_from_room( vehicle );
	obj_to_room( vehicle, to_room );
 
        from_room = ch->in_room;
/*
        for (fch = ch->in_room->people; fch != NULL; fch = fch_next)        
	{
                fch_next = fch->next_in_room;
		if ( fch->vehicle != NULL && fch->vehicle == ch->vehicle ) {
			char_from_room( fch );
			char_to_room( fch, to_room );
		}
        }
*/
        if (door == 0)
        {
                sprintf (bufo, "#Gthe north.#n");
                sprintf (bufi, "#Gthe south.#n");
        }
        else if (door == 1)
        {
                sprintf (bufo, "#Gthe east.#n");
                sprintf (bufi, "#Gthe west.#n");
        }
        else if (door == 2)
        {
                sprintf (bufo, "#Gthe south.#n");
                sprintf (bufi, "#Gthe north.#n");
        }
        else if (door == 3)
        {
                sprintf (bufo, "#Gthe west.#n");
                sprintf (bufi, "#Gthe east.#n");
        }
        else if (door == 4)
        {
                sprintf (bufo, "#Gthe up exit.#n");
                sprintf (bufi, "#Gthe up exit.#n");
        }
        else if (door == 5)
        {
                sprintf (bufo, "#Gthe down exit.#n");
                sprintf (bufi, "#Gthe down exit.#n");
        }


	sprintf( buf, "#g%s drives in from %s#n", vehicle->short_descr, bufi );
        for (fch = to_room->people; fch != NULL; fch = fch->next_in_room)
          act (buf, fch, NULL, NULL, TO_CHAR);

	sprintf( buf, "#gYour vehicle drives towards %s#n\n", bufo );
        for (fch = from_room->people; fch != NULL; fch = fch_next)
        {
                fch_next = fch->next_in_room;
                if ( fch->vehicle != NULL && fch->vehicle == ch->vehicle ) {
                        char_from_room( fch );
                        char_to_room( fch, to_room );
			send_to_char( buf, fch );
		        //do_map (ch, "");
                }
        }


	sprintf( buf, "#g%s drives off to %s#n", vehicle->short_descr, bufo );
        for (fch = from_room->people; fch != NULL; fch = fch->next_in_room)
          act (buf, fch, NULL, NULL, TO_CHAR);

	for (fch = to_room->people; fch != NULL; fch = fch_next)
        {
                fch_next = fch->next_in_room;
		if ( fch->vehicle != NULL && fch->vehicle == ch->vehicle ) {
			do_map( fch, "" );
		}
        }

	return;
  }
  
/* End vehicle code */

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

	if (ch->vehicle != NULL)
	{
		send_to_char ("#gYou cannot open doors from inside a vehicle!#n\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);
		
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
			send_to_char ("The door is already open.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("The door is locked and cannot be opened.\n\r", ch);
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

	if (ch->vehicle != NULL)
	{
		send_to_char ("#gYou cannot close doors from inside a vehicle!#n\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);

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



void do_turn( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char( "Syntax: Turn <book> <forward/back>.\n\r", ch );
	return;
    }

    if (is_number(arg2)) value = atoi(arg2);

    if ( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
    {
	send_to_char("You don't have that book.\n\r",ch);
	return;
    }

    if ( obj->item_type != ITEM_BOOK)
    { send_to_char( "That's not a book.\n\r", ch ); return; }
    if ( IS_SET(obj->value[1], CONT_CLOSED) )
    { send_to_char( "First you should open it.\n\r",    ch ); return; }

    if (!str_cmp(arg2,"f") || !str_cmp(arg2,"forward"))
    {
	if (obj->value[2] >= obj->value[3])
	{
	    send_to_char("But you are already at the end of the book.\n\r",ch);
	    return;
	}
	obj->value[2] += 1;
	act( "You flip forward a page in $p.", ch, obj, NULL, TO_CHAR );
	act( "$n flips forward a page in $p.", ch, obj, NULL, TO_ROOM );
    }
    else if (!str_cmp(arg2,"b") || !str_cmp(arg2,"backward"))
    {
	if (obj->value[2] <= 0)
	{
	    send_to_char("But you are already at the beginning of the book.\n\r",ch);
	    return;
	}
	obj->value[2] -= 1;
	act( "You flip backward a page in $p.", ch, obj, NULL, TO_CHAR );
	act( "$n flips backward a page in $p.", ch, obj, NULL, TO_ROOM );
    }
    else if (is_number(arg2) && value >= 0 && value <= obj->value[3])
    {
	if (value == obj->value[2])
	{
	    act("$p is already open at that page.",ch,obj,NULL,TO_CHAR);
	    return;
	}
	else if (value < obj->value[2])
	{
	    act( "You flip backwards through $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n flips backwards through $p.", ch, obj, NULL, TO_ROOM );
	}
	else
	{
	    act( "You flip forwards through $p.", ch, obj, NULL, TO_CHAR );
	    act( "$n flips forwards through $p.", ch, obj, NULL, TO_ROOM );
	}
	obj->value[2] = value;
    }
    else
	send_to_char("Do you wish to turn forward or backward a page?\n\r",ch);
    return;
}

void do_read( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "What do you wish to read?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char("You don't have that book.\n\r",ch);
	return;
    }

    if ( obj->item_type == ITEM_PAGE)
    {
	if (!str_cmp(obj->victpoweruse,"(null)"))
	    sprintf(buf,"Untitled page.\n\r");
	else
	    sprintf(buf," #w%s#n.\n\r",obj->victpoweruse);
	buf[0] = UPPER(buf[0]);
	send_to_char(buf,ch);
	if (obj->chpoweruse == NULL || obj->chpoweruse == '\0' ||
	    !str_cmp(obj->chpoweruse,"(null)"))
	{
	    if (!are_runes(obj))
		send_to_char("This page is blank.\n\r",ch);
	    else
		send_to_char("This page is blank.\n\r",ch);
	    return;
	}
	send_to_char("#g--------------------------------------------------------------------------------#n\n\r",ch);
	send_to_char(obj->chpoweruse,ch);
	send_to_char("\n\r#g--------------------------------------------------------------------------------#n\n\r",ch);
	return;
    }

    if ( obj->item_type != ITEM_BOOK)
    { send_to_char( "That's not a book.\n\r", ch ); return; }
    if ( IS_SET(obj->value[1], CONT_CLOSED) )
    {
	if (!str_cmp(obj->victpoweruse,"(null)"))
	    sprintf(buf,"The book is untitled.\n\r");
	else
	    sprintf(buf,"The book is titled '%s'.\n\r",obj->victpoweruse);
	buf[0] = UPPER(buf[0]);
	send_to_char(buf,ch);
	return;
    }

    if (obj->value[2] == 0)
    {
	int page;

	sprintf(buf,"'#w%s#n' #g-#n Table of Contents\n\r#g--------------------------------------------------------------------------------#n\n\r", obj->victpoweruse);
		buf[0] = UPPER(buf[0]);
	send_to_char(buf,ch);
	if (obj->value[3] <= 0)
	{
	    send_to_char(" There are no pages in this book.\n\r",ch);
	    return;
	}
	for ( page = 1; page <= obj->value[3]; page ++ )
	{
	    sprintf(buf,"Page %d:",page);
	    send_to_char(buf,ch);
	    show_page(ch,obj,page,TRUE);
	}
    }
    else
    {
	sprintf(buf,"Page %d:",obj->value[2]);
	send_to_char(buf,ch);
	show_page(ch,obj,obj->value[2],FALSE);
    }
    return;
}

void do_write( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
	char buf[MAX_STRING_LENGTH];

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if (IS_NPC(ch)) return;
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	send_to_char("Syntax: Write <page> <title/line> <text>.\n\r",ch);
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1) ) == NULL )
    {
	send_to_char("You are not carrying that item.\n\r",ch);
	return;
    }
    if ( obj->item_type != ITEM_PAGE && obj->item_type != ITEM_BOOK )
    {
	send_to_char("You cannot write on that.\n\r",ch);
	return;
    }
	
	
	
    if (!str_cmp( arg2,"title"))
    {
	free_string( obj->victpoweruse );
	obj->victpoweruse = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	act("$n writes something on $p.",ch,obj,NULL,TO_ROOM);
	return;
    }
	
	
	
else if (str_cmp( arg2,"line"))
    {
	send_to_char("You can write a TITLE or a LINE.\n\r",ch);
	return;
    }
    else if ( obj->item_type == ITEM_BOOK )
    {
	send_to_char("You can only write a title on the book.\n\r",ch);
	return;
    }
    if (obj->chpoweruse != NULL) strcpy(buf,obj->chpoweruse);
	else return;
    if (!str_cmp(buf,"(null)"))
    {
	arg3[0] = UPPER(arg3[0]);
	free_string( obj->chpoweruse );
	obj->chpoweruse = str_dup( arg3 );
	send_to_char("Ok.\n\r",ch);
	act("$n writes something on $p.",ch,obj,NULL,TO_ROOM);
	if (!str_cmp(arg3,"start.damage.spell") && obj->spectype == 0)
	    {SET_BIT(obj->spectype, ADV_STARTED);
	    SET_BIT(obj->spectype, ADV_DAMAGE);}
	else if (!str_cmp(arg3,"start.affect.spell") && obj->spectype == 0)
	    {SET_BIT(obj->spectype, ADV_STARTED);
	    SET_BIT(obj->spectype, ADV_AFFECT);}
	else if (!str_cmp(arg3,"start.action.spell") && obj->spectype == 0)
	    {SET_BIT(obj->spectype, ADV_STARTED);
	    SET_BIT(obj->spectype, ADV_ACTION);}
	else if (!str_cmp(arg3,"start.spell") && obj->spectype == 0)
	    SET_BIT(obj->spectype, ADV_STARTED);
	else if (!IS_SET(obj->spectype, ADV_FINISHED))
	    SET_BIT(obj->spectype, ADV_FAILED);
	return;
    }
    if (obj->chpoweruse != NULL && buf[0] != '\0')
    {
	if (strlen(buf)+strlen(arg3) >= MAX_STRING_LENGTH-4)
	{
	    send_to_char("Line too long.\n\r",ch);
	    return;
	}
	else
	{
	    free_string( obj->chpoweruse );
	    strcat( buf, "\n\r" );
	    strcat( buf, arg3 );
	    obj->chpoweruse = str_dup( buf );

	    argument = one_argument( argument, arg1 );
	    strcpy( arg2, argument );

	    if (!str_cmp(arg1,"start.damage.spell") && obj->spectype == 0)
		{SET_BIT(obj->spectype, ADV_STARTED);
		SET_BIT(obj->spectype, ADV_DAMAGE);}
	    else if (!str_cmp(arg1,"start.affect.spell") && obj->spectype == 0)
		{SET_BIT(obj->spectype, ADV_STARTED);
		SET_BIT(obj->spectype, ADV_AFFECT);}
	    else if (!str_cmp(arg1,"start.action.spell") && obj->spectype == 0)
		{SET_BIT(obj->spectype, ADV_STARTED);
		SET_BIT(obj->spectype, ADV_ACTION);}
	    else if (!str_cmp(arg1,"start.spell") && obj->spectype == 0)
		SET_BIT(obj->spectype, ADV_STARTED);
	    else if (!str_cmp(arg1,"end.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		SET_BIT(obj->spectype, ADV_FINISHED);
		obj->toughness = ch->pcdata->runes[0];
		obj->points += 1;
	    }
	    else if (!str_cmp(arg1,"damage.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_ACTION)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_DAMAGE);
	    else if (!str_cmp(arg1,"affect.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_ACTION)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_AFFECT);
	    else if (!str_cmp(arg1,"action.spell") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_ACTION)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_AFFECT);
	    else if (!str_cmp(arg1,"area.affect") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_AREA_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_AREA_AFFECT);obj->points += 100;}
	    else if (!str_cmp(arg1,"victim.target") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_VICTIM_TARGET)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_VICTIM_TARGET);obj->points += 5;}
	    else if (!str_cmp(arg1,"object.target") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_OBJECT_TARGET)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_OBJECT_TARGET);obj->points += 5;}
	    else if (!str_cmp(arg1,"global.target") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_GLOBAL_TARGET)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_GLOBAL_TARGET);obj->points += 50;}
	    else if (!str_cmp(arg1,"next.page") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_NEXT_PAGE)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_NEXT_PAGE);obj->points += 5;}
	    else if (!str_cmp(arg1,"parameter:") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_PARAMETER)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_PARAMETER);
		}
	    }
	    else if (!str_cmp(arg1,"spell.first") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_SPELL_FIRST)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_SPELL_FIRST);
	    else if (!str_cmp(arg1,"not.caster") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_NOT_CASTER)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_NOT_CASTER);
	    else if (!str_cmp(arg1,"no.players") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_NO_PLAYERS)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_NO_PLAYERS);
	    else if (!str_cmp(arg1,"second.victim") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_SECOND_VICTIM)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_SECOND_VICTIM);obj->points += 5;}
	    else if (!str_cmp(arg1,"second.object") 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_SECOND_OBJECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		{SET_BIT(obj->spectype, ADV_SECOND_OBJECT);obj->points += 5;}
	    else if (!str_cmp(arg1,"reversed")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_REVERSED)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_REVERSED);
	    else if (!str_cmp(arg1,"min.damage:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 0 || atoi(arg2) > 500)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[1] = atoi(arg2);obj->points += (atoi(arg2)*0.5);}
	    }
	    else if (!str_cmp(arg1,"max.damage:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_DAMAGE)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 0 || atoi(arg2) > 1000)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[2] = atoi(arg2);obj->points += (atoi(arg2)*0.5);}
	    }
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {SET_BIT(obj->spectype, ADV_FAILED);return;}
		obj->points += 25;
	    }
	    if (!str_cmp(arg1,"affect:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {SET_BIT(obj->spectype, ADV_FAILED);return;}
		obj->points += 25;
	    }
	    else if (!str_cmp(arg1,"bonus:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 0 || atoi(arg2) > 100)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->value[2] = atoi(arg2);obj->points += (atoi(arg2)*15);}
	    }
	    else if (!str_cmp(arg1,"duration:")
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& IS_SET(obj->spectype, ADV_AFFECT)
		&& obj->level == 0
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' || !is_number(arg2) || 
		    atoi(arg2) < 1 || atoi(arg2) > 60)
		    SET_BIT(obj->spectype, ADV_FAILED);
		else {obj->level = atoi(arg2);obj->points += (atoi(arg2)*10);}
	    }
	    else if ((!str_cmp(arg1,"message.one:") || !str_cmp(arg1,"message.1:")) 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_MESSAGE_1)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_MESSAGE_1);
		}
	    }
	    else if ((!str_cmp(arg1,"message.two:") || !str_cmp(arg1,"message.2:")) 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_MESSAGE_2)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_MESSAGE_2);
		    free_string(obj->victpoweron);
		    obj->victpoweron = str_dup(arg2);
		}
	    }
	    else if ((!str_cmp(arg1,"message.three:") || !str_cmp(arg1,"message.3:")) 
		&& IS_SET(obj->spectype, ADV_STARTED) 
		&& !IS_SET(obj->spectype, ADV_MESSAGE_3)
		&& !IS_SET(obj->spectype, ADV_FINISHED))
	    {
		if ( arg2[0] == '\0' )
		    SET_BIT(obj->spectype, ADV_FAILED);
		else
		{
		    SET_BIT(obj->spectype, ADV_MESSAGE_3);
		    free_string(obj->victpoweroff);
		    obj->victpoweroff = str_dup(arg2);
		}
	    }
	    else if (!IS_SET(obj->spectype, ADV_FINISHED))
		SET_BIT(obj->spectype, ADV_FAILED);
	}
    }
    else return;
    act("$n writes something on $p.",ch,obj,NULL,TO_ROOM);
    send_to_char("Ok.\n\r",ch);
    return;
}

void show_page( CHAR_DATA *ch, OBJ_DATA *book, int pnum, bool pagefalse )
{
    OBJ_DATA *page;
    OBJ_DATA *page_next;
    char buf[MAX_STRING_LENGTH];
    bool found = FALSE;

    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	if (page->value[0] == pnum)
	{
	    found = TRUE;
	    if (!str_cmp(page->victpoweruse,"(null)"))
		sprintf(buf,"#wUntitled page.#n\n\r");
	    else
		sprintf(buf," #w%s#n.\n\r",page->victpoweruse);
	    buf[0] = UPPER(buf[0]);
	    send_to_char(buf,ch);
	    if (!pagefalse)
	    {
		if (page->chpoweruse == NULL || page->chpoweruse == '\0' ||
			!str_cmp(page->chpoweruse,"(null)"))
		{
		    if (!are_runes(page))
			send_to_char("There is nothing written on this page.\n\r",ch);
		    else
			send_to_char("There is nothing written on this page.\n\r",ch);
		    return;
		}
		send_to_char("#g--------------------------------------------------------------------------------#n\n\r",ch);
		send_to_char(page->chpoweruse,ch);
		send_to_char("\n\r#g--------------------------------------------------------------------------------#n\n\r",ch);
	    }
	}
    }
    if (!found)
	send_to_char("This page has been torn out.\n\r",ch);
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
			send_to_char ("You don't have the key to the lock.\n\r", ch);
			return;
		}
		if (IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("It's already locked.\n\r", ch);
			return;
		}

		SET_BIT (obj->value[1], CONT_LOCKED);
		send_to_char ("Inserting the key and turning it, the lock mechanism releases.\n\r", ch);
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
			send_to_char ("The door isn't closed.\n\r", ch);
			act ("$n swings a key around an open $d.", ch, NULL, pexit->keyword, TO_ROOM);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char ("There is no lock on the door.\n\r", ch);
			return;
		}
		if (!has_key (ch, pexit->key))
		{
			send_to_char ("You don't have the key to the lock.\n\r", ch);
			return;
		}
		if (IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("The door is already locked.\n\r", ch);
			return;
		}

		SET_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("The door locks with a satisfying #w*#nclick#w*#n.\n\r", ch);
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
			send_to_char ("The container is not closed.\n\r", ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char ("The container doesn't have a lock on it.\n\r", ch);
			return;
		}
		if (!has_key (ch, obj->value[2]))
		{
			send_to_char ("You don't have the key to the lock. #wRequest#n if you are taking measures to pick the lock or otherwise get through.\n\r", ch);
			return;
		}
		if (!IS_SET (obj->value[1], CONT_LOCKED))
		{
			send_to_char ("The container is already unlocked.\n\r", ch);
			return;
		}

		REMOVE_BIT (obj->value[1], CONT_LOCKED);
		send_to_char ("Inserting the key and turning it, the lock mechanism releases.\n\r", ch);
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
			send_to_char ("The door isn't closed.\n\r", ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char ("The door doesn't have a lock on it.\n\r", ch);
			return;
		}
		if (!has_key (ch, pexit->key))
		{
			send_to_char ("You don't have the key to the lock.\n\r", ch);
			return;
		}
		if (!IS_SET (pexit->exit_info, EX_LOCKED))
		{
			send_to_char ("The door is already unlocked.\n\r", ch);
			return;
		}

		REMOVE_BIT (pexit->exit_info, EX_LOCKED);
		send_to_char ("Inserting the key and turning it, the lock mechanism releases.\n\r", ch);
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
    send_to_char("You rap your knuckles on the door.\n\r",ch);

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
			send_to_char ("You can't wake up...you're in a coma!\n\r", ch);
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

/**** HOME TESTING ****/

/*void do_gohome(CHAR_DATA *ch, char *argument)
{
	act("#R$n leaves for their Home Room (OOC)#n.", ch, NULL, NULL, TO_ROOM);
  transport_char(ch, ch->home);
	act("#R$n arrives in their Home Room (OOC)#n", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "");
	return;
}*/

/**** MORE HOME TESTING BELOW *****/

/*void do_home (CHAR_DATA * ch, char *argument)

{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (arg[0] == '\0' || str_cmp (arg, "here"))
	{
		send_to_char ("#r[#RPlayerHomes#r] :#R If you want this room to be your home, you must type #r'#Rhome here#r.'#n\n\r", ch);
		return;
	}
	if (ch->in_room == NULL)
		return;

	if (ch->in_room->vnum == ch->home)
	{
		send_to_char ("#r[#RPlayerHomes#r] :#R This room is already your home#r.#n\n\r", ch);
		return;
	}

	if (IS_SET (ch->in_room->room_flags, ROOM_NO_RECALL) || IS_SET (ch->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char ("#r[#RPlayerHomes#r] :#R You are unable to set this room as your home#r.#n\n\r", ch);
		return;
	}
	ch->home = ch->in_room->vnum;
	send_to_char ("#r[#RPlayerHomes#r] :#R This room is now your home#r.#n\n\r", ch);
	return;
}*/

/**** END HOME TESTING ****/


void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
    int door;
	int picksucc = 0;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Pick what?\n\r", ch );
	return;
    }
	
	if (ch->pcdata->evileye > 0)
	{
		if (ch->pcdata->evileye > 1)
			sprintf (buf, "You cannot pick another lock for#y %d #nseconds.#n\n\r", ch->pcdata->evileye);
		else
			sprintf (buf, "One more second!\n\r");
		send_to_char (buf, ch);
		return;
	}

    if ( IS_NPC(ch) || (ch->abilities[PHYSICAL][TAL_LARCENY] < 1))
    {
	send_to_char( "You cannot pick locks without Larceny.\n\r", ch);
	return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
	/* 'pick door' */
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;

	pexit = ch->in_room->exit[door];
	if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
	    { send_to_char( "That door isn't closed, let alone locked.\n\r",        ch ); return; }
	if ( pexit->key < 0 )
	    { send_to_char( "That lock cannot be picked.\n\r",     ch ); return; }
	if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
	    { send_to_char( "That door isn't locked.\n\r",  ch ); return; }
	if ( IS_SET(pexit->exit_info, EX_CHANGELING) )
	    { send_to_char( "You failed to pick the lock.\n\r",             ch ); return; }
	
	if(ch->abilities[PHYSICAL][TAL_LARCENY] > 0){
	picksucc = diceroll(ch, ATTRIB_DEX, PHYSICAL, TAL_LARCENY, 8);
	if (picksucc > 3){

	REMOVE_BIT(pexit->exit_info, EX_LOCKED);
	send_to_char( "You fiddle and pick at the lock until it pops open. #G(success)#n\n\r", ch );
	act( "$n picks open the lock on the $d. #G(success)#n", ch, NULL, pexit->keyword, TO_ROOM );

	/* pick the other side */
	if ( ( to_room   = pexit->to_room               ) != NULL
	&&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
	&&   pexit_rev->to_room == ch->in_room )
	{
	    REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
		ch->pcdata->evileye = 10;
	}
    }
    else
	send_to_char( "You fiddle and pick at the lock, but it won't open. #R(failure)#n\n\r", ch );
	act( "$n picks at the lock on the $d. #R(failure)#n", ch, NULL, pexit->keyword, TO_ROOM );
	ch->pcdata->evileye = 10;
	}
	}

    return;
}

void do_sneak( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
    int sneaksucc = 0;
	
    if ( IS_NPC(ch) || (ch->abilities[PHYSICAL][TAL_STEALTH] < 1))
    {
	send_to_char( "You cannot sneak without Stealth.\n\r", ch);
	return;
    }
	
	if (ch->pcdata->evileye > 0)
	{
	if (ch->pcdata->evileye > 1)
	sprintf (buf, "You cannot attempt to sneak again for #y%d #nseconds.#n\n\r", ch->pcdata->evileye);
	else
	sprintf (buf, "One more second!\n\r");
	send_to_char (buf, ch);
	return;
	}
	
	if(ch->abilities[PHYSICAL][TAL_STEALTH] > 0){
	sneaksucc = diceroll(ch, ATTRIB_DEX, PHYSICAL, TAL_STEALTH, 8);
	if (sneaksucc > 3){

	SET_BIT (ch->more3, MORE3_OBFUS3);
	send_to_char( "You attempt to move silently. #G(success)#n\n\r", ch );
	act( "$n steps out of sight, hiding $mself in the shadows.#n", ch, NULL, ch, TO_ROOM );
    ch->pcdata->evileye = 10;
	}
    else
	send_to_char( "You attempt to move silently. #R(failure)#n\n\r", ch );
	ch->pcdata->evileye = 10;
	}

    return;
}

void do_hide( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	int hidesucc = 0;
	
    if ( IS_NPC(ch) || (ch->abilities[PHYSICAL][TAL_STEALTH] < 1))
    {
	send_to_char( "You cannot hide without Stealth.\n\r", ch);
	return;
    }
	
	if (ch->pcdata->evileye > 0)
	{
	if (ch->pcdata->evileye > 1)
	sprintf (buf, "You cannot attempt to hide again for #y%d #nseconds.#n\n\r", ch->pcdata->evileye);
	else
	sprintf (buf, "One more second!\n\r");
	send_to_char (buf, ch);
	return;
	}
	
	if(ch->abilities[PHYSICAL][TAL_STEALTH] > 0){
	hidesucc = diceroll(ch, ATTRIB_DEX, PHYSICAL, TAL_STEALTH, 8);
	if (hidesucc > 2){

	SET_BIT (ch->more3, MORE3_OBFUS1);
	send_to_char( "You attempt to hide. #G(success)#n\n\r", ch );
	act( "$n steps out of sight, hiding $mself in the shadows.#n", ch, NULL, ch, TO_ROOM );
    ch->pcdata->evileye = 10;
	}
    else
	send_to_char( "You attempt to hide. #R(failure)#n\n\r", ch );
	ch->pcdata->evileye = 10;
	}

    return;
}

void do_visible( CHAR_DATA *ch, char *argument )
{
	REMOVE_BIT (ch->more3, MORE3_OBFUS3);
    REMOVE_BIT (ch->more3, MORE3_OBFUS1);
    send_to_char( "You make yourself visible.\n\r", ch );
	act( "$n steps out into the open and reveals $mself.#n", ch, NULL, ch, TO_ROOM );
    return;
}

