/* Code for new Map System     *
 * by Ishmael (Jason Schreier) */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"

extern const sh_int rev_dir[];

#define MAX_MAP		72
#define MAP_DIR		4

const char *map_char[MAX_MAP][MAX_MAP];
int offsets[4][2] = { 
	{-1, 0},	// NORTH (0)
	{ 0, 1},	// EAST  (1)
	{ 1, 0},	// SOUTH (2)
	{ 0, -1} 	// WEST  (3)
};

char *ascii_map[] = {
	"#cI#n",	// SECT_INSIDE		(0)
	"#pC#n",	// SECT_CITY		(1)
	"#mS#n",	// SECT_SEWER		(2)
	"#GF#n",	// SECT_FOREST		(3)
	"#gH#n",	// SECT_HEDGE		(4)
	"#yS#n",	// SECT_SHOP	(5)
	"#BW#n",	// SECT_WATER_SWIM	(6)
	"#rW#n",	// SECT_WATER_NOSWIM	(7)
	"#bU#n",	// SECT_UNDERWATER	(8)
	"#eA#n",	// SECT_AIR		(9)
	"#yO#n",	// SECT_OOC		(10)
	""		// SECT_MAX		(11)
};

// ascii defs
#define MAP_BLANK	"#f#R!#n"
#define MAP_UNKNOWN	" "
#define MAP_SELF	"#R@#n"
#define MAP_OTHER	"#RX#n"
#define MAP_NS_EXIT	"#e|#n"
#define MAP_EW_EXIT	"#e-#n"

// prototypes

void show_map(CHAR_DATA *ch, CHAR_DATA *vict);
void BlankMap(void);
void SendMap(CHAR_DATA *ch, int min, int max);
void MapRoom(CHAR_DATA *ch, ROOM_INDEX_DATA *room, int x, int y, int  
min, int max, CHAR_DATA *vict);
void do_map (CHAR_DATA *ch, char *argument);
void show_legend (CHAR_DATA *ch);

void show_map(CHAR_DATA *ch, CHAR_DATA *vict) {
	int diameter, radius, center, min, max;

	diameter = 20; // should be map size
	radius = (diameter/2);

	center = MAX_MAP/2;		// 36

	min = center - radius;	
	max = center + radius;	

	BlankMap();

	MapRoom(ch, ch->in_room, center, center, min-1, max-1, vict); // start the process

	map_char[center][center] = MAP_SELF;

	SendMap(ch, min, max);

	return;
}

void BlankMap(void) {
	int x, y;

	// Blank out the map
	for (x = 0; x < MAX_MAP; ++x)
		for (y = 0; y < MAX_MAP; ++y)
			map_char[x][y] = MAP_UNKNOWN;
}

// Sends a map to character using given size

void SendMap(CHAR_DATA *ch, int min, int max) {
	int x, y;

	/* every row */
	send_to_char("#g.---------------------------------.#n\r\n", ch);
	for (x = min; x < max; ++x) {
		/* every column */
		send_to_char("#g|#n      ", ch);
		for (y = min; y < max; ++y) 
			send_to_char(map_char[x][y], ch);
		send_to_char("       #g|#n\n", ch);
	}
	send_to_char("#g'---------------------------------'#n\r\n", ch);

	if (IS_SET (ch->act, PLR_LEGEND))
		show_legend(ch);
	else
		send_to_char("#nType #Glegend#n to see what each symbol means.#n\r\n", ch);
	return;
}

void MapRoom(CHAR_DATA *ch, ROOM_INDEX_DATA *room, int x, int y, int min, int max, CHAR_DATA *vict) { 
	ROOM_INDEX_DATA *prospect_room;
	EXIT_DATA *exit, *rev_exit;
	int door, new_x, new_y;

	// can't map above the max!
	if ((x < min) || (y < min) || (x > max) || (y > max))
		return; 

	// let's skip it if we already mapped this
	if (map_char[x][y] != MAP_UNKNOWN)
		return;

	if ((vict != NULL) && (vict->in_room->vnum == room->vnum)) 
		map_char[x][y] = MAP_OTHER;
	else
		map_char[x][y] = ascii_map[room->sector_type];

	for (door = 0; door < MAP_DIR; door++) {
		exit = room->exit[door];
		if (!exit)
			continue;

		if ((prospect_room = exit->to_room))
			rev_exit = prospect_room->exit[rev_dir[door]];
	
		/* if not a two-way exit */
		if (!rev_exit || (rev_exit->to_room != room)) {
			map_char[x][y] = MAP_BLANK;	
			return;
		}

		new_x = (x + offsets[door][0]);
		new_y = (y + offsets[door][1]);

		// now we account for exits
		if (door == DIR_NORTH || door == DIR_SOUTH)
			map_char[new_x][new_y] = MAP_NS_EXIT;
		else if (door == DIR_WEST || door == DIR_EAST)
			map_char[new_x][new_y] = MAP_EW_EXIT;
		else
			send_to_char("Directional error with mapping!\r\n", ch);

		// increment the offsets again
		new_x = (new_x + offsets[door][0]);
		new_y = (new_y + offsets[door][1]);		

		if (map_char[new_x][new_y] == MAP_UNKNOWN)
			MapRoom(ch, prospect_room, new_x, new_y, min, max, vict);
	}
	return;
}




void do_map (CHAR_DATA * ch, char *argument) {
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *vict;

	argument = one_argument (argument, arg);

	if ((arg[0] != '\0') && ((vict = get_char_world (ch, arg)) != NULL))
		show_map(ch, vict);
	else
		show_map(ch, NULL);
}

void show_legend(CHAR_DATA *ch) {
	int counter, columns = 0;
	char buf[MAX_STRING_LENGTH];

	send_to_char("#yLEGEND\r\n", ch);
	
	for (counter = 0; counter < SECT_MAX; counter++) {
		sprintf(buf, "%s %-15.15s %s", 
ascii_map[counter], room_sector(counter), !(++columns % 3) ? "\r\n" : " ");
		send_to_char(buf, ch);
	}
	send_to_char("\r\n", ch);
}

void do_legend (CHAR_DATA *ch, char *argument) {
	int counter, columns = 0;
	char buf[MAX_STRING_LENGTH];

	send_to_char("#yLEGEND\r\n", ch);
	
	for (counter = 0; counter < SECT_MAX; counter++) {
		sprintf(buf, "%s %-15.15s %s", 
ascii_map[counter], room_sector(counter), !(++columns % 3) ? "\r\n" : " ");
		send_to_char(buf, ch);
	}
	send_to_char("\r\n", ch);
}
