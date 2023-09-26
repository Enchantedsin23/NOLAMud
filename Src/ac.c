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
		    sprintf (buf, "#g[#wOOC#g]#n %s#w:#n $t#n", ch->name);
/* To switch back to names in OOC swap the if/else blocks */

		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#g[#wOOC#g]#n A Storyteller#w:#n $t#n");
		else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && (IS_STORYTELLER(ch)))
			sprintf (buf2, "#g[#wOOC#g]#n %s#w:#n $t#n", ch->morph);
		else
			sprintf (buf2, "#g[#wOOC#g]#n %s#w:#n $t#n", ch->name);
	
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;
		
	// Helpnet channel
	case CHANNEL_ADVICE:
		    sprintf (buf, "#y<#gAdvice#y>#n %s#w:#n $t#n", ch->name);
		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#y<#gAdvice#g>#n A Storyteller#w:#n $t#n");
		else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && (IS_STORYTELLER(ch)))
			sprintf (buf2, "#y<#gAdvice#y>#n %s#w:#n $t#n", ch->morph);
	    else if (IS_STORYTELLER (ch))
            sprintf (buf2, "#y<#gAdvice#y>#n %s #e(Staff)#w:#n $t#n", ch->name);
		else if (IS_HIGHJUDGE(ch))
		    sprintf (buf2, "#y<#gAdvice#y>#n %s #e(Staff)#w:#n $t#n", ch->name);
		else
			sprintf (buf2, "#y<#gAdvice#y>#n %s#w:#n $t#n", ch->name);
	
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;
		
		// Hint channel
		
		case CHANNEL_HINT:
		sprintf (buf, "#y[#gNewbie Hint!#y]#n $t#n");
	
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
			sprintf (buf, "#g[#wOOC#g] #n%s $t#n", ch->name);
		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#g[#wOOC#g]#n A Storyteller $t#n");
		else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && (IS_STORYTELLER(ch)))
			sprintf (buf2, "#g[#wOOC#g]#n %s $t#n", ch->morph);
		else
			sprintf (buf2, "#g[#wOOC#g]#n %s $t#n", ch->name);

		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;
/* End OMOTE Code */


	case CHANNEL_OSAY:
		sprintf (buf, "#gYou osay #y'#n$t#y'#n");
		
		
		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#gA Storyteller osays #y'#n$t#y'#n");
			
		else if (IS_MORE3(ch, MORE3_OBFUS3) || IS_MORE3(ch, MORE3_OBFUS1))
			sprintf (buf2, "#gAn unseen player osays #y'#n$t#y'#n");
		else
				sprintf (buf2, "#g%s#g osays #y'#n$t#y'#n", ch->name);
			
		    
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;

	case CHANNEL_STORY:
		sprintf (buf, "#e<#cStaff#e> #n%s#c: #e$t#n", capitalize (ch->name));
		sprintf (buf2, "#e<#cStaff#e> #n%s#c: #e$t#e#n", capitalize (ch->name));
		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;
		
/* New SMOTE Code */
	case CHANNEL_SMOTE:
			sprintf (buf, "#e<#cStaff#e> #n%s $t#n", ch->name);
		if (IS_SET (ch->act, PLR_WIZINVIS))
			sprintf (buf2, "#e<#cStaff#e>#n A Storyteller $t#n");
		else if ((IS_AFFECTED (ch, AFF_POLYMORPH)) && (IS_STORYTELLER(ch)))
			sprintf (buf2, "#e<#cStaff#e>#n %s $t#n", ch->morph);
		else
			sprintf (buf2, "#e<#cStaff#e>#n %s $t#n", ch->name);

		position = ch->position;
		ch->position = POS_STANDING;
		act (buf, ch, argument, NULL, TO_CHAR);
		ch->position = position;
		break;
/* End SMOTE Code */

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
			if (channel == CHANNEL_SMOTE && (!IS_NPC (och) && !IS_STORYTELLER (och)))
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
			
			if (channel != CHANNEL_ADVICE)
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

			// Helpnet channel
			if (channel == CHANNEL_ADVICE)
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
			sprintf (buf, "#w%s#n has been granted death's final release. #r]#n", ch->name);
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

// Helpnet channel
void do_advice (CHAR_DATA * ch, char *argument)
{
	talk_channel (ch, argument, CHANNEL_ADVICE, "advice");
	return;
}

void do_hint (CHAR_DATA * ch, char *argument)
{
	talk_channel (ch, argument, CHANNEL_HINT, "hint");
	return;
}

void do_omote (CHAR_DATA * ch, char *argument)
{
        talk_channel (ch, argument, CHANNEL_OMOTE, "omote");
        return;
}

void do_smote (CHAR_DATA * ch, char *argument)
{
        talk_channel (ch, argument, CHANNEL_SMOTE, "smote");
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
	sprintf (poly, "You %s '#e$T#n'", speak);

//	parse_rp (ch, argument);

	if (!IS_NPC (ch))
	{
		if (IS_SPEAKING (ch, LANG_GERMAN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, SLAVIC);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_FAE))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ch->pcdata->wolfform[1] == FORM_HUMAN)
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, FAE);
			return;
		}
		else if (IS_SPEAKING (ch, LANG_MANDARIN))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, MANDARIN);
			return;
		}
		if (IS_SPEAKING (ch, LANG_JAPANESE))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, VIETNAMESE);
			return;
		}
		if (IS_SPEAKING (ch, LANG_CANTONESE))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
        {
                send_to_char ("You aren't in the right form.\n\r", ch);
                return;
        }
			do_language (ch, argument, CANTONESE);
			return;
		}
		if (IS_SPEAKING (ch, LANG_TAGALOG))
		{
        if (IS_WEREWOLF(ch) && !IS_NPC (ch) &&
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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
                ( ch->pcdata->wolfform[1] == FORM_URHAN || 
		ch->pcdata->wolfform[1] == FORM_URSHUL ))
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

	sprintf (poly, "$n %s '#e$T#n'", speaks);

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
			case RT_VEHICLE:
				if (get_obj_index (OBJ_VNUM_VEHICLE) == NULL)
					return;
				obj = create_object (get_obj_index (OBJ_VNUM_VEHICLE), 0);
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
	OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }
	
	if (ch->in_room->sector_type == SECT_HEDGE) // Can't use in Hedge
    {
	send_to_char( "Modern technology doesn't function in the Hedge.\n\r", ch);
	return;
    }
	
	if (arg[0] == '\0') 
	{
		spam_cellphone_syntax(ch);
		return;
	}
	
	if (IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char ("#y<#gPhone#y> #wYou are silenced.#n\n\r", ch);
		return;
	}
	if (CELL_OFF(ch))    
		do_cell(ch,"on");
	if (ch->onPhone != NULL)
	{
		send_to_char("#y<#gPhone#y> #wYou can't call someone in the middle of another call.#n\n\r.#w Use '#Changup#n' first#n\n\r", ch);
		return;
	}
	
	send_to_char("#y<#gPhone#y> #wCalling...#n\n\r", ch);
	if (   ((victim = get_char_world(ch, arg)) == NULL) 
		|| (!IS_NPC(victim) && victim->desc == NULL)
		|| (CELL_OFF(victim))
		|| (victim == ch)
	   )
	{
		send_to_char("#y<#gPhone#y> #wYour call didn't connect.#n\n\r", ch);
		return;
	}
	if (victim->onPhone != NULL)
	{
		send_to_char("#y<#gPhone#y> #wThe call goes straight to voicemail.#n\n\r", ch);
		return;
	}
	if (victim->phonevibrate)
		send_to_char("#y<#gPhone#y> #wYour phone vibrates...#n\n\r", victim);
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
	OBJ_DATA *obj;
	
	if (argument[0] != '\0')
	{
		spam_cellphone_syntax(ch);
		return;
	}
	
	if (ch->in_room->sector_type == SECT_HEDGE) // Can't use in Hedge
    {
	send_to_char( "Modern technology doesn't function in the Hedge.\n\r", ch);
	return;
    }
	
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }
	
	if (CELL_OFF(ch))
	{
		send_to_char("#y<#gPhone#y> #wYour phone is #Roff#w.#n\n\r", ch);
		return;
	}
	if (ch->onPhone == NULL)
	{
		send_to_char("#y<#gPhone#y> #wNo one is trying to call you.#n\n\r", ch);
		return;
	}
	if (ch->onPhone->onPhone == NULL || ch->onPhone->desc == NULL)
	{
		send_to_char("#y<#gPhone#y> #wYou answer the call, but the phone drops signal.#n\n\r", ch);
		ch->onPhone->onPhone = NULL;
		ch->onPhone = NULL;
		return;
	}
	if (ch->phonestate == -1)
	{
		send_to_char("#y<#gPhone#y> #wYou can't answer your own phone call.#n\n\r", ch);
		return;
	}
	if (ch->phonestate == 1)
	{
		send_to_char("#y<#gPhone#y> #wYou're already on the phone.#n\n\r", ch);
		return;
	}
	ch->onPhone->phonestate = 1;
	ch->phonestate          = 1;
	send_to_char("#y<#gPhone#y> #wYour call is connected.#n\n\r", ch);
	send_to_char("#y<#gPhone#y> #w...the call connects.#n\n\r", ch->onPhone);
}

void do_hangup(register CHAR_DATA * ch, register char *argument)
{
	OBJ_DATA *obj;
	
	if (argument[0] != '\0')
	{
		spam_cellphone_syntax(ch);
		return;
	}
	
	if (ch->in_room->sector_type == SECT_HEDGE) // Can't use in Hedge
    {
	send_to_char( "Modern technology doesn't function in the Hedge.\n\r", ch);
	return;
    }
	
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }
	
	if (CELL_OFF(ch))
	{
		send_to_char("#y<#gPhone#y> #wYour phone is #Roff#w.#n\n\r", ch);
		return;
	}
	if (ch->onPhone == NULL)
	{
		send_to_char("#y<#gPhone#y> #wNo one is trying to call you.#n\n\r", ch);
		return;
	}
	send_to_char("#y<#gPhone#y> #wYou hang up your phone.#n\n\r", ch);
	send_to_char("#y<#gPhone#y> #wThe call disconnects.\n\r", ch->onPhone);
	ch->onPhone->onPhone = NULL;
	ch->onPhone          = NULL;
}

void do_cell(register CHAR_DATA * ch, register char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char saybuf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
	
	if (ch->in_room->sector_type == SECT_HEDGE) // Can't use in Hedge
    {
	send_to_char( "Modern technology doesn't function in the Hedge.\n\r", ch);
	return;
    }
	
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }
	
	if (IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char ("#y<#gPhone#y> #wYou are silenced.#n\n\r", ch);
		return;
	}
	if (!str_cmp(argument, "on"))
	{
		send_to_char("#y<#gPhone#y> #wYour phone is now #Gon#w.#n\n\r", ch);
		REMOVE_BIT(ch->deaf, CHANNEL_CELL);
		ch->phonevibrate = FALSE;
		return;
	}
	if (!str_cmp(argument, "off"))
	{
		if (ch->onPhone != NULL)  do_hangup(ch, "");
		send_to_char("#y<#gPhone#y> #wYour phone is now #Roff#w.#n\n\r", ch);
		SET_BIT(ch->deaf, CHANNEL_CELL);
		ch->phonevibrate = FALSE;
		return;
	}
	if (!str_cmp(argument, "vibrate"))
	{
		if (CELL_OFF(ch)) do_cell(ch, "on");
		send_to_char("#y<#gPhone#y> #wYour phone is now set to #yvibrate#w.#n\n\r", ch);
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
		send_to_char("#y<#gPhone#y> #wYou don't have a connected call.#n\n\r", ch);
		return;
	}
	if (ch->onPhone->onPhone == NULL || ch->onPhone->desc == NULL)
	{
		send_to_char("#y<#gPhone#y> #wYour phone lost signal.#n\n\r", ch);
		ch->onPhone->onPhone = NULL;
		ch->onPhone = NULL;
		return;
	}
	if (ch->phonestate == -1)
	{
		send_to_char("#y<#gPhone#y> #wThey haven't answered yet...#n", ch);
		return;
	}
	if (ch->phonestate == 0)
		do_answer(ch, "");
	sprintf(saybuf, "#w%s", argument);
	sprintf(buf, "#y<#gPhone#y>#n %s says, #g\"#w%s#g\"#n\n\r", ch->pcdata->voice, argument);
	do_say(ch, saybuf);
	send_to_char(buf, ch->onPhone);
}

void do_ringtone(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }
	
	if (argument[0] == '\0')
	{
		sprintf(buf, "#y<#gPhone#y> #wYour ringtone is currently: #w%s#n\n\r", ch->ringtone);
		send_to_char(buf, ch);
		return;
	}
	ch->ringtone = str_dup(argument);
	sprintf(buf, "#y<#gPhone#y> #wYour ringtone is now: #w%s#n\n\r", ch->ringtone);
	send_to_char(buf, ch);
} 
/* end cell phone code*/

/* Text Message Code */

void do_text (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;
	OBJ_DATA *obj;
	
	if (ch->in_room->sector_type == SECT_HEDGE) // Can't use in Hedge
    {
	send_to_char( "Modern technology doesn't function in the Hedge.\n\r", ch);
	return;
    }
	
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("#y<#gSMS#y> #wYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#y<#gSMS#y> #wYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (IS_EXTRA (ch, GAGGED))
	{
		send_to_char ("#y<#gSMS#y> #wYour message didn't get through.#n\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("#y<#gSMS#y> #wText whom what?#n\n\r", ch);
		return;
	}

	/*
	 * Can text to PC's anywhere, but NPC's only in same room.
	 * -- Lea
	 */
	if ((victim = get_char_world (ch, arg)) == NULL || (IS_NPC (victim) && victim->in_room != ch->in_room))
	{
		send_to_char ("#y<#gSMS#y> #wThey aren't here.#n\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("#y<#gSMS#y> #n$E can't hear you.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("#y<#gSMS#y> #n$E is currently link dead.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->extra2, EXTRA2_DND) && !IS_IMMORTAL (ch))
	{
		send_to_char ("#y<#gSMS#y> #nYou have messaged a DND Imp.. Bad move..#n\n\r", ch);
		send_to_char ("#y<#gSMS#y> #nThe message is ignored.#n\n\r", ch);
		do_info (victim, poly);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("#y<#gSMS#y> #n$E can't hear you.#n", ch, 0, victim, TO_CHAR);
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
		sprintf (name, "A Private Number");

	sprintf (poly, "#y<#gSMS to %s#y:#w $t#y>#n", name);
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
		sprintf (name, "A Private Number");

	sprintf (poly, "#y<#gSMS from %s#y:#w $t#g>#n", name);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	return;
}

/* End Text Message Code */

/* Start SMS Photo message code */

void do_snap (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;
	OBJ_DATA *obj;
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_HEDGE)) // Can't use in Hedge
    {
	send_to_char( "Modern technology doesn't function in the Hedge.\n\r", ch);
	return;
    }
	
    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL
	|| obj->item_type != ITEM_PHONE )
    {
    	if ( ( obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL
	    || obj->item_type != ITEM_PHONE )
	{
	    send_to_char( "You aren't holding a phone!\n\r", ch );
	    return;
	}
    }

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("#y<#gMMS#y> #nYour image didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#y<#gMMS#y> #nYour image didn't get through.#n\n\r", ch);
		return;
	}
	if (IS_EXTRA (ch, GAGGED))
	{
		send_to_char ("#y<#gMMS#y> #nYour image didn't get through.#n\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("#y<#gMMS#y> #nSend whom what image?#n\n\r", ch);
		return;
	}

	/*
	 * Can text to PC's anywhere, but NPC's only in same room.
	 * -- Lea
	 */
	if ((victim = get_char_world (ch, arg)) == NULL || (IS_NPC (victim) && victim->in_room != ch->in_room))
	{
		send_to_char ("#y<#gMMS#y> #nThey aren't here.#n\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("#y<#gMMS#y> #n$E can't hear you.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("#y<#gMMS#y> #n$E is currently link dead.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->extra2, EXTRA2_DND) && !IS_IMMORTAL (ch))
	{
		send_to_char ("#y<#gMMS#y> #nYou have messaged a DND Imp.. Bad move..#n\n\r", ch);
		send_to_char ("#y<#gMMS#y> #nThe message is ignored.#n\n\r", ch);
		do_info (victim, poly);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("#y<#gMMS#y> #n$E can't hear you.#n", ch, 0, victim, TO_CHAR);
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
		sprintf (name, "A Private Number");

	sprintf (poly, "#y<#gMMS picture to %s#y:#n\r\n #g*#n$t#g*#y>#n", name);
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
		sprintf (name, "A Private Number");

	sprintf (poly, "#y<#gMMS picture from %s#y:#n\r\n #g*#n$t#g*#y>#n", name);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	return;
}

/* End SMS Photo Message Code */

/* Changeling Dream-tampering code */

 #define DREAM_OFF(ch) (IS_SET(ch->deaf, CHANNEL_CELL))
 #define DREAM_ON(ch)  !DREAMOFF(ch)

void spam_dream_syntax(CHAR_DATA *ch)
{
	send_to_char("Dreamweave commands:\n\r", ch);
	send_to_char("    dreamweave <player>\n\r", ch);
	send_to_char("    slumber\n\r", ch);
	send_to_char("    dream <message>\n\r", ch);
	send_to_char("    sever\n\r", ch);
}

void do_dreamweave(register CHAR_DATA * ch, register char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	register CHAR_DATA *victim;
	argument = one_argument(argument, arg);
	
    if (!IS_CHANGELING (ch)){
	send_to_char("#cOnly Changelings can invade the dreams of others.#n\n\r", ch);
	return;
	}
	
	if (get_disc (ch, DISC_DOMINATE) < 2)
	{
		send_to_char ("#cYou must have Dream at 2 to use Dreamweave.#n\n\r", ch);
		return;
	}
	
    else
	{
	if (arg[0] == '\0') 
	{
		spam_dream_syntax(ch);
		return;
	}
	if (IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char ("#cYou are unable to dream.#n\n\r", ch);
		return;
	}
	if (DREAM_OFF(ch))    
		do_dream(ch,"on");
	if (ch->inDream != NULL)
	{
		send_to_char("#cYou can't dreamweave while awake and distracted.#n\n\r", ch);
		return;
	}
	send_to_char("#cYour mind searches the intangible, navigating nightmares and dreams for a specific soul...#n\n\r", ch);
	if (   ((victim = get_char_world(ch, arg)) == NULL) 
		|| (!IS_NPC(victim) && victim->desc == NULL)
		|| (DREAM_OFF(victim))
		|| (victim == ch)
	   )
	{
		send_to_char("#cYou could not penetrate the dreams of your intended target.#n\n\r", ch);
		return;
	}
	if (victim->inDream != NULL)
	{
		send_to_char("#cYou could not penetrate the dreams of your intended target.#n\n\r", ch);
		return;
	}
	if (victim->dreamvibrate)
		send_to_char("#cYou're tired. Very tired. It wouldn't hurt to drift off into a peaceful '#Cslumber#c.'#n\n\r", victim);
	else
	{
		send_to_char("#cYou're tired. Very tired. It wouldn't hurt to drift off into a peaceful '#Cslumber#c.'#n\n\r", victim);
	}

	ch->inDream        = victim;
	victim->inDream    = ch;
	ch->dreamstate     = -1;  //-1 = changeling and not dreaming
	victim->dreamstate = 0;   // 0 = victim and not dreaming
	}
}

void do_slumber(register CHAR_DATA * ch, register char *argument)
{
	if (argument[0] != '\0')
	{
		spam_dream_syntax(ch);
		return;
	}
	if (DREAM_OFF(ch))
	{
		send_to_char("#cYou are unable to dream.#n\n\r", ch);
		return;
	}
	if (ch->inDream == NULL)
	{
		send_to_char("#cYou have been enjoying long, dreamless nights.#n\n\r", ch);
		return;
	}
	if (ch->inDream->inDream == NULL || ch->inDream->desc == NULL)
	{
		send_to_char("#cYou drift away and start to dream, but the image fractures and dissolves.#n\n\r", ch);
		ch->inDream->inDream = NULL;
		ch->inDream = NULL;
		return;
	}
	if (ch->dreamstate == -1)
	{
		send_to_char("#cYou can't affect the content of your own dreams.#n\n\r", ch);
		return;
	}
	if (ch->dreamstate == 1)
	{
		send_to_char("#cSomeone is already tampering with that target's dream.#n\n\r", ch);
		return;
	}
	ch->inDream->dreamstate = 1;
	ch->dreamstate          = 1;
	send_to_char("#cYou drift away into a heavy, dream-filled slumber... #C(#cUse \"#Cdream <text>#c\" to participate in your dream, and \"#Csever#c\" to abandon it. Wait for an echo to respond.#C)#n\n\r", ch);
	send_to_char("#cYou slip into your victim's dreamscape. #C(#cUse \"#Cdream <text>#c\" to illustrate their dream, and \"#Csever#c\" to abandon it. The victim can respond to your echo.#C)#n\n\r", ch->inDream);
}

void do_sever(register CHAR_DATA * ch, register char *argument)
{
	if (argument[0] != '\0')
	{
		spam_dream_syntax(ch);
		return;
	}
	if (DREAM_OFF(ch))
	{
		send_to_char("#cYou are unable to dream.#n\n\r", ch);
		return;
	}
	if (ch->inDream == NULL)
	{
		send_to_char("#cYou have been enjoying long, dreamless nights.#n\n\r", ch);
		return;
	}
	send_to_char("#cYou awaken with a start, dream-like images still vivid in your mind.#n\n\r", ch);
	send_to_char("#cYou awaken with a start, dream-like images still vivid in your mind.#n\n\r", ch->inDream);
	ch->inDream->inDream = NULL;
	ch->inDream          = NULL;
}

void do_dream(register CHAR_DATA * ch, register char *argument)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_SET(ch->act, PLR_SILENCE))
	{
		send_to_char ("#cYou are unable to affect the dream world.#n\n\r", ch);
		return;
	}
	if (!str_cmp(argument, "on"))
	{
		send_to_char("#cYour mind is open to invasive dreams.#n\n\r", ch);
		REMOVE_BIT(ch->deaf, CHANNEL_CELL);
		ch->dreamvibrate = FALSE;
		return;
	}
	if (!str_cmp(argument, "off"))
	{
		if (ch->inDream != NULL)  do_sever(ch, "");
		send_to_char("#cYour mind is closed to invasive dreams.#n\n\r", ch);
		SET_BIT(ch->deaf, CHANNEL_CELL);
		ch->dreamvibrate = FALSE;
		return;
	}
	if (!str_cmp(argument, "daydream"))
	{
		if (DREAM_OFF(ch)) do_dream(ch, "on");
		send_to_char("#cYour mind is open to invasive dreams.#n\n\r", ch);
		ch->dreamvibrate = TRUE;
		return;
	}
	if (argument[0] == '\0')
	{
		spam_dream_syntax(ch);
		return;
	}
	if (ch->inDream == NULL)
	{
		send_to_char("#cYou aren't connected to anyone in the dreamscape.#n\n\r", ch);
		return;
	}
	if (ch->inDream->inDream == NULL || ch->inDream->desc == NULL)
	{
		send_to_char("#cThe dream suddenly ends; you plummet over a long and unexpected fall, waking when you hit the ground.#n\n\r", ch);
		ch->inDream->inDream = NULL;
		ch->inDream = NULL;
		return;
	}
	if (ch->dreamstate == -1)
	{
		send_to_char("#cThey haven't drifted off to sleep yet.#n", ch);
		return;
	}
	if (ch->dreamstate == 0)
		do_slumber(ch, "");
	sprintf(buf, "#C<#c* %s *#C>#n", argument);
	do_rpose(ch, buf);
	send_to_char(buf, ch->inDream);
}

/* Geist Harassment Code */

void do_geist (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("#eYour thought didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#eYour thought didn't get through.#n\n\r", ch);
		return;
	}
	if (IS_EXTRA (ch, GAGGED))
	{
		send_to_char ("#eYour thought didn't get through.#n\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("#eSend an inner monologue message to which host?#n\n\r", ch);
		return;
	}

	/*
	 * Can send thoughts to Geists anywhere, but NPC's only in same room.
	 * -- Lea
	 */
	if ((victim = get_char_world (ch, arg)) == NULL || (IS_NPC (victim) && victim->in_room != ch->in_room))
	{
		send_to_char ("#eThey aren't here.#n\n\r", ch);
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
		sprintf (name, "your Host");

	sprintf (poly, "#c<#eA thought to %s#c>#e $t#n", name);
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
		sprintf (name, "your Geist");

	sprintf (poly, "#c<#eA thought from %s#c> #e$t#n", name);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
		victim->rgeist = ch;

	return;
}

/* End Geist Harassment Code */

/* Begin rgeist code */

void do_rgeist (CHAR_DATA * ch, char *argument)
{
	char poly[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;
	char name[MAX_STRING_LENGTH];
	char cname[MAX_STRING_LENGTH];

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("#eYour thought didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#eYour thought didn't get through.#n\n\r", ch);
		return;
	}
	if ((victim = ch->rgeist) == NULL)
	{
		send_to_char ("#eThey aren't here.#n\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("#e$E can't receive thoughts.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("#g$E is currently link dead.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("#e$E can't receive thoughts.#n", ch, 0, victim, TO_CHAR);
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
		sprintf (name, "your Geist");

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
		sprintf (cname, "your Geist");


	sprintf (poly, "#c<#eA thought to %s#c>#e $t#n", name);
	act (poly, ch, argument, victim, TO_CHAR);

	position = victim->position;
	victim->position = POS_STANDING;

	sprintf (poly, "#c<#eA thought from %s#c>#e $t#n", cname);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->rgeist = ch;

	return;
}

/* End rgeist code */

void do_tell (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int position;

	if (IS_NPC (ch) || IS_SET (ch->act, PLR_SILENCE))
	{
		send_to_char ("#RYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#RYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (IS_EXTRA (ch, GAGGED))
	{
		send_to_char ("#RYour message didn't get through.#n\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("#RTell whom what?#n\n\r", ch);
		return;
	}

	/*
	 * Can tell to PC's anywhere, but NPC's only in same room.
	 * -- Furey
	 */
	if ((victim = get_char_world (ch, arg)) == NULL || (IS_NPC (victim) && victim->in_room != ch->in_room))
	{
		send_to_char ("#RThey aren't here.#n\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("#R$E can't hear you.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("#R$E is currently link dead.#n", ch, 0, victim, TO_CHAR);
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
			act ("#R$E can't hear you.#n", ch, 0, victim, TO_CHAR);
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
		sprintf (name, "#RA Staff Member#n");

	sprintf (poly, "#RYou tell %s, \"$t\"#n", name);
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

	sprintf (poly, "#R%s tells you, \"$t\"#n", name);
	ADD_COLOUR (victim, poly, WHITE);
	act (poly, ch, argument, victim, TO_VICT);

	victim->position = position;
	victim->reply = ch;

	return;
}


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
	else if (ch->pcdata->language[0] == LANG_MANDARIN){
		strcpy(whisperedword, "mandarin");
		language2 = gsn_mandarin;
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
	else if (ch->pcdata->language[0] == LANG_FAE){
		strcpy(whisperedword, "fae");
		language2 = gsn_fae;
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
	else if (ch->pcdata->language[0] == LANG_CANTONESE){
		strcpy(whisperedword, "cantonese");
		language2 = gsn_cantonese;
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
		do_smoke1(ch,argument);
	
	sprintf (buf, "#eYou whisper #B'#e%s#B' #eto #G%s#e in #G%s#e. \n\r",argument, name2, whisperedword);
		send_to_char (buf, ch);
	if (victim->pcdata->learned[language2] < 75){
	sprintf (buf, "#G%s #ewhispers to you in a language that you don't understand.\n\r", name);
		send_to_char (buf, victim);
	}
	else{
	sprintf (buf, "#G%s#e whispers #B'#e%s#B'#e to you in #G%s#e.\n\r",name ,argument,whisperedword );
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
			 		
			listen_success = diceroll (to, ATTRIB_WIT, PHYSICAL, ATTRIB_APP, listendiff);
			
			if (listen_success > 3 || IS_STORYTELLER(to)){
				if (to->pcdata->learned[language2] < 75){
				sprintf(buf, "#eYou overhear #G%s #ewhisper to #G%s#e but you don't understand the language.#n\n\r", name, name2);
				send_to_char (buf, to);	
			}
			else{
				sprintf(buf, "#eYou overhear #G%s #ewhisper, #B'#e%s#B'#e to #G%s #ein #G%s#e.#n\n\r", name, argument, name2, whisperedword);
				send_to_char (buf, to);		
				}
			}
			else
			{
				sprintf( buf, "#G%s#e whispers something to #G%s.#n", name, name2 );
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
		send_to_char ("#RYour message didn't get through.#n\n\r", ch);
		return;
	}
	if (ch->in_room != NULL && IS_SET (ch->in_room->added_flags, ROOM2_SILENCE))
	{
		send_to_char ("#RYour message didn't get through.#n\n\r", ch);
		return;
	}
	if ((victim = ch->reply) == NULL)
	{
		send_to_char ("#RThey aren't here.#n\n\r", ch);
		return;
	}

	if (!IS_IMMORTAL (ch) && !IS_AWAKE (victim))
	{
		act ("#R$E can't hear you.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (!IS_NPC (victim) && victim->desc == NULL)
	{
		act ("#R$E is currently link dead.#n", ch, 0, victim, TO_CHAR);
		return;
	}

	if (IS_SET (victim->deaf, CHANNEL_TELL))
	{
		if (IS_NPC (victim) || IS_NPC (ch) || strlen (victim->pcdata->marriage) < 2 || str_cmp (ch->name, victim->pcdata->marriage))
		{
			act ("#R$E can't hear you.#n", ch, 0, victim, TO_CHAR);
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
		sprintf (name, "#RA Staff Member#n");

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


	sprintf (poly, "#RYou reply to %s, \"$t\"#n", name);
	act (poly, ch, argument, victim, TO_CHAR);

	position = victim->position;
	victim->position = POS_STANDING;

	sprintf (poly, "#R%s replies to you, \"$t\"#n", cname);
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
		send_to_char ("#ePose what?#n\n\r", ch);
		return;
	}

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);
	if (strchr(argument, '"') != NULL)
		argument = colorPose(argument);

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
		sprintf (poly, "#G%s %s#n\n\r", name, buf);
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
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);
	
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
		sprintf (poly, "#G%s %s#n\n\r", name, buf);
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
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);

	if (IS_NPC (ch))
	{
		if (ch->wizard != NULL)
		{
			if (IS_AFFECTED (ch, AFF_ETHEREAL))
				return;
		}

		act ("$n has left the game.", ch, NULL, NULL, TO_ROOM);
		//if (strlen(ch->pcdata->roomdesc) > 1)
		//	sprintf (buf, "#w%s#e has left Innocence Lost.#n", ch->name);
		//else~
		//	sprintf (buf, "#wA new character#e has left Innocence Lost.");
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
 	
 if (IS_WEREWOLF(ch)){
	char_success = diceroll(ch, ATTRIB_WIT, MENTAL, KNO_INVESTIGATION, 6);
	
	if (char_success < 1){
		send_to_char("#p                      Fantasy is silver and scarlet, indigo and azure,\n\r", ch);
		send_to_char("#p                      obsidian veined with gold and lapis lazuli.\n\r", ch);
		send_to_char("#p                      Reality is plywood and plastic, done up in mud brown\n\r", ch);
		send_to_char("#p                      and olive drab.\n\r", ch);
		send_to_char("#p                      \n\r", ch);
		send_to_char("#p                      Fantasy tastes of habaneros and honey, cinnamon and\n\r", ch);
		send_to_char("#p                      cloves, rare red meat and wines as sweet as summer.\n\r", ch);
		send_to_char("#p                      Reality is beans and tofu, and ashes at the end.\n\r", ch);
		send_to_char("#r                      --George R.R. Martin\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else if ((char_success < 3) && (char_success > 1)){
		send_to_char("#p                      In a world where billions believe their deity\n\r", ch);
		send_to_char("#p                      conceived a mortal child with a virgin human,\n\r", ch);
		send_to_char("#p                      it's stunning how little imagination\n\r", ch);
		send_to_char("#p                      most people display.\n\r", ch);
		send_to_char("#r                      --Chuck Palahniuk\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else if ((char_success > 3) && (char_success < 3)){
		send_to_char("#p                      My mama told me don't trust no white boy, even a pretty one.\n\r", ch);
		send_to_char("#p                      even a pretty one.\n\r", ch);
		send_to_char("#p                      I'm thinkin' a pretty white boy with wings explodin' up from\n\r", ch);
		send_to_char("#p                      the ground in a mess of blood and ugly-ass bird things is\n\r", ch);
		send_to_char("#p                      double trouble.\n\r", ch);
		send_to_char("#r                      --P.C. Cast\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else{
		send_to_char("#r                      I woke up as the sun was reddening;\n\r", ch);
		send_to_char("#r                      and that was the one distinct time in my life,\n\r", ch);
		send_to_char("#r                      the strangest moment of all, when I didn't know who I was -\n\r", ch);
		send_to_char("#r                      I was far away from home, haunted and tired with travel,\n\r", ch);
		send_to_char("#r                      in a cheap hotel room I'd never seen,\n\r", ch);
		send_to_char("#r                      hearing the hiss of steam outside,\n\r", ch);
		send_to_char("#r                      and the creak of the old wood of the hotel,\n\r", ch);
		send_to_char("#r                      and footsteps upstairs, and all the sad sounds,\n\r", ch);
		send_to_char("#r                      and I looked at the cracked high ceiling\n\r", ch);
		send_to_char("#r                      and really didn't know who I was for about fifteen strange seconds.\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		send_to_char("#r                      I wasn't scared; I was just somebody else, some stranger,\n\r", ch);
		send_to_char("#r                      and my whole life was a haunted life, the life of a ghost.\n\r", ch);
		send_to_char("#r                      --Jack Kerouac, On the Road\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	}
	
if ((IS_GHOUL(ch)) || (!strcmp(ch->bloodline, "Ghoul"))){
	char_success = diceroll(ch, ATTRIB_MAN, SOCIAL, SKI_SUBTERFUGE, 6);
	
	if (char_success <= 2){
		send_to_char("#r                      It seems so easy.\n\r", ch); 
		send_to_char("#r                      Just a sip, a taste, and you're better than all the rest.\n\r", ch); 
		send_to_char("#r                      You can knock your enemies through walls with a shove.\n\r", ch); 
		send_to_char("#r                      You can suck up a shotgun to the gut. You can have all the\n\r", ch); 
		send_to_char("#r                      night has to offer without sacrificing your body and soul to it.\n\r", ch); 
		send_to_char("#r                      The deal is too good to be true. Yeah, that's what Faust said.\n\r", ch);
		send_to_char("#r                      And now the Damned want their due.\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else{
		send_to_char("#r                      With his long sharp nails he opened a vein in his breast.\n\r", ch); 
		send_to_char("#r                      When the blood began to spurt out, he took my hands in one\n\r", ch); 
		send_to_char("#r                      of his, holding them tight and with the other ceased my\n\r", ch); 
		send_to_char("#r                      neck and pressed my mouth to the wound so that I must either\n\r", ch); 
		send_to_char("#r                      suffocate or swallow...\n\r", ch); 
		send_to_char("#r                      Some of the... Oh my god... my god. What have I done?\n\r", ch);
		send_to_char("#r                      --Bram Stoker, Dracula\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	}

if ((IS_VAMPIRE(ch)) && (strcmp(ch->bloodline, "Ghoul"))){
	char_success = diceroll(ch, ATTRIB_MAN, SOCIAL, SKI_SUBTERFUGE, 6);
	
	if (char_success < 2){
		send_to_char("#r                      I crave your mouth, your voice, your hair.\n\r", ch);
		send_to_char("#r                      Silent and starving, I prowl through the streets.\n\r", ch);
		send_to_char("#r                      Bread does not nourish me, dawn disrupts me. All day\n\r", ch);
		send_to_char("#r                      I hunt for the liquid measure of your steps.\n\r", ch);
		send_to_char("#r                      --Pablo Neruda\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else if ((char_success > 2) && (char_success < 4)){
		send_to_char("#r                      There was a deliberate voluptuousness that was both\n\r", ch);
		send_to_char("#r                      thrilling and repulsive. And as she arched her neck\n\r", ch);
		send_to_char("#r                      she actually licked her lips like an animal till I\n\r", ch);
		send_to_char("#r                      could see in the moonlight the moisture. Then lapped\n\r", ch);
		send_to_char("#r                      the white, sharp teeth. Lower and lower went her head.\n\r", ch);
		send_to_char("#r                      I closed my eyes in a languorous ecstasy and waited.\n\r", ch);
		send_to_char("#r                      --Bram Stoker, Dracula\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else{
		send_to_char("#r                      Listen to them - children of the night.\n\r", ch);
		send_to_char("#r                      What music they make.\n\r", ch);
		send_to_char("#r                      --Bram Stoker, Dracula#n\n\r\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	}
	
if (IS_CHANGELING (ch)){
	char_success = diceroll(ch, ATTRIB_STR, PHYSICAL, TAL_BRAWL, 6);
	
	if (char_success < 2){
		send_to_char("#g                      I'm going to go home.\n\r", ch);
		send_to_char("#g                      Everything is going to be\n\r", ch);
		send_to_char("#g                      normal again. Boring again.\n\r", ch);
		send_to_char("#g                      Wonderful again.\n\r", ch);
		send_to_char("#p                      --Neil Gaiman, Neverwhere#n\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else if (char_success == 2){
		send_to_char("#g                      The ErlKing has done me the cruelest harm!\n\r", ch);
		send_to_char("#g                      The father shudders, his ride is wild\n\r", ch);
		send_to_char("#g                      In his arms he's holding the shivering child\n\r", ch);
		send_to_char("#g                      He reaches home with toil and dread.\n\r", ch);
		send_to_char("#g                      In his arms, the child was dead.\n\r", ch);
		send_to_char("#p                      --Johann Wolfgang von Goethe#n\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else{
		send_to_char("#g                      One fatal morning I went out,\n\r", ch);
		send_to_char("#g                      Dreading no injury,\n\r", ch);
		send_to_char("#g                      And thinking long, fell soon asleep,\n\r", ch);
		send_to_char("#g                      Beneath an apple tree.\n\r", ch);
		send_to_char("#g                      Then by it came the Faerie Queen,\n\r", ch);
		send_to_char("#g                      And laid her hand on me;\n\r", ch);
		send_to_char("#g                      And from that time since ever I mind.\n\r", ch);
		send_to_char("#g                      I've been in her company.\n\r", ch);
		send_to_char("#p                      --Francis James Child#n\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	}
	
if (IS_GEIST (ch)){
	char_success = diceroll(ch, ATTRIB_STR, PHYSICAL, TAL_BRAWL, 6);
	
	if (char_success < 2){
		send_to_char("#n                      Life is for the living.\n\r", ch);
		send_to_char("#n                      Death is for the dead.\n\r", ch);
		send_to_char("#n                      Let life be like music.\n\r", ch);
		send_to_char("#n                      And death a note unsaid.\n\r", ch);
		send_to_char("#b                      --Langston Hughes#n\n\r", ch);
		send_to_char("#n                      \n\r", ch);
		}
	else if (char_success == 2){
		send_to_char("#n                      When he shall die,\n\r", ch);
		send_to_char("#n                      Take him and cut him out in little stars,\n\r", ch);
		send_to_char("#n                      And he will make the face of heaven so fine\n\r", ch);
		send_to_char("#n                      That all the world will be in love with night\n\r", ch);
		send_to_char("#n                      And pay no worship to the garish sun.\n\r", ch);
		send_to_char("#b                      --William Shakespeare#n\n\r", ch);
		send_to_char("#n                      \n\r", ch);
		}
	else{
		send_to_char("#n                      A girl calls and asks,\n\r", ch);
		send_to_char("#n                      'Does it hurt very much to die?'\n\r", ch);
		send_to_char("#n                      'Well, sweetheart,' I tell her,\n\r", ch);
		send_to_char("#n                      'yes, but it hurts a lot more\n\r", ch);
		send_to_char("#n                      to keep living.'\n\r", ch);
		send_to_char("#b                      --Chuck Palahniuk#n\n\r", ch);
		send_to_char("#n                      \n\r", ch);
		}
	}

if ((!IS_VAMPIRE(ch)) && (!IS_GHOUL (ch)) && (!IS_GEIST (ch)) && (!IS_WEREWOLF (ch)) && (!IS_CHANGELING(ch))){
	char_success = diceroll(ch, ATTRIB_CHA, SOCIAL, SKI_EMPATHY, 6);
	
	if (char_success < 2){
		send_to_char("#C                      Nobody realizes that some people expend tremendous energy\n\r", ch);
		send_to_char("#C                      merely to be normal.\n\r", ch);
		send_to_char("#c                      --Albert Camus\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	else{
		send_to_char("#C                      Life is to be lived, not controlled; and humanity is won\n\r", ch);
		send_to_char("#C                      by continuing to play in face of certain defeat.\n\r", ch);
		send_to_char("#c                      --Ralph Ellison, Invisible Man\n\r", ch);
		send_to_char("#r                      \n\r", ch);
		}
	}
	
	 /*End new Code */
 	//do_help( ch, "LOGOUT" );

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
		act ("$n slowly fades out of existence.", ch, NULL, NULL, TO_ROOM);
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
		//	sprintf (buf, "#w%s#e has left Innocence Lost.#n", ch->name);
		//else
			//sprintf (buf, "#wA new person#e has left Innocence Lost.#n");
		//do_info (ch, buf);
	}
	extract_char (ch, TRUE);
	return;
}



void do_save (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	save_char_obj (ch);
	send_to_char ("#wNew data saved to your character!#n\n\r", ch);
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
		send_to_char ("Follow who?\n\r", ch);
		return;
	}

	if (((victim = get_char_room (ch, arg)) == NULL) && ((victim = get_char_room_from_alias(ch, arg)) == NULL) && ((victim = get_char_room_from_desc(ch, arg)) == NULL))
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

/*	if (IS_AFFECTED (ch, AFF_CHARM) && ch->master != NULL)
	{
		act ("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
		return;
	}*/

	if ( ch->vehicle != NULL ) {
		send_to_char( "You cannot follow someone while riding in a vehicle!\n\r", ch );
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

	if (can_see (master, ch) && !IS_MORE3(ch, MORE3_OBFUS3))
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

	if ((can_see (ch->master, ch)) && !IS_MORE3(ch, MORE3_OBFUS3))
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
				act ("$n slowly fades out of existence.", fch, NULL, NULL, TO_ROOM);
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
		act ("The lift shudders suddenly.", ch, NULL, NULL, TO_ROOM);
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
		act ("The lift shudders suddenly.", ch, NULL, NULL, TO_CHAR);
	if (!same_floor (ch, to_room))
		act ("The lift shudders suddenly.", ch, NULL, NULL, TO_ROOM);
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
		if (obj->item_type != ITEM_VEHICLE)
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
		if (obj->item_type != ITEM_VEHICLE)
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
		if (obj->item_type != ITEM_VEHICLE)
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
		if (obj->item_type != ITEM_VEHICLE)
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
		if (obj->item_type != ITEM_VEHICLE)
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
			"Mandarin", "French", "Latin", "Spanish", "Italian",
			"Gaelic", "Hebrew", "Japanese", "Greek", "Indonesian",
			"Slavic", "Fae", "Korean", "Cantonese", "Tagalog", "Hindu"};

		const sh_int lang_sn[21] = { gsn_english, gsn_german, gsn_vietnamese, gsn_russian, gsn_arabic,
			gsn_mandarin, gsn_french, gsn_latin, gsn_spanish,
			gsn_italian, gsn_gaelic, gsn_hebrew, gsn_japanese, gsn_greek, gsn_indonesian,
			gsn_slavic, gsn_fae, gsn_korean, gsn_cantonese, gsn_tagalog, gsn_hindu };

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
	else if (!str_cmp (arg, "mandarin") && ch->pcdata->learned[gsn_mandarin] > 0)
	{
		if (ch->pcdata->language[0] == LANG_MANDARIN)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_MANDARIN;
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
	else if (!str_cmp (arg, "fae") && ch->pcdata->learned[gsn_fae] > 0)
	{
		if (ch->pcdata->language[0] == LANG_FAE)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_FAE;
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
	else if (!str_cmp (arg, "cantonese") && ch->pcdata->learned[gsn_cantonese] > 0)
	{
		if (ch->pcdata->language[0] == LANG_CANTONESE)
		{
			send_to_char ("But you are already speaking that language!\n\r", ch);
			return;
		}
		ch->pcdata->language[0] = LANG_CANTONESE;
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
			"Mandarin", "French", "Latin", "Spanish", "Italian",
			"Gaelic", "Hebrew", "Japanese", "Greek", "Indonesian",
			"Slavic", "Fae", "Korean", "Cantonese", "Tagalog", "Hindu"};

	const sh_int lang_sn[21] = { gsn_english, gsn_german, gsn_vietnamese, gsn_russian, gsn_arabic,
			gsn_mandarin, gsn_french, gsn_latin, gsn_spanish,
			gsn_italian, gsn_gaelic, gsn_hebrew, gsn_japanese, gsn_greek, gsn_indonesian,
			gsn_slavic, gsn_fae, gsn_korean, gsn_cantonese, gsn_tagalog, gsn_hindu };

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

	numberoflanguages = ch->abilities[MENTAL][KNO_MEDICINE] + 1;

	if (IS_CHANGELING (ch) && ch->pcdata->learned[gsn_fae] < 100)
		ch->pcdata->learned[gsn_fae] = 100;

	if (arg[0] == '\0')
	{
		int x, sn;
		const char *lang[21] = { "English", "German", "Vietnamese", "Russian", "Arabic",
			"Mandarin", "French", "Latin", "Spanish", "Italian",
			"Gaelic", "Hebrew", "Japanese", "Greek", "Indonesian",
			"Slavic", "Fae", "Korean", "Cantonese", "Tagalog", "Hindu"};

		const sh_int lang_sn[21] = { gsn_english, gsn_german, gsn_vietnamese, gsn_russian, gsn_arabic,
			gsn_mandarin, gsn_french, gsn_latin, gsn_spanish,
			gsn_italian, gsn_gaelic, gsn_hebrew, gsn_japanese, gsn_greek, gsn_indonesian,
			gsn_slavic, gsn_fae, gsn_korean, gsn_cantonese, gsn_tagalog, gsn_hindu };
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
	else if (!str_cmp (arg, "mandarin") && ch->pcdata->learned[gsn_mandarin] > 0)
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
		else if (!str_cmp (arg, "cantonese") && ch->pcdata->learned[gsn_cantonese] > 0)
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
	
	
	
	//if number of languages known(- latin) >= medicine - latin
//       send message
	if (ch->pcdata->learned[gsn_arabic] > 0)
		languageslearned++;
	if (ch->pcdata->learned[gsn_mandarin] > 0)
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
	if (ch->pcdata->learned[gsn_cantonese] > 0)
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


	// gives you 75% in the language
	if (!str_cmp (arg, "arabic"))
	{
		ch->pcdata->learned[gsn_arabic] = 75;
		send_to_char ("You start learning the language\n\r", ch);
	}
	else if (!str_cmp (arg, "mandarin"))
	{
		ch->pcdata->learned[gsn_mandarin] = 75;
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
	else if (!str_cmp (arg, "cantonese"))
	{
		ch->pcdata->learned[gsn_cantonese] = 75;
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
		strcat (buf, "Greek, Slavic, Mandarin, Japanese Indonesian,\n\r");
		strcat (buf, "Korean, Vietnamese, Cantonese, Tagalog and Hindu.\n\r");
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


/* Modified/expanded rpose and functions by julius */

CHAR_DATA* getPlayerFromTargetName(CHAR_DATA * ch, char *argument) {

	DESCRIPTOR_DATA *d;

//Loop through the list of players connected
	for (d = descriptor_list; d != NULL; d = d->next) {

//If the player's connection is in a playing state and they share the same room
		if (d->connected == CON_PLAYING
				&& d->character->in_room == ch->in_room) {

// If player's character's name matches the argument passed in
			if (!str_cmp(d->character->name, argument)) {
//Return that character as the match 
				return d->character;
			}
		}
	}
	return NULL;
}

/*void do_rpose(CHAR_DATA * ch, char *argument) {
	CHAR_DATA* target;
	char* poseCopy;
	char* poseCopyWordPtr;
	char buf[MAX_STRING_LENGTH];

	char poseCopyWord[MAX_INPUT_LENGTH];

//Check for an actual rpose 
	if (argument[0] == '\0') {
		send_to_char("#eRpose what?\n\r", ch);
		return;
	}

//Copy the full pose text into the poseCopy variable 
	strcpy(poseCopy, argument);

//Read every word of the poseCopy variable 
	while ((poseCopy = one_argument(poseCopy, poseCopyWord)) != NULL) {

		sprintf(buf, "Target of %s", poseCopyWord);

		send_to_char(buf, ch);

		 poseCopyWordPtr = &poseCopyWord[0]; 

		 //Find the target
		 
		 target = getPlayerFromTargetName(ch, poseCopyWordPtr); 
		 if ( target == NULL )
		 {
		 sprintf(buf, "Target of %s matches no character.", poseCopyWord);
		 send_to_char(buf, ch); 
		 }
		 else
		 {
		 sprintf(buf, "Target of %s matches character %s.", poseCopyWord, target->name); 
		 send_to_char(buf, ch); 

		 }
	}
}*/

void do_rpose (CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;

	if (argument[0] == '\0')
	{
		send_to_char ("#eRPose what?\n\r", ch);
		return;
	}
		
	if (strchr(argument, '"') != NULL)
		argument = colorPose(argument);

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


// RPecho for convenience's sake -- Merry/Muse 

void do_rpecho (CHAR_DATA * ch, char *argument)
{
	DESCRIPTOR_DATA *d;

	if (argument[0] == '\0')
	{
		send_to_char ("#eRPecho what?\n\r", ch);
		return;
	}
		
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

// RPecho end


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

/*void do_vote (CHAR_DATA * ch, char *argument)
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
	
}*/

/*void do_givevote(CHAR_DATA * ch, char *argument)
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
}*/

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
				tempString[x + offset + 2] = 'e';
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
		send_to_char ("#eHuh?#n\n\r", ch);
		return;
	}
	if (arg[0] == '\0')
	{
		send_to_char ("#eWhose request are you handling?#n\n\r", ch);
		return;
	}

	if ((victim = get_char_finger (ch, arg)) == NULL)
	{
		send_to_char ("#eThey aren't here.#n\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char ("#eYou're an immortal, and that's YOUR request. No.#n\n\r", ch);
		return;
	}
	
	sprintf(buf, "I'm now handling %s's request.", victim->name);
	do_story(ch, buf);
	send_to_char("#GA Storyteller is now handling your request#n\n\r", victim);
	
}

void do_nuketell (CHAR_DATA * ch, char *argument)
{
	
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	
	one_argument (argument, arg);
	
	if (arg[0] == '\0')
	{
		send_to_char ("#eYou have to choose whose reply to nuke.#n\n\r", ch);
		return;
	}
	
	if ((victim = get_char_finger (ch, arg)) == NULL)
	{
		send_to_char ("#eThey aren't here.#n\n\r", ch);
		return;
	}	
	
	victim->reply = NULL;
	send_to_char("#eYou've nuked their tell.#n\n\r", ch);

}

void do_lolfix (CHAR_DATA * ch, char *argument)
{
	ch->pcdata->merits = (char **)malloc(sizeof(char *) * 10);
	do_save(ch, "");
	send_to_char("#Rl#ro#ol #yf#Gi#gx#be#Bd#P.\n\r", ch);
}

void do_shout (CHAR_DATA * ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    WAIT_STATE (ch, 12);

    act ("You shout outloud, '#e$T#n'", ch, NULL, argument, TO_CHAR);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if (d->connected == CON_PLAYING &&
            d->character != ch)
        {
            act ("$n can be heard shouting, '#e$t#n'", ch, argument, d->character, TO_VICT);
        }
    }

    return;
}

void do_yell( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_YELL, "yell" );
    return;
}

void do_semote (CHAR_DATA * ch, char * argument)  
{  
    char output[MSL];  
    int npos = 0, pos = 0;  
    bool fSay = FALSE, addname = FALSE;  
   
    if (argument[0] == '\0')  
    {  
        send_to_char ("Syntax: rpecho/rpose/semote <text>\n\r", ch);  
        return;  
    }  
   
//    if (strstr (argument, ch->name) == NULL)  
//		addname = TRUE;  
   
    if (strstr (argument, "#") != NULL)  
    {  
        send_to_char ("Please do not use color codes in your semotes.\n\r", ch);  
        return;  
    }  
   
    while (*argument != '\0')  
    {  
        if (npos > (MSL - 4))  
            break;  
   
        switch(*argument)  
        {  
            default:  
                output[npos++] = *argument++;  
                break;  
            case '@':  
                for (pos = 0; ch->name[pos] != '\0'; pos++)  
                    output[npos++] = ch->name[pos];  
                argument++;  
                break;  
            case '\"':  
                if (!fSay)  
                {  
                    fSay = TRUE;  
                    output[npos++] = '\"';  
                    output[npos++] = '#';  
                    output[npos++] = 'e';  
					argument++;  
                    break;  
                }  
                else  
                {  
                    fSay = FALSE;  
                    output[npos++] = '#';  
                    output[npos++] = 'n';  
                    output[npos++] = '\"';  
					argument++;  
                    break;  
                }  
                break;  
        }  
    }  
    output[npos] = '\0';  
   
	if (addname)
	{  
		act ("#n$n $T #c[$n]#n", ch, NULL, output, TO_CHAR);
		act ("#n$n $T #c[$n]#n", ch, NULL, output, TO_ROOM);  
	}  
	else  
	{  
		act ("#n$T #c[$n]#n", ch, NULL, output, TO_CHAR);  
		act ("#n$T #c[$n]#n", ch, NULL, output, TO_ROOM);  
	}  
}  
