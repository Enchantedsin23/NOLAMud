/*********************************************************
 * This code has been completely revamped for the use of *
 * VtR Role Play MU* Requiem by Muse. To use this code   *
 * provide credit in the greeting or motd.               *
 ********************************************************/

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

size_t strlen_exclude_colors(const char * str)
{
  const char *s;
  size_t count = 0;
  for (s = str; *s; ++s)
    {
    if (s [0] == '#' && isalpha (s [1]))
      ++s;  // skip the letter
    else if (s [0] == '^' && isalpha (s [1]))
      ++s;  // skip the letter
    else
      count++;  // otherwise count the character
    }
  return count;
}

// This file is primarily for Immortal commands

void add_extra_node(CHAR_DATA *victim, char *catagory, char*field, int rating)
{
	EXTRA *extra;
	EXTRA *newextra;
	int found;
	char buf[MSL];
	
	found = 0;
	newextra = (EXTRA*) malloc(sizeof(EXTRA));
	for(extra = victim->pcdata->extra; extra; extra = extra->prev)
	{
		if (!str_cmp(catagory, extra->catagory))
		{
			
			if (victim->pcdata->extra == extra)
				break;
			newextra->next = extra->next;
			if (extra->next)
				extra->next->prev = newextra;
			extra->next = newextra;
			newextra->prev = extra;

			found = 1;
			break;
		}
	}
	if (!found)
	{
		newextra->next = NULL;
		newextra->prev = victim->pcdata->extra;
  		if (newextra->prev)
			newextra->prev->next = newextra;
		victim->pcdata->extra = newextra;
	}
  	newextra->catagory = str_dup(catagory);
  	newextra->field = str_dup(field);
  	newextra->rating = rating;
  	sprintf(buf, "\'%s\' \'%s\' %d", catagory, field, rating);
  	newextra->extra = str_dup(buf);
}

void do_givepower (CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *victim;
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
	add_extra_node(victim, arg2, arg3, atoi(arg4));
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
	DESCRIPTOR_DATA *d; //dummy desc for loading offline characters
	int value;
	int i;
	int clan = -1;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char *pwdnew;
	char *p;
	bool is_offline = FALSE;
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
		send_to_char ("#r -----------------------------------------------#n\n\r", ch);
		send_to_char ("#G Area#n:\n\r", ch);
		send_to_char ("#r -----------------------------------------------#n\n\r", ch);
		send_to_char ("#n Attribute, Mental, Social, Physical, Willpower,\n\r", ch);
		send_to_char ("#n Covenantstatus, Clan, Roadrank, Password, Plasm\n\r", ch);
		send_to_char ("\n\r", ch);
		send_to_char ("#r -----------------------------------------------#n\n\r", ch);
		send_to_char ("#G Field#n:\n\r", ch);
		send_to_char ("#r -----------------------------------------------#n\n\r", ch);
		send_to_char ("#w Attribute#e:#n Strength, Dexterity, Stamina, Presence,\n\r", ch);
		send_to_char ("#n            Manipulation, Composure, Resolve,\n\r", ch);
		send_to_char ("#n            Intelligence, Wits.\n\r", ch);
		send_to_char ("#w Physical#e:#n  Athletics, Brawl, Firearms, Larceny,\n\r", ch);
		send_to_char ("#n            Stealth, Survival, Weaponry, Drive\n\r", ch);
		send_to_char ("#w Social#e:#n    Animal Ken, Empathy, Expression,\n\r", ch);
		send_to_char ("#n            Intimidation, Socialize, Streetwise,\n\r", ch);
		send_to_char ("#n            Persuasion, Subterfuge\n\r", ch);
		send_to_char ("#w Mental#e:  #n  Academics, Computer, Crafts, Investigation,\n\r", ch);
		send_to_char ("#n            Medicine, Occult, Politics, Science\n\r", ch);
		send_to_char ("#w Clan#e:#n      Nosferatu, Gangrel, Ventrue, Daeva,\n\r", ch);
		send_to_char ("#n            Mekhet. (Use for setting Clan)\n\r", ch);
		send_to_char ("#w Roadrank#e:#n  ## (Humanity rating)\n\r", ch);
		send_to_char ("#w Auspice#e:#n   Rahu, Cahalith, Elodoth, Ithaeur, Irraka\n\r", ch);
		send_to_char ("#r -----------------------------------------------#n\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		d = alloc_mem(sizeof(DESCRIPTOR_DATA));
		if (load_char_obj(d, arg1))
		{
			victim = d->character;
			is_offline = TRUE;
		}
		else
		{
			send_to_char("No such character exists.\n\r", ch);
			return;
		}
	}

	/*
	 * Snarf the value (which need not be numeric).
	 */
	value = is_number (arg3) ? atoi (arg3) : -1;

	/*
	 * Set something.
	 */

	if (!str_cmp (arg2, "covenantstatus")){
		
		if (IS_NPC (victim))
		{
			send_to_char( "Bug -- Attempting to set status on NPC\n\r", ch);
		}
		else
		{
			sprintf(buf, "#wP#nlayer#w'#ns#w C#novenant#w S#ntatus #eset to #R%d#e.#n\n\r", value);
			send_to_char(buf, ch);
			victim->pcdata->covenantstatus = value;
		}

}

	//else if (!str_cmp (arg2, "plasm"))
	//{
	//	if (IS_NPC (victim))
	//	{
	//		send_to_char( "Not on NPC's.\n\r", ch);
	//	}
    //
	//	else if (value < 1 || value > 100)
	//	{
	//		send_to_char( "Plasm range is 1 to 100.\n\r", ch);
	//	}
    //
	//	else if (IS_JUDGE(ch))
	//	{
	//		victim->pcdata->plasm[1] = value;
	//		victim->pcdata->plasm[0] = value;
	//		send_to_char ("Ok, Plasm set to desired amount\n\r", ch);
	//	}
	//	else
	//		send_to_char("Sorry, no can do...\n\r", ch);
	//		
	//}

	else if (!str_cmp (arg2, "clanstatus"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 5)
		{
			send_to_char ("#oC#ylan Status#e range is#R 1#e to#R 5#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_CLANSTATUS] = value;
			send_to_char ("#eOkay, #oC#ylan Status#e set.#n\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
	}
	else if (!str_cmp (arg2, "covstatus"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}
		else if (value < 0 || value > 5)
		{
			send_to_char ("#gC#Govenant#g S#Gtatus#e range is#R 1#e to#R 5#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_COVSTATUS] = value;
			send_to_char ("#eOkay, #CCovenant Status#e set.#n\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
	}
	else if (!str_cmp (arg2, "citystatus"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}
		else if (value < 0 || value > 5)
		{
			send_to_char ("#cC#City Status#e range is#R 1#e to#R 5#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_CITYSTATUS] = value;
			send_to_char ("#eOkay, #cC#City Status#e set.#n\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
	}
	else if (!str_cmp (arg2, "vision"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (!IS_WEREWOLF(victim))
		{
			send_to_char("#eOnly for#G Werewolves#e.#n\n\r", ch);
		}

		else if (value < 0 || value > 5)
		{
			send_to_char ("#oV#yision#e range is#R 1#e to#R 5#e.#n\n\r", ch);
			return;
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_VISION] = value;
			send_to_char ("#eOkay, #yV#oision#e set.#n\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
	}
	else if (!str_cmp (arg2, "mantle"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (!IS_CHANGELING(victim))
		{
			send_to_char("#eOnly for#G Changelings#e.#n\n\r", ch);
		}

		else if (value < 0 || value > 5)
		{
			send_to_char ("#rM#Rantle#e range is#R 1#e to#R 5#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->virtues[VIRTUE_MANTLE] = value;
			send_to_char ("#eOkay,#r M#Rantle#e set.#n\n\r", ch);
		}
		else
			send_to_char ("Huh?\n\r", ch);
	}


	else if (!str_cmp (arg2, "tempclarity"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
			return;
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("Temp Clarity range is 1 to 10.\n\r", ch);
			return;
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[TEMPCLARITY] = value;
			send_to_char ("Ok, Temp Clarity set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}
	else if (!str_cmp (arg2, "temphonor"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("Tmp honor range is 1 to 10.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[TEMPHONOR] = value;
			send_to_char ("Ok, Temp Clarity set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}
	else if (!str_cmp (arg2, "temppo"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("Tmp po range is 1 to 10.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[TEMPPO] = value;
			send_to_char ("Ok, Temp po set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}

	else if (!str_cmp (arg2, "permclarity"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("Perm Clarity range is 1 to 10.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[PERMCLARITY] = value;
			send_to_char ("Ok, Perm clarity set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}
	else if (!str_cmp (arg2, "permpo"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("perm po range is 1 to 10.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[PERMPO] = value;
			send_to_char ("Ok, perm po set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}
	else if (!str_cmp (arg2, "permhonor"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("perm honor range is 1 to 10.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->renown[PERMHONOR] = value;
			send_to_char ("Ok, perm honor set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}
	//else if (!str_cmp (arg2, "wyrd"))
	//{
	//	if (IS_NPC (victim))
	//	{
	//		send_to_char ("Not on NPC's.\n\r", ch);
	//	}
	//	else if (value < 1 || value > 10)
	//	{
	//		send_to_char ("Wyrd range is 1 to 10.\n\r", ch);
	//	}
    //
	//	else if (IS_JUDGE (ch))
	//	{
	//		victim->pcdata->wyrd[1] = value;
	//		victim->pcdata->wyrd[0] = value;
	//		send_to_char ("Ok, Wyrd set to desired amount\n\r", ch);
	//	}
	//	else
	//		send_to_char ("Sorry, no can do...\n\r", ch);
	//}

else if (!str_cmp (arg2, "clarity"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 1 || value > 10)
		{
			send_to_char ("clarity range is 1 to 10.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->clarity[1] = value;
			victim->pcdata->clarity[0] = value;
			send_to_char ("Ok, Clarity Rank set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}


	else if (!str_cmp (arg2, "plane"))
	{
		if (value < 0 || value > 100)
		{
			send_to_char ("plane range is 0 to 100.\n\r", ch);
			return;
		}

		else if (IS_JUDGE (ch))
		{
			victim->plane = value;
			send_to_char ("Ok, Clarity Rank set to desired amount\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}

//	else if (!str_cmp (arg2, "glamour"))
//	{
//		if (IS_NPC (victim))
//		{
//			send_to_char ("Not on NPC's.\n\r", ch);
//		}
//		else if (value < 1 || value > 100)
//		{
//			send_to_char ("Glamour range is 1 to 100.\n\r", ch);
//		}
//
//		else if (IS_JUDGE (ch))
//		{
//			victim->pcdata->glamour[1] = value;
//			victim->pcdata->glamour[0] = value;
//			send_to_char ("Ok, Glamour set to desired amount\n\r", ch);
//		}
//		else
//			send_to_char ("Sorry, no can do...\n\r", ch);
//	}


//	else if (!str_cmp (arg2, "plane"))
//	{
//		if (value < 0 || value > 100)
//		{
//			send_to_char ("plane range is 0 to 100.\n\r", ch);
//			return;
//		}
//
//		else if (IS_JUDGE (ch))
//		{
//			victim->plane = value;
//			send_to_char ("Ok, Glamour set to desired amount\n\r", ch);
//		}
//		else
//			send_to_char ("Sorry, no can do...\n\r", ch);
//	}

	else if (!str_cmp (arg2, "harmonybank"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (victim->class != CLASS_WEREWOLF)
		{
			send_to_char ("#eOnly #GWerewolves#e may have a Harmony bank.#n", ch);
		}

		else if (value < 0 || value > 99)
		{
			send_to_char ("#nHarmony B#wank#e range is #C0#e to#C 99#e.#n\n\r", ch);
		}
		
		else if (IS_JUDGE (ch))
		{
			victim->pcdata->harmony_bank = value;
			sprintf(buf, "#eOkay,#n Harmony#n B#wank #eset to#C %d#e.#n\n\r", value);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("Huh?", ch);
		}
	}
	else if (!str_cmp (arg2, "harmony"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (victim->class != CLASS_WEREWOLF)
		{
			send_to_char ("#eOnly #GWerewolves#e may have Harmony#e.#n", ch);
		}

		else if (value < 0 || value > 10)
		{
			send_to_char ("#nHarmony #erange is#C 0#e to #C10#e.#n\n\r", ch);
			return;
		}
		
		else if (IS_JUDGE (ch))
		{
			victim->pcdata->harmony[HARMONY_MAX] = value;
			sprintf(buf, "#eOkay,#n #CH#warmony #eset to#C %d#e.#n\n\r", value);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("Huh?", ch);
			return;
		}
		return;
	}
	
	else if (!str_cmp (arg2, "truefaith"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (victim->class != CLASS_NONE)
		{
			send_to_char ("Only mortals may have true faith.", ch);
		}

		else if (value < 0 || value > 5)
		{
			send_to_char ("#cM#Cercy#e range is#R 0#e to#R 5#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->vision = value;
			sprintf(buf, "#eOkay,#c C#Cov. Status#e set to#R %d#e.#n\n\r", value);
			send_to_char (buf, ch);
		}
		else
		{
			send_to_char ("Huh?", ch);
		}
	}


	else if (!str_cmp (arg2, "rank"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (value < 0 || value > 5)
		{
			send_to_char ("Rank range is from 0 to 5.\n\r", ch);
		}
		else
			victim->pcdata->rank = value;
	}

	else if (!str_cmp (arg2, "attribute"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char ("pset <victim> attribute <attribute> <value>.\n\r", ch);
		}
		else {
			argument = one_argument (argument, arg4);
			value = is_number (arg4) ? atoi (arg4) : -1;

			if (value < 0 || value > MAX_TRAINABLE_ATTRIB)
			{
				sprintf (buf, "Attribute range is 0 to %d.\n\r", MAX_TRAINABLE_ATTRIB);
				send_to_char (buf, ch);
			}

			else if (IS_JUDGE (ch))
			{
				if (!str_cmp (arg3, "strength"))
				{
					victim->attributes[ATTRIB_STR] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "dexterity"))
				{
					victim->attributes[ATTRIB_DEX] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "stamina"))
				{
					victim->attributes[ATTRIB_STA] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "presence"))
				{
					victim->attributes[ATTRIB_CHA] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "manipulation"))
				{
					victim->attributes[ATTRIB_MAN] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "composure"))
				{
					victim->attributes[ATTRIB_APP] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "resolve"))
				{
					victim->attributes[ATTRIB_PER] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "intelligence"))
				{
					victim->attributes[ATTRIB_INT] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "wits"))
				{
					victim->attributes[ATTRIB_WIT] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "all"))
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
				}
				else
					send_to_char ("No such physical exists.\n\r", ch);
			}
			else
				send_to_char ("Sorry, no can do...\n\r", ch);
		}
	}

	else if (!str_cmp (arg2, "physical"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char ("pset <victim> physical <physical> <value>.\n\r", ch);
		}
		else 
		{
			argument = one_argument (argument, arg4);
			value = is_number (arg4) ? atoi (arg4) : -1;

			if (value < 0 || value > MAX_TRAINABLE_ABILITY)
			{
				sprintf (buf, "Physical range is 0 to %d.\n\r", MAX_TRAINABLE_ABILITY);
				send_to_char (buf, ch);
			}
			else if (IS_JUDGE (ch))
			{
				if (!str_cmp (arg3, "all"))
				{
					for (i = 0; i < 10; i++)
					{
						victim->abilities[PHYSICAL][i] = value;
						victim->pcdata->abilities_points[PHYSICAL][i] = ability_insight_levels[value];
					}
					send_to_char ("Ok.\n\r", ch);
				}
				else
				{
					for (i = 0; i < 10; i++)
					{
						if (!str_cmp (arg3, ability_names[PHYSICAL][i][0]))
						{
							victim->abilities[PHYSICAL][i] = value;
							victim->pcdata->abilities_points[PHYSICAL][i] = ability_insight_levels[value];
							send_to_char ("Ok.\n\r", ch);
							break;
						}
					}
					if (i == 10) send_to_char ("No such physical exists.\n\r", ch);
				}
			}
			else
				send_to_char ("Sorry, no can do...\n\r", ch);
		}
	}
	else if (!str_cmp (arg2, "social"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char ("pset <victim> social <social> <value>.\n\r", ch);
		}
		else 
		{
			argument = one_argument (argument, arg4);
			value = is_number (arg4) ? atoi (arg4) : -1;

			if (value < 0 || value > MAX_TRAINABLE_ABILITY)
			{
				sprintf (buf, "Social range is 0 to %d.\n\r", MAX_TRAINABLE_ABILITY);
				send_to_char (buf, ch);
			}

			else if (IS_JUDGE (ch))
			{
				if (!str_cmp (arg3, "all"))
				{
					for (i = 0; i < 10; i++)
					{
						victim->abilities[SOCIAL][i] = value;
						victim->pcdata->abilities_points[SOCIAL][i] = ability_insight_levels[value];

					}
					send_to_char ("Ok.\n\r", ch);
				}
				else
				{
					for (i = 0; i < 10; i++)
					{
						if (!str_cmp (arg3, ability_names[SOCIAL][i][0]))
						{
							victim->abilities[SOCIAL][i] = value;
							victim->pcdata->abilities_points[SOCIAL][i] = ability_insight_levels[value];
							send_to_char ("Ok.\n\r", ch);
							break;
						}
					}
					if (i == 10) send_to_char ("No such Social Ability exists.\n\r", ch);
				}
			}
			else
				send_to_char ("Sorry, no can do...\n\r", ch);
		}
	}

	else if (!str_cmp (arg2, "mental"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char ("pset <victim> mental <mental> <value>.\n\r", ch);
		}
		else 
		{
			argument = one_argument (argument, arg4);
			value = is_number (arg4) ? atoi (arg4) : -1;

			if (value < 0 || value > MAX_TRAINABLE_ABILITY)
			{
				sprintf (buf, "Mental Ability range is 0 to %d.\n\r", MAX_TRAINABLE_ABILITY);
				send_to_char (buf, ch);
			}

			else if (IS_JUDGE (ch))
			{
				if (!str_cmp (arg3, "all"))
				{
					for (i = 0; i < 10; i++)
					{
						victim->abilities[MENTAL][i] = value;
						victim->pcdata->abilities_points[MENTAL][i] = ability_insight_levels[value];
					}
					send_to_char ("Okay.\n\r", ch);
				}
				else
				{
					for (i = 0; i < 10; i++)
					{
						if (!str_cmp (arg3, ability_names[MENTAL][i][0]))
						{
							victim->abilities[MENTAL][i] = value;
							victim->pcdata->abilities_points[MENTAL][i] = ability_insight_levels[value];
							send_to_char ("Okay.\n\r", ch);
							break;
						}
					}
					if (i == 10) send_to_char ("No such Mental Ability exists.\n\r", ch);
				}
			}
			else
				send_to_char ("Sorry, no can do...\n\r", ch);
		}
	}


	else if (!str_cmp (arg2, "spell"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char ("pset <victim> spell <colour> <value>.\n\r", ch);
		}
		else
		{
			argument = one_argument (argument, arg4);
			value = is_number (arg4) ? atoi (arg4) : -1;

			if (value < 0 || value > 200)
			{
				send_to_char ("Spell range is 0 to 200.\n\r", ch);
			}

			else if (IS_JUDGE (ch))
			{
				if (!str_cmp (arg3, "purple"))
				{
					victim->spl[PURPLE_MAGIC] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "red"))
				{
					victim->spl[RED_MAGIC] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "blue"))
				{
					victim->spl[BLUE_MAGIC] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "green"))
				{
					victim->spl[GREEN_MAGIC] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "yellow"))
				{
					victim->spl[YELLOW_MAGIC] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else if (!str_cmp (arg3, "all"))
				{
					victim->spl[PURPLE_MAGIC] = value;
					victim->spl[RED_MAGIC] = value;
					victim->spl[BLUE_MAGIC] = value;
					victim->spl[GREEN_MAGIC] = value;
					victim->spl[YELLOW_MAGIC] = value;
					send_to_char ("Ok.\n\r", ch);
				}
				else
					send_to_char ("No such spell colour exists.\n\r", ch);
			}
			else
				send_to_char ("Sorry, no can do...\n\r", ch);
		}
	}

	else if (!str_cmp (arg2, "class"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (victim->class != CLASS_NONE)
		{
			send_to_char ("They already have a class.\n\r", ch);
		}

		else if (victim->level < LEVEL_AVATAR)
		{
			send_to_char ("You can only do this to avatars.\n\r", ch);
		}

		else if (ch->level != MAX_LEVEL)
		{
			send_to_char ("Sorry, no can do...\n\r", ch);
		}

		else if (!str_cmp (arg3, "vampire"))
		{
			victim->class = CLASS_VAMPIRE;
			victim->vamppot = 1;
			update_pot (victim);
			victim->pcdata->bloodlined[0] = -1;
			victim->pcdata->bloodlined[1] = -1;
			victim->pcdata->bloodlined[2] = -1;
			send_to_char ("Ok.\n\r", ch);
			send_to_char ("You are now the Master Vampire.\n\r", victim);
		}
		else if (!str_cmp (arg3, "changeling"))
		{
			victim->class = CLASS_CHANGELING;
			victim->vamppot = 1;
			victim->pcdata->bloodlined[0] = -1;
			victim->pcdata->bloodlined[1] = -1;
			victim->pcdata->bloodlined[2] = -1;
			send_to_char ("Ok.\n\r", ch);
			send_to_char ("You are now the Master Changeling.\n\r", victim);
		}
		else
			do_pset (ch, "");
	}

	else if (!str_cmp (arg2, "special"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
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
			}

			else
			{
				send_to_char ("Sorry, no can do...\n\r", ch);
			}
		}
	}

	else if (!str_cmp (arg2, "thaum"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC'S.\n\r", ch);
		}
			if (!str_cmp (arg3, "general"))
			{
				if (IS_SET (ch->pcdata->thaum_type, THAUM_TYPE_GENERAL))
				{
					send_to_char ("You remove their general Theban Sorcery flag.\n\r", ch);
					send_to_char ("You no longer know general Theban Sorcery.\n\r", victim);
					REMOVE_BIT (ch->pcdata->thaum_type, THAUM_TYPE_GENERAL);
				}
				else
				{
					send_to_char ("You add a general Theban Sorcery flag to them.\n\r", ch);
					send_to_char ("You now know general Theban Sorcery.\n\r", victim);
					SET_BIT (ch->pcdata->thaum_type, THAUM_TYPE_GENERAL);
				}
			}
			else if (!str_cmp (arg3, "dark"))
			{
				if (IS_SET(ch->pcdata->thaum_type, THAUM_TYPE_DARK))
				{
					send_to_char ("You remove their dark Theban Sorcery flag.\n\r", ch);
					send_to_char ("You no longer know dark Theban Sorcery.\n\r", victim);
					REMOVE_BIT (ch->pcdata->thaum_type, THAUM_TYPE_DARK);
				}
				else
				{
					send_to_char ("You add a dark Theban Sorcery flag to them.\n\r", ch);
					send_to_char ("You now know dark Theban Sorcery.\n\r", victim);
					SET_BIT (ch->pcdata->thaum_type, THAUM_TYPE_DARK);
				}
			}

			else if (!str_cmp (arg3, "koldun"))
			{
				if (IS_SET(ch->pcdata->thaum_type, THAUM_TYPE_KOLDUN))
				{
					send_to_char ("You remove their koldun Theban Sorcery flag.\n\r", ch);
					send_to_char ("You no longer know koldun Theban Sorcery.\n\r", victim);
					REMOVE_BIT (ch->pcdata->thaum_type, THAUM_TYPE_KOLDUN);
				}
				else
				{
					send_to_char ("You add a koldun Theban Sorcery flag to them.\n\r", ch);
					send_to_char ("You now know koldun Theban Sorcery.\n\r", victim);
					SET_BIT (ch->pcdata->thaum_type, THAUM_TYPE_KOLDUN);
				}
			}
			else
			{
				send_to_char ("Sorry, no can do...\n\r", ch);
			}
	}


	else if (!str_cmp (arg2, "beast"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (value < 0 || value > 100)
		{
			send_to_char ("Beast range is 0 to 100.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->beast = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
		return;
	}

	else if (!str_cmp (arg2, "willpower"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}


		else if (value < 1 || value > 10)
		{
			send_to_char ("#oW#yillpower#e points range is#R 1#e to#R 10#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->willpower[0] = value;
			victim->pcdata->willpower[1] = value;
			send_to_char ("Okay.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}

	else if (!str_cmp (arg2, "dps"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}


		else if (value < 1 || value > 200000)
		{
			send_to_char ("Demon points range is 1 to 200000.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->power[0] = value;
			victim->pcdata->power[1] = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}

	else if (!str_cmp (arg2, "quest"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}

		else if (value < 0 || value > 15000)
		{
			send_to_char ("Quest range is 0 to 15000.\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->quest = value;
			send_to_char ("Ok.\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do...\n\r", ch);
	}
	else if (!str_cmp (arg2, "roadrank"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPC's.\n\r", ch);
		}
		else if (value < 0 || value > 10)
		{
			send_to_char ("#gR#Goadrank#e range is #R1#e to#R 10#e.#n\n\r", ch);
		}

		else if (IS_JUDGE (ch))
		{
			victim->pcdata->clant = value;
			send_to_char ("#eOkay, #gr#Goad#g r#Gank#e set to desired amount.#n\n\r", ch);
		}
		else
			send_to_char ("Sorry, no can do.\n\r", ch);
	}

	else if (!str_cmp (arg2, "password"))
	{
		if (IS_NPC(victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char ("pset <victim> password <pass>.\n\r", ch );
		}
		else 
		{
			pwdnew = crypt (arg3, victim->name );
			for (p = pwdnew; *p != '\0'; p++)
			{
				if (*p == '~')
				{
					send_to_char("New password should NOT have a tilde.\n\r", ch);
					break;
				}
			}
			if (*p == '\0')
			{
				free_string (victim->pcdata->pwd);
				victim->pcdata->pwd = str_dup (pwdnew);
				if (!IS_EXTRA (victim, EXTRA_NEWPASS))
					SET_BIT(victim->extra,EXTRA_NEWPASS);
				do_autosave (victim, "");
				send_to_char("Ok.\n\r",ch);
			}
		}
	}

	else if (!str_cmp (arg2, "clan"))
	{
		if (IS_NPC (victim))
		{
			send_to_char ("Not on NPCs.\n\r", ch);
		}

		else if (arg3 == '\0')
		{
			send_to_char( "pset <victim> clan <clan>.\n\r",ch);
			strcpy (buf, clan_names[0][0]);
			for (i = 1; i < MAX_CLANS; i++)
			{
				strcat (buf, " ");
				strcat (buf, clan_names[i][0]);
			}
			strcat (buf, "\n\r");
			send_to_char (buf, ch);
		}

		else if (IS_JUDGE (ch))
		{
			for (i = 0; i < MAX_CLANS; i++)
			{
				if (!str_cmp (arg3, clan_names[i][0]))
				{
					clan = i;
					break;
				}
			}

			if (clan == -1)
			{
				send_to_char ("Seeming must be one of the following: Beast, Darkling, Elemental, Fairest, Ogre, Wizened.\n\r", ch);
			}
			else
			{
				victim->pcdata->clan = clan;
				victim->pcdata->clant = 1;
				sprintf( buf, "#eYour Seeming is set to %s#e.#n\r\n",
				 clan_names[clan][0]);
				send_to_char( buf, victim);
				sprintf( buf, "#eSeeming set to %s.",
				 clan_names[clan][0]);
				send_to_char( buf, ch );
			}
		}
		else
			send_to_char ("Sorry, no can do.\n\r", ch);
	}

	/*
	 * Generate usage message.
	 */
	else 
		do_pset (ch, "");
	
	if (is_offline)
	{
		send_to_char("#RNote#n: Character is offline\n\r", ch);
		save_char_obj(victim);
		free_char(victim);
	}
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
		send_to_char ("Not on NPCs.\n\r", ch);
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
			send_to_char ("#cE#Cxperience#c p#Coints#e range is#R -500#e to#R 500#e.#n\n\r", ch);
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
		send_to_char ("#ePlease specify #CXP#e,#y WP#e or#R DPS#e.#n\n\r", ch);
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
		sprintf (buf, "#eYou have been rewarded#w %d #C%s#e for %s.#n\n\r", v, arg, argument);
		send_to_char (buf, vch);
	}
	sprintf (buf, "#eYou have rewarded them#w %d #C%s#e for %s.#n\n\r", v, arg, argument);
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



OBJ_DATA *get_obj_carry_vehicles (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj) && is_abbr (arg, obj->name) )
		{
			if (++count == number)
				return obj;
		}
	}

	return NULL;
}

OBJ_DATA *get_vehicles_from_inv (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if ( obj->item_type == ITEM_VEHICLE && !str_cmp(argument, obj->name))
			return obj;
	}

	return NULL;
}

void do_restring (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char endchar[15];

	smash_tilde (argument);
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))
		return;


	if (arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0')
	{
		send_to_char ("#c+--------------------------------+-----------------------------------------+\n\r", ch);
		send_to_char ("#c| #CSyntax#c                         | #CExample#c                                 |\n\r", ch);
		send_to_char ("#c+--------------------------------+-----------------------------------------+\n\r", ch);
		send_to_char ("#c|#w restring <object> short <desc> #c| #nrestring boots short pink flip-flops    #c|\n\r", ch);
		send_to_char ("#c|#w restring <object> long <desc>  #c| #nrestring boots long Flip-flops sit here.#c|\n\r", ch);
		send_to_char ("#c|#w restring <object> name <names> #c| #nrestring boots name flip flops          #c|\n\r", ch);
		send_to_char ("#c+-----------------------------------+--------------------------------------+\n\r", ch);
		send_to_char ("#c| #CVehicle only strings#c              | #CExplanation#c                          |\n\r", ch);
		send_to_char ("#c+-----------------------------------+--------------------------------------+\n\r", ch);
		send_to_char ("#c|#w restring <vehicle> inside <desc>  #c| #nDescribes the inside of the vehicle  #c|\n\r", ch);
		send_to_char ("#c|#w restring <vehicle> outside <desc> #c| #nDescribes the outside of the vehicle #c|\n\r", ch);
		send_to_char ("#c+-----------------------------------+--------------------------------------+\n\r", ch);
		return;
	}

	if ((obj = get_obj_carry_vehicles (ch, arg1)) == NULL)
	{
		send_to_char ("You are not carrying that object.\n\r", ch);
		return;
	}


	if (!str_cmp (arg2, "room") || !str_cmp (arg2, "short"))
	{
		if (strlen_exclude_colors (argument) > 59)
		{
			send_to_char ("Short descriptions should be 59 characters.\n\r", ch);
			return;
		}
		endchar[0] = (argument[strlen (argument) - 1]);
		endchar[1] = '\0';
		free_string (obj->short_descr);
		obj->short_descr = str_dup (argument);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		if ( obj->item_type == ITEM_VEHICLE ) {
			
		}
		send_to_char ("#cNew short description set.#n\n\r", ch);
		return;
	}
	else if (!str_cmp (arg2, "ground") || !str_cmp (arg2, "long"))
	{
		if (strlen (argument) < 5)
		{
			send_to_char ("#cDescription for the ground should be at least 5 characters long.#n\n\r", ch);
			return;
		}
		endchar[0] = (argument[strlen (argument) - 1]);
		endchar[1] = '\0';
		sprintf( buf, argument );
		buf[0] = UPPER (buf[0]);
		free_string (obj->description);
		obj->description = str_dup (buf);
		send_to_char ("#cNew long description set.#n\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}

	else if (!str_cmp (arg2, "keyword") || !str_cmp (arg2, "keywords") || !str_cmp (arg2, "name"))
	{
		if (strlen (argument) < 3)
		{
			send_to_char ("#cKeywords should be at least 3 characters long.#n\n\r", ch);
			return;
		}
		if ( obj->item_type == ITEM_VEHICLE ) {
			if ( get_vehicles_from_inv( ch, argument) ) {
				
			}	
		}

		free_string (obj->name);
		obj->name = str_dup (argument);
		send_to_char ("#cNew keywords/name set.#n\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}
	
	// Allow special descs
	
	else if (!str_cmp (arg2, "ed"))
	{
		EXTRA_DESCR_DATA *ed;
		argument = one_argument (argument, arg3);
		if (argument == NULL)
		{
			send_to_char ("#cSyntax: restring <object> ed <keyword> <string>#n\n\r", ch);
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
		send_to_char ("#cNew extra description set.#n\n\r", ch);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		return;
	}
	
	// End special descs
	else if (!str_cmp (arg2, "inside") && obj->item_type == ITEM_VEHICLE )
	{
		if (strlen (argument) < 10)
		{
			send_to_char ("#cInside description should be at least 10 characters long.#n\n\r", ch);
			return;
		}
		endchar[0] = (argument[strlen (argument) - 1]);
		endchar[1] = '\0';
		free_string (obj->inside_desc);
		obj->inside_desc = str_dup (argument);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		send_to_char ("#cNew vehicle interior set.#n\n\r", ch);
		return;
	}
	else if (!str_cmp (arg2, "outside") && obj->item_type == ITEM_VEHICLE )
	{
		if (strlen (argument) < 10)
		{
			send_to_char ("#cOutside description should be at least 10 characters long.#n\n\r", ch);
			return;
		}
		endchar[0] = (argument[strlen (argument) - 1]);
		endchar[1] = '\0';
		free_string (obj->outside_desc);
		obj->outside_desc = str_dup (argument);
		if (obj->questmaker != NULL)
			free_string (obj->questmaker);
		obj->questmaker = str_dup (ch->name);
		send_to_char ("#cNew vehicle exterior set.#n\n\r", ch);
		return;
	}
	send_to_char ("Syntax: restring <object> short <short description>\n\r", ch);
	send_to_char ("Syntax: restring <object> long <long description>\n\r", ch);
	send_to_char ("Syntax: restring <object> keyword <keyword/s>\n\r", ch);
	send_to_char ("Syntax: restring <vehicle> inside <internal vehicle description>\n\r", ch);
	send_to_char ("Syntax: restring <vehicle> outside <external vehicle description>\n\r", ch);
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
	send_to_char ("WHAT? \n\r", ch);
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

		sprintf( addbuf, "[%d] %s #p|#n", i, room->name );
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

void do_motd (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "motd" );
	return;
}

void do_index (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "index" );
	return;
}

void do_werestats (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "werestats" );
	return;
}

void do_freehold (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "freehold" );
	return;
}

void do_chanstats (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "chanstats" );
	return;
}

void do_seemingbonus (CHAR_DATA * ch, char *argument)
{
	do_help (ch, "seemingbonus" );
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
	
	send_to_char("#cCurrent Uptime:\n\r", ch);
	sprintf(buf, "Days: #w%8d#n\n\r", days);
	send_to_char(buf, ch);
	sprintf(buf, "Hours: #w%7d#n\n\r", hours);
	send_to_char(buf, ch);
	sprintf(buf, "Minutes: #w%5d#n\n\r", minutes);
	send_to_char(buf, ch);
	sprintf(buf, "Seconds: #w%5d#n\n\r", temp);
	send_to_char(buf, ch);
}

/* Begin Masquerade Code */

void do_masquerade (CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int char_success;
	int difficulty = 8;
	int burned_blood;
	
	argument = one_argument(argument, arg);
	
	if (IS_NPC(ch))
		return;
	
	 
 	if (!str_cmp(arg, "blood") && IS_VAMPIRE(ch))
	{
		difficulty--;
		
		//Calculate how much blood will be burned
		if (ch->pcdata->clan == CLAN_HUMANITY){
			burned_blood = (6 - ch->pcdata->virtues[VIRTUE_COVSTATUS]);
					}
		else if 
		((ch->pcdata->clan == CLAN_VENTRUE) || (ch->pcdata->clan == CLAN_DARKLING)){
			burned_blood = (8 - ch->pcdata->virtues[VIRTUE_COVSTATUS]);}
			
		else{
			sprintf(buf, "#eYou can not burn #Rblood#e to#c M#Casquerade#e because of your Path.#n\n\r");
			send_to_char(buf, ch);
			return;
		}
		//Check if they can burn that much blood
		
		if (ch->blood[BLOOD_CURRENT] > burned_blood)
		{
			ch->blood[BLOOD_CURRENT] -= burned_blood;	
			sprintf(buf, "#R%d#e blood burned.#n\n\r", burned_blood);
			send_to_char(buf, ch);
		}
		
		else
		{
			sprintf(buf, "#eYou don't have #R%d#e blood to burn.#n\n\r", burned_blood);
			send_to_char(buf, ch);
			return;
		}
	}
	
	if (IS_VAMPIRE(ch))
	{
		if (!str_cmp(ch->bloodline, "Nosferatu"))
		{
			difficulty += 2;
			
			if (ch->pcdata->clan == CLAN_HUMANITY)
			{
				char_success = diceroll(ch, ATTRIB_CHA, SOCIAL, SKI_SUBTERFUGE, difficulty);
				send_to_char("#eAttempting to #cM#Casquerade#e as a human...#n\n\r", ch);
				
				if (char_success < 0)
					char_success = -1;	
				
				sprintf(buf, "#eYou had #C%d#e successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char(buf, ch);
				
				if (char_success < 0)
				 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #RBotched#c their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#n$n#e has #RBotched#e their attempt to #cM#Casquerade#e as a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);	
			}
				else if (char_success == 0)
				 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #wFailed#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n#e looks like a walking corpse, and isn't blinking or breathing. #c(#eRoll #GAlertness#e and earn 2+ successes to notice#c)#n" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else
				{
			  	
				{
				sprintf (buf, "#n[#eLog Masq#n: #eThe Character got #C%d#e successes.#n]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#w$n#e got #C%d#e successes to #cM#Casquerade#e as a human. Effects last for one scene.#n\n\r", char_success);
			}	
					
			}
		
			else
			{
				char_success = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_SUBTERFUGE, difficulty);
				send_to_char ("#eAttempting to #cM#Casquerade#e as a human...#n\n\r", ch);

				if (char_success <0)
					char_success = -1;

				sprintf (buf, "#eYou had #C%d#e successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char (buf, ch);
		
				if (char_success < 0)
				 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #RBotched#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n#e has #RBotched#e their attempt to #cM#Casquerade#e as a human.#n\n\r" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else if (char_success == 0)
				 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #CFailed#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n #nlooks like a walking corpse, and isn't blinking or breathing. #c(#eRoll #GAlertness#e and earn 2+ successes to notice#c)#n" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				
				else
				{
			  	
				{
				sprintf (buf, "#n[#eLog Masq#n: #eThe Character got #C%d#e successes.#n]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#w$n#n got #C%d#e successes to #cM#Casquerade#e as a human. Effects last for one scene.#n\n\r", char_success);
			}	
					
			}
		}			
			
		else if (!str_cmp(ch->bloodline, "Cappadocian"))
		{
			difficulty += 2;
			
			if (ch->pcdata->clan == CLAN_HUMANITY)
			{	
				char_success = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_SUBTERFUGE, difficulty);
				send_to_char ("#eAttempting to #cM#Casquerade#e as a human...#n\n\r", ch);
	
				if (char_success < 0)
					char_success = -1;
		
				sprintf (buf, "#eYou had #C%d#e successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char (buf, ch);
				
				if (char_success < 0)
		        	 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #RBotched#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n#e has #RBotched#e their attempt to #cM#Casquerade #eas a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else if (char_success == 0)
				 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #CFailed#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n #nlooks like a walking corpse, and isn't blinking or breathing. #c(#eRoll #GAlertness#e and earn 2+ successes to notice#c)#n" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else
				{
			  	
				{
				sprintf (buf, "#n[#eLog Masq#n: #eThe Character got #C%d#e successes.#n]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#w$n#e got #C%d#e successes to #cM#Casquerade#e as a human. Effects last for one scene.#n\n\r", char_success);
			}	
				
			}
				
			else
			{
				char_success = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_SUBTERFUGE, difficulty);
				send_to_char ("#eAttempting to #cM#Casquerade #eas a human...#n\n\r", ch);
	
				if (char_success < 0)
					char_success = -1;
	
				sprintf (buf, "#eYou had #C%d#e successes. Effects last for one scene.#n\n\r", char_success);
				send_to_char (buf, ch);
			
				if (char_success < 0)
				 {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #RBotched#e their#c M#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n#e has #RBotched#e their attempt to #cM#Casquerade #eas a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else if (char_success == 0){
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #CFailed#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n #nlooks like a walking corpse, and isn't blinking or breathing. #c(#eRoll #GAlertness#e and earn 2+ successes to notice#c)#n" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
				else
				{
			  	
				{
				sprintf (buf, "#n[#eLog Masq#n: #eThe Character got #C%d#e successes.#n]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#w$n#e got #C%d#e successes to#c M#Casquerade #eas a human. Effects last for one scene.#n\n\r", char_success);
			}	
				
			}
		}
			
		else
		{
			char_success = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_SUBTERFUGE, difficulty);
			send_to_char ("#eAttempting to #cM#Casquerade#e as a human...#n\n\r", ch);
			
			if (char_success < 0) 
				char_success = -1;
				
			sprintf (buf, "#eYou had #C%d#e successes. Effects last for one scene.#n\n\r", char_success);
			send_to_char (buf, ch);
				
			if (char_success < 0)
		        {
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #RBotched#e their #cM#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n#e has #RBotched#e their attempt to #cM#Casquerade#e as a human.#n\n\r" );
				act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
			else if (char_success == 0){
		          	{
				sprintf( buf, "#n[#eLog Masq#n: #eThe Character has #CFailed#e their#c M#Casquerade#e roll.#n]#n\n\r" );
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf( buf, "#w$n #nlooks like a walking corpse, and isn't blinking or breathing. #c(#eRoll #GAlertness#e and earn 2+ successes to notice#c)#n" );
					act (buf, ch, NULL, NULL, TO_NOTVICT);
			}
			else
			{
			  	
				{
				sprintf (buf, "#n[#eLog Masq#n: #eThe Character got #C%d#e successes.#n]#n\n\r", char_success);
				logchan( buf, CHANNEL_LOG_DICE );
				}
				sprintf (buf, "#w$n#e got #C%d#e successes to#c M#Casquerade#e as a human. Effects last for one scene.#n\n\r", char_success);
			}	
			
		}
	}
	
	else
	{
		send_to_char ("#eYou have to be a vampire to#c M#Casquerade#e.#n\n\r", ch);
		
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
      send_to_char ("#rC#Rategories      #r-     #rS#Rpecific #rR#Rolls\n\r", ch);
	    send_to_char ("#r-----------------------------------------------\n\r", ch);
      send_to_char ("#nL#warceny#n               Juggle#e,#w Cheat,#n Trick\n\r", ch);
      send_to_char ("#nS#wubterfuge#n            Signal\n\r", ch);
      send_to_char ("#nE#wmpathy#n               Mood#e, #wComfort\n\r", ch);
      send_to_char ("#nG#wame                  #nPoker#e,#w Dice#e,#n Darts#e,#w Pool\n\r", ch);
      send_to_char ("#nK#wnowledge             #nAny #wMental Ability#n You Possess\n\r", ch);
      send_to_char ("#r-----------------------------------------------\n\r", ch);
      send_to_char ("#rS#Ryntax#r:#n Roll Category specific#r,#n e#r.#ng#r.#n roll game poker or roll subterfuge signal#r.#n\n\r", ch);;
		return;
	}

	if (ch->pcdata->evileye > 0)
	{
		if (ch->pcdata->evileye > 1)
			sprintf (buf, "#eYou will be able to #Groll#e again in #C%d#e seconds.#n\n\r", ch->pcdata->evileye);
		else
			sprintf (buf, "#eOne more second!#n\n\r");
		send_to_char (buf, ch);
		return;
	}
	else if (!str_cmp (arg, "empathy"))
	{
		if (!str_cmp (arg2, "comfort") )
		{
			char_success = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_EMPATHY, 8);
			send_to_char ("#eRolling to comfort someone.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has#R failed#e their attempt to comfort someone.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes to comfort someone.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "mood") )
		{
			char_success = diceroll (ch, ATTRIB_PER, SOCIAL, SKI_EMPATHY, 8);
			send_to_char ("#eRolling to determine someones mood, or the mood of a room.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e to determine the mood of a person or room.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e has #G%d#e successes determining the mood of a person or a room.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#eYou may roll #GEmpathy #Gmood#e or #Gcomfort#e.#n", ch);
			return;
		}
	}
	else if (!str_cmp (arg, "subterfuge"))
	{
		 if (!str_cmp (arg2, "signal") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, SOCIAL, TAL_LARCENY, 8);
			send_to_char ("#eRolling to send a subtle hand signal or wink.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e to send a subtle or hidden signal to someone.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e has #G%d#e successes sending a subtle or hidden signal to someone.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#eYou may roll #GSubterfuge #Gsignal#e.#n", ch);
			return;
		}
	}
		
	
	
	else if (!str_cmp (arg, "larceny"))
	{
		if (!str_cmp (arg2, "juggle") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, SOCIAL, TAL_ATHLETICS, 8);
			send_to_char ("#eRolling to Juggle.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has#R failed#e their juggling roll.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes juggling.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "trick") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, SOCIAL, SKI_STREETWISE, 8);
			send_to_char ("#eRolling to do a simple 'magic' trick.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e to perform a 'magic' trick.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e has #G%d#e successes to perform a 'magic' trick.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if (!str_cmp (arg2, "cheat") )
		{
			char_success = diceroll (ch, ATTRIB_DEX, SOCIAL, TAL_LARCENY, 8);
			send_to_char ("#eRolling to stack a deck or cheat with dice.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has#R failed#e while cheating with cards or dice.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes stacking a deck or cheating with dice.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#eYou may roll #GLarceny trick#e, #Gcheat#e or #Gjuggle#e.", ch);
			return;
		}
	}
	
	else if (!str_cmp (arg, "shift"))
	{
		if (!str_cmp (arg2, "human") )
		{
			char_success = diceroll (ch, ATTRIB_STA, PHYSICAL, TAL_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to human form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to human.\n\r", ch);
				
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has#R Botched#c shifting to human. Please inform the storytellers.#n\n\r", ch->name);
			else
			sprintf (buf, "#C%s#c got #C%d#c successes shifting to human form.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "primal") )
		{
			char_success = diceroll (ch, ATTRIB_STA, PHYSICAL, TAL_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to Primal Beast form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to Primal Beast.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to Primal Beast. Please inform the storytellers.#n\n\r", ch->name);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to Primal Beast form.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "war") )
		{
			char_success = diceroll (ch, ATTRIB_STA, PHYSICAL, TAL_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to War Beast form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to War Beast.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to War Beast. Please inform the storytellers.#n\n\r", ch->name);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to War Beast form.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "throwback") )
		{
			char_success = diceroll (ch, ATTRIB_STA, PHYSICAL, TAL_SURVIVAL, 6);
			send_to_char ("#cRolling to shift to Throwback form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to Throwback.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to Throwback. Please inform the storytellers.#n\n\r", ch->name);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to Throwback form.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else if (!str_cmp (arg2, "dire") )
		{
			char_success = diceroll (ch, ATTRIB_STA, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cRolling to shift to Dire Beast form.#n\n\r", ch);
			if (char_success >= 0)
			{
				sprintf (buf, "#cYou had #C%d#c successes.#n\n\r", char_success);
				send_to_char (buf, ch);
			}
			else
				send_to_char ("#cYou have #RBotched#c shifting to Dire Beast form.\n\r", ch);
			if (char_success < 0)
			  sprintf( buf, "#C%s#c has #RBotched#c shifting to Dire Beast form. Please inform the storytellers.#n\n\r", ch->name);
			else
			sprintf (buf, "#C%s#c has #C%d#c successes shifting to Dire Beast form.#n\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		else
		{
					send_to_char ("#cYou may roll #Cshift #Chuman#c, #Cprimal#c.", ch);
			return;
		}
	}
	
	/*Begin new roll code*/
	else if (!str_cmp (arg, "knowledge"))
	{
		if (!str_cmp (arg2, "science") ){
		if(ch->abilities[MENTAL][KNO_SCIENCE] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_SCIENCE, 8);
			send_to_char ("#eRolling for general Science ability.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Science check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Science check.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_SCIENCE] < 1) {
			send_to_char ("#RError: #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		if (!str_cmp (arg2, "academics") ){
		if(ch->abilities[MENTAL][KNO_ACADEMICS] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_ACADEMICS, 8);
			send_to_char ("#eRolling for Academics ability.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Academics check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Academics check.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_ACADEMICS] < 1) {
			send_to_char ("#RError: #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "computer") ){
		if(ch->abilities[MENTAL][KNO_COMPUTER] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_COMPUTER, 8);
			send_to_char ("#eRolling for Computer ability.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Computer check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Computer check.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_COMPUTER] < 1) {
			send_to_char ("#RError:#n #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "investigation") ){
		if(ch->abilities[MENTAL][KNO_INVESTIGATION] > 0) {
			char_success = diceroll (ch, ATTRIB_WIT, MENTAL, KNO_INVESTIGATION, 8);
			send_to_char ("#eRolling to Investigate.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Investigation check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Investigation check.\n\r", ch->name, char_success);
			logchan( buf, CHANNEL_LOG_DICE );
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_INVESTIGATION] < 1) {
			send_to_char ("#RError: #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "medicine") ){
		if(ch->abilities[MENTAL][KNO_MEDICINE] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_MEDICINE, 8);
			send_to_char ("#eRolling for Medical ability.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Medicine check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Medicine check.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_MEDICINE] < 1) {
			send_to_char ("#RError: #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "occult") ){
		if(ch->abilities[MENTAL][KNO_OCCULT] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_OCCULT, 8);
			send_to_char ("#eRolling for Occult ability.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Occult check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Occult check.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_OCCULT] < 1) {
			send_to_char ("#RError: #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		
		if (!str_cmp (arg2, "politics") ){
		if(ch->abilities[MENTAL][KNO_POLITICS] > 0) {
			char_success = diceroll (ch, ATTRIB_INT, MENTAL, KNO_POLITICS, 8);
			send_to_char ("#eRolling for Political ability.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#w%s#e has #Rfailed#e their Political check.#n\n\r", ch->name);
			else
			sprintf (buf, "#w%s#e got #G%d#e successes on their Political check.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		if(ch->abilities[MENTAL][KNO_POLITICS] < 1) {
			send_to_char ("#RError: #nYou can't perform a knowledge check without the relevant Mental Skill.#n\n\r", ch);
			return;
		}
		}
		
		else	
		{
send_to_char ("#eYou may roll for any Mental knowledge you possess. E.G. #wroll knowledge medicine#n", ch);
return;
}}

else if (!str_cmp (arg, "game")){
	

			if (!str_cmp (arg2, "pool") ){
			char_success = diceroll (ch, ATTRIB_PER, PHYSICAL, TAL_ATHLETICS, 8);
			send_to_char ("#eRolling for playing pool.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#n%s#e has #Rfailed#e their roll to play pool.#n\n\r", ch->name);
			else
			sprintf (buf, "#n%s#e got #G%d#e successes on their roll to play pool.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
			if (!str_cmp (arg2, "dice") ){
			char_success = diceroll (ch, ATTRIB_DEX, SOCIAL, SKI_STREETWISE, 8);
			send_to_char ("#eRolling for shooting dice.#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#n%s#e has #Rfailed#e a roll to shoot dice.#n\n\r", ch->name);
			else
			sprintf (buf, "#n%s#e got #G%d#e successes on their roll to shoot dice.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
			if (!str_cmp (arg2, "darts") ){
			char_success = diceroll (ch, ATTRIB_DEX, PHYSICAL, TAL_ATHLETICS, 8);
			send_to_char ("#eRolling for tossing darts#n\n\r", ch);
			if (char_success <0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#n%s#e has #Rfailed#e a roll to toss darts.#n\n\r", ch->name);
			else
			sprintf (buf, "#n%s#e got #G%d#e successes on their roll to toss darts.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
			if (!str_cmp (arg2, "poker") ){
			doobiesnacks = ((int) (rand() % 7)) - 2;
			char_success = (diceroll (ch, ATTRIB_MAN, SOCIAL, SKI_SUBTERFUGE, 8)) + doobiesnacks;
			send_to_char ("#eRolling to play a hand of poker.#n\n\r", ch);
			if (char_success < 0) char_success = -1;
			sprintf (buf, "#eYou had #G%d#e successes.#n\n\r", char_success);
			send_to_char (buf, ch);
			if (char_success < 0)
			  sprintf( buf, "#n%s#e has #Rfailed#e a roll to play a hand of poker.#n\n\r", ch->name);
			else
			sprintf (buf, "#n%s#e got #G%d#e successes on their roll to play poker.\n\r", ch->name, char_success);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			logchan( buf, CHANNEL_LOG_DICE );
			ch->pcdata->evileye = 10;
			return;
		}
		
		
		else	
			{
			send_to_char ("#eYou may roll #Ggame pool#e, #Gdice#e, #Gpoker#e, #Gdarts#e.", ch);
			return;
   		}
		}
		

/*end roll code*/
else
      send_to_char ("#rC#Rategories      #r-     #rS#Rpecific #rR#Rolls\n\r", ch);
	    send_to_char ("#r-----------------------------------------------\n\r", ch);
      send_to_char ("#nL#warceny#n               Juggle#e,#w Cheat,#n Trick\n\r", ch);
      send_to_char ("#nS#wubterfuge#n            Signal\n\r", ch);
      send_to_char ("#nE#wmpathy#n               Mood#e, #wComfort\n\r", ch);
      send_to_char ("#nG#wame                  #nPoker#e,#w Dice#e,#n Darts#e,#w Pool\n\r", ch);
      send_to_char ("#nK#wnowledge             #nAny #wMental Ability#n You Possess\n\r", ch);
      send_to_char ("#r-----------------------------------------------\n\r", ch);
      send_to_char ("#rS#Ryntax#r:#n Roll Category specific#r,#n e#r.#ng#r.#n roll game poker or roll subterfuge signal#r.#n\n\r", ch);;

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

	send_to_char ("#c[     #CPhysical     #c][      #CSocial#c      ][     #C Mental#c      ]#n\n\r", ch);
	for (i = 0; i < 8; i++)
	{
		sprintf (buf, "#c[#C%-15s #B%d#c ][#C%-15s #B%d#c ][#C%-15s #B%d#c ]#n\n\r", ability_names[PHYSICAL][i][0], ch->abilities[PHYSICAL][i], ability_names[SOCIAL][i][0], ch->abilities[SOCIAL][i], ability_names[MENTAL][i][0], ch->abilities[MENTAL][i]);
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
	else if (language == FAE)
	{
		lan_str = "fae";
		language2 = gsn_fae;
	}

	else if (language == KOREAN)
	{
		lan_str = "korean";
		language2 = gsn_korean;
	}

	else if (language == MANDARIN)
	{
		lan_str = "mandarin";
		language2 = gsn_mandarin;
	}
	else if (language == CANTONESE)
	{
		lan_str = "cantonese";
		language2 = gsn_cantonese;
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
		do_smoke1(ch,argument);
	if (IS_MORE3(ch, MORE3_OBFUS3))
		do_smoke3(ch,argument);
	
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

			ch->pcdata->learned[language2] += ch->abilities[MENTAL][KNO_MEDICINE];

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
					if (IS_CHANGELING (ch) && language == FAE)
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
					if (IS_CHANGELING (ch) && language == FAE)
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
			ch->pcdata->learned[language2] += ch->abilities[MENTAL][KNO_MEDICINE];
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
		send_to_char ("But you have already chosen a covenant!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose a covenant:  Autarkis, Furores, Inconnu and Promethians.\n\r", ch);
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

		if (!str_cmp (ch->bloodline, "Toreador") || !str_cmp (ch->bloodline, "Tzimisce") || !str_cmp (ch->bloodline, "Brujah"))
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
		if (!str_cmp (ch->bloodline, "baali") || !str_cmp (ch->bloodline, "tzimisce"))
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
		send_to_char ("Please choose a covenant:  Autarkis, Furores, Inconnu and Promethians.\n\r", ch);
	return;
}

void do_organization (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (strlen (ch->kithname) > 1)
	{
		send_to_char ("But you have already chosen a Kith!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose an Organization: Blood Talons, Bone Shadows, Hunters in Darkness, Iron Masters, Storm Lords\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "Blood Talons"))
	{
		free_string (ch->kithname);
		ch->kithname = str_dup ("Blood Talons");
		send_to_char ("You are now a Blood Talon.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "Bone Shadow"))
	{
		free_string (ch->kithname);
		ch->kithname = str_dup ("Bone Shadows");
		send_to_char ("You are now a Bone Shadow.\n\r", ch);
		return;
	}
	else
		send_to_char ("Please choose a Organization: Blood Talons, Bone Shadows, Hunters in Darkness, Iron Masters, Storm Lords.\n\r", ch);
	return;
}

void do_auspic (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (strlen (ch->auspice) > 1)
	{
		send_to_char ("But you have already chosen an Auspice!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose an Auspice: Rahu, Cahlaith, Elodoth, Ithaeur, Irraka\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "Rahu"))
	{
		free_string (ch->auspice);
		ch->auspice = str_dup ("Rahu");
		send_to_char ("You are now a Rahu.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "Cahlaith"))
	{
		free_string (ch->auspice);
		ch->auspice = str_dup ("Cahlaith");
		send_to_char ("You are now a Cahlaith.\n\r", ch);
		return;
	}
	else
		send_to_char ("Please choose an Auspice: Rahu, Cahlaith, Elodoth, Ithaeur, Irraka.\n\r", ch);
	return;
}

void do_lodg (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (strlen (ch->courtname) > 1)
	{
		send_to_char ("But you have already chosen a Court!\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Please choose a Court.\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "Arms"))
	{
		free_string (ch->courtname);
		ch->courtname = str_dup ("Arms");
		send_to_char ("You are now belong to the Arms Court.\n\r", ch);
		return;
	}
	else if (!str_cmp (arg, "Stone"))
	{
		free_string (ch->courtname);
		ch->courtname = str_dup ("Stone");
		send_to_char ("You now belong to the Stone Court.\n\r", ch);
		return;
	}
	else
		send_to_char ("Please choose a Court.\n\r", ch);
	return;
}

void do_discset (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	bool is_offline = FALSE;
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
		send_to_char ("#eSyntax : discset [victim] [contract/all] [level]#n\n\r\n\r", ch);
		send_to_char (" #c-----------------------------------------------\n\r", ch);
		send_to_char (" #C        Supernatural Abilities (Discs)#c:\n\r", ch);
		send_to_char (" #c-----------------------------------------------#n\n\r", ch);
		send_to_char (" Dominate, Hearth, Hours, Mirror, Moon, Animalism, Darkness, \n\r", ch);
		send_to_char (" Oath and Punishment, Animation, Artifice, Communion,\n\r", ch);
		send_to_char (" Den, Elements, Forge, Reflections, Shade and Spirit,\n\r", ch);
		send_to_char (" Separation, Stone, Vainglory, Wild, Eternal Spring,\n\r", ch);
		send_to_char (" Eternal Summer, Eternal Winter, Celerity, Obfuscate,\n\r", ch);
		send_to_char (" Fleeting Spring, Fang and Talon, Fleeting Winter,\n\r", ch);
		send_to_char (" Fleeting Autumn, Fleeting Summer, #eTenure#n, #ePhagia#n\n\r", ch);
		send_to_char (" #eTheban Sorcery#n\n\r", ch);
		send_to_char (" #c-----------------------------------------------#n\n\r", ch);
		send_to_char (" #C     Note: Omit spaces when using discset      #n\n\r", ch);
		send_to_char (" #c-----------------------------------------------#n\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		d = alloc_mem(sizeof(DESCRIPTOR_DATA));
		if (load_char_obj(d, arg1))
		{
			victim = d->character;
			is_offline = TRUE;
		}
		else
		{
			send_to_char("No such character exists.\n\r", ch);
			return;
		}
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPCs.\n\r", ch);
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

	else if (!str_cmp (arg2, "dominate"))
		disc = DISC_DOMINATE;
	else if (!str_cmp (arg2, "auspex"))
		disc = DISC_AUSPEX;
	else if (!str_cmp (arg2, "animalism"))
		disc = DISC_ANIMALISM;
	else if (!str_cmp (arg2, "celerity"))
		disc = DISC_CELERITY;
	else if (!str_cmp (arg2, "nightmare"))
		disc = DISC_NIGHTMARE;
	else if (!str_cmp (arg2, "resilience"))
		disc = DISC_RESILIENCE;
	else if (!str_cmp (arg2, "vigor"))
		disc = DISC_VIGOR;
	else if (!str_cmp (arg2, "fleeting winter"))
		disc = DISC_FLEETINGWINTER;
	else if (!str_cmp (arg2, "animation"))
		disc = DISC_ANIMATION;
	else if (!str_cmp (arg2, "eternalsummer"))
		disc = DISC_ETERNALSUMMER;
	else if (!str_cmp (arg2, "Forge"))
		disc = DISC_FORGE;
	else if (!str_cmp (arg2, "obfuscate"))
		disc = DISC_OBFUSCATE;
	else if (!str_cmp (arg2, "darkness"))
		disc = DISC_DARKNESS;
	else if (!str_cmp (arg2, "artifice"))
		disc = DISC_ARTIFICE;
	else if (!str_cmp (arg2, "eternalspring"))
		disc = DISC_ETERNALSPRING;
	else if (!str_cmp (arg2, "protean"))
		disc = DISC_PROTEAN;
	else if (!str_cmp (arg2, "Fleeting Summer"))
		disc = DISC_FLEETINGSUMMER;
	else if (!str_cmp (arg2, "Reflections"))
		disc = DISC_REFLECTIONS;
	else if (!str_cmp (arg2, "Stone"))
		disc = DISC_STONE;
	else if (!str_cmp (arg2, "thebansorcery"))
		disc = DISC_THEBANSORCERY;
	else if (!str_cmp (arg2, "Separation"))
		disc = DISC_SEPARATION;
	else if (!str_cmp (arg2, "shadeandspirit"))
		disc = DISC_SHADEANDSPIRIT;
	else if (!str_cmp (arg2, "communion"))
		disc = DISC_COMMUNION;
	else if (!str_cmp (arg2, "majesty"))
		disc = DISC_MAJESTY;
	else if (!str_cmp (arg2, "hours"))
		disc = DISC_HOURS;
	else if (!str_cmp (arg2, "moon"))
		disc = DISC_MOON;
    else if ( !str_cmp( arg2, "vainglory" )) 
		disc = DISC_VAINGLORY;
    else if ( !str_cmp( arg2, "wild" )) 
		disc = DISC_WILD;
	else if (!str_cmp (arg2, "mirror"))
		disc = DISC_MIRROR;
	else if (!str_cmp (arg2, "fleetingspring"))
		disc = DISC_FLEETINGSPRING;
	else if (!str_cmp (arg2, "fleetingautumn"))
		disc = DISC_FLEETINGAUTUMN;
	else if (!str_cmp (arg2, "eternalwinter"))
		disc = DISC_ETERNALWINTER;
	else if (!str_cmp (arg2, "cunning"))
		disc = DISC_CUNNING;
	else if (!str_cmp (arg2, "glory"))
		disc = DISC_GLORY;
 	else if (!str_cmp (arg2, "honor"))
		disc = DISC_HONOR;
	else if (!str_cmp (arg2, "purity"))
		disc = DISC_PURITY;
 	else if (!str_cmp (arg2, "wisdom"))
		disc = DISC_WISDOM;
	
	else
	{
		do_discset (ch, "");
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;

	if (value < 0 || value > 10)
	{
		send_to_char ("Range is 0 - 10.\n\r", ch);
		return;
	}
	victim->pcdata->powers[disc] = value;
	send_to_char ("Done.\n\r", ch);
	if (is_offline)
	{
		send_to_char("#RNote#n: Character is offline\n\r", ch);
		save_char_obj(victim);
		free_char(victim);
	}
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
		send_to_char ("  Human, Lupus, or Metis\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPCs.\n\r", ch);
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
	else if (!str_cmp (arg2, "human"))
		breed = BREED_HUMAN;
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


void do_setorganization (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int organizations = 0;
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
		send_to_char ("Syntax : setorganization [victim] [organization/all] [level]\n\r\n\r", ch);
		send_to_char (" Organization being one of:\n\r", ch);
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
		send_to_char ("Not on NPCs.\n\r", ch);
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

		for (organizations = 0; organizations <= ORGANIZATION_MAX; organizations++)
			victim->pcdata->organizations[organizations] = value;

		send_to_char ("Done.\n\r", ch);
		return;
	}

	else if (!str_cmp (arg2, "blackfuries"))
		organizations = ORGANIZATION_BLACK_FURIES;
	else if (!str_cmp (arg2, "bonegnawers"))
		organizations = ORGANIZATION_BONE_GNAWERS;
	else if (!str_cmp (arg2, "childrenofgaia"))
		organizations = ORGANIZATION_CHILDREN_OF_GAIA;
	else if (!str_cmp (arg2, "fianna"))
		organizations = ORGANIZATION_FIANNA;
	else if (!str_cmp (arg2, "getoffenris"))
		organizations = ORGANIZATION_GET_OF_FENRIS;
	else if (!str_cmp (arg2, "glasswalkers"))
		organizations = ORGANIZATION_GLASS_WALKERS;
	else if (!str_cmp (arg2, "redtalons"))
		organizations = ORGANIZATION_RED_TALONS;
	else if (!str_cmp (arg2, "shadowlords"))
		organizations = ORGANIZATION_SHADOW_LORDS;
	else if (!str_cmp (arg2, "silentstriders"))
		organizations = ORGANIZATION_SILENT_STRIDERS;
	else if (!str_cmp (arg2, "silverfangs"))
		organizations = ORGANIZATION_SILVER_FANGS;
	else if (!str_cmp (arg2, "stargazers"))
		organizations = ORGANIZATION_STARGAZERS;
	else if (!str_cmp (arg2, "uktena"))
		organizations = ORGANIZATION_UKTENA;
	else if (!str_cmp (arg2, "wendigos"))
		organizations = ORGANIZATION_WENDIGOS;
	else

	value = is_number (arg3) ? atoi (arg3) : -1;

	if (value < 0 || value > 40)
	{
		send_to_char ("Range is 0 - 40.\n\r", ch);
		return;
	}
	victim->pcdata->organizations[organizations] = value;
	send_to_char ("Done.\n\r", ch);
	return;
}


void do_truesight (CHAR_DATA * ch, char *argument)
{
	int po;

	if (IS_NPC (ch))
		return;
	if (get_curr_per (ch) > 4)
		po = get_curr_per (ch);
	else
		po = 4;

	if (!IS_VAMPIRE (ch) && !IS_GHOUL (ch) && !IS_CHANGELING (ch))
	{
		send_to_char ("#eHuh?#n\n\r", ch);
		return;
	}

	if (IS_CHANGELING (ch) && get_breed (ch, BREED_LUPUS) < 1)
	{
		send_to_char ("#eChangelings cannot use #cA#Cuspex 1#e, #nH#eeightened#n S#eenses.#n\n\r", ch);
		return;
	}

	if (get_disc (ch, DISC_AUSPEX) < 1 && (IS_VAMPIRE (ch) || IS_GHOUL (ch)))
	{
		send_to_char ("#eYou require level 1 #cA#Cuspex#e to use #nH#eeightened#n S#eenses.#n\n\r", ch);

		return;
	}

	if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
	{
		REMOVE_BIT (ch->more2, MORE2_HEIGHTENSENSES);
		send_to_char ("#c(#CAuspex 1 Deactivated#c) #eYour senses return to #nnormal#e.#n\n\r", ch);
		ch->pcdata->po = 3;
	}
	else
	{
		SET_BIT (ch->more2, MORE2_HEIGHTENSENSES);
		send_to_char ("#c(#CAuspex 1 Activated#c) #eYour senses increase to#n i#wn#nc#wr#ne#wd#ni#wb#nl#we p#nr#wo#np#wo#nr#wt#ni#wo#nn#ws#e.#n\n\r", ch);
		ch->pcdata->po = po;
	}
	return;
}

void do_skinmask (CHAR_DATA * ch, char *argument)
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
	if (!IS_CHANGELING (ch) && !IS_GHOUL (ch))
	{
		send_to_char ("#eHuh?#n\n\r", ch);
		return;
	}

	if (get_disc (ch, DISC_MIRROR) < 2)
	{
		send_to_char ("#RError: #eYou require the Contract Mirror at level 2 to change your appearance completely#e.#n\n\r", ch);
		return;
	}
	}
	if (argument[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char ("#eChange to look like whom?#n\n\r", ch);
		return;
	}

	if (!str_cmp (arg1, "mod") || !str_cmp (arg1, "modify"))
	{
		argument = one_argument (argument, arg1);
		argument = one_argument (argument, arg2);
		argument = one_argument (argument, arg3);
		if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char ("#eSyntax#n:#e Skinmask Modify #n<#etarget#n>#e <#nmodification#e>#n\n\r", ch);
			return;
		}
		if ((victim = get_char_room (ch, arg2)) == NULL)
		{
			send_to_char ("#RError: #eThey are not here.#n\n\r", ch);
			return;
		}
		if (IS_NPC (victim) && get_trust(ch) < LEVEL_STORYTELLER)
		{
			send_to_char ("#RError: #eYou cannot#n skinmask#e an unwilling person.#n\n\r", ch);
			return;
		}
		if (!IS_IMMUNE (victim, IMM_VAMPIRE) && ch != victim)
		{
			send_to_char ("#RError: #eYou cannot#n skinmask#e an unwilling person.#n\n\r", ch);
			return;
		}
		return;
	}

	if (strlen (argument) > 50)
	{
		send_to_char ("#RError: #ePlease limit your name to #n50#e characters or less.#n\n\r", ch);
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
		send_to_char ("#RError: #eNot while polymorphed.#n\n\r", ch);
		return;
	}

	if (IS_POLYAFF (ch, POLY_ZULO))
	{
		send_to_char ("#RError: #eNot while in Zulo Form.#n\n\r", ch);
		return;
	}

	if (!str_cmp (argument, ch->name) || !str_cmp (argument, "self"))
	{
		if (!IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_DISGUISED))
		{
			send_to_char ("#RError: #eYou already look like yourself!#n\n\r", ch);
			return;
		}
		if (!IS_POLYAFF (ch, POLY_SKINMASK))
		{
			send_to_char ("#RError: #eNothing happens.#n\n\r", ch);
			return;
		}
		sprintf (buf, "#e$n's features twist and distort until $e looks like %s.#n", ch->name);
		act (buf, ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT (ch->polyaff, POLY_SKINMASK);
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
		REMOVE_BIT (ch->extra, EXTRA_SEPARATION);
		free_string (ch->morph);
		ch->morph = str_dup ("");
		act ("#eYour features twist and distort until you look like $n.#n", ch, NULL, NULL, TO_CHAR);
		return;
	}
	if (IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		if (!IS_POLYAFF (ch, POLY_SKINMASK))
		{
			send_to_char ("#RError: #eNothing happens.#n\n\r", ch);
			return;
		}
		sprintf (buf, "#eYour features twist and distort until you look like %s.#n\n\r", argument);
		send_to_char (buf, ch);
		sprintf (buf, "#e$n's features twist and distort until $e looks like %s.#n", argument);
		act (buf, ch, NULL, NULL, TO_ROOM);
		free_string (ch->morph);
		ch->morph = str_dup (argument);
		return;
	}
	sprintf (buf, "#eYour features twist and distort until you look like %s.#n\n\r", argument);
	send_to_char (buf, ch);
	sprintf (buf, "#e$n's features twist and distort until $e looks like %s.#n", argument);
	act (buf, ch, NULL, NULL, TO_ROOM);
	SET_BIT (ch->polyaff, POLY_SKINMASK);
	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->vampaff, VAM_DISGUISED);
	SET_BIT (ch->extra, EXTRA_SEPARATION);
	free_string (ch->morph);
	ch->morph = str_dup (argument);
	return;
}


void do_smoke4 (CHAR_DATA * ch, char *argument)
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
		send_to_char ("#eHuh?#n\n\r", ch);
		return;
	}

	if (get_disc (ch, DISC_OBFUSCATE) < 4)
	{
		send_to_char ("#eYou do not have enough mastery of #Po#pbfuscate#e to change your appearance.#n\n\r", ch);
		return;
	}
	}
	if (argument[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char ("#eChange to look like whom?#n\n\r", ch);
		return;
	}

	

	if (strlen (argument) > MAX_ROOMDESC_LENGTH)
	{
		send_to_char ("#RError: #eName too long.#n\n\r", ch);
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
		send_to_char ("#RError: #eNot while polymorphed.#n\n\r", ch);
		return;
	}

	if (IS_POLYAFF (ch, POLY_ZULO))
	{
		send_to_char ("#RError: #eNot while in Zulo Form.#n\n\r", ch);
		return;
	}

	if (!str_cmp (argument, ch->name) || !str_cmp (argument, "self"))
	{
		if (!IS_AFFECTED (ch, AFF_POLYMORPH) && !IS_VAMPAFF (ch, VAM_DISGUISED))
		{
			send_to_char ("#RError: #eYou already look like yourself!#n\n\r", ch);
			return;
		}
		if (!IS_POLYAFF (ch, POLY_SKINMASK))
		{
			send_to_char ("#RError: #eNothing happens.#n\n\r", ch);
			return;
		}
		sprintf (buf, "#n$n #esuddenly looks different!#n\n\r");
		act (buf, ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT (ch->polyaff, POLY_SKINMASK);
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
		REMOVE_BIT (ch->extra, EXTRA_SEPARATION);
		free_string (ch->morph);
		ch->morph = str_dup ("");
		act ("#eYou now look like $n.#n\n\r", ch, NULL, NULL, TO_CHAR);
		return;
	}
	if (IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		if (!IS_POLYAFF (ch, POLY_SKINMASK))
		{
			send_to_char ("#RError: #eNothing happens.#n\n\r", ch);
			return;
		}
		sprintf (buf, "#eYou now look like %s.#n\n\r", argument);
		send_to_char (buf, ch);
		sprintf (buf, "#n$n #esuddenly looks different!#n\n\r");
		act (buf, ch, NULL, NULL, TO_ROOM);
		free_string (ch->morph);
		ch->morph = str_dup (argument);
		return;
	}
	sprintf (buf, "#eYou now look like %s.#n\n\r#n", argument);
	send_to_char (buf, ch);
	sprintf (buf, "#n$n #esuddenly looks different!#n\n\r");
	act (buf, ch, NULL, NULL, TO_ROOM);
	SET_BIT (ch->polyaff, POLY_SKINMASK);
	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->vampaff, VAM_DISGUISED);

	free_string (ch->morph);
	ch->morph = str_dup (argument);
	return;
}
