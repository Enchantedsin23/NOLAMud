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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"

#if !defined(macintosh)
extern int _filbuf args ((FILE *));
#endif



/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static OBJ_DATA *rgObjNest[MAX_NEST];



/*
 * Local functions.
 */
void fwrite_char args ((CHAR_DATA * ch, FILE * fp));
void fwrite_opt_char args ((CHAR_DATA * ch, FILE * fp));
void fwrite_obj args ((CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest));
void fwrite_opt_obj args ((CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest));
void fread_char args ((CHAR_DATA * ch, FILE * fp));
void fread_opt_char args ((CHAR_DATA * ch, FILE * fp));
void fread_obj args ((CHAR_DATA * ch, FILE * fp));
void fread_opt_obj args ((CHAR_DATA * ch, FILE * fp));
void fread_tiny_obj args ((CHAR_DATA * ch, FILE * fp));



/*
 * Backup a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void backup_char_obj (CHAR_DATA * ch)
{
        char strsave[MAX_STRING_LENGTH];
        FILE *fp;

        if (IS_NPC (ch))
                return;

        if (ch->desc != NULL && ch->desc->connected != CON_PLAYING)
                return;

        if (ch->desc != NULL && ch->desc->original != NULL)
                ch = ch->desc->original;

        ch->save_time = current_time;
        fflush (fpReserve);
        fclose (fpReserve);
        sprintf (strsave, "%s%s", PLAYER_BAK_DIR, capitalize (ch->name));
        if ((fp = fopen (strsave, "w")) == NULL)
        {
                bug ("Backup_char_obj: fopen", 0);
                perror (strsave);
        }
        else
        {
                if (OPTIMISED_PFILES)
                {
                        fwrite_opt_char (ch, fp);
                        if (ch->carrying != NULL)
                                fwrite_opt_obj (ch, ch->carrying, fp, 0);
                }
                else
                {
                        fwrite_char (ch, fp);
                        if (ch->carrying != NULL)
                                fwrite_obj (ch, ch->carrying, fp, 0);
                }
                fprintf (fp, "#END\n");
        }
        fflush (fp);
        fclose (fp);
        fpReserve = fopen (NULL_FILE, "r");
        return;
}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj (CHAR_DATA * ch)
{
	char strsave[MAX_STRING_LENGTH];
	FILE *fp;

	if (IS_NPC (ch))
		return;

	if (ch->desc != NULL && ch->desc->connected != CON_PLAYING)
		return;

	if (ch->desc != NULL && ch->desc->original != NULL)
		ch = ch->desc->original;

	ch->save_time = current_time;
	fflush (fpReserve);
	fclose (fpReserve);
	sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (ch->name));
	
	if ((fp = fopen (strsave, "w")) == NULL)
	{
		bug ("Save_char_obj: fopen", 0);
		perror (strsave);
	}
	else
	{
		if (OPTIMISED_PFILES)
		{
			fwrite_opt_char (ch, fp);
			if (ch->carrying != NULL)
				fwrite_opt_obj (ch, ch->carrying, fp, 0);
		}
		else
		{
			fwrite_char (ch, fp);
			if (ch->carrying != NULL)
				fwrite_obj (ch, ch->carrying, fp, 0);
		}
		fprintf (fp, "#END\n");
	}
	fflush (fp);
	fclose (fp);
	fpReserve = fopen (NULL_FILE, "r");
	return;
}


/*
 * Write the char.
 */
void fwrite_char (CHAR_DATA * ch, FILE * fp)
{
	AFFECT_DATA *paf;
	int sn, i;
	fprintf (fp, "#%s\n", IS_NPC (ch) ? "MOB" : "PLAYER");

	fprintf (fp, "Name         %s~\n", ch->name);

	fprintf (fp, "ShortDescr   %s~\n", ch->short_descr);
	fprintf (fp, "LongDescr    %s~\n", ch->long_descr);
	fprintf (fp, "Description  %s~\n", ch->description);
	fprintf (fp, "Lord         %s~\n", ch->lord);
	fprintf (fp, "Clan         %s~\n", ch->clan);
	fprintf (fp, "Side         %s~\n", ch->side);
	fprintf (fp, "Nature       %s~\n", ch->nature);
	fprintf (fp, "Demeanor     %s~\n", ch->demeanor);
	fprintf (fp, "Order        %s~\n", ch->order);
	fprintf (fp, "Impulse      %s~\n", ch->impulse);
	fprintf (fp, "Morph        %s~\n", ch->morph);
	fprintf (fp, "Createtime   %s~\n", ch->createtime);
	fprintf (fp, "Lasttime     %s~\n", ch->lasttime);
	fprintf (fp, "Lasthost     %s~\n", ch->lasthost);
	fprintf (fp, "Poweraction  %s~\n", ch->poweraction);
	fprintf (fp, "Powertype    %s~\n", ch->powertype);
	fprintf (fp, "Prompt       %s~\n", ch->prompt);
	fprintf (fp, "Cprompt      %s~\n", ch->cprompt);
	fprintf (fp, "Sex          %d\n", ch->sex);
	fprintf (fp, "Race         %d\n", ch->race);
	fprintf (fp, "Class        %d\n", ch->class);
	fprintf (fp, "Immune       %d\n", ch->immune);
	fprintf (fp, "Polyaff      %d\n", ch->polyaff);
	fprintf (fp, "Itemaffect   %d\n", ch->itemaffect);
	fprintf (fp, "Vampaff      %d\n", ch->vampaff);
	fprintf (fp, "Vamppass     %d\n", ch->vamppass);
	fprintf (fp, "Form         %d\n", ch->form);
	fprintf (fp, "Body         %d\n", ch->body);
	fprintf (fp, "Beast        %d\n", ch->beast);
	fprintf (fp, "Truebeast    %d\n", ch->truebeast);
	fprintf (fp, "Vampgen      %d\n", ch->vampgen);
	fprintf (fp, "Spectype     %d\n", ch->spectype);
	fprintf (fp, "Specpower    %d\n", ch->specpower);
	fprintf (fp, "Home         %d\n", ch->home);
	fprintf (fp, "Level        %d\n", ch->level);

	fprintf (fp, "Security     %d\n", ch->pcdata->security);	/* OLC */
	fprintf (fp, "Trust        %d\n", ch->trust);
	fprintf (fp, "Paradox      %d %d %d\n", ch->paradox[0], ch->paradox[1], ch->paradox[2]);
	fprintf (fp, "Played       %d\n", ch->played + (int) (current_time - ch->logon));
	fprintf (fp, "PlayedIC     %d\n", ch->playedIC);
	fprintf (fp, "RPflagTime   %d\n", (int) ch->rpflag_time);
	fprintf (fp, "Room         %d\n", (ch->in_room == get_room_index (ROOM_VNUM_LIMBO) && ch->was_in_room != NULL) ? ch->was_in_room->vnum : ch->in_room->vnum);

	fprintf (fp, "PkPdMkMd     %d %d %d %d\n", ch->pkill, ch->pdeath, ch->mkill, ch->mdeath);

	fprintf (fp, "Weapons      %d %d %d %d %d %d %d %d %d %d %d %d %d\n", ch->wpn[0], ch->wpn[1], ch->wpn[2], ch->wpn[3], ch->wpn[4], ch->wpn[5], ch->wpn[6], ch->wpn[7], ch->wpn[8], ch->wpn[9], ch->wpn[10], ch->wpn[11], ch->wpn[12]);
	fprintf (fp, "Spells       %d %d %d %d %d\n", ch->spl[0], ch->spl[1], ch->spl[2], ch->spl[3], ch->spl[4]);
	fprintf (fp, "Combat       %d %d %d %d %d %d %d %d\n", ch->cmbt[0], ch->cmbt[1], ch->cmbt[2], ch->cmbt[3], ch->cmbt[4], ch->cmbt[5], ch->cmbt[6], ch->cmbt[7]);
	fprintf (fp, "Stance       %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", ch->stance[0], ch->stance[1], ch->stance[2], ch->stance[3], ch->stance[4], ch->stance[5], ch->stance[6], ch->stance[7], ch->stance[8], ch->stance[9], ch->stance[10], ch->stance[11], ch->stance[12], ch->stance[13], ch->stance[14], ch->stance[15]);
	fprintf (fp, "Locationhp   %d %d %d %d %d %d %d\n", ch->loc_hp[0], ch->loc_hp[1], ch->loc_hp[2], ch->loc_hp[3], ch->loc_hp[4], ch->loc_hp[5], ch->loc_hp[6]);
	fprintf (fp, "HpManaMove   %d %d %d %d %d %d\n", ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);
	fprintf (fp, "Agg          %d\n", ch->agg);
	fprintf (fp, "Gold         %d\n", ch->gold);
	fprintf (fp, "Exp          %d\n", ch->exp);
	fprintf (fp, "Act          %d\n", ch->act);
	fprintf (fp, "Extra        %d\n", ch->extra);
	fprintf (fp, "More         %d\n", ch->more);
	fprintf (fp, "More2         %d\n", ch->more2);
	fprintf (fp, "More3         %d\n", ch->more3);
	fprintf (fp, "AffectedBy   %d\n", ch->affected_by);
	/* Bug fix from Alander */
	fprintf (fp, "Position     %d\n", ch->position == POS_FIGHTING ? POS_STANDING : ch->position);
	fprintf (fp, "AggDmg       %d\n", ch->pcdata->aggdamage);
	fprintf (fp, "BashingDmg   %d\n", ch->pcdata->bashingdamage);
	fprintf (fp, "LethalDmg    %d\n", ch->pcdata->lethaldamage);
	fprintf (fp, "SavingThrow  %d\n", ch->saving_throw);
	fprintf (fp, "Alignment    %d\n", ch->alignment);
	fprintf (fp, "Hitroll      %d\n", ch->hitroll);
	fprintf (fp, "Damroll      %d\n", ch->damroll);
	fprintf (fp, "Armor        %d\n", ch->armor);
	fprintf (fp, "Wimpy        %d\n", ch->wimpy);
	fprintf (fp, "Deaf         %d\n", ch->deaf);
	fprintf (fp, "LocationXY   %d %d %d\n", ch->x, ch->y, ch->room);
	fprintf (fp, "HomeXY       %d %d\n", ch->home_x, ch->home_y);
	/*new RP code */
	fprintf (fp, "Votes	   %d\n", ch->pcdata->votes);
	fprintf (fp, "Lastvotetime %d %d %d\n", ch->pcdata->last_vote_time.year, ch->pcdata->last_vote_time.month, ch->pcdata->last_vote_time.day);
	/* end rp code*/
	fprintf (fp, "WpRegain     %d\n", ch->pcdata->wp_regain_counter);
  fprintf (fp, "StatusVoteCounter     %d\n", ch->pcdata->statusvotecounter);
	fprintf (fp, "BloodCounter %d\n", ch->pcdata->blood_burn_counter);
	fprintf (fp, "AutoVote %d\n", ch->pcdata->auto_vote_counter);
	fprintf (fp, "Canbuy %d\n", ch->pcdata->canbuy);
	fprintf (fp, "Canbuycounter %d\n", ch->pcdata->canbuycounter);
	fprintf (fp, "Canvotecounter %d\n", ch->pcdata->canvotecounter);
	fprintf (fp, "SectStatus %d\n", ch->pcdata->sectstatus);
	fprintf (fp, "Ringtone %s~\n", ch->ringtone);
	fprintf (fp, "Vibrate %d~\n", ch->phonevibrate);

	if (IS_NPC (ch))
	{
		fprintf (fp, "Vnum         %d\n", ch->pIndexData->vnum);
	}
	else
	{
		fprintf (fp, "Vision        %d\n", ch->pcdata->vision);
		fprintf (fp, "ShortDescL %s~n", ch->pcdata->lupusdescshort);
		fprintf (fp, "ShortDescC %s~n", ch->pcdata->crinosdescshort);
		fprintf (fp, "ShortDescH %s~n", ch->pcdata->hispodescshort);
		fprintf (fp, "ShortDescG %s~n", ch->pcdata->glabrodescshort);
		fprintf (fp, "LupusDesc  %s~\n", ch->pcdata->lupusdesc);
		fprintf (fp, "CrinosDesc  %s~\n", ch->pcdata->crinosdesc);
		fprintf (fp, "HispoDesc  %s~\n", ch->pcdata->hispodesc);
		fprintf (fp, "GlabroDesc  %s~\n", ch->pcdata->glabrodesc);
		fprintf (fp, "TempDesc  %s~\n", ch->pcdata->tempdesc);
		fprintf (fp, "ShowTempDesc %d\n", ch->pcdata->showtempdesc);
		fprintf (fp, "Giftlist  %s~\n", ch->pcdata->giftlist);
		fprintf (fp, "Password     %s~\n", ch->pcdata->pwd);
		fprintf (fp, "Bamfin       %s~\n", ch->pcdata->bamfin);
		fprintf (fp, "Bamfout      %s~\n", ch->pcdata->bamfout);
		fprintf (fp, "Title        %s~\n", ch->pcdata->title);
		fprintf (fp, "Pretitle     %s~\n", ch->pcdata->pretitle);
		fprintf (fp, "Conception   %s~\n", ch->pcdata->conception);
		fprintf (fp, "Parents      %s~\n", ch->pcdata->parents);
		fprintf (fp, "Cparents     %s~\n", ch->pcdata->cparents);
		fprintf (fp, "Marriage     %s~\n", ch->pcdata->marriage);
		fprintf (fp, "Email        %s~\n", ch->pcdata->email);
		fprintf (fp, "Conding      %s~\n", ch->pcdata->conding);
		fprintf (fp, "Love         %s~\n", ch->pcdata->love);
		fprintf (fp, "Coterie      %s~\n", ch->pcdata->coterie);
		fprintf (fp, "CotTime      %s~\n", ch->pcdata->cot_time);
		fprintf (fp, "CotClans     %s~\n", ch->pcdata->cot_clans);
		fprintf (fp, "CotSides     %s~\n", ch->pcdata->cot_sides);
		fprintf (fp, "Stancename   %s~\n", ch->pcdata->stancename);
		fprintf (fp, "Stancelist   %s~\n", ch->pcdata->stancelist);
		fprintf (fp, "Stancework   %s~\n", ch->pcdata->stancework);
		fprintf (fp, "Leader       %s~\n", ch->pcdata->leader);
		fprintf (fp, "DenyDate     %d\n", ch->pcdata->denydate);
		fprintf (fp, "Stancemove   %d %d %d %d %d\n", ch->pcdata->stancemove[0], ch->pcdata->stancemove[1], ch->pcdata->stancemove[2], ch->pcdata->stancemove[3], ch->pcdata->stancemove[4]);
		fprintf (fp, "AttrPerm     %d %d %d %d %d\n", ch->pcdata->perm_str, ch->pcdata->perm_int, ch->pcdata->perm_wis, ch->pcdata->perm_dex, ch->pcdata->perm_con);

		fprintf (fp, "AttrMod      %d %d %d %d %d\n", ch->pcdata->mod_str, ch->pcdata->mod_int, ch->pcdata->mod_wis, ch->pcdata->mod_dex, ch->pcdata->mod_con);

		fprintf (fp, "Quest        %d\n", ch->pcdata->quest);
		fprintf (fp, "Wolf         %d\n", ch->pcdata->wolf);
		fprintf (fp, "Rank         %d\n", ch->pcdata->rank);
		fprintf (fp, "Virtues      %d %d %d\n", ch->pcdata->virtues[VIRTUE_CONSCIENCE], ch->pcdata->virtues[VIRTUE_SELFCONTROL], ch->pcdata->virtues[VIRTUE_COURAGE]);	
		fprintf (fp, "Faith         %d\n", ch->pcdata->faith);
		if (IS_INQUISITOR(ch))
		{
			fprintf (fp, "Conviction   %d %d\n", ch->pcdata->conviction[CONVICTION_CURRENT], ch->pcdata->conviction[CONVICTION_MAX]);
			fprintf (fp, "ConvictionBank  %d\n", ch->pcdata->conviction_bank);
			fprintf (fp, "Virtues      %d %d %d %d %d %d", ch->pcdata->virtues[VIRTUE_CONSCIENCE], ch->pcdata->virtues[VIRTUE_SELFCONTROL], ch->pcdata->virtues[VIRTUE_COURAGE], ch->pcdata->virtues[VIRTUE_WISDOM], ch->pcdata->virtues[VIRTUE_FAITH], ch->pcdata->virtues[VIRTUE_ZEAL]);
		}
		else if (!IS_WEREWOLF(ch))
		{
			fprintf(fp, "Virtues	   %d %d %d\n", ch->pcdata->virtues[VIRTUE_CONSCIENCE], ch->pcdata->virtues[VIRTUE_SELFCONTROL], ch->pcdata->virtues[VIRTUE_COURAGE]);	
		}
		fprintf (fp, "Regenerate   %d\n", ch->pcdata->regenerate);
		fprintf (fp, "Demonic      %d\n", ch->pcdata->demonic);
		fprintf (fp, "Atm          %d\n", ch->pcdata->atm);
		fprintf (fp, "Necromancy   %d\n", ch->pcdata->necromancy);
		fprintf (fp, "Obeah        %d\n", ch->pcdata->obeah);
		fprintf (fp, "Darkness     %d\n", ch->pcdata->darkness);
		fprintf (fp, "Temporis     %d\n", ch->pcdata->temporis);
		fprintf (fp, "Silence      %d\n", ch->pcdata->silence);
		fprintf (fp, "Dementation      %d\n", ch->pcdata->dementation);
		fprintf (fp, "Devil      %d\n", ch->pcdata->devil);
		fprintf (fp, "Bodymods     %d\n", ch->pcdata->bodymods);
		fprintf (fp, "Reina        %d\n", ch->pcdata->reina);
		fprintf (fp, "Bpoints      %d\n", ch->pcdata->bpoints);
		fprintf (fp, "Dpoints      %d\n", ch->pcdata->dpoints);
		fprintf (fp, "Gpoints      %d\n", ch->pcdata->gpoints);
		fprintf (fp, "Rpoints      %d\n", ch->pcdata->rpoints);
		fprintf (fp, "Wpoints      %d\n", ch->pcdata->wpoints);
		fprintf (fp, "Spirit       %d\n", ch->pcdata->spirit);
		fprintf (fp, "Conjure      %d\n", ch->pcdata->conjure);
		fprintf (fp, "Celerity     %d\n", ch->pcdata->celerity);
		fprintf (fp, "Firstaid     %d\n", ch->pcdata->firstaid);
		fprintf (fp, "Repair       %d\n", ch->pcdata->repair);
		fprintf (fp, "Teach        %d\n", ch->pcdata->teach);
		fprintf (fp, "Learn        %d\n", ch->pcdata->learn);
		fprintf (fp, "Notes        %d\n", ch->pcdata->notes);
		fprintf (fp, "Evileye      %d\n", ch->pcdata->evileye);
		fprintf (fp, "Gnosis       %d %d\n", ch->pcdata->gnosis[0], ch->pcdata->gnosis[1]);
		fprintf (fp, "Rage       %d %d\n", ch->pcdata->rage[0], ch->pcdata->rage[1]);

		fprintf (fp, "Absorb       %d %d %d %d %d %d\n", ch->pcdata->absorb[0], ch->pcdata->absorb[1], ch->pcdata->absorb[2], ch->pcdata->absorb[3], ch->pcdata->absorb[4], ch->pcdata->absorb[5]);
		fprintf (fp, "Qstats       %d %d %d %d %d\n", ch->pcdata->qstats[0], ch->pcdata->qstats[1], ch->pcdata->qstats[2], ch->pcdata->qstats[3], ch->pcdata->qstats[4]);
		fprintf (fp, "Exercise     %d %d %d %d\n", ch->pcdata->exercise[0], ch->pcdata->exercise[1], ch->pcdata->exercise[2], ch->pcdata->exercise[3]);
		fprintf (fp, "Language     %d %d\n", ch->pcdata->language[0], ch->pcdata->language[1]);
		fprintf (fp, "Stage        %d %d %d\n", ch->pcdata->stage[0], ch->pcdata->stage[1], ch->pcdata->stage[2]);
		fprintf (fp, "Status       %d %d %d\n", ch->pcdata->status[0], ch->pcdata->status[1], ch->pcdata->status[2]);
		fprintf (fp, "Wolfform     %d %d\n", ch->pcdata->wolfform[0], ch->pcdata->wolfform[1]);
		fprintf (fp, "Score        %d %d %d %d %d %d\n", ch->pcdata->score[0], ch->pcdata->score[1], ch->pcdata->score[2], ch->pcdata->score[3], ch->pcdata->score[4], ch->pcdata->score[5]);
		fprintf (fp, "Runes        %d %d %d %d\n", ch->pcdata->runes[0], ch->pcdata->runes[1], ch->pcdata->runes[2], ch->pcdata->runes[3]);
		fprintf (fp, "Disc         %d %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->disc[0], ch->pcdata->disc[1], ch->pcdata->disc[2], ch->pcdata->disc[3], ch->pcdata->disc[4], ch->pcdata->disc[5], ch->pcdata->disc[6], ch->pcdata->disc[7], ch->pcdata->disc[8], ch->pcdata->disc[9], ch->pcdata->disc[10]);
		fprintf (fp, "Research     %d %d\n", ch->pcdata->research[0], ch->pcdata->research[1]);

		fprintf (fp, "Rituals      %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->rituals[0], ch->pcdata->rituals[1], ch->pcdata->rituals[2], ch->pcdata->rituals[3], ch->pcdata->rituals[4], ch->pcdata->rituals[5], ch->pcdata->rituals[6], ch->pcdata->rituals[7], ch->pcdata->rituals[8], ch->pcdata->rituals[9]);
		fprintf (fp, "Clandisc     %d %d %d\n", ch->pcdata->cland[0], ch->pcdata->cland[1], ch->pcdata->cland[2]);
		fprintf (fp, "Diableries   %d %d\n", ch->pcdata->diableries[0], ch->pcdata->diableries[1]);
		fprintf (fp, "Renown    %d %d %d %d %d %d\n", ch->pcdata->renown[0], ch->pcdata->renown[1], ch->pcdata->renown[2], ch->pcdata->renown[3], ch->pcdata->renown[4], ch->pcdata->renown[5]);
		fprintf (fp, "Breed        %d %d %d\n", ch->pcdata->breed[0], ch->pcdata->breed[1], ch->pcdata->breed[2]);
		fprintf (fp, "Auspice      %d %d %d %d %d\n", ch->pcdata->auspice[0], ch->pcdata->auspice[1], ch->pcdata->auspice[2], ch->pcdata->auspice[3], ch->pcdata->auspice[4]);
		fprintf (fp, "Tribes       %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
			 ch->pcdata->tribes[0], ch->pcdata->tribes[1], ch->pcdata->tribes[2], ch->pcdata->tribes[3], ch->pcdata->tribes[4], ch->pcdata->tribes[5], ch->pcdata->tribes[6], ch->pcdata->tribes[7], ch->pcdata->tribes[8], ch->pcdata->tribes[9], ch->pcdata->tribes[10], ch->pcdata->tribes[11], ch->pcdata->tribes[12], ch->pcdata->tribes[13], ch->pcdata->tribes[14], ch->pcdata->tribes[15], ch->pcdata->tribes[16], ch->pcdata->tribes[17], ch->pcdata->tribes[18], ch->pcdata->tribes[19]);
		fprintf (fp, "Thaum       %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
			 ch->pcdata->thaum[0], ch->pcdata->thaum[1], ch->pcdata->thaum[2], ch->pcdata->thaum[3], ch->pcdata->thaum[4], ch->pcdata->thaum[5], ch->pcdata->thaum[6], ch->pcdata->thaum[7], ch->pcdata->thaum[8], ch->pcdata->thaum[9], ch->pcdata->thaum[10], ch->pcdata->thaum[11], ch->pcdata->thaum[12], ch->pcdata->thaum[13], ch->pcdata->thaum[14], ch->pcdata->thaum[15], ch->pcdata->thaum[16], ch->pcdata->thaum[17], ch->pcdata->thaum[18], ch->pcdata->thaum[19]);

		fprintf (fp, "Powers %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
			 ch->pcdata->powers[0], ch->pcdata->powers[1], ch->pcdata->powers[2], ch->pcdata->powers[3], ch->pcdata->powers[4], ch->pcdata->powers[5], ch->pcdata->powers[6], ch->pcdata->powers[7], ch->pcdata->powers[8], ch->pcdata->powers[9], ch->pcdata->powers[10], ch->pcdata->powers[11], ch->pcdata->powers[12], ch->pcdata->powers[13], ch->pcdata->powers[14], ch->pcdata->powers[15], ch->pcdata->powers[16], ch->pcdata->powers[17], ch->pcdata->powers[18], ch->pcdata->powers[19],	//added stuff
			 ch->pcdata->powers[20], ch->pcdata->powers[21], ch->pcdata->powers[22],
			 ch->pcdata->powers[23], ch->pcdata->powers[24], ch->pcdata->powers[25], ch->pcdata->powers[26], ch->pcdata->powers[27], ch->pcdata->powers[28], ch->pcdata->powers[29], ch->pcdata->powers[30], ch->pcdata->powers[31], ch->pcdata->powers[32], ch->pcdata->powers[33], ch->pcdata->powers[34], ch->pcdata->powers[35], ch->pcdata->powers[36], ch->pcdata->powers[37], ch->pcdata->powers[38], ch->pcdata->powers[39]);


		fprintf (fp, "PowersMod    %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
			 ch->pcdata->powers_mod[0], ch->pcdata->powers_mod[1], ch->pcdata->powers_mod[2],
			 ch->pcdata->powers_mod[3], ch->pcdata->powers_mod[4], ch->pcdata->powers_mod[5], ch->pcdata->powers_mod[6], ch->pcdata->powers_mod[7], ch->pcdata->powers_mod[8], ch->pcdata->powers_mod[9], ch->pcdata->powers_mod[10], ch->pcdata->powers_mod[11], ch->pcdata->powers_mod[12], ch->pcdata->powers_mod[13], ch->pcdata->powers_mod[14], ch->pcdata->powers_mod[15], ch->pcdata->powers_mod[16], ch->pcdata->powers_mod[17], ch->pcdata->powers_mod[18], ch->pcdata->powers_mod[19]);
		fprintf (fp, "PowersSet    %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
			 ch->pcdata->powers_set[0], ch->pcdata->powers_set[1], ch->pcdata->powers_set[2],
			 ch->pcdata->powers_set[3], ch->pcdata->powers_set[4], ch->pcdata->powers_set[5], ch->pcdata->powers_set[6], ch->pcdata->powers_set[7], ch->pcdata->powers_set[8], ch->pcdata->powers_set[9], ch->pcdata->powers_set[10], ch->pcdata->powers_set[11], ch->pcdata->powers_set[12], ch->pcdata->powers_set[13], ch->pcdata->powers_set[14], ch->pcdata->powers_set[15], ch->pcdata->powers_set[16], ch->pcdata->powers_set[17], ch->pcdata->powers_set[18], ch->pcdata->powers_set[19]);
		fprintf (fp, "PowersWear   %d\n", ch->pcdata->powers_set_wear);
		fprintf (fp, "Genes        %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->genes[0], ch->pcdata->genes[1], ch->pcdata->genes[2], ch->pcdata->genes[3], ch->pcdata->genes[4], ch->pcdata->genes[5], ch->pcdata->genes[6], ch->pcdata->genes[7], ch->pcdata->genes[8], ch->pcdata->genes[9]);
		fprintf (fp, "CotMin       %d %d %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->cot_min[0], ch->pcdata->cot_min[1], ch->pcdata->cot_min[2], ch->pcdata->cot_min[3], ch->pcdata->cot_min[4], ch->pcdata->cot_min[5], ch->pcdata->cot_min[6], ch->pcdata->cot_min[7], ch->pcdata->cot_min[8], ch->pcdata->cot_min[9], ch->pcdata->cot_min[10], ch->pcdata->cot_min[11]);
		fprintf (fp, "Blood        %d %d\n", ch->blood[0], ch->blood[1]);
		fprintf (fp, "Willpower    %d %d\n", ch->pcdata->willpower[0], ch->pcdata->willpower[1]);
		fprintf (fp, "Resist       %d %d %d %d %d\n", ch->pcdata->resist[0], ch->pcdata->resist[1], ch->pcdata->resist[2], ch->pcdata->resist[3], ch->pcdata->resist[4]);
		fprintf (fp, "Power        %d %d\n", ch->pcdata->power[0], ch->pcdata->power[1]);

		fprintf (fp, "FakeCon      %d %d %d %d %d %d %d %d\n", ch->pcdata->fake_skill, ch->pcdata->fake_stance, ch->pcdata->fake_hit, ch->pcdata->fake_dam, ch->pcdata->fake_ac, ch->pcdata->fake_hp, ch->pcdata->fake_mana, ch->pcdata->fake_move);

		fprintf (fp, "FakeTwo      %d %d %d %d %d %d %d %d\n", ch->pcdata->fake_max_hp, ch->pcdata->fake_max_mana, ch->pcdata->fake_max_move, ch->pcdata->fake_stats[STAT_STR], ch->pcdata->fake_stats[STAT_INT], ch->pcdata->fake_stats[STAT_WIS], ch->pcdata->fake_stats[STAT_DEX], ch->pcdata->fake_stats[STAT_CON]);

		if (ch->pcdata->obj_vnum != 0)
			fprintf (fp, "Objvnum      %d\n", ch->pcdata->obj_vnum);

		fprintf (fp, "Majesty      %d\n", ch->pcdata->majesty);

		fprintf (fp, "Condition    %d %d %d\n", ch->pcdata->condition[0], ch->pcdata->condition[1], ch->pcdata->condition[2]);

		fprintf (fp, "Exhaustion   %d\n", ch->pcdata->exhaustion);
		fprintf (fp, "TempClass    %d\n", ch->pcdata->class);

		/* Save note board status */
		/* Save number of boards in case that number changes */
		fprintf (fp, "Boards       %d ", MAX_BOARD);
		for (i = 0; i < MAX_BOARD; i++)
			fprintf (fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
		fprintf (fp, "\n");

		fprintf (fp, "Attributes  ");
		for (i = 0; i < MAX_ATTRIB; i++)
			fprintf (fp, " %d", ch->attributes[i]);
		fprintf (fp, "\n");

		fprintf (fp, "Talents     ");
		for (i = 0; i < MAX_TAL; i++)
			fprintf (fp, " %d", ch->abilities[TALENTS][i]);
		fprintf (fp, "\n");

		fprintf (fp, "Skills      ");
		for (i = 0; i < MAX_SKI; i++)
			fprintf (fp, " %d", ch->abilities[SKILLS][i]);
		fprintf (fp, "\n");

		fprintf (fp, "Knowledges  ");
		for (i = 0; i < MAX_KNO; i++)
			fprintf (fp, " %d", ch->abilities[KNOWLEDGES][i]);
		fprintf (fp, "\n");

		/* Merits/Flaws Stuff */
		for (i = 0; i < 10; i++)
		{
			if (ch->pcdata->merits[i] != NULL)
				fprintf (fp, "Merit	%s~\n", ch->pcdata->merits[i]);
		}

		for (i = 0; i < 10; i++)
		{
			if (ch->pcdata->flaws[i] != NULL)
				fprintf (fp, "Flaw	%s~\n", ch->pcdata->flaws[i]);
		}
		/* End Merits/Flaws Stuff */

		for (sn = 0; sn < MAX_SKILL; sn++)
		{
			if (skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0)
			{
				fprintf (fp, "Skill        %d '%s'\n", ch->pcdata->learned[sn], skill_table[sn].name);
			}
		}
	}

	for (paf = ch->affected; paf != NULL; paf = paf->next)
	{
		/* Thx Alander */
		if (paf->type < 0 || paf->type >= MAX_SKILL)
			continue;

		fprintf (fp, "AffectData   '%s' %3d %3d %3d %10d\n", skill_table[paf->type].name, paf->duration, paf->modifier, paf->location, paf->bitvector);
	}
	fprintf (fp, "End\n\n");
	return;
}



/*
 * Write the char optimised for small harddisks.
 */
void fwrite_opt_char (CHAR_DATA * ch, FILE * fp)
{
	AFFECT_DATA *paf;
	int sn, i;
	EXTRA *extra;
	MEMORY_DATA * memory_ptr;

	fprintf (fp, "#%s\n", IS_NPC (ch) ? "MOB" : "PLR");

	fprintf (fp, "NAM %s~\n", ch->name);
	
	if (strlen (ch->short_descr) > 1)
		fprintf (fp, "SDS %s~\n", ch->short_descr);
	if (strlen (ch->long_descr) > 1)
		fprintf (fp, "LDS %s~\n", ch->long_descr);
	if (strlen (ch->description) > 1)
		fprintf (fp, "DDS %s~\n", ch->description);
	if (ch->pcdata->doing != NULL)
		fprintf (fp, "DNG %s~\n", ch->pcdata->doing);
/* Lumi Code*/
	if (ch->pcdata->aura != NULL)
		fprintf (fp, "AUR %s~\n", ch->pcdata->aura);
	if (ch->pcdata->voice != NULL)
		fprintf (fp, "VOI %s~\n", ch->pcdata->voice);	
	if (ch->pcdata->roomdesc != NULL)
		fprintf (fp, "RDS %s~\n", ch->pcdata->roomdesc);	
/* End */
	if (ch->pcdata->show_social)
		fprintf (fp, "SSD %d\n", ch->pcdata->show_social);
	if (strlen (ch->lord) > 1)
		fprintf (fp, "LRD %s~\n", ch->lord);
	if (strlen (ch->clan) > 1)
		fprintf (fp, "CLN %s~\n", ch->clan);
	if (strlen (ch->side) > 1)
		fprintf (fp, "SID %s~\n", ch->side);
	if (strlen (ch->nature) > 1)
		fprintf (fp, "NTR %s~\n", ch->nature);
	if (strlen (ch->demeanor) > 1)
		fprintf (fp, "DNR %s~\n", ch->demeanor);
	if (strlen (ch->order) > 1)
		fprintf (fp, "ORD %s~\n", ch->order);
	if (strlen (ch->impulse) > 1)
		fprintf (fp, "IMS %s~\n", ch->impulse);
	if (strlen (ch->morph) > 1)
		fprintf (fp, "MRF %s~\n", ch->morph);
	if (strlen (ch->mem_name) > 1)
		fprintf (fp, "MMN %s~\n", ch->mem_name);
	if (strlen (ch->mem_desc) > 1)
		fprintf (fp, "MMD %s~\n", ch->mem_desc);
	fprintf (fp, "CTM %s~\n", ch->createtime);
	fprintf (fp, "LTM %s~\n", ch->lasttime);
	fprintf (fp, "LHS %s~\n", ch->lasthost);
	fprintf (fp, "PLN %d\n", ch->plane);
	fprintf (fp, "QNT %d %d\n", ch->quintessence, ch->max_quintessence);
	if (strlen (ch->poweraction) > 1)
		fprintf (fp, "POA %s~\n", ch->poweraction);
	if (strlen (ch->powertype) > 1)
		fprintf (fp, "POT    %s~\n", ch->powertype);
	if (strlen (ch->prompt) > 1)
		fprintf (fp, "PRM %s~\n", ch->prompt);
	if (strlen (ch->cprompt) > 1)
		fprintf (fp, "CPM %s~\n", ch->cprompt);
	fprintf (fp, "SEX %d\n", ch->sex);
	if (ch->race)
		fprintf (fp, "RAC %d\n", ch->race);
	if (ch->pcdata->vision)
		fprintf (fp, "VSN %d\n", ch->pcdata->vision);

	if (ch->class)
		fprintf (fp, "CLS %d\n", ch->class);
	if (ch->immune)
		fprintf (fp, "IMU %d\n", ch->immune);
	if (ch->polyaff)
		fprintf (fp, "PAF %d\n", ch->polyaff);
	if (ch->itemaffect)
		fprintf (fp, "IAF %d\n", ch->itemaffect);
	if (ch->vampaff)
		fprintf (fp, "VAF %d\n", ch->vampaff);
	if (ch->vamppass != -1)
		fprintf (fp, "VPS %d\n", ch->vamppass);
	if (ch->form)
		fprintf (fp, "FRM %d\n", ch->form);
	if (ch->body != 32767)
		fprintf (fp, "BDY %d\n", ch->body);
	if (ch->beast != 20)
		fprintf (fp, "BST %d\n", ch->beast);
	if (ch->truebeast != 20)
		fprintf (fp, "TBS %d\n", ch->truebeast);
	if (ch->vampgen)
		fprintf (fp, "GEN %d\n", ch->vampgen);
	if (ch->spectype)
		fprintf (fp, "SPT %d\n", ch->spectype);
	if (ch->spectype)
		fprintf (fp, "SPP %d\n", ch->specpower);
	if (ch->home != 3700)
		fprintf (fp, "HOM %d\n", ch->home);
	fprintf (fp, "WXY %d %d\n", ch->x, ch->y);
	fprintf (fp, "HXY %d %d\n", ch->home_x, ch->home_y);
	fprintf (fp, "VOT %d\n", ch->pcdata->votes);
	fprintf (fp, "LVT %d %d %d\n", ch->pcdata->last_vote_time.year, ch->pcdata->last_vote_time.month, ch->pcdata->last_vote_time.day);
	fprintf (fp, "LVL %d\n", ch->level);
/*	if (IS_VAMPIRE (ch) || IS_MORTAL(ch))
		fprintf (fp, "VRT %d %d %d\n", ch->pcdata->virtues[VIRTUE_CONSCIENCE], ch->pcdata->virtues[VIRTUE_SELFCONTROL], ch->pcdata->virtues[VIRTUE_COURAGE]); */
	fprintf (fp, "ROD %d\n", ch->pcdata->road);
	fprintf (fp, "RDT %d\n", ch->pcdata->roadt);
	if (IS_INQUISITOR (ch))
		fprintf (fp, "VRT %d %d %d %d %d %d\n", ch->pcdata->virtues[VIRTUE_CONSCIENCE], ch->pcdata->virtues[VIRTUE_SELFCONTROL], ch->pcdata->virtues[VIRTUE_COURAGE], ch->pcdata->virtues[VIRTUE_WISDOM], ch->pcdata->virtues[VIRTUE_FAITH], ch->pcdata->virtues[VIRTUE_ZEAL]);
	else if (!IS_WEREWOLF (ch))
		fprintf (fp, "VRT %d %d %d\n", ch->pcdata->virtues[VIRTUE_CONSCIENCE], ch->pcdata->virtues[VIRTUE_SELFCONTROL], ch->pcdata->virtues[VIRTUE_COURAGE]);
	if (IS_IMMORTAL (ch))
		fprintf (fp, "SCR %d\n", ch->pcdata->security);	/* OLC */
	if (ch->trust)
		fprintf (fp, "TRS %d\n", ch->trust);
	fprintf (fp, "PLY %d\n", ch->played + (int) (current_time - ch->logon));
	fprintf (fp, "PIC %d\n", ch->playedIC);
	fprintf (fp, "RPF %d\n", (int) ch->rpflag_time);
	fprintf (fp, "ROM %d\n", (ch->in_room == get_room_index (ROOM_VNUM_LIMBO) && ch->was_in_room != NULL) ? ch->was_in_room->vnum : ch->in_room->vnum);

	fprintf (fp, "KAD %d %d %d %d\n", ch->pkill, ch->pdeath, ch->mkill, ch->mdeath);

	fprintf (fp, "WPN %d %d %d %d %d %d %d %d %d %d %d %d %d\n", ch->wpn[0], ch->wpn[1], ch->wpn[2], ch->wpn[3], ch->wpn[4], ch->wpn[5], ch->wpn[6], ch->wpn[7], ch->wpn[8], ch->wpn[9], ch->wpn[10], ch->wpn[11], ch->wpn[12]);
	fprintf (fp, "SPL %d %d %d %d %d\n", ch->spl[0], ch->spl[1], ch->spl[2], ch->spl[3], ch->spl[4]);
	fprintf (fp, "STN %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", ch->stance[0], ch->stance[1], ch->stance[2], ch->stance[3], ch->stance[4], ch->stance[5], ch->stance[6], ch->stance[7], ch->stance[8], ch->stance[9], ch->stance[10], ch->stance[11], ch->stance[12], ch->stance[13], ch->stance[14], ch->stance[15]);
	if (ch->loc_hp[0] | ch->loc_hp[1] | ch->loc_hp[2] | ch->loc_hp[3] | ch->loc_hp[4] | ch->loc_hp[5] | ch->loc_hp[6])
		fprintf (fp, "LHP %d %d %d %d %d %d %d\n", ch->loc_hp[0], ch->loc_hp[1], ch->loc_hp[2], ch->loc_hp[3], ch->loc_hp[4], ch->loc_hp[5], ch->loc_hp[6]);
	fprintf (fp, "HMM %d %d %d %d %d %d\n", ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move);
	if (ch->agg)
		fprintf (fp, "AGG %d\n", ch->agg);
	if (ch->gold)
		fprintf (fp, "GLD %d\n", ch->gold);
	if (ch->exp)
		fprintf (fp, "EXP %d\n", ch->exp);
	if (ch->act != (PLR_BLANK | PLR_COMBINE | PLR_PROMPT))
		fprintf (fp, "ACT %d\n", ch->act);
	if (ch->extra)
		fprintf (fp, "EXT %d\n", ch->extra);
	if (ch->extra2)
		fprintf (fp, "EX2 %d\n", ch->extra2);
	if (ch->more)
		fprintf (fp, "MOR %d\n", ch->more);
	if (ch->more2)
		fprintf (fp, "MOR2 %d\n", ch->more2);
	if (ch->more3)
		fprintf (fp, "MOR3 %d\n", ch->more3);

	if (ch->autostance)
		fprintf (fp, "AUT %d\n", ch->autostance);
	if (ch->affected_by)
		fprintf (fp, "AFB %d\n", ch->affected_by);
	/* Bug fix from Alander */
	fprintf (fp, "POS %d\n", ch->position == POS_FIGHTING ? POS_STANDING : ch->position);

	if (ch->pcdata->aggdamage)
		fprintf (fp, "ADM %d\n", ch->pcdata->aggdamage);
	if (ch->pcdata->lethaldamage)
		fprintf (fp, "LDM %d\n", ch->pcdata->lethaldamage);
	if (ch->pcdata->bashingdamage)
		fprintf (fp, "BDM %d\n", ch->pcdata->bashingdamage);

	if (ch->saving_throw)
		fprintf (fp, "SAV %d\n", ch->saving_throw);
	if (ch->alignment)
		fprintf (fp, "ALN %d\n", ch->alignment);
	fprintf (fp, "HIT %d\n", ch->hitroll);
	fprintf (fp, "DAM %d\n", ch->damroll);
	fprintf (fp, "ARM %d\n", ch->armor);
	if (ch->wimpy)
		fprintf (fp, "WMP %d\n", ch->wimpy);
	if (ch->deaf)
		fprintf (fp, "DEF %d\n", ch->deaf);

	fprintf (fp, "WRC %d\n", ch->pcdata->wp_regain_counter);
	fprintf (fp, "SVC %d\n", ch->pcdata->statusvotecounter);
	fprintf (fp, "CAN %d\n", ch->pcdata->canbuy);
	fprintf (fp, "CNC %d\n", ch->pcdata->canbuycounter);
	fprintf (fp, "CVC %d\n", ch->pcdata->canvotecounter);
	fprintf (fp, "BBC %d\n", ch->pcdata->blood_burn_counter);
	fprintf (fp, "AVC %d\n", ch->pcdata->auto_vote_counter);
	fprintf (fp, "SST %d\n", ch->pcdata->sectstatus);
	fprintf (fp, "RNT %s~\n", ch->ringtone);
	fprintf (fp, "VIB %d", ch->phonevibrate);
	
	fprintf (fp, "PTM");
	for (i = 0; i < MAX_THAUM_PATHS; i++)
			fprintf (fp, " %d", ch->pcdata->pthaum[i]);
	fprintf (fp, "\n");
	fprintf(fp, "THM %d %d %d %d %d\n", ch->pcdata->thaum_type, ch->pcdata->thaum_prime, ch->pcdata->research_power, ch->pcdata->research_time, ch->pcdata->research_type);

	if (IS_NPC (ch))
	{
		fprintf (fp, "VNM %d\n", ch->pIndexData->vnum);
	}
	else
	{
		if (strlen (ch->pcdata->lupusdescshort) > 1 )
			fprintf (fp, "SDL %s~\n", ch->pcdata->lupusdescshort);
		if (strlen (ch->pcdata->crinosdescshort) > 1 )
			fprintf (fp, "SDC %s~\n", ch->pcdata->crinosdescshort);
		if (strlen (ch->pcdata->hispodescshort) > 1 )
			fprintf (fp, "SDH %s~\n", ch->pcdata->hispodescshort);
		if (strlen (ch->pcdata->glabrodescshort) > 1 )
			fprintf (fp, "SDG %s~\n", ch->pcdata->glabrodescshort);
		if (strlen (ch->pcdata->lupusdesc) > 1)
			fprintf (fp, "LDD %s~\n", ch->pcdata->lupusdesc);
		if (strlen (ch->pcdata->crinosdesc) > 1)
			fprintf (fp, "CDD %s~\n", ch->pcdata->crinosdesc);

		if (strlen (ch->pcdata->hispodesc) > 1)
			fprintf (fp, "HDD %s~\n", ch->pcdata->hispodesc);
		if (strlen (ch->pcdata->glabrodesc) > 1)
			fprintf (fp, "GDD %s~\n", ch->pcdata->glabrodesc);
		if (strlen (ch->pcdata->tempdesc) > 1)
			fprintf (fp, "TDC %s~\n", ch->pcdata->tempdesc);
		if (ch->pcdata->showtempdesc)
			fprintf (fp, "TDS %d\n", ch->pcdata->showtempdesc);
		if (strlen (ch->pcdata->giftlist) > 1)
			fprintf (fp, "GFL %s~\n", ch->pcdata->giftlist);


		fprintf (fp, "BRD");
		fprintf (fp, " %d", MAX_BOARD);
		for (i = 0; i < MAX_BOARD; i++)
			fprintf (fp, " %ld", ch->pcdata->last_note[i]);
		fprintf (fp, "\n");

		fprintf (fp, "ATT");
		for (i = 0; i < MAX_ATTRIB; i++)
			fprintf (fp, " %d", ch->attributes[i]);
		fprintf (fp, "\n");

		fprintf (fp, "TAL");
		for (i = 0; i < MAX_TAL; i++)
			fprintf (fp, " %d", ch->abilities[TALENTS][i]);
		fprintf (fp, "\n");

		fprintf (fp, "TLX");
		for (i = 0; i < MAX_TAL; i++)
			fprintf (fp, " %d", ch->pcdata->abilities_points[TALENTS][i]);
		fprintf (fp, "\n");

		fprintf (fp, "SKI");
		for (i = 0; i < MAX_SKI; i++)
			fprintf (fp, " %d", ch->abilities[SKILLS][i]);
		fprintf (fp, "\n");

		fprintf (fp, "SKX");
		for (i = 0; i < MAX_SKI; i++)
			fprintf (fp, " %d", ch->pcdata->abilities_points[SKILLS][i]);
		fprintf (fp, "\n");

		fprintf (fp, "KNO");
		for (i = 0; i < MAX_KNO; i++)
			fprintf (fp, " %d", ch->abilities[KNOWLEDGES][i]);
		fprintf (fp, "\n");

		fprintf (fp, "KNX");
		for (i = 0; i < MAX_KNO; i++)
			fprintf (fp, " %d", ch->pcdata->abilities_points[KNOWLEDGES][i]);
		fprintf (fp, "\n");
		fprintf (fp, "PSW %s~\n", ch->pcdata->pwd);
		if (IS_IMMORTAL(ch))
		{
			if(strlen (ch->pcdata->bamfin) > 1)
				fprintf (fp, "BFI %s~\n", ch->pcdata->bamfin);
		}
		if (IS_IMMORTAL(ch))
		{
			if (strlen (ch->pcdata->bamfout) > 1)
				fprintf (fp, "BFO %s~\n", ch->pcdata->bamfout);
		}
		fprintf (fp, "TTL %s~\n", ch->pcdata->title);
		fprintf (fp, "PRT %s~\n", ch->pcdata->pretitle);
		if (strlen (ch->pcdata->conception) > 1)
			fprintf (fp, "CPT %s~\n", ch->pcdata->conception);
		if (strlen (ch->pcdata->parents) > 1)
			fprintf (fp, "PAR %s~\n", ch->pcdata->parents);
		if (strlen (ch->pcdata->cparents) > 1)
			fprintf (fp, "CPR %s~\n", ch->pcdata->cparents);
		if (strlen (ch->pcdata->marriage) > 1)
			fprintf (fp, "MRG %s~\n", ch->pcdata->marriage);
		if (strlen (ch->pcdata->email) > 1)
			fprintf (fp, "EML %s~\n", ch->pcdata->email);
		if (strlen (ch->pcdata->conding) > 1)
			fprintf (fp, "CND %s~\n", ch->pcdata->conding);
		if (strlen (ch->pcdata->love) > 1)
			fprintf (fp, "LOV %s~\n", ch->pcdata->love);
		if (strlen (ch->pcdata->coterie) > 1)
			fprintf (fp, "COT %s~\n", ch->pcdata->coterie);
		if (str_cmp (ch->pcdata->cot_time, "None"))
			fprintf (fp, "C_T %s~\n", ch->pcdata->cot_time);
		if (str_cmp (ch->pcdata->cot_clans, "All"))
			fprintf (fp, "C_C %s~\n", ch->pcdata->cot_clans);
		if (str_cmp (ch->pcdata->cot_sides, "All"))
			fprintf (fp, "C_S %s~\n", ch->pcdata->cot_sides);
		if (strlen (ch->pcdata->leader) > 1)
			fprintf (fp, "C_L %s~\n", ch->pcdata->leader);

		if (strlen (ch->pcdata->stancename) > 1)
			fprintf (fp, "S_N %s~\n", ch->pcdata->stancename);
		if (strlen (ch->pcdata->stancelist) > 1)
			fprintf (fp, "S_L %s~\n", ch->pcdata->stancelist);
		if (strlen (ch->pcdata->stancework) > 1)
			fprintf (fp, "S_W %s~\n", ch->pcdata->stancework);
		if (ch->pcdata->denydate)
			fprintf (fp, "DNY %d\n", ch->pcdata->denydate);
		if (ch->pcdata->stancemove[0])
			fprintf (fp, "S_M %d %d %d %d %d\n", ch->pcdata->stancemove[0], ch->pcdata->stancemove[1], ch->pcdata->stancemove[2], ch->pcdata->stancemove[3], ch->pcdata->stancemove[4]);

		fprintf (fp, "A_P %d %d %d %d %d\n", ch->pcdata->perm_str, ch->pcdata->perm_int, ch->pcdata->perm_wis, ch->pcdata->perm_dex, ch->pcdata->perm_con);

		if (ch->pcdata->mod_str | ch->pcdata->mod_int | ch->pcdata->mod_wis | ch->pcdata->mod_dex | ch->pcdata->mod_con)
			fprintf (fp, "A_M %d %d %d %d %d\n", ch->pcdata->mod_str, ch->pcdata->mod_int, ch->pcdata->mod_wis, ch->pcdata->mod_dex, ch->pcdata->mod_con);
		if (ch->pcdata->quest)
			fprintf (fp, "QST %d\n", ch->pcdata->quest);
		if (ch->pcdata->wolf)
			fprintf (fp, "WLF %d\n", ch->pcdata->wolf);
		if (ch->pcdata->rank)
			fprintf (fp, "RNK %d\n", ch->pcdata->rank);
		if (ch->pcdata->faith)
			fprintf (fp, "FAI %d\n", ch->pcdata->faith);
		if (IS_INQUISITOR(ch))
		{
			fprintf (fp, "CNV %d %d\n", ch->pcdata->conviction[CONVICTION_CURRENT], ch->pcdata->conviction[CONVICTION_MAX]);
			
			if (ch->pcdata->conviction_bank)
				fprintf (fp, "CVB %d\n", ch->pcdata->conviction_bank);						
		}
		if (ch->pcdata->regenerate)
			fprintf (fp, "RGN %d\n", ch->pcdata->regenerate);
		if (ch->pcdata->demonic)
			fprintf (fp, "DEM %d\n", ch->pcdata->demonic);
		if (ch->pcdata->atm)
			fprintf (fp, "ATM %d\n", ch->pcdata->atm);
		if (ch->pcdata->necromancy)
			fprintf (fp, "NEC %d\n", ch->pcdata->necromancy);
		if (ch->pcdata->obeah)
			fprintf (fp, "OBE %d\n", ch->pcdata->obeah);
		if (ch->pcdata->darkness)
			fprintf (fp, "DRK %d\n", ch->pcdata->darkness);
		if (ch->pcdata->temporis)
			fprintf (fp, "TPR %d\n", ch->pcdata->temporis);
		if (ch->pcdata->dementation)
			fprintf (fp, "D_M %d\n", ch->pcdata->dementation);

		if (ch->pcdata->silence)
			fprintf (fp, "SIL %d\n", ch->pcdata->silence);
		if (ch->pcdata->devil)
			fprintf (fp, "DVL %d\n", ch->pcdata->devil);

		if (ch->pcdata->bodymods)
			fprintf (fp, "BDM %d\n", ch->pcdata->bodymods);
		if (ch->pcdata->reina)
			fprintf (fp, "RNA %d\n", ch->pcdata->reina);
		if (ch->pcdata->bpoints)
			fprintf (fp, "BPT %d\n", ch->pcdata->bpoints);
		if (ch->pcdata->dpoints)
			fprintf (fp, "DPT %d\n", ch->pcdata->dpoints);
		if (ch->pcdata->gpoints)
			fprintf (fp, "GPT %d\n", ch->pcdata->gpoints);
		if (ch->pcdata->rpoints)
			fprintf (fp, "RPT %d\n", ch->pcdata->rpoints);
		if (ch->pcdata->wpoints)
			fprintf (fp, "WPT %d\n", ch->pcdata->wpoints);
		if (ch->pcdata->spirit)
			fprintf (fp, "SPR %d\n", ch->pcdata->spirit);
		if (ch->pcdata->conjure)
			fprintf (fp, "CJR %d\n", ch->pcdata->conjure);
		if (ch->pcdata->celerity)
			fprintf (fp, "CEL %d\n", ch->pcdata->celerity);
		if (ch->pcdata->evileye)
			fprintf (fp, "EVE %d\n", ch->pcdata->evileye);
		if (ch->pcdata->gnosis[0] != 1 || ch->pcdata->gnosis[1] != 1)
			fprintf (fp, "GNO %d %d\n", ch->pcdata->gnosis[0], ch->pcdata->gnosis[1]);
		if (ch->pcdata->rage[0] != 1 || ch->pcdata->rage[1] != 1)
			fprintf (fp, "RAG %d %d\n", ch->pcdata->rage[0], ch->pcdata->rage[1]);

		if (ch->pcdata->absorb[0])
			fprintf (fp, "ABS %d %d %d %d %d %d\n", ch->pcdata->absorb[0], ch->pcdata->absorb[1], ch->pcdata->absorb[2], ch->pcdata->absorb[3], ch->pcdata->absorb[4], ch->pcdata->absorb[5]);
		if (ch->pcdata->qstats[0])
			fprintf (fp, "Q_S %d %d %d %d %d\n", ch->pcdata->qstats[0], ch->pcdata->qstats[1], ch->pcdata->qstats[2], ch->pcdata->qstats[3], ch->pcdata->qstats[4]);
		if (ch->pcdata->language[0] | ch->pcdata->language[1])
			fprintf (fp, "LAN %d %d\n", ch->pcdata->language[0], ch->pcdata->language[1]);
		if (ch->pcdata->stage[0] | ch->pcdata->stage[1] | ch->pcdata->stage[2])
			fprintf (fp, "STG %d %d %d\n", ch->pcdata->stage[0], ch->pcdata->stage[1], ch->pcdata->stage[2]);
		if (ch->pcdata->status[0] | ch->pcdata->status[1] | ch->pcdata->status[2])
			fprintf (fp, "STA %d %d %d\n", ch->pcdata->status[0], ch->pcdata->status[1], ch->pcdata->status[2]);
		fprintf (fp, "WFF %d %d\n", ch->pcdata->wolfform[0], ch->pcdata->wolfform[1]);
		fprintf (fp, "SCO %d %d %d %d %d %d\n", ch->pcdata->score[0], ch->pcdata->score[1], ch->pcdata->score[2], ch->pcdata->score[3], ch->pcdata->score[4], ch->pcdata->score[5]);
		if (ch->pcdata->runes[0] | ch->pcdata->runes[1] | ch->pcdata->runes[2] | ch->pcdata->runes[3])
			fprintf (fp, "RNE %d %d %d %d\n", ch->pcdata->runes[0], ch->pcdata->runes[1], ch->pcdata->runes[2], ch->pcdata->runes[3]);
		if (ch->pcdata->cland[0] | ch->pcdata->cland[1] | ch->pcdata->cland[2])
			fprintf (fp, "CLD %d %d %d\n", ch->pcdata->cland[0], ch->pcdata->cland[1], ch->pcdata->cland[2]);
		if (ch->pcdata->diableries[0] | ch->pcdata->diableries[1])
			fprintf (fp, "DIA %d %d\n", ch->pcdata->diableries[0], ch->pcdata->diableries[1]);
		if (IS_WEREWOLF (ch))
		{
			fprintf (fp, "BRE %d %d %d\n", ch->pcdata->breed[0], ch->pcdata->breed[1], ch->pcdata->breed[2]);
			fprintf (fp, "AUS %d %d %d %d %d\n", ch->pcdata->auspice[0], ch->pcdata->auspice[1], ch->pcdata->auspice[2], ch->pcdata->auspice[3], ch->pcdata->auspice[4]);
			fprintf (fp, "TRB %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				 ch->pcdata->tribes[0], ch->pcdata->tribes[1], ch->pcdata->tribes[2],
				 ch->pcdata->tribes[3], ch->pcdata->tribes[4], ch->pcdata->tribes[5], ch->pcdata->tribes[6], ch->pcdata->tribes[7], ch->pcdata->tribes[8], ch->pcdata->tribes[9], ch->pcdata->tribes[10], ch->pcdata->tribes[11], ch->pcdata->tribes[12], ch->pcdata->tribes[13], ch->pcdata->tribes[14], ch->pcdata->tribes[15], ch->pcdata->tribes[16], ch->pcdata->tribes[17], ch->pcdata->tribes[18], ch->pcdata->tribes[19]);

			fprintf (fp, "RNW    %d %d %d %d %d %d\n", ch->pcdata->renown[0], ch->pcdata->renown[1], ch->pcdata->renown[2], ch->pcdata->renown[3], ch->pcdata->renown[4], ch->pcdata->renown[5]);

		}

		if (ch->pcdata->thaum[0] | ch->pcdata->thaum[1] | ch->pcdata->thaum[2] | ch->pcdata->thaum[3] | ch->pcdata->thaum[4] | ch->pcdata->thaum[5] | ch->pcdata->thaum[6] | ch->pcdata->thaum[7] | ch->pcdata->thaum[8] | ch->pcdata->thaum[9] | ch->pcdata->thaum[10] | ch->pcdata->thaum[11] | ch->pcdata->thaum[12] | ch->pcdata->thaum[13] | ch->pcdata->thaum[14] | ch->pcdata->thaum[15] | ch->pcdata->thaum[16] | ch->pcdata->thaum[17] | ch->pcdata->thaum[18] | ch->pcdata->thaum[19])
		{

			fprintf (fp, "THA %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				 ch->pcdata->thaum[0], ch->pcdata->thaum[1], ch->pcdata->thaum[2], ch->pcdata->thaum[3], ch->pcdata->thaum[4], ch->pcdata->thaum[5], ch->pcdata->thaum[6], ch->pcdata->thaum[7], ch->pcdata->thaum[8], ch->pcdata->thaum[9], ch->pcdata->thaum[10], ch->pcdata->thaum[11], ch->pcdata->thaum[12], ch->pcdata->thaum[13], ch->pcdata->thaum[14], ch->pcdata->thaum[15], ch->pcdata->thaum[16], ch->pcdata->thaum[17], ch->pcdata->thaum[18], ch->pcdata->thaum[19]);

		}
		if (ch->pcdata->powers[0] | ch->pcdata->powers[1] |
		    ch->pcdata->powers[2] | ch->pcdata->powers[3] |
		    ch->pcdata->powers[4] | ch->pcdata->powers[5] |
		    ch->pcdata->powers[6] | ch->pcdata->powers[7] |
		    ch->pcdata->powers[8] | ch->pcdata->powers[9] |
		    ch->pcdata->powers[10] | ch->pcdata->powers[11] |
		    ch->pcdata->powers[12] | ch->pcdata->powers[13] |
		    ch->pcdata->powers[14] | ch->pcdata->powers[15] |
		    ch->pcdata->powers[16] | ch->pcdata->powers[17] |
		    ch->pcdata->powers[18] | ch->pcdata->powers[19] |
		    ch->pcdata->powers[20] | ch->pcdata->powers[21] |
		    ch->pcdata->powers[22] | ch->pcdata->powers[23] | ch->pcdata->powers[24] | ch->pcdata->powers[25] | ch->pcdata->powers[26] | ch->pcdata->powers[27] | ch->pcdata->powers[28] | ch->pcdata->powers[29] | ch->pcdata->powers[30] | ch->pcdata->powers[31] | ch->pcdata->powers[32] | ch->pcdata->powers[33] | ch->pcdata->powers[34] | ch->pcdata->powers[35] | ch->pcdata->powers[36] | ch->pcdata->powers[37] | ch->pcdata->powers[38] | ch->pcdata->powers[39])
		{
			fprintf (fp, "PWR %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				 ch->pcdata->powers[0], ch->pcdata->powers[1], ch->pcdata->powers[2],
				 ch->pcdata->powers[3], ch->pcdata->powers[4], ch->pcdata->powers[5],
				 ch->pcdata->powers[6], ch->pcdata->powers[7], ch->pcdata->powers[8],
				 ch->pcdata->powers[9], ch->pcdata->powers[10], ch->pcdata->powers[11],
				 ch->pcdata->powers[12], ch->pcdata->powers[13], ch->pcdata->powers[14],
				 ch->pcdata->powers[15], ch->pcdata->powers[16], ch->pcdata->powers[17],
				 ch->pcdata->powers[18], ch->pcdata->powers[19], ch->pcdata->powers[20],
				 ch->pcdata->powers[21], ch->pcdata->powers[22], ch->pcdata->powers[23], ch->pcdata->powers[24], ch->pcdata->powers[25], ch->pcdata->powers[26], ch->pcdata->powers[27], ch->pcdata->powers[28], ch->pcdata->powers[29], ch->pcdata->powers[30], ch->pcdata->powers[31], ch->pcdata->powers[32], ch->pcdata->powers[33], ch->pcdata->powers[34], ch->pcdata->powers[35], ch->pcdata->powers[36], ch->pcdata->powers[37], ch->pcdata->powers[38], ch->pcdata->powers[39]);
		}
		if (ch->pcdata->powers_mod[0] | ch->pcdata->powers_mod[1] |
		    ch->pcdata->powers_mod[2] | ch->pcdata->powers_mod[3] |
		    ch->pcdata->powers_mod[4] | ch->pcdata->powers_mod[5] | ch->pcdata->powers_mod[6] | ch->pcdata->powers_mod[7] | ch->pcdata->powers_mod[8] | ch->pcdata->powers_mod[9] | ch->pcdata->powers_mod[10] | ch->pcdata->powers_mod[11] | ch->pcdata->powers_mod[12] | ch->pcdata->powers_mod[13] | ch->pcdata->powers_mod[14] | ch->pcdata->powers_mod[15] | ch->pcdata->powers_mod[16] | ch->pcdata->powers_mod[17] | ch->pcdata->powers_mod[18] | ch->pcdata->powers_mod[19])
		{
			fprintf (fp, "PWM %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				 ch->pcdata->powers_mod[0], ch->pcdata->powers_mod[1], ch->pcdata->powers_mod[2],
				 ch->pcdata->powers_mod[3], ch->pcdata->powers_mod[4], ch->pcdata->powers_mod[5],
				 ch->pcdata->powers_mod[6], ch->pcdata->powers_mod[7], ch->pcdata->powers_mod[8], ch->pcdata->powers_mod[9], ch->pcdata->powers_mod[10], ch->pcdata->powers_mod[11], ch->pcdata->powers_mod[12], ch->pcdata->powers_mod[13], ch->pcdata->powers_mod[14], ch->pcdata->powers_mod[15], ch->pcdata->powers_mod[16], ch->pcdata->powers_mod[17], ch->pcdata->powers_mod[18], ch->pcdata->powers_mod[19]);
		}
		if (ch->pcdata->powers_set[0] | ch->pcdata->powers_set[1] |
		    ch->pcdata->powers_set[2] | ch->pcdata->powers_set[3] |
		    ch->pcdata->powers_set[4] | ch->pcdata->powers_set[5] | ch->pcdata->powers_set[6] | ch->pcdata->powers_set[7] | ch->pcdata->powers_set[8] | ch->pcdata->powers_set[9] | ch->pcdata->powers_set[10] | ch->pcdata->powers_set[11] | ch->pcdata->powers_set[12] | ch->pcdata->powers_set[13] | ch->pcdata->powers_set[14] | ch->pcdata->powers_set[15] | ch->pcdata->powers_set[16] | ch->pcdata->powers_set[17] | ch->pcdata->powers_set[18] | ch->pcdata->powers_set[19])
		{
			fprintf (fp, "PWS %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				 ch->pcdata->powers_set[0], ch->pcdata->powers_set[1], ch->pcdata->powers_set[2],
				 ch->pcdata->powers_set[3], ch->pcdata->powers_set[4], ch->pcdata->powers_set[5],
				 ch->pcdata->powers_set[6], ch->pcdata->powers_set[7], ch->pcdata->powers_set[8], ch->pcdata->powers_set[9], ch->pcdata->powers_set[10], ch->pcdata->powers_set[11], ch->pcdata->powers_set[12], ch->pcdata->powers_set[13], ch->pcdata->powers_set[14], ch->pcdata->powers_set[15], ch->pcdata->powers_set[16], ch->pcdata->powers_set[17], ch->pcdata->powers_set[18], ch->pcdata->powers_set[19]);
		}
		if (ch->pcdata->powers_set_wear)
			fprintf (fp, "PWW %d\n", ch->pcdata->powers_set_wear);
		if (ch->pcdata->genes[0] | ch->pcdata->genes[1] | ch->pcdata->genes[2] | ch->pcdata->genes[3] | ch->pcdata->genes[4] | ch->pcdata->genes[5] | ch->pcdata->genes[6] | ch->pcdata->genes[7] | ch->pcdata->genes[8] | ch->pcdata->genes[9])
		{
			fprintf (fp, "GNS %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->genes[0], ch->pcdata->genes[1], ch->pcdata->genes[2], ch->pcdata->genes[3], ch->pcdata->genes[4], ch->pcdata->genes[5], ch->pcdata->genes[6], ch->pcdata->genes[7], ch->pcdata->genes[8], ch->pcdata->genes[9]);
		}
		if (strlen (ch->pcdata->coterie) > 1)
			fprintf (fp, "C_M %d %d %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->cot_min[0], ch->pcdata->cot_min[1], ch->pcdata->cot_min[2], ch->pcdata->cot_min[3], ch->pcdata->cot_min[4], ch->pcdata->cot_min[5], ch->pcdata->cot_min[6], ch->pcdata->cot_min[7], ch->pcdata->cot_min[8], ch->pcdata->cot_min[9], ch->pcdata->cot_min[10], ch->pcdata->cot_min[11]);

		if (ch->blood[0] != 100 || ch->blood[1] != 100)
			fprintf (fp, "BLD %d %d\n", ch->blood[0], ch->blood[1]);
		fprintf (fp, "WIL %d %d\n", ch->pcdata->willpower[0], ch->pcdata->willpower[1]);
		if (ch->pcdata->resist[0] | ch->pcdata->resist[1] | ch->pcdata->resist[2] | ch->pcdata->resist[3] | ch->pcdata->resist[4])
			fprintf (fp, "RST %d %d %d %d %d\n", ch->pcdata->resist[0], ch->pcdata->resist[1], ch->pcdata->resist[2], ch->pcdata->resist[3], ch->pcdata->resist[4]);

		if (ch->pcdata->power[0] | ch->pcdata->power[1])
			fprintf (fp, "POW %d %d\n", ch->pcdata->power[0], ch->pcdata->power[1]);

		if (ch->pcdata->fake_skill | ch->pcdata->fake_stance | ch->pcdata->fake_hit | ch->pcdata->fake_dam | ch->pcdata->fake_ac | ch->pcdata->fake_hp | ch->pcdata->fake_mana | ch->pcdata->fake_move)
			fprintf (fp, "FK1 %d %d %d %d %d %d %d %d\n", ch->pcdata->fake_skill, ch->pcdata->fake_stance, ch->pcdata->fake_hit, ch->pcdata->fake_dam, ch->pcdata->fake_ac, ch->pcdata->fake_hp, ch->pcdata->fake_mana, ch->pcdata->fake_move);

		if (ch->pcdata->fake_max_hp | ch->pcdata->fake_max_mana | ch->pcdata->fake_max_move | ch->pcdata->fake_stats[0] | ch->pcdata->fake_stats[1] | ch->pcdata->fake_stats[2] | ch->pcdata->fake_stats[3] | ch->pcdata->fake_stats[4])
			fprintf (fp, "FK2 %d %d %d %d %d %d %d %d\n", ch->pcdata->fake_max_hp, ch->pcdata->fake_max_mana, ch->pcdata->fake_max_move, ch->pcdata->fake_stats[0], ch->pcdata->fake_stats[1], ch->pcdata->fake_stats[2], ch->pcdata->fake_stats[3], ch->pcdata->fake_stats[4]);

		if (ch->pcdata->obj_vnum)
			fprintf (fp, "OBV %d\n", ch->pcdata->obj_vnum);

		if (ch->pcdata->majesty)
			fprintf (fp, "MAJ %d\n", ch->pcdata->majesty);

		fprintf (fp, "CON %d %d %d\n", ch->pcdata->condition[0], ch->pcdata->condition[1], ch->pcdata->condition[2]);

                /* Merits/Flaws Stuff */
                for (i = 0; i < 10; i++)
                {
                        if (ch->pcdata->merits[i] != NULL)
                                fprintf (fp, "MRT %s~\n", ch->pcdata->merits[i]);
                }

                for (i = 0; i < 10; i++)
                {
                        if (ch->pcdata->flaws[i] != NULL)
                                fprintf (fp, "FLW %s~\n", ch->pcdata->flaws[i]);
                }
                /* End Merits/Flaws Stuff */
		/* Remember Stuff */
		memory_ptr = ch->memory;
		while(memory_ptr != NULL)
		{
			// Print the real name followed by the remembered name to file
			fprintf (fp, "MEM %s %s~\n", memory_ptr->real_name, memory_ptr->remembered_name);
			memory_ptr = memory_ptr->next;
		}
		/* End Remember Stuff */
		for (sn = 0; sn < MAX_SKILL; sn++)
		{
			if (skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0 && skill_table[sn].skill_level <= MAX_LEVEL)
			{
				fprintf (fp, "SKL %d '%s'\n", ch->pcdata->learned[sn], skill_table[sn].name);
			}
		}
	}

	for (paf = ch->affected; paf != NULL; paf = paf->next)
	{
		/* Thx Alander */
		if (paf->type < 0 || paf->type >= MAX_SKILL)
			continue;

		fprintf (fp, "AFD '%s' %3d %3d %3d %10d\n", skill_table[paf->type].name, paf->duration, paf->modifier, paf->location, paf->bitvector);
	}

	for (extra = ch->pcdata->extra; extra; extra = extra->prev)
	 fprintf(fp, "ETR %s~ \n", extra->extra);
	fprintf (fp, "End\n\n");
	return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj (CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest)
{
	EXTRA_DESCR_DATA *ed;
	AFFECT_DATA *paf;
	bool possessed_item = FALSE;

	/*
	 * Slick recursion to write lists backwards,
	 *   so loading them will load in forwards order.
	 */
	if (obj->next_content != NULL)
		fwrite_obj (ch, obj->next_content, fp, iNest);

	if ((obj->chobj != NULL && (!IS_NPC (obj->chobj) && obj->chobj->pcdata->obj_vnum != 0)))
		possessed_item = TRUE;


	fprintf (fp, "#OBJECT\n");
	fprintf (fp, "Nest         %d\n", iNest);
	fprintf (fp, "Name         %s~\n", obj->name);
	fprintf (fp, "ShortDescr   %s~\n", obj->short_descr);
	fprintf (fp, "Description  %s~\n", obj->description);
	if (obj->chpoweron != NULL && strlen (obj->chpoweron) > 1 && str_cmp (obj->chpoweron, "(null)"))
		fprintf (fp, "Poweronch    %s~\n", obj->chpoweron);
	if (obj->chpoweroff != NULL && strlen (obj->chpoweroff) > 1 && str_cmp (obj->chpoweroff, "(null)"))
		fprintf (fp, "Poweroffch   %s~\n", obj->chpoweroff);
	if (obj->chpoweruse != NULL && strlen (obj->chpoweruse) > 1 && str_cmp (obj->chpoweruse, "(null)"))
		fprintf (fp, "Powerusech   %s~\n", obj->chpoweruse);
	if (obj->victpoweron != NULL && strlen (obj->victpoweron) > 1 && str_cmp (obj->victpoweron, "(null)"))
		fprintf (fp, "Poweronvict  %s~\n", obj->victpoweron);
	if (obj->victpoweroff != NULL && strlen (obj->victpoweroff) > 1 && str_cmp (obj->victpoweroff, "(null)"))
		fprintf (fp, "Poweroffvict %s~\n", obj->victpoweroff);
	if (obj->victpoweruse != NULL && strlen (obj->victpoweruse) > 1 && str_cmp (obj->victpoweruse, "(null)"))
		fprintf (fp, "Powerusevict %s~\n", obj->victpoweruse);
	if (obj->actcommands != NULL && strlen (obj->actcommands) > 1)
		fprintf (fp, "ActCommands  %s~\n", obj->actcommands);
	if (obj->questmaker != NULL && strlen (obj->questmaker) > 1)
		fprintf (fp, "Questmaker   %s~\n", obj->questmaker);
	if (obj->questowner != NULL && strlen (obj->questowner) > 1)
		fprintf (fp, "Questowner   %s~\n", obj->questowner);
	fprintf (fp, "Vnum         %d\n", obj->pIndexData->vnum);

	fprintf (fp, "ExtraFlags   %d\n", obj->extra_flags);
	fprintf (fp, "WearFlags    %d\n", obj->wear_flags);
	fprintf (fp, "WearLoc      %d\n", obj->wear_loc);
	fprintf (fp, "ItemType     %d\n", obj->item_type);
	fprintf (fp, "Weight       %d\n", obj->weight);
	if (obj->spectype != 0)
		fprintf (fp, "Spectype     %d\n", obj->spectype);
	if (obj->specpower != 0)
		fprintf (fp, "Specpower    %d\n", obj->specpower);
	fprintf (fp, "Condition    %d\n", obj->condition);
	fprintf (fp, "Toughness    %d\n", obj->toughness);
	fprintf (fp, "Resistance   %d\n", obj->resistance);
	if (obj->quest != 0)
		fprintf (fp, "Quest        %d\n", obj->quest);
	if (obj->points != 0)
		fprintf (fp, "Points       %d\n", obj->points);
	fprintf (fp, "Level        %d\n", obj->level);
	if (possessed_item)
	{
		fprintf (fp, "Timer        1\n");
	}
	else
		fprintf (fp, "Timer        %d\n", obj->timer);
	fprintf (fp, "Cost         %d\n", obj->cost);
	fprintf (fp, "Values       %d %d %d %d\n", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);

	switch (obj->item_type)
	{
	case ITEM_POTION:
		if (obj->value[1] > 0)
		{
			fprintf (fp, "Spell 1      '%s'\n", skill_table[obj->value[1]].name);
		}

		if (obj->value[2] > 0)
		{
			fprintf (fp, "Spell 2      '%s'\n", skill_table[obj->value[2]].name);
		}

		if (obj->value[3] > 0)
		{
			fprintf (fp, "Spell 3      '%s'\n", skill_table[obj->value[3]].name);
		}

		break;

	case ITEM_SCROLL:
		if (obj->value[1] > 0)
		{
			fprintf (fp, "Spell 1      '%s'\n", skill_table[obj->value[1]].name);
		}

		if (obj->value[2] > 0)
		{
			fprintf (fp, "Spell 2      '%s'\n", skill_table[obj->value[2]].name);
		}

		if (obj->value[3] > 0)
		{
			fprintf (fp, "Spell 3      '%s'\n", skill_table[obj->value[3]].name);
		}

		break;

	case ITEM_PILL:
	case ITEM_STAFF:
	case ITEM_WAND:
		if (obj->value[3] > 0)
		{
			fprintf (fp, "Spell 3      '%s'\n", skill_table[obj->value[3]].name);
		}

		break;
	}

	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		fprintf (fp, "AffectData   %d %d %d\n", paf->duration, paf->modifier, paf->location);
	}

	for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
	{
		fprintf (fp, "ExtraDescr   %s~ %s~\n", ed->keyword, ed->description);
	}

	fprintf (fp, "End\n\n");

	if (obj->contains != NULL)
		fwrite_obj (ch, obj->contains, fp, iNest + 1);

	return;
}



/*
 * Write an object and its contents.
 */
void fwrite_opt_obj (CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest)
{
	EXTRA_DESCR_DATA *ed;
	AFFECT_DATA *paf;
	bool possessed_item = FALSE;
	int timer;

	/*
	 * Slick recursion to write lists backwards,
	 *   so loading them will load in forwards order.
	 */
	if (obj->next_content != NULL)
		fwrite_opt_obj (ch, obj->next_content, fp, iNest);

	if ((obj->chobj != NULL && (!IS_NPC (obj->chobj) && obj->chobj->pcdata->obj_vnum != 0)))
		possessed_item = TRUE;


	if (possessed_item)
		timer = 1;
	else
		timer = obj->timer;

	/* Unmodified objects take less space than usual */
	if (strlen (obj->questmaker) < 2 && strlen (obj->questowner) < 2)
	{
		fprintf (fp, "#O %d %d %d %d %d %d %d %d %d %d %d %d %d\n", obj->pIndexData->vnum, obj->level, iNest, obj->wear_loc, obj->cost, timer, obj->condition, obj->resistance, obj->quest, obj->value[0], obj->value[1], obj->value[2], obj->value[3]);

		if (obj->contains != NULL)
			fwrite_opt_obj (ch, obj->contains, fp, iNest + 1);
		return;
	}

	fprintf (fp, "#OBJ\n");
	fprintf (fp, "NST %d\n", iNest);
	fprintf (fp, "NAM %s~\n", obj->name);
	fprintf (fp, "SDS %s~\n", obj->short_descr);
	fprintf (fp, "DDS %s~\n", obj->description);
	if (obj->chpoweron != NULL && strlen (obj->chpoweron) > 1 && str_cmp (obj->chpoweron, "(null)"))
		fprintf (fp, "PNC %s~\n", obj->chpoweron);
	if (obj->chpoweroff != NULL && strlen (obj->chpoweroff) > 1 && str_cmp (obj->chpoweroff, "(null)"))
		fprintf (fp, "PFC %s~\n", obj->chpoweroff);
	if (obj->chpoweruse != NULL && strlen (obj->chpoweruse) > 1 && str_cmp (obj->chpoweruse, "(null)"))
		fprintf (fp, "PUC %s~\n", obj->chpoweruse);
	if (obj->victpoweron != NULL && strlen (obj->victpoweron) > 1 && str_cmp (obj->victpoweron, "(null)"))
		fprintf (fp, "PNV  %s~\n", obj->victpoweron);
	if (obj->victpoweroff != NULL && strlen (obj->victpoweroff) > 1 && str_cmp (obj->victpoweroff, "(null)"))
		fprintf (fp, "PFV %s~\n", obj->victpoweroff);
	if (obj->victpoweruse != NULL && strlen (obj->victpoweruse) > 1 && str_cmp (obj->victpoweruse, "(null)"))
		fprintf (fp, "PUV %s~\n", obj->victpoweruse);
	if (obj->actcommands != NULL && strlen (obj->actcommands) > 1)
		fprintf (fp, "ACM %s~\n", obj->actcommands);
	if (obj->questmaker != NULL && strlen (obj->questmaker) > 1)
		fprintf (fp, "QMK %s~\n", obj->questmaker);
	if (obj->questowner != NULL && strlen (obj->questowner) > 1)
		fprintf (fp, "QON %s~\n", obj->questowner);

	fprintf (fp, "VNM %d\n", obj->pIndexData->vnum);
	fprintf (fp, "EFL %d\n", obj->extra_flags);
	fprintf (fp, "WFL %d\n", obj->wear_flags);
	if (obj->wear_loc != -1)
		fprintf (fp, "WLC %d\n", obj->wear_loc);
	fprintf (fp, "ITT %d\n", obj->item_type);
	fprintf (fp, "WEI %d\n", obj->weight);
	if (obj->spectype)
		fprintf (fp, "SPT %d\n", obj->spectype);
	if (obj->specpower)
		fprintf (fp, "SPP %d\n", obj->specpower);
	if (obj->condition != 100)
		fprintf (fp, "CON %d\n", obj->condition);
	if (obj->toughness != 5)
		fprintf (fp, "TOU %d\n", obj->toughness);
	if (obj->toughness != 25)
		fprintf (fp, "RES %d\n", obj->resistance);
	if (obj->quest)
		fprintf (fp, "QST %d\n", obj->quest);
	if (obj->points)
		fprintf (fp, "PTS %d\n", obj->points);
	fprintf (fp, "LVL %d\n", obj->level);
	if (obj->timer)
		fprintf (fp, "TMR %d\n", obj->timer);
	fprintf (fp, "CST %d\n", obj->cost);
	fprintf (fp, "VAL %d %d %d %d\n", obj->value[0], obj->value[1], obj->value[2], obj->value[3]);

	switch (obj->item_type)
	{
	case ITEM_POTION:
		if (obj->value[1] > 0)
		{
			fprintf (fp, "SPL 1 '%s'\n", skill_table[obj->value[1]].name);
		}

		if (obj->value[2] > 0)
		{
			fprintf (fp, "SPL 2 '%s'\n", skill_table[obj->value[2]].name);
		}

		if (obj->value[3] > 0)
		{
			fprintf (fp, "SPL 3 '%s'\n", skill_table[obj->value[3]].name);
		}

		break;

	case ITEM_SCROLL:
		if (obj->value[1] > 0)
		{
			fprintf (fp, "SPL 1 '%s'\n", skill_table[obj->value[1]].name);
		}

		if (obj->value[2] > 0)
		{
			fprintf (fp, "SPL 2 '%s'\n", skill_table[obj->value[2]].name);
		}

		if (obj->value[3] > 0)
		{
			fprintf (fp, "SPL 3 '%s'\n", skill_table[obj->value[3]].name);
		}

		break;

	case ITEM_PILL:
	case ITEM_STAFF:
	case ITEM_WAND:
		if (obj->value[3] > 0)
		{
			fprintf (fp, "SPL 3 '%s'\n", skill_table[obj->value[3]].name);
		}

		break;
	}

	for (paf = obj->affected; paf != NULL; paf = paf->next)
	{
		fprintf (fp, "AFD %d %d %d\n", paf->duration, paf->modifier, paf->location);
	}
	for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
	{
		fprintf (fp, "EXD %s~ %s~\n", ed->keyword, ed->description);
	}
	fprintf (fp, "End\n\n");

	if (obj->contains != NULL)
		fwrite_opt_obj (ch, obj->contains, fp, iNest + 1);

	return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj (DESCRIPTOR_DATA * d, char *name)
{
	static PC_DATA pcdata_zero;
	char strsave[MAX_STRING_LENGTH];
	char *strtime;
	CHAR_DATA *ch;
	FILE *fp;
	bool found;
	int i;
	int j;
	if (char_free == NULL)
	{
		ch = alloc_perm (sizeof (*ch));
	}
	else
	{
		ch = char_free;
		char_free = char_free->next;
	}
	clear_char (ch);

	if (pcdata_free == NULL)
	{
		ch->pcdata = alloc_perm (sizeof (*ch->pcdata));
	}
	else
	{
		ch->pcdata = pcdata_free;
		pcdata_free = pcdata_free->next;
	}
	*ch->pcdata = pcdata_zero;

	d->character = ch;
	ch->desc = d;
	ch->name = str_dup (name);
	ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
	ch->extra = 0;
	ch->extra2 = 0;
	ch->more = 0;
	ch->more2 = 0;
	ch->more3 = 0;
	ch->quintessence = 0;
	ch->max_quintessence = 0;
	ch->plane = 0;
	ch->ringtone = str_dup("");
	ch->phonevibrate = FALSE;
	ch->pcdata->familiar = NULL;
	ch->pcdata->partner = NULL;
	ch->pcdata->propose = NULL;
	ch->pcdata->chobj = NULL;
	ch->pcdata->memorised = NULL;
	ch->pcdata->board = &boards[DEFAULT_BOARD];
	ch->pcdata->soul_keeper = str_dup ("");
	ch->pcdata->soul = 10;
	ch->pcdata->thaum_prime = -1;
	ch->pcdata->research_power = -1;
	ch->pcdata->research_time = 0;
	ch->pcdata->research_type = 0;
	ch->pcdata->rp_counter = 0;
	ch->pcdata->research_tick = 0;
	ch->pcdata->infernal_ranking = 0;
	ch->pcdata->pwd = str_dup ("");
	/* new xp code */
	ch->pcdata->votes = 0;
	ch->pcdata->last_vote_time.year = 0;
	ch->pcdata->last_vote_time.month = 0;
	ch->pcdata->last_vote_time.day = 0;
	/* end */
	ch->pcdata->bamfin = str_dup ("");
	ch->pcdata->bamfout = str_dup ("");
	ch->pcdata->lupusdescshort = str_dup ("");
	ch->pcdata->crinosdescshort = str_dup ("");
	ch->pcdata->hispodescshort = str_dup ("");
	ch->pcdata->glabrodescshort = str_dup ("");
	ch->pcdata->title = str_dup ("");
	ch->pcdata->pretitle = str_dup("");
	ch->pcdata->conception = str_dup ("");
	ch->pcdata->parents = str_dup ("");
	ch->pcdata->cparents = str_dup ("");
	ch->pcdata->marriage = str_dup ("");
	ch->pcdata->email = str_dup ("");
	ch->pcdata->conding = str_dup ("");
	ch->pcdata->love = str_dup ("");
	ch->pcdata->coterie = str_dup ("");
	ch->pcdata->cot_time = str_dup ("None");
	ch->pcdata->cot_clans = str_dup ("All");
	ch->pcdata->cot_sides = str_dup ("All");
	ch->pcdata->leader = str_dup ("");
	ch->pcdata->stancename = str_dup ("");
	ch->pcdata->stancelist = str_dup ("");
	ch->pcdata->stancework = str_dup ("");
	ch->pcdata->doing = str_dup ("");
/* Aura Code */
	ch->pcdata->aura = str_dup ("");
	ch->pcdata->voice = str_dup ("");
	ch->pcdata->roomdesc = str_dup ("");
	ch->pcdata->show_social = FALSE;
	ch->pcdata->lupusdesc = str_dup ("");
	ch->pcdata->crinosdesc = str_dup ("");
	ch->pcdata->hispodesc = str_dup ("");
	ch->pcdata->glabrodesc = str_dup ("");
	ch->pcdata->tempdesc = str_dup ("");
	ch->pcdata->showtempdesc = FALSE;
	ch->pcdata->giftlist = str_dup ("");
	ch->lord = str_dup ("");
	ch->clan = str_dup ("");
	ch->side = str_dup ("");
	ch->nature = str_dup("");
	ch->demeanor = str_dup("");
	ch->order = str_dup("");
	ch->impulse = str_dup("");
	ch->morph = str_dup ("");
	ch->mem_name = str_dup ("");
	ch->mem_desc = str_dup ("");
	ch->pload = str_dup ("");
	ch->prompt = str_dup ("");
	ch->cprompt = str_dup ("");
	strtime = ctime (&current_time);
	strtime[strlen (strtime) - 1] = '\0';
	free_string (ch->lasttime);
	ch->createtime = str_dup (strtime);
	ch->lasttime = str_dup ("");
	ch->lasthost = str_dup ("");
	ch->poweraction = str_dup ("");
	ch->powertype = str_dup ("");
	ch->hunting = str_dup ("");
	ch->pcdata->denydate = 0;
	ch->pcdata->stancemove[0] = 0;
	ch->pcdata->stancemove[1] = 0;
	ch->pcdata->stancemove[2] = 0;
	ch->pcdata->stancemove[3] = 0;
	ch->pcdata->stancemove[4] = 0;
	ch->spectype = 0;
	ch->specpower = 0;
	ch->mounted = 0;
	ch->home = 3700;
	ch->agg = 0;
	ch->vampgen = 0;
	ch->paradox[0] = 0;	/* Total paradox */
	ch->paradox[1] = 0;	/* Current Paradox */
	ch->paradox[2] = 0;	/* Paradox Ticker */
	ch->damcap[0] = 1000;
	ch->damcap[1] = 0;
	ch->vampaff = 0;
	ch->itemaffect = 0;
	ch->vamppass = -1;
	ch->polyaff = 0;
	ch->immune = 0;
	ch->form = 0;
	ch->init = 0;
	ch->body = 32767;
	ch->beast = 20;
	ch->truebeast = 20;
	ch->autostance = 0;
	ch->pcdata->virtues[VIRTUE_CONSCIENCE] = 1;
	ch->pcdata->virtues[VIRTUE_SELFCONTROL] = 1;
	ch->pcdata->virtues[VIRTUE_COURAGE] = 1;
	if (IS_INQUISITOR(ch))
	{
		ch->pcdata->virtues[VIRTUE_WISDOM] = 1;
		ch->pcdata->virtues[VIRTUE_FAITH] = 1;
		ch->pcdata->virtues[VIRTUE_ZEAL] = 1;
	}
	ch->loc_hp[0] = 0;
	ch->loc_hp[1] = 0;
	ch->loc_hp[2] = 0;
	ch->loc_hp[3] = 0;
	ch->loc_hp[4] = 0;
	ch->loc_hp[5] = 0;
	ch->loc_hp[6] = 0;
	ch->wpn[0] = 0;
	ch->wpn[1] = 0;
	ch->wpn[2] = 0;
	ch->wpn[3] = 0;
	ch->wpn[4] = 0;
	ch->wpn[5] = 0;
	ch->wpn[6] = 0;
	ch->wpn[7] = 0;
	ch->wpn[8] = 0;
	ch->wpn[9] = 0;
	ch->wpn[10] = 0;
	ch->wpn[11] = 0;
	ch->wpn[12] = 0;
	ch->spl[0] = 4;
	ch->spl[1] = 4;
	ch->spl[2] = 4;
	ch->spl[3] = 4;
	ch->spl[4] = 4;
	ch->cmbt[0] = 0;
	ch->cmbt[1] = 0;
	ch->cmbt[2] = 0;
	ch->cmbt[3] = 0;
	ch->cmbt[4] = 0;
	ch->cmbt[5] = 0;
	ch->cmbt[6] = 0;
	ch->cmbt[7] = 0;
	ch->stance[0] = 0;
	ch->stance[1] = 0;
	ch->stance[2] = 0;
	ch->stance[3] = 0;
	ch->stance[4] = 0;
	ch->stance[5] = 0;
	ch->stance[6] = 0;
	ch->stance[7] = 0;
	ch->stance[8] = 0;
	ch->stance[9] = 0;
	ch->stance[10] = 0;
	ch->stance[11] = 0;
	ch->stance[12] = 0;
	ch->stance[13] = 0;
	ch->stance[14] = 0;
	ch->stance[15] = 0;
	ch->pkill = 0;
	ch->pdeath = 0;
	ch->mkill = 0;
	ch->mdeath = 0;
	ch->embraced = 0;
	ch->blood[0] = 10;
	ch->blood[1] = 10;
	ch->blood[2] = 1;
	ch->pcdata->aggdamage = 0;
	ch->pcdata->bashingdamage = 0;
	ch->pcdata->lethaldamage = 0;
	ch->pcdata->wp_regain_counter = 0;
	ch->pcdata->statusvotecounter = 0;
	ch->pcdata->canbuycounter = 0;
	ch->pcdata->canvotecounter = 0;
	ch->pcdata->canbuy = 0;
	ch->pcdata->sectstatus = 0;
	ch->pcdata->blood_burn_counter = 0;
	ch->pcdata->auto_vote_counter = 0;
	ch->pcdata->perm_str = 13;
	ch->pcdata->perm_int = 13;
	ch->pcdata->perm_wis = 13;
	ch->pcdata->perm_dex = 13;
	ch->pcdata->perm_con = 13;
	ch->pcdata->quest = 0;
	ch->pcdata->wolf = 0;
	ch->pcdata->rank = 0;
	ch->pcdata->faith = 0;
	if (IS_INQUISITOR(ch))
	{
		ch->pcdata->conviction[CONVICTION_CURRENT] = 0;
		ch->pcdata->conviction[CONVICTION_MAX] = 0;
		ch->pcdata->conviction_bank = 0;
	}
	ch->pcdata->regenerate = 0;
	ch->pcdata->demonic = 0;
	ch->pcdata->atm = 0;
	ch->pcdata->song = 0;
	ch->pcdata->necromancy = 0;
	ch->pcdata->obeah = 0;
	ch->pcdata->darkness = 0;
	ch->pcdata->temporis = 0;
	ch->pcdata->silence = 0;
	ch->pcdata->devil = 0;
	ch->pcdata->dementation = 0;
	ch->pcdata->bodymods = 0;
	ch->pcdata->vision = 3;
	ch->pcdata->reina = 0;
	ch->pcdata->bpoints = 0;
	ch->pcdata->dpoints = 0;
	ch->pcdata->gpoints = 0;
	ch->pcdata->rpoints = 0;
	ch->pcdata->wpoints = 0;
	ch->pcdata->spirit = 0;
	ch->pcdata->conjure = 0;
	ch->pcdata->celerity = 0;
	ch->pcdata->firstaid = 0;
	ch->pcdata->repair = 0;
	ch->pcdata->teach = 0;
	ch->pcdata->learn = 0;
	ch->pcdata->notes = 0;
	ch->pcdata->evileye = 0;
	ch->pcdata->followers = 0;
	ch->pcdata->gnosis[0] = 1;
	ch->pcdata->gnosis[1] = 1;
	ch->pcdata->rage[0] = 1;
	ch->pcdata->rage[1] = 1;
	ch->pcdata->absorb[0] = 0;
	ch->pcdata->absorb[1] = 0;
	ch->pcdata->absorb[2] = 0;
	ch->pcdata->absorb[3] = 0;
	ch->pcdata->absorb[4] = 0;
	ch->pcdata->absorb[5] = 0;
	ch->pcdata->qstats[0] = 0;
	ch->pcdata->qstats[1] = 0;
	ch->pcdata->qstats[2] = 0;
	ch->pcdata->qstats[3] = 0;
	ch->pcdata->qstats[4] = 0;
	ch->pcdata->exercise[0] = 0;
	ch->pcdata->exercise[1] = 0;
	ch->pcdata->exercise[2] = 0;
	ch->pcdata->exercise[3] = 0;
	ch->pcdata->language[0] = 0;
	ch->pcdata->language[1] = 0;
	ch->pcdata->stage[0] = 0;
	ch->pcdata->stage[1] = 0;
	ch->pcdata->stage[2] = 0;
	ch->pcdata->status[0] = 0;
	ch->pcdata->status[1] = 0;
	ch->pcdata->status[2] = 0;
	ch->pcdata->wolfform[0] = 1;
	ch->pcdata->wolfform[1] = 1;
	ch->pcdata->score[0] = 0;
	ch->pcdata->score[1] = 0;
	ch->pcdata->score[2] = 0;
	ch->pcdata->score[3] = 0;
	ch->pcdata->score[4] = 0;
	ch->pcdata->score[5] = 0;
	ch->pcdata->runes[0] = 0;
	ch->pcdata->runes[1] = 0;
	ch->pcdata->runes[2] = 0;
	ch->pcdata->runes[3] = 0;
	ch->pcdata->disc[0] = 0;
	ch->pcdata->disc[1] = 0;
	ch->pcdata->disc[2] = 0;
	ch->pcdata->disc[3] = 0;
	ch->pcdata->disc[4] = 0;
	ch->pcdata->disc[5] = 0;
	ch->pcdata->disc[6] = 0;
	ch->pcdata->disc[7] = 0;
	ch->pcdata->disc[8] = 0;
	ch->pcdata->disc[9] = 0;
	ch->pcdata->disc[10] = 0;
	ch->pcdata->research[0] = 0;
	ch->pcdata->research[1] = 0;

	ch->pcdata->rituals[0] = 0;
	ch->pcdata->rituals[1] = 0;
	ch->pcdata->rituals[2] = 0;
	ch->pcdata->rituals[3] = 0;
	ch->pcdata->rituals[4] = 0;
	ch->pcdata->rituals[5] = 0;
	ch->pcdata->rituals[6] = 0;
	ch->pcdata->rituals[7] = 0;
	ch->pcdata->rituals[8] = 0;
	ch->pcdata->rituals[9] = 0;
	ch->pcdata->cland[0] = 0;
	ch->pcdata->cland[1] = 0;
	ch->pcdata->cland[2] = 0;
	ch->pcdata->extended_char_creation_in_progress = FALSE;
	ch->pcdata->diableries[0] = 0;
	ch->pcdata->diableries[1] = 0;
	ch->pcdata->renown[0] = 0;
	ch->pcdata->renown[1] = 0;
	ch->pcdata->renown[2] = 0;
	ch->pcdata->renown[3] = 0;
	ch->pcdata->renown[4] = 0;
	ch->pcdata->renown[5] = 0;

	ch->pcdata->breed[0] = 0;
	ch->pcdata->breed[1] = 0;
	ch->pcdata->breed[2] = 0;
	ch->pcdata->auspice[0] = 0;
	ch->pcdata->auspice[1] = 0;
	ch->pcdata->auspice[2] = 0;
	ch->pcdata->auspice[3] = 0;
	ch->pcdata->auspice[4] = 0;
	ch->pcdata->tribes[0] = 0;
	ch->pcdata->tribes[1] = 0;
	ch->pcdata->tribes[2] = 0;
	ch->pcdata->tribes[3] = 0;
	ch->pcdata->tribes[4] = 0;
	ch->pcdata->tribes[5] = 0;
	ch->pcdata->tribes[6] = 0;
	ch->pcdata->tribes[7] = 0;
	ch->pcdata->tribes[8] = 0;
	ch->pcdata->tribes[9] = 0;
	ch->pcdata->tribes[10] = 0;
	ch->pcdata->tribes[11] = 0;
	ch->pcdata->tribes[12] = 0;
	ch->pcdata->tribes[13] = 0;
	ch->pcdata->tribes[14] = 0;
	ch->pcdata->tribes[15] = 0;
	ch->pcdata->tribes[16] = 0;
	ch->pcdata->tribes[17] = 0;
	ch->pcdata->tribes[18] = 0;
	ch->pcdata->tribes[19] = 0;

	ch->pcdata->thaum[0] = 0;
	ch->pcdata->thaum[1] = 0;
	ch->pcdata->thaum[2] = 0;
	ch->pcdata->thaum[3] = 0;
	ch->pcdata->thaum[4] = 0;
	ch->pcdata->thaum[5] = 0;
	ch->pcdata->thaum[6] = 0;
	ch->pcdata->thaum[7] = 0;
	ch->pcdata->thaum[8] = 0;
	ch->pcdata->thaum[9] = 0;
	ch->pcdata->thaum[10] = 0;
	ch->pcdata->thaum[11] = 0;
	ch->pcdata->thaum[12] = 0;
	ch->pcdata->thaum[13] = 0;
	ch->pcdata->thaum[14] = 0;
	ch->pcdata->thaum[15] = 0;
	ch->pcdata->thaum[16] = 0;
	ch->pcdata->thaum[17] = 0;
	ch->pcdata->thaum[18] = 0;
	ch->pcdata->thaum[19] = 0;

	ch->pcdata->powers[0] = 0;
	ch->pcdata->powers[1] = 0;
	ch->pcdata->powers[2] = 0;
	ch->pcdata->powers[3] = 0;
	ch->pcdata->powers[4] = 0;
	ch->pcdata->powers[5] = 0;
	ch->pcdata->powers[6] = 0;
	ch->pcdata->powers[7] = 0;
	ch->pcdata->powers[8] = 0;
	ch->pcdata->powers[9] = 0;
	ch->pcdata->powers[10] = 0;
	ch->pcdata->powers[11] = 0;
	ch->pcdata->powers[12] = 0;
	ch->pcdata->powers[13] = 0;
	ch->pcdata->powers[14] = 0;
	ch->pcdata->powers[15] = 0;
	ch->pcdata->powers[16] = 0;
	ch->pcdata->powers[17] = 0;
	ch->pcdata->powers[18] = 0;
	ch->pcdata->powers[19] = 0;
	ch->pcdata->powers[20] = 0;
	ch->pcdata->powers[21] = 0;
	ch->pcdata->powers[22] = 0;
	ch->pcdata->powers[23] = 0;
	ch->pcdata->powers[24] = 0;
	ch->pcdata->powers[25] = 0;
	ch->pcdata->powers[26] = 0;
	ch->pcdata->powers[27] = 0;
	ch->pcdata->powers[28] = 0;
	ch->pcdata->powers[29] = 0;
	ch->pcdata->powers[30] = 0;
	ch->pcdata->powers[31] = 0;
	ch->pcdata->powers[32] = 0;
	ch->pcdata->powers[33] = 0;
	ch->pcdata->powers[34] = 0;
	ch->pcdata->powers[35] = 0;
	ch->pcdata->powers[36] = 0;
	ch->pcdata->powers[37] = 0;
	ch->pcdata->powers[38] = 0;
	ch->pcdata->powers[39] = 0;
	ch->pcdata->powers_mod[0] = 0;
	ch->pcdata->powers_mod[1] = 0;
	ch->pcdata->powers_mod[2] = 0;
	ch->pcdata->powers_mod[3] = 0;
	ch->pcdata->powers_mod[4] = 0;
	ch->pcdata->powers_mod[5] = 0;
	ch->pcdata->powers_mod[6] = 0;
	ch->pcdata->powers_mod[7] = 0;
	ch->pcdata->powers_mod[8] = 0;
	ch->pcdata->powers_mod[9] = 0;
	ch->pcdata->powers_mod[10] = 0;
	ch->pcdata->powers_mod[11] = 0;
	ch->pcdata->powers_mod[12] = 0;
	ch->pcdata->powers_mod[13] = 0;
	ch->pcdata->powers_mod[14] = 0;
	ch->pcdata->powers_mod[15] = 0;
	ch->pcdata->powers_mod[16] = 0;
	ch->pcdata->powers_mod[17] = 0;
	ch->pcdata->powers_mod[18] = 0;
	ch->pcdata->powers_mod[19] = 0;
	ch->pcdata->powers_set[0] = 0;
	ch->pcdata->powers_set[1] = 0;
	ch->pcdata->powers_set[2] = 0;
	ch->pcdata->powers_set[3] = 0;
	ch->pcdata->powers_set[4] = 0;
	ch->pcdata->powers_set[5] = 0;
	ch->pcdata->powers_set[6] = 0;
	ch->pcdata->powers_set[7] = 0;
	ch->pcdata->powers_set[8] = 0;
	ch->pcdata->powers_set[9] = 0;
	ch->pcdata->powers_set[10] = 0;
	ch->pcdata->powers_set[11] = 0;
	ch->pcdata->powers_set[12] = 0;
	ch->pcdata->powers_set[13] = 0;
	ch->pcdata->powers_set[14] = 0;
	ch->pcdata->powers_set[15] = 0;
	ch->pcdata->powers_set[16] = 0;
	ch->pcdata->powers_set[17] = 0;
	ch->pcdata->powers_set[18] = 0;
	ch->pcdata->powers_set[19] = 0;
	ch->pcdata->powers_set_wear = 0;
	ch->pcdata->genes[0] = 0;
	ch->pcdata->genes[1] = 0;
	ch->pcdata->genes[2] = 0;
	ch->pcdata->genes[3] = 0;
	ch->pcdata->genes[4] = 0;
	ch->pcdata->genes[5] = 0;
	ch->pcdata->genes[6] = 0;
	ch->pcdata->genes[7] = 0;
	ch->pcdata->genes[8] = 0;
	ch->pcdata->genes[9] = 0;
	ch->pcdata->cot_min[0] = 0;
	ch->pcdata->cot_min[1] = 0;
	ch->pcdata->cot_min[2] = 0;
	ch->pcdata->cot_min[3] = 0;
	ch->pcdata->cot_min[4] = 0;
	ch->pcdata->cot_min[5] = 0;
	ch->pcdata->cot_min[6] = 0;
	ch->pcdata->cot_min[7] = 0;
	ch->pcdata->cot_min[8] = 0;
	ch->pcdata->cot_min[9] = 0;
	ch->pcdata->cot_min[10] = 0;
	ch->pcdata->cot_min[11] = 0;
	ch->pcdata->willpower[0] = 1;
	ch->pcdata->willpower[1] = 1;
	ch->pcdata->resist[0] = 0;
	ch->pcdata->resist[1] = 0;
	ch->pcdata->resist[2] = 0;
	ch->pcdata->resist[3] = 0;
	ch->pcdata->resist[4] = 0;
	ch->pcdata->power[0] = 0;
	ch->pcdata->power[1] = 0;
	ch->pcdata->fake_skill = 0;
	ch->pcdata->fake_stance = 0;
	ch->pcdata->fake_hit = 0;
	ch->pcdata->fake_dam = 0;
	ch->pcdata->fake_ac = 0;
	ch->pcdata->fake_hp = 0;
	ch->pcdata->fake_mana = 0;
	ch->pcdata->fake_move = 0;
	ch->pcdata->fake_max_hp = 0;
	ch->pcdata->fake_max_mana = 0;
	ch->pcdata->fake_max_move = 0;
	ch->pcdata->fake_stats[STAT_STR] = 0;
	ch->pcdata->fake_stats[STAT_INT] = 0;
	ch->pcdata->fake_stats[STAT_WIS] = 0;
	ch->pcdata->fake_stats[STAT_DEX] = 0;
	ch->pcdata->fake_stats[STAT_CON] = 0;
	ch->pcdata->obj_vnum = 0;
	ch->pcdata->majesty = 0;
	ch->pcdata->exhaustion = 0;
	ch->pcdata->lie = 0;
	ch->pcdata->class = 0;
	ch->pcdata->road = -1;
	ch->pcdata->roadt = 1;
	ch->pcdata->board = &boards[DEFAULT_BOARD];
	ch->health_level = HEALTH_NORMAL;
	ch->home_x = 0;
	ch->home_y = 0;
	ch->room = 0;
	ch->state = 0;
	ch->action = 0;
	ch->quality = 0;
	ch->ploaded = FALSE;

	/* Merits/Flaws Stuff */
	ch->pcdata->merits = (char **)malloc(sizeof(char *) * 10); // 10 Spots for Merits
	ch->pcdata->flaws = (char **)malloc(sizeof(char *) * 10); // 10 Spots for Flaws

	for (i = 0; i < 10; i++)
	{
		ch->pcdata->merits[i] = NULL;
		ch->pcdata->flaws[i] = NULL;
	}
	/* End Merits/Flaws Stuff */

	ch->pcdata->in_progress = NULL;

	ch->attributes[ATTRIB_STR] = 1;
	ch->attributes[ATTRIB_DEX] = 1;
	ch->attributes[ATTRIB_STA] = 1;
	ch->attributes[ATTRIB_CHA] = 1;
	ch->attributes[ATTRIB_MAN] = 1;
	ch->attributes[ATTRIB_APP] = 1;
	ch->attributes[ATTRIB_PER] = 1;
	ch->attributes[ATTRIB_INT] = 1;
	ch->attributes[ATTRIB_WIT] = 1;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			ch->abilities[i][j] = 0;
			ch->pcdata->abilities_points[i][j] = 0;
		}
	}

	found = FALSE;
	fflush (fpReserve);
	fclose (fpReserve);
	sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (name));
	if ((fp = fopen (strsave, "r")) != NULL)
	{
		int iNest;

		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;

		found = TRUE;
		for (;;)
		{
			char letter;
			char *word;

			letter = fread_letter (fp);
			if (letter == '*')
			{
				fread_to_eol (fp);
				continue;
			}

			if (letter != '#')
			{
				bug ("Load_char_obj: # not found.", 0);
				break;
			}

			word = fread_word (fp);
			if (!str_cmp (word, "PLAYER"))
				fread_char (ch, fp);
			else if (!str_cmp (word, "PLR"))
				fread_opt_char (ch, fp);
			else if (!str_cmp (word, "OBJECT"))
				fread_obj (ch, fp);
			else if (!str_cmp (word, "OBJ"))
				fread_opt_obj (ch, fp);
			else if (!str_cmp (word, "O"))
				fread_tiny_obj (ch, fp);
			else if (!str_cmp (word, "END"))
				break;
			else
			{
				bug ("Load_char_obj: bad section.", 0);
				break;
			}
		}
		fflush (fp);
		fclose (fp);
	}

	fpReserve = fopen (NULL_FILE, "r");
	return found;
}



bool load_char_short (DESCRIPTOR_DATA * d, char *name)
{
	static PC_DATA pcdata_zero;
	char strsave[MAX_STRING_LENGTH];
	CHAR_DATA *ch;
	char *strtime;
	FILE *fp;
	bool found;
	int i;
	int j;

	if (char_free == NULL)
	{
		ch = alloc_perm (sizeof (*ch));
	}
	else
	{
		ch = char_free;
		char_free = char_free->next;
	}
	clear_char (ch);

	if (pcdata_free == NULL)
	{
		ch->pcdata = alloc_perm (sizeof (*ch->pcdata));
	}
	else
	{
		ch->pcdata = pcdata_free;
		pcdata_free = pcdata_free->next;
	}
	*ch->pcdata = pcdata_zero;

	d->character = ch;
	ch->desc = d;
	ch->name = str_dup (name);
	ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
	ch->extra = 0;
	ch->extra2 = 0;
	ch->more = 0;
	ch->more2 = 0;
	ch->more3 = 0;
	ch->pcdata->familiar = NULL;
	ch->pcdata->partner = NULL;
	ch->pcdata->propose = NULL;
	ch->pcdata->chobj = NULL;
	ch->pcdata->memorised = NULL;
	ch->pcdata->doing = NULL;
/* Aura Code */
	ch->pcdata->aura = NULL;
	ch->pcdata->voice = NULL;	
	ch->pcdata->roomdesc = NULL;
	ch->pcdata->show_social = FALSE;
	ch->pcdata->soul_keeper = str_dup ("");
	ch->pcdata->soul = 10;
	ch->pcdata->thaum_prime = -1;
	ch->pcdata->research_power = -1;
	ch->pcdata->research_time = 0;
	ch->pcdata->research_tick = 0;
	ch->pcdata->research_type = 0;
	ch->pcdata->rp_counter = 0;
	ch->pcdata->infernal_ranking = 0;
	ch->pcdata->pwd = str_dup ("");
	/* New XP Code */
	ch->pcdata->votes = 0;
	ch->pcdata->last_vote_time.year = 0;
	ch->pcdata->last_vote_time.month = 0;
	ch->pcdata->last_vote_time.day = 0;
	/* End XP Code */
	/* Remember Code */
	ch->memory = NULL;
	/* End Remember Code */
	ch->ringtone = str_dup("");
	ch->phonevibrate = FALSE;
	ch->pcdata->wp_regain_counter = 0;
	ch->pcdata->statusvotecounter = 0;
	ch->pcdata->canbuycounter = 0;
	ch->pcdata->canvotecounter = 0;
	ch->pcdata->canbuy = 0;
	ch->pcdata->sectstatus = 0;
	ch->pcdata->blood_burn_counter = 0;
	ch->pcdata->auto_vote_counter = 0;		
	ch->pcdata->bamfout = str_dup ("");
	ch->pcdata->lupusdescshort = str_dup ("");
	ch->pcdata->crinosdescshort = str_dup ("");
	ch->pcdata->hispodescshort = str_dup ("");
	ch->pcdata->glabrodescshort = str_dup ("");
	ch->pcdata->title = str_dup ("");
	ch->pcdata->pretitle = str_dup("");
	ch->pcdata->conception = str_dup ("");
	ch->pcdata->parents = str_dup ("");
	ch->pcdata->cparents = str_dup ("");
	ch->pcdata->marriage = str_dup ("");
	ch->pcdata->email = str_dup ("");
	ch->pcdata->conding = str_dup ("");
	ch->pcdata->love = str_dup ("");
	ch->pcdata->coterie = str_dup ("");
	ch->pcdata->cot_time = str_dup ("None");
	ch->pcdata->cot_clans = str_dup ("All");
	ch->pcdata->cot_sides = str_dup ("All");
	ch->pcdata->leader = str_dup ("");
	ch->pcdata->stancename = str_dup ("");
	ch->pcdata->stancelist = str_dup ("");
	ch->pcdata->stancework = str_dup ("");
	ch->pcdata->stancework = str_dup ("");
	ch->pcdata->lupusdesc = str_dup ("");
	ch->pcdata->crinosdesc = str_dup ("");
	ch->pcdata->hispodesc = str_dup ("");
	ch->pcdata->glabrodesc = str_dup ("");
	ch->pcdata->giftlist = str_dup ("");
	ch->pcdata->tempdesc = str_dup ("");
	ch->pcdata->showtempdesc = FALSE;
	ch->lord = str_dup ("");
	ch->clan = str_dup ("");
	ch->side = str_dup ("");
	ch->nature = str_dup("");
	ch->demeanor = str_dup("");
	ch->order = str_dup("");
	ch->impulse = str_dup("");
	ch->morph = str_dup ("");
	ch->mem_name = str_dup ("");
	ch->mem_desc = str_dup ("");
	ch->pload = str_dup ("");
	ch->prompt = str_dup ("");
	ch->cprompt = str_dup ("");
	strtime = ctime (&current_time);
	strtime[strlen (strtime) - 1] = '\0';
	free_string (ch->lasttime);
	ch->createtime = str_dup (strtime);
	ch->lasttime = str_dup ("");
	ch->lasthost = str_dup ("");
	ch->poweraction = str_dup ("");
	ch->powertype = str_dup ("");
	ch->hunting = str_dup ("");
	ch->pcdata->doing = str_dup ("");
/* Aura Code */
	ch->pcdata->aura = str_dup ("");
	ch->pcdata->voice = str_dup ("");
	ch->pcdata->roomdesc = str_dup ("");
	ch->pcdata->show_social = FALSE;
	ch->pcdata->denydate = 0;
	ch->pcdata->stancemove[0] = 0;
	ch->pcdata->stancemove[1] = 0;
	ch->pcdata->stancemove[2] = 0;
	ch->pcdata->stancemove[3] = 0;
	ch->pcdata->stancemove[4] = 0;
	ch->spectype = 0;
	ch->specpower = 0;
	ch->mounted = 0;
	ch->home = 3700;
	ch->agg = 0;
	ch->vampgen = 0;
	ch->pcdata->virtues[VIRTUE_CONSCIENCE] = 1;
	ch->pcdata->virtues[VIRTUE_SELFCONTROL] = 1;
	ch->pcdata->virtues[VIRTUE_COURAGE] = 1;
	if (IS_INQUISITOR(ch))
	{
		ch->pcdata->virtues[VIRTUE_WISDOM] = 1;
		ch->pcdata->virtues[VIRTUE_FAITH] = 1;
		ch->pcdata->virtues[VIRTUE_ZEAL] = 1;
	}
	ch->paradox[0] = 0;	/* Total paradox */
	ch->paradox[1] = 0;	/* Current Paradox */
	ch->paradox[2] = 0;	/* Paradox Ticker */
	ch->damcap[0] = 1000;
	ch->damcap[1] = 0;
	ch->vampaff = 0;
	ch->itemaffect = 0;
	ch->vamppass = -1;
	ch->polyaff = 0;
	ch->immune = 0;
	ch->form = 0;
	ch->init = 0;
	ch->body = 32767;
	ch->beast = 20;
	ch->truebeast = 20;
	ch->loc_hp[0] = 0;
	ch->loc_hp[1] = 0;
	ch->loc_hp[2] = 0;
	ch->loc_hp[3] = 0;
	ch->loc_hp[4] = 0;
	ch->loc_hp[5] = 0;
	ch->loc_hp[6] = 0;
	ch->wpn[0] = 0;
	ch->wpn[1] = 0;
	ch->wpn[2] = 0;
	ch->wpn[3] = 0;
	ch->wpn[4] = 0;
	ch->wpn[5] = 0;
	ch->wpn[6] = 0;
	ch->wpn[7] = 0;
	ch->wpn[8] = 0;
	ch->wpn[9] = 0;
	ch->wpn[10] = 0;
	ch->wpn[11] = 0;
	ch->wpn[12] = 0;
	ch->spl[0] = 4;
	ch->spl[1] = 4;
	ch->spl[2] = 4;
	ch->spl[3] = 4;
	ch->spl[4] = 4;
	ch->cmbt[0] = 0;
	ch->cmbt[1] = 0;
	ch->cmbt[2] = 0;
	ch->cmbt[3] = 0;
	ch->cmbt[4] = 0;
	ch->cmbt[5] = 0;
	ch->cmbt[6] = 0;
	ch->cmbt[7] = 0;
	ch->stance[0] = 0;
	ch->stance[1] = 0;
	ch->stance[2] = 0;
	ch->stance[3] = 0;
	ch->stance[4] = 0;
	ch->stance[5] = 0;
	ch->stance[6] = 0;
	ch->stance[7] = 0;
	ch->stance[8] = 0;
	ch->stance[9] = 0;
	ch->stance[10] = 0;
	ch->stance[11] = 0;
	ch->stance[12] = 0;
	ch->stance[13] = 0;
	ch->stance[14] = 0;
	ch->stance[15] = 0;
	ch->pcdata->extended_char_creation_in_progress = FALSE;
	ch->pkill = 0;
	ch->pdeath = 0;
	ch->mkill = 0;
	ch->mdeath = 0;
	ch->embraced = 0;
	ch->blood[0] = 10;
	ch->blood[1] = 10;
	ch->blood[2] = 1;
	ch->pcdata->aggdamage = 0;
	ch->pcdata->bashingdamage = 0;
	ch->pcdata->lethaldamage = 0;
	ch->pcdata->perm_str = 13;
	ch->pcdata->perm_int = 13;
	ch->pcdata->perm_wis = 13;
	ch->pcdata->perm_dex = 13;
	ch->pcdata->perm_con = 13;
	ch->pcdata->quest = 0;
	ch->pcdata->wolf = 0;
	ch->pcdata->rank = 0;
	ch->pcdata->faith = 0;
	if (IS_INQUISITOR(ch))
	{
		ch->pcdata->conviction[CONVICTION_CURRENT] = 0;
		ch->pcdata->conviction[CONVICTION_MAX] = 0;
		ch->pcdata->conviction_bank = 0;
	}
	ch->pcdata->regenerate = 0;
	ch->pcdata->demonic = 0;
	ch->pcdata->atm = 0;
	ch->pcdata->song = 0;
	ch->pcdata->necromancy = 0;
	ch->pcdata->obeah = 0;
	ch->pcdata->darkness = 0;
	ch->pcdata->temporis = 0;
	ch->pcdata->silence = 0;
	ch->pcdata->dementation = 0;
	ch->pcdata->devil = 0;
	ch->pcdata->bodymods = 0;
	ch->pcdata->vision = 3;
	ch->pcdata->reina = 0;
	ch->pcdata->bpoints = 0;
	ch->pcdata->dpoints = 0;
	ch->pcdata->gpoints = 0;
	ch->pcdata->rpoints = 0;
	ch->pcdata->wpoints = 0;
	ch->pcdata->spirit = 0;
	ch->pcdata->conjure = 0;
	ch->pcdata->celerity = 0;
	ch->pcdata->firstaid = 0;
	ch->pcdata->repair = 0;
	ch->pcdata->teach = 0;
	ch->pcdata->learn = 0;
	ch->pcdata->notes = 0;
	ch->pcdata->evileye = 0;
	ch->pcdata->followers = 0;
	ch->pcdata->gnosis[0] = 1;
	ch->pcdata->gnosis[1] = 1;
	ch->pcdata->rage[0] = 1;
	ch->pcdata->rage[1] = 1;

	ch->pcdata->absorb[0] = 0;
	ch->pcdata->absorb[1] = 0;
	ch->pcdata->absorb[2] = 0;
	ch->pcdata->absorb[3] = 0;
	ch->pcdata->absorb[4] = 0;
	ch->pcdata->absorb[5] = 0;
	ch->pcdata->qstats[0] = 0;
	ch->pcdata->qstats[1] = 0;
	ch->pcdata->qstats[2] = 0;
	ch->pcdata->qstats[3] = 0;
	ch->pcdata->qstats[4] = 0;
	ch->pcdata->exercise[0] = 0;
	ch->pcdata->exercise[1] = 0;
	ch->pcdata->exercise[2] = 0;
	ch->pcdata->exercise[3] = 0;
	ch->pcdata->language[0] = 0;
	ch->pcdata->language[1] = 0;
	ch->pcdata->stage[0] = 0;
	ch->pcdata->stage[1] = 0;
	ch->pcdata->stage[2] = 0;
	ch->pcdata->status[0] = 0;
	ch->pcdata->status[1] = 0;
	ch->pcdata->status[2] = 0;
	ch->pcdata->wolfform[0] = 1;
	ch->pcdata->wolfform[1] = 1;
	ch->pcdata->score[0] = 0;
	ch->pcdata->score[1] = 0;
	ch->pcdata->score[2] = 0;
	ch->pcdata->score[3] = 0;
	ch->pcdata->score[4] = 0;
	ch->pcdata->score[5] = 0;
	ch->pcdata->runes[0] = 0;
	ch->pcdata->runes[1] = 0;
	ch->pcdata->runes[2] = 0;
	ch->pcdata->runes[3] = 0;
	ch->pcdata->disc[0] = 0;
	ch->pcdata->disc[1] = 0;
	ch->pcdata->disc[2] = 0;
	ch->pcdata->disc[3] = 0;
	ch->pcdata->disc[4] = 0;
	ch->pcdata->disc[5] = 0;
	ch->pcdata->disc[6] = 0;
	ch->pcdata->disc[7] = 0;
	ch->pcdata->disc[8] = 0;
	ch->pcdata->disc[9] = 0;
	ch->pcdata->disc[10] = 0;
	ch->pcdata->research[0] = 0;
	ch->pcdata->research[1] = 0;

	ch->pcdata->rituals[0] = 0;
	ch->pcdata->rituals[1] = 0;
	ch->pcdata->rituals[2] = 0;
	ch->pcdata->rituals[3] = 0;
	ch->pcdata->rituals[4] = 0;
	ch->pcdata->rituals[5] = 0;
	ch->pcdata->rituals[6] = 0;
	ch->pcdata->rituals[7] = 0;
	ch->pcdata->rituals[8] = 0;
	ch->pcdata->rituals[9] = 0;
	ch->pcdata->cland[0] = 0;
	ch->pcdata->cland[1] = 0;
	ch->pcdata->cland[2] = 0;
	ch->pcdata->diableries[0] = 0;
	ch->pcdata->diableries[1] = 0;
	ch->pcdata->renown[0] = 0;
	ch->pcdata->renown[1] = 0;
	ch->pcdata->renown[2] = 0;
	ch->pcdata->renown[3] = 0;
	ch->pcdata->renown[4] = 0;
	ch->pcdata->renown[5] = 0;

	ch->pcdata->breed[0] = 0;
	ch->pcdata->breed[1] = 0;
	ch->pcdata->breed[2] = 0;
	ch->pcdata->auspice[0] = 0;
	ch->pcdata->auspice[1] = 0;
	ch->pcdata->auspice[2] = 0;
	ch->pcdata->auspice[3] = 0;
	ch->pcdata->auspice[4] = 0;
	ch->pcdata->tribes[0] = 0;
	ch->pcdata->tribes[1] = 0;
	ch->pcdata->tribes[2] = 0;
	ch->pcdata->tribes[3] = 0;
	ch->pcdata->tribes[4] = 0;
	ch->pcdata->tribes[5] = 0;
	ch->pcdata->tribes[6] = 0;
	ch->pcdata->tribes[7] = 0;
	ch->pcdata->tribes[8] = 0;
	ch->pcdata->tribes[9] = 0;
	ch->pcdata->tribes[10] = 0;
	ch->pcdata->tribes[11] = 0;
	ch->pcdata->tribes[12] = 0;
	ch->pcdata->tribes[13] = 0;
	ch->pcdata->tribes[14] = 0;
	ch->pcdata->tribes[15] = 0;
	ch->pcdata->tribes[16] = 0;
	ch->pcdata->tribes[17] = 0;
	ch->pcdata->tribes[18] = 0;
	ch->pcdata->tribes[19] = 0;
	ch->pcdata->thaum[0] = 0;
	ch->pcdata->thaum[1] = 0;
	ch->pcdata->thaum[2] = 0;
	ch->pcdata->thaum[3] = 0;
	ch->pcdata->thaum[4] = 0;
	ch->pcdata->thaum[5] = 0;
	ch->pcdata->thaum[6] = 0;
	ch->pcdata->thaum[7] = 0;
	ch->pcdata->thaum[8] = 0;
	ch->pcdata->thaum[9] = 0;
	ch->pcdata->thaum[10] = 0;
	ch->pcdata->thaum[11] = 0;
	ch->pcdata->thaum[12] = 0;
	ch->pcdata->thaum[13] = 0;
	ch->pcdata->thaum[14] = 0;
	ch->pcdata->thaum[15] = 0;
	ch->pcdata->thaum[16] = 0;
	ch->pcdata->thaum[17] = 0;
	ch->pcdata->thaum[18] = 0;
	ch->pcdata->thaum[19] = 0;

	ch->pcdata->powers[0] = 0;
	ch->pcdata->powers[1] = 0;
	ch->pcdata->powers[2] = 0;
	ch->pcdata->powers[3] = 0;
	ch->pcdata->powers[4] = 0;
	ch->pcdata->powers[5] = 0;
	ch->pcdata->powers[6] = 0;
	ch->pcdata->powers[7] = 0;
	ch->pcdata->powers[8] = 0;
	ch->pcdata->powers[9] = 0;
	ch->pcdata->powers[10] = 0;
	ch->pcdata->powers[11] = 0;
	ch->pcdata->powers[12] = 0;
	ch->pcdata->powers[13] = 0;
	ch->pcdata->powers[14] = 0;
	ch->pcdata->powers[15] = 0;
	ch->pcdata->powers[16] = 0;
	ch->pcdata->powers[17] = 0;
	ch->pcdata->powers[18] = 0;
	ch->pcdata->powers[19] = 0;
	ch->pcdata->powers[20] = 0;
	ch->pcdata->powers[21] = 0;
	ch->pcdata->powers[22] = 0;
	ch->pcdata->powers[23] = 0;
	ch->pcdata->powers[24] = 0;
	ch->pcdata->powers[25] = 0;
	ch->pcdata->powers[26] = 0;
	ch->pcdata->powers[27] = 0;
	ch->pcdata->powers[28] = 0;
	ch->pcdata->powers[29] = 0;
	ch->pcdata->powers[30] = 0;
	ch->pcdata->powers[31] = 0;
	ch->pcdata->powers[32] = 0;
	ch->pcdata->powers[33] = 0;
	ch->pcdata->powers[34] = 0;
	ch->pcdata->powers[35] = 0;
	ch->pcdata->powers[36] = 0;
	ch->pcdata->powers[37] = 0;
	ch->pcdata->powers[38] = 0;
	ch->pcdata->powers[39] = 0;
	ch->pcdata->powers_mod[0] = 0;
	ch->pcdata->powers_mod[1] = 0;
	ch->pcdata->powers_mod[2] = 0;
	ch->pcdata->powers_mod[3] = 0;
	ch->pcdata->powers_mod[4] = 0;
	ch->pcdata->powers_mod[5] = 0;
	ch->pcdata->powers_mod[6] = 0;
	ch->pcdata->powers_mod[7] = 0;
	ch->pcdata->powers_mod[8] = 0;
	ch->pcdata->powers_mod[9] = 0;
	ch->pcdata->powers_mod[10] = 0;
	ch->pcdata->powers_mod[11] = 0;
	ch->pcdata->powers_mod[12] = 0;
	ch->pcdata->powers_mod[13] = 0;
	ch->pcdata->powers_mod[14] = 0;
	ch->pcdata->powers_mod[15] = 0;
	ch->pcdata->powers_mod[16] = 0;
	ch->pcdata->powers_mod[17] = 0;
	ch->pcdata->powers_mod[18] = 0;
	ch->pcdata->powers_mod[19] = 0;
	ch->pcdata->powers_set[0] = 0;
	ch->pcdata->powers_set[1] = 0;
	ch->pcdata->powers_set[2] = 0;
	ch->pcdata->powers_set[3] = 0;
	ch->pcdata->powers_set[4] = 0;
	ch->pcdata->powers_set[5] = 0;
	ch->pcdata->powers_set[6] = 0;
	ch->pcdata->powers_set[7] = 0;
	ch->pcdata->powers_set[8] = 0;
	ch->pcdata->powers_set[9] = 0;
	ch->pcdata->powers_set[10] = 0;
	ch->pcdata->powers_set[11] = 0;
	ch->pcdata->powers_set[12] = 0;
	ch->pcdata->powers_set[13] = 0;
	ch->pcdata->powers_set[14] = 0;
	ch->pcdata->powers_set[15] = 0;
	ch->pcdata->powers_set[16] = 0;
	ch->pcdata->powers_set[17] = 0;
	ch->pcdata->powers_set[18] = 0;
	ch->pcdata->powers_set[19] = 0;
	ch->pcdata->powers_set_wear = 0;
	ch->pcdata->genes[0] = 0;
	ch->pcdata->genes[1] = 0;
	ch->pcdata->genes[2] = 0;
	ch->pcdata->genes[3] = 0;
	ch->pcdata->genes[4] = 0;
	ch->pcdata->genes[5] = 0;
	ch->pcdata->genes[6] = 0;
	ch->pcdata->genes[7] = 0;
	ch->pcdata->genes[8] = 0;
	ch->pcdata->genes[9] = 0;
	ch->pcdata->cot_min[0] = 0;
	ch->pcdata->cot_min[1] = 0;
	ch->pcdata->cot_min[2] = 0;
	ch->pcdata->cot_min[3] = 0;
	ch->pcdata->cot_min[4] = 0;
	ch->pcdata->cot_min[5] = 0;
	ch->pcdata->cot_min[6] = 0;
	ch->pcdata->cot_min[7] = 0;
	ch->pcdata->cot_min[8] = 0;
	ch->pcdata->cot_min[9] = 0;
	ch->pcdata->cot_min[10] = 0;
	ch->pcdata->cot_min[11] = 0;
	ch->pcdata->willpower[0] = 1;
	ch->pcdata->willpower[1] = 1;
	ch->pcdata->resist[0] = 0;
	ch->pcdata->resist[1] = 0;
	ch->pcdata->resist[2] = 0;
	ch->pcdata->resist[3] = 0;
	ch->pcdata->resist[4] = 0;
	ch->pcdata->power[0] = 0;
	ch->pcdata->power[1] = 0;
	ch->pcdata->fake_skill = 0;
	ch->pcdata->fake_stance = 0;
	ch->pcdata->fake_hit = 0;
	ch->pcdata->fake_dam = 0;
	ch->pcdata->fake_ac = 0;
	ch->pcdata->fake_hp = 0;
	ch->pcdata->fake_mana = 0;
	ch->pcdata->fake_move = 0;
	ch->pcdata->fake_max_hp = 0;
	ch->pcdata->fake_max_mana = 0;
	ch->pcdata->fake_max_move = 0;
	ch->pcdata->fake_stats[STAT_STR] = 0;
	ch->pcdata->fake_stats[STAT_INT] = 0;
	ch->pcdata->fake_stats[STAT_WIS] = 0;
	ch->pcdata->fake_stats[STAT_DEX] = 0;
	ch->pcdata->fake_stats[STAT_CON] = 0;
	ch->pcdata->obj_vnum = 0;
	ch->pcdata->majesty = 0;
	ch->pcdata->exhaustion = 0;
	ch->pcdata->lie = 0;
	ch->pcdata->class = 0;
	ch->pcdata->board = &boards[DEFAULT_BOARD];
	ch->home_x = 0;
	ch->home_y = 0;
	ch->room = 0;
	ch->state = 0;
	ch->action = 0;
	ch->quality = 0;
	ch->pcdata->extra = NULL;

        /* Merits/Flaws Stuff */
        ch->pcdata->merits = (char **)malloc(sizeof(char *) * 10); // 10 Spots for Merits
        ch->pcdata->flaws = (char **)malloc(sizeof(char *) * 10); // 10 Spots for Flaws

        for (i = 0; i < 10; i++)
        {
                ch->pcdata->merits[i] = NULL;
                ch->pcdata->flaws[i] = NULL;
        }
        /* End Merits/Flaws Stuff */


	ch->pcdata->in_progress = NULL;

	ch->attributes[ATTRIB_STR] = 1;
	ch->attributes[ATTRIB_DEX] = 1;
	ch->attributes[ATTRIB_STA] = 1;
	ch->attributes[ATTRIB_CHA] = 1;
	ch->attributes[ATTRIB_MAN] = 1;
	ch->attributes[ATTRIB_APP] = 1;
	ch->attributes[ATTRIB_PER] = 1;
	ch->attributes[ATTRIB_INT] = 1;
	ch->attributes[ATTRIB_WIT] = 1;

	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 10; j++)
		{
			ch->abilities[i][j] = 0;
		}
	}
	found = FALSE;
	fflush (fpReserve);
	fclose (fpReserve);
	sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (name));
	if ((fp = fopen (strsave, "r")) != NULL)
	{
		int iNest;

		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;

		found = TRUE;
		for (;;)
		{
			char letter;
			char *word;

			letter = fread_letter (fp);
			if (letter == '*')
			{
				fread_to_eol (fp);
				continue;
			}

			if (letter != '#')
			{
				bug ("Load_char_obj: # not found.", 0);
				break;
			}

			word = fread_word (fp);
			if (!str_cmp (word, "PLAYER"))
				fread_char (ch, fp);
			else if (!str_cmp (word, "PLR"))
				fread_opt_char (ch, fp);
			else if (!str_cmp (word, "OBJECT"))
				break;
			else if (!str_cmp (word, "OBJ"))
				break;
			else if (!str_cmp (word, "O"))
				break;
			else if (!str_cmp (word, "END"))
				break;
			else
			{
				bug ("Load_char_obj: bad section.", 0);
				break;
			}
		}
		fflush (fp);
		fclose (fp);
	}

	fpReserve = fopen (NULL_FILE, "r");
	return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_char (CHAR_DATA * ch, FILE * fp)
{
	char buf[MAX_STRING_LENGTH];
	char *word;
	bool fMatch;
	/* Merits/Flaws Stuff */
	char * temp_string;
	int merit_ctr = 0;
	int flaw_ctr = 0;

	temp_string = str_dup("");
	/* End Merits/Flaws Stuff */

	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;

		switch (UPPER (word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;

		case 'A':
			if (!str_cmp (word, "Absorb"))
			{
				ch->pcdata->absorb[0] = fread_number (fp);
				ch->pcdata->absorb[1] = fread_number (fp);
				ch->pcdata->absorb[2] = fread_number (fp);
				ch->pcdata->absorb[3] = fread_number (fp);
				ch->pcdata->absorb[4] = fread_number (fp);
				ch->pcdata->absorb[5] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Act", ch->act, fread_number (fp));
			KEY ("AffectedBy", ch->affected_by, fread_number (fp));
			KEY ("Alignment", ch->alignment, fread_number (fp));
			KEY ("Armor", ch->armor, fread_number (fp));

			if (!str_cmp (word, "Affect") || !str_cmp (word, "AffectData"))
			{
				AFFECT_DATA *paf;

				if (affect_free == NULL)
				{
					paf = alloc_perm (sizeof (*paf));
				}
				else
				{
					paf = affect_free;
					affect_free = affect_free->next;
				}

				if (!str_cmp (word, "Affect"))
				{
					/* Obsolete 2.0 form. */
					paf->type = fread_number (fp);
				}
				else
				{
					int sn;

					sn = skill_lookup (fread_word (fp));
					if (sn < 0)
						bug ("Fread_char: unknown skill.", 0);
					else
						paf->type = sn;
				}

				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = fread_number (fp);
				paf->next = ch->affected;
				ch->affected = paf;
				fMatch = TRUE;
				break;
			}

			KEY ("Agg", ch->agg, fread_number (fp));
			KEY ("Aggdamage", ch->pcdata->aggdamage, fread_number (fp));
			KEY ("AutoVote", ch->pcdata->auto_vote_counter, fread_number(fp));
			if (!str_cmp (word, "Auspice"))
			{
				ch->pcdata->auspice[0] = fread_number (fp);
				ch->pcdata->auspice[1] = fread_number (fp);
				ch->pcdata->auspice[2] = fread_number (fp);
				ch->pcdata->auspice[3] = fread_number (fp);
				ch->pcdata->auspice[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Atm", ch->pcdata->atm, fread_number (fp));

			if (!str_cmp (word, "AttrMod"))
			{
				ch->pcdata->mod_str = fread_number (fp);
				ch->pcdata->mod_int = fread_number (fp);
				ch->pcdata->mod_wis = fread_number (fp);
				ch->pcdata->mod_dex = fread_number (fp);
				ch->pcdata->mod_con = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "AttrPerm"))
			{
				ch->pcdata->perm_str = fread_number (fp);
				ch->pcdata->perm_int = fread_number (fp);
				ch->pcdata->perm_wis = fread_number (fp);
				ch->pcdata->perm_dex = fread_number (fp);
				ch->pcdata->perm_con = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'B':
			KEY ("Bamfin", ch->pcdata->bamfin, fread_string (fp));
			KEY ("Bamfout", ch->pcdata->bamfout, fread_string (fp));
			KEY ("Bashingdamage", ch->pcdata->bashingdamage, fread_number (fp));
			KEY ("Beast", ch->beast, fread_number (fp));
			KEY ("BloodCounter", ch->pcdata->blood_burn_counter, fread_number(fp));
			if (!str_cmp (word, "Blood"))
			{
				ch->blood[0] = fread_number (fp);
				ch->blood[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Body", ch->body, fread_number (fp));
			KEY ("Bodymods", ch->pcdata->bodymods, fread_number (fp));
			KEY ("Bpoints", ch->pcdata->bpoints, fread_number (fp));
			if (!str_cmp (word, "Breed"))
			{
				ch->pcdata->breed[0] = fread_number (fp);
				ch->pcdata->breed[1] = fread_number (fp);
				ch->pcdata->breed[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			/* Read in board status */
			if (!str_cmp (word, "Boards"))
			{
				int i, num = fread_number (fp);	/* number of boards saved */
				char *boardname;

				for (; num; num--)	/* for each of the board saved */
				{
					boardname = fread_word (fp);
					i = board_lookup (boardname);	/* find board number */

					if (i == BOARD_NOTFOUND)	/* Does board still exist ? */
					{
						sprintf (buf, "fread_char: %s had unknown board name: %s. Skipped.", ch->name, boardname);
						log_string (buf, CHANNEL_LOG_NORMAL);
						fread_number (fp);	/* read last_note and skip info */
					}
					else	/* Save it */
						ch->pcdata->last_note[i] = fread_number (fp);
				}	/* for */

				fMatch = TRUE;
			}	/* Boards */

			break;

		case 'C':
			KEY ("CrinosDesc", ch->pcdata->crinosdesc, fread_string (fp));
			KEY ("Celerity", ch->pcdata->celerity, fread_number (fp));
			KEY ("Clan", ch->clan, fread_string (fp));
			if (!str_cmp (word, "Clandisc"))
			{
				ch->pcdata->cland[0] = fread_number (fp);
				ch->pcdata->cland[1] = fread_number (fp);
				ch->pcdata->cland[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Class", ch->class, fread_number (fp));

			if (!str_cmp (word, "Combat"))
			{
				ch->cmbt[0] = fread_number (fp);
				ch->cmbt[1] = fread_number (fp);
				ch->cmbt[2] = fread_number (fp);
				ch->cmbt[3] = fread_number (fp);
				ch->cmbt[4] = fread_number (fp);
				ch->cmbt[5] = fread_number (fp);
				ch->cmbt[6] = fread_number (fp);
				ch->cmbt[7] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Conception", ch->pcdata->conception, fread_string (fp));
			KEY ("Conding", ch->pcdata->conding, fread_string (fp));

			if (!str_cmp (word, "Condition"))
			{
				ch->pcdata->condition[0] = fread_number (fp);
				ch->pcdata->condition[1] = fread_number (fp);
				ch->pcdata->condition[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Conjure", ch->pcdata->conjure, fread_number (fp));
			if (IS_INQUISITOR(ch))
			{
				if (!str_cmp (word, "Conviction"))
				{
					ch->pcdata->conviction[CONVICTION_CURRENT] = fread_number (fp);
					ch->pcdata->conviction[CONVICTION_MAX] = fread_number (fp);
					fMatch = TRUE;
					break;
				}
				KEY ("ConvictionBank", ch->pcdata->conviction_bank, fread_number (fp));
			}
			if (!str_cmp (word, "CotMin"))
			{
				ch->pcdata->cot_min[0] = fread_number (fp);
				ch->pcdata->cot_min[1] = fread_number (fp);
				ch->pcdata->cot_min[2] = fread_number (fp);
				ch->pcdata->cot_min[3] = fread_number (fp);
				ch->pcdata->cot_min[4] = fread_number (fp);
				ch->pcdata->cot_min[5] = fread_number (fp);
				ch->pcdata->cot_min[6] = fread_number (fp);
				ch->pcdata->cot_min[7] = fread_number (fp);
				ch->pcdata->cot_min[8] = fread_number (fp);
				ch->pcdata->cot_min[9] = fread_number (fp);
				ch->pcdata->cot_min[10] = fread_number (fp);
				ch->pcdata->cot_min[11] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Coterie", ch->pcdata->coterie, fread_string (fp));
			KEY ("CotTime", ch->pcdata->cot_time, fread_string (fp));
			KEY ("CotClans", ch->pcdata->cot_clans, fread_string (fp));
			KEY ("CotSides", ch->pcdata->cot_sides, fread_string (fp));
			KEY ("Cparents", ch->pcdata->cparents, fread_string (fp));
			KEY ("Cprompt", ch->cprompt, fread_string (fp));
			KEY ("Createtime", ch->createtime, fread_string (fp));
			break;

		case 'D':
			KEY ("Devil", ch->pcdata->devil, fread_number (fp));
			KEY ("Damroll", ch->damroll, fread_number (fp));
			KEY ("Darkness", ch->pcdata->darkness, fread_number (fp));
			KEY ("Deaf", ch->deaf, fread_number (fp));
			KEY ("Demonic", ch->pcdata->demonic, fread_number (fp));
			KEY ("DenyDate", ch->pcdata->denydate, fread_number (fp));
			KEY ("Description", ch->description, fread_string (fp));
			KEY ("Demeanor", ch->demeanor, fread_string (fp));
			KEY ("Dementation", ch->pcdata->dementation, fread_number (fp));
			if (!str_cmp (word, "Diableries"))
			{
				ch->pcdata->diableries[0] = fread_number (fp);
				ch->pcdata->diableries[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Disc"))
			{
				ch->pcdata->disc[0] = fread_number (fp);
				ch->pcdata->disc[1] = fread_number (fp);
				ch->pcdata->disc[2] = fread_number (fp);
				ch->pcdata->disc[3] = fread_number (fp);
				ch->pcdata->disc[4] = fread_number (fp);
				ch->pcdata->disc[5] = fread_number (fp);
				ch->pcdata->disc[6] = fread_number (fp);
				ch->pcdata->disc[7] = fread_number (fp);
				ch->pcdata->disc[8] = fread_number (fp);
				ch->pcdata->disc[9] = fread_number (fp);
				ch->pcdata->disc[10] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Dpoints", ch->pcdata->dpoints, fread_number (fp));
			break;

		case 'E':
			KEY ("Email", ch->pcdata->email, fread_string (fp));
			if (!str_cmp (word, "End"))
				return;
			KEY ("Evileye", ch->pcdata->evileye, fread_number (fp));
			if (!str_cmp (word, "Exercise"))
			{
				ch->pcdata->exercise[0] = fread_number (fp);
				ch->pcdata->exercise[1] = fread_number (fp);
				ch->pcdata->exercise[2] = fread_number (fp);
				ch->pcdata->exercise[3] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Exhaustion", ch->pcdata->exhaustion, fread_number (fp));
			KEY ("Exp", ch->exp, fread_number (fp));
			KEY ("Extra", ch->extra, fread_number (fp));
			break;

		case 'F':
			KEY ("Faith", ch->pcdata->faith, fread_number (fp));
			if (!str_cmp (word, "FakeCon"))
			{
				ch->pcdata->fake_skill = fread_number (fp);
				ch->pcdata->fake_stance = fread_number (fp);
				ch->pcdata->fake_hit = fread_number (fp);
				ch->pcdata->fake_dam = fread_number (fp);
				ch->pcdata->fake_ac = fread_number (fp);
				ch->pcdata->fake_hp = fread_number (fp);
				ch->pcdata->fake_mana = fread_number (fp);
				ch->pcdata->fake_move = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "FakeTwo"))
			{
				ch->pcdata->fake_max_hp = fread_number (fp);
				ch->pcdata->fake_max_mana = fread_number (fp);
				ch->pcdata->fake_max_move = fread_number (fp);
				ch->pcdata->fake_stats[STAT_STR] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_INT] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_WIS] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_DEX] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_CON] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Firstaid", ch->pcdata->firstaid, fread_number (fp));
			/* Merits/Flaws Stuff */
			if (!str_cmp (word, "Flaw"))
			{
                                temp_string = fread_string (fp);
                                ch->pcdata->flaws[flaw_ctr] = (char *)malloc(sizeof(char) * strlen(temp_string));
                                ch->pcdata->flaws[flaw_ctr++] = strdup(temp_string);
                                fMatch = TRUE;
                                break;
			}
			/* End Merits/Flaws Stuff */
			KEY ("Form", ch->form, fread_number (fp));
			break;

		case 'G':
			KEY ("Giftlist", ch->pcdata->giftlist, fread_string (fp));
			KEY ("GlabroDesc", ch->pcdata->glabrodesc, fread_string (fp));
			if (!str_cmp (word, "Genes"))
			{
				ch->pcdata->genes[0] = fread_number (fp);
				ch->pcdata->genes[1] = fread_number (fp);
				ch->pcdata->genes[2] = fread_number (fp);
				ch->pcdata->genes[3] = fread_number (fp);
				ch->pcdata->genes[4] = fread_number (fp);
				ch->pcdata->genes[5] = fread_number (fp);
				ch->pcdata->genes[6] = fread_number (fp);
				ch->pcdata->genes[7] = fread_number (fp);
				ch->pcdata->genes[8] = fread_number (fp);
				ch->pcdata->genes[9] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "Gnosis"))
			{
				ch->pcdata->gnosis[0] = fread_number (fp);
				ch->pcdata->gnosis[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Gold", ch->gold, fread_number (fp));
			KEY ("Gpoints", ch->pcdata->gpoints, fread_number (fp));
			break;

		case 'H':
			KEY ("HispoDesc", ch->pcdata->hispodesc, fread_string (fp));
			if (!str_cmp (word, "HomeXY"))
			{
				ch->home_x = fread_number (fp);
				ch->home_y = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Hitroll", ch->hitroll, fread_number (fp));
			KEY ("Home", ch->home, fread_number (fp));

			if (!str_cmp (word, "HpManaMove"))
			{
				ch->hit = fread_number (fp);
				ch->max_hit = fread_number (fp);
				ch->mana = fread_number (fp);
				ch->max_mana = fread_number (fp);
				ch->move = fread_number (fp);
				ch->max_move = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'I':
			KEY ("Immune", ch->immune, fread_number (fp));
			KEY ("Impulse", ch->impulse, fread_string (fp));
			KEY ("Itemaffect", ch->itemaffect, fread_number (fp));
			break;

		case 'L':
			KEY ("LupusDesc", ch->pcdata->lupusdesc, fread_string (fp));
			if (!str_cmp (word, "Language"))
			{
				ch->pcdata->language[0] = fread_number (fp);
				ch->pcdata->language[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Lasthost", ch->lasthost, fread_string (fp));
			KEY ("Lasttime", ch->lasttime, fread_string (fp));
	
			if( !str_cmp( word, "Lastvotetime" ) )
			{
			  ch->pcdata->last_vote_time.year = fread_number( fp );
			  ch->pcdata->last_vote_time.month = fread_number( fp );
  			  ch->pcdata->last_vote_time.day = fread_number( fp );
    			  fMatch = TRUE;
    			  break;
			}
	
			KEY ("Leader", ch->pcdata->leader, fread_string (fp));
			KEY ("Learn", ch->pcdata->learn, fread_number (fp));
			KEY ("Lethaldamage", ch->pcdata->learn, fread_number (fp));
			KEY ("Level", ch->level, fread_number (fp));
			if (!str_cmp (word, "Locationhp"))
			{
				ch->loc_hp[0] = fread_number (fp);
				ch->loc_hp[1] = fread_number (fp);
				ch->loc_hp[2] = fread_number (fp);
				ch->loc_hp[3] = fread_number (fp);
				ch->loc_hp[4] = fread_number (fp);
				ch->loc_hp[5] = fread_number (fp);
				ch->loc_hp[6] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "LocationXY"))
			{
				ch->x = fread_number (fp);
				ch->y = fread_number (fp);
				ch->room = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("LongDescr", ch->long_descr, fread_string (fp));
			KEY ("Lord", ch->lord, fread_string (fp));
			KEY ("Love", ch->pcdata->love, fread_string (fp));
			break;

		case 'M':
			KEY ("Majesty", ch->pcdata->majesty, fread_number (fp));
			KEY ("Marriage", ch->pcdata->marriage, fread_string (fp));
			/* Merits/Flaws Stuff */
			if (!str_cmp (word, "Merit"))
			{
				temp_string = fread_string (fp);	
				ch->pcdata->merits[merit_ctr] = (char *)malloc(sizeof(char) * strlen(temp_string));
				ch->pcdata->merits[merit_ctr++] = strdup(temp_string);
				fMatch = TRUE;
				break;
			}
			/* End Merits/Flaws Stuff */
			KEY ("More2", ch->more2, fread_number (fp));
			KEY ("More3", ch->more3, fread_number (fp));
			KEY ("More", ch->more, fread_number (fp));
			KEY ("Morph", ch->morph, fread_string (fp));
			break;

		case 'N':
			if (!str_cmp (word, "Name"))
			{
				/*
				 * Name already set externally.
				 */
				fread_to_eol (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Nature", ch->nature, fread_string (fp));
			KEY ("Necromancy", ch->pcdata->necromancy, fread_number (fp));
			KEY ("Notes", ch->pcdata->notes, fread_number (fp));
			break;

		case 'O':
			KEY ("Obeah", ch->pcdata->obeah, fread_number (fp));
			KEY ("Objvnum", ch->pcdata->obj_vnum, fread_number (fp));
			KEY ("Order", ch->order, fread_string (fp));
			break;

		case 'P':
			if (!str_cmp (word, "Paradox"))
			{
				ch->paradox[0] = fread_number (fp);
				ch->paradox[1] = fread_number (fp);
				ch->paradox[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Parents", ch->pcdata->parents, fread_string (fp));
			KEY ("Password", ch->pcdata->pwd, fread_string (fp));

			KEY ("Played", ch->played, fread_number (fp));
			KEY ("PlayedIC", ch->playedIC, fread_number(fp));
			KEY ("Polyaff", ch->polyaff, fread_number (fp));

			if (!str_cmp (word, "Power"))
			{
				ch->pcdata->power[0] = fread_number (fp);
				ch->pcdata->power[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Powers"))
			{
				ch->pcdata->powers[0] = fread_number (fp);
				ch->pcdata->powers[1] = fread_number (fp);
				ch->pcdata->powers[2] = fread_number (fp);
				ch->pcdata->powers[3] = fread_number (fp);
				ch->pcdata->powers[4] = fread_number (fp);
				ch->pcdata->powers[5] = fread_number (fp);
				ch->pcdata->powers[6] = fread_number (fp);
				ch->pcdata->powers[7] = fread_number (fp);
				ch->pcdata->powers[8] = fread_number (fp);
				ch->pcdata->powers[9] = fread_number (fp);
				ch->pcdata->powers[10] = fread_number (fp);
				ch->pcdata->powers[11] = fread_number (fp);
				ch->pcdata->powers[12] = fread_number (fp);
				ch->pcdata->powers[13] = fread_number (fp);
				ch->pcdata->powers[14] = fread_number (fp);
				ch->pcdata->powers[15] = fread_number (fp);
				ch->pcdata->powers[16] = fread_number (fp);
				ch->pcdata->powers[17] = fread_number (fp);
				ch->pcdata->powers[18] = fread_number (fp);
				ch->pcdata->powers[19] = fread_number (fp);
				ch->pcdata->powers[20] = fread_number (fp);
				ch->pcdata->powers[21] = fread_number (fp);
				ch->pcdata->powers[22] = fread_number (fp);
				ch->pcdata->powers[23] = fread_number (fp);
				ch->pcdata->powers[24] = fread_number (fp);
				ch->pcdata->powers[25] = fread_number (fp);
				ch->pcdata->powers[26] = fread_number (fp);
				ch->pcdata->powers[27] = fread_number (fp);
				ch->pcdata->powers[28] = fread_number (fp);
				ch->pcdata->powers[29] = fread_number (fp);
				ch->pcdata->powers[30] = fread_number (fp);
				ch->pcdata->powers[31] = fread_number (fp);
				ch->pcdata->powers[32] = fread_number (fp);
				ch->pcdata->powers[33] = fread_number (fp);
				ch->pcdata->powers[34] = fread_number (fp);
				ch->pcdata->powers[35] = fread_number (fp);
				ch->pcdata->powers[36] = fread_number (fp);
				ch->pcdata->powers[37] = fread_number (fp);
				ch->pcdata->powers[38] = fread_number (fp);
				ch->pcdata->powers[39] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "PowersMod"))
			{
				ch->pcdata->powers_mod[0] = fread_number (fp);
				ch->pcdata->powers_mod[1] = fread_number (fp);
				ch->pcdata->powers_mod[2] = fread_number (fp);
				ch->pcdata->powers_mod[3] = fread_number (fp);
				ch->pcdata->powers_mod[4] = fread_number (fp);
				ch->pcdata->powers_mod[5] = fread_number (fp);
				ch->pcdata->powers_mod[6] = fread_number (fp);
				ch->pcdata->powers_mod[7] = fread_number (fp);
				ch->pcdata->powers_mod[8] = fread_number (fp);
				ch->pcdata->powers_mod[9] = fread_number (fp);
				ch->pcdata->powers_mod[10] = fread_number (fp);
				ch->pcdata->powers_mod[11] = fread_number (fp);
				ch->pcdata->powers_mod[12] = fread_number (fp);
				ch->pcdata->powers_mod[13] = fread_number (fp);
				ch->pcdata->powers_mod[14] = fread_number (fp);
				ch->pcdata->powers_mod[15] = fread_number (fp);
				ch->pcdata->powers_mod[16] = fread_number (fp);
				ch->pcdata->powers_mod[17] = fread_number (fp);
				ch->pcdata->powers_mod[18] = fread_number (fp);
				ch->pcdata->powers_mod[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "PowersSet"))
			{
				ch->pcdata->powers_set[0] = fread_number (fp);
				ch->pcdata->powers_set[1] = fread_number (fp);
				ch->pcdata->powers_set[2] = fread_number (fp);
				ch->pcdata->powers_set[3] = fread_number (fp);
				ch->pcdata->powers_set[4] = fread_number (fp);
				ch->pcdata->powers_set[5] = fread_number (fp);
				ch->pcdata->powers_set[6] = fread_number (fp);
				ch->pcdata->powers_set[7] = fread_number (fp);
				ch->pcdata->powers_set[8] = fread_number (fp);
				ch->pcdata->powers_set[9] = fread_number (fp);
				ch->pcdata->powers_set[10] = fread_number (fp);
				ch->pcdata->powers_set[11] = fread_number (fp);
				ch->pcdata->powers_set[12] = fread_number (fp);
				ch->pcdata->powers_set[13] = fread_number (fp);
				ch->pcdata->powers_set[14] = fread_number (fp);
				ch->pcdata->powers_set[15] = fread_number (fp);
				ch->pcdata->powers_set[16] = fread_number (fp);
				ch->pcdata->powers_set[17] = fread_number (fp);
				ch->pcdata->powers_set[18] = fread_number (fp);
				ch->pcdata->powers_set[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("PowersWear", ch->pcdata->powers_set_wear, fread_number (fp));

			KEY ("Poweraction", ch->poweraction, fread_string (fp));
			KEY ("Powertype", ch->powertype, fread_string (fp));
			KEY ("Position", ch->position, fread_number (fp));
			if (!str_cmp (word, "PkPdMkMd"))
			{
				ch->pkill = fread_number (fp);
				ch->pdeath = fread_number (fp);
				ch->mkill = fread_number (fp);
				ch->mdeath = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Pretitle", ch->pcdata->pretitle, fread_string(fp));
			KEY ("Prompt", ch->prompt, fread_string (fp));
			break;

		case 'Q':
			if (!str_cmp (word, "Qstats"))
			{
				ch->pcdata->qstats[0] = fread_number (fp);
				ch->pcdata->qstats[1] = fread_number (fp);
				ch->pcdata->qstats[2] = fread_number (fp);
				ch->pcdata->qstats[3] = fread_number (fp);
				ch->pcdata->qstats[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Quest", ch->pcdata->quest, fread_number (fp));
			break;

		case 'R':
			if (!str_cmp (word, "Renown"))
			{
				ch->pcdata->renown[0] = fread_number (fp);
				ch->pcdata->renown[1] = fread_number (fp);
				ch->pcdata->renown[2] = fread_number (fp);
				ch->pcdata->renown[3] = fread_number (fp);
				ch->pcdata->renown[4] = fread_number (fp);
				ch->pcdata->renown[5] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Race", ch->race, fread_number (fp));
			KEY ("Rank", ch->pcdata->rank, fread_number (fp));
			if (!str_cmp (word, "Rage"))
			{
				ch->pcdata->rage[0] = fread_number (fp);
				ch->pcdata->rage[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Regenerate", ch->pcdata->regenerate, fread_number (fp));
			KEY ("Reina", ch->pcdata->reina, fread_number (fp));
			KEY ("Repair", ch->pcdata->repair, fread_number (fp));
			if (!str_cmp (word, "Research"))
			{
				ch->pcdata->research[0] = fread_number (fp);
				ch->pcdata->research[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Resist"))
			{
				ch->pcdata->resist[0] = fread_number (fp);
				ch->pcdata->resist[1] = fread_number (fp);
				ch->pcdata->resist[2] = fread_number (fp);
				ch->pcdata->resist[3] = fread_number (fp);
				ch->pcdata->resist[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Rituals"))
			{
				ch->pcdata->rituals[0] = fread_number (fp);
				ch->pcdata->rituals[1] = fread_number (fp);
				ch->pcdata->rituals[2] = fread_number (fp);
				ch->pcdata->rituals[3] = fread_number (fp);
				ch->pcdata->rituals[4] = fread_number (fp);
				ch->pcdata->rituals[5] = fread_number (fp);
				ch->pcdata->rituals[6] = fread_number (fp);
				ch->pcdata->rituals[7] = fread_number (fp);
				ch->pcdata->rituals[8] = fread_number (fp);
				ch->pcdata->rituals[9] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Room"))
			{
				ch->in_room = get_room_index (fread_number (fp));
				if (ch->in_room == NULL)
					ch->in_room = get_room_index (ROOM_VNUM_LIMBO);
				fMatch = TRUE;
				break;
			}
			
			KEY ("RPflagTime", ch->rpflag_time, (time_t) fread_number (fp));
			KEY ("Rpoints", ch->pcdata->rpoints, fread_number (fp));

			if (!str_cmp (word, "Runes"))
			{
				ch->pcdata->runes[0] = fread_number (fp);
				ch->pcdata->runes[1] = fread_number (fp);
				ch->pcdata->runes[2] = fread_number (fp);
				ch->pcdata->runes[3] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'S':
			KEY ("SavingThrow", ch->saving_throw, fread_number (fp));
			KEY ("ShowTempDesc", ch->pcdata->showtempdesc, fread_number (fp));
			if (!str_cmp (word, "Score"))
			{
				ch->pcdata->score[0] = fread_number (fp);
				ch->pcdata->score[1] = fread_number (fp);
				ch->pcdata->score[2] = fread_number (fp);
				ch->pcdata->score[3] = fread_number (fp);
				ch->pcdata->score[4] = fread_number (fp);
				ch->pcdata->score[5] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Security", ch->pcdata->security, fread_number (fp));	/* OLC */
			KEY ("Sex", ch->sex, fread_number (fp));
			KEY ("ShortDescC", ch->pcdata->crinosdescshort, fread_string (fp));
			KEY ("ShortDescG", ch->pcdata->glabrodescshort, fread_string (fp));
			KEY ("ShortDescH", ch->pcdata->hispodescshort, fread_string (fp));
			KEY ("ShortDescL", ch->pcdata->lupusdescshort, fread_string (fp));
			KEY ("ShortDescr", ch->short_descr, fread_string (fp));
			KEY ("Side", ch->side, fread_string (fp));
			KEY ("Silence", ch->pcdata->silence, fread_number (fp));

			if (!str_cmp (word, "Skill"))
			{
				int sn;
				int value;

				value = fread_number (fp);
				sn = skill_lookup (fread_word (fp));

				if (sn >= 0)
					ch->pcdata->learned[sn] = value;
				fMatch = TRUE;
			}

			KEY ("Specpower", ch->specpower, fread_number (fp));
			KEY ("Spectype", ch->spectype, fread_number (fp));

			if (!str_cmp (word, "Spells"))
			{
				ch->spl[0] = fread_number (fp);
				ch->spl[1] = fread_number (fp);
				ch->spl[2] = fread_number (fp);
				ch->spl[3] = fread_number (fp);
				ch->spl[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Spirit", ch->pcdata->spirit, fread_number (fp));

			if (!str_cmp (word, "Stage"))
			{
				ch->pcdata->stage[0] = fread_number (fp);
				ch->pcdata->stage[1] = fread_number (fp);
				ch->pcdata->stage[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Stance"))
			{
				ch->stance[0] = fread_number (fp);
				ch->stance[1] = fread_number (fp);
				ch->stance[2] = fread_number (fp);
				ch->stance[3] = fread_number (fp);
				ch->stance[4] = fread_number (fp);
				ch->stance[5] = fread_number (fp);
				ch->stance[6] = fread_number (fp);
				ch->stance[7] = fread_number (fp);
				ch->stance[8] = fread_number (fp);
				ch->stance[9] = fread_number (fp);
				ch->stance[10] = fread_number (fp);
				ch->stance[11] = fread_number (fp);
				ch->stance[12] = fread_number (fp);
				ch->stance[13] = fread_number (fp);
				ch->stance[14] = fread_number (fp);
				ch->stance[15] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Stancename", ch->pcdata->stancename, fread_string (fp));
			KEY ("Stancelist", ch->pcdata->stancelist, fread_string (fp));
			KEY ("Stancework", ch->pcdata->stancework, fread_string (fp));
			KEY ("StatusVoteCounter", ch->pcdata->statusvotecounter, fread_number (fp));
			if (!str_cmp (word, "Stancemove"))
			{
				ch->pcdata->stancemove[0] = fread_number (fp);
				ch->pcdata->stancemove[1] = fread_number (fp);
				ch->pcdata->stancemove[2] = fread_number (fp);
				ch->pcdata->stancemove[3] = fread_number (fp);
				ch->pcdata->stancemove[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Status"))
			{
				ch->pcdata->status[0] = fread_number (fp);
				ch->pcdata->status[1] = fread_number (fp);
				ch->pcdata->status[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			break;

		case 'T':
			KEY ("Teach", ch->pcdata->teach, fread_number (fp));
			KEY ("TempClass", ch->pcdata->class, fread_number (fp));
			KEY ("Temporis", ch->pcdata->temporis, fread_number (fp));
			KEY ("TempDesc", ch->pcdata->tempdesc, fread_string (fp));
			if (!str_cmp (word, "Tribes"))
			{
				ch->pcdata->tribes[0] = fread_number (fp);
				ch->pcdata->tribes[1] = fread_number (fp);
				ch->pcdata->tribes[2] = fread_number (fp);
				ch->pcdata->tribes[3] = fread_number (fp);
				ch->pcdata->tribes[4] = fread_number (fp);
				ch->pcdata->tribes[5] = fread_number (fp);
				ch->pcdata->tribes[6] = fread_number (fp);
				ch->pcdata->tribes[7] = fread_number (fp);
				ch->pcdata->tribes[8] = fread_number (fp);
				ch->pcdata->tribes[9] = fread_number (fp);
				ch->pcdata->tribes[10] = fread_number (fp);
				ch->pcdata->tribes[11] = fread_number (fp);
				ch->pcdata->tribes[12] = fread_number (fp);
				ch->pcdata->tribes[13] = fread_number (fp);
				ch->pcdata->tribes[14] = fread_number (fp);
				ch->pcdata->tribes[15] = fread_number (fp);
				ch->pcdata->tribes[16] = fread_number (fp);
				ch->pcdata->tribes[17] = fread_number (fp);
				ch->pcdata->tribes[18] = fread_number (fp);
				ch->pcdata->tribes[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "Thaum"))
			{
				ch->pcdata->thaum[0] = fread_number (fp);
				ch->pcdata->thaum[1] = fread_number (fp);
				ch->pcdata->thaum[2] = fread_number (fp);
				ch->pcdata->thaum[3] = fread_number (fp);
				ch->pcdata->thaum[4] = fread_number (fp);
				ch->pcdata->thaum[5] = fread_number (fp);
				ch->pcdata->thaum[6] = fread_number (fp);
				ch->pcdata->thaum[7] = fread_number (fp);
				ch->pcdata->thaum[8] = fread_number (fp);
				ch->pcdata->thaum[9] = fread_number (fp);
				ch->pcdata->thaum[10] = fread_number (fp);
				ch->pcdata->thaum[11] = fread_number (fp);
				ch->pcdata->thaum[12] = fread_number (fp);
				ch->pcdata->thaum[13] = fread_number (fp);
				ch->pcdata->thaum[14] = fread_number (fp);
				ch->pcdata->thaum[15] = fread_number (fp);
				ch->pcdata->thaum[16] = fread_number (fp);
				ch->pcdata->thaum[17] = fread_number (fp);
				ch->pcdata->thaum[18] = fread_number (fp);
				ch->pcdata->thaum[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("Truebeast", ch->truebeast, fread_number (fp));
			KEY ("Trust", ch->trust, fread_number (fp));

			if (!str_cmp (word, "Title"))
			{
				ch->pcdata->title = fread_string (fp);
				if (isalpha (ch->pcdata->title[0]) || isdigit (ch->pcdata->title[0]))
				{
					sprintf (buf, "%s", ch->pcdata->title);
					free_string (ch->pcdata->title);
					ch->pcdata->title = str_dup (buf);
				}
				fMatch = TRUE;
				break;
			}

			break;

		case 'V':
			KEY ("Vision", ch->pcdata->vision, fread_number (fp));
			KEY ("Vampaff", ch->vampaff, fread_number (fp));
			KEY ("Vampgen", ch->vampgen, fread_number (fp));
			KEY ("Vamppass", ch->vamppass, fread_number (fp));
			if (!str_cmp (word, "Virtues"))
			{
				if (IS_INQUISITOR(ch))
				{
					ch->pcdata->virtues[VIRTUE_CONSCIENCE] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_SELFCONTROL] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_COURAGE] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_WISDOM] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_FAITH] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_ZEAL] = fread_number(fp);
				}
				else
				{
					ch->pcdata->virtues[VIRTUE_CONSCIENCE] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_SELFCONTROL] = fread_number(fp);
					ch->pcdata->virtues[VIRTUE_COURAGE] = fread_number(fp);
				}
			}
			if (!str_cmp (word, "Vnum"))
			{
				ch->pIndexData = get_mob_index (fread_number (fp));
				fMatch = TRUE;
				break;
			}
			KEY ("Votes", ch->pcdata->votes, fread_number(fp));
			break;

		case 'W':
			if (!str_cmp (word, "Weapons"))
			{
				ch->wpn[0] = fread_number (fp);
				ch->wpn[1] = fread_number (fp);
				ch->wpn[2] = fread_number (fp);
				ch->wpn[3] = fread_number (fp);
				ch->wpn[4] = fread_number (fp);
				ch->wpn[5] = fread_number (fp);
				ch->wpn[6] = fread_number (fp);
				ch->wpn[7] = fread_number (fp);
				ch->wpn[8] = fread_number (fp);
				ch->wpn[9] = fread_number (fp);
				ch->wpn[10] = fread_number (fp);
				ch->wpn[11] = fread_number (fp);
				ch->wpn[12] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "Willpower"))
			{
				ch->pcdata->willpower[0] = fread_number (fp);
				ch->pcdata->willpower[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Wimpy", ch->wimpy, fread_number (fp));
			KEY ("Wolf", ch->pcdata->wolf, fread_number (fp));
			if (!str_cmp (word, "Wolfform"))
			{
				ch->pcdata->wolfform[0] = fread_number (fp);
				ch->pcdata->wolfform[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("Wpoints", ch->pcdata->wpoints, fread_number (fp));
			KEY ("WpRegain", ch->pcdata->wp_regain_counter, fread_number (fp));
			break;
		}

		if (!fMatch)
		{
			bug ("Fread_char: no match.", 0);
			fread_to_eol (fp);
		}
	}
}



void fread_opt_char (CHAR_DATA * ch, FILE * fp)
{
	char buf[MAX_STRING_LENGTH];
	char *word;
	bool fMatch;
	int i;
	/* Merits/Flaws Stuff */
	char * temp_string;
	int merit_ctr = 0;
	int flaw_ctr = 0;

	temp_string = strdup("");
	/* End Merits/Flaws Stuff */

	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;

		switch (UPPER (word[0]))
		{
		default:
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;

		case 'A':
			if (!str_cmp (word, "ABS"))
			{
				ch->pcdata->absorb[0] = fread_number (fp);
				ch->pcdata->absorb[1] = fread_number (fp);
				ch->pcdata->absorb[2] = fread_number (fp);
				ch->pcdata->absorb[3] = fread_number (fp);
				ch->pcdata->absorb[4] = fread_number (fp);
				ch->pcdata->absorb[5] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("ACT", ch->act, fread_number (fp));
			KEY ("ADM", ch->pcdata->aggdamage, fread_number (fp));
			KEY ("AFB", ch->affected_by, fread_number (fp));
			KEY ("ALN", ch->alignment, fread_number (fp));
			KEY ("ARM", ch->armor, fread_number (fp));
			/* Aura Code */
			KEY ("AUR", ch->pcdata->aura, fread_string (fp)); 
			KEY ("AUT", ch->autostance, fread_number (fp));
			if (!str_cmp (word, "AFD"))
			{
				AFFECT_DATA *paf;
				int sn;

				if (affect_free == NULL)
				{
					paf = alloc_perm (sizeof (*paf));
				}
				else
				{
					paf = affect_free;
					affect_free = affect_free->next;
				}

				sn = skill_lookup (fread_word (fp));
				if (sn < 0)
					bug ("Fread_opt_char: unknown skill.", 0);
				else
					paf->type = sn;

				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = fread_number (fp);
				paf->next = ch->affected;
				ch->affected = paf;
				fMatch = TRUE;
				break;
			}
			KEY ("AGG", ch->agg, fread_number (fp));
			if (!str_cmp (word, "AUS"))
			{
				ch->pcdata->auspice[0] = fread_number (fp);
				ch->pcdata->auspice[1] = fread_number (fp);
				ch->pcdata->auspice[2] = fread_number (fp);
				ch->pcdata->auspice[3] = fread_number (fp);
				ch->pcdata->auspice[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("AVC", ch->pcdata->auto_vote_counter, fread_number (fp));
			KEY ("ATM", ch->pcdata->atm, fread_number (fp));
			if (!str_cmp (word, "A_M"))
			{
				ch->pcdata->mod_str = fread_number (fp);
				ch->pcdata->mod_int = fread_number (fp);
				ch->pcdata->mod_wis = fread_number (fp);
				ch->pcdata->mod_dex = fread_number (fp);
				ch->pcdata->mod_con = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "ATT"))
			{
				ch->attributes[0] = fread_number (fp);
				ch->attributes[1] = fread_number (fp);
				ch->attributes[2] = fread_number (fp);
				ch->attributes[3] = fread_number (fp);
				ch->attributes[4] = fread_number (fp);
				ch->attributes[5] = fread_number (fp);
				ch->attributes[6] = fread_number (fp);
				ch->attributes[7] = fread_number (fp);
				ch->attributes[8] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "A_P"))
			{
				ch->pcdata->perm_str = fread_number (fp);
				ch->pcdata->perm_int = fread_number (fp);
				ch->pcdata->perm_wis = fread_number (fp);
				ch->pcdata->perm_dex = fread_number (fp);
				ch->pcdata->perm_con = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'B':
			KEY ("BBC", ch->pcdata->blood_burn_counter, fread_number (fp));
			KEY ("BDM", ch->pcdata->bashingdamage, fread_number (fp));
			KEY ("BFI", ch->pcdata->bamfin, fread_string (fp));
			KEY ("BFO", ch->pcdata->bamfout, fread_string (fp));
			KEY ("BST", ch->beast, fread_number (fp));
			if (!str_cmp (word, "BLD"))
			{
				ch->blood[0] = fread_number (fp);
				ch->blood[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("BDY", ch->body, fread_number (fp));
			KEY ("BDM", ch->pcdata->bodymods, fread_number (fp));
			KEY ("BPT", ch->pcdata->bpoints, fread_number (fp));
			if (!str_cmp (word, "BRE"))
			{
				ch->pcdata->breed[0] = fread_number (fp);
				ch->pcdata->breed[1] = fread_number (fp);
				ch->pcdata->breed[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "BRD"))
			{
				int i, maxboards;
				maxboards = fread_number (fp);
				for (i = 0; i < maxboards; i++)
					ch->pcdata->last_note[i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'C':
			KEY ("CDD", ch->pcdata->crinosdesc, fread_string (fp));
			KEY ("C_L", ch->pcdata->leader, fread_string (fp));
			KEY ("CEL", ch->pcdata->celerity, fread_number (fp));
			KEY ("CNC", ch->pcdata->canbuycounter, fread_number (fp));
			KEY ("CVC", ch->pcdata->canvotecounter, fread_number (fp));
			KEY ("CAN", ch->pcdata->canbuy, fread_number (fp));
			KEY ("CLN", ch->clan, fread_string (fp));
			if (!str_cmp (word, "CLD"))
			{
				ch->pcdata->cland[0] = fread_number (fp);
				ch->pcdata->cland[1] = fread_number (fp);
				ch->pcdata->cland[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("CLS", ch->class, fread_number (fp));
			KEY ("CPT", ch->pcdata->conception, fread_string (fp));
			KEY ("CND", ch->pcdata->conding, fread_string (fp));
			if (IS_INQUISITOR(ch))
				if (!str_cmp (word, "CNV"))
				{
					ch->pcdata->conviction[CONVICTION_CURRENT] = fread_number (fp);
					ch->pcdata->conviction[CONVICTION_MAX] = fread_number (fp);
					fMatch = TRUE;
					break;
				}
			if (!str_cmp (word, "CON"))
			{
				ch->pcdata->condition[0] = fread_number (fp);
				ch->pcdata->condition[1] = fread_number (fp);
				ch->pcdata->condition[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("CJR", ch->pcdata->conjure, fread_number (fp));
			if (!str_cmp (word, "C_M"))
			{
				ch->pcdata->cot_min[0] = fread_number (fp);
				ch->pcdata->cot_min[1] = fread_number (fp);
				ch->pcdata->cot_min[2] = fread_number (fp);
				ch->pcdata->cot_min[3] = fread_number (fp);
				ch->pcdata->cot_min[4] = fread_number (fp);
				ch->pcdata->cot_min[5] = fread_number (fp);
				ch->pcdata->cot_min[6] = fread_number (fp);
				ch->pcdata->cot_min[7] = fread_number (fp);
				ch->pcdata->cot_min[8] = fread_number (fp);
				ch->pcdata->cot_min[9] = fread_number (fp);
				ch->pcdata->cot_min[10] = fread_number (fp);
				ch->pcdata->cot_min[11] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("COT", ch->pcdata->coterie, fread_string (fp));
			KEY ("C_T", ch->pcdata->cot_time, fread_string (fp));
			KEY ("C_C", ch->pcdata->cot_clans, fread_string (fp));
			KEY ("C_S", ch->pcdata->cot_sides, fread_string (fp));
			KEY ("CPR", ch->pcdata->cparents, fread_string (fp));
			KEY ("CPM", ch->cprompt, fread_string (fp));
			KEY ("CTM", ch->createtime, fread_string (fp));
			if (IS_INQUISITOR(ch))
				KEY ("CVB", ch->pcdata->conviction_bank, fread_number (fp));
			break;

		case 'D':
			KEY ("D_M", ch->pcdata->dementation, fread_number (fp));
			KEY ("DVL", ch->pcdata->devil, fread_number (fp));
			KEY ("DAM", ch->damroll, fread_number (fp));
			KEY ("DRK", ch->pcdata->darkness, fread_number (fp));
			KEY ("DEF", ch->deaf, fread_number (fp));
			KEY ("DEM", ch->pcdata->demonic, fread_number (fp));
			KEY ("DMP", ch->pcdata->daemon_powers, fread_number (fp));
			KEY ("DNG", ch->pcdata->doing, fread_string (fp));
			KEY ("DNR", ch->demeanor, fread_string (fp));
			KEY ("DNY", ch->pcdata->denydate, fread_number (fp));
			KEY ("DDS", ch->description, fread_string (fp));
			if (!str_cmp (word, "DIA"))
			{
				ch->pcdata->diableries[0] = fread_number (fp);
				ch->pcdata->diableries[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "DIS"))
			{
				ch->pcdata->disc[0] = fread_number (fp);
				ch->pcdata->disc[1] = fread_number (fp);
				ch->pcdata->disc[2] = fread_number (fp);
				ch->pcdata->disc[3] = fread_number (fp);
				ch->pcdata->disc[4] = fread_number (fp);
				ch->pcdata->disc[5] = fread_number (fp);
				ch->pcdata->disc[6] = fread_number (fp);
				ch->pcdata->disc[7] = fread_number (fp);
				ch->pcdata->disc[8] = fread_number (fp);
				ch->pcdata->disc[9] = fread_number (fp);
				ch->pcdata->disc[10] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("DPT", ch->pcdata->dpoints, fread_number (fp));
			break;

		case 'E':
			KEY ("EML", ch->pcdata->email, fread_string (fp));
			if (!str_cmp (word, "End"))
				return;
			if (!str_cmp(word, "ETR"))
			{
			char *buf;
			char arg[MSL];
			EXTRA *extra;
			arg[0] = '\0';
			buf =  fread_string(fp);
			 if (!str_cmp(buf, ""))
			 {
			 fMatch = TRUE;
			 break;
			 }
			if (extra_free == NULL)
			{
			extra = alloc_perm (sizeof (*extra));
			}
			else
			{
			extra = extra_free;
			extra_free = extra_free->next;
			}

			extra->prev = ch->pcdata->extra;
			if (ch->pcdata->extra)
			ch->pcdata->extra->next = extra;

			extra->next = NULL;
			ch->pcdata->extra = extra;
			ch->pcdata->extra->extra = str_dup(buf);
			buf=one_argument_retain_cap(buf, arg);

			ch->pcdata->extra->catagory = str_dup(arg);
			buf=one_argument_retain_cap(buf, arg);

			ch->pcdata->extra->field = str_dup(arg);
			buf=one_argument_retain_cap(buf, arg);

			ch->pcdata->extra->rating = atoi(arg);

			fMatch = TRUE;
			break;
			}
			KEY ("EVE", ch->pcdata->evileye, fread_number (fp));
			KEY ("EXP", ch->exp, fread_number (fp));
			KEY ("EXT", ch->extra, fread_number (fp));
			KEY ("EX2", ch->extra2, fread_number (fp));
			break;

		case 'F':
			KEY ("FAI", ch->pcdata->faith, fread_number (fp));
			if (!str_cmp (word, "FK1"))
			{
				ch->pcdata->fake_skill = fread_number (fp);
				ch->pcdata->fake_stance = fread_number (fp);
				ch->pcdata->fake_hit = fread_number (fp);
				ch->pcdata->fake_dam = fread_number (fp);
				ch->pcdata->fake_ac = fread_number (fp);
				ch->pcdata->fake_hp = fread_number (fp);
				ch->pcdata->fake_mana = fread_number (fp);
				ch->pcdata->fake_move = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "FK2"))
			{
				ch->pcdata->fake_max_hp = fread_number (fp);
				ch->pcdata->fake_max_mana = fread_number (fp);
				ch->pcdata->fake_max_move = fread_number (fp);
				ch->pcdata->fake_stats[STAT_STR] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_INT] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_WIS] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_DEX] = fread_number (fp);
				ch->pcdata->fake_stats[STAT_CON] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			/* Merits/Flaws Stuff */
                        if (!str_cmp (word, "FLW"))
                        {
                                temp_string = fread_string (fp);
                                ch->pcdata->flaws[flaw_ctr] = (char *)malloc(sizeof(char) * strlen(temp_string));
                                ch->pcdata->flaws[flaw_ctr++] = strdup(temp_string);
                                fMatch = TRUE;
                                break;
                        }
			/* End Merits/Flaws Stuff */
			KEY ("FRM", ch->form, fread_number (fp));
			break;

		case 'G':
			KEY ("GFL", ch->pcdata->giftlist, fread_string (fp));
			KEY ("GDD", ch->pcdata->glabrodesc, fread_string (fp));
			KEY ("GEN", ch->vampgen, fread_number (fp));
			if (!str_cmp (word, "GNS"))
			{
				ch->pcdata->genes[0] = fread_number (fp);
				ch->pcdata->genes[1] = fread_number (fp);
				ch->pcdata->genes[2] = fread_number (fp);
				ch->pcdata->genes[3] = fread_number (fp);
				ch->pcdata->genes[4] = fread_number (fp);
				ch->pcdata->genes[5] = fread_number (fp);
				ch->pcdata->genes[6] = fread_number (fp);
				ch->pcdata->genes[7] = fread_number (fp);
				ch->pcdata->genes[8] = fread_number (fp);
				ch->pcdata->genes[9] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "GNO"))
			{
				ch->pcdata->gnosis[0] = fread_number (fp);
				ch->pcdata->gnosis[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("GLD", ch->gold, fread_number (fp));
			KEY ("GPT", ch->pcdata->gpoints, fread_number (fp));
			break;

		case 'H':
			KEY ("HDD", ch->pcdata->hispodesc, fread_string (fp));
			KEY ("HIT", ch->hitroll, fread_number (fp));
			KEY ("HOM", ch->home, fread_number (fp));
			if (!str_cmp (word, "HMM"))
			{
				ch->hit = fread_number (fp);
				ch->max_hit = fread_number (fp);
				ch->mana = fread_number (fp);
				ch->max_mana = fread_number (fp);
				ch->move = fread_number (fp);
				ch->max_move = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "HXY"))
			{
				ch->home_x = fread_number (fp);
				ch->home_y = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'I':
			KEY ("IFR", ch->pcdata->infernal_ranking, fread_number (fp));
			KEY ("IMS", ch->impulse, fread_string (fp));
			KEY ("IMU", ch->immune, fread_number (fp));
			KEY ("IAF", ch->itemaffect, fread_number (fp));
			break;

		case 'K':
			if (!str_cmp (word, "KAD"))
			{
				ch->pkill = fread_number (fp);
				ch->pdeath = fread_number (fp);
				ch->mkill = fread_number (fp);
				ch->mdeath = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "KNO"))
			{
				for (i = 0; i < 10; i++)
					ch->abilities[KNOWLEDGES][i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "KNX"))
			{
				for (i = 0; i < 10; i++)
					ch->pcdata->abilities_points[KNOWLEDGES][i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'L':
			KEY ("LDD", ch->pcdata->lupusdesc, fread_string (fp));
			KEY ("LDM", ch->pcdata->lethaldamage, fread_number (fp));
			if (!str_cmp (word, "LAN"))
			{
				ch->pcdata->language[0] = fread_number (fp);
				ch->pcdata->language[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("LHS", ch->lasthost, fread_string (fp));
			KEY ("LTM", ch->lasttime, fread_string (fp));
			KEY ("LVL", ch->level, fread_number (fp));
			if( !str_cmp( word, "LVT" ) )
			{
			  ch->pcdata->last_vote_time.year = fread_number( fp );
			  ch->pcdata->last_vote_time.month = fread_number( fp );
  			  ch->pcdata->last_vote_time.day = fread_number( fp );
    			  fMatch = TRUE;
    			  break;
			}			
			if (!str_cmp (word, "LHP"))
			{
				ch->loc_hp[0] = fread_number (fp);
				ch->loc_hp[1] = fread_number (fp);
				ch->loc_hp[2] = fread_number (fp);
				ch->loc_hp[3] = fread_number (fp);
				ch->loc_hp[4] = fread_number (fp);
				ch->loc_hp[5] = fread_number (fp);
				ch->loc_hp[6] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("LDS", ch->long_descr, fread_string (fp));
			KEY ("LRD", ch->lord, fread_string (fp));
			KEY ("LOV", ch->pcdata->love, fread_string (fp));
			break;

		case 'M':
			KEY ("MAJ", ch->pcdata->majesty, fread_number (fp));
			/* Remember Stuff */
			if (!str_cmp (word, "MEM"))
			{
				temp_string = fread_word(fp);
				add_memory_from_name(ch, temp_string, fread_string(fp));
				break;
			}
			/* End Remember Stuff */
			KEY ("MRG", ch->pcdata->marriage, fread_string (fp));
			KEY ("MOR3", ch->more3, fread_number (fp));
			KEY ("MOR2", ch->more2, fread_number (fp));
			KEY ("MOR", ch->more, fread_number (fp));
			KEY ("MRF", ch->morph, fread_string (fp));
			/* Merits/Flaws Stuff */
			if (!str_cmp (word, "MRT"))
			{
                                temp_string = fread_string (fp);
                                ch->pcdata->merits[merit_ctr] = (char *)malloc(sizeof(char) * strlen(temp_string));
                                ch->pcdata->merits[merit_ctr++] = strdup(temp_string);
                                fMatch = TRUE;
                                break;
			}
			/* End Merits/Flaws Stuff */
			KEY ("MMN", ch->mem_name, fread_string (fp));
			KEY ("MMD", ch->mem_desc, fread_string (fp));
			break;

		case 'N':
			if (!str_cmp (word, "NAM"))
			{
				/*
				 * Name already set externally.
				 */
				fread_to_eol (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("NEC", ch->pcdata->necromancy, fread_number (fp));
			KEY ("NTR", ch->nature, fread_string (fp));
			break;

		case 'O':
			KEY ("OBE", ch->pcdata->obeah, fread_number (fp));
			KEY ("OBV", ch->pcdata->obj_vnum, fread_number (fp));
			KEY ("ORD", ch->order, fread_string (fp));
			break;

		case 'P':

		  if(!str_cmp(word, "PTM"))
		    {
		      for(i=0;i<MAX_THAUM_PATHS;i++)
			ch->pcdata->pthaum[i] = fread_number(fp);
		      fMatch = TRUE;
		      break;
		    }
			if (!str_cmp (word, "PDX"))
			{
				ch->paradox[0] = fread_number (fp);
				ch->paradox[1] = fread_number (fp);
				ch->paradox[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("PLN", ch->plane, fread_number (fp));
			KEY ("PAR", ch->pcdata->parents, fread_string (fp));
			KEY ("PSW", ch->pcdata->pwd, fread_string (fp));
			KEY ("PLY", ch->played, fread_number (fp));
			KEY ("PIC", ch->playedIC, fread_number(fp));
			KEY ("PAF", ch->polyaff, fread_number (fp));
			if (!str_cmp (word, "POW"))
			{
				ch->pcdata->power[0] = fread_number (fp);
				ch->pcdata->power[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "PWR"))
			{
				ch->pcdata->powers[0] = fread_number (fp);
				ch->pcdata->powers[1] = fread_number (fp);
				ch->pcdata->powers[2] = fread_number (fp);
				ch->pcdata->powers[3] = fread_number (fp);
				ch->pcdata->powers[4] = fread_number (fp);
				ch->pcdata->powers[5] = fread_number (fp);
				ch->pcdata->powers[6] = fread_number (fp);
				ch->pcdata->powers[7] = fread_number (fp);
				ch->pcdata->powers[8] = fread_number (fp);
				ch->pcdata->powers[9] = fread_number (fp);
				ch->pcdata->powers[10] = fread_number (fp);
				ch->pcdata->powers[11] = fread_number (fp);
				ch->pcdata->powers[12] = fread_number (fp);
				ch->pcdata->powers[13] = fread_number (fp);
				ch->pcdata->powers[14] = fread_number (fp);
				ch->pcdata->powers[15] = fread_number (fp);
				ch->pcdata->powers[16] = fread_number (fp);
				ch->pcdata->powers[17] = fread_number (fp);
				ch->pcdata->powers[18] = fread_number (fp);
				ch->pcdata->powers[19] = fread_number (fp);
				ch->pcdata->powers[20] = fread_number (fp);
				ch->pcdata->powers[21] = fread_number (fp);
				ch->pcdata->powers[22] = fread_number (fp);
				ch->pcdata->powers[23] = fread_number (fp);
				ch->pcdata->powers[24] = fread_number (fp);
				ch->pcdata->powers[25] = fread_number (fp);
				ch->pcdata->powers[26] = fread_number (fp);
				ch->pcdata->powers[27] = fread_number (fp);
				ch->pcdata->powers[28] = fread_number (fp);
				ch->pcdata->powers[29] = fread_number (fp);
				ch->pcdata->powers[30] = fread_number (fp);
				ch->pcdata->powers[31] = fread_number (fp);
				ch->pcdata->powers[32] = fread_number (fp);
				ch->pcdata->powers[33] = fread_number (fp);
				ch->pcdata->powers[34] = fread_number (fp);
				ch->pcdata->powers[35] = fread_number (fp);
				ch->pcdata->powers[36] = fread_number (fp);
				ch->pcdata->powers[37] = fread_number (fp);
				ch->pcdata->powers[38] = fread_number (fp);
				ch->pcdata->powers[39] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "PWM"))
			{
				ch->pcdata->powers_mod[0] = fread_number (fp);
				ch->pcdata->powers_mod[1] = fread_number (fp);
				ch->pcdata->powers_mod[2] = fread_number (fp);
				ch->pcdata->powers_mod[3] = fread_number (fp);
				ch->pcdata->powers_mod[4] = fread_number (fp);
				ch->pcdata->powers_mod[5] = fread_number (fp);
				ch->pcdata->powers_mod[6] = fread_number (fp);
				ch->pcdata->powers_mod[7] = fread_number (fp);
				ch->pcdata->powers_mod[8] = fread_number (fp);
				ch->pcdata->powers_mod[9] = fread_number (fp);
				ch->pcdata->powers_mod[10] = fread_number (fp);
				ch->pcdata->powers_mod[11] = fread_number (fp);
				ch->pcdata->powers_mod[12] = fread_number (fp);
				ch->pcdata->powers_mod[13] = fread_number (fp);
				ch->pcdata->powers_mod[14] = fread_number (fp);
				ch->pcdata->powers_mod[15] = fread_number (fp);
				ch->pcdata->powers_mod[16] = fread_number (fp);
				ch->pcdata->powers_mod[17] = fread_number (fp);
				ch->pcdata->powers_mod[18] = fread_number (fp);
				ch->pcdata->powers_mod[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "PWS"))
			{
				ch->pcdata->powers_set[0] = fread_number (fp);
				ch->pcdata->powers_set[1] = fread_number (fp);
				ch->pcdata->powers_set[2] = fread_number (fp);
				ch->pcdata->powers_set[3] = fread_number (fp);
				ch->pcdata->powers_set[4] = fread_number (fp);
				ch->pcdata->powers_set[5] = fread_number (fp);
				ch->pcdata->powers_set[6] = fread_number (fp);
				ch->pcdata->powers_set[7] = fread_number (fp);
				ch->pcdata->powers_set[8] = fread_number (fp);
				ch->pcdata->powers_set[9] = fread_number (fp);
				ch->pcdata->powers_set[10] = fread_number (fp);
				ch->pcdata->powers_set[11] = fread_number (fp);
				ch->pcdata->powers_set[12] = fread_number (fp);
				ch->pcdata->powers_set[13] = fread_number (fp);
				ch->pcdata->powers_set[14] = fread_number (fp);
				ch->pcdata->powers_set[15] = fread_number (fp);
				ch->pcdata->powers_set[16] = fread_number (fp);
				ch->pcdata->powers_set[17] = fread_number (fp);
				ch->pcdata->powers_set[18] = fread_number (fp);
				ch->pcdata->powers_set[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("PWW", ch->pcdata->powers_set_wear, fread_number (fp));
			KEY ("POA", ch->poweraction, fread_string (fp));
			KEY ("POT", ch->powertype, fread_string (fp));
			KEY ("POS", ch->position, fread_number (fp));
			KEY ("PRT", ch->pcdata->pretitle, fread_string(fp));
			KEY ("PRM", ch->prompt, fread_string (fp));
			break;

		case 'Q':
			if (!str_cmp (word, "QNT"))
			{
				ch->quintessence = fread_number (fp);
				ch->max_quintessence = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "Q_S"))
			{
				ch->pcdata->qstats[0] = fread_number (fp);
				ch->pcdata->qstats[1] = fread_number (fp);
				ch->pcdata->qstats[2] = fread_number (fp);
				ch->pcdata->qstats[3] = fread_number (fp);
				ch->pcdata->qstats[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("QST", ch->pcdata->quest, fread_number (fp));
			break;

		case 'R':
			if (!str_cmp (word, "RNW"))
			{
				ch->pcdata->renown[0] = fread_number (fp);
				ch->pcdata->renown[1] = fread_number (fp);
				ch->pcdata->renown[2] = fread_number (fp);
				ch->pcdata->renown[3] = fread_number (fp);
				ch->pcdata->renown[4] = fread_number (fp);
				ch->pcdata->renown[5] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("RAC", ch->race, fread_number (fp));
			KEY ("RDS", ch->pcdata->roomdesc, fread_string (fp));
			KEY ("RNK", ch->pcdata->rank, fread_number (fp));
			if (!str_cmp (word, "RAG"))
			{
				ch->pcdata->rage[0] = fread_number (fp);
				ch->pcdata->rage[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("RGN", ch->pcdata->regenerate, fread_number (fp));
			KEY ("RNA", ch->pcdata->reina, fread_number (fp));
			KEY ("RNT", ch->ringtone, fread_string (fp));
			if (!str_cmp (word, "RST"))
			{
				ch->pcdata->resist[0] = fread_number (fp);
				ch->pcdata->resist[1] = fread_number (fp);
				ch->pcdata->resist[2] = fread_number (fp);
				ch->pcdata->resist[3] = fread_number (fp);
				ch->pcdata->resist[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "ROM"))
			{
				ch->in_room = get_room_index (fread_number (fp));
				if (ch->in_room == NULL)
					ch->in_room = get_room_index (ROOM_VNUM_LIMBO);
				fMatch = TRUE;
				break;
			}
			KEY ("ROD", ch->pcdata->road, fread_number (fp));
			KEY ("RDT", ch->pcdata->roadt, fread_number (fp));
			KEY ("RPF", ch->rpflag_time, (time_t) fread_number (fp));
			KEY ("RPT", ch->pcdata->rpoints, fread_number (fp));
			if (!str_cmp (word, "RNE"))
			{
				ch->pcdata->runes[0] = fread_number (fp);
				ch->pcdata->runes[1] = fread_number (fp);
				ch->pcdata->runes[2] = fread_number (fp);
				ch->pcdata->runes[3] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;

		case 'S':
			KEY ("SAV", ch->saving_throw, fread_number (fp));
			if (!str_cmp (word, "SCO"))
			{
				ch->pcdata->score[0] = fread_number (fp);
				ch->pcdata->score[1] = fread_number (fp);
				ch->pcdata->score[2] = fread_number (fp);
				ch->pcdata->score[3] = fread_number (fp);
				ch->pcdata->score[4] = fread_number (fp);
				ch->pcdata->score[5] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("SCR", ch->pcdata->security, fread_number (fp));	/* OLC */
			KEY ("SEX", ch->sex, fread_number (fp));
			KEY ("SDC", ch->pcdata->crinosdescshort, fread_string (fp));
			KEY ("SDG", ch->pcdata->glabrodescshort, fread_string (fp));
			KEY ("SDH", ch->pcdata->hispodescshort, fread_string (fp));
			KEY ("SDL", ch->pcdata->lupusdescshort, fread_string (fp));
			KEY ("SDS", ch->short_descr, fread_string (fp));
			KEY ("SID", ch->side, fread_string (fp));
			KEY ("SIL", ch->pcdata->silence, fread_number (fp));
			if (!str_cmp (word, "SKI"))
			{
				for (i = 0; i < 10; i++)
					ch->abilities[SKILLS][i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "SKX"))
			{
				for (i = 0; i < 10; i++)
					ch->pcdata->abilities_points[SKILLS][i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "SKL"))
			{
				int sn;
				int value;

				value = fread_number (fp);
				sn = skill_lookup (fread_word (fp));

				if (sn >= 0)
					ch->pcdata->learned[sn] = value;
				fMatch = TRUE;
			}
			KEY ("SLK", ch->pcdata->soul_keeper, fread_string (fp));
			KEY ("SOL", ch->pcdata->soul, fread_number (fp));
			KEY ("SPP", ch->specpower, fread_number (fp));
			KEY ("SPT", ch->spectype, fread_number (fp));
			if (!str_cmp (word, "SPL"))
			{
				ch->spl[0] = fread_number (fp);
				ch->spl[1] = fread_number (fp);
				ch->spl[2] = fread_number (fp);
				ch->spl[3] = fread_number (fp);
				ch->spl[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("SPR", ch->pcdata->spirit, fread_number (fp));
			KEY ("SVC", ch->pcdata->statusvotecounter, fread_number (fp));
			KEY ("SSD", ch->pcdata->show_social, fread_number(fp));
			if (!str_cmp (word, "STG"))
			{
				ch->pcdata->stage[0] = fread_number (fp);
				ch->pcdata->stage[1] = fread_number (fp);
				ch->pcdata->stage[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("SST", ch->pcdata->sectstatus, fread_number (fp));
			if (!str_cmp (word, "STN"))
			{
				ch->stance[0] = fread_number (fp);
				ch->stance[1] = fread_number (fp);
				ch->stance[2] = fread_number (fp);
				ch->stance[3] = fread_number (fp);
				ch->stance[4] = fread_number (fp);
				ch->stance[5] = fread_number (fp);
				ch->stance[6] = fread_number (fp);
				ch->stance[7] = fread_number (fp);
				ch->stance[8] = fread_number (fp);
				ch->stance[9] = fread_number (fp);
				ch->stance[10] = fread_number (fp);
				ch->stance[11] = fread_number (fp);
				ch->stance[12] = fread_number (fp);
				ch->stance[13] = fread_number (fp);
				ch->stance[14] = fread_number (fp);
				ch->stance[15] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("S_N", ch->pcdata->stancename, fread_string (fp));
			KEY ("S_L", ch->pcdata->stancelist, fread_string (fp));
			KEY ("S_W", ch->pcdata->stancework, fread_string (fp));
			if (!str_cmp (word, "S_M"))
			{
				ch->pcdata->stancemove[0] = fread_number (fp);
				ch->pcdata->stancemove[1] = fread_number (fp);
				ch->pcdata->stancemove[2] = fread_number (fp);
				ch->pcdata->stancemove[3] = fread_number (fp);
				ch->pcdata->stancemove[4] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "STA"))
			{
				ch->pcdata->status[0] = fread_number (fp);
				ch->pcdata->status[1] = fread_number (fp);
				ch->pcdata->status[2] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			
			break;

		case 'T':
	                if (!str_cmp (word, "THM"))
			{
				ch->pcdata->thaum_type = fread_number (fp);
				ch->pcdata->thaum_prime = fread_number (fp);
				ch->pcdata->research_power = fread_number (fp);
				ch->pcdata->research_time = fread_number (fp);
				ch->pcdata->research_type = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "TAL"))
			{
				for (i = 0; i < 10; i++)
					ch->abilities[TALENTS][i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "TLX"))
			{
				for (i = 0; i < 10; i++)
					ch->pcdata->abilities_points[TALENTS][i] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("TPR", ch->pcdata->temporis, fread_number (fp));
			if (!str_cmp (word, "TRB"))
			{
				ch->pcdata->tribes[0] = fread_number (fp);
				ch->pcdata->tribes[1] = fread_number (fp);
				ch->pcdata->tribes[2] = fread_number (fp);
				ch->pcdata->tribes[3] = fread_number (fp);
				ch->pcdata->tribes[4] = fread_number (fp);
				ch->pcdata->tribes[5] = fread_number (fp);
				ch->pcdata->tribes[6] = fread_number (fp);
				ch->pcdata->tribes[7] = fread_number (fp);
				ch->pcdata->tribes[8] = fread_number (fp);
				ch->pcdata->tribes[9] = fread_number (fp);
				ch->pcdata->tribes[10] = fread_number (fp);
				ch->pcdata->tribes[11] = fread_number (fp);
				ch->pcdata->tribes[12] = fread_number (fp);
				ch->pcdata->tribes[13] = fread_number (fp);
				ch->pcdata->tribes[14] = fread_number (fp);
				ch->pcdata->tribes[15] = fread_number (fp);
				ch->pcdata->tribes[16] = fread_number (fp);
				ch->pcdata->tribes[17] = fread_number (fp);
				ch->pcdata->tribes[18] = fread_number (fp);
				ch->pcdata->tribes[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "THA"))
			{
				ch->pcdata->thaum[0] = fread_number (fp);
				ch->pcdata->thaum[1] = fread_number (fp);
				ch->pcdata->thaum[2] = fread_number (fp);
				ch->pcdata->thaum[3] = fread_number (fp);
				ch->pcdata->thaum[4] = fread_number (fp);
				ch->pcdata->thaum[5] = fread_number (fp);
				ch->pcdata->thaum[6] = fread_number (fp);
				ch->pcdata->thaum[7] = fread_number (fp);
				ch->pcdata->thaum[8] = fread_number (fp);
				ch->pcdata->thaum[9] = fread_number (fp);
				ch->pcdata->thaum[10] = fread_number (fp);
				ch->pcdata->thaum[11] = fread_number (fp);
				ch->pcdata->thaum[12] = fread_number (fp);
				ch->pcdata->thaum[13] = fread_number (fp);
				ch->pcdata->thaum[14] = fread_number (fp);
				ch->pcdata->thaum[15] = fread_number (fp);
				ch->pcdata->thaum[16] = fread_number (fp);
				ch->pcdata->thaum[17] = fread_number (fp);
				ch->pcdata->thaum[18] = fread_number (fp);
				ch->pcdata->thaum[19] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			KEY ("TDC", ch->pcdata->tempdesc, fread_string (fp));
			KEY ("TDS", ch->pcdata->showtempdesc, fread_number (fp));
			KEY ("TBS", ch->truebeast, fread_number (fp));
			KEY ("TRS", ch->trust, fread_number (fp));
			if (!str_cmp (word, "TTL"))
			{
				ch->pcdata->title = fread_string (fp);
				if (isalpha (ch->pcdata->title[0]) || isdigit (ch->pcdata->title[0]))
				{
					sprintf (buf, "%s", ch->pcdata->title);
					free_string (ch->pcdata->title);
					ch->pcdata->title = str_dup (buf);
				}
				fMatch = TRUE;
				break;
			}
			break;

		case 'V':
			KEY ("VSN", ch->pcdata->vision, fread_number (fp));
			KEY ("VAF", ch->vampaff, fread_number (fp));
			KEY ("Vamppass", ch->vamppass, fread_number (fp));
			KEY ("VIB", ch->phonevibrate, fread_number (fp));
			KEY ("VOI", ch->pcdata->voice, fread_string (fp));
			KEY ("VOT", ch->pcdata->votes, fread_number (fp));
			if (!str_cmp (word, "VRT"))
			{
				ch->pcdata->virtues[VIRTUE_CONSCIENCE] = fread_number (fp);
				ch->pcdata->virtues[VIRTUE_SELFCONTROL] = fread_number (fp);
				ch->pcdata->virtues[VIRTUE_COURAGE] = fread_number (fp);
				if (IS_INQUISITOR(ch))
				{
					ch->pcdata->virtues[VIRTUE_WISDOM] = fread_number (fp);
					ch->pcdata->virtues[VIRTUE_FAITH] = fread_number (fp);
					ch->pcdata->virtues[VIRTUE_ZEAL] = fread_number (fp);
				}
				fMatch = TRUE;
				break;
			}
			break;

		case 'W':
			if (!str_cmp (word, "WPN"))
			{
				ch->wpn[0] = fread_number (fp);
				ch->wpn[1] = fread_number (fp);
				ch->wpn[2] = fread_number (fp);
				ch->wpn[3] = fread_number (fp);
				ch->wpn[4] = fread_number (fp);
				ch->wpn[5] = fread_number (fp);
				ch->wpn[6] = fread_number (fp);
				ch->wpn[7] = fread_number (fp);
				ch->wpn[8] = fread_number (fp);
				ch->wpn[9] = fread_number (fp);
				ch->wpn[10] = fread_number (fp);
				ch->wpn[11] = fread_number (fp);
				ch->wpn[12] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			if (!str_cmp (word, "WIL"))
			{
				ch->pcdata->willpower[0] = fread_number (fp);
				ch->pcdata->willpower[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("WMP", ch->wimpy, fread_number (fp));
			KEY ("WLF", ch->pcdata->wolf, fread_number (fp));
			if (!str_cmp (word, "WFF"))
			{
				ch->pcdata->wolfform[0] = fread_number (fp);
				ch->pcdata->wolfform[1] = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			KEY ("WPT", ch->pcdata->wpoints, fread_number (fp));
			KEY ("WRC", ch->pcdata->wp_regain_counter, fread_number (fp));
			if (!str_cmp (word, "WXY"))
			{
				ch->x = fread_number (fp);
				ch->y = fread_number (fp);
				fMatch = TRUE;
				break;
			}
			break;
		}


		if (!fMatch)
			fread_to_eol (fp);
	}
}



void fread_obj (CHAR_DATA * ch, FILE * fp)
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;
	char *word;
	int iNest;
	bool fMatch;
	bool fNest;
	bool fVnum;

	if (obj_free == NULL)
	{
		obj = alloc_perm (sizeof (*obj));
	}
	else
	{
		obj = obj_free;
		obj_free = obj_free->next;
	}

	*obj = obj_zero;
	obj->name = str_dup ("");
	obj->short_descr = str_dup ("");
	obj->description = str_dup ("");
	obj->chpoweron = str_dup ("(null)");
	obj->chpoweroff = str_dup ("(null)");
	obj->chpoweruse = str_dup ("(null)");
	obj->victpoweron = str_dup ("(null)");
	obj->victpoweroff = str_dup ("(null)");
	obj->victpoweruse = str_dup ("(null)");
	obj->actcommands = str_dup ("");
	obj->questmaker = str_dup ("");
	obj->questowner = str_dup ("");
	obj->spectype = 0;
	obj->specpower = 0;
	obj->condition = 100;
	obj->toughness = 5;
	obj->resistance = 100;
	obj->wear_loc = -1;
	obj->quest = 0;
	obj->points = 0;

	fNest = FALSE;
	fVnum = TRUE;
	iNest = 0;

	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;

		switch (UPPER (word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;

		case 'A':
			KEY ("ActCommands", obj->actcommands, fread_string (fp));
			if (!str_cmp (word, "Affect") || !str_cmp (word, "AffectData"))
			{
				AFFECT_DATA *paf;

				if (affect_free == NULL)
				{
					paf = alloc_perm (sizeof (*paf));
				}
				else
				{
					paf = affect_free;
					affect_free = affect_free->next;
				}

				paf->type = -1;
				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = 0;
				paf->next = obj->affected;
				obj->affected = paf;
				fMatch = TRUE;
				break;
			}
			break;

		case 'C':
			KEY ("Condition", obj->condition, fread_number (fp));
			KEY ("Cost", obj->cost, fread_number (fp));
			break;

		case 'D':
			KEY ("Description", obj->description, fread_string (fp));
			break;

		case 'E':
			KEY ("ExtraFlags", obj->extra_flags, fread_number (fp));

			if (!str_cmp (word, "ExtraDescr"))
			{
				EXTRA_DESCR_DATA *ed;

				if (extra_descr_free == NULL)
				{
					ed = alloc_perm (sizeof (*ed));
				}
				else
				{
					ed = extra_descr_free;
					extra_descr_free = extra_descr_free->next;
				}

				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = obj->extra_descr;
				obj->extra_descr = ed;
				fMatch = TRUE;
			}

			if (!str_cmp (word, "End"))
			{
				if (!fNest || !fVnum)
				{
					bug ("Fread_obj: incomplete object.", 0);
					if (obj->name)
						free_string (obj->name);
					if (obj->description)
						free_string (obj->description);
					if (obj->short_descr)
						free_string (obj->short_descr);
					obj->next = obj_free;
					obj_free = obj;
					return;
				}
				else
				{
					obj->next = object_list;
					object_list = obj;
					obj->pIndexData->count++;
					if (iNest == 0 || rgObjNest[iNest] == NULL)
						obj_to_char (obj, ch);
					else
						obj_to_obj (obj, rgObjNest[iNest - 1]);
					return;
				}
			}
			break;

		case 'I':
			KEY ("ItemType", obj->item_type, fread_number (fp));
			break;

		case 'L':
			KEY ("Level", obj->level, fread_number (fp));
			break;

		case 'N':
			KEY ("Name", obj->name, fread_string (fp));

			if (!str_cmp (word, "Nest"))
			{
				iNest = fread_number (fp);
				if (iNest < 0 || iNest >= MAX_NEST)
				{
					bug ("Fread_obj: bad nest %d.", iNest);
				}
				else
				{
					rgObjNest[iNest] = obj;
					fNest = TRUE;
				}
				fMatch = TRUE;
			}
			break;

		case 'P':
			KEY ("Points", obj->points, fread_number (fp));
			KEY ("Poweronch", obj->chpoweron, fread_string (fp));
			KEY ("Poweroffch", obj->chpoweroff, fread_string (fp));
			KEY ("Powerusech", obj->chpoweruse, fread_string (fp));
			KEY ("Poweronvict", obj->victpoweron, fread_string (fp));
			KEY ("Poweroffvict", obj->victpoweroff, fread_string (fp));
			KEY ("Powerusevict", obj->victpoweruse, fread_string (fp));
			break;

		case 'Q':
			KEY ("Quest", obj->quest, fread_number (fp));
			KEY ("Questmaker", obj->questmaker, fread_string (fp));
			KEY ("Questowner", obj->questowner, fread_string (fp));
			break;

		case 'R':
			KEY ("Resistance", obj->resistance, fread_number (fp));
			break;

		case 'S':
			KEY ("ShortDescr", obj->short_descr, fread_string (fp));
			KEY ("Spectype", obj->spectype, fread_number (fp));
			KEY ("Specpower", obj->specpower, fread_number (fp));

			if (!str_cmp (word, "Spell"))
			{
				int iValue;
				int sn;

				iValue = fread_number (fp);
				sn = skill_lookup (fread_word (fp));
				if (iValue < 0 || iValue > 3)
				{
					bug ("Fread_obj: bad iValue %d.", iValue);
				}
				else if (sn < 0)
				{
					bug ("Fread_obj: unknown skill.", 0);
				}
				else
				{
					obj->value[iValue] = sn;
				}
				fMatch = TRUE;
				break;
			}
			break;

		case 'T':
			KEY ("Timer", obj->timer, fread_number (fp));
			KEY ("Toughness", obj->toughness, fread_number (fp));
			break;

		case 'V':
			if (!str_cmp (word, "Values"))
			{
				obj->value[0] = fread_number (fp);
				obj->value[1] = fread_number (fp);
				obj->value[2] = fread_number (fp);
				obj->value[3] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "Vnum"))
			{
				int vnum;

				vnum = fread_number (fp);
				if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
				{
					bug ("Fread_obj: bad vnum %d.", vnum);
				}
				else
					fVnum = TRUE;
				fMatch = TRUE;
				break;
			}
			break;

		case 'W':
			KEY ("WearFlags", obj->wear_flags, fread_number (fp));
			KEY ("WearLoc", obj->wear_loc, fread_number (fp));
			KEY ("Weight", obj->weight, fread_number (fp));
			break;

		}

		if (!fMatch)
		{
			bug ("Fread_obj: no match.", 0);
			fread_to_eol (fp);
		}
	}
}



void fread_opt_obj (CHAR_DATA * ch, FILE * fp)
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;
	char *word;
	int iNest;
	bool fMatch;
	bool fNest;
	bool fVnum;

	if (obj_free == NULL)
	{
		obj = alloc_perm (sizeof (*obj));
	}
	else
	{
		obj = obj_free;
		obj_free = obj_free->next;
	}

	*obj = obj_zero;
	obj->name = str_dup ("");
	obj->short_descr = str_dup ("");
	obj->description = str_dup ("");
	obj->chpoweron = str_dup ("(null)");
	obj->chpoweroff = str_dup ("(null)");
	obj->chpoweruse = str_dup ("(null)");
	obj->victpoweron = str_dup ("(null)");
	obj->victpoweroff = str_dup ("(null)");
	obj->victpoweruse = str_dup ("(null)");
	obj->actcommands = str_dup ("");
	obj->questmaker = str_dup ("");
	obj->questowner = str_dup ("");
	obj->spectype = 0;
	obj->specpower = 0;
	obj->condition = 100;
	obj->toughness = 5;
	obj->resistance = 100;
	obj->wear_loc = -1;
	obj->quest = 0;
	obj->points = 0;

	fNest = FALSE;
	fVnum = TRUE;
	iNest = 0;

	for (;;)
	{
		word = feof (fp) ? "End" : fread_word (fp);
		fMatch = FALSE;

		switch (UPPER (word[0]))
		{
		case '*':
			fMatch = TRUE;
			fread_to_eol (fp);
			break;

		case 'A':
			KEY ("ACM", obj->actcommands, fread_string (fp));
			if (!str_cmp (word, "AFD"))
			{
				AFFECT_DATA *paf;

				if (affect_free == NULL)
				{
					paf = alloc_perm (sizeof (*paf));
				}
				else
				{
					paf = affect_free;
					affect_free = affect_free->next;
				}

				paf->type = -1;
				paf->duration = fread_number (fp);
				paf->modifier = fread_number (fp);
				paf->location = fread_number (fp);
				paf->bitvector = 0;
				paf->next = obj->affected;
				obj->affected = paf;
				fMatch = TRUE;
				break;
			}
			break;

		case 'C':
			KEY ("CON", obj->condition, fread_number (fp));
			KEY ("CST", obj->cost, fread_number (fp));
			break;

		case 'D':
			KEY ("DDS", obj->description, fread_string (fp));
			break;

		case 'E':
			KEY ("EFL", obj->extra_flags, fread_number (fp));

			if (!str_cmp (word, "EXD"))
			{
				EXTRA_DESCR_DATA *ed;

				if (extra_descr_free == NULL)
				{
					ed = alloc_perm (sizeof (*ed));
				}
				else
				{
					ed = extra_descr_free;
					extra_descr_free = extra_descr_free->next;
				}

				ed->keyword = fread_string (fp);
				ed->description = fread_string (fp);
				ed->next = obj->extra_descr;
				obj->extra_descr = ed;
				fMatch = TRUE;
			}

			if (!str_cmp (word, "End"))
			{
				if (!fNest || !fVnum)
				{
					bug ("Fread_opt_obj: incomplete object.", 0);
					if (obj->name)
						free_string (obj->name);
					if (obj->description)
						free_string (obj->description);
					if (obj->short_descr)
						free_string (obj->short_descr);
					obj->next = obj_free;
					obj_free = obj;
					return;
				}
				else
				{
					obj->next = object_list;
					object_list = obj;
					obj->pIndexData->count++;
					if (iNest == 0 || rgObjNest[iNest] == NULL)
						obj_to_char (obj, ch);
					else
						obj_to_obj (obj, rgObjNest[iNest - 1]);
					return;
				}
			}
			break;

		case 'I':
			KEY ("ITT", obj->item_type, fread_number (fp));
			break;

		case 'L':
			KEY ("LVL", obj->level, fread_number (fp));
			break;

		case 'N':
			KEY ("NAM", obj->name, fread_string (fp));

			if (!str_cmp (word, "NST"))
			{
				iNest = fread_number (fp);
				if (iNest < 0 || iNest >= MAX_NEST)
				{
					bug ("Fread_opt_obj: bad nest %d.", iNest);
				}
				else
				{
					rgObjNest[iNest] = obj;
					fNest = TRUE;
				}
				fMatch = TRUE;
			}
			break;

		case 'P':
			KEY ("PTS", obj->points, fread_number (fp));
			KEY ("PNC", obj->chpoweron, fread_string (fp));
			KEY ("PFC", obj->chpoweroff, fread_string (fp));
			KEY ("PUC", obj->chpoweruse, fread_string (fp));
			KEY ("PNV", obj->victpoweron, fread_string (fp));
			KEY ("PFV", obj->victpoweroff, fread_string (fp));
			KEY ("PUV", obj->victpoweruse, fread_string (fp));
			break;

		case 'Q':
			KEY ("QST", obj->quest, fread_number (fp));
			KEY ("QMK", obj->questmaker, fread_string (fp));
			KEY ("QON", obj->questowner, fread_string (fp));
			break;

		case 'R':
			KEY ("RES", obj->resistance, fread_number (fp));
			break;

		case 'S':
			KEY ("SDS", obj->short_descr, fread_string (fp));
			KEY ("SPT", obj->spectype, fread_number (fp));
			KEY ("SPP", obj->specpower, fread_number (fp));

			if (!str_cmp (word, "SPL"))
			{
				int iValue;
				int sn;

				iValue = fread_number (fp);
				sn = skill_lookup (fread_word (fp));
				if (iValue < 0 || iValue > 3)
				{
					bug ("Fread_opt_obj: bad iValue %d.", iValue);
				}
				else if (sn < 0)
				{
					bug ("Fread_opt_obj: unknown skill.", 0);
				}
				else
				{
					obj->value[iValue] = sn;
				}
				fMatch = TRUE;
				break;
			}
			break;

		case 'T':
			KEY ("TMR", obj->timer, fread_number (fp));
			KEY ("TOU", obj->toughness, fread_number (fp));
			break;

		case 'V':
			if (!str_cmp (word, "VAL"))
			{
				obj->value[0] = fread_number (fp);
				obj->value[1] = fread_number (fp);
				obj->value[2] = fread_number (fp);
				obj->value[3] = fread_number (fp);
				fMatch = TRUE;
				break;
			}

			if (!str_cmp (word, "VNM"))
			{
				int vnum;

				vnum = fread_number (fp);
				if ((obj->pIndexData = get_obj_index (vnum)) == NULL)
				{
					bug ("Fread_opt_obj: bad vnum %d.", vnum);
					return;
				}
				else
					fVnum = TRUE;
				fMatch = TRUE;
				break;
			}
			break;

		case 'W':
			KEY ("WFL", obj->wear_flags, fread_number (fp));
			KEY ("WLC", obj->wear_loc, fread_number (fp));
			KEY ("WEI", obj->weight, fread_number (fp));
			break;

		}

		if (!fMatch)
		{
			bug ("Fread_opt_obj: no match.", 0);
			fread_to_eol (fp);
		}
	}
}

void fread_tiny_obj (CHAR_DATA * ch, FILE * fp)
{
	OBJ_DATA *obj;
	int iNest, vnum, level;
	bool fNest = FALSE;
	bool bad_vnum = FALSE;
	vnum = fread_number (fp);
	level = fread_number (fp);

	if ((obj = create_object (get_obj_index (vnum), level)) == NULL)
	{
		bug ("Fread_tiny_obj: bad vnum %d.", vnum);
		bad_vnum = TRUE;
	}

	iNest = fread_number (fp);

	if (iNest < 0 || iNest >= MAX_NEST)
	{
		bug ("Fread_tiny_obj: bad nest %d.", iNest);
	}
	else
	{
		rgObjNest[iNest] = obj;
		fNest = TRUE;
	}

	obj->wear_loc = fread_number (fp);
	obj->cost = fread_number (fp);
	obj->timer = fread_number (fp);
	obj->condition = fread_number (fp);
	obj->resistance = fread_number (fp);
	obj->quest = fread_number (fp);
	obj->value[0] = fread_number (fp);
	obj->value[1] = fread_number (fp);
	obj->value[2] = fread_number (fp);
	obj->value[3] = fread_number (fp);
	if (bad_vnum)
	{
		if (obj->name)
			free_string (obj->name);
		if (obj->description)
			free_string (obj->description);
		if (obj->short_descr)
			free_string (obj->short_descr);
		obj->next = obj_free;
		obj_free = obj;
		return;
	}
	if (iNest == 0 || rgObjNest[iNest] == NULL)
		obj_to_char (obj, ch);
	else
		obj_to_obj (obj, rgObjNest[iNest - 1]);

	return;
}



void do_finger (CHAR_DATA * ch, char *argument)
{
	char strsave[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char letter;
	CHAR_DATA *victim;
	FILE *fp;
	char *word;
	bool fMatch;
	bool drop_out = FALSE;

	char *f_name = capitalize (argument);
	char *f_title = "";
	char *f_lasthost = "";
	char *f_lasttime = "";
	int f_extra = 0;
	int f_sex = 0;
	char *f_conception = "";
	char *f_createtime = 0;
	int f_level = 0;
	int f_race = 0;
	char *f_marriage = "";
	int f_pkill = 0;
	int f_pdeath = 0;
	int f_mkill = 0;
	int f_mdeath = 0;
	char *f_email = "";
	int f_played = 0;

	if (IS_NPC (ch))
		return;

	if (!check_parse_name (argument))
	{
		send_to_char ("Thats an illegal name.\n\r", ch);
		return;
	}

	if ((victim = get_char_finger (ch, argument)) != NULL)
	{
		/* Just point directly to the string, no str_dup needed */
		f_title = victim->pcdata->title;
		f_lasthost = victim->lasthost;
		f_lasttime = victim->lasttime;
		f_extra = victim->extra;
		f_sex = victim->sex;
		f_conception = str_dup (victim->pcdata->conception);
		f_createtime = victim->createtime;
		f_level = victim->level;
		f_race = victim->race;
		f_marriage = victim->pcdata->marriage;
		f_pkill = victim->pkill;
		f_pdeath = victim->pdeath;
		f_mkill = victim->mkill;
		f_mdeath = victim->mdeath;
		f_email = victim->pcdata->email;
		f_played = victim->played;
	}
	else
	{
		if (!char_exists (FILE_PLAYER, argument))
		{
			send_to_char ("That player doesn't exist.\n\r", ch);
			return;
		}

		fflush (fpReserve);
		fclose (fpReserve);
		sprintf (strsave, "%s%s", PLAYER_DIR, capitalize (argument));
		if ((fp = fopen (strsave, "r")) != NULL)
		{
			if ((letter = fread_letter (fp)) == EOF)
			{
				send_to_char ("The playerfile has been corrupted.\n\r", ch);
				fflush (fp);
				fclose (fp);
				fpReserve = fopen (NULL_FILE, "r");
				return;
			}
			fread_to_eol (fp);

			while (!drop_out)
			{
				word = feof (fp) ? "End" : fread_word (fp);
				fMatch = FALSE;

				switch (UPPER (word[0]))
				{
				default:
				case '*':
					fMatch = TRUE;
					fread_to_eol (fp);
					break;

				case 'C':
					KEY ("CTM", f_createtime, fread_string (fp));
					KEY ("Createtime", f_createtime, fread_string (fp));
					KEY ("CPT", f_conception, fread_string (fp));
					KEY ("Conception", f_conception, fread_string (fp));
					break;

				case 'E':
					KEY ("EML", f_email, fread_string (fp));
					KEY ("Email", f_email, fread_string (fp));
					if (!str_cmp (word, "End"))
						drop_out = TRUE;
					KEY ("EXT", f_extra, fread_number (fp));
					KEY ("Extra", f_extra, fread_number (fp));
					break;

				case 'K':
					if (!str_cmp (word, "KAD"))
					{
						f_pkill = fread_number (fp);
						f_pdeath = fread_number (fp);
						f_mkill = fread_number (fp);
						f_mdeath = fread_number (fp);
						fMatch = TRUE;
						break;
					}
					break;

				case 'M':
					KEY ("MRG", f_marriage, fread_string (fp));
					KEY ("Marriage", f_marriage, fread_string (fp));
					break;

				case 'L':
					KEY ("LHS", f_lasthost, fread_string (fp));
					KEY ("Lasthost", f_lasthost, fread_string (fp));
					KEY ("LTM", f_lasttime, fread_string (fp));
					KEY ("Lasttime", f_lasttime, fread_string (fp));
					KEY ("LVL", f_level, fread_number (fp));
					KEY ("Level", f_level, fread_number (fp));
					break;

				case 'P':
					KEY ("PLY", f_played, fread_number (fp));
					KEY ("Played", f_played, fread_number (fp));
					if (!str_cmp (word, "PkPdMkMd"))
					{
						f_pkill = fread_number (fp);
						f_pdeath = fread_number (fp);
						f_mkill = fread_number (fp);
						f_mdeath = fread_number (fp);
						fMatch = TRUE;
						break;
					}
					break;

				case 'R':
					KEY ("RAC", f_race, fread_number (fp));
					KEY ("Race", f_race, fread_number (fp));
					break;

				case 'S':
					KEY ("SEX", f_sex, fread_number (fp));
					KEY ("Sex", f_sex, fread_number (fp));
					break;

				case 'T':
					if (!str_cmp (word, "TTL") || !str_cmp (word, "Title"))
					{
						f_title = fread_string (fp);
						if (isalpha (ch->pcdata->title[0]) || isdigit (ch->pcdata->title[0]))
						{
							sprintf (buf, "%s", f_title);
							free_string (f_title);
							f_title = str_dup (buf);
						}
						fMatch = TRUE;
						break;
					}
					break;
				}
				if (!fMatch)
					fread_to_eol (fp);
			}
		}
		else
		{
			bug ("Do_finger: fopen", 0);
			perror (strsave);
		}
		fflush (fp);
		fclose (fp);
		fpReserve = fopen (NULL_FILE, "r");
	}
	send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
	sprintf (buf, "%s%s%s.\n\r", f_name, drop_out ? " " : "", f_title);
	send_to_char (buf, ch);
	send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
	if(IS_STORYTELLER(ch))
	{
		sprintf (buf, "Last connected from %s at %s.\n\r", f_lasthost, f_lasttime);
		send_to_char (buf, ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
	}
	sprintf (buf, "Sex: %s. ", f_sex == SEX_MALE ? "Male" : f_sex == SEX_FEMALE ? "Female" : "None");
	send_to_char (buf, ch);


	switch (f_level)
	{
	default:
		send_to_char ("They are mortal, ", ch);
		break;
	case LEVEL_AVATAR:
		send_to_char ("They are mortal, ", ch);
		break;
	case LEVEL_APPRENTICE:
		break;
	case LEVEL_MAGE:
		break;
	case LEVEL_STORYTELLER:
		send_to_char ("They are a Storyteller, ", ch);
		break;
	case LEVEL_BUILDER:
		send_to_char ("They are a Builder, ", ch);
		break;
	case LEVEL_QUESTMAKER:
		send_to_char ("They are a Quest Maker, ", ch);
		break;
	case LEVEL_ENFORCER:
		send_to_char ("They are an Enforcer, ", ch);
		break;
	case LEVEL_JUDGE:
		send_to_char ("They are a Judge, ", ch);
		break;
	case LEVEL_HIGHJUDGE:
		send_to_char ("They are a High Judge, ", ch);
		break;
	case LEVEL_IMPLEMENTOR:
		send_to_char ("They are an Implementor, ", ch);
		break;
	}
	if (f_played > 0)
		f_played = (2 * (f_played / 7200));
	else
		f_played = 0;	/* Avoid negative playing hours */
	sprintf (buf, "and have been playing for %d hours.\n\r", f_played);
	send_to_char (buf, ch);
	if (strlen (f_marriage) > 2)
	{
		if (IS_SET (f_extra, EXTRA_MARRIED))
			sprintf (buf, "They are married to %s.\n\r", f_marriage);
		else
			sprintf (buf, "They are engaged to %s.\n\r", f_marriage);
		send_to_char (buf, ch);
	}
	sprintf (buf, "Player kills: %d, Player Deaths: %d.\n\r", f_pkill, f_pdeath);
	send_to_char (buf, ch);
	sprintf (buf, "Mob kills: %d, Mob Deaths: %d.\n\r", f_mkill, f_mdeath);
	send_to_char (buf, ch);
	send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
	if (strlen (f_email) > 2 && (!IS_SET (f_extra, EXTRA_INVIS_EMAIL) || IS_IMMORTAL (ch)))
	{
		sprintf (buf, "Email: %s\n\r", f_email);
		send_to_char (buf, ch);
		send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
	}
	return;
}

void save_eban_file( )
{
   FILE *fp;
   EBAN_DATA *pban;

   if( ( fp = fopen( EBAN_FILE, "w" ) ) == NULL )
   {
	bug( "save_eban_file: could not write to EBAN_FILE", 0 );
	return;
   }

   for( pban = eban_list; pban; pban = pban->next )
   {
	fprintf( fp, "%s~\n", pban->name );
   }

   fprintf( fp, "End~\n" );
   fclose( fp );
   return;
}

