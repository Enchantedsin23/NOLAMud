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

void	birth_write		args( ( CHAR_DATA *ch, char *argument ) );
bool	birth_ok		args( ( CHAR_DATA *ch, char *argument ) );

void do_birth( CHAR_DATA *ch, char *argument )
{
    char buf2[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    if (!IS_EXTRA(ch, EXTRA_PREGNANT))
    {
	send_to_char("But you are not even pregnant!\n\r",ch);
	return;
    }

    if (!IS_EXTRA(ch, EXTRA_LABOUR))
    {
	send_to_char("You're not ready to give birth yet.\n\r",ch);
	return;
    }

    if ( argument[0] == '\0' )
    {
	if (ch->pcdata->genes[4] == SEX_MALE)
	    send_to_char( "What do you wish to name your little boy?\n\r", ch );
	else if (ch->pcdata->genes[4] == SEX_FEMALE)
	    send_to_char( "What do you wish to name your little girl?\n\r", ch );
	else
	    send_to_char( "What do you wish to name your child?\n\r", ch );
	return;
    }

    if (!check_parse_name( argument ))
    {
	send_to_char( "Thats an illegal name.\n\r", ch );
	return;
    }

    if ( char_exists(FILE_PLAYER,argument) )
    {
	send_to_char( "That player already exists.\n\r", ch );
	return;
    }

    strcpy(buf2,ch->pcdata->cparents);
    strcat(buf2," ");
    strcat(buf2,argument);
    if (!birth_ok(ch, buf2))
    {
	send_to_char( "Bug - please inform KaVir.\n\r", ch );
	return;
    }
    argument[0] = UPPER(argument[0]);
    birth_write( ch, argument );
    ch->pcdata->genes[9] += 1;
    REMOVE_BIT(ch->extra, EXTRA_PREGNANT);
    REMOVE_BIT(ch->extra, EXTRA_LABOUR);
    do_autosave(ch,"");
    return;
}

bool birth_ok( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char mum [MAX_INPUT_LENGTH];
    char dad [MAX_INPUT_LENGTH];
    char child [MAX_INPUT_LENGTH];

    argument = one_argument( argument, mum );
    argument = one_argument( argument, dad );
    argument = one_argument( argument, child );

    if (dad[0] == '\0')
    {
	send_to_char("You are unable to give birth - please inform KaVir.\n\r",ch);
	return FALSE;
    }
    if (child[0] == '\0')
    {
	send_to_char("You are unable to give birth - please inform KaVir.\n\r",ch);
	return FALSE;
    }
    dad[0] = UPPER(dad[0]);
    if (!str_cmp(dad,"Kavir")) strcpy(dad,"KaVir");
    child[0] = UPPER(child[0]);
    if (ch->pcdata->genes[4] == SEX_MALE)
    {
	send_to_char("You give birth to a little boy!\n\r",ch);
	sprintf(buf,"%s has given birth to %s's son, named %s!",ch->name,dad,child);
	do_rpose(ch,buf);
	return TRUE; 
    }
    else if (ch->pcdata->genes[4] == SEX_FEMALE)
    {
	send_to_char("You give birth to a little girl!\n\r",ch);
	sprintf(buf,"%s has given birth to %s's daughter, named %s!",ch->name,dad,child);
	do_rpose(ch,buf);
	return TRUE; 
    }
    return FALSE;
}



void birth_write( CHAR_DATA *ch, char *argument )
{
    FILE *fp;
    char buf [MAX_STRING_LENGTH];
    char *strtime;

    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';

    fflush( fpReserve );
    fclose( fpReserve );
    sprintf( buf, "%s%s", PLAYER_DIR, capitalize( argument ) );
    if ( ( fp = fopen(buf, "w") ) == NULL) 
    {
	bug( "Birth_write: fopen", 0 );
	perror( buf );
    }
    else
    {  
	fprintf( fp, "#PLAYER\n");
	fprintf( fp, "Name         %s~\n",capitalize( argument ));
	fprintf( fp, "Createtime   %s~\n",str_dup( strtime )	);
	fprintf( fp, "Sex          %d\n",ch->pcdata->genes[4]	);
	fprintf( fp, "Race         0\n"				);
	fprintf( fp, "Immune       %d\n",ch->pcdata->genes[3]	);
	fprintf( fp, "Level        2\n"				);
	fprintf( fp, "Room         %d\n",ch->in_room->vnum 	);
	fprintf( fp, "PkPdMkMd     0 0 5 0\n"			);
	fprintf( fp, "HpManaMove   %d %d %d %d %d %d\n",
	    ch->pcdata->genes[0], ch->pcdata->genes[0], ch->pcdata->genes[1], 
	    ch->pcdata->genes[1], ch->pcdata->genes[2], ch->pcdata->genes[2]);
	fprintf( fp, "Act          1600\n"   			);
	fprintf( fp, "Extra        32768\n"   			);
	fprintf( fp, "Position     7\n"				);
	fprintf( fp, "Password     %s~\n",ch->pcdata->pwd	);
	fprintf( fp, "Title         the mortal~\n"		);
	fprintf( fp, "Parents      %s~\n",ch->pcdata->cparents	);
	fprintf( fp, "AttrPerm     %d %d %d %d %d\n",
	    ch->pcdata->perm_str,
	    ch->pcdata->perm_int,
	    ch->pcdata->perm_wis,
	    ch->pcdata->perm_dex,
	    ch->pcdata->perm_con );
	fprintf( fp, "End\n\n" );
	fprintf( fp, "#END\n" );
    }
    fflush( fp );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
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

	if (!IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

//	if (!IS_ARM_L (ch, SLIT_WRIST) && !IS_ARM_R (ch, SLIT_WRIST))
//	{
//		send_to_char ("First you need to SLIT your wrist.\n\r", ch);
//		return;
//	}

	if (arg[0] == '\0')
	{
		send_to_char ("#gWho are you feeding Glamour to?#n\n\r", ch);
		return;
	}
	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("#gThat person isn't here.#n\n\r", ch);
		return;
	}
	if (IS_NPC (victim))
	{
		send_to_char ("#gNot on NPCs.#n\n\r", ch);
		return;
	}
	if ((IS_GHOUL (victim) || victim->class == CLASS_NONE) && victim->blood[0] <= 3)
	{
		if (!IS_HERO (victim))
			send_to_char ("#gGlamour would do them no good.#n\n\r", ch);
		else
			do_bite (ch, victim->name);
		return;
	}
	else if (!IS_VAMPIRE (victim) && !IS_GHOUL (victim) && victim->class != CLASS_NONE)
	{
		send_to_char ("#gGlamour would do them no good.#n\n\r", ch);
		return;
	}
	if (ch == victim)
	{
		send_to_char ("#gYou can't feed yourself Glamour.#n\n\r", ch);
		return;
	}
	if ((!IS_SET (victim->extra2, EXTRA2_TORPORED)) && (!IS_IMMUNE (victim, IMM_VAMPIRE)))
	{
		send_to_char ("#gThey are not willing to take your Glamour.#n\n\r", ch);
		return;
	}
	if (ch->blood[BLOOD_CURRENT] < (1 * ch->blood[BLOOD_POTENCY]))
	{
		send_to_char ("#gYou don't have enough Glamour.#n\n\r", ch);
		return;
	}

	act ("#gYou feed #G$N#g some of your Glamour.#n", ch, NULL, victim, TO_CHAR);
	act ("#G$n#g feeds#G $N#g some of $s Glamour.#n", ch, NULL, victim, TO_NOTVICT);

	if (victim->position < POS_RESTING)
		send_to_char ("#gYou feel an electrifying surge of beautiful warmth and energy.#n\n\r", victim);
	else
		act ("#G$n #gfeeds you some of $s Glamour.#n", ch, NULL, victim, TO_VICT);

	blood = number_range (1, 3) * ch->blood[BLOOD_POTENCY];
	ch->blood[BLOOD_CURRENT] -= blood;
	victim->blood[BLOOD_CURRENT] += blood;
	if (victim->blood[BLOOD_CURRENT] >= victim->blood[BLOOD_POOL])
	{
		victim->blood[BLOOD_CURRENT] = victim->blood[BLOOD_POOL];
		if (IS_VAMPIRE (victim))
			send_to_char ("#gThe unknowable ache for beauty and wonder is sated in you.#n\n\r", victim);
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
//	char buf[MAX_INPUT_LENGTH];
//	bool can_sire = FALSE;
	int blood;
	int ch_pot = ch->vamppot;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	if (!IS_CHANGELING (ch) || (ch->vamppot < 1))
	{
		send_to_char ("You aren't a Changeling.\n\r", ch);
		return;
	}

	if (is_affected (ch, gsn_potency))
		ch_pot++;

//	if (ch->vamppot == 2 && strlen (ch->bloodline) < 2)
//	{
//		send_to_char ("First you need to get a bloodline name.\n\r", ch);
//		return;
//	}


//	if (ch->pcdata->bloodlined[0] == -1 || ch->pcdata->bloodlined[1] == -1 || ch->pcdata->bloodlined[2] == -1)
//	{
//		send_to_char ("First you need to buy your disciplines.\n\r", ch);
//		return;
//	}

//	if (ch_pot > 15)
//		can_sire = FALSE;
//	else
//		can_sire = TRUE;
//
//	if (!can_sire)
//	{
//		send_to_char ("You are not able to create any childer.\n\r", ch);
//		return;
//	}

	if (arg[0] == '\0')
		return;

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("#gThat person isn't here.#n\n\r", ch);
		return;
	}

	if (IS_NPC (victim))
	{
		send_to_char ("#gNot on NPCs.#n\n\r", ch);
		return;
	}

//	if (!IS_IMMUNE (victim, IMM_VAMPIRE))
//	{
//		send_to_char ("#cThey refuse to drink your blood.\n\r", ch);
//		return;
//	}

//	if (ch == victim)
//	{
//		send_to_char ("You are already a vampire!\n\r", ch);
//		return;
//	}

//	if (victim->level != LEVEL_AVATAR && !IS_IMMORTAL (victim))
//	{
//		send_to_char ("You can only make avatars into vampires.\n\r", ch);
//		return;
//	}

//	if (IS_CHANGELING (victim))
//	{
//		send_to_char ("You are unable to create werevamps!\n\r", ch);
//		return;
//	}

//	if (IS_VAMPIRE (victim))
//	{
//		send_to_char ("But they are already a vampire!\n\r", ch);
//		return;
//	}



	if (ch->blood[BLOOD_CURRENT] < 3)
	{
		send_to_char ("#gYou don't have enough Glamour.#n\n\r", ch);
		return;
	}

	blood = number_range (1, 2) * ch->blood[BLOOD_POTENCY];
	ch->blood[BLOOD_CURRENT] -= blood;
	victim->blood[BLOOD_CURRENT] += blood;

	if (ch->beast > 0 && ch->beast < 100)
		ch->beast += 1;
	act ("#gYou feed #G$N#g some of your Glamour.#n", ch, NULL, victim, TO_CHAR);
	act ("#G$n#g feeds #G$N#g some of $s Glamour.#n", ch, NULL, victim, TO_NOTVICT);
	act ("#G$n#g feeds you some of $s Glamour.#n", ch, NULL, victim, TO_VICT);

	/* For Abominations */
//	if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_GET_OF_FENRIS) > 0)
//	{
//		act ("You scream in agony as the blood of Caine burns through your body!", victim, NULL, NULL, TO_CHAR);
//		act ("$n screams in agony and falls to the ground...DEAD!", victim, NULL, NULL, TO_ROOM);
//		victim->form = 1;
//		return;
//	}

//	victim->class = CLASS_VAMPIRE;
//	if (victim->vamppot != 0)
//	{
//		do_autosave (ch, "");
//		do_autosave (victim, "");
//		send_to_char ("Your vampiric powers have been restored.\n\r", victim);
//		return;
//	}

//	send_to_char ("You feel a strange and primal Thirst. You are now a vampire.\n\r", victim);
//	REMOVE_BIT (victim->extra, EXTRA_PREGNANT);
//	REMOVE_BIT (victim->extra, EXTRA_LABOUR);
//	victim->vamppot = ch_pot + 1;
//	update_pot (victim);
//	free_string (victim->lord);
//	if (ch->vamppot == 1)
//		victim->lord = str_dup (ch->name);
//	else
//	{
//		sprintf (buf, "%s %s", ch->lord, ch->name);
//		victim->lord = str_dup (buf);
//	}

//	if (strlen (ch->bloodline) > 1 && (IS_EXTRA (ch, EXTRA_PRINCE) || IS_EXTRA (ch, EXTRA_SIRE)))
//	{
//		free_string (victim->bloodline);
//		victim->bloodline = str_dup (ch->bloodline);
//		if (strlen (ch->side) > 1)
//		{
//			free_string (victim->side);
//			victim->side = str_dup (ch->side);
//		}
//	}
//	REMOVE_BIT (ch->extra, EXTRA_SIRE);
//	if (IS_MORE (ch, MORE_ANTITRIBU))
//		SET_BIT (victim->more, MORE_ANTITRIBU);

//	if (ch->vamppot == 1)
//	{
//		/* Second potency vampires chose all their disciplines */
//		victim->pcdata->bloodlined[0] = -1;
//		victim->pcdata->bloodlined[1] = -1;
//		victim->pcdata->bloodlined[2] = -1;
//	}
//	else
//	{
		
//		improve = victim->pcdata->bloodlined[0] = ch->pcdata->bloodlined[0];	
//		improve = victim->pcdata->bloodlined[1] = ch->pcdata->bloodlined[1];	
//		improve = victim->pcdata->bloodlined[2] = ch->pcdata->bloodlined[2];
//	}

//	for (sn = 0; sn <= DISC_MAX; sn++)
//	{
//		victim->pcdata->powers[sn] = 0;
//	}

//	victim->pcdata->powers[victim->pcdata->bloodlined[0]] = 1;
//	victim->pcdata->powers[victim->pcdata->bloodlined[1]] = 1;
//	victim->pcdata->powers[victim->pcdata->bloodlined[2]] = 1;

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
		send_to_char ("#gYou will now allow the Lost to harvest Glamour from you.#n\n\r", ch);
		SET_BIT (ch->immune, IMM_VAMPIRE);
		return;
	}
	send_to_char ("#gYou will no longer allow the Lost to harvest Glamour from you.#n\n\r", ch);
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
	send_to_char( "#gYou can shift between the following forms:#G Hishu #g(keyword #Ghuman#g),#G Dalu#g,#G Gauru#g,#G Urshul#g, and#G Urhan#n.\n\r", ch );return;}

	 if      ( !str_cmp(arg,"human" ) ) toform = 1;
	 else if ( !str_cmp(arg,"dalu") ) toform = 2;
	 else if ( !str_cmp(arg,"gauru" ) ) toform = 3;
	 else if ( !str_cmp(arg,"urshul" ) ) toform = 4;
	 else if ( !str_cmp(arg,"urhan" ) ) toform = 5;

	 else
{send_to_char( "#gYou can shift between the following forms:#G Hishu#g,#G Dalu#g,#G Gauru#g,#G Urshul#g, and#G Urhan#n.\n\r", ch );return;}

	 if ( ch->pcdata->wolfform[1] < 1 || ch->pcdata->wolfform[1] > 5)
	ch->pcdata->wolfform[1] = 1;
	 if ( ch->pcdata->wolfform[1] == toform )
	 {
	send_to_char( "#gYou are already in that form.#n\n\r", ch );
	return;
	 }

	 if (IS_VAMPAFF(ch,VAM_DISGUISED) )
	 {
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->vampaff, VAM_DISGUISED);
	REMOVE_BIT(ch->extra, EXTRA_SEPARATION);
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

		if (toform == 1)     // hishu
		{
		send_to_char( "You transform into a human being.\n\r", ch );
		sprintf( buf, "%s transforms into human form.\n\r", ch->name);
		act( buf, ch, NULL, NULL, TO_ROOM );
		if(IS_SET(ch->polyaff, POLY_WOLF))
		REMOVE_BIT(ch->polyaff, POLY_WOLF);
		REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
		clear_stats(ch);
		free_string( ch->morph );
		ch->morph = str_dup( "" );
		ch->pcdata->wolfform[1] = FORM_HUMAN; // double check
		if (ch->pcdata->wolf >= 100) ch->pcdata->wolf = 0;
		return;
	 }
	 else	if (toform == 2)     // dalu
		{
	send_to_char( "You transform into Dalu form.\n\r", ch );
	sprintf( buf, "%s shifts into Dalu form.\n\r", ch->name);
	act( buf, ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	free_string( ch->morph );
	if (ch->pcdata->daludescshort[0] == '\0' )
		ch->morph = str_dup( "a Dalu werewolf who has not set his short desc" );
	 else
		ch->morph = str_dup( ch->pcdata->daludescshort);
	ch->pcdata->wolfform[1] = FORM_DALU; //double check
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 1;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 1;

	return;
	 }
	 else	if (toform == 3)     // gauru
		{
	send_to_char( "You transform into Gauru form.\n\r", ch );
	sprintf( buf, "%s shifts into Gauru form.\n\r", ch->name);
	act( buf, ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	free_string( ch->morph );
	if (ch->pcdata->gaurudescshort[0] != '\0' )
		ch->morph = str_dup( ch->pcdata->gaurudescshort);
	else
	 switch (ch->pcdata->bloodlined[2])
	{
		default:  ch->morph = str_dup("a Gauru werewolf who has not set his short desc");  break; //default
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
	ch->pcdata->wolfform[1] = FORM_GAURU; //double check
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 3;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 1;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 2;
	ch->mod_attributes[ATTRIB_PER] = ch->mod_attributes[ATTRIB_PER] + 3;
	return;
	 }
	 else	if (toform == 4)     // urshul
		{
	send_to_char( "You transform into Urshul form.\n\r", ch );
	sprintf( buf, "%s shifts into Urshul form.\n\r", ch->name);
	act( buf, ch, NULL, NULL, TO_ROOM );
	clear_stats(ch);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	free_string( ch->morph );
	if (ch->pcdata->urshuldescshort[0] != '\0' )
		ch->morph = str_dup( ch->pcdata->urshuldescshort);
	else
		 switch (ch->pcdata->bloodlined[2])
		{
		default:  ch->morph = str_dup("a Urshul werewolf who has not set his short desc");  break; //default
		case 0:  ch->morph = str_dup("a large black and white furred urhan wolf"); break;	 //furies
		case 1:  ch->morph = str_dup("a smelly mottled furred wolf"); break;	 //gnawers
		case 2:  ch->morph = str_dup("A non-aggressive looking urhan wolf"); break;	 //gaia
		case 3:  ch->morph = str_dup("a large urhan wolf with auburn fur");break;	 //fianna
		case 4:  ch->morph = str_dup("a hulking grey furred urhan wolf");break; //fenris
		case 5:  ch->morph = str_dup("a domesticated looking urhan wolf");break;	 //walkers
		case 6:  ch->morph = str_dup("a large reddish-brown urhan wolf");break;	 // talons
		case 7: ch->morph = str_dup("a large pitch black urhan wolf");break; 	 //shadow lords
		case 8:  ch->morph = str_dup("a large jackal-like wolf"); break;	 //striders
		case 9:  ch->morph = str_dup("a large silver furred urhan wolf"); break;	 //silverf
		case 10:  ch->morph = str_dup("a slate grey furred urhan wolf"); break;	 //gazers
		case 11:  ch->morph = str_dup("a giant timber wolf"); break;	 //uktena
		case 12:  ch->morph = str_dup("a large reddish black furred wolf"); break;	 //wendigo
		}
	ch->mod_attributes[ATTRIB_STR] = ch->mod_attributes[ATTRIB_STR] + 2;
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 2;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 2;
	ch->mod_attributes[ATTRIB_MAN] = ch->mod_attributes[ATTRIB_MAN] - 3;
	ch->mod_attributes[ATTRIB_PER] = ch->mod_attributes[ATTRIB_PER] + 3;

	ch->pcdata->wolfform[1] = FORM_URSHUL; //double check
	return;
	 }
	 else	if (toform == 5)     // urhan
	 {
		send_to_char( "You transform into Urhan form.\n\r", ch );
		sprintf( buf, "%s transforms into Urhan form.\n\r", ch->name);
		act( buf, ch, NULL, NULL, TO_ROOM );
		clear_stats(ch);
		SET_BIT(ch->polyaff, POLY_WOLF);
		SET_BIT(ch->affected_by, AFF_POLYMORPH);
		free_string( ch->morph );
	if (ch->pcdata->urhandescshort[0] != '\0' )
		ch->morph = str_dup( ch->pcdata->urhandescshort);
	else
	 switch (ch->pcdata->bloodlined[2])
	{
		default:  ch->morph = str_dup("an Urhan werewolf who has not set his short desc");  break; //default
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
	ch->mod_attributes[ATTRIB_DEX] = ch->mod_attributes[ATTRIB_DEX] + 2;
	ch->mod_attributes[ATTRIB_STA] = ch->mod_attributes[ATTRIB_STA] + 1;
	ch->mod_attributes[ATTRIB_PER] = ch->mod_attributes[ATTRIB_PER] + 4;
		ch->pcdata->wolfform[1] = FORM_URHAN;
		return;
	 }
	else
	{send_to_char( "Error, in do_shift, please inform immortals.\n\r", ch );return;}
return;
}


void do_bloodlinename (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	if (IS_NPC (ch))
		return;

	if (arg[0] == '\0')
	{
		send_to_char ("Whose bloodline do you wish to change or name?\n\r", ch);
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
		send_to_char ("Bloodline name should be between 0 and 25 letters long.\n\r", ch);
		send_to_char ("Leave a blank to remove Bloodline.\n\r", ch);
		return;
	}
	free_string (victim->bloodline);
	victim->bloodline = str_dup (argument);
	send_to_char ("Bloodline set.\n\r", ch);
	send_to_char ("Bloodline set.\n\r", victim);
	return;
}

void do_bloodlinedisc (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	int improve, sn, col;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;

	send_to_char ("Huh?\n\r", ch);
	return;

	if (arg[0] == '\0')
	{
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		send_to_char ("                               -= Disciplines =-\n\r", ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		if (ch->vamppot <= 2)
		{
			/* Bloodline discipline */
			improve = ch->pcdata->bloodlined[0];
			sprintf (buf, "Bloodline Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Personal discipline 1 */
			improve = ch->pcdata->bloodlined[1];
			sprintf (buf, "Personal Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Personal discipline 2 */
			improve = ch->pcdata->bloodlined[2];
			sprintf (buf, "Personal Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);
		}
		else
		{
			/* Bloodline discipline */
			improve = ch->pcdata->bloodlined[0];
			sprintf (buf, "Bloodline Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Sires discipline */
			improve = ch->pcdata->bloodlined[1];
			sprintf (buf, "Sires Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);

			/* Personal discipline */
			improve = ch->pcdata->bloodlined[2];
			sprintf (buf, "Personal Discipline: %s.\n\r", disc_name (improve));
			send_to_char (buf, ch);
		}
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
		if (ch->pcdata->bloodlined[2] >= 0)
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

	if (!str_cmp (arg, "dominate"))
		improve = DISC_DOMINATE;
	else if (!str_cmp (arg, "auspex"))
		improve = DISC_AUSPEX;
	else if (!str_cmp (arg, "animalism"))
		improve = DISC_ANIMALISM;
	else if (!str_cmp (arg, "celerity"))
		improve = DISC_CELERITY;
	else if (!str_cmp (arg, "resilience"))
		improve = DISC_RESILIENCE;
	else if (!str_cmp (arg, "vigor"))
		improve = DISC_VIGOR;
	else if (!str_cmp (arg, "fleeting winter"))
		improve = DISC_FLEETINGWINTER;
	else if (!str_cmp (arg, "animation"))
		improve = DISC_ANIMATION;
	else if (!str_cmp (arg, "Eternal Summer"))
		improve = DISC_ETERNALSUMMER;
	else if (!str_cmp (arg, "Forge"))
		improve = DISC_FORGE;
	else if (!str_cmp (arg, "obfuscate"))
		improve = DISC_OBFUSCATE;
	else if (!str_cmp (arg, "darkness"))
		improve = DISC_DARKNESS;
	else if (!str_cmp (arg, "artifice"))
		improve = DISC_ARTIFICE;
	else if (!str_cmp (arg, "eternalspring"))
		improve = DISC_ETERNALSPRING;
	else if (!str_cmp (arg, "protean"))
		improve = DISC_PROTEAN;
	else if (!str_cmp (arg, "fleetingsummer"))
		improve = DISC_FLEETINGSUMMER;
	else if (!str_cmp (arg, "Reflections"))
		improve = DISC_REFLECTIONS;
	else if (!str_cmp (arg, "Stone"))
		improve = DISC_STONE;
	else if (!str_cmp (arg, "thebansorcery"))
		improve = DISC_THEBANSORCERY;
	else if (!str_cmp (arg, "Separation"))
		improve = DISC_SEPARATION;
	else if (!str_cmp (arg, "communion"))
		improve = DISC_COMMUNION;
	else if (!str_cmp (arg, "shadeandspirit"))
		improve = DISC_SHADEANDSPIRIT;
	else if (!str_cmp (arg, "fleetingspring"))
		improve = DISC_FLEETINGSPRING;
	else if (!str_cmp (arg, "majesty"))
		improve = DISC_MAJESTY;
	else if (!str_cmp (arg, "hours"))
		improve = DISC_HOURS;
	else if (!str_cmp (arg, "Moon"))
		improve = DISC_MOON;
	else if (!str_cmp (arg, "Vainglory"))
		improve = DISC_VAINGLORY;
	else if (!str_cmp (arg, "Wild"))
		improve = DISC_WILD;
	else if (!str_cmp (arg, "Mirror"))
		improve = DISC_MIRROR;
	else if (!str_cmp (arg, "Nightmare"))
		improve = DISC_NIGHTMARE;
	else if (!str_cmp (arg, "Tenure"))
		improve = DISC_TENURE;
	else if (!str_cmp (arg, "Phagia"))
		improve = DISC_PHAGIA;
	else if (!str_cmp (arg, "Fleeting Autumn"))
		improve = DISC_FLEETINGAUTUMN;
	else if (!str_cmp (arg, "eternalwinter"))
		improve = DISC_ETERNALWINTER;
	else
	{
		send_to_char ("You know of no such discipline.\n\r", ch);
		return;
	}

	if (ch->pcdata->bloodlined[0] == improve || ch->pcdata->bloodlined[1] == improve || ch->pcdata->bloodlined[2] == improve)
	{
		send_to_char ("You already know that discipline.\n\r", ch);
		return;
	}

	if (ch->vamppot < 3 || ch->class != CLASS_VAMPIRE)
	{
		if (ch->pcdata->bloodlined[0] < 0)
			ch->pcdata->bloodlined[0] = improve;
		else if (ch->pcdata->bloodlined[1] < 0)
			ch->pcdata->bloodlined[1] = improve;
		else if (ch->pcdata->bloodlined[2] < 0)
			ch->pcdata->bloodlined[2] = improve;
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
		if (ch->pcdata->bloodlined[2] < 0)
			ch->pcdata->bloodlined[2] = improve;
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

	if (!IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if ((IS_VAMPIRE (ch) || IS_GHOUL (ch)) && get_path2 (ch, PATH_MSPIRIT) < 5)
	{
		send_to_char ("You require Theban Sorcery Spirit Manipulation level 5 to enter the Umbra", ch);
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
		for (disc = DISC_DOMINATE; disc <= DISC_MAX; disc++)
			ch->pcdata->powers_mod[disc] += 10;
		if (IS_VAMPAFF (ch, VAM_DISGUISED))
		{
			REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
			REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
			REMOVE_BIT (ch->extra, EXTRA_SEPARATION);
			REMOVE_BIT (ch->polyaff, POLY_SPIRIT);
			free_string (ch->morph);
			ch->morph = str_dup ("");
			free_string (ch->long_descr);
			ch->morph = str_dup ("");
		}
		if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
			do_shadowplane (ch, "");
//		if (IS_VAMPAFF (ch, VAM_FANGS))
//			do_fangs (ch, "");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
			do_truesight (ch, "");
		if (IS_SET (ch->act, PLR_HOLYLIGHT))
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_FORGE))
		{
			ch->pcdata->forge = 0;
		}
		ch->pcdata->wolf = 0;
		for (disc = DISC_DOMINATE; disc <= DISC_MAX; disc++)
			ch->pcdata->powers_mod[disc] -= 10;
	}
	return;
}

void do_changeling (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (!IS_CHANGELING (ch))
		return;
	if (IS_SET (ch->act, PLR_WOLFMAN))
		return;
	if (ch->pcdata->wolfform[1] != FORM_HUMAN)
		return;

	if (IS_VAMPAFF (ch, VAM_DISGUISED))
	{
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		REMOVE_BIT (ch->vampaff, VAM_DISGUISED);
		REMOVE_BIT (ch->extra, EXTRA_SEPARATION);
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

	SET_BIT (ch->affected_by, AFF_POLYMORPH);
	SET_BIT (ch->vampaff, VAM_DISGUISED);
	ch->pcdata->wolfform[1] = FORM_GAURU;
	
	if (IS_POLYAFF (ch, POLY_ZULO))
		sprintf (buf, "An enormous black hairy monster");
	else
		sprintf (buf, "%s the huge beast", ch->name);
		
	free_string (ch->morph);
	ch->morph = str_dup (buf);
	ch->pcdata->wolf += 25;
	if (get_organization (ch, ORGANIZATION_GET_OF_FENRIS) > 4)
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

void do_unchangeling (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument (argument, arg);

	if (IS_NPC (ch))
		return;
	if (!IS_CHANGELING (ch))
		return;
	if (!IS_SET (ch->act, PLR_WOLFMAN))
		return;
	REMOVE_BIT (ch->act, PLR_WOLFMAN);
	ch->pcdata->wolfform[1] = FORM_HUMAN;
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

	if (!IS_VAMPIRE (ch) && !IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("Syntax is: favour <target> <favour>\n\r", ch);
		if (ch->vamppot == 3)
		{
			if (IS_CHANGELING (ch))
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
		if (ch->vamppot != 4 && !IS_EXTRA (ch, EXTRA_PRINCE))
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
		if (IS_EXTRA (victim, EXTRA_SIRE))
		{
			if (IS_CHANGELING (ch))
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
		if (IS_CHANGELING (ch))
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
		if (ch->vamppot != 3)
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
	else if (!str_cmp (arg2, "shaman") && IS_CHANGELING (ch))
	{
		if (ch->vamppot != 2)
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
		if (ch->vamppot > 4 && !IS_EXTRA (ch, EXTRA_PRINCE) && !IS_CHANGELING (ch))
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
		if (!IS_IMMUNE (victim, IMM_VAMPIRE))
		{
			send_to_char ("You cannot induct an unwilling person.\n\r", ch);
			return;
		}
		if (ch->vamppot > victim->vamppot)
		{
			if (IS_CHANGELING (ch))
				send_to_char ("You can only induct those of weaker heritage than yourself.\n\r", ch);
			else
				send_to_char ("You can only induct those of higher potency than yourself.\n\r", ch);
			return;
		}
		if (IS_CHANGELING (ch))
		{
			act ("You induct $N into your organization!", ch, NULL, victim, TO_CHAR);
			act ("$n inducts $N into $s organization!", ch, NULL, victim, TO_NOTVICT);
			act ("$n inducts you into $s organization!", ch, NULL, victim, TO_VICT);
		}
		else
		{
			act ("You induct $N into your bloodline!", ch, NULL, victim, TO_CHAR);
			act ("$n inducts $N into $s bloodline!", ch, NULL, victim, TO_NOTVICT);
			act ("$n inducts you into $s bloodline!", ch, NULL, victim, TO_VICT);
		}
		free_string (victim->bloodline);
		victim->bloodline = str_dup (ch->bloodline);
		victim->pcdata->bloodlined[2] = gifts = ch->pcdata->bloodlined[2];	//gives you inductors bloodline gift
		victim->pcdata->organizations[gifts] = 1;
		return;
	}
	else if (!str_cmp (arg2, "outcast"))
	{
		if (ch->vamppot > 4 && !IS_EXTRA (ch, EXTRA_PRINCE))
		{
			send_to_char ("You are unable to grant this sort of favour.\n\r", ch);
			return;
		}
		if (IS_EXTRA (victim, EXTRA_PRINCE) && ch->vamppot != 2)
		{
			if (IS_CHANGELING (ch))
				send_to_char ("You cannot outcast another Shaman.\n\r", ch);
			else
				send_to_char ("You cannot outcast another Prince.\n\r", ch);
			return;
		}
		if (ch->vamppot >= victim->vamppot)
		{
			if (IS_CHANGELING (ch))
				send_to_char ("You can only outcast those of weaker heritage than yourself.\n\r", ch);
			else
				send_to_char ("You can only outcast those of higher potency than yourself.\n\r", ch);
			return;
		}
		if (IS_CHANGELING (ch))
		{
			act ("You outcast $N from your organization!", ch, NULL, victim, TO_CHAR);
			act ("$n outcasts $N from $s organization!", ch, NULL, victim, TO_NOTVICT);
			act ("$n outcasts you from $s organization!", ch, NULL, victim, TO_VICT);
		}
		else
		{
			act ("You outcast $N from your bloodline!", ch, NULL, victim, TO_CHAR);
			act ("$n outcasts $N from $s bloodline!", ch, NULL, victim, TO_NOTVICT);
			act ("$n outcasts you from $s bloodline!", ch, NULL, victim, TO_VICT);
		}
		free_string (victim->bloodline);
		victim->bloodline = str_dup ("");
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

	//fflush (fpReserve);
	//fclose (fpReserve);
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
		//fpReserve = fopen (NULL_FILE, "r");
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

	if (!IS_VAMPIRE(ch) && !IS_WEREWOLF(ch))
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

			case DISC_DOMINATE:
				strcpy (disc, "Dominate");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "Auspex");
				break;
			case DISC_MAJESTY:
				strcpy (disc, "Majesty");
				break;
			case DISC_NIGHTMARE:
				strcpy (disc, "Nightmare");
				break;
			case DISC_ANIMALISM:
				strcpy (disc, "Animalism");
				break;
			case DISC_CELERITY:
				strcpy (disc, "Celerity");
				break;
			case DISC_RESILIENCE:
				strcpy (disc, "Resilience");
				break;
			case DISC_HOURS:
				strcpy (disc, "Hours");
				break;
			case DISC_MOON:
				strcpy (disc, "Moon");
				break;
			case DISC_VIGOR:
				strcpy (disc, "Vigor");
				break;
			case DISC_FLEETINGWINTER:
				strcpy (disc, "Fleeting Winter");
				break;
			case DISC_MIRROR:
				strcpy (disc, "Mirror");
				break;
			case DISC_ANIMATION:
				strcpy (disc, "Animation");
				break;
			case DISC_COMMUNION:
				strcpy (disc, "Communion");
				break;
			case DISC_ETERNALSUMMER:
				strcpy (disc, "Eternal Summer");
				break;
			case DISC_FORGE:
				strcpy (disc, "Forge");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "Obfuscate");
				break;
			case DISC_DARKNESS:
				strcpy (disc, "Darkness");
				break;
			case DISC_ARTIFICE:
				strcpy (disc, "Artifice");
				break;
			case DISC_ETERNALSPRING:
				strcpy (disc, "Eternal Spring");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "Protean");
				break;
			case DISC_FLEETINGSUMMER:
				strcpy (disc, "Fleeting Summer");
				break;
			case DISC_REFLECTIONS:
				strcpy (disc, "Reflections");
				break;
			case DISC_FLEETINGAUTUMN:
				strcpy (disc, "Fleeting Autumn");
				break;
			case DISC_SHADEANDSPIRIT:
				strcpy (disc, "Shade & Spirit");
				break;
			case DISC_STONE:
				strcpy (disc, "Stone");
				break;
			case DISC_THEBANSORCERY:
				strcpy (disc, "Theban Sorcery");
				break;
			case DISC_SEPARATION:
				strcpy (disc, "Separation");
				break;
			case DISC_FLEETINGSPRING:
				strcpy (disc, "Fleeting Spring");
				break;
			case DISC_VAINGLORY:
				strcpy (disc, "Vainglory");
				break;
			case DISC_WILD:
				strcpy (disc, "Wild");
				break;
			case DISC_TENURE:
				strcpy (disc, "Tenure");
				break;
			case DISC_PHAGIA:
				strcpy (disc, "Phagia");
				break;
			case DISC_ETERNALWINTER:
				strcpy (disc, "Eternalwinter");
				break;
			}
			if ((ch->class == CLASS_VAMPIRE || ch->class == CLASS_WEREWOLF) && (ch->pcdata->bloodlined[0] == sn || ch->pcdata->bloodlined[1] == sn || ch->pcdata->bloodlined[2] == sn))
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

	if (!str_cmp (arg, "dominate"))
		improve = DISC_DOMINATE;
	else if (!str_cmp (arg, "auspex"))
		improve = DISC_AUSPEX;
	else if (!str_cmp (arg, "animalism"))
		improve = DISC_ANIMALISM;
	else if (!str_cmp (arg, "celerity"))
		improve = DISC_CELERITY;
	else if (!str_cmp (arg, "resilience"))
		improve = DISC_RESILIENCE;
	else if (!str_cmp (arg, "vigor"))
		improve = DISC_VIGOR;
	else if (!str_cmp (arg, "fleeting winter"))
		improve = DISC_FLEETINGWINTER;
	else if (!str_cmp (arg, "animation"))
		improve = DISC_ANIMATION;
	else if (!str_cmp (arg, "Eternal Summer"))
		improve = DISC_ETERNALSUMMER;
	else if (!str_cmp (arg, "Forge"))
		improve = DISC_FORGE;
	else if (!str_cmp (arg, "obfuscate"))
		improve = DISC_OBFUSCATE;
	else if (!str_cmp (arg, "darkness"))
		improve = DISC_DARKNESS;
	else if (!str_cmp (arg, "artifice"))
		improve = DISC_ARTIFICE;
	else if (!str_cmp (arg, "eternalspring"))
		improve = DISC_ETERNALSPRING;
	else if (!str_cmp (arg, "protean"))
		improve = DISC_PROTEAN;
	else if (!str_cmp (arg, "fleetingsummer"))
		improve = DISC_FLEETINGSUMMER;
	else if (!str_cmp (arg, "Reflections"))
		improve = DISC_REFLECTIONS;
	else if (!str_cmp (arg, "Stone"))
		improve = DISC_STONE;
	else if (!str_cmp (arg, "thebansorcery"))
		improve = DISC_THEBANSORCERY;
	else if (!str_cmp (arg, "Separation"))
		improve = DISC_SEPARATION;
	else if (!str_cmp (arg, "communion"))
		improve = DISC_COMMUNION;
	else if (!str_cmp (arg, "SHADEANDSPIRIT"))
		improve = DISC_SHADEANDSPIRIT;
	else if (!str_cmp (arg, "fleetingspring"))
		improve = DISC_FLEETINGSPRING;
	else if (!str_cmp (arg, "majesty"))
		improve = DISC_MAJESTY;
	else if (!str_cmp (arg, "hours"))
		improve = DISC_HOURS;
	else if (!str_cmp (arg, "Moon"))
		improve = DISC_MOON;
	else if (!str_cmp (arg, "Vainglory"))
		improve = DISC_VAINGLORY;
	else if (!str_cmp (arg, "Wild"))
		improve = DISC_WILD;
	else if (!str_cmp (arg, "Mirror"))
		improve = DISC_MIRROR;
	else if (!str_cmp (arg, "Nightmare"))
		improve = DISC_NIGHTMARE;
	else if (!str_cmp (arg, "Tenure"))
		improve = DISC_TENURE;
	else if (!str_cmp (arg, "Phagia"))
		improve = DISC_PHAGIA;
	else if (!str_cmp (arg, "Fleeting Autumn"))
		improve = DISC_FLEETINGAUTUMN;
	else if (!str_cmp (arg, "eternalwinter"))
		improve = DISC_ETERNALWINTER;
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

	switch (ch->vamppot)
	{
	case 13:
		max_d = 10;
		break;
	case 12:
		max_d = 10;
		break;
	case 11:
		max_d = 10;
		break;
	case 10:
		max_d = 9;
		break;
	case 9:
		max_d = 8;
		break;
	case 8:
		max_d = 7;
		break;
	case 7:
		max_d = 6;
		break;
	default:
		max_d = 5;
		break;
	}

	if (improve == DISC_THEBANSORCERY)
		max_d = 5;	//thaum only has 5

	if (improve == DISC_COMMUNION)
		max_d = 8;
	if (improve == DISC_HOURS)
		max_d = 6;
	if (improve == DISC_MIRROR)
		max_d = 4;
	if (improve == DISC_MAJESTY)
		max_d = 6;
	if (improve == DISC_NIGHTMARE)
		max_d = 9;

	if (improve == DISC_TENURE)
		max_d = 5;
	if (improve == DISC_ANIMATION)
		max_d = 7;
	
	if (improve == DISC_WILD)
		max_d = 7;
	if (improve == DISC_FLEETINGAUTUMN)
		max_d = 7;
	if (improve == DISC_SHADEANDSPIRIT)
		max_d = 9;


	if (ch->pcdata->powers[improve] >= max_d)
	{
		send_to_char ("You are unable to improve your ability in that discipline any further.\n\r", ch);
		return;
	}

	if (ch->pcdata->bloodlined[0] == improve || ch->pcdata->bloodlined[1] == improve || ch->pcdata->bloodlined[2] == improve)
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
			case DISC_DOMINATE:
				strcpy (disc, "Dominate");
				break;
			case DISC_AUSPEX:
				strcpy (disc, "Auspex");
				break;
			case DISC_MAJESTY:
				strcpy (disc, "Majesty");
				break;
			case DISC_NIGHTMARE:
				strcpy (disc, "Nightmare");
				break;
			case DISC_ANIMALISM:
				strcpy (disc, "Animalism");
				break;
			case DISC_CELERITY:
				strcpy (disc, "Celerity");
				break;
			case DISC_RESILIENCE:
				strcpy (disc, "Resilience");
				break;
			case DISC_HOURS:
				strcpy (disc, "Hours");
				break;
			case DISC_MOON:
				strcpy (disc, "Moon");
				break;
			case DISC_VIGOR:
				strcpy (disc, "Vigor");
				break;
			case DISC_FLEETINGWINTER:
				strcpy (disc, "Fleeting Winter");
				break;
			case DISC_MIRROR:
				strcpy (disc, "Mirror");
				break;
			case DISC_ANIMATION:
				strcpy (disc, "Animation");
				break;
			case DISC_COMMUNION:
				strcpy (disc, "Communion");
				break;
			case DISC_ETERNALSUMMER:
				strcpy (disc, "Eternal Summer");
				break;
			case DISC_FORGE:
				strcpy (disc, "Forge");
				break;
			case DISC_OBFUSCATE:
				strcpy (disc, "Obfuscate");
				break;
			case DISC_DARKNESS:
				strcpy (disc, "Darkness");
				break;
			case DISC_ARTIFICE:
				strcpy (disc, "Artifice");
				break;
			case DISC_ETERNALSPRING:
				strcpy (disc, "Eternal Spring");
				break;
			case DISC_PROTEAN:
				strcpy (disc, "Protean");
				break;
			case DISC_FLEETINGSUMMER:
				strcpy (disc, "Fleeting Summer");
				break;
			case DISC_REFLECTIONS:
				strcpy (disc, "Reflections");
				break;
			case DISC_FLEETINGAUTUMN:
				strcpy (disc, "Fleeting Autumn");
				break;
			case DISC_SHADEANDSPIRIT:
				strcpy (disc, "Shade & Spirit");
				break;
			case DISC_STONE:
				strcpy (disc, "Stone");
				break;
			case DISC_THEBANSORCERY:
				strcpy (disc, "Theban Sorcery");
				break;
			case DISC_SEPARATION:
				strcpy (disc, "Separation");
				break;
			case DISC_FLEETINGSPRING:
				strcpy (disc, "Fleeting Spring");
				break;
			case DISC_VAINGLORY:
				strcpy (disc, "Vainglory");
				break;
			case DISC_WILD:
				strcpy (disc, "Wild");
				break;
			case DISC_TENURE:
				strcpy (disc, "Tenure");
				break;
			case DISC_PHAGIA:
				strcpy (disc, "Phagia");
				break;
			case DISC_ETERNALWINTER:
				strcpy (disc, "Eternal Winter");
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

	if (!str_cmp (arg2, "dominate"))
		improve = DISC_DOMINATE;
	else if (!str_cmp (arg2, "auspex"))
		improve = DISC_AUSPEX;
	else if (!str_cmp (arg2, "animalism"))
		improve = DISC_ANIMALISM;
	else if (!str_cmp (arg2, "celerity"))
		improve = DISC_CELERITY;
	else if (!str_cmp (arg2, "resilience"))
		improve = DISC_RESILIENCE;
	else if (!str_cmp (arg2, "vigor"))
		improve = DISC_VIGOR;
	else if (!str_cmp (arg2, "fleeting winter"))
		improve = DISC_FLEETINGWINTER;
	else if (!str_cmp (arg2, "animation"))
		improve = DISC_ANIMATION;
	
	else if (!str_cmp (arg2, "Eternal Summer"))
		improve = DISC_ETERNALSUMMER;
	else if (!str_cmp (arg2, "Forge"))
		improve = DISC_FORGE;
	else if (!str_cmp (arg2, "obfuscate"))
		improve = DISC_OBFUSCATE;
	else if (!str_cmp (arg2, "darkness"))
		improve = DISC_DARKNESS;
	else if (!str_cmp (arg2, "artifice"))
		improve = DISC_ARTIFICE;
	else if (!str_cmp (arg2, "Eternal Spring"))
		improve = DISC_ETERNALSPRING;
	else if (!str_cmp (arg2, "Protean"))
		improve = DISC_PROTEAN;
	else if (!str_cmp (arg2, "fleetingsummer"))
		improve = DISC_FLEETINGSUMMER;
	else if (!str_cmp (arg2, "Reflections"))
		improve = DISC_REFLECTIONS;
	else if (!str_cmp (arg2, "Stone"))
		improve = DISC_STONE;
	else if (!str_cmp (arg2, "thebansorcery"))
		improve = DISC_THEBANSORCERY;
	else if (!str_cmp (arg2, "Separation"))
		improve = DISC_SEPARATION;
	else if (!str_cmp (arg2, "Shade & Spirit"))
		improve = DISC_SHADEANDSPIRIT;
	else if (!str_cmp (arg2, "communion"))
		improve = DISC_COMMUNION;
	else if (!str_cmp (arg2, "fleetingspring"))
		improve = DISC_FLEETINGSPRING;
	else if (!str_cmp (arg2, "majesty"))
		improve = DISC_MAJESTY;
	else if (!str_cmp (arg2, "hours"))
		improve = DISC_HOURS;
	else if (!str_cmp (arg2, "Moon"))
		improve = DISC_MOON;
	else if (!str_cmp (arg2, "Vainglory"))
		improve = DISC_VAINGLORY;
	else if (!str_cmp (arg2, "wild"))
		improve = DISC_WILD;
	else if (!str_cmp (arg2, "Mirror"))
		improve = DISC_MIRROR;
	else if (!str_cmp (arg2, "Nightmare"))
		improve = DISC_NIGHTMARE;
	else if (!str_cmp (arg2, "Tenure"))
		improve = DISC_TENURE;
	else if (!str_cmp (arg2, "Phagia"))
		improve = DISC_PHAGIA;
	else if (!str_cmp (arg2, "fleetingautumn"))
		improve = DISC_FLEETINGAUTUMN;
	else if (!str_cmp (arg2, "eternal winter"))
		improve = DISC_ETERNALWINTER;
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

	if (!IS_CHANGELING (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}

	if ((victim = ch->embrace) != NULL && victim->embrace != NULL && victim->embrace == ch)
	{
		if (ch->embraced == ARE_EMBRACED)
		{
			send_to_char ("#gYou are already being harvested from.#n\n\r", ch);
			return;
		}
		if (number_percent () < ch->beast && ch->blood[0] < ch->blood[1])
		{
			send_to_char ("#gYou are unable to stop harvesting.#n\n\r", ch);
			WAIT_STATE (ch, 12);
			return;
		}
		act ("#gYou stop harvesting Glamour from your victim.#n", ch, NULL, victim, TO_CHAR);
		act ("#gThe sensation of emotion leaving you comes to a steady stop.#n", ch, NULL, victim, TO_VICT);

		ch->embrace = NULL;
		ch->embraced = ARE_NONE;
		victim->embrace = NULL;
		victim->embraced = ARE_NONE;
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char ("Who do you wish harvest glamour from?\n\r", ch);
		return;
	}

	if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
		return;
	}
	if (IS_MORE2 (ch, MORE2_CHISTOP))
	{
		send_to_char ("#gYou find something terribly wrong..you can't harvest Glamour!#n\n\r", ch);
		return;
	}

	if (ch == victim)
	{
		send_to_char ("#gYou cannot harvest Glamour from yourself.#n\n\r", ch);
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
		send_to_char ("#gThey are already being harvested from.#n\n\r", ch);
		return;
	}

	act ("#gYou are about to harvest Glamour from a victim. Mortals have 10 Glamour points. If you take more than 7 they will require Staff intervention. Be sure to specify to your victim which #Gtype#g of emotion is being harvested.#n", ch, NULL, victim, TO_CHAR);
	act ("#gA Changeling is about to harvest Glamour from you. Mortals have 10 Glamour points. If you lose more than 7 please #wrequest#e for a staff member.#n\n\r", ch, NULL, victim, TO_VICT);

	if (IS_MORE2 (ch, MORE2_CHISTOP))
	{
		send_to_char ("#gYou find something terribly wrong... you can't harvest Glamour!#n\n\r", ch);
		send_to_char ("#gYou feel a drain on your emotions that swiftly fades.#n\n\r", victim);
		send_to_char ("#gYou start harvesting emotions and suddenly stop, confused.#n\n\r", ch);
		ch->hit -= dam;
		ch->agg += dam * 0.005;
		return;
	}
	else if (!IS_NPC (victim) && IS_MORE3 (victim, MORE3_ANUBIS))
	{
		send_to_char ("#cA light from above bursts forth and knocks the vampire away from you!#n\n\r", victim);
		send_to_char ("#cYou step towards the mortal....a light hits you from above!#n\n\r", ch);
		ch->hit -= dam;
		ch->agg += dam * 0.005;
		return;
	}
	else if (IS_AFFECTED (victim, AFF_CHARM) && victim->master != NULL && victim->master == ch)
		send_to_char ("#gMortals have 10 Glamour points. If you lose more than 7 please #wrequest#e for a Staff member.#n\n\r", victim);
	else if (IS_IMMUNE (victim, IMM_VAMPIRE))
		act ("#G$n #gharvests your emotions, draining them and numbing their intensity.#n", ch, NULL, victim, TO_VICT);
	else if (victim->position < POS_MEDITATING)
		act ("#gYou begin harvesting emotions from #G$S#g, replenishing your Glamour.#n", ch, NULL, victim, TO_CHAR);
	else
	{
		act ("#y(#gNOTE#y)#g The victim has her or his consent toggled off. If he or she is willing or has no choice, she must toggle #Gconsent#g.#n", ch, NULL, victim, TO_CHAR);
		act ("#y(#gNOTE#y)#g You may not be harvested for Glamour because your consent is not toggled on. Type #Gconsent#g if willing or incapable of prevention.#n", ch, NULL, victim, TO_VICT);
		act ("#y(#gNOTE#y)#g The victim has his or her consent toggled off and cannot be harvested for Glamour.", ch, NULL, victim, TO_NOTVICT);
		return;
	}

	if (ch->position < POS_STANDING)
		return;

//	if (!IS_VAMPAFF (ch, VAM_FANGS))
//		do_fangs (ch, "");

	ch->embrace = victim;
	ch->embraced = ARE_EMBRACING;
	victim->embrace = ch;
	victim->embraced = ARE_EMBRACED;

	return;
}

OBJ_DATA *get_page( OBJ_DATA *book, int page_num )
{
    OBJ_DATA *page;
    OBJ_DATA *page_next;

    if (page_num < 1) return NULL;
    for ( page = book->contains; page != NULL; page = page_next )
    {
	page_next = page->next_content;
	if (page->value[0] == page_num) return page;
    }
    return NULL;
}

void update_pot (CHAR_DATA * ch)
{
	if (IS_NPC (ch) || !IS_CHANGELING (ch))
		return;
	switch (ch->vamppot)
	{
	default:
		ch->blood[BLOOD_POOL] = 10;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 13:
		ch->blood[BLOOD_POOL] = 100;
		ch->blood[BLOOD_POTENCY] = 1;
	case 12:
		ch->blood[BLOOD_POOL] = 75;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 11:
		ch->blood[BLOOD_POOL] = 60;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 10:
		ch->blood[BLOOD_POOL] = 50;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 9:
		ch->blood[BLOOD_POOL] = 40;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 8:
		ch->blood[BLOOD_POOL] = 30;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 7:
		ch->blood[BLOOD_POOL] = 20;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 6:
		ch->blood[BLOOD_POOL] = 15;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 5:
		ch->blood[BLOOD_POOL] = 14;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 4:
		ch->blood[BLOOD_POOL] = 13;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 3:
		ch->blood[BLOOD_POOL] = 12;
		ch->blood[BLOOD_POTENCY] = 1;
		break;
	case 2:
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
	case DISC_DOMINATE:
		strcpy (disc, "Dominate");
		break;
	case DISC_AUSPEX:
		strcpy (disc, "Auspex");
		break;
	case DISC_ANIMALISM:
		strcpy (disc, "Animalism");
		break;
	case DISC_CELERITY:
		strcpy (disc, "Celerity");
		break;
	case DISC_RESILIENCE:
		strcpy (disc, "Resilience");
		break;
	case DISC_VIGOR:
		strcpy (disc, "Vigor");
		break;
	case DISC_FLEETINGWINTER:
		strcpy (disc, "Fleeting Winter");
		break;
	case DISC_ANIMATION:
		strcpy (disc, "Animation");
		break;
	case DISC_ETERNALSUMMER:
		strcpy (disc, "Eternal Summer");
		break;
	case DISC_FORGE:
		strcpy (disc, "Forge");
		break;
	case DISC_OBFUSCATE:
		strcpy (disc, "Obfuscate");
		break;
	case DISC_DARKNESS:
		strcpy (disc, "Darkness");
		break;
	case DISC_ARTIFICE:
		strcpy (disc, "Artifice");
		break;
	case DISC_ETERNALSPRING:
		strcpy (disc, "Eternal Spring");
		break;
	case DISC_PROTEAN:
		strcpy (disc, "Protean");
		break;
	case DISC_FLEETINGSUMMER:
		strcpy (disc, "Fleeting Summer");
		break;
	case DISC_REFLECTIONS:
		strcpy (disc, "Reflections");
		break;
	case DISC_STONE:
		strcpy (disc, "Stone");
		break;
	case DISC_THEBANSORCERY:
		strcpy (disc, "Theban Sorcery");
		break;
	case DISC_SEPARATION:
		strcpy (disc, "Separation");
		break;
	case DISC_COMMUNION:
		strcpy (disc, "Communion");
		break;
	case DISC_SHADEANDSPIRIT:
		strcpy (disc, "Shade & Spirit");
		break;
	case DISC_FLEETINGSPRING:
		strcpy (disc, "Fleeting Spring");
		break;
	case DISC_MAJESTY:
		strcpy (disc, "Majesty");
		break;
	case DISC_HOURS:
		strcpy (disc, "Hours");
		break;
	case DISC_MOON:
		strcpy (disc, "Moon");
		break;
	case DISC_VAINGLORY:
		strcpy (disc, "Vainglory");
		break;
	case DISC_WILD:
		strcpy (disc, "Wild");
		break;
	case DISC_MIRROR:
		strcpy (disc, "Mirror");
		break;
	case DISC_NIGHTMARE:
		strcpy (disc, "Nightmare");
		break;
	case DISC_TENURE:
		strcpy (disc, "Tenure");
		break;
	case DISC_PHAGIA:
		strcpy (disc, "Phagia");
		break;
	case DISC_FLEETINGAUTUMN:
		strcpy (disc, "Fleeting Autumn");
		break;
	case DISC_ETERNALWINTER:
		strcpy (disc, "Eternal Winter");
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
		victim->class = CLASS_VAMPIRE;
	else if (victim->class != CLASS_VAMPIRE)
		return;
	for (sn = 0; sn <= DISC_MAX; sn++)
	{
		gain_disc = TRUE;
		switch (sn)
		{
/*
		case DISC_DOMINATE:
		case DISC_AUSPEX:
		case DISC_ANIMALISM:
		case DISC_VIGOR:
		case DISC_FLEETINGWINTER:
		case DISC_OBFUSCATE: */
		case DISC_ARTIFICE:
/*		case DISC_ETERNALSPRING:
		case DISC_PROTEAN:
		case DISC_REFLECTIONS:
		case DISC_THEBANSORCERY:
		case DISC_SEPARATION: */
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
		send_to_char ("#eHuh??#n\n\r", ch);
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
		send_to_char ("#eHuh??#n\n\r", ch);
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

	if ((!IS_WEREWOLF(ch) && !IS_VAMPIRE(ch)) || ((IS_VAMPIRE(ch) && get_disc (ch, DISC_PROTEAN) < 1 ) && ( IS_VAMPIRE(ch) && get_disc (ch, DISC_REFLECTIONS) < 4 )))
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if (arg[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char( "#gSyntax#G:#g Short <form> <description>\n\r", ch);
		send_to_char( "#gType #Gshort show all #gto see current.#n\n\r", ch );
		return;
	}

	if (!str_cmp( arg, "urhan")) 
	{
		smash_tilde( argument );
		free_string( ch->pcdata->urhandescshort );
		ch->pcdata->urhandescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (!str_cmp( arg, "gauru") || !str_cmp(arg, "bestial"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->gaurudescshort );
		ch->pcdata->gaurudescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (!str_cmp( arg, "urshul"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->urshuldescshort );
		ch->pcdata->urshuldescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (!str_cmp( arg, "dalu"))
	{
		smash_tilde( argument );
		free_string( ch->pcdata->daludescshort );
		ch->pcdata->daludescshort = str_dup( arg2 );
		send_to_char( "Set.\n\r", ch );
	}

	if (IS_WEREWOLF(ch))
	{
	sprintf( buf, "#gDalu short description#G:#g %s#n\n\r#gUrhan short description#G:#g %s#n\n\r#gUrshul short description#G:#g %s#n\n\r#gGauru short description#G:#g %s#n\n\r", ch->pcdata->daludescshort, ch->pcdata->urhandescshort, ch->pcdata->urshuldescshort, ch->pcdata->gaurudescshort );
	send_to_char( buf, ch );
	}
	else if (IS_WEREWOLF(ch) )
	{
	sprintf( buf, "#gUrhan short description#G:#g %s#n\n\r",
		ch->pcdata->urhandescshort);
	send_to_char( buf, ch );
	sprintf( buf, "#gUrshul short description#G:#g %s#n\n\r",
		ch->pcdata->urshuldescshort);
	send_to_char( buf, ch );
	sprintf( buf, "#gDalu short description#G:#g %s#n\n\r",
		ch->pcdata->daludescshort);
	send_to_char( buf, ch );
	sprintf( buf, "#gGauru short description#G:#g %s#n\n\r",
		ch->pcdata->gaurudescshort);
	send_to_char( buf, ch );
	}
	return;
}

