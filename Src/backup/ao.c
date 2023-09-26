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
		ch->gold += obj->value[0];
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
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
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
				if ((arg1[3] == '\0' || is_name (&arg1[4], obj->name)) && can_see_obj (ch, obj))
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

		if (ch->gold < amount)
		{
			send_to_char ("You haven't got that many coins.\n\r", ch);
			return;
		}

		ch->gold -= amount;

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
		act ("$n drops some gold.", ch, NULL, NULL, TO_ROOM);
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
			do_obfuscate1(ch,argument);
		if (IS_MORE3(ch, MORE3_OBFUS2))
			do_obfuscate2(ch,argument);
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
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
	
	if (is_number (arg1))
	{
		/* 'give NNNN coins victim' */
		int amount;

		amount = atoi (arg1);
		if (amount <= 0 || (str_cmp (arg2, "coins") && str_cmp (arg2, "coin")))
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

		if (ch->gold < amount)
		{
			send_to_char ("You haven't got that much gold.\n\r", ch);
			return;
		}

		ch->gold -= amount;
		victim->gold += amount;
		act ("$n gives you some gold.", ch, NULL, victim, TO_VICT);
		act ("$n gives $N some gold.", ch, NULL, victim, TO_NOTVICT);
		act ("You give $N some gold.", ch, NULL, victim, TO_CHAR);
		mprog_bribe_trigger (victim, ch, amount);	//mob programs
		send_to_char ("OK.\n\r", ch);
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

	if (CAN_WEAR (obj, ITEM_WIELD) || CAN_WEAR (obj, ITEM_HOLD) || CAN_WEAR (obj, ITEM_WEAR_SHIELD) || obj->item_type == ITEM_LIGHT)
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

	if (CAN_WEAR (obj, ITEM_WEAR_ARMS))
	{
		if (!remove_obj (ch, WEAR_ARMS, fReplace))
			return;
		if (!IS_NPC (ch) && !IS_FORM (ch, ITEM_WEAR_ARMS))
		{
			send_to_char ("You are unable to wear it.\n\r", ch);
			return;
		}
		if (!is_ok_to_wear (ch, wolf_ok, "arms"))
		{
			send_to_char ("You have no arms to wear them on.\n\r", ch);
			return;
		}
		act ("$n slides $s arms into $p.", ch, obj, NULL, TO_ROOM);
		act ("You slide your arms into $p.", ch, obj, NULL, TO_CHAR);
		equip_char (ch, obj, WEAR_ARMS);
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


	if (IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_NPC (ch) && ch->polyaff != 0 && !IS_VAMPAFF (ch, VAM_DISGUISED) && !IS_WEREWOLF (ch))
	{
		send_to_char ("You cannot wear anything in this form.\n\r", ch);
		return;
	}


	if (!IS_NPC (ch) && IS_WEREWOLF (ch) && ch->pcdata->wolfform[1] > FORM_GLABRO && ch->pcdata->wolfform[1] != FORM_CRINOS)
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
		do_obfuscate1(ch,argument);
		
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
		do_obfuscate1(ch,argument);
	
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
		send_to_char ("Sacrifice what?\n\r", ch);
		return;
	}

	obj = get_obj_list (ch, arg, ch->in_room->contents);
	if (obj == NULL)
	{
		send_to_char ("You can't find it.\n\r", ch);
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

	act ("$p disintegrates into a fine powder.", ch, obj, NULL, TO_CHAR);
	act ("$n drains the energy from $p.", ch, obj, NULL, TO_ROOM);
	act ("$p disintegrates into a fine powder.", ch, obj, NULL, TO_ROOM);
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
			act ("You already have $p in your right scabbard.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		if (obj->item_type == ITEM_WEAPON)
		{
			act ("You slide $p into your right scabbard.", ch, obj, NULL, TO_CHAR);
			act ("$n slides $p into $s right scabbard.", ch, obj, NULL, TO_ROOM);
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
			act ("You already have $p in your left scabbard.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		if (obj->item_type == ITEM_WEAPON)
		{
			act ("You slide $p into your left scabbard.", ch, obj, NULL, TO_CHAR);
			act ("$n slides $p into $s left scabbard.", ch, obj, NULL, TO_ROOM);
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
			send_to_char ("Your right scabbard is empty.\n\r", ch);
			return;
		}
		else if ((obj2 = get_eq_char (ch, WEAR_WIELD)) != NULL)
		{
			act ("You already have $p in your right hand.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		act ("You draw $p from your right scabbard.", ch, obj, NULL, TO_CHAR);
		act ("$n draws $p from $s right scabbard.", ch, obj, NULL, TO_ROOM);
	}
	else
	{
		scabbard = WEAR_SCABBARD_L;
		worn = WEAR_HOLD;
		if ((obj = get_eq_char (ch, scabbard)) == NULL)
		{
			send_to_char ("Your left scabbard is empty.\n\r", ch);
			return;
		}
		else if ((obj2 = get_eq_char (ch, WEAR_HOLD)) != NULL)
		{
			act ("You already have $p in your left hand.", ch, obj2, NULL, TO_CHAR);
			return;
		}
		act ("You draw $p from your left scabbard.", ch, obj, NULL, TO_CHAR);
		act ("$n draws $p from $s left scabbard.", ch, obj, NULL, TO_ROOM);
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
		strcpy (buf, "golden");
		break;
	case 1:
		strcpy (buf, "golden");
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

	for (disc = DISC_ANIMALISM; disc < DISC_MAX; disc++)
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
			if (paf->location >= APPLY_SET_ANIMALISM && paf->location <= APPLY_SET_VICISSITUDE)
				affect_modify (ch, paf, TRUE);
		}
		for (paf = obj->affected; paf != NULL; paf = paf->next)
		{
			if (paf->location >= APPLY_SET_ANIMALISM && paf->location <= APPLY_SET_VICISSITUDE)
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
		for (disc = DISC_ANIMALISM; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] += 20;
			ch->pcdata->powers_set[disc] += 20;
		}
		if (IS_POLYAFF (ch, POLY_BONECRAFT) && get_truedisc (ch, DISC_VICISSITUDE) < 23)
			do_bonecraft (ch, "self");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES) && get_truedisc (ch, DISC_AUSPEX) < 21)
			do_truesight (ch, "");
		if (IS_SET (ch->act, PLR_HOLYLIGHT) && get_truedisc (ch, DISC_AUSPEX) < 21)
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_OBEAH) && get_truedisc (ch, DISC_OBEAH) == 20)
			ch->pcdata->obeah = 0;
		for (disc = DISC_ANIMALISM; disc < DISC_MAX; disc++)
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
	if (IS_VAMPIRE (ch) || IS_GHOUL (ch) || IS_INQUISITOR(ch))
	{
		for (disc = DISC_ANIMALISM; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] += 20;
			ch->pcdata->powers_set[disc] += 20;
		}
		if (IS_POLYAFF (ch, POLY_BONECRAFT))
			do_bonecraft (ch, "self");
		if (IS_SET (ch->act, PLR_HOLYLIGHT))
			do_truesight (ch, "");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_OBEAH))
			ch->pcdata->obeah = 0;

		for (disc = DISC_ANIMALISM; disc < DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] -= 20;
			ch->pcdata->powers_set[disc] -= 20;
		}
	}
	return;
}
