/***************************************************************************
 *  File: string.c                                                         *
 *  Usage: various functions for new NOTE system                           *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  Much of this code came from CircleMUD.                                 *
 *  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
 *                                                                         *
 *  Revised for Merc 2.1 by Jason Dinkel.                                  *
 *  Revised for Envy 1.0 by Jason Dinkel.				   *
 ***************************************************************************/

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
#include "merc.h"



void string_edit (CHAR_DATA * ch, char **pString)
{
	send_to_char ("-=======================================-\n\r", ch);
	send_to_char (" Entering line editing mode.\n\r", ch);
	send_to_char (" Type .h on a new line for help\n\r", ch);
	send_to_char (" Terminate with a ~ or @ on a blank line.\n\r", ch);
	send_to_char ("-=======================================-\n\r", ch);

	if (*pString == NULL)
	{
		*pString = str_dup ("");
	}
	else
	{
		**pString = '\0';
	}

	ch->desc->pString = pString;

	return;
}



void string_append (CHAR_DATA * ch, char **pString)
{
	send_to_char ("-=======- Entering APPEND Mode -========-\n\r", ch);
	send_to_char (" Terminate with a ~ or @ on a blank line.\n\r", ch);
	send_to_char ("-=======================================-\n\r", ch);

	if (*pString == NULL)
	{
		*pString = str_dup ("");
	}
	send_to_char (*pString, ch);

	if (*(*pString + strlen (*pString) - 1) != '\r')
		send_to_char ("\n\r", ch);

	ch->desc->pString = pString;

	return;
}



char *string_replace (char *orig, char *old, char *new)
{
	char xbuf[MAX_STRING_LENGTH];
	int i;

	xbuf[0] = '\0';
	strcpy (xbuf, orig);
	if (strstr (orig, old) != NULL)
	{
		i = strlen (orig) - strlen (strstr (orig, old));
		xbuf[i] = '\0';
		strcat (xbuf, new);
		strcat (xbuf, &orig[i + strlen (old)]);
		free_string (orig);
	}

	return str_dup (xbuf);
}



void string_add (CHAR_DATA * ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];

	if (*argument == '.')
	{
		char arg1[MAX_INPUT_LENGTH];
		char arg2[MAX_INPUT_LENGTH];
		char arg3[MAX_INPUT_LENGTH];

		argument = one_argument (argument, arg1);
		argument = first_arg (argument, arg2, FALSE);
		argument = first_arg (argument, arg3, FALSE);

		if (!str_cmp (arg1, ".p"))
		{
#if defined(unix)
		FILE *out;
		char *pf;

		out = fopen( ".roomdesc", "w" );
		if ( *argument )
			fprintf(out,"%s\n", argument );
		else
			fprintf(out,"%s\n", *ch->desc->pString);
		fflush(out);
		fclose(out);

		out = popen( "ispell -l < .roomdesc", "r" );
		send_to_char( "The following words are misspelled:\n\r",ch);

		while( fgets( buf, MAX_STRING_LENGTH - 1, out ) != NULL)
		{
		  pf = format_string( str_dup( buf ) );
		  send_to_char( pf, ch );
		  free_string( pf );
		}

		pclose( out );
#else
		send_to_char( "Unix systems only.\n\r", ch );
#endif
		return;
		}

		if (!str_cmp (arg1, ".c"))
		{
			send_to_char ("String cleared.\n\r", ch);
			**ch->desc->pString = '\0';
			return;
		}

		if (!str_cmp (arg1, ".s"))
		{
			send_to_char ("String so far:\n\r", ch);
			send_to_char (*ch->desc->pString, ch);
			return;
		}

		if (!str_cmp (arg1, ".r"))
		{
			if (arg2[0] == '\0')
			{
				send_to_char ("usage:  .r \"old string\" \"new string\"\n\r", ch);
				return;
			}

			*ch->desc->pString = string_replace (*ch->desc->pString, arg2, arg3);
			sprintf (buf, "'%s' replaced with '%s'.\n\r", arg2, arg3);
			send_to_char (buf, ch);
			return;
		}

	

		if (!str_cmp (arg1, ".f"))
		{
			*ch->desc->pString = format_string (*ch->desc->pString);
			send_to_char ("String formatted.\n\r", ch);
			return;
		}

		if (!str_cmp (arg1, ".h"))
		{
			send_to_char ("Sedit help (commands available from the beginning of a new line only):  \n\r", ch);
			send_to_char (".r 'old string' 'new string'   - replace a substring with a new string  \n\r", ch);
			send_to_char ("                                (requires '', \"\" around the arguments)\n\r", ch);
			send_to_char (".h                             - get help (this info)                   \n\r", ch);
			send_to_char (".s                             - show string so far                     \n\r", ch);
			send_to_char (".f                             - format (word wrap) string              \n\r", ch);
			send_to_char (".p                             - spell checker              \n\r", ch);
			send_to_char (".c                             - clear string so far                    \n\r", ch);
			send_to_char ("@  or  ~                       - end string editor (end string)         \n\r", ch);
			return;
		}


		send_to_char ("SEdit:  Invalid dot command.\n\r", ch);
		return;
	}

	strcpy (buf, *ch->desc->pString);

	if (*argument == '~' || *argument == '@')
	{
		ch->desc->pString = NULL;
		if (ch->desc->editor == ED_REDITOR)
			do_rshow(ch, "");
		return;
	}

	if (strlen ( *ch->desc->pString ) + strlen (argument) >= (MAX_STRING_LENGTH - 4))
	{
		send_to_char ("String too long, truncating.\n\r", ch);
		strncat (buf, argument, MAX_STRING_LENGTH);
		free_string (*ch->desc->pString);
		*ch->desc->pString = str_dup (buf);
		ch->desc->pString = NULL;
		return;
	}

	strcat (buf, argument);
	strcat (buf, "\n\r");
	free_string (*ch->desc->pString);
	*ch->desc->pString = str_dup (buf);
	return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 */
char *format_string (char *oldstring /*, bool fSpace */ )
{
	char xbuf[MAX_STRING_LENGTH];
	char xbuf2[MAX_STRING_LENGTH];
	char *rdesc;
	int i = 0;
	bool cap = TRUE;

	xbuf[0] = xbuf2[0] = 0;

	i = 0;

	for (rdesc = oldstring; *rdesc; rdesc++)
	{
		if (*rdesc == '\n')
		{
			if (xbuf[i - 1] != ' ')
			{
				xbuf[i] = ' ';
				i++;
			}
		}
		else if (*rdesc == '\r');
		else if (*rdesc == ' ')
		{
			if (xbuf[i - 1] != ' ')
			{
				xbuf[i] = ' ';
				i++;
			}
		} 
		else if (*rdesc == ')')
		{
			if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' && (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
			{
				xbuf[i - 2] = *rdesc;
				xbuf[i - 1] = ' ';
				xbuf[i] = ' ';
				i++;
			}
			else
			{
				xbuf[i] = *rdesc;
				i++;
			}
		}
		else if (*rdesc == '.' || *rdesc == '?' || *rdesc == '!')
		{
			if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' ' && (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' || xbuf[i - 3] == '!'))
			{
				xbuf[i - 2] = *rdesc;
				if (*(rdesc + 1) != '\"')
				{
					xbuf[i - 1] = ' ';
					xbuf[i] = ' ';
					i++;
				}
				else
				{
					xbuf[i - 1] = '\"';
					xbuf[i] = ' ';
					xbuf[i + 1] = ' ';
					i += 2;
					rdesc++;
				}
			}
			else
			{
				xbuf[i] = *rdesc;
				if (*(rdesc + 1) != '\"')
				{
					xbuf[i + 1] = ' ';
					xbuf[i + 2] = ' ';
					i += 3;
				}
				else
				{
					xbuf[i + 1] = '\"';
					xbuf[i + 2] = ' ';
					xbuf[i + 3] = ' ';
					i += 4;
					rdesc++;
				}
			}
			cap = TRUE;
		}
		else
		{
			xbuf[i] = *rdesc;
			if (cap)
			{
				cap = FALSE;
				xbuf[i] = UPPER (xbuf[i]);
			}
			i++;
		}
	}
	xbuf[i] = 0;
	strcpy (xbuf2, xbuf);

	rdesc = xbuf2;

	xbuf[0] = 0;

	for (;;)
	{
		for (i = 0; i < 77; i++)
		{
			if (!*(rdesc + i))
				break;
		}
		if (i < 77)
		{
			break;
		}
		for (i = (xbuf[0] ? 76 : 73); i; i--)
		{
			if (*(rdesc + i) == ' ')
				break;
		}
		if (i)
		{
			*(rdesc + i) = 0;
			strcat (xbuf, rdesc);
			strcat (xbuf, "\n\r");
			rdesc += i + 1;
			while (*rdesc == ' ')
				rdesc++;
		}
		else
		{
			bug ("No spaces", 0);
			*(rdesc + 75) = 0;
			strcat (xbuf, rdesc);
			strcat (xbuf, "-\n\r");
			rdesc += 76;
		}
	}
	while (*(rdesc + i) && (*(rdesc + i) == ' ' || *(rdesc + i) == '\n' || *(rdesc + i) == '\r'))
		i--;
	*(rdesc + i + 1) = 0;
	strcat (xbuf, rdesc);
	if (xbuf[strlen (xbuf) - 2] != '\n')
		strcat (xbuf, "\n\r");

	free_string (oldstring);
	return (str_dup (xbuf));
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes, parenthesis (barring ) ('s) and percentages.
 */
char *first_arg (char *argument, char *arg_first, bool fCase)
{
	char cEnd;

	while (*argument == ' ')
		argument++;

	cEnd = ' ';
	if (*argument == '\'' || *argument == '"' || *argument == '%' || *argument == '(')
	{
		if (*argument == '(')
		{
			cEnd = ')';
			argument++;
		}
		else
			cEnd = *argument++;
	}

	while (*argument != '\0')
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		if (fCase)
			*arg_first = LOWER (*argument);
		else
			*arg_first = *argument;
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while (*argument == ' ')
		argument++;

	return argument;
}



char *string_unpad (char *argument)
{
	char buf[MAX_STRING_LENGTH];
	char *s;

	s = argument;

	while (*s == ' ')
		s++;

	strcpy (buf, s);
	s = buf;

	if (*s != '\0')
	{
		while (*s != '\0')
			s++;
		s--;

		while (*s == ' ')
			s--;
		s++;
		*s = '\0';
	}

	free_string (argument);
	return str_dup (buf);
}



char *string_proper (char *argument)
{
	char *s;

	s = argument;

	while (*s != '\0')
	{
		if (*s != ' ')
		{
			*s = UPPER (*s);
			while (*s != ' ' && *s != '\0')
				s++;
		}
		else
		{
			s++;
		}
	}

	return argument;
}



int arg_count (char *argument)
{
	int total;
	char *s;

	total = 0;
	s = argument;

	while (*s != '\0')
	{
		if (*s != ' ')
		{
			total++;
			while (*s != ' ' && *s != '\0')
				s++;
		}
		else
		{
			s++;
		}
	}

	return total;
}


char *substr(char *sourceString, int startPos, int endPos)
{
	int newStringLength = (endPos - startPos) + 1;
	char temp[newStringLength + 1];
	char *tempString;
	int i;
	tempString = temp;

	for (i = 0; i < newStringLength; i++)
	{
		tempString[i] = sourceString[i + startPos];
	}

	tempString[newStringLength] = '\0';
	
	return tempString;
}


int getSubStrPos(char *sourceString, char *searchString)
{
	int start = -1;	
	int i = 0;
	int j;

	while (i < strlen(sourceString) && start < 0)
	{
		if (sourceString[i] == searchString[0])
		{
			start = i;
			for (j = 1; j < strlen(searchString); j++)
			{
				if (sourceString[i + j] != searchString[j])
					start = -1;
			}
		}
		i++;
	}

	return start;
}


int indexOf(char *sourceString, char argument)
{
	int i;
	
	for (i = 0; i < strlen(sourceString); i++)
	{
		if (sourceString[i] == argument)
			return i;
	}
	
	return -1;
}


int countArgument(char *sourceString, char argument)
{
	int i = 0;
	char *tempString;
	int charPos;

	tempString = sourceString;
	
	charPos = indexOf(tempString, argument);

	while (charPos != -1)
	{
		tempString = substr(tempString, charPos, strlen(tempString));
		i++;
		charPos = indexOf(tempString, argument);
	}
	return i;
}

