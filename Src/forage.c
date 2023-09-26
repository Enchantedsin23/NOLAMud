/* Hedgefruit foraging code by Merry/Leanna */
 
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


void do_forage (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int foragesuccess;
	argument = one_argument (argument, arg);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))	return;
	
	if ((!IS_CHANGELING(ch)) & (!IS_ENSORCELLED(ch))) // Only Changelings can forage
			{
		send_to_char("Only Changelings can forage for Hedgefruit.\n\r", ch);
		return;
	}

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_CITY)) // Cannot forage in the city
			{
		send_to_char("You cannot forage for Hedgefruit outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_INSIDE)) // Cannot forage indoors
			{
		send_to_char("You cannot forage for Hedgefruit outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_WATER_SWIM)) // Cannot forage in the water
			{
		send_to_char("You cannot forage for Hedgefruit outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_WATER_NOSWIM)) // Cannot forage in the water
			{
		send_to_char("You cannot forage for Hedgefruit outside the Hedge.\n\r",
				ch);
		return;
	}

	if (!IS_NPC (ch) && (ch->in_room->sector_type == SECT_OOC)) // Cannot forage in OOC rooms
			{
		send_to_char("You cannot forage for Hedgefruit outside the Hedge.\n\r",
				ch);
		return;
	}
	
	if (arg[0] == '\0')
	{
      send_to_char ("Fruits                                    Herbs\n\r", ch);
	  send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("blushberry                               ertwen\n\r", ch);
      send_to_char ("bloodbane                            murmurleaf\n\r", ch);
	  send_to_char ("amaranthine                                    \n\r", ch);
	  send_to_char ("heartberry                                    \n\r", ch);
      send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("Syntax: forage <fruit/herb> <type>#n\n\r", ch);;
		return;
	}
	
	if (IS_NPC(ch) || (ch->abilities[PHYSICAL][TAL_SURVIVAL] < 1)) // Must have Survival at 1 or higher
			{
		send_to_char("You cannot forage for Hedgefruit without Survival.\n\r",
				ch);
		return;
	}

	// Fruits
	// Blushberry (vnum: 28) -- Heal 1 bashing, +1 glamour (Flags: THROWN) -- Large pink cherries with blood-red juice.
	// Heartberry (vnum: 44) -- Regain 1 glamour (Flags: MAGIC) -- A small, golden heart-shaped berry with exterior seeds.
    // Dream-a-Drupe (vnum: 29) -- Heal 1 lethal (Flags: HUM) -- Purple nectarines. Mildly intoxicating.
    // Amaranthine -- Heal 1 agg (Flags: BLESS) -- Like a small red eggplant.
	
	// Herbs
    // Ertwen -- Heal 2 lethal (Flags: ANTI_EVIL) -- Pods each containing about half-a-dozen large, mealy peas.
    // Murmurleaf -- Heal 2 bashing (Flags: ANTI_GOOD) -- Pale, delicate flowers with curled leaves.
	
	if (ch->pcdata->foragetime > 0)
	{
		if (ch->pcdata->foragetime > 1)
			sprintf (buf, "You cannot forage again so soon.#n\n\r");
		else
			sprintf (buf, "One more second!\n\r");
		send_to_char (buf, ch);
		return;
	}
	else if (!str_cmp (arg, "fruit"))
	{
		if (!str_cmp (arg2, "blushberry") )
		{
			foragesuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cYou search through the high, tangled layers of branches for a Blushberry.\n\r", ch);

			if (foragesuccess < 1)
			{
		    act( "$n forages for fruits and comes up empty handed. #R(failure)#n",  ch, NULL, NULL, TO_ROOM );
            act( "You forage for fruits and come up empty handed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else
			{
			sprintf (buf, "%s pulls a large pink cherry down from the tangled branches overhead. #G(success)#n\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			send_to_char(buf, ch);
			obj_to_char( create_object(get_obj_index( 28 ), 1), ch );
			}
			ch->pcdata->foragetime = 21600; // 43200 for two times a day
			return;
		}
		else if (!str_cmp (arg2, "dream") )
		{
			foragesuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cYou trawl through the Hedge, seeking out the tangled trees that bear Dream-a-Drupe.#n\n\r", ch);

			if (foragesuccess < 2)
			{
		    act( "$n forages for fruits and comes up empty handed. #R(failure)#n",  ch, NULL, NULL, TO_ROOM );
            act( "You forage for fruits and come up empty handed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else
			{
			sprintf (buf, "%s plucks a small, purplish nectarine from a branch. #G(success)#n\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			send_to_char(buf, ch);
			obj_to_char( create_object(get_obj_index( 29 ), 1), ch );
			}
			ch->pcdata->foragetime = 21600; // 43200 for two times a day
			return;
		}
		
		else if (!str_cmp (arg2, "amaranthine") )
		{
			foragesuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cYou trawl the hedge in search of swampy, murk-riddled patches that grow Amaranthine.#n\n\r", ch);

			if (foragesuccess < 4)
			{
		    act( "$n forages for fruits and comes up empty handed. #R(failure)#n",  ch, NULL, NULL, TO_ROOM );
            act( "You forage for fruits and come up empty handed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else
			{
			sprintf (buf, "%s gently frees a palm-sized, deeply red eggplant from a murky bush. #G(success)#n\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			send_to_char(buf, ch);
			obj_to_char( create_object(get_obj_index( 30 ), 1), ch );
			}
			ch->pcdata->foragetime = 21600; // 43200 for two times a day
			return;
		}
		
		else if (!str_cmp (arg2, "heartberry") )
		{
			foragesuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cYou search through low-crowding bushes for a Heartberry.#n\n\r", ch);

			if (foragesuccess < 1)
			{
		    act( "$n forages for fruits and comes up empty handed. #R(failure)#n",  ch, NULL, NULL, TO_ROOM );
            act( "You forage for fruits and come up empty handed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else
			{
			sprintf (buf, "%s plucks a small, golden-colored berry from a low-crowding bush. #G(success)#n\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			send_to_char(buf, ch);
			obj_to_char( create_object(get_obj_index( 44 ), 1), ch );
			}
			ch->pcdata->foragetime = 21600; // 43200 for two times a day
			return;
		}
		
		else
		{
			send_to_char ("You can forage for Blushberry, Dream-a-Drupe #c(dream)#n, Heartberry, Amaranthine fruits.", ch);
			return;
		}
	}

	else if (!str_cmp (arg, "herb"))
	{
		if (!str_cmp (arg2, "ertwen") )
		{
			foragesuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cYou pick through the dry upper layers of foliage, looking for ertwen leaves.#n\n\r", ch);

			if (foragesuccess < 3)
			{
		    act( "$n forages for herbs and comes up empty handed. #R(failure)#n",  ch, NULL, NULL, TO_ROOM );
            act( "You forage for herbs and come up empty handed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else
			{
			sprintf (buf, "%s plucks a mealy pea-pod from a low, jagged bramble. #G(success)#n\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			send_to_char(buf, ch);
			obj_to_char( create_object(get_obj_index( 31 ), 1), ch );
			}
			ch->pcdata->foragetime = 21600; // 43200 for two times a day
			return;
		}
		
		else if (!str_cmp (arg2, "murmurleaf") )
		{
			foragesuccess = diceroll (ch, ATTRIB_WIT, PHYSICAL, TAL_SURVIVAL, 8);
			send_to_char ("#cYou case the Hedge looking for frozen water, locate an icy puddle and search for murmurleaf.#n\n\r", ch);

			if (foragesuccess < 3)
			{
		    act( "$n forages for herbs and comes up empty handed. #R(failure)#n",  ch, NULL, NULL, TO_ROOM );
            act( "You forage for herbs and come up empty handed. #R(failure)#n", ch, NULL, NULL, TO_CHAR );
			}
			else
			{
			sprintf (buf, "%s pinches a pale, delicate flower from the edge of an icy puddle. #G(success)#n\n\r", ch->name);
			act (buf, ch, NULL, NULL, TO_NOTVICT);
			send_to_char(buf, ch);
			obj_to_char( create_object(get_obj_index( 32 ), 1), ch );
			}
			ch->pcdata->foragetime = 21600; // 43200 for two times a day
			return;
		}
		
		else
		{
			send_to_char ("You may forage for Ertwen and Murmurleaf herbs.", ch);
			return;
		}
		
	}

else
      send_to_char ("Fruits                                    Herbs\n\r", ch);
	  send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("blushberry                               ertwen\n\r", ch);
      send_to_char ("bloodbane                            murmurleaf\n\r", ch);
	  send_to_char ("amaranthine                                    \n\r", ch);
	  send_to_char ("heartberry                                    \n\r", ch);
      send_to_char ("#c-----------------------------------------------#n\n\r", ch);
      send_to_char ("Syntax: forage <fruit/herb> <type>#n\n\r", ch);;

	return;
}

// Allow the eating of fruit

void do_eat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	send_to_char( "Eat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
	send_to_char( "You do not have that item.\n\r", ch );
	return;
    }

	else if (( obj->item_type != ITEM_FRUIT ) & ( obj->item_type != ITEM_FOOD ))
	{
		send_to_char( "That's not edible.\n\r", ch );
		return;
	}

    act( "$n eats $p.",  ch, obj, NULL, TO_ROOM );
    act( "You eat $p.", ch, obj, NULL, TO_CHAR );

	// Glamour regain fruit, magic flag
	if (IS_SET (obj->extra_flags, ITEM_MAGIC))
	{
		if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
		{
		act ("#cYou feel a head-rush of glamour from $p.#n", ch, obj, NULL, TO_CHAR);
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
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	// Bashing healing fruit, THROWN flag
	if (IS_SET (obj->extra_flags, ITEM_THROWN))
	{
		if ((ch->pcdata->bashingdamage) > 0)
		{
		act ("#cYour bruises heal as $p saps the aches and pains out of your body.#n", ch, obj, NULL, TO_CHAR);
		ch->pcdata->bashingdamage--;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	// Lethal healing fruit, HUM flag
	if (IS_SET (obj->extra_flags, ITEM_HUM))
	{
		if ((ch->pcdata->lethaldamage) > 0)
		{
		act ("#cYour wounds knit and heal as $p is ingested.#n", ch, obj, NULL, TO_CHAR);
		ch->pcdata->lethaldamage--;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	// Agg healing fruit, BLESS flag
	if (IS_SET (obj->extra_flags, ITEM_BLESS))
	{
		if ((ch->pcdata->aggdamage) > 0)
		{
		act ("#cA potent feeling of warmth and well-being courses through you as $p digests.#n", ch, obj, NULL, TO_CHAR);
		ch->pcdata->aggdamage--;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	// +2 bashing healing fruit, ANTI_GOOD flag
	if (IS_SET (obj->extra_flags, ITEM_ANTI_GOOD))
	{
		if ((ch->pcdata->bashingdamage) > 0)
		{
		act ("#cGlamour weaves through your body, soothing bruises and wounds from the inside as $p digests.#n", ch, obj, NULL, TO_CHAR);
		ch->pcdata->bashingdamage--;
		ch->pcdata->bashingdamage--;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}

	// +2 lethal healing fruit, ANTI_EVIL flag
	if (IS_SET (obj->extra_flags, ITEM_ANTI_EVIL))
	{
		if ((ch->pcdata->lethaldamage) > 0)
		{
		act ("#cGlamour weaves through your body, knitting wounds from the inside as $p digests.#n", ch, obj, NULL, TO_CHAR);
		ch->pcdata->lethaldamage--;
		ch->pcdata->lethaldamage--;
		}
		if (obj != NULL) extract_obj( obj );
		return;
	}
	
	
    if (obj != NULL) extract_obj( obj );
    return;
}
