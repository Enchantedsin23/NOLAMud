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
#include <unistd.h>		/* unlink */
#include "merc.h"
#include <pthread.h>


/*
 * Local functions.
 */
void mobile_update args ((void));
void weather_update args ((void));
void char_update args ((void));
void obj_update args ((void));
void *weather_thread args ((void *arg));

/* Weather Code */

int weatherwind_spd = 0;
int weatherclouds = 0;
int weatherwind_dir = 0;
int weathertemp = 0;



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
					if (IS_VAMPIRE (vch) && IS_MORE2 (vch, MORE2_CRIMSONFURY) && number_range (0, 50) > 40)
					{
						send_to_char ("You feel a burning rage spread within you!\n\r", ch);
						SET_BIT (ch->more, MORE_ANGER);
						ch->pcdata->wolf += 10;
					}


					if (IS_SET (vch->act, ACT_ANIMAL) && IS_VAMPIRE (ch) && get_disc (ch, DISC_BARDO) < 3)
					{
						amount /= 0.5;
						act ("You find you do not get as much nourishment from $N since its an animal.", ch, NULL, vch, TO_CHAR);
					}
					act ("#cYou shudder with ecstacy as you drain $N's lifeblood from $S throat.", ch, NULL, vch, TO_CHAR);
					act ("#cThe gentle motion of $n's lips on your throat sends electric shivers through your body.", ch, NULL, vch, TO_VICT);
					act ("#c$n drinks deeply from $N's throat.", ch, NULL, vch, TO_NOTVICT);
					if (vch->position == POS_STUNNED)
						update_pos (vch);
					if (IS_VAMPIRE (vch) && !str_cmp (vch->clan, "Lamia"))
						send_to_char ("You find the blood tastes somewhat odd....(ask imm for dice check for drinking the blood of a Lamia\n\r", ch);

					if (!IS_NPC (vch) && ((IS_WEREWOLF (vch) && get_tribe (vch, TRIBE_GET_OF_FENRIS) > 0)))
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
							act ("$n keels over and collapses on the floor.", ch, NULL, NULL, TO_ROOM);
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
						if (!IS_VAMPIRE (vch))
						{
							act ("#C$N sways weakly in your arms.", ch, NULL, vch, TO_CHAR);
							act ("#CYou sway weakly in $n's arms.", ch, NULL, vch, TO_VICT);
							act ("#C$N sways weakly in $n's arms.", ch, NULL, vch, TO_NOTVICT);
						}
						if (vch->blood[BLOOD_CURRENT] <= 5)
						{
							if (IS_VAMPIRE (vch))
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
						if (!IS_NPC (vch) && IS_VAMPIRE (vch))
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
							ch->pcdata->diableries[DIAB_TIME] += 10000;
							ch->pcdata->diableries[DIAB_EVER]++;
							ch->beast += 20;
							ch->truebeast += 10;
							if (ch->beast > 100)
								ch->beast = 100;
							if (ch->truebeast > 100)
								ch->truebeast = 100;
							if (is_affected (vch, gsn_potency))
								affect_strip (vch, gsn_potency);
							if (vch->vampgen < ch->vampgen && vch->vampgen > 0)
							{
								/* sprintf (buf, "%s has been diablerised by %s.", vch->name, ch->name);
								do_info (ch, buf); */
								ch->vampgen--;
								update_gen (ch);
								update_gen (vch);
								send_to_char ("You have lowered a generation!\n\r", ch);
							}
							else
							{
								/* sprintf (buf, "%s has been diablerised by %s for no generation.", vch->name, ch->name);
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

			if (IS_VAMPIRE (ch))
			{
				
				if (ch->pcdata->wolf < 1)
				{
					if (ch->pcdata->regenerate > 0)
					{
						if (ch->blood[BLOOD_CURRENT] < 1 && ch->hit > 0)
						{
							send_to_char ("#cYou don't have enough blood to regenerate.\n\r", ch);
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
			else if (IS_WEREWOLF (ch))
			{
				if (ch->position == POS_FIGHTING && !IS_ITEMAFF (ch, ITEMA_RAGER))
				{
					if (ch->pcdata->wolf < 300)
					{
						if (get_tribe (ch, TRIBE_GET_OF_FENRIS) > 4)
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
		if (ch->hit < ch->max_hit / 2 && (door = number_bits (3)) <= 5 && (pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL && !IS_AFFECTED (ch, AFF_WEBBED) && ch->level < 900 && !IS_SET (pexit->exit_info, EX_CLOSED) && !IS_SET (pexit->to_room->room_flags, ROOM_NO_MOB))
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



/*
 * Update the weather.
 */
void weather_update (void)
{
	FILE *weather;
	char wind_d[5],cloud_a[20],cloud_b[20];
	int wind_s,t,check,cloudaval,cloudbval = 0;


	weather=fopen("weather/weather.txt","r");
	if(weather==NULL){
		puts("Error opening first file.");
		exit(0);
	}
	fscanf(weather,"time: %*d:%*d\n");
	fscanf(weather,"wind_dir: %s\n",&wind_d);
	fscanf(weather,"wind_spd: %d\n",&wind_s);
	check=fscanf(weather,"clouds: %s %*d %s %*d\n",&cloud_a,&cloud_b);
	printf("check = %d\n",check);
	fscanf(weather,"temp: %d C",&t);
	fclose(weather);

	
	if (!str_cmp (wind_d, "SW"))
		weatherwind_dir = 1;
	if (!str_cmp (wind_d, "S"))
		weatherwind_dir = 2;
	if (!str_cmp (wind_d, "SE"))
		weatherwind_dir = 3;
	if (!str_cmp (wind_d, "W"))
		weatherwind_dir = 4;
	if (!str_cmp (wind_d, "CALM"))
		weatherwind_dir = 5;
	if (!str_cmp (wind_d, "E"))
		weatherwind_dir = 6;
	if (!str_cmp (wind_d, "NW"))
		weatherwind_dir = 7;
	if (!str_cmp (wind_d, "N"))
		weatherwind_dir = 8;
	if (!str_cmp (wind_d, "NE"))
		weatherwind_dir = 9;
	if (!str_cmp (wind_d, "VAR"))
		weatherwind_dir = 10;
	
//	while(strcmp(wind_d,windarray[count])!=0)
//		count++;
//	direction=count+1;
	
	weatherwind_spd=wind_s;
	

	
	if (!str_cmp (cloud_a, "CLEAR"))
		cloudaval = 1;
	if (!str_cmp (cloud_a, "FEW"))
		cloudaval = 2;
	if (!str_cmp (cloud_a, "SCATTERED"))
		cloudaval = 3;
	if (!str_cmp (cloud_a, "BROKEN"))
		cloudaval = 4;
	if (!str_cmp (cloud_a, "OVERCAST"))
		cloudaval = 5;
		
	if (!str_cmp (cloud_b, "CLEAR"))
		cloudbval = 1;
	if (!str_cmp (cloud_b, "FEW"))
		cloudbval = 2;
	if (!str_cmp (cloud_b, "SCATTERED"))
		cloudbval = 3;
	if (!str_cmp (cloud_b, "BROKEN"))
		cloudbval = 4;
	if (!str_cmp (cloud_b, "OVERCAST"))
		cloudbval = 5;
	
	if (cloudaval >= cloudbval)
		weatherclouds = cloudaval;
	else
		weatherclouds = cloudbval;
		
	
	
//	count=0;
	
//	while(strcmp(cloud_a,cloudarray[count])!=0)
//		count++;
//	sky=count+1;
	
	weathertemp=t;
	

	return;
	
	
	
}	
	
	/*
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ch = NULL;
	int diff;
	bool char_up;
	bool mes_all = FALSE;

	for (d = descriptor_list; d != NULL; d = d->next)
	{
		if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_OSWITCH))
		{
		
			if (ch->pcdata->willpower[0] > ch->pcdata->willpower[1])
				ch->pcdata->willpower[0] = ch->pcdata->willpower[1];
			if (IS_MORE (ch, MORE_INFORM))
				REMOVE_BIT (ch->more, MORE_INFORM);
			if (IS_MORE (ch, MORE_COURAGE) && number_percent () < 50)
			{
				send_to_char ("You no longer feel quite so brave.\n\r", ch);
				REMOVE_BIT (ch->more, MORE_COURAGE);
			}
			if (IS_MORE (ch, MORE_CALM) && number_percent () < 50)
			{
				send_to_char ("You no longer feel so apathetic.\n\r", ch);
				REMOVE_BIT (ch->more, MORE_CALM);
			}
			if (IS_MORE (ch, MORE_ANGER) && number_percent () < 50 && ch->pcdata->wolf < 1)
			{
				send_to_char ("You no longer feel so angry.\n\r", ch);
				REMOVE_BIT (ch->more, MORE_ANGER);
			}


			// Deimos thing
			if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEIMOSWHISPERS) && number_percent () < 25)
			{
				send_to_char ("Your dread seems to melt away.\n\r", ch);
				REMOVE_BIT (ch->more2, MORE2_DEIMOSWHISPERS);
			}
			if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEIMOSSANGUIN))
			{
				if (number_percent () < 25)
				{
					send_to_char ("Your orifices stop bleeding.\n\r", ch);
					REMOVE_BIT (ch->more2, MORE2_DEIMOSSANGUIN);
				}
				else
				{
					if (ch->blood[BLOOD_CURRENT] > 1)
					{
						send_to_char ("Blood seeps out of your eyes, mouth and ears!\n\r", ch);
						act ("$n throws up blood while it also pours out their eyes and ears", ch, NULL, NULL, TO_ROOM);
						ch->blood[BLOOD_CURRENT] -= 1;
					}

				}
			}

			if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEMENTATION6))
			{
				if (ch->hit > 0)
				{
					send_to_char ("You start throwing yourself around the room banging into things!\n\r", ch);
					act ("$n babbles obsenities and throws themselves full speed at the walls.", ch, NULL, NULL, TO_ROOM);
					ch->hit -= number_range (1, 2);
				}
				if (ch->hit < -10)
					ch->hit = -10;

			}
			else if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEMENTATION9))
			{
				if (ch->hit > 0)
				{
					send_to_char ("You start throwing yourself around the room banging your head against the wall!\n\r", ch);
					act ("$n babbles obsenities and throws themselves full speed at the walls.", ch, NULL, NULL, TO_ROOM);
					ch->hit -= number_range (1, 2);
				}
				if (ch->hit < -10)
					ch->hit = -10;
			}


			if (IS_VAMPIRE (ch) && IS_MORE2 (ch, MORE2_CHISTOP) && ch->blood[BLOOD_CURRENT] < 1)
			{
				send_to_char ("All the tainted blood has been cleared..you can once again collect Blood.\n\r", ch);
				REMOVE_BIT (ch->more2, MORE2_CHISTOP);
			}



			if (IS_MORE2 (ch, MORE2_BLACKDEATH))
			{
				send_to_char ("You heave and cough up large amounts of blood....you feel like you are dying.\n\r", ch);
			}


			if (!IS_MORE2 (ch, MORE2_PASSION) && IS_MORE (ch, MORE_COURAGE) && number_percent () < 50)
			{
				send_to_char ("You no longer feel quite so brave.\n\r", ch);
				REMOVE_BIT (ch->more, MORE_COURAGE);
			}

			if (!IS_MORE2 (ch, MORE2_PASSION) && IS_MORE (ch, MORE_CALM) && number_percent () < 50)
			{
				send_to_char ("You no longer feel so apathetic.\n\r", ch);
				REMOVE_BIT (ch->more, MORE_CALM);
			}
			if (!IS_MORE2 (ch, MORE2_PASSION) && IS_MORE (ch, MORE_ANGER) && number_percent () < 50 && ch->pcdata->wolf < 1)
			{
				send_to_char ("You no longer feel so angry.\n\r", ch);
				REMOVE_BIT (ch->more, MORE_ANGER);
			}

			if (IS_MORE2 (ch, MORE2_SENSORYDEPRIVED) && number_percent () < 75)
			{
				send_to_char ("All your senses return to you.\n\r", ch);
				REMOVE_BIT (ch->more2, MORE2_SENSORYDEPRIVED);
			}

		}
	}
	if (IS_SET (world_affects, WORLD_FOG) && number_range (1, 5) == 1)
	{
		strcpy (buf, "#cThe thick #efog#c melts away.\n\r");
		REMOVE_BIT (world_affects, WORLD_FOG);
		mes_all = TRUE;
	}
	else if (IS_SET (world_affects, WORLD_RAIN) && number_range (1, 5) == 1)
	{
		strcpy (buf, "#cThe #brain#c begins tapers off as thunder rolls in the distance.\n\r");
		REMOVE_BIT (world_affects, WORLD_RAIN);
		mes_all = TRUE;
	}
	if (mes_all)
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_OSWITCH) && ch->in_room != NULL && ch->in_room->sector_type != SECT_INSIDE && IS_OUTSIDE (ch))
				send_to_char (buf, ch);
		}
	}
	buf[0] = '\0';
	switch (++time_info.hour)
	{
	case 4:
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_OSWITCH) && IS_VAMPIRE (ch))
			{
				//send_to_char ("You feel weary as the night begins to come to an end.\n\r", ch);
			}
		}
		break;

	case  5:
	break;

	case 7:
	break;

	case 8:
		break;



		for (d = descriptor_list; d != NULL; d = d->next)
		{

			if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_OSWITCH) && !IS_CLASS (ch, CLASS_VAMPIRE) && IS_MORE2 (ch, MORE2_BLACKDEATH))
			{
				send_to_char ("You cough and hack one last time just before the sun goes down..then you fall over dead!\n\r", ch);
				ch->form = 16;
				raw_kill (ch, ch);
				REMOVE_BIT (ch->more2, MORE2_BLACKDEATH);
			}
		}


		break;


	case 12:
		break;

   	case 17:
		break;


	case 18:
		break;


	case 20:
		break;

	case 21:
		break;

	case 22:
		break;

	case 23:
		break;

	case 24:
		time_info.hour = 0;
		time_info.day++;
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			char_up = FALSE;
			if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_OSWITCH))
			{
				if (IS_MORE (ch, MORE_NOPAIN))
				{
					REMOVE_BIT (ch->more, MORE_NOPAIN);
					send_to_char ("You no longer feel so oblivious to pain.\n\r", ch);
				}
				if (IS_VAMPIRE (ch))
				{
					if (ch->hit < ch->max_hit)
					{
						ch->hit = ch->max_hit;
						char_up = TRUE;
					}
					if (ch->move < ch->max_move)
					{
						ch->move = ch->max_move;
						char_up = TRUE;
					}
					ch->loc_hp[0] = 0;
					ch->loc_hp[1] = 0;
					ch->loc_hp[2] = 0;
					ch->loc_hp[3] = 0;
					ch->loc_hp[4] = 0;
					ch->loc_hp[5] = 0;
					ch->loc_hp[6] = 0;
				}
			}
		}
		break;

		// Gobbo Time ************
	case 9:
	case 10:
	case 11:
	case 13:
	case 14:
	case 15:
	case 16:
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_OSWITCH) && IS_VAMPIRE (ch))
			{

				if (IS_OUTSIDE (ch) && !IS_SET (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
				{
					if (IS_MORE3 (ch, MORE3_DEATHSWHISPER))
						send_to_char ("In your present form you avoid the sunlight.\n\r", ch);
					else if (IS_VAMPAFF (ch, VAM_EARTHMELDED))
						send_to_char ("You escape the sunlight in your present form.\n\r", ch);
				}
			}

		}
		break;
		// Gobbo End  ************

	}

	if (time_info.day >= 31)
	{
		time_info.day = 0;
		time_info.month++;
	}

	if (time_info.month >= 12)
	{
		time_info.month = 0;
		time_info.year++;
	}
*/

	/*
	 * Weather change.
	 */
	/* Begin Modified Weather Code */
	
	/*if (time_info.month >= 9 && time_info.month <= 12)
		diff = weather_info.mmhg > 985 ? -2 : 2;
	else
		diff = weather_info.mmhg > 1015 ? -2 : 2;

	weather_info.change += diff * dice (1, 4) + dice (3, 4) - dice (3, 4);
	weather_info.change = UMAX (weather_info.change, -12);
	weather_info.change = UMIN (weather_info.change, 12);

	weather_info.mmhg += weather_info.change;
	weather_info.mmhg = UMAX (weather_info.mmhg, 960);
	weather_info.mmhg = UMIN (weather_info.mmhg, 1060);

	switch (weather_info.sky)
	{
	default:
		bug ("Weather_update: bad sky %d.", weather_info.sky);
		weather_info.sky = SKY_CLOUDLESS;
		break;

	case SKY_CLOUDLESS:
		if (weather_info.mmhg < 1025 || (weather_info.mmhg < 1045 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe #wclouds#c pass by and a few rays of light brighten the area.\n\r");
			weather_info.sky = SKY_SPARSE_CLOUDS;
		}
		break;

	case SKY_CLOUDY:
		if (weather_info.mmhg < 975 || (weather_info.mmhg < 995 && number_bits (2) == 0))
		{
			strcat (buf, "#cSome #wclouds#c drift across the sky, casting dark shadows.\n\r");
			weather_info.sky = SKY_DRIZZLE;
			SET_BIT(world_affects, WORLD_RAIN);
		}

		if (weather_info.mmhg > 1030 || (weather_info.mmhg > 1020 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe #wcloud#c cover lessens somewhat.\n\r");
			weather_info.sky = SKY_MOSTLY_CLOUDY;
		}
		break;

	case SKY_RAINING:
		if (IS_SET (world_affects, WORLD_RAIN))
		{
			strcat (buf, "#cA light #brain#c begins to fall from the cloud shrouded sky.\n\r");
		}
		
		if (weather_info.mmhg < 967 || (weather_info.mmhg < 975 && number_bits (2) == 0))
		{
			strcat (buf, "#yLightning#c flashes and the #brain#c suddenly picks up, hammering the ground with vicious intent.\n\r");
			weather_info.sky = SKY_HEAVY_RAIN;
		}

		if (weather_info.mmhg > 1010 || (weather_info.mmhg > 990 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe fall of #brain#c slows to a light drizzle as the heavy storm moves off.\n\r");
			weather_info.sky = SKY_DRIZZLE;
		}
		break;

	case SKY_LIGHTNING:
		if (IS_SET (world_affects, WORLD_LIGHTNING))
		{
			strcat (buf, "#cWith a peal of #ethunder#c the #bdrizzle#c picks up to a heavy #brain#c.\n\r");
		}
		
		if (weather_info.mmhg > 985 || (weather_info.mmhg > 973 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe #brain#c slows into a light mist, making the air cool, but sticky.\n\r");
			weather_info.sky = SKY_HEAVY_RAIN;
			REMOVE_BIT(world_affects, WORLD_LIGHTNING);
			SET_BIT(world_affects, WORLD_RAIN);
		}
		break;
		
	case SKY_HEAVY_RAIN:
		if (IS_SET (world_affects, WORLD_RAIN))
		{
			strcat (buf, "#bRain#c falls from the sky, soaking the ground as #ylightning#c flashes and #ethunder#c rolls.\n\r");
		}
		
		if (weather_info.mmhg < 965)
		{
			strcat (buf, "#bRain#c falls hard and fast in the blasting wind, creating a dizzying swirl of water!\n\r");
			weather_info.sky = SKY_LIGHTNING;
			SET_BIT(world_affects, WORLD_LIGHTNING);
		}

		if (weather_info.mmhg > 1010 || (weather_info.mmhg > 990 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe heavy #brain#c begins to subside.\n\r");
			weather_info.sky = SKY_RAINING;
		}
		break;
		
	case SKY_DRIZZLE:
		if (IS_SET (world_affects, WORLD_RAIN))
		{
			strcat (buf, "#cThe rain ceases and the air becomes sticky and humid.\n\r");
		}
		
		if (weather_info.mmhg < 975 || (weather_info.mmhg < 985 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe hot, humid air carries a sick green smell from the nearby swamp lands.\n\r");
			weather_info.sky = SKY_RAINING;
		}
		
		if (weather_info.mmhg > 1030 || (weather_info.mmhg > 1010 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe wind dies down leaving only thick gray clouds and foul weather behind.\n\r");
			weather_info.sky = SKY_CLOUDY;
			REMOVE_BIT(world_affects, WORLD_RAIN);
		}
		break;
		
	case SKY_MOSTLY_CLOUDY:
		if (weather_info.mmhg < 990 || (weather_info.mmhg < 1010 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe clouds thicken and the sky is shrouded in darkness as a hot wind blows off the gulf to the south.\n\r");
			weather_info.sky = SKY_CLOUDY;
		}

		if (weather_info.mmhg > 1040 || (weather_info.mmhg > 1030 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe clouds begin to disperse.\n\r");
			weather_info.sky = SKY_PARTLY_CLOUDY;
		}
		break;
	
	case SKY_PARTLY_CLOUDY:
		if (weather_info.mmhg < 1000 || (weather_info.mmhg < 1020 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe clouds thicken and light only peeks through from time to time.\n\r");
			weather_info.sky = SKY_MOSTLY_CLOUDY;
		}

		if (weather_info.mmhg > 1050 || (weather_info.mmhg > 1045 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe cloud cover begins to disperse and the sky is easily visible.\n\r");
			weather_info.sky = SKY_SPARSE_CLOUDS;
		}
		break;
		
	case SKY_SPARSE_CLOUDS:
		if (weather_info.mmhg < 1010 || (weather_info.mmhg < 1030 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe sounds of people enjoying themselves are carried to you on the wind, as well as the distinctive scent of Bourbon Street.\n\r");
			weather_info.sky = SKY_PARTLY_CLOUDY;
		}

		if (weather_info.mmhg > 1055 || (weather_info.mmhg > 1050 && number_bits (2) == 0))
		{
			strcat (buf, "#cThe sky clears completely, giving you a clean view of the heavens.\n\r");
			weather_info.sky = SKY_CLOUDLESS;
		}
		break;
	}

	if (buf[0] != '\0')
	{
		for (d = descriptor_list; d != NULL; d = d->next)
		{
			if (d->connected == CON_PLAYING && IS_OUTSIDE (d->character) && IS_AWAKE (d->character))
				send_to_char (buf, d->character);
		}
	}

	return;
*/


 /* End Modified Weather Code */


// Check to see if a player will gain exp this tick
bool should_gain_exp(CHAR_DATA * ch)
{
	if (ch->pcdata->votes > 5)
		return TRUE;
	else
		return FALSE;
}

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
	char buf[MAX_STRING_LENGTH];

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
		if (!IS_NPC(ch))
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
		}

/* Blood Code */
		/* (uncomment to enable automatic blood burns)
		if (!IS_NPC(ch) && ch->pcdata->blood_burn_counter >= 6)
		{
			if (IS_VAMPIRE(ch))
			{
				
				if (!strcmp(ch->clan, "Ghoul"))  
					send_to_char ("#cYou crave the blood of a vampire.\n\r", ch);
				else if(ch->blood[BLOOD_CURRENT] < 3)
					send_to_char ("#CYou're running dangerously low on blood. You must hunt soon.\n\r", ch);
				else
				{
					ch->blood[BLOOD_CURRENT] = ch->blood[BLOOD_CURRENT] - 1;
					send_to_char ("#cYou burned one blood to awaken this evening.\n\r", ch);
				}
			}
	
			ch->pcdata->blood_burn_counter = 0;
		}
		*/


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
				
	if (!IS_NPC(ch) && (ch->pcdata->canvotecounter >= 3))
		{
			
	
				//send_to_char ("#cYou can now cast a roleplay vote.#n\n\r", ch);
			  ch->pcdata->canvotecounter = 3;
		}	  
/* Auto Vote Code */
/*
		if (!IS_NPC(ch) && ch->pcdata->auto_vote_counter >= 4)
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

			if (!IS_VAMPIRE (ch))
			{
			}
			else
			{
				if (ch->hit > 0 && ch->blood[BLOOD_CURRENT] < 1)
				{
					send_to_char ("You are DYING from lack of blood!\n\r", ch);
					act ("$n gets a hungry look in $s eyes.", ch, NULL, NULL, TO_ROOM);

					if (ch->hit > 0)
						ch->hit -= number_range (2, 5);

					update_pos (ch);

					if (!IS_VAMPAFF (ch, VAM_FANGS))
						do_fangs (ch, "");
				}
				else if (ch->hit > 0 && ch->blood[BLOOD_CURRENT] <= 2)
				{
					send_to_char ("You crave blood.\n\r", ch);

					if (number_percent () > (ch->blood[BLOOD_CURRENT] + 5) && !IS_VAMPAFF (ch, VAM_FANGS))
						do_fangs (ch, "");
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
		/*if (ch->loc_hp[6] < 1 && !is_obj && ch->in_room != NULL && !IS_VAMPIRE (ch))
		{
			if (ch->blood[0] < 10)
				ch->blood[0]++;
		}*/
		if (ch->loc_hp[6] > 0 && !is_obj && ch->in_room != NULL && !IS_VAMPIRE (ch))
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
			if (IS_NPC (ch) || (get_disc (ch, DISC_DAIMOINON) < 6))
			{
				if (!IS_NPC (ch) && (IS_VAMPIRE (ch) || IS_GHOUL (ch)) && get_disc (ch, DISC_FORTITUDE) > 0)
				{
					int per_red = 0;
					switch (get_disc (ch, DISC_FORTITUDE))
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

/*
 * This is a system call, in a separate thread,
 * to the perl script which updates the weather.txt file.
 */

void *weather_thread(void *arg)
{
	// The four letter METAR id must be included in all caps or this won't accomplish much
        system("../src/get_weather.pl KMSY"); 

        return NULL;
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
	static int pulse_mobile;
	static int pulse_violence;
	static int pulse_point;
	static int pulse_hour;
        DESCRIPTOR_DATA *d;
        CHAR_DATA *ch = NULL;
	char buf[MAX_STRING_LENGTH];
  //pthread_t weather_pthread; // Thread to update weather.txt


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
						send_to_char ("You are once more able to use Blood Agony.\n\r", ch);
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
				if (ch->pcdata->necromancy > 0)
				{
					ch->pcdata->necromancy--;
					if (ch->pcdata->necromancy < 1)
						send_to_char ("Your powers of Necromancy have replenished themselves.\n\r", ch);
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
			

				if (ch->pcdata->dementation > 0)
				{
					ch->pcdata->dementation--;
					if (ch->pcdata->dementation < 1)
					{
						if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_HAUNTED))
						{
							send_to_char ("The visions thankfully fade from view.\n\r", ch);
							REMOVE_BIT (ch->more2, MORE2_HAUNTED);
						}
						if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEMENTATION2))
						{
							send_to_char ("Those annoying visions thankfully fade from view.\n\r", ch);
							REMOVE_BIT (ch->more2, MORE2_DEMENTATION2);
						}
						if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEMENTATION4))
						{
							send_to_char ("You shake your head and begin to think clearly again.\n\r", ch);
							REMOVE_BIT (ch->more2, MORE2_DEMENTATION4);
						}
						if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_DEMENTATION7))
						{
							send_to_char ("You shake your head and begin to think clearly again.\n\r", ch);
							REMOVE_BIT (ch->more2, MORE2_DEMENTATION7);
						}
						if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_COMA))
						{
							send_to_char ("You awaken out of your coma.\n\r", ch);
							REMOVE_BIT (ch->more2, MORE2_COMA);
						}


					}
				}


				if (ch->pcdata->obeah > 0 && !IS_MORE (ch, MORE_NEUTRAL))
				{
					ch->pcdata->obeah--;
					if (ch->pcdata->obeah < 1)
					{
						if (IS_WEREWOLF (ch))
							send_to_char ("Your healing powers have returned.\n\r", ch);
						else
						{
							send_to_char ("Your third eye stops glowing.\n\r", ch);
							act ("The eye in $n's forehead stops glowing.", ch, NULL, NULL, TO_ROOM);
						}
					}
				}
				if (ch->pcdata->darkness > 0)
				{
					ch->pcdata->darkness--;
					if (ch->pcdata->darkness < 1)
					{
						if (IS_MORE (ch, MORE_DARKNESS))
						{
							act ("The darkness in the room gradually fades away to nothing.", ch, NULL, NULL, TO_CHAR);
							act ("The darkness in the room gradually fades away to nothing.", ch, NULL, NULL, TO_ROOM);
							REMOVE_BIT (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
							REMOVE_BIT (ch->more, MORE_DARKNESS);
							ch->pcdata->darkness = 60;
						}
						else if (IS_MORE2 (ch, MORE2_DRAENORTENTICLES))
						{
							act ("Your shadow Tendrils fade from view", ch, NULL, NULL, TO_CHAR);
							REMOVE_BIT (ch->more2, MORE2_DRAENORTENTICLES);
							ch->pcdata->darkness = 60;
						}
						else
							send_to_char ("Your ability to use the shadows has returned.\n\r", ch);
					}

				}
				if (ch->pcdata->reina > 0)
				{
					ch->pcdata->reina--;
					if (ch->pcdata->reina < 1)
					{
						if (IS_MORE (ch, MORE_REINA))
						{
							send_to_char ("The peaceful harmony in the air gradually fades away to nothing.\n\r", ch);
							act ("The peaceful harmony in the air gradually fades away to nothing.", ch, NULL, NULL, TO_ROOM);
							REMOVE_BIT (ch->in_room->room_flags, ROOM_SAFE);
							REMOVE_BIT (ch->more, MORE_REINA);
							ch->pcdata->reina = 60;
						}
						else
							send_to_char ("You have recovered your voice.\n\r", ch);
					}
				}
				if (ch->pcdata->majesty > 0)
				{
					ch->pcdata->majesty--;
					if (ch->pcdata->majesty < 1)
					{
						if (IS_WEREWOLF (ch))
							send_to_char ("You can now use your Icy Chill of Despair again if you wish.\n\r", ch);
						else
							send_to_char ("You can now raise your Majesty again if you wish.\n\r", ch);
					}
				}
				if (ch->pcdata->temporis > 0)
				{
					ch->pcdata->temporis--;
					if (ch->pcdata->temporis < 1)
						send_to_char ("Your powers of time control have replenished themselves.\n\r", ch);
				}
				if (ch->pcdata->devil > 0)	//abombwe thing
				{
					ch->pcdata->devil--;
					if (ch->pcdata->devil < 1)
						send_to_char ("You feel the devils blood run try.\n\r", ch);
				}

				if (ch->pcdata->firstaid > 0)
					ch->pcdata->firstaid--;
				if (ch->pcdata->repair > 0)
					ch->pcdata->repair--;
				if (ch->pcdata->evileye > 0)
					ch->pcdata->evileye--;
				if (ch->pcdata->diableries[DIAB_TIME] > 0)
					ch->pcdata->diableries[DIAB_TIME]--;

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
				if (ch->pcdata->resist[WILL_DOMINATE] > 0)
				{
					ch->pcdata->resist[WILL_DOMINATE]--;
					if (ch->pcdata->resist[WILL_DOMINATE] < 1)
					{
						send_to_char ("You are no longer immune to being Dominated.\n\r", ch);
						if (!IS_WILLPOWER (ch, RES_TOGGLE))
							REMOVE_BIT (ch->pcdata->resist[0], RES_DOMINATE);
					}
				}
				if (ch->pcdata->resist[WILL_PRESENCE] > 0)
				{
					ch->pcdata->resist[WILL_PRESENCE]--;
					if (ch->pcdata->resist[WILL_PRESENCE] < 1)
					{
						send_to_char ("You are no longer immune to Dread Gaze and Entrancement.\n\r", ch);
						if (!IS_WILLPOWER (ch, RES_TOGGLE))
							REMOVE_BIT (ch->pcdata->resist[0], RES_PRESENCE);
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

	if (--pulse_mobile <= (PULSE_MOBILE >> 1))
	{
		if (pulse_mobile <= 0)
		{
			pulse_mobile = PULSE_MOBILE;
			mobile_update ();
			obj_update ();
		}
		else if (pulse_mobile == (PULSE_MOBILE >> 1))
			obj_update ();
	}

	if (--pulse_violence <= 0)
	{
		pulse_violence = PULSE_VIOLENCE;
	}

	if (--pulse_point <= 0)
	{
		pulse_point = number_range (PULSE_TICK / 2, 3 * PULSE_TICK / 2);
		weather_update ();
		char_update ();
	}

	if (--pulse_hour <= 0)
	{
		pulse_hour = 3600 * PULSE_PER_SECOND;
		
		/* Weather Thread. Comment out the following line if the thread is messing shit up */
		//pthread_create(&weather_pthread,NULL,weather_thread,"foo"); // Start the weather file update
		

                for (d = descriptor_list; d != NULL; d = d->next)
                {
                        if (d->connected == CON_PLAYING && (ch = d->character) != NULL && !IS_NPC (ch))
			{
				if (!IS_IMMORTAL(ch))
				{
					ch->pcdata->wp_regain_counter += 1;
					//ch->pcdata->blood_burn_counter += 1;    //uncomment for automatic blood burns
					ch->pcdata->canbuycounter += 1;
					
					if (ch->pcdata->sectstatus < -200)
						ch->pcdata->sectstatus = -200;
					
				}
				/*
				if ((!IS_IMMORTAL(ch)) && (IS_MORE2 (ch, MORE2_RPFLAG))){
					ch->pcdata->canvotecounter += 1;
					sprintf(buf, "%s last pose: %d.", ch->name, (int) (current_time - ch->last_pose));
					logchan(buf, CHANNEL_LOG_NORMAL);
					if ((int) (current_time - ch->last_pose) <= 1800)
					{
						logchan("Check succeeded.", CHANNEL_LOG_NORMAL);
						ch->pcdata->auto_vote_counter += 1;
					}
				
				}
				*/
				if (!IS_NPC(ch) && (ch->pcdata->canvotecounter >= 3)){
					send_to_char ("#cYou can now cast a roleplay vote.#n\n\r", ch);
				}	
				if ((!IS_IMMORTAL(ch)) && (IS_MORE2 (ch, MORE2_RPFLAG)) && IS_VAMPIRE(ch) && (ch->pcdata->statusvotecounter <= 7))
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

void do_weather (CHAR_DATA * ch, char *argument)
{
/*
        char buf[MAX_STRING_LENGTH];
        char temp_string[MAX_STRING_LENGTH];

        sprintf(buf, "#c");

        if (weather_info.temperature < -5)
                strcat(buf, "The air is #Bbitterly cold#c");
        if ((weather_info.temperature >= -5) && (weather_info.temperature <= 0))
                strcat(buf, "The air is #Bvery cold#c");
        if ((weather_info.temperature >= 1) && (weather_info.temperature <= 9))
                strcat(buf, "The air is #wcold#c");
        if ((weather_info.temperature >= 10) && (weather_info.temperature <= 13))
                strcat(buf, "The air is #wchilly#c");
        if ((weather_info.temperature>= 14) && (weather_info.temperature <= 18))
                strcat(buf, "The air is brisk");
        if ((weather_info.temperature >= 18) && (weather_info.temperature <= 23))
                strcat(buf, "The air is #ywarm#c");
        if ((weather_info.temperature >= 24) && (weather_info.temperature <= 28))
                strcat(buf, "The air #yvery warm#c");
        if ((weather_info.temperature >= 29) && (weather_info.temperature <= 32))
                strcat(buf, "The air is #Rhot#c");
        if ((weather_info.temperature >= 33) && (weather_info.temperature <= 36))
                strcat(buf, "The air is #Rvery hot#c");
        if (weather_info.temperature > 36)
                strcat(buf, "The air is #Rpressively hot#c");

        if (weather_info.wind_speed < 1)
                strcat(buf, " and the wind is calm");
        if ((weather_info.wind_speed >= 1) && (weather_info.wind_speed <=5))
                strcat(buf, " and there is a light breeze");
        if ((weather_info.wind_speed >= 6) && (weather_info.wind_speed <=10))
                strcat(buf, " and there is a light wind");
        if ((weather_info.wind_speed >= 11) && (weather_info.wind_speed <=20))
                strcat(buf, " and the wind is gusting");
        if ((weather_info.wind_speed >= 21) && (weather_info.wind_speed <=30))
                strcat(buf, " and the wind is blowing hard");
        if ((weather_info.wind_speed >= 31) && (weather_info.wind_speed <=40))
                strcat(buf, " and the wind howls");
        if (weather_info.wind_speed > 40)
                strcat(buf, " and the wind is a gale");

        if (weather_info.wind_speed > 1)
        {
                sprintf(temp_string, " from the %s", weather_info.wind_dir);
                strcat(buf, temp_string);
        }

        if (strcmp(weather_info.clouds, "clear") == 0)
                strcat(buf, " under a clear sky.");
        if (strcmp(weather_info.clouds, "scattered") == 0)
                strcat(buf, " under a few scattered clouds.");
        if (strcmp(weather_info.clouds, "few") == 0)
                strcat(buf, " under a cloudy sky.");
        if (strcmp(weather_info.clouds, "broken") == 0)
                strcat(buf, " and there are few breaks in the dark clouds above you.");
        if (strcmp(weather_info.clouds, "overcast") == 0)
                strcat(buf, " and the sky is completely blanketed in dark clouds.");

        if (weather_info.weathercond != NULL)
        {
                sprintf(temp_string, " There is also a %s present.", weather_info.weathercond);
                strcat(buf, temp_string);
        }

        strcat(buf, "#n\n\r");
        send_to_char(buf, ch);

        return;

*/


	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "#c");
	
	if (weathertemp < -5)
		strcat(buf, "The air is #Bbitterly cold#c");
	if ((weathertemp >= -5) && (weathertemp <= 0))
		strcat(buf, "The air is #Bvery cold#c");
	if ((weathertemp >= 1) && (weathertemp <= 9))
		strcat(buf, "The air is #wcold#c");
	if ((weathertemp >= 10) && (weathertemp <= 13))
		strcat(buf, "The air is #wchilly#c");
	if ((weathertemp >= 14) && (weathertemp <= 18))
		strcat(buf, "The air is brisk");
	if ((weathertemp >= 18) && (weathertemp <= 23))
		strcat(buf, "The air is #ywarm#c");
	if ((weathertemp >= 24) && (weathertemp <= 28))
		strcat(buf, "The air #yvery warm#c");	
	if ((weathertemp >= 29) && (weathertemp <= 32))
		strcat(buf, "The air is #Rhot#c");
	if ((weathertemp >= 33) && (weathertemp <= 36))
		strcat(buf, "The air is #Rvery hot#c");
	if (weathertemp > 36)
		strcat(buf, "The air is #Rpressively hot#c");
	
	if (weatherwind_spd < 1)
		strcat(buf, " and the wind is calm");
	if ((weatherwind_spd >= 1) && (weatherwind_spd <=5))
		strcat(buf, " and there is a light breeze");
	if ((weatherwind_spd >= 6) && (weatherwind_spd <=10))
		strcat(buf, " and there is a light wind");
	if ((weatherwind_spd >= 11) && (weatherwind_spd <=20))
		strcat(buf, " and the wind is gusting");
	if ((weatherwind_spd >= 21) && (weatherwind_spd <=30))
		strcat(buf, " and the wind is blowing hard");
	if ((weatherwind_spd >= 31) && (weatherwind_spd <=40))
		strcat(buf, " and the wind howls");
	if (weatherwind_spd > 40)
		strcat(buf, " and the wind is a gale");
	
	if ((weatherwind_dir == 1) && (weatherwind_spd > 1))
		strcat(buf, " from the southwest");
	if ((weatherwind_dir == 2) && (weatherwind_spd > 1))
		strcat(buf, " from the south");
	if ((weatherwind_dir == 3) && (weatherwind_spd > 1))
		strcat(buf, " from the southeast");
	if ((weatherwind_dir == 4) && (weatherwind_spd > 1))
		strcat(buf, " from the west");
	if ((weatherwind_dir == 5) && (weatherwind_spd > 1))
		strcat(buf, " from the west");
	if ((weatherwind_dir == 6) && (weatherwind_spd > 1))
		strcat(buf, " from the east");
	if ((weatherwind_dir == 7) && (weatherwind_spd > 1))
		strcat(buf, " from the northwest");
	if ((weatherwind_dir == 8) && (weatherwind_spd > 1))
		strcat(buf, " from the north");
	if ((weatherwind_dir == 9) && (weatherwind_spd > 1))
		strcat(buf, " from the northeast");
	if ((weatherwind_dir == 10) && (weatherwind_spd > 1))
		strcat(buf, " all around you");
		
	if (weatherclouds == 1)
		strcat(buf, " under a clear sky.");
	if (weatherclouds == 2)
		strcat(buf, " under a few scattered clouds.");
	if (weatherclouds == 3)
		strcat(buf, " under a cloudy sky.");
	if (weatherclouds == 4)
		strcat(buf, " and there are few breaks in the dark clouds above you.");
	if (weatherclouds == 5)
		strcat(buf, " and the sky is completely blanketed in dark clouds.");
			

	strcat(buf, "#n\n\r");
	send_to_char(buf, ch);
	
//	 Testing purposes
//	sprintf(buf, "\n\r%d degrees C - %d knots - %d wind dir - %d cloud cover\n\r", weathertemp, weatherwind_spd, weatherwind_dir, weatherclouds);
//	send_to_char(buf, ch);
	
	return;
	
}

