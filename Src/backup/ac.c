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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"



/*
 * Local functions.
 */
void talk_channel args ((CHAR_DATA * ch, char *argument, int channel, const char *verb));
char *oldelang args ((CHAR_DATA * ch, char *argument));
char *socialc args ((CHAR_DATA * ch, char *argument, char *you, char *them));
char *socialv args ((CHAR_DATA * ch, char *argument, char *you, char *them));
char *socialn args ((CHAR_DATA * ch, char *argument, char *you, char *them));
char *colorPose args ((char *argument));


char *skill_level (int percent)
{
	percent = PERCENTAGE (percent, 100);

	if (percent <= 0)
		return "unknown";
	if (percent == 1)
		return "untrained";
	if (percent < 10)
		return "dabbling";
	if (percent < 20)
		return "studying";
	if (percent < 30)
		return "novice";
	if (percent < 40)
		return "mediocre";
	if (percent < 50)
		return "adequate";
	if (percent < 60)
		return "advancing";
	if (percent < 70)
		return "skilled";
	if (percent < 80)
		return "trained";
	if (percent < 100)
		return "mastering";
	if (percent >= 100)
		return "an adept";
	return "unknown";
};


/*
 * Generic channel function.
 */
void talk_channel (CHAR_DATA * ch, char *argument, int channel, const char *verb)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	int position;
	
	
	if (argument[0] == '\0')
	{
		sprintf (buf, "%s what?\n\r", verb);
		buf[0] = UPPER (buf[0]);
		return;
	}

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		sprintf (buf, "You can't %s.\n\r", verb);
		send_to_char (buf, ch);
		return;
	}

	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		sprintf (buf, "You can't %s.\n\r", verb);
		send_to_char (buf, ch);
		return;
	}

	REMOVE_BIT (ch->deaf, channel);

	switch (channel)
	{
	default:

		sprintf (buf, "You %s '#R$T#n'", verb);
		act (buf, ch, NULL, argument, TO_CHAR);

		sprintf (buf, "$n %ss '#R$t#n'", verb);
		sprintf (buf2, "$n %ss '#R$t#n'", verb);
		break;

	case CHANNEL_IMMTALK:
		sprintf (buf, "#p<#bIMM#p> #p$n#b: #c$t");
		sprintf (buf2, "$n: $t");
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;

	case CHANNEL_OOC:
		sprintf (buf, "#B<#cOOC#B> #cYou#B:#c $t#n");
/* To switch back to names in OOC swap the if/else blocks */

			if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#B<#cOOC#B>#c Someone#B:#c $t#n");
		else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && (IS_STORYTELLER(ch)))
			sprintf (buf2, "#B<#cOOC#B>#c %s#B:#c $t#n", ch->morph);
		else
			sprintf (buf2, "#B<#cOOC#B>#c %s#B:#c $t#n", ch->name);
	
		/*	if (IS_STORYTELLER(ch))
				sprintf (buf2, "#B<#cOOC#B>#c $n#B:#c $t#n");
			else
				sprintf(buf2, "#B<#cOOC#B>#c %s#B:#c $t#n", capitalize (ch->name));
*/
		/*	
		if (IS_STORYTELLER(ch))
				sprintf (buf2, "#B<#cOOC#B>#c $n#B:#c $t#n");
			else if (strlen(ch->pcdata->roomdesc) > 1)
				sprintf (buf2, "#B<#cOOC#B>#c %s#B:#c $t#n", ch->pcdata->roomdesc);
			else 
				sprintf(buf2, "#B<#cOOC#B>#c Some new person#B:#c $t#n");
				*/
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;

/* New OMOTE Code */
	case CHANNEL_OMOTE:
/* To switch back to names in OMOTE remove the if/else
* keep the sprintf from the if and change the ch->pcdata->roomdesc to ch->name
* then swap the if/else blocks inside the lower else 
*/
			sprintf (buf, "#B<#cOOC#B> #c%s#B#c $t#n", ch->name);
		//if (strlen(ch->pcdata->roomdesc) > 1)
			//sprintf (buf, "#B<#cOOC#B> #c%s#c $t#n", ch->pcdata->roomdesc);
		//else
			//sprintf (buf, "#B<#cOOC#B> #cSome new person#c $t#n");
		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#B<#cOOC#B>#c Someone#c $t#n");
		else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && (IS_STORYTELLER(ch)))
			sprintf (buf2, "#B<#cOOC#B>#c %s#c $t#n", ch->morph);
		else
			sprintf (buf2, "#B<#cOOC#B>#c %s#c $t#n", ch->name);
/*
			if (IS_STORYTELLER(ch))
				sprintf (buf2, "#B<#cOOC#B>#c $n#c $t#n");
			else
				sprintf (buf2, "#B<#cOOC#B>#c %s#c $t#n", ch->name);
*/
			/*if (IS_STORYTELLER(ch))
				sprintf (buf2, "#B<#cOOC#B>#c $n#c $t#n");
			else if (strlen(ch->pcdata->roomdesc) > 1)
				sprintf (buf2, "#B<#cOOC#B>#c %s#c $t#n", ch->pcdata->roomdesc);
			else
				sprintf(buf2, "#B<#cOOC#B>#c Some new person#c $t#n");
		    }*/
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;
/* End OMOTE Code */

	case CHANNEL_OSAY:
		sprintf (buf, "#bYou osay #B'#c$t#B'#n");
		
		
		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#bA Storyteller osays #B'#c$t#B'#n");
			
		else if (IS_MORE3(ch, MORE3_OBFUS2) || IS_MORE3(ch, MORE3_OBFUS1))
			sprintf (buf2, "#bAn obfuscated player osays #B'#c$t#B'#n");
		else
				sprintf (buf2, "#b%s#b osays #B'#c$t#B'#n", ch->pcdata->roomdesc);
			
		    
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;

	case CHANNEL_STORY:
		sprintf (buf, "#e<#CStory#e> #c%s#e: #c$t#n", capitalize (ch->name));
		sprintf (buf2, "#e<#CStory#e> #c%s#e: #c$t#e#n", capitalize (ch->name));
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;

	case CHANNEL_STORYINFORM:
		sprintf (buf, "#eYou request of the storytellers#w:#e $t#n");
		sprintf (buf2, "#w%s requests #e: #w$t#n", capitalize (ch->name));
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;

	}

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		CHAR_DATA *och;
		CHAR_DATA *vch;

		och = d->original ? d->original : d->character;
		vch = d->character;

		if (d->connected == CON_PLAYING && vch != ch && !IS_SET (och->deaf, channel))
		{
			if (channel == CHANNEL_IMMTALK && get_trust (och) <= LEVEL_BUILDER)
				continue;

			if (channel == CHANNEL_STORY && (!IS_NPC (och) && !IS_STORYTELLER (och)))
				continue;
			if (channel == CHANNEL_YELL && vch->in_room->area != ch->in_room->area)
				continue;

			if (channel == CHANNEL_STORYINFORM && (!IS_NPC (och) && !IS_STORYTELLER (och)))
				continue;

			if (channel == CHANNEL_OSAY && vch->in_room->vnum != ch->in_room->vnum)
				continue;

			//part of thaum, if not on same plane, you can't hear each other unless you have spirit manip
			if (channel != CHANNEL_OOC)
			{
				if ((ch->plane != och->plane) && !IS_STORYTELLER(och))
					continue;
				if ((ch->plane != och->plane) && !IS_STORYTELLER(ch))
					continue;
			}

			position = vch->position;
			vch->position = POS_STANDING;

			if (!IS_NPC (vch) && IS_VAMPIRE (vch) && strlen (ch->side) > 1 && strlen (vch->side) > 1 && str_cmp (ch->side, vch->side))
			{
				act (buf2, ch, argument, vch, TO_VICT);
				vch->position = position;
				continue;
			}

			if (channel == CHANNEL_STORYINFORM)
			{
				act (buf2, ch, argument, vch, TO_VICT);
				continue;
			}


			if (channel == CHANNEL_OOC)
			{
				act (buf2, ch, argument, vch, TO_VICT);
				continue;
			}
			
			if (channel == CHANNEL_OMOTE)
			{
				act (buf2, ch, argument, vch, TO_VICT);
				continue;
			}
		
			if (channel == CHANNEL_OSAY)
			{
				act (buf2, ch, argument, vch, TO_VICT);
				continue;
			}

			act (buf, ch, argument, vch, TO_VICT);
			vch->position = position;
		}
	}

	return;
}

void do_story (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch) || !IS_STORYTELLER (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	talk_channel (ch, argument, CHANNEL_STORY, "story");
	return;
}

void do_storyinform (CHAR_DATA * ch, char *argument)
{
	talk_channel (ch, argument, CHANNEL_STORYINFORM, "storyinform");
	return;
}
	

void do_delete (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char newbuf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (ch->pcdata->confirm_delete)
	{
		if (arg[0] != '\0')
		{
			send_to_char ("Delete status removed.\n\r", ch);
			ch->pcdata->confirm_delete = FALSE;
			return;
		}
		else
		{
			sprintf (buf, "#C%s#c has suffered Final Death. We mourn for them.", ch->name);
			do_info (ch, buf);
			sprintf (buf, "%s%s", PLAYER_DIR, capitalize (ch->name));
			backup_char_obj (ch);
			do_quit (ch, "");
			unlink (buf);
			return;
		}
	}

	if (arg[0] != '\0' && !IS_IMP(ch))
	{
		send_to_char ("Just type delete.  No argument.\n\r", ch);
		return;
	}
	else if (IS_IMP(ch))
	{
		if (!check_parse_name (arg))  
	        {
        	        send_to_char ("Thats an illegal name.\n\r", ch);
                	return;
	        }

	        if (!char_exists (FILE_PLAYER, arg))
        	{
                	send_to_char ("That player doesn't exist.\n\r", ch);
	                return;
        	}
		sprintf (buf, "%s%s", PLAYER_DIR, capitalize (arg));
		sprintf (newbuf, "cp %s %s", buf, PLAYER_BAK_DIR );
		system (newbuf);
		unlink (buf);
		send_to_char("Player deleted.\n\r", ch);
		return;
	}
	send_to_char ("Type delete again to confirm this command.\n\r", ch);
	send_to_char ("#RWARNING:#n This command is irreversible.\n\r", ch);
	send_to_char ("Typing delete with an argument will undo delete status.\n\r", ch);
	ch->pcdata->confirm_delete = TRUE;
}


void do_ooc (CHAR_DATA * ch, char *argument)
{
	talk_channel (ch, argument, CHANNEL_OOC, "ooc");
	return;
}

void do_omote (CHAR_DATA * ch, char *argument)
{
        talk_channel (ch, argument, CHANNEL_OMOTE, "omote");
        return;
}


void do_osay (CHAR_DATA * ch, char *argument)
{
	talk_channel (ch, argument, CHANNEL_OSAY, "osay");
	return;
}



void do_immtalk (CHAR_DATA * ch, char *argument)
{
	talk_channel (ch, argument, CHANNEL_IMMTALK, "immtalk");
	return;
}



void do_say (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char name[80];
	char poly[MAX_STRING_LENGTH];
	char speak[15];
	char speaks[15];
	char endbit[5];
	char secbit[5];
	CHAR_DATA *to;
	CHAR_DATA *vch;
	MEMORY_DATA * temp_mem;
	bool is_ok;


	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("Your words make no sound!\n\r", ch);
		return;
	}

	if (strlen (argument) > MAX_INPUT_LENGTH-80)
	{
		send_to_char ("Line too long.\n\r", ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char ("Say what?\n\r", ch);
		return;
	}

	/* Allow the manipulation of a const argument */
	strcpy (buf, argument);
	argument = &buf[0];

	endbit[0] = argument[strlen (argument) - 1];
	endbit[1] = '\0';

	if (strlen (argument) > 1)
		secbit[0] = argument[strlen (argument) - 2];
	else
		secbit[0] = '\0';
	secbit[1] = '\0';

	if (!str_cmp (endbit, "!"))
	{
		sprintf (speak, "exclaim");
		sprintf (speaks, "exclaims");
	}
	else if (!str_cmp (endbit, "?"))
	{
		sprintf (speak, "ask");
		sprintf (speaks, "asks");
	}
	else if (secbit[0] != '\0' && str_cmp (secbit, ".") && !str_cmp (endbit, "."))
	{
		sprintf (speak, "state");
		sprintf (speaks, "states");
	}
	else if (secbit[0] != '\0' && !str_cmp (secbit, ".") && !str_cmp (endbit, "."))
	{
		sprintf (speak, "mutter");
		sprintf (speaks, "mutters");
	}
	else
	{
		sprintf (speak, "say");
		sprintf (speaks, "says");
	}
	sprintf (poly, "You %s '#y$T#n'", speak);

//	parse_rp (ch, argument);

	if (!IS_NPC (ch))
	{
		if (IS_SPEAKING (ch, LANG_GERMAN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, GERMAN);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_ARABIC))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, ARABIC);
			return;
		}
		
		else if (IS_SPEAKING (ch, LANG_FRENCH))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, FRENCH);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_RUSSIAN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, RUSSIAN);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_LATIN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, LATIN);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_SPANISH))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, SPANISH);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_ITALIAN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, ITALIAN);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_HEBREW))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, HEBREW);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_GAELIC))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, GAELIC);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_INDONESIAN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, INDONESIAN);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_ENGLISH))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, ENGLISH);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_GREEK))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, GREEK);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_SLAVIC))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, SLAVIC);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_GAROU))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ch->pcdata->wolfform[1] == FORM_HOMID)
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, GAROU);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_CHINESE))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, CHINESE);
			return;
		}
		if (IS_SPEAKING (ch, LANG_JAPANESE))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, JAPANESE);
			return;
		}
		if (IS_SPEAKING (ch, LANG_KOREAN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, KOREAN);
			return;
		}
		if (IS_SPEAKING (ch, LANG_VIETNAMESE))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, VIETNAMESE);
			return;
		}
		if (IS_SPEAKING (ch, LANG_SWAHILI))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, SWAHILI);
			return;
		}
		if (IS_SPEAKING (ch, LANG_TAGALOG))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, TAGALOG);
			return;
		}
		if (IS_SPEAKING (ch, LANG_HINDU))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_LUPUS || 
		ch->pcdata->wolfform[1] == FORM_HISPO ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, HINDU);
			return;
		}
		
		
		
		else if (IS_SPEAKING (ch, DIA_OLDE))
			act (poly, ch, NULL, argument, TO_CHAR);
		else
			act (poly, ch, NULL, argument, TO_CHAR);
	}
	else
		act (poly, ch, NULL, argument, TO_CHAR);

	sprintf (poly, "$n %s '#y$T#n'", speaks);

	if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
	{
		act (poly, ch, NULL, argument, TO_ROOM);
		if (ch->desc != NULL && ch->desc->snoop_by != NULL)
		{
			if ((vch = ch->desc->snoop_by->character) == NULL)
				ch->desc->snoop_by = NULL;
			else if (ch->in_room == NULL || vch->in_room == NULL)
				bug ("In_room bug - KaVir.", 0);
		}
		MOBtrigger = TRUE;
		mprog_speech_trigger (argument, ch);

		room_text (ch, strlower (argument));
		return;
	}

	to = ch->in_room->people;
	for (; to != NULL; to = to->next_in_room)
	{
		is_ok = FALSE;

		if (to->desc == NULL || !IS_AWAKE (to))
			continue;

		if (ch->plane != to->plane && !IS_STORYTELLER(to))
			continue;

		if (ch == to)
			continue;

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

		if (IS_NPC (ch))
			sprintf (name, ch->short_descr);
		 
		else if (IS_STORYTELLER(to))
				sprintf (name, "%s", ch->name);
		
		else if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		{
			temp_mem = memory_search_real_name(to->memory, ch->name);

			if (IS_STORYTELLER(to))
				sprintf (name, "%s", ch->name);
			
			else if (temp_mem == NULL)
				sprintf (name, "%s", ch->morph);
			else
				sprintf (name, "%s #e(%s)#n", ch->morph, temp_mem->remembered_name);
		}
		else
		{
			temp_mem = memory_search_real_name(to->memory, ch->name);

			if (IS_STORYTELLER(to))
				sprintf (name, "%s", ch->name);
			else if (temp_mem == NULL) 
				sprintf (name, "%s",  ch->pcdata->roomdesc);
			else
				sprintf (name, "%s", temp_mem->remembered_name);
		}
		name[0] = UPPER (name[0]);
		sprintf (poly, "%s %s '%s'.\n\r", name, speaks, argument);
		send_to_char (poly, to);
	}
	MOBtrigger = TRUE;
	mprog_speech_trigger (argument, ch);
	room_text (ch, strlower (argument));
	return;
}

void room_text (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	CHAR_DATA *mob;
	OBJ_DATA *obj;
	ROOMTEXT_DATA *rt;
	bool mobfound;
	bool hop;

	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	for (rt = ch->in_room->roomtext; rt != NULL; rt = rt->next)
	{
		if (!strcmp (argument, rt->input) || is_in (argument, rt->input) || all_in (argument, rt->input))
		{
			if (rt->name != NULL && rt->name != '\0' && str_cmp (rt->name, "all") && str_cmp (rt->name, "|all*"))
				if (!is_in (ch->name, rt->name))
					continue;
			mobfound = TRUE;
			if (rt->mob != 0)
			{
				mobfound = FALSE;
				for (vch = char_list; vch != NULL; vch = vch_next)
				{
					vch_next = vch->next;
					if (vch->in_room == NULL)
						continue;
					if (!IS_NPC (vch))
						continue;
					if (vch->in_room == ch->in_room && vch->pIndexData->vnum == rt->mob)
					{
						mobfound = TRUE;
						break;
					}
				}
			}
			if (!mobfound)
				continue;
			hop = FALSE;
			switch (rt->type % RT_RETURN)
			{
			case RT_SAY:
				break;
			case RT_LIGHTS:
				do_changelight (ch, "");
				break;
			case RT_LIGHT:
				REMOVE_BIT (ch->in_room->room_flags, ROOM_DARK);
				break;
			case RT_DARK:
				SET_BIT (ch->in_room->room_flags, ROOM_DARK);
				break;
			case RT_OBJECT:
				if (get_obj_index (rt->power) == NULL)
					return;
				obj = create_object (get_obj_index (rt->power), ch->level);
				if (IS_SET (rt->type, RT_TIMER))
					obj->timer = 60;
				if (ch->carry_weight + get_obj_weight (obj) > can_carry_w (ch) || ch->carry_number + 1 > can_carry_n (ch))
					extract_obj (obj);
				else
				{
					if (CAN_WEAR (obj, ITEM_TAKE))
						obj_to_char (obj, ch);
					else
						obj_to_room (obj, ch->in_room);
					if (!str_cmp (rt->choutput, "copy"))
						act (rt->output, ch, obj, NULL, TO_CHAR);
					else
						act (rt->choutput, ch, obj, NULL, TO_CHAR);
					if (!IS_SET (rt->type, RT_PERSONAL))
						act (rt->output, ch, obj, NULL, TO_ROOM);
				}
				hop = TRUE;
				break;
			case RT_MOBILE:
				if (get_mob_index (rt->power) == NULL)
					return;
				mob = create_mobile (get_mob_index (rt->power));
				char_to_room (mob, ch->in_room);
				if (!str_cmp (rt->choutput, "copy"))
					act (rt->output, ch, NULL, mob, TO_CHAR);
				else
					act (rt->choutput, ch, NULL, mob, TO_CHAR);
				if (!IS_SET (rt->type, RT_PERSONAL))
					act (rt->output, ch, NULL, mob, TO_ROOM);
				hop = TRUE;
				break;
			case RT_SPELL:
				(*skill_table[rt->power].spell_fun) (rt->power, number_range (20, 30), ch, ch);			
				break;
			case RT_PORTAL:
				if (get_obj_index (OBJ_VNUM_PORTAL) == NULL)
					return;
				obj = create_object (get_obj_index (OBJ_VNUM_PORTAL), 0);
				obj->timer = 5;
				obj->value[0] = rt->power;
				obj->value[1] = 1;
				obj_to_room (obj, ch->in_room);
				break;
			case RT_TELEPORT:
				if (get_room_index (rt->power) == NULL)
					return;
				if (!str_cmp (rt->choutput, "copy"))
					act (rt->output, ch, NULL, NULL, TO_CHAR);
				else
					act (rt->choutput, ch, NULL, NULL, TO_CHAR);
				if (!IS_SET (rt->type, RT_PERSONAL))
					act (rt->output, ch, NULL, NULL, TO_ROOM);
				char_from_room (ch);
				char_to_room (ch, get_room_index (rt->power));
				act ("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
				do_look (ch, "auto");
				hop = TRUE;
				break;
			case RT_ACTION:
				sprintf (arg, argument);
				argument = one_argument (arg, arg1);
				argument = one_argument (arg, arg2);
				if ((mob = get_char_room (ch, arg2)) == NULL)
					continue;
				if (IS_NPC (mob))
					SET_BIT (mob->act, ACT_COMMANDED);
				interpret (mob, rt->output);
				if (IS_NPC (mob))
					REMOVE_BIT (mob->act, ACT_COMMANDED);
				break;
			case RT_OPEN_LIFT:
				open_lift (ch);
				break;
			case RT_CLOSE_LIFT:
				close_lift (ch);
				break;
			case RT_MOVE_LIFT:
				move_lift (ch, rt->power);
				break;
			default:
				break;
			}
			if (hop && IS_SET (rt->type, RT_RETURN))
				return;
			else if (hop)
				continue;
			if (!str_cmp (rt->choutput, "copy") && !IS_SET (rt->type, RT_ACTION))
				act (rt->output, ch, NULL, NULL, TO_CHAR);
			else if (!IS_SET (rt->type, RT_ACTION))
				act (rt->choutput, ch, NULL, NULL, TO_CHAR);
			if (!IS_SET (rt->type, RT_PERSONAL) && !IS_SET (rt->type, RT_ACTION))
				act (rt->output, ch, NULL, NULL, TO_ROOM);
			if (IS_SET (rt->type, RT_RETURN))
				return;
		}
	}
	return;
}

char *strlower (char *ip)
{
	static char buffer[MAX_STRING_LENGTH];
	int pos;

	for (pos = 0; pos < (MAX_STRING_LENGTH - 1) && ip[pos] != '\0'; pos++)
	{
		buffer[pos] = tolower (ip[pos]);
	}
	buffer[pos] = '\0';
	return buffer;
}

bool is_in (char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_STRING_LENGTH];
	int fitted;

	if (ip[0] != '|')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower (arg);
	do
	{
		ip += strlen (cmp) + 1;
		fitted = sscanf (ip, "%[^*]", cmp);
		if (strstr (lo_arg, cmp) != NULL)
		{
			return TRUE;
		}
	}
	while (fitted > 0);
	return FALSE;
}

bool all_in (char *arg, char *ip)
{
	char *lo_arg;
	char cmp[MAX_STRING_LENGTH];
	int fitted;

	if (ip[0] != '&')
		return FALSE;
	cmp[0] = '\0';
	lo_arg = strlower (arg);
	do
	{
		ip += strlen (cmp) + 1;
		fitted = sscanf (ip, "%[^*]", cmp);
		if (strstr (lo_arg, cmp) == NULL)
		{
			return FALSE;
		}
	}
	while (fitted > 0);
	return TRUE;
}


/*
 * Cell phone commands
 */
 #define CELL_OFF(ch) (IS_SET(ch->deaf, CHANNEL_CELL))
 #define CELL_ON(ch)  !CELLOFF(ch)

void spam_cellphone_syntax(CHAR_DATA *ch)
{
	send_to_char("Cellphone commands:\n\r", ch);
	send_to_char("    call <player>\n\r", ch);
	send_to_char("    answer\n\r", ch);
	send_to_char("    hangup\n\r", ch);
	send_to_char("    cell <message>\n\r", ch);
	send_to_char("    cell on/off/vibrate\n\r", ch);
	send_to_char("    ringtone <message>\n\r", ch);
}

void do_call(register CHAR_DATA * ch, register char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	register CHAR_DATA *victim;
	argument = one_argument(argument, arg);

	if (arg[0] == '\0') 
	{
		spam_cellphone_syntax(ch);
		return;
	}
	if (IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char ("You are silenced.\n\r", ch);
		return;
	}
	if (CELL_OFF(ch))    
		do_cell(ch,"on");
	if (ch->onPhone != NULL)
	{
		send_to_char("#GYou can't call someone in the middle of another call.#n\n\r. Use 'hangup' first\n\r", ch);
		return;
	}
	send_to_char("#GCalling...#n\n\r", ch);
	if (   ((victim = get_char_world(ch, arg)) == NULL) 
		|| (!IS_NPC(victim) && victim->desc == NULL)
		|| (CELL_OFF(victim))
		|| (victim == ch)
	   )
	{
		send_to_char("#GYour call didn't connect.#n\n\r", ch);
		return;
	}
	if (victim->onPhone != NULL)
	{
		send_to_char("#GBEEP. BEEP. BEEP. The person you are trying to call has a busy signal.#n\n\r", ch);
		return;
	}
	if (victim->phonevibrate)
		send_to_char("#GYour phone vibrates...#n\n\r", victim);
	else
	{
		sprintf(buf, "#G%s#n", victim->ringtone);
		act(buf, victim, NULL, NULL, TO_ROOM);
		strcat(buf, "\n\r");
		send_to_char(buf, victim);
	}

	ch->onPhone        = victim;
	victim->onPhone    = ch;
	ch->phonestate     = -1;  //-1 = caller and unanswered
	victim->phonestate = 0;   // 0 = callee and unanswered
}

void do_answer(register CHAR_DATA * ch, register char *argument)
{
	if (argument[0] != '\0')
	{
		spam_cellphone_syntax(ch);
		return;
	}
	if (CELL_OFF(ch))
	{
		send_to_char("#GYour phone is off.#n\n\r", ch);
		return;
	}
	if (ch->onPhone == NULL)
	{
		send_to_char("#GNo one is calling you.#n\n\r", ch);
		return;
	}
	if (ch->onPhone->onPhone == NULL || ch->onPhone->desc == NULL)
	{
		send_to_char("#GYou answer, but the call loses signal.#n\n\r", ch);
		ch->onPhone->onPhone = NULL;
		ch->onPhone = NULL;
		return;
	}
	if (ch->phonestate == -1)
	{
		send_to_char("#GYou can't answer your own phone call.#n\n\r", ch);
		return;
	}
	if (ch->phonestate == 1)
	{
		send_to_char("#GYou're already on the phone.#n\n\r", ch);
		return;
	}
	ch->onPhone->phonestate = 1;
	ch->phonestate          = 1;
	send_to_char("#GYour call is connected.#n\n\r", ch);
	send_to_char("#y...the call connects.#n\n\r", ch->onPhone);
}

void do_hangup(register CHAR_DATA * ch, register char *argument)
{
	if (argument[0] != '\0')
	{
		spam_cellphone_syntax(ch);
		return;
	}
	if (CELL_OFF(ch))
	{
		send_to_char("#GYour phone is off.#n\n\r", ch);
		return;
	}
	if (ch->onPhone == NULL)
	{
		send_to_char("#GNo one is calling you.#n\n\r", ch);
		return;
	}
	send_to_char("#GYou hang up your phone.#n\n\r", ch);
	send_to_char("#GThe person on the other end of the phone hangs up.\n\r", ch->onPhone);
	ch->onPhone->onPhone = NULL;
	ch->onPhone          = NULL;
}

void do_cell(register CHAR_DATA * ch, register char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char saybuf[MAX_STRING_LENGTH];

	if (IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char ("You are silenced.\n\r", ch);
		return;
	}
	if (!str_cmp(argument, "on"))
	{
		send_to_char("#GYour phone is now on.#n\n\r", ch);
		REMOVE_BIT(ch->deaf, CHANNEL_CELL);
		ch->phonevibrate = FALSE;
		return;
	}
	if (!str_cmp(argument, "off"))
	{
		if (ch->onPhone != NULL)  do_hangup(ch, "");
		send_to_char("#GYour phone is now off.#n\n\r", ch);
		SET_BIT(ch->deaf, CHANNEL_CELL);
		ch->phonevibrate = FALSE;
		return;
	}
	if (!str_cmp(argument, "vibrate"))
	{
		if (CELL_OFF(ch)) do_cell(ch, "on");
		send_to_char("#GYour phone will now vibrate when called.#n\n\r", ch);
		ch->phonevibrate = TRUE;
		return;
	}
	if (argument[0] == '\0')
	{
		spam_cellphone_syntax(ch);
		return;
	}
	if (ch->onPhone == NULL)
	{
		send_to_char("#GYou are not talking to anyone.#n\n\r", ch);
		return;
	}
	if (ch->onPhone->onPhone == NULL || ch->onPhone->desc == NULL)
	{
		send_to_char("#GYou have lost signal.#n\n\r", ch);
		ch->onPhone->onPhone = NULL;
		ch->onPhone = NULL;
		return;
	}
	if (ch->phonestate == -1)
	{
		send_to_char("#GThey haven't answered yet...#n", ch);
		return;
	}
	if (ch->phonestate == 0)
		do_answer(ch, "");
	sprintf(saybuf, "#C%s", argument);
	sprintf(buf, "#g(#GIncomingCall#g) %s says, #g\"#G%s#g\"#n\n\r", ch->pcdata->voice, argument);
	do_say(ch, saybuf);
	send_to_char(buf, ch->onPhone);
}
void do_ringtone(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	if (argument[0] == '\0')
	{
		sprintf(buf, "#gYour ringtone is currently: #G%s#n\n\r", ch->ringtone);
		send_to_char(buf, ch);
		return;
	}
	ch->ringtone = str_dup(argument);
	sprintf(buf, "#gYour ringtone is now: #G%s#n\n\r", ch->ringtone);
	send_to_char(buf, ch);
} 
/* end cell phone code*/

void do_tell (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("#GYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#GYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (IS_EXTRA (ch, GAGGED))
	{
		send_to_char ("#GYour message didn't get through.#n\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("#GTell whom what?#n\n\r", ch);
		return;
	}

	/*
	 * Can tell to PC's anywhere, but NPC's only in same room.
	 * -- Furey
	 */
	if ((victim = get_char_world (ch, arg)) == NULL || (IS_NPC (victim) && victim->in_room != ch->in_room))
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->extra2, EXTRA2_DND) && !IS_IMMORTAL (ch))
	{
		send_to_char ("You have messaged a DND Imp.. Bad move..\n\r", ch);
		send_to_char ("The message is ignored.\n\r", ch);
		do_info (victim, poly);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
			return;
		}
	}
	if (can_see (ch, victim))
	{
		 if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH) && IS_STORYTELLER(ch) && str_cmp (strlower (victim->name), arg))
			sprintf (name, victim->morph);
		else 
			sprintf (name, victim->name);
	}
	else
		sprintf (name, "Someone");

	sprintf (poly, "You tell %s '#C$t#n'", name);
	act (poly, ch, argument, victim, TO_CHAR);

	position = victim->position;
	victim->position = POS_STANDING;

	if (can_see (victim, ch))
	{
		 if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH) && IS_STORYTELLER(ch))
				sprintf (name, ch->morph); 
		else 
			sprintf (name, ch->name);
	}
	else
		sprintf (name, "Someone");

	sprintf (poly, "%s tells you '#C$t#n'", name);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->reply = ch;

	return;
}

/*
void do_tell (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	MEMORY_DATA * char_mem;
	MEMORY_DATA * victim_mem;
	int position;
	bool found_vic = 0;

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}
	if (IS_EXTRA (ch, GAGGED))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Tell whom what?\n\r", ch);
		return;
	}
*/
	/*
	 * Can tell to PC's anywhere, but NPC's only in same room.
	 * -- Furey
	 */
	/*if (IS_IMMORTAL(ch))
	{
		if ((victim = get_char_world(ch, arg)) != NULL)
			found_vic = 1;
	}
	else if ((victim = get_char_world_alias(ch, arg)) != NULL)
		found_vic = 1;
	else if ((victim = get_char_world_desc(ch, arg)) != NULL)
		found_vic = 1;
	if (!found_vic)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->extra2, EXTRA2_DND) && !IS_IMMORTAL (ch))
	{
		send_to_char ("You have messaged a DND Imp.. Bad move..\n\r", ch);
		send_to_char ("The message is ignored.\n\r", ch);
		do_info (victim, poly);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
			return;
		}
	}
	if (can_see (ch, victim))
	{
		if (IS_IMMORTAL(ch))
		{
			sprintf(name, victim->name);
		}
		else if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH) && IS_STORYTELLER(ch) && str_cmp (strlower (victim->name), arg))
			sprintf (name, victim->morph);
		else 
		{
			char_mem = memory_search_real_name(ch->memory, victim->name);
			if (char_mem == NULL)
				sprintf (name, victim->pcdata->roomdesc);
			else
				sprintf (name, char_mem->remembered_name);
		}
	}
	else
		sprintf (name, "Someone");

	sprintf (poly, "You tell %s '#C$t#n'", name);
	act (poly, ch, argument, victim, TO_CHAR);

	position = victim->position;
	victim->position = POS_STANDING;

	if (can_see (victim, ch))
	{
		if (IS_IMMORTAL(victim))
		{
			sprintf(name, ch->name);
		}
		else if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH) && IS_STORYTELLER(ch))
				sprintf (name, ch->morph); 
		else 
		{
			victim_mem = memory_search_real_name(victim->memory, ch->name);
			if (victim_mem == NULL)
				sprintf(name, ch->pcdata->roomdesc);
			else
				sprintf(name, victim_mem->remembered_name);
		}
	}
	else
		sprintf (name, "Someone");

	sprintf (poly, "%s tells you '#C$t#n'", name);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->reply = ch;

	return;
}

*/

void do_whisper (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	char name2[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *to;
	MEMORY_DATA * victim_mem;
	MEMORY_DATA * ch_mem;
	int listen_success = 0;
  int listendiff = 9;
 	char whisperedword[MAX_INPUT_LENGTH];
  int language2;
	bool found_victim = 0;
  
	argument = one_argument (argument, arg);

	
	
	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Syntax: whisper <person> <message>\n\r", ch);
		return;
	}

	if ((victim = get_char_room_from_alias(ch, arg)) != NULL)
		found_victim = 1;
	else if ((victim = get_char_room_from_desc(ch, arg)) != NULL)
		found_victim = 1;

//	if ((victim = get_char_world (ch, arg)) == NULL || (victim->in_room != ch->in_room))
	if (!found_victim)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!IS_AWAKE (victim))
	{
		act ("$E cannot hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}
	/* Start Code thats never going to work*/
	if (ch->pcdata->language[0] == LANG_ENGLISH){
		strcpy(whisperedword, "english");
	  language2 = gsn_english;
	}
	else if (ch->pcdata->language[0] == LANG_ARABIC){
		strcpy(whisperedword, "arabic");
		language2 = gsn_arabic;
	}
	else if (ch->pcdata->language[0] == LANG_CHINESE){
		strcpy(whisperedword, "chinese");
		language2 = gsn_chinese;
	}
	else if (ch->pcdata->language[0] == LANG_FRENCH){
		strcpy(whisperedword, "french");
		language2 = gsn_french;
	}
	else if (ch->pcdata->language[0] == LANG_ITALIAN){
		strcpy(whisperedword, "italian");
		language2 = gsn_italian;
	}
	else if (ch->pcdata->language[0] == LANG_SPANISH){
		strcpy(whisperedword, "spanish");
		language2 = gsn_spanish;
	}
	else if (ch->pcdata->language[0] == LANG_LATIN){
		strcpy(whisperedword, "latin");
		language2 = gsn_latin;
	}
	else if (ch->pcdata->language[0] == LANG_HEBREW){
		strcpy(whisperedword, "hebrew");
		language2 = gsn_hebrew;
	}
	else if (ch->pcdata->language[0] == LANG_GAELIC){
		strcpy(whisperedword, "gaelic");
		language2 = gsn_gaelic;
	}
	else if (ch->pcdata->language[0] == LANG_JAPANESE){
		strcpy(whisperedword, "japanese");
		language2 = gsn_japanese;
	}
	else if (ch->pcdata->language[0] == LANG_GREEK){
		strcpy(whisperedword, "greek");
		language2 = gsn_greek;
	}
	else if (ch->pcdata->language[0] == LANG_SLAVIC){
		strcpy(whisperedword, "slavic");
		language2 = gsn_slavic;
	}
	else if (ch->pcdata->language[0] == LANG_GAROU){
		strcpy(whisperedword, "garou");
		language2 = gsn_garou;
	}
	else if (ch->pcdata->language[0] == LANG_GERMAN){
		strcpy(whisperedword, "german");
		language2 = gsn_german;
	}
	else if (ch->pcdata->language[0] == LANG_RUSSIAN){
		strcpy(whisperedword, "russian");
		language2 = gsn_russian;
	}
	else if (ch->pcdata->language[0] == LANG_INDONESIAN){
		strcpy(whisperedword, "indonesian");
		language2 = gsn_indonesian;
	}
	else if (ch->pcdata->language[0] == LANG_KOREAN){
		strcpy(whisperedword, "korean");
		language2 = gsn_korean;
	}
	else if (ch->pcdata->language[0] == LANG_VIETNAMESE){
		strcpy(whisperedword, "vietnamese");
		language2 = gsn_vietnamese;
	}
	else if (ch->pcdata->language[0] == LANG_SWAHILI){
		strcpy(whisperedword, "swahili");
		language2 = gsn_swahili;
	}
	else if (ch->pcdata->language[0] == LANG_TAGALOG){
		strcpy(whisperedword, "tagalog");
		language2 = gsn_tagalog;
	}
	else if (ch->pcdata->language[0] == LANG_HINDU){
		strcpy(whisperedword, "hindu");
		language2 = gsn_hindu;
	}
	else{
		strcpy(whisperedword, "english");
	  language2 = gsn_english;
	}
	
		
	
	
	
	
	if (can_see (victim, ch))
	{
		if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
			sprintf (name, ch->morph);
		else
		{
			victim_mem = memory_search_real_name(victim->memory, ch->name);
			if (victim_mem == NULL)
				sprintf(name, ch->pcdata->roomdesc);
			else
				sprintf(name, victim_mem->remembered_name);
//			sprintf (name, ch->name);
		}
	}
	else
		sprintf (name, "Someone");

	if (can_see (ch, victim))
	{
		if (!IS_NPC (victim) && IS_AFFECTED (victim, AFF_POLYMORPH))
			sprintf (name2, victim->morph);
		else
		{
			ch_mem = memory_search_real_name(ch->memory, victim->name);
			if (ch_mem == NULL)
				sprintf(name2, victim->pcdata->roomdesc);
			else
				sprintf(name2, ch_mem->remembered_name);
//			sprintf (name2, victim->name);
		}
	}
	else
		sprintf (name, "Someone");

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	
	sprintf (buf, "#cYou whisper #B'#e%s#B' #cto #C%s#c in #C%s#c. \n\r",argument, name2, whisperedword);
		send_to_char (buf, ch);
	if (victim->pcdata->learned[language2] < 75){
	sprintf (buf, "#C%s #cwhispers to you in a language that you don't understand.\n\r", name);
		send_to_char (buf, victim);
	}
	else{
	sprintf (buf, "#C%s#c whispers #B'#e%s#B'#c to you in #C%s#c.\n\r",name ,argument,whisperedword );
		send_to_char (buf, victim);
	}


// Reset the name and name2 values to the roomdescs because I don't wanna rewrite the new whisper code
	sprintf(name, ch->pcdata->roomdesc);
	sprintf(name2, victim->pcdata->roomdesc);

/* New Whisper Checks*/
to = ch->in_room->people;
for (; to != NULL; to = to->next_in_room)
		{
			if (IS_NPC (victim))
					continue;
			else if (!strcmp (ch->name, to->name))
					continue;
	   	else if (!strcmp (victim->name, to->name))
					continue;
					
					else{
			 
			if (IS_MORE2 (to, MORE2_HEIGHTENSENSES))
			listendiff -= to->pcdata->powers[DISC_AUSPEX];
			 		
			listen_success = diceroll (to, ATTRIB_PER, TALENTS, TAL_ALERTNESS, listendiff);
			
			if (listen_success > 3 || IS_STORYTELLER(to)){
				if (to->pcdata->learned[language2] < 75){
				sprintf(buf, "#cYou overhear #C%s #cwhisper to #C%s#c but you don't understand the language.#n\n\r", name, name2);
				send_to_char (buf, to);	
			}
			else{
				sprintf(buf, "#cYou overhear #C%s #cwhisper, #B'#e%s#B'#c to #C%s #cin #C%s#c.#n\n\r", name, argument, name2, whisperedword);
				send_to_char (buf, to);		
				}
			}
			else
			{
				sprintf( buf, "#C%s#c whispers something to #C%s.#n", name, name2 );
				send_to_char (buf, to);		
			}

			}
		}
		return;
}		
			
		

void do_reply (CHAR_DATA * ch, char *argument)
{
	char poly[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;
	char name[MAX_STRING_LENGTH];
	char cname[MAX_STRING_LENGTH];

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("Your message didn't get through.\n\r", ch);
		return;
	}
	if ((victim = ch->reply) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("$E is currently link dead.", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("$E can't hear you.", ch, 0, victim, TO_CHAR);
			return;
		}
	}
	if (can_see (ch, victim))
	{
		if (IS_NPC(victim))
			sprintf (name, victim->short_descr);
		else if ((IS_AFFECTED (victim, AFF_POLYMORPH)) && IS_STORYTELLER(ch))
			sprintf (name, victim->morph);
		else
			sprintf (name, victim->name);
	}
	else
		sprintf (name, "Someone");

	if (can_see (victim, ch))
	{
		if (IS_NPC(ch))
			sprintf (cname, ch->short_descr);
		/*else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && IS_STORYTELLER(ch))
			sprintf (cname, ch->morph); */
		else
			sprintf (cname, ch->name);
	}
	else
		sprintf (cname, "Someone");


	sprintf (poly, "You reply to %s '#C$t#n'", name);
	act (poly, ch, argument, victim, TO_CHAR);

	position = victim->position;
	victim->position = POS_STANDING;

	sprintf (poly, "%s replies '#C$t#n'", cname);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->reply = ch;

	return;
}
void do_emote (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char *plast;

	char name[80];
	char poly[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	MEMORY_DATA * temp_mem;
	bool is_ok;


	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_NO_EMOTE))
	{
		send_to_char ("You can't show your emotions.\n\r", ch);
		return;
	}
	if (argument[0] == '\0')
	{
		send_to_char ("#cPose what?#n\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
	if (strchr(argument, '"') != NULL)
		argument = colorPose(argument);

// Ensure that char *src is not null, allocate space for char *dest.
/*
int inquotes=0;

while(*dest++=*src++)
  if (inquotes)
    {if (*src=='\'' || *src=='') *dest++='#', *dest++='n', inquotes=0;}
  else
    {if (*dest=='\'' || *dest=='') *dest++='#', *dest++='y', inquotes=1;}

*/
/*	if ((countArgument(argument, '"') != 0) && ((countArgument(argument, '"') % 2) == 0))
	{
		//Maybe i could use strtok here and use " as delimeter and build the string after each token
		//Change it so the check for a lack of %2==0 will simply tag on a " at the end.
		int i = 0;
		int oldCharPos;
		char *tempString;
		char *finalString;
	
		tempString = argument;
	
		int charPos = indexOf(tempString, '"');
		finalString = substr(tempString, 0, charPos);

		while (charPos != -1)
		{
			tempString = substr(tempString, charPos, strlen(tempString));
			oldCharPos = charPos + 1;
			charPos = indexOf(tempString, '"');
			strcat(finalString, substr(tempString, oldCharPos, charPos));
			if (i % 2 == 0)
				strcat(finalString, "#y");
			else
				strcat(finalString, "#n");
			i++;
		}
		argument = finalString;
	}
*/
	for (plast = argument; *plast != '\0'; plast++)
		;

	strcpy (buf, argument);
	if (isalpha (plast[-1]))
		strcat (buf, ".");

	act ("$n $T", ch, NULL, buf, TO_CHAR);

//	parse_rp (ch, argument);

	if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
	{
		act ("$n $T", ch, NULL, buf, TO_ROOM);
		return;
	}

	to = ch->in_room->people;
	for (; to != NULL; to = to->next_in_room)
	{
		is_ok = FALSE;

		if (to->desc == NULL || !IS_AWAKE (to))
			continue;

		if (ch == to)
			continue;

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

		if (IS_NPC (ch))
			sprintf (name, ch->short_descr);
		else if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
			sprintf (name, ch->morph);
		else
		{
			temp_mem = memory_search_real_name(to->memory, ch->name);
			if (temp_mem == NULL)
				sprintf(name, ch->pcdata->roomdesc);
			else
				sprintf(name, temp_mem->remembered_name);
		}
		name[0] = UPPER (name[0]);
		sprintf (poly, "#C%s %s#n\n\r", name, buf);
		send_to_char (poly, to);
	}
	ch->last_pose = current_time;
	return;
}



void do_xemote (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char oldarg[MAX_STRING_LENGTH];
	char *plast;

	char name[80];
	char you[80];
	char them[80];
	char poly[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *to;
	CHAR_DATA *victim;
	bool is_ok;

	argument = one_argument (argument, arg);

	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_NO_EMOTE))
	{
		send_to_char ("You can't show your emotions.\n\r", ch);
		return;
	}

	if (strlen (argument) > MAX_INPUT_LENGTH)
	{
		send_to_char ("Line too long.\n\r", ch);
		return;
	}

	if (argument[0] == '\0' || arg[0] == '\0')
	{
		send_to_char ("Syntax: emote <person> <sentence>\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
	
	if (IS_NPC (ch))
		strcpy (you, ch->short_descr);
	else
		strcpy (you, ch->name);
	if (IS_NPC (victim))
		strcpy (you, victim->short_descr);
	else
		strcpy (you, victim->name);

	strcpy (oldarg, argument);
	strcpy (buf, argument);
	for (plast = argument; *plast != '\0'; plast++)
		;

	if (isalpha (plast[-1]))
		strcat (buf, ".");
	argument = socialc (ch, buf, you, them);

	strcpy (buf, argument);
	strcpy (buf2, "You ");
	buf[0] = LOWER (buf[0]);
	strcat (buf2, buf);
	capitalize (buf2);
	act (buf2, ch, NULL, victim, TO_CHAR);

	if (ch->in_room->vnum != ROOM_VNUM_IN_OBJECT)
	{
		strcpy (buf, oldarg);
		for (plast = argument; *plast != '\0'; plast++)
			;
		if (isalpha (plast[-1]))
			strcat (buf, ".");

		argument = socialn (ch, buf, you, them);

		strcpy (buf, argument);
		strcpy (buf2, "$n ");
		buf[0] = LOWER (buf[0]);
		strcat (buf2, buf);
		capitalize (buf2);
		act (buf2, ch, NULL, victim, TO_NOTVICT);

		strcpy (buf, oldarg);
		for (plast = argument; *plast != '\0'; plast++)
			;
		if (isalpha (plast[-1]))
			strcat (buf, ".");

		argument = socialv (ch, buf, you, them);

		strcpy (buf, argument);
		strcpy (buf2, "$n ");
		buf[0] = LOWER (buf[0]);
		strcat (buf2, buf);
		capitalize (buf2);
		act (buf2, ch, NULL, victim, TO_VICT);
		return;
	}

	to = ch->in_room->people;
	for (; to != NULL; to = to->next_in_room)
	{
		is_ok = FALSE;

		if (to->desc == NULL || !IS_AWAKE (to))
			continue;

		if (ch == to)
			continue;

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

		if (IS_NPC (ch))
			sprintf (name, ch->short_descr);
		else if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
			sprintf (name, ch->morph);
		else
			sprintf (name, ch->name);
		name[0] = UPPER (name[0]);
		sprintf (poly, "#C%s %s#n\n\r", name, buf);
		send_to_char (poly, to);
	}
	return;
}


void do_qui (CHAR_DATA * ch, char *argument)
{
	send_to_char ("If you want to QUIT, you have to spell it out.\n\r", ch);
	return;
}



void do_quit (CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;
	char buf[MAX_STRING_LENGTH];
	int char_success = 0;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
//	CHAR_DATA *mount;

	smash_tilde (argument);
	if (IS_NPC (ch))
		return;

	if (IS_SET (ch->act, ACT_NOQUIT) )
	{
		send_to_char( "You can't quit!\n\r",ch );
		return;
	}

	if (IS_MORE2(ch, MORE2_RPFLAG))
		do_rpflag(ch,argument);
	if (IS_MORE2(ch, MORE2_RPVIS))
		do_rpvis(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);

	if (IS_NPC (ch))
	{
		if (ch->wizard != NULL)
		{
			if (IS_AFFECTED (ch, AFF_ETHEREAL))
				return;
		}

		act ("$n has left the game.", ch, NULL, NULL, TO_ROOM);
		//if (strlen(ch->pcdata->roomdesc) > 1)
			sprintf (buf, "#G%s#e has left #gM#Giami#e by#w N#eight.#n", ch->pcdata->roomdesc);
		//else
		//	sprintf (buf, "#GSome new person#e has left #gM#Giami#e by#w N#eight.#n");
		buf[0] = UPPER (buf[0]);
		extract_char (ch, TRUE);
		return;
	}

	if (ch->position == POS_FIGHTING)
	{
		send_to_char ("No way! You are fighting.\n\r", ch);
		return;
	}

	if (ch->position < POS_SLEEPING)
	{
		send_to_char ("You're not DEAD yet.\n\r", ch);
		return;
	}
	if (!IS_NPC (ch) && IS_MORE (ch, MORE_DARKNESS))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
			REMOVE_BIT (ch->more, MORE_DARKNESS);
			ch->pcdata->darkness = 60;
			send_to_char ("The darkness in the room gradually fades away to nothing.\n\r", ch);
			act ("The darkness in the room gradually fades away to nothing.", ch, NULL, NULL, TO_ROOM);
		}
	}
	if (!IS_NPC (ch) && IS_MORE (ch, MORE_SILENCE))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->in_room->added_flags, ROOM2_SILENCE);
			REMOVE_BIT (ch->more, MORE_SILENCE);
			ch->pcdata->silence = 60;
			send_to_char ("The room is no longer silent.\n\r", ch);
			act ("The room is no longer silent.", ch, NULL, NULL, TO_ROOM);
		}
	}
 	/*New Logoff Message Code*/
 	
 if (IS_INQUISITOR(ch)){
	char_success = diceroll(ch, ATTRIB_WIT, KNOWLEDGES, KNO_INVESTIGATION, 6);
	
	if (char_success < 1){
		send_to_char("#cBut even if you should suffer for what is right, you are blessed. Do not\n\r", ch);
		send_to_char("#cfear what they fear; do not be frightened. Take courage.\n\r\n\r", ch);
		send_to_char("#C      --Peter 3:14#n", ch);
		}
	else if ((char_success < 3) && (char_success > 1)){
		send_to_char("#cThe name of the Lord is a strong tower; the righteous run to it and are safe.#C'#n\n\r", ch);
		send_to_char("#C      --Proverbs 18:10#n", ch);
		}
	else if ((char_success > 3) && (char_success < 5)){
		send_to_char("#cI will fear no evil: for the Lord art with me; thy rod and thy staff they\n\r", ch);
	  send_to_char("#ccomfort me. Thou preparest a table before me in the presence of mine enemies:\n\r", ch);
		send_to_char("#cthou anointest my head with oil; my cup runneth over. Surely goodness and\n\r", ch);
		send_to_char("#cmercy shall follow me all the days of my life: and I will dwell in the house #n\n\r\n\r", ch);
		send_to_char("#cof the Lord for ever.#n\n\r\n\r", ch);
		send_to_char("#C      --Psalm 23#n", ch);
		}
	else{
		send_to_char("#cFor God so loved the world, he gave his only begotten son that whosoever\n\r", ch);
		send_to_char("#cbelieveth in him should not perish, but have everlasting life.#n\n\r\n\r", ch);
		send_to_char("#C      ----John 3:16#n", ch);
		}
	}
	
if ((IS_GHOUL(ch)) || (!strcmp(ch->clan, "Ghoul"))){
	char_success = diceroll(ch, ATTRIB_MAN, TALENTS, TAL_SUBTERFUGE, 6);
	
	if (char_success <= 2){
		send_to_char("#cIt seems so easy.  Just a sip, a taste, and you're better than all the\n\r", ch);
		send_to_char("#crest.  You can knock your enemies through walls with a shove.  You can suck\n\r", ch);
		send_to_char("#cup a sword wound to the gut.  You can have all the night has to offer\n\r", ch);
		send_to_char("#cwithout sacraficing your body and soul to it.  The deal is too good to be\n\r", ch);
		send_to_char("#ctrue.  Yeah.  That is what Faust said.  And now the Damned want their due.#n\n\r", ch);
		}
	else{
		send_to_char("#cFarewell happy fields, where joy for ever dwells; hail horrors, hail\n\r", ch);
		send_to_char("#cinternal world, and thou profoundest hell, receive thy new possessor; one\n\r", ch);
		send_to_char("#cwho brings a mind not to be changed by place or time.  The mind is its own\n\r", ch);
		send_to_char("#cplace, and in itself can make a heaven of hell, a hell of heaven.\n\r", ch);
		send_to_char("#cJustice is the constant and perpetual will to allot to every man his due. \n\r\n\r", ch);
		send_to_char("#R      --Domitus Ulpain (100 - 228 AD)#n\n\r", ch);
		}
	}

if ((IS_VAMPIRE(ch)) && (strcmp(ch->clan, "Ghoul"))){
	char_success = diceroll(ch, ATTRIB_MAN, TALENTS, TAL_SUBTERFUGE, 6);
	
	if (char_success < 2){
		send_to_char("#cBut you must not eat meat that has its lifeblood still in it. And for your\n\r", ch);
		send_to_char("#clifeblood I will surely demand an accounting. I will demand an accounting\n\r", ch);
		send_to_char("#cfrom every animal. And from each man, too, I will demand an accounting for\n\r", ch);
		send_to_char("#cthe life of his fellow man.\n\r\n\r", ch);
		send_to_char("#P      --Genesis 9:4#n\n\r", ch);
		}
	else if ((char_success > 2) && (char_success < 4)){
		send_to_char("#cBut these speak evil of those things which they know not; but what they\n\r", ch);
		send_to_char("#ccome to know naturally as brute beasts, in those things they corrupt\n\r", ch);
		send_to_char("#cthemselves.\n\r", ch);
		send_to_char("#c  Woe unto them! For they have gone in the way of Cain and have run\n\r", ch);
		send_to_char("#cgreedily after the error of Balaam for their reward, and perished in the\n\r", ch);
		send_to_char("#cgainsaying of Korah.\n\r", ch);
		send_to_char("#c  These are spots on your feasts of charity when they feast with you,\n\r", ch);
		send_to_char("#ceeding themselves without fear. Clouds they are without water, carried\n\r", ch);
		send_to_char("#cabout by winds; trees whose fruit withereth, without fruit, twice dead,\n\r", ch);
		send_to_char("#cplucked up by the roots.\n\r\n\r", ch);
		send_to_char("#P      --Jude 1:10-12#n", ch);
		}
	else{
		send_to_char("#cAnd He said, 'What hast thou done? The voice of thy brother's blood\n\r", ch);
		send_to_char("#ccrieth unto Me from the ground.\n\r", ch);
		send_to_char("#c   And now art thou cursed from the earth, which hath opened her mouth to\n\r", ch);
		send_to_char("#creceive thy brother's blood from thy hand.\n\r", ch);
		send_to_char("#c   When thou tillest the ground, it shall not henceforth yield unto thee\n\r", ch);
		send_to_char("#cher strength. A fugitive and a vagabond shalt thou be on the earth.'\n\r", ch);
		send_to_char("#c   And Cain said unto the LORD, 'My punishment is greater than I can bear.\n\r", ch);
		send_to_char("#cBehold, Thou hast driven me out this day from the face of the earth, and\n\r", ch);
		send_to_char("#cfrom Thy face shall I be hid; and I shall be a fugitive and a vagabond on\n\r", ch);
		send_to_char("#cthe earth. And it shall come to pass that every one who findeth me shall\n\r", ch);
		send_to_char("#cslay me.'\n\r", ch);
		send_to_char("#c   And the LORD said unto him, 'Therefore whosoever slayeth Cain, vengeance\n\r", ch);
		send_to_char("#cshall be taken on him sevenfold.' And the LORD set a mark upon Cain, lest\n\r", ch);
		send_to_char("#cny finding him should kill him.\n\r", ch);
		send_to_char("#c   And Cain went out from the presence of the LORD, and dwelt in the land\n\r", ch);
		send_to_char("#cof Nod to the east of Eden.\n\r\n\r", ch);
		send_to_char("#P      --Genesis 4:9-16#n", ch);
		}
	}
	
if (IS_WEREWOLF (ch)){
	char_success = diceroll(ch, ATTRIB_STR, TALENTS, TAL_BRAWL, 6);
	
	if (char_success < 2){
		send_to_char("#gThe paw-print of a two year old alaskan timber wolf, canis lupus\n\r", ch);
		send_to_char("#gpambasilieus, is the same size of the face of a three month old human\n\r", ch);
		send_to_char("#gchild.  Humans fear the beast within the wolf because they do not\n\r", ch);
		send_to_char("#gunderstand the beast within themselves.\n\r", ch);
		}
	else if (char_success == 2){
		send_to_char("#gSister moon will you be my guide, in your blue blue shadows I would hide.\n\r", ch);
		send_to_char("#gAll good people asleep tonight, I'm all by myself in your silver light.\n\r", ch);
		send_to_char("#gTo howl at the moon the whole night through and they really don't care if I\n\r", ch);
		send_to_char("#gdo, I'd go out of my mind, but for you \n\r\n\r", ch);
		send_to_char("#G      --Sting, 'Sister Moon'#n\n\r", ch);
		}
	else{
		send_to_char("#gA few still hunt way out beyond philosophy where nothing is sacred till it\n\r", ch);
		send_to_char("#gis your flesh.#n\n\r", ch);
		}
	}

if ((!IS_VAMPIRE(ch)) && (!IS_GHOUL (ch)) && (!IS_INQUISITOR (ch)) && (!IS_WEREWOLF(ch))){
	char_success = diceroll(ch, ATTRIB_CHA, TALENTS, TAL_EMPATHY, 6);
	
	if (char_success < 2){
		send_to_char("#wA man does what he must - in spite of personal consequences, in spite\n\r", ch);
		send_to_char("#wof obstacles and dangers and pressures - and that is the basis of\n\r", ch);
		send_to_char("#wall human morality.\n\r\n\r", ch);
		send_to_char("#B      --JFK\n\r#n", ch);
		}
	else{
		send_to_char("#wRemember O God that my life is but a breath; My eye will never again see\n\r", ch);
		send_to_char("#wgood. The eye that sees me now will see me no longer, You will look for me,\n\r", ch);
		send_to_char("#wbut I will be gone.\n\r\n\r", ch);
		send_to_char("#B      --Job 7:7-8\n\r", ch);
		}
	}
	
	 /*End new Code */
 	//do_help( ch, "LOGOUT" );

	if (ch->pcdata->in_progress)
		free_note (ch->pcdata->in_progress);

	/*
	 * After extract_char the ch is no longer valid!
	 */
	d = ch->desc;
	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->wear_loc == WEAR_NONE)
			continue;
		if (!IS_NPC (ch) && ((obj->chobj != NULL && !IS_NPC (obj->chobj) && obj->chobj->pcdata->obj_vnum != 0)))
		{
			unequip_char (ch, obj);
			extract_obj (obj);
		}
	}
	do_autosave (ch, "");
	if (ch->pcdata->obj_vnum != 0)
		act ("$n slowly fades out of existance.", ch, NULL, NULL, TO_ROOM);
	else
		act ("$n has left the game.", ch, NULL, NULL, TO_ROOM);


	if (d != NULL)
		close_socket2 (d);

	if (ch->in_room != NULL)
		char_from_room (ch);
	char_to_room (ch, get_room_index (ROOM_VNUM_DISCONNECTION));

	sprintf (log_buf, "%s has quit.", ch->name);
	log_string (log_buf, CHANNEL_LOG_NORMAL);

	if (ch->onPhone != NULL)
		do_hangup(ch, "");
	if ((obj = ch->pcdata->chobj) != NULL)
	{
		obj->chobj = NULL;
		ch->pcdata->chobj = NULL;
		extract_obj (obj);
	}
	else if (ch->pcdata->obj_vnum == 0)
	{
		//if (strlen(ch->pcdata->roomdesc) > 1)
			sprintf (buf, "#G%s#e has left #gM#Giami#e by#w N#eight.#n", ch->pcdata->roomdesc);
		//else
			//sprintf (buf, "#GSome new person#e has left #gM#Giami#e by#w N#eight.#n");
		do_info (ch, buf);
	}
	extract_char (ch, TRUE);
	return;
}



void do_save (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	save_char_obj (ch);
	send_to_char ("#eSomeone saved your soul tonight.#n\n\r", ch);
	return;
}



void do_autosave (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
	if (ch->level < 2)
		return;
	if (ch->mkill < 5 && !IS_IMMORTAL (ch))
		return;
	if (ch->name == NULL)
		return;
	save_char_obj (ch);
	return;
}



void do_follow (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Follow whom?\n\r", ch);
		return;
	}

	if (((victim = get_char_room (ch, arg)) == NULL) && ((victim = get_char_room_from_alias(ch, arg)) == NULL) && ((victim = get_char_room_from_desc(ch, arg)) == NULL))
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL)
	{
		act ("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
		return;
	}

	if (victim == ch)
	{
		if (ch->master == NULL)
		{
			send_to_char ("You already follow yourself.\n\r", ch);
			return;
		}
		stop_follower (ch);
		return;
	}

	if (ch->master != NULL)
		stop_follower (ch);

	add_follower (ch, victim);
	return;
}



void add_follower (CHAR_DATA * ch, CHAR_DATA * master)
{
	if (ch->master != NULL)
	{
		bug ("Add_follower: non-null master.", 0);
		return;
	}

	ch->master = master;
	ch->leader = NULL;

	if (can_see (master, ch) && !IS_MORE3(ch, MORE3_OBFUS2))
		act ("$n now follows you.", ch, NULL, master, TO_VICT);

	act ("You now follow $N.", ch, NULL, master, TO_CHAR);

	return;
}



void stop_follower (CHAR_DATA * ch)
{
	if (ch->master == NULL)
	{
		bug ("Stop_follower: null master.", 0);
		return;
	}

//	if (IS_AFFECTED (ch, AFF_CHARM))
//	{
//		REMOVE_BIT (ch->affected_by, AFF_CHARM);
//		affect_strip (ch, gsn_charm_person);
//	}

	if ((can_see (ch->master, ch)) && !IS_MORE3(ch, MORE3_OBFUS2))
		act ("$n stops following you.", ch, NULL, ch->master, TO_VICT);
	act ("You stop following $N.", ch, NULL, ch->master, TO_CHAR);

	ch->master = NULL;
	ch->leader = NULL;
	return;
}



void die_follower (CHAR_DATA * ch)
{
	CHAR_DATA *fch;
	CHAR_DATA *fch_next;

	if (ch->master != NULL)
		stop_follower (ch);

	ch->leader = NULL;

	for (fch = char_list; fch != NULL; fch = fch_next)
	{
		fch_next = fch->next;
		if (fch->leader == ch)
			fch->leader = fch;
		if (fch->master == ch)
			stop_follower (fch);
		if (!IS_NPC (ch) && IS_NPC (fch) && fch->lord != NULL)
		{
			if (strlen (fch->lord) > 1 && !str_cmp (fch->lord, ch->name))
			{
				act ("$n slowly fades out of existance.", fch, NULL, NULL, TO_ROOM);
				extract_char (fch, TRUE);
			}
		}
	}

	return;
}


/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group (CHAR_DATA * ach, CHAR_DATA * bch)
{
	if (ach->leader != NULL)
		ach = ach->leader;
	if (bch->leader != NULL)
		bch = bch->leader;
	return ach == bch;
}

void do_changelight (CHAR_DATA * ch, char *argument)
{
	if (IS_SET (ch->in_room->room_flags, ROOM_DARK))
	{
		REMOVE_BIT (ch->in_room->room_flags, ROOM_DARK);
		act ("The room is suddenly filled with light!", ch, NULL, NULL, TO_CHAR);
		act ("The room is suddenly filled with light!", ch, NULL, NULL, TO_ROOM);
		return;
	}
	SET_BIT (ch->in_room->room_flags, ROOM_DARK);
	act ("The lights in the room suddenly go out!", ch, NULL, NULL, TO_CHAR);
	act ("The lights in the room suddenly go out!", ch, NULL, NULL, TO_ROOM);
	return;
}

void open_lift (CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *location;
	int in_room;

	in_room = ch->in_room->vnum;
	location = get_room_index (in_room);

	if (is_open (ch))
		return;

	act ("The doors open.", ch, NULL, NULL, TO_CHAR);
	act ("The doors open.", ch, NULL, NULL, TO_ROOM);
	move_door (ch);
	if (is_open (ch))
		act ("The doors close.", ch, NULL, NULL, TO_ROOM);
	if (!same_floor (ch, in_room))
		act ("The lift judders suddenly.", ch, NULL, NULL, TO_ROOM);
	if (is_open (ch))
		act ("The doors open.", ch, NULL, NULL, TO_ROOM);
	move_door (ch);
	open_door (ch, FALSE);
	char_from_room (ch);
	char_to_room (ch, location);
	open_door (ch, TRUE);
	move_door (ch);
	open_door (ch, TRUE);
	thru_door (ch, in_room);
	char_from_room (ch);
	char_to_room (ch, location);
	return;
}

void close_lift (CHAR_DATA * ch)
{
	ROOM_INDEX_DATA *location;
	int in_room;

	in_room = ch->in_room->vnum;
	location = get_room_index (in_room);

	if (!is_open (ch))
		return;
	act ("The doors close.", ch, NULL, NULL, TO_CHAR);
	act ("The doors close.", ch, NULL, NULL, TO_ROOM);
	open_door (ch, FALSE);
	move_door (ch);
	open_door (ch, FALSE);
	char_from_room (ch);
	char_to_room (ch, location);
	return;
}

void move_lift (CHAR_DATA * ch, int to_room)
{
	ROOM_INDEX_DATA *location;
	int in_room;

	in_room = ch->in_room->vnum;
	location = get_room_index (in_room);

	if (is_open (ch))
		act ("The doors close.", ch, NULL, NULL, TO_CHAR);
	if (is_open (ch))
		act ("The doors close.", ch, NULL, NULL, TO_ROOM);
	if (!same_floor (ch, to_room))
		act ("The lift judders suddenly.", ch, NULL, NULL, TO_CHAR);
	if (!same_floor (ch, to_room))
		act ("The lift judders suddenly.", ch, NULL, NULL, TO_ROOM);
	move_door (ch);
	open_door (ch, FALSE);
	char_from_room (ch);
	char_to_room (ch, location);
	open_door (ch, FALSE);
	thru_door (ch, to_room);
	return;
}

bool same_floor (CHAR_DATA * ch, int cmp_room)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->item_type != ITEM_PORTAL)
			continue;
		if (obj->pIndexData->vnum == 30001)
			continue;
		if (obj->value[0] == cmp_room)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

bool is_open (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->item_type != ITEM_PORTAL)
			continue;
		if (obj->pIndexData->vnum == 30001)
			continue;
		if (obj->value[2] == 0)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

void move_door (CHAR_DATA * ch)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	ROOM_INDEX_DATA *pRoomIndex;

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->item_type != ITEM_PORTAL)
			continue;
		if (obj->pIndexData->vnum == 30001)
			continue;
		pRoomIndex = get_room_index (obj->value[0]);
		char_from_room (ch);
		char_to_room (ch, pRoomIndex);
		return;
	}
	return;
}

void thru_door (CHAR_DATA * ch, int doorexit)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->item_type != ITEM_PORTAL)
			continue;
		if (obj->pIndexData->vnum == 30001)
			continue;
		obj->value[0] = doorexit;
		return;
	}
	return;
}

void open_door (CHAR_DATA * ch, bool be_open)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for (obj = ch->in_room->contents; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->item_type != ITEM_PORTAL)
			continue;
		if (obj->pIndexData->vnum == 30001)
			continue;
		if (obj->value[2] == 0 && !be_open)
			obj->value[2] = 3;
		else if (obj->value[2] == 3 && be_open)
			obj->value[2] = 0;
		return;
	}
	return;
}





void do_speak (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	int x, sn;
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (arg[0] == '\0')
	{

		const char *lang[21] = { "English", "German", "Vietnamese", "Russian", "Arabic",
			"Chinese", "French", "Latin", "Spanish", "Italian",
			"Gaelic", "Hebrew", "Japanese", "Greek", "Indonesian",
			"Slavic", "Garou", "Korean", "Swahili", "Tagalog", "Hindu"};

		const sh_int lang_sn[21] = { gsn_english, gsn_german, gsn_vietnamese, gsn_russian, gsn_arabic,
			gsn_chinese, gsn_french, gsn_latin, gsn_spanish,
			gsn_italian, gsn_gaelic, gsn_hebrew, gsn_japanese, gsn_greek, gsn_indonesian,
			gsn_slavic, gsn_garou, gsn_korean, gsn_swahili, gsn_tagalog, gsn_hindu };

		for (x = 0; x < 21; x++)
		{
			sn = lang_sn[x];

			/* Tell them what they know */

			if (PC (ch, learned)[sn] < 0)
				continue;
	
			if (x != 16)
				ch_printf (ch, " %s,",  lang[x]);
			else
				ch_printf (ch, " %s.", lang[x]);
			if (x == 5 || x == 10 || x == 15)
				send_to_char ("\n\r", ch);
		}
		return;
	}


	if (!str_cmp (arg, "english"))
	{
		if (ch->pcdata->language[0] == LANG_ENGLISH)
		{
			send_to_char ("But you are already speaking english!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_ENGLISH;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "olde"))
	{
		if (ch->pcdata->language[0] == DIA_OLDE)
		{
			send_to_char ("But you are already speaking Olde Worlde!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = DIA_OLDE;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "arabic") && ch->pcdata->learned[gsn_arabic] > 0)
	{
		if (ch->pcdata->language[0] == LANG_ARABIC)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_ARABIC;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "chinese") && ch->pcdata->learned[gsn_chinese] > 0)
	{
		if (ch->pcdata->language[0] == LANG_CHINESE)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_CHINESE;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "french") && ch->pcdata->learned[gsn_french] > 0)
	{
		if (ch->pcdata->language[0] == LANG_FRENCH)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_FRENCH;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "latin") && ch->pcdata->learned[gsn_latin] > 0)
	{
		if (ch->pcdata->language[0] == LANG_LATIN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_LATIN;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "spanish") && ch->pcdata->learned[gsn_spanish] > 0)
	{
		if (ch->pcdata->language[0] == LANG_SPANISH)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_SPANISH;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "italian") && ch->pcdata->learned[gsn_italian] > 0)
	{
		if (ch->pcdata->language[0] == LANG_ITALIAN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_ITALIAN;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg, "german") && ch->pcdata->learned[gsn_german] > 0)
	{
		if (ch->pcdata->language[0] == LANG_GERMAN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_GERMAN;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "russian") && ch->pcdata->learned[gsn_russian] > 0)
	{
		if (ch->pcdata->language[0] == LANG_RUSSIAN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_RUSSIAN;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg, "gaelic") && ch->pcdata->learned[gsn_gaelic] > 0)
	{
		if (ch->pcdata->language[0] == LANG_GAELIC)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_GAELIC;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "japanese") && ch->pcdata->learned[gsn_japanese] > 0)
	{
		if (ch->pcdata->language[0] == LANG_JAPANESE)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_JAPANESE;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "hebrew") && ch->pcdata->learned[gsn_hebrew] > 0)
	{
		if (ch->pcdata->language[0] == LANG_HEBREW)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_HEBREW;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg, "greek") && ch->pcdata->learned[gsn_greek] > 0)
	{
		if (ch->pcdata->language[0] == LANG_GREEK)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_GREEK;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "slavic") && ch->pcdata->learned[gsn_slavic] > 0)
	{
		if (ch->pcdata->language[0] == LANG_SLAVIC)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_SLAVIC;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "garou") && ch->pcdata->learned[gsn_garou] > 0)
	{
		if (ch->pcdata->language[0] == LANG_GAROU)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_GAROU;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg, "indonesian") && ch->pcdata->learned[gsn_indonesian] > 0)
	{
		if (ch->pcdata->language[0] == LANG_INDONESIAN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}

		ch->pcdata->language[0] = LANG_INDONESIAN;
		send_to_char ("Ok.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg, "korean") && ch->pcdata->learned[gsn_korean] > 0)
	{
		if (ch->pcdata->language[0] == LANG_KOREAN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_KOREAN;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "vietnamese") && ch->pcdata->learned[gsn_vietnamese] > 0)
	{
		if (ch->pcdata->language[0] == LANG_VIETNAMESE)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_VIETNAMESE;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "swahili") && ch->pcdata->learned[gsn_swahili] > 0)
	{
		if (ch->pcdata->language[0] == LANG_SWAHILI)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_SWAHILI;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "tagalog") && ch->pcdata->learned[gsn_tagalog] > 0)
	{
		if (ch->pcdata->language[0] == LANG_TAGALOG)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_TAGALOG;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "hindu") && ch->pcdata->learned[gsn_hindu] > 0)
	{
		if (ch->pcdata->language[0] == LANG_HINDU)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_HINDU;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	
	else
	{

		const char *lang[21] = { "English", "German", "Vietnamese", "Russian", "Arabic",
			"Chinese", "French", "Latin", "Spanish", "Italian",
			"Gaelic", "Hebrew", "Japanese", "Greek", "Indonesian",
			"Slavic", "Garou", "Korean", "Swahili", "Tagalog", "Hindu"};

	const sh_int lang_sn[21] = { gsn_english, gsn_german, gsn_vietnamese, gsn_russian, gsn_arabic,
			gsn_chinese, gsn_french, gsn_latin, gsn_spanish,
			gsn_italian, gsn_gaelic, gsn_hebrew, gsn_japanese, gsn_greek, gsn_indonesian,
			gsn_slavic, gsn_garou, gsn_korean, gsn_swahili, gsn_tagalog, gsn_hindu };

		for (x = 0; x < 21; x++)
		{
			sn = lang_sn[x];

			/* Tell them what they know */

			if (PC (ch, learned)[sn] < 0)
				continue;
			if (x != 16)
				ch_printf (ch, " %s,",  lang[x]);
			else
				ch_printf (ch, " %s.", lang[x]);
			if (x == 5 || x == 10 || x == 15)
				send_to_char ("\n\r", ch);		
		}
		return;

	}

	return;
}



void do_languagelearn (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int numberoflanguages, languageslearned = 0;
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	numberoflanguages = ch->abilities[KNOWLEDGES][KNO_LINGUISTICS] + 1;

	if (IS_WEREWOLF (ch) && ch->pcdata->learned[gsn_garou] < 100)
		ch->pcdata->learned[gsn_garou] = 100;

	if (arg[0] == '\0')
	{
		int x, sn;
		const char *lang[21] = { "English", "German", "Vietnamese", "Russian", "Arabic",
			"Chinese", "French", "Latin", "Spanish", "Italian",
			"Gaelic", "Hebrew", "Japanese", "Greek", "Indonesian",
			"Slavic", "Garou", "Korean", "Swahili", "Tagalog", "Hindu"};

		const sh_int lang_sn[21] = { gsn_english, gsn_german, gsn_vietnamese, gsn_russian, gsn_arabic,
			gsn_chinese, gsn_french, gsn_latin, gsn_spanish,
			gsn_italian, gsn_gaelic, gsn_hebrew, gsn_japanese, gsn_greek, gsn_indonesian,
			gsn_slavic, gsn_garou, gsn_korean, gsn_swahili, gsn_tagalog, gsn_hindu };
		send_to_char ("You can still learn the following languages.\n\r", ch);

		stc (" ", ch);

		for (x = 0; x < 21; x++)
		{
			sn = lang_sn[x];
			/* Do they already know it ? */
			if (PC (ch, learned)[sn] > 0)
				continue;
			ch_printf (ch, "%s, ", lang[x]);
		}
		send_to_char ("\n\r\n\r", ch);
		for (x = 0; x < 21; x++)
		{
			sn = lang_sn[x];

			/* Tell them what they know */

			if (PC (ch, learned)[sn] < 0)
				continue;
			ch_printf (ch, "You are currently %s in %s.\n\r", skill_level (PC (ch, learned)[sn]), lang[x]);
		}
		return;
	}

	//checks to see if you know it.
	if (!str_cmp (arg, "arabic") && ch->pcdata->learned[gsn_arabic] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "chinese") && ch->pcdata->learned[gsn_chinese] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "french") && ch->pcdata->learned[gsn_french] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "latin") && ch->pcdata->learned[gsn_latin] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "spanish") && ch->pcdata->learned[gsn_spanish] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "italian") && ch->pcdata->learned[gsn_italian] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "german") && ch->pcdata->learned[gsn_german] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "russian") && ch->pcdata->learned[gsn_russian] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "hebrew") && ch->pcdata->learned[gsn_hebrew] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "gaelic") && ch->pcdata->learned[gsn_gaelic] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "japanese") && ch->pcdata->learned[gsn_japanese] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "greek") && ch->pcdata->learned[gsn_greek] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "slavic") && ch->pcdata->learned[gsn_slavic] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "indonesian") && ch->pcdata->learned[gsn_indonesian] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "english") && ch->pcdata->learned[gsn_english] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "korean") && ch->pcdata->learned[gsn_korean] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "vietnamese") && ch->pcdata->learned[gsn_vietnamese] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
		else if (!str_cmp (arg, "swahili") && ch->pcdata->learned[gsn_swahili] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
		else if (!str_cmp (arg, "tagalog") && ch->pcdata->learned[gsn_tagalog] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
		else if (!str_cmp (arg, "hindu") && ch->pcdata->learned[gsn_hindu] > 0)
	{
		send_to_char ("But you are already learning that language!\n\r", ch);
		return;
	}
	
	
	
	//if number of languages known(- latin) >= linguistics - latin
//       send message
	if (ch->pcdata->learned[gsn_arabic] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_chinese] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_french] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_latin] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_spanish] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_italian] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_german] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_russian] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_hebrew] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_gaelic] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_japanese] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_greek] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_slavic] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_indonesian] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_english] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_korean] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_vietnamese] > 0)
		languageslearned++;		
	if (ch->pcdata->learned[gsn_swahili] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_tagalog] > 0)
		languageslearned++;	
	if (ch->pcdata->learned[gsn_hindu] > 0)
		languageslearned++;
	
	
	if (languageslearned >= numberoflanguages)
	{
		send_to_char ("You cannot learn any more languages, increase your linguistics ability\n\r", ch);
		return;
	}


	// gives you 75% in the langauge
	if (!str_cmp (arg, "arabic"))
	{
		ch->pcdata->learned[gsn_arabic] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "chinese"))
	{
		ch->pcdata->learned[gsn_chinese] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "english"))
	{
		ch->pcdata->learned[gsn_english] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "french"))
	{
		ch->pcdata->learned[gsn_french] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "latin"))
	{
		ch->pcdata->learned[gsn_latin] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "spanish"))
	{
		ch->pcdata->learned[gsn_spanish] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "italian"))
	{
		ch->pcdata->learned[gsn_italian] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "german"))
	{
		ch->pcdata->learned[gsn_german] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "russian"))
	{
		ch->pcdata->learned[gsn_russian] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "gaelic"))
	{
		ch->pcdata->learned[gsn_gaelic] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "hebrew"))
	{
		ch->pcdata->learned[gsn_hebrew] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "japanese"))
	{
		ch->pcdata->learned[gsn_japanese] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "greek"))
	{
		ch->pcdata->learned[gsn_greek] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "slavic"))
	{
		ch->pcdata->learned[gsn_slavic] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "indonesian"))
	{
		ch->pcdata->learned[gsn_indonesian] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "korean"))
	{
		ch->pcdata->learned[gsn_korean] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "vietnamese"))
	{
		ch->pcdata->learned[gsn_vietnamese] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "swahili"))
	{
		ch->pcdata->learned[gsn_swahili] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "tagalog"))
	{
		ch->pcdata->learned[gsn_tagalog] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "hindu"))
	{
		ch->pcdata->learned[gsn_hindu] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	
	else
	{
		send_to_char ("You can learn the following languages.\n\r", ch);
		strcpy (buf, "German, Russian, Arabic, French,\n\r");
		strcat (buf, "Latin, Spanish, Italian, Gaelic, Hebrew,\n\r");
		strcat (buf, "Greek, Slavic, Chinese, Japanese Indonesian,\n\r");
		strcat (buf, "Korean, Vietnamese, Swahili, Tagalog and Hindu.\n\r");
		send_to_char (buf, ch);
		return;
	}
	return;
}


char *oldelang (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char *pName;
	int iSyl;
	int length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"have", "hath"},
		{"hello", "hail"},
		{"hi ", "hail "},
		{" hi", " hail"},
		{"are", "art"},
		{"your", "thy"},
		{"you", "thou"},
		{"i think", "methinks"},
		{"do ", "doth "},
		{" do", " doth"},
		{"it was", "twas"},
		{"before", "ere"},
		{"his", "$s"},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};
	buf[0] = '\0';

	if (argument[0] == '\0')
		return argument;

	for (pName = str_dup (argument); *pName != '\0'; pName += length)
	{
		for (iSyl = 0; (length = strlen (spk_table[iSyl].old)) != 0; iSyl++)
		{
			if (!str_prefix (spk_table[iSyl].old, pName))
			{
				strcat (buf, spk_table[iSyl].new);
				break;
			}
		}

		if (length == 0)
			length = 1;
	}

	argument[0] = '\0';
	strcpy (argument, buf);
	argument[0] = UPPER (argument[0]);

	return argument;
}


char *socialc (CHAR_DATA * ch, char *argument, char *you, char *them)
{
	char buf[MAX_STRING_LENGTH];
	char *pName;
	int iSyl;
	int length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"you are", "$E is"},
		{"you.", "$M."},
		{"you,", "$M,"},
		{"you ", "$M "},
		{" you", " $M"},
		{"your ", "$S "},
		{" your", " $S"},
		{"yours.", "theirs."},
		{"yours,", "theirs,"},
		{"yours ", "theirs "},
		{" yours", " theirs"},
		{"begins", "begin"},
		{"caresses", "caress"},
		{"gives", "give"},
		{"glares", "glare"},
		{"grins", "grin"},
		{"licks", "lick"},
		{"looks", "look"},
		{"loves", "love"},
		{"plunges", "plunge"},
		{"presses", "press"},
		{"pulls", "pull"},
		{"runs", "run"},
		{"slaps", "slap"},
		{"slides", "slide"},
		{"smashes", "smash"},
		{"squeezes", "squeeze"},
		{"stares", "stare"},
		{"sticks", "stick"},
		{"strokes", "stroke"},
		{"tugs", "tug"},
		{"thinks", "think"},
		{"thrusts", "thrust"},
		{"whistles", "whistle"},
		{"wraps", "wrap"},
		{"winks", "wink"},
		{"wishes", "wish"},
		{" winks", " wink"},
		{" his", " your"},
		{"his ", "your "},
		{" her", " your"},
		{"her ", "your "},
		{" him", " your"},
		{"him ", "your "},
		{"the", "the"},
		{" he", " you"},
		{"he ", "you "},
		{" she", " you"},
		{"she ", "you "},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};
	buf[0] = '\0';

	if (argument[0] == '\0')
		return argument;

	for (pName = str_dup (argument); *pName != '\0'; pName += length)
	{
		for (iSyl = 0; (length = strlen (spk_table[iSyl].old)) != 0; iSyl++)
		{
			if (!str_prefix (spk_table[iSyl].old, pName))
			{
				strcat (buf, spk_table[iSyl].new);
				break;
			}
		}

		if (length == 0)
			length = 1;
	}

	argument[0] = '\0';
	strcpy (argument, buf);
	argument[0] = UPPER (argument[0]);

	return argument;
}

char *socialv (CHAR_DATA * ch, char *argument, char *you, char *them)
{
	char buf[MAX_STRING_LENGTH];
	char *pName;
	int iSyl;
	int length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{" his", " $s"},
		{"his ", "$s "},
		{" her", " $s"},
		{"her ", "$s "},
		{" him", " $m"},
		{"him ", "$m "},
		{" he", " $e"},
		{"he ", "$e "},
		{" she", " $e"},
		{"she ", "$e "},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};
	buf[0] = '\0';

	if (argument[0] == '\0')
		return argument;

	for (pName = str_dup (argument); *pName != '\0'; pName += length)
	{
		for (iSyl = 0; (length = strlen (spk_table[iSyl].old)) != 0; iSyl++)
		{
			if (!str_prefix (spk_table[iSyl].old, pName))
			{
				strcat (buf, spk_table[iSyl].new);
				break;
			}
		}

		if (length == 0)
			length = 1;
	}

	argument[0] = '\0';
	strcpy (argument, buf);
	argument[0] = UPPER (argument[0]);

	return argument;
}

char *socialn (CHAR_DATA * ch, char *argument, char *you, char *them)
{
	char buf[MAX_STRING_LENGTH];
	char *pName;
	int iSyl;
	int length;

	struct spk_type
	{
		char *old;
		char *new;
	};

	static const struct spk_type spk_table[] = {
		{" ", " "},
		{"you are", "$N is"},
		{"you.", "$N."},
		{"you,", "$N,"},
		{"you ", "$N "},
		{" you", " $N"},
		{"your.", "$N's."},
		{"your,", "$N's,"},
		{"your ", "$N's "},
		{" your", " $N's"},
		{"yourself", "$Mself"},
		{" his", " $s"},
		{"his ", "$s "},
		{" her", " $s"},
		{"her ", "$s "},
		{" him", " $m"},
		{"him ", "$m "},
		{" he", " $e"},
		{"he ", "$e "},
		{" she", " $e"},
		{"she ", "$e "},
		{"a", "a"}, {"b", "b"}, {"c", "c"}, {"d", "d"},
		{"e", "e"}, {"f", "f"}, {"g", "g"}, {"h", "h"},
		{"i", "i"}, {"j", "j"}, {"k", "k"}, {"l", "l"},
		{"m", "m"}, {"n", "n"}, {"o", "o"}, {"p", "p"},
		{"q", "q"}, {"r", "r"}, {"s", "s"}, {"t", "t"},
		{"u", "u"}, {"v", "v"}, {"w", "w"}, {"x", "x"},
		{"y", "y"}, {"z", "z"}, {",", ","}, {".", "."},
		{";", ";"}, {":", ":"}, {"(", "("}, {")", ")"},
		{")", ")"}, {"-", "-"}, {"!", "!"}, {"?", "?"},
		{"1", "1"}, {"2", "2"}, {"3", "3"}, {"4", "4"},
		{"5", "5"}, {"6", "6"}, {"7", "7"}, {"8", "8"},
		{"9", "9"}, {"0", "0"}, {"%", "%"}, {"", ""}
	};
	buf[0] = '\0';

	if (argument[0] == '\0')
		return argument;
	for (pName = str_dup (argument); *pName != '\0'; pName += length)
	{
		for (iSyl = 0; (length = strlen (spk_table[iSyl].old)) != 0; iSyl++)
		{
			if (!str_prefix (spk_table[iSyl].old, pName))
			{
				strcat (buf, spk_table[iSyl].new);
				break;
			}
		}

		if (length == 0)
			length = 1;
	}

	argument[0] = '\0';
	strcpy (argument, buf);
	argument[0] = UPPER (argument[0]);

	return argument;
}

void do_inform (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	char buf2[10];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int num = 0;
	bool t_f;
	bool truth = TRUE;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		send_to_char ("What do you wish to inform?\n\r", ch);
		send_to_char ("Options: Generation Diablerie.\n\r", ch);
		return;
	}

	if (!str_cmp (arg1, "gen") || !str_cmp (arg1, "generation"))
	{
		if (arg2[0] == '\0')
			num = ch->vampgen;
		else if (is_number (arg2))
			num = atoi (arg2);
		else
		{
			send_to_char ("Please select a generation in the range 1 to 13.\n\r", ch);
			return;
		}
		if (num < 1 || num > 13)
		{
			send_to_char ("Please select a generation in the range 1 to 13.\n\r", ch);
			return;
		}
		switch (num)
		{
		default:
			strcpy (buf2, "th");
			break;
		case 1:
			strcpy (buf2, "st");
			break;
		case 2:
			strcpy (buf2, "nd");
			break;
		case 3:
			strcpy (buf2, "rd");
			break;
		}
		if (ch->vampgen != num)
			truth = FALSE;
		sprintf (buf, "I am of the %d%s generation.", num, buf2);
	}
	else if (!str_cmp (arg1, "diab") || !str_cmp (arg1, "diablerie"))
	{
		if (arg2[0] == '\0')
		{
			if (ch->pcdata->diableries[DIAB_EVER] > 0)
				t_f = TRUE;
			else
				t_f = FALSE;
		}
		else if (!str_cmp (arg2, "true") || !str_cmp (arg2, "yes"))
			t_f = TRUE;
		else if (!str_cmp (arg2, "false") || !str_cmp (arg2, "no"))
			t_f = FALSE;
		else
		{
			send_to_char ("Have you ever committed diablerie? YES or NO.\n\r", ch);
			return;
		}
		if (ch->pcdata->diableries[DIAB_EVER] > 0 && !t_f)
			truth = FALSE;
		else if (ch->pcdata->diableries[DIAB_EVER] < 1 && t_f)
			truth = FALSE;
		if (t_f)
			strcpy (buf, "I have committed diablerie in the past.");
		else
			strcpy (buf, "I have never committed diablerie.");
	}
	else
	{
		send_to_char ("What do you wish to inform?\n\r", ch);
		send_to_char ("Options: Generation Diablerie.\n\r", ch);
		return;
	}

	if (truth)
		ch->pcdata->lie = LIE_FALSE;
	else
		ch->pcdata->lie = LIE_TRUE;
	SET_BIT (ch->more, MORE_INFORM);
	do_say (ch, buf);
	return;
}






void do_rpose (CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;



	if (argument[0] == '\0')
	{
		send_to_char ("#cRpose what?\n\r", ch);
		return;
	}
	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
		
	if (strchr(argument, '"') != NULL)
		argument = colorPose(argument);

//	parse_rp (ch, argument);

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING && d->character->in_room == ch->in_room)
		{
			send_to_char (argument, d->character);
			send_to_char ("\n\r", d->character);
		}
	}

	ch->last_pose = current_time;
	return;
}


void log_reward (CHAR_DATA * ch, int reward)
{
	FILE *fp;
	char *temp;
	fp = fopen (REWARD_FILE, "a");

	if (!fp)
	{
		bug ("Could not open " REWARD_FILE " for writing", 0);
		return;
	}
	temp = ctime (&current_time);
	temp[strlen (temp) - 1] = '\0';
	fprintf (fp, "%s: %s was rewarded %d xp.\n\r", temp, ch->name, reward);
	fclose (fp);
}

void do_vote (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_INPUT_LENGTH];
	
	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Vote for whom?\n\r", ch);
		return;
	}

	if (((victim = get_char_finger (ch, arg)) == NULL) && ((victim = get_char_room_from_alias(ch, arg)) == NULL) && ((victim = get_char_room_from_desc(ch, arg)) == NULL))
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	


	if (IS_IMMORTAL(victim))
	{
		send_to_char ("Immortals don't need your votes.\n\r", ch);
	}

	if (victim == ch)
	{
		send_to_char ("Sorry, but you can't vote for yourself.\n\r", ch);
		return;
	}
	
	if ((ch->pcdata->canvotecounter < 3) && !IS_IMMORTAL(ch))
	{
		send_to_char ("You can't vote right now.\n\r", ch);
		return;
	}

	ch->pcdata->last_vote_time.day = time_info.day;
	victim->pcdata->votes++;
	sprintf(buf, "You've now awarded one vote to them.\n\r");

if (!IS_IMMORTAL(ch)){
		ch->pcdata->votes++;
		ch->pcdata->canvotecounter = 0;
	}

	send_to_char(buf, ch);
	
}

void do_givevote(CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument (argument, arg);
	if (arg[0] == '\0')
		send_to_char ("Give vote to whom?\n\r", ch);
	else if ((victim = get_char_world(ch, arg)) == NULL)
		send_to_char("No such player exists.\r\n", ch);
	else if (IS_IMMORTAL(victim))
		send_to_char("Immortals don't have timed votes.\r\n", ch);
	else if (victim->pcdata->canvotecounter >= 3)
		send_to_char("That character can already vote.\r\n", ch);
	else
	{
		victim->pcdata->canvotecounter = 3;
		send_to_char("They can now vote for someone.\r\n", ch);
		send_to_char("#yZOMG!#e You have been given a vote.#n\r\n", victim);
	}
}

char * colorPose(char *argument)
{
	char *final_string;
	char tempString[MAX_STRING_LENGTH];
	bool inquotes = FALSE;
	int offset = 0;
	int x = 0;

	while (x < strlen(argument))
	{
		if (argument[x] == '"')
		{
			if (inquotes == TRUE)
			{
				tempString[x + offset] = '#';
				tempString[x + offset + 1] = 'n';
				tempString[x + offset + 2] = '"';
				offset += 2;
				inquotes = FALSE;
			}
			else
			{
				tempString[x + offset] = '"';
				tempString[x + offset + 1] = '#';
				tempString[x + offset + 2] = 'y';
				offset += 2;
				inquotes = TRUE;
			}
		}
		else
			tempString[x + offset] = argument[x];
	
		x++;
	}

	if (inquotes == TRUE)
	{
		tempString[x + offset] = '#';
		tempString[x + offset + 1] = 'n';
		tempString[x + offset + 2] = '"';
		offset += 3;
		inquotes = FALSE;
	}
	
/*	if (tempString[x + offset] != '.')
	{
		tempString[x + offset] = '.';
		offset++;
	}
*/
	tempString[x + offset] = '\0';

	final_string = tempString;

	return final_string;
}	

void do_got (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	char buf[MAX_INPUT_LENGTH];
	
	one_argument (argument, arg);

	
	if (!IS_IMMORTAL(ch))
	{
		send_to_char ("#cHuh?#n\n\r", ch);
		return;
	}
	if (arg[0] == '\0')
	{
		send_to_char ("#cWho's request are you handling?#n\n\r", ch);
		return;
	}

	if ((victim = get_char_finger (ch, arg)) == NULL)
	{
		send_to_char ("#cThey aren't here.#n\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char ("#cWhy the fuck are you trying to handle your own request? You're an immortal anyway... Idiot.#n\n\r", ch);
		return;
	}
	
	sprintf(buf, "I'm now handling %s's request.", victim->name);
	do_story(ch, buf);
	send_to_char("#GAn ASM Staff Member is now handling your request#n\n\r", victim);
	
}

void do_nuketell (CHAR_DATA * ch, char *argument)
{
	
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("#cYou have to choose whos reply to nuke.#n\n\r", ch);
		return;
	}
	
	if ((victim = get_char_finger (ch, arg)) == NULL)
	{
		send_to_char ("#cThey aren't here.#n\n\r", ch);
		return;
	}	
	
	victim->reply = NULL;
	send_to_char("#cYou've nuked their tell.#n\n\r", ch);

}
