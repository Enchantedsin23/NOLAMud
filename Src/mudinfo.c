/*
 * mudinfo.c
 *
 * A quick piece of code that allows mudadmins to track the activity of
 * their mud, and create statistics of that activity.
 *
 * Code by Brian Graversen aka Jobo
 */
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "merc.h"
#include "mudinfo.h"

int mudinfo[MUDINFO_MAX];
int total_output = 0;

void do_mudinfo(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int a, b, c;

  if (IS_NPC(ch)) return;

  sprintf(buf, "Players : Average Online              : %d\n\r",
    (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS])/mudinfo[MUDINFO_UPDATED]);
  send_to_char(buf, ch);
  sprintf(buf, "Players : Peak Online                 : %d\n\r", mudinfo[MUDINFO_PEAK_USERS]);
  send_to_char(buf, ch);

  /*
  ratio = (100 * mudinfo[MUDINFO_MCCP_USERS] / (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS]));
  sprintf(buf, "Players : Mccp Users                  : %d %%\n\r", ratio);
  send_to_char(buf, ch);
  */

  /*
   * a : Amount of kbit data send totally.
   * b : Amount of kbit data send each 10 seconds
   * c : The fractional part
   */
  a = mudinfo[MUDINFO_MBYTE] * 1024 + mudinfo[MUDINFO_BYTE] / 1024;
  b = a / (mudinfo[MUDINFO_UPDATED] * 3);
  c = b / 10;
  c = c * 10;
  c = b - c;
  sprintf(buf, "Datatransfer : Average Rate           : %d.%d kb/sec\n\r", b/10, c);
  send_to_char(buf, ch);
  b =  mudinfo[MUDINFO_DATA_PEAK] / (3 * 1024);
  c = b / 10;
  c = c * 10;
  c = b - c;
  sprintf(buf, "Datatransfer : Peak Rate              : %d.%d kb/sec\n\r", b/10, c);
  send_to_char(buf, ch); 
  sprintf(buf, "Datatransfer : This Month             : %d MB\n\r", mudinfo[MUDINFO_MBYTE]);
  send_to_char(buf, ch);
  sprintf(buf, "Datatransfer : This Boot              : %d MB\n\r", mudinfo[MUDINFO_MBYTE_S]);
  send_to_char(buf, ch);
  return;  
}

void load_mudinfo()
{
  FILE *fp;
  int i;

  if ((fp = fopen("../area/mudinfo.txt", "r")) == NULL)
  {
//    log_string("Error: mudinfo.txt not found - Creating One!");
    for (i = 0; i < MUDINFO_MAX; i++)
    {
      mudinfo[i] = 0;
    }
    save_mudinfo();
    return;
  }

  for (i = 0; i < (MUDINFO_MAX - 2); i++)
  {
    mudinfo[i] = fread_number(fp);
  }
  mudinfo[MUDINFO_MAX - 2] = 0;
  mudinfo[MUDINFO_MAX - 1] = 0;
  fclose(fp);
}

void save_mudinfo()
{
  FILE *fp;
  int i;
  
  if ((fp = fopen("../area/mudinfo.txt","w")) == NULL)
  {
//    log_string("Error writing to mudinfo.txt");
    return;
  }
    
  for (i = 0; i < (MUDINFO_MAX - 2); i++)
  {
    fprintf(fp, "%d\n", mudinfo[i]);
  }
  fclose(fp);
}

void write_mudinfo_database()
{
  FILE *fp;
  int avusers, a, b, c;
      
  if ((fp = fopen("../area/mud_data.txt","a")) == NULL)
  {
//    log_string("Error writing to mud_data.txt");
    return;
  }
  
  /* Calculate the ratio of users that use mccp */
  /*
  ratio =
    (100 * mudinfo[MUDINFO_MCCP_USERS] /
    (mudinfo[MUDINFO_MCCP_USERS] +
     mudinfo[MUDINFO_OTHER_USERS]));
  */

  /* Calculate the amount of average players online */
  avusers =
    (mudinfo[MUDINFO_MCCP_USERS] +
     mudinfo[MUDINFO_OTHER_USERS]) /
     mudinfo[MUDINFO_UPDATED]; 
  
  /* Calculate the average tranfer rate in kbyte */
  a = mudinfo[MUDINFO_MBYTE] * 1024 + mudinfo[MUDINFO_BYTE] / 1024;
  b = a / (mudinfo[MUDINFO_UPDATED] * 3);
  c = b / 10;
  c = c * 10;
  c = b - c;
  
  /* Append it all to the file */
  fprintf(fp, "\nMudinfo Database Entry\n");
  fprintf(fp, "Average Online Users       %d\n", avusers);
  //  fprintf(fp, "Mccp Ratio                 %d %%\n", ratio);
  fprintf(fp, "Amount of MB send          %d MB\n", mudinfo[MUDINFO_MBYTE]);
  fprintf(fp, "Datatransfer Average       %d.%d\n", b/10, c);
  
  fclose(fp);
}

void increase_total_output(int clenght)
{
  total_output += clenght;
}

void update_mudinfo()
{
  DESCRIPTOR_DATA *d;
  int i, pcount = 0;
  
  /* Increase update count */
  mudinfo[MUDINFO_UPDATED]++;
 
  /*
   * If it's been roughly one month, then update the database,
   * and clear out the old information
   */
  if (mudinfo[MUDINFO_UPDATED] > 86500)
  {
    write_mudinfo_database();
    for (i = 0; i < (MUDINFO_MAX - 2); i++)
    {
      mudinfo[i] = 0;
    }
//    log_string("Mudinfo database updated.");
  }

  /* Outdate the output data */
  
  if (total_output > mudinfo[MUDINFO_DATA_PEAK]) mudinfo[MUDINFO_DATA_PEAK] = total_output;

  /* The stored data */
  if (mudinfo[MUDINFO_BYTE] > 1048576) // 1 megabyte
  {
    mudinfo[MUDINFO_MBYTE]++;
    mudinfo[MUDINFO_BYTE] -= 1048576;
  }
  mudinfo[MUDINFO_BYTE] += total_output;
 
  /* The temp data */
  if (mudinfo[MUDINFO_BYTE_S] > 1048576) // 1 megabyte
  {
    mudinfo[MUDINFO_MBYTE_S]++;
    mudinfo[MUDINFO_BYTE_S] -= 1048576;
  }
  mudinfo[MUDINFO_BYTE_S] += total_output;

  /* We clear the counter */
  total_output = 0;  
     
  for (d = descriptor_list; d; d = d->next) 
  {
    if (d->connected == CON_PLAYING)
    {
      if (d->character)
      {
        if (d->character->level < 7)
        {
          pcount++;

	  /*
          if (d->out_compress) mudinfo[MUDINFO_MCCP_USERS]++;
          else
	  */

          mudinfo[MUDINFO_OTHER_USERS]++;
        }
      }
    }
  }

  if (pcount > mudinfo[MUDINFO_PEAK_USERS]) mudinfo[MUDINFO_PEAK_USERS] = pcount;
   
  save_mudinfo();
}
