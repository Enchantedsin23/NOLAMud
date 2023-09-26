#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "merc.h"

char * strcasestr (char *haystack, char *needle);
void add_memory_node(CHAR_DATA * ch, CHAR_DATA * victim, char * remebered_name);
void add_memory_from_name(CHAR_DATA * ch, char * real_name, char * remembered_name);
void remove_memory_node(CHAR_DATA * ch, MEMORY_DATA * node_to_remove);
void replace_memory_node(MEMORY_DATA * node_to_fix, char * new_name);
MEMORY_DATA * memory_search_real_name(MEMORY_DATA * memory_start, char * search_name);
MEMORY_DATA * memory_search_remembered_name(MEMORY_DATA * memory_start, char * search_name);
void find_memory(CHAR_DATA * ch, MEMORY_DATA * potential_memories, MEMORY_DATA * memory_start, char * search_name);
void do_forget(CHAR_DATA * ch, char * argument);
void do_remember(CHAR_DATA * ch, char * argument);
void do_memory_list(CHAR_DATA * ch, char * argument);
CHAR_DATA * get_char_room_from_desc(CHAR_DATA * ch, char * argument);
CHAR_DATA * get_char_room_from_alias(CHAR_DATA * ch, char * argument);
CHAR_DATA * get_char_world_alias(CHAR_DATA * ch, char *argument);
CHAR_DATA * get_char_world_desc(CHAR_DATA * ch, char *argument);
int count_of_ch_by_alias(CHAR_DATA * ch, char * argument);
int count_of_ch_by_desc(CHAR_DATA * ch, char * argument);


char * strcasestr (char *haystack, char *needle)
{
	char *p, *startn = 0, *np = 0;

	for (p = haystack; *p; p++) {
		if (np) {
			if (toupper(*p) == toupper(*np)) {
				if (!*++np)
					return startn;
			} else
				np = 0;
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return 0;
}

/******************************************************************************
* Adding a memory to the player's memory list.
*
* You pass in the character whose memory list is being updated, the character
* who is being remembered, and the name they're being remembered by.
*
* A node is created and filled with the appropriate data then placed at the
* head of the list.
******************************************************************************/
void add_memory_node(CHAR_DATA * ch, CHAR_DATA * victim, char * remembered_name)
{
	MEMORY_DATA * temp;

	temp = (MEMORY_DATA *)malloc(sizeof(MEMORY_DATA));

	temp->real_name = strdup(victim->name);

	temp->remembered_name = strdup(remembered_name);

	temp->next = ch->memory;
	
	if (ch->memory != NULL)
			ch->memory->previous = temp;
	
	temp->previous = NULL;
	ch->memory = temp;
}

/******************************************************************************
 * Adding a memory to the player's memory list from real name
 *
 * This is used in sv.c to load a char from file
 *
 *****************************************************************************/
void add_memory_from_name(CHAR_DATA * ch, char * real_name, char * remembered_name)
{
	MEMORY_DATA * temp;

	temp = (MEMORY_DATA *)malloc(sizeof(MEMORY_DATA));

	temp->real_name = strdup(real_name);

	temp->remembered_name = strdup(remembered_name);

	temp->next = ch->memory;
	
	if (ch->memory != NULL)
			ch->memory->previous = temp;
	
	temp->previous = NULL;
	ch->memory = temp;
}

/******************************************************************************
* Removing an association a player has between a remembered name and another
* player.
* 
* You pass the node you want to remove.
*
* Links are reconfigured to maintain the list.
*
* The node is then removed from the linked list and it, along with it's data,
* is freed from memory.
******************************************************************************/
void remove_memory_node(CHAR_DATA * ch, MEMORY_DATA * node_to_remove)
{
	MEMORY_DATA * temp;

/*
	Let the node after the one we're dropping, point to the node before
	the one we're dropping. If it's the head we're dropping, previous
	will be pointing to NULL like it should be anyway.
*/
	temp = node_to_remove->next;
	
	if (node_to_remove->next != NULL)
		temp->previous = node_to_remove->previous;

	/* Are we removing the head of the list? */
	if (node_to_remove->previous == NULL)
		ch->memory = temp;	// Head of the list is reassigned

	else
	{
	/* temp is the previous node and then temp->next is redirected */
		temp = node_to_remove->previous;
		temp->next = node_to_remove->next;
										   
	}

	free(node_to_remove->real_name); //free the name
	free(node_to_remove->remembered_name); //free the other name
	free(node_to_remove); //free the node
}


/******************************************************************************
* Replacing one remembered name with another.
*
* You've found a node you want to switch the remembered name for.
* That node is passed along with the new name to remember a player by.
* The name is replaced.
******************************************************************************/
void replace_memory_node(MEMORY_DATA * node_to_fix, char * new_name)
{
	free(node_to_fix->remembered_name);

	node_to_fix->remembered_name = strdup(new_name);
}


/******************************************************************************
* Search a player's memory by actual player names.
*
* Used for do_look and things to that effect.
* 
* Return the node with the "remembered name" for that player.
* If one isn't found, return NULL.
******************************************************************************/
MEMORY_DATA * memory_search_real_name(MEMORY_DATA * memory_start, char * search_name)
{
	MEMORY_DATA * current;

	for (current = memory_start; current != NULL; current = current->next)
	{
		if (strcmp(current->real_name, search_name) == 0)
			break;
	}

	return current;
}


/******************************************************************************
* Search a player's memory by remembered names.
*
* Used to remove a node or replace a node.
* 
* Returns the node with the remembered name you pass to it.
* If it's not found, NULL is returned.
******************************************************************************/
MEMORY_DATA * memory_search_remembered_name(MEMORY_DATA * memory_start, char * search_name)
{
	MEMORY_DATA * current;

	current = memory_start;

	while ((current != NULL) && (strcmp(current->remembered_name, search_name) != 0))
	{
		current = current->next;
	}
	return current;
}


/******************************************************************************
* Wrapper for the memory_search_remembered_name function
*
* Returns a list of potential memories a player is looking for
*
* Assumes potential_memories is already malloc'd prior to this call
*
* Returns potential_memories filled with a list of possible memories
******************************************************************************/
void find_memory(CHAR_DATA * ch, MEMORY_DATA * potential_memories, MEMORY_DATA * memory_start, char * search_name)
{
	CHAR_DATA * victim;
	MEMORY_DATA * returned_val;

	potential_memories->next = NULL; // reset potential memories
	
	returned_val = memory_search_remembered_name(memory_start, search_name);

	while(returned_val != NULL)
	{
		// If they're in the same room as the player
		if ((victim = get_char_room(ch, returned_val->real_name)) != NULL)
		{
			potential_memories->next = (MEMORY_DATA *)malloc(sizeof(MEMORY_DATA));
			potential_memories->next->previous = potential_memories;
			potential_memories = potential_memories->next;

			potential_memories->real_name = strdup(returned_val->real_name);
			potential_memories->remembered_name = strdup(returned_val->remembered_name);
			potential_memories->next = NULL;
		}
		returned_val = memory_search_remembered_name(returned_val->next, search_name);
	}
}


void do_remember(CHAR_DATA * ch, char * argument)
{
	CHAR_DATA * victim;
	char buf [MAX_STRING_LENGTH];
	char arg1 [MAX_INPUT_LENGTH];
	char arg2 [MAX_INPUT_LENGTH];
	char arg3 [MAX_INPUT_LENGTH];
	char num_string [MAX_INPUT_LENGTH];
	MEMORY_DATA * temp_memory;
	int char_num = 0;
	int desc_count = 0;
	int alias_count = 0;
	int i;
	bool skip_this_section = 0;

	if (argument[0] == '\0')
	{
		send_to_char ("Syntax: remember character as alias\r\n", ch);
		return;
	}
	else if (arg_count(argument) < 3)
	{
		send_to_char ("Syntax: remember character as alias\r\n", ch);
		return;
	}

	// get the name argument
	argument = one_argument_retain_cap(argument, arg1);

	// see if there's multiple people with that desc or alias
	// if so inform the user and exit
	if (arg1[0] == '\0')
	{
		send_to_char ("Syntax: remember character as alias\r\n", ch);
		return;
	}
	if (arg1[1] != '\0')
	{
		if (arg1[0] != '1' && arg1[1] != '.')
		{
			char_num = number_argument(arg1, arg1);
			if (char_num == 1)
			{
				alias_count = count_of_ch_by_alias(ch, arg1);
				desc_count = count_of_ch_by_desc(ch, arg1);

				if (alias_count == 1 && desc_count == 1)
				{
					sprintf(num_string, "%d.%s", 1, arg1);
					victim = get_char_room_from_alias(ch, num_string);
					// if the two characters are different, continue onward
					if (!str_cmp(victim->name, (get_char_room_from_desc(ch, num_string))->name))
					{
						skip_this_section = 1;
					}
				}

				if (skip_this_section)
				{
				}	
				else if (alias_count + desc_count > 1)
				{
					send_to_char("Be more specific:\r\n", ch);
					send_to_char("Potential matching aliases:\r\n", ch);
					for (i = 1; i <= alias_count; i++)
					{
						sprintf(num_string, "%d.%s", i, arg1);
						victim = get_char_room_from_alias(ch, num_string);
						sprintf(buf, "%d.%s\r\n", i, victim->pcdata->roomdesc );
						send_to_char(buf, ch);
					}
					send_to_char("\r\nPotential matching descriptions:\r\n", ch);
					for (i = 1; i <= desc_count; i++)
					{
						sprintf(num_string, "%d.%s", i, arg1);
						victim = get_char_room_from_desc(ch, num_string);
						sprintf(buf, "%d.%s\r\n", i, victim->pcdata->roomdesc);
						send_to_char(buf, ch);
					}

					send_to_char("\r\nSyntax: remember x.keyword as alias\r\n\r\n", ch);

					return;
				}
			}
		}
	}

	argument = one_argument_retain_cap(argument, arg2);
	// arg1 gives victim name from shortdesc
	
	if (strcmp(arg2, "as") != 0)
	{
		send_to_char("Syntax: remember character as alias\r\n", ch);
		return;
	}
	one_argument_retain_cap(argument, arg3);
	
	// Find char by alias first, then desc
	victim = get_char_room_from_alias(ch, arg1);
	if (victim == NULL)
	{
		victim = get_char_room_from_desc(ch, arg1);
		if (victim == NULL)
		{
			send_to_char("They aren't here.\n\r", ch);
			return;
		}
	}

	// Try to find an old memory of the victim
  	temp_memory = memory_search_real_name(ch->memory, victim->name);

	if (temp_memory == NULL) // if ch hasn't remembered victim
	{
		add_memory_node(ch, victim, arg3);
		sprintf(buf, "You remembered %s as %s\r\n", victim->pcdata->roomdesc, arg3);
		send_to_char(buf, ch);
		return;
	}
	else
	{
		// Otherwise update the alias
		replace_memory_node(temp_memory, arg3);
		sprintf(buf, "You now remember %s by the new name %s\r\n", victim->pcdata->roomdesc, arg3);
		send_to_char(buf, ch);
		return;
	}
}


void do_forget(CHAR_DATA * ch, char * argument)
{
	MEMORY_DATA * potential_mem = NULL;
	MEMORY_DATA * current_mem = NULL;
	CHAR_DATA * victim = NULL;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char * position;
	int char_num;
	int i;
	
	potential_mem = (MEMORY_DATA *)malloc(sizeof(MEMORY_DATA));

	if (argument[0] == '\0')
	{
		send_to_char("Syntax: forget alias\r\n", ch);
		return;
	}

	position = strchr(argument, '.');
	if (position != NULL)
	{
		argument[abs(argument - position)] = ' ';
	}

	argument = one_argument_retain_cap(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: forget alias\r\n", ch);
		return;
	}

	if (isdigit(arg1[0]))
	{
		char_num = atoi(arg1);

		one_argument_retain_cap(argument, arg1);
		
		find_memory(ch, potential_mem, ch->memory, arg1);
	
		current_mem = potential_mem->next;

		if (current_mem == NULL)
		{
			send_to_char("You don't remember anyone by that name.\r\n", ch);
			return;
		}
		
		// Find the memory to remove
		i = 1;
		while(i < char_num && current_mem != NULL)
		{
			i++;
			current_mem = current_mem->next;
		}
		// If you didn't reach the numbered memory requested, complain
		if (i != char_num || current_mem == NULL)
		{
			sprintf(arg1, "Invalid memory number: %d\r\n", char_num);
			send_to_char(arg1, ch);
			return;
		}
		else
		{
			sprintf(buf, "Stopped remembering %s.\r\n", current_mem->remembered_name);
			send_to_char(buf, ch);
			remove_memory_node(ch, memory_search_real_name(ch->memory, current_mem->real_name));
/*
	Free the copies of the memories because find_memory returns a new set of memories
	rather than a reference to the players memories.
*/
			free(current_mem->real_name);
			free(current_mem->remembered_name);
			free(current_mem);
			free(potential_mem);
			return;
		}
	}
	// get name from remembered
	find_memory(ch, potential_mem, ch->memory, arg1);


	current_mem = potential_mem->next;

	if(current_mem != NULL)
	{
		if(current_mem->next == NULL)
		{
			sprintf(buf, "Stopped remembering %s.\r\n", current_mem->remembered_name);
			send_to_char(buf, ch);
			remove_memory_node(ch, memory_search_real_name(ch->memory, current_mem->real_name));
/*
	Free the copies of the memories because find_memory returns a new set of memories
	rather than a reference to the players memories.
*/
			free(current_mem->real_name);
			free(current_mem->remembered_name);
			free(current_mem);
			free(potential_mem);
		}
		else
		{
			for (i = 1; current_mem != NULL; current_mem = current_mem->next, i++)
			{
				victim = get_char_room(ch, current_mem->real_name);
				sprintf(buf, "%d.%s is %s\r\n", i, current_mem->remembered_name, victim->pcdata->roomdesc);
				send_to_char(buf, ch);
			}
			send_to_char("\r\nMultiple characters remembered by that name\r\n", ch);
			send_to_char("Use following syntax: forget # alias\r\n\r\n", ch);
		}
	}
	else
	{
		send_to_char("You don't remember anyone by that name.\n\r", ch);
	}
}



void do_memory_list(CHAR_DATA * ch, char * argument)
{
	char buf[MAX_INPUT_LENGTH];
	MEMORY_DATA * memory_pointer;
	CHAR_DATA * rch;

	memory_pointer = ch->memory;
	send_to_char("#GC#eurrent #GMe#emory #GL#eist#G:#n\r\n", ch);
	while(memory_pointer != NULL)
	{
		rch = get_char_world(ch, memory_pointer->real_name);
		if (rch == NULL)
		{
			sprintf(buf, "#B(#e%s#B) #G-#c Offline#n\r\n", memory_pointer->remembered_name);
		}
		else
		{
			sprintf(buf, "#B(#e%s#B) #G-#c %s#n\r\n", memory_pointer->remembered_name, rch->pcdata->roomdesc);
		}
		send_to_char(buf, ch);
		memory_pointer = memory_pointer->next;
	}
	send_to_char("#G+#e--------\r\n", ch);
}


CHAR_DATA * get_char_room_from_desc(CHAR_DATA * ch, char * argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *rch;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	if (is_abbr (arg, "self me myself") && (IS_NPC (ch) || ch->pcdata->chobj ==
NULL))
		return ch;
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	{
		if (!IS_NPC (rch) && IS_EXTRA (rch, EXTRA_OSWITCH))
			continue;
		else if (!can_see (ch, rch))
			continue;
		// Returns NULL if arg isn't in roomdesc
		else if (strcasestr(rch->pcdata->roomdesc, arg) == NULL)
		{
			continue;
		}
		if (++count == number)
			return rch;
	}

	return NULL;
}


CHAR_DATA * get_char_room_from_alias(CHAR_DATA * ch, char * argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *rch;
	MEMORY_DATA * temp_mem;
	int number;
	int count;

	number = number_argument (argument, arg);
	count = 0;
	if (is_abbr (arg, "self me myself") && (IS_NPC (ch) || ch->pcdata->chobj == NULL))
		return ch;
	
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	{
		if (!can_see (ch, rch))
			continue;
		temp_mem = memory_search_real_name(ch->memory, rch->name);
		if (temp_mem == NULL)
			continue;
//		if (strcmp(strlower(temp_mem->remembered_name), strlower(arg)) == 0)
		if(!str_cmp(temp_mem->remembered_name, arg))
			if (++count == number)
				return rch;
	}

	return NULL;
}



CHAR_DATA * get_char_world_alias(CHAR_DATA * ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        CHAR_DATA *wch;
	MEMORY_DATA * temp_mem;
        int number;
        int count;

        if ((wch = get_char_room_from_alias (ch, argument)) != NULL)
                return wch;

        number = number_argument (argument, arg);
        count = 0;
        for (wch = char_list; wch != NULL; wch = wch->next)
        {
                if (!IS_NPC (wch) && IS_EXTRA (wch, EXTRA_OSWITCH))
                        continue;
                else if (!can_see (ch, wch))
                        continue;
		else if (IS_NPC (wch) || !is_abbr (arg, wch->morph))
			continue;

		temp_mem = memory_search_real_name(ch->memory, wch->name);
		if (temp_mem == NULL)
			continue;
		if (strcmp(strlower(temp_mem->remembered_name), strlower(arg)) == 0)
			if (++count == number)
                        	return wch;
        }

        return NULL;
}



CHAR_DATA * get_char_world_desc(CHAR_DATA * ch, char *argument)
{
        char arg[MAX_INPUT_LENGTH];
        CHAR_DATA *wch;
        int number;
        int count;

	if ((wch = get_char_room_from_desc (ch, argument)) != NULL)
		return wch;

        number = number_argument (argument, arg);
        count = 0;
        for (wch = char_list; wch != NULL; wch = wch->next)
        {
                if (!IS_NPC (wch) && IS_EXTRA (wch, EXTRA_OSWITCH))
                        continue;
                else if (!can_see (ch, wch))
                        continue;
		else if (IS_NPC (wch) || !is_abbr (arg, wch->morph))
			continue;

		else if (strcasestr(wch->pcdata->roomdesc, arg) == NULL)
		{
			continue;
		}

		if (++count == number)
                       	return wch;
        }

        return NULL;
}


int count_of_ch_by_desc(CHAR_DATA * ch, char * argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *rch;
	int counter = 0;

	one_argument_retain_cap(argument, arg);

	if (is_abbr (arg, "self me myself") && (IS_NPC (ch) || ch->pcdata->chobj ==
NULL))
		return 0;
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	{
		if (!IS_NPC (rch) && IS_EXTRA (rch, EXTRA_OSWITCH))
			continue;
		else if (!can_see (ch, rch))
			continue;
		// Returns NULL if arg isn't in roomdesc
		else if (strcasestr(rch->pcdata->roomdesc, arg) == NULL)
		{
			continue;
		}
		counter++;
	}

	return counter;
}



int count_of_ch_by_alias(CHAR_DATA * ch, char * argument)
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA * rch;
	MEMORY_DATA * temp_mem;
	int counter = 0;

	one_argument_retain_cap(argument, arg);
	if (is_abbr (arg, "self me myself") && (IS_NPC (ch) || ch->pcdata->chobj == NULL))
		return 0;	
	for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
	{
		temp_mem = memory_search_real_name(ch->memory, rch->name);
		if (temp_mem == NULL)
			continue;
		if (strcmp(temp_mem->remembered_name, arg) == 0)
			counter++;
	}

	return counter;
}



