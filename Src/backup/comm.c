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
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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
 
/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>

#include <sys/wait.h>      // we need these to call wait()
#include "merc.h"


/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern int malloc_debug args ((int));
extern int malloc_verify args ((void));
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>   /* for RLIMIT_NOFILE */
#endif

#if defined(apollo)
#undef __attribute
#endif

/*
 * Socket and TCP/IP stuff.
 */
#if   defined(macintosh) || defined(MSDOS)
const char echo_off_str[] = { '\0' };
const char echo_on_str[] = { '\0' };
const char go_ahead_str[] = { '\0' };
#endif

#if   defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
#include "telnet.h"
const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] = { IAC, GA, '\0' };
#endif
/*
 * OS-dependent declarations.
 */
#if   defined(_AIX)
#include <sys/select.h>
int accept args ((int s, struct sockaddr * addr, int *addrlen));
int bind args ((int s, struct sockaddr * name, int namelen));
void bzero args ((char *b, int length));
int getpeername args ((int s, struct sockaddr * name, int *namelen));
int getsockname args ((int s, struct sockaddr * name, int *namelen));
int gettimeofday args ((struct timeval * tp, struct timezone * tzp));
int listen args ((int s, int backlog));
int setsockopt args ((int s, int level, int optname, void *optval, int optlen));
int socket args ((int domain, int type, int protocol));
#endif

#if   defined(apollo)
#include <unistd.h>
void bzero args ((char *b, int length));
#endif

#if   defined(__hpux)
int accept args ((int s, void *addr, int *addrlen));
int bind args ((int s, const void *addr, int addrlen));
void bzero args ((char *b, int length));
int getpeername args ((int s, void *addr, int *addrlen));
int getsockname args ((int s, void *name, int *addrlen));
int gettimeofday args ((struct timeval * tp, struct timezone * tzp));
int listen args ((int s, int backlog));
int setsockopt args ((int s, int level, int optname, const void *optval, int optlen));
int socket args ((int domain, int type, int protocol));
#endif

#if   defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if   defined(linux)
#if 0
int accept args ((int s, struct sockaddr * addr, int *addrlen));
int bind args ((int s, struct sockaddr * name, int namelen));
#endif
int close args ((int fd));
//int   getpeername     args( ( int s, struct sockaddr *name, int *namelen ) );
//int   getsockname     args( ( int s, struct sockaddr *name, int *namelen ) );
int gettimeofday args ((struct timeval * tp, struct timezone * tzp));
//int   listen          args( ( int s, int backlog ) );
/* kavirpoint: put this in for nin site.
int   read      args( ( int fd, char *buf, int nbyte ) );
*/
int select args ((int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout));
int socket args ((int domain, int type, int protocol));
/* kavirpoint: put this in for nin site.
int   write      args( ( int fd, char *buf, int nbyte ) );
*/
#endif

#if   defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct timeval
{
   time_t tv_sec;
   time_t tv_usec;
};
#if   !defined(isascii)
#define   isascii(c)      ( (c) < 0200 )
#endif
static long theKeys[4];

int gettimeofday args ((struct timeval * tp, void *tzp));
#endif

#if   defined(MIPS_OS)
extern int errno;
#endif

#if   defined(MSDOS)
int gettimeofday args ((struct timeval * tp, void *tzp));
int kbhit args ((void));
#endif

#if   defined(NeXT)
int close args ((int fd));
int fcntl args ((int fd, int cmd, int arg));
#if   !defined(htons)
u_short htons args ((u_short hostshort));
#endif
#if   !defined(ntohl)
u_long ntohl args ((u_long hostlong));
#endif
int read args ((int fd, char *buf, int nbyte));
int select args ((int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout));
int write args ((int fd, char *buf, int nbyte));
#endif

#if   defined(sequent)
int accept args ((int s, struct sockaddr * addr, int *addrlen));
int bind args ((int s, struct sockaddr * name, int namelen));
int close args ((int fd));
int fcntl args ((int fd, int cmd, int arg));
int getpeername args ((int s, struct sockaddr * name, int *namelen));
int getsockname args ((int s, struct sockaddr * name, int *namelen));
int gettimeofday args ((struct timeval * tp, struct timezone * tzp));
#if   !defined(htons)
u_short htons args ((u_short hostshort));
#endif
int listen args ((int s, int backlog));
#if   !defined(ntohl)
u_long ntohl args ((u_long hostlong));
#endif
int read args ((int fd, char *buf, int nbyte));
int select args ((int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout));
int setsockopt args ((int s, int level, int optname, caddr_t optval, int optlen));
int socket args ((int domain, int type, int protocol));
int write args ((int fd, char *buf, int nbyte));
#endif

/*
 * This includes Solaris SYSV as well.
 */
#if defined(sun)
int accept args ((int s, struct sockaddr * addr, int *addrlen));
int bind args ((int s, struct sockaddr * name, int namelen));
void bzero args ((char *b, int length));
int close args ((int fd));
int getpeername args ((int s, struct sockaddr * name, int *namelen));
int getsockname args ((int s, struct sockaddr * name, int *namelen));
int gettimeofday args ((struct timeval * tp, struct timezone * tzp));
int listen args ((int s, int backlog));
int read args ((int fd, char *buf, int nbyte));
int select args ((int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout));
#if defined(SYSV)
int setsockopt args ((int s, int level, int optname, const char *optval, int optlen));
#else
int setsockopt args ((int s, int level, int optname, void *optval, int optlen));
#endif
int socket args ((int domain, int type, int protocol));
int write args ((int fd, char *buf, int nbyte));
#endif

#if defined(ultrix)
int accept args ((int s, struct sockaddr * addr, int *addrlen));
int bind args ((int s, struct sockaddr * name, int namelen));
void bzero args ((char *b, int length));
int close args ((int fd));
int getpeername args ((int s, struct sockaddr * name, int *namelen));
int getsockname args ((int s, struct sockaddr * name, int *namelen));
int gettimeofday args ((struct timeval * tp, struct timezone * tzp));
int listen args ((int s, int backlog));
int read args ((int fd, char *buf, int nbyte));
int select args ((int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout));
int setsockopt args ((int s, int level, int optname, void *optval, int optlen));
int socket args ((int domain, int type, int protocol));
int write args ((int fd, char *buf, int nbyte));
#endif


void show_doc args ((CHAR_DATA * ch ));

/*
 * Global variables.
 */
DESCRIPTOR_DATA *descriptor_free;   /* Free list for descriptors    */
DESCRIPTOR_DATA *descriptor_list;   /* All open descriptors         */
DESCRIPTOR_DATA *d_next;   /* Next descriptor in loop      */
FILE *fpReserve;      /* Reserved file handle         */
bool god;         /* All new chars are gods!      */
bool merc_down;         /* Shutdown                     */
bool wizlock;         /* Game is wizlocked            */
char str_boot_time[MAX_INPUT_LENGTH];
char crypt_pwd[MAX_INPUT_LENGTH];
time_t current_time;      /* Time of this pulse           */
int chars_logged_in;      /* Stores the types of char     */
int total_number_login;      /* Stores the total logins      */
int high_number_login;      /* Stores most logins at once   */
char first_place[40];      /* Name of best score on        */
char second_place[40];      /* Name of 2nd best score on    */
char third_place[40];      /* Name of 3rd best score on    */
int score_1 = 0;      /* Actual scores                */
int score_2 = 0;      /* Actual scores                */
int score_3 = 0;      /* Actual scores                */
int world_affects = 0;      /* World Affect bits            */
int bootcount = 0;      /* Used for timed reboots       */
int chainspell = 0;      /* Used for mage chain spells   */

/* Colour scale char list - Calamar */

char *scale[SCALE_COLS] = {
   L_RED,
   L_BLUE,
   L_GREEN,
   YELLOW
};

/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos args ((void));
bool read_from_descriptor args ((DESCRIPTOR_DATA * d));
bool write_to_descriptor args ((int desc, char *txt, int length));
#endif

#if defined(unix)
void game_loop_unix args ((int control));
int init_socket args ((int port));
void new_descriptor args ((int control));
void init_descriptor args ((DESCRIPTOR_DATA * dnew, int desc));
bool read_from_descriptor args ((DESCRIPTOR_DATA * d));
bool write_to_descriptor args ((int desc, char *txt, int length));
#endif




/*
 * Other local functions (OS-independent).
 */
bool check_reconnect args ((DESCRIPTOR_DATA * d, char *name, bool fConn));
bool check_kickoff args ((DESCRIPTOR_DATA * d, char *name, bool fConn));
bool check_playing args ((DESCRIPTOR_DATA * d, char *name));
int main args ((int argc, char **argv));
void nanny args ((DESCRIPTOR_DATA * d, char *argument));
bool process_output args ((DESCRIPTOR_DATA * d, bool fPrompt));
void read_from_buffer args ((DESCRIPTOR_DATA * d));
void stop_idling args ((CHAR_DATA * ch));
void bust_a_prompt args ((DESCRIPTOR_DATA * d));

int proc_pid;
int port;

#if defined(unix)
int control;
#endif
DUMMY_ARG        *     dummy_free;
DUMMY_ARG        *     dummy_list;

int main (int argc, char **argv)
{
   struct timeval now_time;
   bool fCopyOver = FALSE;

   /*
    * Memory debugging if needed.
    */
#if defined(MALLOC_DEBUG)
   malloc_debug (2);
#endif

#ifdef RLIMIT_NOFILE
#ifndef min
# define min(a,b)     (((a) < (b)) ? (a) : (b))
#endif
   {
      struct rlimit rlp;
      (void) getrlimit (RLIMIT_NOFILE, &rlp);
      rlp.rlim_cur = min (rlp.rlim_max, FD_SETSIZE);
      (void) setrlimit (RLIMIT_NOFILE, &rlp);
   }
#endif

   /*
    * Crash recovery by Mandrax
    */

   proc_pid = getpid ();

   /*
    * Init time and encryption.
    */
   gettimeofday (&now_time, NULL);
   current_time = (time_t) now_time.tv_sec;
   strcpy (str_boot_time, ctime (&current_time));
   strcpy (crypt_pwd, "KaG7eS0AGJ82s");
   strcpy (first_place, "Nobody");
   strcpy (second_place, "Nobody");
   strcpy (third_place, "Nobody");
   /*
    * Macintosh console initialization.
    */
#if defined(macintosh)
   console_options.nrows = 31;
   cshow (stdout);
   csetmode (C_RAW, stdin);
   cecho2file ("log file", 1, stderr);
#endif

   /*
    * Reserve one channel for our use.
    */
   if ((fpReserve = fopen (NULL_FILE, "r")) == NULL)
   {
      perror (NULL_FILE);
      exit (1);
   }

   /*
    * Get the port number.
    */
   port = DEFAULT_PORT;
   if (argc > 1)
   {
      if (!is_number (argv[1]))
      {
         fprintf (stderr, "Usage: %s [port #]\n", argv[0]);
         exit (1);
      }
      port = atoi (argv[1]);
   }

   if (argv[2] && argv[2][0])
   {
      fCopyOver = TRUE;
      control = atoi (argv[3]);
   }
   else
      fCopyOver = FALSE;

   /*
    * Run the game.
    */
#if defined(macintosh) || defined(MSDOS)
   boot_db ();
   log_string ("CoS is up", 
CHANNEL_LOG_NORMAL);
   game_loop_mac_msdos ();
#endif

#if defined(unix)
   if (!fCopyOver)
      control = init_socket (port);
   boot_db (fCopyOver);
   sprintf (log_buf, "Streetlight Manifesto has docked at port %d.", port);
   log_string (log_buf, CHANNEL_LOG_NORMAL);
   game_loop_unix (control);
   close (control);
#endif

   /*
    * That's all, folks.
    */
   log_string ("Normal termination of game.", CHANNEL_LOG_NORMAL);
   exit (0);
   return 0;
}



#if defined(unix)
int init_socket (int port)
{
   static struct sockaddr_in sa_zero;
   struct sockaddr_in sa;
   int x;
   int fd;

   if ((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
   {
      perror ("Init_socket: socket");
      exit (1);
   }

   if (setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, (char *) &x, sizeof (x)) < 0)
   {
      perror ("Init_socket: SO_REUSEADDR");
      close (fd);
      exit (1);
   }

#if defined(SO_DONTLINGER) && !defined(SYSV)
   {
      struct linger ld;

      ld.l_onoff = 1;
      ld.l_linger = 1000;

      if (setsockopt (fd, SOL_SOCKET, SO_DONTLINGER, (char *) &ld, sizeof (ld)) < 0)
      {
         perror ("Init_socket: SO_DONTLINGER");
         close (fd);
         exit (1);
      }
   }
#endif

   sa = sa_zero;
   sa.sin_family = AF_INET;
   sa.sin_port = htons (port);

   if (bind (fd, (struct sockaddr *) &sa, sizeof (sa)) < 0)
   {
      perror ("Init_socket: bind");
      close (fd);
      exit (1);
   }

   if (listen (fd, 3) < 0)
   {
      perror ("Init_socket: listen");
      close (fd);
      exit (1);
   }

   return fd;
}
#endif



#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos (void)
{
   struct timeval last_time;
   struct timeval now_time;
   static DESCRIPTOR_DATA dcon;

   gettimeofday (&last_time, NULL);
   current_time = (time_t) last_time.tv_sec;

   /*
    * New_descriptor analogue.
    */
   dcon.descriptor = 0;
   dcon.character = NULL;
   dcon.connected = CON_GET_NAME;
   dcon.host = str_dup ("localhost");
   dcon.outsize = 2000;
   dcon.outbuf = alloc_mem (dcon.outsize);
   dcon.next = descriptor_list;
   dcon.pEdit = NULL;   /* OLC */
   dcon.pString = NULL;   /* OLC */
   descriptor_list = &dcon;

   /*
    * Send the greeting.
    */
   {
      extern char *help_greeting;
      if (help_greeting[0] == '.')
         write_to_buffer (&dcon, help_greeting + 1, 0);
      else
         write_to_buffer (&dcon, help_greeting, 0);
   }


   /* Main loop */
   while (!merc_down)
   {
      DESCRIPTOR_DATA *d;

      /*
       * Process input.
       */
      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;
         d->fcommand = FALSE;

#if defined(MSDOS)
         if (kbhit ())
#endif
         {
            if (d->character != NULL)
               d->character->timer = 0;
            if (!read_from_descriptor (d))
            {
               if (d->character != NULL)
				{
				  if (d->character->onPhone != NULL)
					do_hangup(d->character, "");
                  do_autosave (d->character, "");
				}
               d->outtop = 0;
               close_socket (d);
               continue;
            }
         }

         if (d->character != NULL && d->character->wait > 0)
         {
            --d->character->wait;
            continue;
         }

         read_from_buffer (d);
         if (d->incomm[0] != '\0')
         {
            d->fcommand = TRUE;
            stop_idling (d->character);

            if (d->character->desc->pString != NULL)
               string_add (d->character, d->incomm);
            else if (d->showstr_point)
               show_string (d, d->incomm);
            else
            {
               switch (d->connected)
               {
               case CON_PLAYING:
                  switch (d->editor)
                  {
                  case ED_AEDITOR:
                     aedit (d->character, d->incomm);
                     break;
                  case ED_REDITOR:
                     redit (d->character, d->incomm);
                     break;
                  case ED_OEDITOR:
                     oedit (d->character, d->incomm);
                     break;
                  case ED_MEDITOR:
                     medit (d->character, d->incomm);
                     break;
                  case ED_HEDITOR:
                     hedit (d->character, d->incomm);
                     break;
                  default:
                     interpret (d->character, d->incomm);
                     break;
                  }
                  break;
               case CON_CREATEVAMPIRE:
                  vamp_create (d->character, d->incomm);
                  break;

               default:
                  nanny (d, d->incomm);
                  break;
               }
            }
            d->incomm[0] = '\0';

         }
      }



      /*
       * Autonomous game motion.
       */
      update_handler ();



      /*
       * Output.
       */
      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;

         if ((d->fcommand || d->outtop > 0))
         {
            if (!process_output (d, TRUE))
            {
               if (d->character != NULL)
                  do_autosave (d->character, "");
               d->outtop = 0;
               close_socket (d);
            }
         }
      }



      /*
       * Synchronize to a clock.
       * Busy wait (blargh).
       */
      now_time = last_time;
      for (;;)
      {
         int delta;

#if defined(MSDOS)
         if (kbhit ())
#endif
         {
            if (dcon.character != NULL)
               dcon.character->timer = 0;
            if (!read_from_descriptor (&dcon))
            {
               if (dcon.character != NULL)
                  do_autosave (d->character, "");
               dcon.outtop = 0;
               close_socket (&dcon);
            }
#if defined(MSDOS)
            break;
#endif
         }

         gettimeofday (&now_time, NULL);
         delta = (now_time.tv_sec - last_time.tv_sec) * 1000 * 1000 + (now_time.tv_usec - last_time.tv_usec);
         if (delta >= 1000000 / PULSE_PER_SECOND)
            break;
      }
      last_time = now_time;
      current_time = (time_t) last_time.tv_sec;
   }

   return;
}
#endif



#if defined(unix)

void excessive_cpu (int blx)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next;

      if (!IS_NPC (vch))
      {
         send_to_char ("Mud frozen: Autosave and quit.  The mud will reboot in 2 seconds.\n\r", vch);
         interpret (vch, "quit");
      }
   }
   exit (1);
}

void game_loop_unix (int control)
{
   static struct timeval null_time;
   struct timeval last_time;

   //signal (SIGBUS, handler);
   //signal( SIGSEGV, crashrecov );
   //signal (SIGSEGV, handler);
   //signal (SIGILL, handler);

   signal (SIGPIPE, SIG_IGN);
   gettimeofday (&last_time, NULL);
   current_time = (time_t) last_time.tv_sec;

   /* Main loop */
   while (!merc_down)
   {
      fd_set in_set;
      fd_set out_set;
      fd_set exc_set;
      DESCRIPTOR_DATA *d;
      int maxdesc;

#if defined(MALLOC_DEBUG)
      if (malloc_verify () != 1)
         abort ();
#endif

      /*
       * Poll all active descriptors.
       */
      FD_ZERO (&in_set);
      FD_ZERO (&out_set);
      FD_ZERO (&exc_set);
      FD_SET (control, &in_set);
      maxdesc = control;
      for (d = descriptor_list; d != NULL; d = d->next)
      {
         maxdesc = UMAX (maxdesc, d->descriptor);
         FD_SET (d->descriptor, &in_set);
         FD_SET (d->descriptor, &out_set);
         FD_SET (d->descriptor, &exc_set);
      }

      if (select (maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0)
      {
         perror ("Game_loop: select: poll");
         exit (1);
      }

      /*
       * New connection?
       */
      if (FD_ISSET (control, &in_set))
         new_descriptor (control);

      /*
       * Kick out the freaky folks.
       */
      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;
         if (FD_ISSET (d->descriptor, &exc_set))
         {
            FD_CLR (d->descriptor, &in_set);
            FD_CLR (d->descriptor, &out_set);
            if (d->character)
               do_autosave (d->character, "");
            d->outtop = 0;
            close_socket (d);
         }
      }

      /*
       * Process input.
       */
      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;
         d->fcommand = FALSE;

         if (FD_ISSET (d->descriptor, &in_set))
         {
            if (d->character != NULL)
               d->character->timer = 0;
            if (!read_from_descriptor (d))
            {
			   if (d->character != NULL && d->character->onPhone != NULL)
				   do_hangup(d->character, "");
               FD_CLR (d->descriptor, &out_set);

               d->outtop = 0;
               close_socket (d);
               continue;
            }
         }

         if (d->character != NULL && d->character->wait > 0)
         {
            --d->character->wait;
            continue;
         }

         read_from_buffer (d);
         if (d->incomm[0] != '\0')
         {
            d->fcommand = TRUE;
            stop_idling (d->character);


            if (d->pString != NULL)
               string_add (d->character, d->incomm);   /* OLC */
            else if (d->showstr_point)
               show_string (d, d->incomm);
            else
            {
               switch (d->connected)
               {
               case CON_PLAYING:
                  switch (d->editor)
                  {
                  case ED_AEDITOR:
                     aedit (d->character, d->incomm);
                     break;
                  case ED_REDITOR:
                     redit (d->character, d->incomm);
                     break;
                  case ED_OEDITOR:
                     oedit (d->character, d->incomm);
                     break;
                  case ED_MEDITOR:
                     medit (d->character, d->incomm);
                     break;
                  case ED_HEDITOR:
                     hedit (d->character, d->incomm);
                     break;
                  case ED_MPEDITOR:
                     //mpedit (d->character, d->incomm);
                     break;
                  case ED_SMPEDITOR:
                     //sing_mpedit (d->character, d->incomm);
                     break;
                  default:
                     interpret (d->character, d->incomm);
                     break;
                  }
                  break;

               default:
                  nanny (d, d->incomm);
                  break;
               }
            }
            d->incomm[0] = '\0';

         }
      }

      /*
       * Autonomous game motion.
       */
      update_handler ();



      /*
       * Output.
       */
      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;

         if ((d->fcommand || d->outtop > 0) && FD_ISSET (d->descriptor, &out_set))
         {
            if (!process_output (d, TRUE))
            {
               if (d->character != NULL)
                  do_autosave (d->character, "");
               d->outtop = 0;
               close_socket (d);
            }
         }
      }



      /*
       * Synchronize to a clock.
       * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
       * Careful here of signed versus unsigned arithmetic.
       */
      {
         struct timeval now_time;
         long secDelta;
         long usecDelta;

         gettimeofday (&now_time, NULL);
         usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec) + 1000000 / PULSE_PER_SECOND;
         secDelta = ((int) last_time.tv_sec) - ((int) now_time.tv_sec);
         while (usecDelta < 0)
         {
            usecDelta += 1000000;
            secDelta -= 1;
         }

         while (usecDelta >= 1000000)
         {
            usecDelta -= 1000000;
            secDelta += 1;
         }

         if (secDelta > 0 || (secDelta == 0 && usecDelta > 0))
         {
            struct timeval stall_time;

            stall_time.tv_usec = usecDelta;
            stall_time.tv_sec = secDelta;
            if (select (0, NULL, NULL, NULL, &stall_time) < 0)
            {
               perror ("Game_loop: select: stall");
               exit (1);
            }
         }
      }

      gettimeofday (&last_time, NULL);
      current_time = (time_t) last_time.tv_sec;
   }

   return;
}
#endif


void init_descriptor (DESCRIPTOR_DATA * dnew, int desc)
{
   static DESCRIPTOR_DATA d_zero;
   *dnew = d_zero;
   dnew->descriptor = desc;
   dnew->character = NULL;
   dnew->connected = CON_GET_NAME;
   dnew->outsize = 2000;
   dnew->pEdit = NULL;   /* OLC */
   dnew->pString = NULL;   /* OLC */
   dnew->outbuf = alloc_mem (dnew->outsize);
   dnew->editor = 0;
}



#if defined(unix)
void new_descriptor (int control)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *dnew;
   BAN_DATA *pban;
   struct sockaddr_in sock;
   struct hostent *from;
   int desc;
   int size;

   size = sizeof (sock);
   getsockname (control, (struct sockaddr *) &sock, &size);
   if ((desc = accept (control, (struct sockaddr *) &sock, &size)) < 0)
   {
      perror ("New_descriptor: accept");
      return;
   }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

   if (fcntl (desc, F_SETFL, FNDELAY) == -1)
   {
      perror ("New_descriptor: fcntl: FNDELAY");
      return;
   }

   /*
    * Cons a new descriptor.
    */
   if (descriptor_free == NULL)
   {
      dnew = alloc_perm (sizeof (*dnew));
   }
   else
   {
      dnew = descriptor_free;
      descriptor_free = descriptor_free->next;
   }
   init_descriptor (dnew, desc);
   size = sizeof (sock);
   if (getpeername (desc, (struct sockaddr *) &sock, &size) < 0)
   {
      perror ("New_descriptor: getpeername");
      dnew->host = str_dup ("(unknown)");
   }
   else
   { 
      /*
       * Would be nice to use inet_ntoa here but it takes a struct arg,
       * which ain't very compatible between gcc and system libraries.
       */
      int addr;

      addr = ntohl (sock.sin_addr.s_addr);
      sprintf (buf, "%d.%d.%d.%d", (addr >> 24) & 0xFF, (addr >> 16) & 0xFF, (addr >> 8) & 0xFF, (addr) & 0xFF);
      sprintf (log_buf, "Sock.sinaddr:  %s", buf);
      log_string (log_buf, CHANNEL_LOG_CODER);
      if((from = gethostbyaddr ((char *) &sock.sin_addr, sizeof (sock.sin_addr), AF_INET)))
         dnew->host = str_dup(from->h_name);
      else
         dnew->host = str_dup(buf);
        from = gethostbyaddr ((char *) &sock.sin_addr,
                              sizeof (sock.sin_addr), AF_INET);
        dnew->host = str_dup (from ? from->h_name : buf);
   }
   /*
    * Swiftest: I added the following to ban sites.  I don't
    * endorse banning of sites, but Copper has few descriptors now
    * and some people from certain sites keep abusing access by
    * using automated 'autodialers' and leaving connections hanging.
    *
    * Furey: added suffix check by request of Nickel of HiddenWorlds.
    */
   for (pban = ban_list; pban != NULL; pban = pban->next)
   {
      if (!str_suffix (pban->name, dnew->host))
      {
         write_to_descriptor (desc, "Your site has been banned from this Mud. Try emailing an administrator.\n\r", 0);
         close (desc);
         free_string (dnew->host);
         free_mem (dnew->outbuf, dnew->outsize);
         dnew->next = descriptor_free;
         descriptor_free = dnew;
         return;
      }
   }

   /*
    * Init descriptor data.
    */
   dnew->next = descriptor_list;
   descriptor_list = dnew;

   /*
    * Send the greeting.
    */
   {
      extern char *help_greeting;
      if (help_greeting[0] == '.')
         write_to_buffer (dnew, help_greeting + 1, 0);
      else
         write_to_buffer (dnew, help_greeting, 0);
   }

   return;
} 

#endif

void close_socket( DESCRIPTOR_DATA *dclose )
{
  CHAR_DATA *ch;

  if (dclose->lookup_status > STATUS_DONE) return;
  dclose->lookup_status += 2;

  if (dclose->outtop > 0) process_output( dclose, FALSE );

  if ( dclose->snoop_by != NULL )
    write_to_buffer( dclose->snoop_by, "Your victim has left the game.\n\r", 0 );

  if (dclose->character != NULL && dclose->connected == CON_PLAYING &&
      IS_NPC(dclose->character)) do_return(dclose->character,"");

  /*
   * Clear snoops
   */
  {
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
      if (d->snoop_by == dclose) d->snoop_by = NULL;
  }
  if ((ch = dclose->character) != NULL)
  {
    sprintf( log_buf, "Closing link to %s.", ch->name );
    log_string( log_buf, CHANNEL_LOG_NORMAL );
    if ((dclose->connected == CON_PLAYING) || ((dclose->connected >= CON_NOTE_TO) && (dclose->connected <= CON_NOTE_FINISH)))
    {
      if (IS_NPC(ch) || ch->pcdata->obj_vnum == 0)
        act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
      ch->desc = NULL;
    }
    else
    {
      free_char( dclose->character );
    }
  }
  if (d_next == dclose) d_next = d_next->next;
  if (dclose == descriptor_list)
  {
    descriptor_list = descriptor_list->next;
  }
  else
  {
    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d && d->next != dclose; d = d->next)
      ;
    if (d != NULL)
      d->next = dclose->next;
    else
      bug ("Close_socket: dclose not found.", 0);
  }
  dclose->connected = CON_NOT_PLAYING;

  close (dclose->descriptor);
  free_string (dclose->host);
  /* RT socket leak fix */
  free_mem (dclose->outbuf, dclose->outsize);

  dclose->next = descriptor_free;
  descriptor_free = dclose;
  return;
}

void close_socket2( DESCRIPTOR_DATA *dclose )
{
  CHAR_DATA *ch;

  if (dclose->lookup_status > STATUS_DONE) return;
  dclose->lookup_status += 2;

  if (dclose->outtop > 0) process_output( dclose, FALSE );

  if ( dclose->snoop_by != NULL )
    write_to_buffer( dclose->snoop_by, "Your victim has left the game.\n\r", 0 );

  if (dclose->character != NULL && dclose->connected == CON_PLAYING &&
      IS_NPC(dclose->character)) do_return(dclose->character,"");

  /*
   * Clear snoops
   */
  {
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )      if 
(d->snoop_by == dclose) d->snoop_by = NULL;
  }
  if ((ch = dclose->character) != NULL)
  {
    if (dclose->connected == CON_PLAYING)
    {
      ch->desc = NULL;
    }
    else
    {
      free_char (dclose->character);
    }
  }

  if (d_next == dclose) d_next = d_next->next;
  if (dclose == descriptor_list)
  {
    descriptor_list = descriptor_list->next;
  }
  else
  {
    DESCRIPTOR_DATA *d;

    for (d = descriptor_list; d && d->next != dclose; d = d->next)
      ;
    if (d != NULL)
      d->next = dclose->next;
    else
      bug ("Close_socket: dclose not found.", 0);
  }
  dclose->connected = CON_NOT_PLAYING;

  close (dclose->descriptor);
  free_string (dclose->host);
  /* RT socket leak fix */
  free_mem (dclose->outbuf, dclose->outsize);

  dclose->next = descriptor_free;
  descriptor_free = dclose;
  return;
}


bool read_from_descriptor (DESCRIPTOR_DATA * d)
{
   int iStart;
   char buf[MAX_STRING_LENGTH];

   /* Hold horses if pending command already. */
   if (d->incomm[0] != '\0')
      return TRUE;

   if (d->connected == CON_NOT_PLAYING)
      return TRUE;

   /* Check for overflow. */
   iStart = strlen (d->inbuf);
   if (iStart >= sizeof (d->inbuf) - 10)
   {
      if (d != NULL && d->character != NULL)
         sprintf (log_buf, "%s input overflow!", d->character->lasthost);
      else if (d->lookup_status == STATUS_DONE)
         sprintf (log_buf, "%s input overflow!", d->host);
      log_string (log_buf, CHANNEL_LOG_CODER);

      write_to_descriptor (d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
      return FALSE;
   }

   /* Snarf input. */
#if defined(macintosh)
   for (;;)
   {
      int c;
      c = getc (stdin);
      if (c == '\0' || c == EOF)
         break;
      putc (c, stdout);
      if (c == '\r')
         putc ('\n', stdout);
      d->inbuf[iStart++] = c;
      if (iStart > sizeof (d->inbuf) - 10)
         break;
   }
#endif

#if defined(MSDOS) || defined(unix)
   for (;;)
   {
      int nRead;
   
      nRead = read (d->descriptor, d->inbuf + iStart, sizeof (d->inbuf) - 10 - iStart);
      if (nRead > 0)
      {
         iStart += nRead;
         if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
            break;
      }
      else if (nRead == 0)
      {
         sprintf( buf, "EOF encountered on read. %s", d->host);
         log_string (buf, CHANNEL_LOG_CODER);
         return FALSE;
      }
      else if (errno == EWOULDBLOCK)
         break;
      else
      {
         perror ("Read_from_descriptor");
         return FALSE;
      }
   }
#endif

   d->inbuf[iStart] = '\0';
   return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer (DESCRIPTOR_DATA * d)
{
   int i, j, k;
   char buf[MAX_STRING_LENGTH];
   char telbuf[MAX_STRING_LENGTH];
   //char buf2[MAX_STRING_LENGTH];

   /*
    * Hold horses if pending command already.
    */
   if (d->incomm[0] != '\0')
      return;

   /*
    * Look for at least one new line.
    */
   for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
   {
      if (d->inbuf[i] == '\0')
         return;
   }

   /*
    * Canonical input processing.
    */
   for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
   {
      if (k >= MAX_INPUT_LENGTH - 2)
      {
         write_to_descriptor (d->descriptor, "Line too long.\n\r", 0);
         log_string ("oshi :o", CHANNEL_LOG_CODER);

         /* skip the rest of the line */
         for (; d->inbuf[i] != '\0'; i++)
         {
            if (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
               break;
         }
         d->inbuf[i] = '\n';
         d->inbuf[i + 1] = '\0';
         break;
      }

    /* //remove for MXP.
      if (d->inbuf[i] == '\b' && k > 0)
         --k;
      else if (isascii (d->inbuf[i]) && isprint (d->inbuf[i]))
         d->incomm[k++] = d->inbuf[i];
    */
       //MXP
	  
	  //telnet negotiation (Cone)
		if ( IS_TELNET(d->inbuf[i], IAC))
        {                                         
			strcpy(telbuf, str_dup(""));
		  strcat(telbuf, "Telnet sequence received: IAC");
		  i++;
		  strcat(telbuf, " ");
		  //handle subnegotiation
		  if (IS_TELNET(d->inbuf[i], SB))
		  {
			  strcat(telbuf, "SB");
			  i++;
			  for (; IS_TELNET(d->inbuf[i], SB) && d->inbuf[i] != '\0' && d->inbuf[i] != '\r' && d->inbuf[i] != '\n' ; i++)
			  {
				  strcat(telbuf, " ");
				  if (IS_TELOPT(d->inbuf[i]))
					  strcat(telbuf, telopts[(unsigned char)d->inbuf[i]]);
				  else
				  {
					  sprintf(buf, "(%d)", (unsigned char) d->inbuf[i]);
					  strcat(telbuf, buf);
				  }
			  }
		  }
		  //handle telnet commands
		  else if (IS_TELCMD(d->inbuf[i]))
			  strcat(telbuf, telcmds[(unsigned char)d->inbuf[i] - 240]);
		  else
			{
			  //jk not a telnet command
			  d->incomm[k++] = d->inbuf[i-1];
			  d->incomm[k++] = d->inbuf[i];
			  continue;
			}
		  //handle DO, DONT, WILL, WONT options
		  if ((signed char) 255 > d->inbuf[i] > (signed char) 250)
		  {
			  i++;
			  strcat(telbuf, " ");
			  if (IS_TELOPT(d->inbuf[i]))
				  strcat(telbuf, telopts[(unsigned char)d->inbuf[i]]);
			  else
			  {
				  sprintf(buf, "(%d)", d->inbuf[i]);
				  strcat(telbuf, buf);
			  }
		  }
		  logchan(telbuf, CHANNEL_LOG_CODER);
		}
        else if ( d->inbuf[i] == '\b' && k > 0 )
            --k;
        else if (((unsigned char) d->inbuf[i]) > 31)
            d->incomm[k++] = d->inbuf[i];
		else
	   {
			sprintf(buf, "Illegal character received: %d", (unsigned char) d->inbuf[i]);
			logchan(buf, CHANNEL_LOG_CODER);
	   }
	   //sprintf(&buf2[i*4], "%-4d", (unsigned char) d->inbuf[i]);

   }
   //logchan(buf2, CHANNEL_LOG_CODER);

   /*
    * Finish off the line.
    */
   if (k == 0)
      d->incomm[k++] = ' ';
   d->incomm[k] = '\0';

   /*
    * Deal with bozos with #repeat 1000 ...
    */
   if (k > 1 || d->incomm[0] == '!')
   {
      if (d->incomm[0] != '!' && strcmp (d->incomm, d->inlast))
      {
         d->repeat = 0;
      }
      else
      {
         if (++d->repeat >= 40)
         {
            if (d != NULL && d->character != NULL)
               sprintf (log_buf, "%s input spamming!", d->character->lasthost);
            else if (d->lookup_status == STATUS_DONE)
               sprintf (log_buf, "%s input spamming!", d->host);
            log_string (log_buf, CHANNEL_LOG_CODER);
            write_to_descriptor (d->descriptor, "\n\r*** PUT A LID ON IT!!! ***\n\r", 0);
            strcpy (d->incomm, "quit");
         }
      }
   }

   /*
    * Do '!' substitution.
    */
   if (d->incomm[0] == '!')
      strcpy (d->incomm, d->inlast);
   else
      strcpy (d->inlast, d->incomm);

   /*
    * Shift the input buffer.
    */
   while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
      i++;
   for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
      ;
   return;
}



/*
 * Low level output function.
 */
bool process_output (DESCRIPTOR_DATA * d, bool fPrompt)
{
   extern bool merc_down;



   /*
    * Bust a prompt.
    */
   if (!merc_down && d->connected == CON_PLAYING && d->pString != NULL)
      write_to_buffer (d, "<[OLC]> ", 0);
   else if (d->showstr_point)
   {
      write_to_buffer (d,
      "#e  -==<#r(#RC#r)#eontinue, #r(#RR#r)#eefresh, #r(#RB#r)#eack, #r(#RH#r)#eelp, #r(#RQ#r)#euit, or #RRETURN#e>==-  ", 0);
   }
   else if (fPrompt && !merc_down && d->connected == CON_PLAYING)
   {
      CHAR_DATA *ch;
      CHAR_DATA *victim;

      ch = d->original ? d->original : d->character;
      if (IS_SET (ch->act, PLR_BLANK))
         write_to_buffer (d, "\n\r", 2);

      if (IS_SET (ch->act, PLR_PROMPT) && IS_EXTRA (ch, EXTRA_PROMPT) && ch->desc->editor != ED_REDITOR)
         bust_a_prompt (d);
      else if (IS_SET (ch->act, PLR_PROMPT) && ch->desc->editor != ED_REDITOR)
      {
         char buf[MAX_STRING_LENGTH];
         char cond[MAX_INPUT_LENGTH];
         char hit_str[MAX_INPUT_LENGTH];
         char exp_str[MAX_INPUT_LENGTH];
         char wpmax[MAX_INPUT_LENGTH];
         char wpmin[MAX_INPUT_LENGTH];
         char convcurrent[MAX_INPUT_LENGTH];
         char gnosismax[MAX_INPUT_LENGTH];
         char gnosismin[MAX_INPUT_LENGTH];
         char ragemin[MAX_INPUT_LENGTH];
         char ragemax[MAX_INPUT_LENGTH];
         char bloodmax[MAX_INPUT_LENGTH];
         char bloodmin[MAX_INPUT_LENGTH];
         char qp_str[MAX_INPUT_LENGTH];
	 char spokenword[MAX_INPUT_LENGTH];
         
         ch = d->character;

	
	if (ch->pcdata->language[0] == LANG_ENGLISH)
		strcpy(spokenword, "english");
	else if (ch->pcdata->language[0] == LANG_ARABIC)
		strcpy(spokenword, "arabic");
	else if (ch->pcdata->language[0] == LANG_CHINESE)
		strcpy(spokenword, "chinese");
	else if (ch->pcdata->language[0] == LANG_FRENCH)
		strcpy(spokenword, "french");
	else if (ch->pcdata->language[0] == LANG_ITALIAN)
		strcpy(spokenword, "italian");
	else if (ch->pcdata->language[0] == LANG_SPANISH)
		strcpy(spokenword, "spanish");
	else if (ch->pcdata->language[0] == LANG_LATIN)
		strcpy(spokenword, "latin");
	else if (ch->pcdata->language[0] == LANG_HEBREW)
		strcpy(spokenword, "hebrew");
	else if (ch->pcdata->language[0] == LANG_GAELIC)
		strcpy(spokenword, "gaelic");
	else if (ch->pcdata->language[0] == LANG_JAPANESE)
		strcpy(spokenword, "japanese");
	else if (ch->pcdata->language[0] == LANG_GREEK)
		strcpy(spokenword, "greek");
	else if (ch->pcdata->language[0] == LANG_SLAVIC)
		strcpy(spokenword, "slavic");
	else if (ch->pcdata->language[0] == LANG_GAROU)
		strcpy(spokenword, "garou");
	else if (ch->pcdata->language[0] == LANG_GERMAN)
		strcpy(spokenword, "german");
	else if (ch->pcdata->language[0] == LANG_RUSSIAN)
		strcpy(spokenword, "russian");
	else if (ch->pcdata->language[0] == LANG_INDONESIAN)
		strcpy(spokenword, "indonesian");
	else if (ch->pcdata->language[0] == LANG_KOREAN)
		strcpy(spokenword, "korean");
	else if (ch->pcdata->language[0] == LANG_VIETNAMESE)
		strcpy(spokenword, "vietnamese");
	else if (ch->pcdata->language[0] == LANG_SWAHILI)
		strcpy(spokenword, "swahili");
	else if (ch->pcdata->language[0] == LANG_TAGALOG)
		strcpy(spokenword, "tagalog");
	else if (ch->pcdata->language[0] == LANG_HINDU)
		strcpy(spokenword, "hindu");
	else strcpy(spokenword, "None");
	if (IS_EXTRA (ch, EXTRA_OSWITCH))
	{
            sprintf (exp_str, "%d", ch->exp);
            COL_SCALE (exp_str, ch, ch->exp, 100);
            sprintf (buf, "<[%s] [?H]> ", exp_str);
	}
         else if (ch->position == POS_FIGHTING)
         {
            victim = ch->fighting;
            if (victim->hit < 1 || victim->max_hit < 1)
            {
               strcpy (cond, "Incapacitated");
               ADD_COLOUR (ch, cond, L_RED);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 17)
            {
               strcpy (cond, "Crippled");
               ADD_COLOUR (ch, cond, L_RED);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 34)
            {
               strcpy (cond, "Mauled");
               ADD_COLOUR (ch, cond, L_BLUE);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 50)
            {
               strcpy (cond, "Wounded");
               ADD_COLOUR (ch, cond, L_BLUE);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 67)
            {
               strcpy (cond, "Injured");
               ADD_COLOUR (ch, cond, L_GREEN);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 83)
            {
               strcpy (cond, "Hurt");
               ADD_COLOUR (ch, cond, L_GREEN);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 100)
            {
               strcpy (cond, "Bruised");
               ADD_COLOUR (ch, cond, YELLOW);
            }
            else
            {
               strcpy (cond, "Perfect");
               ADD_COLOUR (ch, cond, L_CYAN);
            }
            sprintf (hit_str, "%d", ch->hit);
            COL_SCALE (hit_str, ch, ch->hit, ch->max_hit);
            sprintf (buf, "<[%s] [%sH]> ", cond, hit_str);
         }
         else
         {
            sprintf (hit_str, "%d", ch->hit);
            COL_SCALE (hit_str, ch, ch->hit, ch->max_hit);
            sprintf (exp_str, "%d", ch->exp);
            COL_SCALE (exp_str, ch, ch->exp, 100);
            sprintf (qp_str, "%d", ch->quintessence);
            COL_SCALE (qp_str, ch, ch->quintessence,ch->max_quintessence);

            sprintf (wpmin, "%d", ch->pcdata->willpower[WILLPOWER_CURRENT]);
            COL_SCALE (exp_str, ch, ch->pcdata->willpower[WILLPOWER_CURRENT], 100);

            sprintf (wpmax, "%d", ch->pcdata->willpower[WILLPOWER_MAX]);
            COL_SCALE (exp_str, ch, ch->pcdata->willpower[WILLPOWER_MAX], 100);

               sprintf (gnosismin, "%d", ch->pcdata->gnosis[0]);
            COL_SCALE (exp_str, ch, ch->pcdata->gnosis[0], 100);
               sprintf (gnosismax, "%d", ch->pcdata->gnosis[1]);
            COL_SCALE (exp_str, ch, ch->pcdata->gnosis[1], 100);
               sprintf (ragemin, "%d", ch->pcdata->rage[0]);
            COL_SCALE (exp_str, ch, ch->pcdata->rage[0], 100);
               sprintf (ragemax, "%d", ch->pcdata->rage[1]);
            COL_SCALE (exp_str, ch, ch->pcdata->rage[1], 100);
               sprintf (bloodmax, "%d", ch->blood[BLOOD_CURRENT]);
            COL_SCALE (exp_str, ch, ch->blood[BLOOD_CURRENT], 100);
               sprintf (bloodmin, "%d", ch->blood[BLOOD_POOL]);
            COL_SCALE (exp_str, ch, ch->blood[BLOOD_POOL], 100);
	      /* Lumi's Code */
	      sprintf (convcurrent, "%d", ch->pcdata->conviction[CONVICTION_CURRENT]);
	    COL_SCALE (exp_str, ch, ch->pcdata->conviction[CONVICTION_CURRENT], 100);
            /* done */
	    if (!IS_NPC(ch))
            {
               
           
           if (IS_STORYTELLER(ch)){
           		if (IS_AFFECTED(ch, AFF_POLYMORPH))	
           			sprintf (buf, "#G<#evnum#G:#e%d#G> #G<#eMask#G:#e %s#G> ", ch->in_room->vnum, ch->morph);
           		else
           			sprintf (buf, "#G<#evnum#G:#e%d#G> ", ch->in_room->vnum);
           		
           		if (IS_SET (ch->act, PLR_WIZINVIS))
          			strcat(buf, "#G<#einvis#G>#n ");
          		if (IS_SET (ch->act, PLR_INCOG))	
          			strcat(buf, "#G<#eincog#G>#n ");
          		if (ch->desc->editor != ED_NONE)
          			strcat(buf, "#G<#eolc#G>#n ");
          		}	
          			
          
           else	if (IS_VAMPIRE(ch)){
		    sprintf (buf, "#c<#bWP #B%s#c/#B%s#c> <#rBP #R%s#c/#R%s#c> <#p%s#c>",wpmin,wpmax,bloodmax, bloodmin, spokenword);    
		        if (IS_VAMPAFF (ch, VAM_FANGS))
                  		strcat(buf, "#c <#CF#c>");
                  	if (IS_MORE2 (ch, MORE2_HEIGHTENSENSES))
                  		strcat(buf, "#c <#CHS#c>");
                  	if ((IS_MORE3(ch, MORE3_OBFUS2)) || (IS_MORE3(ch, MORE3_OBFUS1)))
                  		strcat(buf, "#c <#eObfus#c>");
                  	if (IS_VAMPAFF (ch, VAM_DISGUISED))
                  		strcat(buf, "#c <#eMask#c>");
                  	if (IS_MORE2 (ch, MORE2_RPFLAG))
                  		strcat(buf, "#c <#CIC#c>");
					//if (ch->pcdata->canvotecounter >= 3)
						//strcat(buf, "#c <#ev#c>");
					if (ch->onPhone != NULL)
					{
						if (ch->phonestate == 1)
							strcat(buf, "#g <#GPhone#g>");
						else if (ch->phonestate == 0)
							strcat(buf, "#r <#RRinging...#r>");
						else if (ch->phonestate == -1)
							strcat(buf, "#g <#GPhone#g>");
					}
			}
                  	
        else if (IS_SHIFTER(ch)){
                     sprintf (buf, "#c<#bWP #B%s#c/#B%s#c> <#gGN #G%s#c/#G%s#c> <#oRG #y%s#c/#y%s#c> <#p%s#c>",wpmin, wpmax, gnosismin,gnosismax,ragemin,ragemax, spokenword);
                     	if (IS_AFFECTED (ch, AFF_SHADOWPLANE))
                     		strcat(buf, "#c <#BUmbra#c>");
                     
                  		/* Why doesnt this work? */
                     if ((ch->pcdata->wolfform[1] == FORM_GLABRO))
                     		strcat(buf, "#c <#GGlabro#c>");
                     if ((ch->pcdata->wolfform[1] == FORM_CRINOS))
                     		strcat(buf, "#c <#GCrinos#c>");
                     if ((ch->pcdata->wolfform[1] == FORM_HISPO))
                     		strcat(buf, "#c <#GHispo#c>");	
                     if ((ch->pcdata->wolfform[1] == FORM_LUPUS))
                     		strcat(buf, "#c <#GLupus#c>");
                     		 /*end*/
                     if (IS_MORE2 (ch, MORE2_RPFLAG))
                  		strcat(buf, "#c <#CIC#c>");
					 //if (ch->pcdata->canvotecounter >= 3)
						//strcat(buf, "#c <#ev#c>");
					 if (ch->onPhone != NULL)
					{
						if (ch->phonestate == 1)
							strcat(buf, "#g <#GPhone#g>");
						else if (ch->phonestate == 0)
							strcat(buf, "#r <#RRinging...#r>");
						else if (ch->phonestate == -1)
							strcat(buf, "#g <#GPhone#g>");
					}
                     
                     
                     }
                     
         else if (IS_INQUISITOR(ch)){
					   sprintf (buf, "#c<#bWP #B%s#c/#B%s#c> <#nCP#B:#w%s#c> <#p%s#c>#n", wpmin, wpmax, convcurrent, spokenword);
                       if (IS_MORE2 (ch, MORE2_RPFLAG))
						   strcat(buf, "#c <#CIC#c>");
					   //if (ch->pcdata->canvotecounter >= 3)
						   //strcat(buf, "#c <#ev#c>");
					   if (ch->onPhone != NULL)
						{
							if (ch->phonestate == 1)
								strcat(buf, "#g <#GPhone#g>");
							else if (ch->phonestate == 0)
								strcat(buf, "#r <#RRinging...#r>");
							else if (ch->phonestate == -1)
								strcat(buf, "#g <#GPhone#g>");
						}
		 }
                  
		  
		  else {
                     sprintf (buf, "#c<#bWP #B%s#c/#B%s#c> <#p%s#c>",wpmin,wpmax,spokenword);
                     if (IS_MORE2 (ch, MORE2_RPFLAG))
                  		strcat(buf, "#c <#CIC#c>");
					 //if (ch->pcdata->canvotecounter >= 3)
						//strcat(buf, "#c <#ev#c>");
					 if (ch->onPhone != NULL)
					{
						if (ch->phonestate == 1)
							strcat(buf, "#g <#GPhone#g>");
						else if (ch->phonestate == 0)
							strcat(buf, "#r <#RRinging...#r>");
						else if (ch->phonestate == -1)
							strcat(buf, "#g <#GCalling...#g>");
					}
				}
			}
            else
               sprintf (buf, "<[%sX] [%sH]> ", exp_str, hit_str);

         }
         write_to_buffer (d, buf, 0);
      }

      if (IS_SET (ch->act, PLR_TELNET_GA))
         write_to_buffer (d, go_ahead_str, 0);
   }

   /*
    * Short-circuit if nothing to write.
    */
   if (d->outtop == 0)
      return TRUE;

   /*
    * Snoop-o-rama.
    */
   if (d->snoop_by != NULL)
   {
      if (d->snoop_by->character != NULL && IS_IMMORTAL (d->snoop_by->character))
      {
         write_to_buffer (d->snoop_by, "% ", 2);
         write_to_buffer (d->snoop_by, d->outbuf, d->outtop);
      }
   }

   /*
    * OS-dependent output.
    */
   if (!write_to_descriptor (d->descriptor, d->outbuf, d->outtop))
   {
      d->outtop = 0;
      return FALSE;
   }
   else
   {
      d->outtop = 0;
      return TRUE;
   }
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer (DESCRIPTOR_DATA * d, const char *txt, int length)
{
   char tb[30000], ccode;
   int i, j;
   bool ansi, grfx = FALSE;

   /*
    * Find length in case caller didn't.
    */
   if (length <= 0)
      length = strlen (txt);

   if (length >= MAX_STRING_LENGTH)
   {
      bug ("Write_to_buffer: Way too big. Closing.", 0);
      return;
   }

   if (d->character == NULL)
      ansi = FALSE;
   else
      ansi = (IS_SET (d->character->act, PLR_ANSI)) ? TRUE : FALSE;
   if (d->connected == CON_GET_NAME)
      ansi = TRUE;

   /*
    * Initial \n\r if needed.
    */
   if (d->outtop == 0 && !d->fcommand)
   {
      d->outbuf[0] = '\n';
      d->outbuf[1] = '\r';
      d->outtop = 2;
   }

   /*
    * Expand d->outbuf for ansi info
    */

   j = 0;
   ccode = '3';
   tb[0] = '\0';
   for (i = 0; i < length; i++)
   {
      if ((txt[i] == '#' || txt[i] == '`') && txt[i + 1] != 'I' && txt[i + 1] != 'N')
      {
         if (txt[i] == '`')
            ansi = TRUE;
         if (txt[++i] == 'K')
         {
            ccode = '3';
            i++;
         }
         else
            ccode = '3';

         switch (txt[i])
         {
         default:
            break;
         case '#':
            tb[j++] = '#';
            break;
         case '`':
            tb[j++] = '`';
            break;
         case '-':
            tb[j++] = '~';
            break;
         case '+':
            tb[j++] = '%';
            break;
         case 'I':
         case 'i':
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '7';
            tb[j++] = 'm';
            break;
         case 'f':
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '5';
            tb[j++] = 'm';
            break;
         case 'u':
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '4';
            tb[j++] = 'm';
            break;
         case 't':
            if (!ansi)
               continue;
            tb[j++] = '[';
            tb[j++] = '4';
            tb[j++] = 'm';
            break;
         case 'T':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '4';
            tb[j++] = '1';
            break;
         case 'd':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '5';
            tb[j++] = '1';
            break;
         case 'D':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '6';
            tb[j++] = '1';
            break;
         case 'N':
         case 'n':
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '7';
            tb[j++] = 'm';
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = 'm';
            break;
         case '0':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '0';
            tb[j++] = 'm';
            break;
         case '1':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '1';
            tb[j++] = 'm';
            break;
         case '2':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '2';
            tb[j++] = 'm';
            break;
         case '3':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '3';
            tb[j++] = 'm';
            break;
         case '4':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '4';
            tb[j++] = 'm';
            break;
         case '5':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '5';
            tb[j++] = 'm';
            break;
         case '6':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '6';
            tb[j++] = 'm';
            break;
         case '7':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '7';
            tb[j++] = 'm';
            break;
         case 'w':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '7';
            tb[j++] = 'm';
            break;
         case 'r':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '1';
            tb[j++] = 'm';
            break;
         case 'R':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '1';
            tb[j++] = 'm';
            break;
         case 'g':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '2';
            tb[j++] = 'm';
            break;
         case 'G':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '2';
            tb[j++] = 'm';
            break;
         case 'o':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '3';
            tb[j++] = 'm';
            break;
         case 'y':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '3';
            tb[j++] = 'm';
            break;
         case 'b':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '4';
            tb[j++] = 'm';
            break;
         case 'B':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '4';
            tb[j++] = 'm';
            break;
         case 'p':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '5';
            tb[j++] = 'm';
            break;
         case 'P':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '5';
            tb[j++] = 'm';
            break;
         case 'c':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '0';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '6';
            tb[j++] = 'm';
            break;
         case 'C':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '6';
            tb[j++] = 'm';
            break;
         case 'e':
            if (!ansi)
               continue;
            tb[j++] = 27;
            tb[j++] = '[';
            tb[j++] = '1';
            tb[j++] = ';';
            tb[j++] = ccode;
            tb[j++] = '0';
            tb[j++] = 'm';
            break;

         case 'S':
            if (!ansi)
               continue;
            switch (number_range (1, 4))
            {
            case 1:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '0';
               tb[j++] = 'm';
               break;
            case 2:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '6';
               tb[j++] = 'm';
               break;
            case 3:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '4';
               tb[j++] = 'm';
               break;
            case 4:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '7';
               tb[j++] = 'm';
               break;
            }
            break;
         case 's':
            if (!ansi)
               continue;
            switch (number_range (1, 15))
            {
            case 1:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '3';
               tb[j++] = 'm';
               break;
            case 2:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '4';
               tb[j++] = 'm';
               break;
            case 3:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '7';
               tb[j++] = 'm';
               break;
            case 4:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '0';
               tb[j++] = 'm';
               break;
            case 5:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '1';
               tb[j++] = 'm';
               break;
            case 6:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '2';
               tb[j++] = 'm';
               break;
            case 7:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '5';
               tb[j++] = 'm';
               break;
            case 8:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '3';
               tb[j++] = 'm';
               break;
            case 9:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '4';
               tb[j++] = 'm';
               break;
            case 10:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '1';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '6';
               tb[j++] = 'm';
               break;
            case 11:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '3';
               tb[j++] = 'm';
               break;
            case 12:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '2';
               tb[j++] = 'm';
               break;
            case 13:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '1';
               tb[j++] = 'm';
               break;
            case 14:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '5';
               tb[j++] = 'm';
               break;
            case 15:
               tb[j++] = 27;
               tb[j++] = '[';
               tb[j++] = '0';
               tb[j++] = ';';
               tb[j++] = ccode;
               tb[j++] = '6';
               tb[j++] = 'm';
               break;

            }
            break;


         }
      }
      else
         tb[j++] = txt[i];
   }

   {
      tb[j++] = 27;
      tb[j++] = '[';
      tb[j++] = '0';
      tb[j++] = 'm';
      if (grfx)
      {
         tb[j++] = 27;
         tb[j++] = '(';
         tb[j++] = 'B';
      }
   }
   tb[j] = '\0';

   length = j;

   /*
    * Expand the buffer as needed.
    */
   while (d->outtop + length >= d->outsize)
   {
      char *outbuf;

      if (d->outsize >= 32000)
      {
         bug ("Buffer overflow. Closing.", 0);
         close_socket (d);
         return;
      }
      outbuf = alloc_mem (2 * d->outsize);
      strncpy (outbuf, d->outbuf, d->outtop);
      free_mem (d->outbuf, d->outsize);
      d->outbuf = outbuf;
      d->outsize *= 2;
   }

   /*
    * Copy.
    */
   strncpy (d->outbuf + d->outtop, tb, length);
   d->outtop += length;
   return;
}


/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor (int desc, char *txt, int length)
{
   int iStart;
   int nWrite;
   int nBlock;

#if defined(macintosh) || defined(MSDOS)
   if (desc == 0)
      desc = 1;
#endif

   if (length <= 0)
      length = strlen (txt);

   for (iStart = 0; iStart < length; iStart += nWrite)
   {
      nBlock = UMIN (length - iStart, 4096);
      if ((nWrite = write (desc, txt + iStart, nBlock)) < 0)
      {
         perror ("Write_to_descriptor");
         return FALSE;
      }
   }

   return TRUE;
}


/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny (DESCRIPTOR_DATA * d, char *argument)
{
//   char arg[MAX_INPUT_LENGTH];
//   char command[MAX_INPUT_LENGTH];
//   FILE *fp;
   char buf[MAX_STRING_LENGTH];
   char kav[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   EBAN_DATA *eban;
//   bool fTilde = FALSE;
   char *pwdnew;
   char *p;
   char *strtime;
   char *lasttime;
   int menu_selection;
   bool fOld;
   char tempbuf[50];
   int j = 0;      // Stat areas counter
   int i = 0;
   int stat_display_counter = 0;
   int temp_num;
   int ability_level = 0;


   /* Delete leading spaces UNLESS character is writing a note */
   if (d->connected != CON_NOTE_TEXT)
   {
      while (isspace (*argument))
         argument++;
   }

   ch = d->character;


   switch (d->connected)
   {

   default:
      bug ("Nanny: bad d->connected %d.", d->connected);
      close_socket (d);
      return;

   case CON_MAIN_MENU:
      do_help( ch, "SYSMENU" );
      if (argument[0] == '\0')
      {
         write_to_buffer( d, "Please enter a command.\n\r", 0);
         d->connected = CON_MAIN_MENU;
         break;
      }

      switch (LOWER( argument[0]))
      {
         case 'e':
         {
            write_to_buffer( d, "Please enter a name: ", 0);
            d->connected = CON_GET_NAME;
            break;
         }

         case 'd':
         {
            do_help( ch, "DOCUMENT_MENU_TEXT" );
            d->connected = CON_DOCUMENT_MENU;
            break;
         }

         case 'q':
         {
            write_to_buffer( d, "Thanks for stopping by!", 0);
            close_socket (d);
            return;
         }
         default:
         d->connected = CON_MAIN_MENU;
         break;
      }

   case CON_DOCUMENT_MENU:
      menu_selection = atoi( argument);
      switch (menu_selection)
      {
         default: do_help(ch, "DOCUMENT_MENU_TEXT"); break;
         case 1: do_help( ch, "DOC_MENU_1" ); break;
         case 2: do_help( ch, "DOC_MENU_2" ); break;
         case 3: do_help( ch, "DOC_MENU_3" ); break;
         case 99: d->connected = CON_MAIN_MENU; break;
      }
      break;

   case CON_GET_NAME:
      if (argument[0] == '\0')
      {
         close_socket(d);
         return;
      }

      if (check_banned(d))
      {
         write_to_buffer( d, "Your site has been banned from this mud\n\r", 0 );
         close_socket( d );
         return;
      }
      argument[0] = UPPER (argument[0]);
      if (!check_parse_name (argument))
      {
         write_to_buffer (d, "Illegal name, try another.\n\rName: ", 0);
         return;
      }

      sprintf (kav, "%s trying to connect.", argument);
      log_string (kav, CHANNEL_LOG_NORMAL);
      fOld = load_char_short (d, argument);
      ch = d->character;

      if (!IS_NPC (ch) && ch->pcdata->denydate > 0)
      {
         if (!play_yet (ch))
         {
            sprintf (log_buf, "Denying access to %s@%s.", argument, ch->lasthost);
            log_string (log_buf, CHANNEL_LOG_CODER);
            close_socket (d);
            return;
         }
      }

      if (check_reconnect (d, argument, FALSE))
      {
         fOld = TRUE;
      }
      else
      {
         /* Check max number of players - KaVir */

         DESCRIPTOR_DATA *dcheck;
         DESCRIPTOR_DATA *dcheck_next;
         int countdesc = 0;
         int max_players = 50;

         for (dcheck = descriptor_list; dcheck != NULL; dcheck = dcheck_next)
         {
            dcheck_next = dcheck->next;
            if (dcheck->character != NULL && dcheck != d)
            {
               if (!IS_NPC (dcheck->character) && ch->level < LEVEL_MORTAL && strlen (dcheck->character->name) > 0 && !str_cmp (dcheck->character->name, ch->name))
               {
                  write_to_buffer (d, "Sorry, there is already someone of your name connected.\n\r", 0);
                  close_socket (d);
                  return;
               }
            }
            countdesc++;
         }

         if (countdesc > max_players && !IS_IMMORTAL (ch))
         {
            write_to_buffer (d, "Too many players connected, please try again in a couple of minutes.\n\r", 0);
            close_socket (d);
            return;
         }

         if (wizlock && !IS_IMMORTAL (ch))
         {
            write_to_buffer (d, "The game is wizlocked.\n\r", 0);
            close_socket (d);
            return;
         }
         if (countdesc > high_number_login)
            high_number_login = countdesc;
      }

      if (fOld)
      {
         /* Old player */
         write_to_buffer (d, "#cPlease enter your password:#n ", 0);
         write_to_buffer (d, echo_off_str, 0);
         d->connected = CON_GET_OLD_PASSWORD;
         return;
      }
      else
      {
         /* New player */
         /*do_help(ch, "name_rules");  Add in after we have naming conventions set */
         sprintf (buf, "#cAre you certain #C%s#c is an appropriate name for modern night Roleplay mu*? #c(#CY#c/#eN#C)#c? ", argument);
         write_to_buffer (d, buf, 0);
         d->connected = CON_CONFIRM_NEW_NAME;
         return;
      }
      break;

   case CON_GET_OLD_PASSWORD:
#if defined(unix)
      write_to_buffer (d, "\n\r", 2);
#endif


      if (ch == NULL || (!IS_EXTRA (ch, EXTRA_NEWPASS) && strcmp (argument, ch->pcdata->pwd) && strcmp (crypt (argument, ch->pcdata->pwd), ch->pcdata->pwd)))
      {
         write_to_buffer (d, "Wrong password.\n\r", 0);
         close_socket (d);
         return;
      }
      else if (ch == NULL || (IS_EXTRA (ch, EXTRA_NEWPASS) && strcmp (crypt (argument, crypt_pwd), crypt_pwd) && strcmp (crypt (argument, ch->pcdata->pwd), ch->pcdata->pwd)))
      {
         write_to_buffer (d, "Wrong password.\n\r", 0);
         close_socket (d);
         return;
      }

      write_to_buffer (d, echo_on_str, 0);

      if (check_reconnect (d, ch->name, TRUE))
         return;

      if (check_playing (d, ch->name))
         return;

      if (ch->level > 1)
      {
         strcpy (kav, ch->name);
         free_char (d->character);
         d->character = NULL;
         fOld = load_char_obj (d, kav);
         ch = d->character;
      }

      if (!IS_EXTRA (ch, EXTRA_NEWPASS) && strlen (argument) > 1)
      {
         sprintf (kav, "%s %s", argument, argument);
         do_password (ch, kav);
      }

      if (ch->lasthost != NULL)
         free_string (ch->lasthost);
      if (ch->desc != NULL && ch->desc->host != NULL)
      {
         ch->lasthost = str_dup (ch->desc->host);
      }
      else
         ch->lasthost = str_dup ("(unknown)");

      if (strlen (ch->lasttime) >= 11)
      {
         lasttime = str_dup (ch->lasttime);
         lasttime[11] = '\0';
         strtime = ctime (&current_time);
         strtime[11] = '\0';
      }

      strtime = ctime (&current_time);
      strtime[strlen (strtime) - 1] = '\0';
      free_string (ch->lasttime);
      ch->lasttime = str_dup (strtime);
	  sprintf (log_buf, "%s@%s has connected.", ch->name, ch->lasthost);
	  log_string (log_buf, CHANNEL_LOG_CODER);

        if (ch->pcdata->email[0]=='\0')
        {
             write_to_buffer(d,"Please enter your email address:  ",0);
            d->connected = CON_GET_EMAIL;
            return;
        }

      for( eban = eban_list; eban; eban = eban->next )
      {
         if( !str_cmp( ch->pcdata->email, eban->name ) )
         {
           send_to_char("That email address has been banned.\n\r", ch);
           close_socket (d);
           return;
         }
      }

      /* In case we have level 4+ players from another merc mud, or 
       * players who have somehow got file access and changed their pfiles.
       */
      if (ch->level > 3 && ch->trust == 0)
         ch->level = 3;
      else
      {
         if (ch->level > MAX_LEVEL)
            ch->level = MAX_LEVEL;
         if (ch->trust > MAX_LEVEL)
            ch->trust = MAX_LEVEL;
      }

      if (ch->level >= 7)
      {
         stc ("#cLog on in wiz-invis mode?#n (y/n): ", ch);
         d->connected = CON_WIZ_INVIS;
         return;
      }
      do_help (ch, "motd");
      d->connected = CON_READ_MOTD;
      break;

   case CON_GET_EMAIL:
   {
      smash_tilde( argument );

      if( argument[0] == '\0' )
      {
         write_to_buffer( d, "You MUST enter a valid email.\n\r", 0);
         write_to_buffer( d, "Email address: ", 0 );
         d->connected = CON_GET_EMAIL;
       	 break; 
      }   

      free_string( ch->pcdata->email );
      ch->pcdata->email = str_dup(argument);

      for( eban = eban_list; eban; eban = eban->next )
      {
         if( !str_cmp( ch->pcdata->email, eban->name ) )
         {
           send_to_char("That email address has been banned.\n\r", ch);
           close_socket (d);
           return;
         }
      }


      /* In case we have level 4+ players from another merc mud, or 
       * players who have somehow got file access and changed their pfiles.
       */
      if (ch->level > 3 && ch->trust == 0)
         ch->level = 3;
      else
      {
         if (ch->level > MAX_LEVEL)
            ch->level = MAX_LEVEL;
         if (ch->trust > MAX_LEVEL)
            ch->trust = MAX_LEVEL;
      }

      if (ch->level >= 7)
      {
         stc ("#cLog on in wiz-invis mode?#n (y/n): ", ch);
         d->connected = CON_WIZ_INVIS;
         return;
      }
      do_help (ch, "motd");
      d->connected = CON_READ_MOTD;
      break;

   }   

   case CON_WIZ_INVIS:
      if (LOWER (*argument) == 'y')
      {
         SET_BIT (ch->act, PLR_WIZINVIS);
         SET_BIT (ch->act, PLR_INCOG);
         stc ("#cLogging on in wiz-invis.#n\n\r", ch);
         do_help (ch, "imotd");
         do_help (ch, "motd");
         d->connected = CON_READ_MOTD;
         return;
      }
      else if (LOWER (*argument) == 'n')
      {
         REMOVE_BIT (ch->act, PLR_WIZINVIS);
         REMOVE_BIT (ch->act, PLR_INCOG);
         stc ("Logging on without wiz-invis.\n\r", ch);
         do_help (ch, "imotd");
         do_help (ch, "motd");
         d->connected = CON_READ_MOTD;
         return;
      }
      else
         stc ("#cLog on in wiz-invis mode?#n (y/n): ", ch);
      return;


      write_to_buffer (d, "\n\r", 2);

      if (count_ability_levels (TALENTS, ch) + count_ability_levels (SKILLS, ch) + count_ability_levels (KNOWLEDGES, ch) >= 27)
      {
         if (IS_IMMORTAL (ch))
            do_help (ch, "imotd");
         else
            do_help (ch, "motd");
         d->connected = CON_READ_MOTD;
      }
      else
      {
         send_to_char ("\n\r#R XP is made slowly, so choose carefully. View the 'help vote' helpfile after you create.#n\n\r", ch);
         send_to_char ("\n\r#cDo you want #B[#CT#B]#calents, #B[#CS#B]#ckills, or #B[#CK#B]#cnowledges\n\rto be your area of expertise? ", ch);
         d->connected = CON_GET_PRIMARY_ABILITY_AREA;
      }
      break;

   case CON_ACCEPT_DISCLAIMER:
      switch (*argument)
      {
      case 'a': case 'A':
         write_to_buffer( d, "#wWelcome to A Streetlight Manifesto.\n\r", 0);
         write_to_buffer (d, "#cWhat is your sex (M/F)?#n ", 0);
         d->connected = CON_GET_NEW_SEX;
      break;

      case 'r': case 'R':
         do_help(d->character, "disclaimer");
      break;

      case 'd': case 'D':
         close_socket (d);
      return;

      default:
         write_to_buffer (d, "#CThis mud has some rules you have to abide by to play.  Do you wish to #c(#Br#c)#Cead the rules,#C(#Ba#C)#cgree or #C(#Bd#C)#cisagree to abide by them? ", 0);
         d->connected = CON_ACCEPT_DISCLAIMER;
      break;
      }
      break;

   case CON_CONFIRM_NEW_NAME:
      switch (*argument)
      {
      case 'y':
      case 'Y':
       sprintf(buf, "Enter a password for %s: ", ch->name);
       write_to_buffer(d, buf, 0);
       d->connected = CON_GET_NEW_PASSWORD;
      break;

      case 'n':
      case 'N':
         write_to_buffer (d, "Ok, what #RIS#n it, then? ", 0);
         free_char (d->character);
         d->character = NULL;
         d->connected = CON_GET_NAME;
         break;

      default:
         write_to_buffer (d, "Please type Yes or No. ", 0);
         break;
      }
      break;

   case CON_GET_NEW_PASSWORD:
#if defined(unix)
      write_to_buffer (d, "\n\r", 2);
#endif

      if (strlen (argument) < 5)
      {
         write_to_buffer (d, "#sP#sa#ss#ss#sw#so#sr#sd#n must be at least five characters long.\n\rPassword: ", 0);
         return;
      }

      pwdnew = crypt (argument, ch->name);

      for (p = pwdnew; *p != '\0'; p++)
      {
         if (*p == '~')
         {
            write_to_buffer (d, "New password not acceptable, try again!\n\rPassword: ", 0);
            return;
         }
      }

      free_string (ch->pcdata->pwd);
      ch->pcdata->pwd = str_dup (pwdnew);

      write_to_buffer (d, "Please retype password: ", 0);
      d->connected = CON_CONFIRM_NEW_PASSWORD;
      break;

   case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
      write_to_buffer (d, "\n\r", 2);
#endif

      if (strcmp (crypt (argument, ch->pcdata->pwd), ch->pcdata->pwd))
      {
         write_to_buffer (d, "Passwords don't match.\n\rRetype password: ", 0);
         d->connected = CON_GET_NEW_PASSWORD;
         return;
      }

      write_to_buffer (d, echo_on_str, 0);
      write_to_buffer (d, "#CThis mud has some rules you must agree to abide by to play.  Do you wish to #c(#Br#c)#Cead the rules,#C(#Ba#C)#cgree or #C(#Bd#C)#cisagree to abide by them? ", 0);
      d->connected = CON_ACCEPT_DISCLAIMER;
   break;

   case CON_GET_NEW_SEX:
      switch (argument[0])
      {
      case 'm':
      case 'M':
         ch->sex = SEX_MALE;
         break;
      case 'f':
      case 'F':
         ch->sex = SEX_FEMALE;
         break;
      default:
         write_to_buffer (d, "#cA gender please.. \n\rWhat #RIS#c your gender? ", 0);
         return;
      }
            
            write_to_buffer(d,"Please enter your email address:  ",0);
            d->connected = CON_GET_NEW_EMAIL;
      break;

   case CON_GET_NEW_EMAIL:
   {
      smash_tilde( argument );

      if( argument[0] == '\0' )
      {
         write_to_buffer( d, "You MUST enter a valid email.\n\r", 0);
         write_to_buffer( d, "Email address: ", 0 );
         d->connected = CON_GET_NEW_EMAIL;
         break;
      }   

      free_string( ch->pcdata->email );
      ch->pcdata->email = str_dup(argument);

      for( eban = eban_list; eban; eban = eban->next )
      {
         if( !str_cmp( ch->pcdata->email, eban->name ) )
         {
           send_to_char("That email address has been banned.\n\r", ch);
           close_socket (d);
           return;
         }
      }

      write_to_buffer (d, "\n\r", 2);
      write_to_buffer (d, "Do you wish to use #sA#sN#sS#sI#n color (y/n)? ", 0);
      d->connected = CON_GET_NEW_ANSI;
      break;
   }

   case CON_GET_NEW_ANSI:
      switch (argument[0])
      {
      case 'y':
      case 'Y':
         SET_BIT (ch->act, PLR_ANSI);
         break;
      case 'n':
      case 'N':
         break;
      default:
         write_to_buffer (d, "Do you wish to use #RA#yN#GS#CI#n (y/n)? ", 0);
         return;
      }
      d->connected = CON_GET_PRIMARY_STAT_AREA;
      write_to_buffer (d, "\n\r#R Visit www.astreetlightmanifesto.tk for assistance with the creation process.#n\n\r", 0);
      write_to_buffer (d, "\n\r#cDo you want [#CP#c]hysical, [#CS#c]ocial, or [#CM#c]ental skills\n\rto be your area of expertise? ", 0);
      break;

   case CON_GET_PRIMARY_STAT_AREA:
      if (ch->level == 0)
      {
         ch->attributes[ATTRIB_STR] = 1;
         ch->attributes[ATTRIB_DEX] = 1;
         ch->attributes[ATTRIB_STA] = 1;
         ch->attributes[ATTRIB_CHA] = 1;
         ch->attributes[ATTRIB_MAN] = 1;
         ch->attributes[ATTRIB_APP] = 1;
         ch->attributes[ATTRIB_PER] = 1;
         ch->attributes[ATTRIB_INT] = 1;
         ch->attributes[ATTRIB_WIT] = 1;
         
      }
      ch->pcdata->board = NULL;

      if (argument[0] == 'P' || argument[0] == 'S' || argument[0] == 'M')
         argument[0] = argument[0] + 32;


      switch (*argument)
      {
      case 'p':
      case 'P':
         write_to_buffer (d, "#cYou have choosen physical as your area of expertise.\n\r\n\r", 0);
         break;
      case 'm':
      case 'M':
         write_to_buffer (d, "#cYou have choosen mental as your area of expertise.\n\r\n\r", 0);
         break;
      case 's':
      case 'S':
         write_to_buffer (d, "#cYou have choosen social as your area of expertise.\n\r\n\r", 0);
         break;
      default:
         write_to_buffer (d, "#RError#C:#c Please choose what your primary stat group will be\n\r([P]hysical, [M]ental, or [S]ocial)", 0);
         return;
      }
      ch->pcdata->stat_groups[0] = *argument;
      d->connected = CON_GET_SECONDARY_STAT_AREA;
      write_to_buffer (d, "#cDo you want [#CP#c]hysical, [#CS#c]ocial, or [#CM#c]ental\n\r skills to be your secondary area of expertise? ", 0);
      break;


   case CON_GET_SECONDARY_STAT_AREA:
      if (argument[0] == 'P' || argument[0] == 'S' || argument[0] == 'M')
         argument[0] = argument[0] + 32;
      if (*argument == ch->pcdata->stat_groups[0])
         *argument = ' ';
      switch (*argument)
      {
      case 'p':
      case 'P':
         write_to_buffer (d, "You have choosen physical as your secondary area of expertise.\n\r\n\r", 0);
         break;
      case 'm':
      case 'M':
         write_to_buffer (d, "You have choosen mental as your secondary area of expertise.\n\r\n\r", 0);
         break;
      case 'S':
      case 's':
         write_to_buffer (d, "You have choosen social as your secondary area of expertise.\n\r\n\r", 0);
         break;
      default:
         write_to_buffer (d, "Error: Please choose what your secondary stat group will be\n\r([P]hysical, [S]ocial, or [M]ental)", 0);
         return;
      }
      ch->pcdata->stat_groups[1] = *argument;
      if ((ch->pcdata->stat_groups[0] == 'p' && ch->pcdata->stat_groups[1] == 'm') || (ch->pcdata->stat_groups[0] == 'm' && ch->pcdata->stat_groups[1] == 'p'))
         ch->pcdata->stat_groups[2] = 's';
      else if ((ch->pcdata->stat_groups[0] == 'p' && ch->pcdata->stat_groups[1] == 's') || (ch->pcdata->stat_groups[0] == 's' && ch->pcdata->stat_groups[1] == 'p'))
         ch->pcdata->stat_groups[2] = 'm';
      else if ((ch->pcdata->stat_groups[0] == 's' && ch->pcdata->stat_groups[1] == 'm') || (ch->pcdata->stat_groups[0] == 'm' && ch->pcdata->stat_groups[1] == 's'))
         ch->pcdata->stat_groups[2] = 'p';

      if (ch->pcdata->stat_groups[0] == 'p')
         j = 0;
      if (ch->pcdata->stat_groups[0] == 's')
         j = 1;
      if (ch->pcdata->stat_groups[0] == 'm')
         j = 2;
   /********* DISPLAY STATS ****************************/

      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);
      sprintf (tempbuf, "%s:[", attrib_names[j][0][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[0 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][1][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[1 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][2][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[2 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "Points Left:[");
      for (stat_display_counter = 0; stat_display_counter < points_distribution[0] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[0 + 3 * j]); stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]\n\r");
      write_to_buffer (d, tempbuf, 0);
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);

      sprintf (tempbuf, "1) Increase %-14s\t\t2) Decrease %-14s\n\r", attrib_names[j][0][0], attrib_names[j][0][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "3) Increase %-14s\t\t4) Decrease %-14s\n\r", attrib_names[j][1][0], attrib_names[j][1][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "5) Increase %-14s\t\t6) Decrease %-14s\n\r", attrib_names[j][2][0], attrib_names[j][2][0]);
      write_to_buffer (d, tempbuf, 0);
      d->connected = CON_GET_PRIMARY_STATS;
      break;

   case CON_GET_PRIMARY_STATS:
      if (ch->pcdata->stat_groups[0] == 'p')
         j = 0;
      else if (ch->pcdata->stat_groups[0] == 's')
         j = 1;
      else if (ch->pcdata->stat_groups[0] == 'm')
         j = 2;
      switch (argument[0])
      {
      case '1':
         if (ch->attributes[0 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[0 + 3 * j]++;
         break;
      case '2':
         if (ch->attributes[0 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[0 + 3 * j]--;
         break;
      case '3':
         if (ch->attributes[1 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[1 + 3 * j]++;
         break;
      case '4':
         if (ch->attributes[1 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[1 + 3 * j]--;
         break;
      case '5':
         if (ch->attributes[2 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[2 + 3 * j]++;
         break;
      case '6':
         if (ch->attributes[2 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[2 + 3 * j]--;
         break;
      }
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);
      sprintf (tempbuf, "%s:[", attrib_names[j][0][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[0 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][1][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[1 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][2][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[2 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "Points Left:[");
      for (stat_display_counter = 0; stat_display_counter < points_distribution[0] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[2 + 3 * j]); stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]\n\r");
      write_to_buffer (d, tempbuf, 0);
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);

      sprintf (tempbuf, "1) Increase %-14s\t\t2) Decrease %-14s\n\r", attrib_names[j][0][0], attrib_names[j][0][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "3) Increase %-14s\t\t4) Decrease %-14s\n\r", attrib_names[j][1][0], attrib_names[j][1][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "5) Increase %-14s\t\t6) Decrease %-14s\n\r", attrib_names[j][2][0], attrib_names[j][2][0]);
      write_to_buffer (d, tempbuf, 0);
      if (points_distribution[0] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[2 + 3 * j]) <= 0)
      {
         write_to_buffer (d, "\n\r\n\rYou are out of points, proceeding to next stat section\n\r", 0);
         write_to_buffer (d, "Press <Enter> to continue\n\r\n\r", 0);
         d->connected = CON_GET_SECONDARY_STATS;
         break;
      }

      d->connected = CON_GET_PRIMARY_STATS;
      break;

   case CON_GET_SECONDARY_STATS:
      if (ch->pcdata->stat_groups[1] == 'p')
         j = 0;
      else if (ch->pcdata->stat_groups[1] == 's')
         j = 1;
      else if (ch->pcdata->stat_groups[1] == 'm')
         j = 2;
      switch (argument[0])
      {
      case '1':
         if (ch->attributes[0 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[0 + 3 * j]++;
         break;
      case '2':
         if (ch->attributes[0 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[0 + 3 * j]--;
         break;
      case '3':
         if (ch->attributes[1 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[1 + 3 * j]++;
         break;
      case '4':
         if (ch->attributes[1 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[1 + 3 * j]--;
         break;
      case '5':
         if (ch->attributes[2 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[2 + 3 * j]++;
         break;
      case '6':
         if (ch->attributes[2 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[2 + 3 * j]--;
         break;
      }
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);
      sprintf (tempbuf, "%s:[", attrib_names[j][0][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[0 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][1][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[1 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][2][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[2 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "Points Left:[");
      for (stat_display_counter = 0; stat_display_counter < points_distribution[1] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[2 + 3 * j]); stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]\n\r");
      write_to_buffer (d, tempbuf, 0);
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);


      sprintf (tempbuf, "1) Increase %-14s\t\t2) Decrease %-14s\n\r", attrib_names[j][0][0], attrib_names[j][0][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "3) Increase %-14s\t\t4) Decrease %-14s\n\r", attrib_names[j][1][0], attrib_names[j][1][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "5) Increase %-14s\t\t6) Decrease %-14s\n\r", attrib_names[j][2][0], attrib_names[j][2][0]);
      write_to_buffer (d, tempbuf, 0);
      if (points_distribution[1] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[2 + 3 * j]) <= 0)
      {
         write_to_buffer (d, "\n\r\n\rYou are out of points, proceeding to next stat section\n\r", 0);
         write_to_buffer (d, "Press <Enter> to continue\n\r\n\r", 0);
         d->connected = CON_GET_TERTIARY_STATS;
         break;
      }

      d->connected = CON_GET_SECONDARY_STATS;
      break;

   case CON_GET_TERTIARY_STATS:
      if (ch->pcdata->stat_groups[2] == 'p')
         j = 0;
      else if (ch->pcdata->stat_groups[2] == 's')
         j = 1;
      else if (ch->pcdata->stat_groups[2] == 'm')
         j = 2;
      switch (argument[0])
      {
      case '1':
         if (ch->attributes[0 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[0 + 3 * j]++;
         break;
      case '2':
         if (ch->attributes[0 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[0 + 3 * j]--;
         break;
      case '3':
         if (ch->attributes[1 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[1 + 3 * j]++;
         break;
      case '4':
         if (ch->attributes[1 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[1 + 3 * j]--;
         break;
      case '5':
         if (ch->attributes[2 + 3 * j] == MAX_CHAR_CREATION_ATTRIB)
         {
            write_to_buffer (d, "I'm sorry you have already maxxed that attribute\n\r", 0);
            break;
         }
         ch->attributes[2 + 3 * j]++;
         break;
      case '6':
         if (ch->attributes[2 + 3 * j] == 1)
         {
            write_to_buffer (d, "I'm sorry you cannot go BELOW 1 on any attribute\n\r", 0);
            break;
         }
         ch->attributes[2 + 3 * j]--;
         break;
      }
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);
      sprintf (tempbuf, "%s:[", attrib_names[j][0][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[0 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][1][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[1 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "%s:[", attrib_names[j][2][0]);
      for (stat_display_counter = 0; stat_display_counter < ch->attributes[2 + 3 * j]; stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]   ");
      write_to_buffer (d, tempbuf, 0);

      sprintf (tempbuf, "Points Left:[");
      for (stat_display_counter = 0; stat_display_counter < points_distribution[2] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[2 + 3 * j]); stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "]\n\r");
      write_to_buffer (d, tempbuf, 0);
      write_to_buffer (d, "------------------------------------------------------------------\n\r", 0);


      sprintf (tempbuf, "1) Increase %-14s\t\t2) Decrease %-14s\n\r", attrib_names[j][0][0], attrib_names[j][0][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "3) Increase %-14s\t\t4) Decrease %-14s\n\r", attrib_names[j][1][0], attrib_names[j][1][0]);
      write_to_buffer (d, tempbuf, 0);
      sprintf (tempbuf, "5) Increase %-14s\t\t6) Decrease %-14s\n\r", attrib_names[j][2][0], attrib_names[j][2][0]);
      write_to_buffer (d, tempbuf, 0);
      if (points_distribution[2] - (ch->attributes[0 + 3 * j] + ch->attributes[1 + 3 * j] + ch->attributes[2 + 3 * j]) <= 0)
      {
         write_to_buffer (d, "\n\r\n\rYou are out of points, proceeding to next stat section\n\r", 0);
         write_to_buffer (d, "Press <Enter> to continue\n\r\n\r", 0);

         d->connected = CON_GET_PRIMARY_ABILITY_AREA;
         write_to_buffer (d, "\n\r#BDo you want [#eT#B]alents, [#eS#B]kills, or [#eK#B]nowledges\n\rto be your area of expertise? ", 0);
         break;
      }

      d->connected = CON_GET_TERTIARY_STATS;
      break;

   case CON_GET_PRIMARY_ABILITY_AREA:
      // Reset everything to 0
      for (i = 0; i < 3; i++)
         for (j = 0; j < 10; j++)
         {
            ch->abilities[i][j] = 0;
            ch->pcdata->abilities_points[i][j] = 0;
         }

      if (argument[0] == 'T' || argument[0] == 'S' || argument[0] == 'K')
         argument[0] = argument[0] + 32;


      switch (*argument)
      {
      case 't':
      case 'T':
         write_to_buffer (d, "#BYou have choosen talents as your area of expertise.\n\r\n\r", 0);
         break;
      case 's':
      case 'S':
         write_to_buffer (d, "#BYou have choosen skills as your area of expertise.\n\r\n\r", 0);
         break;
      case 'k':
      case 'K':
         write_to_buffer (d, "#BYou have choosen knowledges as your area of expertise.\n\r\n\r", 0);
         break;
      default:
         write_to_buffer (d, "#RError#B: Please choose what your primary expertise group will be\n\r([#eT#B]alents, [#eS#B]kills, or [#eK#B]nowledges)", 0);
         return;
      }
      ch->pcdata->ability_groups[0] = *argument;
      write_to_buffer (d, "\n\r#BDo you want [#eT#B]alents, [#eS#B]kills, or [#eK#B]nowledges\n\rto be your secondary area of expertise? ", 0);
      d->connected = CON_GET_SECONDARY_ABILITY_AREA;
      break;

   case CON_GET_SECONDARY_ABILITY_AREA:
      if (argument[0] == 'T' || argument[0] == 'S' || argument[0] == 'K')
         argument[0] = argument[0] + 32;

      if (*argument == ch->pcdata->ability_groups[0])
         *argument = ' ';

      switch (*argument)
      {
      case 't':
      case 'T':
         write_to_buffer (d, "#BYou have choosen talents as your secondary area of expertise.\n\r\n\r", 0);
         break;
      case 's':
      case 'S':
         write_to_buffer (d, "#BYou have choosen skills as your secondary area of expertise.\n\r\n\r", 0);
         break;
      case 'k':
      case 'K':
         write_to_buffer (d, "#BYou have choosen knowledges as your secondary area of expertise.\n\r\n\r", 0);
         break;
      default:
         write_to_buffer (d, "#RError#B: Please choose what your secondary expertise group will be\n\r([#eT#B]alents, [#eS#B]kills, or [#eK#B]nowledges)", 0);
         return;
      }
      ch->pcdata->ability_groups[1] = *argument;
      if ((ch->pcdata->ability_groups[0] == 't' && ch->pcdata->ability_groups[1] == 'k') || (ch->pcdata->ability_groups[0] == 'k' && ch->pcdata->ability_groups[1] == 't'))
         ch->pcdata->ability_groups[2] = 's';
      else if ((ch->pcdata->ability_groups[0] == 't' && ch->pcdata->ability_groups[1] == 's') || (ch->pcdata->ability_groups[0] == 's' && ch->pcdata->ability_groups[1] == 't'))
         ch->pcdata->ability_groups[2] = 'k';
      else if ((ch->pcdata->ability_groups[0] == 's' && ch->pcdata->ability_groups[1] == 'k') || (ch->pcdata->ability_groups[0] == 'k' && ch->pcdata->ability_groups[1] == 's'))
         ch->pcdata->ability_groups[2] = 't';
      sprintf (log_buf, "#RAREAS: %c  %c  %c.\n\r", ch->pcdata->ability_groups[0], ch->pcdata->ability_groups[1], ch->pcdata->ability_groups[2]);
      send_to_char (log_buf, ch);


      if (ch->pcdata->ability_groups[0] == 't')
         j = TALENTS;
      if (ch->pcdata->ability_groups[0] == 's')
         j = SKILLS;
      if (ch->pcdata->ability_groups[0] == 'k')
         j = KNOWLEDGES;
   /********* DISPLAY STATS ****************************/

      write_to_buffer (d, "#c------------------------------------------------------------------\n\r", 0);
      for (i = 0; i < 10; i++)
      {
         if (i % 3 == 0)
            send_to_char ("\n\r", ch);
         sprintf (tempbuf, "#B%-15s:#c[#C", ability_names[j][i][0]);
         for (stat_display_counter = 0; stat_display_counter < ch->abilities[j][i]; stat_display_counter++)
            strcat (tempbuf, "o");
         strcat (tempbuf, "#c]    ");
         send_to_char (tempbuf, ch);
      }
      send_to_char ("\n\r", ch);

      sprintf (tempbuf, "#BPoints Left:[#e");
      for (stat_display_counter = 0; stat_display_counter < abilities_points_distribution[0] - count_ability_levels (j, ch); stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "#B]\n\r");
      send_to_char (tempbuf, ch);
      send_to_char ("#c------------------------------------------------------------------\n\r", ch);

      for (i = 0; i < 10; i++)
      {
         sprintf (tempbuf, "#B%d) #cIncrease %-14s\t\t%d) Decrease %-14s\n\r", i, ability_names[j][i][0], i + 10, ability_names[j][i][0]);
         send_to_char (tempbuf, ch);
      }
      send_to_char ("#c------------------------------------------------------------------\n\r", ch);
      d->connected = CON_GET_ABILITIES;
      break;

   case CON_GET_ABILITIES:
      if (ch->pcdata->ability_groups[0] == 't')
         j = TALENTS;
      else if (ch->pcdata->ability_groups[0] == 's')
         j = SKILLS;
      else if (ch->pcdata->ability_groups[0] == 'k')
         j = KNOWLEDGES;
      ability_level = 0;

      if (count_ability_levels (j, ch) == 13)
      {
         if (ch->pcdata->ability_groups[1] == 't')
            j = TALENTS;
         else if (ch->pcdata->ability_groups[1] == 's')
            j = SKILLS;
         else if (ch->pcdata->ability_groups[1] == 'k')
            j = KNOWLEDGES;
         ability_level = 1;

         if (count_ability_levels (j, ch) == 9)
         {
            if (ch->pcdata->ability_groups[2] == 't')
               j = TALENTS;
            else if (ch->pcdata->ability_groups[2] == 's')
               j = SKILLS;
            else if (ch->pcdata->ability_groups[2] == 'k')
               j = KNOWLEDGES;
            ability_level = 2;

            if (count_ability_levels (j, ch) == 5)
            {
                sprintf (log_buf, "%s@%s new player.", ch->name, d->host);
                log_string (log_buf, CHANNEL_LOG_CODER);


      	    	if (ch->pcdata->vision < 1)
         		ch->pcdata->vision = 3;

	    	if (ch->pcdata->rank < 0)
            		ch->pcdata->rank = 0;

	    	break;
            }
         }
      }

      if (strlen (argument) == 1)
      {
         argument[1] = argument[0];
         argument[0] = '0';
         argument[2] = '\0';
      }

      if (strlen (argument) == 0)
         send_to_char ("\n\r", ch);
      else if ((argument[0] >= 48 && argument[0] <= 57) && (argument[1] >= 48 && argument[1] <= 57))
      {
         temp_num = argument[1] - 48;
         if (argument[0] == '0')
         {
            if (ch->abilities[j][temp_num] >= MAX_CHAR_CREATION_ABILITY)
               send_to_char ("#RError#B:#C You simply cannot go any higher with that ability.\n\r", ch);
            else
            {
               ch->abilities[j][temp_num] = ch->abilities[j][temp_num] + 1;
               ch->pcdata->abilities_points[j][temp_num] = ability_insight_levels[ch->abilities[j][temp_num]];
            }

         }
         else if (argument[0] == '1')
         {
            if (ch->abilities[j][temp_num] <= 0)
               send_to_char ("#RError#B: How do you propose to have a negative ability?\n\r", ch);
            else
            {
               ch->abilities[j][temp_num] = ch->abilities[j][temp_num] - 1;
               ch->pcdata->abilities_points[j][temp_num] = ability_insight_levels[ch->abilities[j][temp_num]];
            }


         }
         else
            send_to_char ("#RError#B: Try Again\n\r", ch);
      }
      else
         send_to_char ("#RError#B: Send the proper input, twit.\n\r", ch);


      write_to_buffer (d, "#B------------------------------------------------------------------\n\r", 0);
      for (i = 0; i < 10; i++)
      {
         if (i % 3 == 0)
            send_to_char ("\n\r", ch);
         sprintf (tempbuf, "#B%-15s:[#e", ability_names[j][i][0]);
         for (stat_display_counter = 0; stat_display_counter < ch->abilities[j][i]; stat_display_counter++)
            strcat (tempbuf, "o");
         strcat (tempbuf, "#B]    ");
         send_to_char (tempbuf, ch);
      }
      send_to_char ("\n\r", ch);

      sprintf (tempbuf, "#BPoints Left:[#e");
      for (stat_display_counter = 0; stat_display_counter < abilities_points_distribution[ability_level] - count_ability_levels (j, ch); stat_display_counter++)
         strcat (tempbuf, "o");
      strcat (tempbuf, "#B]\n\r");
      send_to_char (tempbuf, ch);
      send_to_char ("#B------------------------------------------------------------------\n\r", ch);

      for (i = 0; i < 10; i++)
      {
         sprintf (tempbuf, "#B%d) Increase %-14s\t\t%d) Decrease %-14s\n\r", i, ability_names[j][i][0], i + 10, ability_names[j][i][0]);
         send_to_char (tempbuf, ch);
      }
      send_to_char ("#B------------------------------------------------------------------\n\r", ch);

      if ((abilities_points_distribution[ability_level] - count_ability_levels (j, ch)) <= 0)
      {
         send_to_char ("\n\r\n\r#BYou are out of points, proceeding to next abilities section\n\r", ch);
         send_to_char ("#B<Press Enter to Continue>\n\r", ch);
      }
      if(ability_level == 2 && (abilities_points_distribution[ability_level] - count_ability_levels (j, ch)) <= 0)
      {
      	d->connected = CON_READ_MOTD;
      	break;
      }

      d->connected = CON_GET_ABILITIES;
      break;



   case CON_READ_MOTD:


      if (IS_SET (ch->act, PLR_VANISH) && !IS_WEREWOLF (ch))
      {
         REMOVE_BIT (ch->act, PLR_VANISH);
      }
      ch->next = char_list;
      char_list = ch;
      d->connected = CON_PLAYING;
      total_number_login++;

      if (ch->pcdata->vision < 1)
         ch->pcdata->vision = 3;

      if (ch->pcdata->rank < 0)
         ch->pcdata->rank = 0;

      if (!IS_IMMORTAL (ch) && IS_SET (ch->act, PLR_HOLYLIGHT))
      {
         REMOVE_BIT (ch->act, PLR_HOLYLIGHT);
         write_to_buffer (d, "\n\r#3Removing truesight for new code.#n\n\r", 0);
      }

      if (ch->level == 0)
      {
	 ch->memory = NULL;
	 ch->pcdata->roomdesc = str_dup("");
	 ch->pcdata->wp_regain_counter = 1;
	 ch->pcdata->blood_burn_counter = 1;
	 ch->pcdata->auto_vote_counter = 1;
         ch->level = 3;
         ch->mkill = 5;
         ch->hit = ch->max_hit;
         ch->mana = ch->max_mana;
         ch->move = ch->max_move;
         ch->health_level = HEALTH_NORMAL;
         set_title (ch, "is new");
         ch->exp = 0;
         ch->pcdata->class = ch->class;
         ch->pcdata->security = 9;   /* OLC */
         ch->class = CLASS_NONE;
         ch->hit = ch->max_hit;
         ch->mana = ch->max_mana;
         ch->move = ch->max_move;
         ch->pcdata->vision = 3;
         ch->gold = 20;
         ch->pcdata->board = &boards[DEFAULT_BOARD];
         send_to_char ("--------------------------------------------------------------------------------\n\r", ch);
         send_to_char ("If you need help, just ask over OOC. See #Bhelp channels#n for details.\n\r", ch);
         send_to_char ("--------------------------------------------------------------------------------\n\r", ch);

         char_to_room (ch, get_room_index (ROOM_VNUM_SCHOOL));
         /*** Newbie Equipment ***/
         obj_to_char( create_object(get_obj_index( 3355 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3358 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 1011 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 1008 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 1016 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3706 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 9563 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3365 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3364 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3364 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3362 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3032 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 1002 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3361 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3363 ), 1), ch );
         obj_to_char( create_object(get_obj_index( 3363 ), 1), ch );

	 do_look (ch, "auto");
         save_char_obj( ch );
         do_save( ch, "");
         do_autoexit( ch, "");

      }
      else if (!IS_NPC (ch) && ch->pcdata->obj_vnum != 0)
      {
         if (ch->in_room != NULL)
            char_to_room (ch, ch->in_room);
         else
            char_to_room (ch, get_room_index (ROOM_VNUM_SCHOOL));
         bind_char (ch);
         break;
      }
#if 0
      else if (ch->x >= 0 && ch->y >= 0)
      {
         char_to_room (ch, get_room_index (WILDERNESS_LOW_VNUM));
      }
#endif
      else if (ch->in_room != NULL)
      {
         char_to_room (ch, ch->in_room);
         do_look (ch, "auto");
      }
      else if (IS_IMMORTAL (ch))
      {
         char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
         do_look (ch, "auto");
      }
      else
      {
         char_to_room (ch, get_room_index (ROOM_VNUM_LIMBO));
         do_look (ch, "auto");
      }

//      if ( ( ( get_age(ch) - 17 ) * 2) < 2 )
      //if (strlen(ch->pcdata->roomdesc) < 1)
      //sprintf (buf, "#GSome new person #ehas entered #gM#Giami#e by#w N#eight.#n");
      //else
      sprintf (buf, "#G%s #ehas entered #gM#Giami#e by#w N#eight.#n", ch->pcdata->roomdesc);
 
     if ( !IS_SET( ch->act, PLR_WIZINVIS))
         do_info (ch, buf);
      act ("#C$n#c has entered the game.#c", ch, NULL, NULL, TO_ROOM);
      
      room_text (ch, ">ENTER<");
      ch->pcdata->board = &boards[DEFAULT_BOARD];
      do_board (ch, "");   /* Show board status */

      if (ch->class == CLASS_NONE)
      {
         if (IS_WEREWOLF (ch))
            ch->class += CLASS_WEREWOLF;
      }
      break;

      /* states for new note system, (c)1995-96 erwin@andreasen.org */
      /* ch MUST be PC here; have nwrite check for PC status! */

   case CON_NOTE_TO:
      handle_con_note_to (d, argument);
      break;

   case CON_NOTE_SUBJECT:
      handle_con_note_subject (d, argument);
      break;      /* subject */

   case CON_NOTE_EXPIRE:
      handle_con_note_expire (d, argument);
      break;

   case CON_NOTE_TEXT:
      handle_con_note_text (d, argument);
      break;

   case CON_NOTE_FINISH:
      handle_con_note_finish (d, argument);
      break;
   case CON_NOT_PLAYING:
      break;
   }

   return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name (char *name)
{
   /*
    * Reserved words.
    */

   if (is_name (name, "narses")) return TRUE;
   if (is_name (name, "all auto immortal self someone gaia"))
      return FALSE;

   if (is_in (name, "|shit*self*fuck*arse*cunt*wank*assh*dick*cock*bitch*"))
      return FALSE;

   /*
    * Length restrictions.
    */
   if (strlen (name) < 3)
      return FALSE;

#if defined(MSDOS)
   if (strlen (name) > 8)
      return FALSE;
#endif

#if defined(macintosh) || defined(unix)
   if (strlen (name) > 15)
      return FALSE;
#endif

   /*
    * Alphanumerics only.
    * Lock out IllIll twits.
    */
   {
      char *pc;
      bool fIll;

      fIll = TRUE;
      for (pc = name; *pc != '\0'; pc++)
      {
         if (!isalpha (*pc))
            return FALSE;
         if (LOWER (*pc) != 'i' && LOWER (*pc) != 'l')
            fIll = FALSE;
      }

      if (fIll)
         return FALSE;
   }

   return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect (DESCRIPTOR_DATA * d, char *name, bool fConn)
{
   CHAR_DATA *ch;

   for (ch = char_list; ch != NULL; ch = ch->next)
   {
      if (!IS_NPC (ch) && !IS_EXTRA (ch, EXTRA_SWITCH) && (!fConn || ch->desc == NULL) && !str_cmp (d->character->name, ch->name))
      {
         if (fConn == FALSE)
         {
            free_string (d->character->pcdata->pwd);
            d->character->pcdata->pwd = str_dup (ch->pcdata->pwd);
         }
         else
         {
            free_char (d->character);
            d->character = ch;
            ch->desc = d;
            ch->timer = 0;

            send_to_char ("Reconnecting.\n\r", ch);
            if (IS_NPC (ch) || ch->pcdata->obj_vnum == 0)
               act ("$n is no longer linkdead", ch, NULL, NULL, TO_ROOM);
            sprintf (log_buf, "%s reconnected.", ch->name);
            log_string (log_buf, CHANNEL_LOG_NORMAL);
            d->connected = CON_PLAYING;
            /* Inform the character of a note in progress and the possbility of continuation! */

            if (ch->pcdata->in_progress)
               send_to_char ("You have a note in progress. Type NWRITE to continue it.\n\r", ch);

         }
         return TRUE;
      }
   }

   return FALSE;
}

/*
 * Kick off old connection.  KaVir.
 */
bool check_kickoff (DESCRIPTOR_DATA * d, char *name, bool fConn)
{
   CHAR_DATA *ch;

   for (ch = char_list; ch != NULL; ch = ch->next)
   {
      if (!IS_NPC (ch) && (!fConn || ch->desc == NULL) && !str_cmp (d->character->name, ch->name))
      {
         if (fConn == FALSE)
         {
            free_string (d->character->pcdata->pwd);
            d->character->pcdata->pwd = str_dup (ch->pcdata->pwd);
         }
         else
         {
            free_char (d->character);
            d->character = ch;
            ch->desc = d;
            ch->timer = 0;
            send_to_char ("You take over your body, which was already in use.\n\r", ch);
            act ("...$n's body has been taken over by another spirit!", ch, NULL, NULL, TO_ROOM);
            sprintf (log_buf, "%s kicking off old link.", ch->name);
            log_string (log_buf, CHANNEL_LOG_NORMAL);
            d->connected = CON_PLAYING;
         }
         return TRUE;
      }
   }

   return FALSE;
}



/*
 * Check if already playing - KaVir.
 * Using kickoff code from Malice, as mine is v. dodgy.
 */
bool check_playing (DESCRIPTOR_DATA * d, char *name)
{
   DESCRIPTOR_DATA *dold;

   for (dold = descriptor_list; dold != NULL; dold = dold->next)
   {
      if (dold != d && dold->character != NULL && dold->connected != CON_GET_NAME && dold->connected != CON_GET_OLD_PASSWORD && !str_cmp (name, dold->original ? dold->original->name : dold->character->name))
      {
         char buf[MAX_STRING_LENGTH];
         if (d->character != NULL)
         {
            free_char (d->character);
            d->character = NULL;
         }
         send_to_char ("This body has been taken over!\n\r", dold->character);
         d->connected = dold->connected;
         d->character = dold->character;
         d->character->desc = d;
		 d->character->ploaded = FALSE;
         send_to_char ("You take over your body, which was already in use.\n\r", d->character);
         act ("$n doubles over in agony and $s eyes roll up into $s head.", d->character, NULL, NULL, TO_ROOM);
         act ("...$n's body has been taken over by another spirit!", d->character, NULL, NULL, TO_ROOM);
         dold->character = NULL;

         sprintf (buf, "Kicking off old connection %s", d->character->name);
         log_string (buf, CHANNEL_LOG_NORMAL);
         close_socket (dold);   /*Slam the old connection into the ether */
         return TRUE;
      }
   }

   return FALSE;
}



void stop_idling (CHAR_DATA * ch)
{
   if (ch == NULL || ch->desc == NULL || ch->desc->connected != CON_PLAYING || ch->was_in_room == NULL || ch->in_room != get_room_index (ROOM_VNUM_LIMBO))
      return;

   ch->timer = 0;
   char_from_room (ch);
   char_to_room (ch, ch->was_in_room);
   ch->was_in_room = NULL;
   act ("$n has returned from the void.", ch, NULL, NULL, TO_ROOM);
   return;
}



/*
 * Send a page to one char.
 */
void page_to_char (const char *txt, CHAR_DATA * ch)
{
   if (txt == NULL || ch == NULL || (ch != NULL && ch->desc == NULL))
      return;

   free_string (ch->desc->showstr_head);
   ch->desc->showstr_head = str_dup (txt);
   ch->desc->showstr_point = ch->desc->showstr_head;
   show_string (ch->desc, "");
}


/*
 * The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom for porting
 * this SillyMud code for MERC 2.0 and laying down the groundwork. Thanks
 * to Blackstar, hopper.cs.uiowa.edu 4000 for which the improvements to
 * the pager was modeled from.  - Kahn 
 */

void show_string (struct descriptor_data *d, char *input)
{
   char buffer[MAX_STRING_LENGTH * 3];
   char buf[MAX_INPUT_LENGTH];
   register char *scan, *chk;
   int lines = 0, toggle = 1;

   one_argument (input, buf);

   switch (UPPER (buf[0]))
     {
     case '\0':
     case 'C':            
        lines = 0;
        break;

     case 'R':              
        lines = -1 - (30);
        break;

     case 'B':           
        lines = -(2 * 30);
        break;

     case 'H':         
        write_to_buffer (d,
                   "C, or Return = continue, R = redraw this page,\n\r", 0);
        write_to_buffer (d,
        "B = back one page, H = this help, Q or other keys = exit.\n\r\n\r",
                         0);
        lines = -1 - (30);
        break;

     default:                   
                                  
        if (d->showstr_head)
          {
             free_string (d->showstr_head);
             d->showstr_head = str_dup ("");
          }
        d->showstr_point = 0;
        return;
     }

   /*
    * do any backing up necessary 
    */
   if (lines < 0)
     {
        for (scan = d->showstr_point; scan > d->showstr_head; scan--)
           if ((*scan == '\n') || (*scan == '\r'))
             {
                toggle = -toggle;
                if (toggle < 0)
                   if (!(++lines))
                      break;
             }
        d->showstr_point = scan;
     }
   /*
    * show a chunk 
    */
   lines = 0;
   toggle = 1;
   for (scan = buffer;; scan++, d->showstr_point++)
      if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
          && (toggle = -toggle) < 0)
         lines++;
      else if (!*scan || (d->character && !IS_NPC (d->character)
                          && lines >= 30))
        {

           *scan = '\0';
           write_to_buffer (d, buffer, strlen (buffer));

           /*
            * See if this is the end (or near the end) of the string 
            */
           for (chk = d->showstr_point; isspace (*chk); chk++);
           if (!*chk)
             {
                if (d->showstr_head)
                  {
                     free_string (d->showstr_head);
                     d->showstr_head = str_dup ("");
                  }
                d->showstr_point = 0;
             }
           return;
        }
   return;
}

/*
 * Write to one char.
 */
void send_to_char (const char *txt, CHAR_DATA * ch)
{
   if (txt != NULL && ch->desc != NULL)
      write_to_buffer (ch->desc, txt, strlen (txt));
   return;
}



/*
 * Write to all characters.
 */
void send_to_all_char (const char *text)
{
   DESCRIPTOR_DATA *d;

   if (text == NULL)
      return;
   for (d = descriptor_list; d != NULL; d = d->next)
      if (d->connected == CON_PLAYING)
         send_to_char (text, d->character);

   return;
}


/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt (DESCRIPTOR_DATA * d)
{
   CHAR_DATA *ch;
   CHAR_DATA *victim;
   CHAR_DATA *tank;
   const char *str;
   const char *i;
   char *point;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   bool is_fighting = TRUE;

   if ((ch = d->character) == NULL)
      return;
   if (ch->pcdata == NULL)
   {
      send_to_char ("\n\r\n\r", ch);
      return;
   }
   if (ch->position == POS_FIGHTING && ch->cprompt[0] == '\0')
   {
      if (ch->prompt[0] == '\0')
      {
         send_to_char ("\n\r\n\r", ch);
         return;
      }
      is_fighting = FALSE;
   }
   else if (ch->position != POS_FIGHTING && ch->prompt[0] == '\0')
   {
      send_to_char ("\n\r\n\r", ch);
      return;
   }

   point = buf;
   if (ch->position == POS_FIGHTING && is_fighting)
      str = d->original ? d->original->cprompt : d->character->cprompt;
   else
      str = d->original ? d->original->prompt : d->character->prompt;
   while (*str != '\0')
   {
      if (*str != '%')
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch (*str)
      {
      default:
         i = " ";
         break;
      case 'h':
         sprintf (buf2, "%d", ch->hit);
         COL_SCALE (buf2, ch, ch->hit, ch->max_hit);
         i = buf2;
         break;
      case 'H':
         sprintf (buf2, "%d", ch->max_hit);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
         break;

      case 'a':   break;
      case 'A':break;
      case 'b': break;
      case 'c': break;
      case 'd':break;
      case 'D':      break;
      case 'i':break;
      case 'I':      break;
      case 'j':break;
      case 'J':      break;
      case 'k':break;
      case 'K':      break;
      case 'l':      break;
      case 'L':      break;
      case 'm': break;
      case 'M':      break;
      case 'o':      break;
      case 'O':      break;
      case 'p':      break;
      case 'P':      break;
      case 'q':      break;
      case 'Q':      break;
      case 's':      break;
      case 'S':      break;
      case 't':
         sprintf (buf2, "%d",    ch->pcdata->rage[0]);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
      break;
      case 'T':
         sprintf (buf2, "%d",    ch->pcdata->rage[1]);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
      break;
      case 'u':
      sprintf (buf2, "%d", ch->pcdata->gnosis[0]);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
      break;
      case 'U':
         sprintf (buf2, "%d",    ch->pcdata->gnosis[1]);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
      break;
      case 'v':      break;
      case 'V':   break;
      case 'y':      break;
      case 'Y':      break;
      case 'x':
         sprintf (buf2, "%d", ch->exp);
         COL_SCALE (buf2, ch, ch->exp, 100);
         i = buf2;
         break;
      case 'g':
         sprintf (buf2, "%d", ch->gold);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
         break;
      case 'f':
         if ((victim = ch->fighting) == NULL)
         {
            strcpy (buf2, "N/A");
            ADD_COLOUR (ch, buf2, L_CYAN);
         }
         else
         {
            if (victim->hit < 1 || victim->max_hit < 1)
            {
               strcpy (buf2, "Incapacitated");
               ADD_COLOUR (ch, buf2, L_RED);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 17)
            {
               strcpy (buf2, "Crippled");
               ADD_COLOUR (ch, buf2, L_RED);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 34)
            {
               strcpy (buf2, "Mauled");
               ADD_COLOUR (ch, buf2, L_BLUE);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 50)
            {
               strcpy (buf2, "Wounded");
               ADD_COLOUR (ch, buf2, L_BLUE);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 75)
            {
               strcpy (buf2, "Injured");
               ADD_COLOUR (ch, buf2, L_GREEN);
            }
            else if ((victim->hit * 100 / victim->max_hit) < 100)
            {
               strcpy (buf2, "Hurt");
               ADD_COLOUR (ch, buf2, L_GREEN);
            }
            else if ((victim->hit * 100 / victim->max_hit) >= 100)
            {
               strcpy (buf2, "Bruised");
               ADD_COLOUR (ch, buf2, YELLOW);
            }
            else if ((victim->hit * 100 / victim->max_hit) >= 100)
            {
               strcpy (buf2, "Perfect");
               ADD_COLOUR (ch, buf2, L_CYAN);
            }
         }
         i = buf2;
         break;
      case 'F':
         if ((victim = ch->fighting) == NULL)
         {
            strcpy (buf2, "N/A");
            ADD_COLOUR (ch, buf2, L_CYAN);
         }
         else if ((tank = victim->fighting) == NULL)
         {
            strcpy (buf2, "N/A");
            ADD_COLOUR (ch, buf2, L_CYAN);
         }
         else
         {
            if (tank->hit < 1 || tank->max_hit < 1)
            {
               strcpy (buf2, "Incapacitated");
               ADD_COLOUR (ch, buf2, L_RED);
            }
            else if ((tank->hit * 100 / tank->max_hit) < 17)
            {
               strcpy (buf2, "Crippled");
               ADD_COLOUR (ch, buf2, L_RED);
            }
            else if ((tank->hit * 100 / tank->max_hit) < 34)
            {
               strcpy (buf2, "Mauled");
               ADD_COLOUR (ch, buf2, L_BLUE);
            }
            else if ((tank->hit * 100 / tank->max_hit) < 50)
            {
               strcpy (buf2, "Wounded");
               ADD_COLOUR (ch, buf2, L_BLUE);
            }
            else if ((tank->hit * 100 / tank->max_hit) < 75)
            {
               strcpy (buf2, "Injured");
               ADD_COLOUR (ch, buf2, L_GREEN);
            }
            else if ((tank->hit * 100 / tank->max_hit) < 100)
            {
               strcpy (buf2, "Hurt");
               ADD_COLOUR (ch, buf2, L_GREEN);
            }
            else if ((tank->hit * 100 / tank->max_hit) >= 100)
            {
               strcpy (buf2, "Bruised");
               ADD_COLOUR (ch, buf2, YELLOW);
            }
            else if ((tank->hit * 100 / tank->max_hit) >= 100)
            {
               strcpy (buf2, "Perfect");
               ADD_COLOUR (ch, buf2, L_CYAN);
            }
         }
         i = buf2;
         break;
      case 'n':
         if ((victim = ch->fighting) == NULL)
            strcpy (buf2, "N/A");
         else
         {
            if (IS_AFFECTED (victim, AFF_POLYMORPH))
               strcpy (buf2, victim->morph);
            else if (IS_NPC (victim))
               strcpy (buf2, victim->short_descr);
            else
               strcpy (buf2, victim->name);
            buf2[0] = UPPER (buf2[0]);
         }
         i = buf2;
         break;
      case 'N':
         if ((victim = ch->fighting) == NULL)
            strcpy (buf2, "N/A");
         else if ((tank = victim->fighting) == NULL)
            strcpy (buf2, "N/A");
         else
         {
            if (ch == tank)
               strcpy (buf2, "You");
            else if (IS_AFFECTED (tank, AFF_POLYMORPH))
               strcpy (buf2, tank->morph);
            else if (IS_NPC (victim))
               strcpy (buf2, tank->short_descr);
            else
               strcpy (buf2, tank->name);
            buf2[0] = UPPER (buf2[0]);
         }
         i = buf2;
         break;
      case 'e':
         if ((victim = ch->embrace) == NULL)
            strcpy (buf2, "N/A");
         else
         {
            if (IS_AFFECTED (victim, AFF_POLYMORPH))
               strcpy (buf2, victim->morph);
            else if (IS_NPC (victim))
               strcpy (buf2, victim->short_descr);
            else
               strcpy (buf2, victim->name);
            buf2[0] = UPPER (buf2[0]);
         }
         i = buf2;
         break;
      case 'E':
         if ((victim = ch->embrace) == NULL)
         {
            strcpy (buf2, "N/A");
            ADD_COLOUR (ch, buf2, L_CYAN);
         }
         else
         {
            sprintf (buf2, "%d", victim->blood[BLOOD_CURRENT]);
            ADD_COLOUR (ch, buf2, L_RED);
         }
         i = buf2;
         break;
      case 'r':
         if (!IS_NPC(ch))
            {
               if (IS_MORE2 (ch, MORE2_RPFLAG))
                     strcpy (buf2, "#Crpflag#n");
               else
                     strcpy (buf2, "#cno rpflag#C");
            }
         else
            strcpy (buf2, "n/a");

         i = buf2;


         break;
      case 'W':
         sprintf (buf2, "%d", ch->pcdata->willpower[WILLPOWER_MAX]);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
         break;
      case 'w':
         sprintf (buf2, "%d", ch->pcdata->willpower[WILLPOWER_CURRENT]);
         ADD_COLOUR (ch, buf2, L_CYAN);
         i = buf2;
         break;

      case 'B':
         sprintf (buf2, "%d", ch->blood[BLOOD_CURRENT]);
         ADD_COLOUR (ch, buf2, D_RED);
         i = buf2;
         break;
      case 'R':
         sprintf (buf2, "%d", ch->blood[BLOOD_CURRENT]);
         ADD_COLOUR (ch, buf2, D_RED);
         i = buf2;
         break;

      case 'C':
         sprintf (buf2, "%d", ch->blood[BLOOD_POOL]);
         ADD_COLOUR (ch, buf2, D_RED);
         i = buf2;
         break;
      case 'z':   /* OLC */
         if (IS_IMMORTAL (ch))
         {
            switch (ch->desc->editor)
            {
            case ED_AEDITOR:
               sprintf (buf2, "AEdit");
               break;
            case ED_REDITOR:
               sprintf (buf2, "REdit");
               break;
            case ED_OEDITOR:
               sprintf (buf2, "OEdit");
               break;
            case ED_MEDITOR:
               sprintf (buf2, "MEdit");
               break;
            case ED_HEDITOR:
               sprintf (buf2, "HEdit");
            default:
               buf2[0] = '\0';
               break;
            }
         }
         else
            sprintf (buf2, " ");
         i = buf2;
         break;
      case '%':
         sprintf (buf2, "%%");
         i = buf2;
         break;
      }
      ++str;
      while ((*point = *i) != '\0')
         ++point, ++i;
   }
   write_to_buffer (d, buf, point - buf);
   return;
}

void do_password (CHAR_DATA * ch, char *argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char *pArg;
   char *pwdnew;
   char *p;
   char cEnd;

   if (IS_NPC (ch))
      return;

   /*
    * Can't use one_argument here because it smashes case.
    * So we just steal all its code.  Bleagh.
    */
   pArg = arg1;
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
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   pArg = arg2;
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
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   if (arg1[0] == '\0' || arg2[0] == '\0')
   {
      send_to_char ("Syntax: password <old> <new>.\n\r", ch);
      return;
   }

   if (strcmp (arg1, ch->pcdata->pwd) && strcmp (crypt (arg1, ch->pcdata->pwd), ch->pcdata->pwd))
   {
      WAIT_STATE (ch, 40);
      send_to_char ("Wrong password.  Wait 10 seconds.\n\r", ch);
      return;
   }

   if (strlen (arg2) < 5)
   {
      send_to_char ("New password must be at least five characters long.\n\r", ch);
      return;
   }

   /*
    * No tilde allowed because of player file format.
    */
   pwdnew = crypt (arg2, ch->name);
   for (p = pwdnew; *p != '\0'; p++)
   {
      if (*p == '~')
      {
         send_to_char ("New password not acceptable, try again.\n\r", ch);
         return;
      }
   }

   free_string (ch->pcdata->pwd);
   ch->pcdata->pwd = str_dup (pwdnew);
   if (!IS_EXTRA (ch, EXTRA_NEWPASS))
      SET_BIT (ch->extra, EXTRA_NEWPASS);
   do_autosave (ch, "");
   if (ch->desc != NULL && ch->desc->connected == CON_PLAYING)
      send_to_char ("Ok.\n\r", ch);
   return;
}

void do_login (CHAR_DATA * ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];

   send_to_char ("-= Login Information =-\n\r", ch);
   sprintf (buf, "Total number of login attempts: %d\n\r", total_number_login);
   send_to_char (buf, ch);
   sprintf (buf, "Highest number of logins at one time: %d\n\r", high_number_login);
   send_to_char (buf, ch);
   send_to_char ("Connection types:\n\r", ch);
   if (IS_SET (chars_logged_in, LOGIN_NEWBIE))
      send_to_char ("...Any newbies? YES.\n\r", ch);
   else
      send_to_char ("...Any newbies? NO.\n\r", ch);
   if (IS_SET (chars_logged_in, LOGIN_MORTAL))
      send_to_char ("...Any mortals? YES.\n\r", ch);
   else
      send_to_char ("...Any mortals? NO.\n\r", ch);
   if (IS_SET (chars_logged_in, LOGIN_GOD))
      send_to_char ("...Any gods? Yes.\n\r", ch);
   else
      send_to_char ("...Any gods? NO.\n\r", ch);
   if (IS_SET (chars_logged_in, LOGIN_VAMPIRE))
      send_to_char ("...Any vampires? YES.\n\r", ch);
   else
      send_to_char ("...Any vampires? NO.\n\r", ch);
   if (IS_SET (chars_logged_in, LOGIN_WEREWOLF))
      send_to_char ("...Any werewolves? YES.\n\r", ch);
   else
      send_to_char ("...Any werewolves? NO.\n\r", ch);
   return;
}

/*
 * Crash recovery system written by Mandrax, based on copyover
 * Includes call to signal() in main.
 * Mostly copied from copyover, but doesn't save chars.
 */
void crashrecov (int iSignal)
{
   CHAR_DATA *gch;
   FILE *fp;
   DESCRIPTOR_DATA *d, *d_next;
   char buf[200], buf2[100];
   int pid, iFork;
   FILE *fReturn;
   FILE *fCrash;

   /*
    * An attempt to avoid spam crashes
    */
   if ((fReturn = fopen (CRASH_TEMP_FILE, "r")) != NULL)
   {
      signal (SIGSEGV, SIG_IGN);
      raise (SIGSEGV);
      return;
   }
   fCrash = fopen (CRASH_TEMP_FILE, "w");
   fprintf (fCrash, "0");
   fclose (fCrash);

   dump_last_command ();

   /*
    * This will cause a core dump, even though the signal was handled
    */
   iFork = fork ();
   wait (NULL);
   if ((pid = getpid ()) != proc_pid)
   {
      signal (SIGSEGV, SIG_IGN);
      raise (SIGSEGV);
      return;
   }

   fp = fopen (COPYOVER_FILE, "w");
   if (!fp)
   {
      perror ("crashrecov:fopen");
      return;
   }

   for (gch = char_list; gch != NULL; gch = gch->next)
   {
      if (IS_NPC (gch))
         continue;

      /* Fix any possibly head/object forms */
      if (IS_SET (gch->extra, EXTRA_OSWITCH))
      {
         REMOVE_BIT (gch->affected_by, AFF_POLYMORPH);
         REMOVE_BIT (gch->extra, EXTRA_OSWITCH);
         gch->morph = str_dup ("");
         gch->pcdata->chobj = NULL;
         gch->pcdata->obj_vnum = 0;
         char_from_room (gch);
         char_to_room (gch, get_room_index (ROOM_VNUM_LIMBO));
      }

      /* Make sure the player is saved with all his eq */
      gch->hit = gch->max_hit;
      gch->position = POS_STANDING;
      do_call (gch, "all");
      save_char_obj (gch);
   }

   sprintf (buf, "\n\rWe have crashed, attempting to restore last savefile\n\r");

   /* For each playing descriptor, save its state */
   for (d = descriptor_list; d; d = d_next)
   {
      CHAR_DATA *och = d->character;

      d_next = d->next;   /* We delete from the list , so need to save this */
      if (!d->character || d->connected != 0)   /* drop those logging on */
      {
         write_to_descriptor (d->descriptor, "\n\rSorry, the mud has crashed.\n\rPlease log on another char and report this, your char MAY be bugged.\n\r", 0);
         close_socket2 (d);   /* throw'em out */
      }
      else
      {
         fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
         write_to_descriptor (d->descriptor, buf, 0);
      }
   }

   fprintf (fp, "-1\n");
   fclose (fp);

   /* Close reserve and other always-open files and release other resources */
   fclose (fpReserve);

   /* exec - descriptors are inherited */

   sprintf (buf, "%d", port);
   sprintf (buf2, "%d", control);


   execl (EXE_FILE, "twilight", buf, "crashrecov", buf2, (char *) NULL);

   /* Failed - sucessful exec will not return */

   perror ("crashrecov: execl");
   log_string ("Crash recovery FAILED!\n\r", CHANNEL_LOG_NORMAL);

   /* The least we can do is exit gracefully :P */
   exit (1);
}

void ch_printf (CHAR_DATA * ch, char *fmt, ...)
{
   char buf[MAX_STRING_LENGTH];
   va_list args;

   va_start (args, fmt);
   vsprintf (buf, fmt, args);
   va_end (args);

   send_to_char (buf, ch);
   return;
}

void dump_last_command ()
{
   FILE *fp;
   char buf[MAX_STRING_LENGTH];
        char *strtime;

        strtime = ctime (&current_time);
        strtime[strlen (strtime) -1] = '\0';
   fp = fopen ("crash.txt", "a");
   fprintf (fp, "Last command typed : %s at %s\n", 
      last_command, strtime);
   fflush (fp);
   fclose (fp);

   /*
    * creates a note to the immortals
    */
   sprintf (buf, "It seems we have crashed, the last command typed was\n\r\n\r");
   strcat (buf, last_command);
   strcat (buf, "\n\r\n\rPlease remember that this doesn't mean that this caused the crash.\n\r\n\rRegards,\n\r\n\rThe Crash Code");
}




/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday (struct timeval *tp, void *tzp)
{
   tp->tv_sec = time (NULL);
   tp->tv_usec = 0;
}
#endif

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )               \
            if ( !str_cmp( word, literal ) )   \
            {               \
                field  = value;         \
                fMatch = TRUE;         \
                break;            \
            }


bool check_banned( DESCRIPTOR_DATA *dnew )
{
  BAN_DATA *pban;

  for (pban = ban_list; pban != NULL; pban = pban->next)
    if (!str_suffix(pban->name, dnew->host)) return TRUE;
  return FALSE;
}

/* recycles closed descriptors */
void recycle_descriptors()
{
  DESCRIPTOR_DATA *dclose;
  DESCRIPTOR_DATA *dclose_next;
         
  for (dclose = descriptor_list; dclose; dclose = dclose_next)
  {
    dclose_next = dclose->next;
    if (dclose->lookup_status != STATUS_CLOSED) continue;
       
    /*
     * First let's get it out of the descriptor list.
     */
    if ( dclose == descriptor_list )
    {
      descriptor_list = descriptor_list->next;
    }
    else
    {
      DESCRIPTOR_DATA *d;
  
      for (d = descriptor_list; d && d->next != dclose; d = d->next)
        ;
      if (d != NULL)
        d->next = dclose->next;
      else
      {
        bug( "Recycle_descriptors: dclose not found.", 0 );
        continue;
      }
    }

    /*
     * Clear out that memory
     */
    free_string( dclose->host );
    free_mem( dclose->outbuf, dclose->outsize );

    /* 
     * Bye bye mr. Descriptor.
     */
    close( dclose->descriptor );
       
    /*
     * And then we recycle
     */
    dclose->next        = descriptor_free;
    descriptor_free     = dclose;
  }
}

/* recycles used dummy arguments */
void recycle_dummys()
{
  DUMMY_ARG *dummy;
  DUMMY_ARG *dummy_next;
   
  for (dummy = dummy_list; dummy; dummy = dummy_next)
  {
    dummy_next = dummy->next;
    if (dummy->status == 1) continue;  // being used

    if (dummy == dummy_list)
    {
      dummy_list = dummy_list->next;
    }
    else
    {
      DUMMY_ARG *prev;
     
      /* we find the prev dummy arg */
      for (prev = dummy_list; prev && prev->next != dummy; prev = prev->next)
        ;
      if (prev != NULL) 
        prev->next = dummy->next;
      else
      {
        bug( "Recycle_dymmys: dummy not found.", 0 );
        continue;
      } 
      /* recycle */
      dummy->next = dummy_free;
      dummy_free = dummy;
    }   
  }  
}

void init_char (DESCRIPTOR_DATA * d)
{
   static PC_DATA pcdata_zero;
   CHAR_DATA *ch;

   if (d == NULL)
      return;

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
   ch->name = str_dup ("guest");
   return;
}

