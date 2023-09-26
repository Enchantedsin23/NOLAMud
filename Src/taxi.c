/* Taxi code by Merry */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include "merc.h"
#include <pthread.h>


void do_taxi (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))	return;

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_HEDGE)) 
			{
		send_to_char("You cannot hail taxis from inside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_INSIDE))
			{
		send_to_char("You cannot hail taxis from indoors.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_WATER_SWIM)) 
			{
		send_to_char("You cannot hail taxis from underwater.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_WATER_NOSWIM)) 
			{
		send_to_char("You cannot hail taxis from underwater.\n\r",
				ch);
		return;
	}

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_OOC))
			{
		send_to_char("You cannot hail taxis from OOC areas.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && ch->pounds < 10 ) 
	    {
            send_to_char("You cannot afford to pay for a taxi.\n\r", ch);
	        return;
		}
	
	if (arg[0] == '\0')
	{
      send_to_char ("#c(keyword)   #wRegion#n              Destination #w(##)#n\n\r", ch);
      send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("#c(hyde)#w      Hyde Park#n      Wellington Arch #w(01)#n\n\r", ch);
      send_to_char ("                                 Lido Cafe #w(02)#n\n\r", ch);
      send_to_char ("                             Riding manege #w(03)#n\n\r", ch);
      send_to_char ("#c(victoria)  #wVictoria#n        Pavilion Hotel #w(01)#n\n\r", ch);
      send_to_char ("                           Black Friar Pub #w(02)#n\n\r", ch);
      send_to_char ("#c(cannon)    #wCannon St.#n     Tower of London #w(01)#n\n\r", ch);
      send_to_char ("                               All Hallows #w(02)#n\n\r", ch);
      send_to_char ("                            Trinity Garden #w(03)#n\n\r", ch);
      send_to_char ("#c(southwark) #wSouthwark#n           Cable Club #w(01)#n\n\r", ch);
      send_to_char ("                             Globe Theater #w(02)#n\n\r", ch);
      send_to_char ("#c(central)   #wCentral#n       Asda Supercenter #w(01)#n\n\r", ch);
      send_to_char ("#c(bethnal)   #wBethnal#n    Viajante Restaurant #w(01)#n\n\r", ch);
      send_to_char ("#c(north)     #wNorth#n     King's Cross Station #w(01)#n\n\r", ch);
      send_to_char ("                            The Harrow pub #w(02)#n\n\r", ch);
      send_to_char ("                              Deeds Street #w(03)#n\n\r", ch);
      send_to_char ("#c(brixton)   #wBrixton#n              Geek Chic #w(01)#n\n\r", ch);
      send_to_char ("                             Ritzy Theater #w(02)#n\n\r", ch);
      send_to_char ("                        Post Office Bakery #w(03)#n\n\r", ch);
      send_to_char ("#c(ludgate)   #wLudgate#n   St. Paul's Cathedral #w(01)#n\n\r", ch);
      send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("Syntax: taxi <region> <destination ##>\n\r", ch);;
		return;
	}

	else if (!str_cmp (arg, "hyde"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20001);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		else if (!str_cmp (arg2, "02") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20007);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "03") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20016);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Hyde Park destinations are Wellington Arch (01), Lido Cafe (02), and the riding manege (03).", ch);
			return;
		}
	}

	else if (!str_cmp (arg, "victoria"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20024);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "02") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20029);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Victoria Embankment destinations include the Pavilion Hotel (04) and Black Friar pub (05).", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "cannon"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20038);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "02") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20038);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "03") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20040);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Cannon Street destinations include the Tower of London (06), All Hallows (07), and Trinity Gardens (08).", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "southwark"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20062);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "02") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20018);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Southwark destinations include Cable Club (09) and the Globe Theater (10).", ch);
			return;
		}
		
	}

	else if (!str_cmp (arg, "central"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20063);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Southwark destinations include Cable Club (09) and the Globe Theater (10).", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "bethnal"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20052);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Bethnal Green destinations include Viajante Restaurant (12).", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "north"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20078);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "02") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20076);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "03") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20131);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid North London destinations include King's Cross Station (13), The Harrow (14), Deeds Street (15).", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "brixton"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20058);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "02") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20058);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else if (!str_cmp (arg2, "03") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20124);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Brixton destinations include Geek Chic (16), The Ritzy Theater (17), Post Office Bakery (18).", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "ludgate"))
	{
		if (!str_cmp (arg2, "01") )
		{
	        act("#c$n hails a cab and slips into the back seat. The cab pulls off into traffic.#n", ch, NULL, NULL, TO_ROOM);
	        transport_char(ch, 20047);
			act("#cThe cab navigates traffic and pulls up to the curb at your destination. You shuffle your #C10#c pound fare to the driver and step out.#n", ch, NULL, NULL, TO_CHAR);
	        act("#cA cab pulls up on the curb and $n steps out of the back seat.#n", ch, NULL, NULL, TO_ROOM);
			ch->pounds -= 10;
	        do_look(ch, "");
			return;
		}
		
		else
		{
			send_to_char ("Valid Ludgate destinations include St. Paul's Cathedral (12).", ch);
			return;
		}
		
	}
	
else
      send_to_char ("#c(keyword)   #wRegion#n              Destination #w(##)#n\n\r", ch);
      send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("#c(hyde)#w      Hyde Park#n      Wellington Arch #w(01)#n\n\r", ch);
      send_to_char ("                                 Lido Cafe #w(02)#n\n\r", ch);
      send_to_char ("                             Riding manege #w(03)#n\n\r", ch);
      send_to_char ("#c(victoria)  #wVictoria#n        Pavilion Hotel #w(01)#n\n\r", ch);
      send_to_char ("                           Black Friar Pub #w(02)#n\n\r", ch);
      send_to_char ("#c(cannon)    #wCannon St.#n     Tower of London #w(01)#n\n\r", ch);
      send_to_char ("                               All Hallows #w(02)#n\n\r", ch);
      send_to_char ("                            Trinity Garden #w(03)#n\n\r", ch);
      send_to_char ("#c(southwark) #wSouthwark#n           Cable Club #w(01)#n\n\r", ch);
      send_to_char ("                             Globe Theater #w(02)#n\n\r", ch);
      send_to_char ("#c(central)   #wCentral#n       Asda Supercenter #w(01)#n\n\r", ch);
      send_to_char ("#c(bethnal)   #wBethnal#n    Viajante Restaurant #w(01)#n\n\r", ch);
      send_to_char ("#c(north)     #wNorth#n     King's Cross Station #w(01)#n\n\r", ch);
      send_to_char ("                            The Harrow pub #w(02)#n\n\r", ch);
      send_to_char ("                              Deeds Street #w(03)#n\n\r", ch);
      send_to_char ("#c(brixton)   #wBrixton#n              Geek Chic #w(01)#n\n\r", ch);
      send_to_char ("                             Ritzy Theater #w(02)#n\n\r", ch);
      send_to_char ("                        Post Office Bakery #w(03)#n\n\r", ch);
      send_to_char ("#c(ludgate)   #wLudgate#n   St. Paul's Cathedral #w(01)#n\n\r", ch);
      send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("Syntax: taxi <region> <destination ##>\n\r", ch);;

	return;
}

