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

/* Test */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>		// unlink
#include "merc.h"
#include <pthread.h>
#include "recycle.h"

/*
 * Local functions.
 */
void mobile_update args ((void));
void char_update args ((void));
void obj_update args ((void));
void	weather_update	args( ( void ) );
void free_event args( (EVENT_DATA *ev) );
void	msdp_update	args( ( void ) ); /* <--- Add this line */

/* Hint Channel */
void	newbie_update	args( ( void ) );

EVENT_DATA	*events;

#define kill_event \
				if(!ev_next)  {   free_event(ev); return; }  \
				if(ev_last!=NULL) ev_last->next=ev_next;  \
				             else events=ev_next;  \
				free_event(ev);  \
				continue;
				
/*
 * Handle events.
 */
void event_update(void)
{
   EVENT_DATA *ev = NULL, *ev_next, *ev_last = ev;


   if(!events) return;

   for( ev = events; ev; ev = ev_next)
   {
        ev_next = ev->next;
        if(ev->delay-- <= 0)
        {
            switch(ev->action)
            {
                case ACTION_PRINT:
                  if( ( !ev->args[0]) || (! ev->argv[0] ) )
                  {
                      kill_event
                  }
                  stc( ev->args[0], ev->argv[0] );
                  break;

                case ACTION_FUNCTION:
                  if( !ev->args[0] )
                  {
                      kill_event
                  }
                  do_function(ev->argv[0], ev->do_fun, ev->args[0]);
                  break;

                case ACTION_WAIT:
                  if(!ev->argv[0])
                  {
                      kill_event
                  }
                  WAIT_STATE(ev->to,PULSE_PER_SECOND*ev->argi[0]);
                  break;

                case ACTION_ACT:
                  if(!ev->args[0] || !ev->argv[0])
                  {
                       kill_event
                  }
                  act(ev->args[0], ev->argv[0], ev->argv[1],ev->argv[2],ev->argi[0]);
                  break;
            }

            if(!ev_next)
            {
                 if( (ev != events) && (ev_last != NULL) )
                 {
                      if( ev_last->next == ev )
                         ev_last->next=NULL;
                 }
                 else
                 {
                 if( ev == events)
                    events = NULL;
                 }
                 free_event( ev );
                 return;
            }

            if( ev_last != NULL)
                ev_last->next = ev_next;
            else
                events = ev_next;

            free_event( ev );
            continue;
        } /* end delay */

        ev_last = ev;

   } /* end loop */
return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update (void)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	EXIT_DATA *pexit;
	int door;

	/* Examine all mobs. */
	for (ch = char_list; ch != NULL; ch = ch_next)
	{
		ch_next = ch->next;

		if (ch->in_room == NULL)
			continue;

		if (!IS_NPC (ch))
		{
			if (ch->desc == NULL || !IS_PLAYING (ch->desc))
				continue;
			if (ch->embraced == ARE_EMBRACING)
			{
				CHAR_DATA *vch;

				int amount = 1;
				if (ch->position < POS_STANDING)
				{
					if ((vch = ch->embrace) != NULL)
					{
						vch->embrace = NULL;
						vch->embraced = ARE_NONE;
					}
					ch->embrace = NULL;
					ch->embraced = ARE_NONE;
				}
				else if ((vch = ch->embrace) == NULL)
					ch->embraced = ARE_NONE;
				else if (vch->position == POS_FIGHTING)
				{
					vch->embrace = NULL;
					vch->embraced = ARE_NONE;
					ch->embrace = NULL;
					ch->embraced = ARE_NONE;
				}
				else if (vch->in_room == NULL || ch->in_room == NULL || ch->in_room != vch->in_room || is_safe (ch, vch))
				{
					ch->embrace = NULL;
					ch->embraced = ARE_NONE;
					vch->embrace = NULL;
					vch->embraced = ARE_NONE;
				}
				else if (vch->level > 100 && vch->position > POS_STUNNED)
				{
					if (vch->position < POS_FIGHTING)
					{
						act ("You clamber to your feet.", vch, NULL, NULL, TO_CHAR);
						act ("$n clambers to $s feet.", vch, NULL, NULL, TO_ROOM);
						vch->position = POS_STANDING;
					}
					act ("$N rips your head away from $S throat and hurls you across the room.", ch, NULL, vch, TO_CHAR);
					act ("You rip $n's head away from your throat and hurl $m across the room.", ch, NULL, vch, TO_VICT);
					act ("$N rips $n's head away from $S throat and hurls $m across the room.", ch, NULL, vch, TO_NOTVICT);
					act ("You crash to the ground, stunned.", ch, NULL, NULL, TO_CHAR);
					act ("$n crashes to the ground, stunned.", ch, NULL, NULL, TO_ROOM);
					ch->position = POS_STUNNED;
					ch->embrace = NULL;
					ch->embraced = ARE_NONE;
					vch->embrace = NULL;
					vch->embraced = ARE_NONE;
				}
				else
				{
					if (IS_CHANGELING (vch) && IS_MORE2 (vch, MORE2_CRIMSONFURY) && number_range (0, 50) > 40)
					{
						send_to_char ("You feel a burning rage spread within you!\n\r", ch);
						SET_BIT (ch->more, MORE_ANGER);
						ch->pcdata->wolf += 10;
					}


					if (IS_SET (vch->act, ACT_ANIMAL) && IS_CHANGELING (ch) && get_disc (ch, DISC_NIGHTMARE) < 3)
					{
						amount /= 0.5;
						act ("You find you do not get as much nourishment from $N since its an animal.", ch, NULL, vch, TO_CHAR);
					}
					act ("#CYou harvest #w1#C Glamour point from #w$N#C.#n", ch, NULL, vch, TO_CHAR);
					act ("#CYou lose #w1#C Glamour point to #C$n#w.#n", ch, NULL, vch, TO_VICT);
					act ("#w$n#C takes #w1#C Glamour point from #w$N#C.#n", ch, NULL, vch, TO_NOTVICT);
					if (vch->position == POS_STUNNED)
						update_pos (vch);
					if (IS_CHANGELING (vch) && !str_cmp (vch->bloodline, "Lamia"))
						send_to_char ("You find the blood tastes somewhat odd....(ask imm for dice check for drinking the blood of a Lamia\n\r", ch);

					if (!IS_NPC (vch) && ((IS_CHANGELING (vch) && get_organization (vch, ORGANIZATION_GET_OF_FENRIS) > 0)))
					{
						int dam = (number_range (1, 5) * amount * vch->blood[BLOOD_POTENCY]);
						act ("Your lips blister and burn as $N's blood enters your mouth!", ch, NULL, vch, TO_CHAR);
						ch->hit -= dam;
						ch->agg += dam * 0.005;
						if (ch->agg > 100)
							ch->agg = 100;
						if (ch->hit < -10)
							ch->hit = -10;
						update_pos (ch);
						if (ch->position <= POS_STUNNED)
						{
							act ("$n bursts into tears and collapses on the floor.", ch, NULL, NULL, TO_ROOM);
							ch->embrace = NULL;
							ch->embraced = ARE_NONE;

							vch->embrace = NULL;
							vch->embraced = ARE_NONE;
						}
					}
					if (IS_MORE (vch, MORE_BRISTLES))
					{
						update_pos (ch);
						if (ch->position <= POS_STUNNED)
						{
							act ("$n keels over and collapses on the floor.", ch, NULL, NULL, TO_ROOM);
							ch->embrace = NULL;
							ch->embraced = ARE_NONE;
							vch->embrace = NULL;
							vch->embraced = ARE_NONE;
						}
					}
					if (vch->blood[BLOOD_CURRENT] > amount && (vch->blood[BLOOD_CURRENT] - amount) <= 3)
					{
						ch->blood[BLOOD_CURRENT] += (amount * vch->blood[BLOOD_POTENCY]);
						vch->blood[BLOOD_CURRENT] -= amount;
						if (!IS_CHANGELING (vch))
						{
							act ("#C$N sways weakly in your arms.", ch, NULL, vch, TO_CHAR);
							act ("#CYou sway weakly in $n's arms.", ch, NULL, vch, TO_VICT);
							act ("#C$N sways weakly in $n's arms.", ch, NULL, vch, TO_NOTVICT);
						}
						if (vch->blood[BLOOD_CURRENT] <= 5)
						{
							if (IS_CHANGELING (vch))
							{
								act ("You can feel that $N is very low on blood.", ch, NULL, vch, TO_CHAR);
								send_to_char ("You are very low on blood.\n\r", vch);
							}
							else
							{
								act ("You can feel that $N is on the verge of death.", ch, NULL, vch, TO_CHAR);
								send_to_char ("You feel very faint.\n\r", vch);
							}
						}
					}
					else if (vch->blood[BLOOD_CURRENT] - amount <= 0)
					{
						ch->blood[BLOOD_CURRENT] += vch->blood[BLOOD_CURRENT];
						vch->blood[BLOOD_CURRENT] = 0;
						act ("Your body falls limply to the ground.", vch, NULL, NULL, TO_CHAR);
						act ("$n's body falls limply to the ground.", vch, NULL, NULL, TO_ROOM);
						if (!IS_NPC (vch) && IS_CHANGELING (vch))
						{
							act ("You suck $N's soul out of $S body.", ch, NULL, vch, TO_CHAR);
							act ("$n sucks your soul out of your body.", ch, NULL, vch, TO_VICT);
							if (ch->beast == 0)
							{
								send_to_char ("You have lost Golconda!\n\r", ch);
								sprintf (buf, "%s has lost Golconda!", ch->name);
								do_info (ch, buf);
								ch->beast = 80;
								ch->truebeast = 90;
							}
							ch->beast += 20;
							ch->truebeast += 10;
							if (ch->beast > 100)
								ch->beast = 100;
							if (ch->truebeast > 100)
								ch->truebeast = 100;
							if (is_affected (vch, gsn_potency))
								affect_strip (vch, gsn_potency);
							if (vch->vamppot < ch->vamppot && vch->vamppot > 0)
							{
								/* sprintf (buf, "%s has been diablerised by %s.", vch->name, ch->name);
								do_info (ch, buf); */
								ch->vamppot--;
								update_pot (ch);
								update_pot (vch);
								send_to_char ("You have lowered a potency level!\n\r", ch);
							}
							else
							{
								/* sprintf (buf, "%s has been diablerised by %s for no potency.", vch->name, ch->name);
								do_info (ch, buf); */
							}
						}
						ch->embrace = NULL;
						ch->embraced = ARE_NONE;
						vch->embrace = NULL;
						vch->embraced = ARE_NONE;
						vch->form = 0;
					}
					else
					{
						ch->blood[BLOOD_CURRENT] += (amount * vch->blood[BLOOD_POTENCY]);
						vch->blood[BLOOD_CURRENT] -= amount;
					}
					if (ch->blood[BLOOD_CURRENT] >= ch->blood[BLOOD_POOL])
						ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
				}
			}

			if (IS_CHANGELING (ch))
			{
				
				if (ch->pcdata->wolf < 1)
				{
					if (ch->pcdata->regenerate > 0)
					{
						if (ch->blood[BLOOD_CURRENT] < 1 && ch->hit > 0)
						{
							send_to_char ("#eYou don't have enough #Rblood#e to regenerate.#n\n\r", ch);
							continue;
						}
						ch->pcdata->regenerate--;
						ch->blood[BLOOD_CURRENT] -= 1;

						if (ch->loc_hp[6] > 0)
							ch->loc_hp[0] = 0;
					}
					continue;
				}
				if (ch->move > 1)
					ch->move--;
			}
			else if (IS_CHANGELING (ch))
			{
				if (ch->position == POS_FIGHTING && !IS_ITEMAFF (ch, ITEMA_RAGER))
				{
					if (ch->pcdata->wolf < 300)
					{
						if (get_organization (ch, ORGANIZATION_GET_OF_FENRIS) > 4)
							ch->pcdata->wolf += number_range (15, 30);
						else
							ch->pcdata->wolf += number_range (5, 10);
					}
				}
				else if (ch->pcdata->wolf > 0 && !IS_ITEMAFF (ch, ITEMA_RAGER))
				{
					ch->pcdata->wolf--;
				}

				if (ch->blood[BLOOD_CURRENT] < ch->blood[BLOOD_POOL])
				{
					ch->blood[BLOOD_CURRENT] += number_range (1, 2);
					if (ch->blood[BLOOD_CURRENT] > ch->blood[BLOOD_POOL])
						ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_POOL];
				}

				if (ch->loc_hp[6] > 0)
				{
					int sn = skill_lookup ("clot");
					(*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
				}

			}
			if (IS_ITEMAFF (ch, ITEMA_REGENERATE) && !IS_NPC (ch))
			{
				if (ch->loc_hp[6] > 0)
				{
					int sn = skill_lookup ("clot");
					(*skill_table[sn].spell_fun) (sn, ch->level, ch, ch);
				}
			}
			continue;
		}

		if (IS_AFFECTED (ch, AFF_CHARM) || IS_EXTRA (ch, EXTRA_ILLUSION) || ch->wizard != NULL)
			continue;

		/* Examine call for special procedure */
		if (ch->spec_fun != 0)
		{
			if ((*ch->spec_fun) (ch))
				continue;
			if (ch == NULL)
				continue;
		}


		/* That's all for sleeping / busy monster */
		if (ch->position != POS_STANDING)
			continue;

		/* MOBprogram random trigger */
		if (ch->in_room->area->nplayer > 0)
		{
			mprog_random_trigger (ch);
			/* If ch dies or changes
			   position due to it's random
			   trigger, continue - Kahn */
			if (ch->position < POS_STANDING)
				continue;
		}


		/* Scavenge */
		if (IS_SET (ch->act, ACT_SCAVENGER) && ch->in_room->contents != NULL && number_bits (2) == 0)
		{
			OBJ_DATA *obj;
			OBJ_DATA *obj_best;
			int max;

			max = 1;
			obj_best = 0;
			for (obj = ch->in_room->contents; obj; obj = obj->next_content)
			{
				if (obj->item_type == ITEM_CORPSE_PC)
					continue;
				if (CAN_WEAR (obj, ITEM_TAKE) && obj->cost > max)
				{
					obj_best = obj;
					max = obj->cost;
				}
			}

			if (obj_best)
			{
				obj_from_room (obj_best);
				obj_to_char (obj_best, ch);
				act ("$n picks $p up.", ch, obj_best, NULL, TO_ROOM);
				act ("You pick $p up.", ch, obj_best, NULL, TO_CHAR);
			}
		}

		/* Wander */
		if (!IS_SET (ch->act, ACT_SENTINEL) && (door = number_bits (5)) <= 5 && (pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL && !IS_SET (pexit->exit_info, EX_CLOSED) && !IS_SET (pexit->to_room->room_flags, ROOM_NO_MOB) && (ch->hunting == NULL || strlen (ch->hunting) < 2) && ((!IS_SET (ch->act, ACT_STAY_AREA) && ch->level < 900) || pexit->to_room->area == ch->in_room->area))
		{
			move_char (ch, door);
			/* If ch changes position due   to it's or someother mob's
			   movement via MOBProgs,       continue - Kahn */
			if (ch->position < POS_STANDING)
				continue;

		}

		/* Flee */
		if (ch->hit < ch->max_hit / 2 && (door = number_bits (3)) <= 5 && (pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL && !IS_AFFECTED (ch, AFF_PERFUME) && ch->level < 900 && !IS_SET (pexit->exit_info, EX_CLOSED) && !IS_SET (pexit->to_room->room_flags, ROOM_NO_MOB))
		{
			CHAR_DATA *rch;
			bool found;

			found = FALSE;
			for (rch = pexit->to_room->people; rch != NULL; rch = rch->next_in_room)
			{
				if (!IS_NPC (rch))
				{
					found = TRUE;
					break;
				}
			}
			if (!found)
				move_char (ch, door);
		}

	}

	return;
}

void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC(ch) )
	return;

    condition				= ch->pcdata->condition[iCond];
    if (!IS_NPC(ch) && !IS_VAMPIRE(ch) )
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );
    else
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 100 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "#cYou are sober.#n\n\r", ch );
	    break;
	}
    }
    return;
}






// Check to see if a player will gain exp this tick
/*bool should_gain_exp(CHAR_DATA * ch)
{
	if (ch->pcdata->votes > 5)
		return TRUE;
	else
		return FALSE;
}*/

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update (void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *ch_save;
	CHAR_DATA *ch_quit;
	bool is_obj;
	bool drop_out = FALSE;
	int sn;
	time_t save_time;
//	char buf[MAX_STRING_LENGTH];

	save_time = current_time;
	ch_save = NULL;
	ch_quit = NULL;
	for (ch = char_list; ch != NULL; ch = ch_next)
	{


		AFFECT_DATA *paf;
		AFFECT_DATA *paf_next;

		ch_next = ch->next;
		drop_out = FALSE;

//Cone's new exp code 
/*		if (!IS_NPC(ch))
		{
			if (should_gain_exp(ch))
			{
				int xp = 1;
			
				sprintf (buf, "%s has been rewarded %d xp", ch->name, xp);
				log_string (buf, CHANNEL_LOG_NORMAL);
	
				sprintf (buf, "#CYou have gained %d xp for Roleplay. Remember to vote for good roleplay!\n\r", xp);
				send_to_char (buf, ch);
				ch->exp = ch->exp + xp;
				ch->pcdata->votes = ch->pcdata->votes - 6;
			}	
		}*/

/* Blood Code */
//		 (uncomment to enable automatic blood burns)

/*		if (!IS_NPC(ch) && ch->pcdata->blood_burn_counter >= 6)
		{
			if (IS_CHANGELING(ch))
			{
				
				if (!strcmp(ch->bloodline, "Ghoul"))  
					send_to_char ("#eYou crave the#R blood#e of a vampire.#n\n\r", ch);
				else if(ch->blood[BLOOD_CURRENT] < 3)
					send_to_char ("#eYou feel depressed; the world is bleak.#n\n\r", ch);
				else
				{
					ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_CURRENT] - 1;
					send_to_char ("#eYou lost one Glamour over the course of the day.#n\n\r", ch);
				}
			}
	
			ch->pcdata->blood_burn_counter = 0;
		}*/
		


/* WP Code */
		if (!IS_NPC(ch) && ch->pcdata->wp_regain_counter >= 8)
		{
			if (ch->pcdata->willpower[WILLPOWER_CURRENT] < ch->pcdata->willpower[WILLPOWER_MAX])
			{
				ch->pcdata->willpower[WILLPOWER_CURRENT] += 1;
				send_to_char ("You regain one willpower from rest.\n\r", ch);
			}
			
			ch->pcdata->wp_regain_counter = 0;
		}
		
		
/* Shopping code */

		if (!IS_NPC(ch) && (ch->pcdata->canbuycounter >= 4) && (ch->pcdata->canbuy < 9))
		{
			
				ch->pcdata->canbuy += 1;
				ch->pcdata->canbuycounter = 0;
		}	
				
	/*if (!IS_NPC(ch) && (ch->pcdata->canvotecounter >= 3))
		{
			
	
				send_to_char ("#cYou can now cast a roleplay vote.#n\n\r", ch);
			  ch->pcdata->canvotecounter = 3;
		}*/
/* Auto Vote Code */

/*		if (!IS_NPC(ch) && ch->pcdata->auto_vote_counter >= 4)
		{
			ch->pcdata->votes ++;
			ch->pcdata->auto_vote_counter = 0;
			sprintf(buf, "auto-vote given to %s", ch->name);
			logchan(buf, CHANNEL_LOG_NORMAL);
		}
	*/
		if (!IS_NPC (ch) && IS_SET (ch->extra2, EXTRA2_DAEMON_REGEN))
		{
			send_to_char ("#eYour regeneration is complete.  Raise Hell", ch);
			REMOVE_BIT (ch->extra2, EXTRA2_DAEMON_REGEN);
		}

		if (!IS_NPC (ch) && IS_SET (ch->extra2, EXTRA2_TORPORED) && (ch->hit > 0))
		{
			REMOVE_BIT (ch->extra2, EXTRA2_TORPORED);
			send_to_char ("#eYou awaken from torpor.", ch);
		}

		if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH))
			is_obj = TRUE;
		else if (!IS_NPC (ch) && ch->pcdata->obj_vnum != 0)
		{
			is_obj = TRUE;
			SET_BIT (ch->extra, EXTRA_OSWITCH);
		}
		else
			is_obj = FALSE; 
		/*
		 * Find dude with oldest save time.
		 */
		if (!IS_NPC (ch) && (ch->desc == NULL || ch->desc->connected == CON_PLAYING) && ch->level >= 2 && ch->save_time < save_time)
		{
			ch_save = ch;
			save_time = ch->save_time;
		}

		if (ch->position == POS_STUNNED && !is_obj)
		{
			update_pos (ch);
		}






		if (!IS_NPC (ch) && ch->level < LEVEL_IMMORTAL && !is_obj)
		{
			OBJ_DATA *obj;

			if (((obj = get_eq_char (ch, WEAR_WIELD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0) || ((obj = get_eq_char (ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0))
			{
				if (--obj->value[2] == 0 && ch->in_room != NULL)
				{
					--ch->in_room->light;
					act ("$p goes out.", ch, obj, NULL, TO_ROOM);
					act ("$p goes out.", ch, obj, NULL, TO_CHAR);
					extract_obj (obj);
				}
			}

			if (++ch->timer >= 12)
			{
				if (ch->was_in_room == NULL && ch->in_room != NULL)
				{
					ch->was_in_room = ch->in_room;
					send_to_char ("You disappear into the void.\n\r", ch);
					act ("$n disappears into the void.", ch, NULL, NULL, TO_ROOM);
					if (!IS_NPC (ch) && ch->pcdata->chobj != NULL)
					{
						OBJ_DATA *chobj = ch->pcdata->chobj;
						if (chobj->in_room != NULL)
							obj_from_room (chobj);
						else if (chobj->carried_by != NULL)
							obj_from_char (chobj);
						else if (chobj->in_obj != NULL)
							obj_from_obj (chobj);
						obj_to_room (chobj, get_room_index (ROOM_VNUM_LIMBO));
					}
					do_autosave (ch, ch->name);
					char_from_room (ch);
					char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
				}
			}

			if (ch->timer > 30)
				ch_quit = ch;

			gain_condition( ch, COND_DRUNK,  -1 );
			
			if (!IS_CHANGELING (ch))
			{
			}
			else
			{
				if (ch->hit > 0 && ch->blood[BLOOD_CURRENT] < 1)
				{
					send_to_char ("You are suicidal from lack of glamour!\n\r", ch);
					act ("$n gets a look of madness in $s eyes.", ch, NULL, NULL, TO_ROOM);

					if (ch->hit > 0)
						ch->hit -= number_range (2, 5);

					update_pos (ch);

				//	if (!IS_VAMPAFF (ch, VAM_FANGS))
				//		do_fangs (ch, "");
				}
				else if (ch->hit > 0 && ch->blood[BLOOD_CURRENT] <= 2)
				{
					send_to_char ("#eYou desperately need glamour.#n\n\r", ch);

				//	if (number_percent () > (ch->blood[BLOOD_CURRENT] + 5) && !IS_VAMPAFF (ch, VAM_FANGS))
				//		do_fangs (ch, "");
				}
			}
		}

		for (paf = ch->affected; paf != NULL; paf = paf_next)
		{
			paf_next = paf->next;
			if (paf->duration > 0)
				paf->duration--;
			else if (paf->duration < 0)
				;
			else
			{
				if (paf_next == NULL || paf_next->type != paf->type || paf_next->duration > 0)
				{
					if (paf->type > 0 && skill_table[paf->type].msg_off && !is_obj)
					{
						send_to_char (skill_table[paf->type].msg_off, ch);
						send_to_char ("\n\r", ch);
					}
				}

				affect_remove (ch, paf);
			}
		}

		/*
		 * Careful with the damages here,
		 *   MUST NOT refer to ch after damage taken,
		 *   as it may be lethal damage (on NPC).
		 */
		if (ch->loc_hp[6] < 1 && !is_obj && ch->in_room != NULL && !IS_CHANGELING (ch))
		{
			if (ch->blood[0] < 10)
				ch->blood[0]++;
		}
		if (ch->loc_hp[6] > 0 && !is_obj && ch->in_room != NULL && !IS_CHANGELING (ch))
		{
			ch->in_room->blood += number_range (1, 2);
			if (ch->in_room->blood > 100)
				ch->in_room->blood = 100;
		}
		if (IS_AFFECTED (ch, AFF_FLAMING) && !is_obj && !drop_out && ch->in_room != NULL)
		{
			int dam = number_range (250, 500);

			if (IS_MORE (ch, MORE_FLAMES))
				dam *= 2;
			if (IS_NPC (ch) || (get_disc (ch, DISC_RESILIENCE) < 6))
			{
				if (!IS_NPC (ch) && (IS_CHANGELING (ch) || IS_GHOUL (ch)) && get_disc (ch, DISC_FLEETINGWINTER) > 0)
				{
					int per_red = 0;
					switch (get_disc (ch, DISC_FLEETINGWINTER))
					{
					default:
						per_red = 0;
						break;
					case 1:
						per_red = number_range (5, 10);
						break;
					case 2:
						per_red = number_range (10, 20);
						break;
					case 3:
						per_red = number_range (15, 30);
						break;
					case 4:
						per_red = number_range (20, 40);
						break;
					case 5:
						per_red = number_range (25, 50);
						break;
					case 6:
						per_red = number_range (30, 60);
						break;
					case 7:
						per_red = number_range (35, 70);
						break;
					case 8:
						per_red = number_range (40, 80);
						break;
					case 9:
						per_red = number_range (45, 90);
						break;
					case 10:
						per_red = number_range (50, 100);
						break;
					}
					ch->hit += (per_red * dam * 0.01);
				}
				ch->hit -= dam;
				update_pos (ch);
				if (ch == NULL)
					continue;
				if (ch->position == POS_DEAD)
				{
					ch->form = 14;
					drop_out = TRUE;
					continue;
				}
			}
		}
		if (IS_AFFECTED (ch, AFF_ROT) && !is_obj && !drop_out)
		{
			act ("Pieces of rotting flesh fall from $n's body.", ch, NULL, NULL, TO_ROOM);
			send_to_char ("Pieces of rotting flesh fall from your body.\n\r", ch);
			if (ch == NULL || ch->position == POS_DEAD)
				continue;
			else if (get_curr_sta (ch) > 0 && (sn = skill_lookup ("rotting flesh")) >= 0)
			{
				AFFECT_DATA af;
				af.type = sn;
				af.location = APPLY_NONE;
				af.modifier = -1;
				af.duration = 0;
				af.bitvector = AFF_ROT;
				affect_join (ch, &af);
			}
		}
		else if (IS_AFFECTED (ch, AFF_POISON) && !is_obj && !drop_out)
		{
			act ("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
			send_to_char ("You shiver and suffer.\n\r", ch);
			if (ch == NULL || ch->position == POS_DEAD)
				continue;
		}

		else if (ch->position == POS_INCAP && !is_obj && !drop_out)
		{
			if (!IS_SET (ch->extra2, EXTRA2_TORPORED))
			{
				if (IS_HERO (ch))
					ch->hit = ch->hit + number_range (2, 4);
				else
					ch->hit = ch->hit - number_range (1, 2);
				update_pos (ch);
			}
			if (ch->position > POS_INCAP)
			{
				act ("$n's wounds stop bleeding and seal up.", ch, NULL, NULL, TO_ROOM);
				send_to_char ("Your wounds stop bleeding and seal up.\n\r", ch);
			}
			if (ch->position > POS_STUNNED)
			{
				act ("$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM);
				send_to_char ("You clamber back to your feet.\n\r", ch);
			}
		}
		else if (ch->position == POS_MORTAL && !is_obj && !drop_out)
		{
			drop_out = FALSE;
			if (IS_HERO (ch))
				ch->hit = ch->hit + number_range (2, 4);
			else
			{
				ch->hit = ch->hit - number_range (1, 2);
				if ((!IS_NPC (ch) && ch->hit < -10) || (IS_NPC (ch) && ch->hit < 0))
				{
/* kavirp */
					ch->form = 16;
					continue;
				}
			}
			if (!drop_out)
			{
				update_pos (ch);
				if (ch->position == POS_INCAP)
				{
					act ("$n's wounds begin to close, and $s bones pop back into place.", ch, NULL, NULL, TO_ROOM);
					send_to_char ("Your wounds begin to close, and your bones pop back into place.\n\r", ch);
				}
			}
		}
		else if (ch->position == POS_DEAD && !is_obj && !drop_out)
		{
			if (!IS_NPC (ch))
			{
				ch->form = 16;
				continue;
			}
		}
		drop_out = FALSE;
	}

	/*
	 * Autosave and autoquit.
	 * Check that these chars still exist.
	 */
	if (ch_save != NULL || ch_quit != NULL)
	{
		for (ch = char_list; ch != NULL; ch = ch_next)
		{
			ch_next = ch->next;
			if (ch == ch_save)
				do_autosave (ch, ch->name);
			if (ch == ch_quit)
				do_quit (ch, "");
		}
	}

	return;
}



/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update (void)
{
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	for (obj = object_list; obj != NULL; obj = obj_next)
	{
		CHAR_DATA *rch;
		char *message;

		obj_next = obj->next;
		
/*			if (obj->item_type != ITEM_PAGE)
	{
	    if ( regenerate_part( obj ) ) continue;
	}*/
	
		if (obj->item_type != ITEM_PAGE)
	{
	    if (strlen(obj->chpoweruse) > 1 && strlen(obj->victpoweruse) > 1 && 
		number_range(1,30) == 1)
	    {
		if ( obj->carried_by != NULL )
		{
		    act( obj->chpoweruse, obj->carried_by, obj, NULL, TO_CHAR );
		    act( obj->victpoweruse, obj->carried_by, obj, NULL, TO_ROOM );
		}
		else if ( obj->in_room != NULL 
		&&      ( rch = obj->in_room->people ) != NULL )
		{
		    act( obj->chpoweruse, rch, obj, NULL, TO_ROOM );
		    act( obj->chpoweruse, rch, obj, NULL, TO_CHAR );
		}

	    }
	}

		if (obj->timer <= 0 || --obj->timer > 0)
			continue;

		if (IS_SET (obj->extra_flags, ITEM_VANISH))
			message = "$p slowly fades away to nothing.";
		else
			switch (obj->item_type)
			{
			default:
				message = "$p vanishes.";
				break;
			case ITEM_FOUNTAIN:
				message = "$p dries up.";
				break;
			case ITEM_CORPSE_NPC:
				message = "$p decays into dust.";
				break;
			case ITEM_CORPSE_PC:
				message = "$p decays into dust.";
				break;
			case ITEM_FOOD:
				message = "$p decomposes.";
				break;
			case ITEM_TRASH:
				message = "$p crumbles into dust.";
				break;
			case ITEM_EGG:
				message = "$p cracks open.";
				break;
			case ITEM_WEAPON:
				message = "$p crumbles into dust.";
				break;
			case ITEM_WALL:
				message = "$p sinks back into the ground.";
				break;
			case ITEM_STAKE:
				message = "$p crumbles into dust.";
				break;
			}

		if (obj->carried_by != NULL)
		{
			act (message, obj->carried_by, obj, NULL, TO_CHAR);
		}
		else if (obj->in_room != NULL && (rch = obj->in_room->people) != NULL)
		{
			act (message, rch, obj, NULL, TO_ROOM);
			act (message, rch, obj, NULL, TO_CHAR);
		}

		/* If the item is an egg, we need to create a mob and shell!
		 * KaVir
		 */
		/* That we do!
		 * Lumi
		 */
		if (obj->item_type == ITEM_EGG)
		{
			CHAR_DATA *creature;
			OBJ_DATA *egg;
			if (get_mob_index (obj->value[0]) != NULL)
			{
				if (obj->carried_by != NULL && obj->carried_by->in_room != NULL)
				{
					creature = create_mobile (get_mob_index (obj->value[0]));
					char_to_room (creature, obj->carried_by->in_room);
				}
				else if (obj->in_room != NULL)
				{
					creature = create_mobile (get_mob_index (obj->value[0]));
					char_to_room (creature, obj->in_room);
				}
				else
				{
					creature = create_mobile (get_mob_index (obj->value[0]));
					char_to_room (creature, get_room_index (ROOM_VNUM_LIMBO));
				}
				egg = create_object (get_obj_index (OBJ_VNUM_EMPTY_EGG), 0);
				egg->timer = 2;
				obj_to_room (egg, creature->in_room);
				act ("$n clambers out of $p.", creature, obj, NULL, TO_ROOM);
			}
			else if (obj->in_room != NULL)
			{
				egg = create_object (get_obj_index (OBJ_VNUM_EMPTY_EGG), 0);
				egg->timer = 2;
				obj_to_room (egg, obj->in_room);
			}
		}
		extract_obj (obj);
	}

	return;
}

void hint_update( void )
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;

  for(ch=char_list;ch!=NULL;ch=ch_next)
    {
    ch_next = ch->next;

    if(!IS_NPC(ch) && !IS_SET(ch->deaf, CHANNEL_HINT))
      {
      send_to_char("\n\r#y[#gNewbie Hint!#y]#n ",ch);
      switch(number_range(0,33))
        {
        default: send_to_char("#gYou can adjust your most basic game settings with #Gconfig#g.#n", ch); break;
        case 0: send_to_char("#gNeed new clothes or flavor objects? Use#G purchase#g!#n",ch); break;
        case 1: send_to_char("#gAll channels can be toggled on and off with #Gchannel -channelname#g.#n",ch); break;
        case 2: send_to_char("#gPhones and SMS are a common way of communicating IC. See #Ghelp phone#g and #Ghelp sms#g.#n",ch); break;
        case 3: send_to_char("#gYou can hide yourself on the where list by toggling #Grpvis#g.#n",ch); break;
        case 4: send_to_char("#gLooking for your Merits, Equipment, Pledges or Specialties? Check #Gscore extra#g!#n",ch); break;
        case 5: send_to_char("#gCharacter death is permanent on Innocence Lost. Check out #Ghelp death#g for more information.#n",ch); break;
        case 6: send_to_char("#GRollref#g contains the pools for commons rolls made in roleplay.#n",ch); break;
        case 7: send_to_char("#gChangelings with the right Contracts can use #Gskinmask#g and #Glightshy#g to change their appearance or go invisible!#n",ch); break;
        case 8: send_to_char("#gYou can store any held item in your pockets with the #Gpocket#g command.#n",ch); break;
        case 9: send_to_char("#gChangelings must set #Gmlongdesc#g and #Gmiendesc#g in addition to desc and roomdesc.#n",ch); break;
        case 10: send_to_char("#gYou can contact the Staff real-time using the #Grequest <text here>#g command. The request board is for longer-term needs.#n",ch); break;
		case 11: send_to_char("#gXP is gained through the recommendation system. See #Ghelp rec#g for information!#n",ch); break;
		case 12: send_to_char("#gYou can get rid of unwanted items by dropping, and #Gjunk#ging them.#n",ch); break;
		case 13: send_to_char("#gDon't forget to #Gremember#g characters you've met and know.#n",ch); break;
		case 14: send_to_char("#gThe #Gvehicle#g code lets you customize and drive your own car! See #Ghelp vehicle#g for commands.#n",ch); break;
		case 15: send_to_char("#gThe #Gmap#g command shows you a live version of your whereabouts.#n",ch); break;
		case 16: send_to_char("#gList the members of London's Freehold with the #Gfreehold#g command#n",ch); break;
		case 17: send_to_char("#gAlways be mindful of your current #Gdoing#g! It follows you from room to room.#n",ch); break;
		case 18: send_to_char("#gYou can display your current damage levels to the room with the #Greport#g command.#n",ch); break;
		case 19: send_to_char("#gDon't forget to explore! Use #Glook around#g to see details around London.#n",ch); break;
		case 20: send_to_char("#gChangelings should pay close attention to the grid. Hedge #Ggate#gs can be opened in some areas!#n",ch); break;
		case 21: send_to_char("#gEach month the Freehold Knights track down the latest #Gmarket#g entrances. RP with your fellow Lost to stay in the know!#n",ch); break;
		case 22: send_to_char("#gPD has an optional OOC channel. Use #Gchan +ooc#g and #Gchan +omote#g to turn it on.#n",ch); break;
		case 23: send_to_char("#gChangelings have access to the #Ggate#g,#G market#g, and #Gescape#g commands. It's up to them to figure out where to use them!#n",ch); break;
		case 24: send_to_char("#gYou can use the #Gsnap#g command to send IC pictures to other characters.#n",ch); break;
		case 25: send_to_char("#gCharacters can become engaged and marry one another in-game. See #Ghelp marriage#g for details.#n",ch); break;
		case 26: send_to_char("#gYou can #Gorder#g drinks at bars and #Gpurchase#g goods from stores.#n",ch); break;
		case 27: send_to_char("#gThinking about forming a motley? Check out #Ghelp motley#g for commands!#n",ch); break;
		case 28: send_to_char("#gYou can #Gwrite#g notes and put them into books with the right equipment.#n",ch); break;
		case 29: send_to_char("#GApply#g is used to apply perfumes and colognes to your character.#n",ch); break;
		case 30: send_to_char("#gThe #Grat#gs of the London Underground are happy to deliver notes and items to the Lost.#n",ch); break;
		case 31: send_to_char("#gThere are many ways for Lost to regain Glamour: Harvesting from players (help harvest), foraging for Hedgefruit (help forage), hunting for Glamour sources in the city (help hunt), and finding special locations which cater drinks to the Lost (help order).#n",ch); break;
		case 32: send_to_char("#gBe mindful of what is in your #Gaffects#g score. It could be important!#n",ch); break;
		case 33: send_to_char("#gPlayers can use #Gpathfind#g to track down players visible on where.#n",ch); break;
        }
      send_to_char("\n\r",ch);
      }
    }
return;
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler (void)
{
	static int pulse_gain_exp;
	static int pulse_area;
	static int pulse_day;
	static int pulse_violence;
	static int pulse_point;
    static  int     pulse_msdp; /* <--- Add this line */
	static int pulse_hint;
	static int pulse_mudinfo;
	static int pulse_hour;
    static	int	    pulse_event;
        DESCRIPTOR_DATA *d;
        CHAR_DATA *ch = NULL;

    if ( --pulse_msdp <= 0 )
    {
        pulse_msdp      = PULSE_PER_SECOND;
        msdp_update();
    }
		
    if ( --pulse_event        <= 0 )
    {
        pulse_event             = PULSE_EVENT;
        event_update();
    }

	if (--pulse_gain_exp <= 0)
	{
		char buf[MAX_STRING_LENGTH];
		CHAR_DATA *ch_next = NULL;
		int sn;
		extern int bootcount;
		pulse_gain_exp = PULSE_GAIN_EXP;
		if (bootcount > 0)
		{
			bootcount--;
			switch (bootcount)
			{
			default:
				strcpy (buf, " ");
				break;
			case 600:
				strcpy (buf, "#RReboot in ten minutes");
				break;
			case 300:
				strcpy (buf, "#RReboot in five minutes");
				break;
			case 120:
				strcpy (buf, "#Reboot in two minutes");
				break;
			case 60:
				strcpy (buf, "#RReboot in one minute");
				break;
			case 30:
				strcpy (buf, "#RReboot in thirty seconds");
				break;
			case 10:
				strcpy (buf, "#RReboot in ten seconds");
				break;
			case 0:
				strcpy (buf, "#RReboot NOW");
				break;
			}
			if (strlen (buf) > 2)
				sysmes (buf);
			if (bootcount <= 0)
			{
				extern bool merc_down;
				for (ch = char_list; ch != NULL; ch = ch_next)
				{
					ch_next = ch->next;
					if (!IS_NPC (ch))
						do_autosave (ch, ch->name);
				}
				sprintf (log_buf, "Timed reboot.");
				log_string (log_buf, CHANNEL_LOG_NORMAL);
				merc_down = TRUE;
			}
		}
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch))
			{
				if (ch->position == POS_FIGHTING && ch->move > 0 && !IS_HERO (ch))
					ch->move--;
				if (ch->pcdata->bpoints > 0)
				{
					ch->pcdata->bpoints--;
					if (ch->pcdata->bpoints < 1)
						send_to_char ("You are once more able to use Blood Cunning.\n\r", ch);
				}
				if (ch->pcdata->dpoints > 0)
				{
					ch->pcdata->dpoints--;
					if (ch->pcdata->dpoints < 1)
					{
						if (IS_GHOUL (ch))
						{
							bool lost_disc = FALSE;
							mortal_vamp (ch);
							for (sn = 0; sn <= DISC_MAX; sn++)
							{
								if (ch->pcdata->powers[sn] > 0)
									lost_disc = TRUE;
								ch->pcdata->powers[sn] = 0;
							}
							if (lost_disc)
								send_to_char ("You have lost your blood powers.\n\r", ch);
						}
						else
							send_to_char ("You are once more able to sire a childe.\n\r", ch);
					}
				}
				if (ch->pcdata->silence > 0)
				{
					ch->pcdata->silence--;
					if (ch->pcdata->silence < 1)
					{
						if (IS_MORE (ch, MORE_SILENCE))
						{
							act ("The room is no longer silent.", ch, NULL, NULL, TO_CHAR);
							act ("The room is no longer silent.", ch, NULL, NULL, TO_ROOM);
							REMOVE_BIT (ch->in_room->added_flags, ROOM2_SILENCE);
							REMOVE_BIT (ch->more, MORE_SILENCE);
							ch->pcdata->silence = 60;
						}
						else
							send_to_char ("Your ability to call the Silence of Death has returned.\n\r", ch);
					}
				}
				if (ch->pcdata->eternalsummer > 0)
				{
					ch->pcdata->eternalsummer--;
					if (ch->pcdata->eternalsummer < 1)
						send_to_char ("Your powers of Eternal Summer have replenished themselves.\n\r", ch);
				}
				if (ch->pcdata->rpoints > 0)
				{
					ch->pcdata->rpoints--;
					if (ch->pcdata->rpoints < 1)
						send_to_char ("You have regained sufficient strength to use another ritual.\n\r", ch);
				}
				if (ch->pcdata->wpoints > 0)
				{
					ch->pcdata->wpoints--;
					if (ch->pcdata->wpoints < 1)
						send_to_char ("Your powers of weather control have replenished themselves.\n\r", ch);
				}
				if (ch->pcdata->spirit > 0)
				{
					ch->pcdata->spirit--;
					if (ch->pcdata->spirit < 1)
						send_to_char ("Your powers over the spirits have replenished themselves.\n\r", ch);
				}
				if (ch->pcdata->conjure > 0)
				{
					ch->pcdata->conjure--;
					if (ch->pcdata->conjure < 1)
						send_to_char ("Your powers of conjuration have replenished themselves.\n\r", ch);
				}
			

    if ( --pulse_mudinfo  <= 0 )
    { 
      pulse_mudinfo = 30 * PULSE_PER_SECOND;
      update_mudinfo();
    } 
				
				    if ( --pulse_hint <= 0)
                     {
                    pulse_hint = PULSE_HINT;
                    hint_update ( );
                    }


				if (ch->pcdata->evileye > 0)
					ch->pcdata->evileye--;
				
				if (ch->pcdata->foragetime > 0)
				{
					ch->pcdata->foragetime--;
					if (ch->pcdata->foragetime < 1)
					{
						if (IS_CHANGELING (ch))
							send_to_char ("#cYou may forage for Hedgefruit again.#n\n\r", ch);
						else
							send_to_char ("#cYou may forage for Hedgefruit again.\n\r", ch);
					}
				}

				if (ch->pcdata->hunttime > 0)
				{
					ch->pcdata->hunttime--;
					if (ch->pcdata->hunttime < 1)
					{
						if (IS_CHANGELING (ch))
							send_to_char ("#cYou may hunt again.#n\n\r", ch);
						else
							send_to_char ("#cYou may hunt again.\n\r", ch);
					}					

				}
				
				// Paydays
				if (ch->pcdata->paytime > 0)
				{
					ch->pcdata->paytime--;
					if (ch->pcdata->paytime < 1)
					{
						if (IS_CHANGELING (ch))
							send_to_char ("#cYou may collect your paycheck.#n\n\r", ch);
						else
							send_to_char ("#cYou may collect your paycheck.\n\r", ch);
					}					

				}
				// End Paydays

				if (ch->pcdata->resist[WILL_AUSPEX] > 0)
				{
					ch->pcdata->resist[WILL_AUSPEX]--;
					if (ch->pcdata->resist[WILL_AUSPEX] < 1)
					{
						send_to_char ("You are no longer immune to having your mind Unveiled.\n\r", ch);
						if (!IS_WILLPOWER (ch, RES_TOGGLE))
							REMOVE_BIT (ch->pcdata->resist[0], RES_AUSPEX);
					}
				}
				if (ch->pcdata->resist[WILL_VIGOR] > 0)
				{
					ch->pcdata->resist[WILL_VIGOR]--;
					if (ch->pcdata->resist[WILL_VIGOR] < 1)
					{
						send_to_char ("You are no longer immune to being Dominated.\n\r", ch);
						if (!IS_WILLPOWER (ch, RES_TOGGLE))
							REMOVE_BIT (ch->pcdata->resist[0], RES_VIGOR);
					}
				}
				if (ch->pcdata->resist[WILL_ETERNALSPRING] > 0)
				{
					ch->pcdata->resist[WILL_ETERNALSPRING]--;
					if (ch->pcdata->resist[WILL_ETERNALSPRING] < 1)
					{
						send_to_char ("You are no longer immune to Dread Gaze and Entrancement.\n\r", ch);
						if (!IS_WILLPOWER (ch, RES_TOGGLE))
							REMOVE_BIT (ch->pcdata->resist[0], RES_ETERNALSPRING);
					}
				}
			}
		}
	}

	if (--pulse_area <= 0)
	{
		pulse_area = number_range (PULSE_AREA / 2, 3 * PULSE_AREA / 2);
		area_update ();
		save_eban_file ();
	}

	if (--pulse_day <= 0)
	{
		pulse_day = PULSE_WEEK;
	}


	if (--pulse_violence <= 0)
	{
		pulse_violence = PULSE_VIOLENCE;
	}

	if (--pulse_point <= 0)
	{
		pulse_point = number_range (PULSE_TICK / 2, 3 * PULSE_TICK / 2);
	    weather_update	( );
		char_update ();
	}

	if (--pulse_hour <= 0)
	{
		pulse_hour = 3600 * PULSE_PER_SECOND;
		
                for (d = descriptor_list; d != NULL; d = d->next)
                {
                        if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch))
			{
				if (!IS_IMMORTAL(ch))
				{
					ch->pcdata->wp_regain_counter += 1;
					ch->pcdata->blood_burn_counter += 1;
					ch->pcdata->canbuycounter += 1;
					
					if (ch->pcdata->covenantstatus < -200)
						ch->pcdata->covenantstatus = -200;
					
				}
				/*if ((!IS_IMMORTAL(ch)) && (IS_MORE2 (ch, MORE2_RPFLAG))){
					ch->pcdata->canvotecounter += 1;
					ch->pcdata->auto_vote_counter += 1;
				}
				if (!IS_NPC(ch) && (ch->pcdata->canvotecounter >= 3)){
					send_to_char ("#cYou can now cast a roleplay vote.#n\n\r", ch);
				}*/	
				if ((!IS_IMMORTAL(ch)) && (IS_MORE2 (ch, MORE2_RPFLAG)) && IS_CHANGELING(ch) && (ch->pcdata->statusvotecounter <= 7))
					ch->pcdata->statusvotecounter += 1;
			}
		}	
	}

	if (iDelete == 0)
		iDelete++;
	else if (iDelete == 1)
	{
		unlink (CRASH_TEMP_FILE);
		iDelete++;
	}
	tail_chain ();
	return;
}



void do_force_char_update(CHAR_DATA * ch, char *argument)
{
	char_update ();
	send_to_char("char_update() complete", ch);
}

void msdp_update( void )
{
    DESCRIPTOR_DATA *d;
    int PlayerCount = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
	if ( d->character && d->connected == CON_PLAYING && !IS_NPC(d->character) )
        {
            char buf[MAX_STRING_LENGTH];
            CHAR_DATA *pOpponent = d->character->fighting;
            ROOM_INDEX_DATA *pRoom = d->character->in_room;
            AFFECT_DATA *paf;

            ++PlayerCount;

            MSDPSetString( d, eMSDP_CHARACTER_NAME, d->character->name );
            MSDPSetNumber( d, eMSDP_ALIGNMENT, d->character->alignment );
            MSDPSetNumber( d, eMSDP_EXPERIENCE, d->character->exp );
//            MSDPSetNumber( d, eMSDP_EXPERIENCE_MAX, exp_per_level(d->character, 
//               d->character->pcdata->points)  ); 
//            MSDPSetNumber( d, eMSDP_EXPERIENCE_TNL, ((d->character->level + 1) *
//               exp_per_level(d->character, d->character->pcdata->points) - 
//               d->character->exp ) );

            MSDPSetNumber( d, eMSDP_HEALTH, d->character->hit );
            MSDPSetNumber( d, eMSDP_HEALTH_MAX, d->character->max_hit );
            MSDPSetNumber( d, eMSDP_LEVEL, d->character->level );
/*
            MSDPSetNumber( d, eMSDP_RACE, TBD );
            MSDPSetNumber( d, eMSDP_CLASS, TBD );
*/
            MSDPSetNumber( d, eMSDP_MANA, d->character->mana );
            MSDPSetNumber( d, eMSDP_MANA_MAX, d->character->max_mana );
            MSDPSetNumber( d, eMSDP_WIMPY, d->character->wimpy );
//            MSDPSetNumber( d, eMSDP_PRACTICE, d->character->practice );
            MSDPSetNumber( d, eMSDP_MONEY, d->character->pounds );
            MSDPSetNumber( d, eMSDP_MOVEMENT, d->character->move );
            MSDPSetNumber( d, eMSDP_MOVEMENT_MAX, d->character->max_move );
            MSDPSetNumber( d, eMSDP_HITROLL, GET_HITROLL(d->character) );
            MSDPSetNumber( d, eMSDP_DAMROLL, GET_DAMROLL(d->character) );
            MSDPSetNumber( d, eMSDP_AC, GET_AC(d->character) );
            MSDPSetNumber( d, eMSDP_STR, get_curr_str(d->character) );
            MSDPSetNumber( d, eMSDP_INT, get_curr_int(d->character) );
//            MSDPSetNumber( d, eMSDP_WIS, get_curr_wis(d->character) );
            MSDPSetNumber( d, eMSDP_DEX, get_curr_dex(d->character) );
//            MSDPSetNumber( d, eMSDP_CON, get_curr_con(d->character) );
            MSDPSetNumber( d, eMSDP_STR_PERM, d->character->pcdata->perm_str );
            MSDPSetNumber( d, eMSDP_INT_PERM, d->character->pcdata->perm_int );
            MSDPSetNumber( d, eMSDP_WIS_PERM, d->character->pcdata->perm_wis );
            MSDPSetNumber( d, eMSDP_DEX_PERM, d->character->pcdata->perm_dex );
            MSDPSetNumber( d, eMSDP_CON_PERM, d->character->pcdata->perm_con );

            /* This would be better moved elsewhere */
            if ( pOpponent != NULL )
            {
                int hit_points = (pOpponent->hit * 100) / pOpponent->max_hit;
                MSDPSetNumber( d, eMSDP_OPPONENT_HEALTH, hit_points );
                MSDPSetNumber( d, eMSDP_OPPONENT_HEALTH_MAX, 100 );
                MSDPSetNumber( d, eMSDP_OPPONENT_LEVEL, pOpponent->level );
                MSDPSetString( d, eMSDP_OPPONENT_NAME, pOpponent->name );
            }
            else /* Clear the values */
            {
                MSDPSetNumber( d, eMSDP_OPPONENT_HEALTH, 0 );
                MSDPSetNumber( d, eMSDP_OPPONENT_LEVEL, 0 );
                MSDPSetString( d, eMSDP_OPPONENT_NAME, "" );
            }

            /* Only update room stuff if they've changed room */
            if ( pRoom && pRoom->vnum != d->pProtocol->pVariables[eMSDP_ROOM_VNUM]->ValueInt )
            {
                int i; /* Loop counter */
                buf[0] = '\0';

                for ( i = DIR_NORTH; i < MAX_DIR; ++i )
                {
                    if ( pRoom->exit[i] != NULL )
                    {
                        const char MsdpVar[] = { (char)MSDP_VAR, '\0' };
                        const char MsdpVal[] = { (char)MSDP_VAL, '\0' };
                        extern char *const dir_name[];

                        strcat( buf, MsdpVar );
                        strcat( buf, dir_name[i] );
                        strcat( buf, MsdpVal );

                        if ( IS_SET(pRoom->exit[i]->exit_info, EX_CLOSED) )
                            strcat( buf, "C" );
                        else /* The exit is open */
                            strcat( buf, "O" );
                    }
                }

                if ( pRoom->area != NULL )
                    MSDPSetString( d, eMSDP_AREA_NAME, pRoom->area->name );

                MSDPSetString( d, eMSDP_ROOM_NAME, pRoom->name );
                MSDPSetTable( d, eMSDP_ROOM_EXITS, buf );
                MSDPSetNumber( d, eMSDP_ROOM_VNUM, pRoom->vnum );
            }
/*
            MSDPSetNumber( d, eMSDP_WORLD_TIME, d->character-> );
*/

            buf[0] = '\0';
            for ( paf = d->character->affected; paf; paf = paf->next )
            {
                char skill_buf[MAX_STRING_LENGTH];
                sprintf( skill_buf, "%c%s%c%d",
                    (char)MSDP_VAR, skill_table[paf->type].name, 
                    (char)MSDP_VAL, paf->duration );
                strcat( buf, skill_buf );
            }
            MSDPSetTable( d, eMSDP_AFFECTS, buf );

            MSDPUpdate( d );
        }
    }

    /* Ideally this should be called once at startup, and again whenever 
     * someone leaves or joins the mud.  But this works, and it keeps the 
     * snippet simple.  Optimise as you see fit.
     */
    MSSPSetPlayers( PlayerCount );
}
