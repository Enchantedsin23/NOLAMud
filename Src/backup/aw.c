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
#include <unistd.h>
#include <stdarg.h>
#include "merc.h"
#include <pthread.h>

#define END_MARKER	"END"	/* end file marker for banlist */


/*
 * Local functions.
 */
void call_all args ((CHAR_DATA * ch));
bool write_to_descriptor args ((int desc, char *txt, int length));
void init_descriptor args ((DESCRIPTOR_DATA * dnew, int desc));
/* Merits/Flaws Stuff */
bool has_merit args ((CHAR_DATA * ch, char * merit));
bool has_flaw args ((CHAR_DATA * ch, char * flaw));
int find_merit args ((CHAR_DATA * ch, char * merit_name));
int find_flaw args ((CHAR_DATA * ch, char * flaw_name));
/* End Merits/Flaws Stuff */
char *const eq_table[] = {
	"light", "finger_l", "finger_r", "neck_1", "neck_2", "torso", "head",
	"legs",
	"feet", "hands", "arms", "shield", "body", "waist", "wrist_l",
	"wrist_r",
	"wield", "hold", "float", NULL
};

char *const pos_table[] = {
	"dead", "mortal", "incap", "stun", "sleep", "rest", "sit", "fight",
	"stand",
	NULL
};


char *const imm_title_table[] = {
	"#GImplementer", "#CAdministrator", "#cAdministrator", 
        "#bStoryteller", "#BStoryteller","#rStoryteller", "#RStoryteller"
};


void do_wizhelp (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	int cmd;
	int col;
	int i;
	int j = 0;


	for (i = LEVEL_IMPLEMENTOR; i >= LEVEL_STORYTELLER; i--)
	{

		col = 0;
		if (get_trust (ch) >= i)
		{
			sprintf (buf, "<--------------------------- #R%s#n ------------------------------>\n\r", imm_title_table[j]);
			send_to_char (buf, ch);
			for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
			{
				if (cmd_table[cmd].level == i && cmd_table[cmd].level <= get_trust (ch))
				{
					sprintf (buf, "%-15s", cmd_table[cmd].name);
					send_to_char (buf, ch);
					if (++col % 5 == 0)
						send_to_char ("\n\r", ch);
				}
			}
			if (col % 5 != 0)
				send_to_char ("\n\r", ch);
		}
		j++;
	}
	return;
}



void do_bamfin (CHAR_DATA * ch, char *argument)
{
	if (!IS_NPC (ch))
	{
		smash_tilde (argument);
		free_string (ch->pcdata->bamfin);
		ch->pcdata->bamfin = str_dup (argument);
		send_to_char ("Ok.\n\r", ch);
	}
	return;
}



void do_bamfout (CHAR_DATA * ch, char *argument)
{
	if (!IS_NPC (ch))
	{
		smash_tilde (argument);
		free_string (ch->pcdata->bamfout);
		ch->pcdata->bamfout = str_dup (argument);
		send_to_char ("Ok.\n\r", ch);
	}
	return;
}

	
void do_nice (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Do you wish to switch it ON or OFF?\n\r", ch);
		return;
	}

	if (IS_SET (world_affects, WORLD_NICE) && !str_cmp (arg, "off"))
	{
		REMOVE_BIT (world_affects, WORLD_NICE);
		send_to_char ("Nice mode OFF.\n\r", ch);
	}
	else if (!IS_SET (world_affects, WORLD_NICE) && !str_cmp (arg, "off"))
		send_to_char ("Nice mode already off.\n\r", ch);
	else if (!IS_SET (world_affects, WORLD_NICE) && !str_cmp (arg, "on"))
	{
		SET_BIT (world_affects, WORLD_NICE);
		send_to_char ("Nice mode ON.\n\r", ch);
	}
	else if (IS_SET (world_affects, WORLD_NICE) && !str_cmp (arg, "on"))
		send_to_char ("Nice mode already on.\n\r", ch);
	else
		send_to_char ("Do you wish to switch it ON or OFF?\n\r", ch);
	return;
}


void do_string (CHAR_DATA * ch, char *argument)
{
	char old_name[MAX_INPUT_LENGTH], new_name[MAX_INPUT_LENGTH], strsave[MAX_INPUT_LENGTH];

	CHAR_DATA *victim;
	FILE *file;

	argument = one_argument (argument, old_name);
	one_argument (argument, new_name);

	if (!old_name[0])
	{
		send_to_char ("Rename who?\n\r", ch);
		return;
	}

	victim = get_char_world (ch, old_name);

	if (!victim)
	{
		send_to_char ("There is no such a person online.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("You cannot use Rename on NPCs.\n\r", ch);
		return;
	}


	if ((victim != ch) && (get_trust (victim) >= get_trust (ch)))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if (!victim->desc || (victim->desc->connected != CON_PLAYING))
	{
		send_to_char ("This player has lost his link or is inside a pager or the like. \n\r", ch);
		return;
	}
	if (!new_name[0])
	{
		send_to_char ("Rename to what new name?\n\r", ch);
		return;
	}

	if (!check_parse_name (new_name))
	{
		send_to_char ("The new name is illegal.\n\r", ch);
		return;
	}

	sprintf (strsave, "%s%s%s", PLAYER_DIR, "/", capitalize (new_name));
	fclose (fpReserve);
	file = fopen (strsave, "r");
	if (file)
	{
		send_to_char ("A player with that name already exists!\n\r", ch);
		fclose (file);
		fpReserve = fopen (NULL_FILE, "r");	/* is this really necessary 
							 * these days? */
		return;
	}
	fpReserve = fopen (NULL_FILE, "r");
	if (get_char_world (ch, new_name))
	{
		send_to_char ("A player with the name you specified already exists !\n\r", ch);
		return;
	}
	sprintf (strsave, "%s%s%s", PLAYER_DIR, "/", capitalize (victim->name));
	free_string (victim->name);
	victim->name = str_dup (capitalize (new_name));
	save_char_obj (victim);
	unlink (strsave);
	send_to_char ("Character renamed.\n\r", ch);
	act ("$n has renamed you to $N!", ch, NULL, victim, TO_VICT);
}

void do_noquit (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	argument = one_argument (argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char ("Syntax: noquit <player>\n\r",ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if ( IS_SET( victim->act, ACT_NOQUIT ) )
	{
		REMOVE_BIT( victim->act, ACT_NOQUIT );
		sprintf( buf, "%s can quit again.", victim->name);
		send_to_char( buf, ch );
		send_to_char( "You can quit once more.\n\r", victim);
	}
	else
	{
		SET_BIT( victim->act, ACT_NOQUIT );
		send_to_char( "You can no longer quit.\n\r", victim);
		sprintf( buf, "%s can no longer quit.", victim->name);
		send_to_char( buf, ch );
	}

	return;
}

void do_deny (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char *strtime;
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	int release_date;

	sprintf (buf, "%s: Deny %s", ch->name, argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: Deny <player> <number of days>\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!is_number (arg2) || (release_date = atoi (arg2)) < 1)
	{
		send_to_char ("Please enter a valid number of days.\n\r", ch);
		return;
	}

	if (release_date > 365)
	{
		send_to_char ("Geez...why don't you just delete them instead?\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	strtime = ctime (&current_time);
	strtime[strlen (strtime) - 1] = '\0';
	sprintf (buf, "You have been denied access for %d days.\n\r", release_date);
	send_to_char (buf, victim);
	send_to_char ("OK.\n\r", ch);
	release_date += date_to_int (strtime);
	victim->pcdata->denydate = release_date;
	do_quit (victim, "");
	return;
}



void do_disconnect (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	int disco_num = 0;

	sprintf (buf, "%s: Disconnect %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Disconnect whom?\n\r", ch);
		return;
	}

	if (arg[0] >= 49 && arg[0] <= 57)
	{
		disco_num = atoi (arg);
	}
	else
	{
		if ((victim = get_char_world (ch, arg)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}

		if (victim->desc == NULL)
		{
			act ("$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR);
			return;
		}
	}

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (disco_num != 0)
		{
			if (d->descriptor == disco_num)
			{

				close_socket (d);
				send_to_char ("Ok.\n\r", ch);
				return;
			}
		}
		else
		{
			if (d == victim->desc)
			{



				close_socket (d);
				send_to_char ("Ok.\n\r", ch);
				return;
			}
		}
	}

	bug ("Do_disconnect: desc not found.", 0);
	send_to_char ("Descriptor not found!\n\r", ch);
	return;
}


void do_info (CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	if (argument[0] == '\0')
		return;
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING && !IS_SET (d->character->deaf, CHANNEL_INFO))
		{
			send_to_char ("#eThe wind whispers: ", d->character);
			send_to_char (argument, d->character);
			send_to_char ("\n\r", d->character);
		}
	}

	return;
}

void do_radio (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	DESCRIPTOR_DATA *d;
	if (argument[0] == '\0')
		return;
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING && !IS_SET (d->character->deaf, CHANNEL_SHOUT))
		{
			sprintf(buf, "#G(Radio) WHDR, 93.1 FM: #g%s#n\n\r", argument);
			send_to_char (buf, d->character);
		}
	}

	return;
}

void sysmes (char *argument)
{
	DESCRIPTOR_DATA *d;
	if (argument[0] == '\0')
	{
		return;
	}

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING)
		{
			send_to_char ("[SYSTEM: ", d->character);
			send_to_char (argument, d->character);
			send_to_char ("]\n\r", d->character);
		}
	}

	return;
}


void logchan (char *argument, int bit)
{
	CHAR_DATA *ch;
	DESCRIPTOR_DATA *d;
	if (argument[0] == '\0')
	{
		return;
	}

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if ((ch = d->character) == NULL)
			continue;
		if ((d->connected == CON_PLAYING) &&
		!IS_SET (ch->deaf, bit) && !IS_NPC(ch))
		{
			if (bit == CHANNEL_LOG_CODER)
			{
				if (IS_IMP(ch))
				{
				send_to_char("#P(#pImp#P:#w ", ch);
				send_to_char(argument, ch);
				send_to_char("#P)#n\n\r",ch);
				}
			}
 			else if (((bit == CHANNEL_LOG_STORYTELLER) && IS_STORYTELLER(ch)) ||
			((bit == CHANNEL_LOG_DICE) && IS_IMMORTAL(ch)) ||
			IS_IMP(ch) || (ch->level >= LEVEL_BUILDER))
			{
				if (bit != CHANNEL_LOG_DICE)
 				  send_to_char ("#C[#cLog#C:#w ", ch);
				send_to_char (argument, ch);
				if (bit != CHANNEL_LOG_DICE)
				  send_to_char ("#C]\n\r", ch);
			}
		}
	}

	return;
}

ROOM_INDEX_DATA *find_location (CHAR_DATA * ch, char *arg)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	if (is_number (arg) && atoi (arg) != 30008)
		return get_room_index (atoi (arg));
	if ((victim = get_char_world (ch, arg)) != NULL)
		return victim->in_room;
	if ((obj = get_obj_world (ch, arg)) != NULL && obj->in_room != NULL)
		return obj->in_room;
	if (obj != NULL && obj->carried_by != NULL && obj->carried_by->in_room != NULL)
		return obj->carried_by->in_room;
	if (obj != NULL && obj->in_obj != NULL && obj->in_obj->in_room != NULL)
		return obj->in_obj->in_room;
	if (obj != NULL && obj->in_obj != NULL && obj->in_obj->carried_by && obj->in_obj->carried_by->in_room != NULL)
		return obj->in_obj->carried_by->in_room;
	return NULL;
}



void do_transfer (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	CHAR_DATA *mount;
	sprintf (buf, "%s: Transfer %s", ch->name, argument);

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0')
	{
		send_to_char ("Transfer whom (and where)?\n\r", ch);
		return;
	}

	if (!str_cmp (arg1, "all"))
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && d->character != ch && d->character->in_room != NULL && can_see (ch, d->character))
			{
				char buf[MAX_STRING_LENGTH];
				sprintf (buf, "%s %s", d->character->name, arg2);
				do_transfer (ch, buf);
			}
		}
		return;
	}

	/*
	 * Thanks to Grodyn for the optional location parameter.
	 */
	if (arg2[0] == '\0')
	{
		location = ch->in_room;
	}
	else
	{
		if ((location = find_location (ch, arg2)) == NULL)
		{
			send_to_char ("No such location.\n\r", ch);
			return;
		}

		if (room_is_private (location))
		{
			send_to_char ("That room is private right now.\n\r", ch);
			return;
		}
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (victim->in_room == NULL)
	{
		send_to_char ("They are in limbo.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		if (!get_trust (ch) == 12)
		{
			send_to_char ("Do it yourself!\n\r", ch);
			return;
		}
	}

	if (IS_SET(victim->act,PLR_NOTRANS))
	  return;

	act ("$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM);
	char_from_room (victim);
	char_to_room (victim, location);
	act ("$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM);
	if (ch != victim)
		act ("$n has transferred you.", ch, NULL, victim, TO_VICT);
	do_look (victim, "auto");
	if ((mount = victim->mount) == NULL)
		return;
	char_from_room (mount);
	char_to_room (mount, get_room_index (victim->in_room->vnum));
	if (ch != mount)
		act ("$n has transferred you.", ch, NULL, mount, TO_VICT);
	do_look (mount, "auto");
}



void do_at (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	CHAR_DATA *wch;
	sprintf (buf, "%s: At %s", ch->name, argument);
	argument = one_argument (argument, arg);
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("At where what?\n\r", ch);
		return;
	}

	if ((location = find_location (ch, arg)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}

	if (room_is_private (location))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}

	original = ch->in_room;
	char_from_room (ch);
	char_to_room (ch, location);
	interpret (ch, argument);
	/*
	 * See if 'ch' still exists before continuing!
	 * Handles 'at XXXX quit' case.
	 */
	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (wch == ch)
		{
			char_from_room (ch);
			char_to_room (ch, original);
			break;
		}
	}

	return;
}


void do_wgoto (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *mount;
	CHAR_DATA *victim;
	int xx = -1;		//wilderness
	int yy = -1;
	int zz = 0;
	one_argument (argument, arg);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	if (arg[0] == '\0')
	{
		send_to_char ("Goto where?\n\r", ch);
		return;
	}

	if (arg2[0] != '\0' && is_number (arg) && is_number (arg2))
	{
		xx = atoi (arg);
		yy = atoi (arg2);
		zz = 0;
		if (xx < 0 || xx > 999)
		{
			send_to_char ("Goto 'x' coordinate must be in the range 0-999.\n\r", ch);
			return;
		}
		if (yy < 0 || yy > 999)
		{
			send_to_char ("Goto 'y' coordinate must be in the range 0-999.\n\r", ch);
			return;
		}
		if ((location = find_location (ch, arg3)) == NULL)
		{
			send_to_char ("Nothing happens.\n\r", ch);
			return;
		}
		ch->x = victim->x;
		ch->y = victim->y;
		ch->room = 0;
	}
	else if ((location = find_location (ch, arg3)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}

	if (!IS_SET (ch->act, PLR_WIZINVIS))
	{
		act ("$T.", ch, NULL, (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0') ? ch->pcdata->bamfout : "leaves in a swirling mist", TO_ROOM);
	}

	char_from_room (ch);
	char_to_room (ch, location);
	if (ch->in_room->vnum >= 20000 && ch->in_room->vnum < 21000)
	{
		for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
		{
			if (ch == victim)
				continue;
			ch->x = victim->x;
			ch->y = victim->y;

			break;
		}
	}

	if (!IS_SET (ch->act, PLR_WIZINVIS))
	{
		act ("$T.", ch, NULL, (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0') ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM);
	}

	do_look (ch, "auto");
	if ((mount = ch->mount) == NULL)
		return;
	char_from_room (mount);
	char_to_room (mount, ch->in_room);
	do_look (mount, "auto");
	return;
}



void do_goto (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *mount;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Goto where?\n\r", ch);
		return;
	}

	if ((location = find_location (ch, arg)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}

	if (room_is_private (location) && get_trust (ch) < LEVEL_IMPLEMENTOR)
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}

	if (!IS_SET (ch->act, PLR_WIZINVIS))
	{
		act ("$T.", ch, NULL, (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0') ? ch->pcdata->bamfout : "leaves in a swirling mist", TO_ROOM);
	}

	char_from_room (ch);
	char_to_room (ch, location);
	if (!IS_SET (ch->act, PLR_WIZINVIS))
	{
		act ("$T.", ch, NULL, (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0') ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM);
	}

	do_look (ch, "auto");
	if ((mount = ch->mount) == NULL)
		return;
	char_from_room (mount);
	char_to_room (mount, ch->in_room);
	do_look (mount, "auto");
	return;
}



void do_rstat (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	OBJ_DATA *obj;
	CHAR_DATA *rch;
	int door;
	one_argument (argument, arg);
	location = (arg[0] == '\0') ? ch->in_room : find_location (ch, arg);
	if (location == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}

	if (ch->in_room != location && room_is_private (location))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}


	sprintf (buf, "X: %d, Y: %d\n\r", ch->x, ch->y);
	send_to_char (buf, ch);
	sprintf (buf, "Name: '%s.'\n\rArea: '%s'.\n\r", location->name, location->area->name);
	send_to_char (buf, ch);
	sprintf (buf, "Vnum: %d.  Sector: %d.  Light: %d.\n\r", location->vnum, location->sector_type, location->light);
	send_to_char (buf, ch);
	sprintf (buf, "Room flags: %d.\n\rDescription:\n\r%s", location->room_flags, location->description);
	send_to_char (buf, ch);
	if (location->extra_descr != NULL)
	{
		EXTRA_DESCR_DATA *ed;
		send_to_char ("Extra description keywords: '", ch);
		for (ed = location->extra_descr; ed; ed = ed->next)
		{
			send_to_char (ed->keyword, ch);
			if (ed->next != NULL)
				send_to_char (" ", ch);
		}
		send_to_char ("'.\n\r", ch);
	}

	send_to_char ("Characters:", ch);
	for (rch = location->people; rch; rch = rch->next_in_room)
	{
		send_to_char (" ", ch);
		if (get_trust (ch) >= get_trust (rch))
		  one_argument (rch->name, buf);
		else
		  sprintf( buf, " " );
		send_to_char (buf, ch);
	}

	send_to_char (".\n\rObjects:   ", ch);
	for (obj = location->contents; obj; obj = obj->next_content)
	{
		send_to_char (" ", ch);
		one_argument (obj->name, buf);
		send_to_char (buf, ch);
	}
	send_to_char (".\n\r", ch);
	for (door = 0; door <= 5; door++)
	{
		EXIT_DATA *pexit;
		if ((pexit = location->exit[door]) != NULL)
		{
			sprintf (buf, "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s", door, pexit->to_room != NULL ? pexit->to_room->vnum : 0, pexit->key, pexit->exit_info, pexit->keyword, pexit->description[0] != '\0' ? pexit->description : "(none).\n\r");
			send_to_char (buf, ch);
		}
	}

	return;
}



void do_ostat (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char nm1[40];
	char nm2[40];
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Ostat what?\n\r", ch);
		return;
	}

	if ((obj = get_obj_world (ch, arg)) == NULL)
	{
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
		return;
	}

	if (obj->questmaker != NULL && strlen (obj->questmaker) > 1)
		sprintf (nm1, obj->questmaker);
	else
		sprintf (nm1, "None");
	if (obj->questowner != NULL && strlen (obj->questowner) > 1)
		sprintf (nm2, obj->questowner);
	else
		sprintf (nm2, "None");
	sprintf (buf, "Name: %s.\n\r", obj->name);
	send_to_char (buf, ch);
	sprintf (buf, "Vnum: %d.  Type: %s.\n\r", obj->pIndexData->vnum, item_type_name (obj->item_type));
	send_to_char (buf, ch);
	sprintf (buf, "Short description: %s.\n\rLong description: %s\n\r", obj->short_descr, obj->description);
	send_to_char (buf, ch);
	sprintf (buf, "Object creator: %s.  Object owner: %s.  Quest points: %d.\n\r", nm1, nm2, obj->points);
	send_to_char (buf, ch);
	sprintf (buf, "Wear bits: %d.  Extra bits: %s.\n\r", obj->wear_flags, extra_bit_name (obj->extra_flags));
	send_to_char (buf, ch);
	sprintf (buf, "Weight: %d/%d.\n\r", obj->weight, get_obj_weight (obj));
	send_to_char (buf, ch);
	sprintf (buf, "Cost: %d.  Timer: %d.  Level: %d.\n\r", obj->cost, obj->timer, obj->level);
	send_to_char (buf, ch);
	sprintf (buf, "In room: %d.  In object: %s.  Carried by: %s.  Wear_loc: %d.\n\r", obj->in_room == NULL ? 0 : obj->in_room->vnum, obj->in_obj == NULL ? "(none)" : obj->in_obj->short_descr, obj->carried_by == NULL ? "(none)" : obj->carried_by->name, obj->wear_loc);
	send_to_char (buf, ch);
	sprintf (buf, "Values: %d %d %d %d.\n\r", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
	send_to_char (buf, ch);
	if (obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL)
	{
		EXTRA_DESCR_DATA *ed;
		send_to_char ("Extra description keywords: '", ch);
		for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
		{
			send_to_char (ed->keyword, ch);
			if (ed->next != NULL)
				send_to_char (" ", ch);
		}

		for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next)
		{
			send_to_char (ed->keyword, ch);
			if (ed->next != NULL)
				send_to_char (" ", ch);
		}

		send_to_char ("'.\n\r", ch);
	}

	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		sprintf (buf, "%s %d.\n\r", affect_loc_name (paf->location), paf->modifier);
		send_to_char (buf, ch);
	}

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
	{
		sprintf (buf, "%s %d.\n\r", affect_loc_name (paf->location), paf->modifier);
		send_to_char (buf, ch);
	}

	return;
}



void do_mstat (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	AFFECT_DATA *paf;
	CHAR_DATA *victim;
        int played_hours = 0;

	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Mstat whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	sprintf (buf, "Name: %s.\n\r", victim->name);
	send_to_char (buf, ch);
	sprintf (buf, "Plane: %d.\n\r", victim->plane);
	send_to_char (buf, ch);
	sprintf (buf, "Vnum: %d.  Sex: %s.  Room: %d.\n\r", IS_NPC (victim) ? victim->pIndexData->vnum : 0, victim->sex == SEX_MALE ? "male" : victim->sex == SEX_FEMALE ? "female" : "neutral", victim->in_room == NULL ? 0 : victim->in_room->vnum);
	send_to_char (buf, ch);
	sprintf (buf, "Hp: %d/%d.\n\r", victim->hit, victim->max_hit );
	send_to_char (buf, ch);
	sprintf (buf, "Lv: %d.  Trst: %d.  Security: %d.    Exp: %d.\n\r", victim->level, victim->trust, IS_NPC (victim) ? 0 : GET_PC (victim, security, 0),  victim->exp);
	send_to_char (buf, ch);
	sprintf (buf, "Position: %d.  Wimpy: %d.\n\r", victim->position, victim->wimpy);
	send_to_char (buf, ch);
	sprintf (buf, "Fighting: %s.\n\r", victim->fighting ? victim->fighting->name : "(none)");
	send_to_char (buf, ch);
	if (!IS_NPC (victim))
	{
		sprintf (buf, "Learn: %d.  Teach: %d.  Blood: %d/%d.\n\r", victim->pcdata->learn, victim->pcdata->teach, victim->blood[0], victim->blood[1]);
		send_to_char (buf, ch);
		if (IS_VAMPIRE (victim) || IS_WEREWOLF (victim))
		{
			if (strlen (victim->clan) > 1)
				sprintf (buf, "Clan: %s.  ", victim->clan);
			else
				sprintf (buf, "Clan:  None. ");
			send_to_char (buf, ch);
			sprintf (buf, "Rage: %d.  ", victim->pcdata->wolf);
			send_to_char (buf, ch);
			if (IS_VAMPIRE (victim))
			{
				sprintf (buf, "Beast:  %d. ", victim->beast);
				send_to_char (buf, ch);
				sprintf (buf, "Generation:  %d.", victim->vampgen);
				send_to_char (buf, ch);
			}
			send_to_char ("\n\r", ch);
		}

		if (IS_SET (victim->act, PLR_CHAMPION))
		{
			if (strlen (victim->lord) > 1)
				sprintf (buf, "Lord: %s. ", victim->lord);
			else
				sprintf (buf, "Lord: None. ");
			send_to_char (buf, ch);

			sprintf (buf, "Demonic armor: %d pieces. ", victim->pcdata->demonic);
			send_to_char (buf, ch);
			sprintf (buf, "Power: %d (%d).", victim->pcdata->power[0], victim->pcdata->power[1]);
			send_to_char (buf, ch);
			send_to_char ("\n\r", ch);
		}
	}

	played_hours = (get_age (victim) - 17) * 2;
	sprintf (buf, "Carry number: %d.  Carry weight: %d.\n\r", victim->carry_number, victim->carry_weight);
	send_to_char (buf, ch);
	sprintf (buf, "Age: %d.  Played: %d.  Timer: %d.  Act: %d.\n\r", get_age (victim), played_hours, victim->timer, victim->act);
	send_to_char (buf, ch);
	sprintf (buf, "Master: %s.  Leader: %s.  Affected by: %s.\n\r", victim->master ? victim->master->name : "(none)", victim->leader ? victim->leader->name : "(none)", affect_bit_name (victim->affected_by));
	send_to_char (buf, ch);
	sprintf (buf, "Short description: %s.\n\rLong  description: %s", victim->short_descr[0] != '\0' ? victim->short_descr : "(none)", victim->long_descr[0] != '\0' ? victim->long_descr : "(none).\n\r");
	send_to_char (buf, ch);
	if (IS_NPC (victim) && victim->spec_fun != 0)
		send_to_char ("Mobile has spec fun.\n\r", ch);
	for (paf = victim->affected; paf != NULL; paf = paf->next)
	{
		sprintf (buf, "Spell: '%s' modifies %s by %d for %d hours with bits %s.\n\r", skill_table[(int) paf->type].name, affect_loc_name (paf->location), paf->modifier, paf->duration, affect_bit_name (paf->bitvector));
		send_to_char (buf, ch);
	}

	return;
}



void do_mfind (CHAR_DATA * ch, char *argument)
{
	extern int top_mob_index;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	MOB_INDEX_DATA *pMobIndex;
	int vnum;
	int nMatch;
	bool fAll;
	bool found;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Mfind whom?\n\r", ch);
		return;
	}

	fAll = !str_cmp (arg, "all");
	found = FALSE;
	nMatch = 0;
	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_mob_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	for (vnum = 0; nMatch < top_mob_index; vnum++)
	{
		if ((pMobIndex = get_mob_index (vnum)) != NULL)
		{
			nMatch++;
			if (fAll || is_name (arg, pMobIndex->player_name))
			{
				found = TRUE;
				sprintf (buf, "[%5d] %s\n\r", pMobIndex->vnum, capitalize (pMobIndex->short_descr));
				send_to_char (buf, ch);
			}
		}
	}

	if (!found)
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
}



void do_ofind (CHAR_DATA * ch, char *argument)
{
	extern int top_obj_index;
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	OBJ_INDEX_DATA *pObjIndex;
	int vnum;
	int nMatch;
	bool fAll;
	bool found;
	one_argument (argument, arg);
	if (!IS_IMMORTAL (ch) && !IS_SHOPKEEPER (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Ofind what?\n\r", ch);
		return;
	}

	fAll = !str_cmp (arg, "all");
	found = FALSE;
	nMatch = 0;
	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_obj_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	for (vnum = 0; nMatch < top_obj_index; vnum++)
	{
		if ((pObjIndex = get_obj_index (vnum)) != NULL)
		{
			nMatch++;
			if (fAll || is_name (arg, pObjIndex->name))
			{
				found = TRUE;
				sprintf (buf, "[%5d] %s\n\r", pObjIndex->vnum, capitalize (pObjIndex->short_descr));
				send_to_char (buf, ch);
			}
		}
	}

	if (!found)
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
	return;
}



void do_mwhere (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	bool found;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Mwhere whom?\n\r", ch);
		return;
	}

	found = FALSE;
	for (victim = char_list; victim != NULL; victim = victim->next)
	{
		if (IS_NPC (victim) && victim->in_room != NULL && is_name (arg, victim->name))
		{
			found = TRUE;
			sprintf (buf, "[%5d] %-28s [%5d] %s\n\r", victim->pIndexData->vnum, victim->short_descr, victim->in_room->vnum, victim->in_room->name);
			send_to_char (buf, ch);
		}
	}

	if (!found)
		act ("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
	return;
}



void do_reboo (CHAR_DATA * ch, char *argument)
{
	send_to_char ("If you want to REBOOT, spell it out.\n\r", ch);
	return;
}



void do_reboot (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	extern int bootcount;
	int boottime;
	if (argument[0] == '\0')
	{
		char buf[MAX_STRING_LENGTH];
		extern bool merc_down;
		sprintf (buf, "world Reboot by %s.", ch->name);
		do_echo (ch, buf);
		do_forceauto (ch, "save");
		do_autosave (ch, "");
		merc_down = TRUE;
		return;
	}
	if (!str_cmp (argument, "stop"))
	{
		strcpy (buf, "Reboot aborted");
		sysmes (buf);
		bootcount = 0;
		return;
	}
	if (!is_number (argument))
	{
		send_to_char ("Syntax: Reboot <STOP/number of seconds>\n\r", ch);
		return;
	}
	if ((boottime = atoi (argument)) < 1 || boottime > 600)
	{
		send_to_char ("Please enter between 1 and 600 seconds.\n\r", ch);
		return;
	}
	bootcount = ++boottime;
	return;
}



void do_shutdow (CHAR_DATA * ch, char *argument)
{
	send_to_char ("If you want to SHUTDOWN, spell it out.\n\r", ch);
	return;
}



void do_shutdown (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	extern bool merc_down;
	sprintf (buf, "world Shutdown by %s.", ch->name);
	append_file (ch, SHUTDOWN_FILE, buf);
	strcat (buf, "\n\r");
	do_echo (ch, buf);
	do_forceauto (ch, "save");
	do_autosave (ch, "");
	merc_down = TRUE;
	return;
}



void do_snoop (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	sprintf (buf, "%s: Snoop %s", ch->name, argument);
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Snoop whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (victim->desc == NULL)
	{
		send_to_char ("No descriptor to snoop.\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char ("Cancelling all snoops.\n\r", ch);
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->snoop_by == ch->desc)
			{
				send_to_char("#RYou are no longer being snooped.#n\n\r", d->character);
				d->snoop_by = NULL;
			}
		}
		return;
	}

	if (victim->desc->snoop_by != NULL)
	{
		send_to_char ("Busy already.\n\r", ch);
		return;
	}

	if (IS_SET(victim->act, PLR_SNOOP_PROOF))
	{
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if (ch->desc != NULL)
	{
		for (d = ch->desc->snoop_by; d != NULL; d = d->snoop_by)
		{
			if (d->character == victim || d->original == victim)
			{
				send_to_char ("No snoop loops.\n\r", ch);
				return;
			}
		}
	}
	send_to_char("#RYou are now being snooped by a Storyteller.#n\n\r", victim);
	victim->desc->snoop_by = ch->desc;
	send_to_char ("Ok.\n\r", ch);
	return;
}

void do_oswitch (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	sprintf (buf, "%s: Oswitch %s", ch->name, argument);
	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Switch into what?\n\r", ch);
		return;
	}

	if (IS_EXTRA (ch, EXTRA_OSWITCH))
	{
		send_to_char ("You are already oswitched.\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, AFF_POLYMORPH))
	{
		send_to_char ("Not while polymorphed.\n\r", ch);
		return;
	}

	if (IS_NPC (ch) || IS_EXTRA (ch, EXTRA_SWITCH))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	if ((obj = get_obj_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (obj->chobj != NULL)
	{
		send_to_char ("Object in use.\n\r", ch);
		return;
	}

	obj->chobj = ch;
	ch->pcdata->chobj = obj;
	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->extra, EXTRA_OSWITCH);
	free_string (ch->morph);
	ch->morph = str_dup (obj->short_descr);
	send_to_char ("Ok.\n\r", ch);
	return;
}



void do_oreturn (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	sprintf (buf, "%s: Oreturn", ch->name);
	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (!IS_EXTRA (ch, EXTRA_OSWITCH))
	{
		send_to_char ("You are not oswitched.\n\r", ch);
		return;
	}
	if ((obj = ch->pcdata->chobj) != NULL)
		obj->chobj = NULL;
	ch->pcdata->chobj = NULL;
	REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT (ch->extra, EXTRA_OSWITCH);
	free_string (ch->morph);
	ch->morph = str_dup ("");
	char_from_room (ch);
	char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
	send_to_char ("Ok.\n\r", ch);
	return;
}



void do_switch (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	sprintf (buf, "%s: Switch %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Switch into whom?\n\r", ch);
		return;
	}

	if (ch->desc == NULL)
		return;
	if (ch->desc->original != NULL)
	{
		send_to_char ("You are already switched.\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (victim->desc != NULL)
	{
		send_to_char ("Character in use.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim))
	{
		send_to_char ("Only on NPC's.\n\r", ch);
		return;
	}

	SET_BIT (ch->extra, EXTRA_SWITCH);
	ch->desc->character = victim;
	ch->desc->original = ch;
	victim->desc = ch->desc;
	ch->desc = NULL;
	send_to_char ("Ok.\n\r", victim);
	return;
}



void do_return (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	sprintf (buf, "%s: Return %s", ch->name, argument);

	if (ch->desc == NULL)
		return;
	if (ch->desc->original == NULL)
	{
		send_to_char ("You aren't switched.\n\r", ch);
		return;
	}

	send_to_char ("You return to your original body.\n\r", ch);
	REMOVE_BIT (ch->desc->original->extra, EXTRA_SWITCH);
	ch->desc->character = ch->desc->original;
	ch->desc->original = NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc = NULL;
	return;
}



void do_mload (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA *victim;

	sprintf (buf, "%s: Mload %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0' || !is_number (arg))
	{
		send_to_char ("Syntax: mload <vnum>.\n\r", ch);
		return;
	}

	if ((pMobIndex = get_mob_index (atoi (arg))) == NULL)
	{
		send_to_char ("No mob has that vnum.\n\r", ch);
		return;
	}

	victim = create_mobile (pMobIndex);
	char_to_room (victim, ch->in_room);
	act ("$n has created $N!", ch, NULL, victim, TO_ROOM);
	act ("You have created $N!", ch, NULL, victim, TO_CHAR);
	return;
}



void do_pload (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *in_room;
	bool fOld;
	if (IS_NPC (ch) || ch->desc == NULL || ch->in_room == NULL)
		return;
	if (ch->level < 12 && !ch->ploaded)
	{
		send_to_char("#cHuh??\n\r", ch);
		return;
	}
	if (argument[0] == '\0')
	{
		send_to_char ("Syntax: pload <name>.\n\r", ch);
		return;
	}

	if (!check_parse_name (argument))
	{
		send_to_char ("Thats an illegal name.\n\r", ch);
		return;
	}

	if (!char_exists (FILE_PLAYER, argument))
	{
		send_to_char ("That player doesn't exist.\n\r", ch);
		return;
	}

	sprintf (buf, "%s: Pload %s", ch->name, argument);
	argument[0] = UPPER (argument[0]);
	sprintf (buf, "You transform into %s.\n\r", argument);
	send_to_char (buf, ch);
	sprintf (buf, "$n transforms into %s.", argument);
	act (buf, ch, NULL, NULL, TO_ROOM);
	d = ch->desc;
	do_autosave (ch, "");
	in_room = ch->in_room;
	extract_char (ch, TRUE);
	d->character = NULL;
	fOld = load_char_obj (d, argument);
	ch = d->character;
	ch->next = char_list;
	char_list = ch;
	char_to_room (ch, in_room);
	ch->ploaded = TRUE;
	return;
}



void do_preturn (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	bool fOld;
	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (ch->pload == NULL)
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	sprintf (arg, ch->pload);
	if (strlen (arg) < 3 || strlen (arg) > 8)
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (!str_cmp (ch->name, arg))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	d = ch->desc;
	sprintf (buf, "You transform back into %s.\n\r", capitalize (ch->pload));
	send_to_char (buf, ch);
	sprintf (buf, "$n transforms back into %s.", capitalize (ch->pload));
	act (buf, ch, NULL, NULL, TO_ROOM);
	do_autosave (ch, "");
	if (ch != NULL && ch->desc != NULL)
		extract_char (ch, TRUE);
	else if (ch != NULL)
		extract_char (ch, TRUE);
	if (ch->desc)
		ch->desc->character = NULL;

	fOld = load_char_obj (d, capitalize (arg));
	if (ch->in_room != NULL)
		char_to_room (ch, ch->in_room);
	else
		char_to_room (ch, get_room_index (3001));
	free_string (ch->pload);
	ch->pload = str_dup ("");
	return;
}




void do_oload (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	int level;
	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}
	if (!IS_IMMORTAL (ch) && !IS_SHOPKEEPER (ch))
		return;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0' || !is_number (arg1))
	{
		send_to_char ("Syntax: oload <vnum> <level>.\n\r", ch);
		return;
	}

	if (arg2[0] == '\0')
	{
		level = get_trust (ch);
	}
	else
	{
		/*
		 * New feature from Alander.
		 */
		if (!is_number (arg2))
		{
			send_to_char ("Syntax: oload <vnum> <level>.\n\r", ch);
			return;
		}
		level = atoi (arg2);
		if (level < 0 || level > get_trust (ch))
		{
			send_to_char ("Limited to your trust level.\n\r", ch);
			return;
		}
	}

	if ((pObjIndex = get_obj_index (atoi (arg1))) == NULL)
	{
		send_to_char ("No object has that vnum.\n\r", ch);
		return;
	}

	obj = create_object (pObjIndex, level);
	if (CAN_WEAR (obj, ITEM_TAKE))
	{
		obj_to_char (obj, ch);
		act ("$p appears in $n's hands!", ch, obj, NULL, TO_ROOM);
	}
	else
	{
		obj_to_room (obj, ch->in_room);
		act ("$n has created $p!", ch, obj, NULL, TO_ROOM);
	}
	act ("You create $p.", ch, obj, NULL, TO_CHAR);
	if (obj->questmaker != NULL)
		free_string (obj->questmaker);
	obj->questmaker = str_dup (ch->name);
	return;
}



void do_purge (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		/* 'purge' */
		CHAR_DATA *vnext;
		OBJ_DATA *obj_next;
		for (victim = ch->in_room->people; victim != NULL; victim = vnext)
		{
			vnext = victim->next_in_room;
			if (IS_NPC (victim) && victim->desc == NULL && victim->mount == NULL && victim->embrace == NULL)
				extract_char (victim, TRUE);
		}

		for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			extract_obj (obj);
		}

		act ("$n purges the room!", ch, NULL, NULL, TO_ROOM);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim))
	{
		send_to_char ("Not on PC's.\n\r", ch);
		return;
	}
	if (victim->desc != NULL)
	{
		send_to_char ("Not on switched players.\n\r", ch);
		return;
	}

	act ("$n purges $N.", ch, NULL, victim, TO_NOTVICT);
	extract_char (victim, TRUE);
	return;
}



void do_restore (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *prev = ch;
	DESCRIPTOR_DATA *d;
	int sn, check, oldcheck;
	sprintf (buf, "%s: Restore %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Restore whom?\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "all"))
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && (victim = d->character) != NULL && victim != ch && victim->in_room != NULL && !IS_SET (victim->extra2, EXTRA2_TORPORED) && can_see (ch, victim))
			{
				prev = victim;
				victim->mana = victim->max_mana;
				victim->move = victim->max_move;
				if (!((victim->loc_hp[0] + victim->loc_hp[1] + victim->loc_hp[2] + victim->loc_hp[3] + victim->loc_hp[4] + victim->loc_hp[5] + victim->loc_hp[6]) == 0 && victim->hit == victim->max_hit))
				{
					if ((check = victim->loc_hp[6]) > 0)
					{
						oldcheck = 0;
						sn = skill_lookup ("clot");
						while (oldcheck != (check = victim->loc_hp[6]))
						{
							oldcheck = check;
							(*skill_table[sn].spell_fun) (sn, ch->level, ch, victim);
						}
					}
					victim->hit = victim->max_hit;
					victim->agg = 0;
		if (!IS_NPC(victim))
		{
			victim->pcdata->stage[1] = 0;
			victim->pcdata->stage[2] = 0;
		}
					send_to_char ("You have been completely healed!\n\r", victim);
					act ("$n's wounds have been completely healed!", victim, NULL, NULL, TO_ROOM);
					update_pos (victim);
				}
			}
		}
		if (!strcmp (ch->name, "Leah"))
			do_echo (ch, "world #PPixie dust sprinkles down around you as Leah restores you!#n");
		else if (!strcmp (ch->name, "Reuben"))
			do_echo (ch, "world #SReuben #emakes you feel all better!");
		else if (!strcmp (ch->name, "Persephone"))
			do_echo (ch, "world #SThe scent of spring surrounds you as Persephone restores you.#n");
		else
			do_echo (ch, "world #eSomeone seems to have restored you... Thank Them#n");
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	victim->hit = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;
	victim->loc_hp[0] = 0;
	victim->loc_hp[1] = 0;
	victim->loc_hp[2] = 0;
	victim->loc_hp[3] = 0;
	victim->loc_hp[4] = 0;
	victim->loc_hp[5] = 0;
	victim->loc_hp[6] = 0;
	update_pos (victim);
	act ("$n has restored you.", ch, NULL, victim, TO_VICT);
	send_to_char ("Ok.\n\r", ch);
	return;
}



void do_freeze (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	sprintf (buf, "%s: Freeze %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Freeze whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if (IS_SET (victim->act, PLR_FREEZE))
	{
		REMOVE_BIT (victim->act, PLR_FREEZE);
		send_to_char ("You can play again.\n\r", victim);
		send_to_char ("FREEZE removed.\n\r", ch);
	}
	else
	{
		SET_BIT (victim->act, PLR_FREEZE);
		send_to_char ("You can't do ANYthing!\n\r", victim);
		send_to_char ("FREEZE set.\n\r", ch);
	}

	do_autosave (victim, "");
	return;
}



void do_log (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	sprintf (buf, "%s: Log %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Log whom?\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "all"))
	{
		if (fLogAll)
		{
			fLogAll = FALSE;
			send_to_char ("Log ALL off.\n\r", ch);
		}
		else
		{
			fLogAll = TRUE;
			send_to_char ("Log ALL on.\n\r", ch);
		}
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	/*
	 * No level check, gods can log anyone.
	 */
	if (IS_SET (victim->act, PLR_LOG))
	{
		REMOVE_BIT (victim->act, PLR_LOG);
		send_to_char ("LOG removed.\n\r", ch);
	}
	else
	{
		SET_BIT (victim->act, PLR_LOG);
		send_to_char ("LOG set.\n\r", ch);
	}

	return;
}



void do_noemote (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	sprintf (buf, "%s: Noemote %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Noemote whom?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if (IS_SET (victim->act, PLR_NO_EMOTE))
	{
		REMOVE_BIT (victim->act, PLR_NO_EMOTE);
		send_to_char ("You can emote again.\n\r", victim);
		send_to_char ("NO_EMOTE removed.\n\r", ch);
	}
	else
	{
		SET_BIT (victim->act, PLR_NO_EMOTE);
		send_to_char ("You can't emote!\n\r", victim);
		send_to_char ("NO_EMOTE set.\n\r", ch);
	}

	return;
}



void do_notell (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	sprintf (buf, "%s: Notell %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Notell whom?", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if (IS_SET (victim->act, PLR_NO_TELL))
	{
		REMOVE_BIT (victim->act, PLR_NO_TELL);
		send_to_char ("You can tell again.\n\r", victim);
		send_to_char ("NO_TELL removed.\n\r", ch);
	}
	else
	{
		SET_BIT (victim->act, PLR_NO_TELL);
		send_to_char ("You can't tell!\n\r", victim);
		send_to_char ("NO_TELL set.\n\r", ch);
	}

	return;
}



void do_silence (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	sprintf (buf, "%s: Silence %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Silence whom?", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (get_trust (victim) >= get_trust (ch))
	{
		send_to_char ("You failed.\n\r", ch);
		return;
	}

	if (IS_SET (victim->act, PLR_SILENCE))
	{
		REMOVE_BIT (victim->act, PLR_SILENCE);
		send_to_char ("You can use channels again.\n\r", victim);
		send_to_char ("SILENCE removed.\n\r", ch);
	}
	else
	{
		SET_BIT (victim->act, PLR_SILENCE);
		send_to_char ("You can't use channels!\n\r", victim);
		send_to_char ("SILENCE set.\n\r", ch);
	}

	return;
}


BAN_DATA *ban_free;
BAN_DATA *ban_list;
void do_ban (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	BAN_DATA *pban;

	sprintf (buf2, "%s: Ban %s", ch->name, argument);

	if (IS_NPC (ch))
		return;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		strcpy (buf, "Banned sites:\n\r");
		for (pban = ban_list; pban != NULL; pban = pban->next)
		{
			strcat (buf, pban->name);
			strcat (buf, "\n\r");
		}
		send_to_char (buf, ch);
		return;
	}

	for (pban = ban_list; pban != NULL; pban = pban->next)
	{
		if (!str_cmp (arg, pban->name))
		{
			send_to_char ("That site is already banned!\n\r", ch);
			return;
		}
	}

	if (ban_free == NULL)
	{
		pban = alloc_perm (sizeof (*pban));
	}
	else
	{
		pban = ban_free;
		ban_free = ban_free->next;
	}

	pban->name = str_dup (arg);
	pban->next = ban_list;
	ban_list = pban;
	save_banlist ();
	send_to_char ("Ok.\n\r", ch);
	return;
}



void do_allow (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	BAN_DATA *prev;
	BAN_DATA *curr;

	sprintf (buf, "%s: Allow %s", ch->name, argument);
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Remove which site from the ban list?\n\r", ch);
		return;
	}

	prev = NULL;
	for (curr = ban_list; curr != NULL; prev = curr, curr = curr->next)
	{
		if (!str_cmp (arg, curr->name))
		{
			if (prev == NULL)
				ban_list = ban_list->next;
			else
				prev->next = curr->next;
			free_string (curr->name);
			curr->next = ban_free;
			ban_free = curr;
			save_banlist ();
			send_to_char ("Ok.\n\r", ch);
			return;
		}
	}

	send_to_char ("Site is not banned.\n\r", ch);
	return;
}

/* Load banlist */
void load_banlist ()
{
	FILE *fp;
	BAN_DATA *p;
	char *ban;
	ban_list = NULL;
	fp = fopen (BAN_FILE, "r");
	if (!fp)		/* No ban file.. no bans : */
		return;
	ban = fread_word (fp);
	while (str_cmp (ban, END_MARKER))
	{
		p = alloc_mem (sizeof (BAN_DATA));
		p->name = str_dup (ban);
		p->next = ban_list;
		ban_list = p;
		ban = fread_word (fp);
	}

	fclose (fp);
}

/* Save banlist */
void save_banlist ()
{
	FILE *fp;
	BAN_DATA *p;
	if (!ban_list)
	{			/* delete file if there are no bans */
		unlink (BAN_FILE);
		return;
	}

	fp = fopen (BAN_FILE, "w");
	if (!fp)
	{
		bug ("Could not open " BAN_FILE " for writing", 0);
		return;
	}

	for (p = ban_list; p; p = p->next)
		fprintf (fp, "%s\n", p->name);
	fprintf (fp, "%s\n", END_MARKER);
	fclose (fp);
}




void do_wizlock (CHAR_DATA * ch, char *argument)
{
	extern bool wizlock;
	char buf[MAX_STRING_LENGTH];

	sprintf (buf, "%s: Wizlock %s", ch->name, argument);
	wizlock = !wizlock;

	if (wizlock)
		send_to_char ("Game wizlocked.\n\r", ch);
	else
		send_to_char ("Game un-wizlocked.\n\r", ch);
	return;
}



void do_slookup (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int sn;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Slookup what?\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "all"))
	{
		for (sn = 0; sn < MAX_SKILL; sn++)
		{
			if (skill_table[sn].name == NULL)
				break;
			sprintf (buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r", sn, skill_table[sn].slot, skill_table[sn].name);
			send_to_char (buf, ch);
		}
	}
	else
	{
		if ((sn = skill_lookup (arg)) < 0)
		{
			send_to_char ("No such skill or spell.\n\r", ch);
			return;
		}

		sprintf (buf, "Sn: %4d Slot: %4d Skill/spell: '%s'\n\r", sn, skill_table[sn].slot, skill_table[sn].name);
		send_to_char (buf, ch);
	}

	return;
}



void do_sset (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int value;
	int sn;
	bool fAll;

	sprintf (buf, "%s: Sset %s", ch->name, argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax: sset <victim> <skill> <value>\n\r", ch);
		send_to_char ("or:     sset <victim> all     <value>\n\r", ch);
		send_to_char ("Skill being any skill or spell.\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	fAll = !str_cmp (arg2, "all");
	sn = 0;
	if (!fAll && (sn = skill_lookup (arg2)) < 0)
	{
		send_to_char ("No such skill or spell.\n\r", ch);
		return;
	}

	/*
	 * Snarf the value.
	 */
	if (!is_number (arg3))
	{
		send_to_char ("Value must be numeric.\n\r", ch);
		return;
	}

	value = atoi (arg3);
	if (value < 0 || value > 100)
	{
		send_to_char ("Value range is 0 to 100.\n\r", ch);
		return;
	}

	if (fAll)
	{
		for (sn = 0; sn < MAX_SKILL; sn++)
		{
			if (skill_table[sn].name != NULL)
				victim->pcdata->learned[sn] = value;
		}
	}
	else
	{
		victim->pcdata->learned[sn] = value;
	}

	send_to_char ("Ok.\n\r", ch);
	return;
}



void do_mset (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH + 5];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int value;

	sprintf (buf, "%s: Mset %s", ch->name, argument);
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax: mset <victim> <field>  <value>\n\r", ch);
		send_to_char ("or:     mset <victim> <string> <value>\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("Field being one of:\n\r", ch);
		send_to_char ("  level trust exp notes notes+ notes-\n\r", ch);
		send_to_char ("  gold hp mana move sex\n\r", ch);
		send_to_char ("  thirst drunk full blood\n\r", ch);
		send_to_char ("  security\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("String being one of:\n\r", ch);
		send_to_char ("  name short long description title spec\n\r", ch);
		send_to_char ("  email\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim) && ch->level < 11)
	{
		send_to_char ("Your level isn't high enough to mset players.\n\r", ch);
		return;
	}
	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = is_number (arg3) ? atoi (arg3) : -1;
	/*
	 * Set something.
	 */
	if (!str_cmp (arg2, "security"))
	{
		int security;
		if (IS_NPC (ch) || IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		security = IS_JUDGE (ch) ? 1 : GET_PC (ch, security, 0);
		if (ch == victim && !IS_JUDGE (ch))
		{
			send_to_char ("Not on yourself.\n\r", ch);
			return;
		}
		if (value < security || value > 9)
		{
			if (security != 9)
			{
				sprintf (buf, "Enter a security level between %d and 9.\n\r", security);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("You can only set their security level to 9.\n\r", ch);
			return;
		}

		if (victim->pcdata->security == value)
		{
			sprintf (buf, "They already have security level %d!\n\r", value);
			send_to_char (buf, ch);
			return;
		}
		victim->pcdata->security = value;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "notes"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 25)
		{
			send_to_char ("Notes range is 0 to 25.\n\r", ch);
			return;
		}
		victim->pcdata->notes = value;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "notes+"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 1)
		{
			send_to_char ("Please enter a positive value.\n\r", ch);
			return;
		}
		if ((ch->pcdata->notes + value) >= 25)
		{
			send_to_char ("Setting notes limit to 25.\n\r", ch);
			victim->pcdata->notes = 25;
			return;
		}
		victim->pcdata->notes += value;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "notes-"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 1)
		{
			send_to_char ("Please enter a positive value.\n\r", ch);
			return;
		}
		if ((ch->pcdata->notes - value) < 1)
		{
			send_to_char ("Setting notes limit to 0.\n\r", ch);
			victim->pcdata->notes = 0;
			return;
		}
		victim->pcdata->notes -= value;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "sex"))
	{
		if (!str_cmp (arg3, "none"))
			value = 0;
		else if (!str_cmp (arg3, "male"))
			value = 1;
		else if (!str_cmp (arg3, "female"))
			value = 2;
		else if (value < 0 || value > 2)
		{
			send_to_char ("Sex should be one of: Male, Female, None.\n\r", ch);
			return;
		}
		victim->sex = value;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "level"))
	{
		if (IS_NPC (victim) && (value < 1 || value > 250))
		{
			send_to_char ("Level range is 1 to 250 for mobs.\n\r", ch);
			return;
		}
		else if (!IS_JUDGE (ch))
		{
			send_to_char ("Sorry, no can do...\n\r", ch);
			return;
		}
		if (!str_cmp (arg3, "mortal"))
			value = 2;
		else if (!str_cmp (arg3, "avatar"))
			value = 3;
		else if (!str_cmp (arg3, "apprentice"))
			value = 4;
		else if (!str_cmp (arg3, "mage"))
			value = 5;
		else if (!str_cmp (arg3, "storyteller"))
			value = 6;
		else if (!str_cmp (arg3, "shopkeeper"))
			value = 5;
		else if (!str_cmp (arg3, "builder"))
			value = 7;
		else if (!str_cmp (arg3, "questmaker"))
			value = 8;
		else if (!str_cmp (arg3, "enforcer"))
			value = 9;
		else if (!str_cmp (arg3, "judge"))
			value = 10;
		else if (!str_cmp (arg3, "highjudge"))
			value = 11;
		else if (!IS_NPC (victim))
		{
			send_to_char ("Level should be one of the following:\n\rMortal, Avatar, Apprentice, Mage, Storyteller, Shopkeeper,Builder, Questmaker, Enforcer,\n\rJudge, or Highjudge.\n\r", ch);
			return;
		}

		if (value >= ch->level && !IS_NPC (victim))
			send_to_char ("Sorry, no can do...\n\r", ch);
		else
		{
			victim->level = value;
			send_to_char ("Ok.\n\r", ch);
		}
		return;
	}

	if (!str_cmp (arg2, "trust"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		else if (!IS_JUDGE (ch))
		{
			send_to_char ("Sorry, no can do...\n\r", ch);
			return;
		}
		if (!str_cmp (arg3, "none"))
			value = 0;
		else if (!str_cmp (arg3, "apprentice"))
			value = 4;
		else if (!str_cmp (arg3, "mage"))
			value = 5;
		else if (!str_cmp (arg3, "storyteller"))
			value = 6;
		else if (!str_cmp (arg3, "shopkeeper"))
			value = 5;
		else if (!str_cmp (arg3, "builder"))
			value = 7;
		else if (!str_cmp (arg3, "questmaker"))
			value = 8;
		else if (!str_cmp (arg3, "enforcer"))
			value = 9;
		else if (!str_cmp (arg3, "judge"))
			value = 10;
		else if (!str_cmp (arg3, "highjudge"))
			value = 11;
		else
		{
			send_to_char ("Trust should be one of the following:\n\rNone, Apprentice, Mage, Storyteller, Shopkeeper,Builder, Questmaker, Enforcer,\n\rJudge, or Highjudge.\n\r", ch);
			return;
		}


		if (value >= ch->level)
			send_to_char ("Sorry, no can do...\n\r", ch);
		else
		{
			victim->trust = value;
			send_to_char ("Ok.\n\r", ch);
		}
		return;
	}


	if (!str_cmp (arg2, "exp"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (value < 0)
		{
			send_to_char ("Exp must be at least 0.\n\r", ch);
			return;
		}

		if (value > 50000000)
		{
			send_to_char ("No more than 50000000 possible.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch) || (ch == victim))
		{
			victim->exp = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "gold"))
	{
		if (value > 100000 && !IS_JUDGE (ch))
			send_to_char ("Don't be so damn greedy!\n\r", ch);
		else
		{
			victim->gold = value;
			send_to_char ("Ok.\n\r", ch);
		}
		return;
	}

	if (!str_cmp (arg2, "hp"))
	{
		if (value < 1 || value > 30000)
		{
			send_to_char ("Hp range is 1 to 30,000 hit points.\n\r", ch);
			return;
		}
		if (IS_JUDGE (ch) || (ch == victim) || (IS_NPC (victim)))
		{
			victim->max_hit = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "mana"))
	{
		if (value < 0 || value > 30000)
		{
			send_to_char ("Mana range is 0 to 30,000 mana points.\n\r", ch);
			return;
		}
		if (IS_JUDGE (ch) || (ch == victim) || (IS_NPC (victim)))
		{
			victim->max_mana = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "move"))
	{
		if (value < 0 || value > 30000)
		{
			send_to_char ("Move range is 0 to 30,000 move points.\n\r", ch);
			return;
		}
		if (IS_JUDGE (ch) || (ch == victim) || (IS_NPC (victim)))
		{
			victim->max_move = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "blood"))
	{
		if (value < 0 || value > victim->blood[BLOOD_POOL])
		{
			sprintf (buf, "Blood range is 0 to %d.\n\r", victim->blood[BLOOD_POOL]);
			send_to_char (buf, ch);
			return;
		}

		victim->blood[BLOOD_CURRENT] = value;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "name"))
	{
		if (!IS_NPC (victim))
		{
			send_to_char ("Not on PC's.\n\r", ch);
			return;
		}

		free_string (victim->name);
		victim->name = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "short"))
	{
		free_string (victim->short_descr);
		victim->short_descr = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "long"))
	{
		free_string (victim->long_descr);
		strcat (arg3, "\n\r");
		victim->long_descr = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "side"))
	{
		if (!str_cmp (arg3, "camarilla"))
		{
			free_string (victim->side);
			victim->side = str_dup ("Camarilla");
			send_to_char ("Ok.\n\r", ch);
		}
		else if (!str_cmp (arg3, "sabbat"))
		{
			free_string (victim->side);
			victim->side = str_dup ("Sabbat");
			send_to_char ("Ok.\n\r", ch);
		}
		else if (!str_cmp (arg3, "anarch"))
		{
			free_string (victim->side);
			victim->side = str_dup ("Anarch");
			send_to_char ("Ok.\n\r", ch);
		}
		else if (!str_cmp (arg3, "inconnu"))
		{
			free_string (victim->side);
			victim->side = str_dup ("Inconnu");
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Please choose Camarilla, Sabbat, Anarch or Inconnu.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "email"))
	{
		if (!IS_JUDGE (ch))
		{
			send_to_char ("Only Judges can change a player's email address.\n\r", ch);
			return;
		}

		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		free_string (victim->pcdata->email);
		victim->pcdata->email = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "title"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		set_title (victim, arg3);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	/*
	 * Generate usage message.
	 */
	do_mset (ch, "");
	return;
}



void do_oset (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	OBJ_DATA *morph;
	OBJ_INDEX_DATA *pObjIndex;
	int value;
	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	sprintf (buf, "%s: Oset %s", ch->name, argument);
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax: oset <object> <field>  <value>\n\r", ch);
		send_to_char ("or:     oset <object> <string> <value>\n\r", ch);
		send_to_char ("or:     oset <object> <affect> <value>\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("Field being one of:\n\r", ch);
		send_to_char ("  value0 value1 value2 value3\n\r", ch);
		send_to_char ("  level weight cost timer morph\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("String being one of:\n\r", ch);
		send_to_char ("  name short long ed type extra wear owner\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("Affect being one of:\n\r", ch);
		send_to_char ("  hit dam ac hp mana move\n\r", ch);
		return;
	}

	if ((obj = get_obj_world (ch, arg1)) == NULL)
	{
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
		return;
	}


	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = atoi (arg3);
	/*
	 * Set something.
	 */
	if (!str_cmp (arg2, "value0") || !str_cmp (arg2, "v0"))
	{
		if (obj->item_type == ITEM_WEAPON && !IS_JUDGE (ch))
		{
			send_to_char ("You are not authorised to create spell weapons.\n\r", ch);
			return;
		}
		else if (obj->item_type == ITEM_QUEST)
		{
			send_to_char ("You cannot change a quest tokens value with oset.\n\r", ch);
			return;
		}
		else if (obj->item_type == ITEM_ARMOR && value > 15)
			obj->value[0] = 15;
		else
			obj->value[0] = value;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "value1") || !str_cmp (arg2, "v1"))
	{
		if (obj->item_type == ITEM_WEAPON && value > 10)
			obj->value[1] = 10;
		else
			obj->value[1] = value;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "value2") || !str_cmp (arg2, "v2"))
	{
		if (obj->item_type == ITEM_WEAPON && value > 20)
			obj->value[2] = 20;
		else
			obj->value[2] = value;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "value3") || !str_cmp (arg2, "v3"))
	{
		if (obj->item_type == ITEM_ARMOR && !IS_JUDGE (ch))
			send_to_char ("You are not authorised to create spell armour.\n\r", ch);
		else
		{
			obj->value[3] = value;
			send_to_char ("Ok.\n\r", ch);
		}
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "morph"))
	{
		int mnum;

		if ((pObjIndex = get_obj_index (value)) == NULL)
		{
			send_to_char ("No object has that vnum.\n\r", ch);
			return;
		}
		morph = create_object (pObjIndex, obj->level);
		if (!CAN_WEAR (morph, ITEM_TAKE))
			SET_BIT (morph->wear_flags, ITEM_TAKE);

		obj_to_char (morph, ch);
		act ("$p morphs into $P in $n's hands!", ch, obj, morph, TO_ROOM);
		act ("$p morphs into $P in your hands!", ch, obj, morph, TO_CHAR);
		mnum = obj->wear_loc;
		obj_from_char (obj);
		obj_to_obj (obj, morph);
		if (morph->wear_flags == obj->wear_flags && mnum != WEAR_NONE)
			equip_char (ch, morph, mnum);
		if (morph->questmaker != NULL)
			free_string (morph->questmaker);
		morph->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "extra"))
	{
		if (!str_cmp (arg3, "glow"))
			value = ITEM_GLOW;
		else if (!str_cmp (arg3, "hum"))
			value = ITEM_HUM;
		else if (!str_cmp (arg3, "thrown"))
			value = ITEM_THROWN;
		else if (!str_cmp (arg3, "vanish"))
			value = ITEM_VANISH;
		else if (!str_cmp (arg3, "invis"))
			value = ITEM_INVIS;
		else if (!str_cmp (arg3, "magic"))
			value = ITEM_MAGIC;
		else if (!str_cmp (arg3, "nodrop"))
			value = ITEM_NODROP;
		else if (!str_cmp (arg3, "bless"))
			value = ITEM_BLESS;
		else if (!str_cmp (arg3, "anti-good"))
			value = ITEM_ANTI_GOOD;
		else if (!str_cmp (arg3, "anti-evil"))
			value = ITEM_ANTI_EVIL;
		else if (!str_cmp (arg3, "anti-neutral"))
			value = ITEM_ANTI_NEUTRAL;
		else if (!str_cmp (arg3, "noremove"))
			value = ITEM_NOREMOVE;
		else if (!str_cmp (arg3, "inventory"))
			value = ITEM_INVENTORY;
		else if (!str_cmp (arg3, "loyal"))
			value = ITEM_LOYAL;
		else
		{
			send_to_char ("Extra flag can be from the following: Glow, Hum, Thrown, Vanish, Invis, Magic, Nodrop, Bless, Anti - Good, Anti - Evil, Anti - Neutral, Noremove, Inventory, Loyal. Silver\n\r ", ch);
			return;
		}
		/* Removing magic flag allows multiple enchants */
		if (IS_SET (obj->extra_flags, value) && value == ITEM_MAGIC && !IS_JUDGE (ch))
		{
			send_to_char ("Sorry, no can do...\n\r", ch);
			return;
		}

		if (IS_SET (obj->extra_flags, value))
			REMOVE_BIT (obj->extra_flags, value);
		else
			SET_BIT (obj->extra_flags, value);
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "wear"))
	{
		if (!str_cmp (arg3, "none") || !str_cmp (arg3, "clear"))
		{
			obj->wear_flags = 0;
			send_to_char ("Ok.\n\r", ch);
			if (obj->questmaker != NULL)
				free_string (obj->questmaker);
			obj->questmaker = str_dup (ch->name);
			return;
		}
		else if (!str_cmp (arg3, "take"))
		{
			if (IS_SET (obj->wear_flags, ITEM_TAKE))
				REMOVE_BIT (obj->wear_flags, ITEM_TAKE);
			else
				SET_BIT (obj->wear_flags, ITEM_TAKE);
			send_to_char ("Ok.\n\r", ch);
			if (obj->questmaker != NULL)
				free_string (obj->questmaker);
			obj->questmaker = str_dup (ch->name);
			return;
		}
		else if (!str_cmp (arg3, "finger"))
			value = ITEM_WEAR_FINGER;
		else if (!str_cmp (arg3, "neck"))
			value = ITEM_WEAR_NECK;
		else if (!str_cmp (arg3, "body"))
			value = ITEM_WEAR_BODY;
		else if (!str_cmp (arg3, "head"))
			value = ITEM_WEAR_HEAD;
		else if (!str_cmp (arg3, "legs"))
			value = ITEM_WEAR_LEGS;
		else if (!str_cmp (arg3, "feet"))
			value = ITEM_WEAR_FEET;
		else if (!str_cmp (arg3, "hands"))
			value = ITEM_WEAR_HANDS;
		else if (!str_cmp (arg3, "arms"))
			value = ITEM_WEAR_ARMS;
		else if (!str_cmp (arg3, "about"))
			value = ITEM_WEAR_ABOUT;
		else if (!str_cmp (arg3, "waist"))
			value = ITEM_WEAR_WAIST;
		else if (!str_cmp (arg3, "wrist"))
			value = ITEM_WEAR_WRIST;
		else if (!str_cmp (arg3, "hold"))
			value = ITEM_WIELD;
		else if (!str_cmp (arg3, "face"))
			value = ITEM_WEAR_FACE;
		else
		{
			send_to_char ("Wear location can be from: None, Take, Finger, Neck, Body, Head, Legs, Hands, Arms, About, Waist, Hold, Face. \n\r ", ch);
			return;
		}
		if (IS_SET (obj->wear_flags, ITEM_TAKE))
			value += 1;
		obj->wear_flags = value;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "level"))
	{
		if (value < 1)
			value = 1;
		else if (value > 50)
			value = 50;
		if (!IS_JUDGE (ch))
			send_to_char ("You are not authorised to change an items level.\n\r", ch);
		else
		{
			obj->level = value;
			send_to_char ("Ok.\n\r", ch);
			if (obj->questmaker != NULL)
				free_string (obj->questmaker);
			obj->questmaker = str_dup (ch->name);
		}
		return;
	}

	if (!str_cmp (arg2, "weight"))
	{
		obj->weight = value;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "cost"))
	{
		if (value > 100000 && !IS_JUDGE (ch))
			send_to_char ("Don't be so damn greedy!\n\r", ch);
		else
		{
			obj->cost = value;
			send_to_char ("Ok.\n\r", ch);
			if (obj->questmaker != NULL)
				free_string (obj->questmaker);
			obj->questmaker = str_dup (ch->name);
		}
		return;
	}

	if (!str_cmp (arg2, "timer"))
	{
		obj->timer = value;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "type"))
	{
		if (!IS_JUDGE (ch))
		{
			send_to_char ("You are not authorised to change an item type.\n\r", ch);
			return;
		}
		if (!str_cmp (arg3, "light"))
			obj->item_type = 1;
		else if (!str_cmp (arg3, "scroll"))
			obj->item_type = 2;
		else if (!str_cmp (arg3, "wand"))
			obj->item_type = 3;
		else if (!str_cmp (arg3, "staff"))
			obj->item_type = 4;
		else if (!str_cmp (arg3, "weapon"))
			obj->item_type = 5;
		else if (!str_cmp (arg3, "treasure"))
			obj->item_type = 8;
		else if (!str_cmp (arg3, "armor"))
			obj->item_type = 9;
		else if (!str_cmp (arg3, "armour"))
			obj->item_type = 9;
		else if (!str_cmp (arg3, "potion"))
			obj->item_type = 10;
		else if (!str_cmp (arg3, "furniture"))
			obj->item_type = 12;
		else if (!str_cmp (arg3, "trash"))
			obj->item_type = 13;
		else if (!str_cmp (arg3, "container"))
			obj->item_type = 15;
		else if (!str_cmp (arg3, "drink"))
			obj->item_type = 17;
		else if (!str_cmp (arg3, "key"))
			obj->item_type = 18;
		else if (!str_cmp (arg3, "food"))
			obj->item_type = 19;
		else if (!str_cmp (arg3, "money"))
			obj->item_type = 20;
		else if (!str_cmp (arg3, "boat"))
			obj->item_type = 22;
		else if (!str_cmp (arg3, "corpse"))
			obj->item_type = 23;
		else if (!str_cmp (arg3, "fountain"))
			obj->item_type = 25;
		else if (!str_cmp (arg3, "pill"))
			obj->item_type = 26;
		else if (!str_cmp (arg3, "portal"))
			obj->item_type = 27;
		else if (!str_cmp (arg3, "stake"))
			obj->item_type = 30;
	 	else if (!str_cmp( arg3, "tool"))
			obj->item_type = 39;
		else
		{
			send_to_char ("Type can be one of: Light, Scroll, Wand, Staff, Weapon, Treasure, Armor, Potion, Furniture, Trash, Container, Drink, Key, Food, Money, Boat, Corpse, Fountain, Pill, Portal, Stake, Tool. \n\r ", ch);
			return;
		}
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "owner"))
	{
		if (IS_NPC (ch))
		{
			send_to_char ("Not while switched.\n\r", ch);
			return;
		}
		if (!IS_JUDGE (ch) && (obj->questmaker == NULL || str_cmp (ch->name, obj->questmaker)))
		{
			send_to_char ("Someone else has already changed this item.\n\r", ch);
			return;
		}
		if ((victim = get_char_world (ch, arg3)) == NULL)
		{
			send_to_char ("You cannot find any player by that name.\n\r", ch);
			return;
		}
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		if (obj->questowner != NULL)
			free_string (obj->questowner);
		obj->questowner = str_dup (victim->name);
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "name"))
	{
		free_string (obj->name);
		obj->name = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "short"))
	{
		free_string (obj->short_descr);
		obj->short_descr = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "long"))
	{
		free_string (obj->description);
		obj->description = str_dup (arg3);
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	if (!str_cmp (arg2, "ed"))
	{
		EXTRA_DESCR_DATA *ed;
		argument = one_argument (argument, arg3);
		if (argument == NULL)
		{
			send_to_char ("Syntax: oset <object> ed <keyword> <string>\n\r", ch);
			return;
		}

		if (extra_descr_free == NULL)
		{
			ed = alloc_perm (sizeof (*ed));
		}
		else
		{
			ed = extra_descr_free;
			extra_descr_free = extra_descr_free->next;
		}

		ed->keyword = str_dup (arg3);
		ed->description = str_dup (argument);
		ed->next = obj->extra_descr;
		obj->extra_descr = ed;
		send_to_char ("Ok.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	/*
	 * Generate usage message.
	 */
	do_oset (ch, "");
	return;
}

void do_recall(CHAR_DATA *ch, char *argument)
{
	act("#g$n#g returns to the OOC area.#n", ch, NULL, NULL, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, get_room_index (6050));
	act("#g$n#g has arrived from an IC map.#n", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "");
	return;
}

void do_rset (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;
	int value;

	sprintf (buf, "%s: Rset %s", ch->name, argument);
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax: rset <location> <field> value\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("Field being one of:\n\r", ch);
		send_to_char ("  flags sector\n\r", ch);
		return;
	}

	if ((location = find_location (ch, arg1)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}

	/*
	 * Snarf the value.
	 */
	if (!is_number (arg3))
	{
		send_to_char ("Value must be numeric.\n\r", ch);
		return;
	}
	value = atoi (arg3);
	/*
	 * Set something.
	 */
	if (!str_cmp (arg2, "flags"))
	{
		location->room_flags = value;
		return;
	}

	if (!str_cmp (arg2, "sector"))
	{
		location->sector_type = value;
		return;
	}

	/*
	 * Generate usage message.
	 */
	do_rset (ch, "");
	return;
}


void do_users (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *gch;
	DESCRIPTOR_DATA *d;
	DESCRIPTOR_DATA *d2;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int count;
	char *st;
	char s[100];
	char idle[10];
	bool found = FALSE;
	bool found2 = FALSE;

	count = 0;
	buf[0] = '\0';
	buf2[0] = '\0';
	strcat (buf2, "\n\r[Num Connected_State Login@ Idl] Player Name  Host\n\r");
	strcat (buf2, "-------------------------------------------------------------------------------\n\r");

	for (d = descriptor_list; d; d = d->next)
	{
		if (d->character && can_see (ch, d->character))
		{
			/* NB: You may need to edit the CON_ values */
			switch (d->connected)
			{
			case CON_PLAYING:
				st = "    PLAYING    ";
				break;
			case CON_GET_NAME:
				st = "   Get Name    ";
				break;
			case CON_GET_OLD_PASSWORD:
				st = "Get Old Passwd ";
				break;
			case CON_CONFIRM_NEW_NAME:
				st = " Confirm Name  ";
				break;
			case CON_GET_NEW_PASSWORD:
				st = "Get New Passwd ";
				break;
			case CON_CONFIRM_NEW_PASSWORD:
				st = "Confirm Passwd ";
				break;
			case CON_GET_NEW_SEX:
				st = "  Get New Sex  ";
				break;
			case CON_ACCEPT_DISCLAIMER:
				st = "  Disclaimer   ";
				break;
			case CON_MAIN_MENU:
				st = "  Main Menu    ";
				break;
			case CON_DOCUMENT_MENU:
				st = "  Documentation";
				break;
			case CON_GET_EMAIL:
				st = "  Get Email    ";
				break;
			case CON_GET_NEW_EMAIL:
				st = "  Get New Email";
				break;
			case CON_GET_NEW_ANSI:
				st = "  Get New Ansi ";
				break;
			case CON_GET_PRIMARY_STAT_AREA:
				st = "Get Prim Stats ";
				break;
			case CON_GET_SECONDARY_STAT_AREA:
				st = " Get Sec Stats ";
				break;
			case CON_GET_PRIMARY_STATS:
				st = "Choose Prim St ";
				break;
			case CON_GET_SECONDARY_STATS:
				st = " Choose Sec St ";
				break;
			case CON_GET_TERTIARY_STATS:
				st = " Choose Ter St ";
				break;
			case CON_CONFIRM_STATS:
				st = " Confirm Stats ";
				break;
			case CON_GET_PRIMARY_ABILITY_AREA:
				st = " PrimAbil Area ";
				break;
			case CON_GET_SECONDARY_ABILITY_AREA:
				st = " Sec Abil Area ";
				break;
			case CON_GET_ABILITIES:
				st = " ChseAbilities ";
				break;
			case CON_READ_MOTD:
				st = "  Reading MOTD ";
				break;
				  case CON_NOTE_TO:              st = " Writing Note To "; break;
				  case CON_NOTE_EXPIRE:          st = "Writing Note Expire "; break;
				  case CON_NOTE_TEXT:            st = " Writing Note  "; break;
				  case CON_NOTE_FINISH:          st = " Finishing  Note  "; break;
				  case CON_NOTE_SUBJECT:         st = " Writing Note Subject "; break;

			default:
				st = "   !UNKNOWN!   ";
				break;
			}
			count++;
			/* Format "login" value... */
			vch = d->original ? d->original : d->character;
			if (vch->logon > 0)
			strftime (s, 100, "%I:%M%p", localtime (&vch->logon));
			else
			sprintf( s, " ");
			if (vch->timer > 0)
				sprintf (idle, "%-2d", vch->timer);
			else
				sprintf (idle, "  ");
				sprintf (buf, "[%3d %s %7s %2s] %-12s %-32.32s\n\r", d->descriptor, st, s, idle, (d->original) ? d->original->name : (d->character) ? d->character->name : "(None!)", d->host);
			strcat (buf2, buf);
		}
		else if (!d->character)
		{
			st = "      NULL     ";
			count++;
			/* Format "login" value... */
				sprintf (idle, "  ");
				sprintf(s, " ");
				sprintf (buf, "[%3d %s %7s %2s] %-12s %-32.32s\n\r", d->descriptor, st, s, idle, (d->original) ? d->original->name : (d->character) ? d->character->name : "(None!)", d->host);
			strcat (buf2, buf);
		}
	}

	sprintf (buf, "\n\r%d user%s\n\r", count, count == 1 ? "" : "s");
	strcat (buf2, buf);
	send_to_char (buf2, ch);
	send_to_char ("\n\r", ch);
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected != CON_PLAYING)
			continue;
		for (d2 = d->next; d2 != NULL; d2 = d2->next)
		{
			if (!str_cmp (d->host, d2->host))
			{
				if (d2->connected != CON_PLAYING)
					continue;
				if (d2->character == NULL || d->character == NULL)
					continue;
				found = TRUE;
				sprintf (buf, "%s and %s are Multiplaying.\n\r", d2->character->name, d->character->name);
				send_to_char (buf, ch);
			}
		}
	}
	if (!found)
		send_to_char ("No one is Multiplaying.\n\r", ch);
	send_to_char ("\n\r", ch);
	for (gch = char_list; gch != NULL; gch = gch->next)
	{
		if (IS_NPC (gch) || gch->desc)
			continue;
		found2 = TRUE;
		sprintf (buf, "Name: %12s. (Room: %5d)\n\r", gch->name, gch->in_room == NULL ? : gch->in_room->vnum);
		send_to_char (buf, ch);
	}
	if (!found2)
		send_to_char ("No Linkdead Players found.\n\r", ch);
	return;
}


void do_force (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	sprintf (buf, "%s: Force %s", ch->name, argument);
	argument = one_argument (argument, arg);
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Force whom to do what?\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "all"))
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;
		for (vch = char_list; vch != NULL; vch = vch_next)
		{
			vch_next = vch->next;
			if (!IS_NPC (vch) && get_trust (vch) < get_trust (ch))
			{
				interpret (vch, argument);
			}
		}
	}
	else
	{
		CHAR_DATA *victim;
		if ((victim = get_char_world (ch, arg)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}

		if (victim == ch)
		{
			send_to_char ("Force yourself to do something?\n\r", ch);
			return;
		}

		if (get_trust (victim) >= get_trust (ch))
		{
			send_to_char ("You failed.\n\r", ch);
			return;
		}
		interpret (victim, argument);
	}

	send_to_char ("Ok.\n\r", ch);
	return;
}

void do_forceauto (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	for (vch = char_list; vch != NULL; vch = vch_next)
	{
		vch_next = vch->next;
		if (!IS_NPC (vch) && vch != ch)
		{
			act ("Autocommand: $t.", ch, argument, vch, TO_VICT);
			interpret (vch, argument);
		}
	}
	return;
}


void do_incog (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_INCOG))
	{
		REMOVE_BIT (ch->act, PLR_INCOG);
		act ("$n appears before the room.", ch, NULL, NULL, TO_ROOM);
		send_to_char ("You appear before the room.\n\r", ch);
	}
	else
	{
		act ("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM);
		send_to_char ("You slowly vanish into thin air.\n\r", ch);
		send_to_char ("You go incog.\n\r", ch);
		SET_BIT (ch->act, PLR_INCOG);
	}

	return;
}

/*
 * New routines by Dionysos.
 */
void do_invis (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_WIZINVIS))
	{
		REMOVE_BIT (ch->act, PLR_WIZINVIS);
		act ("$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM);
		send_to_char ("You slowly fade back into existence.\n\r", ch);
	}
	else
	{
		act ("$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM);
		send_to_char ("You slowly vanish into thin air.\n\r", ch);
		SET_BIT (ch->act, PLR_WIZINVIS);
	}

	return;
}



void do_holylight (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->act, PLR_HOLYLIGHT))
	{
		REMOVE_BIT (ch->act, PLR_HOLYLIGHT);
		send_to_char ("Holy light mode off.\n\r", ch);
	}
	else
	{
		SET_BIT (ch->act, PLR_HOLYLIGHT);
		send_to_char ("Holy light mode on.\n\r", ch);
	}

	return;
}

void do_safe (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	one_argument (argument, arg);
	if (IS_NPC (ch) || ch->in_room == NULL)
		return;
	if (IS_SET (ch->in_room->room_flags, ROOM_SAFE))
		send_to_char ("You cannot be attacked by other players here.\n\r", ch);
	else
		send_to_char ("You are not safe from player attacks in this room.\n\r", ch);
	if (!IS_VAMPIRE (ch))
		return;
	if (ch->in_room->sector_type == SECT_INSIDE)
	{
		send_to_char ("You are inside, which means you are safe from sunlight.\n\r", ch);
		return;
	}

	if (room_is_dark (ch->in_room))
	{
		send_to_char ("This room is dark, and will protect you from the sunlight.\n\r", ch);
		return;
	}

	send_to_char ("This room will provide you no protection from the sunlight.\n\r", ch);
	return;
}

void do_qstat (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Which item?\n\r", ch);
		return;
	}
	if ((obj = get_obj_carry (ch, arg)) == NULL)
	{
		send_to_char ("You are not carrying that item.\n\r", ch);
		return;
	}
	sprintf (buf, "Object %s.\n\r", obj->short_descr);
	send_to_char (buf, ch);
	sprintf (buf, "Owner when worn: %s\n\r", obj->chpoweron);
	send_to_char (buf, ch);
	sprintf (buf, "Other when worn: %s\n\r", obj->victpoweron);
	send_to_char (buf, ch);
	sprintf (buf, "Owner when removed: %s\n\r", obj->chpoweroff);
	send_to_char (buf, ch);
	sprintf (buf, "Other when removed: %s\n\r", obj->victpoweroff);
	send_to_char (buf, ch);
	sprintf (buf, "Owner when used: %s\n\r", obj->chpoweruse);
	send_to_char (buf, ch);
	sprintf (buf, "Other when used: %s\n\r", obj->victpoweruse);
	send_to_char (buf, ch);
	send_to_char ("Type:", ch);
	send_to_char (".\n\r", ch);
	sprintf (buf, "Power: %d.\n\r", obj->specpower);
	send_to_char (buf, ch);
	return;
}

void do_qset (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	int value;
	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '0')
	{
		send_to_char ("You can change the following fields...\n\r", ch);
		send_to_char ("chwear   = Message to owner when item is worn.\n\r", ch);
		send_to_char ("chrem    = Message to owner when item is removed.\n\r", ch);
		send_to_char ("chuse    = Message to owner when item is used.\n\r", ch);
		send_to_char ("victwear = Message to others in room when item is worn.\n\r", ch);
		send_to_char ("victrem  = Message to others in room when item is removed.\n\r", ch);
		send_to_char ("victuse  = Message to others in room when item is used.\n\r", ch);
		send_to_char ("type       activate     = Item can be activated.\n\r", ch);
		send_to_char ("           twist        = Item can be twisted.\n\r", ch);
		send_to_char ("           press        = Item can be pressed.\n\r", ch);
		send_to_char ("           pull         = Item can be pulled.\n\r", ch);
		send_to_char ("           target       = Item can target people (for spell, etc).\n\r", ch);
		send_to_char ("           spell        = Item can cast spells.\n\r", ch);
		send_to_char ("           transporter  = Owner can transport freely between two locations.\n\r", ch);
		send_to_char ("           teleporter   = Owner can transport to target location at will.\n\r", ch);
		send_to_char ("           object       = Owner can create the specified object.\n\r", ch);
		send_to_char ("           mobile       = Owner can create the specified mobile.\n\r", ch);
		send_to_char ("           action       = Target must perform an action.\n\r", ch);
		send_to_char ("           delay1       = Sets a delay of half a round on spell items.\n\r", ch);
		send_to_char ("           delay2       = Sets a delay of one round on spell items.\n\r", ch);
		send_to_char ("           song         = Use this on a page to let people sing.\n\r", ch);
		send_to_char ("           message      = Object gives message with chuse and victuse.\n\r", ch);
		send_to_char ("power      <value>      = Spell number/transporter room number.\n\r", ch);
		return;
	}
	if ((obj = get_obj_carry (ch, arg1)) == NULL)
	{
		send_to_char ("You are not carrying that item.\n\r", ch);
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;
	if (!str_cmp (arg2, "chwear"))
	{
		if (obj->chpoweron != NULL)
			strcpy (buf, obj->chpoweron);
		if (!str_cmp (arg3, "clear"))
		{
			free_string (obj->chpoweron);
			obj->chpoweron = str_dup ("(null)");
		}
		else if (obj->chpoweron != NULL && buf[0] != '\0' && str_cmp (buf, "(null)"))
		{
			if (strlen (buf) + strlen (arg3) >= MAX_STRING_LENGTH - 4)
			{
				send_to_char ("Line too long.\n\r", ch);
				return;
			}
			else
			{
				free_string (obj->chpoweron);
				strcat (buf, "\n\r");
				strcat (buf, arg3);
				obj->chpoweron = str_dup (buf);
			}
		}
		else
		{
			free_string (obj->chpoweron);
			obj->chpoweron = str_dup (arg3);
		}
	}
	else if (!str_cmp (arg2, "chrem"))
	{
		if (obj->chpoweroff != NULL)
			strcpy (buf, obj->chpoweroff);
		if (!str_cmp (arg3, "clear"))
		{
			free_string (obj->chpoweroff);
			obj->chpoweroff = str_dup ("(null)");
		}
		else if (obj->chpoweroff != NULL && buf[0] != '\0' && str_cmp (buf, "(null)"))
		{
			if (strlen (buf) + strlen (arg3) >= MAX_STRING_LENGTH - 4)
			{
				send_to_char ("Line too long.\n\r", ch);
				return;
			}
			else
			{
				free_string (obj->chpoweroff);
				strcat (buf, "\n\r");
				strcat (buf, arg3);
				obj->chpoweroff = str_dup (buf);
			}
		}
		else
		{
			free_string (obj->chpoweroff);
			obj->chpoweroff = str_dup (arg3);
		}
	}
	else if (!str_cmp (arg2, "chuse"))
	{
		if (obj->chpoweruse != NULL)
			strcpy (buf, obj->chpoweruse);
		if (!str_cmp (arg3, "clear"))
		{
			free_string (obj->chpoweruse);
			obj->chpoweruse = str_dup ("(null)");
		}
		else if (obj->chpoweruse != NULL && buf[0] != '\0' && str_cmp (buf, "(null)"))
		{
			if (strlen (buf) + strlen (arg3) >= MAX_STRING_LENGTH - 4)
			{
				send_to_char ("Line too long.\n\r", ch);
				return;
			}
			else
			{
				free_string (obj->chpoweruse);
				strcat (buf, "\n\r");
				strcat (buf, arg3);
				obj->chpoweruse = str_dup (buf);
			}
		}
		else
		{
			free_string (obj->chpoweruse);
			obj->chpoweruse = str_dup (arg3);
		}
	}
	else if (!str_cmp (arg2, "victwear"))
	{
		if (obj->victpoweron != NULL)
			strcpy (buf, obj->victpoweron);
		if (!str_cmp (arg3, "clear"))
		{
			free_string (obj->victpoweron);
			obj->victpoweron = str_dup ("(null)");
		}
		else if (obj->victpoweron != NULL && buf[0] != '\0' && str_cmp (buf, "(null)"))
		{
			if (strlen (buf) + strlen (arg3) >= MAX_STRING_LENGTH - 4)
			{
				send_to_char ("Line too long.\n\r", ch);
				return;
			}
			else
			{
				free_string (obj->victpoweron);
				strcat (buf, "\n\r");
				strcat (buf, arg3);
				obj->victpoweron = str_dup (buf);
			}
		}
		else
		{
			free_string (obj->victpoweron);
			obj->victpoweron = str_dup (arg3);
		}
	}
	else if (!str_cmp (arg2, "victrem"))
	{
		if (obj->victpoweroff != NULL)
			strcpy (buf, obj->victpoweroff);
		if (!str_cmp (arg3, "clear"))
		{
			free_string (obj->victpoweroff);
			obj->victpoweroff = str_dup ("(null)");
		}
		else if (obj->victpoweroff != NULL && buf[0] != '\0' && str_cmp (buf, "(null)"))
		{
			if (strlen (buf) + strlen (arg3) >= MAX_STRING_LENGTH - 4)
			{
				send_to_char ("Line too long.\n\r", ch);
				return;
			}
			else
			{
				free_string (obj->victpoweroff);
				strcat (buf, "\n\r");
				strcat (buf, arg3);
				obj->victpoweroff = str_dup (buf);
			}
		}
		else
		{
			free_string (obj->victpoweroff);
			obj->victpoweroff = str_dup (arg3);
		}
	}
	else if (!str_cmp (arg2, "victuse"))
	{
		if (obj->victpoweruse != NULL)
			strcpy (buf, obj->victpoweruse);
		if (!str_cmp (arg3, "clear"))
		{
			free_string (obj->victpoweruse);
			obj->victpoweruse = str_dup ("(null)");
		}
		else if (obj->victpoweruse != NULL && buf[0] != '\0' && str_cmp (buf, "(null)"))
		{
			if (strlen (buf) + strlen (arg3) >= MAX_STRING_LENGTH - 4)
			{
				send_to_char ("Line too long.\n\r", ch);
				return;
			}
			else
			{
				free_string (obj->victpoweruse);
				strcat (buf, "\n\r");
				strcat (buf, arg3);
				obj->victpoweruse = str_dup (buf);
			}
		}
		else
		{
			free_string (obj->victpoweruse);
			obj->victpoweruse = str_dup (arg3);
		}
	}
	else if (!str_cmp (arg2, "power"))
		obj->specpower = value;
	else
	{
		send_to_char ("No such flag.\n\r", ch);
		return;
	}
	send_to_char ("Ok.\n\r", ch);
	return;
}


void do_oclone (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf2;
	argument = one_argument (argument, arg1);
	if (arg1[0] == '\0')
	{
		send_to_char ("Make a clone of what object?\n\r", ch);
		return;
	}

	if ((obj = get_obj_world (ch, arg1)) == NULL)
	{
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
		return;
	}

	if (!IS_JUDGE (ch) && (obj->questmaker == NULL || str_cmp (ch->name, obj->questmaker) || strlen (obj->questmaker) < 2))
	{
		send_to_char ("You can only clone your own creations.\n\r", ch);
		return;
	}

	pObjIndex = get_obj_index (obj->pIndexData->vnum);
	obj2 = create_object (pObjIndex, obj->level);
	/* Copy any changed parts of the object. */
	free_string (obj2->name);
	obj2->name = str_dup (obj->name);
	free_string (obj2->short_descr);
	obj2->short_descr = str_dup (obj->short_descr);
	free_string (obj2->description);
	obj2->description = str_dup (obj->description);
	if (obj->questmaker != NULL && strlen (obj->questmaker) > 1)
	{
		free_string (obj2->questmaker);
		obj2->questmaker = str_dup (obj->questmaker);
	}

	if (obj->chpoweron != NULL)
	{
		free_string (obj2->chpoweron);
		obj2->chpoweron = str_dup (obj->chpoweron);
	}
	if (obj->chpoweroff != NULL)
	{
		free_string (obj2->chpoweroff);
		obj2->chpoweroff = str_dup (obj->chpoweroff);
	}
	if (obj->chpoweruse != NULL)
	{
		free_string (obj2->chpoweruse);
		obj2->chpoweruse = str_dup (obj->chpoweruse);
	}
	if (obj->victpoweron != NULL)
	{
		free_string (obj2->victpoweron);
		obj2->victpoweron = str_dup (obj->victpoweron);
	}
	if (obj->victpoweroff != NULL)
	{
		free_string (obj2->victpoweroff);
		obj2->victpoweroff = str_dup (obj->victpoweroff);
	}
	if (obj->victpoweruse != NULL)
	{
		free_string (obj2->victpoweruse);
		obj2->victpoweruse = str_dup (obj->victpoweruse);
	}
	obj2->item_type = obj->item_type;
	obj2->extra_flags = obj->extra_flags;
	obj2->wear_flags = obj->wear_flags;
	obj2->weight = obj->weight;
	obj2->spectype = obj->spectype;
	obj2->specpower = obj->specpower;
	obj2->condition = obj->condition;
	obj2->toughness = obj->toughness;
	obj2->resistance = obj->resistance;
	obj2->quest = obj->quest;
	obj2->points = obj->points;
	obj2->cost = obj->cost;
	obj2->value[0] = obj->value[0];
	obj2->value[1] = obj->value[1];
	obj2->value[2] = obj->value[2];
	obj2->value[3] = obj->value[3];
    /*****************************************/
	obj_to_char (obj2, ch);
	if (obj->affected != NULL)
	{
		for (paf = obj->affected; paf != NULL; paf = paf->next)
		{
			if (affect_free == NULL)
				paf2 = alloc_perm (sizeof (*paf));
			else
			{
				paf2 = affect_free;
				affect_free = affect_free->next;
			}
			paf2->type = 0;
			paf2->duration = paf->duration;
			paf2->location = paf->location;
			paf2->modifier = paf->modifier;
			paf2->bitvector = 0;
			paf2->next = obj2->affected;
			obj2->affected = paf2;
		}
	}

	act ("You create a clone of $p.", ch, obj, NULL, TO_CHAR);
	return;
}

void do_clearclas (CHAR_DATA * ch, char *argument)
{
	if (ch->trust < 8)
		send_to_char ("Huh?\n\r", ch);
	else
		send_to_char ("If you want to CLEARCLASS, spell it out.\n\r", ch);
	return;
}


void do_clearclass (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	int i;
	one_argument (argument, arg);
	if (arg[0] == '\0' || is_number (arg))
	{
		send_to_char ("Clear who's clan?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (IS_VAMPIRE (victim) || IS_GHOUL(victim) || IS_INQUISITOR(ch))
		mortal_vamp (victim);

	for(i=0;i<MAX_THAUM_PATHS;i++)
            victim->pcdata->pthaum[i] = 0;
	victim->pcdata->thaum_type = 0;
	victim->pcdata->thaum_prime = -1;
	victim->pcdata->research_power = -1;
	victim->pcdata->research_time = 0;
        victim->pcdata->research_tick = 0;
	victim->pcdata->road = -1;
	victim->blood[BLOOD_POOL] = 10;
	victim->blood[BLOOD_CURRENT] = 10;
	victim->blood[BLOOD_POTENCY] = 1;
	victim->class = CLASS_NONE;
	free_string (victim->lord);
	victim->lord = str_dup ("");
	free_string (victim->clan);
	victim->clan = str_dup ("");
	victim->vampgen = 0;
	victim->vampaff = 0;
	victim->vamppass = -1;
	victim->pcdata->aggdamage = 0;
	victim->pcdata->bashingdamage = 0;
	victim->pcdata->lethaldamage = 0;
	victim->pcdata->virtues[VIRTUE_CONSCIENCE] = 1;
	victim->pcdata->virtues[VIRTUE_SELFCONTROL] = 1;
	victim->pcdata->virtues[VIRTUE_COURAGE] = 1;
	victim->pcdata->virtues[VIRTUE_WISDOM] = 1;
	victim->pcdata->virtues[VIRTUE_FAITH] = 1;
	victim->pcdata->virtues[VIRTUE_ZEAL] = 1;
	victim->pcdata->willpower[WILLPOWER_CURRENT] = 1;
	victim->pcdata->willpower[WILLPOWER_MAX] = 1;
	victim->pcdata->cland[0] = -1;
	victim->pcdata->cland[1] = -1;
	victim->pcdata->cland[2] = -1;
	for (i = 0; i <= 2; i++)
		victim->pcdata->cland[i] = 0;
	for (i = 0; i <= BREED_MAX; i++)
		victim->pcdata->breed[i] = 0;
	for (i = 0; i <= AUSPICE_MAX; i++)
		victim->pcdata->auspice[i] = 0;
	for (i = 0; i <= DISC_MAX; i++)
		victim->pcdata->powers[i] = 0;
	for (i = 0; i <= TRIBE_MAX; i++)
		victim->pcdata->tribes[i] = 0;
	REMOVE_BIT (victim->extra, EXTRA_SIRE);
	REMOVE_BIT (victim->extra, EXTRA_PRINCE);
	REMOVE_BIT (victim->act, PLR_CHAMPION);
	victim->pcdata->wolf = 0;
	if (!IS_IMMORTAL (victim))
	{
		victim->level = LEVEL_AVATAR;
		victim->trust = 0;
	}
	send_to_char ("Ok.\n\r", ch);
	send_to_char ("Your class has been cleared.\n\r", victim);
	return;
}

/*void do_call (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	CHAR_DATA *victim = NULL;
	ROOM_INDEX_DATA *chroom;
	ROOM_INDEX_DATA *objroom;
	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("What object do you wish to call?\n\r", ch);
		return;
	}

	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	act ("Your eyes flicker with yellow energy.", ch, NULL, NULL, TO_CHAR);
	act ("$n's eyes flicker with yellow energy.", ch, NULL, NULL, TO_ROOM);

	if (!str_cmp (arg, "all"))
	{
		call_all (ch);
		return;
	}

	if ((obj = get_obj_world (ch, arg)) == NULL)
	{
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
		return;
	}

	if (obj->questowner == NULL || strlen (obj->questowner) < 2 || str_cmp (obj->questowner, ch->name) || obj->item_type == ITEM_PAGE || obj->item_type == ITEM_CORPSE_PC)
	{
		send_to_char ("Nothing happens.\n\r", ch);
		return;
	}

	if (obj->carried_by != NULL && obj->carried_by != ch)
	{
		victim = obj->carried_by;
		if (!IS_NPC (victim) && victim->desc != NULL && victim->desc->connected != CON_PLAYING)
			return;
		act ("$p suddenly vanishes from your hands!", victim, obj, NULL, TO_CHAR);
		act ("$p suddenly vanishes from $n's hands!", victim, obj, NULL, TO_ROOM);
		obj_from_char (obj);
	}
	else if (obj->in_room != NULL)
	{
		chroom = ch->in_room;
		objroom = obj->in_room;
		char_from_room (ch);
		char_to_room (ch, objroom);
		act ("$p vanishes from the ground!", ch, obj, NULL, TO_ROOM);
		if (chroom == objroom)
			act ("$p vanishes from the ground!", ch, obj, NULL, TO_CHAR);
		char_from_room (ch);
		char_to_room (ch, chroom);
		obj_from_room (obj);
	}
	else if (obj->in_obj != NULL)
		obj_from_obj (obj);
	else
	{
		send_to_char ("Nothing happens.\n\r", ch);
		return;
	}

	obj_to_char (obj, ch);
	if (IS_SET (obj->extra_flags, ITEM_SHADOWPLANE))
		REMOVE_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
	act ("$p materializes in your hands.", ch, obj, NULL, TO_CHAR);
	act ("$p materializes in $n's hands.", ch, obj, NULL, TO_ROOM);
	do_autosave (ch, "");
	if (victim != NULL)
		do_autosave (victim, "");
	return;
}
*/
void call_all (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	CHAR_DATA *victim = NULL;
	DESCRIPTOR_DATA *d;
	ROOM_INDEX_DATA *chroom;
	ROOM_INDEX_DATA *objroom;
	bool found = FALSE;
	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (obj->questowner == NULL || strlen (obj->questowner) < 2 || str_cmp (ch->name, obj->questowner) || obj->item_type == ITEM_PAGE || obj->item_type == ITEM_CORPSE_PC)
			continue;
		found = TRUE;
		for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);
		if (in_obj->carried_by != NULL)
		{
			if (in_obj->carried_by == ch)
				continue;
		}

		if (obj->carried_by != NULL)
		{
			if (obj->carried_by == ch || obj->carried_by->desc == NULL || obj->carried_by->desc->connected != CON_PLAYING)
			{
				if (!IS_NPC (obj->carried_by))
					return;
			}
			act ("$p suddenly vanishes from your hands!", obj->carried_by, obj, NULL, TO_CHAR);
			act ("$p suddenly vanishes from $n's hands!", obj->carried_by, obj, NULL, TO_ROOM);
			SET_BIT (obj->carried_by->extra, EXTRA_CALL_ALL);
			obj_from_char (obj);
		}
		else if (obj->in_room != NULL)
		{
			chroom = ch->in_room;
			objroom = obj->in_room;
			char_from_room (ch);
			char_to_room (ch, objroom);
			act ("$p vanishes from the ground!", ch, obj, NULL, TO_ROOM);
			if (chroom == objroom)
				act ("$p vanishes from the ground!", ch, obj, NULL, TO_CHAR);
			char_from_room (ch);
			char_to_room (ch, chroom);
			obj_from_room (obj);
		}
		else if (obj->in_obj != NULL)
			obj_from_obj (obj);
		else
			continue;
		obj_to_char (obj, ch);
		if (IS_SET (obj->extra_flags, ITEM_SHADOWPLANE))
			REMOVE_BIT (obj->extra_flags, ITEM_SHADOWPLANE);

		act ("$p materializes in your hands.", ch, obj, NULL, TO_CHAR);
		act ("$p materializes in $n's hands.", ch, obj, NULL, TO_ROOM);
	}

	if (!found)
		send_to_char ("Nothing happens.\n\r", ch);
	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected != CON_PLAYING)
			continue;
		if ((victim = d->character) == NULL)
			continue;
		if (IS_NPC (victim))
			continue;
		if (ch != victim && !IS_EXTRA (victim, EXTRA_CALL_ALL))
			continue;
		REMOVE_BIT (victim->extra, EXTRA_CALL_ALL);
		do_autosave (victim, "");
	}
	return;
}

void do_artifact (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	found = FALSE;
	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		found = TRUE;
		for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);
		if (in_obj->carried_by != NULL)
		{
			sprintf (buf, "%s created by %s and carried by %s.\n\r", obj->short_descr, obj->questmaker, PERS (in_obj->carried_by, ch));
		}
		else
		{
			sprintf (buf, "%s created by %s and in %s.\n\r", obj->short_descr, obj->questmaker, in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name);
		}

		buf[0] = UPPER (buf[0]);
		send_to_char (buf, ch);
	}

	if (!found)
		send_to_char ("There are no artifacts in the game.\n\r", ch);
	return;
}

void do_locate (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	found = FALSE;
	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (!can_see_obj (ch, obj) || obj->questowner == NULL || strlen (obj->questowner) < 2 || str_cmp (ch->name, obj->questowner))
			continue;
		found = TRUE;
		for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj);
		if (in_obj->carried_by != NULL)
		{
			sprintf (buf, "%s carried by %s.\n\r", obj->short_descr, PERS (in_obj->carried_by, ch));
		}
		else
		{
			sprintf (buf, "%s in %s.\n\r", obj->short_descr, in_obj->in_room == NULL ? "somewhere" : in_obj->in_room->name);
		}

		buf[0] = UPPER (buf[0]);
		send_to_char (buf, ch);
	}

	if (!found)
		send_to_char ("You cannot locate any items belonging to you.\n\r", ch);
	return;
}

void do_claim (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	one_argument (argument, arg);
	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	if (ch->exp < 5)
	{
		send_to_char ("It costs 5 exp to claim ownership of an item.\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("What object do you wish to claim ownership of?\n\r", ch);
		return;
	}

	if ((obj = get_obj_carry (ch, arg)) == NULL)
	{
		send_to_char ("You are not carrying that item.\n\r", ch);
		return;
	}

	if (obj->item_type == ITEM_QUEST || obj->item_type == ITEM_AMMO || obj->item_type == ITEM_EGG || obj->item_type == ITEM_VOODOO || obj->item_type == ITEM_MONEY || obj->item_type == ITEM_TREASURE || obj->item_type == ITEM_PAGE)
	{
		send_to_char ("You cannot claim that item.\n\r", ch);
		return;
	}
	else if (obj->chobj != NULL && !IS_NPC (obj->chobj) && obj->chobj->pcdata->obj_vnum != 0)
	{
		send_to_char ("You cannot claim that item.\n\r", ch);
		return;
	}

	if (obj->questowner != NULL && strlen (obj->questowner) > 1)
	{
		if (!str_cmp (ch->name, obj->questowner))
			send_to_char ("But you already own it!\n\r", ch);
		else
			send_to_char ("Someone else has already claimed ownership to it.\n\r", ch);
		return;
	}

	ch->exp -= 5;
	if (obj->questowner != NULL)
		free_string (obj->questowner);
	obj->questowner = str_dup (ch->name);
	act ("You are now the owner of $p.", ch, obj, NULL, TO_CHAR);
	act ("$n is now the owner of $p.", ch, obj, NULL, TO_ROOM);
	return;
}

void do_gift (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	CHAR_DATA *victim;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (IS_NPC (ch))
	{
		send_to_char ("Not while switched.\n\r", ch);
		return;
	}

	if (ch->exp < 5)
	{
		send_to_char ("It costs 5 exp to make a gift of an item.\n\r", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Make a gift of which object to whom?\n\r", ch);
		return;
	}

	if ((obj = get_obj_carry (ch, arg1)) == NULL)
	{
		send_to_char ("You are not carrying that item.\n\r", ch);
		return;
	}
	if ((victim = get_char_room (ch, arg2)) == NULL)
	{
		send_to_char ("Nobody here by that name.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	if (obj->questowner == NULL || strlen (obj->questowner) < 2)
	{
		send_to_char ("That item has not yet been claimed.\n\r", ch);
		return;
	}
	if (str_cmp (ch->name, obj->questowner))
	{
		send_to_char ("But you don't own it!\n\r", ch);
		return;
	}
	ch->exp -= 5;
	if (obj->questowner != NULL)
		free_string (obj->questowner);
	obj->questowner = str_dup (victim->name);
	act ("You grant ownership of $p to $N.", ch, obj, victim, TO_CHAR);
	act ("$n grants ownership of $p to $N.", ch, obj, victim, TO_NOTVICT);
	act ("$n grants ownership of $p to you.", ch, obj, victim, TO_VICT);
	return;
}

void do_create (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	int itemtype = 13;
	int level;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0')
		itemtype = ITEM_TRASH;
	else if (!str_cmp (arg1, "light"))
		itemtype = ITEM_LIGHT;
	else if (!str_cmp (arg1, "scroll"))
		itemtype = ITEM_SCROLL;
	else if (!str_cmp (arg1, "wand"))
		itemtype = ITEM_WAND;
	else if (!str_cmp (arg1, "staff"))
		itemtype = ITEM_STAFF;
	else if (!str_cmp (arg1, "weapon"))
		itemtype = ITEM_WEAPON;
	else if (!str_cmp (arg1, "treasure"))
		itemtype = ITEM_TREASURE;
	else if (!str_cmp (arg1, "armor"))
		itemtype = ITEM_ARMOR;
	else if (!str_cmp (arg1, "armour"))
		itemtype = ITEM_ARMOR;
	else if (!str_cmp (arg1, "potion"))
		itemtype = ITEM_POTION;
	else if (!str_cmp (arg1, "furniture"))
		itemtype = ITEM_FURNITURE;
	else if (!str_cmp (arg1, "trash"))
		itemtype = ITEM_TRASH;
	else if (!str_cmp (arg1, "container"))
		itemtype = ITEM_CONTAINER;
	else if (!str_cmp (arg1, "drink"))
		itemtype = ITEM_DRINK_CON;
	else if (!str_cmp (arg1, "key"))
		itemtype = ITEM_KEY;
	else if (!str_cmp (arg1, "food"))
		itemtype = ITEM_FOOD;
	else if (!str_cmp (arg1, "money"))
		itemtype = ITEM_MONEY;
	else if (!str_cmp (arg1, "boat"))
		itemtype = ITEM_BOAT;
	else if (!str_cmp (arg1, "corpse"))
		itemtype = ITEM_CORPSE_NPC;
	else if (!str_cmp (arg1, "fountain"))
		itemtype = ITEM_FOUNTAIN;
	else if (!str_cmp (arg1, "pill"))
		itemtype = ITEM_PILL;
	else if (!str_cmp (arg1, "portal"))
		itemtype = ITEM_PORTAL;
	else if (!str_cmp (arg1, "egg"))
		itemtype = ITEM_EGG;
	else if (!str_cmp (arg1, "stake"))
		itemtype = ITEM_STAKE;
	else if (!str_cmp (arg1, "missile"))
		itemtype = ITEM_MISSILE;
	else
		itemtype = ITEM_TRASH;
	if (arg2[0] == '\0' || !is_number (arg2))
	{
		level = 0;
	}
	else
	{
		level = atoi (arg2);
		if (level < 1 || level > 50)
		{
			send_to_char ("Level should be within range 1 to 50.\n\r", ch);
			return;
		}
	}

	if ((pObjIndex = get_obj_index (OBJ_VNUM_PROTOPLASM)) == NULL)
	{
		send_to_char ("Error...missing object, please inform KaVir.\n\r", ch);
		return;
	}

	obj = create_object (pObjIndex, level);
	obj->level = level;
	obj->item_type = itemtype;
	obj_to_char (obj, ch);
	if (obj->questmaker != NULL)
		free_string (obj->questmaker);
	obj->questmaker = str_dup (ch->name);
	act ("You reach up into the air and draw out a ball of protoplasm.", ch, obj, NULL, TO_CHAR);
	act ("$n reaches up into the air and draws out a ball of protoplasm.", ch, obj, NULL, TO_ROOM);
	return;
}



void do_mclear (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int disc;
	argument = one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax: mclear <char>.\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("That player is not here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	for (obj = victim->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->wear_loc != WEAR_NONE)
		{
			obj_from_char (obj);
			obj_to_char (obj, victim);
		}
	}
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
	victim->armor = 100;
	victim->hit = UMAX (1, victim->hit);
	victim->mana = UMAX (1, victim->mana);
	victim->move = UMAX (1, victim->move);
	victim->hitroll = 0;
	victim->damroll = 0;
	victim->saving_throw = 0;
	victim->pcdata->demonic = 0;
	victim->pcdata->atm = 0;
	victim->pcdata->followers = 0;
	victim->pcdata->powers_set_wear = 0;
	send_to_char ("Your stats have been cleared.  Please rewear your equipment.\n\r", victim);
	send_to_char ("Ok.\n\r", ch);
	if (victim->polyaff > 0)
	{
		if (IS_POLYAFF (victim, POLY_BAT) || IS_POLYAFF (victim, POLY_WOLF) || IS_POLYAFF (victim, POLY_MIST) || IS_POLYAFF (victim, POLY_SERPENT) || IS_POLYAFF (victim, POLY_RAVEN) || IS_POLYAFF (victim, POLY_FISH) || IS_POLYAFF (victim, POLY_FROG) || IS_POLYAFF (victim, POLY_SHADOW))
		{
			do_autosave (victim, "");
			return;
		}
	}
	if (IS_VAMPIRE (victim) && victim->polyaff < 1)
	{
		for (disc = DISC_ANIMALISM; disc <= DISC_MAX; disc++)
		{
			victim->pcdata->powers_mod[disc] = 0;
			victim->pcdata->powers_set[disc] = 0;
		}
	}
	do_autosave (victim, "");
	return;
}

void do_clearstats (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int disc;
	if (IS_NPC (ch))
		return;
	if (ch->in_room == NULL || ch->in_room->vnum != 3054)
	{
		send_to_char ("You can only clear your stats at the Temple Altar of Midgaard.\n\r", ch);
		return;
	}

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->wear_loc != WEAR_NONE)
		{
			obj_from_char (obj);
			obj_to_char (obj, ch);
		}
	}
	while (ch->affected)
		affect_remove (ch, ch->affected);
	if (!IS_AFFECTED (ch, AFF_POLYMORPH))
	{
		free_string (ch->long_descr);
		ch->long_descr = str_dup ("");
	}
	if (IS_AFFECTED (ch, AFF_POLYMORPH) && IS_AFFECTED (ch, AFF_ETHEREAL))
	{
		ch->affected_by = AFF_POLYMORPH + AFF_ETHEREAL;
	}
	else if (IS_AFFECTED (ch, AFF_POLYMORPH))
		ch->affected_by = AFF_POLYMORPH;
	else if (IS_AFFECTED (ch, AFF_ETHEREAL))
		ch->affected_by = AFF_ETHEREAL;
	else
		ch->affected_by = 0;
	ch->armor = 100;
	ch->hit = UMAX (1, ch->hit);
	ch->mana = UMAX (1, ch->mana);
	ch->move = UMAX (1, ch->move);
	ch->hitroll = 0;
	ch->damroll = 0;
	ch->saving_throw = 0;
	ch->pcdata->demonic = 0;
	ch->pcdata->atm = 0;
	ch->pcdata->followers = 0;
	ch->pcdata->powers_set_wear = 0;
	send_to_char ("Your stats have been cleared.  Please rewear your equipment.\n\r", ch);
	if (ch->polyaff > 0)
	{
		if (IS_POLYAFF (ch, POLY_BAT) || IS_POLYAFF (ch, POLY_WOLF) || IS_POLYAFF (ch, POLY_MIST) || IS_POLYAFF (ch, POLY_SERPENT) || IS_POLYAFF (ch, POLY_RAVEN) || IS_POLYAFF (ch, POLY_FISH) || IS_POLYAFF (ch, POLY_FROG) || IS_POLYAFF (ch, POLY_SHADOW))
		{
			do_autosave (ch, "");
			return;
		}
	}
	if (IS_VAMPIRE (ch) && ch->polyaff < 1)
	{
		for (disc = DISC_ANIMALISM; disc <= DISC_MAX; disc++)
		{
			ch->pcdata->powers_mod[disc] = 0;
			ch->pcdata->powers_set[disc] = 0;
		}
	}
	do_autosave (ch, "");
	return;
}

void do_otransfer (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	CHAR_DATA *victim;
	ROOM_INDEX_DATA *chroom;
	ROOM_INDEX_DATA *objroom;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0')
	{
		send_to_char ("Otransfer which object?\n\r", ch);
		return;
	}

	if (arg2[0] == '\0')
		victim = ch;
	else if ((victim = get_char_world (ch, arg2)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if ((obj = get_obj_world (ch, arg1)) == NULL)
	{
		send_to_char ("Nothing like that in hell, earth, or heaven.\n\r", ch);
		return;
	}

	if (!IS_JUDGE (ch) && (obj->questmaker == NULL || str_cmp (ch->name, obj->questmaker) || strlen (obj->questmaker) < 2))
	{
		send_to_char ("You don't have permission to otransfer that item.\n\r", ch);
		return;
	}

	if (obj->carried_by != NULL)
	{
		act ("$p vanishes from your hands in an explosion of energy.", obj->carried_by, obj, NULL, TO_CHAR);
		act ("$p vanishes from $n's hands in an explosion of energy.", obj->carried_by, obj, NULL, TO_ROOM);
		obj_from_char (obj);
	}
	else if (obj->in_obj != NULL)
		obj_from_obj (obj);
	else if (obj->in_room != NULL)
	{
		chroom = ch->in_room;
		objroom = obj->in_room;
		char_from_room (ch);
		char_to_room (ch, objroom);
		act ("$p vanishes from the ground in an explosion of energy.", ch, obj, NULL, TO_ROOM);
		if (chroom == objroom)
			act ("$p vanishes from the ground in an explosion of energy.", ch, obj, NULL, TO_CHAR);
		char_from_room (ch);
		char_to_room (ch, chroom);
		obj_from_room (obj);
	}
	else
	{
		send_to_char ("You were unable to get it.\n\r", ch);
		return;
	}
	obj_to_char (obj, victim);
	act ("$p appears in your hands in an explosion of energy.", victim, obj, NULL, TO_CHAR);
	act ("$p appears in $n's hands in an explosion of energy.", victim, obj, NULL, TO_ROOM);
	return;
}

void bind_char (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_INDEX_DATA *pObjIndex;
	ROOM_INDEX_DATA *location;
	if (IS_NPC (ch) || ch->pcdata->obj_vnum < 1)
		return;
	if ((pObjIndex = get_obj_index (ch->pcdata->obj_vnum)) == NULL)
		return;
	if (ch->in_room == NULL || ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
	{
		location = get_room_index (ROOM_VNUM_LIMBO);
		char_from_room (ch);
		char_to_room (ch, location);
	}
	else
		location = ch->in_room;
	obj = create_object (pObjIndex, 50);
	obj_to_room (obj, location);
	obj->chobj = ch;
	ch->pcdata->chobj = obj;
	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->extra, EXTRA_OSWITCH);
	free_string (ch->morph);
	ch->morph = str_dup (obj->short_descr);
	send_to_char ("You reform yourself.\n\r", ch);
	act ("$p fades into existance on the floor.", ch, obj, NULL, TO_ROOM);
	do_look (ch, "auto");
	return;
}

void do_bind (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	CHAR_DATA *victim;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: bind <player> <object>\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg1)) == NULL)
	{
		send_to_char ("That player is not here.\n\r", ch);
		return;
	}
	if (ch == victim)
	{
		send_to_char ("You can't do this to yourself.\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	else if (IS_AFFECTED (victim, AFF_POLYMORPH))
	{
		send_to_char ("You cannot do this while they are polymorphed.\n\r", ch);
		return;
	}
	else if (IS_IMMORTAL (victim))
	{
		send_to_char ("Only on mortals or avatars.\n\r", ch);
		return;
	}
	if ((obj = get_obj_carry (ch, arg2)) == NULL)
	{
		send_to_char ("You are not carrying that item.\n\r", ch);
		return;
	}
	if (obj->questmaker != NULL && strlen (obj->questmaker) > 1)
	{
		send_to_char ("You cannot bind someone into a modified item.\n\r", ch);
		return;
	}
	if (obj->chobj != NULL)
	{
		send_to_char ("That item already has someone bound in it.\n\r", ch);
		return;
	}
	send_to_char ("Ok.\n\r", ch);
	act ("$n transforms into a white vapour and pours into $p.", victim, obj, NULL, TO_ROOM);
	act ("You transform into a white vapour and pour into $p.", victim, obj, NULL, TO_CHAR);
	victim->pcdata->obj_vnum = obj->pIndexData->vnum;
	obj->chobj = victim;
	victim->pcdata->chobj = obj;
	SET_BIT (victim->affected_by, AFF_POLYMORPH);
	SET_BIT (victim->extra, EXTRA_OSWITCH);
	free_string (victim->morph);
	victim->morph = str_dup (obj->short_descr);
	return;
}

void do_morph (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *morph;
	int mnum;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (arg1[0] == '\0')
	{
		send_to_char ("Syntax: morph <object> <form>\n\r", ch);
		return;
	}

	if ((obj = get_obj_wear (ch, arg1)) == NULL)
	{
		if ((obj = get_obj_carry (ch, arg1)) == NULL)
		{
			send_to_char ("You are not carrying that item.\n\r", ch);
			return;
		}
	}

	if (arg2[0] == '\0')
	{
		act ("$p can morph into the following forms:", ch, obj, NULL, TO_CHAR);
		show_list_to_char (obj->contains, ch, TRUE, TRUE);
		return;
	}

	if ((morph = get_obj_list (ch, arg2, obj->contains)) == NULL)
	{
		send_to_char ("It cannot assume that form.\n\r", ch);
		return;
	}
	obj_from_obj (morph);
	obj_to_char (morph, ch);
	act ("$p morphs into $P in $n's hands!", ch, obj, morph, TO_ROOM);
	act ("$p morphs into $P in your hands!", ch, obj, morph, TO_CHAR);
	mnum = obj->wear_loc;
	obj_from_char (obj);
	obj_to_obj (obj, morph);
	if (morph->wear_flags == obj->wear_flags && mnum != WEAR_NONE)
		equip_char (ch, morph, mnum);
	return;
}

void do_move (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *mount;
	one_argument (argument, arg);
	if (!IS_NPC (ch) || ch->wizard == NULL || !IS_AFFECTED (ch, AFF_ETHEREAL))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Move where?\n\r", ch);
		return;
	}

	if ((location = find_location (ch, arg)) == NULL)
	{
		send_to_char ("No such location.\n\r", ch);
		return;
	}

	if (room_is_private (location))
	{
		send_to_char ("That room is private right now.\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
		
	char_from_room (ch);
	char_to_room (ch, location);
	do_look (ch, "auto");
	if ((mount = ch->mount) == NULL)
		return;
	char_from_room (mount);
	char_to_room (mount, ch->in_room);
	do_look (mount, "auto");
	return;
}

void do_shell (CHAR_DATA * ch, char *argument)
{
/*
	 system( argument );
*/
	return;
}

/***************** COPYOVER CRAP **************************/


#define CH(descriptor) ((descriptor)->original ? (descriptor)->original : (descriptor)->character)

extern int port, control;
void do_copyover (CHAR_DATA * ch, char *argument)
{
	FILE *fp;
	DESCRIPTOR_DATA *d, *d_next;
	char buf[100], buf2[100];
	fp = fopen (COPYOVER_FILE, "w");
	if (!fp)
	{
		send_to_char ("copyover file not writeable, aborted.\n\r", ch);
		perror ("do_copyover:fopen");
		return;
	}

	sprintf (buf, "\n\r ** COPYOVER by %s - hold onto yer undies! ** \n\r", ch->name);
	fprintf(fp, "%ld\n", uptime);
	for (d = descriptor_list; d; d = d_next)
	{
		CHAR_DATA *och = CH (d);
		d_next = d->next;
		if (!d->character || (d->connected > CON_PLAYING && d->connected != CON_CREATEVAMPIRE))
		{
			write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting.  Come back in a few minutes.\n\r", 0);
			close_socket (d);
		}
		else
		{
			fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
			if (d->connected == CON_CREATEVAMPIRE)
			{
				write_to_descriptor (d->descriptor, "\n\rSorry we had to do a copyover, while you were in vampire creation.\n\r", 0);
				write_to_descriptor (d->descriptor, "You have been placed in the OOC room, when we are back, please type.\n\r", 0);
				write_to_descriptor (d->descriptor, "'vampirecreation' again, and start over.  Thank you for your cooperation.\n\r", 0);
				d->character->pcdata->extended_char_creation_in_progress = TRUE;
			}
			else
				d->character->pcdata->extended_char_creation_in_progress = FALSE;
			if (IS_SET (och->act, PLR_WOLFMAN))
				do_unwerewolf (och, "");
			if (och->mkill < 5)
			{
				och->mkill = 5;
				och->level = 2;
			}
			
			save_char_obj (och);
		}
	}
	fprintf (fp, "-1\n");
	fclose (fp);
	fclose (fpReserve);
	sprintf (buf, "%d", port);
	sprintf (buf2, "%d", control);
	execl (EXE_FILE, "asm", buf, "copyover", buf2, (char *) NULL);
	perror ("do_copyover: execl");
	send_to_char ("Copyover FAILED!\n\r", ch);
}

void copyover_recover ()
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name[100];
	char host[MAX_STRING_LENGTH];
	int desc;
	bool fOld;
	fp = fopen (COPYOVER_FILE, "r");
	if (!fp)
	{
		perror ("copover_recover:fopen");
		exit (1);
	}

	fscanf(fp, "%ld\n", &uptime);
	for (;;)
	{
		fscanf (fp, "%d %s %s\n", &desc, name, host);
		if (desc == -1)
			break;
		if (!write_to_descriptor (desc, "\n\rCopyover initiated. Please keep your panties securely in place.\n\r ", 0))
		{
			close (desc);
			continue;
		}

		d = alloc_perm (sizeof (DESCRIPTOR_DATA));
		init_descriptor (d, desc);
		d->host = str_dup (host);
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_COPYOVER_RECOVER;
		fOld = load_char_obj (d, name);
		if (!fOld)
		{
			write_to_descriptor (desc, "Sorry, your character got eaten by the copyover ghost. contact an admin.\n\r", 0);
			close_socket (d);
		}
		else
		{
			write_to_descriptor (desc, "Copyover complete, you may now release your undergarments.\n\r", 0);
			if (!d->character->in_room)
				d->character->in_room = get_room_index (ROOM_VNUM_OOC);
			d->character->next = char_list;
			char_list = d->character;


			char_to_room (d->character, d->character->in_room);
			do_look (d->character, "");
			if (d->showstr_head)
			{
				free_string(d->showstr_head);
				d->showstr_head = str_dup ("");
			}
			d->showstr_point = 0;
			act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
			d->connected = CON_PLAYING;
		}
	}
	fclose (fp);
}

void do_smite (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];	/* Lot of arguments */
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	char arg6[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *SmittenEQ;
	const int MAX_SMITE_FRACTION = 95;	/* You can change this if you want */
	int hp_percent = 0;
	int mana_percent = 0;
	int move_percent = 0;
	int pos = 0;
	int eq = 0;
	argument = one_argument (argument, arg1);	/* Combine the arguments */
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	argument = one_argument (argument, arg4);
	argument = one_argument (argument, arg5);
	argument = one_argument (argument, arg6);
	if (IS_NPC (ch))	/* NPCs may get to be smited, but switch immortals can't */
		return;
	if (arg1[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("Smite <victim> <hp> <mana> <move> <position> <equipment>\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They are saved only through their abscence.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim) && victim->level >= get_trust (ch))
	{
		send_to_char ("Your reach exceeds your grasp.\n\r", ch);
		return;
	}

	if (arg2[0] != '\0')
		hp_percent = atoi (arg2);
	else
		hp_percent = 50;
	if (hp_percent > MAX_SMITE_FRACTION || hp_percent < 0)
	{
		send_to_char ("Hp percent must be between 0 and 95.\n\r", ch);
		return;
	}

	if (arg3[0] != '\0')
		mana_percent = atoi (arg3);
	else
		mana_percent = 0;
	if (mana_percent > MAX_SMITE_FRACTION || mana_percent < 0)
	{
		send_to_char ("Mana percent must be between 0 and 95.\n\r", ch);
		return;
	}

	if (arg4[0] != '\0')
		move_percent = atoi (arg4);
	else
		move_percent = 0;
	if (move_percent > MAX_SMITE_FRACTION || move_percent < 0)
	{
		send_to_char ("Move percent must be between 0 and 95.\n\r", ch);
		return;
	}

	/* Customize stuff by alignment */

	if (ch->alignment > 300)
	{
		act ("Your actions have brought the holy power of $n upon you!", ch, NULL, victim, TO_VICT);
		act ("$N has brought the holy power of $n upon themselves!", ch, NULL, victim, TO_NOTVICT);
	}
	if (ch->alignment > -301 && ch->alignment < 301)
	{
		act ("Your actions have called the divine fury of $n upon you!", ch, NULL, victim, TO_VICT);
		act ("$N has called the divine fury of $n upon themselves!", ch, NULL, victim, TO_NOTVICT);
	}
	if (ch->alignment < -300)
	{
		act ("You are struck down by the infernal power of $n!!", ch, NULL, victim, TO_VICT);
		act ("The hellspawned, infernal power of $n has struck down $N!!", ch, NULL, victim, TO_NOTVICT);
	}

	/* If it REALLY hurt */

	if (hp_percent > 75 && victim->hit > victim->max_hit / 4)
		send_to_char ("#1That really did HURT!#n\n\r", victim);
	/* Let's see if equipment needs to be 'blown away' */
	for (eq = 0; eq_table[eq] != NULL; eq++)
	{
		if (!str_prefix (eq_table[eq], arg6))
		{
			if ((SmittenEQ = get_eq_char (victim, eq)) != NULL)
			{
				sprintf (buf, "$n's %s is blown away!", SmittenEQ->short_descr);
				act (buf, victim, NULL, NULL, TO_ROOM);
				sprintf (buf, "Your %s is blown away!\n\r", SmittenEQ->short_descr);
				send_to_char (buf, victim);
				unequip_char (victim, SmittenEQ);
				obj_from_char (SmittenEQ);
				obj_to_room (SmittenEQ, victim->in_room);
			}
			break;
		}
	}

	/* Let's see what position to put the victim in */

	for (pos = 0; pos_table[pos] != NULL; pos++)
	{
		if (!str_prefix (pos_table[pos], arg5))
		{
			victim->position = pos;	/* This only works because of the way the pos_table is arranged. */
			if (victim->position == POS_FIGHTING)
				victim->position = POS_STANDING;	/* POS_FIGHTING is bad */
			if (victim->position < POS_STUNNED)
				victim->position = POS_STUNNED;
			if (victim->position == POS_STUNNED)
			{
				act ("$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM);
				send_to_char ("You are stunned, but will probably recover.\n\r", victim);
			}

			if (victim->position == POS_RESTING || victim->position == POS_SITTING)
			{
				act ("$n is knocked onto $s butt!", victim, NULL, NULL, TO_ROOM);
				send_to_char ("You are knocked onto your butt!\n\r", victim);
			}
			break;
		}
	}

	/* Calculate total hp loss */
	victim->hit -= ((victim->hit * hp_percent) / 100);
	if (victim->hit < 1)
		victim->hit = 1;
	/* Calculate total mana loss */
	victim->mana -= ((victim->mana * mana_percent) / 100);
	if (victim->mana < 1)
		victim->mana = 1;
	/* Calculate total move loss */
	victim->move -= ((victim->move * move_percent) / 100);
	if (victim->move < 1)
		victim->move = 1;
	send_to_char ("Your will is done, your power felt.\n\r", ch);
	return;
}



void do_dnd (CHAR_DATA * ch, char *argument)
{

	char buf[MAX_STRING_LENGTH];
	if (IS_NPC (ch))
		return;
	if (IS_SET (ch->extra2, EXTRA2_DND))
	{
		send_to_char ("You no longer are flagged DND.\n\r", ch);
		sprintf (buf, "%s no longer has the DND flag on.\n\r", ch->name);
		do_info (ch, buf);
		REMOVE_BIT (ch->extra2, EXTRA2_DND);
		return;
	}
	if (!IS_SET (ch->extra2, EXTRA2_DND))
	{
		send_to_char ("You have now flagged yourself DND.\n\r", ch);
		sprintf (buf, "%s is DND", ch->name);
		do_info (ch, buf);
		SET_BIT (ch->extra2, EXTRA2_DND);
		return;
	}

	return;
}

void do_pstat (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	argument = one_argument (argument, arg1);
	/*int i;*/

	
	if (arg1[0] == '\0')
	{
		send_to_char ("Who do you wish to violate?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_NPC(victim))
	{
		send_to_char ("Use mstat on NPCs.\n\r", ch);
		return;
	}


	sprintf (buf, "#G(#eName#G)#c %s\n\r", victim->name);
	send_to_char (buf, ch);
	sprintf (buf, "#G(#eEmail#G)#c %s\n\r", victim->pcdata->email);
	send_to_char (buf, ch );
		sprintf (buf, "#G(#eRoomdesc#G)#c %s\n\r", victim->pcdata->roomdesc);
	send_to_char (buf, ch );
	sprintf (buf, "#G(#eHours#G)#c %d\n\r", (get_age (victim) - 17) * 2);
	send_to_char (buf, ch );
	sprintf (buf, "#G(#eGender#G)#c %s  #G(#eRoom#G)#c %d\n\r", victim->sex == SEX_MALE ? "Male" : victim->sex == SEX_FEMALE ? "Female" : "None", victim->in_room == NULL ? 0 : victim->in_room->vnum);
	send_to_char (buf, ch);
	sprintf (buf, "#G(#eStr#G)#c %d  #G(#eDex#G)#c %d  #G(#eSta#G)#c %d  #G(#eCha#G)#c %d  #G(#eMan#G)#c %d  #G(#eApp#G)#c %d  #G(#ePer#G)#c %d  #G(#eInt#G)#c %d  #G(#eWit#G)#c %d\n\r", get_curr_str (victim), get_curr_dex (victim), get_curr_sta (victim), get_curr_cha (victim), get_curr_man (victim), get_curr_app (victim), get_curr_per (victim), get_curr_int (victim), get_curr_wit (victim));
	send_to_char (buf, ch);
	sprintf (buf, "#G(#eAgg#G)#c %d  #G(#eBash#G)#c %d  #G(#eLethal#G)#c %d  #G(#eTotal#G)#c %d\n\r", victim->pcdata->aggdamage, victim->pcdata->bashingdamage, victim->pcdata->lethaldamage, (victim->pcdata->aggdamage + victim->pcdata->bashingdamage + victim->pcdata->lethaldamage));
	send_to_char (buf, ch);
	sprintf (buf, "#G(#eWillpower#G)#c %d/%d    #G(#eSect Status#G)#c %d\n\r", victim->pcdata->willpower[0], victim->pcdata->willpower[1], victim->pcdata->sectstatus);
	send_to_char (buf, ch);
	if (victim->class == CLASS_NONE)
	{
		sprintf (buf, "#G#e(Blood#G)#c %d/%d  #G#e(XP#G)#c %d  #G#e(Piety#G)#c %d\n\r", victim->blood[0], victim->blood[1], victim->exp, victim->pcdata->roadt);
	send_to_char (buf, ch);
	sprintf (buf, "#G#e(Courage#G)#c %d.  #G#e(Self-Control#G)#c %d  #G#e(Conscience#G)#c %d\n\r", victim->pcdata->virtues[VIRTUE_COURAGE], victim->pcdata->virtues[VIRTUE_SELFCONTROL], victim->pcdata->virtues[VIRTUE_CONSCIENCE]);
	send_to_char (buf, ch);
	}

	else
	{
		sprintf (buf, "#G(#eBlood#G)#c %d/%d #G(#eXP#G)#c %d\n\r", victim->blood[0], victim->blood[1], victim->exp);
		send_to_char (buf, ch);
	}

	sprintf (buf, "#G(#eNature#G)#c %s  #G(#eDemeanor#G)#c %s\n\r", victim->nature, victim->demeanor);
	send_to_char (buf, ch);
	
	if (IS_VAMPIRE(victim))
	{
		sprintf( buf, "#G(#eCourage#G)#c %d  #G(#eSelf-Control#G)#c %d  #G(#eConscience#G)#c %d\n\r",
		victim->pcdata->virtues[VIRTUE_COURAGE], victim->pcdata->virtues[VIRTUE_SELFCONTROL],
		victim->pcdata->virtues[VIRTUE_CONSCIENCE]);
		send_to_char( buf, ch);
	}
	
	if (IS_INQUISITOR(victim))
	{
		sprintf (buf, "#G(#eOrder#G)#c %s  #G(#eImpulse#G)#c %s\n\r", victim->order, victim->impulse);
		send_to_char (buf, ch);
	}

	if (!IS_NPC(victim))
	{
	sprintf (buf, "                                #G(#eABILITIES#G)#n");
	send_to_char (buf, ch);
	sprintf (buf, "\n\r#G[#e        Talents        #G ][#e         Skills     #G    ][#e       Knowledges #G      ]#n\n\r");
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eExpression             #c%d#G][#eAnimal Ken             #c%d#G][#eAcademics              #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_EXPRESSION], 
	victim->abilities[SKILLS][SKI_ANIMALKEN], 
	victim->abilities[KNOWLEDGES][KNO_ACADEMICS]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eAlertness              #c%d#G][#eFirearms               #c%d#G][#eComputer               #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_ALERTNESS], 
	victim->abilities[SKILLS][SKI_FIREARMS], 
	victim->abilities[KNOWLEDGES][KNO_COMPUTER]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eAthletics              #c%d#G][#eCrafts                 #c%d#G][#eInvestigation          #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_ATHLETICS], 
	victim->abilities[SKILLS][SKI_CRAFTS], 
	victim->abilities[KNOWLEDGES][KNO_INVESTIGATION]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eBrawl                  #c%d#G][#eEtiquette              #c%d#G][#eLaw                    #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_BRAWL], 
	victim->abilities[SKILLS][SKI_ETIQUETTE], 
	victim->abilities[KNOWLEDGES][KNO_LAW]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eDodge                  #c%d#G][#eSecurity               #c%d#G][#eLinguistics            #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_DODGE], 
	victim->abilities[SKILLS][SKI_SECURITY], 
	victim->abilities[KNOWLEDGES][KNO_LINGUISTICS]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eEmpathy                #c%d#G][#eMelee                  #c%d#G][#eMedicine               #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_EMPATHY], 
	victim->abilities[SKILLS][SKI_MELEE], 
	victim->abilities[KNOWLEDGES][KNO_MEDICINE]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eIntimidation           #c%d#G][#ePerformance            #c%d#G][#eOccult                 #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_INTIMIDATION], 
	victim->abilities[SKILLS][SKI_PERFORMANCE], 
	victim->abilities[KNOWLEDGES][KNO_OCCULT]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eStreetwise             #c%d#G][#eDrive                  #c%d#G][#ePolitics               #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_STREETWISE], 
	victim->abilities[SKILLS][SKI_DRIVE], 
	victim->abilities[KNOWLEDGES][KNO_POLITICS]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eLeadership             #c%d#G][#eStealth                #c%d#G][#eScience                #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_LEADERSHIP], 
	victim->abilities[SKILLS][SKI_STEALTH], 
	victim->abilities[KNOWLEDGES][KNO_SCIENCE]);
	send_to_char (buf, ch);
	sprintf (buf, "#G[#eSubterfuge             #c%d#G][#eSurvival               #c%d#G][#eFinance                #c%d#G]#n\n\r", 
	victim->abilities[TALENTS][TAL_SUBTERFUGE], 
	victim->abilities[SKILLS][SKI_SURVIVAL], 
	victim->abilities[KNOWLEDGES][KNO_FINANCE]);
	send_to_char (buf, ch);
	}
	do_powerlevels(ch, arg1);
	if (!IS_NPC(victim))
	{
	send_to_char("\n\r", ch);
	
	
	
	
	send_to_char(display_extra(victim), ch);
	send_to_char("\n\r", ch);
	}

	return;
}


void do_powerlevels (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char disc[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int col = 0;
	int sn;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Check who's power levels??\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's!\n\r", ch);
		return;
	}


	// shows vampire discipline levels
	if (IS_VAMPIRE (victim) || IS_GHOUL(victim))
	{
		send_to_char ("#G[#eVampire or Ghoul#G]\n\r", ch);
		sprintf (buf, "#G(#eRoad#G)#c %s #G(#c%d#G)\n\r", road_names[victim->pcdata->road][0], victim->pcdata->roadt);
		send_to_char (buf, ch);
		sprintf (buf, "#G(#eSect#G)#c %s  #G(#eClan#G)#c %s   #G(#eGeneration#G)#c   %d#n\n\r#G[#eDisciplines#G]\n\r", strlen (victim->side) < 2 ? "None" : victim->side, strlen (victim->clan) < 2 ? "Caitiff" : victim->clan, victim->vampgen);
		send_to_char (buf, ch);
		col = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (victim->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "None");
				break;

			case DISC_ANIMALISM:
				strcpy (disc, "Animalism");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "Auspex");
				break;
			case DISC_ABOMBWE:
				strcpy (disc, "Abombwe");
				break;
			case DISC_BARDO:
				strcpy (disc, "Bardo");
				break;
			case DISC_CELERITY:
				strcpy (disc, "Celerity");
				break;
			case DISC_CHIMERSTRY:
				strcpy (disc, "Chimerstry");
				break;
			case DISC_DAIMOINON:
				strcpy (disc, "Daimoinon");
				break;
			case DISC_DEIMOS:
				strcpy (disc, "Deimos");
				break;
			case DISC_DEMENTATION:
				strcpy (disc, "Dementation");
				break;
			case DISC_DOMINATE:
				strcpy (disc, "Dominate");
				break;
			case DISC_FORTITUDE:
				strcpy (disc, "Fortitude");
				break;
			case DISC_MA:
				strcpy (disc, "Ma");
				break;
			case DISC_MELPOMINEE:
				strcpy (disc, "Melpominee");
				break;
			case DISC_MYTHERCERIA:
				strcpy (disc, "Mytherceria");
				break;
			
			case DISC_MORTIS:
				strcpy (disc, "Mortis");
				break;
			case DISC_NECROMANCY:
				strcpy (disc, "Necromancy");
				break;
			case DISC_OBEAH:
				strcpy (disc, "Obeah");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "Obfuscate");
				break;
			case DISC_OBTENEBRATION:
				strcpy (disc, "Obtenebration");
				break;
			case DISC_POTENCE:
				strcpy (disc, "Potence");
				break;
			case DISC_PRESENCE:
				strcpy (disc, "Presence");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "Protean");
				break;
			case DISC_QUIETUS:
				strcpy (disc, "Quietus");
				break;
			case DISC_SERPENTIS:
				strcpy (disc, "Serpentis");
				break;
			case DISC_SPIRITUS:
				strcpy (disc, "Spiritus");
				break;
			case DISC_TEMPORIS:
				strcpy (disc, "Temporis");
				break;
			case DISC_THANATOSIS:
				strcpy (disc, "Thanatosis");
				break;
			case DISC_THAUMATURGY:
				strcpy (disc, "Thaumaturgy");
				break;
			case DISC_VICISSITUDE:
				strcpy (disc, "Vicissitude");
				break;
			case DISC_VALEREN:
				strcpy (disc, "Valeren");
				break;
			case DISC_GROTESQUOUS:
				strcpy (disc, "Grotesquous");
				break;
			case DISC_NIHILISTICS:
				strcpy (disc, "Nihilistics");
				break;
			case DISC_MALEFICIA:
				strcpy (disc, "Maleficia");
				break;
			case DISC_MYTHERCIA:
				strcpy (disc, "Mythercia");
				break;
			case DISC_STIGA:
				strcpy (disc, "Stiga");
				break;
			case DISC_VISCERATIKA:
				strcpy (disc, "Visceratika");
				break;
			}
			sprintf (buf, "#G(#e%s#G)#c %d    ", disc, get_disc (victim, sn));
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
	}
	else if (IS_WEREWOLF (victim))
	{
		send_to_char ("Werewolf\n\r", ch);
		col = 0;


		for (sn = BREED_HOMID; sn <= BREED_METIS; sn++)
		{
			if (victim->pcdata->breed[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "None");
				break;
			case BREED_HOMID:
				strcpy (disc, "Homid");
				break;
			case BREED_LUPUS:
				strcpy (disc, "Lupus");
				break;
			case BREED_METIS:
				strcpy (disc, "Metis");
				break;
			}
			if (victim->pcdata->breed[sn] != 0)
			{
				sprintf (buf, "%s: [%d/5] ", disc, get_breed (victim, sn));
				send_to_char (buf, ch);
				if (++col % 3 == 0)
					send_to_char ("\n\r", ch);
			}
		}

		for (sn = AUSPICE_RAGABASH; sn <= AUSPICE_AHROUN; sn++)
		{
			if (victim->pcdata->auspice[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "None");
				break;
			case AUSPICE_RAGABASH:
				strcpy (disc, "Ragabash");
				break;
			case AUSPICE_THEURGE:
				strcpy (disc, "Theurge");
				break;
			case AUSPICE_PHILODOX:
				strcpy (disc, "Philodox");
				break;
			case AUSPICE_GALLIARD:
				strcpy (disc, "Galliard");
				break;
			case AUSPICE_AHROUN:
				strcpy (disc, "Ahroun");
				break;
			}
			if (victim->pcdata->auspice[sn] != 0)
			{
				sprintf (buf, "%s: [%d/5] ", disc, get_auspice (victim, sn));
				send_to_char (buf, ch);
				if (++col % 3 == 0)
					send_to_char ("\n\r", ch);
			}
		}

		for (sn = TRIBE_BLACK_FURIES; sn <= TRIBE_WENDIGOS; sn++)
		{
			if (victim->pcdata->tribes[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "None");
				break;
			case TRIBE_BLACK_FURIES:
				strcpy (disc, "Black Furies");
				break;
			case TRIBE_BONE_GNAWERS:
				strcpy (disc, "Bone Gnawers");
				break;
			case TRIBE_CHILDREN_OF_GAIA:
				strcpy (disc, "Children of Gaia");
				break;
			case TRIBE_FIANNA:
				strcpy (disc, "Fianna");
				break;
			case TRIBE_GET_OF_FENRIS:
				strcpy (disc, "Get of Fenris");
				break;
			case TRIBE_GLASS_WALKERS:
				strcpy (disc, "Glass Walkers");
				break;
			case TRIBE_RED_TALONS:
				strcpy (disc, "Red Talons");
				break;
			case TRIBE_SHADOW_LORDS:
				strcpy (disc, "Shadow Lords");
				break;
			case TRIBE_SILENT_STRIDERS:
				strcpy (disc, "Silent Striders");
				break;
			case TRIBE_SILVER_FANGS:
				strcpy (disc, "Silver Fangs");
				break;
			case TRIBE_STARGAZERS:
				strcpy (disc, "Stargazers");
				break;
			case TRIBE_UKTENA:
				strcpy (disc, "Uktena");
				break;
			case TRIBE_WENDIGOS:
				strcpy (disc, "Wendigos");
				break;
			}
			if (victim->pcdata->tribes[sn] != 0)
			{
				sprintf (buf, "%s: [%d/5] ", disc, get_tribe (victim, sn));
				send_to_char (buf, ch);
				if (++col % 3 == 0)
					send_to_char ("\n\r", ch);
			}
		}


		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
	}
	else if (IS_INQUISITOR(victim))
	{
		send_to_char ("Inquisitor\n\r\n\r", ch);
		sprintf (buf, "Piety: %d  Courage: %d  Conscience: %d  Faith: %d\n\r", victim->pcdata->roadt, victim->pcdata->virtues[VIRTUE_COURAGE], victim->pcdata->virtues[VIRTUE_CONSCIENCE], victim->pcdata->virtues[VIRTUE_FAITH]);
		send_to_char (buf, ch);
		sprintf (buf, "Zeal: %d  Self-Control: %d  Wisdom: %d\n\r", victim->pcdata->virtues[VIRTUE_ZEAL], victim->pcdata->virtues[VIRTUE_SELFCONTROL], victim->pcdata->virtues[VIRTUE_WISDOM]);
		send_to_char (buf, ch);
		sprintf (buf, "Conviction: %d  TempConviction: %d  Conviction Bank: %d\n\r", victim->pcdata->conviction[CONVICTION_MAX], victim->pcdata->conviction[CONVICTION_CURRENT], victim->pcdata->conviction_bank);
		send_to_char (buf, ch);
		col = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (victim->pcdata->powers[sn] == 0)
				continue;
			switch (sn)
			{
			default:
				strcpy (disc, "None");
				break;

			case DISC_ANIMALISM:
				strcpy (disc, "Animalism");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "Auspex");
				break;
			case DISC_ABOMBWE:
				strcpy (disc, "Abombwe");
				break;
			case DISC_BARDO:
				strcpy (disc, "Bardo");
				break;
			case DISC_CELERITY:
				strcpy (disc, "Celerity");
				break;
			case DISC_CHIMERSTRY:
				strcpy (disc, "Chimerstry");
				break;
			case DISC_DAIMOINON:
				strcpy (disc, "Daimoinon");
				break;
			case DISC_DEIMOS:
				strcpy (disc, "Deimos");
				break;
			case DISC_DEMENTATION:
				strcpy (disc, "Dementation");
				break;
			case DISC_DOMINATE:
				strcpy (disc, "Dominate");
				break;
			case DISC_FORTITUDE:
				strcpy (disc, "Fortitude");
				break;
			case DISC_MA:
				strcpy (disc, "Ma");
				break;
			case DISC_MELPOMINEE:
				strcpy (disc, "Melpominee");
				break;
			case DISC_MYTHERCERIA:
				strcpy (disc, "Mytherceria");
				break;
			
			case DISC_MORTIS:
				strcpy (disc, "Mortis");
				break;
			case DISC_NECROMANCY:
				strcpy (disc, "Necromancy");
				break;
			case DISC_OBEAH:
				strcpy (disc, "Obeah");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "Obfuscate");
				break;
			case DISC_OBTENEBRATION:
				strcpy (disc, "Obtenebration");
				break;
			case DISC_POTENCE:
				strcpy (disc, "Potence");
				break;
			case DISC_PRESENCE:
				strcpy (disc, "Presence");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "Protean");
				break;
			case DISC_QUIETUS:
				strcpy (disc, "Quietus");
				break;
			case DISC_SERPENTIS:
				strcpy (disc, "Serpentis");
				break;
			case DISC_SPIRITUS:
				strcpy (disc, "Spiritus");
				break;
			case DISC_TEMPORIS:
				strcpy (disc, "Temporis");
				break;
			case DISC_THANATOSIS:
				strcpy (disc, "Thanatosis");
				break;
			case DISC_THAUMATURGY:
				strcpy (disc, "Thaumaturgy");
				break;
			case DISC_VICISSITUDE:
				strcpy (disc, "Vicissitude");
				break;
			case DISC_VALEREN:
				strcpy (disc, "Valeren");
				break;
			case DISC_GROTESQUOUS:
				strcpy (disc, "Grotesquous");
				break;
			case DISC_NIHILISTICS:
				strcpy (disc, "Nihilistics");
				break;
			case DISC_MALEFICIA:
				strcpy (disc, "Maleficia");
				break;
			case DISC_MYTHERCIA:
				strcpy (disc, "Mythercia");
				break;
			case DISC_STIGA:
				strcpy (disc, "Stiga");
				break;
			case DISC_VISCERATIKA:
				strcpy (disc, "Visceratika");
				break;
			case DISC_SOPHIA_PATRIS:
				strcpy (disc, "Sophia Patris");
				break;
			case DISC_SEDULITAS_FILIUS:
				strcpy (disc, "Sedulitas Filius");
				break;
			case DISC_BONI_SPIRITUS:
				strcpy (disc, "Boni Spiritus");
				break;
			}
			sprintf (buf, "%s: [%d/10] ", disc, get_disc (victim, sn));
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);		
	}
	
	if (IS_SHIFTER(victim))
	{
		sprintf(buf, "Gnosis: %d, Rage: %d\n\r",victim->pcdata->gnosis[0],victim->pcdata->rage[0]);
		send_to_char(buf,ch);

		sprintf (buf, "Glory: %d/%d, Honor : %d/%d, Wisdom : %d/%d\n\r", 
		victim->pcdata->renown[TEMPGLORY], victim->pcdata->renown[PERMGLORY], victim->pcdata->renown[TEMPHONOR], victim->pcdata->renown[PERMHONOR], victim->pcdata->renown[TEMPWISDOM], victim->pcdata->renown[PERMWISDOM]);
		send_to_char (buf, ch);
	}
	return;
}




void do_burn (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int *pArea;
	int amount;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	if (IS_NPC (ch))
		return;
	if (!IS_STORYTELLER (ch))
	{
		if (arg1[0] == '\0')
		{
			send_to_char("Syntax: burn <value> wp/bp/rage/gnosis/conviction\n\r", ch);
			return;
		}
		if (!str_cmp (arg2, "willpower") || !str_cmp (arg2, "wp"))
			pArea = &ch->pcdata->willpower[WILLPOWER_CURRENT];
		else if (!str_cmp (arg2, "blood") || !str_cmp (arg2, "bp") || !str_cmp (arg2, "vim"))
			pArea = &ch->blood[BLOOD_CURRENT];
		else if (!str_cmp (arg2, "conviction"))
			pArea = &ch->pcdata->conviction[CONVICTION_CURRENT];
		else if (!str_cmp (arg2, "quintessence") || !str_cmp (arg2, "qp"))
			pArea = &ch->quintessence;
		else if (!str_cmp (arg2, "rage"))
			pArea = &ch->pcdata->rage[GAROU_RAGE];
		else if (!str_cmp (arg2, "gnosis"))
			pArea = &ch->pcdata->gnosis[GAROU_GNOSIS];
		else
		{
			send_to_char("Syntax: burn <value> wp/bp/rage/gnosis/conviction\n\r", ch);
			return;
		}
		amount = atoi(arg1);
		if (amount <= 0 || amount > 10)
		{
			send_to_char ("#rNumber must be between 1 and 10.#n\n\r", ch);
			return;
		}
		if ((*pArea - amount) < 1)
		{
			send_to_char ("#rYou cannot burn that much.#n\n\r", ch);
			return;
		}
		*pArea -= amount;
		sprintf(buf, "#e%s burns #w%s #e%s#n", ch->name, arg1, arg2);
		sprintf(buf2,"#eYou burn #w%s #e%s#n\n\r", arg1, arg2);
		act(buf, ch, NULL, NULL, TO_ROOM);
		send_to_char (buf2, ch);
	}
	else
	{

		if (arg1[0] == '\0')
		{
			send_to_char ("Syntax: burn <character> <willpower/rage/gnosis/blood/qp/conviction> <value>\n\r", ch);
			return;
		}

		if ((victim = get_char_world (ch, arg1)) == NULL)
		{
			send_to_char ("#RError: #nWhy would you try and use this command on someone who is not here?.\n\r", ch);
			return;
		}

		if (!str_cmp (arg2, "willpower") || !str_cmp (arg2, "will"))
		{
			pArea = &victim->pcdata->willpower[WILLPOWER_CURRENT];
		}
		else if (!str_cmp (arg2, "blood") || !str_cmp (arg2, "bp"))
		{
			pArea = &victim->blood[BLOOD_CURRENT];
		}
		else if (!str_cmp (arg2, "conviction") || !str_cmp (arg2, "convic"))
		{
			pArea = &victim->pcdata->conviction[CONVICTION_CURRENT];
		}
		else if (!str_cmp (arg2, "quintessence") || !str_cmp (arg2, "qp"))
		{
			pArea = &victim->quintessence;
		}
		else if (!str_cmp (arg2, "rage"))
		{
			pArea = &victim->pcdata->rage[GAROU_RAGE];
		}
		else if (!str_cmp (arg2, "gnosis"))
		{
			pArea = &victim->pcdata->gnosis[GAROU_GNOSIS];
		}
		else
		{
			send_to_char ("#RError: #nUnknown Area.\n\r", ch);
			return;
		}
		amount = atoi (arg3);
		if (amount <= 0 || amount > 10)
		{
			send_to_char ("#RError: Your value must be between 1 and 10.\n\r", ch);
			return;
		}

		*pArea -= amount;
		if (!str_cmp (arg2, "willpower"))
		{
			if (victim->pcdata->willpower[WILLPOWER_CURRENT] >  victim->pcdata->willpower[WILLPOWER_MAX]);
		}
		else if (str_cmp (arg2, "conviction"))
		{
			if (victim->pcdata->conviction[CONVICTION_CURRENT] > victim->pcdata->conviction[CONVICTION_MAX]);
		}

		if (*pArea < 1)
		{
			send_to_char ("#RError: #nThey do not have enough for you to burn.\n\r", ch);
			return;
		}
		send_to_char ("Done.\n\r", ch);
		send_to_char ("Burn.\n\r", victim);
	}
	return;
}

void do_regain (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int *pArea;
	int amount;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	if (IS_NPC (ch))
		return;
	if (!IS_STORYTELLER (ch))
	{
		if (arg1[0] == '\0')
		{
			send_to_char("Syntax: regain <value> wp/bp/rage/gnosis/conviction\n\r", ch);
			return;
		}
		if (!str_cmp (arg2, "willpower") || !str_cmp (arg2, "wp"))
			pArea = &ch->pcdata->willpower[WILLPOWER_CURRENT];
		else if (!str_cmp (arg2, "blood") || !str_cmp (arg2, "bp"))
			pArea = &ch->blood[BLOOD_CURRENT];
		else if (!str_cmp (arg2, "conviction"))
			pArea = &ch->pcdata->conviction[CONVICTION_CURRENT];
		else if (!str_cmp (arg2, "quintessence") || !str_cmp (arg2, "qp"))
			pArea = &ch->quintessence;
		else if (!str_cmp (arg2, "rage"))
			pArea = &ch->pcdata->rage[GAROU_RAGE];
		else if (!str_cmp (arg2, "gnosis"))
			pArea = &ch->pcdata->gnosis[GAROU_GNOSIS];
		else
		{
			send_to_char("Syntax: regain <value> wp/bp/rage/gnosis/conviction\n\r", ch);
			return;
		}
		amount = atoi(arg1);
		if (amount <= 0 || amount > 10)
		{
			send_to_char ("#rNumber must be between 1 and 10.#n\n\r", ch);
			return;
		}
		*pArea += amount;
		if (!str_cmp (arg2, "willpower") || !str_cmp(arg2, "wp"))
		{
			if (ch->pcdata->willpower[WILLPOWER_CURRENT] > ch->pcdata->willpower[WILLPOWER_MAX])
				ch->pcdata->willpower[WILLPOWER_CURRENT] = ch->pcdata->willpower[WILLPOWER_MAX];
		}
		else if (!str_cmp (arg2, "conviction"))
		{
			if (ch->pcdata->conviction[CONVICTION_CURRENT] > ch->pcdata->conviction[CONVICTION_MAX])
				ch->pcdata->conviction[CONVICTION_CURRENT] = ch->pcdata->conviction[CONVICTION_MAX];
		}
		else if (!str_cmp (arg2, "quintessence") || !str_cmp (arg2, "qp"))
		{
			if (ch->quintessence > ch->max_quintessence)
				ch->quintessence = ch->max_quintessence;
		}
		else if (!str_cmp (arg2, "rage"))
		{
			if (ch->pcdata->rage[GAROU_RAGE] > ch->pcdata->rage[GAROU_MAXRAGE])
				ch->pcdata->rage[GAROU_RAGE] = ch->pcdata->rage[GAROU_MAXRAGE];
		}
		else if (!str_cmp (arg2, "gnosis"))
		{
			if (ch->pcdata->gnosis[GAROU_GNOSIS] > ch->pcdata->gnosis[GAROU_MAXGNOSIS])
				ch->pcdata->gnosis[GAROU_GNOSIS] = ch->pcdata->gnosis[GAROU_MAXGNOSIS];
		}
		sprintf(buf, "#e%s regains #w%s #e%s#n", ch->name, arg1, arg2);
		sprintf(buf2,"#eYou regain #w%s #e%s#n\n\r", arg1, arg2);
		act(buf, ch, NULL, NULL, TO_ROOM);
		send_to_char (buf2, ch);
	}
	else
	{
		if (arg1[0] == '\0')
		{
			send_to_char ("Syntax: regain <character> <willpower/rage/gnosis/qp/conviction/blood> <value>\n\r", ch);
			return;
		}

		if ((victim = get_char_world (ch, arg1)) == NULL)
		{
			send_to_char ("#RError: #nWhy would you try and use this command on someone who is not here?.\n\r", ch);
			return;
		}

		if (!str_cmp (arg2, "willpower"))
		{
			pArea = &victim->pcdata->willpower[WILLPOWER_CURRENT];
		}
		else if (!str_cmp (arg2, "conviction"))
		{
			pArea = &victim->pcdata->conviction[CONVICTION_CURRENT];
		}
		else if (!str_cmp (arg2, "blood") || !str_cmp  (arg2, "bp"))
		{
			pArea = &victim->blood[BLOOD_CURRENT];
		}
		else if (!str_cmp (arg2, "quintessence") || !str_cmp  (arg2, "qp"))
		{
			pArea = &victim->quintessence;
		}
		else if (!str_cmp (arg2, "rage"))
		{
			pArea = &victim->pcdata->rage[GAROU_RAGE];
		}
		else if (!str_cmp (arg2, "gnosis"))
		{
			pArea = &victim->pcdata->gnosis[GAROU_GNOSIS];
		}
		else
		{
			send_to_char ("#RError: #nUnknown Area.\n\r", ch);
			return;
		}
		amount = atoi (arg3);
		if (amount <= 0 || amount > 10)
		{
			send_to_char ("#RError: #nYour value must be between 1 and 10.\n\r", ch);
			return;
		}

		*pArea += amount;
		if (!str_cmp (arg2, "willpower"))
		{
			if (victim->pcdata->willpower[WILLPOWER_CURRENT] > victim->pcdata->willpower[WILLPOWER_MAX])
				victim->pcdata->willpower[WILLPOWER_CURRENT] = victim->pcdata->willpower[WILLPOWER_MAX];
		}
		else if (!str_cmp (arg2, "conviction"))
		{
			if (victim->pcdata->conviction[CONVICTION_CURRENT] > victim->pcdata->conviction[CONVICTION_MAX])
				victim->pcdata->conviction[CONVICTION_CURRENT] = victim->pcdata->conviction[CONVICTION_MAX];
		}
		else if (!str_cmp (arg2, "quintessence") || !str_cmp (arg2, "qp"))
		{
			if (victim->quintessence > victim->max_quintessence)
				victim->quintessence = victim->max_quintessence;
		}
		else if (!str_cmp (arg2, "rage"))
		{
			if (victim->pcdata->rage[GAROU_RAGE] > victim->pcdata->rage[GAROU_MAXRAGE])
				victim->pcdata->rage[GAROU_RAGE] = victim->pcdata->rage[GAROU_MAXRAGE];
		}
		else if (!str_cmp (arg2, "gnosis"))
		{
			if (victim->pcdata->gnosis[GAROU_GNOSIS] > victim->pcdata->gnosis[GAROU_MAXGNOSIS])
				victim->pcdata->gnosis[GAROU_GNOSIS] = victim->pcdata->gnosis[GAROU_MAXGNOSIS];
		}
		send_to_char ("Done\n\r", ch);
		send_to_char ("Regain\n\r", victim);
	}
	return;
}

void do_fixcharacter (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int disc;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Fix which character? (basically clears stats and affects)\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's!\n\r", ch);
		return;
	}

	for (obj = victim->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->wear_loc != WEAR_NONE)
		{
			obj_from_char (obj);
			obj_to_char (obj, victim);
		}
	}
	while (victim->affected)
		affect_remove (victim, victim->affected);
	if (!IS_AFFECTED (victim, AFF_POLYMORPH))
	{
		free_string (victim->long_descr);
		victim->long_descr = str_dup ("");
	}
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
	victim->more = 0;
	victim->more2 = 0;
	victim->more3 = 0;
	ch->mod_attributes[ATTRIB_STR] = 0;
	ch->mod_attributes[ATTRIB_DEX] = 0;
	ch->mod_attributes[ATTRIB_STA] = 0;
	ch->mod_attributes[ATTRIB_CHA] = 0;
	ch->mod_attributes[ATTRIB_MAN] = 0;
	ch->mod_attributes[ATTRIB_APP] = 0;
	ch->mod_attributes[ATTRIB_PER] = 0;
	ch->mod_attributes[ATTRIB_INT] = 0;
	ch->mod_attributes[ATTRIB_WIT] = 0;
	victim->armor = 100;
	victim->hit = UMAX (1, victim->hit);
	victim->mana = UMAX (1, victim->mana);
	victim->move = UMAX (1, victim->move);
	victim->hitroll = 0;
	victim->damroll = 0;
	victim->saving_throw = 0;
	victim->pcdata->aggdamage = 0;
	victim->pcdata->bashingdamage = 0;
	victim->pcdata->lethaldamage = 0;
	victim->pcdata->demonic = 0;
	victim->pcdata->atm = 0;
	victim->pcdata->followers = 0;
	victim->pcdata->powers_set_wear = 0;
	send_to_char ("Your stats have been cleared.  Please rewear your equipment.\n\r", victim);
	if (victim->polyaff > 0)
	{
		if (IS_POLYAFF (victim, POLY_BAT) || IS_POLYAFF (victim, POLY_WOLF) || IS_POLYAFF (victim, POLY_MIST) || IS_POLYAFF (victim, POLY_SERPENT) || IS_POLYAFF (victim, POLY_RAVEN) || IS_POLYAFF (victim, POLY_FISH) || IS_POLYAFF (victim, POLY_FROG) || IS_POLYAFF (victim, POLY_SHADOW))
		{
			do_autosave (victim, "");
			return;
		}
	}

	if (IS_VAMPIRE (victim) && victim->polyaff < 1)
	{
		for (disc = DISC_ANIMALISM; disc <= DISC_VICISSITUDE; disc++)
		{
			victim->pcdata->powers_mod[disc] = 0;
			victim->pcdata->powers_set[disc] = 0;
		}
	}
	do_autosave (victim, "");
	return;
}



void do_generation (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	int gen;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (IS_NPC (ch))
		return;
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: generation <char> <generation>.\n\r", ch);
		send_to_char ("Generation 1 is a Master <class> and 2 is normally clan leader.\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("That player is not here.\n\r", ch);
		return;
	}

	gen = is_number (arg2) ? atoi (arg2) : -1;
	send_to_char ("Generation Set.\n\r", ch);
	victim->vampgen = gen;
	update_gen (victim);
	return;
}


void do_ifavour (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (IS_NPC (ch))
		return;
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax is: favour <target> <favour>\n\r", ch);
		send_to_char ("Vampire: Induct Outcast Sire\n\r", ch);
		send_to_char ("Werewolf: Induct Outcast Sire Shaman.\n\r", ch);
		send_to_char ("\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	if (ch == victim)
	{
		send_to_char ("Not on yourself!\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "sire"))
	{


		if (IS_EXTRA (victim, EXTRA_SIRE))
		{

			if (IS_WEREWOLF (victim))
			{
				act ("You grant $N permission to claw a pup!", ch, NULL, victim, TO_CHAR);
				act ("$n has granted $N permission to claw a pup!", ch, NULL, victim, TO_NOTVICT);
				act ("$n has granted you permission to claw a pup!", ch, NULL, victim, TO_VICT);
			}
			else
			{
				act ("You remove $N's permission to sire a childe!", ch, NULL, victim, TO_CHAR);
				act ("$n has removed $N's permission to sire a childe!", ch, NULL, victim, TO_NOTVICT);
				act ("$n has remove your permission to sire a childe!", ch, NULL, victim, TO_VICT);
			}
			REMOVE_BIT (victim->extra, EXTRA_SIRE);
			return;
		}
		if (IS_WEREWOLF (victim))
		{
			act ("You remove $N's permission to claw a pup!", ch, NULL, victim, TO_CHAR);
			act ("$n has removed $N's permission to claw a pup!", ch, NULL, victim, TO_NOTVICT);
			act ("$n has remove your permission to claw a pup!", ch, NULL, victim, TO_VICT);
		}
		else
		{
			act ("You grant $N permission to sire a childe!", ch, NULL, victim, TO_CHAR);
			act ("$n has granted $N permission to sire a childe!", ch, NULL, victim, TO_NOTVICT);
			act ("$n has granted you permission to sire a childe!", ch, NULL, victim, TO_VICT);
		}
		SET_BIT (victim->extra, EXTRA_SIRE);
		return;
	}
	else if (!str_cmp (arg2, "shaman") && IS_WEREWOLF (victim))
	{
		if (IS_EXTRA (victim, EXTRA_PRINCE))
		{
			act ("You remove $N's Shaman status!", ch, NULL, victim, TO_CHAR);
			act ("$n has removed $N's Shaman status!", ch, NULL, victim, TO_NOTVICT);
			act ("$n has removed your Shaman status!", ch, NULL, victim, TO_VICT);
			REMOVE_BIT (victim->extra, EXTRA_PRINCE);
			return;
		}
		act ("You grant $N Shaman status!", ch, NULL, victim, TO_CHAR);
		act ("$n has granted $N Shaman status!", ch, NULL, victim, TO_NOTVICT);
		act ("$n has granted you Shaman status!", ch, NULL, victim, TO_VICT);
		SET_BIT (victim->extra, EXTRA_PRINCE);
		return;
	}
	else if (!str_cmp (arg2, "prince"))
	{
		if (IS_EXTRA (victim, EXTRA_PRINCE))
		{
			act ("You remove $N's Prince status!", ch, NULL, victim, TO_CHAR);
			act ("$n has removed $N's Prince status!", ch, NULL, victim, TO_NOTVICT);
			act ("$n has removed your Prince status!", ch, NULL, victim, TO_VICT);
			REMOVE_BIT (victim->extra, EXTRA_PRINCE);
			return;
		}
		act ("You grant $N Prince status!", ch, NULL, victim, TO_CHAR);
		act ("$n has granted $N Prince status!", ch, NULL, victim, TO_NOTVICT);
		act ("$n has granted you Prince status!", ch, NULL, victim, TO_VICT);
		SET_BIT (victim->extra, EXTRA_PRINCE);
		return;
	}
	else if (!str_cmp (arg2, "induct"))
	{
		if (IS_WEREWOLF (victim))
		{
			act ("You induct $N into your tribe!", ch, NULL, victim, TO_CHAR);
			act ("$n inducts $N into $s tribe!", ch, NULL, victim, TO_NOTVICT);
			act ("$n inducts you into $s tribe!", ch, NULL, victim, TO_VICT);
		}
		else
		{
			act ("You induct $N into your clan!", ch, NULL, victim, TO_CHAR);
			act ("$n inducts $N into $s clan!", ch, NULL, victim, TO_NOTVICT);
			act ("$n inducts you into $s clan!", ch, NULL, victim, TO_VICT);
		}
		free_string (victim->clan);
		victim->clan = str_dup (ch->clan);
		return;
	}

	else if (!str_cmp (arg2, "outcast"))
	{
		if (IS_WEREWOLF (victim))
		{
			act ("You outcast $N from your tribe!", ch, NULL, victim, TO_CHAR);
			act ("$n outcasts $N from $s tribe!", ch, NULL, victim, TO_NOTVICT);
			act ("$n outcasts you from $s tribe!", ch, NULL, victim, TO_VICT);
		}
		else
		{
			act ("You outcast $N from your clan!", ch, NULL, victim, TO_CHAR);
			act ("$n outcasts $N from $s clan!", ch, NULL, victim, TO_NOTVICT);
			act ("$n outcasts you from $s clan!", ch, NULL, victim, TO_VICT);
		}
		free_string (victim->clan);
		victim->clan = str_dup ("");
		REMOVE_BIT (victim->extra, EXTRA_SIRE);
		REMOVE_BIT (victim->extra, EXTRA_PRINCE);
		return;
	}
	else
		send_to_char ("You are unable to grant that sort of favour.\n\r", ch);
	return;
}


void do_pathclear (CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument (argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char ("Clear who's primary path?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's!\n\r", ch);
		return;
	}

	if (victim->pcdata->thaum_prime == -1)
	{
		send_to_char ("They do not have a primary path assigned to them.\n\r", ch);
		return;
	}

	victim->pcdata->thaum_prime = -1;
	send_to_char ("Primary Path Cleared.\n\r", ch);
	if(ch != victim)
	  send_to_char ("Primary Path Cleared.\n\r", victim);
	return;
}


void do_roadclear (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Clear who's road?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's!\n\r", ch);
		return;
	}

	if (victim->pcdata->road == -1)
	{
		send_to_char ("They do not have a road assigned to them.", ch);
		return;
	}

	victim->pcdata->road = -1;
	send_to_char ("Road Cleared.", ch);
	send_to_char ("Road Cleared.", victim);
	return;
}




void do_sectname (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Whose sect do you wish to change or name?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
		return;
	smash_tilde (argument);
	if (strlen (argument) < 0 || strlen (argument) > 25)
	{
		send_to_char ("Sect name should be between 0 and 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove Sect.\n\r", ch);
		return;
	}
	free_string (victim->side);
	victim->side = str_dup (argument);
	send_to_char ("Sect set.\n\r", ch);
	send_to_char ("Sect set.\n\r", victim);
	return;
}




void do_class (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	int i;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	if (IS_NPC (ch))
		return;
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: class <char> <class>.\n\r", ch);
		send_to_char ("Classes:\n\r", ch);
		send_to_char ("Werewolf, Vampire, Inquisitor\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("That player is not here.\n\r", ch);
		return;
	}
	if (victim->level < LEVEL_AVATAR)
	{
		send_to_char ("You can only do this to avatars.\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "none"))
	{
		if (IS_VAMPIRE (victim) || IS_GHOUL(victim) || IS_INQUISITOR(ch))
			mortal_vamp (victim);
		victim->pcdata->road = -1;
		victim->blood[BLOOD_POOL] = 10;
		victim->blood[BLOOD_CURRENT] = 10;
		victim->blood[BLOOD_POTENCY] = 1;
		victim->blood[BLOOD_POTENCY] = 1;
		victim->pcdata->virtues[VIRTUE_CONSCIENCE] = 1;
		victim->pcdata->virtues[VIRTUE_SELFCONTROL] = 1;
		victim->pcdata->virtues[VIRTUE_COURAGE] = 1;
		victim->pcdata->virtues[VIRTUE_ZEAL] = 1;
		victim->pcdata->virtues[VIRTUE_FAITH] = 1;
		victim->pcdata->virtues[VIRTUE_WISDOM] = 1;		
		victim->pcdata->willpower[WILLPOWER_CURRENT] = 1;
		victim->pcdata->willpower[WILLPOWER_MAX] = 1;
		victim->class = CLASS_NONE;
		free_string (victim->lord);
		victim->lord = str_dup ("");
		free_string (victim->clan);
		victim->clan = str_dup ("");
		free_string (victim->side);
		victim->side = str_dup ("");
		victim->vampgen = 3;
		victim->vampaff = 0;
		victim->vamppass = -1;
		victim->pcdata->wolfform[1] = FORM_HOMID;
		victim->pcdata->cland[0] = -1;
		victim->pcdata->cland[1] = -1;
		victim->pcdata->cland[2] = -1;
		for (i = 0; i <= BREED_MAX; i++)
			victim->pcdata->breed[i] = 0;
		for (i = 0; i <= AUSPICE_MAX; i++)
			victim->pcdata->auspice[i] = 0;
		for (i = 0; i <= TRIBE_MAX; i++)
			victim->pcdata->tribes[i] = 0;
		for (i = 0; i <= DISC_MAX; i++)
			victim->pcdata->powers[i] = 0;
		REMOVE_BIT (victim->act, PLR_CHAMPION);
		REMOVE_BIT (victim->extra, EXTRA_SIRE);
		REMOVE_BIT (victim->extra, EXTRA_PRINCE);
		victim->pcdata->wolf = 0;
		victim->class = 0;
		if (!IS_IMMORTAL (victim))
		{
			victim->level = LEVEL_AVATAR;
			victim->trust = 0;
		}
		return;
	}
	if (victim->class != CLASS_NONE)
	{
		send_to_char ("They already have a class.\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "vampire"))
	{
		victim->class = CLASS_VAMPIRE;
		victim->vampgen = 1;
		update_gen (victim);
		victim->pcdata->cland[0] = -1;
		victim->pcdata->cland[1] = -1;
		victim->pcdata->cland[2] = -1;
		send_to_char ("Ok.\n\r", ch);
		send_to_char ("You are now the Master Vampire.\n\r", victim);
	}
	else if (!str_cmp (arg2, "werewolf"))
	{
		victim->class = CLASS_WEREWOLF;
		victim->vampgen = 1;
		victim->pcdata->cland[0] = -1;
		victim->pcdata->cland[1] = -1;
		victim->pcdata->cland[2] = -1;
		send_to_char ("Ok.\n\r", ch);
		send_to_char ("You are now the Master Werewolf.\n\r", victim);
	}
	else if (!str_cmp (arg2, "inquisitor"))
	{
		victim->class = CLASS_INQUISITOR;
		victim->pcdata->road = ROAD_HEAVEN;
		victim->pcdata->roadt = 1;
		victim->pcdata->cland[0] = -1;
		victim->pcdata->cland[1] = -1;
		victim->pcdata->cland[2] = -1;
		send_to_char ("Ok.\n\r", ch);
		send_to_char ("You are now an Inquisitor.\n\r", victim);
	}		
	else
	{
		send_to_char ("Syntax: class <char> <class>.\n\r", ch);
		send_to_char ("Classes:\n\r", ch);
		send_to_char ("Werewolf, Vampire, Inquisitor\n\r", ch);
		return;
	}
	send_to_char ("Class Set.\n\r", ch);
	return;
}





void do_omni (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;

	if (IS_NPC (ch))
		return;

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
		if (!can_see (ch, d->character))	continue;

			sprintf (buf, "Name:%8s|HL:%2d|Vnum:%7d|Room Name:%10s\n\r",
			victim->name, victim->hit, victim->in_room->vnum, victim->in_room->name);
		send_to_char (buf, ch);
	}

	return;
}

void do_echo (CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	if (argument[0] == '\0')
	{
		send_to_char ("Command: Echo\n\r", ch);
		send_to_char ("Options: world, area, room, player\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg1);
	if (!strcmp (arg1, "world"))
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING)
			{
				send_to_char (argument, d->character);
				send_to_char ("\n\r", d->character);
			}
		}
		return;
	}
	else if (!strcmp (arg1, "area"))
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && d->character->in_room->area == ch->in_room->area)
			{
				send_to_char (argument, d->character);
				send_to_char ("\n\r", d->character);
			}
		}
		return;
	}
	else if (!strcmp (arg1, "room"))
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && d->character->in_room == ch->in_room)
			{
				send_to_char (argument, d->character);
				send_to_char ("\n\r", d->character);
			}
		}
		return;
	}
	else if (!strcmp (arg1, "player"))
	{
		argument = one_argument (argument, arg2);
		if ((victim = get_char_world (ch, arg2)) == NULL)
		{
			send_to_char ("That player is not here.\n\r", ch);
			return;
		}

		if (victim->level >= ch->level)
		{
			send_to_char ("You cannot do player echo's to people whose level is higher than yours.\n\r", ch);
			return;
		}

		send_to_char (argument, victim);
		send_to_char ("\n\r", victim);
		return;
	}
	send_to_char ("Unknown option for echo\n\r", ch);
	return;
}

void do_dice (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int num_of_dice;
	int roll;
	int bonus_dice = 0;
	int i, mod, p;
	int success = 0;
	int difficulty = 0;
	bool had_a_success = FALSE;
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int stat;
	int abil_group;
	int ability;
	int ability_dice = 0;
	int j;
	int found = FALSE;
	strcpy(buf, "dice ");
	strcat(buf, argument);
	sprintf( buf2, "#C[#wDice Log #C(#c%s#C) #B: #w%s #C]#n\n\r", ch->name, buf);
	log_string(buf2, CHANNEL_LOG_DICE);
	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;

	if (!IS_STORYTELLER (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (!strcmp (arg1, "damage"))
	{

		argument = one_argument (argument, arg1);
		difficulty = 6;
		if (arg1[0] == '\0')
		{
			send_to_char ("Syntax: dice damage <## of dice>\n\r", ch);
			return;
		}
		else
			num_of_dice = atoi (arg1);
		if (num_of_dice > 30 || num_of_dice < 1)
		{
			sprintf (buf, "#RError: #nNumber of Dice must be between 1 and 30... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		sprintf (buf, "Damage Dice: -%d-\n\r", num_of_dice);
		send_to_char (buf, ch);
		sprintf (buf, "Rolled: ");
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char( buf, ch );
		logchan( buf, CHANNEL_LOG_DICE );
		for (i = 0; i < num_of_dice; i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#C%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
			send_to_char (buf, ch);
		}
		
		if (bonus_dice > 0)
		{
			sprintf (buf, "Extra from 10s: ");
			
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
				send_to_char( buf, ch );
			logchan( buf, CHANNEL_LOG_DICE );

			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				sprintf (buf, "#C%d ", roll);
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
				logchan( buf, CHANNEL_LOG_DICE );
				
				if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
					send_to_char (buf, ch);
			}
		}

		if (success > 0)
			sprintf (buf, "The character had %d successes.\n\r", success);
		else if (success == 0)
			sprintf (buf, "The character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf( buf, "The character has #RBOTCHED!#n.\n\r");
		send_to_char (buf, ch);
			logchan( buf, CHANNEL_LOG_DICE );
			
		
		return;
	}
	else if (!strcmp (arg1, "normal"))
	{


		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		argument = one_argument (argument, arg4);
		argument = one_argument (argument, arg5);
		if (arg2[0] == '\0')
		{
			send_to_char ("Syntax:\n\r", ch);
			send_to_char ("dice normal <victim> <stat> <ability> <difficulty>\n\r", ch);
			send_to_char ("Stats: Str, Dex, Sta, Cha, Man, App, Per, Int, Wit\n\r", ch);
			send_to_char ("Abilities:\n\r", ch);
			send_to_char ("   Expression, Alertness, Athletics, Brawl, Dodge\n\r", ch);
			send_to_char ("   Empathy, Intimidation, Steetwise, Leadership, Subterfuge\n\r", ch);
			send_to_char ("   AnimalKen, Firearms, Crafts, Etiquette, Security\n\r", ch);
			send_to_char ("   Melee, Performance, Drive, Stealth, Survival\n\r", ch);
			send_to_char ("   Academics, Computer, Investigation, Law, Linguistics\n\r", ch);
			send_to_char ("   Medicine, Occult, Politics, Science, Finance\n\r", ch);
			send_to_char ("   Rituals, Primal-Urge\n\r", ch);
			send_to_char ("   Lore, Culture, Research\n\r", ch);
			send_to_char ("Difficulty: A number between 1 and 10\n\r", ch);
			return;
		}

		if ((victim = get_char_world (ch, arg2)) == NULL)
		{
			send_to_char ("#RError: #nWhy would you try and roll for someone who is not here?.\n\r", ch);
			return;
		}

		if (!str_cmp (arg3, "strength") || !str_cmp (arg3, "str"))
		{
			stat = ATTRIB_STR;
		}

		else if (!str_cmp (arg3, "dexterity") || !str_cmp (arg3, "dex"))
		{
			stat = ATTRIB_DEX;
		}

		else if (!str_cmp (arg3, "stamina") || !str_cmp (arg3, "sta"))
		{
			stat = ATTRIB_STA;
		}

		else if (!str_cmp (arg3, "charisma") || !str_cmp (arg3, "cha"))
		{
			stat = ATTRIB_CHA;
		}

		else if (!str_cmp (arg3, "manipulation") || !str_cmp (arg3, "man"))
		{
			stat = ATTRIB_MAN;
		}

		else if (!str_cmp (arg3, "appearance") || !str_cmp (arg3, "app"))
		{
			stat = ATTRIB_APP;
		}

		else if (!str_cmp (arg3, "perception") || !str_cmp (arg3, "per"))
		{
			stat = ATTRIB_PER;
		}

		else if (!str_cmp (arg3, "intelligence") || !str_cmp (arg3, "int"))
		{
			stat = ATTRIB_INT;
		}

		else if (!str_cmp (arg3, "wits") || !str_cmp (arg3, "wit"))
		{
			stat = ATTRIB_WIT;
		}
		else
		{
			sprintf (buf, "#RError: #nUnknown Stat... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}
		if (!str_cmp(arg4, "Primal-urge") && IS_SHIFTER(victim))
		sprintf(arg4, "survival");
		if (!str_cmp(arg4, "Rituals") && IS_SHIFTER(victim))
		sprintf(arg4, "occult");

		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < 10; i++)
			{
				if (!str_cmp (arg4, ability_names[j][i][0]))
				{
					abil_group = j;
					ability = i;
					found = TRUE;
				}

				if (found == TRUE)
					break;
			}
			if (found == TRUE)
				break;
		}

		if (found != TRUE)
		{
			sprintf (buf, "#RError: #nUnknown Ability... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		if (arg5[0] != '\0')
			difficulty = atoi (arg5);
		else
			difficulty = 5;
		if (difficulty > 10 || difficulty < 2)
		{
			sprintf (buf, "#RError: #nDifficulty rating must be between 2 and 10... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		ability_dice = victim->abilities[abil_group][ability];
		sprintf (buf, "Difficulty: -%d-, Victim's Attribute Raing: -%d- Victim's Ability Rating: -%d-\n\r", difficulty, victim->attributes[stat] + victim->mod_attributes[stat], victim->abilities[abil_group][ability]);
		send_to_char (buf, ch);
		sprintf( buf, "Rolled: ");
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char( buf, ch );
		logchan(buf,CHANNEL_LOG_DICE);
		for (i = 0; i < (victim->attributes[stat] + victim->mod_attributes[stat] + victim->abilities[abil_group][ability]); i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#C%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
			send_to_char (buf, ch);
		}

		if (bonus_dice > 0)
		{
			sprintf (buf, "Extra from 10s: ");
			
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
				send_to_char( buf, ch );
			logchan( buf, CHANNEL_LOG_DICE );

			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				sprintf (buf, "#C%d ", roll);
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
				logchan( buf, CHANNEL_LOG_DICE );
				
				if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
					send_to_char (buf, ch);
			}
		}


		if (success > 0)
			sprintf (buf, "The character had %d successes.\n\r", success);
		else if (success == 0)
			sprintf (buf, "The character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf (buf, "The character has #RBOTCHED#n.\n\r");
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char (buf, ch);
			logchan( buf, CHANNEL_LOG_DICE );
		
		

		return;
	}
	else if (!strcmp (arg1, "soak"))
	{
		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		difficulty = 6;
		if ( (arg2[0] == '\0') || arg3[0] == '\0' )
		{
			send_to_char ("Syntax: dice soak <target> <type>\n\r", ch);
			send_to_char ("Type: lethal, aggro, bash\n\r", ch );
			return;
		}

		if ((victim = get_char_world (ch, arg2)) == NULL)
		{
			send_to_char ("#RError: #nWhy would you try and roll for someone who is not here?.\n\r", ch);
			return;
		}
		if (!(!strcmp(arg3, "lethal") || !strcmp(arg3, "bash") || !strcmp(arg3, "aggro") || !strcmp(arg3, "bashing") || !strcmp(arg3, "aggravated")))
		{
			send_to_char("#RError: #nInvalid damage type", ch);
			return;
		}
		if (!strcmp (arg3, "lethal"))
		{
			if (!IS_SUPERNATURAL(victim) && 
			   !IS_SHIFTER(victim))
			{
				send_to_char("They can't soak that.\n\r",ch);
				return;
			}
		}

		if (!strcmp (arg3, "aggro") || !strcmp(arg3, "aggravated"))
		{
			if (IS_VAMPIRE(victim))
			{
				if (get_disc (victim, DISC_FORTITUDE) < 1)
				{
					send_to_char("They can't soak that.\n\r", ch);
					return;
				}
			}
			
			if (!IS_SUPERNATURAL(victim) && 
			  !IS_SHIFTER(victim))
			{
				send_to_char("They can't soak that.\n\r",ch);
				return;
			}
		}

		if (!strcmp (arg3, "bash") || !strcmp(arg3, "bashing"))
		{
			if (IS_VAMPIRE(victim))
			{
				send_to_char("Remember to halve damage, rounded down, after successes.\n\r", ch);
			}
		}

		sprintf (buf, "Soak Roll For %s:\n\r", victim->name);
		log_string_quiet( buf, CHANNEL_LOG_NORMAL );
		logchan( buf, CHANNEL_LOG_DICE );
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char (buf, ch);
		p = 0;
		sprintf( buf, "Rolled: ");
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char( buf, ch);
		logchan( buf, CHANNEL_LOG_DICE );
		if ((!strcmp (arg3, "aggro") || !strcmp(arg3, "aggravated")) && IS_VAMPIRE(victim))
		{
		for (i = 0; i < (get_disc (victim, DISC_FORTITUDE) ) + p; i++)
		  {
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
				success++;
			if (roll == 1)
				success--;
			sprintf (buf, "#C%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
			send_to_char (buf, ch);
		  }
		}
		else
		for (i = 0; i < (victim->attributes[ATTRIB_STA] + victim->mod_attributes[ATTRIB_STA] + get_disc (victim, DISC_FORTITUDE)) + p; i++)
		  {
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
				success++;
			if (roll == 1)
				success--;
			sprintf (buf, "#C%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
			send_to_char (buf, ch);
		  }
		if (success > 0)
			sprintf (buf, "The character has soaked %d damage.\n\r", success);
		else if (success == 0)
			sprintf (buf, "The character has soaked no damage.\n\r");
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char (buf, ch);
			logchan( buf, CHANNEL_LOG_DICE );
		
		return;
	}
	else if (!strcmp (arg1, "init"))
	{
		send_to_char ("#RRolling Initiative for the room\n\r", ch);
		sprintf( buf, "Dice Init Rolled\n\r");
		logchan(buf, CHANNEL_LOG_DICE );
		for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
		{
			if (IS_NPC (victim))
				continue;
			else
			{
				mod = victim->attributes[ATTRIB_DEX] + victim->attributes[ATTRIB_WIT];
				roll = rand () % 10 + 1;
				i = roll + mod;
				sprintf (buf, "%s: %d\n", victim->name, i);
				logchan( buf, CHANNEL_LOG_DICE );
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
				send_to_char (buf, ch);
			}
		}
		
	}
	else if (!strcmp (arg1, "generic"))
	{
		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char ("Syntax: dice generic <number> <difficulty> \n\r", ch);
			return;
		}
		else
			num_of_dice = atoi (arg2);
		argument = one_argument (argument, arg3);
		if (arg3[0] == '\0')
		{
			send_to_char ("Syntax: dice generic <number> <difficulty> \n\r", ch);
			return;
		}
		else
			difficulty = atoi (arg3);
		if (num_of_dice > 30 || num_of_dice < 1)
		{
			sprintf (buf, "#RError: #nNumber must be between 1 and 30... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		if (difficulty > 10 || difficulty < 1)
		{
			sprintf (buf, "#RError: #nDifficulty must be between 1 and 10... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		sprintf (buf, "Dice number: -%d-\n\r", num_of_dice);
		log_string_quiet( buf, CHANNEL_LOG_NORMAL );
		logchan( buf, CHANNEL_LOG_DICE );
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char (buf, ch);
		sprintf (buf, "Rolled: ");
		if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char( buf, ch );
		logchan( buf, CHANNEL_LOG_DICE );
		for (i = 0; i < num_of_dice; i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#C%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
			send_to_char (buf, ch);
		}

		if (bonus_dice > 0)
		{
			sprintf (buf, "Extra from 10s: ");
			
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
				send_to_char( buf, ch );
			logchan( buf, CHANNEL_LOG_DICE );

			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				sprintf (buf, "#C%d ", roll);
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
				logchan( buf, CHANNEL_LOG_DICE );
				
				if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
					send_to_char (buf, ch);
			}
		}

		if (success > 0)
			sprintf (buf, "The character had %d successes.\n\r", success);
		else if (success == 0)
			sprintf (buf, "The character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf (buf, "The character has #RBOTCHED#n.\n\r");
			if ((IS_SET(ch->deaf, CHANNEL_LOG_DICE) && IS_BUILDER(ch)) || (( IS_STORYTELLER(ch) && ch->level < LEVEL_BUILDER )))
		send_to_char (buf, ch);
			logchan( buf, CHANNEL_LOG_DICE );
		
		return;
	}
	else
	{
		send_to_char ("Syntax: dice <type>\n\r", ch);
		send_to_char ("Types: normal, generic, damage, soak, init\n\r", ch);
		return;
	}
}

void do_pdice (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int num_of_dice;
	int roll;
	int bonus_dice = 0;
	int i, mod, p;
	int success = 0;
	int difficulty = 0;
	bool had_a_success = FALSE;
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int stat;
	int abil_group;
	int ability;
	int ability_dice = 0;
	int j;
	int found = FALSE;
	strcpy(buf, "pdice ");
	strcat(buf, argument);
	sprintf( buf2, "#C[#wDice Log #C(#c%s#C) #B: #w%s #C]#n\n\r", ch->name, buf);
	log_string(buf2, CHANNEL_LOG_DICE);

	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;


	if (!strcmp (arg1, "damage"))
	{

		argument = one_argument (argument, arg1);
		difficulty = 6;
		if (arg1[0] == '\0')
		{
			send_to_char ("Syntax: pdice damage <## of dice>\n\r", ch);
			return;
		}
		else
			num_of_dice = atoi (arg1);
		if (num_of_dice > 30 || num_of_dice < 1)
		{
			sprintf (buf, "#RError: #nNumber of Dice must be between 1 and 30... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}
		sprintf(buf, "#e%s rolls #w%d#e damage dice.\n\r", ch->name, num_of_dice);
		act (buf, ch, NULL, NULL, -1);
		sprintf (buf, "#eRolled: ");
		logchan( buf, CHANNEL_LOG_DICE );
		for (i = 0; i < num_of_dice; i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#w%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
		}
		
		if (bonus_dice > 0)
		{
			sprintf (buf, "#eExtra from 10s: ");
			send_to_char( buf, ch );
			logchan( buf, CHANNEL_LOG_DICE );

			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				sprintf (buf, "#w%d ", roll);
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
				logchan( buf, CHANNEL_LOG_DICE );
			}
		}

		if (success > 0)
			sprintf (buf, "#eThe character had #w%d #esuccesses.\n\r", success);
		else if (success == 0)
			sprintf (buf, "#eThe character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf( buf, "#eThe character has #RBOTCHED!#n.\n\r");
	    act (buf, ch, NULL, NULL, TO_MORTS);
		logchan( buf, CHANNEL_LOG_DICE );
			
		
		return;
	}
	else if (!strcmp (arg1, "normal"))
	{


		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		argument = one_argument (argument, arg4);
		argument = one_argument (argument, arg5);
		if (arg2[0] == '\0')
		{
			send_to_char ("Syntax:\n\r", ch);
			send_to_char ("pdice normal <victim> <stat> <ability> <difficulty>\n\r", ch);
			send_to_char ("Stats: Str, Dex, Sta, Cha, Man, App, Per, Int, Wit\n\r", ch);
			send_to_char ("Abilities:\n\r", ch);
			send_to_char ("   Expression, Alertness, Athletics, Brawl, Dodge\n\r", ch);
			send_to_char ("   Empathy, Intimidation, Steetwise, Leadership, Subterfuge\n\r", ch);
			send_to_char ("   AnimalKen, Firearms, Crafts, Etiquette, Security\n\r", ch);
			send_to_char ("   Melee, Performance, Drive, Stealth, Survival\n\r", ch);
			send_to_char ("   Academics, Computer, Investigation, Law, Linguistics\n\r", ch);
			send_to_char ("   Medicine, Occult, Politics, Science, Finance\n\r", ch);
			send_to_char ("   Rituals, Primal-Urge\n\r", ch);
			send_to_char ("   Lore, Culture, Research\n\r", ch);
			send_to_char ("Difficulty: A number between 1 and 10\n\r", ch);
			return;
		}

		if ((victim = get_char_room (ch, arg2)) == NULL)
		{
			send_to_char ("#RError: #nTarget not found.\n\r", ch);
			return;
		}

		if (!str_cmp (arg3, "strength") || !str_cmp (arg3, "str"))
		{
			stat = ATTRIB_STR;
		}

		else if (!str_cmp (arg3, "dexterity") || !str_cmp (arg3, "dex"))
		{
			stat = ATTRIB_DEX;
		}

		else if (!str_cmp (arg3, "stamina") || !str_cmp (arg3, "sta"))
		{
			stat = ATTRIB_STA;
		}

		else if (!str_cmp (arg3, "charisma") || !str_cmp (arg3, "cha"))
		{
			stat = ATTRIB_CHA;
		}

		else if (!str_cmp (arg3, "manipulation") || !str_cmp (arg3, "man"))
		{
			stat = ATTRIB_MAN;
		}

		else if (!str_cmp (arg3, "appearance") || !str_cmp (arg3, "app"))
		{
			stat = ATTRIB_APP;
		}

		else if (!str_cmp (arg3, "perception") || !str_cmp (arg3, "per"))
		{
			stat = ATTRIB_PER;
		}

		else if (!str_cmp (arg3, "intelligence") || !str_cmp (arg3, "int"))
		{
			stat = ATTRIB_INT;
		}

		else if (!str_cmp (arg3, "wits") || !str_cmp (arg3, "wit"))
		{
			stat = ATTRIB_WIT;
		}
		else
		{
			sprintf (buf, "#RError: #nUnknown Stat...\n\r");
			send_to_char (buf, ch);
			return;
		}
		if (!str_cmp(arg4, "Primal-urge") && IS_SHIFTER(victim))
		sprintf(arg4, "survival");
		if (!str_cmp(arg4, "Rituals") && IS_SHIFTER(victim))
		sprintf(arg4, "occult");

		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < 10; i++)
			{
				if (!str_cmp (arg4, ability_names[j][i][0]))
				{
					abil_group = j;
					ability = i;
					found = TRUE;
				}

				if (found == TRUE)
					break;
			}
			if (found == TRUE)
				break;
		}

		if (found != TRUE)
		{
			sprintf (buf, "#RError: #nUnknown Ability...\n\r");
			send_to_char (buf, ch);
			return;
		}

		if (arg5[0] != '\0')
			difficulty = atoi (arg5);
		else
			difficulty = 5;
		if (difficulty > 10 || difficulty < 2)
		{
			sprintf (buf, "#RError: #nDifficulty rating must be between 2 and 10...\n\r");
			send_to_char (buf, ch);
			return;
		}
		if (ch == victim)
			sprintf(buf, "#e%s rolls $s #w%s#e and #w%s#e at difficulty #w%d#e.\n\r", ch->name, arg3, arg4, difficulty);
		else 
			sprintf(buf, "#e%s #erolls %s's #w%s#e and #w%s#e at difficulty #w%d#e.\n\r", ch->name, victim->name, arg3, arg4, difficulty);
		act (buf, ch, NULL, NULL, -1);
		ability_dice = victim->abilities[abil_group][ability];
		sprintf (buf, "Difficulty: -%d-, Victim's Attribute Raing: -%d- Victim's Ability Rating: -%d-\n\r", difficulty, victim->attributes[stat] + victim->mod_attributes[stat], victim->abilities[abil_group][ability]);
		logchan(buf, CHANNEL_LOG_DICE);
		sprintf( buf, "#eRolled: ");
		logchan(buf,CHANNEL_LOG_DICE);
		for (i = 0; i < (victim->attributes[stat] + victim->mod_attributes[stat] + victim->abilities[abil_group][ability]); i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#w%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
		}

		if (bonus_dice > 0)
		{
			sprintf (buf, "#eExtra from 10s: ");
			logchan( buf, CHANNEL_LOG_DICE );

			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				sprintf (buf, "#w%d ", roll);
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
				logchan( buf, CHANNEL_LOG_DICE );
			}
		}


		if (success > 0)
			sprintf (buf, "#eThe character had #w%d#e successes.\n\r", success);
		else if (success == 0)
			sprintf (buf, "#eThe character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf (buf, "#eThe character has #RBOTCHED#n.\n\r");
		act (buf, ch, NULL, NULL, TO_MORTS);
		logchan( buf, CHANNEL_LOG_DICE );
		return;
	}
	else if (!strcmp (arg1, "soak"))
	{
		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		difficulty = 6;
		if ( (arg2[0] == '\0') || arg3[0] == '\0' )
		{
			send_to_char ("Syntax: pdice soak <target> <type>\n\r", ch);
			send_to_char ("Type: lethal, aggro, bash\n\r", ch );
			return;
		}

		if ((victim = get_char_room (ch, arg2)) == NULL)
		{
			send_to_char ("#RError: #nTarget not found.\n\r", ch);
			return;
		}
		if (!(!strcmp(arg3, "lethal") || !strcmp(arg3, "bash") || !strcmp(arg3, "aggro") || !strcmp(arg3, "bashing") || !strcmp(arg3, "aggravated")))
		{
			send_to_char("#RError: #nInvalid damage type", ch);
			return;
		}

		if (ch == victim)
			sprintf(buf, "#e%s #erolls $s soak dice against #r%s#e damage.\n\r", ch->name, arg3);
		else 
			sprintf(buf, "#e%s #erolls %s's soak dice against #r%s#e damage.\n\r", ch->name, victim->name, arg3);
		act(buf, ch, NULL, NULL, -1);
		if (!strcmp (arg3, "lethal"))
		{
			if (!IS_SUPERNATURAL(victim) && 
			   !IS_SHIFTER(victim))
			{
				sprintf(buf, "#eThey can't soak that.\n\r");
				act(buf, ch, NULL, NULL, TO_MORTS);
				logchan(buf, CHANNEL_LOG_DICE);
				return;
			}
		}

		if (!strcmp (arg3, "aggro") || !strcmp(arg3, "aggravated"))
		{
			if (IS_VAMPIRE(victim))
			{
				if (get_disc (victim, DISC_FORTITUDE) < 1)
				{
					sprintf(buf, "#eThey can't soak that.\n\r");
					act(buf, ch, NULL, NULL, TO_MORTS);
					logchan(buf, CHANNEL_LOG_DICE);
					return;
				}
			}
			
			if (!IS_SUPERNATURAL(victim) && 
			  !IS_SHIFTER(victim))
			{
				sprintf(buf, "#eThey can't soak that.\n\r");
				act(buf, ch, NULL, NULL, TO_MORTS);
				logchan(buf, CHANNEL_LOG_DICE);
				return;
			}
		}

		if (!strcmp (arg3, "bash") || !strcmp(arg3, "bashing"))
		{
			if (IS_VAMPIRE(victim))
			{
				sprintf(buf, "#RRemember to halve damage, rounded down, after successes.\n\r");
				act(buf, ch, NULL, NULL, -1);
				return;
			}
		}
		p = 0;
		sprintf( buf, "#eRolled: ");
		logchan( buf, CHANNEL_LOG_DICE );
		if ((!strcmp (arg3, "aggro") || !strcmp(arg3, "aggravated")) && IS_VAMPIRE(victim))
		{
		for (i = 0; i < (get_disc (victim, DISC_FORTITUDE) ) + p; i++)
		  {
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
				success++;
			if (roll == 1)
				success--;
			sprintf (buf, "#w%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
		  }
		}
		else
		for (i = 0; i < (victim->attributes[ATTRIB_STA] + victim->mod_attributes[ATTRIB_STA] + get_disc (victim, DISC_FORTITUDE)) + p; i++)
		  {
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
				success++;
			if (roll == 1)
				success--;
			sprintf (buf, "#w%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
		  }
		if (success > 0)
			sprintf (buf, "#eThe character has soaked #w%d #edamage.\n\r", success);
		else if (success == 0)
			sprintf (buf, "#eThe character has soaked #Rno #edamage.\n\r");
		act(buf, ch, NULL, NULL, TO_MORTS);
		logchan( buf, CHANNEL_LOG_DICE );
		return;
	}
	else if (!strcmp (arg1, "init"))
	{
		sprintf(buf, "#e%s #erolls initiative for the room.\n\r", ch->name);
		send_to_char( buf, ch );
		act(buf, ch, NULL, NULL, TO_ROOM);
		for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
		{
			if (IS_NPC (victim))
				continue;
			else
			{
				mod = victim->attributes[ATTRIB_DEX] + victim->attributes[ATTRIB_WIT];
				roll = rand () % 10 + 1;
				i = roll + mod;
				sprintf (buf, "%s: %d\n", victim->name, i);
				act(buf, ch, NULL, NULL, TO_MORTS);
				logchan( buf, CHANNEL_LOG_DICE );
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			}
		}
		return;
	}
	else if (!strcmp (arg1, "generic"))
	{
		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char ("Syntax: pdice generic <number> <difficulty> \n\r", ch);
			return;
		}
		else
			num_of_dice = atoi (arg2);
		argument = one_argument (argument, arg3);
		if (arg3[0] == '\0')
		{
			send_to_char ("Syntax: pdice generic <number> <difficulty> \n\r", ch);
			return;
		}
		else
			difficulty = atoi (arg3);
		if (num_of_dice > 30 || num_of_dice < 1)
		{
			sprintf (buf, "#RError: #nNumber must be between 1 and 30... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		if (difficulty > 10 || difficulty < 1)
		{
			sprintf (buf, "#RError: #nDifficulty must be between 1 and 10... Aborting Roll\n\r");
			send_to_char (buf, ch);
			return;
		}

		sprintf (buf, "#e%s rolls #w%d#e dice at difficulty #w%d\n\r",ch->name, num_of_dice, difficulty);
		act(buf, ch, NULL, NULL, TO_ROOM);
		log_string_quiet( buf, CHANNEL_LOG_NORMAL );
		logchan( buf, CHANNEL_LOG_DICE );
		sprintf (buf, "#eRolled: ");
		logchan( buf, CHANNEL_LOG_DICE );
		for (i = 0; i < num_of_dice; i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#w%d ", roll);			
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
		}

		if (bonus_dice > 0)
		{
			
			sprintf (buf, "Extra from 10s: ");
			logchan(buf, CHANNEL_LOG_DICE );
			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				sprintf (buf, "#w%d ", roll);
				act(buf, ch, NULL, NULL, TO_MORTS);
				log_string_quiet( buf, CHANNEL_LOG_NORMAL );
				logchan( buf, CHANNEL_LOG_DICE );
			}
		}

		if (success > 0)
			sprintf (buf, "#eThe character had #w%d#e successes.\n\r", success);
		else if (success == 0)
			sprintf (buf, "#eThe character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf (buf, "#eThe character has #RBOTCHED#n.\n\r");
		act(buf, ch, NULL, NULL, TO_MORTS);
		logchan(buf, CHANNEL_LOG_DICE );
		return;
	}
	else
	{
		send_to_char ("Syntax: pdice <type>\n\r", ch);
		send_to_char ("Types: normal, generic, damage, soak, init\n\r", ch);
		return;
	}
	/* OLD PDICE COMMMAND
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	int roll;
	int bonus_dice = 0;
	int i;
	int success = 0;
	int difficulty = 0;
	bool had_a_success = FALSE;
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int stat;
	int abil_group;
	int ability;
	int ability_dice = 0;
	int j;
	int found = FALSE;
	
	strcpy(buf, "$n rolled playerdice ");
	strcat(buf, argument);
	sprintf( buf2, "#C[#wDice Log #C(#c%s#C) #B: #w%s #C]#n\n\r", ch->name, buf);
	log_string(buf2, CHANNEL_LOG_DICE);
	act (buf, ch, NULL, NULL, TO_NOTVICT);
	
	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;

	if (arg1[0] == '\0')
	{
		send_to_char ("Syntax: pdice normal player attrib ability diff\n\r", ch);
		return;
	}
	
	
	else if (!strcmp (arg1, "normal"))
	{


		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		argument = one_argument (argument, arg4);
		argument = one_argument (argument, arg5);
		if (arg2[0] == '\0')
		{
			send_to_char ("#cSyntax:\n\r", ch);
			send_to_char ("#cpdice normal <victim> <stat> <ability> <difficulty>\n\r", ch);
			send_to_char ("#c   Stats: Str, Dex, Sta, Cha, Man, App, Per, Int, Wit\n\r", ch);
			send_to_char ("#cAbilities:\n\r", ch);
			send_to_char ("#c   Expression, Alertness, Athletics, Brawl, Dodge\n\r", ch);
			send_to_char ("#c   Empathy, Intimidation, Streetwise, Leadership, Subterfuge\n\r", ch);
			send_to_char ("#c   AnimalKen, Firearms, Crafts, Etiquette, Security\n\r", ch);
			send_to_char ("#c   Melee, Performance, Drive, Stealth, Survival\n\r", ch);
			send_to_char ("#c   Academics, Computer, Investigation, Law, Linguistics\n\r", ch);
			send_to_char ("#c   Medicine, Occult, Politics, Science, Finance\n\r", ch);
			send_to_char ("#c   Rituals, Primal-Urge#n\n\r", ch);
	
			send_to_char ("#cDifficulty: A number between 1 and 10#n\n\r", ch);
			return;
		}

		if ((victim = get_char_room (ch, arg2)) == NULL)
		{
			send_to_char ("#cWhy would you try and roll for someone who is not here?#n\n\r", ch);
			return;
		}

		if (!str_cmp (arg3, "strength") || !str_cmp (arg3, "str"))
		{
			stat = ATTRIB_STR;
		}

		else if (!str_cmp (arg3, "dexterity") || !str_cmp (arg3, "dex"))
		{
			stat = ATTRIB_DEX;
		}

		else if (!str_cmp (arg3, "stamina") || !str_cmp (arg3, "sta"))
		{
			stat = ATTRIB_STA;
		}

		else if (!str_cmp (arg3, "charisma") || !str_cmp (arg3, "cha"))
		{
			stat = ATTRIB_CHA;
		}

		else if (!str_cmp (arg3, "manipulation") || !str_cmp (arg3, "man"))
		{
			stat = ATTRIB_MAN;
		}

		else if (!str_cmp (arg3, "appearance") || !str_cmp (arg3, "app"))
		{
			stat = ATTRIB_APP;
		}

		else if (!str_cmp (arg3, "perception") || !str_cmp (arg3, "per"))
		{
			stat = ATTRIB_PER;
		}

		else if (!str_cmp (arg3, "intelligence") || !str_cmp (arg3, "int"))
		{
			stat = ATTRIB_INT;
		}

		else if (!str_cmp (arg3, "wits") || !str_cmp (arg3, "wit"))
		{
			stat = ATTRIB_WIT;
		}
		else
		{
			sprintf (buf, "#cInvalid Attribute Choice.\n\r");
			send_to_char (buf, ch);
			return;
		}
		if (!str_cmp(arg4, "Primal-urge") && IS_SHIFTER(victim))
		sprintf(arg4, "survival");
		if (!str_cmp(arg4, "Rituals") && IS_SHIFTER(victim))
		sprintf(arg4, "occult");

		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < 10; i++)
			{
				if (!str_cmp (arg4, ability_names[j][i][0]))
				{
					abil_group = j;
					ability = i;
					found = TRUE;
				}

				if (found == TRUE)
					break;
			}
			if (found == TRUE)
				break;
		}

		if (found != TRUE)
		{
			sprintf (buf, "#cInvalid Ability#n\n\r");
			send_to_char (buf, ch);
			return;
		}

		if (arg5[0] != '\0')
			difficulty = atoi (arg5);
		else
			difficulty = 5;
		if (difficulty > 10 || difficulty < 2)
		{
			sprintf (buf, "#cDifficulty must be between 2 and 10#n\n\r");
			send_to_char (buf, ch);
			return;
		}

		ability_dice = victim->abilities[abil_group][ability];
		sprintf (buf, "Difficulty: -%d-\n\r", difficulty);
		send_to_char (buf, ch);
		for (i = 0; i < (victim->attributes[stat] + victim->mod_attributes[stat] + victim->abilities[abil_group][ability]); i++)
		{
			roll = rand () % 10 + 1;
			if (roll >= difficulty)
			{
				success++;
				had_a_success = TRUE;
			}
			if (roll == 1)
				success--;
			if (roll == 10)
				bonus_dice++;
			sprintf (buf, "#C%d ", roll);
			log_string_quiet( buf, CHANNEL_LOG_NORMAL );
			logchan( buf, CHANNEL_LOG_DICE );
			
		}

		if (bonus_dice > 0)
		{
			sprintf (buf, "Extra dice from 10's. \n\r");
			send_to_char( buf, ch );
			act (buf, ch, NULL, NULL, TO_NOTVICT);

			for (i = 0; i < bonus_dice; i++)
			{
				roll = rand () % 10 + 1;
				if (roll >= difficulty)
					success++;
				if (roll == 10)
					bonus_dice++;
				
				
			}
		}


		if (success > 0)
			sprintf (buf, "#CThe character had %d successes.\n\r", success);
		else if (success == 0)
			sprintf (buf, "#CThe character has no successes.\n\r");
		else if (success < 0 && had_a_success == FALSE)
			sprintf (buf, "#CThe character has #RBOTCHED#n.\n\r");
		send_to_char (buf, ch);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );

		return;
	}
	
	else
	{
		send_to_char ("#cSyntax: pdice normal player attrib ability diff\n\r", ch);
	}
	*/
}


void do_brestore( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        DESCRIPTOR_DATA *d;

        sprintf(buf,"%s: Brestore %s",ch->name,argument);

        one_argument( argument, arg );

		  if ( arg[0] == '\0' )
        {
                  send_to_char( "Restore whose blood?\n\r", ch );
                  return;  
        }

        if ( get_trust(ch) >=  MAX_LEVEL - 7 && !str_cmp(arg,"all"))
         {
                  for (d = descriptor_list; d != NULL; d = d->next)
                  {             
                                victim = d->character;
                                         
                                if (victim == NULL || IS_NPC(victim))
                                         continue;

				{
					victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
 					update_pos( victim);
													 if (victim->in_room != NULL)
						act("$n has restored your bloodpool.",ch,NULL,victim,TO_VICT);
				}
                  }
                  send_to_char("All active players have had their blood restored.\n\r",ch);  
                  return;
         }
	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
         {
                  send_to_char( "They aren't here.\n\r", ch );
                  return;
         }

    victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
    update_pos( victim );
    act( "$n has restored your blood.", ch, NULL, victim, TO_VICT );
    stc("Ok.\n\r",ch);
    return;
}


EBAN_DATA	*	eban_free;
EBAN_DATA	*	eban_list;

void do_eban( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   EBAN_DATA *pban;

   if( ch && IS_NPC(ch) )
   return;

   one_argument( argument, arg );

   if ( arg[0] == '\0' && ch )
   {
	strcpy( buf, "Banned emails: \n\r" );
  	for ( pban = eban_list; pban != NULL; pban = pban->next )
	{
	   strcat( buf, pban->name );
	   strcat( buf, "\n\r" );
	}
	write_to_buffer(ch->desc, buf, 0);
	return;
   }
   else if( arg[0] == '\0' )
   {
	return;
   }

   for ( pban = eban_list; pban != NULL; pban = pban->next )
   {
	if ( !str_cmp( arg, pban->name ) )
	{
	   if( ch )
		send_to_char( "That email is already banned.\n\r", ch );
	   return;
	}
   }

   if( eban_free == NULL )
   {
	pban = alloc_perm( sizeof( *pban ) );
   }
   else
   {
	pban = eban_free;
	eban_free = eban_free->next;
   }

   pban->name = str_dup( arg );
   pban->next = eban_list;
   eban_list  = pban;

   if( ch )
	send_to_char( "Email banned.\n\r", ch );
   save_eban_file ();
   return;
}

void do_eallow( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   EBAN_DATA *prev;
   EBAN_DATA *curr;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
	send_to_char( "Remove which email from the eban list?\n\r", ch);
	return;
   }

   prev = NULL;

   for( curr = eban_list; curr != NULL; prev = curr, curr = curr->next )
   {
	if ( !str_cmp( arg, curr->name ) )
	{
	   if ( prev == NULL )
		eban_list = eban_list->next;
	   else
		prev->next = curr->next;

	   free_string( curr->name );
	   curr->next = eban_free;
	   eban_free = curr;
	   send_to_char( "Ok.\n\r", ch );
	   save_eban_file ();
	   return;
	}
   }

   send_to_char( "That email is not banned currently.\n\r", ch);
   return;
}

void do_vrestore( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        DESCRIPTOR_DATA *d;

        sprintf(buf,"%s: Vrestore %s",ch->name,argument);

        one_argument( argument, arg );

		  if ( arg[0] == '\0' )
        {
                  send_to_char( "Restore whose vim?\n\r", ch );
                  return;  
        }

        if ( get_trust(ch) >=  MAX_LEVEL - 2 && !str_cmp(arg,"all"))
         {
                  for (d = descriptor_list; d != NULL; d = d->next)
                  {             
                                victim = d->character;
                                         
                                if (victim == NULL || IS_NPC(victim))
                                         continue;
			
				{
					victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
 					update_pos( victim);
													 if (victim->in_room != NULL)
						act("$n has restored your vim.",ch,NULL,victim,TO_VICT);
				}
                  }
                  send_to_char("All active demons have had their vim restored.\n\r",ch);  
                  return;
         }
	 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
         {
                  send_to_char( "They aren't here.\n\r", ch );
                  return;
         }

    victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
    update_pos( victim );
    act( "$n has restored your vim.", ch, NULL, victim, TO_VICT );
    stc("Ok.\n\r",ch);
    return;
}

void clear_stats (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int disc;

	if (IS_NPC (ch))
		return;

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->wear_loc != WEAR_NONE)
		{
			obj_from_char (obj);
			obj_to_char (obj, ch);
		}
	}
	while (ch->affected)
		affect_remove (ch, ch->affected);
	ch->armor = 100;
	ch->hitroll = 0;
	ch->damroll = 0;
	ch->saving_throw = 0;
	ch->pcdata->demonic = 0;
	ch->pcdata->atm = 0;
	ch->pcdata->followers = 0;
	ch->pcdata->powers_set_wear = 0;

	if (ch->polyaff > 0)
	{
		if (IS_POLYAFF (ch, POLY_BAT) || IS_POLYAFF (ch, POLY_WOLF) || IS_POLYAFF (ch, POLY_MIST) || IS_POLYAFF (ch, POLY_SERPENT) || IS_POLYAFF (ch, POLY_RAVEN) || IS_POLYAFF (ch, POLY_FISH) || IS_POLYAFF (ch, POLY_FROG) || IS_POLYAFF (ch, POLY_SHADOW))
		{
			do_autosave (ch, "");
			return;
		}
	}
	if (IS_VAMPIRE (ch) && ch->polyaff < 1)
	{
		for (disc = DISC_ANIMALISM; disc <= DISC_VICISSITUDE; disc++)
		{
			ch->pcdata->powers_mod[disc] = 0;
			ch->pcdata->powers_set[disc] = 0;
		}
	}

	do_autosave (ch, "");
	return;
}

void do_relevel (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}


	/*Implementors */
	if (!str_cmp (ch->name, "Freya"))
	{
		ch->level = MAX_LEVEL;
		ch->trust = MAX_LEVEL;
		send_to_char ("Resetting to Implementor.\n\r", ch);
		ch->pcdata->security = 1;
		send_to_char ("Security group 1.\n\r", ch);
		SET_BIT (ch->act, PLR_HOLYLIGHT);
		send_to_char ("Holy light mode on.\n\r", ch);
		send_to_char ("Done.\n\r", ch);
	}
	else
		send_to_char ("#cHuh??\n\r", ch);
	return;
}

void oset_affect (CHAR_DATA * ch, OBJ_DATA * obj, int value, int affect, bool is_quest)
{
	char buf[MAX_STRING_LENGTH];
	AFFECT_DATA *paf;
	int quest;
	int range;
	int cost;
	int max;

	if (IS_NPC (ch))
	{
		send_to_char ("Switch back, smart ass.\n\r", ch);
		return;
	}

	if (value == 0)
	{
		send_to_char ("Please enter a positive or negative amount.\n\r", ch);
		return;
	}

	if (!IS_JUDGE (ch) && (obj->questowner == NULL))
	{
		send_to_char ("First you must set the owners name on the object.\n\r", ch);
		return;
	}

	if (!IS_JUDGE (ch) && (obj->questmaker == NULL || str_cmp (ch->name, obj->questmaker)) && !is_quest)
	{
		send_to_char ("That item has already been oset by someone else.\n\r", ch);
		return;
	}

	if (affect == APPLY_HIT)
	{
		range = 25;
		cost = 5;
	}
	else if (affect == APPLY_MANA)
	{
		range = 25;
		cost = 5;
	}
	else if (affect == APPLY_MOVE)
	{
		range = 25;
		cost = 5;
	}
	else if (affect == APPLY_HITROLL)
	{
		range = 5;
		cost = 30;
	}
	else if (affect == APPLY_DAMROLL)
	{
		range = 5;
		cost = 30;
	}
	else if (affect == APPLY_AC)
	{
		range = 25;
		cost = 10;
	}
	else
		return;

	if (obj->pIndexData->vnum == OBJ_VNUM_PROTOPLASM)
	{
		range *= 2;
		max = 750 + (ch->race * 10);
	}
	else
		max = 400 + (ch->race * 10);
	if (obj->item_type == ITEM_WEAPON)
	{
		max *= 2;
		range *= 2;
	}
	if (!IS_JUDGE (ch) && ((value > 0 && value > range) || (value < 0 && value < (range - range - range))))
	{
		send_to_char ("That is not within the acceptable range...\n\r", ch);
		send_to_char ("Hp, Mana, Move............ max =  25 each, at   5 quest point per point.\n\r", ch);
		send_to_char ("Hitroll, Damroll.......... max =   5 each, at  30 quest points per point.\n\r", ch);
		send_to_char ("Ac........................ max = -25,      at  10 points per point.\n\r", ch);
		send_to_char ("\n\rNote: Created items can have upto 2 times the above maximum.\n\r", ch);
		send_to_char ("Also: Weapons may have upto 2 (4 for created) times the above maximum.\n\r", ch);
		return;
	}

	cost *= value;

	if (cost < 0)
		cost = 0;

	if (!IS_JUDGE (ch) && IS_SET (obj->quest, quest))
	{
		send_to_char ("That affect has already been set on this object.\n\r", ch);
		return;
	}

	if (!IS_JUDGE (ch) && (obj->points + cost > max))
	{
		sprintf (buf, "You are limited to %d quest points per item.\n\r", max);
		send_to_char (buf, ch);
		return;
	}

	if (is_quest && ch->pcdata->quest < cost)
	{
		sprintf (buf, "That costs %d quest points, while you only have %d.\n\r", cost, ch->pcdata->quest);
		send_to_char (buf, ch);
		return;
	}

	if (!IS_SET (obj->quest, quest))
		SET_BIT (obj->quest, quest);

	if (is_quest)
		ch->pcdata->quest -= cost;
	obj->points += cost;
	if (obj->questmaker != NULL)
		free_string (obj->questmaker);
	obj->questmaker = str_dup (ch->name);

	if (affect_free == NULL)
	{
		paf = alloc_perm (sizeof (*paf));
	}
	else
	{
		paf = affect_free;
		affect_free = affect_free->next;
	}

	paf->type = -1;
	paf->duration = -1;
	paf->location = affect;
	paf->modifier = value;
	paf->bitvector = 0;
	paf->next = obj->affected;
	obj->affected = paf;

	send_to_char ("Ok.\n\r", ch);
	return;
}

void do_clist (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *wch;
	char buf[MAX_STRING_LENGTH];

	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (IS_NPC (wch))
			continue;
		if (!can_see(ch,wch)) continue;
		if (wch->pcdata->coterie == NULL)
			continue;
		if (wch->pcdata->leader == NULL)
			continue;

		if (strlen (wch->pcdata->coterie) < 2 || strlen (wch->pcdata->leader) < 2)
		{
			sprintf (buf, "[%s] No coterie.\n\r", wch->name);
			send_to_char (buf, ch);
		}
		else
		{
			sprintf (buf, "[%s] The %s coterie, lead by %s.\n\r", wch->name, wch->pcdata->coterie, wch->pcdata->leader);
			send_to_char (buf, ch);
		}
	}
	return;
}


void do_protect( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (argument[0] == '\0')
    {
	send_to_char("Protect whom from snooping?\n\r",ch);
	return;
    }

    if ((victim = get_char_world(ch,argument)) == NULL)
    {
	send_to_char("You can't find them.\n\r",ch);
	return;
    }
 
    if (IS_SET(victim->act,PLR_SNOOP_PROOF))
    {
	send_to_char("Your snoop-proofing was just removed.\n\r",victim);
	REMOVE_BIT(victim->act,PLR_SNOOP_PROOF);
    }
    else
    {
	send_to_char("You are now immune to snooping.\n\r",victim);
	SET_BIT(victim->act,PLR_SNOOP_PROOF);
    }

    send_to_char( "Done.\n\r", ch);
    return;
}

void do_prestore( CHAR_DATA * ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char newbuf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

        if (arg[0] != '\0' && !IS_IMP(ch))
        {
                send_to_char ("Who are you trying to restore?\n\r", ch);
                return;
        }
        else if (IS_IMP(ch))
        {
                if (!check_parse_name (arg))
                {
                        send_to_char ("That's an illegal name.\n\r", ch);
                        return;
                }

                if (!char_exists (FILE_PLAYER_BAK, arg))
                {
                        send_to_char ("That player doesn't exist.\n\r", ch);
                        return;
                }
                sprintf (buf, "%s%s", PLAYER_BAK_DIR, capitalize (arg));
                sprintf (newbuf, "cp %s %s", buf, PLAYER_DIR );
                system (newbuf);
		send_to_char( "Player restored.\n\r", ch );
		return;
	}

	send_to_char( "Syntax: Prestore <name>\n\r", ch );
	return;
}

void do_nature (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Whose nature do you wish to set?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
		return;
	smash_tilde (argument);
	if (strlen (argument) > 25)
	{
		send_to_char ("Nature can't be more than 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove nature.\n\r", ch);
		return;
	}
	free_string (victim->nature);
	victim->nature = str_dup (argument);
	send_to_char ("Nature set.\n\r", ch);
	send_to_char ("Nature set.\n\r", victim);
	return;
}


void do_demeanor (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Whose demeanor do you wish to set?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
		return;
	smash_tilde (argument);
	if (strlen (argument) > 25)
	{
		send_to_char ("Demeanor can't be more than 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove demeanor.\n\r", ch);
		return;
	}
	free_string (victim->demeanor);
	victim->demeanor = str_dup (argument);
	send_to_char ("Demeanor set.\n\r", ch);
	send_to_char ("Demeanor set.\n\r", victim);
	return;
}


void do_impulse (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Whose impulse do you wish to set?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
		return;
		
	if (!IS_INQUISITOR(victim))
	{
		send_to_char ("Impulse is only for Inquisitors.\n\r", ch);
		return;
	}
	
	smash_tilde (argument);
	if (strlen (argument) > 25)
	{
		send_to_char ("Impulse can't be more than 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove impulse.\n\r", ch);
		return;
	}
	free_string (victim->impulse);
	victim->impulse = str_dup (argument);
	send_to_char ("Impulse set.\n\r", ch);
	send_to_char ("Impulse set.\n\r", victim);
	return;
}

void * update_file(void *args)
{
	system("../src/get_weather.pl KMSY");
	return NULL;
}

void do_update_weather_file (CHAR_DATA * ch, char * argument)
{
	pthread_t pth;

	if (pthread_create(&pth,NULL,update_file,"foo") == 0)
		send_to_char("File updating\r\n", ch);
	else
		send_to_char ("Error creating thread\r\n", ch);
}

void do_setOrder (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;
	if (arg[0] == '\0')
	{
		send_to_char ("Whose order do you wish to set?\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
		return;
		
	if (!IS_INQUISITOR(victim))
	{
		send_to_char ("Order is only for Inquisitors.\n\r", ch);
		return;
	}
	
	smash_tilde (argument);
	if (strlen (argument) > 25)
	{
		send_to_char ("Order can't be more than 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove Order.\n\r", ch);
		return;
	}
	free_string (victim->order);
	victim->order = str_dup (argument);
	send_to_char ("Order set.\n\r", ch);
	send_to_char ("Order set.\n\r", victim);
	return;
}

void do_reseed (CHAR_DATA * ch, char *argument)
{
	srand(time(0));
	
	send_to_char("#eBeautiful and care free, that's how I used to be.\n\r", ch);
	do_info(ch, "#RBADA DOO BWOOW!!!");
	return;
}

/* Merits/Flaws Stuff */
// Merits
void do_give_merit(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * victim;
        char arg1[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
	int i;

        argument = one_argument (argument, arg1);

        if (arg1[0] == '\0' || argument[0] =='\0')
        {
                send_to_char ("Syntax: givemerit <player> <merit>\n\r",ch);
                return;
        }

        if ((victim = get_char_world (ch, arg1)) == NULL)
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

	argument = strlower(argument);

	for (i = 0; i < 10; i++)
	{
		if (victim->pcdata->merits[i] == NULL)
		{
			victim->pcdata->merits[i] = (char *)malloc(sizeof(char) * strlen(argument));
			victim->pcdata->merits[i] = strdup(argument);
			sprintf(buf, "%s has been given the merit: %s.\r\n", victim->name, argument);
			send_to_char(buf, ch);
			return;
		}
	}
	send_to_char("This character already has 10 merits\r\n", ch);
}


void do_rem_merit(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * victim;
        char arg1[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
	int merit_number;

        argument = one_argument (argument, arg1);


        if (arg1[0] == '\0' || argument[0] =='\0')
        {
                send_to_char ("Syntax: remmerit <player> <merit>\n\r",ch);
                return;
        }

        if ((victim = get_char_world (ch, arg1)) == NULL)
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

	merit_number = find_merit(victim, argument);

	if (merit_number == -1)
	{
		sprintf(buf, "%s doesn't have the merit: %s.\r\n", victim->name, argument);
		send_to_char(buf, ch);
		return;
	}
	else
	{
		free(victim->pcdata->merits[merit_number]);
		victim->pcdata->merits[merit_number] = NULL;
		sprintf(buf, "%s has been removed from %s.\r\n", argument, victim->name);
		send_to_char(buf, ch);
	}
}


int find_merit(CHAR_DATA * ch, char * merit_name)
{
	int i;

	merit_name = strlower(merit_name);

	for (i = 0; i < 10; i++)
	{
		if(ch->pcdata->merits[i] != NULL)
			if(strcmp(merit_name, ch->pcdata->merits[i]) == 0)
				return i;
	}

	return -1;
}


bool has_merit(CHAR_DATA * ch, char * merit)
{
	if (find_merit(ch, merit) == -1)
		return FALSE;
	else
		return TRUE;
}


// Flaws
void do_give_flaw(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * victim;
        char arg1[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
	int i;

        argument = one_argument (argument, arg1);

        if (arg1[0] == '\0' || argument[0] =='\0')
        {
                send_to_char ("Syntax: giveflaw <player> <flaw>\n\r",ch);
                return;
        }

        if ((victim = get_char_world (ch, arg1)) == NULL)
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

	argument = strlower(argument);

	for (i = 0; i < 10; i++)
	{
		if (victim->pcdata->flaws[i] == NULL)
		{
			victim->pcdata->flaws[i] = (char *)malloc(sizeof(char) * strlen(argument));
			victim->pcdata->flaws[i] = strdup(argument);
			sprintf(buf, "%s has been given the flaw: %s.\r\n", victim->name, argument);
			send_to_char(buf, ch);
			return;
		}
	}
	send_to_char("This character already has 10 flaws\r\n", ch);
}


void do_rem_flaw(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * victim;
        char arg1[MAX_INPUT_LENGTH];
        char buf[MAX_STRING_LENGTH];
	int flaw_number;

        argument = one_argument (argument, arg1);


        if (arg1[0] == '\0' || argument[0] =='\0')
        {
                send_to_char ("Syntax: remflaw <player> <flaw>\n\r",ch);
        }

        if ((victim = get_char_world (ch, arg1)) == NULL)
        {
                send_to_char ("They aren't here.\n\r", ch);
                return;
        }

	flaw_number = find_flaw(victim, argument);

	if (flaw_number == -1)
	{
		sprintf(buf, "%s doesn't have the flaw: %s.\r\n", victim->name, argument);
		send_to_char(buf, ch);
		return;
	}
	else
	{
		free(victim->pcdata->flaws[flaw_number]);
		victim->pcdata->flaws[flaw_number] = NULL;
		sprintf(buf, "%s has been removed from %s.\r\n", argument, victim->name);
		send_to_char(buf, ch);
	}
}


int find_flaw(CHAR_DATA * ch, char * flaw_name)
{
	int i;

	flaw_name = strlower(flaw_name);
	
	for (i = 0; i < 10; i++)
	{
		if(ch->pcdata->flaws[i] != NULL)
			if(strcmp(flaw_name, ch->pcdata->flaws[i]) == 0)
				return i;
	}

	return -1;
}


bool has_flaw(CHAR_DATA * ch, char * flaw)
{
	if (find_flaw(ch, flaw) == -1)
		return FALSE;
	else
		return TRUE;
}

/* End Merits/Flaws Stuff */

void do_set_damage (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int damage;
	int damage_type;
	int current_total_damage;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (!IS_STORYTELLER (ch))
	{
		victim = ch;
		if (arg2[0] == '\0' || arg1[0] == '\0')
		{
			send_to_char("#eSyntax: damage <amount> bashing/lethal/agg#n\n\r", ch);
			return;
		}
		damage = atoi(arg1);
		current_total_damage = victim->pcdata->aggdamage + victim->pcdata->bashingdamage + victim->pcdata->lethaldamage;
	
		if ((current_total_damage + damage) >= 10)
		{
			send_to_char ("#eThat much damage would kill you.#n\n\r", ch);
			return;
		}
		if ((current_total_damage + damage) < 0)
		{
			send_to_char ("#eYou can't have negative damage...#n\n\r", ch);
			return;
		}

		if (!strcmp (arg2, "agg"))
			damage_type = 1;
		else if (!strcmp (arg2, "bashing"))
			damage_type = 2;
		else if (!strcmp (arg2, "lethal"))
			damage_type = 3;
		else
		{
			send_to_char("#eSyntax: damage <value> bashing/lethal/agg\n\r", ch);
			return;
		}
		switch (damage_type)
			{
				case 1:
					victim->pcdata->aggdamage = victim->pcdata->aggdamage + damage;
					if (damage > 0){
						sprintf(buf, "#cYou take #R%d #caggravated damage.#n\n\r", damage);
						send_to_char(buf, victim);
					}
					else{
						sprintf(buf, "#cYou are healed of #B%d #caggravated damage.#n\n\r", abs(damage));
						send_to_char(buf, victim);
					}
					break;

				case 2:
					victim->pcdata->bashingdamage = victim->pcdata->bashingdamage + damage;
					if (damage > 0){
						sprintf(buf, "#cYou take #R%d #cbashing damage.#n\n\r", damage);
						send_to_char(buf, victim);
					}
					else{
						sprintf(buf, "#cYou are healed of #B%d #cbashing damage.#n\n\r", abs(damage));
						send_to_char(buf, victim);
					}
					break;

				case 3:
					victim->pcdata->lethaldamage = victim->pcdata->lethaldamage + damage;
					if (damage > 0){
						sprintf(buf, "#cYou take #R%d #clethal damage.#n\n\r", damage);
						send_to_char(buf, victim);
					}
					else{
						sprintf(buf, "#cYou are healed of #B%d #clethal damage.#n\n\r", abs(damage));
						send_to_char(buf, victim);
					}
					break;

				default:
					break;
			}
	}
	else
	{	
		if (arg1[0] == '\0')
		{
			send_to_char("Whose damage do you wish to set?\n\r", ch);
			return;
		}

		if ((victim = get_char_world (ch, arg1)) == NULL)
		{
			send_to_char ("They aren't here.\n\r", ch);
			return;
		}

		if (IS_NPC (victim))
			return;

		if (arg2[0] != '\0')
		{
			damage = atoi(arg2);
			current_total_damage = victim->pcdata->aggdamage + victim->pcdata->bashingdamage + victim->pcdata->lethaldamage;
		
			if ((current_total_damage + damage) >= 10)
			{
				send_to_char ("That much damage would kill them.\n\r", ch);
				return;
			}
			if ((current_total_damage + damage) < 0)
			{
				send_to_char ("They can have a negative amount of wounds, eh?\n\r", ch);
				return;
			}
		
			
		
		}
		else
		{
			send_to_char("How much damage do you wish to give?\n\r", ch);
			return;
		}

		if (arg3[0] == '\0')
		{
			send_to_char ("Syntax: damage victim value agg/bashing/lethal\n\r", ch);
			return;
		}
		else if (!strcmp (arg3, "agg"))
			damage_type = 1;
		else if (!strcmp (arg3, "bashing"))
			damage_type = 2;
		else if (!strcmp (arg3, "lethal"))
			damage_type = 3;
		else
		{
			send_to_char("Syntax: damage victim value agg/bashing/lethal\n\r", ch);
			return;
		}
		switch (damage_type)
			{
				case 1:
					victim->pcdata->aggdamage = victim->pcdata->aggdamage + damage;
					sprintf(buf, "You deal %d aggravated damage to %s.\n\r", damage, victim->name);
					send_to_char(buf, ch);
					if (damage > 0){
						sprintf(buf, "#cYou take #R%d #caggravated damage.#n\n\r", damage);
						send_to_char(buf, victim);
					}
					else{
						sprintf(buf, "#cYou are healed of #B%d #caggravated damage.#n\n\r", abs(damage));
						send_to_char(buf, victim);
					}
					
					break;

				case 2:
					victim->pcdata->bashingdamage = victim->pcdata->bashingdamage + damage;
					sprintf(buf, "You deal %d bashing damage to %s.\n\r", damage, victim->name);
					send_to_char(buf, ch);
					if (damage > 0){
						sprintf(buf, "#cYou take #R%d #cbashing damage.#n\n\r", damage);
						send_to_char(buf, victim);
					}
					else{
						sprintf(buf, "#cYou are healed of #B%d #cbashing damage.#n\n\r", abs(damage));
						send_to_char(buf, victim);
					}
					break;

				case 3:
					victim->pcdata->lethaldamage = victim->pcdata->lethaldamage + damage;
					sprintf(buf, "You deal %d lethal damage to %s.\n\r", damage, victim->name);
					send_to_char(buf, ch);
					if (damage > 0){
						sprintf(buf, "#cYou take #R%d #clethal damage.#n\n\r", damage);
						send_to_char(buf, victim);
					}
					else{
						sprintf(buf, "#cYou are healed of #B%d #clethal damage.#n\n\r", abs(damage));
						send_to_char(buf, victim);
					}
					break;

				default:
					break;
			}
	}
	return;
}

