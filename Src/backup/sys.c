/***************************************************************************
 *  Old System Files.                                                      *
 *                                                                         *
 *  Please abide by the Diku/Merc/Godwars policys when using this code.    *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
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
#include <errno.h>
#include <assert.h>
#include <math.h>
#include <sys/dir.h>
#include "merc.h"

#define COLUMNS 5

// This file is primarily for Immortal commands

void do_givepower (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	EXTRA *extra;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char buf[MSL];
	
	buf[0] = '\0';
	
	argument = one_argument_retain_cap (argument, arg1);
	argument = one_argument_retain_cap (argument, arg2);
	argument = one_argument_retain_cap (argument, arg3);
	
	strcpy (arg4, argument);
	
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' || arg4[0] == '\0')
	{
 		send_to_char("Syntax: givepower <target> <\'catagory name\'> <\'field\'> <rating> \n\r", ch);
 		return;
	}

	if (!isdigit(arg4[0]))
	{
 		if ((arg4[0] == '-')  && !isdigit(arg4[1]))
 		{
 			send_to_char("Syntax: givepower <target> <\'catagory name\'> <\'field\'> <rating> \n\r", ch);
 			return;
 		}
	}

	if (!(victim = get_char_world(ch, arg1)))
	{
 		send_to_char("Cannot find target.\n\r", ch);
 		return;
	}

	if (IS_NPC(victim))
	{
 		send_to_char("Invalid target.\n\r", ch);
 		return;
	}

 	if (extra_free == NULL)
  	{
   		extra = alloc_perm (sizeof (*extra));
  	}
  	else
  	{
   		extra = extra_free;
   		extra_free = extra_free->next;
  	}

  	if (victim->pcdata->extra)
  		extra->prev = victim->pcdata->extra;
  	else
  		extra->prev = NULL;


  	extra->next = NULL;

  	extra->catagory = str_dup(arg2);
  	extra->field = str_dup(arg3);
  	extra->rating = atoi(arg4);
  	sprintf(buf, "\'%s\' \'%s\' %s", arg2, arg3, arg4);
  	extra->extra = str_dup(buf);

  	if (victim->pcdata->extra)
  		victim->pcdata->extra->next = extra;
  	
  	victim->pcdata->extra = extra;

  	sprintf(buf, "Cat: %s   Field: %s   Rating: %s\n\rSet on: %s\n\r", arg2, arg3, arg4, arg1);
  	send_to_char(buf, ch);  

	return;
}


void do_removepower (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
	EXTRA *extra;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	strcpy (arg3, argument);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
 		send_to_char("Syntax: removepower <target> <\'catagory name\'> <field> \n\r"
		"Remember to include color codes, and no \' \' grouping for the field.\n\r", ch);
 		return;
	}


	if (!(victim = get_char_world(ch, arg1)))
	{
 		send_to_char("Cannot find target.\n\r", ch);
 		return;
	}

	if (IS_NPC(victim))
	{
 		send_to_char("Invalid target.\n\r", ch);
 		return;
	}

	if (!victim->pcdata->extra)
	{
 		send_to_char("Target does not have any extra fields.\n\r", ch);
	 	return;
	}

	for (extra = victim->pcdata->extra; extra; extra = extra->prev)
	{
 		if (str_cmp(arg2, extra->catagory))
  			continue;
 		if (str_cmp(arg3, extra->field))
  			continue;

 		if (extra->prev)
  			extra->prev->next = extra->next;
 		if (extra->next)
  			extra->next->prev = extra->prev;

 		if (!extra->next)
 		{
  			if (extra->prev)
  				victim->pcdata->extra = extra->prev;
  			else
  				victim->pcdata->extra = NULL;
		}
 		
 		free_extra(extra);
 		send_to_char("Extra Field Removed.\n\r", ch);
 		
 		return;

	}
	
	send_to_char("No extra field of that name is available.\n\r", ch);
	
	return;
}


/* Re-Code by Kyrlin */
void do_pset (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int value;
	int i;
	int road = -1;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char *pwdnew;
	char *p;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);
	strcpy (arg4, argument);

	sprintf (buf, "%s: Pset %s", ch->name, argument);

	smash_tilde (argument);
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax: pset <victim> <area> <field> <value>\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("Area being one of:\n\r", ch);
		send_to_char (" Attribute, Class, Immune, Knowledge, Skill, Special, Spell,\n\r", ch);
		send_to_char ("  Talent, Weapon, quest, willpower, dps, beast, rank.\n\r", ch);
		send_to_char ("  diablerie, gnosis, truefaith, conviction, convictionbank, rage.\n\r", ch);
		send_to_char ("  tempglory, temphonor, tempwisdom, permglory,permhonor, permwisdom.\n\r", ch);
		send_to_char ("  daemon plane thaum, sectstatus.\n\r", ch);
		send_to_char ("  selfcontrol, courage, conscience, wisdom, faith, zeal.\n\r",ch);
		send_to_char ("  password quintessence maxquintessence.\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("Field being one of:\n\r", ch);
		send_to_char ("Attribute:  Strength, Dexterity, Stamina, Charisma,\n\r", ch);
		send_to_char ("            Manipulation, Appearance, Perception, Intelligence,\n\r", ch);
		send_to_char ("            Wits, All.\n\r", ch);
		send_to_char ("Class:      Vampire, Werewolf, Skin Dancer\n\r", ch);
		send_to_char ("Talent:     Expression, Alertness, Athletics, Brawl, Dodge, Empathy,\n\r", ch);
		send_to_char ("            Intimidation, Streetwise, Leadership, Subterfuge, All.\n\r", ch);
		send_to_char ("Skill:      Animalken, Archery, Crafts, Etiquette, Commerce,\n\r", ch);
		send_to_char ("            Melee, Performance, Ride, Stealth, Survival, All.\n\r", ch);
		send_to_char ("Knowledge:  Academics, Hearthwisdom, Investigation, Law,\n\r", ch);
		send_to_char ("            Linguistics, Medicine, Occult, Politics, Theology,\n\r", ch);
		send_to_char ("            Seneschal, All.\n\r", ch);
		send_to_char ("Special:    Storyteller, Shopkeeper\n\r", ch);
		send_to_char ("Thaum:      General, Dark, Koldun\n\r", ch);
		send_to_char ("Road:	   (name of the road)\n\r", ch);
		send_to_char ("Roadrank:   (number)\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = is_number (arg3) ? atoi (arg3) : -1;

	/*
	 * Set something.
	 */

	if (!str_cmp (arg2, "sectstatus")){
		
		if (IS_NPC (victim))
		{
			send_to_char( "Bug -- Attempting to set status on NPC\n\r", ch);
			return;
		}
	
		sprintf(buf, "#cPlayer's Sect Status set to #C%d#c.#n\n\r", value);
		send_to_char(buf, ch);
		victim->pcdata->sectstatus = value;
		return;

}
	
	
	
	
	if (!str_cmp (arg2, "quintessence"))
	{
		if (IS_NPC (victim))
		{
			send_to_char( "Not on NPC's.\n\r", ch);
			return;
		}

		if (value < 0 || value > 20)
		{
			send_to_char( "A little excessive?\n\r", ch);
			return;
		}

		if (IS_JUDGE(ch))
		{
			victim->quintessence = value;
			send_to_char( "Quintessence set.\n\r", ch);
		}
		else
			send_to_char("Huh?\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "maxquintessence"))
	{
		if (IS_NPC (victim))
		{
			send_to_char( "Not on NPC's.\n\r", ch);
			return;
		}

		if (value < 0 || value > 20)
		{
			send_to_char( "A little excessive?\n\r", ch);
			return;
		}

		if (IS_JUDGE(ch))
		{
			victim->max_quintessence = value;
			send_to_char( "Max Quintessence set.\n\r", ch);
		}
		else
			send_to_char("Huh?\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "courage"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 5)
		{
			send_to_char ("Courage range is 1 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_COURAGE] = value;
			send_to_char ("Ok, Courage set.\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "selfcontrol"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 5)
		{
			send_to_char ("Self Control range is 1 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_SELFCONTROL] = value;
			send_to_char ("Ok, Self Control set.\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "conscience"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 5)
		{
			send_to_char ("Conscience range is 1 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_CONSCIENCE] = value;
			send_to_char ("Ok, Conscience set.\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "wisdom"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (!IS_INQUISITOR(victim))
		{
			send_to_char("Only for Inquisitors.\n\r", ch);
		}

		if (value < 0 || value > 5)
		{
			send_to_char ("Wisdom range is 1 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_WISDOM] = value;
			send_to_char ("Ok, Wisdom set.\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "faith"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (!IS_INQUISITOR(victim))
		{
			send_to_char("Only for Inquisitors.\n\r", ch);
		}

		if (value < 0 || value > 5)
		{
			send_to_char ("Faith range is 1 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_FAITH] = value;
			send_to_char ("Ok, Faith set.\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "zeal"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (!IS_INQUISITOR(victim))
		{
			send_to_char("Only for Inquisitors.\n\r", ch);
		}

		if (value < 0 || value > 5)
		{
			send_to_char ("Zeal range is 1 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_ZEAL] = value;
			send_to_char ("Ok, Zeal set.\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "tempglory"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("Tmp Glory range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[TEMPGLORY] = value;
			send_to_char ("Ok, Temp glory set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "temphonor"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("Tmp honor range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[TEMPHONOR] = value;
			send_to_char ("Ok, Temp glory set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "tempwisdom"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("Tmp wisdom range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[TEMPWISDOM] = value;
			send_to_char ("Ok, Temp wisdom set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "permglory"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("Perm Glory range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[PERMGLORY] = value;
			send_to_char ("Ok, Perm glory set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "permwisdom"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("perm wisdom range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[PERMWISDOM] = value;
			send_to_char ("Ok, perm wisdom set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "permhonor"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("perm honor range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[PERMHONOR] = value;
			send_to_char ("Ok, perm honor set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "gnosis"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}


		if (value < 1 || value > 10)
		{
			send_to_char ("Gnosis range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->gnosis[1] = value;
			victim->pcdata->gnosis[0] = value;
			send_to_char ("Ok, Gnosis set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "rage"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}


		if (value < 1 || value > 10)
		{
			send_to_char ("rage range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->rage[1] = value;
			victim->pcdata->rage[0] = value;
			send_to_char ("Ok, rage set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "plane"))
	{
		if (value < 0 || value > 100)
		{
			send_to_char ("plane range is 0 to 100.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->plane = value;
			send_to_char ("Ok, rage set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "convictionbank"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (victim->class != CLASS_INQUISITOR)
		{
			send_to_char ("Only Inquisitors may have a conviction bank.", ch);
			return;
		}

		if (value < 0 || value > 99)
		{
			send_to_char ("Conviction Bank range is 0 to 99.\n\r", ch);
			return;
		}
		
		if (IS_JUDGE (ch))
		{
			victim->pcdata->conviction_bank = value;
			sprintf(buf, "Ok, Conviction Bank set to %d.\n\r", value);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("Huh???", ch);
			return;
		}
		return;
	}
	
	if (!str_cmp (arg2, "conviction"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (victim->class != CLASS_INQUISITOR)
		{
			send_to_char ("Only Inquisitors may have conviction.", ch);
			return;
		}

		if (value < 0 || value > 10)
		{
			send_to_char ("Conviction range is 0 to 10.\n\r", ch);
			return;
		}
		
		if (IS_JUDGE (ch))
		{
			victim->pcdata->conviction[CONVICTION_MAX] = value;
			sprintf(buf, "Ok, Conviction set to %d.\n\r", value);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("Huh???", ch);
			return;
		}
		return;
	}
	
	if (!str_cmp (arg2, "truefaith"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (victim->class != CLASS_NONE)
		{
			send_to_char ("Only mortals may have true faith.", ch);
			return;
		}

		if (value < 0 || value > 5)
		{
			send_to_char ("Faith range is 0 to 5.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->faith = value;
			sprintf(buf, "Ok, Faith set to %d.\n\r", value);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("Huh???", ch);
			return;
		}
		return;
	}

	if (!str_cmp (arg2, "diablerie"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 100)
		{
			send_to_char ("Diablerie range is 0 to 100.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			ch->pcdata->diableries[DIAB_EVER] = value;
			ch->pcdata->diableries[DIAB_TIME] = 0;
			send_to_char ("Diableries set.\n\r", ch);

		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "rank"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}


		if (value < 0 || value > 5)
		{
			send_to_char ("Rank range is from 0 to 5.\n\r", ch);
			return;
		}

		victim->pcdata->rank = value;
		return;
	}

	if (!str_cmp (arg2, "attribute"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char ("pset <victim> attribute <attribute> <value>.\n\r", ch);
			return;
		}

		argument = one_argument (argument, arg4);
		value = is_number (arg4) ? atoi (arg4) : -1;

		if (value < 0 || value > MAX_TRAINABLE_ATTRIB)
		{
			sprintf (buf, "Attribute range is 0 to %d.\n\r", MAX_TRAINABLE_ATTRIB);
			send_to_char (buf, ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			if (!str_cmp (arg3, "strength"))
			{
				victim->attributes[ATTRIB_STR] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "dexterity"))
			{
				victim->attributes[ATTRIB_DEX] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "stamina"))
			{
				victim->attributes[ATTRIB_STA] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "charisma"))
			{
				victim->attributes[ATTRIB_CHA] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "manipulation"))
			{
				victim->attributes[ATTRIB_MAN] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "appearance"))
			{
				victim->attributes[ATTRIB_APP] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "perception"))
			{
				victim->attributes[ATTRIB_PER] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "intelligence"))
			{
				victim->attributes[ATTRIB_INT] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "wits"))
			{
				victim->attributes[ATTRIB_WIT] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "all"))
			{
				victim->attributes[ATTRIB_STR] = value;
				victim->attributes[ATTRIB_DEX] = value;
				victim->attributes[ATTRIB_STA] = value;
				victim->attributes[ATTRIB_CHA] = value;
				victim->attributes[ATTRIB_MAN] = value;
				victim->attributes[ATTRIB_APP] = value;
				victim->attributes[ATTRIB_PER] = value;
				victim->attributes[ATTRIB_INT] = value;
				victim->attributes[ATTRIB_WIT] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			else
				send_to_char ("No such talent exists.\n\r", ch);
			return;
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "talent"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char ("pset <victim> talent <talent> <value>.\n\r", ch);
			return;
		}

		argument = one_argument (argument, arg4);
		value = is_number (arg4) ? atoi (arg4) : -1;

		if (value < 0 || value > MAX_TRAINABLE_ABILITY)
		{
			sprintf (buf, "Talent range is 0 to %d.\n\r", MAX_TRAINABLE_ABILITY);
			send_to_char (buf, ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			if (!str_cmp (arg3, "all"))
			{
				for (i = 0; i < 10; i++)
				{
					victim->abilities[TALENTS][i] = value;
					victim->pcdata->abilities_points[TALENTS][i] = ability_insight_levels[value];
				}
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			else
			{
				for (i = 0; i < 10; i++)
				{
					if (!str_cmp (arg3, ability_names[TALENTS][i][0]))
					{
						victim->abilities[TALENTS][i] = value;
						victim->pcdata->abilities_points[TALENTS][i] = ability_insight_levels[value];
						send_to_char ("Ok.\n\r", ch);
						return;
					}
				}
			}
			send_to_char ("No such talent exists.\n\r", ch);
			return;
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "skill"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char ("pset <victim> skill <skill> <value>.\n\r", ch);
			return;
		}

		argument = one_argument (argument, arg4);
		value = is_number (arg4) ? atoi (arg4) : -1;

		if (value < 0 || value > MAX_TRAINABLE_ABILITY)
		{
			sprintf (buf, "Skill range is 0 to %d.\n\r", MAX_TRAINABLE_ABILITY);
			send_to_char (buf, ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			if (!str_cmp (arg3, "all"))
			{
				for (i = 0; i < 10; i++)
				{
					victim->abilities[SKILLS][i] = value;
					victim->pcdata->abilities_points[SKILLS][i] = ability_insight_levels[value];

				}
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			else
			{
				for (i = 0; i < 10; i++)
				{
					if (!str_cmp (arg3, ability_names[SKILLS][i][0]))
					{
						victim->abilities[SKILLS][i] = value;
						victim->pcdata->abilities_points[SKILLS][i] = ability_insight_levels[value];
						send_to_char ("Ok.\n\r", ch);
						return;
					}
				}
			}
			send_to_char ("No such skill exists.\n\r", ch);
			return;
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "knowledge"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char ("pset <victim> knowledge <knowledge> <value>.\n\r", ch);
			return;
		}

		argument = one_argument (argument, arg4);
		value = is_number (arg4) ? atoi (arg4) : -1;

		if (value < 0 || value > MAX_TRAINABLE_ABILITY)
		{
			sprintf (buf, "Knowledge range is 0 to %d.\n\r", MAX_TRAINABLE_ABILITY);
			send_to_char (buf, ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			if (!str_cmp (arg3, "all"))
			{
				for (i = 0; i < 10; i++)
				{
					victim->abilities[KNOWLEDGES][i] = value;
					victim->pcdata->abilities_points[KNOWLEDGES][i] = ability_insight_levels[value];
				}
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			else
			{
				for (i = 0; i < 10; i++)
				{
					if (!str_cmp (arg3, ability_names[KNOWLEDGES][i][0]))
					{
						victim->abilities[KNOWLEDGES][i] = value;
						victim->pcdata->abilities_points[KNOWLEDGES][i] = ability_insight_levels[value];
						send_to_char ("Ok.\n\r", ch);
						return;
					}
				}
			}
			send_to_char ("No such knowledge exists.\n\r", ch);
			return;
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "spell"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char ("pset <victim> spell <colour> <value>.\n\r", ch);
			return;
		}

		argument = one_argument (argument, arg4);
		value = is_number (arg4) ? atoi (arg4) : -1;

		if (value < 0 || value > 200)
		{
			send_to_char ("Spell range is 0 to 200.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			if (!str_cmp (arg3, "purple"))
			{
				victim->spl[PURPLE_MAGIC] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "red"))
			{
				victim->spl[RED_MAGIC] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "blue"))
			{
				victim->spl[BLUE_MAGIC] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "green"))
			{
				victim->spl[GREEN_MAGIC] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "yellow"))
			{
				victim->spl[YELLOW_MAGIC] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			if (!str_cmp (arg3, "all"))
			{
				victim->spl[PURPLE_MAGIC] = value;
				victim->spl[RED_MAGIC] = value;
				victim->spl[BLUE_MAGIC] = value;
				victim->spl[GREEN_MAGIC] = value;
				victim->spl[YELLOW_MAGIC] = value;
				send_to_char ("Ok.\n\r", ch);
				return;
			}
			else
				send_to_char ("No such spell colour exists.\n\r", ch);
			return;
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "class"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (victim->class != CLASS_NONE)
		{
			send_to_char ("They already have a class.\n\r", ch);
			return;
		}

		if (victim->level < LEVEL_AVATAR)
		{
			send_to_char ("You can only do this to avatars.\n\r", ch);
			return;
		}

		if (ch->level != MAX_LEVEL)
		{
			send_to_char ("Sorry, no can do...\n\r", ch);
			return;
		}

		if (!str_cmp (arg3, "vampire"))
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
		else if (!str_cmp (arg3, "werewolf"))
		{
			victim->class = CLASS_WEREWOLF;
			victim->vampgen = 1;
			victim->pcdata->cland[0] = -1;
			victim->pcdata->cland[1] = -1;
			victim->pcdata->cland[2] = -1;
			send_to_char ("Ok.\n\r", ch);
			send_to_char ("You are now the Master Werewolf.\n\r", victim);
		}
		else
			do_pset (ch, "");
		return;
	}

	if (!str_cmp (arg2, "special"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC'S.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			if (!str_cmp (arg3, "storyteller"))
			{
				if (IS_SET (victim->act, PLR_STORYTELLER))
				{
					send_to_char ("You remove their Storyteller flag.\n\r", ch);
					send_to_char ("You are no longer a Storyteller.\n\r", victim);
					REMOVE_BIT (victim->act, PLR_STORYTELLER);
				}
				else
				{
					send_to_char ("You add a Storyteller flag to them.\n\r", ch);
					send_to_char ("Youy are now a Storyteller.\n\r", victim);
					SET_BIT (victim->act, PLR_STORYTELLER);
				}
				return;
			}
			else if (!str_cmp (arg3, "shopkeeper"))
			{
				if (IS_EXTRA (victim, EXTRA_SHOPKEEPER))
				{
					send_to_char ("You remove their Storyteller flag.\n\r", ch);
					send_to_char ("You are no longer a Storyteller.\n\r", victim);
					REMOVE_BIT (victim->extra, EXTRA_SHOPKEEPER);
				}
				else
				{
					send_to_char ("You add a Storyteller flag to them.\n\r", ch);
					send_to_char ("Youy are now a Storyteller.\n\r", victim);
					SET_BIT (victim->extra, EXTRA_SHOPKEEPER);
				}
				return;
			}

			else
			{
				send_to_char ("Sorry, no can do...\n\r", ch);
				return;
			}
		}
	}

	if (!str_cmp (arg2, "thaum"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC'S.\n\r", ch);
			return;
		}
			if (!str_cmp (arg3, "general"))
			{
				if (IS_SET (ch->pcdata->thaum_type, THAUM_TYPE_GENERAL))
				{
					send_to_char ("You remove their general thaumaturgy flag.\n\r", ch);
					send_to_char ("You no longer know general thaumaturgy.\n\r", victim);
					REMOVE_BIT (ch->pcdata->thaum_type, THAUM_TYPE_GENERAL);
				}
				else
				{
					send_to_char ("You add a general thaumaturgy flag to them.\n\r", ch);
					send_to_char ("You now know general thaumaturgy.\n\r", victim);
					SET_BIT (ch->pcdata->thaum_type, THAUM_TYPE_GENERAL);
				}
				return;
			}
			else if (!str_cmp (arg3, "dark"))
			{
				if (IS_SET(ch->pcdata->thaum_type, THAUM_TYPE_DARK))
				{
					send_to_char ("You remove their dark thaumaturgy flag.\n\r", ch);
					send_to_char ("You no longer know dark thaumaturgy.\n\r", victim);
					REMOVE_BIT (ch->pcdata->thaum_type, THAUM_TYPE_DARK);
				}
				else
				{
					send_to_char ("You add a dark thaumaturgy flag to them.\n\r", ch);
					send_to_char ("You now know dark thaumaturgy.\n\r", victim);
					SET_BIT (ch->pcdata->thaum_type, THAUM_TYPE_DARK);
				}
				return;
			}

			else if (!str_cmp (arg3, "koldun"))
			{
				if (IS_SET(ch->pcdata->thaum_type, THAUM_TYPE_KOLDUN))
				{
					send_to_char ("You remove their koldun thaumaturgy flag.\n\r", ch);
					send_to_char ("You no longer know koldun thaumaturgy.\n\r", victim);
					REMOVE_BIT (ch->pcdata->thaum_type, THAUM_TYPE_KOLDUN);
				}
				else
				{
					send_to_char ("You add a koldun thaumaturgy flag to them.\n\r", ch);
					send_to_char ("You now know koldun thaumaturgy.\n\r", victim);
					SET_BIT (ch->pcdata->thaum_type, THAUM_TYPE_KOLDUN);
				}
				return;
			}
			else
			{
				send_to_char ("Sorry, no can do...\n\r", ch);
				return;
			}
	}


	if (!str_cmp (arg2, "beast"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (value < 0 || value > 100)
		{
			send_to_char ("Beast range is 0 to 100.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->beast = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "willpower"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}


		if (value < 1 || value > 10)
		{
			send_to_char ("Willpower points range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->willpower[0] = value;
			victim->pcdata->willpower[1] = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "dps"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}


		if (value < 1 || value > 200000)
		{
			send_to_char ("Demon points range is 1 to 200000.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->power[0] = value;
			victim->pcdata->power[1] = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "quest"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (value < 0 || value > 15000)
		{
			send_to_char ("Quest range is 0 to 15000.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->quest = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	if (!str_cmp (arg2, "roadrank"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		if (value < 0 || value > 10)
		{
			send_to_char ("Roadrank range is 1 to 10.\n\r", ch);
			return;
		}

		if (IS_JUDGE (ch))
		{
			victim->pcdata->roadt = value;
			send_to_char ("Ok, road rank set to desired amount.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "password"))
	{
		if (IS_NPC(victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char ("pset <victim> password <pass>.\n\r", ch );
			return;
		}

		pwdnew = crypt (arg3, victim->name );
		for (p = pwdnew; *p != '\0'; p++)
		{
			if (*p == '~')
			{
				send_to_char("New password should NOT have a tilde.\n\r", ch);
				return;
			}
		}

		free_string (victim->pcdata->pwd);
		victim->pcdata->pwd = str_dup (pwdnew);
		if (!IS_EXTRA (victim, EXTRA_NEWPASS))
			SET_BIT(victim->extra,EXTRA_NEWPASS);
		do_autosave (victim, "");
		send_to_char("Ok.\n\r",ch);
		return;
	}

	if (!str_cmp (arg2, "road"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}

		if (arg3 == '\0')
		{
			send_to_char( "pset <victim> road <road>.\n\r",ch);
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

		if (IS_JUDGE (ch))
		{
			for (i = 0; i < MAX_ROADS; i++)
			{
				if (!str_cmp (arg3, road_names[i][0]))
				{
					road = i;
					break;
				}
			}

			if (road == -1)
			{
				send_to_char ("Unknown road.\n\r", ch);
				return;
			}

			victim->pcdata->road = road;
			victim->pcdata->roadt = 1;
			sprintf( buf, "You now follow the Road of %s.",
			 road_names[road][0]);
			send_to_char( buf, victim);
			sprintf( buf, "They now follow the Road of %s",
			 road_names[road][0]);
			send_to_char( buf, ch );
			return;
		}
		else
			send_to_char ("Sorry, no can do.\n\r", ch);
		return;
	}

	/*
	 * Generate usage message.
	 */
	do_pset (ch, "");
	return;
}

void do_reward (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int v;
	bool is_offline;

	is_offline = FALSE;

	sprintf (buf, "%s: Pset %s", ch->name, argument);

	smash_tilde (argument);
	argument = one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Reward <target> <xp|dps|wp> <amount> <message>\n\r", ch);
		return;
	}
	if ((vch = get_char_world(ch, arg)) == NULL)
	{
		d = alloc_mem(sizeof(*d));
		if (load_char_obj(d, arg))
		{
			send_to_char ("#RNote#n: Character not online.\n\r", ch);
			vch = d->character;
			is_offline = TRUE;
		}
		else
		{
			send_to_char("No such character exists.\n\r", ch);
			return;
		}
	}

	if (IS_NPC (vch))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);
	v = atoi (arg2);

	if (arg[0] == '\0' || arg2[0] == '\0' || (!is_number (arg2) && v >= 0))
	{
		do_reward (ch, "");
		return;
	}

	if (!str_cmp (arg, "xp"))
	{
		if (v < -500 || v > 500)
		{
			send_to_char ("Experience points range is -500 to 500.\n\r", ch);
			return;
		}
		vch->exp += v;
		vch->pcdata->score[SCORE_TOTAL_XP] += v;
	}
	else if (!str_cmp (arg, "wp") || !str_cmp (arg, "willpower"))
	{
		if (v < 0 || v > 3)
		{
		   send_to_char ("Be reasonable.\n\r", ch );
		   return;
		}
		vch->pcdata->willpower[0] += v;
	}
	else if (!str_cmp (arg, "dps"))
	{
		if (v < -5000 || v > 5000)
		{
			send_to_char ("Demon points range is -5000 to 5000.\n\r", ch);
			return;
		}
		vch->pcdata->power[0] += v;
		vch->pcdata->power[1] += v;
		send_to_char ("Ok.\n\r", ch);
		return;
	}
	else
	{
		send_to_char ("Please specify XP, WP or DPS.\n\r", ch);
		return;
	}

	if (vch->mkill < 5)
	{
		vch->mkill = 5;
		do_autosave (ch, "");
	}
	if (is_offline)
	{
		save_char_obj(vch);
		free_char(vch);
	}
	else
	{	
		sprintf (buf, "You have been rewarded %d %s for %s.\n\r", v, arg, argument);
		send_to_char (buf, vch);
	}
	sprintf (buf, "You have rewarded them %d %s for %s.\n\r", v, arg, argument);
	send_to_char (buf, ch);
	
		
}

/* To have VLIST show more than vnum 0 - 9900, change the number below: */

#define MAX_SHOW_VNUM   320	/* show only 1 - 100*100 */

#define NUL '\0'


extern ROOM_INDEX_DATA *room_index_hash[MAX_KEY_HASH];
/* db.c */

/* opposite directions */
const sh_int opposite_dir[6] = { DIR_SOUTH, DIR_WEST, DIR_NORTH, DIR_EAST, DIR_DOWN, DIR_UP };


/* get the 'short' name of an area (e.g. MIDGAARD, MIRROR etc. */
/* assumes that the filename saved in the AREA_DATA struct is something like midgaard.are */
char *area_name (AREA_DATA * pArea)
{
	static char buffer[64];	/* short filename */
	char *period;

	assert (pArea != NULL);

	strncpy (buffer, pArea->filename, 64);	/* copy the filename */
	period = strchr (buffer, '.');	/* find the period (midgaard.are) */
	if (period)		/* if there was one */
		*period = '\0';	/* terminate the string there (midgaard) */

	return buffer;
}

typedef enum
{ exit_from, exit_to, exit_both }
exit_status;

/* depending on status print > or < or <> between the 2 rooms */
void room_pair (ROOM_INDEX_DATA * left, ROOM_INDEX_DATA * right, exit_status ex, char *buffer)
{
	char *sExit;

	switch (ex)
	{
	default:
		sExit = "??";
		break;		/* invalid usage */
	case exit_from:
		sExit = "< ";
		break;
	case exit_to:
		sExit = " >";
		break;
	case exit_both:
		sExit = "<>";
		break;
	}

	sprintf (buffer, "%5d %-26.26s %s%5d %-26.26s(%-8.8s)\n\r", left->vnum, left->name, sExit, right->vnum, right->name, area_name (right->area));
}

/* for every exit in 'room' which leads to or from pArea but NOT both, print it */
void checkexits (ROOM_INDEX_DATA * room, AREA_DATA * pArea, char *buffer)
{
	char buf[MAX_STRING_LENGTH];
	int i;
	EXIT_DATA *exit;
	ROOM_INDEX_DATA *to_room;

	strcpy (buffer, "");
	for (i = 0; i < 6; i++)
	{
		exit = room->exit[i];
		if (!exit)
			continue;
		else
			to_room = exit->to_room;

		if (to_room)	/* there is something on the other side */
		{
			if ((room->area == pArea) && (to_room->area != pArea))
			{	/* an exit from our area to another area */
				/* check first if it is a two-way exit */

				if (to_room->exit[opposite_dir[i]] && to_room->exit[opposite_dir[i]]->to_room == room)
					room_pair (room, to_room, exit_both, buf);	/* <> */
				else
					room_pair (room, to_room, exit_to, buf);	/* > */

				strcat (buffer, buf);
			}
			else if ((room->area != pArea) && (exit->to_room->area == pArea))
			{	/* an exit from another area to our area */

				if (!(to_room->exit[opposite_dir[i]] && to_room->exit[opposite_dir[i]]->to_room == room))
					/* two-way exits are handled in the other if */
				{
					room_pair (to_room, room, exit_from, buf);
					strcat (buffer, buf);
				}

			}	/* if room->area */
		}		/* if to_room */
	}			/* for */

}

/* for now, no arguments, just list the current area */
void do_exlist (CHAR_DATA * ch, char *argument)
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *room;
	int i;
	char buffer[MAX_STRING_LENGTH];

	pArea = ch->in_room->area;	/* this is the area we want info on */
	for (i = 0; i < MAX_KEY_HASH; i++)	/* room index hash table */
		for (room = room_index_hash[i]; room != NULL; room = room->next)
			/* run through all the rooms on the MUD */

		{
			checkexits (room, pArea, buffer);
			send_to_char (buffer, ch);
		}
}


void do_restring (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char endchar[15];

	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))
		return;


	if (arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("Syntax: restring <object> room/short <short description>\n\r", ch);
		send_to_char ("Syntax: restring <object> keyword/name <extra keyword/s>\n\r", ch);
		send_to_char ("Syntax: restring <object> ground/long <on ground description>\n\r", ch);
		return;
	}

	if ((obj = get_obj_carry (ch, arg1)) == NULL)
	{
		send_to_char ("You are not carrying that object.\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "room") || !str_cmp (arg2, "short"))
	{
		if (strlen (argument) < 5)
		{
			send_to_char ("Name should be at least 5 characters long.\n\r", ch);
			return;
		}
		endchar[0] = (argument[strlen (argument) - 1]);
		endchar[1] = '\0';
		free_string (obj->short_descr);
		obj->short_descr = str_dup (argument);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		send_to_char ("Ok. Short description set\n\r", ch);
		return;
	}
	else if (!str_cmp (arg2, "ground") || !str_cmp (arg2, "long"))
	{
		if (strlen (argument) < 5)
		{
			send_to_char ("Description for the ground should be at least 5 characters long.\n\r", ch);
			return;
		}
		endchar[0] = (argument[strlen (argument) - 1]);
		endchar[1] = '\0';
		sprintf( buf, argument );
		buf[0] = UPPER (buf[0]);
		free_string (obj->description);
		obj->description = str_dup (buf);
		send_to_char ("Ok. Long description set\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	else if (!str_cmp (arg2, "keyword") || !str_cmp (arg2, "keywords") || !str_cmp (arg2, "name"))
	{
		if (strlen (argument) < 3)
		{
			send_to_char ("Keywords should be at least 3 characters long.\n\r", ch);
			return;
		}

		free_string (obj->name);
		obj->name = str_dup (argument);
		send_to_char ("Ok. Keywords set.\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}
	send_to_char ("Syntax: restring <object> room  <short description>\n\r", ch);
	send_to_char ("Syntax: restring <object> keyword <extra keyword/s>\n\r", ch);
	send_to_char ("Syntax: restring <object> ground <on ground description>\n\r", ch);
	return;
}


void do_fvlist (CHAR_DATA * ch, char *argument)
{
	int i, j;
	char buf3[MSL];
	char arg[MAX_INPUT_LENGTH];
	char *string;

	string = one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("Syntax:\n\r", ch);
		send_to_char ("  fvlist obj\n\r", ch);
		send_to_char ("  fvlist mob\n\r", ch);
		send_to_char ("  fvlist room\n\r", ch);
		return;
	}
	j = 1;

	if (!str_cmp (arg, "obj"))
	{
		sprintf (buf3, "Free %s vnum listing for area %s\n\r", arg, ch->in_room->area->name);
		stc (buf3, ch);

		stc ("==============================================================================\n\r", ch);

		for (i = ch->in_room->area->lvnum; i <= ch->in_room->area->uvnum; i++)
		{
			if (get_obj_index (i) == NULL)
			{
				sprintf (buf3, "%8d, ", i);
				stc (buf3, ch);
				if (j == COLUMNS)
				{
					send_to_char ("\n\r", ch);
					j = 0;
				}
				j++;
			}
		}
		send_to_char ("\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "mob"))
	{
		sprintf (buf3, "Free %s vnum listing for area %s\n\r", arg, ch->in_room->area->name);
		stc (buf3, ch);

		stc ("{Y==============================================================================\n\r", ch);
		for (i = ch->in_room->area->lvnum; i <= ch->in_room->area->uvnum; i++)
		{
			if (get_mob_index (i) == NULL)
			{
				sprintf (buf3, "%8d, ", i);
				stc (buf3, ch);
				if (j == COLUMNS)
				{
					send_to_char ("\n\r", ch);
					j = 0;
				}
				else
					j++;
			}
		}
		send_to_char ("\n\r", ch);
		return;
	}
	if (!str_cmp (arg, "room"))
	{
		sprintf (buf3, "Free %s vnum listing for area %s\n\r", arg, ch->in_room->area->name);

		stc ("==============================================================================\n\r", ch);
		for (i = ch->in_room->area->lvnum; i <= ch->in_room->area->uvnum; i++)
		{
			if (get_room_index (i) == NULL)
			{
				sprintf (buf3, "%8d, ", i);
				stc (buf3, ch);
				if (j == COLUMNS)
				{
					send_to_char ("\n\r", ch);
					j = 0;
				}
				else
					j++;
			}
		}
		send_to_char ("\n\r", ch);
		return;
	}
	send_to_char ("WHAT??? \n\r", ch);
	send_to_char ("Syntax:\n\r", ch);
	send_to_char ("  fvlist obj\n\r", ch);
	send_to_char ("  fvlist mob\n\r", ch);
	send_to_char ("  fvlist room\n\r", ch);
}


void do_rlist (CHAR_DATA * ch, char *argument)
{
	char buf[MSL];
	char addbuf[MSL];
	ROOM_INDEX_DATA *room;
	int lvnum, uvnum, i;

	sprintf (buf, "Rooms in area %s.\n\r", ch->in_room->area->name);
	lvnum = ch->in_room->area->lvnum;
	uvnum = ch->in_room->area->uvnum;

	if (uvnum - lvnum > 1024)
	{
		send_to_char( "Too long.  Aborting.\n\r", ch );
		return;
	}

	for (i = lvnum; i <= uvnum; i++)
	{
		if ((room = get_room_index (i)) == NULL)
			continue;

		sprintf( addbuf, "[%d] %s #R|#n", i, room->name );
		strcat( buf, addbuf );
	}
	page_to_char( buf, ch );
	return;
}

void do_olist (CHAR_DATA * ch, char *argument)
{
	char buf[MSL];
	char addbuf[MSL];
	OBJ_INDEX_DATA *obj;
	int lvnum, uvnum, i;
	int count = 0;

	sprintf (buf, "Objects in area %s.\n\r", ch->in_room->area->name);

	lvnum = ch->in_room->area->lvnum;
	uvnum = ch->in_room->area->uvnum;

	for (i = lvnum; i <= uvnum; i++)
	{
		if ((obj = get_obj_index (i)) == NULL)
			continue;
		if (count == 4)
			stc ("\n\r", ch);
		sprintf (addbuf, "[%d] %s  ", i, obj->name);
		strcat( buf, addbuf );
		count++;
	}
	page_to_char( buf, ch );
	return;
}

void do_mlist (CHAR_DATA * ch, char *argument)
{
	char buf[MSL];
	char addbuf[MSL];
	MOB_INDEX_DATA *mob;
	int lvnum, uvnum, i;
	int count = 0;

	sprintf (buf, "Mobs in area %s.\n\r", ch->in_room->area->name);

	lvnum = ch->in_room->area->lvnum;
	uvnum = ch->in_room->area->uvnum;

	for (i = lvnum; i <= uvnum; i++)
	{
		if ((mob = get_mob_index (i)) == NULL)
			continue;
		if (count == 4)
			stc ("\n\r", ch);
		sprintf (addbuf, "[%d] %s  ", i, mob->short_descr);
		strcat( buf, addbuf );
		count++;
	}

	page_to_char( buf, ch );
	return;
}


void do_wizlist (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "wizlist" );
	return;
}

void do_nuke (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char filename[100];
	char name[30];
	char buf[40];

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (arg[0] == '\0')
	{
		send_to_char ("You need to specify a target.\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}


	if (IS_NPC (victim))
	{
		send_to_char ("You can't nuke a NPC.\n\r", ch);
		return;
	}
	send_to_char ("Aye Carumba!  You pissed off the wrong Immortal mate.\n\r", victim);
	do_ban (ch, victim->desc->host);
	send_to_char ("Banned.\n\r", ch);
	strcpy (filename, "../player/");
	strcpy (name, victim->name);
	if ((int) name[0] >= 97 && (int) name[0] <= 122)
		name[0] = (int) name[0] - 32;
	strcat (filename, name);
	send_to_char (filename, ch);
	do_quit (victim, "");
	send_to_char ("Disconnected.\n\r", ch);
	if (unlink (filename) != 0)
	{
		sprintf (buf, "Errno: %d\n\r", errno);
		send_to_char (buf, ch);
	}
	send_to_char ("Deleted.\n\r", ch);
	send_to_char ("Nuke Complete.\n\r", ch);
}


void do_hedit (CHAR_DATA * ch, char *argument)
{
	HELP_DATA *help_temp;
	HELP_DATA *help_search;
	HELP_DATA *previous;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	bool found = FALSE;

	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;
	if (arg1[0] == '\0')
	{
		send_to_char ("hedit - Online Help File Editor\n\r", ch);
		send_to_char ("-------------------------------\n\r", ch);
		send_to_char ("Options: create, save, delete, edit\n\r", ch);
		send_to_char ("Note:    delete and edit also require you to specify\n\r", ch);
		send_to_char ("         the file you want to edit\n\r", ch);
		send_to_char ("Examples:\n\r", ch);
		send_to_char ("hedit create	 - creates a new help file\n\r", ch);
		send_to_char ("hedit save	 - saves ALL the help files\n\r", ch);
		send_to_char ("hedit delete fight - deletes the fight help file\n\r", ch);
		send_to_char ("hedit edit drink   - deletes the drink help file\n\r", ch);
		return;
	}
	if (!strcmp (arg1, "create"))
	{
		help_temp = alloc_mem (sizeof (HELP_DATA));
		help_temp->level = 0;
		help_temp->keyword = str_dup ("");
		help_temp->text = str_dup ("");
		help_temp->next = NULL;
		ch->desc->pEdit = (void *) help_temp;
		ch->desc->editor = ED_HEDITOR;
		hedit (ch, "");
		return;
	}
	else if (!strcmp (arg1, "save"))
	{
		FILE *fp;
		fp = fopen ("help.temp", "w");

		if (fp == NULL)
		{
			log_string ("Error opening temp help file\n\r", CHANNEL_LOG_NORMAL);
			send_to_char ("Error opening temp help file\n\r", ch);
			return;
		}
		fprintf (fp, "#HELPS\n\n\n");
		for (help_search = help_first; help_search != NULL; help_search = help_search->next)
			fprintf (fp, "%d %s~\n%s~ ", help_search->level, help_search->keyword, help_search->text);
		fprintf (fp, "0 $~ \n\n#$ \n");
		fclose (fp);
		system ("mv help.are help.old");
		system ("mv help.temp help.are");
		system ("rm help.old");
		send_to_char ("File Saved\n", ch);
		return;
	}
	else if (!strcmp (arg1, "delete"))
	{
		while (argument[0] != '\0' && found == FALSE)
		{
			argument = one_argument (argument, arg2);
			previous = help_first;
			for (help_search = help_first; help_search != NULL; help_search = help_search->next)
			{
				if (is_names (arg2, help_search->keyword))
				{
					found = TRUE;
					sprintf (buf, "Found: %s\n\r", help_search->keyword);
					send_to_char (buf, ch);
					previous->next = help_search->next;
					sprintf (buf, "Deleting: %s\n\r", help_search->keyword);
					send_to_char (buf, ch);
					free_mem (help_search, sizeof (HELP_DATA));
					top_help--;
					return;
				}
				else
					previous = help_search;
			}
		}
		if (found == FALSE)
		{
			send_to_char ("Error Helpfile not found, make sure it exists before you try to delete it.\n\r", ch);
			return;
		}
	}

	else if (!strcmp (arg1, "edit"))
	{
		while (argument[0] != '\0' && found == FALSE)
		{
			argument = one_argument (argument, arg2);
			previous = help_first;
			for (help_search = help_first; help_search != NULL; help_search = help_search->next)
			{
				if (is_names (arg2, help_search->keyword))
				{
					found = TRUE;
					if (previous == help_first)
						help_first = help_first->next;
					else
						previous->next = help_search->next;
					sprintf (buf, "Found: %s\n\r", help_search->keyword);
					send_to_char (buf, ch);
					ch->desc->pEdit = (void *) help_search;
					ch->desc->editor = ED_HEDITOR;
					hedit (ch, "");
					return;
				}
				if (previous != help_search)
					previous = help_search;
			}
		}
		if (found == FALSE)
		{
			send_to_char ("Error Helpfile not found, try hedit create\n\r", ch);
			return;
		}
	}
}

void hedit (CHAR_DATA * ch, char *argument)
{
	HELP_DATA *help_edit;
	HELP_DATA *help_search;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char argall[MAX_INPUT_LENGTH];
	char buf[4 * MAX_STRING_LENGTH];
	int level = 0;
	bool bad = FALSE;

	help_edit = (HELP_DATA *) ch->desc->pEdit;
	argument = one_argument (argument, arg1);

	if (!strcmp (arg1, "show") || arg1[0] == '\0')
	{
		send_to_char ("#B****************************** #eHELP FILE EDITOR #B********************************#n\n\r", ch);
		sprintf (buf, "Level: %d\n\r", help_edit->level);
		send_to_char (buf, ch);
		sprintf (buf, "Keywords: %s\n\r", help_edit->keyword);
		send_to_char (buf, ch);
		sprintf (buf, "Text: %s\n\r", help_edit->text);
		send_to_char (buf, ch);
		send_to_char ("#B********************************************************************************#e\n\r", ch);
		send_to_char ("Options:\n\r", ch);
		send_to_char ("done                       - exits from the help file editor\n\r", ch);
		send_to_char ("drop                       - abandons the help file and exits from the editor\n\r", ch);
		send_to_char ("level <# between 0 and 10> - sets the level required to read help file\n\r", ch);
		send_to_char ("keyword <all keywords>     - sets the help file names\n\r", ch);
		send_to_char ("text                       - jumps to text editor mode to create the help description\n\r", ch);
		send_to_char ("#B********************************************************************************#e\n\r", ch);
		return;
	}

	if (!strcmp (arg1, "done"))
	{
		if (!strcmp (help_edit->keyword, ""))
		{
			send_to_char ("#RError, Keywords are blank.#n\n\r", ch);
			hedit (ch, "");
			return;
		}
		if (!strcmp (help_edit->text, ""))
		{
			send_to_char ("#RError, help text is blank.#n\n\r", ch);
			hedit (ch, "");
			return;
		}


		if (help_first == NULL)
			help_first = help_edit;
		else
		{
			help_edit->next = help_first;
			help_first = help_edit;
		}
		//      if (help_last != NULL)
		//              help_last->next = help_edit;

		//      help_last = help_edit;
		//      help_edit->next = NULL;


		top_help++;

		ch->desc->pEdit = NULL;
		ch->desc->connected = CON_PLAYING;
		ch->desc->editor = 0;
		return;
	}

	if (!strcmp (arg1, "drop"))
	{
		free_mem (help_edit, sizeof (HELP_DATA));
		ch->desc->pEdit = NULL;
		ch->desc->connected = CON_PLAYING;
		ch->desc->editor = 0;
		return;
	}

	if (!strcmp (arg1, "keyword") || !strcmp (arg1, "keywords"))
	{
		argall[0] = '\0';
		while (argument[0] != '\0')
		{
			argument = one_argument (argument, arg1);
			if (argall[0] != '\0')
				strcat (argall, " ");
			strcat (argall, arg1);
		}

		for (help_search = help_first; help_search != NULL; help_search = help_search->next)
			if (is_names (argall, help_search->keyword))
				bad = TRUE;

		if (bad == FALSE)
		{
			while (argall[level] != '\0')
			{
				if ((isalpha (argall[level])) && (argall[level] > 96) && (argall[level] < 123))
					argall[level] = argall[level] - 32;
				level++;
			}
			free_string (help_edit->keyword);
			help_edit->keyword = str_dup (argall);
			hedit (ch, "");
			return;
		}
		else
		{
			send_to_char ("#RError, one of your keywords has already been used for another help file.#n\n\r", ch);
			hedit (ch, "");
			return;

		}
	}


	if (!strcmp (arg1, "level"))
	{
		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char ("#RError, you must specify the level#n\n\r", ch);
			hedit (ch, "");
			return;
		}
		level = atoi (arg2);
		if (level < 0 || level > 12)
		{
			send_to_char ("#RError, level must be within 0-12#n\n\r", ch);
			hedit (ch, "");
			level = 0;
			return;
		}
		help_edit->level = level;
		hedit (ch, "");
	}

	if (!strcmp (arg1, "text"))
	{
		string_append (ch, &help_edit->text);
		return;
	}
}




/*
void do_mpedit (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	MPROG_DATA *mptemp;

	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;
	if (arg1[0] == '\0')
	{
		send_to_char ("#R-------------------------------------------------------------#n\n\r", ch);
		send_to_char ("#R|#n mpedit - Online MobProg Editor                            #R|#n\n\r", ch);
		send_to_char ("#R-------------------------------------------------------------#n\n\r", ch);
		send_to_char ("#R|#n #BOptions: #ncreate, delete, edit, list                       #R|#n\n\r", ch);
		send_to_char ("#R|#n #BNote:    #ndelete and edit also require you to specify      #R|#n\n\r", ch);
		send_to_char ("#R|#n          the file you want to delete or edit              #R|#n\n\r", ch);
		send_to_char ("#R|#n #BExamples:#n                                                 #R|#n\n\r", ch);
		send_to_char ("#R|#n mpedit create	       - creates a new mob prog             #R|#n\n\r", ch);
		send_to_char ("#R|#n mpedit delete beggar - deletes the 'beggar' mob prog      #R|#n\n\r", ch);
		send_to_char ("#R|#n mpedit edit beggar   - edits the 'beggar' mob prog        #R|#n\n\r", ch);
		send_to_char ("#R|#n mpedit list	       - lists all the mob progs created    #R|#n\n\r", ch);
		send_to_char ("#R-------------------------------------------------------------#n\n\r", ch);
		return;
	}
	if (!strcmp (arg1, "list"))
	{
		char buf[30];
		struct dirent *direntp;
		DIR *dirp;

		send_to_char("#R---------------------------#n\n\r", ch);
		send_to_char("#R|#n Current Mob Programs    #R|#n\n\r", ch);
		send_to_char("#R---------------------------#n\n\r", ch);
		strcpy(buf, "./");
		strcat(buf, MOB_DIR);
		dirp = opendir (buf);
		while ((direntp = readdir(dirp)) != NULL)
		{
			if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
				continue;
			else
			{
				if(!strcmp((direntp->d_name+strlen(direntp->d_name)-4), ".prg"));
				{				
					direntp->d_name[strlen(direntp->d_name)-4] = '\0';
					send_to_char(direntp->d_name, ch);
					send_to_char("\n\r", ch);
				}
			}
		}
		send_to_char("\n\r", ch);
		send_to_char("\n\r", ch);
		return;
	}

	if (!strcmp (arg1, "create"))
	{
		mptemp = NULL;
		ch->desc->pEdit = (void *) mptemp;
		ch->desc->editor = ED_MPEDITOR;
		mpedit (ch, "");
		return;
	}

	else if (!strcmp (arg1, "delete"))
	{
		char buf[30];
		struct dirent *direntp;
		DIR *dirp;
		bool found = FALSE;

		argument = one_argument (argument, arg2);
		if(arg2[0] == '\0')
		{
			send_to_char("#RError: #nYou must provide the name to delete.\n\r", ch);
			return;
		}
		strcpy(buf, "./");
		strcat(buf, MOB_DIR);
		dirp = opendir (buf);
		while ((direntp = readdir(dirp)) != NULL && found == FALSE)
		{
			if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
				continue;
			else
			{
				if(!strcmp((direntp->d_name+strlen(direntp->d_name)-4), ".prg"));
				{				
					direntp->d_name[strlen(direntp->d_name)-4] = '\0';
					if(!strcmp(direntp->d_name, arg2))
						found = TRUE;
				}
			}
		}

		if (found == TRUE)
		{
			strcpy(buf, "rm ");
			strcat(buf, "./");
			strcat(buf, MOB_DIR);
			strcat(buf, arg2);
			strcat(buf, ".prg");
			system(buf);
			send_to_char("Delete successful.\n\r", ch);
		}
		else
			send_to_char("#RError: #nThe mobprog file could not be found.\n\r", ch);
		return;
	}
	else if (!strcmp (arg1, "edit"))
	{
		char buf[30];
		struct dirent *direntp;
		DIR *dirp;
		bool found = FALSE;
		FILE *fp;

		ch->desc->pEdit = NULL;
		argument = one_argument (argument, arg2);
		if(arg2[0] == '\0')
		{
			send_to_char("#RError: #nYou must provide the name to delete.\n\r", ch);
			return;
		}
		strcpy(buf, "./");
		strcat(buf, MOB_DIR);
		dirp = opendir (buf);
		while ((direntp = readdir(dirp)) != NULL && found == FALSE)
		{
			if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
				continue;
			else
			{
				if(!strcmp((direntp->d_name+strlen(direntp->d_name)-4), ".prg"));
				{				
					direntp->d_name[strlen(direntp->d_name)-4] = '\0';
					if(!strcmp(direntp->d_name, arg2))
						found = TRUE;
				}
			}
		}

		if (found == TRUE)
		{
			char letter;
			bool done = FALSE;		
	
			strcat(buf, arg2);
			strcat(buf, ".prg");
			fp = fopen (buf, "r");

			if (fp == NULL)
			{
				log_string ("Error opening readable mobprog file\n\r", CHANNEL_LOG_NORMAL);
				send_to_char ("Error opening readable mobprog file\n\r", ch);
				return;
			}
			mptemp = alloc_mem (sizeof (MPROG_DATA));
			mptemp->next = NULL;
			if((letter = fread_letter (fp)) != '>')
			{
				log_string("Mpedit Load MobProg edit file error", CHANNEL_LOG_NORMAL);		
				send_to_char("File is screwed... Inform an Imp.\n\r", ch);
				return;
			}

			while (!done)
			{
				char *buf2;
				mptemp->type = mprog_name_to_type(fread_word(fp));
				switch(mptemp->type)
				{
				case ERROR_PROG:
					log_string("Mpedit Load MobProg edit file error", CHANNEL_LOG_NORMAL);		
					send_to_char("File is screwed... Inform an Imp.\n\r", ch);
					return;
				case IN_FILE_PROG:
					log_string("Mpedit Load MobProg edit file error called an infile", CHANNEL_LOG_NORMAL);		
					send_to_char("File is screwed... Inform an Imp.\n\r", ch);
					return;
				default:
					buf2 = fread_string(fp);
					mptemp->arglist = str_dup(buf2);
					mptemp->comlist = fread_string(fp);
					switch(letter = fread_letter(fp))
					{
						case '>':
							mptemp->next = alloc_mem (sizeof (MPROG_DATA));
							if(ch->desc->pEdit == NULL)
								ch->desc->pEdit = (void *)mptemp;
							mptemp = mptemp->next;
							mptemp->next = NULL;
							break;
						case '|':
							done = TRUE;
							if(ch->desc->pEdit == NULL)
								ch->desc->pEdit = (void *)mptemp;					
							break;
						default:
							log_string("bug in mobprog file syntax error.", CHANNEL_LOG_NORMAL);
							return;
					}
					break;
				}						
			}
			fclose(fp);
			ch->desc->tempString = str_dup(arg2);
			ch->desc->editor = ED_MPEDITOR;
			mpedit (ch, "");
			return;

		}
		else
			send_to_char("#RError: #nThe mobprog file could not be found.\n\r", ch);
		return;
	}
}
*/
/*
void mpedit(CHAR_DATA *ch, char *argument)
{
	int i=0;
	MPROG_DATA *current;
	MPROG_DATA *previous;
	MPROG_DATA *mpeditt;
	MPROG_DATA *mptemp;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int func;

	mpeditt = (MPROG_DATA *) ch->desc->pEdit;
	current = mpeditt;
	argument = one_argument(argument, arg1);
	if (!strcmp (arg1, "show") || arg1[0] == '\0')
	{
		send_to_char ("#B******************************* #eMOB PROG EDITOR #B********************************#n\n\r", ch);
		while(current != NULL)
		{
			sprintf(buf, "%d) %s\n\r", i, mprog_type_to_name(current->type));
			send_to_char(buf, ch);
			current = current->next;
			i++;
		}
		send_to_char ("#B********************************************************************************#e\n\r", ch);
		send_to_char ("#ROptions:#n\n\r", ch);
		send_to_char ("create                        - creates new mob function\n\r", ch);
		send_to_char ("edit <number>                 - edits individual mob function\n\r", ch);
		send_to_char ("delete <number>               - deletes individual mob function\n\r", ch);
		send_to_char ("view <number>                 - views individual mob function\n\r", ch);
		send_to_char ("list			      - lists all the saved mobprog filenames\n\r", ch);
		send_to_char ("drop			      - exit the editor without saving any changes\n\r", ch);
		send_to_char ("save			      - exit the editor and save ALL changes\n\r", ch);
		send_to_char ("#B********************************************************************************#e\n\r", ch);
		return;
	}
	else if (!strcmp (arg1, "delete"))
	{
		argument = one_argument(argument, arg2);
		if(arg2[0] == '\0')
		{
			send_to_char("#RError: #nYou must include which function you want to delete.\n\r", ch);
			return;
		}
		if(current == NULL)
		{
			send_to_char("#RError: #nYou haven't defined any functions yet.\n\r", ch);
			return;
		}
		func = atoi(arg2);
		if(func < 0)
		{
			send_to_char("#RError: #nBad View Value.\n\r", ch);
			return;
		}
		previous = current;
		for(i=0;i<func;i++)
		{
			if (current->next != NULL)
			{
				if(previous != current)
					previous = current;
				current = current->next;
			}
			else
			{
				send_to_char("#RError: #nValue too high.\n\r", ch);
				log_string("Mob Prog editor trying to go past available function linked list.", CHANNEL_LOG_NORMAL);
				return;		
			}
		}
		if(func == 0)
		{
			previous = previous->next;
			ch->desc->pEdit = (void *)previous;
		}
		else
			previous->next = current->next;
		free_mem (current, sizeof (MPROG_DATA));
		return;
	}
	else if (!strcmp (arg1, "view"))
	{
		argument = one_argument(argument, arg2);
		if(arg2[0] == '\0')
		{
			send_to_char("#RError: #nYou must include which function you want to view.\n\r", ch);
			return;
		}
		if(current == NULL)
		{
			send_to_char("#RError: #nYou haven't defined any functions yet.\n\r", ch);
			return;
		}
		func = atoi(arg2);
		if(func < 0)
		{
			send_to_char("#RError: #nBad View Value.\n\r", ch);
			return;
		}
		for(i=0;i<func;i++)
		{
			if (current->next != NULL)
				current = current->next;
			else
			{
				send_to_char("#RError: #nValue too high.\n\r", ch);
				log_string("Mob Prog editor trying to go past available function linked list.", CHANNEL_LOG_NORMAL);
				return;		
			}
		}
		sprintf (buf, "Type: %s\n\r", mprog_type_to_name(current->type));
		send_to_char(buf, ch);
		sprintf (buf, "Percentage: %s\n\r", current->arglist);
		send_to_char(buf, ch);
		sprintf (buf, "Mob Prog: \n\r%s\n\r", current->comlist);
		send_to_char (buf, ch);
		return;
	}
	else if (!strcmp (arg1, "create"))
	{
		mptemp = alloc_mem (sizeof (MPROG_DATA));
		mptemp->type = ERROR_PROG;
		mptemp->arglist = str_dup("");
		mptemp->next = NULL;
		mptemp->comlist = str_dup ("");
		ch->desc->pEdit2 = (void *) mptemp;
		ch->desc->editor = ED_SMPEDITOR;
		sing_mpedit (ch, "");
		return;
	}
	else if (!strcmp (arg1, "edit"))
	{
		argument = one_argument(argument, arg2);
		if(arg2[0] == '\0')
		{
			send_to_char("#RError: #nYou must include which function you want to edit.\n\r", ch);
			return;
		}
		if(current == NULL)
		{
			send_to_char("#RError: #nYou haven't defined any functions yet.\n\r", ch);
			return;
		}
		func = atoi(arg2);
		if(func < 0)
		{
			send_to_char("#RError: #nBad View Value.\n\r", ch);
			return;
		}
		previous = current;
		for(i=0;i<func;i++)
		{
			if (current->next != NULL)
			{
				if(previous != current)
					previous = current;
				current = current->next;
			}
			else
			{
				send_to_char("#RError: #nValue too high.\n\r", ch);
				log_string("Mob Prog editor trying to go past available function linked list.", CHANNEL_LOG_NORMAL);
				return;		
			}
		}

		if(func == 0)
		{
			ch->desc->pEdit = (void *) current->next;
			current->next = NULL;
			ch->desc->pEdit2 = (void *) current;
		}
		else
		{
			previous->next = current->next;
			current->next = NULL;
			ch->desc->pEdit2 = (void *) current;
		}				
		ch->desc->editor = ED_SMPEDITOR;
		sing_mpedit (ch, "");
		return;
	}
	else if (!strcmp (arg1, "drop"))
	{
		if(current != NULL)
		{
			previous = current;
			while (current != NULL)
			{
				if(previous != current)
				{
					free_mem (previous, sizeof (MPROG_DATA));
					previous = current;
				}
				current = current->next;
			}
			free_mem (previous, sizeof (MPROG_DATA));
		}
		ch->desc->pEdit = NULL;
		ch->desc->pEdit2 = NULL;
		ch->desc->connected = CON_PLAYING;
		ch->desc->editor = 0;
		send_to_char("Exiting editor and dropping all changes.\n\r", ch);
	}
	else if (!strcmp (arg1, "list"))
	{
		char buf[30];
		struct dirent *direntp;
		DIR *dirp;


		send_to_char("#R---------------------------#n\n\r", ch);
		send_to_char("#R|#n Current Mob Programs    #R|#n\n\r", ch);
		send_to_char("#R---------------------------#n\n\r", ch);
		strcpy(buf, "./");
		strcat(buf, MOB_DIR);
		dirp = opendir (buf);
		while ((direntp = readdir(dirp)) != NULL)
		{
			if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
				continue;
			else
			{
				if(!strcmp((direntp->d_name+strlen(direntp->d_name)-4), ".prg"));
				{				
					direntp->d_name[strlen(direntp->d_name)-4] = '\0';
					send_to_char(direntp->d_name, ch);
					send_to_char("\n\r", ch);
				}
			}
		}
		send_to_char("\n\r", ch);
		send_to_char("\n\r", ch);
		mpedit(ch, "");
		return;
	}
	else if (!strcmp (arg1, "save"))
	{
		char buf[30];
		struct dirent *direntp;
		DIR *dirp;
		bool dup = FALSE;
		FILE *fp;
		bool overwrite = FALSE;

		argument = one_argument (argument, arg2);
		if (mpeditt == NULL)
		{
			send_to_char("#RError: #nThere is nothing to save.\n\r", ch);
			return;
		}

		if(arg2[0] == '\0')
		{
			if(ch->desc->tempString != NULL)
			{
				send_to_char("Attempting to overwrite the old file.\n\r", ch);
				strcpy(arg2, ch->desc->tempString);
				overwrite = TRUE;
			}
			else
			{
				send_to_char("#RError: #nYou must specify a filename.\n\r", ch);
				return;
			}
		}
		strcpy(buf, "./");
		strcat(buf, MOB_DIR);
		dirp = opendir (buf);
		while ((direntp = readdir(dirp)) != NULL)
		{
			if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
				continue;
			else
			{
				if(!strcmp((direntp->d_name+strlen(direntp->d_name)-4), ".prg"));
				{				
					direntp->d_name[strlen(direntp->d_name)-4] = '\0';
					if(!strcmp(direntp->d_name, arg2))
						dup = TRUE;
				}
			}
		}

		if (dup == FALSE || overwrite == TRUE)
		{
			strcpy(buf, "./");
			strcat(buf, MOB_DIR);
			strcat(buf, arg2);
			strcat(buf, ".prg");
			fp = fopen (buf, "w");

			if (fp == NULL)
			{
				log_string ("Error opening writable mobprog file\n\r", CHANNEL_LOG_NORMAL);
				send_to_char ("Error opening writable mobprog file\n\r", ch);
				return;
			}
			for (current = mpeditt; current != NULL; current = current->next)
				fprintf (fp, ">%s %s~\n%s~\n", mprog_type_to_name(current->type), current->arglist, current->comlist);
			fprintf (fp, "|\n");
			fclose (fp);

			current = mpeditt;	
			if(current != NULL)
			{
				previous = current;
				while (current != NULL)
				{
					if(previous != current)
					{
						free_mem (previous, sizeof (MPROG_DATA));
						previous = current;
					}
					current = current->next;
				}
				free_mem (previous, sizeof (MPROG_DATA));
			}
			ch->desc->pEdit = NULL;
			ch->desc->pEdit2 = NULL;
			ch->desc->connected = CON_PLAYING;
			ch->desc->editor = 0;
			send_to_char ("File Saved\n", ch);
		}
		else
		{
			send_to_char("#RError: #nThe mobprog file already exists.\n\r", ch);
			mpedit(ch, "");
		}
		return;
	}
	else
	{
		send_to_char("#RError: #nUnknown Command#n\n\r", ch);
		mpedit(ch, "");
		return;
	}
}
*//*
void sing_mpedit (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[4 * MAX_STRING_LENGTH];
	MPROG_DATA *mpeditt;
	MPROG_DATA *total;


	mpeditt = (MPROG_DATA *) ch->desc->pEdit2;
	total = (MPROG_DATA *) ch->desc->pEdit;
	argument = one_argument (argument, arg1);

	if (!strcmp (arg1, "show") || arg1[0] == '\0')
	{
		send_to_char ("#B****************************** #eMOB PROG EDITOR #B********************************#n\n\r", ch);
		sprintf (buf, "#RType:#n %s\n\r", mprog_type_to_name(mpeditt->type));
		send_to_char(buf, ch);
		sprintf (buf, "#RArgument:#n %s\n\r", mpeditt->arglist);
		send_to_char(buf, ch);
		sprintf (buf, "#RMob Prog:#n\n\r%s\n\r", mpeditt->comlist);
		send_to_char (buf, ch);
		send_to_char ("#B********************************************************************************#e\n\r", ch);
		send_to_char ("#ROptions:#n\n\r", ch);
		send_to_char ("type                       - lists all the possible mob prog functions\n\r", ch);
		send_to_char ("type <type>                - type of mob prog function\n\r", ch);
		send_to_char ("argument <argument>        - sets the argument of the function\n\r", ch);
		send_to_char ("mobprog                    - enters the mob prog text editor\n\r", ch);
		send_to_char ("drop                       - abandons the help file and exits from the editor\n\r", ch);
		send_to_char ("done                       - exits from the help file editor\n\r", ch);
		send_to_char ("#B********************************************************************************#e\n\r", ch);
		return;
	}
	else if (!strcmp(arg1, "arg") || (!strcmp(arg1, "argument")))
	{
		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char("#RError: #nYou need to include the actual argument.\n\r", ch);
			sing_mpedit(ch, "");
			return;
		}

		mpeditt->arglist = str_dup(arg2);
		sing_mpedit(ch, "");
		return;
	}
	
	else if (!strcmp (arg1, "type"))
	{
		argument = one_argument (argument, arg2);
		if (arg2[0] == '\0')
		{
			int i = 1;
			buf[0] = '\0';
			send_to_char("#R--------------------------------------#n\n\r", ch);
			send_to_char("#R|#n     Possible Mob Program Types     #R|#n\n\r", ch);
			send_to_char("#R--------------------------------------#n\n\r", ch);
			while(i<=MAX_PROG)
			{
				if((i==256) || (i==16))
					strcat(buf, "\n\r");
				strcat(buf, mprog_type_to_name(i));
				strcat(buf, " ");
				i = i*2;			
			}
			send_to_char(buf, ch);
			send_to_char("\n\r\n\r", ch);
			sing_mpedit(ch, "");
			return;
		}
		else
		{
			int i = 1;
			bool found = FALSE;
			while(i<=MAX_PROG)
			{
				if (!strcmp(arg2, mprog_type_to_name(i)))
				{
					mpeditt->type = i;
					free_string(mpeditt->arglist);
					mpeditt->arglist = str_dup("");
					found = TRUE;
				}
				i=i*2;
			}
			if(found == FALSE)
				send_to_char("#RError: #nUnknown MobProgType\n\r", ch);
			send_to_char("\n\r\n\r", ch);
			sing_mpedit(ch, "");
			return;

		}	
	}

	else if (!strcmp (arg1, "done"))
	{
		if (mpeditt->type == ERROR_PROG)
		{
			send_to_char ("#RError, unknown mob program type.#n\n\r", ch);
			sing_mpedit (ch, "");
			return;
		}
		if (!strcmp (mpeditt->comlist, ""))
		{
			send_to_char ("#RError, filename is blank.#n\n\r", ch);
			sing_mpedit (ch, "");
			return;
		}
		mpeditt->next = total;
		ch->desc->pEdit = mpeditt;
		ch->desc->pEdit2 = NULL;
		ch->desc->editor = ED_MPEDITOR;
		send_to_char("File written\n\r", ch);
		mpedit(ch, "");
		return;
	}

	else if (!strcmp (arg1, "drop"))
	{
		free_mem (mpeditt, sizeof (MPROG_DATA));
		ch->desc->pEdit2 = NULL;
		ch->desc->editor = ED_MPEDITOR;
		mpedit(ch, "");
		return;
	}
	else if (!strcmp (arg1, "mp") || !strcmp(arg1, "mobprog"))
	{
		string_append (ch, &mpeditt->comlist);
		return;
	}
	else
	{
		send_to_char("#RError: #nUnknown Command#n\n\r", ch);
		sing_mpedit (ch, "");
		return;
	}
}
*/
void do_mpresets (CHAR_DATA * ch, char *argument)
{
	MPROG_RESETS *temp;
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;
	if(!IS_IMMORTAL(ch))
	{
		send_to_char("Huh?\n\r", ch);
		return;
	}

	send_to_char("#R---------------------------#n\n\r", ch);
	send_to_char("#R|#n Mob Prog Area Resets    #R|#n\n\r", ch);
	send_to_char("#R---------------------------#n\n\r", ch);

	for(temp=ch->in_room->area->mprogs;temp!=NULL;temp = temp->next)
	{
		sprintf(buf, "%d %s\n\r", temp->vnum, temp->filename);
		send_to_char(buf, ch);
	}
	send_to_char("\n\r", ch);
	return;
}

void do_uptime (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	int temp = 0;
	int minutes = 0;
	int hours = 0;
	int days = 0;
	if (IS_NPC(ch))
		return;
	temp = (int)(current_time-uptime);
	days = (temp/(60*60*24));
	if (days != 0)
		temp = (temp%(60*60*24));
	hours = temp/(60*60);
	if (hours != 0)
		temp = (temp%(60*60));
	minutes = temp/60;
	if (minutes != 0)
		temp = (temp%(60));
	
	send_to_char("#BCurrent Uptime:\n\r", ch);
	sprintf(buf, "Days: #R%8d#n\n\r", days);
	send_to_char(buf, ch);
	sprintf(buf, "Hours: #R%7d#n\n\r", hours);
	send_to_char(buf, ch);
	sprintf(buf, "Minutes: #R%5d#n\n\r", minutes);
	send_to_char(buf, ch);
	sprintf(buf, "Seconds: #R%5d#n\n\r", temp);
	send_to_char(buf, ch);
}

/* Begin Masquerade Code */

void do_masquerade (CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int char_success;
	int difficulty = 5;
	int burned_blood;
	
	argument = one_argument(argument, arg);
	
	if (IS_NPC(ch))
		return;
	
	 
 	if (!str_cmp(arg, "blood") && IS_VAMPIRE(ch))
	{
		difficulty--;
		
		//Calculate how much blood will be burned
		if (ch->pcdata->road == ROAD_HUMANITY){
			burned_blood = (6 - ch->pcdata->virtues[VIRTUE_SELFCONTROL]);
					}
		else if 
		((ch->pcdata->road == ROAD_KING) || (ch->pcdata->road == ROAD_HEAVEN)){
			burned_blood = (8 - ch->pcdata->virtues[VIRTUE_SELFCONTROL]);}
			
		else{
			sprintf(buf, "#cYou can not burn blood to Masquerade because of your Path.#n\n\r");
			send_to_char(buf, ch);
			return;
		}
		//Check if they can burn that much blood
		
		if (ch->blood[BLOOD_CURRENT] > burned_blood)
		{
			ch->blood[BLOOD_CURRENT] -= burned_blood;	
			sprintf(buf, "#R%d#c blood burned.#n\n\r", burned_blood);
			send_to_char(buf, ch);
		}
		
		else
		{
			sprintf(buf, "#cYou don't have #R%d#c blood to burn#n\n\r", burned_blood);
			send_to_char(buf, ch);
			return;
		}
	}
	
	if (IS_VAMPIRE(ch))
	{
		if (!str_cmp(ch->clan, "Nosferatu"))
		{
			difficulty += 3;
			
			if (ch->pcdata->road == ROAD_HUMANITY)
			{
				char_success = diceroll(ch, ATTRIB_CHA, TALENTS, TAL_SUBTERFUGE, difficulty);
				send_to_char("#cAttempting to Masquerade as a human#n\n\r", ch);
				
				if (char_success < 0)
					char_success = -1;	
				
				sprintf(buf, "#cYou had #C%d#c successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char(buf, ch);
				
				if (char_success < 0)
				 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #RBotched#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#C$n#c has #RBotched#c their attempt to Masquerade as a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);	
			}
				else if (char_success == 0)
				 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #CFailed#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#n$n looks a bit ill." );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else
				{
			  	
				{
				sprintf (buf, "#C[#cLog Masq#C: #cThe Character got #C%d#c successes.#C]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#C$n#c got #C%d#c successes Masquerading as a human. Effects last for one scene.\n\r", char_success);
			}	
					
			}
		
			else
			{
				char_success = diceroll (ch, ATTRIB_CHA, TALENTS, TAL_SUBTERFUGE, difficulty);
				send_to_char ("#cAttempting to Masquerade as a human#n\n\r", ch);

				if (char_success <0)
					char_success = -1;

				sprintf (buf, "#cYou had #C%d#c successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char (buf, ch);
		
				if (char_success < 0)
				 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #RBotched#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#C$n#c has #RBotched#c their attempt to Masquerade as a human.#n\n\r" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else if (char_success == 0)
				 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #CFailed#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#n$n looks a bit ill." );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				
				else
				{
			  	
				{
				sprintf (buf, "#C[#cLog Masq#C: #cThe Character got #C%d#c successes.#C]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#C$n#c got #C%d#c successes Masquerading as a human. Effects last for one scene.\n\r", char_success);
			}	
					
			}
		}			
			
		else if (!str_cmp(ch->clan, "Cappadocian"))
		{
			difficulty += 3;
			
			if (ch->pcdata->road == ROAD_HUMANITY)
			{	
				char_success = diceroll (ch, ATTRIB_CHA, TALENTS, TAL_SUBTERFUGE, difficulty);
				send_to_char ("#cAttempting to Masquerade as a human#n\n\r", ch);
	
				if (char_success < 0)
					char_success = -1;
		
				sprintf (buf, "#cYou had #C%d#c successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char (buf, ch);
				
				if (char_success < 0)
		        	 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #RBotched#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#C$n#c has #RBotched#c their attempt to Masquerade as a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else if (char_success == 0)
				 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #CFailed#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#n$n looks a bit ill." );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else
				{
			  	
				{
				sprintf (buf, "#C[#cLog Masq#C: #cThe Character got #C%d#c successes.#C]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#C$n#c got #C%d#c successes Masquerading as a human. Effects last for one scene.\n\r", char_success);
			}	
				
			}
				
			else
			{
				char_success = diceroll (ch, ATTRIB_CHA, TALENTS, TAL_SUBTERFUGE, difficulty);
				send_to_char ("#cAttempting to Masquerade as a human#n\n\r", ch);
	
				if (char_success < 0)
					char_success = -1;
	
				sprintf (buf, "#cYou had #C%d#c successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char (buf, ch);
			
				if (char_success < 0)
				 {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #RBotched#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#C$n#c has #RBotched#c their attempt to Masquerade as a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else if (char_success == 0){
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #CFailed#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#n$n looks a bit ill." );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else
				{
			  	
				{
				sprintf (buf, "#C[#cLog Masq#C: #cThe Character got #C%d#c successes.#C]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#C$n#c got #C%d#c successes Masquerading as a human. Effects last for one scene.\n\r", char_success);
			}	
				
			}
		}
			
		else
		{
			char_success = diceroll (ch, ATTRIB_CHA, TALENTS, TAL_SUBTERFUGE, difficulty);
			send_to_char ("#cAttempting to Masquerade as a human#n\n\r", ch);
			
			if (char_success < 0) 
				char_success = -1;
				
			sprintf (buf, "#cYou had #C%d#c successes. Effects last for one scene.#n\n\r", char_success);
			send_to_char (buf, ch);
				
			if (char_success < 0)
		        {
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #RBotched#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#C$n#c has #RBotched#c their attempt to Masquerade as a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
			else if (char_success == 0){
		          	{
				sprintf( buf, "#C[#cLog Masq#C: #cThe Character has #CFailed#c their Masq roll.#C]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#n$n looks a bit ill." );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
			else
			{
			  	
				{
				sprintf (buf, "#C[#cLog Masq#C: #cThe Character got #C%d#c successes.#C]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#C$n#c got #C%d#c successes Masquerading as a human. Effects last for one scene.\n\r", char_success);
			}	
			
		}
	}
	
	else
	{
		send_to_char ("#cYou have to be a vampire to Masquerade.\n\r", ch);
		
	}
}
/* End Masquerade Code */



void do_proll (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int char_success;
  int doobiesnacks; 
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))	return;

	if (arg[0] == '\0')
	{
			send_to_char ("#cSyntax: Roll Category specific, i.e. roll game poker or roll subterfuge signal\n\r\n\r", ch);
      send_to_char ("#CCategories      #c-     #CSpecific Rolls\n\r", ch);
			send_to_char ("#c-----------------------------------------------\n\r", ch);
      send_to_char ("#CPerformance           Singing#c,#C Acting#c,#C Dancing#c,\n\r", ch);
      send_to_char ("#C                      Instrument#c. \n\r", ch);
      send_to_char ("#CStreetwise            Juggle#c,#C Cheat#c,#C Trick\n\r", ch);
      send_to_char ("#CSubterfuge            Signal\n\r", ch);
      send_to_char ("#CEmpathy               Mood#c, #CComfort\n\r", ch);
      send_to_char ("#CLanguage              Identify\n\r", ch);
      send_to_char ("#CAlertness             #c(#eJust roll alertness#c)\n\r", ch);
      send_to_char ("#CEtiquette             Street#c,#C Business#c,#C Court#c,\n\r", ch);
      send_to_char ("#C                      Lowclass#c,#C Midclass#c,#C Highclass\n\r", ch);
      send_to_char ("#CGame                  Poker#c,#C Dice#c,#C Darts#c,#C Pool\n\r", ch);
      send_to_char ("#CKnowledge             Any Knowledge You Possess\n\r", ch);
		return;
	}

	if (ch->pcdata->evileye > 0)
	{
		if (ch->pcdata->evileye > 1)
			sprintf (buf, "#cYou will be able to roll again in %d seconds.#n\n\r", ch->pcdata->evileye);
		else
			sprintf (buf, "#cOne more second!!#n\n\r");
		send_to_char (buf, ch);
		return;
	}

	if (!str_cmp (arg, "performance"))
	{
		if (!str_cmp (arg2, "instrument") ){
		if(ch->abilities[SKILLS][SKI_PERFORMANCE] > 0) {
			char_success = diceroll (ch, ATTRIB_DEX, SKILLS, SKI_PERFORMANCE, 6);
			send_to_char ("#cRolling for playing an instrument.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to play an instrument.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll for playing an instrument.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[SKILLS][SKI_PERFORMANCE] < 1) {
			char_success = diceroll (ch, ATTRIB_DEX, SKILLS, SKI_PERFORMANCE, 8);
			send_to_char ("#cRolling for playing an instrument without training.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to play an instrument.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll for playing an instrument.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		}
		else if (!str_cmp (arg2, "singing") )
		{
			char_success = diceroll (ch, ATTRIB_CHA, SKILLS, SKI_PERFORMANCE, 6);
			send_to_char ("#cRolling for singing.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c singing.#n.\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes singing.#n\n\r", ch->pcdata->roomdesc, char_success);
			 act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			  ch->pcdata->evileye = 10;
			
			return;
		}
		
		else if (!str_cmp (arg2, "acting") )
		{
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_PERFORMANCE, 6);
			send_to_char ("#cRolling for acting.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c an acting roll.#n.\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes acting.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
		if (!str_cmp (arg2, "dancing") ){
		if(ch->abilities[SKILLS][SKI_PERFORMANCE] > 0) {
			char_success = diceroll (ch, ATTRIB_DEX, SKILLS, SKI_PERFORMANCE, 6);
			send_to_char ("#cRolling for dancing!#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to dance.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll to dance.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[SKILLS][SKI_PERFORMANCE] < 1) {
			char_success = diceroll (ch, ATTRIB_DEX, SKILLS, SKI_PERFORMANCE, 7);
			send_to_char ("#cRolling for dancing without training.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to dance.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll for dancing.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		}
		
		else
		{
		send_to_char ("#cYou may roll for #CPerformance #Csinging#c, #Cinstrument#c, #Cacting#c, or #Cdancing#n", ch);
			return;
		}
	}
	else if (!str_cmp (arg, "language"))
	{
		if (!str_cmp (arg2, "identify") ){
		if(ch->abilities[KNOWLEDGES][KNO_LINGUISTICS] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_LINGUISTICS, 7);
			send_to_char ("#cRolling for Identifying a Language#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c identifying a language.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c  successes identifying a language.#n\n\r", ch->pcdata->roomdesc, char_success);
			ch->pcdata->evileye = 10;
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_LINGUISTICS] > 0) {
			send_to_char ("#cYou can't identify a language without Linguistics.#n\n\r", ch);
			return;
		}
		}
		else if (!str_cmp (arg2, "learn") )
		{
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_LINGUISTICS, 8);
			send_to_char ("#cRolling to learn a new language#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c learning a language.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes towards learning a language. 7 successes over time are needed to learn a language.#n\n\r", ch->pcdata->roomdesc, char_success);
			ch->pcdata->evileye = 10;
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			return;
		}
		else
		{
			send_to_char ("#cYou may roll for #CLanguage #CIdentify#c or #CLearn#c.", ch);
			return;
		}
	}
	else if (!str_cmp (arg, "empathy"))
	{
		if (!str_cmp (arg2, "comfort") )
		{
			char_success = diceroll (ch, ATTRIB_CHA, TALENTS, TAL_EMPATHY, 6);
			send_to_char ("#cRolling to comfort someone.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has#R Botched#c while comforting someone.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes towards comforting someone.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "mood") )
		{
			char_success = diceroll (ch, ATTRIB_PER, TALENTS, TAL_EMPATHY, 6);
			send_to_char ("#cRolling to determine someones mood, or the mood of a room.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c determing the mood of a person or a room.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes determining the mood of a person or a room.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#cYou may roll #CEmpathy #Cmood#c or #Ccomfort#c.", ch);
			return;
		}
	}
	else if (!str_cmp (arg, "subterfuge"))
	{
		 if (!str_cmp (arg2, "signal") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, TALENTS, TAL_SUBTERFUGE, 7);
			send_to_char ("#cRolling to send a subtle hand signal or wink.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c sending a subtle or hidden signal to someone.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes sending a subtle or hidden signal to someone..#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#cYou may roll #CSubterfuge #Csignal#c.", ch);
			return;
		}
	}
	
	else if (!str_cmp (arg, "alertness"))
	{
		 
		
			char_success = diceroll (ch, ATTRIB_PER, TALENTS, TAL_ALERTNESS, 6);
			send_to_char ("#cRolling to notice a covert action.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c noticing a covert action.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes noticing a covert action..#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
	
	
	else if (!str_cmp (arg, "streetwise"))
	{
		if (!str_cmp (arg2, "juggle") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, TALENTS, TAL_STREETWISE, 6);
			send_to_char ("#cRolling to Juggle.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has#R Botched#c their juggling roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes juggling.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "trick") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, TALENTS, TAL_STREETWISE, 7);
			send_to_char ("#cRolling to do a simple 'magic' trick.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c doing a 'magic' trick.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes doing a 'magic' trick.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if (!str_cmp (arg2, "cheat") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, TALENTS, TAL_STREETWISE, 6);
			send_to_char ("#cRolling to stack a deck or cheat with dice.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has#R Botched#c while cheating with cards or dice.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes stacking a deck or cheating with dice.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#cYou may roll #CStreetwise #Ctrick#c, #Ccheat#c or #Cjuggle#c.", ch);
			return;
		}
	}
	
	else if (!str_cmp (arg, "shift"))
	{
		if (!str_cmp (arg2, "homid") )
		{
			char_success = diceroll (ch, ATTRIB_STA, SKILLS, SKI_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to homid form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to homid.\n\r", ch);
				
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has#R Botched#c shifting to homid. Please inform the storytellers.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes shifting to homid form.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "glabro") )
		{
			char_success = diceroll (ch, ATTRIB_STA, SKILLS, SKI_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to glabro form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to glabro.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to glabro. Please inform the storytellers.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to glabro form.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "crinos") )
		{
			char_success = diceroll (ch, ATTRIB_STA, SKILLS, SKI_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to crinos form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to crinos.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to crinos. Please inform the storytellers.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to crinos form.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "hispo") )
		{
			char_success = diceroll (ch, ATTRIB_STA, SKILLS, SKI_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to hispo form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to hispo.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to hispo. Please inform the storytellers.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to hispo form.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "lupus") )
		{
			char_success = diceroll (ch, ATTRIB_STA, SKILLS, SKI_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to lupus form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to lupus.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to lupus. Please inform the storytellers.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to lupus form.#n\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#cYou may roll #Cshift #Chomid#c, #Cglabro#c, #Ccrinos#c, #Chispo#c or #Clupus#c.", ch);
			return;
		}
	}
	
	/*Begin new roll code*/
	else if (!str_cmp (arg, "knowledge"))
	{
		if (!str_cmp (arg2, "science") ){
		if(ch->abilities[KNOWLEDGES][KNO_SCIENCE] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_SCIENCE, 6);
			send_to_char ("#cRolling for General Science Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Science Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their General Science Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_SCIENCE] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		if (!str_cmp (arg2, "academics") ){
		if(ch->abilities[KNOWLEDGES][KNO_ACADEMICS] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_ACADEMICS, 6);
			send_to_char ("#cRolling for Academics Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c an Academics Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Academics Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_ACADEMICS] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "computer") ){
		if(ch->abilities[KNOWLEDGES][KNO_COMPUTER] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_COMPUTER, 6);
			send_to_char ("#cRolling for Computer knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Computer Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Computer Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_COMPUTER] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "investigation") ){
		if(ch->abilities[KNOWLEDGES][KNO_INVESTIGATION] > 0) {
			char_success = diceroll (ch, ATTRIB_WIT, KNOWLEDGES, KNO_INVESTIGATION, 7);
			send_to_char ("#cRolling for Investigation.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c an Investigation roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Investigation roll.\n\r", ch->pcdata->roomdesc, char_success);
			logchan( buf, CHANNEL_LOG_DICE );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_INVESTIGATION] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "law") ){
		if(ch->abilities[KNOWLEDGES][KNO_LAW] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_LAW, 6);
			send_to_char ("#cRolling for Law Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Law Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Law Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_LAW] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "medicine") ){
		if(ch->abilities[KNOWLEDGES][KNO_MEDICINE] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_MEDICINE, 6);
			send_to_char ("#cRolling for Medicine Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Medicine Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Medicine Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_MEDICINE] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "occult") ){
		if(ch->abilities[KNOWLEDGES][KNO_OCCULT] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_OCCULT, 7);
			send_to_char ("#cRolling for Occult Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c an Occult Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Occult Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_OCCULT] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "politics") ){
		if(ch->abilities[KNOWLEDGES][KNO_POLITICS] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_POLITICS, 6);
			send_to_char ("#cRolling for Political Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Political Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Political Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_POLITICS] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "finance") ){
		if(ch->abilities[KNOWLEDGES][KNO_FINANCE] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, KNOWLEDGES, KNO_FINANCE, 6);
			send_to_char ("#cRolling for Finance Knowledge.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Finance Knowledge roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Finance Knowledge roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[KNOWLEDGES][KNO_FINANCE] < 1) {
			send_to_char ("#cYou can not get a knowledge roll if you don't have the knowledge#n\n\r", ch);
			return;
		}
		}
		
		else	
		{
send_to_char ("#cYou may roll for any knowledge you possess. I.E. roll knowledge hearthwisdom", ch);
return;
}}


else if (!str_cmp (arg, "etiquette")){
	
		if (ch->abilities[SKILLS][SKI_ETIQUETTE] <1){
			if (!str_cmp (arg2, "court") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 7);
			send_to_char ("#cRolling for Court Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Court Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Court Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
		if (!str_cmp (arg2, "business") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 8);
			send_to_char ("#cRolling for Business Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Business Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Business Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "Street") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 8);
			send_to_char ("#cRolling for Street Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Street Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Street Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "highclass") ){
			char_success = diceroll (ch, ATTRIB_CHA, SKILLS, SKI_ETIQUETTE, 9);
			send_to_char ("#cRolling for High Class Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a High Class Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their High Class Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "midclass") ){
			char_success = diceroll (ch, ATTRIB_CHA, SKILLS, SKI_ETIQUETTE, 7);
			send_to_char ("#cRolling for Mid Class Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Middle Class Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Middle Class Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "lowclass") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 7);
			send_to_char ("#cRolling for Low Class Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Low Class Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Low Class Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
	}
		else{
		
		if (!str_cmp (arg2, "court") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 6);
			send_to_char ("#cRolling for Court Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Court Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Court Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
		if (!str_cmp (arg2, "business") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 7);
			send_to_char ("#cRolling for Business Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Business Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Business Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "Street") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 7);
			send_to_char ("#cRolling for Street Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Street Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Street Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "highclass") ){
			char_success = diceroll (ch, ATTRIB_CHA, SKILLS, SKI_ETIQUETTE, 8);
			send_to_char ("#cRolling for High Class Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a High Class Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their High Class Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "midclass") ){
			char_success = diceroll (ch, ATTRIB_CHA, SKILLS, SKI_ETIQUETTE, 6);
			send_to_char ("#cRolling for Mid Class Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Middle Class Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Middle Class Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
				if (!str_cmp (arg2, "lowclass") ){
			char_success = diceroll (ch, ATTRIB_MAN, SKILLS, SKI_ETIQUETTE, 6);
			send_to_char ("#cRolling for Low Class Etiquette.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a Low Class Etiquette roll.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their Low Class Etiquette roll.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
	
else	
			{
			send_to_char ("#cYou may roll #Cetiquette court#c, #Cstreet#c, #Cbusiness#c, #Chighclass#c. #Clowclass#c, #Cmidclass#c.", ch);
			return;
   		}
		}
	}

else if (!str_cmp (arg, "game")){
	

			if (!str_cmp (arg2, "pool") ){
			char_success = diceroll (ch, ATTRIB_PER, TALENTS, TAL_ATHLETICS, 6);
			send_to_char ("#cRolling for playing pool.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to play pool.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll to play pool.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
			if (!str_cmp (arg2, "dice") ){
			char_success = diceroll (ch, ATTRIB_DEX, TALENTS, TAL_STREETWISE, 6);
			send_to_char ("#cRolling for shooting dice.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to shoot dice.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll to shoot dice.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
			if (!str_cmp (arg2, "darts") ){
			char_success = diceroll (ch, ATTRIB_DEX, TALENTS, TAL_ATHLETICS, 6);
			send_to_char ("#cRolling for tossing darts#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to toss darts.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll to toss darts.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
			if (!str_cmp (arg2, "poker") ){
			doobiesnacks = ((int) (rand() % 7)) - 2;
			char_success = (diceroll (ch, ATTRIB_MAN, TALENTS, TAL_SUBTERFUGE, 6)) + doobiesnacks;
			send_to_char ("#cRolling to play a hand of poker.#n\n\r", ch);
			if (char_success < 0) char_success = -1;
			sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c a roll to play a hand of poker.#n\n\r", ch->pcdata->roomdesc);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes on their roll to play poker.\n\r", ch->pcdata->roomdesc, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
		
		else	
			{
			send_to_char ("#cYou may roll #Cgame pool#c, #Cdice#c, #Cpoker#c, #Cdarts#c.", ch);
			return;
   		}
		}
		

/*end roll code*/
else
			send_to_char ("#cSyntax: Roll Category specific, i.e. roll game poker or roll subterfuge signal\n\r\n\r", ch);
      send_to_char ("#CCategories      #c-     #CSpecific Rolls\n\r", ch);
			send_to_char ("#c-----------------------------------------------\n\r", ch);
      send_to_char ("#CPerformance           Singing#c,#C Acting#c,#C Dancing#c,\n\r", ch);
      send_to_char ("#C                      Instrument#c. \n\r", ch);
      send_to_char ("#CStreetwise            Juggle#c,#C Cheat#c,#C Trick\n\r", ch);
      send_to_char ("#CSubterfuge            Signal\n\r", ch);
      send_to_char ("#CEmpathy               Mood#c, #CComfort\n\r", ch);
      send_to_char ("#CLanguage              Identify\n\r", ch);
      send_to_char ("#CAlertness             #c(#eJust roll alertness#c)\n\r", ch);
      send_to_char ("#CEtiquette             Street#c,#C Business#c,#C Court#c,\n\r", ch);
      send_to_char ("#C                      Lowclass#c,#C Midclass#c,#C Highclass\n\r", ch);
      send_to_char ("#CGame                  Poker#c,#C Dice#c,#C Darts#c,#C Pool\n\r", ch);
      send_to_char ("#CKnowledge             Any Knowledge You Possess\n\r", ch);

	return;
}

void do_areset( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	AREA_DATA *pArea = ch->in_room->area;
	ROOM_INDEX_DATA *cur_room;
	int top, bottom, curr;

	one_argument( argument, arg );

	if (arg[0] != '\0' && is_number( arg ) )
	{
	  curr = atoi( arg );
	  for( pArea = area_first; pArea != NULL; pArea = pArea->next )
		if( pArea->vnum == curr )
		  break;
		if( pArea == NULL )
		  pArea = ch->in_room->area;
	}

	reset_area ( pArea );
	bottom = pArea->lvnum;
	top = pArea->uvnum;
	for( curr = bottom; curr <= top; curr++ )
	{
	  if( ( cur_room = get_room_index( curr ) ) != NULL )
	  {
		if( cur_room->people )
		{
		  act( "Your vision blurs, when it restores, things"
		  " are different.", cur_room->people, NULL, NULL, TO_ROOM );
		  act( "Your vision blurs, when it restores, things"
		  " are different.", cur_room->people, NULL, NULL, TO_CHAR );
		}
	  }
	}

	sprintf( buf, "%s (%d) reset.", pArea->name, pArea->vnum );
	strcat( buf, "\n\r" );
	send_to_char( buf, ch );
	return;
}

void do_abilities (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int i;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	send_to_char ("#c[     #CTalents      #c][      #CSkills#c      ][   #C Knowledges#c    ]#n\n\r", ch);
	for (i = 0; i < 10; i++)
	{
		sprintf (buf, "#c[#C%-15s #B%d#c ][#C%-15s #B%d#c ][#C%-15s #B%d#c ]#n\n\r", ability_names[TALENTS][i][0], ch->abilities[TALENTS][i], ability_names[SKILLS][i][0], ch->abilities[SKILLS][i], ability_names[KNOWLEDGES][i][0], ch->abilities[KNOWLEDGES][i]);
		send_to_char (buf, ch);
	}
	return;
}



void do_language (CHAR_DATA * ch, char *argument, int language)
{
	CHAR_DATA *och;
	MEMORY_DATA * temp_mem;
	int chance;
	int chance2;
	int language2;
	char *lan_str;
	char buf[MAX_INPUT_LENGTH];
	char poly[MAX_STRING_LENGTH];

	if (language == ENGLISH)
	{
		lan_str = "english";
		language2 = gsn_english;
	}
	else if (language == GERMAN)
	{
		lan_str = "german";
		language2 = gsn_german;
	}
	else if (language == VIETNAMESE)
	{
		lan_str = "vietnamese";
		language2 = gsn_vietnamese;
	}
	else if (language == RUSSIAN)
	{
		lan_str = "russian";
		language2 = gsn_russian;
	}
	else if (language == ARABIC)
	{
		lan_str = "arabic";
		language2 = gsn_arabic;
	}
	else if (language == FRENCH)
	{
		lan_str = "french";
		language2 = gsn_french;
	}
	else if (language == LATIN)
	{
		lan_str = "latin";
		language2 = gsn_latin;
	}
	else if (language == SPANISH)
	{
		lan_str = "spanish";
		language2 = gsn_spanish;
	}
	else if (language == ITALIAN)
	{
		lan_str = "italian";
		language2 = gsn_italian;
	}
	else if (language == HEBREW)
	{
		lan_str = "hebrew";
		language2 = gsn_hebrew;
	}
	else if (language == GAELIC)
	{
		lan_str = "gaelic";
		language2 = gsn_gaelic;
	}
	else if (language == INDONESIAN)
	{
		lan_str = "indonesian";
		language2 = gsn_indonesian;
	}
	else if (language == JAPANESE)
	{
		lan_str = "japanese";
		language2 = gsn_japanese;
	}
	else if (language == GREEK)
	{
		lan_str = "greek";
		language2 = gsn_greek;
	}
	else if (language == SLAVIC)
	{
		lan_str = "slavic";
		language2 = gsn_slavic;
	}
	else if (language == GAROU)
	{
		lan_str = "garou";
		language2 = gsn_garou;
	}

	else if (language == KOREAN)
	{
		lan_str = "korean";
		language2 = gsn_korean;
	}

	else if (language == CHINESE)
	{
		lan_str = "chinese";
		language2 = gsn_chinese;
	}
	else if (language == SWAHILI)
	{
		lan_str = "swahili";
		language2 = gsn_swahili;
	}
	else if (language == TAGALOG)
	{
		lan_str = "tagalog";
		language2 = gsn_tagalog;
	}
	else if (language == HINDU)
	{
		lan_str = "hindu";
		language2 = gsn_hindu;
	}
	
	buf[0] = '\0';

	if (IS_MORE3(ch, MORE3_OBFUS1))
		do_obfuscate1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS2))
		do_obfuscate2(ch,argument);
	
	/* Now find out if we can speak it... */
	if ((chance = ch->pcdata->learned[language2]) == 0)
	{
		sprintf (buf, "You don't know how to speak %s...how did you pick this in the first place?\n\r", lan_str);
		send_to_char (buf, ch);
		return;
	}
	if (number_percent () <= chance)
	{
		
		buf[0] = '\0';
		sprintf (buf, "#cYou say in %s #B'#3%s#B' #c(#C%s#c)#n\n\r", ch->pcdata->voice, argument, lan_str);
		send_to_char (buf, ch);
		
		
		if (ch->pcdata->learned[language2] < 100)
		{

			ch->pcdata->learned[language2] += ch->abilities[KNOWLEDGES][KNO_LINGUISTICS];

			if (ch->pcdata->learned[language2] == 100)
			{
				send_to_char ("You have mastered that language!\n\r", ch);
			}
			if (ch->pcdata->learned[language2] > 100)
				ch->pcdata->learned[language2] = 100;
		}
		for (och = ch->in_room->people; och != NULL; och = och->next_in_room)
		{
		        if (IS_NPC (ch))
		                sprintf (poly, ch->short_descr);
		        else if (IS_STORYTELLER(och))
		        				sprintf(poly, "%s", ch->name);
		        else if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
		                sprintf (poly, "#C%s#c#n", ch->morph);
		        else
		        {
		                temp_mem = memory_search_real_name(och->memory, ch->name);
				
				if (IS_STORYTELLER(och))
		        				sprintf(poly, "%s", ch->name);
				else if (temp_mem == NULL)
					sprintf(poly, ch->pcdata->roomdesc);
				else
					sprintf(poly, temp_mem->remembered_name);
		        }


			if (!IS_NPC (och) && (och != ch) && (ch->plane == och->plane || IS_STORYTELLER(och)))
			{
				if (IS_SET(och->act, PLR_CHAMPION) || IS_STORYTELLER(och))
				{
					chance2 = 100;
				} else {
					chance2 = och->pcdata->learned[language2];
				}
				if (chance2 == 0)
				{
					if (IS_WEREWOLF (ch) && language == GAROU)
						act ("$n growls and yelps#n", ch, NULL, och, TO_VICT);
					else
						act ("$n says something in a strange tongue.", ch, NULL, och, TO_VICT);
				}
				else if (number_percent () <= chance2)
				{
					if ((ch->pcdata->voice != NULL) && (strlen (ch->pcdata->voice) > 3))
						{
						buf[0] = '\0';
						sprintf (buf, "#C%s#c says in %s, #B'#3%s#B'#c #c(#C%s#c)#n\n\r", poly, ch->pcdata->voice, argument, lan_str);
						send_to_char (buf, och);
						}
					else{
						buf[0] = '\0';
						sprintf (buf, "#C%s#c says, #B'#3%s#B'#c #c(#C%s#c)#n\n\r", poly, argument, lan_str);
						send_to_char (buf, och);
					    }
				}
				else
				{
					buf[0] = '\0';
					sprintf (buf, "#C%s#c says something in a language you can't understand.#n\n\r", poly);
					send_to_char (buf, och);
				}
			}
		}
	}
	else
	{
		buf[0] = '\0';
		sprintf (buf, "#cYou try to say #B'#3%s#B'#c in #C%s#c, but it doesn't sound correct.#n\n\r", argument, lan_str);
		send_to_char (buf, ch);
		for (och = ch->in_room->people; och != NULL; och = och->next_in_room)
		{
                        if (IS_NPC (ch))
                                sprintf (poly, ch->short_descr);
                        else if (!IS_NPC (ch) && IS_AFFECTED (ch, AFF_POLYMORPH))
                                sprintf (poly, "#C%s#c#n", ch->morph);
                        else
                        {
                                temp_mem = memory_search_real_name(och->memory, ch->name);
                                if (temp_mem == NULL)
                                        sprintf(poly, ch->pcdata->roomdesc);
                                else
                                        sprintf(poly, temp_mem->remembered_name);
                        }


			if (!IS_NPC (och) && (och != ch))
			{

				if ((chance2 = och->pcdata->learned[language2]) == 0)
				{
					if (IS_WEREWOLF (ch) && language == GAROU)
						act ("#c$n whines and barks#n", ch, NULL, och, TO_VICT);
					else
						act ("$n says something in a strange tongue.", ch, NULL, och, TO_VICT);
				}
				else if (number_percent () <= chance2)
				{
					buf[0] = '\0';
					sprintf (buf, "#cIn a weird form of #C%s#c, #C%s#c says something uncomprehensible.#n\n\r", lan_str, poly);
					send_to_char (buf, och);
				}
				else
				{
					buf[0] = '\0';
					sprintf (buf, "#C%s#c says something in #C%s#c you can't understand.#n\n\r", poly, lan_str);
					send_to_char (buf, och);
				}
			}
		}
		if (ch->pcdata->learned[language2] < 100)
		{
			ch->pcdata->learned[language2] += ch->abilities[KNOWLEDGES][KNO_LINGUISTICS];
			if (ch->pcdata->learned[language2] == 100)
			{
				send_to_char ("You have mastered a new language!\n\r", ch);
			}

			if (ch->pcdata->learned[language2] > 100)
				ch->pcdata->learned[language2] = 100;
		}
	}
}

void do_side (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (strlen (ch->side) > 1)
	{
		send_to_char ("But you have already chosen a sect!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose a sect:  Autarkis, Furores, Inconnu and Promethians.\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "Autarkis"))
	{
		free_string (ch->side);
		ch->side = str_dup ("Autarkis");
		send_to_char ("You are now an Autarkis.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "Furores"))
	{
		free_string (ch->side);
		ch->side = str_dup ("Furores");
		send_to_char ("You are now a member of the Furores.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "Inconnu"))
	{

		if (!str_cmp (ch->clan, "Toreador") || !str_cmp (ch->clan, "Tzimisce") || !str_cmp (ch->clan, "Brujah"))
		{
			send_to_char ("Your kind are not welcome in the Inconnu.\n\r", ch);
			return;
		}


		free_string (ch->side);
		ch->side = str_dup ("Inconnu");
		send_to_char ("You are now a member of the Inconnu.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "Promethians"))
	{
		if (!str_cmp (ch->clan, "baali") || !str_cmp (ch->clan, "tzimisce"))
		{
			send_to_char ("Your kind are not welcome in the Promethians.\n\r", ch);
			return;
		}

		free_string (ch->side);
		ch->side = str_dup ("Promethians");
		send_to_char ("You are now a member of the Promethians.\n\r", ch);
		return;
	}
	else
		send_to_char ("Please choose a sect:  Autarkis, Furores, Inconnu and Promethians.\n\r", ch);
	return;
}


void do_discset (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int disc = 0;
	sh_int value = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax : setdisc [victim] [disc/all] [level]\n\r\n\r", ch);
		send_to_char (" Disc being one of:\n\r", ch);
		send_to_char ("  animalism auspex celerity chimerstry\n\r", ch);
		send_to_char ("  daimoinon dominate fortitude melpominee\n\r", ch);
		send_to_char ("  necromancy obeah obfuscate obtenebration\n\r", ch);
		send_to_char ("  potence presence protean quietus serpentis\n\r", ch);
		send_to_char ("  thanatosis thaumaturgy vicissitude\n\r", ch);
		send_to_char ("  Temporis Mortis spiritus Mytherceria\n\r", ch);
		send_to_char ("  Abombwe Deimos Dementation Grotestquous\n\r", ch);
		send_to_char ("  Nihilistics Ma Valeren bardo\n\r", ch);
		send_to_char ("  Visceratika SophiaPatris \n\r", ch );
		send_to_char ("  SedulitasFilius BoniSpiritus\n\r", ch);
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

	if (!str_cmp (arg2, "all"))
	{
		value = is_number (arg3) ? atoi (arg3) : -1;

		if (value < 0 || value > 10)
		{
			send_to_char ("Range is 0 - 10.\n\r", ch);
			return;
		}

		if (value == 0)
			value = -1;

		for (disc = 0; disc <= DISC_MAX; disc++)
			victim->pcdata->powers[disc] = value;

		send_to_char ("Done.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg2, "animalism"))
		disc = DISC_ANIMALISM;
	else if (!str_cmp (arg2, "auspex"))
		disc = DISC_AUSPEX;
	else if (!str_cmp (arg2, "celerity"))
		disc = DISC_CELERITY;
	else if (!str_cmp (arg2, "chimerstry"))
		disc = DISC_CHIMERSTRY;
	else if (!str_cmp (arg2, "bardo"))
		disc = DISC_BARDO;
	else if (!str_cmp (arg2, "daimoinon"))
		disc = DISC_DAIMOINON;
	else if (!str_cmp (arg2, "dominate"))
		disc = DISC_DOMINATE;
	else if (!str_cmp (arg2, "fortitude"))
		disc = DISC_FORTITUDE;
	else if (!str_cmp (arg2, "melpominee"))
		disc = DISC_MELPOMINEE;
	else if (!str_cmp (arg2, "mytherceria"))
		disc = DISC_MYTHERCERIA;
	else if (!str_cmp (arg2, "necromancy"))
		disc = DISC_NECROMANCY;
	else if (!str_cmp (arg2, "obeah"))
		disc = DISC_OBEAH;
	else if (!str_cmp (arg2, "obfuscate"))
		disc = DISC_OBFUSCATE;
	else if (!str_cmp (arg2, "obtenebration"))
		disc = DISC_OBTENEBRATION;
	else if (!str_cmp (arg2, "potence"))
		disc = DISC_POTENCE;
	else if (!str_cmp (arg2, "presence"))
		disc = DISC_PRESENCE;
	else if (!str_cmp (arg2, "protean"))
		disc = DISC_PROTEAN;
	else if (!str_cmp (arg2, "quietus"))
		disc = DISC_QUIETUS;
	else if (!str_cmp (arg2, "serpentis"))
		disc = DISC_SERPENTIS;
	else if (!str_cmp (arg2, "thanatosis"))
		disc = DISC_THANATOSIS;
	else if (!str_cmp (arg2, "thaumaturgy"))
		disc = DISC_THAUMATURGY;
	else if (!str_cmp (arg2, "vicissitude"))
		disc = DISC_VICISSITUDE;
	else if (!str_cmp (arg2, "temporis"))
		disc = DISC_TEMPORIS;
	else if (!str_cmp (arg2, "mortis"))
		disc = DISC_MORTIS;
	else if (!str_cmp (arg2, "abombwe"))
		disc = DISC_ABOMBWE;
	else if (!str_cmp (arg2, "deimos"))
		disc = DISC_DEIMOS;
	else if (!str_cmp (arg2, "dementation"))
		disc = DISC_DEMENTATION;
//       else if ( !str_cmp( arg2, "grotesquous" ) ) disc = DISC_GROTESQUOUS;
//       else if ( !str_cmp( arg2, "nihilistics" ) ) disc = DISC_NIHILISTICS;
	else if (!str_cmp (arg2, "ma"))
		disc = DISC_MA;
	else if (!str_cmp (arg2, "valeren"))
		disc = DISC_VALEREN;
	else if (!str_cmp (arg2, "spiritus"))
		disc = DISC_SPIRITUS;
	else if (!str_cmp (arg2, "visceratika"))
		disc = DISC_VISCERATIKA;
	else if (!str_cmp (arg2, "sophiapatris"))
		disc = DISC_SOPHIA_PATRIS;
	else if (!str_cmp (arg2, "sedulitasfilius"))
		disc = DISC_SEDULITAS_FILIUS;
	else if (!str_cmp (arg2, "bonispiritus"))
		disc = DISC_BONI_SPIRITUS;	
	else
	{
		do_discset (ch, "");
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;
//       value = is_number( arg3 );

	if (value < 0 || value > 10)
	{
		send_to_char ("Range is 0 - 10.\n\r", ch);
		return;
	}
	victim->pcdata->powers[disc] = value;
	send_to_char ("Done.\n\r", ch);
	return;
}


void do_setbreed (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int breed = 0;
	sh_int value = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax : setbreed [victim] [breed/all] [level]\n\r\n\r", ch);
		send_to_char (" Breed being one of:\n\r", ch);
		send_to_char ("  Homid, Lupus, or Metis\n\r", ch);
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

	if (!str_cmp (arg2, "all"))
	{
		value = is_number (arg3) ? atoi (arg3) : -1;

		if (value < 0 || value > 40)
		{
			send_to_char ("Range is 0 - 40.\n\r", ch);
			return;
		}

		if (value == 0)
			value = -1;

		for (breed = 0; breed <= BREED_MAX; breed++)
			victim->pcdata->breed[breed] = value;

		send_to_char ("Done.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg2, "lupus"))
		breed = BREED_LUPUS;
	else if (!str_cmp (arg2, "homid"))
		breed = BREED_HOMID;
	else if (!str_cmp (arg2, "metis"))
		breed = BREED_METIS;
	else
	{
		do_setbreed (ch, "");
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;

	if (value < 0 || value > 40)
	{
		send_to_char ("Range is 0 - 40.\n\r", ch);
		return;
	}
	victim->pcdata->breed[breed] = value;
	send_to_char ("Done.\n\r", ch);
	return;
}


void do_setauspice (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int auspice = 0;
	sh_int value = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax : setauspice [victim] [auspice/all] [level]\n\r\n\r", ch);
		send_to_char (" Auspice being one of:\n\r", ch);
		send_to_char ("  Ragabash, Theurge, Philodox, Galliard, or Ahroun.\n\r", ch);
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

	if (!str_cmp (arg2, "all"))
	{
		value = is_number (arg3) ? atoi (arg3) : -1;

		if (value < 0 || value > 40)
		{
			send_to_char ("Range is 0 - 40.\n\r", ch);
			return;
		}

		if (value == 0)
			value = -1;

		for (auspice = 0; auspice <= AUSPICE_MAX; auspice++)
			victim->pcdata->auspice[auspice] = value;

		send_to_char ("Done.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg2, "ragabash"))
		auspice = AUSPICE_RAGABASH;
	else if (!str_cmp (arg2, "theurge"))
		auspice = AUSPICE_THEURGE;
	else if (!str_cmp (arg2, "philodox"))
		auspice = AUSPICE_PHILODOX;
	else if (!str_cmp (arg2, "galliard"))
		auspice = AUSPICE_GALLIARD;
	else if (!str_cmp (arg2, "ahroun"))
		auspice = AUSPICE_AHROUN;
	else
	{
		do_setauspice (ch, "");
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;

	if (value < 0 || value > 40)
	{
		send_to_char ("Range is 0 - 40.\n\r", ch);
		return;
	}
	victim->pcdata->auspice[auspice] = value;
	send_to_char ("Done.\n\r", ch);
	return;
}


void do_settribe (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int tribes = 0;
	sh_int value = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("Syntax : settribe [victim] [tribe/all] [level]\n\r\n\r", ch);
		send_to_char (" Tribe being one of:\n\r", ch);
		send_to_char ("  Blackfuries, Bonegnawers, Childrenofgaia, Fianna, Getofffenris,\n\r", ch);
		send_to_char ("  Glasswalkers, Redtalons, Shadowlords, Silentstriders, Silverfangs,\n\r", ch);
		send_to_char ("  Stargazers, Uktena, or Wendigos.\n\r", ch);
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

	if (!str_cmp (arg2, "all"))
	{
		value = is_number (arg3) ? atoi (arg3) : -1;

		if (value < 0 || value > 40)
		{
			send_to_char ("Range is 0 - 40.\n\r", ch);
			return;
		}

		if (value == 0)
			value = -1;

		for (tribes = 0; tribes <= TRIBE_MAX; tribes++)
			victim->pcdata->tribes[tribes] = value;

		send_to_char ("Done.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg2, "blackfuries"))
		tribes = TRIBE_BLACK_FURIES;
	else if (!str_cmp (arg2, "bonegnawers"))
		tribes = TRIBE_BONE_GNAWERS;
	else if (!str_cmp (arg2, "childrenofgaia"))
		tribes = TRIBE_CHILDREN_OF_GAIA;
	else if (!str_cmp (arg2, "fianna"))
		tribes = TRIBE_FIANNA;
	else if (!str_cmp (arg2, "getoffenris"))
		tribes = TRIBE_GET_OF_FENRIS;
	else if (!str_cmp (arg2, "glasswalkers"))
		tribes = TRIBE_GLASS_WALKERS;
	else if (!str_cmp (arg2, "redtalons"))
		tribes = TRIBE_RED_TALONS;
	else if (!str_cmp (arg2, "shadowlords"))
		tribes = TRIBE_SHADOW_LORDS;
	else if (!str_cmp (arg2, "silentstriders"))
		tribes = TRIBE_SILENT_STRIDERS;
	else if (!str_cmp (arg2, "silverfangs"))
		tribes = TRIBE_SILVER_FANGS;
	else if (!str_cmp (arg2, "stargazers"))
		tribes = TRIBE_STARGAZERS;
	else if (!str_cmp (arg2, "uktena"))
		tribes = TRIBE_UKTENA;
	else if (!str_cmp (arg2, "wendigos"))
		tribes = TRIBE_WENDIGOS;
	else
	{
		do_setauspice (ch, "");
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;

	if (value < 0 || value > 40)
	{
		send_to_char ("Range is 0 - 40.\n\r", ch);
		return;
	}
	victim->pcdata->tribes[tribes] = value;
	send_to_char ("Done.\n\r", ch);
	return;
}


void do_truesight (CHAR_DATA * ch, char *argument)
{
	int vision;

	if (IS_NPC (ch))
		return;
	if (get_curr_per (ch) > 4)
		vision = get_curr_per (ch);
	else
		vision = 4;

	if (!IS_VAMPIRE (ch) && !IS_GHOUL (ch) && !IS_WEREWOLF (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (IS_WEREWOLF (ch) && get_breed (ch, BREED_LUPUS) < 1)
	{
		send_to_char ("You require the level 1 Lupis power to use Heightened Senses.\n\r", ch);
		return;
	}

	if (get_disc (ch, DISC_AUSPEX) < 1 && (IS_VAMPIRE (ch) || IS_GHOUL (ch)))
	{
		send_to_char ("You require level 1 Auspex to use Heightened Senses.\n\r", ch);

		return;
	}

	if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
	{
		REMOVE_BIT (ch->more2, MORE2_HEIGHTENSENSES);
		send_to_char ("Your senses return to normal.\n\r", ch);
		ch->pcdata->vision = 3;
	}
	else
	{
		SET_BIT (ch->more2, MORE2_HEIGHTENSENSES);
		send_to_char ("Your senses increase to incredible proportions.\n\r", ch);
		ch->pcdata->vision = vision;
	}
	return;
}

void do_bonecraft (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	if (IS_NPC (ch))
		return;

	one_argument (argument, arg1);
	smash_tilde (argument);

	if (get_trust(ch) < LEVEL_STORYTELLER)
	{
	if (!IS_VAMPIRE (ch) && !IS_GHOUL (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (get_disc (ch, DISC_VICISSITUDE) < 3)
	{
		send_to_char ("You require level 3 Vicissitude to use Bonecraft.\n\r", ch);
		return;
	}
	}
	if (argument[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char ("Change to look like whom?\n\r", ch);
		return;
	}

	if (!str_cmp (arg1, "mod") || !str_cmp (arg1, "modify"))
	{
		argument = one_argument (argument, arg1);
		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char ("Syntax: Bonecraft Modify <target> <modification>\n\r", ch);
			return;
		}
		if ((victim = get_char_room (ch, arg2)) == NULL)
		{
			send_to_char ("They are not here.\n\r", ch);
			return;
		}
		if (IS_NPC (victim) && get_trust(ch) < LEVEL_STORYTELLER)
		{
			send_to_char ("You cannot bonecraft an unwilling person.\n\r", ch);
			return;
		}
		if (!IS_IMMUNE (victim, IMM_VAMPIRE) && ch != victim)
		{
			send_to_char ("You cannot bonecraft an unwilling person.\n\r", ch);
			return;
		}
		return;
	}

	if (strlen (argument) > 30)
	{
		send_to_char ("Please limit your name to 30 characters or less.\n\r", ch);
		return;
	}

	/*if (str_cmp (argument, "self") && !check_parse_name (argument))
	{
		send_to_char ("Illegal name.\n\r", ch);
		return;
	}
	*/
	

	if (IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		send_to_char ("Not while polymorphed.\n\r", ch);
		return;
	}

	if (IS_POLYAFF (ch, POLY_ZULO))
	{
		send_to_char ("Not while in Zulo Form.\n\r", ch);
		return;
	}

	if (!str_cmp (argument, ch->name) || !str_cmp (argument, "self"))
	{
		if (!IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_DISGUISED))
		{
			send_to_char ("You already look like yourself!\n\r", ch);
			return;
		}
		if (!IS_POLYAFF (ch, POLY_BONECRAFT))
		{
			send_to_char ("Nothing happens.\n\r", ch);
			return;
		}
		sprintf (buf, "$n's features twist and distort until $e looks like %s.", ch->name);
		act (buf, ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT (ch->polyaff, POLY_BONECRAFT);
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
		REMOVE_BIT (ch->extra, EXTRA_VICISSITUDE);
		free_string (ch->morph);
		ch->morph = str_dup ("");
		act ("Your features twist and distort until you look like $n.", ch, NULL, NULL, TO_CHAR);
		return;
	}
	if (IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		if (!IS_POLYAFF (ch, POLY_BONECRAFT))
		{
			send_to_char ("Nothing happens.\n\r", ch);
			return;
		}
		sprintf (buf, "Your features twist and distort until you look like %s.\n\r", argument);
		send_to_char (buf, ch);
		sprintf (buf, "$n's features twist and distort until $e looks like %s.", argument);
		act (buf, ch, NULL, NULL, TO_ROOM);
		free_string (ch->morph);
		ch->morph = str_dup (argument);
		return;
	}
	sprintf (buf, "Your features twist and distort until you look like %s.\n\r", argument);
	send_to_char (buf, ch);
	sprintf (buf, "$n's features twist and distort until $e looks like %s.", argument);
	act (buf, ch, NULL, NULL, TO_ROOM);
	SET_BIT (ch->polyaff, POLY_BONECRAFT);
	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->vampaff, VAM_DISGUISED);
	SET_BIT (ch->extra, EXTRA_VICISSITUDE);
	free_string (ch->morph);
	ch->morph = str_dup (argument);
	return;
}


void do_obfuscate3 (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	

	if (IS_NPC (ch))
		return;

	one_argument (argument, arg1);
	smash_tilde (argument);

	if (get_trust(ch) < LEVEL_STORYTELLER)
	{
	if (!IS_VAMPIRE (ch) && !IS_GHOUL (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (get_disc (ch, DISC_OBFUSCATE) < 3)
	{
		send_to_char ("#cYou do not have enough mastery of obfuscate to change your appearance.#n\n\r", ch);
		return;
	}
	}
	if (argument[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char ("Change to look like whom?\n\r", ch);
		return;
	}

	

	if (strlen (argument) > MAX_ROOMDESC_LENGTH)
	{
		send_to_char ("#cName too long.\n\r", ch);
		return;
	}

	/*if (str_cmp (argument, "self") && !check_parse_name (argument))
	{
		send_to_char ("Illegal name.\n\r", ch);
		return;
	}
	*/
	

	if (IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		send_to_char ("Not while polymorphed.\n\r", ch);
		return;
	}

	if (IS_POLYAFF (ch, POLY_ZULO))
	{
		send_to_char ("Not while in Zulo Form.\n\r", ch);
		return;
	}

	if (!str_cmp (argument, ch->name) || !str_cmp (argument, "self"))
	{
		if (!IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_DISGUISED))
		{
			send_to_char ("You already look like yourself!\n\r", ch);
			return;
		}
		if (!IS_POLYAFF (ch, POLY_BONECRAFT))
		{
			send_to_char ("Nothing happens.\n\r", ch);
			return;
		}
		sprintf (buf, "#c$n suddenly looks different!#n\n\r");
		act (buf, ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT (ch->polyaff, POLY_BONECRAFT);
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
		REMOVE_BIT (ch->extra, EXTRA_VICISSITUDE);
		free_string (ch->morph);
		ch->morph = str_dup ("");
		act ("#cYou now look like $n.#n\n\r", ch, NULL, NULL, TO_CHAR);
		return;
	}
	if (IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		if (!IS_POLYAFF (ch, POLY_BONECRAFT))
		{
			send_to_char ("Nothing happens.\n\r", ch);
			return;
		}
		sprintf (buf, "You now look like %s.\n\r", argument);
		send_to_char (buf, ch);
		sprintf (buf, "#c$n suddenly looks different!#n\n\r");
		act (buf, ch, NULL, NULL, TO_ROOM);
		free_string (ch->morph);
		ch->morph = str_dup (argument);
		return;
	}
	sprintf (buf, "#cYou now look like %s.\n\r#n", argument);
	send_to_char (buf, ch);
	sprintf (buf, "#c$n suddenly looks different!#n\n\r");
	act (buf, ch, NULL, NULL, TO_ROOM);
	SET_BIT (ch->polyaff, POLY_BONECRAFT);
	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->vampaff, VAM_DISGUISED);

	free_string (ch->morph);
	ch->morph = str_dup (argument);
	return;
}
