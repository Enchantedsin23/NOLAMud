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
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"



/* Local functions */
bool check_social args ((CHAR_DATA * ch, char *command, char *argument));
void	make_preg	args( ( CHAR_DATA *mother, CHAR_DATA *father ) );

void save_disabled args (());
void load_disabled args (());


/*
 * Command logging types.
 */
#define LOG_NORMAL	0
#define LOG_ALWAYS	1
#define LOG_NEVER	2
#define LOG_IFIMM   3 //logs the command if the user is an immortal



/*
 * Log-all switch.
 */
char *last_command = NULL;
int iDelete = 0;
bool fLogAll = FALSE;

bool check_disabled (const struct cmd_type *command);
DISABLED_DATA *disabled_first;
#define END_MARKER    "END"	/* for load_disabled() and save_disabled() */



/*
 * Command table.
 */
const struct cmd_type cmd_table[] = {
	/*
	 * Common movement commands.
	 */
	{"north", do_north, POS_STANDING, 0, LOG_NORMAL},
	{"east", do_east, POS_STANDING, 0, LOG_NORMAL},
	{"south", do_south, POS_STANDING, 0, LOG_NORMAL},
	{"west", do_west, POS_STANDING, 0, LOG_NORMAL},
	{"up", do_up, POS_STANDING, 0, LOG_NORMAL},
	{"down", do_down, POS_STANDING, 0, LOG_NORMAL},

	/*
	 * Common other commands.
	 * Placed here so one and two letter abbreviations work.
	 */
	//{"lolfix", do_lolfix, POS_DEAD, 0, LOG_ALWAYS},
	{ "xlabor",		do_labor,	POS_STANDING,	 1,  LOG_NORMAL	},
	{ "birth",		do_birth,	POS_STANDING,	 1,  LOG_NORMAL	},
	{"enter", do_enter, POS_STANDING, 0, LOG_NORMAL},
	/* Vehicle code by Julius */
	{"drive", do_drive, POS_STANDING, 0, LOG_NORMAL},
	/* End vehicle code */
	{"exits", do_exits, POS_SITTING, 0, LOG_NORMAL},
	{"get", do_get, POS_SITTING, 0, LOG_NORMAL},
	{"inventory", do_inventory, POS_DEAD, 0, LOG_NORMAL},
	{"knock", do_knock, POS_SITTING, 0, LOG_NORMAL},
	{"look", do_look, POS_MEDITATING, 0, LOG_NORMAL},
	{"rat",	do_rat,	POS_STANDING, 0,  LOG_NORMAL},
	// Books
	{"read", do_read, POS_MEDITATING, 0, LOG_NORMAL},
	{"write", do_write, POS_MEDITATING, 0, LOG_NORMAL},
	{"turn", do_turn, POS_MEDITATING, 0, LOG_NORMAL},
	{"insert", do_insertpage, POS_MEDITATING, 0, LOG_NORMAL},
	{"tear", do_tearpage, POS_MEDITATING, 0, LOG_NORMAL},
	// End books
	//{"moon", do_moon_phase, POS_MEDITATING, 0, LOG_NORMAL},
	{"taxi", do_taxi, POS_MEDITATING, 0, LOG_NORMAL},
	{"leave", do_leave, POS_STANDING, 0, LOG_NORMAL},
	{"roll", do_proll, POS_STANDING, 0, LOG_ALWAYS},
	{"order", do_order, POS_STANDING, 0, LOG_NORMAL},
	{"purchase", do_purchase, POS_STANDING, 0, LOG_NORMAL},
	{"paycheck", do_paycheque, POS_STANDING, 0, LOG_NORMAL},
	{"wpburn", do_wpburn, POS_STANDING, 0, LOG_ALWAYS},
	{"contracts",	do_contracts,	POS_DEAD, 0, LOG_NORMAL},
	/* Coterie stuff */
	{"motley",	do_coterie,	POS_DEAD, 0, LOG_NORMAL},
	{"mtalk", do_ctalk,	POS_DEAD, 0, LOG_NORMAL},
	/* Obfuscate Code */
	{"nulled", do_smoke1, POS_STANDING, 0, LOG_ALWAYS},
	{"lightshy", do_smoke3, POS_STANDING, 0, LOG_ALWAYS},
	{"stranger", do_smoke4, POS_STANDING, 0, LOG_ALWAYS},
	{"skills", do_skills, POS_STANDING, 0, LOG_NORMAL},
	{"fishslap", do_fishslap, POS_STANDING, 0, LOG_NORMAL},
	{"finish", do_finish, POS_STANDING, 0, LOG_ALWAYS},
	{"pathfind", do_pathfind, POS_SLEEPING, 0, LOG_NORMAL},
	{"forage", do_forage, POS_STANDING, 0, LOG_NORMAL},
	{"hunt", do_hunt, POS_STANDING, 0, LOG_NORMAL},
	{"report", do_report, POS_SLEEPING, 0, LOG_NORMAL},
	{"rest", do_rest, POS_MEDITATING, 0, LOG_NORMAL},
	{"v", do_v, POS_MEDITATING, 0, LOG_NORMAL},
	{"short", do_short, POS_DEAD, 0, LOG_NORMAL},
	{"sit", do_sit, POS_SITTING, 0, LOG_NORMAL},
	{"stand", do_stand, POS_SLEEPING, 0, LOG_NORMAL},
	{"tell", do_tell, POS_MEDITATING, 0, LOG_NORMAL},
	{"text", do_text, POS_MEDITATING, 0, LOG_NORMAL},
	{"snap", do_snap, POS_MEDITATING, 0, LOG_NORMAL},
	{"geist", do_geist, POS_MEDITATING, 0, LOG_NORMAL},
	{"whisper", do_whisper, POS_SITTING, 0, LOG_NORMAL},
	{"wield", do_wear, POS_SITTING, 0, LOG_NORMAL},
	{"wizhelp", do_wizhelp, POS_DEAD, 6, LOG_NORMAL},
	{"wizlist", do_wizlist, POS_DEAD, 0, LOG_NORMAL},
	{"motd", do_motd, POS_DEAD, 0, LOG_NORMAL},
	{"index", do_index, POS_DEAD, 0, LOG_NORMAL},
	{"werestats", do_werestats, POS_DEAD, 0, LOG_NORMAL},
	{"freehold", do_freehold, POS_DEAD, 0, LOG_NORMAL},
	{"chanstats", do_chanstats, POS_DEAD, 0, LOG_NORMAL},
	{"seemingbonus", do_seemingbonus, POS_DEAD, 0, LOG_NORMAL},
	{"version", do_version, POS_DEAD, 0, LOG_NORMAL},
	{"areas", do_areas, POS_DEAD, 0, LOG_NORMAL},
	{"credits", do_credits, POS_DEAD, 0, LOG_NORMAL},
	{"doing", do_doing, POS_DEAD, 0, LOG_NORMAL},
	{"action", do_doing, POS_DEAD, 0, LOG_NORMAL},
	{"showsocial", do_showsocial, POS_DEAD, 0, LOG_NORMAL},
	{"eat", do_eat, POS_DEAD, 0, LOG_NORMAL},
	{"drink", do_drink, POS_DEAD, 0, LOG_NORMAL},
	{"empty", do_empty, POS_DEAD, 0, LOG_NORMAL},
	{"equipment", do_equipment, POS_DEAD, 0, LOG_NORMAL},
	{"examine", do_examine, POS_SITTING, 0, LOG_NORMAL},
	{"help", do_help, POS_DEAD, 0, LOG_NORMAL},
	{"score", do_score, POS_DEAD, 0, LOG_NORMAL},
	{"abilities", do_abilities, POS_DEAD, 0, LOG_NORMAL},
	{"socials", do_socials, POS_DEAD, 0, LOG_NORMAL},
	{"time", do_weather, POS_DEAD, 0, LOG_NORMAL},
	{"apply", do_apply, POS_DEAD, 0, LOG_NORMAL},
//	{"vote", do_vote, POS_DEAD, 0, LOG_ALWAYS},
	{"wash", do_wash, POS_SITTING, 0, LOG_NORMAL},
	{"worth", do_worth, POS_SITTING, 0, LOG_NORMAL},
	{"who", do_who, POS_DEAD, 0, LOG_NORMAL},
	{"shout", do_shout, POS_SLEEPING, 0, LOG_NORMAL},
	{"xemote", do_xemote, POS_SITTING, 1, LOG_NORMAL },
    { "yell",		do_yell,	POS_SITTING,	 0,  LOG_NORMAL	},


	/*
	 * Configuration commands.
	 */

	{"afk", do_afk, POS_SITTING, 1, LOG_NORMAL},
	{"channels", do_channels, POS_DEAD, 0, LOG_NORMAL},
	{"config", do_config, POS_DEAD, 0, LOG_NORMAL},
	{"description", do_description, POS_DEAD, 0, LOG_NORMAL},
	{"urhan", do_urhandesc, POS_DEAD, 0, LOG_NORMAL},
	{"feline", do_urhandesc, POS_DEAD, 0, LOG_NORMAL},
	{"gauru", do_gaurudesc, POS_DEAD, 0, LOG_NORMAL},
	{"dalu", do_daludesc, POS_DEAD, 0, LOG_NORMAL},
	{"sokoto", do_daludesc, POS_DEAD, 0, LOG_NORMAL},
	{"urshul", do_urshuldesc, POS_DEAD, 0, LOG_NORMAL},
	{"delete", do_delete, POS_DEAD, 2, LOG_ALWAYS},
	{"password", do_password, POS_DEAD, 0, LOG_NEVER},
	{"title", do_title, POS_DEAD, 0, LOG_NORMAL},
	{"pretitle", do_pretitle, POS_DEAD, 0, LOG_ALWAYS},
	{"tempdesc", do_tempdesc, POS_DEAD, 0, LOG_NORMAL},
	{"biography", do_biography, POS_DEAD, 0, LOG_NORMAL},
	{"mlongdesc", do_mlongdesc, POS_DEAD, 0, LOG_NORMAL},
	{"ansi", do_ansi, POS_DEAD, 0, LOG_NORMAL},
	{"autoexit", do_autoexit, POS_DEAD, 0, LOG_NORMAL},
	{"blank", do_blank, POS_DEAD, 0, LOG_NORMAL},
	{"brief", do_brief, POS_DEAD, 0, LOG_NORMAL},
	{"breed", do_breed, POS_DEAD, 0, LOG_NORMAL},
	
	// CharGen Commands
	{"clan", do_clan, POS_DEAD, 0, LOG_ALWAYS},
	{"covenant", do_covenant, POS_DEAD, 0, LOG_NORMAL},
	{"dirge", do_dirge, POS_DEAD, 0, LOG_ALWAYS},
	{"mask", do_mask, POS_DEAD, 0, LOG_ALWAYS},
	{"template", do_template, POS_DEAD, 0, LOG_ALWAYS},
	{"vice", do_vice, POS_DEAD, 0, LOG_ALWAYS},
	{"virtue", do_virtue, POS_DEAD, 0, LOG_ALWAYS},
	
//	{"cprompt", do_cprompt, POS_DEAD, 0, LOG_NORMAL},
//	{"prompt", do_prompt, POS_DEAD, 0, LOG_NORMAL},

	/*
	 * Communication commands.
	 */
	{"emote", do_emote, POS_SITTING, 0, LOG_NORMAL},
	{"semote", do_semote, POS_SITTING, 0, LOG_NORMAL},
	{"rpose", do_rpose, POS_SITTING, 0, LOG_NORMAL},
	{"rpecho", do_rpecho, POS_SITTING, 0, LOG_NORMAL},
	{",", do_xemote, POS_SITTING, 0, LOG_NORMAL},
	{"email", do_email, POS_DEAD, 0, LOG_NORMAL},
	{"site", do_email, POS_DEAD, 0, LOG_NORMAL},
	{"note", do_note, POS_DEAD, 0, LOG_NORMAL},
	{"grant", do_grant, POS_DEAD, 7, LOG_ALWAYS},
	{"ooc", do_ooc, POS_DEAD, 0, LOG_NORMAL},
	{"advice", do_advice, POS_DEAD, 0, LOG_NORMAL},
	{"hint", do_hint, POS_DEAD, 0, LOG_NORMAL},
    {"omote", do_omote, POS_DEAD, 0, LOG_NORMAL},
	{"osay", do_osay, POS_DEAD, 0, LOG_NORMAL},
	{"pose", do_emote, POS_SITTING, 0, LOG_NORMAL},
	{"reply", do_reply, POS_MEDITATING, 0, LOG_NORMAL},
	{"rgeist", do_rgeist, POS_MEDITATING, 0, LOG_NORMAL},
	{"say", do_say, POS_MEDITATING, 0, LOG_NORMAL},
	{"speak", do_speak, POS_MEDITATING, 0, LOG_NORMAL},
	{"'", do_say, POS_MEDITATING, 0, LOG_NORMAL},
	{"story", do_story, POS_DEAD, 0, LOG_NORMAL},
	{"staff", do_story, POS_DEAD, 0, LOG_NORMAL},
	{"smote", do_smote, POS_DEAD, 0, LOG_NORMAL},
	{"request", do_storyinform, POS_DEAD, 0, LOG_NORMAL},
	{"board", do_board, POS_DEAD, 0, LOG_NORMAL},
	//Cell phone commands
	{"call", do_call, POS_DEAD, 0, LOG_NORMAL},
	{"answer", do_answer, POS_DEAD, 0, LOG_NORMAL},
	{"hangup", do_hangup, POS_DEAD, 0, LOG_NORMAL},
	{"cell", do_cell, POS_DEAD, 0, LOG_NORMAL},
	{"ringtone", do_ringtone, POS_DEAD, 0, LOG_NORMAL},
	// Dream code commands
	{"dream", do_dream, POS_DEAD, 0, LOG_NORMAL},
	{"dreamweave", do_dreamweave, POS_DEAD, 0, LOG_ALWAYS},
	{"slumber", do_slumber, POS_DEAD, 0, LOG_NORMAL},
	{"sever", do_sever, POS_DEAD, 0, LOG_NORMAL},
	//Howl command
	{"howl", do_howl, POS_DEAD, 0, LOG_NORMAL},
	//end
	/*
	 * Object manipulation commands.
	 */
	{"close", do_close, POS_SITTING, 0, LOG_NORMAL},
	{"draw", do_draw, POS_FIGHTING, 0, LOG_NORMAL},
	{"drop", do_drop, POS_SITTING, 0, LOG_NORMAL},
	{"give", do_give, POS_SITTING, 0, LOG_NORMAL},
	{"hold", do_wear, POS_SITTING, 0, LOG_NORMAL},
	{"languagelearn", do_languagelearn, POS_MEDITATING, 0, LOG_NORMAL},
	{"lock", do_lock, POS_SITTING, 0, LOG_NORMAL},

	{"open", do_open, POS_SITTING, 0, LOG_NORMAL},
	{"put", do_put, POS_SITTING, 0, LOG_NORMAL},
	{"remove", do_remove, POS_SITTING, 0, LOG_NORMAL},
	{"sheath", do_sheath, POS_STANDING, 0, LOG_NORMAL},
	{"pocket", do_sheath, POS_STANDING, 0, LOG_NORMAL},
	{"take", do_get, POS_SITTING, 0, LOG_NORMAL},
	// Books
	{"read", do_read, POS_MEDITATING, 0, LOG_NORMAL},
	{"write", do_write, POS_MEDITATING, 0, LOG_NORMAL},
	{"turn", do_turn, POS_MEDITATING, 0, LOG_NORMAL},
	{"insert", do_insertpage, POS_MEDITATING, 0, LOG_NORMAL},
	{"tear", do_tearpage, POS_MEDITATING, 0, LOG_NORMAL},
	// End books
	{"sacrifice", do_sacrifice, POS_SITTING, 0, LOG_NORMAL},
	{"junk", do_sacrifice, POS_SITTING, 0, LOG_NORMAL},
	{"unlock", do_unlock, POS_SITTING, 0, LOG_NORMAL},
	{"wear", do_wear, POS_SITTING, 0, LOG_NORMAL},
	{"weather", do_weather, POS_SITTING, 0, LOG_NORMAL},
	 /*
	  * Recommend commands.
	  */
	{"recommend", do_recommend, POS_DEAD, 0, LOG_NORMAL},
	//{"rec", do_recommend, POS_DEAD, 0, LOG_NORMAL},
    {"renown", do_renown, POS_DEAD, 0, LOG_NORMAL},
    /* Lumi Code */
	{"aura", do_aura, POS_SITTING, 0, LOG_NORMAL},
	{"voice", do_voice, POS_SITTING, 0, LOG_NORMAL},
	{"roomdesc", do_roomdesc, POS_SITTING, 0, LOG_NORMAL},
	{"miendesc", do_miendesc, POS_SITTING, 0, LOG_NORMAL},
	{"whererp", do_whererp, POS_SITTING, 0, LOG_NORMAL},
	{"rpvis", do_rpvis, POS_SITTING, 0, LOG_NORMAL},
    /* Finger commands testing */
    {"surname", do_surname, POS_SITTING, 0, LOG_ALWAYS},
    {"wiki", do_wiki, POS_SITTING, 0, LOG_NORMAL},
    {"pref", do_pref, POS_SITTING, 0, LOG_NORMAL},
    {"reputation", do_reputation, POS_SITTING, 0, LOG_NORMAL},
    /* End finger commands */
	/*
	 * Miscellaneous commands.
	 */
	{"finger", do_finger, POS_SITTING, 0, LOG_NORMAL},
	{"follow", do_follow, POS_SITTING, 0, LOG_NORMAL},
//	{"home", do_home, POS_STANDING, 0, LOG_NORMAL},
//	{"lick", do_lick, POS_MEDITATING, 0, LOG_NORMAL},
	{"qui", do_qui, POS_DEAD, 0, LOG_NORMAL},
	{"quit", do_quit, POS_SLEEPING, 0, LOG_NORMAL},
	{"save", do_save, POS_DEAD, 0, LOG_NORMAL},
//	{"slit", do_slit, POS_RESTING, 0, LOG_NORMAL},
	{"slot", do_slot, POS_RESTING, 0, LOG_NORMAL},
	{"train", do_train, POS_SLEEPING, 0, LOG_ALWAYS},
//	{"buymerit", do_buymerit, POS_SLEEPING, 0, LOG_ALWAYS},
	{"wake", do_wake, POS_SLEEPING, 0, LOG_NORMAL},
	/* Remember and Forget */
	{"remember", do_remember, POS_SITTING, 0, LOG_NORMAL},
	{"forget", do_forget, POS_SITTING, 0, LOG_NORMAL},
	{"merits", do_merits, POS_STANDING, 0, LOG_NORMAL},

	/*
	 * Vampire and Changeling commands.
	 */
	{"accept", do_accept, POS_STANDING,	0, LOG_NORMAL},
	{"affects", do_affects, POS_STANDING,	0, LOG_NORMAL},
	{"skinmask", do_skinmask, POS_STANDING, 3, LOG_NORMAL},
	{"bloodlinedisc", do_bloodlinedisc, POS_STANDING, 3, LOG_ALWAYS},
	{"bloodlinename", do_bloodlinename, POS_STANDING, 10, LOG_ALWAYS},
	{"breakup", do_breakup,	POS_STANDING, 0, LOG_NORMAL},
	{"immcovenant", do_immcovenant, POS_STANDING, 10, LOG_ALWAYS},
	{"kithname", do_kithname, POS_STANDING, 10, LOG_ALWAYS},
	{"auspice", do_auspice, POS_STANDING, 10, LOG_ALWAYS},
	{"courtname", do_courtname, POS_STANDING, 10, LOG_ALWAYS},
	{"discipline", do_discipline, POS_DEAD, 3, LOG_NORMAL},
	{"embrace", do_embrace, POS_STANDING, 3, LOG_NORMAL},
	{"harvest", do_embrace, POS_STANDING, 3, LOG_NORMAL},
	{"restring", do_restring, POS_STANDING, 3, LOG_NORMAL},
	{"tool", do_restring, POS_STANDING, 3, LOG_NORMAL},
//	{"fangs", do_fangs, POS_SITTING, 3, LOG_NORMAL},
	{"ifavour", do_ifavour, POS_SITTING, 7, LOG_NORMAL},
	{"favour", do_favour, POS_SITTING, 3, LOG_NORMAL},
//	{"feed", do_feed, POS_SITTING, 3, LOG_NORMAL},
	{"heightenedsenses", do_truesight, POS_SITTING, 3, LOG_ALWAYS},
//	{"clan", do_clan, POS_STANDING, 3, LOG_NORMAL},
	{"clanclear", do_clanclear, POS_STANDING, 7, LOG_NORMAL},
	{"covenant", do_side, POS_STANDING, 3, LOG_NORMAL},
	{"organization", do_kithname, POS_STANDING, 3, LOG_NORMAL},
	{"auspic", do_auspice, POS_STANDING, 3, LOG_NORMAL},
	{"shift", do_shift, POS_STANDING, 3, LOG_ALWAYS},
	{"side", do_side, POS_STANDING, 3, LOG_NORMAL},
	{"teachdisc", do_teachdisc, POS_STANDING, 3, LOG_ALWAYS},
	{"umbra", do_shadowplane, POS_STANDING, 3, LOG_NORMAL},
	{"shadowplane", do_shadowplane, POS_STANDING, 3, LOG_NORMAL},
	/*
	 * Mortal storytelling commands
	 */
	 {"pdice", do_pdice, POS_STANDING, 0, LOG_NORMAL},
	 {"burn", do_burn, POS_DEAD, 0, LOG_ALWAYS},
	 {"regain", do_regain, POS_DEAD, 6, LOG_ALWAYS},
	 {"damage", do_set_damage, POS_DEAD, 0, LOG_ALWAYS},
	 
	 /* Navigation Commands */
//   {"recall", do_recall, POS_DEAD, 0, LOG_NORMAL},
     {"gate", do_gate, POS_DEAD, 0, LOG_NORMAL},
     {"market", do_market, POS_DEAD, 0, LOG_NORMAL},
     {"escape", do_escape, POS_DEAD, 0, LOG_NORMAL},
//   {"gmarket", do_gmarket, POS_DEAD, 0, LOG_NORMAL},
//   {"quarter", do_quarter, POS_DEAD, 0, LOG_NORMAL},
//   {"garden", do_garden, POS_DEAD, 0, LOG_NORMAL},
//   {"gohome", do_gohome, POS_DEAD, 0, LOG_NORMAL},

	/*
	 * Immortal commands.
	 */
	{"prestore", do_prestore, POS_DEAD, 12, LOG_ALWAYS},
	{"eban", do_eban, POS_DEAD, 11, LOG_ALWAYS},
	{"eallow", do_eallow, POS_DEAD, 11, LOG_ALWAYS},
	{"allow", do_allow, POS_DEAD, 11, LOG_ALWAYS},
    {"areset", do_areset, POS_DEAD, 6, LOG_ALWAYS},
    {"nuketell", do_nuketell, POS_DEAD, 6, LOG_ALWAYS},
	{"ban", do_ban, POS_DEAD, 11, LOG_ALWAYS},
	{"clearstats", do_clearstats, POS_STANDING, 0, LOG_NORMAL},
	{"fixcharacter", do_fixcharacter, POS_STANDING, 7, LOG_NORMAL},
	{"clearclas", do_clearclas, POS_STANDING, 1, LOG_NORMAL},
	{"clearclass", do_clearclass, POS_STANDING, 11, LOG_ALWAYS},
	{"motlist", do_clist, POS_DEAD, 6, LOG_NORMAL},
	{"copyover", do_copyover, POS_DEAD, 11, LOG_ALWAYS},
	{"create", do_create, POS_STANDING, 6, LOG_NORMAL},
	{"got", do_got, POS_STANDING, 6, LOG_NORMAL},
	{"deny", do_deny, POS_DEAD, LEVEL_STORYTELLER, LOG_ALWAYS},
	{"dice", do_dice, POS_DEAD, 6, LOG_NORMAL},
	{"disable", do_disable, POS_DEAD, 11, LOG_ALWAYS},
	{"disconnect", do_disconnect, POS_DEAD, 10, LOG_ALWAYS},
	{"divorce", do_divorce,	POS_DEAD, 9, LOG_ALWAYS	},
	{"dnd", do_dnd, POS_DEAD, 12, LOG_NEVER},
	{"exlist", do_exlist, POS_DEAD, 7, LOG_NORMAL},
	{"freeze", do_freeze, POS_DEAD, 6, LOG_ALWAYS},
	{"marry", do_marry,	POS_DEAD, 9, LOG_ALWAYS},
	{"info", do_info, POS_DEAD, 9, LOG_NORMAL},
	{"givepower", do_givepower, POS_DEAD, 6, LOG_ALWAYS},
//	{"givevote", do_givevote, POS_DEAD, 6, LOG_ALWAYS},
	{"hedit", do_hedit, POS_DEAD, 7, LOG_ALWAYS},
	{"noquit", do_noquit, POS_DEAD, 6, LOG_ALWAYS},
	{"nuke", do_nuke, POS_DEAD, 12, LOG_ALWAYS},
	{"pstat", do_pstat, POS_DEAD, 6, LOG_ALWAYS},
	{"qset", do_qset, POS_DEAD, 8, LOG_ALWAYS},
	{"qstat", do_qstat, POS_DEAD, 8, LOG_ALWAYS},
	{"reboo", do_reboo, POS_DEAD, 11, LOG_NORMAL},
	{"reboot", do_reboot, POS_DEAD, 11, LOG_ALWAYS},
	{"mudinfo", do_mudinfo, POS_DEAD, 6, LOG_ALWAYS},
	{"removepower", do_removepower, POS_DEAD, 6, LOG_ALWAYS},
	{"shutdow", do_shutdow, POS_DEAD, 11, LOG_NORMAL},
	{"shutdown", do_shutdown, POS_DEAD, 11, LOG_ALWAYS},
	{"string", do_string, POS_DEAD, LEVEL_STORYTELLER, LOG_ALWAYS},
	{"socket", do_users, POS_DEAD, 12, LOG_NORMAL},
	{"use", do_use, POS_DEAD, 0, LOG_NORMAL},
	{"stat", do_omni, POS_DEAD, 6, LOG_NORMAL},
	{"wizlock", do_wizlock, POS_DEAD, 11, LOG_ALWAYS},
	{"wset", do_wset, POS_DEAD, 9, LOG_ALWAYS},
	{"force", do_force, POS_DEAD, 6, LOG_ALWAYS},
	{"mclear", do_mclear, POS_DEAD, 9, LOG_ALWAYS},
	{"mload", do_mload, POS_DEAD, 8, LOG_ALWAYS},
	{"mset", do_mset, POS_DEAD, 6, LOG_ALWAYS},
	{"nice", do_nice, POS_DEAD, 10, LOG_ALWAYS},
	{"noemote", do_noemote, POS_DEAD, 9, LOG_NORMAL},
	{"notell", do_notell, POS_DEAD, 9, LOG_NORMAL},
	{"oclone", do_oclone, POS_DEAD, 8, LOG_ALWAYS},
	{"oload", do_oload, POS_DEAD, 5, LOG_ALWAYS},
	{"oset", do_oset, POS_DEAD, 7, LOG_ALWAYS},
	{"otransfer", do_otransfer, POS_DEAD, 6, LOG_ALWAYS},
	{"pload", do_pload, POS_DEAD, 0, LOG_ALWAYS},
	{"pset", do_pset, POS_DEAD, 10, LOG_ALWAYS}, 
	{"class", do_class, POS_DEAD, 7, LOG_NORMAL},
	// Changes
    {"changes", do_changes, POS_DEAD, 1, LOG_NORMAL},
    {"delchange", do_delchange, POS_DEAD, 7, LOG_NORMAL},
    {"addchange", do_addchange, POS_DEAD, 7, LOG_NORMAL},
	// End changes
	{"pathclear", do_pathclear, POS_STANDING, 10, LOG_NORMAL},
	{"protect", do_protect, POS_DEAD, 12, LOG_ALWAYS},
	{"purge", do_purge, POS_DEAD, 7, LOG_NORMAL},
	{"relevel", do_relevel, POS_DEAD, 1, LOG_NEVER},
	{"grestore", do_brestore, POS_DEAD, 6, LOG_ALWAYS},
	{"rset", do_rset, POS_DEAD, 7, LOG_ALWAYS},
	{"reward", do_reward, POS_DEAD, 6, LOG_ALWAYS},
	{"silence", do_silence, POS_DEAD, 9, LOG_ALWAYS},
	{"sla", do_sla, POS_DEAD, 10, LOG_NORMAL},
	{"slay", do_slay, POS_DEAD, 10, LOG_ALWAYS},
	{"smite", do_smite, POS_STANDING, 6, LOG_NORMAL},
	{"special", do_special, POS_DEAD, 10, LOG_ALWAYS},
	{"sset", do_sset, POS_DEAD, 10, LOG_ALWAYS},
	{"immvirtue", do_immvirtue, POS_DEAD, 6, LOG_NORMAL},
	{"immvice", do_immvice, POS_DEAD, 6, LOG_NORMAL},
	{"lodge", do_setLodge, POS_DEAD, 6, LOG_NORMAL},
	{"impulse", do_impulse, POS_DEAD, 6, LOG_NORMAL},
	{"wyrd", do_potency, POS_DEAD, 6, LOG_NORMAL},
	/* Merits/Specs Stuff */
	{"givemerit", do_give_merit, POS_DEAD, 6, LOG_ALWAYS},
	{"remmerit", do_rem_merit, POS_DEAD, 6, LOG_ALWAYS},
	{"givespec", do_give_spec, POS_DEAD, 6, LOG_ALWAYS},
	{"remspec", do_rem_spec, POS_DEAD, 6, LOG_ALWAYS},
	{"givestyle", do_give_style, POS_DEAD, 6, LOG_ALWAYS},
	{"remstyle", do_rem_style, POS_DEAD, 6, LOG_ALWAYS},
	{"giveenote", do_give_enote, POS_DEAD, 6, LOG_ALWAYS},
	{"remenote", do_rem_enote, POS_DEAD, 6, LOG_ALWAYS},
	{"givepnote", do_give_pnote, POS_DEAD, 6, LOG_ALWAYS},
	{"rempnote", do_rem_pnote, POS_DEAD, 6, LOG_ALWAYS},
	/* End Merits/Specs Stuff */
	{"fupdate", do_force_char_update, POS_DEAD, 11, LOG_ALWAYS},
	{"rekarma", do_reseed, POS_DEAD, 11, LOG_NORMAL},
	{"transfer", do_transfer, POS_DEAD, 6, LOG_ALWAYS},
	{"at", do_at, POS_DEAD, 6, LOG_NORMAL},
	{"bamfin", do_bamfin, POS_DEAD, 6, LOG_NORMAL},
	{"bamfout", do_bamfout, POS_DEAD, 6, LOG_NORMAL},
	{"echo", do_echo, POS_DEAD, 6, LOG_ALWAYS},
	{"goto", do_goto, POS_DEAD, 6, LOG_NORMAL},
	{"holylight", do_holylight, POS_DEAD, 6, LOG_NORMAL},
	{"invis", do_invis, POS_DEAD, 6, LOG_NORMAL},
	{"incog", do_incog, POS_DEAD, 6, LOG_NORMAL},
	{"log", do_log, POS_DEAD, 9, LOG_ALWAYS},
	{"roleplay", do_rpflag, POS_MEDITATING, 0, LOG_ALWAYS},
	{"status", do_status, POS_DEAD, 0, LOG_ALWAYS},
	{"login", do_login, POS_DEAD, 9, LOG_NORMAL},
	{"memory", do_memory_list, POS_DEAD, 0, LOG_NORMAL},
	{"memreport", do_memory, POS_DEAD, 7, LOG_NORMAL},
	{"mfind", do_mfind, POS_DEAD, 7, LOG_NORMAL},
	{"mpresets", do_mpresets, POS_DEAD, 7, LOG_NORMAL},
	{"move", do_move, POS_STANDING, 1, LOG_NORMAL},
	{"mstat", do_mstat, POS_DEAD, 6, LOG_NORMAL},
	{"mwhere", do_mwhere, POS_DEAD, 8, LOG_NORMAL},
	{"mlist", do_mlist, POS_DEAD, 6, LOG_NORMAL},
	{"olist", do_olist, POS_DEAD, 6, LOG_NORMAL},
	{"rlist", do_rlist, POS_DEAD, 6, LOG_NORMAL},
	{"fvlist", do_fvlist, POS_DEAD, 6, LOG_NORMAL},
	{"ofind", do_ofind, POS_DEAD, 5, LOG_NORMAL},
	{"ostat", do_ostat, POS_DEAD, 6, LOG_NORMAL},
	{"propose",	do_propose,	POS_STANDING, 0, LOG_NORMAL},
	{"return", do_return, POS_DEAD, 6, LOG_NORMAL},
	{"rstat", do_rstat, POS_DEAD, 7, LOG_NORMAL},
	{"snoop", do_snoop, POS_DEAD, 8, LOG_ALWAYS},
	{"switch", do_switch, POS_DEAD, 6, LOG_ALWAYS},
	{"uptime", do_uptime, POS_DEAD, 12, LOG_NORMAL},
	{"consent", do_vampire, POS_STANDING, 0, LOG_NORMAL},
	{"vampire", do_vampire, POS_STANDING, 0, LOG_NORMAL},

	{"immtalk", do_immtalk, POS_DEAD, 8, LOG_NORMAL},
	{":", do_immtalk, POS_DEAD, 8, LOG_NORMAL},

	{"roomflags", do_roomflags, POS_SITTING, 6, LOG_NORMAL},
	{"vision", do_vision, POS_SITTING, 7, LOG_NORMAL},

	/*
	 * Online creation commands.
	 */
	{"aedit", do_aedit, POS_DEAD, 10, LOG_NORMAL},
	{"redit", do_redit, POS_DEAD, 7, LOG_NORMAL},
	{"oedit", do_oedit, POS_DEAD, 7, LOG_NORMAL},
	{"medit", do_medit, POS_DEAD, 7, LOG_NORMAL},
	//{"mpedit", do_mpedit, POS_DEAD, 10, LOG_NORMAL},

	{"ashow", do_ashow, POS_DEAD, 7, LOG_NORMAL},
	{"rshow", do_rshow, POS_DEAD, 7, LOG_NORMAL},
	{"oshow", do_oshow, POS_DEAD, 6, LOG_NORMAL},
	{"mshow", do_mshow, POS_DEAD, 6, LOG_NORMAL},

	{"resets", do_resets, POS_DEAD, 7, LOG_NORMAL},
	{"asave", do_asave, POS_DEAD, 7, LOG_NORMAL},
	{"alist", do_arealist, POS_DEAD, 7, LOG_NORMAL},
	{"powerlevels", do_powerlevels, POS_DEAD, 6, LOG_NORMAL},
	{"discset", do_discset, POS_DEAD, 10, LOG_ALWAYS},
	{"meritset", do_meritset, POS_DEAD, 10, LOG_ALWAYS},
	{"setbreed", do_setbreed, POS_DEAD, 10, LOG_ALWAYS},
	{"setorganization", do_setorganization, POS_DEAD, 10, LOG_ALWAYS},
	{"potency", do_potency, POS_DEAD, 7, LOG_ALWAYS},
	{"pot", do_potency, POS_DEAD, 7, LOG_ALWAYS},
	
	/* New Penny Dreadful functionality */
	{"pick", do_pick, POS_DEAD, 0, LOG_ALWAYS},
	{"scan", do_scan, POS_SITTING, 0, LOG_NORMAL},
    {"spy",	do_spy,	POS_SITTING, 0, LOG_NORMAL},
	{"sneak", do_sneak,	POS_SITTING, 0, LOG_NORMAL},
	{"visible", do_visible,	POS_SITTING, 0, LOG_NORMAL},
	{"hide", do_hide, POS_SITTING, 0, LOG_NORMAL},
	
	
	// Map, written by julius with the lowercase j //
    {"legend", do_legend, POS_DEAD, 0, LOG_NORMAL}, 
    {"map", do_map, POS_SITTING, 0, LOG_NORMAL}, 
    {"garage", do_garage, POS_SITTING, 0, LOG_NORMAL}, 

	/*
	 * * MOBprogram commands.
	 */
/*	{"mpstat", do_mpstat, POS_DEAD, 38, LOG_NORMAL},
	{"mpasound", do_mpasound, POS_DEAD, 0, LOG_NORMAL},
	{"mpjunk", do_mpjunk, POS_DEAD, 0, LOG_NORMAL},
	{"mpecho", do_mpecho, POS_DEAD, 0, LOG_NORMAL},
	{"mpechoat", do_mpechoat, POS_DEAD, 0, LOG_NORMAL},
	{"mpechoaround", do_mpechoaround, POS_DEAD, 0, LOG_NORMAL},
	{"mpkill", do_mpkill, POS_DEAD, 0, LOG_NORMAL},
	{"mpmload", do_mpmload, POS_DEAD, 0, LOG_NORMAL},
	{"mpoload", do_mpoload, POS_DEAD, 0, LOG_NORMAL},
	{"mppurge", do_mppurge, POS_DEAD, 0, LOG_NORMAL},
	{"mpgoto", do_mpgoto, POS_DEAD, 0, LOG_NORMAL},
	{"mpat", do_mpat, POS_DEAD, 0, LOG_NORMAL},
	{"mptransfer", do_mptransfer, POS_DEAD, 0, LOG_NORMAL},
	{"mpforce", do_mpforce, POS_DEAD, 0, LOG_NORMAL},
        {"mpgold", do_mpgold, POS_DEAD, 0, LOG_NORMAL},
*/
		/* weird */
	{"masquerade", do_masquerade, POS_STANDING, 0, LOG_ALWAYS},
	/*
	 * End of list.
	 */
	{"", 0, POS_DEAD, 0, LOG_NORMAL}
};




/*
 * The social table.
 * Add new socials here.
 * Alphabetical order is not required.
 */
const struct social_type social_table[] = {
	

	{
	 "afk",
	 "You announce that you are going away from keyboard.",
	 "$n announces that $e is going away from keyboard.",
	 "You announce that you are going away from keyboard.",
	 "$n informs $N that $e is going away from keyboard.",
	 "$n informs you that $e is going away from keyboard.",
	 "You announce that you are going away from keyboard.",
	 "$n announces that $e is going away from keyboard.",
	 },


	{
	 "",
	 NULL, NULL, NULL, NULL, NULL, NULL, NULL}

};


/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret (CHAR_DATA * ch, char *argument)
{
/*  ROOMTEXT_DATA *rt;
    char kavirarg[MAX_INPUT_LENGTH]; */
	CHAR_DATA *vch;
	char arg[MAX_STRING_LENGTH];
	char argu[MAX_STRING_LENGTH];
	char command[MAX_STRING_LENGTH];
	char logline[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int cmd;
	int trust;
	bool found;

	if (!ch) return;
	sprintf (argu, "%s %s", arg, one_argument (argument, arg));

	/*
	 * Strip leading spaces.
	 */
	while (isspace (*argument))
		argument++;
	if (argument[0] == '\0')
		return;


	/*
	 * Implement freeze command.
	 */
	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_FREEZE))
	{
		send_to_char ("You're totally frozen!\n\r", ch);
		return;
	}
	else if (number_percent () <= 25 && IS_MORE (ch, MORE_CALM))
	{
		send_to_char ("You really can't be bothered.\n\r", ch);
		return;
	}
	else if (IS_MORE3 (ch, MORE3_DEATHSWHISPER))
	{

		if (ch->blood[BLOOD_CURRENT] > 2)
		{
			send_to_char ("You burn 2 blood to remove Deaths Whisper.\n\r", ch);
			REMOVE_BIT (ch->more3, MORE3_DEATHSWHISPER);
			act ("$n takes a reflexive breath and gets up.", ch, NULL, NULL, TO_ROOM);
		}
		else
			send_to_char ("You do not have the 2 blood to remove Deaths Whisper.\n\r", ch);

		return;
	}



	/*
	 * Grab the command word.
	 * Special parsing so ' can be a command,
	 *   also no spaces needed after punctuation.
	 */
	strcpy (logline, argument);

	if (!isalpha (argument[0]) && !isdigit (argument[0]))
	{
		command[0] = argument[0];
		command[1] = '\0';
		argument++;
		while (isspace (*argument))
			argument++;
	}
	else
	{
		argument = one_argument (argument, command);
	}

	/*
	 * Look for command in command table.
	 */
	found = FALSE;
	trust = get_trust (ch);
	for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
	{

		if (command[0] == cmd_table[cmd].name[0] && !str_prefix (command, cmd_table[cmd].name) && (cmd_table[cmd].level <= trust))
		{
		  if(IS_SET(ch->extra2, EXTRA2_RESEARCH))
		    {	
		        if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "hint"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "quit"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "research"))
					found = TRUE;
				else
				{
					send_to_char ("You must concentrate while researching.  Sit Still.\n\r", ch);
					return;
				}
			}
		      

			if (!IS_NPC (ch) && IS_SET (ch->extra2, EXTRA2_DAEMON_REGEN))
			{
				if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "quit"))
					found = TRUE;
				else
				{
					send_to_char ("You must concentrate when regenerating.  Sit Still.\n\r", ch);
					return;
				}
			}
			if (!IS_NPC (ch) && IS_SET (ch->extra2, EXTRA2_TORPORED))
			{
				if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "quit"))
					found = TRUE;
				else
				{
					send_to_char ("You are in torpor.  Sit Still.\n\r", ch);
					return;
				}
			}

			if (IS_EXTRA (ch, EXTRA_OSWITCH))
			{
				if (IS_MORE (ch, MORE_MUMMIFIED))
				{
					if (!str_cmp (cmd_table[cmd].name, "reply"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "look"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "where"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "score"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "save"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "who"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "quit"))
						found = TRUE;
					else
					{
						send_to_char ("Not while mummified!\n\r", ch);
						return;
					}
				}

				else
				{
					if (!str_cmp (cmd_table[cmd].name, "say"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "'"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "immtalk"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, ":"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "ctalk"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "look"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "save"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "exits"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "emote"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "semote"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "tell"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "reply"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "text"))
						found = TRUE;
				    else if (!str_cmp (cmd_table[cmd].name, "snap"))
					    found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "rgeist"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "geist"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "order"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "ashestoashes"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "plasmaform"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "who"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "where"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "relevel"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "safe"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "scan"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "spy"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "score"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "save"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "inventory"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "oreturn"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "roll"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "pour"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "leap"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "nightsight"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "truesight"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "horns"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "fangs"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "cast"))
						found = TRUE;
					else if (!str_cmp (cmd_table[cmd].name, "quit") && !IS_NPC (ch) && ch->pcdata->obj_vnum != 0)
						found = TRUE;
					else
					{
						if (IS_MORE (ch, MORE_STAKED) && get_disc (ch, DISC_PROTEAN) >= 5)
						{
							if (!str_cmp (cmd_table[cmd].name, "stake"))
								found = TRUE;
							else
							{
								send_to_char ("Not while staked!\n\r", ch);
								return;
							}
						}
						else
						{
							if (IS_MORE (ch, MORE_STAKED))
								send_to_char ("Not while staked!\n\r", ch);
							else
								send_to_char ("Not without a body!\n\r", ch);
							return;
						}
					}
				}
			}
			else if (IS_MORE2 (ch, MORE2_COMA))
			{
				if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immtalk"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "hint"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "inventory"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "text"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "snap"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "rgeist"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "geist"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "relevel"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "quit"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "vbloodline"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "upkeep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "affects"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immune"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "help"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "astral"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "projection"))
					found = TRUE;
				else
				{
					send_to_char ("You can't wake up...your in a coma!\n\r", ch);
					return;
				}
			}
			else if (IS_EXTRA (ch, EXTRA_NO_ACTION))
			{
				if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "astral"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "projection"))
					found = TRUE;
				else
				{
					send_to_char ("You are not in control of your body.\n\r", ch);
					if (ch->pcdata->familiar == NULL)
						REMOVE_BIT (ch->extra, EXTRA_NO_ACTION);
					return;
				}
			}
			else if (ch->embraced != ARE_NONE)
			{
				if (!str_cmp (cmd_table[cmd].name, "say"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "'"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immtalk"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, ":"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "ctalk"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "yell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "shout"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "look"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "kill"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "inventory"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "embrace"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "harvest"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "equipment"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "text"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "snap"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "rgeist"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "geist"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "relevel"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "wake"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "stand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tier"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "vbloodline"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "upkeep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "affects"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "goto"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "where"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "burn"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "flex"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "group"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "gtell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, ";"))
					found = TRUE;
				else
				{
					send_to_char ("Not while you are in an embrace.\n\r", ch);
					return;
				}
			}
			else if (IS_EXTRA (ch, TIED_UP))
			{
				if (!str_cmp (cmd_table[cmd].name, "say"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "'"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immtalk"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, ":"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "ctalk"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "yell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "shout"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "look"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "exits"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "inventory"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "text"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "snap"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "rgeist"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "geist"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "order"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "where"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "introduce"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "relevel"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "safe"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "scan"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "spy"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "sleep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "wake"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "stand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fangs"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "claws"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "nightsight"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "umbralvision"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "umbra"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "shield"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "vbloodline"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "upkeep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "affects"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immune"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "report"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "goto"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "flex"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "change"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "drink"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "astral"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "projection"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "plasmaform"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "earthmeld"))
					found = TRUE;
				else
				{
					send_to_char ("Not while tied up.\n\r", ch);
					if (ch->position > POS_STUNNED)
						act ("$n strains against $s bonds.", ch, NULL, NULL, TO_ROOM);
					return;
				}
			}
			else if (!IS_VAMPIRE (ch) && IS_VAMPAFF (ch, VAM_EARTHMELDED))
			{
				if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "inventory"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "relevel"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "safe"))
					found = TRUE;
				else if (!str_cmp( cmd_table[cmd].name, "scan" )) found = TRUE;
		        else if (!str_cmp( cmd_table[cmd].name, "spy"  )) found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "vbloodline"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "upkeep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "affects"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immune"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "earthmeld"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "burrow"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "astral"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "projection"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "help"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "hint"))
					found = TRUE;
				else
				{
					send_to_char ("You cannot do that while you are buried in the earth.\n\r", ch);
					return;
				}

			}
			else if (IS_VAMPIRE (ch) && get_disc (ch, DISC_PROTEAN) > 6 && IS_VAMPAFF (ch, VAM_EARTHMELDED))
			{
				if (!str_cmp (cmd_table[cmd].name, "north"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "east"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "south"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "west"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "down"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "up"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "inventory"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "relevel"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "safe"))
					found = TRUE;
				else if (!str_cmp( cmd_table[cmd].name, "scan" )) found = TRUE;
		        else if (!str_cmp( cmd_table[cmd].name, "spy"  )) found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "vbloodline"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "upkeep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "affects"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immune"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "earthmeld"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "burrow"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "astral"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "projection"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "help"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;				
				else if (!str_cmp (cmd_table[cmd].name, "hint"))
					found = TRUE;
				else
				{
					send_to_char ("You cannot do that while you are buried in the earth.\n\r", ch);
					return;
				}
			}
			else if (IS_VAMPIRE (ch) && get_disc (ch, DISC_PROTEAN) < 6 && IS_VAMPAFF (ch, VAM_EARTHMELDED))
			{
				if (!str_cmp (cmd_table[cmd].name, "save"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "inventory"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "who"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "relevel"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "safe"))
					found = TRUE;
				else if (!str_cmp( cmd_table[cmd].name, "scan" )) found = TRUE;
		        else if (!str_cmp( cmd_table[cmd].name, "spy"  )) found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "vbloodline"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "upkeep"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "score"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "affects"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "immune"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "earthmeld"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "burrow"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "regenerate"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "["))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "astral"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "projection"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "fcommand"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "help"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "tell"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "reply"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "ooc"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "advice"))
					found = TRUE;
				else if (!str_cmp (cmd_table[cmd].name, "hint"))
					found = TRUE;

				else
				{
					send_to_char ("You cannot do that while you are buried in the earth.\n\r", ch);
					return;
				}
			}
			found = TRUE;
			break;
		}
	}

	/*
	 * Log and snoop.
	 */

	if (((!IS_NPC (ch) && IS_SET (ch->act, PLR_LOG)) || fLogAll || cmd_table[cmd].log == LOG_ALWAYS) || 
		(cmd_table[cmd].log == LOG_IFIMM && IS_IMMORTAL(ch)))
	{
		if (cmd_table[cmd].log == LOG_NEVER)
			strcpy (logline, "(Omitted)");
		else
		{
			sprintf (log_buf, "%s: %s", ch->name, logline);
			log_string (log_buf, CHANNEL_LOG_NORMAL);
		}
	}
	else
	{
		//sprintf( log_buf, "%s: %s", ch->name, logline);
		//log_string_quiet (log_buf, CHANNEL_LOG_NORMAL);
	}

	if (ch->desc != NULL && ch->desc->snoop_by != NULL)
	{
		if ((vch = ch->desc->snoop_by->character) == NULL)
			ch->desc->snoop_by = NULL;
		else if (IS_IMMORTAL (vch))
		{
			write_to_buffer (ch->desc->snoop_by, "% ", 2);
			write_to_buffer (ch->desc->snoop_by, logline, 0);
			write_to_buffer (ch->desc->snoop_by, "\n\r", 2);
		}
		else if (ch->in_room == NULL || vch->in_room == NULL)
		{
			sprintf (buf, "You loose your mental link with %s.\n\r", ch->name);
			send_to_char (buf, vch);
			bug ("Auspex power 4 in-room bug.", 0);
			ch->desc->snoop_by = NULL;
		}
		else if (ch->in_room != vch->in_room)
		{
			sprintf (buf, "You loose your mental link with %s.\n\r", ch->name);
			send_to_char (buf, vch);
			ch->desc->snoop_by = NULL;
		}
		else
		{
			sprintf (buf, "%s thinks: %s.\n\r", ch->name, logline);
			send_to_char (buf, vch);
		}
	}

	if (!found)
	{
		/*
		 * Look for command in socials table.
		 */
		if (!check_social (ch, command, argument))
		{
			send_to_char ("#eHuh??#n\n\r", ch);
	        }
		return;
	}
	else /* a normal valid command.. check if it's disabled */ if (check_disabled (&cmd_table[cmd]))
	{
		send_to_char ("#eHuh??#n\n\r", ch);
		return;
	}
	if (IS_SET (ch->extra2, EXTRA2_AFK))
	{
		if (str_cmp (cmd_table[cmd].name, "afk"))
		{
			send_to_char ("#yWelcome back.#n\n\r", ch);
			REMOVE_BIT (ch->extra2, EXTRA2_AFK);
		}
	}

	/*
	 * Character not in position for command?
	 */
	if (ch->position < cmd_table[cmd].position && !IS_NPC (ch) && IS_VAMPIRE (ch) && get_disc (ch, DISC_PROTEAN) >= 5)
	{
		bool stop_now = TRUE;
		switch (ch->position)
		{
		case POS_DEAD:
			send_to_char ("Lie still; you are DEAD.\n\r", ch);
			break;

		case POS_MORTAL:
		case POS_INCAP:
			stop_now = FALSE;
			WAIT_STATE (ch, 12);
			break;

		case POS_STUNNED:
			stop_now = FALSE;
			WAIT_STATE (ch, 12);
			break;

		case POS_MEDITATING:
		case POS_SITTING:
		case POS_RESTING:
			send_to_char ("Nah... You feel too relaxed...\n\r", ch);
			break;
		}
		if (stop_now)
			return;
	}
	else if (ch->position < cmd_table[cmd].position)
	{
		switch (ch->position)
		{
		case POS_DEAD:
			send_to_char ("Lie still; you are DEAD.\n\r", ch);
			break;

		case POS_MORTAL:
		case POS_INCAP:
			send_to_char ("You are hurt far too bad for that.\n\r", ch);
			break;

		case POS_STUNNED:
			send_to_char ("You are too stunned to do that.\n\r", ch);
			break;

		case POS_MEDITATING:
		case POS_SITTING:
		case POS_RESTING:
			send_to_char ("Nah... You feel too relaxed...\n\r", ch);
			break;
		}
		return;
	}

	/*
	 * Dispatch the command.
	 */

	(*cmd_table[cmd].do_fun) (ch, argument);
	tail_chain ();

	if (last_command != NULL)
		free_string (last_command);
	sprintf (log_buf, "%s %s BY %s", cmd_table[cmd].name, argument, ch->name);
	last_command = str_dup (log_buf);

	return;
}



bool check_social (CHAR_DATA * ch, char *command, char *argument)
{
	char arg[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	int cmd;
	bool found;

	found = FALSE;
	for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
	{
		if (command[0] == social_table[cmd].name[0] && !str_prefix (command, social_table[cmd].name))
		{
			found = TRUE;
			break;
		}
	}

	if (!found)
		return FALSE;

	if (!IS_NPC (ch) && IS_SET (ch->act, PLR_NO_EMOTE))
	{
		send_to_char ("You are anti-social!\n\r", ch);
		return TRUE;
	}
	if (IS_NPC (ch) && IS_SET (ch->act, ACT_NOPARTS))
	{
		send_to_char ("You are unable to use socials!\n\r", ch);
		return TRUE;
	}

	if (!IS_NPC (ch) && IS_VAMPAFF (ch, VAM_EARTHMELDED))
	{
		send_to_char ("You cannot socialise while earthmelded!\n\r", ch);
		return TRUE;
	}

	if (!IS_NPC (ch) && get_disc (ch, DISC_PROTEAN) >= 5 && ch->position <= POS_SLEEPING)
	{
		send_to_char ("You use Movement of the Slowed body.\n\r", ch);
		WAIT_STATE (ch, 12);
	}
	else
		switch (ch->position)
		{
		case POS_DEAD:
			send_to_char ("Lie still; you are DEAD.\n\r", ch);
			return TRUE;

		case POS_INCAP:
		case POS_MORTAL:
			send_to_char ("You are hurt far too bad for that.\n\r", ch);
			return TRUE;

		case POS_STUNNED:
			send_to_char ("You are too stunned to do that.\n\r", ch);
			return TRUE;
		}

	one_argument (argument, arg);
	victim = NULL;

	if (arg[0] == '\0')
	{
		act (social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM);
		act (social_table[cmd].char_no_arg, ch, NULL, victim, TO_CHAR);
	}
	else if ((victim = get_char_room (ch, arg)) == NULL)
	{
		send_to_char ("They aren't here.\n\r", ch);
	}
	else if (victim == ch)
	{
		act (social_table[cmd].others_auto, ch, NULL, victim, TO_ROOM);
		act (social_table[cmd].char_auto, ch, NULL, victim, TO_CHAR);
	}
	else
	{
		act (social_table[cmd].others_found, ch, NULL, victim, TO_NOTVICT);
		act (social_table[cmd].char_found, ch, NULL, victim, TO_CHAR);
		act (social_table[cmd].vict_found, ch, NULL, victim, TO_VICT);

		if (!IS_NPC (ch) && IS_NPC (victim) && !IS_AFFECTED (victim, AFF_CHARM) && !IS_EXTRA (victim, EXTRA_ILLUSION) && !IS_ITEMAFF (ch, ITEMA_PEACE) && can_see (victim, ch) && IS_AWAKE (victim))
		{
			if (IS_SET (victim->act, ACT_DOG))
				switch (number_range (1, 20))
				{
				default:
					break;
				case 0:
					act ("$n licks $N's hand.", victim, NULL, ch, TO_NOTVICT);
					act ("You lick $N's hand.", victim, NULL, ch, TO_CHAR);
					act ("$n licks you hand.", victim, NULL, ch, TO_VICT);
					break;
				case 1:
					act ("$n growls playfully at $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You growl playfully at $N.", victim, NULL, ch, TO_CHAR);
					act ("$n growls playfully at you.", victim, NULL, ch, TO_VICT);
					break;
				case 2:
					act ("$n yelps loudly at $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You yelp loudly at $N.", victim, NULL, ch, TO_CHAR);
					act ("$n yelps loudly at you.", victim, NULL, ch, TO_VICT);
					break;
				case 3:
					act ("$n sniffs $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You sniff $N.", victim, NULL, ch, TO_CHAR);
					act ("$n sniffs you.", victim, NULL, ch, TO_VICT);
					break;
				case 4:
					act ("$n barks loudly at $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You bark loudly at $N.", victim, NULL, ch, TO_CHAR);
					act ("$n barks loudly at you.", victim, NULL, ch, TO_VICT);
					break;
				case 5:
					act ("$n wags $s tail at $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You wag your tail at $N.", victim, NULL, ch, TO_CHAR);
					act ("$n wags $s tail at you.", victim, NULL, ch, TO_VICT);
					break;
				case 6:
					act ("$n sniffs $N's feet.", victim, NULL, ch, TO_NOTVICT);
					act ("You sniff $N's feet.", victim, NULL, ch, TO_CHAR);
					act ("$n sniffs your feet.", victim, NULL, ch, TO_VICT);
					break;
				case 7:
					act ("$n whines at $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You whine $N.", victim, NULL, ch, TO_CHAR);
					act ("$n whines at you.", victim, NULL, ch, TO_VICT);
					break;
				case 8:
					act ("$n gazes lovingly at $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You gaze lovingly at $N.", victim, NULL, ch, TO_CHAR);
					act ("$n gazes lovingly at you.", victim, NULL, ch, TO_VICT);
					break;
				case 9:
					act ("$n sniffs at $N in search of food.", victim, NULL, ch, TO_NOTVICT);
					act ("You sniff at $N in search of food.", victim, NULL, ch, TO_CHAR);
					act ("$n sniffs at you in search of food.", victim, NULL, ch, TO_VICT);
					break;
				case 10:
					act ("$n rolls over, hoping $N will scratch $s tummy.", victim, NULL, ch, TO_NOTVICT);
					act ("You roll over, hoping that $N will scratch your tummy.", victim, NULL, ch, TO_CHAR);
					act ("$n rolls over, hoping that you will scratch $s tummy.", victim, NULL, ch, TO_VICT);
					break;
				case 11:
					act ("$n begs $N for some food.", victim, NULL, ch, TO_NOTVICT);
					act ("You beg $N for some food.", victim, NULL, ch, TO_CHAR);
					act ("$n begs you for some food.", victim, NULL, ch, TO_VICT);
					break;
				case 12:
					act ("$n jumps up against $N's leg.", victim, NULL, ch, TO_NOTVICT);
					act ("You jump up against $N's leg.", victim, NULL, ch, TO_CHAR);
					act ("$n jumps up against your leg.", victim, NULL, ch, TO_VICT);
					break;
				case 13:
					act ("$n tries to eat $N's shoes.", victim, NULL, ch, TO_NOTVICT);
					act ("You try to eat $N's shoes.", victim, NULL, ch, TO_CHAR);
					act ("$n tries to eat your shoes.", victim, NULL, ch, TO_VICT);
					break;
				case 14:
					act ("$n runs in circles around $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You run in circles around $N.", victim, NULL, ch, TO_CHAR);
					act ("$n runs in circles around you.", victim, NULL, ch, TO_VICT);
					break;
				}
			else
				switch (number_bits (4))
				{
				default:
				case 0:
					break;

				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
					act (social_table[cmd].others_found, victim, NULL, ch, TO_NOTVICT);
					act (social_table[cmd].char_found, victim, NULL, ch, TO_CHAR);
					act (social_table[cmd].vict_found, victim, NULL, ch, TO_VICT);
					break;

				case 9:
				case 10:
				case 11:
				case 12:
					act ("$n slaps $N.", victim, NULL, ch, TO_NOTVICT);
					act ("You slap $N.", victim, NULL, ch, TO_CHAR);
					act ("$n slaps you.", victim, NULL, ch, TO_VICT);
					break;
				}
		}
	}
	return TRUE;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number (char *arg)
{
	if (*arg == '\0')
		return FALSE;

	for (; *arg != '\0'; arg++)
	{
		if (!isdigit (*arg))
			return FALSE;
	}

	return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument (char *argument, char *arg)
{
	char *pdot;
	int number;

	for (pdot = argument; *pdot != '\0'; pdot++)
	{
		if (*pdot == '.')
		{
			*pdot = '\0';
			number = atoi (argument);
			*pdot = '.';
			strcpy (arg, pdot + 1);
			return number;
		}
	}

	strcpy (arg, argument);
	return 1;
}


/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument (char *argument, char *arg_first)
{
	char cEnd;

	while (isspace (*argument))
		argument++;

	cEnd = ' ';
	if (*argument == '\'' || *argument == '"')
		cEnd = *argument++;

	while (*argument != '\0')
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		*arg_first = LOWER (*argument);
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while (isspace (*argument))
		argument++;

	return argument;
}

char *one_argument_retain_cap (char *argument, char *arg_first)
{
	char cEnd;

	while (isspace (*argument))
		argument++;

	cEnd = ' ';
	if (*argument == '\'' || *argument == '"')
		cEnd = *argument++;

	while (*argument != '\0')
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		*arg_first = *argument;
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while (isspace (*argument))
		argument++;

	return argument;
}


/* Syntax is:
disable - shows disabled commands
disable <command> - toggles disable status of command
*/

void do_disable (CHAR_DATA * ch, char *argument)
{
	int i;
	DISABLED_DATA *p, *q;
	char buf[100];

	if (IS_NPC (ch))
	{
		send_to_char ("RETURN first.\n\r", ch);
		return;
	}

	if (!argument[0])	/* Nothing specified. Show disabled commands. */
	{
		if (!disabled_first)	/* Any disabled at all ? */
		{
			send_to_char ("There are no commands disabled.\n\r", ch);
			return;
		}

		send_to_char ("Disabled commands:\n\r" "Command      Level   Disabled by\n\r", ch);

		for (p = disabled_first; p; p = p->next)
		{
			sprintf (buf, "%-12s %5d   %-12s\n\r", p->command->name, p->level, p->disabled_by);
			send_to_char (buf, ch);
		}
		return;
	}

	/* command given */

	/* First check if it is one of the disabled commands */
	for (p = disabled_first; p; p = p->next)
		if (!str_cmp (argument, p->command->name))
			break;

	if (p)			/* this command is disabled */
	{
		/* Optional: The level of the imm to enable the command must equal or exceed level
		 * of the one that disabled it */

		if (get_trust (ch) < p->level)
		{
			send_to_char ("This command was disabled by a higher power.\n\r", ch);
			return;
		}

		/* Remove */

		if (disabled_first == p)	/* node to be removed == head ? */
			disabled_first = p->next;
		else		/* Find the node before this one */
		{
			for (q = disabled_first; q->next != p; q = q->next);	/* empty for */
			q->next = p->next;
		}

		free_string (p->disabled_by);	/* free name of disabler */
		free_mem (p, sizeof (DISABLED_DATA));	/* free node */

		save_disabled ();	/* save to disk */
		send_to_char ("Command enabled.\n\r", ch);
	}
	else			/* not a disabled command, check if that command exists */
	{
		/* IQ test */
		if (!str_cmp (argument, "disable"))
		{
			send_to_char ("You cannot disable the disable command.\n\r", ch);
			return;
		}

		/* Search for the command */
		for (i = 0; cmd_table[i].name[0] != '\0'; i++)
			if (!str_cmp (cmd_table[i].name, argument))
				break;

		/* Found? */
		if (cmd_table[i].name[0] == '\0')
		{
			send_to_char ("No such command.\n\r", ch);
			return;
		}

		/* Can the imm use this command at all ? */
		if (cmd_table[i].level > get_trust (ch))
		{
			send_to_char ("You dot have access to that command; you cannot disable it.\n\r", ch);
			return;
		}

		/* Disable the command */

		p = alloc_mem (sizeof (DISABLED_DATA));

		p->command = &cmd_table[i];
		p->disabled_by = str_dup (ch->name);
		p->level = get_trust (ch);	/* save trust */
		p->next = disabled_first;
		disabled_first = p;	/* add before the current first element */

		send_to_char ("Command disabled.\n\r", ch);
		save_disabled ();	/* save to disk */
	}
}

/* Check if that command is disabled 
   Note that we check for equivalence of the do_fun pointers; this means
   that disabling 'chat' will also disable the '.' command
*/
bool check_disabled (const struct cmd_type *command)
{
	DISABLED_DATA *p;

	for (p = disabled_first; p; p = p->next)
		if (p->command->do_fun == command->do_fun)
			return TRUE;

	return FALSE;
}

/* Load disabled commands */
void load_disabled ()
{
	FILE *fp;
	DISABLED_DATA *p;
	char *name;
	int i;

	disabled_first = NULL;

	fp = fopen (DISABLED_FILE, "r");

	if (!fp)		/* No disabled file.. no disabled commands : */
		return;

	name = fread_word (fp);

	while (str_cmp (name, END_MARKER))	/* as long as name is NOT END_MARKER :) */
	{
		/* Find the command in the table */
		for (i = 0; cmd_table[i].name[0]; i++)
			if (!str_cmp (cmd_table[i].name, name))
				break;

		if (!cmd_table[i].name[0])	/* command does not exist? */
		{
			bug ("Skipping uknown command in " DISABLED_FILE " file.", 0);
			fread_number (fp);	/* level */
			fread_word (fp);	/* disabled_by */
		}
		else		/* add new disabled command */
		{
			p = alloc_mem (sizeof (DISABLED_DATA));
			p->command = &cmd_table[i];
			p->level = fread_number (fp);
			p->disabled_by = str_dup (fread_word (fp));
			p->next = disabled_first;

			disabled_first = p;

		}

		name = fread_word (fp);
	}

	fclose (fp);
}

/* Save disabled commands */
void save_disabled ()
{
	FILE *fp;
	DISABLED_DATA *p;

	if (!disabled_first)	/* delete file if no commands are disabled */
	{
		unlink (DISABLED_FILE);
		return;
	}

	fp = fopen (DISABLED_FILE, "w");

	if (!fp)
	{
		bug ("Could not open " DISABLED_FILE " for writing", 0);
		return;
	}

	for (p = disabled_first; p; p = p->next)
		fprintf (fp, "%s %d %s\n", p->command->name, p->level, p->disabled_by);

	fprintf (fp, "%s\n", END_MARKER);

	fclose (fp);
}

void make_preg( CHAR_DATA *mother, CHAR_DATA *father )
{
    char *strtime;
    char buf [MAX_STRING_LENGTH];

    if (IS_NPC(mother) || IS_NPC(father)) return;
    if (IS_AFFECTED(mother, AFF_CONTRACEPTION)) return;
	if (IS_AFFECTED(father, AFF_CONTRACEPTION)) return;
    if (IS_VAMPIRE(mother) || IS_VAMPIRE(father)) return;
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    free_string(mother->pcdata->conception);
    mother->pcdata->conception = str_dup(strtime);
    sprintf(buf,"%s %s",mother->name,father->name);
    free_string(mother->pcdata->cparents);
    mother->pcdata->cparents = str_dup(buf);
    SET_BIT(mother->extra, EXTRA_PREGNANT);
    mother->pcdata->genes[0] = (mother->max_hit + father->max_hit) * 0.5;
    mother->pcdata->genes[1] = (mother->max_mana + father->max_mana) * 0.5;
    mother->pcdata->genes[2] = (mother->max_move + father->max_move) * 0.5;
    if (mother->pcdata->genes[0] > 5000) mother->pcdata->genes[0] = 5000;
    if (mother->pcdata->genes[1] > 5000) mother->pcdata->genes[1] = 5000;
    if (mother->pcdata->genes[2] > 5000) mother->pcdata->genes[2] = 5000;

    mother->pcdata->genes[4] = number_range(1,2);
    return;
}

/* function to keep argument safe in all commands -- no static strings */
void do_function (CHAR_DATA *ch, DO_FUN *do_fun, char *argument)
{
    char *command_string;
    
    /* copy the string */
    command_string = str_dup(argument);
    
    /* dispatch the command */
    (*do_fun) (ch, command_string);
    
    /* free the string */
    free_string(command_string);
}
