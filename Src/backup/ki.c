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
