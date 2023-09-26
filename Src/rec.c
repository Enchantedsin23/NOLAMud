/*
	pose and renown recommendation system.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "merc.h"

void add_pose_rec args((char *giver, char *receiver, char *text));
void add_renown_rec args((char *giver, char *receiver, int amount, int type,  char *text));
int remove_pose_rec args((int index));
int remove_renown_rec args((int index));
POSE_REC *get_pose_rec(int index);
RENOWN_REC *get_renown_rec(int index);
void save_pose_recs();
void save_renown_recs();
void load_pose_recs();
void load_renown_recs();
void do_recommend(CHAR_DATA *ch, char *argument);
void do_rec_list(CHAR_DATA *ch, char *arg);
void do_rec_remove(CHAR_DATA *ch, char *arg);
void do_renown(CHAR_DATA *ch, char *argument);
void do_renown_remove(CHAR_DATA *ch, char *arg);
void do_renown_list(CHAR_DATA *ch, char *arg);

const char *renown2name [] = {"clarity", "honor", "po"};

//Global linked lists
POSE_REC *pose_recs;
RENOWN_REC *renown_recs;
//Length counters
int npose_recs;
int nrenown_recs;


//Adds a new pose rec to the global list.
void add_pose_rec(char *giver, char *receiver, char *text)
{
	POSE_REC *temp;
	POSE_REC *before;
	giver[0] = toupper(giver[0]);
	receiver[0] = toupper(receiver[0]);
	
	//Add rec to list, sorted by receiver name.
	if (pose_recs==NULL || !strcmp(pose_recs->receiver, receiver))
	{
		temp = (POSE_REC *)malloc(sizeof(POSE_REC));
		temp->next = pose_recs;
		pose_recs = temp;
	}
	else
	{
		for (temp = pose_recs;temp!=NULL; before = temp, temp = temp->next)
		{
			if (!strcmp(temp->receiver, receiver)) break;
		}
		before->next = (POSE_REC *)malloc(sizeof(POSE_REC));
		before->next->next = temp;
		
		temp = before->next;
	}
	//Fill in struct values
	temp->giver = strdup(giver);
	temp->receiver = strdup(receiver);
	temp->text = strdup(text);
	npose_recs++;
	return;
}

//Adds a renown recommendation to the global list.
void add_renown_rec(char *giver, char *receiver, int amount, int type,  char *text)
{
	RENOWN_REC *temp;
	RENOWN_REC *before;
	giver[0] = toupper(giver[0]);
	receiver[0] = toupper(receiver[0]);
	
	//Add rec to list, sorted by receiver name.
	if (renown_recs==NULL || !strcmp(renown_recs->receiver, receiver))
	{
		temp= (RENOWN_REC *)malloc(sizeof(RENOWN_REC));
		temp->next = renown_recs;
		renown_recs = temp;
	}
	else
	{
		for (temp = renown_recs;temp!=NULL; before = temp, temp = temp->next)
		{
			if (!strcmp(temp->receiver, receiver)) break;
		}
		before->next = (RENOWN_REC *)malloc(sizeof(RENOWN_REC));
		before->next->next = temp;
		
		temp = before->next;
	}

	//Fill in struct values
	temp->giver = strdup(giver);
	temp->receiver = strdup(receiver);
	temp->amount = amount;
	temp->type = type;
	temp->text = strdup(text);
	nrenown_recs++;
	return;
}

//Removes a pose rec by index. Indices start at 0. 
//Returns 0 if index is too large, returns 1 if successful.
int remove_pose_rec(int index)
{
	POSE_REC *toFree; POSE_REC *before; int i;
	
	if (index >= npose_recs) return 0;
	if (index==0)
	{
		toFree = pose_recs;
		//unlink first node
		pose_recs = pose_recs->next;
	}
	else
	{
		toFree = pose_recs;
		for( i=0; i<index; ++i)
		{
			before = toFree;
			toFree = toFree->next;
		}
		//unlink node from the one before it
		before->next = toFree->next;
	}
	//delete node
	free(toFree->giver);
	free(toFree->receiver);
	free(toFree->text);
	free(toFree);
	//decrement number count
	npose_recs--;
	return 1;
}



//Removes a renown rec by index. Starts at 0. 
//Returns 0 if index is too large and 1 if successful.
int remove_renown_rec(int index)
{
	RENOWN_REC *toFree; RENOWN_REC *before; int i;
	
	if (index >= nrenown_recs) return 0;
	if (index==0)
	{
		toFree = renown_recs;
		//unlink first node
		renown_recs = renown_recs->next;
	}
	else
	{
		toFree = renown_recs;
		for( i=0; i<index; ++i)
		{
			before = toFree;
			toFree = toFree->next;
		}
		//unlink node from the one before it
		before->next = toFree->next;
	}
	//delete node
	free(toFree->giver);
	free(toFree->receiver);
	free(toFree->text);
	free(toFree);
	//decrement number count
	nrenown_recs--;
	return 1;
}

//Retrieves a pose_rec by index number. Returns NULL if index is too high.
POSE_REC *get_pose_rec(int index)
{
	POSE_REC *result; int i;
	if (index >= npose_recs) return NULL;
	result = pose_recs;
	for(i=0; i<index; i++) result = result->next;
	return result;
}
//Retrieves a RENOWN_rec by index number. Returns NULL if index is too high.
RENOWN_REC *get_renown_rec(int index)
{
	RENOWN_REC *result; int i;
	if (index >= nrenown_recs) return NULL;
	result = renown_recs;
	for(i=0; i<index; i++) result = result->next;
	return result;
}


void save_pose_recs()
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;
	POSE_REC *r;
	
	//save a backup copy
	sprintf(buf, "cp -f %s %s.bak", POSE_REC_FILE, POSE_REC_FILE);
	system(buf);	
	if ((fp = fopen(POSE_REC_FILE, "w")) == NULL)
	{
		bug("Couldn't open the pose recommendation file. Pose recs cannot be saved.", 0);
		return;
	}
	for (r = pose_recs; r != NULL; r = r->next)
		fprintf(fp, "%s %s %s\n", r->giver, r->receiver, r->text);
	fclose(fp);
}

void save_renown_recs()
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;
	RENOWN_REC *r;
	
	//save a backup copy
	sprintf(buf, "cp -f %s %s.bak", RENOWN_REC_FILE, RENOWN_REC_FILE);
	system(buf);
	if ((fp = fopen(RENOWN_REC_FILE, "w")) == NULL)
	{
		bug("Couldn't open the renown recommendation file. Renown recs cannot be saved.", 0);
		return;
	}
	for (r = renown_recs; r != NULL; r = r->next)
		fprintf(fp, "%s %s %d %d %s\n", r->giver, r->receiver, r->amount, r->type, r->text);
	fclose(fp);
}

void load_pose_recs()
{
	FILE *fp; char c; int i;
	char giver[MAX_STRING_LENGTH];
	char receiver[MAX_STRING_LENGTH];
	char text[MAX_STRING_LENGTH];
	
	if ((fp = fopen(POSE_REC_FILE, "r")) == NULL)
	{
		bug("Couldn't find the pose recommendation file. Unable to load old pose recs.", 0);
		return;
	}
	while (fscanf(fp, "%s %s ", giver, receiver) != EOF)
	{
		for(i=0;(c=fgetc(fp))!='\n'; i++) text[i] = c;
		text[i] = '\0'; 
		add_pose_rec(giver, receiver, text);
	}
	return;
}

void load_renown_recs()
{
	FILE *fp; int i; char c;
	char giver[MAX_STRING_LENGTH]; char receiver[MAX_STRING_LENGTH]; int amount; int type; char text[MAX_STRING_LENGTH];
	
	if ((fp = fopen(RENOWN_REC_FILE, "r")) == NULL)
	{
		bug("Couldn't find the renown recommendation file. Unable to load old renown recs.", 0);
		return;
	}
	while (fscanf(fp, "%s %s %d %d ", giver, receiver, &amount, &type) != EOF)
	{
		for(i=0;(c=fgetc(fp))!='\n'; i++) text[i] = c;
		text[i] = '\0'; 
		add_renown_rec(giver, receiver, amount, type, text);
	}
	return;
}


void do_recommend(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	
	argument = one_argument(argument, arg);
	if (IS_STORYTELLER(ch))
	{
		if (arg[0] == '\0')
		{
			send_to_char("#c.-----------------------.\n\r", ch);
			send_to_char("#c|#w Syntax:#c               |\n\r", ch);
			send_to_char("#c|-----------------------|\n\r", ch);
			send_to_char("#c| #nrec list              #c|\n\r", ch);
			send_to_char("#c| #nrec read <number>     #c|\n\r", ch);
			send_to_char("#c| #nrec remove <number>  #c |\n\r", ch);
			send_to_char("#c| #nrec <player> <pose>#c   |\n\r", ch);
			send_to_char("#c'-----------------------'#n\n\r", ch);
			return;
		}
		if (!str_cmp(arg, "list") || !str_cmp(arg, "read"))
		{
			do_rec_list(ch, argument);
			return;
		}
		if (!str_cmp(arg, "remove"))
		{
			do_rec_remove(ch, argument);
			return;
		}
	}
	if (argument[0] == '\0')
	{
		send_to_char("#cSyntax: rec <player> <pose>#n\n\r", ch);
		return;
	}
	add_pose_rec(ch->name, arg, argument);
	save_pose_recs();
	send_to_char ("#cYou submit an XP recommendation for:#C ", ch);
	send_to_char (arg ? : "#C(None)#n.\n\r", ch);
	logchan("#nNew roleplay #wrecommendation#n added.#n", CHANNEL_LOG_NORMAL);
	return;
}

void do_rec_list(CHAR_DATA *ch, char *arg)
{
	char buf[MAX_STRING_LENGTH];
	POSE_REC *r;
	int i;
	if (arg[0] == '\0')
	{
		for (	i=0, r = pose_recs; 
				r != NULL;
				i++, r = r->next)
		{
			sprintf(buf, "#e%d#G)   #eBy#G: #e%-12s #eFor#G:#e %s#n\n\r", i, r->giver, r->receiver);
			send_to_char(buf, ch);
		}
	}
	else
	{
		i = atoi(arg);
		if (i >= npose_recs || i < 0)
		{
			send_to_char("#rInvalid pose recommendation.#n\n\r", ch);
			return;
		}
		r = get_pose_rec(i);
		sprintf(buf, "#w%d#c)   #nBy#C: #w%s    #nFor#C: #w%s#n\n\r%s\n\r", i, r->giver, r->receiver, r->text);
		send_to_char(buf, ch);
	}
	return;
}

void do_rec_remove(CHAR_DATA *ch, char *arg)
{
	int i;
	if (arg[0] == '\0')
	{
		send_to_char("Syntax: rec remove <number>", ch);
		return;
	}
	i = atoi(arg);
	if (i >= npose_recs || i < 0)
	{
		send_to_char("#rInvalid pose recommendation.#n\n\r", ch);
		return;
	}
	remove_pose_rec(i);
	save_pose_recs();
	send_to_char("#rRoleplay recommendation removed.#n\n\r", ch);
	logchan("Roleplay recommendation removed.", CHANNEL_LOG_NORMAL);
	return;
}

void do_renown(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	int amount; int type;
	
	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if (IS_STORYTELLER(ch))
	{
		if (arg[0] == '\0')
		{
			send_to_char("Syntax:\n\r", ch);
			send_to_char("	renown list\n\r", ch);
			send_to_char("	renown read <number>\n\r", ch);
			send_to_char("	renown remove <number>\n\r", ch);
			send_to_char("	renown <player> <amount> clarity/honor/po <reason>\n\r", ch);
			return;
		}
		if (!str_cmp(arg, "list") || !str_cmp(arg, "read"))
		{
			do_renown_list(ch, arg2);
			return;
		}
		if (!str_cmp(arg, "remove"))
		{
			do_renown_remove(ch, arg2);
			return;
		}
	}

	if (argument[0] == '\0')
	{
		send_to_char("Syntax: renown <player> <amount> clarity/honor/po <reason>#n\n\r", ch);
		return;
	}
	if ((amount = atoi(arg2)) < 1)
	{
		send_to_char("#rNot a valid amount of renown. :3#n\n\r", ch);
		return;
	}
	if      (!str_cmp (arg3, "clarity"))  type = CLARITY_REC;
	else if (!str_cmp (arg3, "honor"))  type = HONOR_REC;
	else if (!str_cmp (arg3, "po")) type = PO_REC;
	else 
	{
		send_to_char("Syntax: rec <player> <amount> clarity/honor/po <reason>\n\r", ch);
		return;
	}
	
	add_renown_rec(ch->name, arg, amount, type, argument);
	save_renown_recs();
	send_to_char("You have recommended their actions for temporary renown.\n\r", ch);
	logchan("New renown rec added.", CHANNEL_LOG_NORMAL);
	return;
}

void do_renown_list(CHAR_DATA *ch, char *arg)
{
	char buf[MAX_STRING_LENGTH];
	RENOWN_REC *r;
	int i;
	
	if (arg[0] == '\0')
	{
		for (	i=0, r = renown_recs; 
				r != NULL;
				i++, r = r->next)
		{
			sprintf(buf, "#g%d#C)   #GBy#C: #g%-12s #GFor#C: #g%-12s #GRenown#C: #g%d %s#n\n\r", i, r->giver, r->receiver, r->amount, renown2name[r->type])
;
			send_to_char(buf, ch);
		}
	}
	else
	{
		i = atoi(arg);
		if (i >= nrenown_recs || i < 0)
		{
			send_to_char("#rInvalid renown recommendation.#n\n\r", ch);
			return;
		}
		r = get_renown_rec(i);
		sprintf(buf, "#g%d#C)   #GBy#C: #g%s     #GFor#C: #c%s     #GRenown#C: #g%d %s#n\n\r%s\n\r", i, r->giver, r->receiver, r->amount, renown2name[r->type], r->text);
		send_to_char(buf, ch);
	}
	return;
}

void do_renown_remove(CHAR_DATA *ch, char *arg)
{
	int i;
	if (arg[0] == '\0')
	{
		send_to_char("Syntax: renown remove <number>", ch);
		return;
	}
	i = atoi(arg);
	if (i >= nrenown_recs || i < 0)
	{
		send_to_char("#rInvalid renown recommendation.#n\n\r", ch);
		return;
	}
	remove_renown_rec(i);
	save_renown_recs();
	send_to_char("Renown recommendation removed.\n\r", ch);
	logchan("Renown rec removed.", CHANNEL_LOG_NORMAL);
	return;
}
	
