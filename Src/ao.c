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
#include "unistd.h"



/*
 * Local functions.
 */
#define CD CHAR_DATA
void get_obj args ((CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container));
void sheath args ((CHAR_DATA * ch, bool right));
void draw args ((CHAR_DATA * ch, bool right));
char *special_item_name args ((OBJ_DATA * obj));
#undef	CD




void get_obj (CHAR_DATA * ch, OBJ_DATA * obj, OBJ_DATA * container)
{
	OBJ_DATA *obj2;
	OBJ_DATA *obj_next;
	ROOM_INDEX_DATA *objroom;
	bool move_ch = FALSE;

	/* Objects should only have a shadowplane flag when on the floor */
	if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && obj->in_room != NULL && (!IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
	{
		send_to_char ("Your hand passes right through it!\n\r", ch);
		return;
	}
	if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && obj->in_room != NULL && (IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
	{
		send_to_char ("Your hand passes right through it!\n\r", ch);
		return;
	}
	if (obj->item_type == ITEM_VEHICLE && !IS_STORYTELLER(ch))
	{
		send_to_char ("You cannot pick up vehicles!\n\r", ch);
		return;
	}
	if (!CAN_WEAR (obj, ITEM_TAKE))
	{
		send_to_char ("You can't take that.\n\r", ch);
		return;
	}

	/*if (ch->carry_number + 1 > can_carry_n (ch))
	{
		act ("$d: you can't carry that many items.", ch, NULL, obj->name, TO_CHAR);
		return;
	}

	if (ch->carry_weight + get_obj_weight (obj) > can_carry_w (ch))
	{
		act ("$d: you can't carry that much weight.", ch, NULL, obj->name, TO_CHAR);
		return;
	}*/

	if (container != NULL)
	{

		if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_SET (container->extra_flags, ITEM_SHADOWPLANE) && (container->carried_by == NULL || container->carried_by != ch))
		{
			send_to_char ("Your hand passes right through it!\n\r", ch);
			return;
		}
		if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_SET (container->extra_flags, ITEM_SHADOWPLANE) && (container->carried_by == NULL || container->carried_by != ch))
		{
			send_to_char ("Your hand passes right through it!\n\r", ch);
			return;
		}
		act ("You get $p from $P.", ch, obj, container, TO_CHAR);
		act ("$n gets $p from $P.", ch, obj, container, TO_ROOM);
		for (obj2 = container->contains; obj2 != NULL; obj2 = obj_next)
		{
			obj_next = obj2->next_content;
			if (obj2->chobj != NULL)
			{
				act ("A hand reaches inside $P and takes $p out.", obj2->chobj, obj, container, TO_CHAR);
				move_ch = TRUE;
			}
		}
		obj_from_obj (obj);
	}
	else
	{
		act ("You pick up $p.", ch, obj, container, TO_CHAR);
		act ("$n picks $p up.", ch, obj, container, TO_ROOM);
		if (obj != NULL)
			obj_from_room (obj);
	}

	if (obj->item_type == ITEM_MONEY)
	{
		ch->pounds += obj->value[0];
		extract_obj (obj);
	}
	else
	{
		obj_to_char (obj, ch);
		if (move_ch && obj->chobj != NULL)
		{
			if (obj->carried_by != NULL && obj->carried_by != obj->chobj)
				objroom = get_room_index (obj->carried_by->in_room->vnum);
			else
				objroom = NULL;
			if (objroom != NULL && get_room_index (obj->chobj->in_room->vnum) != objroom)
			{
				char_from_room (obj->chobj);
				char_to_room (obj->chobj, objroom);
				do_look (obj->chobj, "auto");
			}
		}
		if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && (IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
			REMOVE_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
	}

	return;
}



void do_get (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	OBJ_DATA *container;
	bool found;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_AFFECTED (ch, AFF_ETHEREAL) || IS_EXTRA (ch, EXTRA_ILLUSION))
	{
		send_to_char ("You cannot pick things up while ethereal.\n\r", ch);
		return;
	}

	/* Get type. */
	if (arg1[0] == '\0')
	{
		send_to_char ("Get what?\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);
	if (arg2[0] == '\0')
	{
		if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
		{
			/* 'get obj' */
			obj = get_obj_list (ch, arg1, ch->in_room->contents);

			if (obj == NULL)
			{
				OBJ_INDEX_DATA *obj_index;
				CHAR_DATA *mob;
				int mob_vnum;
				if ((mob = get_char_room (ch, arg1)) != NULL)
				{
					if (IS_NPC (mob) && IS_SET (mob->act, ACT_TINY))
					{
						mob_vnum = mob->pIndexData->vnum;
						if (get_obj_index (mob_vnum) != NULL && (obj_index = get_obj_index (mob_vnum)) != NULL && (obj = create_object (obj_index, 0)) != NULL)
						{
							obj->item_type = ITEM_TRASH;
							obj_to_room (obj, mob->in_room);
							obj->item_type = ITEM_CREATURE;
							extract_char (mob, TRUE);
						}
					}
				}

				if (obj == NULL)
				{
					act ("I see no $T here.", ch, NULL, arg1, TO_CHAR);
					return;
				}
			}

			get_obj (ch, obj, NULL);
		}
		else
		{
			/* 'get all' or 'get all.obj' */
			found = FALSE;
			for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
			{
				obj_next = obj->next_content;
				if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name)) && can_see_obj (ch, obj) && obj->item_type != ITEM_VEHICLE)
				{
					found = TRUE;
					get_obj (ch, obj, NULL);
				}
			}

			if (!found)
			{
				if (arg1[3] == '\0')
					send_to_char ("I see nothing here.\n\r", ch);
				else
					act ("I see no $T here.", ch, NULL, &arg1[4], TO_CHAR);
			}
		}
	}
	else
	{
		/* 'get ... container' */
		if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
		{
			send_to_char ("You can't do that.\n\r", ch);
			return;
		}

		if ((container = get_obj_here (ch, arg2)) == NULL)
		{
			act ("I see no $T here.", ch, NULL, arg2, TO_CHAR);
			return;
		}

		switch (container->item_type)
		{
		default:
			send_to_char ("That's not a container.\n\r", ch);
			return;

		case ITEM_CONTAINER:
		case ITEM_CORPSE_NPC:
			break;

		case ITEM_CORPSE_PC:
			{
				if (IS_NPC (ch))
				{
					send_to_char ("You can't do that.\n\r", ch);
					return;
				}
				if (IS_NEWBIE (ch) && strlen (container->questowner) > 1)
				{
					if (str_cmp (container->questowner, ch->name))
					{
						send_to_char ("You cannot loot the corpses of other players.\n\r", ch);
						return;
					}
				}
			}
		}

		if (IS_SET (container->value[1], CONT_CLOSED))
		{
			act ("The $d is closed.", ch, NULL, container->name, TO_CHAR);
			return;
		}

		if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
		{
			/* 'get obj container' */
			obj = get_obj_list (ch, arg1, container->contains);
			if (obj == NULL)
			{
				act ("I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR);
				return;
			}
			get_obj (ch, obj, container);
		}
		else
		{
			/* 'get all container' or 'get all.obj container' */
			found = FALSE;
			for (obj = container->contains; obj != NULL; obj = obj_next)
			{
				obj_next = obj->next_content;
				if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name)) && can_see_obj (ch, obj))
				{
					found = TRUE;
					get_obj (ch, obj, container);
				}
			}

			if (!found)
			{
				if (arg1[3] == '\0')
					act ("I see nothing in the $T.", ch, NULL, arg2, TO_CHAR);
				else
					act ("I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR);
			}
		}
	}
	do_autosave (ch, "");
	return;
}



void do_put (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *container;
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	OBJ_DATA *obj_next;
	OBJ_DATA *obj_next2;
	ROOM_INDEX_DATA *objroom = get_room_index (ROOM_VNUM_IN_OBJECT);

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Put what in what?\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "all") || !str_prefix ("all.", arg2))
	{
		send_to_char ("You can't do that.\n\r", ch);
		return;
	}

	if ((container = get_obj_here (ch, arg2)) == NULL)
	{
		act ("I see no $T here.", ch, NULL, arg2, TO_CHAR);
		return;
	}

	if (container->item_type != ITEM_CONTAINER)
	{
		send_to_char ("That's not a container.\n\r", ch);
		return;
	}

	if (IS_SET (container->value[1], CONT_CLOSED))
	{
		act ("The $d is closed.", ch, NULL, container->name, TO_CHAR);
		return;
	}

	if (str_cmp (arg1, "all") && str_prefix ("all.", arg1))
	{
		/* 'put obj container' */
		if ((obj = get_obj_carry (ch, arg1)) == NULL)
		{
			send_to_char ("You do not have that item.\n\r", ch);
			return;
		}

		if (obj == container)
		{
			send_to_char ("You can't fold it into itself.\n\r", ch);
			return;
		}

		if (!can_drop_obj (ch, obj))
		{
			send_to_char ("You can't let go of it.\n\r", ch);
			return;
		}

		if (get_obj_weight (obj) + get_obj_weight (container) > container->value[0])
		{
			send_to_char ("It won't fit.\n\r", ch);
			return;
		}

		if (hundred_objects (ch, container))
			return;

		for (obj2 = container->contains; obj2 != NULL; obj2 = obj_next2)
		{
			obj_next2 = obj2->next_content;
			if (obj2->chobj != NULL && obj != obj2)
				act ("A hand reaches inside $P and drops $p.", obj2->chobj, obj, container, TO_CHAR);
		}
		obj_from_char (obj);
		obj_to_obj (obj, container);
		act ("$n puts $p in $P.", ch, obj, container, TO_ROOM);
		act ("You put $p in $P.", ch, obj, container, TO_CHAR);
	}
	else
	{
		/* 'put all container' or 'put all.obj container' */
		if (hundred_objects (ch, container))
			return;
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;

			if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name)) && can_see_obj (ch, obj) && obj->wear_loc == WEAR_NONE && obj != container && can_drop_obj (ch, obj) && get_obj_weight (obj) + get_obj_weight (container) <= container->value[0])
			{
				for (obj2 = container->contains; obj2 != NULL; obj2 = obj_next2)
				{
					obj_next2 = obj2->next_content;
					if (obj2->chobj != NULL && obj2->chobj->in_room != NULL)
					{
						if (objroom != get_room_index (obj2->chobj->in_room->vnum))
						{
							char_from_room (obj2->chobj);
							char_to_room (obj2->chobj, objroom);
							do_look (obj2->chobj, "auto");
						}
						if (obj != obj2)
							act ("A hand reaches inside $P and drops $p.", obj2->chobj, obj, container, TO_CHAR);
					}
				}
				obj_from_char (obj);
				obj_to_obj (obj, container);
				act ("$n puts $p in $P.", ch, obj, container, TO_ROOM);
				act ("You put $p in $P.", ch, obj, container, TO_CHAR);
			}
		}
	}
	do_autosave (ch, "");
	return;
}



void do_drop (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	bool found;

	argument = one_argument (argument, arg);

	if (IS_AFFECTED (ch, AFF_ETHEREAL) || IS_EXTRA (ch, EXTRA_ILLUSION))
	{
		send_to_char ("You cannot pick things up while ethereal.\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Drop what?\n\r", ch);
		return;
	}

	if (is_number (arg))
	{
		/* 'drop NNNN coins' */
		int amount;

		amount = atoi (arg);
		argument = one_argument (argument, arg);
		if (amount <= 0 || (str_cmp (arg, "coins") && str_cmp (arg, "coin")))
		{
			send_to_char ("Sorry, you can't do that.\n\r", ch);
			return;
		}

		/* Otherwise causes complications if there's a pile on each plane */
		if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
		{
			send_to_char ("You cannot drop coins in the Umbra.\n\r", ch);
			return;
		}

		if (ch->pounds < amount)
		{
			send_to_char ("You haven't got that many pounds.\n\r", ch);
			return;
		}

		ch->pounds -= amount;

		for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;

			switch (obj->pIndexData->vnum)
			{
			case OBJ_VNUM_MONEY_ONE:
				amount += 1;
				extract_obj (obj);
				break;

			case OBJ_VNUM_MONEY_SOME:
				amount += obj->value[0];
				extract_obj (obj);
				break;
			}
		}
		obj_to_room (create_money (amount), ch->in_room);
		act ("$n drops some pound coins.", ch, NULL, NULL, TO_ROOM);
		send_to_char ("OK.\n\r", ch);
		do_autosave (ch, "");
		return;
	}

	if (str_cmp (arg, "all") && str_prefix ("all.", arg))
	{
		/* 'drop obj' */
		if ((obj = get_obj_carry (ch, arg)) == NULL)
		{
			send_to_char ("You do not have that item.\n\r", ch);
			return;
		}

		if (!can_drop_obj (ch, obj))
		{
			send_to_char ("You can't let go of it.\n\r", ch);
			return;
		}

		if (fifty_objects (ch))
			return;

		if (IS_MORE3(ch, MORE3_OBFUS1))
			do_smoke1(ch,argument);
		if (IS_MORE3(ch, MORE3_OBFUS3))
			do_smoke3(ch,argument);
		obj_from_char (obj);
		obj_to_room (obj, ch->in_room);
		/* Objects should only have a shadowplane flag when on the floor */
		if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && (!IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
			SET_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
		act ("$n drops $p.", ch, obj, NULL, TO_ROOM);
		act ("You drop $p.", ch, obj, NULL, TO_CHAR);
	}
	else
	{
		/* 'drop all' or 'drop all.obj' */
		if (fifty_objects (ch))
			return;
		found = FALSE;
		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;

			if ((arg[3] == '\0' || is_name (&arg[4], obj->name)) && can_see_obj (ch, obj) && obj->wear_loc == WEAR_NONE && can_drop_obj (ch, obj))
			{
				found = TRUE;
				obj_from_char (obj);
				obj_to_room (obj, ch->in_room);
				/* Objects should only have a shadowplane flag when on the floor */
				if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && (!IS_SET (obj->extra_flags, ITEM_SHADOWPLANE)))
					SET_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
				act ("$n drops $p.", ch, obj, NULL, TO_ROOM);
				act ("You drop $p.", ch, obj, NULL, TO_CHAR);
			}
		}

		if (!found)
		{
			if (arg[3] == '\0')
				act ("You are not carrying anything.", ch, NULL, arg, TO_CHAR);
			else
				act ("You are not carrying any $T.", ch, NULL, &arg[4], TO_CHAR);
		}
	}

	do_autosave (ch, "");
	return;
}



void do_give (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	bool found_vic = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Give what to whom?\n\r", ch);
		return;
	}
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);
	
	if (is_number (arg1))
	{
		/* 'give NNNN coins victim' */
		int amount;

		amount = atoi (arg1);
		if (amount <= 0 || (str_cmp (arg2, "pounds") && str_cmp (arg2, "pound")))
		{
			send_to_char ("Sorry, you can't do that.\n\r", ch);
			return;
		}

		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char ("Give what to whom?\n\r", ch);
			return;
		}
		
		if ((victim = get_char_room_from_alias(ch, arg2)) != NULL)
			found_vic = 1;
		else if ((victim = get_char_room_from_desc(ch, arg2)) != NULL)
			found_vic = 1;
		if (!found_vic)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}

		if (IS_AFFECTED (victim, AFF_ETHEREAL) || IS_EXTRA (victim, EXTRA_ILLUSION))
		{
			send_to_char ("You cannot give things to ethereal people.\n\r", ch);
			return;
		}

		if (ch->pounds < amount)
		{
			send_to_char ("You haven't got that many pounds.\n\r", ch);
			return;
		}

		ch->pounds -= amount;
		victim->pounds += amount;
		act ("$n hands you some pound coins.", ch, NULL, victim, TO_VICT);
		act ("$n hands $N some pound coins.", ch, NULL, victim, TO_NOTVICT);
		act ("You hand $N some pound coins.", ch, NULL, victim, TO_CHAR);
		mprog_bribe_trigger (victim, ch, amount);	//mob programs
//		send_to_char ("OK.\n\r", ch);
		do_autosave (ch, "");
		do_autosave (victim, "");
		return;
	}

	if ((obj = get_obj_carry (ch, arg1)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}

	if (obj->wear_loc != WEAR_NONE)
	{
		send_to_char ("You must remove it first.\n\r", ch);
		return;
	}

	if ((victim = get_char_room_from_alias(ch, arg2)) != NULL)
		found_vic = 1;
	else if ((victim = get_char_room_from_desc(ch, arg2)) != NULL)
		found_vic = 1;
	if (!found_vic)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (get_obj_carry (victim, "contract") != NULL)
	{
		send_to_char (" They already have a daemon contract, they cannot have another until they have dealt with the first.\n\r", ch);
		return;
	}

	if (obj->item_type != ITEM_CONTRACT && !can_drop_obj (ch, obj))
	{
		send_to_char ("You can't let go of it.\n\r", ch);
		return;
	}

	if (IS_AFFECTED (victim, AFF_ETHEREAL))
	{
		send_to_char ("You cannot give things to ethereal people.\n\r", ch);
		return;
	}

	if (victim->carry_number + 1 > can_carry_n (victim))
	{
		act ("$N has $S hands full.", ch, NULL, victim, TO_CHAR);
		return;
	}

	/*if (victim->carry_weight + get_obj_weight (obj) > can_carry_w (victim))
	{
		act ("$N can't carry that much weight.", ch, NULL, victim, TO_CHAR);
		return;
	}*/

	if (!can_see_obj (victim, obj))
	{
		act ("$N can't see it.", ch, NULL, victim, TO_CHAR);
		return;
	}

	obj_from_char (obj);
	obj_to_char (obj, victim);
	act ("$n gives $p to $N.", ch, obj, victim, TO_NOTVICT);
	act ("$n gives you $p.", ch, obj, victim, TO_VICT);
	act ("You give $p to $N.", ch, obj, victim, TO_CHAR);
	if (obj->item_type == ITEM_CONTRACT)
	{
		if (obj->questowner != NULL)
			free_string (obj->questowner);
		obj->questowner = str_dup (victim->name);
	}

	do_autosave (ch, "");
	do_autosave (victim, "");

	mprog_give_trigger (victim, ch, obj);	//mob programs

	return;
}

/*
 * Remove an object.
 */
bool remove_obj (CHAR_DATA * ch, int iWear, bool fReplace)
{
	OBJ_DATA *obj;

	if ((obj = get_eq_char (ch, iWear)) == NULL)
		return TRUE;

	if (!fReplace)
		return FALSE;

	if (IS_SET (obj->extra_flags, ITEM_NOREMOVE))
	{
		act ("You can't remove $p.", ch, obj, NULL, TO_CHAR);
		return FALSE;
	}

	unequip_char (ch, obj);
	act ("$n stops using $p.", ch, obj, NULL, TO_ROOM);
	act ("You stop using $p.", ch, obj, NULL, TO_CHAR);
	return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj (CHAR_DATA * ch, OBJ_DATA * obj, bool fReplace)
{
	bool wolf_ok = TRUE;

	if (CAN_WEAR (obj, ITEM_WIELD) || CAN_WEAR (obj, ITEM_HOLD) || obj->item_type == ITEM_LIGHT)
	{
		if (get_eq_char (ch, WEAR_WIELD) != NULL && get_eq_char (ch, WEAR_HOLD) != NULL && !remove_obj (ch, WEAR_WIELD, fReplace) && !remove_obj (ch, WEAR_HOLD, fReplace))
			return;

		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WIELD))
		{
			send_to_char ("You are unable to use it.\n\r", ch);
			return;
		}
		if (get_eq_char (ch, WEAR_WIELD) == NULL && is_ok_to_wear (ch, wolf_ok, "right_hand"))
		{
			if (obj->item_type == ITEM_LIGHT)
			{
				act ("$n lights $p and clutches it in $s right hand.", ch, obj, NULL, TO_ROOM);
				act ("You light $p and clutch it in your right hand.", ch, obj, NULL, TO_CHAR);
			}
			else
			{
				act ("$n clutches $p in $s right hand.", ch, obj, NULL, TO_ROOM);
				act ("You clutch $p in your right hand.", ch, obj, NULL, TO_CHAR);
			}
			if (obj->item_type == ITEM_WEAPON)
			{
				if (IS_OBJ_STAT (obj, ITEM_LOYAL))
				{
					if (strlen (obj->questowner) > 1 && str_cmp (ch->name, obj->questowner))
					{
						act ("$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM);
						act ("$p leaps out of your hand.", ch, obj, NULL, TO_CHAR);
						obj_from_char (obj);
						obj_to_room (obj, ch->in_room);
						return;
					}
				}
				equip_char (ch, obj, WEAR_WIELD);
				return;
			}
			equip_char (ch, obj, WEAR_WIELD);
			return;
		}
		else if (get_eq_char (ch, WEAR_HOLD) == NULL && is_ok_to_wear (ch, wolf_ok, "left_hand"))
		{
			if (obj->item_type == ITEM_LIGHT)
			{
				act ("$n lights $p and clutches it in $s left hand.", ch, obj, NULL, TO_ROOM);
				act ("You light $p and clutch it in your left hand.", ch, obj, NULL, TO_CHAR);
			}
			else
			{
				act ("$n clutches $p in $s left hand.", ch, obj, NULL, TO_ROOM);
				act ("You clutch $p in your left hand.", ch, obj, NULL, TO_CHAR);
			}
			if (obj->item_type == ITEM_WEAPON)
			{
				if (IS_OBJ_STAT (obj, ITEM_LOYAL))
				{
					if (strlen (obj->questowner) > 1 && str_cmp (ch->name, obj->questowner))
					{
						act ("$p leaps out of $n's hand.", ch, obj, NULL, TO_ROOM);
						act ("$p leaps out of your hand.", ch, obj, NULL, TO_CHAR);
						obj_from_char (obj);
						obj_to_room (obj, ch->in_room);
						return;
					}
				}
				equip_char (ch, obj, WEAR_HOLD);
				return;
			}
			equip_char (ch, obj, WEAR_HOLD);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "left_hand") && !is_ok_to_wear (ch, wolf_ok, "right_hand"))
			send_to_char ("You cannot use anything in your hands.\n\r", ch);
		else
			send_to_char ("You have no free hands.\n\r", ch);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_FINGER))
	{
		if (get_eq_char (ch, WEAR_FINGER_L) != NULL && get_eq_char (ch, WEAR_FINGER_R) != NULL && !remove_obj (ch, WEAR_FINGER_L, fReplace) && !remove_obj (ch, WEAR_FINGER_R, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_FINGER))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}

		if (get_eq_char (ch, WEAR_FINGER_L) == NULL && is_ok_to_wear (ch, wolf_ok, "left_finger"))
		{
			act ("$n wears $p on $s left finger.", ch, obj, NULL, TO_ROOM);
			act ("You wear $p on your left finger.", ch, obj, NULL, TO_CHAR);
			equip_char (ch, obj, WEAR_FINGER_L);
			return;
		}
		else if (get_eq_char (ch, WEAR_FINGER_R) == NULL && is_ok_to_wear (ch, wolf_ok, "right_finger"))
		{
			act ("$n wears $p on $s right finger.", ch, obj, NULL, TO_ROOM);
			act ("You wear $p on your right finger.", ch, obj, NULL, TO_CHAR);
			equip_char (ch, obj, WEAR_FINGER_R);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "left_finger") && !is_ok_to_wear (ch, wolf_ok, "right_finger"))
			send_to_char ("You cannot wear any rings.\n\r", ch);
		else
			send_to_char ("You cannot wear any more rings.\n\r", ch);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_NECK))
	{
		if (get_eq_char (ch, WEAR_NECK_1) != NULL && get_eq_char (ch, WEAR_NECK_2) != NULL && !remove_obj (ch, WEAR_NECK_1, fReplace) && !remove_obj (ch, WEAR_NECK_2, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_NECK))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}

		if (get_eq_char (ch, WEAR_NECK_1) == NULL)
		{
			act ("$n slips $p around $s neck.", ch, obj, NULL, TO_ROOM);
			act ("You slip $p around your neck.", ch, obj, NULL, TO_CHAR);
			equip_char (ch, obj, WEAR_NECK_1);
			return;
		}

		if (get_eq_char (ch, WEAR_NECK_2) == NULL)
		{
			act ("$n slips $p around $s neck.", ch, obj, NULL, TO_ROOM);
			act ("You slip $p around your neck.", ch, obj, NULL, TO_CHAR);
			equip_char (ch, obj, WEAR_NECK_2);
			return;
		}
		bug ("Wear_obj: no free neck.", 0);
		send_to_char ("You are already wearing two things around your neck.\n\r", ch);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_BODY))
	{
		if (!remove_obj (ch, WEAR_BODY, fReplace))
			return;

		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_BODY))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		act ("$n fits $p on $s body.", ch, obj, NULL, TO_ROOM);
		act ("You fit $p on your body.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_BODY);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_HEAD))
	{
		if (!remove_obj (ch, WEAR_HEAD, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_HEAD))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "head"))
		{
			send_to_char ("You have no head to wear it on.\n\r", ch);
			return;
		}
		act ("$n places $p on $s head.", ch, obj, NULL, TO_ROOM);
		act ("You place $p on your head.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_HEAD);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_FACE))
	{
		if (!remove_obj (ch, WEAR_FACE, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_HEAD))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "face"))
		{
			send_to_char ("You have no face to wear it on.\n\r", ch);
			return;
		}
		act ("$n places $p on $s face.", ch, obj, NULL, TO_ROOM);
		act ("You place $p on your face.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_FACE);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_LEGS))
	{
		if (!remove_obj (ch, WEAR_LEGS, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_LEGS))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "legs"))
		{
			send_to_char ("You have no legs to wear them on.\n\r", ch);
			return;
		}
		act ("$n slips $s legs into $p.", ch, obj, NULL, TO_ROOM);
		act ("You slip your legs into $p.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_LEGS);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_FEET))
	{
		if (!remove_obj (ch, WEAR_FEET, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_FEET))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "feet"))
		{
			send_to_char ("You have no feet to wear them on.\n\r", ch);
			return;
		}
		act ("$n slips $s feet into $p.", ch, obj, NULL, TO_ROOM);
		act ("You slip your feet into $p.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_FEET);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_HANDS))
	{
		if (!remove_obj (ch, WEAR_HANDS, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_HANDS))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "hands"))
		{
			send_to_char ("You have no hands to wear them on.\n\r", ch);
			return;
		}
		act ("$n pulls $p onto $s hands.", ch, obj, NULL, TO_ROOM);
		act ("You pull $p onto your hands.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_HANDS);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_TATTOO))
	{
		if (!remove_obj (ch, WEAR_TATTOO, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_TATTOO))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "tattoo"))
		{
			send_to_char ("You have no body to tattoo anything onto.\n\r", ch);
			return;
		}
		act ("$n tattoos $s body with $p.", ch, obj, NULL, TO_ROOM);
		act ("You tattoo your body with $p.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_TATTOO);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_ABOUT))
	{
		if (!remove_obj (ch, WEAR_ABOUT, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_ABOUT))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		act ("$n pulls $p about $s body.", ch, obj, NULL, TO_ROOM);
		act ("You pull $p about your body.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_ABOUT);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_WAIST))
	{
		if (!remove_obj (ch, WEAR_WAIST, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_WAIST))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		act ("$n ties $p around $s waist.", ch, obj, NULL, TO_ROOM);
		act ("You tie $p around your waist.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_WAIST);
		return;
	}
	
		if (CAN_WEAR (obj, ITEM_WEAR_SHOULDER))
	{
		if (!remove_obj (ch, WEAR_SHOULDER, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_SHOULDER))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		act ("$n slings $p over $s shoulder.", ch, obj, NULL, TO_ROOM);
		act ("You sling $p over your shoulder.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_SHOULDER);
		return;
	}

	if (CAN_WEAR (obj, ITEM_WEAR_WRIST))
	{
		if (get_eq_char (ch, WEAR_WRIST_L) != NULL && get_eq_char (ch, WEAR_WRIST_R) != NULL && !remove_obj (ch, WEAR_WRIST_L, fReplace) && !remove_obj (ch, WEAR_WRIST_R, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_WRIST))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}

		if (get_eq_char (ch, WEAR_WRIST_L) == NULL && is_ok_to_wear (ch, wolf_ok, "right_wrist"))
		{
			act ("$n slides $s left wrist into $p.", ch, obj, NULL, TO_ROOM);
			act ("You slide your left wrist into $p.", ch, obj, NULL, TO_CHAR);
			equip_char (ch, obj, WEAR_WRIST_L);
			return;
		}
		else if (get_eq_char (ch, WEAR_WRIST_R) == NULL && is_ok_to_wear (ch, wolf_ok, "left_wrist"))
		{
			act ("$n slides $s right wrist into $p.", ch, obj, NULL, TO_ROOM);
			act ("You slide your right wrist into $p.", ch, obj, NULL, TO_CHAR);
			equip_char (ch, obj, WEAR_WRIST_R);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "left_wrist") && !is_ok_to_wear (ch, wolf_ok, "right_wrist"))
			send_to_char ("You cannot wear anything on your wrists.\n\r", ch);
		else
			send_to_char ("You cannot wear any more on your wrists.\n\r", ch);
		return;
	}

	if (fReplace)
		send_to_char ("You can't wear, wield or hold that.\n\r", ch);

	return;
}



void do_wear (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument (argument, arg);


	if (IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_NPC (ch) && ch->polyaff != 0 && !IS_VAMPAFF (ch, VAM_DISGUISED) && !IS_CHANGELING (ch))
	{
		send_to_char ("You cannot wear anything in this form.\n\r", ch);
		return;
	}


	if (!IS_NPC (ch) && IS_WEREWOLF (ch) && ch->pcdata->wolfform[1] > FORM_DALU && ch->pcdata->wolfform[1] != FORM_GAURU)
	{
		send_to_char ("You cannot wield EQ in this form.\n\r", ch);
		return;
	}

	if (!IS_NPC (ch) && (IS_VAMPIRE (ch) || IS_GHOUL (ch)) && IS_VAMPAFF (ch, VAM_CHANGED) && (IS_POLYAFF (ch, POLY_MIST) || IS_POLYAFF (ch, POLY_ZULO) || IS_POLYAFF (ch, POLY_SHADOW) || IS_POLYAFF (ch, POLY_WILDEBEAST)))
	{
		send_to_char ("You cannot wield EQ in this form.\n\r", ch);
		return;
	}


	if (arg[0] == '\0')
	{
		send_to_char ("Wear, wield, or hold what?\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
		
	if (!str_cmp (arg, "all"))
	{
		OBJ_DATA *obj_next;

		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj))
				wear_obj (ch, obj, FALSE);
		}
		return;
	}
	else
	{
		if ((obj = get_obj_carry (ch, arg)) == NULL)
		{
			send_to_char ("You do not have that item.\n\r", ch);
			return;
		}

		wear_obj (ch, obj, TRUE);
	}

	return;
}



void do_remove (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument (argument, arg);

	if (IS_EXTRA (ch, EXTRA_ILLUSION))
	{
		send_to_char ("You cannot remove your equipment.\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Remove what?\n\r", ch);
		return;
	}
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
	
	if (!str_cmp (arg, "all"))
	{
		OBJ_DATA *obj_next;

		for (obj = ch->carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			if (obj->wear_loc != WEAR_NONE && can_see_obj (ch, obj))
				remove_obj (ch, obj->wear_loc, TRUE);
		}
		return;
	}
	if ((obj = get_obj_wear (ch, arg)) == NULL)
	{
		send_to_char ("You do not have that item.\n\r", ch);
		return;
	}
	remove_obj (ch, obj->wear_loc, TRUE);
	return;
}



void do_sacrifice (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	char buf[MAX_INPUT_LENGTH];
	int expgain;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("#gJunk what?#n\n\r", ch);
		return;
	}

	obj = get_obj_list (ch, arg, ch->in_room->contents);
	if (obj == NULL)
	{
		send_to_char ("#gYou don't see item that on the ground.#n\n\r", ch);
		return;
	}
	if (!CAN_WEAR (obj, ITEM_TAKE) || obj->item_type == ITEM_QUEST || obj->item_type == ITEM_MONEY || obj->item_type == ITEM_TREASURE || obj->item_type == ITEM_QUESTCARD || (obj->questowner != NULL && strlen (obj->questowner) > 1 && str_cmp (ch->name, obj->questowner) && obj->item_type != ITEM_CORPSE_NPC))
	{
		act ("You are unable to drain any energy from $p.", ch, obj, 0, TO_CHAR);
		return;
	}
	else if (obj->chobj != NULL && !IS_NPC (obj->chobj) && obj->chobj->pcdata->obj_vnum != 0)
	{
		act ("You are unable to drain any energy from $p.", ch, obj, 0, TO_CHAR);
		return;
	}
	expgain = obj->cost / 100;
	if (expgain < 1)
		expgain = 1;
	if (expgain > 50)
		expgain = 50;

	act ("#gYou toss #G$p#g into the bin.#n", ch, obj, NULL, TO_CHAR);
//	act ("$n drains the energy from $p.", ch, obj, NULL, TO_ROOM);
	act ("#g$n tosses #G$p#g into the bin.#n", ch, obj, NULL, TO_ROOM);
	if (obj->points > 0 && !IS_NPC (ch) && obj->item_type != ITEM_PAGE)
	{
		sprintf (buf, "You receive a refund of %d quest points from $p.", obj->points);
		act (buf, ch, obj, NULL, TO_CHAR);
		ch->pcdata->quest += obj->points;
		do_autosave (ch, "");
	}
	extract_obj (obj);
	return;
}



bool is_ok_to_wear (CHAR_DATA * ch, bool wolf_ok, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);

	if (!str_cmp (arg, "left_hand"))
	{
		if (!IS_NPC (ch) && IS_SET (ch->act, PLR_WOLFMAN) && !wolf_ok)
			return FALSE;
	}
	else if (!str_cmp (arg, "right_hand"))
	{
		if (!IS_NPC (ch) && IS_SET (ch->act, PLR_WOLFMAN) && !wolf_ok)
			return FALSE;
	}
	return TRUE;
}



void do_sheath (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (arg[0] == '\0')
		send_to_char ("Which hand, left or right?\n\r", ch);
	else if (!str_cmp (arg, "all") || !str_cmp (arg, "both"))
	{
		sheath (ch, TRUE);
		sheath (ch, FALSE);
	}
	else if (!str_cmp (arg, "l") || !str_cmp (arg, "left"))
		sheath (ch, FALSE);
	else if (!str_cmp (arg, "r") || !str_cmp (arg, "right"))
		sheath (ch, TRUE);
	else
		send_to_char ("Which hand, left or right?\n\r", ch);
	return;
}

void do_draw (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (arg[0] == '\0')
		send_to_char ("Which hand, left or right?\n\r", ch);
	else if (!str_cmp (arg, "all") || !str_cmp (arg, "both"))
	{
		draw (ch, TRUE);
		draw (ch, FALSE);
	}
	else if (!str_cmp (arg, "l") || !str_cmp (arg, "left"))
		draw (ch, FALSE);
	else if (!str_cmp (arg, "r") || !str_cmp (arg, "right"))
		draw (ch, TRUE);
	else
		send_to_char ("Which hand, left or right?\n\r", ch);
	return;
}

void sheath (CHAR_DATA * ch, bool right)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	int scabbard;

	if (right)
	{
		scabbard = WEAR_SCABBARD_R;
		if ((obj = get_eq_char (ch, WEAR_WIELD)) == NULL)
		{
			send_to_char ("You are not holding anything in your right hand.\n\r", ch);
			return;
		}
		else if ((obj2 = get_eq_char (ch, scabbard)) != NULL)
		{
			act ("You already have $p in your right pocket.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		if (obj->item_type == ITEM_WEAPON)
		{
			act ("You slide $p into your right pocket.", ch, obj, NULL, TO_CHAR);
			act ("$n slides $p into $s right pocket.", ch, obj, NULL, TO_ROOM);
		}
	}
	else
	{
		scabbard = WEAR_SCABBARD_L;
		if ((obj = get_eq_char (ch, WEAR_HOLD)) == NULL)
		{
			send_to_char ("You are not holding anything in your left hand.\n\r", ch);
			return;
		}
		else if ((obj2 = get_eq_char (ch, scabbard)) != NULL)
		{
			act ("You already have $p in your left pocket.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		if (obj->item_type == ITEM_WEAPON)
		{
			act ("You slide $p into your left pocket.", ch, obj, NULL, TO_CHAR);
			act ("$n slides $p into $s left pocket.", ch, obj, NULL, TO_ROOM);
		}
	}
	if (obj->item_type != ITEM_WEAPON)
	{
		act ("$p is not a weapon.", ch, obj, NULL, TO_CHAR);
		return;
	}
	unequip_char (ch, obj);
	obj->wear_loc = scabbard;
	return;
}

void draw (CHAR_DATA * ch, bool right)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	int scabbard;
	int worn;
	if (right)
	{
		scabbard = WEAR_SCABBARD_R;
		worn = WEAR_WIELD;
		if ((obj = get_eq_char (ch, scabbard)) == NULL)
		{
			send_to_char ("Your right pocket is empty.\n\r", ch);
			return;
		}
		else if ((obj2 = get_eq_char (ch, WEAR_WIELD)) != NULL)
		{
			act ("You already have $p in your right hand.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		act ("You draw $p from your right pocket.", ch, obj, NULL, TO_CHAR);
		act ("$n draws $p from $s right pocket.", ch, obj, NULL, TO_ROOM);
	}
	else
	{
		scabbard = WEAR_SCABBARD_L;
		worn = WEAR_HOLD;
		if ((obj = get_eq_char (ch, scabbard)) == NULL)
		{
			send_to_char ("Your left pocket is empty.\n\r", ch);
			return;
		}
		else if ((obj2 = get_eq_char (ch, WEAR_HOLD)) != NULL)
		{
			act ("You already have $p in your left hand.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		act ("You draw $p from your left pocket.", ch, obj, NULL, TO_CHAR);
		act ("$n draws $p from $s left pocket.", ch, obj, NULL, TO_ROOM);
	}
	obj->wear_loc = -1;
	equip_char (ch, obj, worn);
	return;
}

void do_special (CHAR_DATA * ch, char *argument)
{
	char bname[MAX_INPUT_LENGTH];
	char bshort[MAX_INPUT_LENGTH];
	char blong[MAX_INPUT_LENGTH];
	char *kav;
	int dice = number_range (1, 3);
	OBJ_DATA *obj;

	obj = create_object (get_obj_index (OBJ_VNUM_PROTOPLASM), 0);

	kav = special_item_name (obj);

	switch (dice)
	{
	default:
		sprintf (bname, "%s ring", kav);
		sprintf (bshort, "a %s ring", kav);
		sprintf (blong, "A %s ring lies here.", kav);
		obj->wear_flags = ITEM_WEAR_FINGER + ITEM_TAKE;
		break;
	case 1:
		sprintf (bname, "%s ring", kav);
		sprintf (bshort, "a %s ring", kav);
		sprintf (blong, "A %s ring lies here.", kav);
		obj->wear_flags = ITEM_WEAR_FINGER + ITEM_TAKE;
		break;
	case 2:
		sprintf (bname, "%s necklace", kav);
		sprintf (bshort, "a %s necklace", kav);
		sprintf (blong, "A %s necklace lies here.", kav);
		obj->wear_flags = ITEM_WEAR_NECK + ITEM_TAKE;
		break;
	case 3:
		sprintf (bname, "%s plate", kav);
		sprintf (bshort, "a suit of %s platemail", kav);
		sprintf (blong, "A suit of %s platemail lies here.", kav);
		obj->wear_flags = ITEM_WEAR_BODY + ITEM_TAKE;
		break;
	}

	if (obj->wear_flags == 513 || obj->wear_flags == 8193 || obj->wear_flags == 16385)
	{
		obj->item_type = ITEM_WEAPON;
		obj->value[1] = 10;
		obj->value[2] = 20;
		obj->value[3] = number_range (1, 12);
	}
	else
	{
		obj->item_type = ITEM_ARMOR;
		obj->value[0] = 15;
	}

	obj->level = 50;
	obj->cost = 100000;

	if (obj->questmaker != NULL)
		free_string (obj->questmaker);
	obj->questmaker = str_dup (ch->name);

	free_string (obj->name);
	obj->name = str_dup (bname);

	free_string (obj->short_descr);
	obj->short_descr = str_dup (bshort);

	free_string (obj->description);
	obj->description = str_dup (blong);

	obj_to_char (obj, ch);
	return;
}

char *special_item_name (OBJ_DATA * obj)
{
	static char buf[MAX_INPUT_LENGTH];
	int dice = number_range (1, 4);
	switch (dice)
	{
	default:
		strcpy (buf, "pound");
		break;
	case 1:
		strcpy (buf, "pound");
		break;
	case 2:
		strcpy (buf, "silver");
		break;
	case 3:
		strcpy (buf, "brass");
		break;
	case 4:
		strcpy (buf, "copper");
		break;
	}
	return buf;
}

bool fifty_objects (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int obj_count = 0;

	if (ch->in_room == NULL)
		return TRUE;

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		obj_count++;
		if (obj_count > 50)
		{
			send_to_char ("You are unable to drop anything else here.\n\r", ch);
			return TRUE;
		}
	}
	return FALSE;
}

bool hundred_objects (CHAR_DATA * ch, OBJ_DATA * container)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int obj_count = 0;

	for (obj = container->contains; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		obj_count++;
		if (obj_count > 100)
		{
			act ("You cannot fit anything else into $p.", ch, container, NULL, TO_CHAR);
			return TRUE;
		}
	}
	return FALSE;
}

void set_disc (CHAR_DATA * ch)
{
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int disc;
	int set_wear;

	if (IS_NPC (ch))
		return;

	if (ch->polyaff > 0)
	{
		if (IS_POLYAFF (ch, POLY_BAT) || IS_POLYAFF (ch, POLY_WOLF) || IS_POLYAFF (ch, POLY_MIST) || IS_POLYAFF (ch, POLY_SERPENT) || IS_POLYAFF (ch, POLY_RAVEN) || IS_POLYAFF (ch, POLY_FISH) || IS_POLYAFF (ch, POLY_FROG) || IS_POLYAFF (ch, POLY_SHADOW))
			return;
	}

	set_wear = ch->pcdata->powers_set_wear;
	ch->pcdata->powers_set_wear = 0;

	for (disc = DISC_DOMINATE; disc < DISC_MAX; disc++)
	{
		ch->pcdata->powers_set[disc] = 0;
	}

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->wear_loc == WEAR_NONE)
			continue;
		for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location >= APPLY_SET_DOMINATE && paf->location <= APPLY_SET_SEPARATION)
				affect_modify (ch, paf, TRUE);
		}
		for (paf = obj->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location >= APPLY_SET_DOMINATE && paf->location <= APPLY_SET_SEPARATION)
				affect_modify (ch, paf, TRUE);
		}
	}
	if (set_wear != ch->pcdata->powers_set_wear)
		strip_disc (ch);
	return;
}

void strip_disc (CHAR_DATA * ch)
{
	int disc;

	if (IS_NPC (ch))
		return;
	if (IS_VAMPIRE (ch) || IS_GHOUL (ch))
	{
		for (disc = DISC_DOMINATE; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] += 20;
			ch->pcdata->powers_set[disc] += 20;
		}
		if (IS_POLYAFF (ch, POLY_SKINMASK) && get_truedisc (ch, DISC_SEPARATION) < 23)
			do_skinmask (ch, "self");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES) && get_truedisc (ch, DISC_AUSPEX) < 21)
			do_truesight (ch, "");
		if (IS_SET (ch->act, PLR_HOLYLIGHT) && get_truedisc (ch, DISC_AUSPEX) < 21)
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_FORGE) && get_truedisc (ch, DISC_FORGE) == 20)
			ch->pcdata->forge = 0;
		for (disc = DISC_DOMINATE; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] -= 20;
			ch->pcdata->powers_set[disc] -= 20;
		}
	}
	return;
}

void mortal_vamp (CHAR_DATA * ch)
{
	int disc;

	if (IS_NPC (ch))
		return;
	if (IS_CHANGELING (ch) || IS_GHOUL (ch) || IS_WEREWOLF(ch))
	{
		for (disc = DISC_MIRROR; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] += 20;
			ch->pcdata->powers_set[disc] += 20;
		}
		if (IS_POLYAFF (ch, POLY_SKINMASK))
			do_skinmask (ch, "self");
		if (IS_SET (ch->act, PLR_HOLYLIGHT))
			do_truesight (ch, "");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_FORGE))
			ch->pcdata->forge = 0;

		for (disc = DISC_MIRROR; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] -= 20;
			ch->pcdata->powers_set[disc] -= 20;
		}
	}
	return;
}

void do_insertpage( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *page;
    OBJ_DATA *book;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {send_to_char("Syntax is: insert page book.\n\r",ch);return;}
    if ( ( page = get_obj_carry( ch, arg1 ) ) == NULL )
    {send_to_char("You are not carrying that page.\n\r",ch);return;}
    if ( page->item_type != ITEM_PAGE )
    {send_to_char("That item isn't a page.\n\r",ch);return;}
    if ( ( book = get_obj_carry( ch, arg2 ) ) == NULL )
    {send_to_char("You are not carrying that book.\n\r",ch);return;}
    if ( book->item_type != ITEM_BOOK )
    {send_to_char("That item isn't a book.\n\r",ch);return;}
    if ( IS_SET(book->value[1], CONT_CLOSED) )
    {send_to_char("First you need to open it!\n\r",ch);return;}

    obj_from_char(page);
    obj_to_obj(page,book);
    book->value[3] += 1;
    book->value[2] = book->value[3];
    page->value[0] = book->value[3];
    page->specpower = book->value[3]+1;

    act("You insert $p into $P.",ch,page,book,TO_CHAR);
    act("$n inserts $p into $P.",ch,page,book,TO_ROOM);
    return;
}

void do_tearpage( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_DATA *page;
    OBJ_DATA *book;
    OBJ_DATA *page_next;
    int count = 0;

	argument = one_argument (argument, arg1);

    if ( arg1[0] == '\0' )
    {send_to_char("Syntax is: cast 'remove page' <book>.\n\r",ch);return;}
    if ( ( book = get_obj_carry( ch, arg1 ) ) == NULL )
    {send_to_char("You are not carrying that book.\n\r",ch);return;}
    if ( book->item_type != ITEM_BOOK )
    {send_to_char("That item isn't a book.\n\r",ch);return;}
    if ( IS_SET(book->value[1], CONT_CLOSED) )
    {send_to_char("First you need to open it!\n\r",ch);return;}
    if ( book->value[2] == 0 )
    {send_to_char("You cannot remove the index page!\n\r",ch);return;}

    if ( ( page = get_page(book, book->value[2]) ) == NULL )
    {
	send_to_char("The page seems to have been torn out.\n\r",ch);
	return;
    }

    obj_from_obj(page);
    obj_to_char(page,ch);
    page->value[0] = 0;

    act("You remove $p from $P.",ch,page,book,TO_CHAR);
    act("$n removes $p from $P.",ch,page,book,TO_ROOM);

    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	count += 1;
	page->value[0] = count;
    }
    book->value[3] = count;

    if (book->value[2] > book->value[3]) book->value[2] = book->value[3];

    return;
}

void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	{
	    if ( obj->item_type == ITEM_FOUNTAIN )
		break;
	}

	if ( obj == NULL )
	{
	    send_to_char( "Drink what?\n\r", ch );
	    return;
	}
    }
    else
    {
	if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
	{
	    send_to_char( "You can't find it.\n\r", ch );
	    return;
	}
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 15 )
    {
	send_to_char( "You are too drunk to drink even one drop more.\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You can't drink from that.\n\r", ch );
	break;
	
    case ITEM_FOUNTAIN:
	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	if (IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		obj->in_room != NULL &&
		!IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
	{
	    send_to_char( "You are too insubstantial.\n\r", ch );
	    break;
	}
	else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) &&
		obj->in_room != NULL &&
		IS_SET(obj->extra_flags, ITEM_SHADOWPLANE) )
	{
	    send_to_char( "It is too insubstantial.\n\r", ch );
	    break;
	}
    	else if (IS_AFFECTED(ch,AFF_ETHEREAL) && obj->carried_by == NULL )
    	{
	    send_to_char( "You can only drink from things you are carrying while ethereal.\n\r", ch );
	    return;
    	}

	if ( liquid != 13 && !IS_NPC(ch) && IS_VAMPIRE(ch) )
	{
	    send_to_char( "You can only drink blood.\n\r", ch );
	    break;
	}

	act( "$n drinks $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You drink $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_CHAR );
	if (!IS_VAMPIRE(ch) && ch->blood[0] < 100) ch->blood[0]++;

	amount = number_range(3, 10);
	amount = UMIN(amount, obj->value[1]);
	
	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );

	if ( !IS_NPC(ch) && !IS_VAMPIRE(ch) &&
		ch->pcdata->condition[COND_DRUNK]  > 10 )
	    send_to_char( "You feel drunk.\n\r", ch );

	if ( obj->value[3] != 0 && (!IS_NPC(ch) && !IS_VAMPIRE(ch)))
	{
	    /* The shit was poisoned ! */
	    AFFECT_DATA af;

	    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
	    send_to_char( "You choke and gag.\n\r", ch );
	    af.type      = gsn_poison;
	    af.duration  = 3 * amount;
	    af.location  = APPLY_NONE;
	    af.modifier  = 0;
	    af.bitvector = AFF_POISON;
	    affect_join( ch, &af );
	}
	break;

    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	if ( liquid != 13 && !IS_NPC(ch) && IS_VAMPIRE(ch) )
	{
	    send_to_char( "You can only drink blood.\n\r", ch );
	    break;
	}

	act( "$n drinks $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You drink $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR );

	amount = number_range(3, 10);
	amount = UMIN(amount, obj->value[1]);
	
	if (IS_SET (obj->extra_flags, ITEM_MAGIC))
	{
		if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		{
		act ("You feel a head-rush of glamour from $p.", ch, obj, NULL, TO_CHAR);
		ch->blood[BLOOD_CURRENT]++;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	if (IS_SET (obj->extra_flags, ITEM_HUM))
	{
		if ((ch->pcdata->lethaldamage) > 0)
		{
		act ("As $p slips down your throat you feel a soft, soothing warmth creep through your body.", ch, obj, NULL, TO_CHAR);
		ch->pcdata->lethaldamage--;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	gain_condition( ch, COND_DRUNK,
	    amount * liq_table[liquid].liq_affect[COND_DRUNK  ] );

	if ( !IS_NPC(ch) && !IS_VAMPIRE(ch) &&
		ch->pcdata->condition[COND_DRUNK]  > 10 )
	    send_to_char( "You feel drunk.\n\r", ch );
	
	obj->value[1] -= amount;
	if ( obj->value[1] <= 0 )
	{
		obj->value[1] = 0;
	}
	break;
    }
    //if (obj != NULL) extract_obj( obj );
    return;
}

void do_empty( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int liquid;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Empty what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
    {
	send_to_char( "You can't find it.\n\r", ch );
	return;
    }

    switch ( obj->item_type )
    {
    default:
	send_to_char( "You cannot empty that.\n\r", ch );
	break;

    case ITEM_DRINK_CON:
	if ( obj->value[1] <= 0 )
	{
	    send_to_char( "It is already empty.\n\r", ch );
	    return;
	}

	if ( ( liquid = obj->value[2] ) >= LIQ_MAX )
	{
	    bug( "Do_drink: bad liquid number %d.", liquid );
	    liquid = obj->value[2] = 0;
	}

	act( "$n empties $T from $p.", ch, obj, liq_table[liquid].liq_name, TO_ROOM );
	act( "You empty $T from $p.",
	    ch, obj, liq_table[liquid].liq_name, TO_CHAR );
	
	obj->value[1] = 0;
	break;
    }

    return;
}

// Allow the applying of perfumes

void do_apply( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Apply what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

	else if ( obj->item_type != ITEM_SCENT )
	{
		send_to_char( "That's not a perfume or cologne.\n\r", ch );
		return;
	}

    act( "$n applies a little bit of $p.",  ch, obj, NULL, TO_ROOM );
    act( "You apply a little bit of $p.", ch, obj, NULL, TO_CHAR );

	if (IS_SET (obj->extra_flags, ITEM_MAGIC))
	{
		act ("#gYou smell like $p.#n", ch, obj, NULL, TO_CHAR);
		SET_BIT(ch->affected_by, AFF_PERFUME);
		if (obj != NULL) extract_obj( obj );
		return;
	}

    if (obj != NULL) extract_obj( obj );
    return;
}

// Washing off scents

/*void do_wash (CHAR_DATA *ch, char *argument)
{
	
	if ( !IS_SET(ch->in_room->room_flags, ROOM_BATH))
	{
	send_to_char("There is nowhere to wash here.\n\r", ch);
    return;
	}
	
	act( "#g$n hops into a hot shower and washes off the grime and sweat of the day.#n",  ch, NULL, NULL, TO_ROOM );
    act( "#gYou hop into a hot shower and wash off the grime and sweat of the day.#n", ch, NULL, NULL, TO_CHAR );
    REMOVE_BIT (ch->affected_by, AFF_PERFUME);
    return;
}*/
	
void do_wash (CHAR_DATA *ch, char *argument)
{	
    char buf[MAX_STRING_LENGTH];

	// Check to ensure it's a bathroom
	if ( !IS_SET(ch->in_room->room_flags, ROOM_BATH))
	{
	send_to_char("There is nowhere to wash here.\n\r", ch);
    return;
	}

WAIT_STATE( ch, PULSE_EVENT * 15 );
wait_printf(ch,1,"#cYou turn on the tap and wait until the water runs hot.#n\n\r",buf);
wait_act(1,"#c$n turns on the shower tap and waits until the water runs hot.#n",ch,buf,0,TO_ROOM);

wait_act(5,"#cYou step into the shower, letting water careen over your head and body.#n",ch,buf,0,TO_CHAR);
wait_act(5,"#c$n steps into the shower, letting water careen over their head and body.#n",ch,buf,0,TO_ROOM);

wait_act(10,"#cYou lather up the soap and rub yourself down, rinsing suds that swirl down the drain.#n",ch,buf,0,TO_CHAR);
wait_act(10,"#c$n lathers up the soap and rubs down, rinsing suds that swirl down the drain.#n",ch,buf,0,TO_ROOM);

wait_act(15,"#c$n tilts the handle to turn the tap off, steps out and towels off.#n", ch, buf, NULL, TO_ROOM );
wait_act(15,"#cYou tilt the handle to turn the tap off, then step out and towel off.#n", ch, buf, NULL, TO_CHAR );
    REMOVE_BIT (ch->affected_by, AFF_PERFUME);
	REMOVE_BIT (ch->affected_by, AFF_DIRTY);
    return;
}

// Rat delivery service

void guardian_message( CHAR_DATA *ch )
{
	send_to_char("Your guardian spirit sends you a telepathic message:\n\r",ch);
    return;
}

void do_rat( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if (!IS_CHANGELING(ch) && !IS_ENSORCELLED(ch))
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

	// Start

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_CITY)) // Cannot send from city
			{
		send_to_char("You cannot do that outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_INSIDE)) // Cannot send from city
			{
		send_to_char("You cannot do that outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_WATER_SWIM)) // Cannot send from city
			{
		send_to_char("You cannot do that outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_WATER_NOSWIM)) // Cannot send from city
			{
		send_to_char("You cannot do that outside the Hedge.\n\r",
				ch);
		return;
	}

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_OOC)) // Cannot send from city
			{
		send_to_char("You cannot do that outside the Hedge.\n\r",
				ch);
		return;
	}

	// End

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Transport which object?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        send_to_char( "Transport what to whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if (victim == ch)
    {
	send_to_char( "You cannot send things to yourself!\n\r", ch );
	return;
    }

    if (victim->in_room == NULL || victim->in_room == ch->in_room)
    {
	send_to_char( "But they are already here!\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
    {
	send_to_char( "You are not carrying that item.\n\r", ch );
	return;
    }


    if ( IS_AFFECTED(ch,AFF_ETHEREAL) || IS_AFFECTED(victim,AFF_ETHEREAL) )
    {
	send_to_char( "Nothing happens.\n\r", ch );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via rat.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_number + 1 > can_carry_n( victim ) )
    {
	act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via rat.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
	act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via rat.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
	act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
	if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
	{
	    guardian_message( victim );
	    act("$n just failed to send $p to you via rat.",ch,obj,victim,TO_VICT);
	}
	return;
    }

    send_to_char( "A rat scuttles out of the brambles and squeaks at your feet.\n\r", ch );
    act("A rat scuttles out of the brambles and squeaks at $n's feet.",ch,NULL,NULL,TO_ROOM);
    act("You place $p in the rat's mouth.",ch,obj,NULL,TO_CHAR);
    act("$n places $p in the rat's mouth.",ch,obj,NULL,TO_ROOM);
    send_to_char( "The rat takes off, scampering into shadows and brambles.\n\r", ch );
    act("The rat takes off from $n's feet, scampering into shadows and brambles.",ch,NULL,NULL,TO_ROOM);

    obj_from_char(obj);
    obj_to_char(obj,victim);

    send_to_char( "A rat scuttles out of the brambles and squeaks at your feet.\n\r", victim );
    act("A rat scuttles out of the brambles and squeaks at $n's feet.",victim,NULL,NULL,TO_ROOM);
    act("You take $p from the rat's mouth.",victim,obj,NULL,TO_CHAR);
    act("$n takes $p from the rat's mouth.",victim,obj,NULL,TO_ROOM);
    send_to_char( "The rat takes off, scampering into shadows and brambles.\n\r", victim );
    act("The rat takes off from $n's feet, scampering into shadows and brambles.",victim,NULL,NULL,TO_ROOM);

    do_autosave(ch,"");
    do_autosave(victim,"");
    if (!IS_NPC(victim) && ch != victim && IS_MORE(victim, MORE_GUARDIAN))
    {
	guardian_message( victim );
	act("$n just sent $p to you via rat.",ch,obj,victim,TO_VICT);
    }
    return;
}

void show_slot(CHAR_DATA *ch, int num)
{
    switch(num)
    {
        case 1:
          send_to_char("   #Rcherry#n   ",ch); // cherry
          break;
        case 2:
          send_to_char("    #pplum#n    ",ch); // plum
          break;
        case 3:
          send_to_char("   #Pgrape#n    ",ch); // grape
          break;
        case 4:
          send_to_char(" #gwatermelon#n ",ch); // watermelon
          break;
        case 5:
          send_to_char("   #oorange#n   ",ch); // orange
          break;
        case 6:
          send_to_char("   #ylemon#n    ",ch); // lemon
          break;
        case 7:
          send_to_char("    #cbar#n     ",ch); // bar
          break;
        case 8:
          send_to_char("    #C2bar#n    ",ch); // 2bar
          break;
        case 9:
          send_to_char("    #C3bar#n    ",ch); // 3bar
          break;
        case 10:
          send_to_char("   #Bseven#n    ",ch); // seven
          break;
    }
return;
}

/*
 *  Rolls the slots.
 */

int roll(void)
{
        int num;
        num = number_range(1,10);
        return num;
}

/*
 *  The two functions just written will need defined in
 *  in either manga.h or at the top of the file the code
 *  is being inserted into.  Define them as shown:
 *
 *  int roll            args( ( void ) );
 *  int show_slot	args( ( CHAR_DATA *ch, int num ) );
 */

/*
 *  The main function... prepare for confusion.
 */

void do_slot(CHAR_DATA *ch, char *argument) // - Nikola(manga.thedigitalrealities.com port: 6969)
{
	    int roll            args( ( void ) );
        int show_slot	args( ( CHAR_DATA *ch, int num ) );
        OBJ_DATA *slot=NULL, *obj;
        char arg1[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];
        char buf[MSL];
        bool CanPlay = TRUE;
        bool winner = FALSE;
        int winnings=0, basebet=0;
        int sa=0, sb=0, sc=0, sd=0, se=0, sf=0, sg=0, sh=0, si=0;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

        if(IS_NPC(ch))
        {
          send_to_char("Mobs cannot gamble.\n\r",ch);
          return;
        }

	//Check the room for the object.

        for ( obj = object_list; obj != NULL; obj = obj->next )
        {
          if(!obj->in_room)
            continue;
          if(obj->in_room != ch->in_room)
            continue;
          if(obj->pIndexData->vnum == OBJ_VNUM_SLOT) //Note:  This is the only line of code that would
          {					     //need changed if you decided to make a 'slot' obj
            slot = obj;				     //type.
            break;
          }
        }

        if(slot == NULL)
        {
          send_to_char("There's no slot machine here.\n\r",ch);
          return;
        }

        if(arg1[0] == '\0')
        {
          send_to_char("Slot commands#n: <#nPLAY#n/#nASCII#n>#n\n\rFor more information see #n'#nHELP SLOTS#n'#n.\n\r",ch);
          return;
        }

	/*
	 *  I added this small bit right here for players with odd fonts or
	 *  strange MUD clients.  Allows them to toggle the ascii imagine
	 *  on or off.
	 */

        if(!strcmp(arg1, "ascii"))
        {
          if(arg2[0] == '\0')
          {
            send_to_char("Syntax#n:#n SLOTS ASCII #n<#nON#n/#nOFF#n>#n\n\r",ch);
            return;
          }

          if(!strcmp(arg2, "on"))
          {
            if(ch->slascii <= 0)
            {
              send_to_char("#RSlot ascii is already turned #rON#R.#n\n\r",ch);
              return;
            }
            ch->slascii = 0;
            send_to_char("#nYou will now see slot ascii.#n\n\r",ch);
          }
          else if(!strcmp(arg2, "off"))
          {
            if(ch->slascii >= 1)
            {
              send_to_char("#RSlot ascii is already turned #rOFF#R.#n\n\r",ch);
              return;
            }
            ch->slascii = 1;
            send_to_char("#nYou will no longer see slot ascii.#n\n\r",ch);
          }
          else
          {
            send_to_char("Syntax#n:#n SLOT ASCII #n<#nON#n/#nOFF#n>#n\n\r",ch);
            return;
          }
        return;
        }

	//Now for the fun part.

        else if(!strcmp(arg1, "play"))
        {
          if(arg2[0] == '\0')
          {
            send_to_char("Syntax#n:#n SLOT PLAY #n<#nAMOUNT#n>#n\n\r",ch);
            return;
          }

          basebet = atoi(arg2);

	  //I made it so that only certain amounts can be wagered, it would be
	  //entirely possible to remove this and go with however much they want
	  //to wager, however I added this to give the slots a touch of reality.

          switch(basebet)
          {
            case 25:
              break;
            default:
              CanPlay = FALSE;
              break;
          }

          if(CanPlay == FALSE)
          {
            send_to_char("The machine only accepts wagers of 25 pounds.\n\r",ch);
            return;
          }

	  //Check to see if the player has enough money and subtract it from
	  //them.  The way I wrote this, if they were to have, say... 1000
	  //dollars in silver, but have no pounds and make a 10 pounds bet, it will
	  //subtract the amount from their silver rather than not let them play.
	  //This also works oppositely, if they have 10 pounds, but don't have
	  //and silver and make a 25 silver bet, it will change 1 pounds into
	  //100 silver and subtract 25 from the silver.

			if ( ch->pounds < 25 )
	        {
            send_to_char("You cannot afford the slot machine.\n\r", ch);
	        return;
	        }

	  //On Manga, we changed 'pounds' and 'silver' to 'dollars' and 'cents',
	  //so you may need to change some of the messages.

          if(basebet < 25)
          {
            sprintf(buf,"You put %d pounds into the slot machine and pull the handle.\n\r",basebet);
            send_to_char(buf,ch);
          }

          act("$n puts some money in the slot machine and pulls the handle.",ch,NULL,NULL,TO_ROOM);

	  //Roll the slots!

          sa = roll();
          sb = roll();
          sc = roll();
          sd = roll();
          se = roll();
          sf = roll();
          sg = roll();
          sh = roll();
          si = roll();

	  //The following section reveals the slots to the player.

          if(ch->slascii >= 1)
          {
            send_to_char("\n\r             #C[#nGEEK CHIC SLOT#C]#n\n\r\n\r",ch);
            send_to_char("  ",ch);
            show_slot(ch,sa);
            send_to_char("  ",ch);
            show_slot(ch,sb);
            send_to_char("  ",ch);
            show_slot(ch,sc);
            send_to_char("\n\r\n\r",ch);
            send_to_char("  ",ch);
            show_slot(ch,sd);
            send_to_char("  ",ch);
            show_slot(ch,se);
            send_to_char("  ",ch);
            show_slot(ch,sf);
            send_to_char("\n\r\n\r",ch);
            send_to_char("  ",ch);
            show_slot(ch,sg);
            send_to_char("  ",ch);
            show_slot(ch,sh);
            send_to_char("  ",ch);
            show_slot(ch,si);
            send_to_char("\n\r\n\r",ch);
            send_to_char("             #C[#nGEEK CHIC SLOT#C]#n\n\r\n\r",ch);
          }
          else
          {
	    send_to_char("\n\r#c||||||||||||||||||||||||||||||||||||||||||||#n\n\r#c||||||||||||||#C[#nGEEK CHIC SLOT#C]#c||||||||||||||#n\n\r#c||||||||||||||||||||||||||||||||||||||||||||#n\n\r#c||            ||            ||            ||#n\n\r",ch);
            send_to_char("#c||#n",ch);
            show_slot(ch,sa);
            send_to_char("#c||#n",ch);
            show_slot(ch,sb);
            send_to_char("#c||#n",ch);
            show_slot(ch,sc);
            send_to_char("#c||#n",ch);
	    send_to_char("\n\r#c||            ||            ||            ||#n\n\r#c||||||||||||||||||||||||||||||||||||||||||||#n\n\r#c||            ||            ||            ||#n\n\r",ch);
            send_to_char("#c||#n",ch);
            show_slot(ch,sd);
            send_to_char("#c||#n",ch);
            show_slot(ch,se);
            send_to_char("#c||#n",ch);
            show_slot(ch,sf);
            send_to_char("#c||#n",ch);
	    send_to_char("\n\r#c||            ||            ||            ||#n\n\r#c||||||||||||||||||||||||||||||||||||||||||||#n\n\r#c||            ||            ||            ||#n\n\r",ch);
            send_to_char("#c||#n",ch);
            show_slot(ch,sg);
            send_to_char("#c||#n",ch);
            show_slot(ch,sh);
            send_to_char("#c||#n",ch);
            show_slot(ch,si);
            send_to_char("#c||#n",ch);    
			send_to_char("\n\r#c||            ||            ||            ||#n\n\r#c||||||||||||||||||||||||||||||||||||||||||||#n\n\r#c||||||||||||||#C[#nGEEK CHIC SLOT#C]#c||||||||||||||#n\n\r#c||||||||||||||||||||||||||||||||||||||||||||#n\n\r",ch);
          }

	  //Decide whether or not they won.

          if(sa == 1 && sb != 1)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet/4;
          }
          else if(sa == 1 && sb == 1 && sc != 1)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet+2;
          }

          if(sd == 1 && se != 1)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet+4;
          }
          else if(sd == 1 && se == 1 && sf != 1)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet+2;
          }

          if(sg == 1 && sh != 1)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet+4;
          }
          else if(sg == 1 && sh == 1 && si != 1)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet+2;
          }

          if(sa == sb && sb == sc)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * sb;
          }

          if(sd == se && se == sf)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * se;
          }

          if(sg == sh && sh == si)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * sh;
          }

          if(sa == se && se == si)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * se;
          }

          if(sg == se && se == sc)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * se;
          }

          if(sa == sd && sd == sg)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * sd;
          }

          if(sb == se && se == sh)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * se;
          }

          if(sc == sf && sf == si)
          {
            if(winner == FALSE)
              winner = TRUE;
            winnings += basebet * sf;
          }

	  //This is the JACKPOT!  It gives a special message and is therefore
	  //separated from the regular winning check.  I have never hit the
	  //jackpot, so I don't really know if it works right. :-p

          if(sa == 10 && se == 10 && si == 10 && sg == 10 && sc == 10)
          {
            send_to_char("#RJACKPOT!#n",ch); //Potty word, if you're a pussy pg13 MUD you'll probably to change it.
            act("The red light on top of $n's slot machine illuminates and spins. #RJACKPOT!#n",ch,NULL,NULL,TO_ROOM);
            winnings += basebet * 50;
            winnings += number_range(0,500);
            if(winnings < 100)
            {
              send_to_char("#nYou hit the Jackpot of 130 pounds!#n\n\r",ch);
              ch->pounds += +130;
            }
            else if(winnings == 100)
            {
              send_to_char("#nYou hit the Jackpot of 200 pounds!#n\n\r",ch);
              ch->pounds += +200;
            }
            else if(winnings > 100)
            {
             send_to_char("#nYou hit the MEGA JACKPOT of 400 pounds!#n\n\r",ch);
              ch->pounds += +400;
            }
          }

	  //Regular winner.

          else if(winner == TRUE)
          {
            send_to_char("#yYeeehaawww! A winner!#n\n\r",ch);
            act("$n's slot machine flashes and makes loud noises.",ch,NULL,NULL,TO_ROOM);
            if(winnings < 100)
            {
              send_to_char("#nYou break even and get to keep your 25 pounds!#n\n\r",ch);
            }
            else if(winnings == 100)
            {
              send_to_char("#nYou double your bet and get 50 pounds!#n\n\r",ch);
              ch->pounds += +50;
            }
            else if(winnings > 100)
            {
              send_to_char("#nYou quadruple your bet and get 100 pounds!#n\n\r",ch);
              ch->pounds += +100;
            }
          }

	  //Loser. :(

          else
          {
            send_to_char("#RYou lose your 25 pounds.#n\n\r",ch); //Another potty word.
			ch->pounds -= 25;
          }
        return;
        }
        else
        {
          send_to_char("Slot commands#n: <#nPLAY#n/#nASCII#n>#n\n\rFor more information see #n'#nHELP SLOTS#n'#n.\n\r",ch);
          return;
        }
return;
}

// Allow the use of contraceptives

void do_use( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "#cUse what?#n\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "#cYou do not have that item.#n\n\r", ch );
	return;
    }

	else if ( obj->item_type != ITEM_CONDOM )
	{
		send_to_char( "#cThat's not a type of contraception.#n\n\r", ch );
		return;
	}

    act( "#c$n uses $p as a precaution against pregnancy.#n",  ch, obj, NULL, TO_ROOM );
    act( "#cYou use $p as a precaution against pregnancy.#n", ch, obj, NULL, TO_CHAR );

    SET_BIT(ch->affected_by, AFF_CONTRACEPTION);
    if (obj != NULL) extract_obj( obj );
    return;
}
