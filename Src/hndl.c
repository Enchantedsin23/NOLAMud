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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "merc.h"


AFFECT_DATA *affect_free;
PLAYER_INFO	* player_info_free;

int top_player_info	= 0;

/*
 * Local functions.
 */


//Gets length of a string without ansi codes
int strlen_noansi(char * str)
{
	int i;
	int len;
	for (i=0,len=0; str[i]!='\0'; i++)
	{
		if (str[i] != '#')
			len++;
		else if (str[i+1] == '#')
		{	
			i++;
			len += 2;
		}
		else
			i++;
	}
	return len;
}

int diceroll (CHAR_DATA * ch, int attribute, int ability_are, int ability, int difficulty)
{
	int success = 0;
	int i;
	int roll;
	char buf[MAX_STRING_LENGTH];
	int atleastonesuccess = 0;

	if (difficulty > 10)
		difficulty = 10;

	if ((attribute >= 10) || (attribute < 0))
	{
		bug ("attribute  in diceroll unknown", 0);
		return 0;
	}

	if ((ability_are >= 3) || (ability_are < 0))
	{
		bug ("ability area in diceroll unknown", 0);
		return 0;
	}

	if ((ability >= 10) || (ability < 0))
	{
		bug ("ability in diceroll unknown", 0);
		return 0;
	}

	if ((difficulty > 10) || (difficulty <= 1))
	{
		bug ("wacked out difficulty score in diceroll(probably willpower bug- merek)", 0);
		return 0;
	}

	for (i = 0; i < (ch->attributes[attribute] + ch->abilities[ability_are][ability]); i++)
	{
		roll = rand () % 10 + 1;

		if (roll == 10)
			i--;
		
		if (roll >= difficulty){
			success++;
			atleastonesuccess++;
		}

		if (roll == 1)	
			success--;

		
		
		sprintf( buf, "Rolled: #b%d#n.\n\r", roll );
//		send_to_char( buf, ch );
	}
	if ((success < 0) && (atleastonesuccess >= 1)){
			success = 0;
		}
	return success;
}


/*
 * Retrieve total abilities points in a grouping.
 */
int count_ability_levels (int area, CHAR_DATA * ch)
{
	int i;
	int sum = 0;
	for (i = 0; i < 10; i++)
	{
		sum = sum + ch->abilities[area][i];
	}
	return sum;
}


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust (CHAR_DATA * ch)
{
	if (ch->desc != NULL && ch->desc->original != NULL)
		ch = ch->desc->original;

	if (ch->trust != 0)
		return ch->trust;

	if (IS_NPC (ch) && ch->level >= LEVEL_HERO)
		return LEVEL_HERO - 1;
	else
		return ch->level;
}



/*
 * Retrieve a character's age.
 */
int get_age (CHAR_DATA * ch)
{
	return 17 + (ch->played + (int) (current_time - ch->logon)) / 7200;
}

int get_hours (CHAR_DATA * ch)
{
	return (ch->played + (int) (current_time - ch->logon)) / 3600;
}

int get_hoursIC (CHAR_DATA * ch)
{
	if (IS_MORE2(ch, MORE2_RPFLAG))
		return (ch->playedIC + (int) (current_time - ch->rpflag_time)) / 3600;
	else
		return ch->playedIC / 3600;
}

/*
 * Retrieve character's current strength.
 */
int get_curr_str (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;

	max = 11;

	return URANGE (1, (ch->attributes[ATTRIB_STR] + ch->mod_attributes[ATTRIB_STR]), max);
}


/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_DEX] + ch->mod_attributes[ATTRIB_DEX], max);
}


/*
 * Retrieve character's current stamina.
 */
int get_curr_sta (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_STA] + ch->mod_attributes[ATTRIB_STA], max);
}

/*
 * Retrieve character's current presence
 */
int get_curr_cha (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;
	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_MOON7))
		return 1;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_CHA] + ch->mod_attributes[ATTRIB_CHA], max);
}

/*
 * Retrieve character's current manipulation
 */
int get_curr_man (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;
	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_MOON7))
		return 1;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_MAN] + ch->mod_attributes[ATTRIB_MAN], max);
}


/*
 * Retrieve character's current composure.
 */
int get_curr_app (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_APP] + ch->mod_attributes[ATTRIB_APP], max);
}

/*
 * Retrieve character's current resolve.
 */
int get_curr_per (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_PER] + ch->mod_attributes[ATTRIB_PER], max);
}

/*
 * Retrieve character's current intelligence.
 */
int get_curr_int (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;
	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_MOON7))
		return 1;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_INT] + ch->mod_attributes[ATTRIB_INT], max);
}


/*
 * Retrieve character's current wits.
 */
int get_curr_wit (CHAR_DATA * ch)
{
	int max;

	if (IS_NPC (ch))
		return 2;
	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_MOON7))
		return 1;

	max = 11;

	return URANGE (1, ch->attributes[ATTRIB_WIT] + ch->mod_attributes[ATTRIB_WIT], max);
}




/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n (CHAR_DATA * ch)
{
	if (!IS_NPC (ch) && ch->level >= LEVEL_IMMORTAL)
		return 1000;

	if (IS_NPC (ch) && IS_SET (ch->act, ACT_PET))
		return 0;

	return MAX_WEAR + 10;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w (CHAR_DATA * ch)
{
	if (!IS_NPC (ch) && ch->level >= LEVEL_IMMORTAL)
		return 1000000;


	if (IS_NPC (ch) && IS_SET (ch->act, ACT_PET))
		return 0;

	return str_bonus[get_curr_str (ch)].carry;
}



/*
 * See if a string is one of the names of an object.
 */
bool is_name (const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument (namelist, name);
		if (name[0] == '\0')
			return FALSE;
		if (!str_cmp (str, name))
			return TRUE;
	}
}

bool is_names (char *string1, char *string2)
{
	char name1[MAX_INPUT_LENGTH];

	for (;;)
	{
		string1 = one_argument (string1, name1);
		if (name1[0] == '\0')
			return FALSE;
		else if (is_name (name1, string2))
			return TRUE;
	}
}


/*
 * Gives the number of the string.  KaVir.
 */
int loc_name (const char *str, char *namelist)
{
	int pos = 0;
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument (namelist, name);
		if (name[0] == '\0')
			return -1;
		if (!str_cmp (str, name))
			return pos;
		pos++;
	}
}

bool is_full_name (const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument (namelist, name);
		if (name[0] == '\0')
			return FALSE;
		if (!str_cmp (str, name))
			return TRUE;
	}
}

/*
 * Apply or remove an affect to a character.
 */
void affect_modify (CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd)
{
	OBJ_DATA *wield;
	int mod;

	mod = paf->modifier;

	if (fAdd)
	{
		SET_BIT (ch->affected_by, paf->bitvector);
	}
	else
	{
		REMOVE_BIT (ch->affected_by, paf->bitvector);
		mod = 0 - mod;
	}

	if (IS_NPC (ch))
	{
		switch (paf->location)
		{
		default:
			break;
		case APPLY_NONE:
			break;
		case APPLY_MANA:
			ch->max_mana += mod;
			break;
		case APPLY_HIT:
			ch->max_hit += mod;
			break;
		case APPLY_MOVE:
			ch->max_move += mod;
			break;
		case APPLY_AC:
			ch->armor += mod;
			break;
		case APPLY_HITROLL:
			ch->hitroll += mod;
			break;
		case APPLY_DAMROLL:
			ch->damroll += mod;
			break;
		case APPLY_SAVING_PARA:
			ch->saving_throw += mod;
			break;
		case APPLY_SAVING_ROD:
			ch->saving_throw += mod;
			break;
		case APPLY_SAVING_PETRI:
			ch->saving_throw += mod;
			break;
		case APPLY_SAVING_BREATH:
			ch->saving_throw += mod;
			break;
		case APPLY_SAVING_SPELL:
			ch->saving_throw += mod;
			break;
		case APPLY_POLY:
			ch->polyaff += mod;
			break;
		case APPLY_POTENCY:
			ch->vamppot += mod;
			break;
		case APPLY_BLOOD_MAX:
			ch->blood[BLOOD_POOL] += mod;
			break;
		case APPLY_BLOOD_POT:
			ch->blood[BLOOD_POTENCY] += mod;
			break;
		}
		return;
	}


	switch (paf->location)
	{
	default:
		bug ("Affect_modify: unknown location %d.", paf->location);
		return;

	case APPLY_NONE:
		break;
	case APPLY_SEX:
		break;
	case APPLY_CLASS:
		break;
	case APPLY_LEVEL:
		break;
	case APPLY_AGE:
		break;
	case APPLY_HEIGHT:
		break;
	case APPLY_WEIGHT:
		break;
	case APPLY_MANA:
		ch->max_mana += mod;
		break;
	case APPLY_HIT:
		ch->max_hit += mod;
		break;
	case APPLY_MOVE:
		ch->max_move += mod;
		break;
	case APPLY_POUNDS:
		break;
	case APPLY_EXP:
		break;
	case APPLY_AC:
		ch->armor += mod;
		break;
	case APPLY_HITROLL:
		ch->hitroll += mod;
		break;
	case APPLY_DAMROLL:
		ch->damroll += mod;
		break;
	case APPLY_SAVING_PARA:
		ch->saving_throw += mod;
		break;
	case APPLY_SAVING_ROD:
		ch->saving_throw += mod;
		break;
	case APPLY_SAVING_PETRI:
		ch->saving_throw += mod;
		break;
	case APPLY_SAVING_BREATH:
		ch->saving_throw += mod;
		break;
	case APPLY_SAVING_SPELL:
		ch->saving_throw += mod;
		break;
	case APPLY_POLY:
		ch->polyaff += mod;
		break;
	case APPLY_POTENCY:
		ch->vamppot += mod;
		break;
	case APPLY_DOMINATE:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_DOMINATE] += mod;
		break;
	case APPLY_AUSPEX:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_AUSPEX] += mod;
		break;
	case APPLY_ANIMALISM:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_ANIMALISM] += mod;
		break;
	case APPLY_CELERITY:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_CELERITY] += mod;
		break;
	case APPLY_RESILIENCE:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_RESILIENCE] += mod;
		break;
	case APPLY_VIGOR:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_VIGOR] += mod;
		break;
	case APPLY_FLEETINGWINTER:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_FLEETINGWINTER] += mod;
		break;
	case APPLY_MOON:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_MOON] += mod;
		break;
	case APPLY_ETERNALSUMMER:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_ETERNALSUMMER] += mod;
		break;
	case APPLY_FORGE:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_FORGE] += mod;
		break;
	case APPLY_OBFUSCATE:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_OBFUSCATE] += mod;
		break;
	case APPLY_DARKNESS:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_DARKNESS] += mod;
		break;
	case APPLY_ARTIFICE:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_ARTIFICE] += mod;
		break;
	case APPLY_ETERNALSPRING:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_ETERNALSPRING] += mod;
		break;
	case APPLY_PROTEAN:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_PROTEAN] += mod;
		break;
	case APPLY_FLEETINGSUMMER:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_FLEETINGSUMMER] += mod;
		break;
	case APPLY_REFLECTIONS:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_REFLECTIONS] += mod;
		break;
	case APPLY_COMMUNION:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_COMMUNION] += mod;
		break;
	case APPLY_THEBANSORCERY:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_THEBANSORCERY] += mod;
		break;
	case APPLY_SEPARATION:
		if (!IS_NPC (ch))
			ch->pcdata->powers_mod[DISC_SEPARATION] += mod;
		break;
	case APPLY_SET_DOMINATE:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_DOMINATE] < mod)
			ch->pcdata->powers_set[DISC_DOMINATE] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_AUSPEX:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_AUSPEX] < mod)
			ch->pcdata->powers_set[DISC_AUSPEX] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_ANIMALISM:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_ANIMALISM] < mod)
			ch->pcdata->powers_set[DISC_ANIMALISM] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_CELERITY:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_CELERITY] < mod)
			ch->pcdata->powers_set[DISC_CELERITY] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_RESILIENCE:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_RESILIENCE] < mod)
			ch->pcdata->powers_set[DISC_RESILIENCE] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_VIGOR:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_VIGOR] < mod)
			ch->pcdata->powers_set[DISC_VIGOR] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_FLEETINGWINTER:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_FLEETINGWINTER] < mod)
			ch->pcdata->powers_set[DISC_FLEETINGWINTER] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_MOON:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_MOON] < mod)
			ch->pcdata->powers_set[DISC_MOON] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_ETERNALSUMMER:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_ETERNALSUMMER] < mod)
			ch->pcdata->powers_set[DISC_ETERNALSUMMER] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_FORGE:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_FORGE] < mod)
			ch->pcdata->powers_set[DISC_FORGE] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_OBFUSCATE:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_OBFUSCATE] < mod)
			ch->pcdata->powers_set[DISC_OBFUSCATE] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_DARKNESS:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_DARKNESS] < mod)
			ch->pcdata->powers_set[DISC_DARKNESS] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_ARTIFICE:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_ARTIFICE] < mod)
			ch->pcdata->powers_set[DISC_ARTIFICE] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_ETERNALSPRING:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_ETERNALSPRING] < mod)
			ch->pcdata->powers_set[DISC_ETERNALSPRING] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_PROTEAN:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_PROTEAN] < mod)
			ch->pcdata->powers_set[DISC_PROTEAN] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_FLEETINGSUMMER:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_FLEETINGSUMMER] < mod)
			ch->pcdata->powers_set[DISC_FLEETINGSUMMER] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_REFLECTIONS:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_REFLECTIONS] < mod)
			ch->pcdata->powers_set[DISC_REFLECTIONS] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_COMMUNION:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_COMMUNION] < mod)
			ch->pcdata->powers_set[DISC_COMMUNION] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_THEBANSORCERY:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_THEBANSORCERY] < mod)
			ch->pcdata->powers_set[DISC_THEBANSORCERY] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_SET_SEPARATION:
		if (!IS_NPC (ch) && ch->pcdata->powers_set[DISC_SEPARATION] < mod)
			ch->pcdata->powers_set[DISC_SEPARATION] = mod;
		ch->pcdata->powers_set_wear++;
		break;
	case APPLY_BLOOD_MAX:
		ch->blood[BLOOD_POOL] += mod;
		break;
	case APPLY_BLOOD_POT:
		ch->blood[BLOOD_POTENCY] += mod;
		break;
	}

	/*
	 * Check for weapon wielding.
	 * Guard against recursion (for weapons with affects).
	 */
	if ((wield = get_eq_char (ch, WEAR_WIELD)) != NULL && wield->item_type == ITEM_WEAPON && get_obj_weight (wield) > str_bonus[get_curr_str (ch)].wield)
	{
		static int depth;

		if (depth == 0)
		{
			depth++;
			act ("You drop $p.", ch, wield, NULL, TO_CHAR);
			act ("$n drops $p.", ch, wield, NULL, TO_ROOM);
			obj_from_char (wield);
			obj_to_room (wield, ch->in_room);
			depth--;
		}
	}

	return;
}



/*
 * Give an affect to a char.
 */
void affect_to_char (CHAR_DATA * ch, AFFECT_DATA * paf)
{
	AFFECT_DATA *paf_new;

	if (affect_free == NULL)
	{
		paf_new = alloc_perm (sizeof (*paf_new));
	}
	else
	{
		paf_new = affect_free;
		affect_free = affect_free->next;
	}

	*paf_new = *paf;
	paf_new->next = ch->affected;
	ch->affected = paf_new;

	affect_modify (ch, paf_new, TRUE);
	return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove (CHAR_DATA * ch, AFFECT_DATA * paf)
{
	if (ch->affected == NULL)
	{
		bug ("Affect_remove: no affect.", 0);
		return;
	}

	affect_modify (ch, paf, FALSE);

	if (paf == ch->affected)
	{
		ch->affected = paf->next;
	}
	else
	{
		AFFECT_DATA *prev;

		for (prev = ch->affected; prev != NULL; prev = prev->next)
		{
			if (prev->next == paf)
			{
				prev->next = paf->next;
				break;
			}
		}

		if (prev == NULL)
		{
			bug ("Affect_remove: cannot find paf.", 0);
			return;
		}
	}

	paf->next = affect_free;
	affect_free = paf->next;
	return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip (CHAR_DATA * ch, int sn)
{
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	for (paf = ch->affected; paf != NULL; paf = paf_next)
	{
		paf_next = paf->next;
		if (paf->type == sn)
			affect_remove (ch, paf);
	}

	return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected (CHAR_DATA * ch, int sn)
{
	AFFECT_DATA *paf;

	for (paf = ch->affected; paf != NULL; paf = paf->next)
	{
		if (paf->type == sn)
			return TRUE;
	}

	return FALSE;
}



/*
 * Add or enhance an affect.
 */
void affect_join (CHAR_DATA * ch, AFFECT_DATA * paf)
{
	AFFECT_DATA *paf_old;
	bool found;

	found = FALSE;
	for (paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next)
	{
		if (paf_old->type == paf->type)
		{
			paf->duration += paf_old->duration;
			paf->modifier += paf_old->modifier;
			affect_remove (ch, paf_old);
			break;
		}
	}

	affect_to_char (ch, paf);
	return;
}



/*
 * Move a char out of a room.
 */
void char_from_room (CHAR_DATA * ch)
{
	OBJ_DATA *obj;

	if (ch->in_room == NULL)
	{
		bug ("Char_from_room: NULL.", 0);
		return;
	}

	if (!IS_NPC (ch))
		--ch->in_room->area->nplayer;

	if (!IS_NPC (ch) && IS_VAMPIRE (ch) && IS_MORE (ch, MORE_HAND_FLAME) && ch->in_room->light > 0)
		--ch->in_room->light;
	else if ((obj = get_eq_char (ch, WEAR_WIELD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room->light > 0)
		--ch->in_room->light;
	else if ((obj = get_eq_char (ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room->light > 0)
		--ch->in_room->light;

	if (!IS_NPC (ch) && IS_MORE (ch, MORE_DARKNESS))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
			REMOVE_BIT (ch->more, MORE_DARKNESS);
			ch->pcdata->darkness = 60;
			act ("The darkness in the room gradually fades away to nothing.", ch, NULL, NULL, TO_CHAR);
			act ("The darkness in the room gradually fades away to nothing.", ch, NULL, NULL, TO_ROOM);
		}
	}
	if (!IS_NPC (ch) && IS_MORE (ch, MORE_SILENCE))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->in_room->added_flags, ROOM2_SILENCE);
			REMOVE_BIT (ch->more, MORE_SILENCE);
			ch->pcdata->silence = 60;
			act ("The room is no longer silent.", ch, NULL, NULL, TO_CHAR);
			act ("The room is no longer silent.", ch, NULL, NULL, TO_ROOM);
		}
	}

	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_BLESSING))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->more2, MORE2_BLESSING);
			ch->pcdata->forge = 120;
			act ("The soft golden glow in the room fades.", ch, NULL, NULL, TO_CHAR);
			act ("The soft golden glow in the room fades.", ch, NULL, NULL, TO_ROOM);
		}
	}


	if (!IS_NPC (ch) && (IS_CHANGELING (ch)) && IS_SET (ch->act, PLR_VANISH))
	{
		REMOVE_BIT (ch->act, PLR_VANISH);
		send_to_char ("Your movement has removed possibility of camouflage.\n\r", ch);
	}



	if (!IS_NPC (ch) && IS_MORE (ch, MORE_REINA))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->in_room->room_flags, ROOM_SAFE);
			REMOVE_BIT (ch->more, MORE_REINA);
			ch->pcdata->reina = 60;
			send_to_char ("The peaceful harmony in the air gradually fades away to nothing.\n\r", ch);
			act ("The peaceful harmony in the air gradually fades away to nothing.", ch, NULL, NULL, TO_ROOM);
		}
	}
	else if (!IS_NPC (ch) && IS_MORE (ch, MORE_NEUTRAL))
	{
		if (ch->in_room != NULL)
		{
			REMOVE_BIT (ch->in_room->room_flags, ROOM_SAFE);
			REMOVE_BIT (ch->more, MORE_NEUTRAL);
			ch->pcdata->forge = 30;
			send_to_char ("The peaceful harmony in the air gradually fades away to nothing.\n\r", ch);
			act ("The peaceful harmony in the air gradually fades away to nothing.", ch, NULL, NULL, TO_ROOM);
		}
	}
	if (IS_AFFECTED (ch, AFF_HIDE))
	{
		REMOVE_BIT (ch->affected_by, AFF_HIDE);
		send_to_char ("You step from the shadows.\n\r", ch);
//	act("$n steps from the shadows.",ch,NULL,NULL,TO_ROOM);

	}

	if (ch == ch->in_room->people)
	{
		ch->in_room->people = ch->next_in_room;
	}
	else
	{
		CHAR_DATA *prev;

		for (prev = ch->in_room->people; prev; prev = prev->next_in_room)
		{
			if (prev->next_in_room == ch)
			{
				prev->next_in_room = ch->next_in_room;
				break;
			}
		}

		if (prev == NULL)
			bug ("Char_from_room: ch not found.", 0);
	}

	ch->in_room = NULL;
	ch->next_in_room = NULL;
	return;
}



/*
 * Move a char into a room.
 */
void char_to_room (CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex)
{
	OBJ_DATA *obj;

	if (pRoomIndex == NULL)
	{
		bug ("Char_to_room: NULL.", 0);
		return;
	}

	ch->in_room = pRoomIndex;
	ch->next_in_room = pRoomIndex->people;
	pRoomIndex->people = ch;

	if (!IS_NPC (ch))
		++ch->in_room->area->nplayer;

	if (!IS_NPC (ch) && IS_VAMPIRE (ch) && IS_MORE (ch, MORE_HAND_FLAME))
		++ch->in_room->light;
	else if ((obj = get_eq_char (ch, WEAR_WIELD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
		++ch->in_room->light;
	else if ((obj = get_eq_char (ch, WEAR_HOLD)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
		++ch->in_room->light;

	if (ch->loc_hp[6] > 0 && ch->in_room->blood < 100)
	{
		if (IS_VAMPIRE (ch))
			ch->loc_hp[6] = 0;
		else
			ch->in_room->blood += 1;
	}

	if (!IS_NPC (ch) && IS_MORE (ch, MORE_DARKNESS))
	{
		SET_BIT (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
		ch->pcdata->darkness = 60;
		act ("The room is filled with darkness.", ch, NULL, NULL, TO_CHAR);
		act ("The room is filled with darkness.", ch, NULL, NULL, TO_ROOM);
	}

	if (!IS_NPC (ch) && IS_MORE (ch, MORE_SILENCE))
	{
		SET_BIT (ch->in_room->added_flags, ROOM2_SILENCE);
		ch->pcdata->silence = 60;
		act ("The room is filled with silence.", ch, NULL, NULL, TO_CHAR);
		act ("The room is filled with silence.", ch, NULL, NULL, TO_ROOM);
	}

	if (!IS_NPC (ch) && IS_MORE (ch, MORE_REINA))
	{
		SET_BIT (ch->in_room->room_flags, ROOM_SAFE);
		ch->pcdata->reina = 60;
		act ("The air is filled with a peaceful harmony.", ch, NULL, NULL, TO_CHAR);
		act ("The air is filled with a peaceful harmony.", ch, NULL, NULL, TO_ROOM);
	}
	else if (!IS_NPC (ch) && IS_MORE (ch, MORE_NEUTRAL))
	{
		SET_BIT (ch->in_room->room_flags, ROOM_SAFE);
		ch->pcdata->forge = 30;
		act ("The air is filled with a peaceful harmony.", ch, NULL, NULL, TO_CHAR);
		act ("The air is filled with a peaceful harmony.", ch, NULL, NULL, TO_ROOM);
	}

	return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char (OBJ_DATA * obj, CHAR_DATA * ch)
{
	if (obj == NULL)
		return;
	obj->next_content = ch->carrying;
	ch->carrying = obj;
	obj->carried_by = ch;
	obj->in_room = NULL;
	obj->in_obj = NULL;
	ch->carry_number += 1;
	ch->carry_weight += get_obj_weight (obj);
}



/*
 * Take an obj from its character.
 */
void obj_from_char (OBJ_DATA * obj)
{
	CHAR_DATA *ch;

	if (obj == NULL)
		return;
	if ((ch = obj->carried_by) == NULL)
	{
		bug ("Obj_from_char: null ch.", 0);
		return;
	}

	if (obj->wear_loc != WEAR_NONE)
		unequip_char (ch, obj);

	if (ch->carrying == obj)
	{
		ch->carrying = obj->next_content;
	}
	else
	{
		OBJ_DATA *prev;

		for (prev = ch->carrying; prev != NULL; prev = prev->next_content)
		{
			if (prev->next_content == obj)
			{
				prev->next_content = obj->next_content;
				break;
			}
		}

		if (prev == NULL)
			bug ("Obj_from_char: obj not in list.", 0);
	}

	obj->carried_by = NULL;
	obj->next_content = NULL;
	ch->carry_number -= 1;
	ch->carry_weight -= get_obj_weight (obj);
	return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac (OBJ_DATA * obj, int iWear)
{
	if (obj->item_type != ITEM_ARMOR)
		return 0;

	switch (iWear)
	{
	case WEAR_BODY:
		return 3 * obj->value[0];
	case WEAR_HEAD:
		return 2 * obj->value[0];
	case WEAR_LEGS:
		return 2 * obj->value[0];
	case WEAR_FEET:
		return obj->value[0];
	case WEAR_HANDS:
		return obj->value[0];
	case WEAR_TATTOO:
		return obj->value[0];
	case WEAR_SHOULDER:
		return obj->value[0];
	case WEAR_FINGER_L:
		return obj->value[0];
	case WEAR_FINGER_R:
		return obj->value[0];
	case WEAR_NECK_1:
		return obj->value[0];
	case WEAR_NECK_2:
		return obj->value[0];
	case WEAR_ABOUT:
		return 2 * obj->value[0];
	case WEAR_WAIST:
		return obj->value[0];
	case WEAR_WRIST_L:
		return obj->value[0];
	case WEAR_WRIST_R:
		return obj->value[0];
	case WEAR_HOLD:
		return obj->value[0];
	case WEAR_FACE:
		return obj->value[0];
	case WEAR_SCABBARD_L:
		return 0;
	case WEAR_SCABBARD_R:
		return 0;
	case WEAR_STAKE:
		return 0;
	}

	return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char (CHAR_DATA * ch, int iWear)
{
	OBJ_DATA *obj;

	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->wear_loc == iWear)
			return obj;
	}

	return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char (CHAR_DATA * ch, OBJ_DATA * obj, int iWear)
{
	CHAR_DATA *chch;
	AFFECT_DATA *paf;
	int sn;

	if (get_eq_char (ch, iWear) != NULL)
	{
		bug ("Equip_char: already equipped (%d).", iWear);
		return;
	}

	if ((IS_OBJ_STAT (obj, ITEM_ANTI_EVIL) && IS_EVIL (ch)) || (IS_OBJ_STAT (obj, ITEM_ANTI_GOOD) && IS_GOOD (ch)) || (IS_OBJ_STAT (obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL (ch)))
	{
		/*
		 * Thanks to Morgenes for the bug fix here!
		 */
		act ("You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR);
		act ("$n is zapped by $p and drops it.", ch, obj, NULL, TO_ROOM);
		obj_from_char (obj);
		obj_to_room (obj, ch->in_room);
		do_autosave (ch, "");
		return;
	}

	if (iWear == WEAR_SCABBARD_L || iWear == WEAR_SCABBARD_R || iWear == WEAR_STAKE)
	{
		obj->wear_loc = iWear;
		return;
	}

	if (IS_NPC (ch))
		ch->armor -= apply_ac (obj, iWear);
	obj->wear_loc = iWear;

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		affect_modify (ch, paf, TRUE);
	for (paf = obj->affected; paf != NULL; paf = paf->next)
		affect_modify (ch, paf, TRUE);

	if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room != NULL)
		++ch->in_room->light;

	if (!IS_NPC (ch) && ch->pcdata->powers_set_wear > 0)
		set_disc (ch);

	if (!IS_NPC (ch))
	{
		if ((chch = get_char_world (ch, ch->name)) == NULL)
			return;
		if (chch->desc != ch->desc)
			return;
	}

	if (obj->wear_loc == WEAR_NONE)
		return;

	if (((obj->item_type == ITEM_ARMOR) && (obj->value[3] >= 1)) || ((obj->item_type == ITEM_WEAPON) && (obj->value[0] >= 1000)))
	{
		/* It would be so much easier if weapons had 5 values *sigh*.  
		 * Oh well, I'll just have to use v0 for two.  KaVir.
		 */
		if (obj->item_type == ITEM_ARMOR)
			sn = obj->value[3];
		else
			sn = obj->value[0] / 1000;

		if ((sn == 4) && (IS_AFFECTED (ch, AFF_BLIND)))
			return;
		else if ((sn == 27) && (IS_AFFECTED (ch, AFF_DETECT_INVIS)))
			return;
		else if ((sn == 39) && (IS_AFFECTED (ch, AFF_FLYING)))
			return;
		else if ((sn == 45) && (IS_AFFECTED (ch, AFF_INFRARED)))
			return;
		else if ((sn == 46) && (IS_AFFECTED (ch, AFF_INVISIBLE)))
			return;
		else if ((sn == 52) && (IS_AFFECTED (ch, AFF_PASS_DOOR)))
			return;
		else if ((sn == 54) && (IS_AFFECTED (ch, AFF_PROTECT)))
			return;
		else if ((sn == 57) && (IS_AFFECTED (ch, AFF_SANCTUARY)))
			return;
		else if ((sn == 2) && (IS_AFFECTED (ch, AFF_DETECT_INVIS)))
			return;
		else if ((sn == 3) && (IS_AFFECTED (ch, AFF_FLYING)))
			return;
		else if ((sn == 1) && (IS_AFFECTED (ch, AFF_INFRARED)))
			return;
		else if ((sn == 5) && (IS_AFFECTED (ch, AFF_INVISIBLE)))
			return;
		else if ((sn == 6) && (IS_AFFECTED (ch, AFF_PASS_DOOR)))
			return;
		else if ((sn == 7) && (IS_AFFECTED (ch, AFF_PROTECT)))
			return;
		else if ((sn == 8) && (IS_AFFECTED (ch, AFF_SANCTUARY)))
			return;
		else if ((sn == 9) && (IS_AFFECTED (ch, AFF_SNEAK)))
			return;
		else if ((sn == 10) && (IS_ITEMAFF (ch, ITEMA_SHOCKSHIELD)))
			return;
		else if ((sn == 11) && (IS_ITEMAFF (ch, ITEMA_FIRESHIELD)))
			return;
		else if ((sn == 12) && (IS_ITEMAFF (ch, ITEMA_ICESHIELD)))
			return;
		else if ((sn == 13) && (IS_ITEMAFF (ch, ITEMA_ACIDSHIELD)))
			return;
		else if ((sn == 14) && (IS_ITEMAFF (ch, ITEMA_DBPASS)))
			return;
		else if ((sn == 15) && (IS_ITEMAFF (ch, ITEMA_CHAOSSHIELD)))
			return;
		else if ((sn == 16) && (IS_ITEMAFF (ch, ITEMA_REGENERATE)))
			return;
		else if ((sn == 17) && (IS_ITEMAFF (ch, ITEMA_SPEED)))
			return;
		else if ((sn == 18) && (IS_ITEMAFF (ch, ITEMA_VORPAL)))
			return;
		else if ((sn == 19) && (IS_ITEMAFF (ch, ITEMA_PEACE)))
			return;
		else if ((sn == 20) && (IS_ITEMAFF (ch, ITEMA_REFLECT)))
			return;
		else if ((sn == 21) && (IS_ITEMAFF (ch, ITEMA_RESISTANCE)))
			return;
		else if ((sn == 22) && (IS_ITEMAFF (ch, ITEMA_VISION)))
			return;
		else if ((sn == 23) && (IS_ITEMAFF (ch, ITEMA_STALKER)))
			return;
		else if ((sn == 24) && (IS_ITEMAFF (ch, ITEMA_VANISH)))
			return;
		else if ((sn == 25) && (IS_ITEMAFF (ch, ITEMA_RAGER)))
			return;
		else if ((sn == 26) && (IS_ITEMAFF (ch, ITEMA_SHADES)))
			return;
		if (sn == 4)
		{
			SET_BIT (ch->affected_by, AFF_BLIND);
			send_to_char ("You cannot see a thing!\n\r", ch);
			act ("$n seems to be blinded!", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 27 || sn == 2)
		{
			SET_BIT (ch->affected_by, AFF_DETECT_INVIS);
			send_to_char ("Your eyes tingle.\n\r", ch);
			act ("$n's eyes flicker with light.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 39 || sn == 3)
		{
			SET_BIT (ch->affected_by, AFF_FLYING);
			send_to_char ("Your feet rise off the ground.\n\r", ch);
			act ("$n's feet rise off the ground.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 45 || sn == 1)
		{
			SET_BIT (ch->affected_by, AFF_INFRARED);
			send_to_char ("Your eyes glow red.\n\r", ch);
			act ("$n's eyes glow red.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 46 || sn == 5)
		{
			SET_BIT (ch->affected_by, AFF_INVISIBLE);
			send_to_char ("You fade out of existance.\n\r", ch);
			act ("$n fades out of existance.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 52 || sn == 6)
		{
			SET_BIT (ch->affected_by, AFF_PASS_DOOR);
			send_to_char ("You turn translucent.\n\r", ch);
			act ("$n turns translucent.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 54 || sn == 7)
		{
			SET_BIT (ch->affected_by, AFF_PROTECT);
			send_to_char ("You are surrounded by a divine aura.\n\r", ch);
			act ("$n is surrounded by a divine aura.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 57 || sn == 8)
		{
			SET_BIT (ch->affected_by, AFF_SANCTUARY);
			send_to_char ("You are surrounded by a white aura.\n\r", ch);
			act ("$n is surrounded by a white aura.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 9)
		{
			SET_BIT (ch->affected_by, AFF_SNEAK);
			send_to_char ("Your footsteps stop making any sound.\n\r", ch);
			act ("$n's footsteps stop making any sound.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 10)
		{
			SET_BIT (ch->itemaffect, ITEMA_SHOCKSHIELD);
			send_to_char ("You are surrounded by a crackling shield of lightning.\n\r", ch);
			act ("$n is surrounded by a crackling shield of lightning.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 11)
		{
			SET_BIT (ch->itemaffect, ITEMA_FIRESHIELD);
			send_to_char ("You are surrounded by a burning shield of flames.\n\r", ch);
			act ("$n is surrounded by a burning shield of flames.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 12)
		{
			SET_BIT (ch->itemaffect, ITEMA_ICESHIELD);
			send_to_char ("You are surrounded by a shimmering shield of ice.\n\r", ch);
			act ("$n is surrounded by a shimmering shield of ice.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 13)
		{
			SET_BIT (ch->itemaffect, ITEMA_ACIDSHIELD);
			send_to_char ("You are surrounded by a bubbling shield of acid.\n\r", ch);
			act ("$n is surrounded by a bubbling shield of acid.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 14)
		{
			SET_BIT (ch->itemaffect, ITEMA_DBPASS);
			send_to_char ("You are now safe from the DarkBlade clan guardians.\n\r", ch);
		}
		else if (sn == 15)
		{
			SET_BIT (ch->itemaffect, ITEMA_CHAOSSHIELD);
			send_to_char ("You are surrounded by a swirling shield of chaos.\n\r", ch);
			act ("$n is surrounded by a swirling shield of chaos.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 16)
			SET_BIT (ch->itemaffect, ITEMA_REGENERATE);
		else if (sn == 17)
		{
			SET_BIT (ch->itemaffect, ITEMA_SPEED);
			send_to_char ("You start moving faster than the eye can follow.\n\r", ch);
			act ("$n starts moving faster than the eye can follow.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 18)
			SET_BIT (ch->itemaffect, ITEMA_VORPAL);
		else if (sn == 19)
			SET_BIT (ch->itemaffect, ITEMA_PEACE);
		else if (sn == 20)
		{
			SET_BIT (ch->itemaffect, ITEMA_REFLECT);
			send_to_char ("You are surrounded by flickering shield of darkness.\n\r", ch);
			act ("$n is surrounded by a flickering shield of darkness.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 21)
			SET_BIT (ch->itemaffect, ITEMA_RESISTANCE);
		else if (sn == 22)
		{
			SET_BIT (ch->itemaffect, ITEMA_VISION);
			send_to_char ("Your eyes begin to glow bright white.\n\r", ch);
			act ("$n's eyes begin to glow bright white.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 23)
			SET_BIT (ch->itemaffect, ITEMA_STALKER);
		else if (sn == 24)
		{
			SET_BIT (ch->itemaffect, ITEMA_VANISH);
			send_to_char ("You blend into the shadows.\n\r", ch);
			act ("$n gradually fades into the shadows.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 25 && !IS_NPC (ch))
		{
			SET_BIT (ch->itemaffect, ITEMA_RAGER);
			if (IS_CHANGELING (ch) && ch->pcdata->wolf < 100)
			{
				ch->pcdata->wolf = 300;
				do_changeling (ch, "");
			}
			else if (IS_CHANGELING (ch))
				ch->pcdata->wolf = 300;
			else if (IS_VAMPIRE (ch))
				ch->pcdata->wolf = 25;
		}
		else if (sn == 26)
			SET_BIT (ch->itemaffect, ITEMA_SHADES);
	}
	return;
}

/*
 * Unequip a char with an obj.
 */
void unequip_char (CHAR_DATA * ch, OBJ_DATA * obj)
{
	CHAR_DATA *chch;
	AFFECT_DATA *paf;
	int sn;

	if (obj->wear_loc == WEAR_NONE)
	{
		bug ("Unequip_char: already unequipped.", 0);
		return;
	}

	if (obj->wear_loc == WEAR_SCABBARD_L || obj->wear_loc == WEAR_SCABBARD_R || obj->wear_loc == WEAR_STAKE)
	{
		obj->wear_loc = -1;
		return;
	}

	if (IS_NPC (ch))
		ch->armor += apply_ac (obj, obj->wear_loc);
	obj->wear_loc = -1;

	for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
		affect_modify (ch, paf, FALSE);
	for (paf = obj->affected; paf != NULL; paf = paf->next)
		affect_modify (ch, paf, FALSE);

	if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room != NULL && ch->in_room->light > 0)
		--ch->in_room->light;
	if (!IS_NPC (ch) && ch->pcdata->powers_set_wear > 0)
		set_disc (ch);

	if ((chch = get_char_world (ch, ch->name)) == NULL)
		return;
	if (chch->desc != ch->desc)
		return;

	if (!IS_NPC (ch) && (ch->desc != NULL && ch->desc->connected != CON_PLAYING))
		return;

	if (((obj->item_type == ITEM_ARMOR) && (obj->value[3] >= 1)) || ((obj->item_type == ITEM_WEAPON) && (obj->value[0] >= 1000)))
	{
		if (obj->item_type == ITEM_ARMOR)
			sn = obj->value[3];
		else
			sn = obj->value[0] * 0.001;
		if (IS_AFFECTED (ch, AFF_BLIND) && (sn == 4) && !is_affected (ch, 4))
		{
			REMOVE_BIT (ch->affected_by, AFF_BLIND);
			send_to_char ("You can see again.\n\r", ch);
			act ("$n seems to be able to see again.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_DETECT_INVIS) && (sn == 27 || sn == 2) && !is_affected (ch, 27))
		{
			REMOVE_BIT (ch->affected_by, AFF_DETECT_INVIS);
			send_to_char ("Your eyes stop tingling.\n\r", ch);
			act ("$n's eyes stop flickering with light.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_FLYING) && (sn == 39 || sn == 3) && !is_affected (ch, 39))
		{
			REMOVE_BIT (ch->affected_by, AFF_FLYING);
			send_to_char ("You slowly float to the ground.\n\r", ch);
			act ("$n slowly floats to the ground.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_INFRARED) && (sn == 45 || sn == 1) && !is_affected (ch, 45))
		{
			REMOVE_BIT (ch->affected_by, AFF_INFRARED);
			send_to_char ("Your eyes stop glowing red.\n\r", ch);
			act ("$n's eyes stop glowing red.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_INVISIBLE) && (sn == 46 || sn == 5) && !is_affected (ch, 46))
		{
			REMOVE_BIT (ch->affected_by, AFF_INVISIBLE);
			send_to_char ("You fade into existance.\n\r", ch);
			act ("$n fades into existance.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_PASS_DOOR) && (sn == 52 || sn == 6) && !is_affected (ch, 52))
		{
			REMOVE_BIT (ch->affected_by, AFF_PASS_DOOR);
			send_to_char ("You feel solid again.\n\r", ch);
			act ("$n is no longer translucent.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_PROTECT) && (sn == 54 || sn == 7) && !is_affected (ch, 54))
		{
			REMOVE_BIT (ch->affected_by, AFF_PROTECT);
			send_to_char ("The divine aura around you fades.\n\r", ch);
			act ("The divine aura around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_SANCTUARY) && (sn == 57 || sn == 8) && !is_affected (ch, 57))
		{
			REMOVE_BIT (ch->affected_by, AFF_SANCTUARY);
			send_to_char ("The white aura around your body fades.\n\r", ch);
			act ("The white aura about $n's body fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_AFFECTED (ch, AFF_SNEAK) && (sn == 9) && !is_affected (ch, 80))
		{
			REMOVE_BIT (ch->affected_by, AFF_SNEAK);
			send_to_char ("You are no longer moving so quietly.\n\r", ch);
			act ("$n is no longer moving so quietly.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_ITEMAFF (ch, ITEMA_SHOCKSHIELD) && (sn == 10))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_SHOCKSHIELD);
			send_to_char ("The crackling shield of lightning around you fades.\n\r", ch);
			act ("The crackling shield of lightning around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_ITEMAFF (ch, ITEMA_FIRESHIELD) && (sn == 11))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_FIRESHIELD);
			send_to_char ("The burning shield of fire around you fades.\n\r", ch);
			act ("The burning shield of fire around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_ITEMAFF (ch, ITEMA_ICESHIELD) && (sn == 12))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_ICESHIELD);
			send_to_char ("The shimmering shield of ice around you fades.\n\r", ch);
			act ("The shimmering shield of ice around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_ITEMAFF (ch, ITEMA_ACIDSHIELD) && (sn == 13))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_ACIDSHIELD);
			send_to_char ("The bubbling shield of acid around you fades.\n\r", ch);
			act ("The bubbling shield of acid around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (IS_ITEMAFF (ch, ITEMA_DBPASS) && (sn == 14))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_DBPASS);
			send_to_char ("You are no longer safe from the DarkBlade clan guardians.\n\r", ch);
		}
		else if (IS_ITEMAFF (ch, ITEMA_CHAOSSHIELD) && (sn == 15))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_CHAOSSHIELD);
			send_to_char ("The swirling shield of chaos around you fades.\n\r", ch);
			act ("The swirling shield of chaos around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 16)
			REMOVE_BIT (ch->itemaffect, ITEMA_REGENERATE);
		else if (IS_ITEMAFF (ch, ITEMA_SPEED) && (sn == 17))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_SPEED);
			send_to_char ("Your actions slow down to normal speed.\n\r", ch);
			act ("$n stops moving at supernatural speed.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 18)
			REMOVE_BIT (ch->itemaffect, ITEMA_VORPAL);
		else if (sn == 19)
			REMOVE_BIT (ch->itemaffect, ITEMA_PEACE);
		else if (IS_ITEMAFF (ch, ITEMA_REFLECT) && sn == 20)
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_REFLECT);
			send_to_char ("The flickering shield of darkness around you fades.\n\r", ch);
			act ("The flickering shield of darkness around $n fades.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 21)
			REMOVE_BIT (ch->itemaffect, ITEMA_RESISTANCE);
		else if (IS_ITEMAFF (ch, ITEMA_VISION) && sn == 22)
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_VISION);
			send_to_char ("Your eyes stop glowing bright white.\n\r", ch);
			act ("$n's eyes stop glowing bright white.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 23)
			REMOVE_BIT (ch->itemaffect, ITEMA_STALKER);
		else if (IS_ITEMAFF (ch, ITEMA_VANISH) && sn == 24)
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_VANISH);
			send_to_char ("You emerge from the shadows.\n\r", ch);
			act ("$n gradually fades out of the shadows.", ch, NULL, NULL, TO_ROOM);
		}
		else if (sn == 25 && !IS_NPC (ch) && IS_ITEMAFF (ch, ITEMA_RAGER))
		{
			REMOVE_BIT (ch->itemaffect, ITEMA_RAGER);
			if (IS_CHANGELING (ch) && ch->pcdata->wolf >= 100)
			{
				ch->pcdata->wolf = 0;
				do_unchangeling (ch, "");
			}
			ch->pcdata->wolf = 0;
		}
		else if (IS_ITEMAFF (ch, ITEMA_SHADES) && (sn == 26))
			REMOVE_BIT (ch->itemaffect, ITEMA_SHADES);
	}
	return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list (OBJ_INDEX_DATA * pObjIndex, OBJ_DATA * list)
{
	OBJ_DATA *obj;
	int nMatch;

	nMatch = 0;
	for (obj = list; obj != NULL; obj = obj->next_content)
	{
		if (obj->pIndexData == pObjIndex)
			nMatch++;
	}

	return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room (OBJ_DATA * obj)
{
	ROOM_INDEX_DATA *in_room;

	if (obj == NULL)
		return;
	if ((in_room = obj->in_room) == NULL)
	{
		bug ("obj_from_room: NULL.", 0);
		return;
	}

	if (obj == in_room->contents)
	{
		in_room->contents = obj->next_content;
	}
	else
	{
		OBJ_DATA *prev;

		for (prev = in_room->contents; prev; prev = prev->next_content)
		{
			if (prev->next_content == obj)
			{
				prev->next_content = obj->next_content;
				break;
			}
		}

		if (prev == NULL)
		{
			bug ("Obj_from_room: obj not found.", 0);
			return;
		}
	}

	obj->in_room = NULL;
	obj->next_content = NULL;
	return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room (OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex)
{
	if (obj == NULL)
		return;
	if (obj->item_type == ITEM_CREATURE)
	{
		MOB_INDEX_DATA *mob_index;
		if ((mob_index = get_mob_index (obj->pIndexData->vnum)) != NULL)
		{
			CHAR_DATA *ch = create_mobile (mob_index);


			char_to_room (ch, pRoomIndex);

			/*
			 * You can't just extract the object, as it might be used
			 * after this call.
			 */
			pRoomIndex = get_room_index (ROOM_VNUM_LIMBO);
			obj->timer = 1;
		}
	}
	if(pRoomIndex == NULL)
		return;
	obj->next_content = pRoomIndex->contents;
	pRoomIndex->contents = obj;
	obj->in_room = pRoomIndex;
	obj->carried_by = NULL;
	obj->in_obj = NULL;
	return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj (OBJ_DATA * obj, OBJ_DATA * obj_to)
{
	if (obj == NULL)
		return;
	obj->next_content = obj_to->contains;
	obj_to->contains = obj;
	obj->in_obj = obj_to;
	obj->in_room = NULL;
	obj->carried_by = NULL;

	return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj (OBJ_DATA * obj)
{
	OBJ_DATA *obj_from;

	if (obj == NULL)
		return;
	if ((obj_from = obj->in_obj) == NULL)
	{
		bug ("Obj_from_obj: null obj_from.", 0);
		return;
	}

	if (obj == obj_from->contains)
	{
		obj_from->contains = obj->next_content;
	}
	else
	{
		OBJ_DATA *prev;

		for (prev = obj_from->contains; prev; prev = prev->next_content)
		{
			if (prev->next_content == obj)
			{
				prev->next_content = obj->next_content;
				break;
			}
		}

		if (prev == NULL)
		{
			bug ("Obj_from_obj: obj not found.", 0);
			return;
		}
	}

	obj->next_content = NULL;
	obj->in_obj = NULL;

	return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj (OBJ_DATA * obj)
{
	CHAR_DATA *ch;
	OBJ_DATA *obj_content;
	OBJ_DATA *obj_next;
	ROOM_INDEX_DATA *to_room;

	if (obj->item_type == ITEM_WALL && obj->in_room != NULL && obj->value[0] >= 0 && obj->value[0] <= 5)
	{
		EXIT_DATA *pexit;
		int direction = obj->value[0];

		if ((pexit = obj->in_room->exit[direction]) != NULL)
		{
			REMOVE_BIT (pexit->exit_info, EX_WALL_WATER);
			REMOVE_BIT (pexit->exit_info, EX_WALL_GRANITE);
			REMOVE_BIT (pexit->exit_info, EX_WALL_FLAME);
		}
	}
	/*else if (obj->item_type == ITEM_WARD && obj->in_room != NULL && obj->value[0] >= 0 && obj->value[0] <= 5)
	{
		EXIT_DATA *pexit;
		int direction = obj->value[0];

		if ((pexit = obj->in_room->exit[direction]) != NULL)
		{
			REMOVE_BIT (pexit->exit_info, EX_WARD_GHOULS);
			REMOVE_BIT (pexit->exit_info, EX_WARD_LUPINES);
			REMOVE_BIT (pexit->exit_info, EX_WARD_KINDRED);
			REMOVE_BIT (pexit->exit_info, EX_WARD_SPIRITS);
		}
	}*/

	if (obj->in_room != NULL)
		obj_from_room (obj);
	else if (obj->carried_by != NULL)
		obj_from_char (obj);
	else if (obj->in_obj != NULL)
		obj_from_obj (obj);


	if ((ch = obj->chobj) != NULL && !IS_NPC (ch) && ch->pcdata->chobj == obj)
	{
		REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
		free_string (ch->morph);
		ch->morph = str_dup ("");
		ch->hit = 1;
		char_from_room (ch);
		char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
		ch->pcdata->chobj = NULL;
		obj->chobj = NULL;
		send_to_char ("You have been KILLED!!\n\r", ch);
		do_look (ch, "auto");
		ch->position = POS_RESTING;
	}
	else if ((ch = obj->chobj) != NULL && !IS_NPC (ch) && ch->pcdata->chobj == obj && (IS_EXTRA (ch, EXTRA_OSWITCH) || ch->pcdata->obj_vnum != 0))
	{
		if (ch->pcdata->obj_vnum != 0)
		{
			send_to_char ("You have been destroyed!\n\r", ch);
			ch->pcdata->chobj = NULL;
			obj->chobj = NULL;
		}
		else
		{
			REMOVE_BIT (ch->extra, EXTRA_OSWITCH);
			REMOVE_BIT (ch->affected_by, AFF_POLYMORPH);
			free_string (ch->morph);
			ch->morph = str_dup ("");
			char_from_room (ch);
			char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
			ch->pcdata->chobj = NULL;
			obj->chobj = NULL;
			send_to_char ("You return to your body.\n\r", ch);
		}
	}
	else if (obj->chobj != NULL)
	{
		if (!IS_NPC (obj->chobj))
			obj->chobj->pcdata->chobj = NULL;
		obj->chobj = NULL;
		bug ("Extract_obj: obj %d chobj invalid.", obj->pIndexData->vnum);
	}

	if ((ch = obj->sat) != NULL && ch->sat == obj)
	{
		obj->sat = NULL;
		ch->sat = NULL;
	}

	if ((to_room = obj->to_room) != NULL && to_room->to_obj == obj)
	{
		obj->to_room = NULL;
		to_room->to_obj = NULL;
	}

	for (obj_content = obj->contains; obj_content; obj_content = obj_next)
	{
		obj_next = obj_content->next_content;
		if (obj->contains != NULL)
			extract_obj (obj->contains);
	}

	if (object_list == obj)
	{
		object_list = obj->next;
	}
	else
	{
		OBJ_DATA *prev;

		for (prev = object_list; prev != NULL; prev = prev->next)
		{
			if (prev->next == obj)
			{
				prev->next = obj->next;
				break;
			}
		}

		if (prev == NULL)
		{
			bug ("Extract_obj: obj %d not found.", obj->pIndexData->vnum);
			return;
		}
	}

	{
		AFFECT_DATA *paf;
		AFFECT_DATA *paf_next;

		for (paf = obj->affected; paf != NULL; paf = paf_next)
		{
			paf_next = paf->next;
			paf->next = affect_free;
			affect_free = paf;
/* kavirp */
		}
	}

	{
		EXTRA_DESCR_DATA *ed;
		EXTRA_DESCR_DATA *ed_next;

		for (ed = obj->extra_descr; ed != NULL; ed = ed_next)
		{
			ed_next = ed->next;
			free_string (ed->description);
			free_string (ed->keyword);
			/* kavirp */
			ed->next = extra_descr_free;
			extra_descr_free = ed;
		}
	}

	free_string (obj->name);
	free_string (obj->description);
	free_string (obj->short_descr);
	if (obj->inside_desc != NULL)
		free_string (obj->inside_desc);
	if (obj->outside_desc != NULL)
		free_string (obj->outside_desc);
	if (obj->chpoweruse != NULL)
		free_string (obj->chpoweruse);
	if (obj->victpoweron != NULL)
		free_string (obj->victpoweron);
	if (obj->victpoweroff != NULL)
		free_string (obj->victpoweroff);
	if (obj->victpoweruse != NULL)
		free_string (obj->victpoweruse);
	if (obj->actcommands != NULL)
		free_string (obj->actcommands);
	if (obj->questmaker != NULL)
		free_string (obj->questmaker);
	if (obj->questowner != NULL)
		free_string (obj->questowner);
	--obj->pIndexData->count;
	obj->next = obj_free;
	obj_free = obj;
	return;
}



/*
 * Extract a char from the world.
 */
void extract_char (CHAR_DATA * ch, bool fPull)
{
	CHAR_DATA *wch;
	CHAR_DATA *familiar;
	CHAR_DATA *wizard;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if (ch == NULL)
		return;

	if (ch->in_room == NULL)
	{
		bug ("Extract_char: NULL.", 0);
		return;
	}

	if (fPull)
		die_follower (ch);

	for (obj = ch->carrying; obj != NULL; obj = obj_next)
	{
		obj_next = obj->next_content;
		extract_obj (obj);
	}

	char_from_room (ch);

	if (IS_NPC (ch))
		--ch->pIndexData->count;
	else if (ch->pcdata->chobj != NULL)
	{
		ch->pcdata->chobj->chobj = NULL;
		ch->pcdata->chobj = NULL;
	}

	if ((obj = ch->sat) != NULL && obj->sat == ch)
	{
		obj->sat = NULL;
		ch->sat = NULL;
	}

	if (!fPull)
	{
		char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
		return;
	}

	if (ch->desc != NULL && ch->desc->original != NULL)
		do_return (ch, "");

	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (wch->reply == ch)
			wch->reply = NULL;
		if (wch->onPhone == ch)
			ch->onPhone = NULL;
		if (wch->inDream == ch)
			ch->inDream = NULL;
	}

	if (ch == char_list)
	{
		char_list = ch->next;
	}
	else
	{
		CHAR_DATA *prev;

		for (prev = char_list; prev != NULL; prev = prev->next)
		{
			if (prev->next == ch)
			{
				prev->next = ch->next;
				break;
			}
		}

		if (prev == NULL)
		{
/*
			bug ("Extract_char: char not found.", 0); */
			return;
		}
	}

	if (ch->desc)
		ch->desc->character = NULL;

	if ((wizard = ch->wizard) != NULL)
	{
		if (!IS_NPC (wizard))
			wizard->pcdata->familiar = NULL;
		ch->wizard = NULL;
	}
	if ((wizard = ch->mount) != NULL)
	{
		if (IS_SET (ch->mounted, IS_MOUNT))
		{
			send_to_char ("You fall to the ground.\n\r", wizard);
			act ("$n falls to the ground.", wizard, NULL, NULL, TO_ROOM);
			wizard->position = POS_RESTING;
		}
		wizard->mount = NULL;
		wizard->mounted = 0;
		ch->mount = NULL;
		ch->mounted = 0;
	}

	if ((familiar = ch->embrace) != NULL)
	{
		ch->embrace = NULL;
		ch->embraced = ARE_NONE;
		familiar->embrace = NULL;
		familiar->embraced = ARE_NONE;
	}

	if (!IS_NPC (ch))
	{
		if ((familiar = ch->pcdata->partner) != NULL)
			ch->pcdata->partner = NULL;
		if ((familiar = ch->pcdata->propose) != NULL)
		{
			ch->pcdata->propose = NULL;
			familiar->pcdata->propose = NULL;
		}
		if ((familiar = ch->pcdata->familiar) != NULL)
		{
			familiar->wizard = NULL;
			ch->pcdata->familiar = NULL;
			if (IS_NPC (familiar))
			{
				act ("$n slowly fades away to nothing.", familiar, NULL, NULL, TO_ROOM);
				extract_char (familiar, TRUE);
			}
		}
	}

	for (familiar = char_list; familiar != NULL; familiar = familiar->next)
	{
		if (!IS_NPC (familiar) && familiar->pcdata->partner == ch)
			familiar->pcdata->partner = NULL;
	}

	if (IS_NPC (ch) && strlen (ch->lord) > 1)
	{
		for (wch = char_list; wch != NULL; wch = wch->next)
		{
			if (IS_NPC (wch))
				continue;
			if (str_cmp (wch->name, ch->lord))
				continue;
			if (wch->pcdata->followers > 0)
				wch->pcdata->followers--;
			if (IS_EXTRA (wch, EXTRA_NO_ACTION))
				REMOVE_BIT (wch->extra, EXTRA_NO_ACTION);
		}
	}

	free_char (ch);
	return;
}






/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *wch;
	int number;
	int count;

	if ((wch = get_char_room (ch, argument)) != NULL)
		return wch;

	number = number_argument (argument, arg);
	count = 0;
	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (!IS_NPC (wch) && IS_EXTRA (wch, EXTRA_OSWITCH))
			continue;
		else if (!can_see (ch, wch) || (!is_abbr (arg, wch->name) && (IS_NPC (wch) || !is_abbr (arg, wch->morph))))
			continue;
		if (++count == number)
			return wch;
	}

	return NULL;
}



/*
 * Get the char, ignoring abbreviations, vision, distance, etc
 */
CHAR_DATA *get_char_finger (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *wch;

	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (!IS_NPC (wch) && !str_cmp (argument, wch->name))
			return wch;
	}

	return NULL;
}



/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type (OBJ_INDEX_DATA * pObjIndex)
{
	OBJ_DATA *obj;

	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (obj->pIndexData == pObjIndex)
			return obj;
	}

	return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list (CHAR_DATA * ch, char *argument, OBJ_DATA * list)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	for (obj = list; obj != NULL; obj = obj->next_content)
	{
		if (can_see_obj (ch, obj) && is_abbr (arg, obj->name))
		{
			if (++count == number)
				return obj;
		}
	}

	return NULL;
}



/*
 * Find an object within the object you are in.
 */
OBJ_DATA *get_obj_in_obj (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *obj2;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	if (IS_NPC (ch) || ch->pcdata->chobj == NULL || ch->pcdata->chobj->in_obj == NULL)
		return NULL;
	obj = ch->pcdata->chobj;
	if (obj->in_obj->item_type != ITEM_CONTAINER && obj->in_obj->item_type != ITEM_CORPSE_NPC && obj->in_obj->item_type != ITEM_CORPSE_PC)
		return NULL;

	for (obj2 = obj->in_obj->contains; obj2 != NULL; obj2 = obj2->next_content)
	{
		if (obj != obj2 && is_abbr (arg, obj2->name))
		{
			if (++count == number)
				return obj2;
		}
	}

	return NULL;
}



/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->wear_loc == WEAR_NONE && can_see_obj (ch, obj) && is_abbr (arg, obj->name) && obj->item_type != ITEM_VEHICLE)
		{
			if (++count == number)
				return obj;
		}
	}

	return NULL;
}



/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
	{
		if (obj->wear_loc != WEAR_NONE && can_see_obj (ch, obj) && is_abbr (arg, obj->name))
		{
			if (++count == number)
				return obj;
		}
	}

	return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if (is_abbr (argument, "self me myself") && !IS_NPC (ch) && ch->pcdata->chobj != NULL)
		return ch->pcdata->chobj;

	obj = get_obj_list (ch, argument, ch->in_room->contents);
	if (obj != NULL)
		return obj;

	if ((obj = get_obj_carry (ch, argument)) != NULL)
		return obj;

	if ((obj = get_obj_wear (ch, argument)) != NULL)
		return obj;

	if ((obj = get_obj_in_obj (ch, argument)) != NULL)
		return obj;

	return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_room (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;

	if (is_abbr (argument, "self me myself") && !IS_NPC (ch) && ch->pcdata->chobj != NULL)
		return ch->pcdata->chobj;

	obj = get_obj_list (ch, argument, ch->in_room->contents);
	if (obj != NULL)
		return obj;

	return NULL;
}



/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number;
	int count;

	if (is_abbr (argument, "self me myself") && !IS_NPC (ch) && ch->pcdata->chobj != NULL)
		return ch->pcdata->chobj;

	if ((obj = get_obj_here (ch, argument)) != NULL)
		return obj;

	number = number_argument (argument, arg);
	count = 0;
	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (can_see_obj (ch, obj) && is_abbr (arg, obj->name))
		{
			if (++count == number)
				return obj;
		}
	}

	return NULL;
}



/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money (int amount)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	if (amount <= 0)
	{
		bug ("Create_money: zero or negative money %d.", amount);
		amount = 1;
	}

	if (amount == 1)
	{
		obj = create_object (get_obj_index (OBJ_VNUM_MONEY_ONE), 0);
	}
	else
	{
		obj = create_object (get_obj_index (OBJ_VNUM_MONEY_SOME), 0);
		sprintf (buf, obj->short_descr, amount);
		free_string (obj->short_descr);
		obj->short_descr = str_dup (buf);
		obj->value[0] = amount;
	}

	return obj;
}



/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight (OBJ_DATA * obj)
{
	int weight;

	weight = obj->weight;
	for (obj = obj->contains; obj != NULL; obj = obj->next_content)
		weight += get_obj_weight (obj);

	return weight;
}



/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex->light > 0 )
	return FALSE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK) )
	return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
    ||   pRoomIndex->sector_type == SECT_CITY )
	return FALSE;

    if ( weather_info.sunlight == SUN_SET
    ||   weather_info.sunlight == SUN_DARK )
	return TRUE;

    return FALSE;
}

bool weather_move( CHAR_DATA *ch, ROOM_INDEX_DATA *to_room, EXIT_DATA *pexit )
{
    ROOM_INDEX_DATA *in_room = ch->in_room;

    if (weather_info.sky == SKY_BLIZZARD) 
    { 		
        if  (((in_room->sector_type == SECT_INSIDE) || (in_room->room_flags == ROOM_INDOORS)) 
            && ((to_room->sector_type != SECT_INSIDE )|| (to_room->room_flags != ROOM_INDOORS))
            &&((!IS_AFFECTED(ch, AFF_FLYING)) && (!IS_AFFECTED(ch,AFF_INFRARED))))
        {
            send_to_char( "A #wblizzard#n rages outside, biting exposed skin and obscuring vision; it's bone-chillingly cold.\n\r", ch );
            return FALSE;
        }

       if ( IS_AFFECTED ( ch, AFF_FLYING ) )
       {
           if ( number_range( 0, 100) >= 75  ) /* 25 % chance for these poor souls */
           {
               send_to_char( "You can't take any more flying. Have a seat and rest for a few minutes.\n\r", ch );
               ch->move *= .80;
               return FALSE;
           }
       }

       if ( IS_AFFECTED ( ch, AFF_INFRARED ) )
       {
           if ( number_range( 0, 100) >= 75  ) /* 25 % chance of sight loss */
           {
               send_to_char( "Your infravision begins to fail. Have a seat for a few minutes.\n\r", ch );
               ch->move *= .80;
               return FALSE;
           }
       }
    }
	 
    if (weather_info.sky == SKY_HAILSTORM) 
    { 		
        if  (((in_room->sector_type == SECT_INSIDE) || (in_room->room_flags == ROOM_INDOORS)) 
            && ((to_room->sector_type != SECT_INSIDE )|| (to_room->room_flags != ROOM_INDOORS))
            && ((!IS_AFFECTED(ch, AFF_FLYING)) 
            && (!IS_AFFECTED(ch,AFF_INFRARED))))
        {
            send_to_char( "The #chail#n pelts your skin in stinging strokes, coating the city in icy debris.\n\r", ch );
            return FALSE;
        }

        if ( IS_AFFECTED ( ch, AFF_FLYING ) )
        {
            if ( number_range( 0, 100) >= 75  ) /* 25 % chance for these poor souls */
            {
                send_to_char( "Your can't take any more flying. Have a seat and rest for a few minutes.\n\r", ch );
	        ch->move *= .80;
                return FALSE;
            }

        }

        if ( IS_AFFECTED ( ch, AFF_INFRARED ) )
        {
            if ( number_range( 0, 100) >= 75  ) /* 25 % chance of sight loss */
            {
                send_to_char( "Your infravision begins to fail. Have a seat for a few minutes.\n\r", ch );
                ch->move *= .80;
                return FALSE;
            }

        }
    }
	 
    if (weather_info.sky == SKY_ICESTORM) 
    {  		
        if (((in_room->sector_type == SECT_INSIDE) || (in_room->room_flags == ROOM_INDOORS)) 
          && ((to_room->sector_type != SECT_INSIDE ) || (to_room->room_flags != ROOM_INDOORS))
          && ((!IS_AFFECTED(ch, AFF_FLYING)) &&  (!IS_AFFECTED(ch,AFF_INFRARED))))
        {
           send_to_char( "An #Cice storm#n rages in wild flurries of brutal cold, stinging exposed skin and obscuring vision.\n\r", ch );
           return FALSE;
        }

        if ( IS_AFFECTED ( ch, AFF_FLYING ) )
        {
            if ( number_range( 0, 100) >= 75  ) /* 25 % chance for these poor souls */
            {
                send_to_char( "Your can't take any more flying. Have a seat and rest for a few minutes.\n\r", ch );
                ch->move *= .80;
                return FALSE;
            }

        }

        if ( IS_AFFECTED ( ch, AFF_INFRARED ) )
        {
            if ( number_range( 0, 100) >= 75  ) /* 25 % chance of sight loss */
            {
                send_to_char( "Your infravision begins to fail. Have a seat for a few minutes.\n\r", ch );
                ch->move *= .80;
                return FALSE;
            }
        }
    }
	
    if (weather_info.sky == SKY_FOGGY) 
    { 		
        if  (((in_room->sector_type == SECT_INSIDE) || (in_room->room_flags == ROOM_INDOORS)) 
            && ((to_room->sector_type != SECT_INSIDE )|| (to_room->room_flags != ROOM_INDOORS))
            &&((!IS_AFFECTED(ch, AFF_FLYING)) && (!IS_AFFECTED(ch,AFF_INFRARED))))
        {
             send_to_char( "A thick, pea soup #cfog#n hangs over the city, cutting visibility by half.\n\r", ch );
             return FALSE;
        }

        if ( IS_AFFECTED ( ch, AFF_FLYING ) )
        {
            if ( number_range( 0, 100) >= 75  ) /* 25 % chance for these poor souls */
            {
                send_to_char( "Your tired from flying. Have a seat and rest for a few minutes.\n\r", ch );
	        ch->move *= .80;
                return FALSE;
            }
        }
       
        if ( IS_AFFECTED ( ch, AFF_INFRARED ) )
        { 
            if ( number_range( 0, 100) >= 75  ) /* 25 % chance of sight loss */
            {
                send_to_char( "Your infravision begins to fail. Have a seat for a few minutes.\n\r", ch );
	        ch->move *= .80;
                return FALSE;
            }
        }
    }

    return TRUE;
}

void weather_check( CHAR_DATA *ch )
{
    if ( weather_info.sky == SKY_BLIZZARD && number_chance( 25 ) )
    {
        blizzard(ch);
    }
        
    if ( weather_info.sky == SKY_FOGGY && number_chance( 25 ) )
    {
        fog(ch);
    }

    if ( weather_info.sky == SKY_ICESTORM && number_chance( 25 ) )
    {      
        ice(ch);
        blizzard(ch);
    }	

    if ( weather_info.sky == SKY_CLOUDY && number_chance( 25 ) )
    {
        blizzard(ch);
    }

    return;
}


bool room_has_shadows (ROOM_INDEX_DATA * pRoomIndex)
{

	if (pRoomIndex->light < 0)
		return TRUE;

	if (IS_SET (pRoomIndex->room_flags, ROOM_TOTAL_DARKNESS))
		return TRUE;
	if (IS_SET (pRoomIndex->room_flags, ROOM_DARK))
		return TRUE;
	if (IS_SET (pRoomIndex->room_flags, ROOM_INDOORS))
		return TRUE;

	return FALSE;
}
/*
 * True if room is private.
 */
bool room_is_private (ROOM_INDEX_DATA * pRoomIndex)
{
	CHAR_DATA *rch;
	int count;

	count = 0;

	for (rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room)
	{
		if (!IS_NPC (rch))
			count++;
	}

	if (IS_SET (pRoomIndex->room_flags, ROOM_PRIVATE) && count >= 2)
		return TRUE;

	return FALSE;
}


/*
 * True if a char can sense the victim.
 */
bool can_sense (CHAR_DATA * ch, CHAR_DATA * victim)
{
	if (IS_IMMORTAL (ch))
		return FALSE;

	if (number_range (1, 50) == 1 && IS_VAMPIRE (victim) && IS_VAMPAFF (victim, VAM_OBFUSCATE) && IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
		return TRUE;

	if (!can_see (ch, victim) && room_is_dark (ch->in_room) && IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
		return TRUE;

	return FALSE;
}


bool can_see (CHAR_DATA * ch, CHAR_DATA * victim ) {
	return can_see_2(ch, victim, TRUE);
}

/*
 * True if char can see victim.
 */
bool can_see_2 (CHAR_DATA * ch, CHAR_DATA * victim, bool checkVehicles )
{
	if (ch == victim)
		return TRUE;

	if (ch->plane != victim->plane)
	{
		if (!IS_STORYTELLER(ch))
			return FALSE;
	}

	if (!checkVehicles && ch->vehicle != victim->vehicle)
	{
		if (!IS_STORYTELLER(ch))
			return FALSE;
	}

	if (IS_VAMPAFF (victim, VAM_OBFUSCATE) && !IS_VAMPIRE (ch) && !IS_CHANGELING (ch) && !IS_STORYTELLER (ch))
		return FALSE;

	if (!IS_NPC (victim) && IS_VAMPAFF (victim, VAM_EARTHMELDED) && !IS_STORYTELLER (ch))
		return FALSE;

	if (IS_MORE (victim, MORE_SPIRIT) && !IS_VAMPAFF (ch, VAM_SPIRITEYES) && !IS_STORYTELLER (ch))
		return FALSE;

	if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_CAN_SEE))
	{
		if (ch->pcdata->familiar == NULL)
			REMOVE_BIT (ch->act, EXTRA_CAN_SEE);
		else if (victim->in_room != NULL && ch->in_room != NULL && ch->pcdata->familiar->in_room != NULL)
		{
			if (ch->in_room != ch->pcdata->familiar->in_room && ch->pcdata->familiar->in_room == victim->in_room)
			{
				if (can_see (ch->pcdata->familiar, victim))
					return TRUE;
				else
					return FALSE;
			}
		}
	}

	if (!IS_NPC (victim) && IS_SET (victim->act, PLR_WIZINVIS) && get_trust (ch) < get_trust (victim))
		return FALSE;

	if (!IS_NPC (victim) && IS_SET (victim->act, PLR_INCOG) 
     && get_trust (ch) < (get_trust (victim)) && ch->in_room != victim->in_room)
		return FALSE;

	if (IS_STORYTELLER (ch))
		return TRUE;

	if (IS_MORE3 (ch, MORE3_DEATHSWHISPER))	return FALSE;


	if (IS_VAMPAFF (victim, VAM_OBFUSCATE))
	{
		if (IS_NPC (ch))
			return FALSE;

		if (IS_STORYTELLER (ch))
			return TRUE;

		else if (IS_VAMPAFF (victim, VAM_OBFUSCATE) && IS_VAMPIRE (victim) && IS_VAMPIRE (ch) && get_disc (ch, DISC_AUSPEX) < get_disc (victim, DISC_OBFUSCATE) && get_disc (ch, DISC_CELERITY) < get_disc (victim, DISC_OBFUSCATE))
			return FALSE;
		else if (IS_MORTAL (ch))
			return FALSE;
	}


	if (!IS_STORYTELLER (ch) && IS_SET (ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
	{
		if (!IS_NPC (ch))
		{
			if (IS_STORYTELLER (ch) || IS_ITEMAFF (ch, ITEMA_VISION))
				return TRUE;
			else if (IS_VAMPIRE (ch) && get_disc (ch, DISC_DARKNESS) != 0)
				return TRUE;
			else if (IS_CHANGELING (ch) && get_organization (ch, ORGANIZATION_UKTENA) > 0)
				return TRUE;
			else
				return FALSE;
		}
		else
			return FALSE;
	}

	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
		return TRUE;


	if (IS_ITEMAFF (ch, ITEMA_VISION))
		return TRUE;


	if (IS_AFFECTED (ch, AFF_SHADOWPLANE) && !IS_AFFECTED (victim, AFF_SHADOWPLANE) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT))
		return FALSE;

	if (!IS_AFFECTED (ch, AFF_SHADOWPLANE) && IS_AFFECTED (victim, AFF_SHADOWPLANE) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT))
		return FALSE;


	if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_SONIC))
		return TRUE;

	if (room_is_dark (ch->in_room) && !IS_AFFECTED (ch, AFF_INFRARED) && !IS_VAMPAFF (ch, VAM_NIGHTSIGHT))
		return FALSE;


	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
		return TRUE;

	if (IS_EXTRA (ch, BLINDFOLDED))
		return FALSE;

    if ( IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R) )
	return FALSE;
	
	if (IS_AFFECTED (ch, AFF_BLIND) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT))
		return FALSE;


	if (IS_AFFECTED (victim, AFF_INVISIBLE) && !IS_AFFECTED (ch, AFF_DETECT_INVIS))
		return FALSE;

	if (IS_AFFECTED (victim, AFF_HIDE) && !IS_AFFECTED (ch, AFF_DETECT_HIDDEN))
		return FALSE;

	if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH))
		return TRUE;

	if (!IS_NPC (ch) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		return TRUE;

	if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		return TRUE;

	return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj (CHAR_DATA * ch, OBJ_DATA * obj)
{
	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
		return TRUE;


	if (IS_ITEMAFF (ch, ITEMA_VISION))
		return TRUE;

	if ((IS_SET (obj->extra_flags, ITEM_SHADOWPLANE) && obj->carried_by == NULL) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT) && !IS_AFFECTED (ch, AFF_SHADOWPLANE))
		return FALSE;

	if ((!IS_SET (obj->extra_flags, ITEM_SHADOWPLANE) && obj->carried_by == NULL) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT) && IS_AFFECTED (ch, AFF_SHADOWPLANE))
		return FALSE;


	if (!IS_NPC (ch) && IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
		return TRUE;

	if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_SONIC))
		return TRUE;

	if (obj->item_type == ITEM_POTION)
		return TRUE;

    if ( IS_HEAD(ch, LOST_EYE_L) && IS_HEAD(ch, LOST_EYE_R) )
	return FALSE;

	if (IS_EXTRA (ch, BLINDFOLDED))
		return FALSE;

	if (IS_AFFECTED (ch, AFF_BLIND) && !IS_AFFECTED (ch, AFF_SHADOWSIGHT))
		return FALSE;

	if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
		return TRUE;

	if (room_is_dark (ch->in_room) && !IS_AFFECTED (ch, AFF_INFRARED) && !IS_VAMPAFF (ch, VAM_NIGHTSIGHT))
		return FALSE;

	if (IS_SET (obj->extra_flags, ITEM_INVIS) && !IS_AFFECTED (ch, AFF_DETECT_INVIS))
		return FALSE;

	if (!IS_NPC (ch) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		return TRUE;

	if (!IS_NPC (ch) && IS_EXTRA (ch, EXTRA_OSWITCH) && ch->in_room != NULL && ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
		return TRUE;

	return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj (CHAR_DATA * ch, OBJ_DATA * obj)
{
	if (obj->item_type == ITEM_CONTRACT)
		return FALSE;

	if (obj->item_type == ITEM_VEHICLE)
		return FALSE;

	if (!IS_SET (obj->extra_flags, ITEM_NODROP))
		return TRUE;

	if (!IS_NPC (ch) && ch->level >= LEVEL_IMMORTAL)
		return TRUE;

	return FALSE;
}



/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name (int location)
{
	switch (location)
	{
	case APPLY_NONE:
		return "Modifies none by";
	case APPLY_SEX:
		return "Modifies sex by";
	case APPLY_CLASS:
		return "Modifies class by";
	case APPLY_LEVEL:
		return "Modifies level by";
	case APPLY_AGE:
		return "Modifies age by";
	case APPLY_MANA:
		return "Modifies mana by";
	case APPLY_HIT:
		return "Modifies hit points by";
	case APPLY_MOVE:
		return "Modifies vitality by";
	case APPLY_POUNDS:
		return "Modifies pounds (money) by";
	case APPLY_EXP:
		return "Modifies experience by";
	case APPLY_AC:
		return "Modifies armor class by";
	case APPLY_HITROLL:
		return "Modifies hit roll by";
	case APPLY_DAMROLL:
		return "Modifies damage roll by";
	case APPLY_SAVING_PARA:
		return "Modifies save vs paralysis by";
	case APPLY_SAVING_ROD:
		return "Modifies save vs rod by";
	case APPLY_SAVING_PETRI:
		return "Modifies your save vs petrification by";
	case APPLY_SAVING_BREATH:
		return "Modifies your save vs breath by";
	case APPLY_SAVING_SPELL:
		return "Modifies your save vs spell by";
	case APPLY_POLY:
		return "Modifies your polymorph form by";
	case APPLY_POTENCY:
		return "Modifies Potency by";
	case APPLY_DOMINATE:
		return "Modifies Dominate discipline by";
	case APPLY_AUSPEX:
		return "Modifies Auspex discipline by";
	case APPLY_ANIMALISM:
		return "Modifies Animalism contract by";
	case APPLY_CELERITY:
		return "Modifies Celerity discipline by";
	case APPLY_RESILIENCE:
		return "Modifies Resilience discipline by";
	case APPLY_VIGOR:
		return "Modifies Vigor discipline by";
	case APPLY_FLEETINGWINTER:
		return "Modifies Fleeting Winter discipline by";
	case APPLY_MOON:
		return "Modifies Moon Contract by";
	case APPLY_ETERNALSUMMER:
		return "Modifies Eternal Summer discipline by";
	case APPLY_FORGE:
		return "Modifies Forge discipline by";
	case APPLY_OBFUSCATE:
		return "Modifies Obfuscate discipline by";
	case APPLY_DARKNESS:
		return "Modifies Darkness contact by";
	case APPLY_ARTIFICE:
		return "Modifies Artifice contract by";
	case APPLY_ETERNALSPRING:
		return "Modifies Eternal Spring discipline by";
	case APPLY_PROTEAN:
		return "Modifies Protean discipline by";
	case APPLY_FLEETINGSUMMER:
		return "Modifies Fleeting Summer discipline by";
	case APPLY_REFLECTIONS:
		return "Modifies Reflections discipline by";
	case APPLY_COMMUNION:
		return "Modifies Communion contract by";
	case APPLY_THEBANSORCERY:
		return "Modifies Theban Sorcery discipline by";
	case APPLY_SEPARATION:
		return "Modifies Separation discipline by";
	case APPLY_BLOOD_MAX:
		return "Modifies blood pool by";
	case APPLY_BLOOD_POT:
		return "Modifies blood potency by";
	case APPLY_SET_DOMINATE:
		return "Gives min Discipline discipline of";
	case APPLY_SET_AUSPEX:
		return "Gives min Auspex discipline of";
	case APPLY_SET_ANIMALISM:
		return "Gives min Animalism contract of";
	case APPLY_SET_CELERITY:
		return "Gives min Celerity discipline of";
	case APPLY_SET_RESILIENCE:
		return "Gives min Resilience discipline of";
	case APPLY_SET_VIGOR:
		return "Gives min Vigor discipline of";
	case APPLY_SET_FLEETINGWINTER:
		return "Gives min Fleeting Winter discipline of";
	case APPLY_SET_MOON:
		return "Gives min Moon contract of";
	case APPLY_SET_ETERNALSUMMER:
		return "Gives min Eternal Summer discipline of";
	case APPLY_SET_FORGE:
		return "Gives min Forge discipline of";
	case APPLY_SET_OBFUSCATE:
		return "Gives min Obfuscate discipline of level";
	case APPLY_SET_DARKNESS:
		return "Gives min Darkness contact of";
	case APPLY_SET_ARTIFICE:
		return "Gives min Artifice contract of";
	case APPLY_SET_ETERNALSPRING:
		return "Gives min Eternal Spring discipline of";
	case APPLY_SET_PROTEAN:
		return "Gives min Protean discipline of";
	case APPLY_SET_FLEETINGSUMMER:
		return "Gives min Fleeting Summer discipline of";
	case APPLY_SET_REFLECTIONS:
		return "Gives min Reflections discipline of";
	case APPLY_SET_COMMUNION:
		return "Gives min Communion discipline of";
	case APPLY_SET_THEBANSORCERY:
		return "Gives min Theban Sorcery discipline of";
	case APPLY_SET_SEPARATION:
		return "Gives min Separation discipline of";
	}

	bug ("Affect_location_name: unknown location %d.", location);
	return "(unknown)";
}



/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name (int vector)
{
	static char buf[512];

	buf[0] = '\0';
	if (vector & AFF_BLIND)
		strcat (buf, " blind");
	if (vector & AFF_INVISIBLE)
		strcat (buf, " invisible");
	if (vector & AFF_DETECT_EVIL)
		strcat (buf, " detect_evil");
	if (vector & AFF_DETECT_INVIS)
		strcat (buf, " detect_invis");
	if (vector & AFF_DETECT_MAGIC)
		strcat (buf, " detect_magic");
	if (vector & AFF_DETECT_HIDDEN)
		strcat (buf, " detect_hidden");
	if (vector & AFF_SHADOWPLANE)
		strcat (buf, " umbra");
	if (vector & AFF_SANCTUARY)
		strcat (buf, " sanctuary");
	if (vector & AFF_DIRTY)
		strcat (buf, " dirty");
	if (vector & AFF_INFRARED)
		strcat (buf, " infrared");
	if (vector & AFF_CURSE)
		strcat (buf, " curse");
	if (vector & AFF_FLAMING)
		strcat (buf, " flaming");
	if (vector & AFF_POISON)
		strcat (buf, " poison");
	if (vector & AFF_PROTECT)
		strcat (buf, " protect");
	if (vector & AFF_ETHEREAL)
		strcat (buf, " ethereal");
	if (vector & AFF_SLEEP)
		strcat (buf, " sleep");
	if (vector & AFF_SNEAK)
		strcat (buf, " sneak");
	if (vector & AFF_HIDE)
		strcat (buf, " hide");
	if (vector & AFF_CHARM)
		strcat (buf, " charm");
	if (vector & AFF_FLYING)
		strcat (buf, " flying");
	if (vector & AFF_PASS_DOOR)
		strcat (buf, " pass_door");
	if (vector & AFF_POLYMORPH)
		strcat (buf, " polymorph");
	if (vector & AFF_SHADOWSIGHT)
		strcat (buf, " umbra");
	if (vector & AFF_PERFUME)
		strcat (buf, " perfume");
	if (vector & AFF_CONTRACEPTION)
		strcat (buf, " contraception");
	if (vector & AFF_SHADOW)
		strcat (buf, " dark_tendrils");
	if (vector & AFF_ROT)
		strcat (buf, " rotting_flesh");
	if (vector & AFF_FEAR)
		strcat (buf, " fear");
	if (vector & AFF_JAIL)
		strcat (buf, " jail");
	return (buf[0] != '\0') ? buf + 1 : "none";
}


CHAR_DATA *get_char (CHAR_DATA * ch)
{
	if (!ch->pcdata)
		return ch->desc->original;
	else
		return ch;
}

CHAR_DATA *get_char_world2 (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *wch;

	if (argument[0] == '\0')
		return NULL;

	for (wch = char_list; wch != NULL; wch = wch->next)
	{
		if (IS_NPC (wch) && !str_cmp (argument, wch->short_descr))
			return wch;
	}

	return NULL;
}

OBJ_DATA *get_obj_world2 (CHAR_DATA * ch, char *argument)
{
	OBJ_DATA *obj;
	int vnum = 0;

	if (argument[0] == '\0')
		return NULL;

	for (obj = object_list; obj != NULL; obj = obj->next)
	{
		if (!str_cmp (argument, obj->short_descr))
		{
			if ((vnum = obj->pIndexData->vnum) == 30037 || vnum == 30041)
				continue;
			return obj;
		}
	}

	return NULL;
}

/*
 * Returns number of comparisons in two input parameter strings.
 * KaVir.
 */
int str_close (char *a, char *b)
{
	int cmp = 0;

	while (*a && *b)
	{
		if (LOWER (*a++) == LOWER (*b++))
			cmp++;
	}
	return cmp;
}


/*
 * Like is_name, but uses abbreviations.
 */
bool is_abbr (const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument (namelist, name);
		if (name[0] == '\0')	/* Ignore empty strings */
			return FALSE;
		if (name[1] == '\0')	/* 1 char strings must be exact match */
		{
			if (!str_cmp (str, name))
				return TRUE;
			else
				return FALSE;
		}
		else if (!str_prefix (str, name))
			return TRUE;
	}
	return FALSE;
}





/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room (CHAR_DATA * ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *rch;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	if (is_abbr (arg, "self me myself") && (IS_NPC (ch) || ch->pcdata->chobj == NULL))
		return ch;
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	{
		if (!IS_NPC (rch) && IS_EXTRA (rch, EXTRA_OSWITCH))
			continue;
		else if (!can_see (ch, rch) || (!is_abbr (arg, rch->name) && (IS_NPC (rch) || !is_abbr (arg, rch->morph))))
			continue;
		if (++count == number)
			return rch;
	}

	return NULL;
}



/*
 * Find a percentage, with error checking.
 */
int PERCENTAGE (int amount, int max)
{
	if (max > 0)
		return (amount * 100) / max;

	return amount * 100;
}
#define IS_ADEPT(ch,sn)    ( PC(ch,learned)[sn] >= 100 )


/*
 * Newt: General purpose func to advance a skill. 
 * Returns: TRUE if (ch)'s skill (sn) was advanced
 *          FALSE if not
 */
bool advance_skill (CHAR_DATA * ch, int sn, int advance, int time_mod)
{
	if (!IS_NPC (ch)	/* PCs only             */
	    && !IS_ADEPT (ch, sn)	/* Not yet an adept     */
	    && advance > 0)	/* Actually improved    */
	{
		if (PC (ch, learned)[sn] <= 0 && skill_table[sn].min_mana > 0)
			return FALSE;	/* can't discover spells this way */


		PC (ch, learned)[sn] += advance;

		if (PC (ch, learned)[sn] == 1)
			act ("You discover $t.", ch, skill_table[sn].name, NULL, TO_CHAR);
		else if (PC (ch, learned)[sn] > 1)
			act ("You improve in $t.", ch, skill_table[sn].name, NULL, TO_CHAR);

		return TRUE;
	}
	return FALSE;
}

/*
 * Check for a skill.
 */

/*
 * Same as is_number but allows numbers of the form "x.yK" or "x.yM"
 */
bool is_number_special (const char *arg)
{
	bool point = FALSE;
	bool end = FALSE;
	const char *multiples = "kKmMbBtT";

	if (*arg == '\0')
		return FALSE;

	if (*arg == '+' || *arg == '-')
		arg++;

	for (; *arg != '\0'; arg++)
	{
		if (*arg == '.')
		{
			if (!point)
				point = TRUE;
			else
				return FALSE;
		}
		else if (end || !isdigit (*arg))
		{
			if (strchr (multiples, *arg))
			{
				if (!end)
					end = TRUE;
				else
					return FALSE;
			}
			else
				return FALSE;
		}
	}
	return TRUE;
}



char *room_sector (int sector)
{
	static char buf[64];

	if (sector == SECT_INSIDE)
		sprintf (buf, "Inside");
	else if (sector == SECT_CITY)
		sprintf (buf, "City");
	else if (sector == SECT_SEWER)
		sprintf (buf, "Sewer");
	else if (sector == SECT_FOREST)
		sprintf (buf, "Forest");
	else if (sector == SECT_HEDGE)
		sprintf (buf, "Hedge");
	else if (sector == SECT_HOME)
		sprintf (buf, "Home");
	else if (sector == SECT_WATER_SWIM)
		sprintf (buf, "Water-Swim");
	else if (sector == SECT_WATER_NOSWIM)
		sprintf (buf, "Water-NoSwim");
	else if (sector == SECT_UNDERWATER)
		sprintf (buf, "Under_water");
	else if (sector == SECT_AIR)
		sprintf (buf, "Air");
	else if (sector == SECT_OOC)
		sprintf (buf, "OOC");
	else if (sector == SECT_MAX)
		sprintf (buf, "MaxSect");
	
return buf;
}



int isname (const char *str, const char *namelist)
{
	const char *curname, *curstr;

	curname = namelist;
	for (;;)
	{
		for (curstr = str;; curstr++, curname++)
		{
			if (!*curstr && !isalpha (*curname))
				return (1);

			if (!*curname)
				return (0);

			if (!*curstr || *curname == ' ')
				break;

			if (LOWER (*curstr) != LOWER (*curname))
				break;
		}

		/* skip to next name */

		for (; isalpha (*curname); curname++);
		if (!*curname)
			return (0);
		curname++;	/* first char of new name */
	}
}

PLAYER_INFO *new_player_info( )
{
   PLAYER_INFO *pPInfo;

   if( player_info_free == NULL )
   {
	pPInfo = alloc_perm( sizeof(*pPInfo) );
	top_player_info++;
   }
   else
   {
	pPInfo = player_info_free;
  	player_info_free = player_info_free->next;
   }

   pPInfo->next		= NULL;
   pPInfo->name		= NULL;
   pPInfo->email	= NULL;
   pPInfo->password	= NULL;
   pPInfo->last_host	= NULL;
   pPInfo->flags	= 0;

   return pPInfo;
}

void free_player_info( PLAYER_INFO *pPInfo )
{
   EBAN_DATA *site;
   EBAN_DATA *site_next;

   for( site = pPInfo->accept_sites; site; site = site_next )
   {
 	site_next = site->next;
  	free_string( site->name );
	site->next = eban_free;
	eban_free = site;
   }

   for( site = pPInfo->deny_sites; site; site = site_next )
   {
	site_next = site->next;
	free_string( site->name );
	site->next = eban_free;
	eban_free = site;
   }

   free_string( pPInfo->name );
   free_string( pPInfo->last_host );
   free_string( pPInfo->email );
   free_string( pPInfo->password );
   pPInfo->next		= player_info_free;
   player_info_free	= pPInfo;
   top_player_info--;
   return;
}

bool is_obj (CHAR_DATA * ch)
{
	OBJ_DATA *obj;

	if ((obj = ch->pcdata->chobj) == NULL)
	{
		send_to_char ("Huh?\n\r", ch);
		return FALSE;
	}
	if (obj->chobj == NULL || obj->chobj != ch)
	{
		send_to_char ("Huh?\n\r", ch);
		return FALSE;
	}
	if (!IS_SET (ch->act, PLR_CHAMPION))
	{
		send_to_char ("Huh?\n\r", ch);
		return FALSE;
	}

	return TRUE;
}

int skill_lookup (const char *name)
{
	int sn;

	for (sn = 0; sn < MAX_SKILL; sn++)
	{
		if (skill_table[sn].name == NULL)
			break;
		if (LOWER (name[0]) == LOWER (skill_table[sn].name[0]) && !str_prefix (name, skill_table[sn].name))
			return sn;
	}

	return -1;
}

int slot_lookup (int slot)
{
	extern bool fBootDb;
	int sn;

	if (slot <= 0)
		return -1;

	for (sn = 0; sn < MAX_SKILL; sn++)
	{
		if (slot == skill_table[sn].slot)
			return sn;
	}

	if (fBootDb)
	{
		bug ("Slot_lookup: bad slot %d.", slot);
		abort ();
	}

	return -1;
}

int sn_lookup (int sn)
{
	int num;

	if (sn <= 0 || sn > MAX_SKILL)
		return 0;

	num = skill_table[sn].slot;

	return num;
}

void spell_null (int sn, int level, CHAR_DATA * ch, void *vo)
{
	send_to_char ("That's not a spell!\n\r", ch);
	return;
}
