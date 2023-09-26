/*
 * mudinfo.h
 *
 * A quick piece of code that allows mudadmins to track the activity of
 * their mud, and create statistics of that activity.
 *
 * Code by Brian Graversen aka Jobo
 */


#define MUDINFO_UPDATED      0
#define MUDINFO_MCCP_USERS   1
#define MUDINFO_OTHER_USERS  2
#define MUDINFO_PEAK_USERS   3
#define MUDINFO_MBYTE        4
#define MUDINFO_BYTE         5
#define MUDINFO_DATA_PEAK    6
#define MUDINFO_MBYTE_S      7
#define MUDINFO_BYTE_S       8
#define MUDINFO_MAX          9


/*
 * Internal functions
 */

void  save_mudinfo             args (( void ));
void  write_mudinfo_database   args (( void ));


/*
 * Let's define the needed array
 */
int   mudinfo[MUDINFO_MAX];
int   total_output;
