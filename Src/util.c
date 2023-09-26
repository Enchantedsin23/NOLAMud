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
 *  Dystopia Mud improvements copyright (C) 2000, 2001 by Brian Graversen  *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"


bool gFound;

#define RID ROOM_INDEX_DATA

bool  examine_room         args (( RID *pRoom, RID *tRoom, AREA_DATA *pArea, int steps ));
void  dijkstra             args (( RID *chRoom, RID *victRoom ));
RID  *heap_getMinElement   args (( HEAP *heap ));
HEAP *init_heap            args (( RID *root ));

/*
 * Knuth-Morris-Pratt Pattern Matching Algorithm (sensitive)
 */
bool is_contained2(const char *astr, const char *bstr)
{
  int n, m;

  m = strlen(astr);
  n = strlen(bstr);

  /* if the pattern is longer than the string */
  if (m > n) return FALSE;

  {
    int f[m], i = 1, j = 0;

    f[0] = 0;

    /* calculating the error fuction f[] */
    while (i < m)
    {
      if (astr[j] == astr[i])
      {
        f[i] = j + 1;
        i++; j++;
      }
      else if (j > 0) j = f[j - 1];
      else
      {
        f[i] = 0;
        i++;
      }
    }

    j = 0;

    /* KMP algorith */
    for (i = 0; i < n; i++)
    {
      while (j > 0 && astr[j] != bstr[i])
        j = f[j-1];
      if (astr[j] == bstr[i]) j++;
      if (j == m) return TRUE;
    }
  }
  return FALSE;
}

/*  
 * Knuth-Morris-Pratt Pattern Matching Algorithm (insensitive)
 */
bool is_contained(const char *astr, const char *bstr)
{
  int n, m;
        
  m = strlen(astr);
  n = strlen(bstr);
      
  /* if the pattern is longer than the string */
  if (m > n) return FALSE;
        
  {
    int f[m], i = 1, j = 0;
     
    f[0] = 0;
    
    /* calculating the error fuction f[] */
    while (i < m)
    {
      if (UPPER(astr[j]) == UPPER(astr[i]))
      {
        f[i] = j + 1;
        i++; j++;  
      }
      else if (j > 0) j = f[j - 1];
      else
      {
        f[i] = 0;
        i++;
      }
    }
  
    j = 0;
    
    /* KMP algorith */
    for (i = 0; i < n; i++)
    {
      while (j > 0 && UPPER(astr[j]) != UPPER(bstr[i]))
        j = f[j-1];  
      if (UPPER(astr[j]) == UPPER(bstr[i])) j++;
      if (j == m) return TRUE;
    }
  }
  return FALSE;
}

int strlen2(const char *s)
{
  int i, b, count=0;

  if (s[0] == '\0') return 0;
  b = strlen(s);
  for (i = 0; i < b; i++)
  {
    if (s[i] == '#') count++;
  }
  return (b + 7 * count);
}


int get_ratio(CHAR_DATA *ch)
{
  long ratio;

  if (IS_NPC(ch)) return 0;
  if ((ch->pkill + ch->pdeath) == 0) ratio = 0; // to avoid divide by zero.
  else if (ch->pkill > ch->pdeath)
    ratio = ch->pkill * 100 * ((ch->pkill * ch->pkill) - (ch->pdeath * ch->pdeath))/((ch->pkill + ch->pdeath) * (ch->pkill + ch->pdeath));
  else if (ch->pkill > 0)
    ratio = (-100) * (ch->pdeath - ch->pkill) / ch->pkill;
  else
    ratio = (-100) * ch->pdeath;
  return (int) ratio;
}


/*
 * Writes a string straight to stderr
 */
void log_string2(const char *str)
{
  char *strtime;

  strtime = ctime(&current_time);
  strtime[strlen(strtime)-1] = '\0';
  fprintf(stderr, "%s :: %s\n", strtime, str);

  return;
}

int calc_ratio(int a, int b)
{
  int ratio;

  if (b == 0 && a > 0) ratio = 100;
  else if ((a + b) != 0) ratio = (100*a)/(a+b);
  else ratio = 0;
  return ratio;
}

RID *heap_getMinElement(HEAP *heap)
{
  RID *tRoom;
  RID *pRoom;
  bool done = FALSE;
  int i = 1;

  /* this is the element we wish to return */
  pRoom = heap->knude[1];

//  if (pRoom->steps == 2 * MAX_KNUDE)
 //   bug("Removing room with max steps : %d", pRoom->vnum);

  /* We move the last vertice to the front */
  heap->knude[1] = heap->knude[heap->iVertice];
  heap->knude[1]->heap_index = 1;

  /* Decrease the size of the heap and remove the last entry */
  heap->knude[heap->iVertice] = NULL;
  heap->iVertice--;

  /* Swap places till it fits */
  while(!done)
  {
    if (heap->knude[i] == NULL)
      done = TRUE;
    else if (heap->knude[2*i] == NULL)
      done = TRUE;
    else if (heap->knude[2*i+1] == NULL)
    {
      if (heap->knude[i]->steps > heap->knude[2*i]->steps)
      {
        tRoom                        = heap->knude[i];
        heap->knude[i]               = heap->knude[2*i];
        heap->knude[i]->heap_index   = i;
        heap->knude[2*i]             = tRoom;
        heap->knude[2*i]->heap_index = 2*i;
        i = 2*i;
      }
      done = TRUE;
    }
    else if (heap->knude[i]->steps <= heap->knude[2*i]->steps &&
        heap->knude[i]->steps <= heap->knude[2*i+1]->steps)
      done = TRUE;
    else if (heap->knude[2*i]->steps <= heap->knude[2*i+1]->steps)
    {
      tRoom                        = heap->knude[i];
      heap->knude[i]               = heap->knude[2*i];
      heap->knude[i]->heap_index   = i;
      heap->knude[2*i]             = tRoom;
      heap->knude[2*i]->heap_index = 2*i;
      i = 2*i;
    }
    else
    {
      tRoom                          = heap->knude[i];
      heap->knude[i]                 = heap->knude[2*i+1];
      heap->knude[i]->heap_index     = i;
      heap->knude[2*i+1]             = tRoom;
      heap->knude[2*i+1]->heap_index = 2*i+1;
      i = 2*i+1;
    }
  }

  /* return the element */
  return pRoom;
}

HEAP *init_heap(RID *root)
{
  AREA_DATA *pArea;
  RID *pRoom;
  HEAP * heap;
  int i, size, vnum;

  if ((pArea = root->area) == NULL) return NULL;
  size = pArea->uvnum - pArea->lvnum;

  if (size >= MAX_KNUDE)
  {
    bug("Init_heap: Size %d exceeds MAX_KNUDE", size);
    return NULL;
  }

  heap = calloc(1, sizeof(*heap));

  /* we want the root at the beginning */
  heap->knude[1]             = root;
  heap->knude[1]->steps      = 0;
  heap->knude[1]->heap_index = 1;

  /* initializing the rest of the heap */
  for (i = 2, vnum = pArea->lvnum; vnum < pArea->uvnum; vnum++)
  {
    if ((pRoom = get_room_index(vnum)))
    {
      if (pRoom == root) continue;
      heap->knude[i]             = pRoom;
      heap->knude[i]->steps      = 2 * MAX_KNUDE;
      heap->knude[i]->heap_index = i;
      i++;
    }
  }

  heap->iVertice = i-1;

  /* setting the rest to NULL */
  for (; i < MAX_KNUDE; i++)
    heap->knude[i] = NULL;

  return heap;
}

void dijkstra(RID *chRoom, RID *victRoom)
{
  RID *pRoom;
  RID *tRoom;
  RID *xRoom;
  HEAP *heap;
  int door, x;
  bool stop;

  /* allocate a new heap */
  heap = init_heap(chRoom);

  /* find shortest amounts of steps to each room */
  while (heap->iVertice)
  {
    if ((pRoom = heap_getMinElement(heap)) == NULL)
    {
      bug("Dijstra: Getting NULL room", 0);
      return;
    }
    if (pRoom == victRoom)
      gFound = TRUE;

    /* update all exits */
    for (door = 0; door < 6; door++)
    {
      if (pRoom->exit[door] == NULL) continue;   
      if (pRoom->exit[door]->to_room == NULL) continue;

      /* update step count, and swap in the heap */
      if (pRoom->exit[door]->to_room->steps > pRoom->steps + 1)
      {
        xRoom = pRoom->exit[door]->to_room;
        xRoom->steps = pRoom->steps + 1;
        stop = FALSE;
        while ((x = xRoom->heap_index) != 1 && !stop)
        {
          if (heap->knude[x/2]->steps > xRoom->steps)
          {
            tRoom                       = heap->knude[x/2];
            heap->knude[x/2]            = xRoom;
            xRoom->heap_index           = xRoom->heap_index/2;
            heap->knude[x]              = tRoom;
            heap->knude[x]->heap_index  = x;
          }
          else stop = TRUE;
        }
      }
    }
  }

  /* free the heap */
  free(heap);
}

char *line_indent(char *text, int wBegin, int wMax)
{
  static char buf[MAX_STRING_LENGTH];
  char *ptr;
  char *ptr2;
  int count = 0;
  bool stop = FALSE;
  int wEnd;

  buf[0] = '\0';
  ptr = text;
  ptr2 = buf;

  while (!stop)
  {
    if (count == 0)
    {
      if (*ptr == '\0') wEnd = wMax - wBegin;
      else if (strlen(ptr) < (wMax - wBegin)) wEnd = wMax - wBegin;
      else
      {
        int x = 0;

        while (*(ptr + (wMax - wBegin - x)) != ' ') x++;
        wEnd = wMax - wBegin - (x - 1);
        if (wEnd < 1) wEnd = wMax - wBegin;
      }
    }
    if (count == 0 && *ptr == ' ') ptr++;
    else if (++count != wEnd)
    {
      if ((*ptr2++ = *ptr++) == '\0') stop = TRUE;
    }
    else if (*ptr == '\0')
    {
      stop = TRUE;
      *ptr2 = '\0';
    }
    else
    {
      int k;

      count = 0;
      *ptr2++ = '\n';
      *ptr2++ = '\r';
      for (k = 0; k < wBegin; k++)
        *ptr2++ = ' ';
    }
  }
  return buf;
}

char *pathfind(CHAR_DATA *ch, CHAR_DATA *victim)
{
  int const exit_names [] = { 'n', 'e', 's', 'w', 'u', 'd' };
  RID *pRoom;
  AREA_DATA *pArea;
  static char path[MAX_STRING_LENGTH]; // should be plenty.
  int iPath = 0, vnum, door;
  bool found;

  if (!ch->in_room || !victim->in_room) return NULL;
  if (ch->in_room == victim->in_room) return NULL;
  if ((pArea = ch->in_room->area) != victim->in_room->area) return NULL;

  /* initialize all rooms in the area */
  for (vnum = pArea->lvnum; vnum < pArea->uvnum; vnum++)
  {
    if ((pRoom = get_room_index(vnum)))
    {
      pRoom->visited = FALSE;
      for (door = 0; door < 6; door++)
      {
        if (pRoom->exit[door] == NULL) continue;
        pRoom->exit[door]->color = FALSE;
      }
    }
  }

  /* initialize variables */
  pRoom = ch->in_room;
  gFound = FALSE;

  /* In the first run, we only count steps, no coloring is done */
  dijkstra(pRoom, victim->in_room);

  /* If the target room was never found, we return NULL */
  if (!gFound) return NULL;

  /* in the second run, we color the shortest path using the step counts */
  if (!examine_room(pRoom, victim->in_room, pArea, 0))
    return NULL;

  /* then we follow the trace */
  while (pRoom != victim->in_room)
  {
    found = FALSE;
    for (door = 0; door < 6 && !found; door++)  
    {
      if (pRoom->exit[door] == NULL) continue;
      if (pRoom->exit[door]->to_room == NULL) continue;
      if (!pRoom->exit[door]->color) continue;

      pRoom->exit[door]->color = FALSE;
      found = TRUE;
      path[iPath] = exit_names[door];
      iPath++;
      pRoom = pRoom->exit[door]->to_room;
    }
    if (!found)
    {
      bug("Pathfind: Fatal Error in %d.", pRoom->vnum); 
      return NULL;
    }
  }  

  path[iPath] = '\0';
  return path;
}

char *get_exits(CHAR_DATA *ch)
{
  extern char *const dir_name[];
  static char buf[MAX_STRING_LENGTH];
  EXIT_DATA *pexit;
  bool found;
  int door;

  buf[0] = '\0';

//  if (!check_blind(ch)) return "";
 
  sprintf(buf, "#0[#GExits#9:#C");

  found = FALSE;
  for (door = 0; door <= 5; door++)
  {
    if ((pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL)
    {
      found = TRUE;
      if (IS_SET(pexit->exit_info, EX_CLOSED))
      {
        strcat(buf, " #0(#C");
        strcat(buf, dir_name[door]);
        strcat(buf, "#0)#C");
      }
      else
      {  
        strcat(buf, " ");
        strcat(buf, dir_name[door]);
      }
    }
  }
  if (!found)
    strcat(buf, " none");
  strcat(buf, "#0]#n\n\r");

  return buf;
}

bool examine_room(RID *pRoom, RID *tRoom, AREA_DATA *pArea, int steps)
{
  int door;

  /* been here before, out of the area or can we get here faster */
  if (pRoom->area != pArea) return FALSE;
  if (pRoom->visited) return FALSE;
  if (pRoom->steps < steps) return FALSE;

  /* Have we found the room we are searching for */
  if (pRoom == tRoom)
    return TRUE;

  /* mark the room so we know we have been here */
  pRoom->visited = TRUE;

  /* Depth first traversel of all exits */
  for (door = 0; door < 6; door++)
  {
    if (pRoom->exit[door] == NULL) continue;
    if (pRoom->exit[door]->to_room == NULL) continue;

    /* assume we are walking the right way */
    pRoom->exit[door]->color = TRUE;

    /* recursive return */
    if (examine_room(pRoom->exit[door]->to_room, tRoom, pArea, steps + 1))
      return TRUE;

    /* it seems we did not walk the right way */
    pRoom->exit[door]->color = FALSE;
  }
  return FALSE;
}

void do_pathfind(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char *path;

  one_argument(argument, arg);
  if ((victim = get_char_world(ch, arg)) == NULL) return;

  if ((path = pathfind(ch, victim)) != NULL)
    sprintf(buf, "#gPath to target:#n %s\n\r", path);
  else
    sprintf(buf, "#gPath to target:#n Unknown.\n\r");
  send_to_char(buf, ch);

  return;
}

