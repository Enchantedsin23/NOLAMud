#include <malloc.h>
#include "protocol.h"
#include "merits.h"
#include "merc.h"

int get_merit (CHAR_DATA * ch, int merit)
{
	int max = 10;
	int total_merit;
	int min_merit = 0;
	if (merit < MERIT_AREAOFEXPERTISE || merit > MERIT_MAX)
	{
		bug ("Get_merit: Out of merit range.", 0);
		return 0;
	}
	if (IS_NPC (ch))
		return 0;
	if (ch->pcdata->powers_set[merit] > 0)
		min_merit = ch->pcdata->powers_set[merit];

	if (ch->class != CLASS_VAMPIRE)
		max = 10;
	if (merit == MERIT_COMMONSENSE)
		max = 5;

	if (min_merit > max)
		min_merit = max;

	if (ch->pcdata->powers_set[merit] > min_merit)
		min_merit = ch->pcdata->powers_set[merit];

	if (ch->pcdata->powers_mod[merit] < 1)
	{
		if (ch->pcdata->powers[merit] > min_merit)
		{
			if (ch->pcdata->powers[merit] > max)
				return max;
			else
				return ch->pcdata->powers[merit];
		}
		else
			return min_merit;
	}

	if (ch->pcdata->powers[merit] < 1)
		total_merit = 0;
	else
		total_merit = ch->pcdata->powers[merit];
	if (ch->pcdata->powers_mod[merit] > 0)
		total_merit += ch->pcdata->powers_mod[merit];
	if (total_merit > max)
		total_merit = max;
	if (total_merit < min_merit)
		return min_merit;
	return total_merit;
}

int get_truemerit (CHAR_DATA * ch, int merit)
{
	int total_merit;
	int min_merit = 0;
	if (merit < MERIT_AREAOFEXPERTISE || merit > MERIT_MAX)
	{
		bug ("Get_truemerit: Out of meritipline range.", 0);
		return 0;
	}
	if (IS_NPC (ch))
		return 0;
	if (ch->pcdata->powers_set[merit] > 0)
		min_merit = ch->pcdata->powers_set[merit];

	if (ch->pcdata->powers_mod[merit] < 1)
	{
		if (ch->pcdata->powers[merit] > min_merit)
			return ch->pcdata->powers[merit];
		else
			return min_merit;
	}

	if (ch->pcdata->powers[merit] < 1)
		total_merit = 0;
	else
		total_merit = ch->pcdata->powers[merit];
	if (ch->pcdata->powers_mod[merit] > 0)
		total_merit += ch->pcdata->powers_mod[merit];
	if (total_merit < min_merit)
		return min_merit;
	return total_merit;
}

void set_merit (CHAR_DATA * ch)
{
	int set_wear;

	if (IS_NPC (ch))
		return;
	
	set_wear = ch->pcdata->powers_set_wear;
	ch->pcdata->powers_set_wear = 0;

	if (set_wear != ch->pcdata->powers_set_wear)
		
		strip_merit (ch);
		
	return;
}

void strip_merit (CHAR_DATA * ch)
{
	int merit;

	if (IS_NPC (ch))
		return;
	if (IS_VAMPIRE (ch) || IS_GHOUL (ch))
	{
		for (merit = MERIT_DIRECTIONSENSE; merit < MERIT_MAX; merit++)
		{
			ch->pcdata->powers_mod[merit] += 20;
			ch->pcdata->powers_set[merit] += 20;
		}
		if (IS_POLYAFF (ch, POLY_SKINMASK) && get_truemerit (ch, MERIT_AREAOFEXPERTISE) < 23)
			do_skinmask (ch, "self");
		if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES) && get_truemerit (ch, MERIT_COMMONSENSE) < 21)
			do_truesight (ch, "");
		if (IS_SET (ch->act, PLR_HOLYLIGHT) && get_truemerit (ch, MERIT_COMMONSENSE) < 21)
			do_truesight (ch, "");
		if (IS_MORE (ch, MORE_FORGE) && get_truemerit (ch, MERIT_DANGERSENSE) == 20)
			ch->pcdata->forge = 0;
		for (merit = MERIT_DIRECTIONSENSE; merit < MERIT_MAX; merit++)
		{
			ch->pcdata->powers_mod[merit] -= 20;
			ch->pcdata->powers_set[merit] -= 20;
		}
	}
	return;
}

void do_meritset (CHAR_DATA * ch, char *argument)
{
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	bool is_offline = FALSE;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	sh_int merit = 0;
	sh_int value = 0;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);
	argument = one_argument (argument, arg3);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
	{
		send_to_char ("#eSyntax : meritset [victim] [merit/all] [rank]#n\n\r\n\r", ch);
		send_to_char (" #g--------------------------------------------------------------------------------\n\r", ch);
		send_to_char (" #w Merits and Fighting Styles#c:\n\r", ch);
		send_to_char (" #g--------------------------------------------------------------------------------#n\n\r", ch);
		send_to_char (" #n Universal#y:#n#n\n\r", ch);
		send_to_char (" #nareaofexpertise, commonsense, dangersense, directionsense, eideticmemory, encyclopedicknowledge, eyeforthestrange, fastreflexes, goodtimemanagement, holisticawareness, indomitable, interdisciplinary, investigativea (investigative aide), investigativep (investigative prodigy), language, library, advancedlibrary, meditativemind, multilingual, patient, toleranceforbio (tolerance for biology), trainedobserver, viceridden, virtuous, ambidextrous, automativegenius, crackdriver, demolisher, doublejointed, fleetoffoot, giant, hardy, greyhound, ironstamina, quickdraw, relentless, seizingtheedge, sleightofhand, smallframed, allies, alternateidentity, anonymity, barfly, closedbook, contacts, fame, fixer, hobbyistclique, inspiring, ironwill, mentor, pusher, resources, retainer, safeplace, smallunittactics, spindoctor, staff, status, strikinglooks, sympathetic, tableturner, takesonetoknowone, taste, truefriend, untouchable, runecaster, cheapshot, chokehold, defensivecombat, fightingfinesse, ironskin, killerinstinct, shiv, subduingstrikes, clearsighted, weakenedbond, producer, beastwhispers, beloved, protected, animalspeech, aurareading, automaticwriting, biokinesis, citywalker, clairvoyance, cursed, layingonhands, medium, mindofamadman, omensensitivity, numbingtouch, psychokinesis, psychometry, telekinesis, telepathy, thiefoffate, unseensense \n\r", ch);
        send_to_char (" #g--------------------------------------------------------------------------------#n\n\r", ch);
		return;
	}

	if ((victim = get_char_world (ch, arg1)) == NULL)
	{
		d = alloc_mem(sizeof(DESCRIPTOR_DATA));
		if (load_char_obj(d, arg1))
		{
			victim = d->character;
			is_offline = TRUE;
		}
		else
		{
			send_to_char("No such character exists.\n\r", ch);
			return;
		}
	}
	if (IS_NPC (victim))
	{
		send_to_char ("Not on NPCs.\n\r", ch);
		return;
	}

	if (!str_cmp (arg2, "all"))
	{
		value = is_number (arg3) ? atoi (arg3) : -1;

		if (value < 0 || value > 10)
		{
			send_to_char ("Range is 0 - 10.\n\r", ch);
			return;
		}

		if (value == 0)
			value = -1;

		for (merit = 0; merit <= MERIT_MAX; merit++)
			victim->pcdata->powers[merit] = value;

		send_to_char ("Done.\n\r", ch);
		return;
	}
	
	else if (!str_cmp (arg2, "areaofexpertise"))
		merit = MERIT_AREAOFEXPERTISE;
	else if (!str_cmp (arg2, "dangersense"))
		merit = MERIT_DANGERSENSE;
	else if (!str_cmp (arg2, "directionsense"))
		merit = MERIT_DIRECTIONSENSE;
	else if (!str_cmp (arg2, "commonsense"))
		merit = MERIT_COMMONSENSE;
	else if (!str_cmp (arg2, "eideticmemory"))
		merit = MERIT_EIDETICMEMORY;
	else if (!str_cmp (arg2, "encyclopedicknowledge"))
		merit = MERIT_ENCYCLOPEDICKNOWLEDGE;
	else if (!str_cmp (arg2, "eyeforthestrange"))
		merit = MERIT_EYEFORTHESTRANGE;
	else if (!str_cmp (arg2, "fastreflexes"))
		merit = MERIT_FASTREFLEXES;
	else if (!str_cmp (arg2, "goodtimemanagement"))
		merit = MERIT_GOODTIMEMANAGEMENT;
	else if (!str_cmp (arg2, "holisticawareness"))
		merit = MERIT_HOLISTICAWARENESS;
	else if (!str_cmp (arg2, "indomitable"))
		merit = MERIT_INDOMITABLE;
	else if (!str_cmp (arg2, "interdisciplinary"))
		merit = MERIT_INTERDISCIPLINARY;
	else if (!str_cmp (arg2, "investigativeaide"))
		merit = MERIT_INVESTIGATIVEA;
	else if (!str_cmp (arg2, "investigativeprodigy"))
		merit = MERIT_INVESTIGATIVEP;
	else if (!str_cmp (arg2, "language"))
		merit = MERIT_LANGUAGE;
	else if (!str_cmp (arg2, "library"))
		merit = MERIT_LIBRARY;
	else if (!str_cmp (arg2, "advancedlibrary"))
		merit = MERIT_ADVANCEDLIBRARY;
	else if (!str_cmp (arg2, "meditativemind"))
		merit = MERIT_MEDITATIVEMIND;
	else if (!str_cmp (arg2, "multilingual"))
		merit = MERIT_MULTILINGUAL;
	else if (!str_cmp (arg2, "patient"))
		merit = MERIT_PATIENT;
	else if (!str_cmp (arg2, "toleranceforbiology"))
		merit = MERIT_TOLERANCEFORBIO;
	else if (!str_cmp (arg2, "trainedobserver"))
		merit = MERIT_TRAINEDOBSERVER;
	else if (!str_cmp (arg2, "viceridden"))
		merit = MERIT_VICERIDDEN;
	else if (!str_cmp (arg2, "virtuous"))
		merit = MERIT_VIRTUOUS;
	else if (!str_cmp (arg2, "ambidextrous"))
		merit = MERIT_AMBIDEXTROUS;
	else if (!str_cmp (arg2, "automativegenius"))
		merit = MERIT_AUTOMATIVEGENIUS;
	else if (!str_cmp (arg2, "crackdriver"))
		merit = MERIT_CRACKDRIVER;
	else if (!str_cmp (arg2, "demolisher"))
		merit = MERIT_DEMOLISHER;
	else if (!str_cmp (arg2, "doublejointed"))
		merit = MERIT_DOUBLEJOINTED;
	else if (!str_cmp (arg2, "fleetoffoot"))
		merit = MERIT_FLEETOFFOOT;
	else if (!str_cmp (arg2, "giant"))
		merit = MERIT_GIANT;
	else if (!str_cmp (arg2, "hardy"))
		merit = MERIT_HARDY;
	else if (!str_cmp (arg2, "greyhound"))
		merit = MERIT_GREYHOUND;
	else if (!str_cmp (arg2, "ironstamina"))
		merit = MERIT_IRONSTAMINA;
	else if (!str_cmp (arg2, "quickdraw"))
		merit = MERIT_QUICKDRAW;
	else if (!str_cmp (arg2, "relentless"))
		merit = MERIT_RELENTLESS;
	else if (!str_cmp (arg2, "seizingtheedge"))
		merit = MERIT_SEIZINGTHEEDGE;
	else if (!str_cmp (arg2, "sleightofhand"))
		merit = MERIT_SLEIGHTOFHAND;
	else if (!str_cmp (arg2, "smallframed"))
		merit = MERIT_SMALLFRAMED;
	else if (!str_cmp (arg2, "allies"))
		merit = MERIT_ALLIES;
	else if (!str_cmp (arg2, "alternateidentity"))
		merit = MERIT_ALTERNATEIDENTITY;
	else if (!str_cmp (arg2, "anonymity"))
		merit = MERIT_ANONYMITY;
	else if (!str_cmp (arg2, "barfly"))
		merit = MERIT_BARFLY;
	else if (!str_cmp (arg2, "closedbook"))
		merit = MERIT_CLOSEDBOOK;
	else if (!str_cmp (arg2, "contacts"))
		merit = MERIT_CONTACTS;
	else if (!str_cmp (arg2, "fame"))
		merit = MERIT_FAME;
	else if (!str_cmp (arg2, "fixer"))
		merit = MERIT_FIXER;
	else if (!str_cmp (arg2, "hobbyistclique"))
		merit = MERIT_HOBBYISTCLIQUE;
	else if (!str_cmp (arg2, "inspiring"))
		merit = MERIT_INSPIRING;	
	else if (!str_cmp (arg2, "ironwill"))
		merit = MERIT_IRONWILL;
	else if (!str_cmp (arg2, "mentor"))
		merit = MERIT_MENTOR;
	else if (!str_cmp (arg2, "pusher"))
		merit = MERIT_PUSHER;
	else if (!str_cmp (arg2, "resources"))
		merit = MERIT_RESOURCES;
	else if (!str_cmp (arg2, "retainer"))
		merit = MERIT_RETAINER;
	else if (!str_cmp (arg2, "safeplace"))
		merit = MERIT_SAFEPLACE;
	else if (!str_cmp (arg2, "smallunittactics"))
		merit = MERIT_SMALLUNITTACTICS;	
	else if (!str_cmp (arg2, "spindoctor"))
		merit = MERIT_SPINDOCTOR;
	else if (!str_cmp (arg2, "staff"))
		merit = MERIT_STAFF;
	else if (!str_cmp (arg2, "status"))
		merit = MERIT_STATUS;
	else if (!str_cmp (arg2, "strikinglooks"))
		merit = MERIT_STRIKINGLOOKS;
	else if (!str_cmp (arg2, "sympathetic"))
		merit = MERIT_SYMPATHETIC;
	else if (!str_cmp (arg2, "tableturner"))
		merit = MERIT_TABLETURNER;
	else if (!str_cmp (arg2, "takesonetoknowone"))
		merit = MERIT_TAKESONETOKNOWONE;
	else if (!str_cmp (arg2, "taste"))
		merit = MERIT_TASTE;
	else if (!str_cmp (arg2, "truefriend"))
		merit = MERIT_TRUEFRIEND;
	else if (!str_cmp (arg2, "untouchable"))
		merit = MERIT_UNTOUCHABLE;
	else if (!str_cmp (arg2, "runecaster"))
		merit = MERIT_RUNECASTER;
	else if (!str_cmp (arg2, "cheapshot"))
		merit = MERIT_CHEAPSHOT;
	else if (!str_cmp (arg2, "chokehold"))
		merit = MERIT_CHOKEHOLD;
	else if (!str_cmp (arg2, "defensivecombat"))
		merit = MERIT_DEFENSIVECOMBAT;
	else if (!str_cmp (arg2, "fightingfinesse"))
		merit = MERIT_FIGHTINGFINESSE;
	else if (!str_cmp (arg2, "ironskin"))
		merit = MERIT_IRONSKIN;
	else if (!str_cmp (arg2, "killerinstinct"))
		merit = MERIT_KILLERINSTINCT;
	else if (!str_cmp (arg2, "shiv"))
		merit = MERIT_SHIV;
	else if (!str_cmp (arg2, "subduingstrikes"))
		merit = MERIT_SUBDUINGSTRIKES;
	else if (!str_cmp (arg2, "clearsighted"))
		merit = MERIT_CLEARSIGHTED;
	else if (!str_cmp (arg2, "weakenedbond"))
		merit = MERIT_WEAKENEDBOND;
	else if (!str_cmp (arg2, "producer"))
		merit = MERIT_PRODUCER;
	else if (!str_cmp (arg2, "beastwhispers"))
		merit = MERIT_BEASTWHISPERS;
	else if (!str_cmp (arg2, "beloved"))
		merit = MERIT_BELOVED;
	else if (!str_cmp (arg2, "protected"))
		merit = MERIT_PROTECTED;
	else if (!str_cmp (arg2, "animalspeech"))
		merit = MERIT_ANIMALSPEECH;
	else if (!str_cmp (arg2, "aurareading"))
		merit = MERIT_AURAREADING;
	else if (!str_cmp (arg2, "automaticwriting"))
		merit = MERIT_AUTOMATICWRITING;
	else if (!str_cmp (arg2, "biokinesis"))
		merit = MERIT_BIOKINESIS;
	else if (!str_cmp (arg2, "citywalker"))
		merit = MERIT_CITYWALKER;
	else if (!str_cmp (arg2, "clairvoyance"))
		merit = MERIT_CLAIRVOYANCE;
	else if (!str_cmp (arg2, "cursed"))
		merit = MERIT_CURSED;
	else if (!str_cmp (arg2, "layingonhands"))
		merit = MERIT_LAYINGONHANDS;
	else if (!str_cmp (arg2, "medium"))
		merit = MERIT_MEDIUM;
	else if (!str_cmp (arg2, "mindofamadman"))
		merit = MERIT_MINDOFAMADMAN;
	else if (!str_cmp (arg2, "omensensitivity"))
		merit = MERIT_OMENSENSITIVITY;
	else if (!str_cmp (arg2, "numbingtouch"))
		merit = MERIT_NUMBINGTOUCH;
	else if (!str_cmp (arg2, "psychokinesis"))
		merit = MERIT_PSYCHOKINESIS;
	else if (!str_cmp (arg2, "psychometry"))
		merit = MERIT_PSYCHOMETRY;
	else if (!str_cmp (arg2, "telekinesis"))
		merit = MERIT_TELEKINESIS;
	else if (!str_cmp (arg2, "telepathy"))
		merit = MERIT_TELEPATHY;
	else if (!str_cmp (arg2, "thiefoffate"))
		merit = MERIT_THIEFOFFATE;
	else if (!str_cmp (arg2, "unseensense"))
		merit = MERIT_UNSEENSENSE;

	else
	{
		do_meritset (ch, "");
		return;
	}
	value = is_number (arg3) ? atoi (arg3) : -1;

	if (value < 0 || value > 10)
	{
		send_to_char ("Range is 0 - 10.\n\r", ch);
		return;
	}
	victim->pcdata->powers[merit] = value;
	send_to_char ("Done.\n\r", ch);
	if (is_offline)
	{
		send_to_char("#RNote#n: Character is offline\n\r", ch);
		save_char_obj(victim);
		free_char(victim);
	}
	return;
}

// Players setting their own merits, costs XP

/*void do_buymerit (CHAR_DATA * ch, char *argument)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	sh_int merit = 0;
	sh_int value = 0;
	long cost;
	bool last = TRUE;

	argument = one_argument (argument, arg1);
	argument = one_argument (argument, arg2);

	if (IS_NPC (ch))
	{
		send_to_char ("Huh?\n\r", ch);
		return;
	}
	
	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char ("#eSyntax : meritset [victim] [merit/all] [rank]#n\n\r\n\r", ch);
		send_to_char (" #g--------------------------------------------------------------------------------\n\r", ch);
		send_to_char (" #w Merits and Fighting Styles#c:\n\r", ch);
		send_to_char (" #g--------------------------------------------------------------------------------#n\n\r", ch);
		send_to_char (" #n Universal#y:#n#n\n\r", ch);
		send_to_char (" #nareaofexpertise, commonsense, dangersense, directionsense, eideticmemory, encyclopedicknowledge, eyeforthestrange, fastreflexes, goodtimemanagement, holisticawareness, indomitable, interdisciplinary, investigativea (investigative aide), investigativep (investigative prodigy), language, library, advancedlibrary, meditativemind, multilingual, patient, toleranceforbio (tolerance for biology), trainedobserver, viceridden, virtuous, ambidextrous, automativegenius, crackdriver, demolisher, doublejointed, fleetoffoot, giant, hardy, greyhound, ironstamina, quickdraw, relentless, seizingtheedge, sleightofhand, smallframed, allies, alternateidentity, anonymity, barfly, closedbook, contacts, fame, fixer, hobbyistclique, inspiring, ironwill, mentor, pusher, resources, retainer, safeplace, smallunittactics, spindoctor, staff, status, strikinglooks, sympathetic, tableturner, takesonetoknowone, taste, truefriend, untouchable, runecaster, cheapshot, chokehold, defensivecombat, fightingfinesse, ironskin, killerinstinct, shiv, subduingstrikes, clearsighted, weakenedbond, producer, beastwhispers, beloved, protected, animalspeech, aurareading, automaticwriting, biokinesis, citywalker, clairvoyance, cursed, layingonhands, medium, mindofamadman, omensensitivity, numbingtouch, psychokinesis, psychometry, telekinesis, telepathy, thiefoffate, unseensense \n\r", ch);
        send_to_char (" #g--------------------------------------------------------------------------------#n\n\r", ch);
		return;
	}
	
	else if (!str_cmp (arg1, "areaofexpertise"))
	{
		if (str_cmp (arg2, "1"))
		{
			send_to_char ("#y[#gTrain#y]#n Area of Expertise is a 1 point Merit.", ch);
		}
		else
		{
			ch->exp -= (cost < 1 ? 1 : cost);
			send_to_char ("#y[#gTrain#y]#n Merit Area of Expertise purchased.", ch);
		    merit = MERIT_AREAOFEXPERTISE;
		}
    }
	else if (!str_cmp (arg1, "dangersense"))
	{
		if (str_cmp (arg2, "2"))
		{
			send_to_char ("#y[#gTrain#y]#n Danger Sense is a 2 point Merit.", ch);
		}
		else
		{
			ch->exp -= (cost < 2 ? 2 : cost);
			send_to_char ("#y[#gTrain#y]#n Merit Danger Sense purchased.", ch);
		    merit = MERIT_DANGERSENSE;
		}
    }
	else if (!str_cmp (arg1, "directionsense"))
	{
		if (str_cmp (arg2, "1"))
		{
			send_to_char ("#y[#gTrain#y]#n Direction Sense is a 1 point Merit.", ch);
		}
		else
		{
			ch->exp -= (cost < 1 ? 1 : cost);
			send_to_char ("#y[#gTrain#y]#n Merit Direction Sense purchased.", ch);
		    merit = MERIT_DIRECTIONSENSE;
		}
    }
	else if (!str_cmp (arg1, "commonsense"))
	{
		if (str_cmp (arg2, "3"))
		{
			send_to_char ("#y[#gTrain#y]#n Common Sense is a 3 point Merit.", ch);
		}
		else
		{
			ch->exp -= (cost < 3 ? 3 : cost);
			send_to_char ("#y[#gTrain#y]#n Merit Common Sense purchased.", ch);
		    merit = MERIT_COMMONSENSE;
		}
    }
	else if (!str_cmp (arg1, "eideticmemory"))
	{
		if (str_cmp (arg2, "2"))
		{
			send_to_char ("#y[#gTrain#y]#n Eidetic Memory is a 2 point Merit.", ch);
		}
		else
		{
			ch->exp -= (cost < 2 ? 2 : cost);
			send_to_char ("#y[#gTrain#y]#n Merit Eidetic Memory purchased.", ch);
		    merit = MERIT_EIDETICMEMORY;
		}
    }
	else if (!str_cmp (arg1, "encyclopedicknowledge"))
		merit = MERIT_ENCYCLOPEDICKNOWLEDGE;
	else if (!str_cmp (arg1, "eyeforthestrange"))
		merit = MERIT_EYEFORTHESTRANGE;
	else if (!str_cmp (arg1, "fastreflexes"))
		merit = MERIT_FASTREFLEXES;
	else if (!str_cmp (arg1, "goodtimemanagement"))
		merit = MERIT_GOODTIMEMANAGEMENT;
	else if (!str_cmp (arg1, "holisticawareness"))
		merit = MERIT_HOLISTICAWARENESS;
	else if (!str_cmp (arg1, "indomitable"))
		merit = MERIT_INDOMITABLE;
	else if (!str_cmp (arg1, "interdisciplinary"))
		merit = MERIT_INTERDISCIPLINARY;
	else if (!str_cmp (arg1, "investigativeaide"))
		merit = MERIT_INVESTIGATIVEA;
	else if (!str_cmp (arg1, "investigativeprodigy"))
		merit = MERIT_INVESTIGATIVEP;
	else if (!str_cmp (arg1, "language"))
		merit = MERIT_LANGUAGE;
	else if (!str_cmp (arg1, "library"))
		merit = MERIT_LIBRARY;
	else if (!str_cmp (arg1, "advancedlibrary"))
		merit = MERIT_ADVANCEDLIBRARY;
	else if (!str_cmp (arg1, "meditativemind"))
		merit = MERIT_MEDITATIVEMIND;
	else if (!str_cmp (arg1, "multilingual"))
		merit = MERIT_MULTILINGUAL;
	else if (!str_cmp (arg1, "patient"))
		merit = MERIT_PATIENT;
	else if (!str_cmp (arg1, "toleranceforbiology"))
		merit = MERIT_TOLERANCEFORBIO;
	else if (!str_cmp (arg1, "trainedobserver"))
		merit = MERIT_TRAINEDOBSERVER;
	else if (!str_cmp (arg1, "viceridden"))
		merit = MERIT_VICERIDDEN;
	else if (!str_cmp (arg1, "virtuous"))
		merit = MERIT_VIRTUOUS;
	else if (!str_cmp (arg1, "ambidextrous"))
		merit = MERIT_AMBIDEXTROUS;
	else if (!str_cmp (arg1, "automativegenius"))
		merit = MERIT_AUTOMATIVEGENIUS;
	else if (!str_cmp (arg1, "crackdriver"))
		merit = MERIT_CRACKDRIVER;
	else if (!str_cmp (arg1, "demolisher"))
		merit = MERIT_DEMOLISHER;
	else if (!str_cmp (arg1, "doublejointed"))
		merit = MERIT_DOUBLEJOINTED;
	else if (!str_cmp (arg1, "fleetoffoot"))
		merit = MERIT_FLEETOFFOOT;
	else if (!str_cmp (arg1, "giant"))
		merit = MERIT_GIANT;
	else if (!str_cmp (arg1, "hardy"))
		merit = MERIT_HARDY;
	else if (!str_cmp (arg1, "greyhound"))
		merit = MERIT_GREYHOUND;
	else if (!str_cmp (arg1, "ironstamina"))
		merit = MERIT_IRONSTAMINA;
	else if (!str_cmp (arg1, "quickdraw"))
		merit = MERIT_QUICKDRAW;
	else if (!str_cmp (arg1, "relentless"))
		merit = MERIT_RELENTLESS;
	else if (!str_cmp (arg1, "seizingtheedge"))
		merit = MERIT_SEIZINGTHEEDGE;
	else if (!str_cmp (arg1, "sleightofhand"))
		merit = MERIT_SLEIGHTOFHAND;
	else if (!str_cmp (arg1, "smallframed"))
		merit = MERIT_SMALLFRAMED;
	else if (!str_cmp (arg1, "allies"))
		merit = MERIT_ALLIES;
	else if (!str_cmp (arg1, "alternateidentity"))
		merit = MERIT_ALTERNATEIDENTITY;
	else if (!str_cmp (arg1, "anonymity"))
		merit = MERIT_ANONYMITY;
	else if (!str_cmp (arg1, "barfly"))
		merit = MERIT_BARFLY;
	else if (!str_cmp (arg1, "closedbook"))
		merit = MERIT_CLOSEDBOOK;
	else if (!str_cmp (arg1, "contacts"))
		merit = MERIT_CONTACTS;
	else if (!str_cmp (arg1, "fame"))
		merit = MERIT_FAME;
	else if (!str_cmp (arg1, "fixer"))
		merit = MERIT_FIXER;
	else if (!str_cmp (arg1, "hobbyistclique"))
		merit = MERIT_HOBBYISTCLIQUE;
	else if (!str_cmp (arg1, "inspiring"))
		merit = MERIT_INSPIRING;	
	else if (!str_cmp (arg1, "ironwill"))
		merit = MERIT_IRONWILL;
	else if (!str_cmp (arg1, "mentor"))
		merit = MERIT_MENTOR;
	else if (!str_cmp (arg1, "pusher"))
		merit = MERIT_PUSHER;
	else if (!str_cmp (arg1, "resources"))
		merit = MERIT_RESOURCES;
	else if (!str_cmp (arg1, "retainer"))
		merit = MERIT_RETAINER;
	else if (!str_cmp (arg1, "safeplace"))
		merit = MERIT_SAFEPLACE;
	else if (!str_cmp (arg1, "smallunittactics"))
		merit = MERIT_SMALLUNITTACTICS;	
	else if (!str_cmp (arg1, "spindoctor"))
		merit = MERIT_SPINDOCTOR;
	else if (!str_cmp (arg1, "staff"))
		merit = MERIT_STAFF;
	else if (!str_cmp (arg1, "status"))
		merit = MERIT_STATUS;
	else if (!str_cmp (arg1, "strikinglooks"))
		merit = MERIT_STRIKINGLOOKS;
	else if (!str_cmp (arg1, "sympathetic"))
		merit = MERIT_SYMPATHETIC;
	else if (!str_cmp (arg1, "tableturner"))
		merit = MERIT_TABLETURNER;
	else if (!str_cmp (arg1, "takesonetoknowone"))
		merit = MERIT_TAKESONETOKNOWONE;
	else if (!str_cmp (arg1, "taste"))
		merit = MERIT_TASTE;
	else if (!str_cmp (arg1, "truefriend"))
		merit = MERIT_TRUEFRIEND;
	else if (!str_cmp (arg1, "untouchable"))
		merit = MERIT_UNTOUCHABLE;
	else if (!str_cmp (arg1, "runecaster"))
		merit = MERIT_RUNECASTER;
	else if (!str_cmp (arg1, "cheapshot"))
		merit = MERIT_CHEAPSHOT;
	else if (!str_cmp (arg1, "chokehold"))
		merit = MERIT_CHOKEHOLD;
	else if (!str_cmp (arg1, "defensivecombat"))
		merit = MERIT_DEFENSIVECOMBAT;
	else if (!str_cmp (arg1, "fightingfinesse"))
		merit = MERIT_FIGHTINGFINESSE;
	else if (!str_cmp (arg1, "ironskin"))
		merit = MERIT_IRONSKIN;
	else if (!str_cmp (arg1, "killerinstinct"))
		merit = MERIT_KILLERINSTINCT;
	else if (!str_cmp (arg1, "shiv"))
		merit = MERIT_SHIV;
	else if (!str_cmp (arg1, "subduingstrikes"))
		merit = MERIT_SUBDUINGSTRIKES;
	else if (!str_cmp (arg1, "clearsighted"))
		merit = MERIT_CLEARSIGHTED;
	else if (!str_cmp (arg1, "weakenedbond"))
		merit = MERIT_WEAKENEDBOND;
	else if (!str_cmp (arg1, "producer"))
		merit = MERIT_PRODUCER;
	else if (!str_cmp (arg1, "beastwhispers"))
		merit = MERIT_BEASTWHISPERS;
	else if (!str_cmp (arg1, "beloved"))
		merit = MERIT_BELOVED;
	else if (!str_cmp (arg1, "protected"))
		merit = MERIT_PROTECTED;
	else if (!str_cmp (arg1, "animalspeech"))
		merit = MERIT_ANIMALSPEECH;
	else if (!str_cmp (arg1, "aurareading"))
		merit = MERIT_AURAREADING;
	else if (!str_cmp (arg1, "automaticwriting"))
		merit = MERIT_AUTOMATICWRITING;
	else if (!str_cmp (arg1, "biokinesis"))
		merit = MERIT_BIOKINESIS;
	else if (!str_cmp (arg1, "citywalker"))
		merit = MERIT_CITYWALKER;
	else if (!str_cmp (arg1, "clairvoyance"))
		merit = MERIT_CLAIRVOYANCE;
	else if (!str_cmp (arg1, "cursed"))
		merit = MERIT_CURSED;
	else if (!str_cmp (arg1, "layingonhands"))
		merit = MERIT_LAYINGONHANDS;
	else if (!str_cmp (arg1, "medium"))
		merit = MERIT_MEDIUM;
	else if (!str_cmp (arg1, "mindofamadman"))
		merit = MERIT_MINDOFAMADMAN;
	else if (!str_cmp (arg1, "omensensitivity"))
		merit = MERIT_OMENSENSITIVITY;
	else if (!str_cmp (arg1, "numbingtouch"))
		merit = MERIT_NUMBINGTOUCH;
	else if (!str_cmp (arg1, "psychokinesis"))
		merit = MERIT_PSYCHOKINESIS;
	else if (!str_cmp (arg1, "psychometry"))
		merit = MERIT_PSYCHOMETRY;
	else if (!str_cmp (arg1, "telekinesis"))
		merit = MERIT_TELEKINESIS;
	else if (!str_cmp (arg1, "telepathy"))
		merit = MERIT_TELEPATHY;
	else if (!str_cmp (arg1, "thiefoffate"))
		merit = MERIT_THIEFOFFATE;
	else if (!str_cmp (arg1, "unseensense"))
		merit = MERIT_UNSEENSENSE;

	else
	{
		do_meritset (ch, "");
		return;
	}
	value = is_number (arg2) ? atoi (arg2) : -1;

	if (value < 0 || value > 10)
	{
		send_to_char ("Range is 0 - 10.\n\r", ch);
		return;
	}
	
	if (cost > ch->exp || ch->exp < 1)
	{
		if (last)
			send_to_char ("#y[#gTrain#y]#n You don't have enough xp.\n\r", ch);
		return;
	}
	ch->pcdata->powers[merit] = value;

	return;
}*/
