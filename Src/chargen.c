/*=============================================================================
*      Innocence Lost RPI -- CharGen Code, Chronicles of Darkness 2E          *
*     Written by Enigma (Leanna Clarke) -- Last Updated December 2016         *
*     Heavily influenced by Amaranth's do_train                               *
=============================================================================*/

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
#include "merits.h"

bool has_merit args ((CHAR_DATA * ch, char * merit));
int find_merit args ((CHAR_DATA * ch, char * merit_name));

/* Setting class */

void do_template( CHAR_DATA *ch, char *argument )
{
	if ((!IS_NPC (ch) && !IS_SET (ch->in_room->room_flags, ROOM_CHARGEN)))
	{
	send_to_char ("You cannot choose a template outside of CharGen.\n\r", ch);
	return;
	}
	
if ((!IS_NPC (ch) && IS_SET (ch->in_room->room_flags, ROOM_CHARGEN)))
{
	char arg1[MAX_INPUT_LENGTH];
	bool is_offline = FALSE;	
	argument = one_argument (argument, arg1);
	if (IS_NPC (ch))
		return;
	
	if (arg1[0] == '\0')
	{
		send_to_char ("Syntax: class <class>.\n\r", ch);
		send_to_char ("Classes:\n\r", ch);
		send_to_char ("Mortal, Vampire\n\r", ch);
		return;
	}
	
	if (ch->class != CLASS_NONE)
	{
		send_to_char ("#y[#gCharGen#y]#n You have already selected a class template.\n\r", ch);
		return;
	}

	if (!str_cmp (arg1, "vampire"))
	{
		ch->class = CLASS_VAMPIRE;
		ch->vamppot = 1;
		update_pot (ch);
		ch->pcdata->bloodlined[0] = -1;
		ch->pcdata->bloodlined[1] = -1;
		ch->pcdata->bloodlined[2] = -1;
		send_to_char ("Ok.\n\r", ch);
		ch->exp += 25;
		if (!is_offline) send_to_char ("#y[#gCharGen#y]#n Your template is set to #RVampire#n.\n\r", ch);
	}	
	
	else if (!str_cmp (arg1, "mortal"))
	{
		ch->class = CLASS_NONE;
		send_to_char ("Ok.\n\r", ch);
		ch->exp += 15;
		if (!is_offline) send_to_char ("#y[#gCharGen#y]#n Your template is set to #cMortal#n.\n\r", ch);
	}	
	
/*	else if (!str_cmp(arg1, "mage"))
	{
		ch->class = CLASS_GEIST;
		ch->vamppot = 1;
		update_pot (ch);
		ch->pcdata->bloodlined[0] = -1;
		ch->pcdata->bloodlined[1] = -1;
		ch->pcdata->bloodlined[2] = -1;
		send_to_char ("Ok.\n\r", ch);
		ch->exp += 35;
		if (!is_offline) send_to_char ("#y[#gCharGen#y]#n Your template is set to #pMage#n.\n\r", ch);
	}*/

	else
	{
		send_to_char ("Syntax: class <class>.\n\r", ch);
		send_to_char ("Classes:\n\r", ch);
		send_to_char ("Mortal, Vampire\n\r", ch);
		return;
	}
}
}

/* Setting Clan */

void do_clan (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int i = 0;
	int clan = -1;

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if ((!IS_NPC (ch) && !IS_SET (ch->in_room->room_flags, ROOM_CHARGEN)))
	{
	send_to_char ("You cannot change your Clan outside of CharGen.\n\r", ch);
	return;
	}
	
	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("#y[#gCharGen#y]#n Only Vampires can set a Clan.\n\r", ch);
		return;
	}

	if (ch->pcdata->clan != -1)
	{
		
	if (clan == CLAN_DAEVA)
	{
    ch->pcdata->powers[DISC_CELERITY] = 1;
	ch->pcdata->powers[DISC_VIGOR] = 1;
	ch->pcdata->powers[DISC_MAJESTY] = 1;
	}
	
	if (clan == CLAN_GANGREL)
	{
    ch->pcdata->powers[DISC_ANIMALISM] = 1;
	ch->pcdata->powers[DISC_PROTEAN] = 1;
	ch->pcdata->powers[DISC_RESILIENCE] = 1;
	}
	
	if (clan == CLAN_MEKHET)
	{
    ch->pcdata->powers[DISC_AUSPEX] = 1;
	ch->pcdata->powers[DISC_CELERITY] = 1;
	ch->pcdata->powers[DISC_OBFUSCATE] = 1;
	}
	
	if (clan == CLAN_NOSFERATU)
	{
    ch->pcdata->powers[DISC_NIGHTMARE] = 1;
	ch->pcdata->powers[DISC_OBFUSCATE] = 1;
	ch->pcdata->powers[DISC_VIGOR] = 1;
	}
		
	if (clan == CLAN_VENTRUE)
	{
    ch->pcdata->powers[DISC_ANIMALISM] = 1;
	ch->pcdata->powers[DISC_DOMINATE] = 1;
	ch->pcdata->powers[DISC_RESILIENCE] = 1;
	}
		
		sprintf (buf, "#y[#gCharGen#y]#n Clan %s set.\n\r", clan_names[ch->pcdata->clan][0]);
		send_to_char (buf, ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		send_to_char ("#y[#gCharGen#y]#n Clan options#y:#n Daeva, Gangrel, Mekhet, Nosferatu, Ventrue", ch);
		return;
	}

	for (i = 0; i < MAX_CLANS; i++)
	{
		if (!str_cmp (arg, clan_names[i][0]))
		{
			clan = i;
			break;
		}
	}

	if (clan == -1)
	{
		send_to_char ("#y[#gCharGen#y]#n Unknown Clan, Try Again\n\r", ch);
		return;
	}

	ch->pcdata->clan = clan;
	ch->pcdata->clant = 1;
	sprintf (buf, "#y[#gCharGen#y]#n Clan %s set.", clan_names[clan][0]);
	send_to_char (buf, ch);
	return;
}

/* Setting Covenant */

void set_chargcov (CHAR_DATA * ch, char *side)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC (ch))
	{
		bug ("Set_covenant: NPC.", 0);
		return;
	}

	strcpy (buf, side);

	free_string (ch->side);
	ch->side = str_dup (buf);
	return;
}

void do_covenant (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (!IS_SET (ch->in_room->room_flags, ROOM_CHARGEN))
	{
	send_to_char ("You cannot change your Covenant outside of CharGen.\n\r", ch);
	return;
	}

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("#y[#gCharGen#y]#n Only Vampires may set a Covenant.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("#y[#gCharGen#y]#n Covenant Options#y:#n Society, Cause, Unaffiliated.\n\r", ch);
		return;
	}

	else 
	{
	smash_tilde (argument);
	set_chargcov (ch, argument);
	send_to_char ("#y[#gCharGen#y]#n Covenant set!\n\r", ch);
	}
}

/* Set Virtue */

void set_chargvirtue (CHAR_DATA * ch, char *virtue)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC (ch))
	{
		bug ("Set_virtue: NPC.", 0);
		return;
	}

	strcpy (buf, virtue);

	free_string (ch->virtue);
	ch->virtue = str_dup (buf);
	return;
}

void do_virtue (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (!IS_SET (ch->in_room->room_flags, ROOM_CHARGEN))
	{
	send_to_char ("You cannot change your Virtue outside of CharGen.\n\r", ch);
	return;
	}

	if (IS_VAMPIRE (ch))
	{
		send_to_char ("#y[#gCharGen#y]#n Vampires cannot set a Virtue.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("#y[#gCharGen#y]#n Please set your Virtue, #wvirtue <virtue>#n.\n\r", ch);
		return;
	}

	else 
	{
	smash_tilde (argument);
	set_chargvirtue (ch, argument);
	send_to_char ("#y[#gCharGen#y]#n Virtue set!\n\r", ch);
	}
}

/* Set Vice */

void set_chargvice (CHAR_DATA * ch, char *vice)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC (ch))
	{
		bug ("Set_vice: NPC.", 0);
		return;
	}

	strcpy (buf, vice);

	free_string (ch->vice);
	ch->vice = str_dup (buf);
	return;
}

void do_vice (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (!IS_SET (ch->in_room->room_flags, ROOM_CHARGEN))
	{
	send_to_char ("You cannot change your Vice outside of CharGen.\n\r", ch);
	return;
	}

	if (IS_VAMPIRE (ch))
	{
		send_to_char ("#y[#gCharGen#y]#n Vampires cannot set a Vice.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("#y[#gCharGen#y]#n Please set your Vice, #wvice <vice>#n.\n\r", ch);
		return;
	}

	else 
	{
	smash_tilde (argument);
	set_chargvice (ch, argument);
	send_to_char ("#y[#gCharGen#y]#n Vice set!\n\r", ch);
	}
}

/* Set Mask */

void do_mask (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;
		
	if (!IS_SET (ch->in_room->room_flags, ROOM_CHARGEN))
	{
	send_to_char ("You cannot change your Mask outside of CharGen.\n\r", ch);
	return;
	}
	
	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("#y[#gCharGen#y]#n Only Vampires may set a Mask.\n\r", ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char ("#y[#gCharGen#y]#n Please set your Mask, #wmask <mask>#n.\n\r", ch);
		return;
	}	

	else 
	{
	smash_tilde (argument);
	set_chargvirtue (ch, argument);
	send_to_char ("#y[#gCharGen#y]#n Mask set!\n\r", ch);
	}
}

/* Set Dirge */

void do_dirge (CHAR_DATA * ch, char *argument)
{
	if (IS_NPC (ch))
		return;

	if (!IS_SET (ch->in_room->room_flags, ROOM_CHARGEN))
	{
	send_to_char ("You cannot change your Dirge outside of CharGen.\n\r", ch);
	return;
	}

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("#y[#gCharGen#y]#n Only Vampires may set a Dirge.\n\r", ch);
		return;
	}
	
	if (argument[0] == '\0')
	{
		send_to_char ("#y[#gCharGen#y]#n Please set your Dirge, #wdirge <dirge>#n.\n\r", ch);
		return;
	}

	else 
	{
	smash_tilde (argument);
	set_chargvice (ch, argument);
	send_to_char ("#y[#gCharGen#y]#n Dirge set!\n\r", ch);
	}
}

/* Bonus XP and Training System, adapted from Amaranth/Godwars */

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
	int max_train_trait = 0;
	int max_train_ability = MAX_TRAINABLE_ABILITY - 1;

	argument = one_argument (argument, arg1);

	if (IS_NPC (ch))
		return;

	cost = 1;

	if (ch->class == CLASS_VAMPIRE)
		max_train_ability = MAX_TRAINABLE_ABILITY;

	if (ch->class == CLASS_VAMPIRE)
	{
		switch (ch->vamppot)
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
	
	// General pools

	/*if (!str_cmp (arg1, "willpower"))
	{
		cost = 1;
		pAbility = &ch->pcdata->willpower[WILLPOWER_MAX];
		pOutput = "willpower";
	}*/
	
	if (!str_cmp (arg1, "potency"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Potency is a Vampire-only Trait.\n\r", ch);
		 return;
	     }

		cost = 5;
		pAbility = &ch->vamppot;
		pOutput = "potency";
	}
	
	else if (!str_cmp (arg1, "humanity"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Humanity is a Vampire-only Trait.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->clant;
		pOutput = "humanity";
	}
	
	else if (!str_cmp (arg1, "integrity"))
	{
	     if (IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Integrity is a Mortal-only Trait.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->clant;
		pOutput = "integrity";
	}
	
	// Attributes
	
    else if (!str_cmp (arg1, "strength"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_STR];
		pOutput = "strength";
	}

	else if (!str_cmp (arg1, "dexterity"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_DEX];
		pOutput = "dexterity";
	}

	else if (!str_cmp (arg1, "stamina"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_STA];
		pOutput = "stamina";
	}
	
	else if (!str_cmp (arg1, "presence"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_CHA];
		pOutput = "presence";
	}
	
	else if (!str_cmp (arg1, "manipulation"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_MAN];
		pOutput = "manipulation";
	}	
	
	else if (!str_cmp (arg1, "composure"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_APP];
		ch->pcdata->willpower[WILLPOWER_MAX] = ((ch->attributes[ATTRIB_PER]) + (ch->attributes[ATTRIB_APP]) + 1);
		pOutput = "composure";
	}
	
	else if (!str_cmp (arg1, "resolve"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_PER];
		ch->pcdata->willpower[WILLPOWER_MAX] = ((ch->attributes[ATTRIB_PER]) + (ch->attributes[ATTRIB_APP]) + 1);
		pOutput = "resolve";
	}
	
	else if (!str_cmp (arg1, "intelligence"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_INT];
		pOutput = "intelligence";
	}
	
	else if (!str_cmp (arg1, "wits"))
	{
		cost = 4;
		pAbility = &ch->attributes[ATTRIB_WIT];
		pOutput = "wits";
	}
	
    // Skills
	
	else if (!str_cmp (arg1, "athletics"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_ATHLETICS];
		pOutput = "athletics";
	}
	
	else if (!str_cmp (arg1, "brawl"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_BRAWL];
		pOutput = "brawl";
	}
	
	else if (!str_cmp (arg1, "firearms"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_FIREARMS];
		pOutput = "firearms";
	}
	
	else if (!str_cmp (arg1, "larceny"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_LARCENY];
		pOutput = "larceny";
	}
	
	else if (!str_cmp (arg1, "stealth"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_STEALTH];
		pOutput = "stealth";
	}
	
	else if (!str_cmp (arg1, "survival"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_SURVIVAL];
		pOutput = "survival";
	}
	
	else if (!str_cmp (arg1, "weaponry"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_WEAPONRY];
		pOutput = "weaponry";
	}
	
	else if (!str_cmp (arg1, "drive"))
	{
		cost = 2;
		pAbility = &ch->abilities[PHYSICAL][TAL_DRIVE];
		pOutput = "drive";
	}
	
	else if (!str_cmp (arg1, "animalken"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_ANIMALKEN];
		pOutput = "animalken";
	}
	
	else if (!str_cmp (arg1, "empathy"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_EMPATHY];
		pOutput = "empathy";
	}
	
	else if (!str_cmp (arg1, "expression"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_EXPRESSION];
		pOutput = "expression";
	}
	
	else if (!str_cmp (arg1, "intimidation"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_INTIMIDATION];
		pOutput = "intimidation";
	}
	
	else if (!str_cmp (arg1, "socialize"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_SOCIALIZE];
		pOutput = "socialize";
	}
	
	else if (!str_cmp (arg1, "streetwise"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_STREETWISE];
		pOutput = "streetwise";
	}
	
	else if (!str_cmp (arg1, "persuasion"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_PERSUASION];
		pOutput = "persuasion";
	}
	
	else if (!str_cmp (arg1, "subterfuge"))
	{
		cost = 2;
		pAbility = &ch->abilities[SOCIAL][SKI_SUBTERFUGE];
		pOutput = "subterfuge";
	}
	
	else if (!str_cmp (arg1, "academics"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_ACADEMICS];
		pOutput = "academics";
	}
	
	else if (!str_cmp (arg1, "computer"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_COMPUTER];
		pOutput = "computer";
	}

	else if (!str_cmp (arg1, "crafts"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_CRAFTS];
		pOutput = "crafts";
	}
	
	else if (!str_cmp (arg1, "investigation"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_INVESTIGATION];
		pOutput = "investigation";
	}
	
	else if (!str_cmp (arg1, "medicine"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_MEDICINE];
		pOutput = "medicine";
	}	
	
	else if (!str_cmp (arg1, "occult"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_OCCULT];
		pOutput = "occult";
	}
	
	else if (!str_cmp (arg1, "politics"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_POLITICS];
		pOutput = "politics";
	}
	
	else if (!str_cmp (arg1, "science"))
	{
		cost = 2;
		pAbility = &ch->abilities[MENTAL][KNO_SCIENCE];
		pOutput = "science";
	}
	
    // Disciplines

	else if (!str_cmp (arg1, "auspex"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 4;
		pAbility = &ch->pcdata->powers[DISC_AUSPEX];
		pOutput = "auspex";
	}
	
	else if (!str_cmp (arg1, "dominate"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 4;
		pAbility = &ch->pcdata->powers[DISC_DOMINATE];
		pOutput = "dominate";
	}
	
	else if (!str_cmp (arg1, "majesty"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 4;
		pAbility = &ch->pcdata->powers[DISC_MAJESTY];
		pOutput = "majesty";
	}

	else if (!str_cmp (arg1, "nightmare"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 4;
		pAbility = &ch->pcdata->powers[DISC_NIGHTMARE];
		pOutput = "nightmare";
	}
	
	else if (!str_cmp (arg1, "protean"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 4;
		pAbility = &ch->pcdata->powers[DISC_PROTEAN];
		pOutput = "protean";
	}
	
	else if (!str_cmp (arg1, "animalism"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->powers[DISC_ANIMALISM];
		pOutput = "animalism";
	}
	
	else if (!str_cmp (arg1, "celerity"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->powers[DISC_CELERITY];
		pOutput = "celerity";
	}
	
	else if (!str_cmp (arg1, "obfuscate"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->powers[DISC_OBFUSCATE];
		pOutput = "obfuscate";
	}
	
	else if (!str_cmp (arg1, "resilience"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->powers[DISC_RESILIENCE];
		pOutput = "resilience";
	}
	
	else if (!str_cmp (arg1, "vigor"))
	{
	     if (!IS_VAMPIRE (ch))
	     {
		 send_to_char ("#y[#gTrain#y]#n Disciplines are Vampire-only Traits.\n\r", ch);
		 return;
	     }
		
		cost = 3;
		pAbility = &ch->pcdata->powers[DISC_VIGOR];
		pOutput = "vigor";
	}
	
// Merits

/*	else if (!str_cmp (arg1, "commonsense"))
	{
		cost = 1;
		pAbility = &ch->pcdata->powers[MERIT_COMMONSENSE];
		pOutput = "the Common Sense Merit #e(Buy to 3pts)#n";
	}*/

	else
	{
		sprintf (buf, "#g-------------------------------------------------------------------------------#n\n\r#nYou can train the following Character Traits:\n\r#g-------------------------------------------------------------------------------#n\n\r");
		send_to_char (buf, ch);

		send_to_char ("#wAttributes#y:#n\n\r", ch);
        send_to_char("#nStrength#e...............4#n   Presence#e...............4#n   Resolve#e.................4#n\n\r", ch);
        send_to_char("#nDexterity#e..............4#n   Manipulation#e...........4#n   Intelligence#e............4#n\n\r", ch);
        send_to_char("#nStamina#e................4#n   Composure#e..............4#n   Wits#e....................4#n\n\r", ch);
		
		send_to_char ("\n\r#wSkills#y:#n\n\r", ch);
		send_to_char ("#nAthletics#e..............2   #nAnimalken#e..............2   #nAcademics#e...............2#n\n\r", ch);
		send_to_char ("#nBrawl#e..................2   #nEmpathy#e................2   #nComputer#e................2#n\n\r", ch);
		send_to_char ("#nFirearms#e...............2   #nExpression#e.............2   #nCrafts#e..................2#n\n\r", ch);
		send_to_char ("#nLarceny#e................2   #nIntimidation#e...........2   #nInvestigation#e...........2#n\n\r", ch);
		send_to_char ("#nStealth#e................2   #nSocialize#e..............2   #nMedicine#e................2#n\n\r", ch);
		send_to_char ("#nSurvival#e...............2   #nStreetwise#e.............2   #nOccult#e..................2#n\n\r", ch);
		send_to_char ("#nWeaponry#e...............2   #nPersuasion#e.............2   #nPolitics#e................2#n\n\r", ch);
		send_to_char ("#nDrive#e..................2   #nSubterfuge#e.............2   #nScience#e.................2#n\n\r", ch);

		if (ch->class == CLASS_VAMPIRE)
		{
        send_to_char("#g-------------------------------------------------------------------------------#n\n\r", ch);
        send_to_char("#nCommon Disciplines                                     Out of Clan Disciplines#n\n\r", ch);
        send_to_char("#g-------------------------------------------------------------------------------#n\n\r", ch);
        send_to_char("#nAnimalism#e..............3                               #nAuspex#e.................4#n\n\r", ch);
        send_to_char("#nCelerity#e...............3                               #nDominate#e...............4#n\n\r", ch);
        send_to_char("#nObfuscate#e..............3                               #nMajesty#e................4#n\n\r", ch);
        send_to_char("#nResilience#e.............3                               #nNightmare#e..............4#n\n\r", ch);
        send_to_char("#nVigor#e..................3                               #nProtean#e................4#n", ch);
		}
			else			
				send_to_char("                           ", ch);

		send_to_char ("\n\r#g-------------------------------------------------------------------------------#n\n\r", ch);
		sprintf (buf, "#nYou have #W%d XP#n available for training.#n\n\r", ch->exp);
		send_to_char (buf, ch);
		send_to_char ("#g-------------------------------------------------------------------------------#n\n\r", ch);
		return;
	}

    // Use this to limit vamppot
	if ((*pAbility >= 5) && (!str_cmp (arg1, "clanstatus") || !str_cmp (arg1, "citystatus") || !str_cmp (arg1, "vision") || !str_cmp (arg1, "covstatus") || !str_cmp (arg1, "mantle")))
	{
		if (last)
			act ("#y[#gTrain#y]#n Your $T is already at maximum.",  ch, NULL, pOutput, TO_CHAR);
		return;
	}

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if ((!str_cmp (arg1, ability_names[i][j][0])) && (ch->abilities[i][j] >= max_train_ability))
			{
				if (last)
					send_to_char ("#y[#gTrain#y]#n You already know too much about the ability.\n\r", ch);
				return;
			}


			if ((!str_cmp (arg1, ability_names[i][j][0])) && (ch->pcdata->abilities_points[i][j] < ability_insight_levels[ch->abilities[i][j]]))
			{
				if (last)
					send_to_char ("#y[#gTrain#y]#n You don't have enough insight into that skill to raise it.n\r", ch);
				return;
			}

		}
	}

	if (cost > ch->exp || ch->exp < 1)
	{
		if (last)
			send_to_char ("#y[#gTrain#y]#n You don't have enough xp.\n\r", ch);
		return;
	}

	ch->exp -= (cost < 1 ? 1 : cost);
	*pAbility += 1;
	if (last)
		act ("#y[#gTrain#y]#n You train #w$T#n.", ch, NULL, pOutput, TO_CHAR);
	return;
}

// Design idea for specs: Add to creation, pick 3 specs. Check they have the ability at 1. If they do, allow the spec to be set. If not, tell them they don't have that skill. 

// Has Resources 1, setting to resources 2 -- If has merit resources 1 and arg is resources 2, remove 1, allow and take XP. If not raising from 1 to 2, disallow and mention they need to buy them one level at a time. Add one if for each level progression: 0-1, 1-2, 2-3, 3-4, 4-5.
	
// Alternatively, "train resources" removes all instances of resources from merits and adds whatever they asked for. Cost = number of rating.

void do_finish(CHAR_DATA *ch, char *argument)
{
	if ((!IS_NPC (ch) && !IS_SET (ch->in_room->room_flags, ROOM_CHARGEN)))
	{
	send_to_char ("Your character is already finalized.\n\r", ch);
	return;
	}
	
/*	if ((ch->pcdata->surname = NULL))
	{
		send_to_char ("#y[#gCharGen#y]#n You cannot be finalized without a #wsurname#n.\n\r", ch);
		return;
	}*/
	
	send_to_char ("#G[ #gYour character is finalized; welcome to Los Angeles. #G]#n\n\r\n\r", ch);
	act("#g$n finishes Character Generation.#n", ch, NULL, NULL, TO_ROOM);
    transport_char(ch, 30060);
	act("#g$n arrives in Los Angeles.#n", ch, NULL, NULL, TO_ROOM);
	do_look(ch, "");
	return;
}
