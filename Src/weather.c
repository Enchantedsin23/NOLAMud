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
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/  

/***************************************************************************
*       ROM 2.4 is copyright 1993-1998 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@hypercube.org)                            *
*           Gabrielle Taylor (gtaylor@hypercube.org)                       *
*           Brian Moore (zump@rom.org)                                     *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/   

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "db.h"

void    lightning        args( ( void ) );
void    ice	         args( ( CHAR_DATA *ch ) );
void    hail	         args( ( void ) );
void    blizzard         args( ( CHAR_DATA *ch ) );
void    fog              args( ( CHAR_DATA *ch ) );
void    tornado          args( ( void ) );
void    weather_update   args( ( void ) );
extern char *target_name;

/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The first grayish fingers of dawn reach over the horizon.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The brilliant face of the sun rises up over the western horizon.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun sinks steadily in the east, fading behind the city in streaks of gold and red.\n\r" );
	break;

    case 20:
	weather_info.sunlight = SUN_DARK;
	strcat( buf, "Faint stars blink to life as #edarkness#n falls, claiming the world for #enight#n.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	break;

    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 12 )
    {
	time_info.month = 0;
	time_info.year++;
    }


    switch ( weather_info.sky )
    {
        default: 
	    weather_info.sky = SKY_CLOUDLESS;
	    break;

        case SKY_CLOUDLESS:
            if ((number_chance(10)) && (time_info.hour <= 6) )
            {
	        weather_info.sky = SKY_CLOUDLESS;
            }

            else if (number_chance(15))
            {
                weather_info.sky = SKY_FOGGY;
            }   
          
            else if (number_chance(15))
            {
	        weather_info.sky = SKY_CLOUDY;
            }

            else if (number_chance(20))
            {
	        weather_info.sky = SKY_RAINING;
            }
      
            else if (number_chance(25))
            {
	        weather_info.sky = SKY_RAINING;
            }

            else if (number_chance(45))
            {
                weather_info.sky = SKY_CLOUDLESS;
            }

	    break;
	
    case SKY_CLOUDY:
            if (number_chance(15))
            {
                weather_info.sky = SKY_CLOUDY;
            }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_SNOWING;
	    }
	
            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_HAILSTORM;
	    }
	
            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_THUNDERSTORM;
	    }

	    else if (number_chance(15))
	    {
	        weather_info.sky = SKY_ICESTORM;
	    }      
            
            else if (number_chance(20))
	    {
	        weather_info.sky = SKY_CLOUDLESS;
	    }

            else if (number_chance(25))
	    {
	        weather_info.sky = SKY_CLOUDY;
	    }

            break;

    case SKY_BREEZE:
            if (number_chance(10))
            {
                weather_info.sky = SKY_BREEZE;
            }

            else if (number_chance(15))
            {
                weather_info.sky = SKY_CLOUDY;
            }
            
            else if (number_chance(15))
            {
                weather_info.sky = SKY_THUNDERSTORM;
            }

            else if (number_chance(15))
            {
                weather_info.sky = SKY_CLOUDLESS;
            }

            else if (number_chance(55))
            {
                weather_info.sky = SKY_RAINING;
            }
        
            break;

    case SKY_WIND:
            if (number_chance(10))
            {
                weather_info.sky = SKY_WIND;
            }
      
            else if (number_chance(15))
            {
                weather_info.sky = SKY_CLOUDY;
            }
            
            else if (number_chance(15))
            {
                weather_info.sky = SKY_THUNDERSTORM;
                tornado();
            }

            else if (number_chance(15))
            {
                weather_info.sky = SKY_RAINING;
            }

            else if (number_chance(20))
            {
                weather_info.sky = SKY_WIND;
                tornado();
            }

            else if (number_chance(25))
            {
                weather_info.sky = SKY_HAILSTORM;
                tornado();
	        hail();
            }
            
            else if (number_chance(55))
            {
                weather_info.sky = SKY_BLIZZARD;
            }

            break;

    case SKY_RAINING:
            if ( number_chance( 10 ) )
            {
                weather_info.sky = SKY_RAINING;
            }

            else if (number_chance(10))
            {
                weather_info.sky = SKY_CLOUDY;
            }
      
            else if (number_chance(15))
            {
                weather_info.sky = SKY_LIGHTNING;
                lightning( );
            }

            else if (number_chance(10))
            {
                weather_info.sky = SKY_THUNDERSTORM;
            }
            
            else if (number_chance(20))
            {
                weather_info.sky = SKY_HAILSTORM;
            }

            else if (number_chance(55))
            {
                weather_info.sky = SKY_RAINING;
            }
        
            break;

    case SKY_SNOWING:
       	    if (number_chance(10))
	    {
	        weather_info.sky = SKY_SNOWING;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_BLIZZARD;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_CLOUDY; 
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_RAINING;
	    }

       	    else if (number_chance(50))
	    {
	        weather_info.sky = SKY_SNOWING;
	    }

	    break;

    case SKY_LIGHTNING:
            if (number_chance(10))
	    {
	        weather_info.sky = SKY_LIGHTNING;
            }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_CLOUDY;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_THUNDERSTORM;
            }

            else if (number_chance(15))
	    {  
	        weather_info.sky = SKY_RAINING;
	    }

            else if (number_chance(40))
	    {
	        weather_info.sky = SKY_LIGHTNING;
            }

	    break;

    case SKY_FOGGY:
            if (number_chance(10))
	    {
                weather_info.sky = SKY_FOGGY;
 	        break;
	    }

            else if (number_chance(20))
	    {
                weather_info.sky = SKY_CLOUDY;
            }

            else if (number_chance(30))
            {
                weather_info.sky = SKY_RAINING;
            }

            else if (number_chance(40))
	    {
                weather_info.sky = SKY_FOGGY;
 	        break;
	    }

    case SKY_THUNDERSTORM:
            if (number_chance(10))
	    {
	        weather_info.sky = SKY_THUNDERSTORM;
            }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_CLOUDY;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_RAINING;
            }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_LIGHTNING;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_HAILSTORM;
                hail ();    
            }

            else if (number_chance(40))
	    {
	        weather_info.sky = SKY_THUNDERSTORM;
            }

  	    break;

    case SKY_HAILSTORM:
            if (number_chance(15))
	    {
	        weather_info.sky = SKY_HAILSTORM;
	        hail();
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_CLOUDY;
	    }

            else if (number_chance(30))
	    {
	        weather_info.sky = SKY_RAINING;
            }

            else if (number_chance(50))
	    {
	        weather_info.sky = SKY_HAILSTORM;
	        hail();
	    }

  	    break;

    case SKY_ICESTORM:
            if (number_chance(10))
	    {
                weather_info.sky = SKY_ICESTORM;
            }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_CLOUDY;
            }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_SNOWING;
	    }
     
            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_BLIZZARD;
            }

            else if (number_chance(55))
	    {
                weather_info.sky = SKY_ICESTORM;
            }
	
            break;

    case SKY_BLIZZARD:
            if (number_chance(10))
	    {
	        weather_info.sky = SKY_BLIZZARD;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_CLOUDY;
	    }

            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_SNOWING;
	    }
        
            else if (number_chance(15))
	    {
	        weather_info.sky = SKY_ICESTORM;
	    }

            else if (number_chance(55))
	    {
	        weather_info.sky = SKY_BLIZZARD;
	    }

            break; 
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->connected == CON_PLAYING
	    &&   IS_OUTDOORS(d->character)
	    &&   IS_AWAKE(d->character) )
		send_to_char( buf, d->character );
	}
    }

    return;
}

void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf2[MSL];
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *in_room;
    char *suf;
    int day;
    int message;

    in_room = ch->in_room;
    pArea = ch->in_room->area;

    if ( in_room == NULL || pArea == NULL )
        bug( "PArea or in_room was null for do_weather", 0 );

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";


    if ( !IS_OUTSIDE(ch) )
    {
	send_to_char( "#cYou can't see the weather indoors.#n\n\r", ch );
	return;
    }
        
    send_to_char("#gConditions in #GLos Angeles#g and the surrounding areas:#n\n\r",ch); 

    if (weather_info.sky == SKY_RAINING)
    {
        message = number_range( 0, MAX_RAIN -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        rain_table[message].description );
    }
    
    else if (weather_info.sky == SKY_CLOUDY)
    {
        message = number_range( 0, MAX_CLOUD -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r", 
        cloud_table[message].description );
    }
                 
    else if (weather_info.sky == SKY_CLOUDLESS)
    {
        message = number_range( 0, MAX_CLOUDY -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        cloudy_table[message].description );
    }
                
    else if (weather_info.sky == SKY_THUNDERSTORM)
    {
        message = number_range( 0, MAX_THUNDERSTORM -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        thunderstorm_table[message].description );
    }
                 
    else if (weather_info.sky == SKY_ICESTORM)
    {
        message = number_range( 0, MAX_ICESTORM -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        icestorm_table[message].description );
    }
                
    else if (weather_info.sky == SKY_HAILSTORM)
    {
        message = number_range( 0, MAX_HAILSTORM -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        hailstorm_table[message].description );
    }
                
    else if (weather_info.sky == SKY_SNOWING)
    {
        message = number_range( 0, MAX_SNOW -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        snow_table[message].description );
    }
      
    else if (weather_info.sky == SKY_BLIZZARD)
    {
        message = number_range( 0, MAX_BLIZZARD -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        blizzard_table[message].description );
    }
         
    else if (weather_info.sky == SKY_FOGGY)
    {
        message = number_range( 0, MAX_FOGGY -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        foggy_table[message].description );
    }

    else if (weather_info.sky == SKY_LIGHTNING)
    {
        message = number_range( 0, MAX_LIGHTNING -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        lightning_table[message].description );
    }

    else if (weather_info.sky == SKY_BREEZE)
    {
        message = number_range( 0, MAX_BREEZE -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        breeze_table[message].description );
    }

    else if (weather_info.sky == SKY_WIND)
    {
        message = number_range( 0, MAX_WIND -1 );
        printf_to_char( ch, "\n\r#y [#gWeather#y] #n%s#n\n\r",
        wind_table[message].description );
    }

    sprintf( buf2,
    "#y [#gDayTime#y] #nIt is %d o'clock %s.#n\n\r",
    (time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
    time_info.hour >= 12 ? "PM" : "AM");
    send_to_char( buf2, ch );
    return;

} 

bool IS_OUTDOORS( CHAR_DATA *ch )
{
    if(!IS_NPC(ch) && ch->desc->connected != CON_PLAYING )
       return FALSE;

    if( !IS_SET(ch->in_room->room_flags, ROOM_INDOORS) &&
       ch->in_room->sector_type != SECT_INSIDE )
       return TRUE;

    else
       return FALSE;
}


void show_weather(CHAR_DATA *ch)
{
    int message;

    if (weather_info.sky == SKY_RAINING)
    {
        message = number_range( 0, MAX_RAIN -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        rain_table[message].description );
    }
        
    else if (weather_info.sky == SKY_CLOUDY)
    {
        message = number_range( 0, MAX_CLOUD -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
                 cloud_table[message].description );
    }
            
    else if (weather_info.sky == SKY_CLOUDLESS)
    {
        message = number_range( 0, MAX_CLOUDY -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
                 cloudy_table[message].description );
    }
                 
    else if (weather_info.sky == SKY_THUNDERSTORM)
    {
        message = number_range( 0, MAX_THUNDERSTORM -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
                 thunderstorm_table[message].description );
    }
                 
    else if (weather_info.sky == SKY_ICESTORM)
    {
        message = number_range( 0, MAX_ICESTORM -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        icestorm_table[message].description );
    }
                
    else if (weather_info.sky == SKY_HAILSTORM)
    {
        message = number_range( 0, MAX_HAILSTORM -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        hailstorm_table[message].description );
    }
               
    else if (weather_info.sky == SKY_SNOWING)
    {
        message = number_range( 0, MAX_SNOW -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        snow_table[message].description );
    }
           
    else if (weather_info.sky == SKY_BLIZZARD)
    {
        message = number_range( 0, MAX_BLIZZARD -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        blizzard_table[message].description );
    }
               
    else if (weather_info.sky == SKY_FOGGY)
    {
        message = number_range( 0, MAX_FOGGY -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        foggy_table[message].description );
    }

    else if (weather_info.sky == SKY_LIGHTNING)
    {
        message = number_range( 0, MAX_LIGHTNING -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        lightning_table[message].description );
    }

    else if (weather_info.sky == SKY_BREEZE)
    {
        message = number_range( 0, MAX_BREEZE -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        breeze_table[message].description );
    }

    else if (weather_info.sky == SKY_WIND)
    {
        message = number_range( 0, MAX_WIND -1 );
        printf_to_char( ch, "#G[#g%s#G]#n\n\r",
        wind_table[message].description );
    }
      
    else 
    {
        send_to_char("#y[#gWeather#y] #GBUG!#n A description does not exist for this condition!#n\n\r",ch);
    }
}

void lightning( void )
{
  DESCRIPTOR_DATA *d;

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
       if ( d->connected == CON_PLAYING
           && IS_OUTDOORS( d->character )
           && IS_AWAKE  ( d->character )
           && number_chance(10)
           && !IS_IMMORTAL(d->character)
           && d->character->level > 17
           && weather_info.sky == SKY_LIGHTNING && d->character->fighting == NULL ) 
       {

           send_to_char("#yYou see a brilliant flash; your eyes darken and your consciousness slips away.#n\n\r",d->character);
           act( "#yA bolt of lightning streaks down from the sky, evading buildings and streetlights to strike $n squarely upon the back. They crumple to the ground unconscious.#n", d->character, NULL, NULL,TO_ROOM);
        
       } 
  }
}

void blizzard( CHAR_DATA *ch )
{
       if  ( IS_OUTDOORS( ch )
           && weather_info.sky == SKY_BLIZZARD ) 
       {

           if ( number_range( 0, 2000 ) <= 1000  )
           {
               send_to_char("#nThe sky is a blur of #wmisty white#n flurries of snow. Perhaps you should find safety indoors.#n\n\r",ch);
           }

           else if ( number_range( 0, 2000 ) <= 1250  )
           {
               send_to_char("#RYou stumble in the snow and lodge your foot in a sodden pothole.#n \n\r",ch );
               act( "$n stumbles in the snow and lodges their foot into a sodden pothole.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 15, 30 ), 0, DAM_BASH, FALSE );
           }

           else if ( number_range( 0, 2000 ) <= 1500  )
           {
               send_to_char("The snowstorm obscures near everything in your path, causing you to collide face-first with a streetlamp.\n\r",ch);
               act( "$n struggles with the snow, walking face-first into a streetlamp.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 20, 40 ), 0, DAM_BASH, FALSE );
           } 

           else if ( number_range( 0, 2000 ) <= 1950  )
           {
               send_to_char("#nThe sky is a blur of #wmisty white#n flurries of snow. Perhaps you should find safety indoors.#n\n\r",ch);
           }

           else if ( number_range( 0, 2000 ) <= 2000  )
           {   
               send_to_char("Your limbs and extremities are eerily cold and non-responsive.#n \n\r", ch);
               act( "You watch as $n starts to slow and drag, their extremities blue and lips colorless.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 25, 50 ), 0, DAM_COLD, FALSE );
           }
       } 
}

void ice( CHAR_DATA *ch )
{

       if  ( IS_OUTDOORS( ch )
           && weather_info.sky == SKY_ICESTORM )
       {
           if ( number_range( 0, 2000 ) <= 1000  )
           {
               send_to_char("#nThe sky is streaked with vicious, chilling sheets of #Cicy rain#n.#n\n\r", ch);
           }
        
           else if ( number_range( 0, 2000 ) <= 1250  )
           {
               send_to_char("#RYou lose your footing over the slick ice and fall, hard, onto the ground.#n\n\r",ch );
               act( "$n slips on the ice and falls hard onto the ground.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 10, 20 ), 0, DAM_COLD, FALSE );
           }

           else if ( number_range( 0, 2000 ) <= 1500  )
           {
               send_to_char("#RYou loose traction on the ice and begin to slide, slamming face-first into a streetlight.#n\n\r",ch);
               act( "$n slips on the ice and slides right into a streetlight.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 15, 30 ), 0, DAM_BASH, FALSE );
           }

           else if ( number_range( 0, 2000 ) <= 1950  )
           {
               send_to_char("#nThe sky is streaked with vicious, chilling sheets of #Cicy rain#n.#n\n\r",ch);
           }

           else if ( number_range( 0, 2000 ) <= 2000  )
           {
               send_to_char("#RYour extremities are losing feeling; the veins look strange and dark.#n \n\r", ch);
               act( "You watch as $n's extremities start to go blue.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 20, 40 ), 0, DAM_COLD, FALSE );
           }
       }  
}

void hail( void )
{
  DESCRIPTOR_DATA *d;

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
       if ( d->connected == CON_PLAYING
          && IS_OUTDOORS( d->character )
          && weather_info.sky == SKY_HAILSTORM && d->character->fighting == NULL )
       {

          if ( number_range( 0, 2000 ) <= 1000  )
          {
              send_to_char("Was that a golf ball or hail? Either way, it left a goose egg on your head.\n\r",d->character);
          }
          
          else if ( number_range( 0, 2000 ) <= 1250  )
          {
              send_to_char("A huge chunk of hail hurtles down from the clouds and strikes you square in the head.\n\r",d->character );
              act( "A huge chunk of hail hurtles down from the clouds and strikes $n in the head.", d->character, NULL, NULL,TO_ROOM);
            //  damage( d->character, d->character, number_range( 15, 30 ), 0, DAM_COLD, FALSE );
              
          }
           
          else if ( number_range( 0, 2000 ) <= 1950  )
          {
              send_to_char("Spheres of icy hail are shaken down from the skies, rock hard in varying sizes.\n\r",d->character);
          }

          else if ( number_range( 0, 2000 ) <= 2000  )
          {
              send_to_char("#RA small pellet of hail hurtles from the clouds and strikes you in the eye!#n \n\r", d->character);
              act( "A small pellet of hail hurtles from the clouds and strikes $n in the eye!", d->character, NULL, NULL,TO_ROOM);
            //  damage( d->character, d->character, number_range( 20, 40 ), 0, DAM_COLD, FALSE );
          }
       }      
  }
}
 
void fog( CHAR_DATA *ch )
{
       if ( IS_OUTDOORS( ch )
          && weather_info.sky == SKY_FOGGY )
       {
 
           if ( number_range( 0, 2000 ) <= 1000  )
           {
               send_to_char("The morning #cfog#n is as thick as pea soup, making it very hard to see.#n \n\r",ch);
           }
          
           else if ( number_range( 0, 2000 ) <= 1250  )
           {
               send_to_char("#RThe fog veils your vision so badly you miss an open door and run right into it.#n \n\r",ch );
               act( "$n runs face-first into an open door.", ch, NULL, NULL,TO_ROOM);
           //    damage( ch, ch, number_range( 5, 25 ), 0, DAM_BASH, FALSE ); 
           }
         
           else if ( number_range( 0, 2000 ) <= 1500  )
           {
               send_to_char("#RMomentarily confused by the fog, you end up a quarter block away from your intended destination.#n \n\r",ch);
               act( "$n walks to the end of the street and turns back, confused by the fog.", ch, NULL, NULL,TO_ROOM);
            //   damage( ch, ch, number_range( 10, 30 ), 0, DAM_BASH, FALSE );
           }

           else if ( number_range( 0, 2000 ) <= 1950  )
           {
              send_to_char("The morning #cfog #nis as thick as pea soup, making it very hard to see.#n \n\r",ch);
           }

           else if ( number_range( 0, 2000 ) <= 2000  )
           {

              send_to_char("#RA cab driver doesn't see you through the fog; the taxi hits you at a slow speed.\n\r", ch);
              act( "$n is hit by a passing taxi at a moderately slow speed.", ch, NULL, NULL,TO_ROOM);
           //   damage( ch, ch, number_range( 20, 40 ), 0, DAM_BASH, FALSE );
           }
       }  
}

 
void tornado( void )
{
  DESCRIPTOR_DATA *d;
  ROOM_INDEX_DATA *pRoomIndex;

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
       if ( d->connected == CON_PLAYING
          && IS_OUTDOORS( d->character )
          && number_chance( 10 )
          && d->character->level >= 25 
          && weather_info.sky == SKY_WIND && d->character->fighting == NULL )
       {

          if ( number_range( 0, 2000 ) <= 1000  )
          {
              send_to_char("A large #Ctornado#n forms in the distance, the funnel slowly stretches towards the ground, and starts creeping in your direction.\n\r",d->character);
          }
          
          else if ( number_range( 0, 2000 ) <= 1250  )
          {
              send_to_char("Debris is kicked up by the#C tornado#n; a chunk of cobblestone strikes your back.\n\r",d->character );
              act( "Debris is kicked up by the #Ctornado#n; a chunk of cobblestone strikes $n in the back.", d->character, NULL, NULL,TO_ROOM);
            //  damage( d->character, d->character, number_range( 40, 80 ), 0, DAM_WIND, FALSE );
          }
           
          else if ( number_range( 0, 2000 ) <= 1950  )
          {
              send_to_char("A large funnel cloud is forming on the horizon, the possibility of a #Ctornado#n is highly likely. You should find your way indoors.\n\r",d->character);
          }

          else if ( number_range( 0, 2000 ) <= 2000  )
          {
              send_to_char("#cThe clouds above you twist, turn and thunder into life as a large tornado is formed. The tornado touches down and begins sucking up everything in its path. There is nothing you can do but close your eyes as you are sucked up into the funnel.#n\n\r", d->character);
              act( "#cThe clouds twist, turn and thunder into life as a large tornado is formed. $n closes $m eyes up tight as the tornado sucks them up into the funnel. It is time for you to RUN.#n", d->character, NULL, NULL,TO_ROOM);
            //  damage( d->character, d->character, number_range( 250, 500 ), 0, DAM_WIND, FALSE );
              char_from_room( d->character );
              char_to_room( d->character, pRoomIndex );  
              do_function( d->character, &do_look, "auto" );
          }
       }   
  }
}

bool number_chance(int num)
{
    if (number_range(1,100) <= num) return TRUE;
    else return FALSE;
}

void spell_control_weather(int sn,int level,CHAR_DATA *ch,void *vo,int target)
{
    if ( !str_cmp( target_name, "better" ) )
    {

        if (weather_info.sky == SKY_CLOUDLESS)
        {
            send_to_char("But it's so beautiful outside already\n\r",ch);
            return;
        }

        else if (weather_info.sky == SKY_CLOUDY)
        {
            send_to_char("You recite the ancient spell and the clouds part in obedience.\n\r",ch);
            act( "$n makes a strange movement with their hands and the clouds part.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_CLOUDLESS;
            return;
        }

        else if (weather_info.sky == SKY_RAINING)
        {
            send_to_char("You recite the ancient spell and the  rain stops in obedience.\n\r",ch);
            act( "$n makes a strange movement with their hands and the rain stops.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_CLOUDY;
            return;
        }
        
        else if (weather_info.sky == SKY_LIGHTNING)
        {
            send_to_char("You recite the ancient spell and the lightning ceases in obedience.\n\r",ch);
            act( "$n makes a strange movement with their hands and the lightning stops.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_THUNDERSTORM;
            return;
        }
         
        else if (weather_info.sky == SKY_THUNDERSTORM)
        {
            send_to_char("You recite the ancient spell and the storm ceases.\n\r",ch);
            act( "$n makes a strange movement with their hands and the  storm ceases.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_RAINING;
            return;
        }
       
        else if (weather_info.sky == SKY_SNOWING)
        {
            send_to_char("You recite the ancient spell and the snow ceases in obedience.\n\r",ch);
            act( "$n makes a strange movement with their hands and the snow ceases.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_CLOUDY;
            return;
        }
 
        else if (weather_info.sky == SKY_BLIZZARD)
        {
            send_to_char("You recite the ancient spell and the  horizon clears.\n\r",ch);
            act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_SNOWING;
            return;
        }

        else if (weather_info.sky == SKY_ICESTORM)
        {
            send_to_char("You recite the ancient spell and the  horizon clears.\n\r",ch);
            act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_RAINING;
            return;
        }

        else if (weather_info.sky == SKY_HAILSTORM)
        {
            send_to_char("You recite the ancient spell and the  horizon clears.\n\r",ch);
            act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_RAINING;
            return;
        }

        else if (weather_info.sky == SKY_FOGGY)
        {
            send_to_char("You recite the ancient spell and the  horizon clears.\n\r",ch);
            act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_CLOUDY;
            return;
        }

        else 
        {
            send_to_char("Bad Weather Call. Please notify the imms.\n\r",ch);
        }

    }

    else if ( !str_cmp( target_name, "worse" ) )
    {
        if (weather_info.sky == SKY_CLOUDLESS)
        {
            send_to_char("You recite the ancient spell and the clouds come at your command.\n\r",ch);
            act( "$n makes a strange movement with their hands and the clouds darken the sky.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_CLOUDY;
            return;
        }
          
        else if (weather_info.sky == SKY_CLOUDY)
        {
            send_to_char("You recite the ancient spell and the clouds trickle down rain.\n\r",ch);
            act( "$n makes a strange movement with their hands and the clouds open up to rain.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_RAINING;
            return; 
        }

        else if (weather_info.sky == SKY_RAINING)
        {
            send_to_char("You recite the ancient spell and the  rain  turns to hail.\n\r",ch);
            act( "$n makes a strange movement with their hands and the rain turns to hail.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_HAILSTORM;
            return;
        }
        
        else if (weather_info.sky == SKY_LIGHTNING)
        {
            send_to_char("You recite the ancient spell and the  clouds send down sheets of ice\n\r",ch);
            act( "$n makes a strange movement with their hands and the lightning turns to ice.", ch, NULL, NULL, TO_ROOM);
            weather_info.sky = SKY_ICESTORM;
            return;
        }

        else if (weather_info.sky == SKY_THUNDERSTORM)
        {
           send_to_char("You recite the ancient spell and the clouds clap in thunderous approval.\n\r",ch);
           act( "$n makes a strange movement with their hands and the  clouds clap in thunder.", ch, NULL, NULL, TO_ROOM);
           weather_info.sky = SKY_LIGHTNING;
           return;
        }

        else if (weather_info.sky == SKY_SNOWING)
        {
           send_to_char("You recite the ancient spell and the snow increases in obedience.\n\r",ch);
           act( "$n makes a strange movement with their hands and the snow turns to a blizzard.", ch, NULL, NULL, TO_ROOM);
           weather_info.sky = SKY_BLIZZARD; 
           return;
        }

        else if (weather_info.sky == SKY_BLIZZARD)
        {
           send_to_char("It's already as bad as it can get\n\r",ch);
           return;
        }

        else if (weather_info.sky == SKY_ICESTORM)
        {
           send_to_char("It's already as bad as it can get\n\r",ch);
           return;
        }

        else if (weather_info.sky == SKY_HAILSTORM)
        {
           send_to_char("It's already as bad as it can get\n\r",ch);
           return;
        }
      
        else if (weather_info.sky == SKY_FOGGY)
        {
           send_to_char("You recite the ancient spell and the  horizon clears.\n\r",ch);
           act( "$n makes a strange movement with their hands and the horizon clears.", ch, NULL, NULL, TO_ROOM);
           weather_info.sky = SKY_CLOUDY;
           return;
        }

        else 
        {
           send_to_char("Bad Weather Call. Please notify the imms.\n\r",ch);
        }

    }

    else
        send_to_char ("Do you want it to get better or worse?\n\r", ch );

    send_to_char( "Ok.\n\r", ch );
    return;
}

