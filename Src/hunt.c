/* Glamour hunt code by Merry/Leanna */
 
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


void do_hunt (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int huntsuccess;
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))	return;
	
	if ((!IS_CHANGELING(ch)) & (!IS_ENSORCELLED(ch))) // Only Changelings can hunt
			{
		send_to_char("Only Changelings can hunt for Glamour.\n\r", ch);
		return;
	}

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_HEDGE)) // Cannot hunt in the Hedge
			{
		send_to_char("You cannot hunt for Glamour where no mortals are present.\n\r",
				ch);
		return;
	}

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_OOC)) // Cannot hunt in OOC rooms
			{
		send_to_char("You cannot hunt for Glamour in OOC rooms.\n\r",
				ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
      send_to_char ("#nGlamour Type                             Method\n\r", ch);
      send_to_char ("#c-----------------------------------------------\n\r", ch);
      send_to_char ("#nanger                          fight, instigate\n\r", ch);
      send_to_char ("#nsorrow                       ruminate, gaslight\n\r", ch);
	  send_to_char ("#nlust                             seduce, voyeur\n\r", ch);
	  send_to_char ("#nfear                        startle, intimidate\n\r", ch);
	  send_to_char ("#njoy                            inspire, perform\n\r", ch);
      send_to_char ("#nmerit                                   harvest\n\r", ch);
      send_to_char ("#c-----------------------------------------------\n\r", ch);
      send_to_char ("#nSyntax: hunt <type> <method>\n\r", ch);;
		return;
	}
	
	if (ch->pcdata->hunttime > 0)
	{
		if (ch->pcdata->hunttime > 1)
			sprintf (buf, "You cannot hunt again so soon.\n\r");
		else
			sprintf (buf, "One more second!\n\r");
		send_to_char (buf, ch);
		return;
	}
	else if (!str_cmp (arg, "anger"))
	{
		if (!str_cmp (arg2, "fight") )
		{
			huntsuccess = diceroll (ch, ATTRIB_STR, PHYSICAL, TAL_BRAWL, 8);
			send_to_char ("#cYou search the city for a person capable of putting up a fight.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "You fail to start a fight with any of the peace-loving beatniks you come across. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange a few full-body shoves with a lanky loudmouth before he leaves off. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange a couple punches with a drunken loudmouth. You both leave angry and bruised. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
					ch->pcdata->bashingdamage++;
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange a couple punches with a drunken loudmouth. You both leave angry and bruised. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
					ch->pcdata->bashingdamage++;
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get into a full-scale brawl with a man who doesn't like the cut of your jib. You both leave angry and bruised. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
					ch->pcdata->bashingdamage++;
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get involved in a full-scale bar brawl with angry hooligans over the state of English Football. Everybody leaves angry and battered. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
					ch->pcdata->bashingdamage++;
			}
			else
			{
			act( "You fail to start a fight with any of the peace-loving beatniks you come across. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		else if (!str_cmp (arg2, "instigate") )
		{
			huntsuccess = diceroll (ch, ATTRIB_MAN, SOCIAL, SKI_EXPRESSION, 8);
			send_to_char ("#cYou find a shout out an insult to somebody walking down the sidewalk.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "They ignore you, and you fail to reap any glamour. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim angrily screams back and infers that your mother is a prostitute. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim angrily screams back, calling out, \"Fucking knob!\" from across the street. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim angrily screams back a whole slough of insults. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim angrily screams back a whole slough of insults. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim angrily screams back a whole slough of insults. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}

		else
		{
			send_to_char ("Methods of gaining anger glamour include: fight, and instigate.", ch);
			return;
		}
	}

	else if (!str_cmp (arg, "sorrow"))
	{
		if (!str_cmp (arg2, "ruminate") )
		{
			huntsuccess = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_EMPATHY, 8);
			send_to_char ("#cDepression seeks out depression; you approach a sad-looking man at the pub.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "Sharing your woes somehow manages to cheer your victim up. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange stories and let your victim vent his sorrows. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange stories and let your victim vent his sorrows. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange stories and let your victim vent his sorrows. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You exchange stories and let your victim vent his sorrows. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "The conversation leads through your victim's many woes and sorrows; you drain his suicidal feelings into yourself. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else if (!str_cmp (arg2, "gaslight") )
		{
			huntsuccess = diceroll (ch, ATTRIB_MAN, SOCIAL, SKI_SUBTERFUGE, 8);
			send_to_char ("#cYou take a seat next to the first uncomfortable-looking woman you see at a local pub.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "Your victim laughs off your attempts. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You talk your victim into believing she may have a mental health problem. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You talk your victim into believing she may have a mental health problem. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You talk your victim into believing she may have a mental health problem. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You talk your victim into believing she may have a mental health problem. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You talk your victim into believing she may have a serious mental health problem. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else
		{
			send_to_char ("Methods of gaining sorrow glamour include: ruminate, gaslight.", ch);
			return;
		}
		
	}

	else if (!str_cmp (arg, "lust"))
	{
		if (!str_cmp (arg2, "seduce") )
		{
			huntsuccess = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_PERSUASION, 8);
			send_to_char ("#cYou bring a drunken pub-goer and a pack of condoms home with you.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "Your victim loses interest before you can get them into bed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get your victim into bed, but they aren't especially enthused. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get your victim into bed and spend a half hour tumbling around in the sheets. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get your victim into bed and spend an hour tumbling around in the sheets. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get your victim into bed and explore their body, bringing them to orgasm. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You get your victim into bed and time seems to stand still; writhing, moaning, trembling lusts are absorbed from their body. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else if (!str_cmp (arg2, "voyeur") )
		{
			huntsuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_STEALTH, 8);
			send_to_char ("#cYou grab your binoculars and look for a window with an attractive victim.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "Your victim feels eyes upon them and pulls the window curtains closed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim sits down at their computer and watches a brief pornographic video. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim undresses and steps into the shower. You get a glimpse of a sex toy before the doors close. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim cooks supper for a guest in the nude; you watch them glide around the kitchen in solely an apron. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "Your victim settles onto the sofa and slips their hand down their jeans, coming to heavy orgasm before your eyes. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "An acquaintance stops by your victim's flat. You watch as they tear each others' clothes away and rut on the floor. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else
		{
			send_to_char ("Methods of gaining lust glamour include: seduce, voyeur.", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "fear"))
	{
		if (!str_cmp (arg2, "startle") )
		{
			huntsuccess = diceroll (ch, ATTRIB_MAN, PHYSICAL, TAL_STEALTH, 8);
			send_to_char ("#cYou wait in a dark alley for somebody to pass.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "Everyone that passes is in a group or walking with a friend. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You leap out and comically shout, \"BOO!\". Your victim is more irritated than startled. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You leap out and shout, \"FUCK YOU!\" Your victim takes off running down the sidewalk. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You slip your hand into your coat and jump out; your victim believes you have a weapon and bolts in fear. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You reach out and make a grab for your victim; they scream and run away, terrified. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You reach out quick and grab your victim, slamming them into the alley wall. A stream of urine spills down their leg before they take off running. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else if (!str_cmp (arg2, "intimidate") )
		{
			huntsuccess = diceroll (ch, ATTRIB_MAN, SOCIAL, SKI_INTIMIDATION, 8);
			send_to_char ("#cYou case a local pub looking for an appropriate scenario to butt in on.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "The pub is empty save for the bartender, and he's busy watching the tele. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You notice a man staring at you from down the bar. You meet his eye until he looks away in discomfort. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You notice a man looking at you from down the bar. You stare him down until he's so uncomfortable that he gets up and leaves. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You interject yourself between group of harassing chavs and a teenager. When you tell them to fuck off, they most certainly do. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You follow someone down the street, making yourself visible as you match their stride. They break after half a block and take off running. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You inform the asshole hitting on every woman in the pub that you'll be wearing his skin as a suit if he doesn't take off. He believes you, and leaves to change his knickers. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else
		{
			send_to_char ("Methods of gaining fear glamour include: startle, intimidate.", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "joy"))
	{
		if (!str_cmp (arg2, "inspire") )
		{
			huntsuccess = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_EMPATHY, 8);
			send_to_char ("#cYou search the city looking for someone or something to to inspire.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "You try and cheer up and old lady on the sidewalk. She hits you with her umbrella instead. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You notice a chubby kid being bullied and stop on the sidewalk to talk to him. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You stop by Speaker's Corner and lecture on the outdated premise of a Constitutional Monarchy. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You stop by Speaker's Corner and lecture on the outdated premise of a Constitutional Monarchy. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You take a shift at a Suicide Hotline and convince several desperate people of the preciousness and beauty of life. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You give a rousing, engaging philosophical speech to a rapt afternoon crowd at Speaker's Corner. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else if (!str_cmp (arg2, "perform") )
		{
			huntsuccess = diceroll (ch, ATTRIB_CHA, SOCIAL, SKI_EXPRESSION, 8);
			send_to_char ("#cYou find a likely target and get into a pushing match.#n\n\r", ch);

			if (huntsuccess == 0)
			{
            act( "Your attempt to get a smile out of a performance falls completely flat. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else if (huntsuccess == 1)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You busk on the sidewalk, attracting enough positive attention for a quid or two. #G(minor success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 2)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You busk on the sidewalk, attracting a moderate crowd of smiling onlookers. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 3)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You busk on the sidewalk, attracting a sizable crowd of smiling onlookers. #G(moderate success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if (huntsuccess == 4)
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You gather a sizable crowd and perform, leaving every witness with a feeling of happiness and well-being. #G(high success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			else if ((huntsuccess = 5) & (huntsuccess > 5))
			{
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		    		{
		    		act( "You produce a fantastic display of artistic performance, leaving all who witnessed it deeply touched. #G(exceptional success)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    		}
			}
			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		
		else
		{
			send_to_char ("Methods of gaining joy glamour include: inspire, perform.", ch);
			return;
		}
		
	}
	
	else if (!str_cmp (arg, "merit"))
	{
		if (!str_cmp (arg2, "harvest") )
		{
			// Has Harvest 1
			if(has_merit(ch, "harvest 1"))
			{
                 if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		         {
		    		act( "#cYou tap into your established, familiar source of Glamour. #C(Harvest 1)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
		    	 }
			}
			// Has Harvest 2
			if(has_merit(ch, "harvest 2"))
			{
                 if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		         {
		    		act( "#cYou tap into your established, familiar source of Glamour. #C(Harvest 2)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    	 }
			}
			// Has Harvest 3
			if(has_merit(ch, "harvest 3"))
			{
                 if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		         {
		    		act( "#cYou tap into your established, familiar source of Glamour. #C(Harvest 3)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    	 }
			}
			// Has Harvest 4
			if(has_merit(ch, "harvest 4"))
			{
                 if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		         {
		    		act( "#cYou tap into your established, familiar source of Glamour. #C(Harvest 4)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    	 }
			}
			// Has Harvest 5
			if(has_merit(ch, "harvest 5"))
			{
                 if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		         {
		    		act( "#cYou tap into your established, familiar source of Glamour. #C(Harvest 5)#n", ch, NULL, NULL, TO_CHAR );
		    		ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
					}
					if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
					{
					ch->blood[BLOOD_CURRENT]++;
		    	 }
			}

			ch->pcdata->hunttime = 21600; // 86400 for once a day
			return;
		}
		else
		{
			send_to_char ("Methods of gaining Glamour through Merits include: Harvest", ch);
			return;
		}
		
	}
	
else
      send_to_char ("#nGlamour Type                             Method\n\r", ch);
      send_to_char ("#c-----------------------------------------------\n\r", ch);
      send_to_char ("#nanger                          fight, instigate\n\r", ch);
      send_to_char ("#nsorrow                       ruminate, gaslight\n\r", ch);
	  send_to_char ("#nlust                             seduce, voyeur\n\r", ch);
	  send_to_char ("#nfear                        startle, intimidate\n\r", ch);
	  send_to_char ("#njoy                            inspire, perform\n\r", ch);
      send_to_char ("#nmerit                                   harvest\n\r", ch);
      send_to_char ("#c-----------------------------------------------\n\r", ch);
      send_to_char ("#nSyntax: hunt <type> <method>\n\r", ch);;

	return;
}
