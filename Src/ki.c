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

bool play_yet (CHAR_DATA * ch)
{
	char buf[MAX_STRING_LENGTH];
	char *strtime;
	int current_date;

	strtime = ctime (&current_time);
	strtime[strlen (strtime) - 1] = '\0';

	if (IS_NPC (ch))
		return TRUE;

	current_date = date_to_int (strtime);
	if (current_date >= ch->pcdata->denydate)
		return TRUE;
	else
	{
		sprintf (buf, "You are denied for %d more days.\n\r", ch->pcdata->denydate - current_date);
		send_to_char (buf, ch);
	}
	return FALSE;
}

int date_to_int (char *argument)
{
	char buf[MAX_STRING_LENGTH];
	int day_count = 0;

	if (argument[0] == '\0')
		return 0;

	argument = one_argument (argument, buf);

	if (buf[0] == '\0')
		return 0;

	argument = one_argument (argument, buf);

	if (buf[0] == '\0')
		return 0;

	if (!str_cmp (buf, "Jan"))
		day_count = 0;
	else if (!str_cmp (buf, "Feb"))
		day_count = 31;
	else if (!str_cmp (buf, "Mar"))
		day_count = 59;
	else if (!str_cmp (buf, "Apr"))
		day_count = 90;
	else if (!str_cmp (buf, "May"))
		day_count = 120;
	else if (!str_cmp (buf, "Jun"))
		day_count = 151;
	else if (!str_cmp (buf, "Jul"))
		day_count = 181;
	else if (!str_cmp (buf, "Aug"))
		day_count = 212;
	else if (!str_cmp (buf, "Sep"))
		day_count = 242;
	else if (!str_cmp (buf, "Oct"))
		day_count = 273;
	else if (!str_cmp (buf, "Nov"))
		day_count = 304;
	else if (!str_cmp (buf, "Dec"))
		day_count = 334;

	argument = one_argument (argument, buf);

	if (buf[0] == '\0')
		return 0;
	if (!is_number (buf) || atoi (buf) < 1)
		return 0;

	day_count += atoi (buf);

	argument = one_argument (argument, buf);
	argument = one_argument (argument, buf);
	if (buf[0] == '\0')
		return 0;
	if (!is_number (buf) || atoi (buf) < 1)
		return 0;

	day_count += (365 * atoi (buf));

	return day_count;
}

/* This command needs renaming to prevent crossover with Harvest code */

/*void do_consent( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if ( strlen(ch->pcdata->marriage) < 2 || !IS_EXTRA(ch, EXTRA_MARRIED) )
    {
	send_to_char("But you are not even married!\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if (IS_NPC(victim))
    {
	send_to_char("Not on NPCs.\n\r",ch);
	return;
    }

    if (ch == victim)
    {
	send_to_char("Not on yourself!\n\r",ch);
	return;
    }

    if (strlen(ch->pcdata->marriage) > 1 && 
	str_cmp(ch->pcdata->marriage,victim->name))
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	    send_to_char("But you are married to someone else!\n\r",ch);
	else
	    send_to_char("But you are engaged to someone else!\n\r",ch);
	return;
    }
    if (strlen(victim->pcdata->marriage) > 1 && 
	str_cmp(victim->pcdata->marriage,ch->name))
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	    send_to_char("But they are married to someone else!\n\r",ch);
	else
	    send_to_char("But they are engaged to someone else!\n\r",ch);
	return;
    }
}*/

void do_propose( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (strlen(ch->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	    send_to_char("But you are already married!\n\r",ch);
	else
	    send_to_char("But you are already engaged!\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("Who do you wish to propose marriage to?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (strlen(victim->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	    send_to_char("But they are already married!\n\r",ch);
	else
	    send_to_char("But they are already engaged!\n\r",ch);
	return;
    }

    if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||
	 (ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )
    {
	ch->pcdata->propose = victim;
	act("You propose marriage to $M.",ch,NULL,victim,TO_CHAR);
	act("$n gets down on one knee and proposes to $N.",ch,NULL,victim,TO_NOTVICT);
	act("$n asks you quietly 'Will you marry me?'",ch,NULL,victim,TO_VICT);
	return;
    }
    send_to_char("I don't think that would be a very good idea...\n\r",ch);
    return;
}

void do_accept( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (strlen(ch->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	    send_to_char("But you are already married!\n\r",ch);
	else
	    send_to_char("But you are already engaged!\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("Who's proposal of marriage do you wish to accept?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (strlen(victim->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	    send_to_char("But they are already married!\n\r",ch);
	else
	    send_to_char("But they are already engaged!\n\r",ch);
	return;
    }

    if ( victim->pcdata->propose == NULL || victim->pcdata->propose != ch )
    {
	send_to_char("But they haven't proposed to you!\n\r",ch);
	return;
    }

    if ( (ch->sex == SEX_MALE && victim->sex == SEX_FEMALE) ||
	 (ch->sex == SEX_FEMALE && victim->sex == SEX_MALE) )
    {
	victim->pcdata->propose = NULL;
	ch->pcdata->propose = NULL;
	free_string(victim->pcdata->marriage);
	victim->pcdata->marriage = str_dup( ch->name );
	free_string(ch->pcdata->marriage);
	ch->pcdata->marriage = str_dup( victim->name );
	act("You accept $S offer of marriage.",ch,NULL,victim,TO_CHAR);
	act("$n accepts $N's offer of marriage.",ch,NULL,victim,TO_NOTVICT);
	act("$n accepts your offer of marriage.",ch,NULL,victim,TO_VICT);
	save_char_obj(ch);
	save_char_obj(victim);
	sprintf(buf,"#g%s and %s are now engaged!#n",ch->name,victim->name);
	do_rpose(ch,buf);
	return;
    }
    send_to_char("I don't think that would be a very good idea...\n\r",ch);
    return;
}

void do_breakup( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( IS_NPC(ch) ) return;

    if (strlen(ch->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(ch, EXTRA_MARRIED))
	{
	    send_to_char("You'll have to get divorced.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("But you are not even engaged!\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char("Who do you wish to break up with?\n\r",ch);
	return;
    }

    if ( ( victim = get_char_room(ch, arg) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }

    if (strlen(victim->pcdata->marriage) > 1)
    {
	if (IS_EXTRA(victim, EXTRA_MARRIED))
	{
	    send_to_char("They'll have to get divorced.\n\r",ch);
	    return;
	}
    }
    else
    {
	send_to_char("But they are not even engaged!\n\r",ch);
	return;
    }

    if (!str_cmp(ch->name, victim->pcdata->marriage) &&
	!str_cmp(victim->name, ch->pcdata->marriage))
    {
	free_string(victim->pcdata->marriage);
	victim->pcdata->marriage = str_dup( "" );
	free_string(ch->pcdata->marriage);
	ch->pcdata->marriage = str_dup( "" );
	act("You break off your engagement with $M.",ch,NULL,victim,TO_CHAR);
	act("$n breaks off $n engagement with $N.",ch,NULL,victim,TO_NOTVICT);
	act("$n breaks off $s engagement with you.",ch,NULL,victim,TO_VICT);
	save_char_obj(ch);
	save_char_obj(victim);
	sprintf(buf,"#g%s and %s have severed their engagement!#n",ch->name,victim->name);
	do_rpose(ch,buf);
	return;
    }
    send_to_char("You are not engaged to them.\n\r",ch);
    return;
}

void do_marry( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;
    CHAR_DATA *victim2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: marry <person> <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if ( ( victim2 = get_char_room(ch, arg2) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1) || IS_NPC(victim2))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (!str_cmp(victim1->name, victim2->pcdata->marriage) &&
	!str_cmp(victim2->name, victim1->pcdata->marriage))
    {
	SET_BIT(victim1->extra, EXTRA_MARRIED);
	SET_BIT(victim2->extra, EXTRA_MARRIED);
	save_char_obj(victim1);
	save_char_obj(victim2);
	sprintf(buf,"#g%s and %s are now married!#n",victim1->name,victim2->name);
	do_rpose(ch,buf);
	return;
    }
    send_to_char("But they are not yet engaged!\n\r",ch);
    return;
}

void do_divorce( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim1;
    CHAR_DATA *victim2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
	send_to_char("Syntax: divorce <person> <person>\n\r",ch);
	return;
    }
    if ( ( victim1 = get_char_room(ch, arg1) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if ( ( victim2 = get_char_room(ch, arg2) ) == NULL )
    {
	send_to_char("They are not here.\n\r",ch);
	return;
    }
    if (IS_NPC(victim1) || IS_NPC(victim2))
    {
	send_to_char("Not on NPC's.\n\r",ch);
	return;
    }
    if (!str_cmp(victim1->name, victim2->pcdata->marriage) &&
	!str_cmp(victim2->name, victim1->pcdata->marriage))
    {
	if (!IS_EXTRA(victim1,EXTRA_MARRIED) || !IS_EXTRA(victim2,EXTRA_MARRIED))
	{
	    send_to_char("But they are not married!\n\r",ch);
	    return;
	}
	REMOVE_BIT(victim1->extra, EXTRA_MARRIED);
	REMOVE_BIT(victim2->extra, EXTRA_MARRIED);
	free_string(victim1->pcdata->marriage);
	victim1->pcdata->marriage = str_dup( "" );
	free_string(victim2->pcdata->marriage);
	victim2->pcdata->marriage = str_dup( "" );
	save_char_obj(victim1);
	save_char_obj(victim2);
	sprintf(buf,"#g%s and %s are now divorced!#n",victim1->name,victim2->name);
	do_rpose(ch,buf);
	return;
    }
    send_to_char("But they are not married!\n\r",ch);
    return;
}

void do_scan( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;

    location = ch->in_room;

    send_to_char("#y[#gNorth#y]#n\n\r",ch);
    do_spydirection(ch,"n");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("#y[#gEast#y]#n\n\r",ch);
    do_spydirection(ch,"e");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("#y[#gSouth#y]#n\n\r",ch);
    do_spydirection(ch,"s");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("#y[#gWest#y]#n\n\r",ch);
    do_spydirection(ch,"w");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("#y[#gUp#y]#n\n\r",ch);
    do_spydirection(ch,"u");
    char_from_room(ch);
    char_to_room(ch,location);

    send_to_char("#y[#gDown#y]#n\n\r",ch);
    do_spydirection(ch,"d");
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}


void do_spy( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    char            arg1      [MAX_INPUT_LENGTH];
    int             door;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
	send_to_char("Which direction do you wish to spy?\n\r", ch);
	return;
    }

    if      ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) )
	door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east" ) )
	door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) )
	door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west" ) )
	door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up" ) )
	door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down" ) )
	door = 5;
    else
    {
	send_to_char("You can only spy people north, south, east, west, up or down.\n\r", ch);
	return;
    }

    location = ch->in_room;

    send_to_char("#y[#gClose by#y]#n\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);
    send_to_char("#y[#gAt a moderate distance#y]#n\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);
    send_to_char("#y[#gFar away#y]#n\n\r",ch);
    do_spydirection(ch,arg1);
    send_to_char("\n\r",ch);

    /* Move them back */
    char_from_room(ch);
    char_to_room(ch,location);
    return;
}

void do_spydirection( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA       *pexit;
    CHAR_DATA       *vch;
    CHAR_DATA       *vch_next;
    char            arg      [MAX_INPUT_LENGTH];
    char            buf      [MAX_INPUT_LENGTH];
    int             door;
    bool            is_empty;

    argument = one_argument( argument, arg );

    is_empty = TRUE;

    if ( arg[0] == '\0' )
	return;

    if      ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
    else return;

    if (( pexit = ch->in_room->exit[door]) == NULL || (to_room = pexit->to_room) == NULL)
    {
	send_to_char("#g   A wall blocks your view.#n\n\r",ch);
	return;
    }
    pexit = ch->in_room->exit[door];
    if (IS_SET(pexit->exit_info, EX_CLOSED) )
    {
	send_to_char("#g   The door is closed.#n\n\r",ch);
	return;
    }
    if (IS_SET(world_affects, WORLD_FOG))
    {
	send_to_char("#g   The fog is too thick.#n\n\r",ch);
	return;
    }
    if (room_is_dark(to_room) && !IS_VAMPAFF(ch, VAM_NIGHTSIGHT))
    {
	send_to_char("#g   Too dark to see.#n\n\r",ch);
	return;
    }
    char_from_room(ch);
    char_to_room(ch,to_room);
    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next	= vch->next;
	if ( vch->in_room == NULL )
	    continue;
	if ( vch == ch )
	    continue;
	if ( !can_see(ch,vch) )
	    continue;
	if ( vch->in_room == ch->in_room )
	{
	    if (!IS_NPC(vch) && !IS_AFFECTED(vch,AFF_POLYMORPH))
		sprintf(buf,"#n   %s #n(Player)#n\n\r",vch->name);
	    else if (!IS_NPC(vch) && IS_AFFECTED(vch,AFF_POLYMORPH))
		sprintf(buf,"#n   %s #n(Player)#n\n\r",vch->morph);
	    else if (IS_NPC(vch))
		sprintf(buf,"#n   %s\n\r",capitalize(vch->short_descr));
	    send_to_char(buf,ch);
	    is_empty = FALSE;
	    continue;
	}
    }
    if (is_empty)
    {
	send_to_char("#n   No one in your vicinity.#n\n\r",ch);
	return;
    }
    return;
}

void birth_date( CHAR_DATA *ch, bool is_self )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char mstatus[20];
    char dstatus[20];
    char omin[3];
    char cmin[3];
    char ohour[3];
    char chour[3];
    char oday0[2];
    char cday0[2];
    char oday1[2];
    char cday1[2];
    char omonth[4];
    char cmonth[4];
    char oyear[5];
    char cyear[5];
    char *strtime;
    int oyear2 = 0;
    int cyear2 = 0;
    int omonth2 = 0;
    int cmonth2 = 0;
    int oday2 = 0;
    int cday2 = 0;
    int total = 0;
    int dd = 0;
    int mm = 0;
    int yy = 0;
    bool o_month = FALSE;
    bool c_month = FALSE;

    if (IS_NPC(ch)) return;

    if (is_self)
	strcpy(buf,ch->createtime);
    else
	strcpy(buf,ch->pcdata->conception);
    if (strlen(buf) < 24) return;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    strcpy(buf2,strtime);
    oyear[0] = buf[20];
    oyear[1] = buf[21];
    oyear[2] = buf[22];
    oyear[3] = buf[23];
    oyear[4] = '\0';
    omonth[0] = buf[4];
    omonth[1] = buf[5];
    omonth[2] = buf[6];
    omonth[3] = '\0';
    oday0[0] = buf[8];
    oday0[1] = '\0';
    oday1[0] = buf[9];
    oday1[1] = '\0';
    ohour[0] = buf[11];
    ohour[1] = buf[12];
    ohour[2] = '\0';
    omin[0] = buf[14];
    omin[1] = buf[15];
    omin[2] = '\0';

    cyear[0] = buf2[20];
    cyear[1] = buf2[21];
    cyear[2] = buf2[22];
    cyear[3] = buf2[23];
    cyear[4] = '\0';
    cmonth[0] = buf2[4];
    cmonth[1] = buf2[5];
    cmonth[2] = buf2[6];
    cmonth[3] = '\0';
    cday0[0] = buf2[8];
    cday0[1] = '\0';
    cday1[0] = buf2[9];
    cday1[1] = '\0';
    chour[0] = buf2[11];
    chour[1] = buf2[12];
    chour[2] = '\0';
    cmin[0] = buf2[14];
    cmin[1] = buf2[15];
    cmin[2] = '\0';

    if (!str_cmp(omonth,"Dec")) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Nov") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Oct") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Sep") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Aug") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jul") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jun") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"May") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Apr") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Mar") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Feb") || o_month) {omonth2 += 28;o_month = TRUE;}
    if (!str_cmp(omonth,"Jan") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!o_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (!str_cmp(cmonth,"Dec")) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Nov") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Oct") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Sep") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Aug") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jul") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jun") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"May") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Apr") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Mar") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Feb") || c_month) {cmonth2 += 28;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jan") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!c_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (is_number(oyear)) oyear2 += atoi(oyear);
    if (is_number(cyear)) cyear2 += atoi(cyear);
    if (is_number(oday0)) oday2 += (atoi(oday0) * 10);
    if (is_number(oday1)) oday2 += atoi(oday1);
    if (is_number(cday0)) cday2 += (atoi(cday0) * 10);
    if (is_number(cday1)) cday2 += atoi(cday1);
    total += ((cyear2 - oyear2) * 365);
    total += (cmonth2 - omonth2);
    total += (cday2 - oday2);
    total *= 24; /* Total playing time is now in hours */
    if (is_number(chour) && is_number(ohour))
	total += (atoi(chour) - atoi(ohour));
    total *= 60; /* Total now in minutes */
    if (is_number(cmin) && is_number(omin))
	total += (atoi(cmin) - atoi(omin));
    if (total < 1) total = 0;
	else total /= 12; /* Time now in game days */
    for ( ;; )
    {
	if (total >= 365)
	{
	    total -= 365;
	    yy += 1;
	}
	else if (total >= 30)
	{
	    total -= 30;
	    mm += 1;
	}
	else
	{
	    dd += total;
	    break;
	}
    }
    if (mm == 1) strcpy(mstatus,"month");
	else strcpy(mstatus,"months");
    if (dd == 1) strcpy(dstatus,"day");
	else strcpy(dstatus,"days");
    if (is_self)
    {
	if (!IS_EXTRA(ch, EXTRA_BORN)) yy += 17;
	sprintf(buf,"You have aged %d years, %d %s and %d %s.\n\r",
	    yy,mm,mstatus,dd,dstatus);
	send_to_char(buf,ch);
    }
    else
    {
	if (yy > 0 || (dd > 0 && mm >= 4) || IS_EXTRA(ch, EXTRA_LABOUR))
	{
	    if (!IS_EXTRA(ch, EXTRA_LABOUR)) SET_BIT(ch->extra, EXTRA_LABOUR);
	    sprintf(buf,"#yYou are ready to give birth.#n");
	    send_to_char(buf,ch);
	}
	else if (yy > 0 || mm > 0)
	{
	    sprintf(buf,"You are %d %s and %d %s pregnant.",
	    mm,mstatus,dd,dstatus);
	    send_to_char(buf,ch);
	}
    }
    return;
}

void other_age( CHAR_DATA *ch, int extra, bool is_preg, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char mstatus[20];
    char dstatus[20];
    char omin[3];
    char cmin[3];
    char ohour[3];
    char chour[3];
    char oday0[2];
    char cday0[2];
    char oday1[2];
    char cday1[2];
    char omonth[4];
    char cmonth[4];
    char oyear[5];
    char cyear[5];
    char *strtime;
    int oyear2 = 0;
    int cyear2 = 0;
    int omonth2 = 0;
    int cmonth2 = 0;
    int oday2 = 0;
    int cday2 = 0;
    int total = 0;
    int dd = 0;
    int mm = 0;
    int yy = 0;
    bool o_month = FALSE;
    bool c_month = FALSE;

    strcpy(buf,argument);
    if (strlen(buf) < 24) return;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    strcpy(buf2,strtime);
    oyear[0] = buf[20];
    oyear[1] = buf[21];
    oyear[2] = buf[22];
    oyear[3] = buf[23];
    oyear[4] = '\0';
    omonth[0] = buf[4];
    omonth[1] = buf[5];
    omonth[2] = buf[6];
    omonth[3] = '\0';
    oday0[0] = buf[8];
    oday0[1] = '\0';
    oday1[0] = buf[9];
    oday1[1] = '\0';
    ohour[0] = buf[11];
    ohour[1] = buf[12];
    ohour[2] = '\0';
    omin[0] = buf[14];
    omin[1] = buf[15];
    omin[2] = '\0';

    cyear[0] = buf2[20];
    cyear[1] = buf2[21];
    cyear[2] = buf2[22];
    cyear[3] = buf2[23];
    cyear[4] = '\0';
    cmonth[0] = buf2[4];
    cmonth[1] = buf2[5];
    cmonth[2] = buf2[6];
    cmonth[3] = '\0';
    cday0[0] = buf2[8];
    cday0[1] = '\0';
    cday1[0] = buf2[9];
    cday1[1] = '\0';
    chour[0] = buf2[11];
    chour[1] = buf2[12];
    chour[2] = '\0';
    cmin[0] = buf2[14];
    cmin[1] = buf2[15];
    cmin[2] = '\0';

    if (!str_cmp(omonth,"Dec")) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Nov") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Oct") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Sep") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Aug") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jul") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jun") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"May") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Apr") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Mar") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Feb") || o_month) {omonth2 += 28;o_month = TRUE;}
    if (!str_cmp(omonth,"Jan") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!o_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (!str_cmp(cmonth,"Dec")) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Nov") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Oct") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Sep") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Aug") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jul") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jun") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"May") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Apr") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Mar") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Feb") || c_month) {cmonth2 += 28;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jan") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!c_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return;}
    if (is_number(oyear)) oyear2 += atoi(oyear);
    if (is_number(cyear)) cyear2 += atoi(cyear);
    if (is_number(oday0)) oday2 += (atoi(oday0) * 10);
    if (is_number(oday1)) oday2 += atoi(oday1);
    if (is_number(cday0)) cday2 += (atoi(cday0) * 10);
    if (is_number(cday1)) cday2 += atoi(cday1);
    total += ((cyear2 - oyear2) * 365);
    total += (cmonth2 - omonth2);
    total += (cday2 - oday2);
    total *= 24; /* Total playing time is now in hours */
    if (is_number(chour) && is_number(ohour))
	total += (atoi(chour) - atoi(ohour));
    total *= 60; /* Total now in minutes */
    if (is_number(cmin) && is_number(omin))
	total += (atoi(cmin) - atoi(omin));
    if (total < 1) total = 0;
	else total /= 12; /* Time now in game days */
    for ( ;; )
    {
	if (total >= 365)
	{
	    total -= 365;
	    yy += 1;
	}
	else if (total >= 30)
	{
	    total -= 30;
	    mm += 1;
	}
	else
	{
	    dd += total;
	    break;
	}
    }
    if (mm == 1) strcpy(mstatus,"month");
	else strcpy(mstatus,"months");
    if (dd == 1) strcpy(dstatus,"day");
	else strcpy(dstatus,"days");
    if (!is_preg)
    {
	if (!IS_EXTRA(ch, EXTRA_BORN)) yy += 17;
	sprintf(buf,"Age: %d years, %d %s and %d %s old.\n\r",
	    yy,mm,mstatus,dd,dstatus);
	send_to_char(buf,ch);
    }
    else
    {
	if (yy > 0 || (dd > 0 && mm >= 4) || IS_SET(extra, EXTRA_LABOUR))
	{
	    sprintf(buf,"She is ready to give birth.\n\r");
	    send_to_char(buf,ch);
	}
	else if (yy > 0 || mm > 0)
	{
	    sprintf(buf,"She is %d %s and %d %s pregnant.\n\r",
	    mm,mstatus,dd,dstatus);
	    send_to_char(buf,ch);
	}
    }
    return;
}

int years_old( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char omin[3];
    char cmin[3];
    char ohour[3];
    char chour[3];
    char oday0[2];
    char cday0[2];
    char oday1[2];
    char cday1[2];
    char omonth[4];
    char cmonth[4];
    char oyear[5];
    char cyear[5];
    char *strtime;
    int oyear2 = 0;
    int cyear2 = 0;
    int omonth2 = 0;
    int cmonth2 = 0;
    int oday2 = 0;
    int cday2 = 0;
    int total = 0;
    int yy = 0;
    bool o_month = FALSE;
    bool c_month = FALSE;

    if (IS_NPC(ch)) return 0;

    strcpy(buf,ch->createtime);
    if (strlen(buf) < 24) return 0;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    strcpy(buf2,strtime);
    oyear[0] = buf[20];
    oyear[1] = buf[21];
    oyear[2] = buf[22];
    oyear[3] = buf[23];
    oyear[4] = '\0';
    omonth[0] = buf[4];
    omonth[1] = buf[5];
    omonth[2] = buf[6];
    omonth[3] = '\0';
    oday0[0] = buf[8];
    oday0[1] = '\0';
    oday1[0] = buf[9];
    oday1[1] = '\0';
    ohour[0] = buf[11];
    ohour[1] = buf[12];
    ohour[2] = '\0';
    omin[0] = buf[14];
    omin[1] = buf[15];
    omin[2] = '\0';

    cyear[0] = buf2[20];
    cyear[1] = buf2[21];
    cyear[2] = buf2[22];
    cyear[3] = buf2[23];
    cyear[4] = '\0';
    cmonth[0] = buf2[4];
    cmonth[1] = buf2[5];
    cmonth[2] = buf2[6];
    cmonth[3] = '\0';
    cday0[0] = buf2[8];
    cday0[1] = '\0';
    cday1[0] = buf2[9];
    cday1[1] = '\0';
    chour[0] = buf2[11];
    chour[1] = buf2[12];
    chour[2] = '\0';
    cmin[0] = buf2[14];
    cmin[1] = buf2[15];
    cmin[2] = '\0';

    if (!str_cmp(omonth,"Dec")) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Nov") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Oct") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Sep") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Aug") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jul") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Jun") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"May") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Apr") || o_month) {omonth2 += 30;o_month = TRUE;}
    if (!str_cmp(omonth,"Mar") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!str_cmp(omonth,"Feb") || o_month) {omonth2 += 28;o_month = TRUE;}
    if (!str_cmp(omonth,"Jan") || o_month) {omonth2 += 31;o_month = TRUE;}
    if (!o_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return 0;}
    if (!str_cmp(cmonth,"Dec")) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Nov") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Oct") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Sep") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Aug") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jul") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jun") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"May") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Apr") || c_month) {cmonth2 += 30;c_month = TRUE;}
    if (!str_cmp(cmonth,"Mar") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!str_cmp(cmonth,"Feb") || c_month) {cmonth2 += 28;c_month = TRUE;}
    if (!str_cmp(cmonth,"Jan") || c_month) {cmonth2 += 31;c_month = TRUE;}
    if (!c_month) {send_to_char("Error! Please inform KaVir.\n\r",ch);return 0;}
    if (is_number(oyear)) oyear2 += atoi(oyear);
    if (is_number(cyear)) cyear2 += atoi(cyear);
    if (is_number(oday0)) oday2 += (atoi(oday0) * 10);
    if (is_number(oday1)) oday2 += atoi(oday1);
    if (is_number(cday0)) cday2 += (atoi(cday0) * 10);
    if (is_number(cday1)) cday2 += atoi(cday1);
    total += ((cyear2 - oyear2) * 365);
    total += (cmonth2 - omonth2);
    total += (cday2 - oday2);
    total *= 24; /* Total playing time is now in hours */
    if (is_number(chour) && is_number(ohour))
	total += (atoi(chour) - atoi(ohour));
    total *= 60; /* Total now in minutes */
    if (is_number(cmin) && is_number(omin))
	total += (atoi(cmin) - atoi(omin));
    if (total < 1) total = 0;
	else total /= 12; /* Time now in game days */
    for ( ;; )
    {
	if (total >= 365)
	{
	    total -= 365;
	    yy += 1;
	}
	else break;
    }
    if (!IS_EXTRA(ch, EXTRA_BORN)) yy += 17; /* Starting age for all players */
    return yy;
}

