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



void do_fangs (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch) && !IS_WEREWOLF (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (IS_VAMPIRE (ch) && ch->pcdata->wolf > 0)
	{
		send_to_char ("Your beast won't let you retract your fangs.\n\r", ch);
		return;
	}

	if (IS_VAMPAFF (ch, VAM_FANGS))
	{
		send_to_char ("Your fangs slide back into your gums.\n\r", ch);
		act ("$n's fangs slide back into $s gums.", ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT (ch->vampaff, VAM_FANGS);
		return;
	}
	send_to_char ("Your fangs extend out of your gums.\n\r", ch);
	act ("A pair of razor sharp fangs extend from $n's mouth.", ch, NULL, NULL, TO_ROOM);
	SET_BIT (ch->vampaff, VAM_FANGS);
	return;
}



void do_slit (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument (argument, arg);

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
		send_to_char ("Syntax: Slit <left/right>.\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "l") || !str_cmp (arg, "left"))
	{
		if ((obj = get_eq_char (ch, WEAR_WIELD)) != NULL)
		{
			if (obj->item_type != ITEM_WEAPON || (obj->value[3] != 1 && obj->value[3] != 3 && obj->value[3] != 5))
			{
				send_to_char ("You need to hold a sharp item in your right hand.\n\r", ch);
				return;
			}
		}
		else if (!IS_VAMPAFF (ch, VAM_CLAWS) && !IS_VAMPAFF (ch, VAM_FANGS))
		{
			send_to_char ("You need to hold a sharp item in your right hand, or use claws or fangs.\n\r", ch);
			return;
		}

		if (IS_ARM_L (ch, SLIT_WRIST))
		{
			send_to_char ("Your left wrist has already been slit.\n\r", ch);
			return;
		}
		if (obj != NULL)
		{
			act ("You slit open your left wrist with $p.", ch, obj, NULL, TO_CHAR);
			act ("$n slits open $s left wrist with $p.", ch, obj, NULL, TO_ROOM);
		}
		else if (IS_VAMPAFF (ch, VAM_CLAWS))
		{
			send_to_char ("You slit open your left wrist with your claws.\n\r", ch);
			act ("$n slits open $s left wrist with $s claws.", ch, NULL, NULL, TO_ROOM);
		}
		else
		{
			send_to_char ("You rip open your left wrist with your fangs.\n\r", ch);
			act ("$n rips open $s left wrist with $s fangs.", ch, NULL, NULL, TO_ROOM);
		}
		SET_BIT (ch->loc_hp[LOC_ARM_L], SLIT_WRIST);
		SET_BIT (ch->loc_hp[6], BLEEDING_WRIST_L);
		return;
	}
	else if (!str_cmp (arg, "r") || !str_cmp (arg, "right"))
	{
		if ((obj = get_eq_char (ch, WEAR_HOLD)) != NULL)
		{
			if (obj->item_type != ITEM_WEAPON || (obj->value[3] != 1 && obj->value[3] != 3 && obj->value[3] != 5))
			{
				send_to_char ("You need to hold a sharp item in your left hand.\n\r", ch);
				return;
			}
		}
		else if (!IS_VAMPAFF (ch, VAM_CLAWS) && !IS_VAMPAFF (ch, VAM_FANGS))
		{
			send_to_char ("You need to hold a sharp item in your left hand, or use claws or fangs.\n\r", ch);
			return;
		}

		if (IS_ARM_R (ch, SLIT_WRIST))
		{
			send_to_char ("Your right wrist has already been slit.\n\r", ch);
			return;
		}
		if (obj != NULL)
		{
			act ("You slit open your right wrist with $p.", ch, obj, NULL, TO_CHAR);
			act ("$n slits open $s right wrist with $p.", ch, obj, NULL, TO_ROOM);
		}
		else if (IS_VAMPAFF (ch, VAM_CLAWS))
		{
			send_to_char ("You slit open your right wrist with your claws.\n\r", ch);
			act ("$n slits open $s right wrist with $s claws.", ch, NULL, NULL, TO_ROOM);
		}
		else
		{
			send_to_char ("You rip open your right wrist with your fangs.\n\r", ch);
			act ("$n rips open $s right wrist with $s fangs.", ch, NULL, NULL, TO_ROOM);
		}
		SET_BIT (ch->loc_hp[LOC_ARM_R], SLIT_WRIST);
		SET_BIT (ch->loc_hp[6], BLEEDING_WRIST_R);
		return;
	}
	else
		send_to_char ("Syntax: Slit <left/right>.\n\r", ch);
	return;
}


void do_feed (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	int blood;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (!IS_ARM_L (ch, SLIT_WRIST) && !IS_ARM_R (ch, SLIT_WRIST))
	{
		send_to_char ("First you need to SLIT your wrist.\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Who do you wish to feed?\n\r", ch);
		return;
	}
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}
	if ((IS_GHOUL (victim) || victim->class == CLASS_NONE) && victim->blood[0] <= 3)
	{
		if (!IS_HERO (victim))
			send_to_char ("Blood does them no good at all.\n\r", ch);
		else
			do_bite (ch, victim->name);
		return;
	}
	else if (!IS_VAMPIRE (victim) && !IS_GHOUL (victim) && victim->class != CLASS_NONE)
	{
		send_to_char ("Blood does them no good at all.\n\r", ch);
		return;
	}
	if (ch == victim)
	{
		send_to_char ("There is no point in feeding yourself blood!\n\r", ch);
		return;
	}
	if ((!IS_SET (victim->extra2, EXTRA2_TORPORED)) && (!IS_IMMUNE (victim, IMM_VAMPIRE)))
	{
		send_to_char ("They refuse to drink your blood.\n\r", ch);
		return;
	}
	if (ch->blood[BLOOD_CURRENT] < (1 * ch->blood[BLOOD_POTENCY]))
	{
		send_to_char ("You don't have enough blood.\n\r", ch);
		return;
	}

	act ("You feed $N some blood from your open wrist.", ch, NULL, victim, TO_CHAR);
	act ("$n feeds $N some blood from $s open wrist.", ch, NULL, victim, TO_NOTVICT);

	if (victim->position < POS_RESTING)
		send_to_char ("You feel some blood poured down your throat.\n\r", victim);
	else
		act ("$n feeds you some blood from $s open wrist.", ch, NULL, victim, TO_VICT);

	blood = number_range (1, 3) * ch->blood[BLOOD_POTENCY];
	ch->blood[BLOOD_CURRENT] -= blood;
	victim->blood[BLOOD_CURRENT] += blood;
	if (victim->blood[BLOOD_CURRENT] >= victim->blood[BLOOD_POOL])
	{
		victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
		if (IS_VAMPIRE (victim))
			send_to_char ("Your blood lust is sated.\n\r", victim);
	}
	if (IS_HERO (victim) && (victim->class == CLASS_NONE || IS_GHOUL (victim)))
	{
		power_ghoul (ch, victim);
		if (!IS_NPC (victim) && victim->pcdata->dpoints < (ch->blood[2] * 1000))
		{
			if (!IS_VAMPIRE (ch))
				victim->pcdata->dpoints += (blood * 6);
			if (victim->pcdata->dpoints > (ch->blood[2] * 10))
				victim->pcdata->dpoints = (ch->blood[2] * 10);
		}
	}
	return;
}


void do_bite (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	bool can_sire = FALSE;
	int blood, sn, improve;
	int ch_gen = ch->vampgen;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch) || (ch->vampgen < 1))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (is_affected (ch, gsn_potency))
		ch_gen++;

	if (ch->vampgen == 2 && strlen (ch->clan) < 2)
	{
		send_to_char ("First you need to get a clan name.\n\r", ch);
		return;
	}


	if (ch->pcdata->cland[0] == -1 || ch->pcdata->cland[1] == -1 || ch->pcdata->cland[2] == -1)
	{
		send_to_char ("First you need to buy your disciplines.\n\r", ch);
		return;
	}

	if (ch_gen > 15)
		can_sire = FALSE;
	else
		can_sire = TRUE;

	if (!can_sire)
	{
		send_to_char ("You are not able to create any childer.\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
		return;

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (!IS_IMMUNE (victim, IMM_VAMPIRE))
	{
		send_to_char ("They refuse to drink your blood.\n\r", ch);
		return;
	}

	if (ch == victim)
	{
		send_to_char ("You are already a vampire!\n\r", ch);
		return;
	}

	if (victim->level != LEVEL_AVATAR && !IS_IMMORTAL (victim))
	{
		send_to_char ("You can only make avatars into vampires.\n\r", ch);
		return;
	}

	if (IS_WEREWOLF (victim))
	{
		send_to_char ("You are unable to create werevamps!\n\r", ch);
		return;
	}

	if (IS_VAMPIRE (victim))
	{
		send_to_char ("But they are already a vampire!\n\r", ch);
		return;
	}



	if (ch->blood[BLOOD_CURRENT] < 3)
	{
		send_to_char ("You don't have enough blood.\n\r", ch);
		return;
	}

	blood = number_range (1, 2) * ch->blood[BLOOD_POTENCY];
	ch->blood[BLOOD_CURRENT] -= blood;
	victim->blood[BLOOD_CURRENT] += blood;

	if (ch->beast > 0 && ch->beast < 100)
		ch->beast += 1;
	act ("You feed $N some blood from your open wrist.", ch, NULL, victim, TO_CHAR);
	act ("$n feeds $N some blood from $s open wrist.", ch, NULL, victim, TO_NOTVICT);
	act ("$n feeds you some blood from $s open wrist.", ch, NULL, victim, TO_VICT);

	/* For Abominations */
	if (IS_WEREWOLF (ch) && get_tribe (ch, TRIBE_GET_OF_FENRIS) > 0)
	{
		act ("You scream in agony as the blood of Caine burns through your body!", victim, NULL, NULL, TO_CHAR);
		act ("$n screams in agony and falls to the ground...DEAD!", victim, NULL, NULL, TO_ROOM);
		victim->form = 1;
		return;
	}

	victim->class = CLASS_VAMPIRE;
	if (victim->vampgen != 0)
	{
		do_autosave (ch, "");
		do_autosave (victim, "");
		send_to_char ("Your vampiric powers have been restored.\n\r", victim);
		return;
	}

	send_to_char ("You feel a strange and primal Thirst. You are now a vampire.\n\r", victim);
	REMOVE_BIT (victim->extra, EXTRA_PREGNANT);
	REMOVE_BIT (victim->extra, EXTRA_LABOUR);
	victim->vampgen = ch_gen + 1;
	update_gen (victim);
	free_string (victim->lord);
	if (ch->vampgen == 1)
		victim->lord = str_dup (ch->name);
	else
	{
		sprintf (buf, "%s %s", ch->lord, ch->name);
		victim->lord = str_dup (buf);
	}

	if (strlen (ch->clan) > 1 && (IS_EXTRA (ch, EXTRA_PRINCE) || IS_EXTRA (ch, EXTRA_SIRE)))
	{
		free_string (victim->clan);
		victim->clan = str_dup (ch->clan);
		if (strlen (ch->side) > 1)
		{
			free_string (victim->side);
			victim->side = str_dup (ch->side);
		}
	}
	REMOVE_BIT (ch->extra, EXTRA_SIRE);
	if (IS_MORE (ch, MORE_ANTITRIBU))
		SET_BIT (victim->more, MORE_ANTITRIBU);

	if (ch->vampgen == 1)
	{
		/* Second generation vampires chose all their disciplines */
		victim->pcdata->cland[0] = -1;
		victim->pcdata->cland[1] = -1;
		victim->pcdata->cland[2] = -1;
	}
	else
	{
		
		improve = victim->pcdata->cland[0] = ch->pcdata->cland[0];

		
		improve = victim->pcdata->cland[1] = ch->pcdata->cland[1];

		
		improve = victim->pcdata->cland[2] = ch->pcdata->cland[2];
	}

	for (sn = 0; sn <= DISC_MAX; sn++)
	{
		victim->pcdata->powers[sn] = 0;
	}

	victim->pcdata->powers[victim->pcdata->cland[0]] = 1;
	victim->pcdata->powers[victim->pcdata->cland[1]] = 1;
	victim->pcdata->powers[victim->pcdata->cland[2]] = 1;



	do_autosave (victim, "");
	return;
}


void do_vampire (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_IMMUNE (ch, IMM_VAMPIRE))
	{
		send_to_char ("You will now allow vampires to bite you.\n\r", ch);
		SET_BIT (ch->immune, IMM_VAMPIRE);
		return;
	}
	send_to_char ("You will no longer allow vampires to bite you.\n\r", ch);
	REMOVE_BIT (ch->immune, IMM_VAMPIRE);
	return;
}


void do_shift( CHAR_DATA *ch, char *argument )
{
	 char arg [MAX_INPUT_LENGTH];
	 char buf [MAX_STRING_LENGTH];
	 int toform = 0;

	 argument = one_argument( argument, arg );

	 if (IS_NPC(ch)) return;
	 if (!IS_WEREWOLF(ch))
	 {
	send_to_char("Huh?\n\r",ch);
	return;
	 }

	 if ( arg[0] == '\0' )
	 {
	send_to_char( "You can shift between the following forms: Homid, Glabro, Crinos, Hispo, Lupus.\n\r", ch );return;}

	 if      ( !str_cmp(arg,"homid" ) ) toform = 1;
	 else if ( !str_cmp(arg,"glabro") ) toform = 2;
	 else if ( !str_cmp(arg,"crinos" ) ) toform = 3;
	 else if ( !str_cmp(arg,"hispo" ) ) toform = 4;
	 else if ( !str_cmp(arg,"lupus" ) ) toform = 5;

	 else
{send_to_char( "You can shift between the following forms: Homid, Glabro, Crinos, Hispo, Lupus.\n\r", ch );return;}

	 if ( ch->pcdata->wolfform[1] < 1 || ch->pcdata->wolfform[1] > 5)
	ch->pcdata->wolfform[1] = 1;
	 if ( ch->pcdata->wolfform[1] == toform )
	 {
	send_to_char( "You are already in that form.\n\r", ch );
	return;
	 }

	 if (IS_VAMPAFF(ch,VAM_DISGUISED) )
	 {
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_VICISSITUDE);
	REMOVE_BIT(ch->polyaff, POLY_SPIRIT);
	free_string( ch->morph );
	ch->morph = str_dup( "" );
	free_string( ch->long_descr );
	ch->morph = str_dup( "" );

	 }
		if (IS_VAMPAFF(ch,VAM_CLAWS)) REMOVE_BIT(ch->vampaff, VAM_CLAWS);
		if (IS_VAMPAFF(ch,VAM_FANGS)) REMOVE_BIT(ch->vampaff, VAM_FANGS);
		if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
		
		free_string( ch->morph );
		ch->morph = str_dup( "" );
	 // Removal of Stat Bonus
	ch->mod_attributes[ATTRIB_STR] = 0;
	ch->mod_attributes[ATTRIB_DEX] = 0;
	ch->mod_attributes[ATTRIB_STA] = 0;
	ch->mod_attributes[ATTRIB_APP] = 0;
	ch->mod_attributes[ATTRIB_MAN] = 0;

	 ch->pcdata->wolfform[1] = toform;

		if (toform == 1)     // homid
		{
		if (IS_VAMPAFF(ch,VAM_CLAWS)) REMOVE_BIT(ch->vampaff, VAM_CLAWS);
		if (IS_VAMPAFF(ch,VAM_FANGS)) REMOVE_BIT(ch->vampaff, VAM_FANGS);
		if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) REMOVE_BIT(ch->vampaff, VAM_NIGHTSIGHT);
		send_to_char( "You transform into homid form.\n\r", ch );
		sprintf( buf, "%s transforms into human form.\n\r", ch->name);
		act( buf, ch, NULL, NULL, TO_ROOM );
		if(IS_SET(ch->polyaff, POLY_WOLF))
		REMOVE_BIT(ch->polyaff, POLY_WOLF);
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		clear_stats(ch);
		free_string( ch->morph );
		ch->morph = str_dup( "" );
		ch->pcdata->wolfform[1] = FORM_HOMID; // double check
		if (ch->pcdata->wolf >= 100) ch->pcdata->wolf = 0;
		return;
	 }
	 else	if (toform == 2)     // glabro
		{
	if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	if (!IS_VAMPAFF(ch,VAM_CLAWS)) SET_BIT(ch->vampaff, VAM_CLAWS);
	send_to_char( "You transform into Glabro form.\n\r", ch );
	sprintf( buf, "%s shifts into a large husky human.\n\r", ch->name);
	act( buf, ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	free_string( ch->morph );
	if (ch->pcdata->glabrodescshort[0] == '\0' )
		ch->morph = str_dup( "a large husky human" );
	 else
		ch->morph = str_dup( ch->pcdata->glabrodescshort);
	ch->pcdata->wolfform[1] = FORM_GLABRO; //double check
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 2;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 2;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] - 1;
	ch->mod_attributes[ATTRIB_APP] = ch->mod_attributes[ATTRIB_APP] - 1;

	return;
	 }
	 else	if (toform == 3)     // crinos
		{
	if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
	if (!IS_VAMPAFF(ch,VAM_CLAWS)) SET_BIT(ch->vampaff, VAM_CLAWS);
	send_to_char("A pair of long fangs extend from your mouth.\n\r",ch);
	act("A pair of long fangs extend from $n's mouth.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_FANGS);
		 send_to_char("Razor sharp talons extend from your fingers.\n\r",ch);
		 act("Razor sharp talons extend from $n's fingers.",ch,NULL,NULL,TO_ROOM);
	SET_BIT(ch->vampaff, VAM_CLAWS);

	send_to_char( "You transform into Crinos form.\n\r", ch );
	sprintf( buf, "%s shifts into a large werewolf!\n\r", ch->name);
	act( buf, ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	free_string( ch->morph );
	if (ch->pcdata->crinosdescshort[0] != '\0' )
		ch->morph = str_dup( ch->pcdata->crinosdescshort);
	else
	 switch (ch->pcdata->cland[2])
	{
		default:  ch->morph = str_dup("a large werewolf");  break; //default
		case 0:  ch->morph = str_dup("a large black and white furred werewolf"); break;	 //furies
		case 1:  ch->morph = str_dup("a large and smelly mottled furred werewolf"); break;	 //gnawers
		case 2:  ch->morph = str_dup("a mystical werewolf of legend"); break;	 //gaia
		case 3:  ch->morph = str_dup("a large auburn colored werewolf");break;	 //fianna
		case 4:  ch->morph = str_dup("a very muscular grey furred werewolf");break; //fenris
		case 5:  ch->morph = str_dup("a intellegent looking werewolf");break;	 //walkers
		case 6:  ch->morph = str_dup("a huge reddish-brown furred werewolf");break;	 // talons
		case 7: ch->morph = str_dup("a sinister looking pitch black werewolf");break; 	 //shadow lords
		case 8:  ch->morph = str_dup("a sleek bidpedal jackal-like wolf"); break;	 //striders
		case 9:  ch->morph = str_dup("a noble-looking silver werewolf"); break;	 //silverf
		case 10:  ch->morph = str_dup("a large slate grey furred werewolf"); break;	 //gazers
		case 11:  ch->morph = str_dup("a huge broad shouldered werewolf"); break;	 //uktena
		case 12:  ch->morph = str_dup("a large reddish black furred werewolf"); break;	 //wendigo
	}
	ch->pcdata->wolfform[1] = FORM_CRINOS; //double check
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 4;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 1;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 3;
	ch->mod_attributes[ATTRIB_APP] = ch->mod_attributes[ATTRIB_APP] - 5;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] - 5;
	return;
	 }
	 else	if (toform == 4)     // hispo
		{
	if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
		if (!IS_VAMPAFF(ch,VAM_FANGS)) SET_BIT(ch->vampaff, VAM_FANGS);
	if (!IS_VAMPAFF(ch,VAM_CLAWS)) SET_BIT(ch->vampaff, VAM_CLAWS);

	send_to_char( "You transform into hispo form.\n\r", ch );
	sprintf( buf, "%s shifts into a large, shaggy wolf.\n\r", ch->name);
	act( buf, ch, NULL, NULL, TO_ROOM );
	clear_stats(ch);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	free_string( ch->morph );
	if (ch->pcdata->hispodescshort[0] != '\0' )
		ch->morph = str_dup( ch->pcdata->hispodescshort);
	else
		 switch (ch->pcdata->cland[2])
		{
		default:  ch->morph = str_dup("a dire wolf");  break; //default
		case 0:  ch->morph = str_dup("a large black and white furred dire wolf"); break;	 //furies
		case 1:  ch->morph = str_dup("a smelly mottled furred wolf"); break;	 //gnawers
		case 2:  ch->morph = str_dup("A non-aggressive looking dire wolf"); break;	 //gaia
		case 3:  ch->morph = str_dup("a large dire wolf with auburn fur");break;	 //fianna
		case 4:  ch->morph = str_dup("a hulking grey furred dire wolf");break; //fenris
		case 5:  ch->morph = str_dup("a domesticated looking dire wolf");break;	 //walkers
		case 6:  ch->morph = str_dup("a large reddish-brown dire wolf");break;	 // talons
		case 7: ch->morph = str_dup("a large pitch black dire wolf");break; 	 //shadow lords
		case 8:  ch->morph = str_dup("a large jackal-like wolf"); break;	 //striders
		case 9:  ch->morph = str_dup("a large silver furred dire wolf"); break;	 //silverf
		case 10:  ch->morph = str_dup("a slate grey furred dire wolf"); break;	 //gazers
		case 11:  ch->morph = str_dup("a giant timber wolf"); break;	 //uktena
		case 12:  ch->morph = str_dup("a large reddish black furred wolf"); break;	 //wendigo
		}
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 3;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 2;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 3;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] - 5;

	ch->pcdata->wolfform[1] = FORM_HISPO; //double check
	return;
	 }
	 else	if (toform == 5)     // lupus
	 {
		if (!IS_VAMPAFF(ch,VAM_NIGHTSIGHT)) SET_BIT(ch->vampaff, VAM_NIGHTSIGHT);
		if (!IS_VAMPAFF(ch,VAM_FANGS)) SET_BIT(ch->vampaff, VAM_FANGS);
		if (!IS_VAMPAFF(ch,VAM_CLAWS)) SET_BIT(ch->vampaff, VAM_CLAWS);

		send_to_char( "You transform into lupus form.\n\r", ch );
		sprintf( buf, "%s transforms into a wolf.\n\r", ch->name);
		act( buf, ch, NULL, NULL, TO_ROOM );
		clear_stats(ch);
		SET_BIT(ch->polyaff, POLY_WOLF);
		SET_BIT(ch->affected_by, AFF_POLYMORPH);
		free_string( ch->morph );
	if (ch->pcdata->lupusdescshort[0] != '\0' )
		ch->morph = str_dup( ch->pcdata->lupusdescshort);
	else
	 switch (ch->pcdata->cland[2])
	{
		default:  ch->morph = str_dup("a wolf");  break; //default
		case 0:  ch->morph = str_dup("a medium sized black and white furred wolf"); break;	 //furies
		case 1:  ch->morph = str_dup("a small, stinky mutt of a wolf"); break;	 //gnawers
		case 2:  ch->morph = str_dup("a placid looking wolf"); break;	 //gaia
		case 3:  ch->morph = str_dup("a medium sized auburn colored wolf");break;	 //fianna
		case 4:  ch->morph = str_dup("a large grey furred wolf");break; //fenris
		case 5:  ch->morph = str_dup("a intellegent looking wolf");break;	 //walkers
		case 6:  ch->morph = str_dup("a ferocious looking reddish-brown wolf");break;	 // talons
		case 7: ch->morph = str_dup("a average sized pitch-black wolf");break; 	 //shadow lords
		case 8:  ch->morph = str_dup("a sleek jackal-like wolf"); break;	 //striders
		case 9:  ch->morph = str_dup("a silver furred wolf"); break;	 //silverf
		case 10:  ch->morph = str_dup("a slate grey furred wolf"); break;	 //gazers
		case 11:  ch->morph = str_dup("A large timber wolf"); break;	 //uktena
		case 12:  ch->morph = str_dup("a reddish black furred wolf"); break;	 //wendigo
	}
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 1;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 2;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 2;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] - 5;
		ch->pcdata->wolfform[1] = FORM_LUPUS;
		return;
	 }
	else
	{send_to_char( "Error, in do_shift, please inform immortals.\n\r", ch );return;}
return;
}


void do_clanname (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;

	if (arg[0] == '\0')
	{
		send_to_char ("Whose clan do you wish to change or name?\n\r", ch);
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
		send_to_char ("Clan name should be between 0 and 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove clan.\n\r", ch);
		return;
	}
	free_string (victim->clan);
	victim->clan = str_dup (argument);
	send_to_char ("Clan set.\n\r", ch);
	send_to_char ("Clan set.\n\r", victim);
	return;
}

void do_clandisc (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int improve, sn, col;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char ("                               -= Disciplines =-\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		if (ch->vampgen <= 2)
		{
			/* Clan discipline */
			improve = ch->pcdata->cland[0];
			sprintf (buf, "Clan Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Personal discipline 1 */
			improve = ch->pcdata->cland[1];
			sprintf (buf, "Personal Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Personal discipline 2 */
			improve = ch->pcdata->cland[2];
			sprintf (buf, "Personal Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);
		}
		else
		{
			/* Clan discipline */
			improve = ch->pcdata->cland[0];
			sprintf (buf, "Clan Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Sires discipline */
			improve = ch->pcdata->cland[1];
			sprintf (buf, "Sires Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Personal discipline */
			improve = ch->pcdata->cland[2];
			sprintf (buf, "Personal Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);
		}
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		if (ch->pcdata->cland[2] >= 0)
			return;
		send_to_char ("Please select another discipline from the list below.\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		col = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] != 0)
				continue;
			sprintf (buf, "%-15s           ", disc_name (sn));
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "animalism"))
		improve = DISC_ANIMALISM;
	else if (!str_cmp (arg, "auspex"))
		improve = DISC_AUSPEX;
	else if (!str_cmp (arg, "celerity"))
		improve = DISC_CELERITY;
	else if (!str_cmp (arg, "chimerstry"))
		improve = DISC_CHIMERSTRY;
	else if (!str_cmp (arg, "daimoinon"))
		improve = DISC_DAIMOINON;
	else if (!str_cmp (arg, "dominate"))
		improve = DISC_DOMINATE;
	else if (!str_cmp (arg, "fortitude"))
		improve = DISC_FORTITUDE;
	else if (!str_cmp (arg, "melpominee"))
		improve = DISC_MELPOMINEE;
	else if (!str_cmp (arg, "mytherceria"))
		improve = DISC_MYTHERCERIA;
	else if (!str_cmp (arg, "necromancy"))
		improve = DISC_NECROMANCY;
	else if (!str_cmp (arg, "obeah"))
		improve = DISC_OBEAH;
	else if (!str_cmp (arg, "obfuscate"))
		improve = DISC_OBFUSCATE;
	else if (!str_cmp (arg, "obtenebration"))
		improve = DISC_OBTENEBRATION;
	else if (!str_cmp (arg, "potence"))
		improve = DISC_POTENCE;
	else if (!str_cmp (arg, "presence"))
		improve = DISC_PRESENCE;
	else if (!str_cmp (arg, "protean"))
		improve = DISC_PROTEAN;
	else if (!str_cmp (arg, "quietus"))
		improve = DISC_QUIETUS;
	else if (!str_cmp (arg, "serpentis"))
		improve = DISC_SERPENTIS;
	else if (!str_cmp (arg, "thanatosis"))
		improve = DISC_THANATOSIS;
	else if (!str_cmp (arg, "thaumaturgy"))
		improve = DISC_THAUMATURGY;
	else if (!str_cmp (arg, "vicissitude"))
		improve = DISC_VICISSITUDE;
	else if (!str_cmp (arg, "mortis"))
		improve = DISC_MORTIS;
	else if (!str_cmp (arg, "temporis"))
		improve = DISC_TEMPORIS;
	else if (!str_cmp (arg, "Valeren"))
		improve = DISC_VALEREN;
	else if (!str_cmp (arg, "abombwe"))
		improve = DISC_ABOMBWE;
	else if (!str_cmp (arg, "deimos"))
		improve = DISC_DEIMOS;
	else if (!str_cmp (arg, "dementation"))
		improve = DISC_DEMENTATION;
	else if (!str_cmp (arg, "grotesquous"))
		improve = DISC_GROTESQUOUS;
	else if (!str_cmp (arg, "nihilistics"))
		improve = DISC_NIHILISTICS;
	else if (!str_cmp (arg, "ma"))
		improve = DISC_MA;
	else if (!str_cmp (arg, "bardo"))
		improve = DISC_BARDO;
	else if (!str_cmp (arg, "maleficia"))
		improve = DISC_MALEFICIA;
	else if (!str_cmp (arg, "mythercia"))
		improve = DISC_MYTHERCIA;
	else if (!str_cmp (arg, "spiritus"))
		improve = DISC_SPIRITUS;
	else if (!str_cmp (arg, "stiga"))
		improve = DISC_STIGA;
	else if (!str_cmp (arg, "visceratika"))
		improve = DISC_VISCERATIKA;
	else
	{
		send_to_char ("You know of no such discipline.\n\r", ch);
		return;
	}

	if (ch->pcdata->cland[0] == improve || ch->pcdata->cland[1] == improve || ch->pcdata->cland[2] == improve)
	{
		send_to_char ("You already know that discipline.\n\r", ch);
		return;
	}

	if (ch->vampgen < 3 || ch->class != CLASS_VAMPIRE)
	{
		if (ch->pcdata->cland[0] < 0)
			ch->pcdata->cland[0] = improve;
		else if (ch->pcdata->cland[1] < 0)
			ch->pcdata->cland[1] = improve;
		else if (ch->pcdata->cland[2] < 0)
			ch->pcdata->cland[2] = improve;
		else
		{
			send_to_char ("All of your discipline slots are full.\n\r", ch);
			return;
		}
		ch->pcdata->powers[improve] = -1;
		send_to_char ("Ok.\n\r", ch);
	}
	else
	{
		if (ch->pcdata->cland[2] < 0)
			ch->pcdata->cland[2] = improve;
		else
		{
			send_to_char ("Your discipline slot is full.\n\r", ch);
			return;
		}
		ch->pcdata->powers[improve] = -1;
		send_to_char ("Ok.\n\r", ch);
	}

	return;
}


void do_shadowplane (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_WEREWOLF (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if ((IS_VAMPIRE (ch) || IS_GHOUL (ch)) && get_path2 (ch, PATH_MSPIRIT) < 5)
	{
		send_to_char ("You require Thaumaturgy Spirit Manipulation level 5 to enter the Umbra", ch);
		return;
	}


	if (arg[0] == '\0')
	{
		if (!IS_AFFECTED (ch, AFF_SHADOWPLANE))
		{
			send_to_char ("You fade into the Umbra.\n\r", ch);
			act ("$n slowly fades out of existance.", ch, NULL, NULL, TO_ROOM);
			SET_BIT (ch->affected_by, AFF_SHADOWPLANE);
			do_look (ch, "auto");
			return;
		}
		REMOVE_BIT (ch->affected_by, AFF_SHADOWPLANE);
		send_to_char ("You fade back into the real world.\n\r", ch);
		act ("$n slowly fades into existance.", ch, NULL, NULL, TO_ROOM);
		do_look (ch, "auto");
		return;
	}

	if ((obj = get_obj_carry (ch, arg)) == NULL)
	{
		send_to_char ("You are not carrying that object.\n\r", ch);
		return;
	}

	if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
	{
		act ("You toss $p to the ground and it vanishes into the real world.", ch, obj, NULL, TO_CHAR);
		act ("$n tosses $p to the ground and it vanishes.", ch, obj, NULL, TO_ROOM);
		obj_from_char (obj);
		obj_to_room (obj, ch->in_room);
	}
	else
	{
		act ("You toss $p to the ground and it vanishes into the Umbra.", ch, obj, NULL, TO_CHAR);
		act ("$n tosses $p to the ground and it vanishes.", ch, obj, NULL, TO_ROOM);
		obj_from_char (obj);
		obj_to_room (obj, ch->in_room);
		SET_BIT (obj->extra_flags, ITEM_SHADOWPLANE);
	}
	return;
}


void do_mortalvamp (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	int disc;
	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (IS_VAMPIRE (ch) || IS_GHOUL (ch))
	{
		for (disc = DISC_ANIMALISM; disc <= DISC_MAX; disc++)
			ch->pcdata->powers_mod[disc] += 10;
		if (IS_VAMPAFF (ch, VAM_DISGUISED))
		{
			REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
			REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
			REMOVE_BIT (ch->extra, EXTRA_VICISSITUDE);
			REMOVE_BIT (ch->polyaff, POLY_SPIRIT);
			free_string (ch->morph);
			ch->morph = str_dup ("");
			free_string (ch->long_descr);
			ch->morph = str_dup ("");
		}
		if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
			do_shadowplane (ch, "");
		if (IS_VAMPAFF (ch, VAM_FANGS))
			do_fangs (ch, "");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
			do_truesight (ch, "");
		if (IS_SET (ch->act, PLR_HOLYLIGHT))
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_OBEAH))
		{
			ch->pcdata->obeah = 0;
		}
		ch->pcdata->wolf = 0;
		for (disc = DISC_ANIMALISM; disc <= DISC_MAX; disc++)
			ch->pcdata->powers_mod[disc] -= 10;
	}
	return;
}

void do_werewolf (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (!IS_WEREWOLF (ch))
		return;
	if (IS_SET (ch->act, PLR_WOLFMAN))
		return;
	if (ch->pcdata->wolfform[1] != FORM_HOMID)
		return;

	if (IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
		REMOVE_BIT (ch->extra, EXTRA_VICISSITUDE);
		REMOVE_BIT (ch->polyaff, POLY_SPIRIT);
		free_string (ch->morph);
		ch->morph = str_dup ("");
		free_string (ch->long_descr);
		ch->morph = str_dup ("");
	}
	SET_BIT (ch->act, PLR_WOLFMAN);

	send_to_char ("You throw back your head and howl with rage!\n\r", ch);
	act ("$n throws back $s head and howls with rage!.", ch, NULL, NULL, TO_ROOM);
	send_to_char ("Coarse dark hair sprouts from your body.\n\r", ch);
	act ("Coarse dark hair sprouts from $n's body.", ch, NULL, NULL, TO_ROOM);

	// Stat Bonus
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 4;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 1;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 3;
	ch->mod_attributes[ATTRIB_APP] = ch->mod_attributes[ATTRIB_APP] - 11;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] - 3;


	if (!IS_VAMPAFF (ch, VAM_FANGS))
	{
		send_to_char ("A pair of long fangs extend from your mouth.\n\r", ch);
		act ("A pair of long fangs extend from $n's mouth.", ch, NULL, NULL, TO_ROOM);
		SET_BIT (ch->vampaff, VAM_FANGS);
	}
	if (!IS_VAMPAFF (ch, VAM_CLAWS))
	{
		if (get_auspice (ch, AUSPICE_AHROUN) > 3)
		{
			send_to_char ("Gleaming silver talons extend from your fingers.\n\r", ch);
			act ("Gleaming silver talons extend from $n's fingers.", ch, NULL, NULL, TO_ROOM);
		}
		else
		{
			send_to_char ("Razor sharp talons extend from your fingers.\n\r", ch);
			act ("Razor sharp talons extend from $n's fingers.", ch, NULL, NULL, TO_ROOM);
		}
		SET_BIT (ch->vampaff, VAM_CLAWS);
	}

	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->vampaff, VAM_DISGUISED);
	ch->pcdata->wolfform[1] = FORM_CRINOS;
	
	if (IS_POLYAFF (ch, POLY_ZULO))
		sprintf (buf, "An enormous black hairy monster");
	else
		sprintf (buf, "%s the huge werewolf", ch->name);
		
	free_string (ch->morph);
	ch->morph = str_dup (buf);
	ch->pcdata->wolf += 25;
	if (get_tribe (ch, TRIBE_GET_OF_FENRIS) > 4)
		ch->pcdata->wolf = 300;
	else if (ch->pcdata->wolf > 300)
		ch->pcdata->wolf = 300;

	for (vch = char_list; vch != NULL; vch = vch_next)
	{
		vch_next = vch->next;
		if (vch->in_room == NULL)
			continue;
		if (ch == vch)
		{
			act ("You throw back your head and howl with rage!", ch, NULL, NULL, TO_CHAR);
			continue;
		}
		if (!IS_NPC (vch) && vch->pcdata->chobj != NULL)
			continue;
		if (!IS_NPC (vch))
		{
			if (vch->in_room == ch->in_room)
				act ("$n throws back $s head and howls with rage!", ch, NULL, vch, TO_VICT);
			else if (vch->in_room->area == ch->in_room->area)
				send_to_char ("You hear a fearsome howl close by!\n\r", vch);
			else
				send_to_char ("You hear a fearsome howl far off in the distance!\n\r", vch);
			if (!CAN_PK (vch))
				continue;
		}
	}
	return;
}

void do_unwerewolf (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (!IS_WEREWOLF (ch))
		return;
	if (!IS_SET (ch->act, PLR_WOLFMAN))
		return;
	REMOVE_BIT (ch->act, PLR_WOLFMAN);
	ch->pcdata->wolfform[1] = FORM_HOMID;
	free_string (ch->morph);

	if (IS_POLYAFF (ch, POLY_ZULO))
		ch->morph = str_dup ("a huge black monster");
	else
	{
		ch->morph = str_dup ("");
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
	}

	if (IS_VAMPAFF (ch, VAM_CLAWS) && !IS_POLYAFF (ch, POLY_ZULO))
	{
		if (get_auspice (ch, AUSPICE_AHROUN) > 3)
		{
			send_to_char ("Your gleaming silver talons slide back into your fingers.\n\r", ch);
			act ("$n's gleaming silver talons slide back into $s fingers.", ch, NULL, NULL, TO_ROOM);
		}
		else
		{
			send_to_char ("Your talons slide back into your fingers.\n\r", ch);
			act ("$n's talons slide back into $s fingers.", ch, NULL, NULL, TO_ROOM);
		}
		REMOVE_BIT (ch->vampaff, VAM_CLAWS);
	}
	if (IS_VAMPAFF (ch, VAM_FANGS) && !IS_POLYAFF (ch, POLY_ZULO))
	{
		send_to_char ("Your fangs slide back into your mouth.\n\r", ch);
		act ("$n's fangs slide back into $s mouth.", ch, NULL, NULL, TO_ROOM);
		REMOVE_BIT (ch->vampaff, VAM_FANGS);
	}

	send_to_char ("Your coarse hair shrinks back into your body.\n\r", ch);
	act ("$n's coarse hair shrinks back into $s body.", ch, NULL, NULL, TO_ROOM);
	ch->pcdata->wolf -= 25;

	// Removal of Stat Bonus
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] - 4;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] - 1;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] - 3;
	ch->mod_attributes[ATTRIB_APP] = ch->mod_attributes[ATTRIB_APP] + 11;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] + 3;



	if (ch->pcdata->wolf < 0)
		ch->pcdata->wolf = 0;
	return;
}

void do_favour (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	sh_int gifts = 0;
	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch) && !IS_WEREWOLF (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax is: favour <target> <favour>\n\r", ch);
		if (ch->vampgen == 3)
		{
			if (IS_WEREWOLF (ch))
				send_to_char ("Favours you can grant: Induct Outcast Sire Shaman.\n\r", ch);
			else
				send_to_char ("Favours you can grant: Induct Outcast Sire Prince.\n\r", ch);
		}
		else if (IS_EXTRA (ch, EXTRA_PRINCE))
			send_to_char ("Favours you can grant: Induct Outcast Sire.\n\r", ch);
		else
			send_to_char ("None.\n\r", ch);
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

	if (ch->class != victim->class)
	{
		send_to_char ("You cannot do that, fool!\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "sire"))
	{
		if (ch->vampgen != 4 && !IS_EXTRA (ch, EXTRA_PRINCE))
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
		if (IS_EXTRA (victim, EXTRA_SIRE))
		{
			if (IS_WEREWOLF (ch))
			{
				act ("You remove $N's permission to claw a pup!", ch, NULL, victim, TO_CHAR);
				act ("$n has removed $N's permission to claw a pup!", ch, NULL, victim, TO_NOTVICT);
				act ("$n has remove your permission to claw a pup!", ch, NULL, victim, TO_VICT);
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
		if (IS_WEREWOLF (ch))
		{
			act ("You grant $N permission to claw a pup!", ch, NULL, victim, TO_CHAR);
			act ("$n has granted $N permission to claw a pup!", ch, NULL, victim, TO_NOTVICT);
			act ("$n has granted you permission to claw a pup!", ch, NULL, victim, TO_VICT);
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
	else if (!str_cmp (arg2, "prince") && IS_VAMPIRE (ch))
	{
		if (ch->vampgen != 3)
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
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
	else if (!str_cmp (arg2, "shaman") && IS_WEREWOLF (ch))
	{
		if (ch->vampgen != 2)
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
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
	else if (!str_cmp (arg2, "induct"))
	{
		if (ch->vampgen > 4 && !IS_EXTRA (ch, EXTRA_PRINCE) && !IS_WEREWOLF (ch))
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
		if (!IS_IMMUNE (victim, IMM_VAMPIRE))
		{
			send_to_char ("You cannot induct an unwilling person.\n\r", ch);
			return;
		}
		if (ch->vampgen > victim->vampgen)
		{
			if (IS_WEREWOLF (ch))
				send_to_char ("You can only induct those of weaker heritage than yourself.\n\r", ch);
			else
				send_to_char ("You can only induct those of higher generation than yourself.\n\r", ch);
			return;
		}
		if (IS_WEREWOLF (ch))
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
		victim->pcdata->cland[2] = gifts = ch->pcdata->cland[2];	//gives you inductors clan gift
		victim->pcdata->tribes[gifts] = 1;
		return;
	}
	else if (!str_cmp (arg2, "outcast"))
	{
		if (ch->vampgen > 4 && !IS_EXTRA (ch, EXTRA_PRINCE))
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
		if (IS_EXTRA (victim, EXTRA_PRINCE) && ch->vampgen != 2)
		{
			if (IS_WEREWOLF (ch))
				send_to_char ("You cannot outcast another Shaman.\n\r", ch);
			else
				send_to_char ("You cannot outcast another Prince.\n\r", ch);
			return;
		}
		if (ch->vampgen >= victim->vampgen)
		{
			if (IS_WEREWOLF (ch))
				send_to_char ("You can only outcast those of weaker heritage than yourself.\n\r", ch);
			else
				send_to_char ("You can only outcast those of higher generation than yourself.\n\r", ch);
			return;
		}
		if (IS_WEREWOLF (ch))
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


bool char_exists (int file_dir, char *argument)
{
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	bool found = FALSE;

	fflush (fpReserve);
	fclose (fpReserve);
	switch (file_dir)
	{
	default:
		return FALSE;
		break;
	case FILE_PLAYER:
		sprintf (buf, "%s%s", PLAYER_DIR, capitalize (argument));
		break;
	case FILE_PLAYER_BAK:
		sprintf( buf, "%s%s", PLAYER_BAK_DIR, capitalize (argument));
		break;
	}
	if ((fp = fopen (buf, "r")) != NULL)
	{
		found = TRUE;
		fclose (fp);
		fflush (fp);
		fpReserve = fopen (NULL_FILE, "r");
	}
	return found;
}


void do_discipline (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char disc[25];
	int improve;
	int cost;
	int max_d;
	int sn, col;
	bool got_disc = FALSE;

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE(ch) && !IS_INQUISITOR(ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char ("                               -= Disciplines =-\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		col = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
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
			if ((ch->class == CLASS_VAMPIRE || ch->class == CLASS_INQUISITOR) && (ch->pcdata->cland[0] == sn || ch->pcdata->cland[1] == sn || ch->pcdata->cland[2] == sn))
				capitalize (disc);

			sprintf (buf, "%-15s: %-2d       ", disc, get_disc (ch, sn));
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
			got_disc = TRUE;
		}
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
		if (!got_disc)
			send_to_char ("You currently have no disciplines.\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char ("To improve a discipline rating, type: discipline <name>\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}

	if (!str_cmp (arg, "animalism"))
		improve = DISC_ANIMALISM;
	else if (!str_cmp (arg, "auspex"))
		improve = DISC_AUSPEX;
	else if (!str_cmp (arg, "celerity"))
		improve = DISC_CELERITY;
	else if (!str_cmp (arg, "chimerstry"))
		improve = DISC_CHIMERSTRY;
	else if (!str_cmp (arg, "daimoinon"))
		improve = DISC_DAIMOINON;
	else if (!str_cmp (arg, "dominate"))
		improve = DISC_DOMINATE;
	else if (!str_cmp (arg, "fortitude"))
		improve = DISC_FORTITUDE;
	else if (!str_cmp (arg, "melpominee"))
		improve = DISC_MELPOMINEE;
	else if (!str_cmp (arg, "mytherceria"))
		improve = DISC_MYTHERCERIA;
	else if (!str_cmp (arg, "necromancy"))
		improve = DISC_NECROMANCY;
	else if (!str_cmp (arg, "obeah"))
		improve = DISC_OBEAH;
	else if (!str_cmp (arg, "obfuscate"))
		improve = DISC_OBFUSCATE;
	else if (!str_cmp (arg, "obtenebration"))
		improve = DISC_OBTENEBRATION;
	else if (!str_cmp (arg, "potence"))
		improve = DISC_POTENCE;
	else if (!str_cmp (arg, "presence"))
		improve = DISC_PRESENCE;
	else if (!str_cmp (arg, "protean"))
		improve = DISC_PROTEAN;
	else if (!str_cmp (arg, "quietus"))
		improve = DISC_QUIETUS;
	else if (!str_cmp (arg, "serpentis"))
		improve = DISC_SERPENTIS;
	else if (!str_cmp (arg, "thanatosis"))
		improve = DISC_THANATOSIS;
	else if (!str_cmp (arg, "thaumaturgy"))
		improve = DISC_THAUMATURGY;
	else if (!str_cmp (arg, "vicissitude"))
		improve = DISC_VICISSITUDE;
	else if (!str_cmp (arg, "mortis"))
		improve = DISC_MORTIS;
	else if (!str_cmp (arg, "temporis"))
		improve = DISC_TEMPORIS;
	else if (!str_cmp (arg, "Valeren"))
		improve = DISC_VALEREN;
	else if (!str_cmp (arg, "abombwe"))
		improve = DISC_ABOMBWE;
	else if (!str_cmp (arg, "deimos"))
		improve = DISC_DEIMOS;
	else if (!str_cmp (arg, "dementation"))
		improve = DISC_DEMENTATION;
	else if (!str_cmp (arg, "grotesquous"))
		improve = DISC_GROTESQUOUS;
	else if (!str_cmp (arg, "nihilistics"))
		improve = DISC_NIHILISTICS;
	else if (!str_cmp (arg, "ma"))
		improve = DISC_MA;
	else if (!str_cmp (arg, "bardo"))
		improve = DISC_BARDO;
	else if (!str_cmp (arg, "maleficia"))
		improve = DISC_MALEFICIA;
	else if (!str_cmp (arg, "mythercia"))
		improve = DISC_MYTHERCIA;
	else if (!str_cmp (arg, "spiritus"))
		improve = DISC_SPIRITUS;
	else if (!str_cmp (arg, "stiga"))
		improve = DISC_STIGA;
	else if (!str_cmp (arg, "visceratika"))
		improve = DISC_VISCERATIKA;
	else if (!str_cmp (arg, "sophiapatris"))
		improve = DISC_SOPHIA_PATRIS;
	else if (!str_cmp (arg, "sedulitasfilius"))
		improve = DISC_SEDULITAS_FILIUS;
	else if (!str_cmp (arg, "bonispiritus"))
		improve = DISC_BONI_SPIRITUS;
	else
	{
		send_to_char ("You know of no such discipline.\n\r", ch);
		return;
	}

	if (ch->pcdata->powers[improve] == 0)
	{
		send_to_char ("You know of no such discipline.\n\r", ch);
		return;
	}

	switch (ch->vampgen)
	{
	case 1:
		max_d = 10;
		break;
	case 2:
		max_d = 10;
		break;
	case 3:
		max_d = 10;
		break;
	case 4:
		max_d = 9;
		break;
	case 5:
		max_d = 8;
		break;
	case 6:
		max_d = 7;
		break;
	case 7:
		max_d = 6;
		break;
	default:
		max_d = 5;
		break;
	}

	if (improve == DISC_THAUMATURGY)
		max_d = 5;	//thaum only has 5

	if (improve == DISC_MORTIS)
		max_d = 8;
	if (improve == DISC_DEIMOS)
		max_d = 6;
	if (improve == DISC_MA)
		max_d = 4;
	if (improve == DISC_ABOMBWE)
		max_d = 6;
	if (improve == DISC_BARDO)
		max_d = 9;

	if (improve == DISC_MALEFICIA)
		max_d = 5;
	if (improve == DISC_MELPOMINEE)
		max_d = 7;
	if (improve == DISC_MYTHERCERIA)
		max_d = 7;
	
	if (improve == DISC_NIHILISTICS)
		max_d = 7;
	if (improve == DISC_SPIRITUS)
		max_d = 7;
	if (improve == DISC_TEMPORIS)
		max_d = 9;


	if (ch->pcdata->powers[improve] >= max_d)
	{
		send_to_char ("You are unable to improve your ability in that discipline any further.\n\r", ch);
		return;
	}

	if (ch->pcdata->cland[0] == improve || ch->pcdata->cland[1] == improve || ch->pcdata->cland[2] == improve)
	{
		if (ch->pcdata->powers[improve] == -1)
			cost = 10;
		else
			cost = (ch->pcdata->powers[improve]) * 5;
	}
	else
	{
		if (ch->pcdata->powers[improve] == -1)
			cost = 10;
		else
			cost = (ch->pcdata->powers[improve]) * 7;
	}

	if (ch->exp < cost)
	{
		sprintf (buf, "You need another %d exp to increase that discipline.\n\r", cost - ch->exp);
		send_to_char (buf, ch);
		return;
	}

	if (ch->pcdata->powers[improve] < 1)
		ch->pcdata->powers[improve] = 1;
	else
		ch->pcdata->powers[improve]++;

	ch->exp -= cost;
	send_to_char ("You improve your discipline.\n\r", ch);

	return;
}



void do_teachdisc (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char disc[25];
	CHAR_DATA *victim;
	int improve, sn, col;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (IS_MORE (ch, MORE_NPC))
	{
		send_to_char ("NPC's cannot teach disciplines.\n\r", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax: teach <person> <discipline>\n\r", ch);
		send_to_char ("The following list contains your disciplines and the level you can teach to.\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);

		col = 0;
		for (sn = 0; sn <= DISC_MAX; sn++)
		{
			if (ch->pcdata->powers[sn] == 0)
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
			sprintf (buf, "%-15s: %-2d       ", disc, ch->pcdata->powers[sn]);
			send_to_char (buf, ch);
			if (++col % 3 == 0)
				send_to_char ("\n\r", ch);
		}
		if (col % 3 != 0)
			send_to_char ("\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg1)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}

	if (ch == victim)
	{
		send_to_char ("You cannot teach yourself!\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPC's.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "animalism"))
		improve = DISC_ANIMALISM;
	else if (!str_cmp (arg2, "auspex"))
		improve = DISC_AUSPEX;
	else if (!str_cmp (arg2, "celerity"))
		improve = DISC_CELERITY;
	else if (!str_cmp (arg2, "chimerstry"))
		improve = DISC_CHIMERSTRY;
	else if (!str_cmp (arg2, "daimoinon"))
		improve = DISC_DAIMOINON;
	else if (!str_cmp (arg2, "dominate"))
		improve = DISC_DOMINATE;
	else if (!str_cmp (arg2, "fortitude"))
		improve = DISC_FORTITUDE;
	else if (!str_cmp (arg2, "melpominee"))
		improve = DISC_MELPOMINEE;
	else if (!str_cmp (arg2, "mytherceria"))
		improve = DISC_MYTHERCERIA;
	
	else if (!str_cmp (arg2, "necromancy"))
		improve = DISC_NECROMANCY;
	else if (!str_cmp (arg2, "obeah"))
		improve = DISC_OBEAH;
	else if (!str_cmp (arg2, "obfuscate"))
		improve = DISC_OBFUSCATE;
	else if (!str_cmp (arg2, "obtenebration"))
		improve = DISC_OBTENEBRATION;
	else if (!str_cmp (arg2, "potence"))
		improve = DISC_POTENCE;
	else if (!str_cmp (arg2, "presence"))
		improve = DISC_PRESENCE;
	else if (!str_cmp (arg2, "protean"))
		improve = DISC_PROTEAN;
	else if (!str_cmp (arg2, "quietus"))
		improve = DISC_QUIETUS;
	else if (!str_cmp (arg2, "serpentis"))
		improve = DISC_SERPENTIS;
	else if (!str_cmp (arg2, "thanatosis"))
		improve = DISC_THANATOSIS;
	else if (!str_cmp (arg2, "thaumaturgy"))
		improve = DISC_THAUMATURGY;
	else if (!str_cmp (arg2, "vicissitude"))
		improve = DISC_VICISSITUDE;
	else if (!str_cmp (arg2, "temporis"))
		improve = DISC_TEMPORIS;
	else if (!str_cmp (arg2, "mortis"))
		improve = DISC_MORTIS;
	else if (!str_cmp (arg2, "Valeren"))
		improve = DISC_VALEREN;
	else if (!str_cmp (arg2, "abombwe"))
		improve = DISC_ABOMBWE;
	else if (!str_cmp (arg2, "deimos"))
		improve = DISC_DEIMOS;
	else if (!str_cmp (arg2, "dementation"))
		improve = DISC_DEMENTATION;
	else if (!str_cmp (arg2, "grotesquous"))
		improve = DISC_GROTESQUOUS;
	else if (!str_cmp (arg2, "nihilistics"))
		improve = DISC_NIHILISTICS;
	else if (!str_cmp (arg2, "Ma"))
		improve = DISC_MA;
	else if (!str_cmp (arg2, "bardo"))
		improve = DISC_BARDO;
	else if (!str_cmp (arg2, "maleficia"))
		improve = DISC_MALEFICIA;
	else if (!str_cmp (arg2, "mythercia"))
		improve = DISC_MYTHERCIA;
	else if (!str_cmp (arg2, "spiritus"))
		improve = DISC_SPIRITUS;
	else if (!str_cmp (arg2, "stiga"))
		improve = DISC_STIGA;
	else if (!str_cmp (arg2, "visceratika"))
		improve = DISC_VISCERATIKA;
	else if (!str_cmp (arg2, "sophiapatris"))
		improve = DISC_SOPHIA_PATRIS;
	else if (!str_cmp (arg2, "sedulitasfilius"))
		improve = DISC_SEDULITAS_FILIUS;
	else if (!str_cmp (arg2, "bonispiritus"))
		improve = DISC_BONI_SPIRITUS;
	else
	{
		send_to_char ("You know of no such discipline.\n\r", ch);
		return;
	}

	if (ch->pcdata->powers[improve] == 0)
	{
		send_to_char ("You know of no such discipline.\n\r", ch);
		return;
	}

	if (ch->pcdata->powers[improve] < 3)
	{
		send_to_char ("You require level 3 in a discipline before you can teach it.\n\r", ch);
		return;
	}

	if (!IS_VAMPIRE (victim))
	{
		send_to_char ("They are unable to learn disciplines.\n\r", ch);
		return;
	}

	if (!IS_IMMUNE (victim, IMM_VAMPIRE))
	{
		send_to_char ("They refuse to learn any disciplines.\n\r", ch);
		return;
	}

	if (victim->pcdata->powers[improve] != 0)
	{
		send_to_char ("They already know that discipline.\n\r", ch);
		return;
	}

	victim->pcdata->powers[improve] = -1;
	sprintf (buf, "You teach $N how to use the %s discipline.", capitalize (arg2));
	act (buf, ch, NULL, victim, TO_CHAR);
	sprintf (buf, "$n teaches you how to use the %s discipline.", capitalize (arg2));
	act (buf, ch, NULL, victim, TO_VICT);
	return;
}





void do_embrace (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	int dam = number_range (1, 3);

	one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_VAMPIRE (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if ((victim = ch->embrace) != NULL && victim->embrace != NULL && victim->embrace == ch)
	{
		if (ch->embraced == ARE_EMBRACED)
		{
			send_to_char ("You are already being embraced.\n\r", ch);
			return;
		}
		if (number_percent () < ch->beast && ch->blood[0] < ch->blood[1])
		{
			send_to_char ("You are unable to stop drinking.\n\r", ch);
			WAIT_STATE (ch, 12);
			return;
		}
		act ("You retract your fangs from $N's neck.", ch, NULL, victim, TO_CHAR);
		act ("$n retracts $s fangs from your neck.", ch, NULL, victim, TO_VICT);
		act ("$n retracts $s fangs from $N's neck.", ch, NULL, victim, TO_NOTVICT);

		act ("You release your hold on $N.", ch, NULL, victim, TO_CHAR);
		act ("$n releases $s hold on you.", ch, NULL, victim, TO_VICT);
		act ("$n releases $s hold on $N.", ch, NULL, victim, TO_NOTVICT);

		ch->embrace = NULL;
		ch->embraced = ARE_NONE;
		victim->embrace = NULL;
		victim->embraced = ARE_NONE;
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Who do you wish to embrace?\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_MORE2 (ch, MORE2_CHISTOP))
	{
		send_to_char ("You find something terribly wrong..you can't feed!\n\r", ch);
		return;
	}

	if (ch == victim)
	{
		send_to_char ("You cannot embrace yourself, fool!\n\r", ch);
		return;
	}

	if (is_safe (ch, victim))
		return;

	if (victim->position == POS_FIGHTING)
	{
		send_to_char ("They are too busy fighting at the moment.\n\r", ch);
		return;
	}

	if (!IS_NPC (victim) && victim->position == POS_STUNNED)
	{
		send_to_char ("Not while they are stunned.\n\r", ch);
		return;
	}

	if (victim->embrace != NULL)
	{
		send_to_char ("They are already being embraced.\n\r", ch);
		return;
	}

	act ("You step towards $N, with your arms outstretched.", ch, NULL, victim, TO_CHAR);
	act ("$n step towards you, with $s arms outstretched.", ch, NULL, victim, TO_VICT);
	act ("$n step towards $N, with $s arms outstretched.", ch, NULL, victim, TO_NOTVICT);

	if (IS_MORE2 (ch, MORE2_CHISTOP))
	{
		send_to_char ("You find something terribly wrong..you can't feed!\n\r", ch);
		send_to_char ("You feel the vampires lips on your throat..then they pull away suddenly!\n\r", victim);
		send_to_char ("You start draining the mortal.....you step away perpelxed!\n\r", ch);
		ch->hit -= dam;
		ch->agg += dam * 0.005;
		return;
	}
	else if (!IS_NPC (victim) && IS_MORE3 (victim, MORE3_ANUBIS))
	{
		send_to_char ("A light from above bursts forth and knocks the vampire away from you!\n\r", victim);
		send_to_char ("You step towards the mortal....a light hits you from above!!\n\r", ch);
		ch->hit -= dam;
		ch->agg += dam * 0.005;
		return;
	}
	else if (IS_AFFECTED (victim, AFF_CHARM) && victim->master != NULL && victim->master == ch)
		send_to_char ("You stand helpless before your master.\n\r", victim);
	else if (IS_IMMUNE (victim, IMM_VAMPIRE))
		act ("You stand defenceless before $m", ch, NULL, victim, TO_VICT);
	else if (victim->position < POS_MEDITATING)
		act ("You leap onto $S defenseless body.", ch, NULL, victim, TO_CHAR);
	else
	{
		act ("$N leaps out of your reach!", ch, NULL, victim, TO_CHAR);
		act ("You leap out of $s reach!", ch, NULL, victim, TO_VICT);
		act ("$N leap out of $n's reach!", ch, NULL, victim, TO_NOTVICT);
		return;
	}

	act ("You grab hold of $N in a close embrace.", ch, NULL, victim, TO_CHAR);
	act ("$n grabs hold of you in a close embrace.", ch, NULL, victim, TO_VICT);
	act ("$n grabs hold of $N in a close embrace.", ch, NULL, victim, TO_NOTVICT);

	if (ch->position < POS_STANDING)
		return;

	if (!IS_VAMPAFF (ch, VAM_FANGS))
		do_fangs (ch, "");

	act ("You sink your fangs into $N's neck.", ch, NULL, victim, TO_CHAR);
	act ("$n sinks $s fangs into your neck.", ch, NULL, victim, TO_VICT);
	act ("$n sinks $s fangs into $N's neck.", ch, NULL, victim, TO_NOTVICT);

	ch->embrace = victim;
	ch->embraced = ARE_EMBRACING;
	victim->embrace = ch;
	victim->embraced = ARE_EMBRACED;

	return;
}



void update_gen (CHAR_DATA * ch)
{
	if (IS_NPC (ch) || !IS_VAMPIRE (ch))
		return;
	switch (ch->vampgen)
	{
	default:
		ch->blood[BLOOD_POOL] = 10;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 1:
		ch->blood[BLOOD_POOL] = 100;
		ch->blood[BLOOD_POTENCY] = 1;
	case 2:
		ch->blood[BLOOD_POOL] = 75;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 3:
		ch->blood[BLOOD_POOL] = 60;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 4:
		ch->blood[BLOOD_POOL] = 50;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 5:
		ch->blood[BLOOD_POOL] = 40;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 6:
		ch->blood[BLOOD_POOL] = 30;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 7:
		ch->blood[BLOOD_POOL] = 20;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 8:
		ch->blood[BLOOD_POOL] = 15;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 9:
		ch->blood[BLOOD_POOL] = 14;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 10:
		ch->blood[BLOOD_POOL] = 13;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 11:
		ch->blood[BLOOD_POOL] = 12;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 12:
		ch->blood[BLOOD_POOL] = 11;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	}
	return;
}



char *disc_name (int disc_no)
{
	static char disc[20];

	switch (disc_no)
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
	case DISC_CELERITY:
		strcpy (disc, "Celerity");
		break;
	case DISC_CHIMERSTRY:
		strcpy (disc, "Chimerstry");
		break;
	case DISC_DAIMOINON:
		strcpy (disc, "Daimoinon");
		break;
	case DISC_DOMINATE:
		strcpy (disc, "Dominate");
		break;
	case DISC_FORTITUDE:
		strcpy (disc, "Fortitude");
		break;
	case DISC_MELPOMINEE:
		strcpy (disc, "Melpominee");
		break;
	case DISC_MYTHERCERIA:
		strcpy (disc, "Mytherceria");
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
	case DISC_THANATOSIS:
		strcpy (disc, "Thanatosis");
		break;
	case DISC_THAUMATURGY:
		strcpy (disc, "Thaumaturgy");
		break;
	case DISC_VICISSITUDE:
		strcpy (disc, "Vicissitude");
		break;
	case DISC_MORTIS:
		strcpy (disc, "Mortis");
		break;
	case DISC_TEMPORIS:
		strcpy (disc, "Temporis");
		break;
	case DISC_VALEREN:
		strcpy (disc, "Valeren");
		break;
	case DISC_ABOMBWE:
		strcpy (disc, "Abombwe");
		break;
	case DISC_DEIMOS:
		strcpy (disc, "Deimos");
		break;
	case DISC_DEMENTATION:
		strcpy (disc, "Dementation");
		break;
	case DISC_GROTESQUOUS:
		strcpy (disc, "Grotesquous");
		break;
	case DISC_NIHILISTICS:
		strcpy (disc, "Nihilistics");
		break;
	case DISC_MA:
		strcpy (disc, "Ma");
		break;
	case DISC_BARDO:
		strcpy (disc, "Bardo");
		break;
	case DISC_MALEFICIA:
		strcpy (disc, "Maleficia");
		break;
	case DISC_MYTHERCIA:
		strcpy (disc, "Mythercia");
		break;
	case DISC_SPIRITUS:
		strcpy (disc, "Spiritus");
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
	return &disc[0];
}



void power_ghoul (CHAR_DATA * ch, CHAR_DATA * victim)
{
	int sn, disc;
	bool enhanced = FALSE;
	bool gain_disc;

	if (IS_NPC (ch) || IS_NPC (victim) || !IS_HERO (victim) || !IS_VAMPIRE (ch))
		return;
	if (victim->class == CLASS_NONE)
		victim->class = CLASS_GHOUL;
	else if (victim->class != CLASS_GHOUL)
		return;
	for (sn = 0; sn <= DISC_MAX; sn++)
	{
		gain_disc = TRUE;
		switch (sn)
		{
/*
		case DISC_ANIMALISM:
		case DISC_AUSPEX:
		case DISC_CELERITY:
		case DISC_DOMINATE:
		case DISC_FORTITUDE:
		case DISC_OBFUSCATE: */
		case DISC_POTENCE:
/*		case DISC_PRESENCE:
		case DISC_PROTEAN:
		case DISC_SERPENTIS:
		case DISC_THAUMATURGY:
		case DISC_VICISSITUDE: */
			break;
		default:
			gain_disc = FALSE;
			break;
		}
		if (!gain_disc)
			continue;
		if (ch->pcdata->powers[sn] > 0)
			disc = 1;
//			disc = (1 + ch->pcdata->powers[sn]) * 0.5;
		else
			disc = 1;

		if (disc > victim->pcdata->powers[sn])
		{
			victim->pcdata->powers[sn] = disc;
			enhanced = TRUE;
		}

	}

	if (enhanced)
		send_to_char ("You feel the potency of the blood run through your body.\n\r", victim);

	return;
}

void do_lick (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	one_argument (argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char ("#cHuh??#n\n\r", ch);
		return;
	}
	else
	{
		if (IS_VAMPIRE (ch))
		{
			if (!str_cmp (arg, "l") || !str_cmp (arg, "left"))
			{
				if (!IS_ARM_L (ch, SLIT_WRIST))
				{
					send_to_char ("Your left wrist doesn't need licking shut.\n\r", ch);
					return;
				}
				act ("You lick shut the cut in your left wrist.", ch, NULL, NULL, TO_CHAR);
				act ("$n licks shut the cut in $s left wrist.", ch, NULL, NULL, TO_ROOM);
				REMOVE_BIT (ch->loc_hp[6], BLEEDING_WRIST_L);
				REMOVE_BIT (ch->loc_hp[LOC_ARM_L], SLIT_WRIST);
				return;
			}
			else if (!str_cmp (arg, "r") || !str_cmp (arg, "right"))
			{
				if (!IS_ARM_R (ch, SLIT_WRIST))
				{
					send_to_char ("Your right wrist doesn't need licking shut.\n\r", ch);
					return;
				}
				act ("You lick shut the cut in your right wrist.", ch, NULL, NULL, TO_CHAR);
				act ("$n licks shut the cut in $s right wrist.", ch, NULL, NULL, TO_ROOM);
				REMOVE_BIT (ch->loc_hp[6], BLEEDING_WRIST_R);
				REMOVE_BIT (ch->loc_hp[LOC_ARM_R], SLIT_WRIST);
				return;
			}
		}
		send_to_char ("#cHuh??#n\n\r", ch);
		return;
	}
	return;
}


void do_short( CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg);
	argument = one_argument( argument, arg2);

	if (IS_NPC(ch))
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if ((!IS_WEREWOLF(ch) && !IS_VAMPIRE(ch)) || ((IS_VAMPIRE(ch) && get_disc (ch, DISC_PROTEAN) < 1 ) && ( IS_VAMPIRE(ch) && get_disc (ch, DISC_SERPENTIS) < 4 )))
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if (arg[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char( "Syntax: Short <form> <description>\n\r", ch);
		send_to_char( "Type short show all to see current.\n\r", ch );
		return;
	}

	if (!str_cmp( arg, "lupus") || !str_cmp(arg, "wolf") 
	|| !str_cmp( arg, "squamus") || !str_cmp(arg, "feline"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->lupusdescshort );
		ch->pcdata->lupusdescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (!str_cmp( arg, "crinos") || !str_cmp(arg, "bestial"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->crinosdescshort );
		ch->pcdata->crinosdescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (!str_cmp( arg, "hispo") || !str_cmp(arg, "bat")
	    || !str_cmp(arg, "chasmus")
	    || !str_cmp(arg, "cobra") || !str_cmp(arg, "sokoto"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->hispodescshort );
		ch->pcdata->hispodescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (!str_cmp( arg, "glabro") || !str_cmp(arg, "mist") 
	|| !str_cmp(arg, "gladius") || !str_cmp(arg, "chatro"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->glabrodescshort );
		ch->pcdata->glabrodescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (IS_WEREWOLF(ch))
	{
	sprintf( buf, "Lupus form short description: %s\n\r", ch->pcdata->lupusdescshort );
	send_to_char( buf, ch );
	sprintf( buf, "Crinos form short description: %s\n\r", ch->pcdata->crinosdescshort );
	send_to_char( buf, ch );
	sprintf( buf, "Hispo form short description: %s\n\r", ch->pcdata->hispodescshort );
	send_to_char( buf, ch );
	sprintf( buf, "Glabro form short description: %s\n\r", ch->pcdata->glabrodescshort );
	send_to_char( buf, ch );
	}
	else if (IS_VAMPIRE(ch) && get_disc (ch, DISC_PROTEAN) > 1 )
	{
	sprintf( buf, "Wolf form short description: %s\n\r",
		ch->pcdata->lupusdescshort);
	send_to_char( buf, ch );
	sprintf( buf, "Bat form short description: %s\n\r",
		ch->pcdata->hispodescshort);
	send_to_char( buf, ch );
	sprintf( buf, "Mist form short description: %s\n\r",
		ch->pcdata->glabrodescshort);
	send_to_char( buf, ch );
	sprintf( buf, "Bestial form short description: %s\n\r",
		ch->pcdata->crinosdescshort);
	send_to_char( buf, ch );
	}
	else
	{
	sprintf( buf, "Cobra form short description: %s\n\r",
		ch->pcdata->hispodescshort);
	send_to_char( buf, ch );
	}
	return;
}

