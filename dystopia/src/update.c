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
#include <time.h>
#include <unistd.h> /* unlink */
#include "merc.h"



/*
 * Local functions.
 */
void	mobile_update	args( ( void ) );
void	weather_update	args( ( void ) );
void    room_update     args( ( void ) );
void	char_update	args( ( void ) );
void	obj_update	args( ( void ) );
void    arti_update     args( ( void ) );
void    ww_update       args( ( void ) );
void    embrace_update  args((void));
void	werewolf_regen	args ( ( CHAR_DATA *ch, int multiplier) );

void update_active_counters                     args ((CHAR_DATA *ch));
void update_morted_timer			args ((CHAR_DATA *ch));
void update_sit_safe_counter			args ((CHAR_DATA *ch));
void update_drunks				args ((CHAR_DATA *ch));
void sex_update					args ((CHAR_DATA *ch));
void update_vampire				args ((CHAR_DATA *ch));
void update_vampire_regen			args ((CHAR_DATA *ch));
void update_cyborg 				args ((CHAR_DATA *ch));
void update_monk				args ((CHAR_DATA *ch));
void update_drider				args ((CHAR_DATA *ch));
void update_lich				args ((CHAR_DATA *ch));
void update_hobbit				args ((CHAR_DATA *ch));
void update_lich_regen                          args ((CHAR_DATA *ch));
void update_angel                               args ((CHAR_DATA *ch));
void update_ninja				args ((CHAR_DATA *ch));
void update_fae					args ((CHAR_DATA *ch));
void update_werewolf				args ((CHAR_DATA *ch));
void update_demon_regen				args ((CHAR_DATA *ch));
void update_demon				args ((CHAR_DATA *ch));
void update_tanarri				args ((CHAR_DATA *ch));
void update_drow				args ((CHAR_DATA *ch));
void update_highlander				args ((CHAR_DATA *ch));
void update_mage				args ((CHAR_DATA *ch));
void update_knight				args ((CHAR_DATA *ch));
void update_shapeshifter			args ((CHAR_DATA *ch));
void update_arti_regen				args ((CHAR_DATA *ch));
void regen_limb					args ((CHAR_DATA *ch));
void update_safe_powers				args ((CHAR_DATA *ch));

void gain_exp( CHAR_DATA *ch, int gain )
{
    CHAR_DATA *mount = NULL;
    CHAR_DATA *master = NULL;
    if ( IS_NPC(ch) && (mount = ch->mount) != NULL && !IS_NPC(mount))
    {
	if ( (master = ch->master) == NULL || master != mount )
	    mount->exp += gain;
    }
    if ( !IS_NPC(ch) )
	ch->exp += gain;
    return;
}

void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
  int condition;

  if ( value == 0 || IS_NPC(ch) ) return;

    if (!IS_NPC(ch) && IS_HERO(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) 
	&& iCond != COND_DRUNK )
	return;

    condition				= ch->pcdata->condition[iCond];
    if (!IS_NPC(ch) && !IS_CLASS(ch, CLASS_VAMPIRE) )
	{
			ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 48 );
	}
	else
	ch->pcdata->condition[iCond]	= URANGE( 0, condition + value, 20000/ch->generation );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE))
	    {
		send_to_char( "You are REALLY hungry.\n\r",  ch );
		act( "You hear $n's stomach rumbling.", ch, NULL, NULL, TO_ROOM );
	    }
	    break;

	case COND_THIRST:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
		send_to_char( "You are REALLY thirsty.\n\r", ch );
	    else if (ch->hit > 0)
	    {
		send_to_char( "You are DYING from lack of blood!\n\r", ch );
		act( "$n gets a hungry look in $s eyes.", ch, NULL, NULL, TO_ROOM );
		ch->hit = ch->hit - number_range(2,5);
		if (number_percent() <= ch->beast && ch->beast > 0) vamp_rage(ch);
		if (!IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
	    break;

	case COND_DRUNK:
	    if ( condition != 0 )
		send_to_char( "You are sober.\n\r", ch );
	    break;
	}
    }
    else if ( ch->pcdata->condition[iCond] < 10 )
    {
	switch ( iCond )
	{
	case COND_FULL:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
		send_to_char( "You feel hungry.\n\r",  ch );
	    break;

	case COND_THIRST:
	    if (!IS_CLASS(ch, CLASS_VAMPIRE)) 
		send_to_char( "You feel thirsty.\n\r", ch );
	    else
	    {
		send_to_char( "You crave blood.\n\r", ch );
		if (number_range(1,1000) <= ch->beast && ch->beast > 0) vamp_rage(ch);
		if (number_percent() > (ch->pcdata->condition[COND_THIRST]+75)
		    && !IS_VAMPAFF(ch, VAM_FANGS)) do_fangs(ch,"");
	    }
	    break;
	}
    }

    return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to % of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  EXIT_DATA *pexit;
  int door;

  /* Examine all mobs. */
  for ( ch = char_list; ch != NULL; ch = ch_next )
  {
    ch_next = ch->next;

    if ( ch->in_room == NULL ) continue;

    if (!IS_NPC(ch) && ch->hunting != NULL && ch->hunting != '\0' && strlen(ch->hunting) > 1 )
    {
      check_hunt( ch );
      continue;
    }
    if ( !IS_NPC(ch) )
    {
      update_morted_timer(ch);
      update_sit_safe_counter(ch);
      update_drunks(ch);
      sex_update(ch);
      if (ch->position != POS_FIGHTING && IS_SET(ch->pcdata->jflags, JFLAG_DRONEWORLD))
      {
        send_to_char("Reality returns to normal.\n\r", ch);
        REMOVE_BIT(ch->pcdata->jflags, JFLAG_DRONEWORLD);
      }
      if (ch->level < 7 && IS_HERO(ch)) update_safe_powers(ch);
      if (IS_HERO(ch) && ch->hit > 0 && !IS_SET(ch->extra, EXTRA_AFK))
      {
        if (IS_CLASS(ch, CLASS_VAMPIRE)) 
        {
          if (ch->rage >0) update_vampire_regen(ch);
          else if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
          update_vampire(ch);
        }
        if (IS_CLASS(ch, CLASS_DROID))
        {
          update_drider(ch);
          if (IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_REGROWTH)) update_cyborg(ch);
          else if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (IS_CLASS(ch, CLASS_ANGEL))
        {
          if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
          update_angel(ch);
        }
        if (IS_CLASS(ch, CLASS_TANARRI))
        {
          if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
          update_tanarri(ch);
        }
        if (IS_CLASS(ch, CLASS_LICH))
        {
          if (ch->pcdata->powers[LIFE_LORE] > 0) update_lich_regen(ch);
          else if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
          update_lich(ch);
        }
        if (IS_CLASS(ch, CLASS_HOBBIT))
        {
          if (ch->pcdata->powers[HOBBIT_DISHES] > 15) update_hobbit(ch);
          else if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (IS_CLASS(ch, CLASS_GIANT))
        {
          if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (IS_CLASS(ch, CLASS_DRONE))
        {
          if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (IS_CLASS(ch, CLASS_FAE)) update_fae(ch);
        if (IS_CLASS(ch, CLASS_MONK)) update_monk(ch);
        if (IS_CLASS(ch, CLASS_NINJA)) update_ninja(ch);
        if (IS_CLASS(ch, CLASS_UNDEAD_KNIGHT)) update_knight(ch);
        if (IS_CLASS(ch, CLASS_WEREWOLF)) update_werewolf(ch);
        if (IS_CLASS( ch, CLASS_DEMON))
        {
          update_demon(ch);
          if (IS_SET(ch->warp, WARP_REGENERATE)) update_demon_regen(ch);
          else if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (IS_CLASS(ch, CLASS_SHAPESHIFTER)) update_shapeshifter(ch);
        if (IS_CLASS(ch, CLASS_DROW))
        {
          update_drow(ch);
          if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (IS_CLASS(ch, CLASS_SAMURAI)) update_highlander(ch);
        if (IS_CLASS(ch, CLASS_MAGE))
        {
          update_mage(ch);
          if (IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
        }
        if (ch->class == 0 && IS_ITEMAFF(ch, ITEMA_REGENERATE)) update_arti_regen(ch);
      }
      else 
      {
        ch->hit += number_range(1, 5);
        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        update_pos(ch);
        if (IS_ITEMAFF(ch, ITEMA_REGENERATE) && ch->hit > 0)
        update_arti_regen(ch);
      }
    }
    else // This is for the mobs
    {
      if ( IS_AFFECTED(ch, AFF_CHARM) ) continue;
      if ( ch->spec_fun != 0 )
      {
        if ( (*ch->spec_fun) ( ch ) ) continue;
	if (ch == NULL) continue;
      }
      if ( ch->position != POS_STANDING ) {do_stand(ch, "");continue;}
      if ( IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room->contents != NULL && number_bits( 2 ) == 0 )
      {
        OBJ_DATA *obj;
        OBJ_DATA *obj_best = 0;
        int max=1;
        for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
        {
          if (!can_see_obj(ch, obj)) continue;
          if (IS_SET(obj->quest, QUEST_ARTIFACT)) continue;
          if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max )
          {
            obj_best    = obj;
            max         = obj->cost;
          }
        }
        if ( obj_best )
        {
          obj_from_room( obj_best );
          obj_to_char( obj_best, ch );
          act( "$n picks $p up.", ch, obj_best, NULL, TO_ROOM );
          act( "You pick $p up.", ch, obj_best, NULL, TO_CHAR );
        }
      }
      if ( !IS_SET(ch->act, ACT_SENTINEL) && ( door = number_bits( 5 ) ) <= 5 && ( pexit = ch->in_room->exit[door] ) != NULL
          && pexit->to_room != NULL && !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
          && ( ch->hunting == NULL || strlen(ch->hunting) < 2 ) && ( (!IS_SET(ch->act, ACT_STAY_AREA) && ch->level < 900)
          || pexit->to_room->area == ch->in_room->area ) )
      {
        move_char( ch, door );
      }
      if ( ch->hit < ch->max_hit / 2 && ( door = number_bits( 3 ) ) <= 5 && ( pexit = ch->in_room->exit[door] ) != NULL
          && pexit->to_room != NULL && !IS_AFFECTED(ch, AFF_WEBBED) && ch->level < 900 && !IS_SET(pexit->exit_info, EX_CLOSED)
          && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
      {
        CHAR_DATA *rch;	    
        bool found;
        found = FALSE;
        for ( rch  = pexit->to_room->people;
        rch != NULL;
        rch  = rch->next_in_room )
        {
          if ( !IS_NPC(rch) )
          {
            found = TRUE;
            break;
          }
        }	    
        if ( !found )
        move_char( ch, door );
        }
      }
    }
  return;
}


/*
 * Update the weather.
 */
void weather_update( void )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch = NULL;
  int diff;
  bool char_up;
  bool thunder = FALSE;

  buf[0] = '\0';

  switch ( ++time_info.hour )
  {
    case  5:
	weather_info.sunlight = SUN_LIGHT;
	strcat( buf, "The day has begun.\n\r" );
	break;

    case  6:
	weather_info.sunlight = SUN_RISE;
	strcat( buf, "The sun rises in the east.\n\r" );
	break;

    case 19:
	weather_info.sunlight = SUN_SET;
	strcat( buf, "The sun slowly disappears in the west.\n\r" );
	break;

    case 20:
        weather_info.sunlight = SUN_DARK;
	strcat( buf, "The night has begun.\n\r" );
	break;

    case 24:
	time_info.hour = 0;
	time_info.day++;
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    char_up = FALSE;
	    if (( d->connected == CON_PLAYING
	    ||    d->connected == CON_EDITING )
	    &&   (ch = d->character) != NULL
	    &&  !IS_NPC(ch))
	    {
		if (ch->fighting == NULL && !IS_SET(ch->newbits, NEW_NATURAL)
		 && ch->monkab[SPIRIT] >= 2 )
		  SET_BIT(ch->newbits, NEW_NATURAL);

		send_to_char( "You hear a clock in the distance strike midnight.\n\r",ch);
		if (IS_EXTRA(ch, EXTRA_ROT))
                {
                        send_to_char("Your flesh feels better.\n\r", ch);
                        REMOVE_BIT(ch->extra, EXTRA_ROT);
                        
                }
                if (ch->pcdata->followers > 0) ch->pcdata->followers=0;
                if (IS_SET(ch->in_room->room_flags, ROOM_SILENCE))
                {
                send_to_char("The silence leaves the room.\n\r",ch);
                act("The silence leaves the room.",ch,NULL,NULL,TO_ROOM);
                REMOVE_BIT(ch->in_room->room_flags,ROOM_SILENCE);
                
                }

                if (IS_SET(ch->in_room->room_flags,ROOM_FLAMING))
                {
                send_to_char("The flames in the room die down.\n\r",ch);
                act("The flames in the room die down.",ch,NULL,NULL,TO_ROOM);
                REMOVE_BIT(ch->in_room->room_flags,ROOM_FLAMING);
                }
                if ( IS_CLASS(ch, CLASS_VAMPIRE) )
		{
		    if (ch->hit < ch->max_hit)
			{ ch->hit = ch->max_hit; char_up = TRUE; }
		    if (ch->mana < ch->max_mana)
			{ ch->mana = ch->max_mana; char_up = TRUE; }
		    if (ch->move < ch->max_move)
			{ ch->move = ch->max_move; char_up = TRUE; }
		    if (char_up) send_to_char( "You feel the strength of the kindred flow through your veins!\n\r", ch );

                    if (IS_SET(ch->newbits, NEW_TIDE))
 	            {
 		     REMOVE_BIT(ch->newbits, NEW_TIDE);
                     send_to_char("The tide of vitae leaves you.\n\r", ch);
		    }

		    if (IS_SET(ch->extra, EXTRA_BAAL))
                    {
                       send_to_char("The spirit of Baal escapes you.\n\r",ch);  
                       ch->power[DISC_VAMP_POTE] -= 2;
                       ch->power[DISC_VAMP_CELE] -= 2;
                       ch->power[DISC_VAMP_FORT] -= 2;
                       REMOVE_BIT(ch->extra, EXTRA_BAAL);
                    }   

                    if (IS_EXTRA(ch, EXTRA_FLASH))
                    {
                       send_to_char("Your speed slows.\n\r",ch);  
                       ch->power[DISC_VAMP_CELE] -= 2;
                       REMOVE_BIT(ch->extra, EXTRA_FLASH);
                    }   

                   }			
		  } 
                 
	    
	}
	break;
    }

    if ( time_info.day   >= 35 )
    {
	time_info.day = 0;
	time_info.month++;
    }

    if ( time_info.month >= 17 )
    {
	time_info.month = 0;
	time_info.year++;
    }

    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
	diff = weather_info.mmhg >  985 ? -2 : 2;
    else
	diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default: 
	bug( "Weather_update: bad sky %d.", weather_info.sky );
	weather_info.sky = SKY_CLOUDLESS;
	break;

    case SKY_CLOUDLESS:
	if ( weather_info.mmhg <  990
	|| ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The sky is getting cloudy.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_CLOUDY:
	if ( weather_info.mmhg <  970
	|| ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "It starts to rain.\n\r" );
	    weather_info.sky = SKY_RAINING;
	}

	if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "The clouds disappear.\n\r" );
	    weather_info.sky = SKY_CLOUDLESS;
	}
	break;

    case SKY_RAINING:
	if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
	{
	    strcat( buf, "Lightning flashes in the sky.\n\r" );
            thunder = TRUE;
	    weather_info.sky = SKY_LIGHTNING;
	}

	if ( weather_info.mmhg > 1030
	|| ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The rain stopped.\n\r" );
	    weather_info.sky = SKY_CLOUDY;
	}
	break;

    case SKY_LIGHTNING:
	if ( weather_info.mmhg > 1010
	|| ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
	{
	    strcat( buf, "The lightning has stopped.\n\r" );
	    weather_info.sky = SKY_RAINING;
	    break;
	}
	break;
    }

    if ( buf[0] != '\0' )
    {
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if (( d->connected == CON_PLAYING
	    ||    d->connected == CON_EDITING )
	    &&   IS_OUTSIDE(d->character)
	    &&   IS_AWAKE(d->character) )
            {
                if (thunder) send_thunder(d->character);
		send_to_char( buf, d->character );
            }
	}
    }

    return;
}


int is_wall(EXIT_DATA *ex)
{

    if (IS_SET(ex->exit_info, EX_ICE_WALL)) return 1;
    if (IS_SET(ex->exit_info, EX_FIRE_WALL)) return 2;
    if (IS_SET(ex->exit_info, EX_SWORD_WALL)) return 3;
    if (IS_SET(ex->exit_info, EX_PRISMATIC_WALL)) return 4;
    if (IS_SET(ex->exit_info, EX_IRON_WALL)) return 5;
    if (IS_SET(ex->exit_info, EX_MUSHROOM_WALL)) return 6;
    if (IS_SET(ex->exit_info, EX_CALTROP_WALL)) return 7;
    if (IS_SET(ex->exit_info, EX_ASH_WALL)) return 8;
    return 0;
}

const char * wall[MAX_WALL+1] = 
{
    "","wall of ice", "wall of fire", "wall of swords",
	"prismatic wall", "wall or iron", "wall of mushrooms",
	"wall of caltrops", "wall of ash"
};

/*
 * Update all rooooooms, like gore, poison clouds etc....
 */
void room_update( void )
{
    int i;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *room;

    
    for ( room = room_list; room != NULL; room = room->next_room)
    {
        
        if (RTIMER(room, RTIMER_WALL_NORTH) == 1
	    && room->exit[DIR_NORTH] != NULL
	    && is_wall(room->exit[DIR_NORTH])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_NORTH])]);
	    room_message(room, buf);
	    make_wall(room, DIR_NORTH, 0);
        }

        if (RTIMER(room, RTIMER_WALL_SOUTH) == 1
	    && room->exit[DIR_SOUTH] != NULL
	    && is_wall(room->exit[DIR_SOUTH])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_SOUTH])]);
	    room_message(room, buf);
	    make_wall(room, DIR_SOUTH, 0);
        }

        if (RTIMER(room, RTIMER_WALL_EAST) == 1
	    && room->exit[DIR_EAST] != NULL
	    && is_wall(room->exit[DIR_EAST])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_EAST])]);
	    room_message(room, buf);
	    make_wall(room, DIR_EAST, 0);
        }

        if (RTIMER(room, RTIMER_WALL_WEST) == 1
	    && room->exit[DIR_WEST] != NULL
	    && is_wall(room->exit[DIR_WEST])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_WEST])]);
	    room_message(room, buf);
	    make_wall(room, DIR_WEST, 0);
        }

        if (RTIMER(room, RTIMER_WALL_UP) == 1
	    && room->exit[DIR_UP] != NULL
	    && is_wall(room->exit[DIR_UP])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_UP])]);
	    room_message(room, buf);
	    make_wall(room, DIR_UP, 0);
        }

        if (RTIMER(room, RTIMER_WALL_DOWN) == 1
	    && room->exit[DIR_DOWN] != NULL
	    && is_wall(room->exit[DIR_DOWN])!= 0)
        {
	    sprintf(buf, "The %s slowly fades away.", wall[is_wall(room->exit[DIR_DOWN])]);
	    room_message(room, buf);
	    make_wall(room, DIR_DOWN, 0);
        }


	if (RTIMER(room, RTIMER_STINKING_CLOUD) == 1)
	    room_message(room, "The poisonous vapours dissipate and clear.");

	if (RTIMER(room, RTIMER_HIDE_ROOM) == 1)
	    room_message(room, "The shroud leaves the room.");

	if (RTIMER(room, RTIMER_GHOST_LIGHT) == 2)
	    room_message(room, "The vapourous ghosts start howling insanely.");

	if (RTIMER(room, RTIMER_GHOST_LIGHT) == 3)
	    room_message(room, "The vapourous ghosts start moaning.");

	if (RTIMER(room, RTIMER_GHOST_LIGHT) == 1)
	    room_message(room, "The vapourous ghosts dissipate and vanish.");

	if (RTIMER(room, RTIMER_GLYPH_PROTECTION) == 1)
	    room_message(room, "The glyph of protection flares and vanishes.");

	if (RTIMER(room, RTIMER_SWARM_BEES) == 1)
	    room_message(room, "The bees fly away into the sky.");

	if (RTIMER(room, RTIMER_DISCORD) == 1)
	    room_message(room, "The banging and crashing stops.");

	if (RTIMER(room, RTIMER_SWARM_BATS) == 1)
	    room_message(room, "The bats flap away into the night.");

	if (RTIMER(room, RTIMER_SWARM_RATS) == 1)
	    room_message(room, "The rats scurry away into the floorboards.");
  
      if( RTIMER(room, RTIMER_SILENCE) < 0) room->tick_timer[i] = UMAX(room->tick_timer[i], 0);
      if (RTIMER(room, RTIMER_SILENCE) == 1)
	    room_message(room, "The silence disappates.");

	for (i = 0 ; i < MAX_RTIMER ; i++)
	    room->tick_timer[i] = UMAX(room->tick_timer[i] - 1, 0);

    }

    return;

}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update( void )
{
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;
  QUEST_DATA *quest;
  QUEST_DATA *quest_next;
  CHAR_DATA *ch_quit = NULL;
  CHAR_DATA *ch_save = NULL;
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  char buf[MAX_STRING_LENGTH];
  bool is_obj;
  time_t save_time;
  int count = 0, i;

  save_time = current_time;

  for ( ch = char_list; ch != NULL; ch = ch_next )
  {
    ch_next = ch->next;
    count++;

    /*
     * Is the player an object ?
     */
    if (!IS_NPC(ch) && (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH))) is_obj = TRUE;
    else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
    {
      is_obj = TRUE;
      SET_BIT(ch->extra, EXTRA_OSWITCH);
    }
    else is_obj = FALSE;

    if (IS_NPC(ch))
    {
      if (ch->decay_pulse > 0)
      {
        if (--ch->decay_pulse <= 0)
        {
          act("$n vanishes in a puff of smoke.", ch, NULL, NULL, TO_ROOM);
          extract_char(ch, TRUE);
          continue;
        }
      }
    }

    /*
     * Tick Timers and other PC only stuff
     */
    if (!IS_NPC(ch))
    {
      if (ch->tick_timer[TIMER_CHALLENGE] == 1)
        send_to_char("You can use challenge again.\n\r", ch);
      if (ch->tick_timer[TIMER_DRONE_DISTORT] == 1)
        send_to_char("You can use distort again.\n\r", ch);
      if (ch->tick_timer[TIMER_GFURY] == 1)
        send_to_char("You can invoke your godly fury again.\n\r", ch);
      if (ch->tick_timer[TIMER_DEJAVU] == 1)
        send_to_char("You can invoke deja-vu again.\n\r", ch);
      if (ch->tick_timer[TIMER_MADNESS] == 1)
        send_to_char("You can induce madness again.\n\r", ch);
      if (ch->tick_timer[TIMER_PESTILENCE] == 1)
        send_to_char("You can lay the Pestilence enchantment again.\n\r", ch);
      if (ch->tick_timer[TIMER_HDESIRE] == 1)
        send_to_char("You can invoke the hearts desires again.\n\r", ch);
      if (ch->tick_timer[TIMER_CALLGOLEMS] == 1)
        send_to_char("You can call on your golems again.\n\r", ch);
      if (ch->tick_timer[TIMER_DJINN] == 1)
        send_to_char("You can call upon the djinn of the seven ages again.\n\r", ch);
      if (ch->tick_timer[TIMER_SUPERNOVA] == 1)
        send_to_char("You can create another supernova.\n\r", ch);
      if (ch->tick_timer[TIMER_HUNTER] == 1)
        send_to_char("You can call upon the dark hunter again.\n\r", ch);
      if (ch->tick_timer[TIMER_GENIE] == 1)
        send_to_char("You can call upon the services of the mighty genie again.\n\r", ch);
      if (ch->tick_timer[TIMER_BADMOON] == 1)
        send_to_char("You can lay the badmoon enchantment again.\n\r", ch); 

      for (i = 0; i < MAX_TIMER; i++)
        if (ch->tick_timer[i] > 0) ch->tick_timer[i] --;

      /*
       * update active quests
       */
      for (quest = ch->pcdata->quests; quest; quest = quest_next)
      {
        quest_next = quest->next;
        if (--quest->time > 0) continue;
        quest_from_char(ch, quest);
        send_to_char("You have failed to complete a quest.\n\r", ch);
      }

      /*
       * void, autosave, time bonus, etc
       */
      if ((ch->level < LEVEL_IMMORTAL || !ch->desc) && !is_obj && !IS_SET(ch->extra,EXTRA_AFK))
      {
        if ((ch->desc == NULL || ch->desc->connected == CON_PLAYING) && ch->level >= 2 && ch->save_time < save_time )
        {
          ch_save = ch;
          save_time = ch->save_time;
        }
        if (++ch->timer >= 12 && !in_fortress(ch))
        {
          if (ch->was_in_room == NULL && ch->in_room != NULL)
          {
            ch->was_in_room = ch->in_room;
            if ( ch->fighting != NULL ) stop_fighting( ch, TRUE );
            act( "$n disappears into the void.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You disappear into the void.\n\r", ch );
            save_char_obj( ch );
            char_from_room( ch );
            char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
          }
        }  
        update_active_counters(ch);
        if (ch->timer > 20) ch_quit = ch;
      }
    }

    /*
     * updating spells on all mobs and players
     */
    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
      paf_next  = paf->next;
      if ( paf->duration > 0 ) paf->duration--;
      else
      {   
        if ( paf_next == NULL  || paf_next->type != paf->type || paf_next->duration > 0 )
        {
          if (paf->type > 0 && paf->type < MAX_SKILL && skill_table[paf->type].msg_off && !is_obj)
          {
            send_to_char( skill_table[paf->type].msg_off, ch );
            send_to_char( "\n\r", ch );
          }
          else if (paf->type >= MAX_SKILL)
          {
            sprintf(buf, "Update : Bad affect on %s.", ch->name);
            bug(buf, 0);
          }
        }  
        affect_remove( ch, paf );
      }
    }

    /*
     * Update class stuff
     */
    if (ch->fighting == NULL && !IS_NPC(ch))
    {
      if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->gnosis[GMAXIMUM] > ch->gnosis[GCURRENT])
      {
        if (ch->position <= POS_SLEEPING ) ch->gnosis[GCURRENT] += number_range(2,3);
        else if (ch->position <= POS_RESTING ) ch->gnosis[GCURRENT] += number_range(1,3);
        if (ch->gnosis[GCURRENT] > ch->gnosis[GMAXIMUM]) ch->gnosis[GCURRENT] = ch->gnosis[GMAXIMUM];
      }
      if (IS_CLASS(ch, CLASS_VAMPIRE) && ch->beast > 0 && ch->pcdata->condition[COND_THIRST] <= 15)
      {
        act("You bare your fangs and scream in rage from lack of blood.",ch,NULL,NULL,TO_CHAR);
        act("$n bares $s fangs and screams in rage.",ch,NULL,NULL,TO_ROOM);
        do_beastlike(ch,"");
      }
      if (!IS_SET(ch->newbits, NEW_CLOAK) && !is_obj && ((IS_CLASS(ch, CLASS_MONK)
        && ch->pcdata->powers[PMONK] > 10) || (IS_CLASS(ch,CLASS_UNDEAD_KNIGHT) && ch->pcdata->powers[NECROMANCY] > 9)))
      {
        SET_BIT(ch->newbits, NEW_CLOAK);
        if (IS_CLASS(ch, CLASS_MONK)) send_to_char("Your Cloak of Life is restored.\n\r",ch);
        else send_to_char("Your cloak of death is restored.\n\r",ch);
      }
      if (IS_CLASS(ch, CLASS_GIANT))
      {
        if (ch->pcdata->powers[GIANT_FRENZY] == 1)
        {
          ch->pcdata->powers[GIANT_FRENZY] = 0;
          send_to_char("You can enter the deathfrenzy again.\n\r",ch);
        }
      }
    }

    /*
     * Updating current position
     */
    if ( ch->position == POS_MORTAL || ch->position == POS_STUNNED || ch->position == POS_INCAP )
    {
      update_pos(ch);
    }
    if ( ch->position > POS_STUNNED && !is_obj)
    {
      if ( ch->hit  < ch->max_hit ) ch->hit = UMIN(ch->hit + number_range(5,10), ch->max_hit);
      if ( ch->mana < ch->max_mana ) ch->mana = UMIN(ch->mana + number_range(5,10), ch->max_mana);
      if ( ch->move < ch->max_move ) ch->move = UMIN(ch->move + number_range(5,10), ch->max_move);
    }
    else if ( ch->position <= POS_STUNNED && !is_obj)
    {
      ch->hit = ch->hit + number_range(2,4);
      update_pos( ch );
      if (ch->position > POS_STUNNED)
      {
        act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
        act( "You clamber back to your feet.", ch, NULL, NULL, TO_CHAR );
      }
    }

    /*
     * Dealing damage due to missling limbs, etc.
     */
    if ( ch->loc_hp[6] > 0 && !is_obj && ch->in_room != NULL )
    {
      int dam = 0;

      if (IS_BLEEDING(ch,BLEEDING_HEAD))
      {
        act( "A spray of blood shoots from the stump of $n's neck.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your neck.\n\r", ch );
        dam += number_range(20,50);
      }
      if (IS_BLEEDING(ch,BLEEDING_THROAT))
      {
        act( "Blood pours from the slash in $n's throat.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Blood pours from the slash in your throat.\n\r", ch );
        dam += number_range(10,20);
      }
      if (IS_BLEEDING(ch,BLEEDING_ARM_L))
      {
        act( "A spray of blood shoots from the stump of $n's left arm.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your left arm.\n\r", ch );
        dam += number_range(10,20);
      }
      else if (IS_BLEEDING(ch,BLEEDING_HAND_L))
      {
        act( "A spray of blood shoots from the stump of $n's left wrist.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your left wrist.\n\r", ch );
        dam += number_range(5,10);
      }
      if (IS_BLEEDING(ch,BLEEDING_ARM_R))
      {
        act( "A spray of blood shoots from the stump of $n's right arm.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your right arm.\n\r", ch );
        dam += number_range(10,20);
      }
      else if (IS_BLEEDING(ch,BLEEDING_HAND_R))
      {
        act( "A spray of blood shoots from the stump of $n's right wrist.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your right wrist.\n\r", ch );
        dam += number_range(5,10);
      }
      if (IS_BLEEDING(ch,BLEEDING_LEG_L))
      {
        act( "A spray of blood shoots from the stump of $n's left leg.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your left leg.\n\r", ch );
        dam += number_range(10,20);
      }
      else if (IS_BLEEDING(ch,BLEEDING_FOOT_L))
      {
        act( "A spray of blood shoots from the stump of $n's left ankle.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your left ankle.\n\r", ch );
        dam += number_range(5,10);
      }
      if (IS_BLEEDING(ch,BLEEDING_LEG_R))
      {
        act( "A spray of blood shoots from the stump of $n's right leg.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your right leg.\n\r", ch );
        dam += number_range(10,20);
      }
      else if (IS_BLEEDING(ch,BLEEDING_FOOT_R))
      {
        act( "A spray of blood shoots from the stump of $n's right ankle.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "A spray of blood shoots from the stump of your right ankle.\n\r", ch );
        dam += number_range(5,10);
      }
      ch->hit = ch->hit - dam;
      if (ch->hit < 1) ch->hit = 1;
      update_pos(ch);
      ch->in_room->blood += dam;
      if (ch->in_room->blood > 1000) ch->in_room->blood = 1000;
    }
    if (IS_EXTRA(ch,EXTRA_ROT) && !is_obj)
    {
      int dam;

      act( "$n's flesh shrivels and tears.", ch, NULL, NULL, TO_ROOM);
      send_to_char( "Your flesh shrivels and tears.\n\r", ch);
      dam = number_range(250,500);
      ch->hit = ch->hit - dam;
      if (ch->hit < 1) ch->hit = 1;
      update_pos(ch);
    }
    if (IS_AFFECTED(ch, AFF_FLAMING) && !is_obj)
    {
      int dam;

      act( "$n's flesh burns and crisps.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "Your flesh burns and crisps.\n\r", ch );
      dam = number_range(250,300);
      ch->hit = ch->hit - dam;
      if (ch->hit < 1) ch->hit = 1;
      update_pos(ch);
    }

    /*
     * Class special damage
     */
    if (IS_CLASS(ch, CLASS_VAMPIRE) && (!IS_AFFECTED(ch,AFF_SHADOWPLANE)) &&
       (!IS_NPC(ch) && !IS_IMMUNE(ch,IMM_SUNLIGHT)) && ch->in_room != NULL &&
       (!ch->in_room->sector_type == SECT_INSIDE) && !is_obj &&
       (!room_is_dark(ch->in_room)) && (weather_info.sunlight != SUN_DARK))
    {
      act( "$n's flesh smolders in the sunlight!", ch, NULL, NULL, TO_ROOM );
      send_to_char( "Your flesh smolders in the sunlight!\n\r", ch );

      /* This one's to keep Zarkas quiet ;) */
      if (IS_POLYAFF(ch, POLY_SERPENT))
    	ch->hit = ch->hit - number_range(2,4);
      else
    	ch->hit = ch->hit - number_range(5,10);
      update_pos(ch);
    }

    /*
     * More damage stuff
     */
    if (IS_AFFECTED(ch, AFF_POISON) && !is_obj)
    {
      act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You shiver and suffer.\n\r", ch );
      ch->hit = ch->hit - number_range(100, 200);
      if (ch->hit < 1) ch->hit = 1;
      if (number_range(1,4)==1)
      {
        REMOVE_BIT(ch->affected_by, AFF_POISON);
        send_to_char("You feel the poison leave your system.\n\r",ch);
      }
    }

    /*
     * and then we do some healing - messy ?
     */
    if ( ch->position == POS_INCAP && !is_obj)
    {
      ch->hit = ch->hit + number_range(2,4);
      update_pos( ch );
      if (ch->position > POS_INCAP)
      {
        act( "$n's wounds stop bleeding and seal up.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Your wounds stop bleeding and seal up.\n\r", ch );
      }
      if (ch->position > POS_STUNNED)
      {
        act( "$n clambers back to $s feet.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You clamber back to your feet.\n\r", ch );
      }
    }
    else if (ch->position == POS_MORTAL && !is_obj)
    {
      ch->hit = ch->hit + number_range(2,4);
      update_pos( ch );
      if (ch->position == POS_INCAP)
      {
        act( "$n's wounds begin to close, and $s bones pop back into place.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Your wounds begin to close, and your bones pop back into place.\n\r", ch );
      }
    }
  }

  /*
   * Autosave, Autoquit checks
   */
  if ( ch_save != NULL || ch_quit != NULL )
  {
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
      ch_next = ch->next;
      if ( ch == ch_save ) save_char_obj( ch );
      if ( ch == ch_quit ) do_quit( ch, "" );  
    }
  }
  return;
}

void arti_update( void )
{
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObjIndex;
  ROOM_INDEX_DATA *location;
  bool ARTI_RING = FALSE;
  bool ARTI_BRACER = FALSE;
  bool ARTI_MAIL = FALSE;
  bool ARTI_SNAKE = FALSE;
  bool ARTI_SWORD = FALSE;
  bool ARTI_EYE = FALSE;
  bool ARTI_SIGIL = FALSE;
  int max_arti = 3;
  int number_artifact = 0;

  for (obj = object_list; obj != NULL; obj = obj->next)
  {
    if (obj->pIndexData->vnum == 33980)       {ARTI_RING    = TRUE; number_artifact++;}
    else if (obj->pIndexData->vnum == 33981)  {ARTI_BRACER  = TRUE; number_artifact++;}
    else if (obj->pIndexData->vnum == 33982)  {ARTI_MAIL    = TRUE; number_artifact++;}
    else if (obj->pIndexData->vnum == 33983)  {ARTI_SNAKE   = TRUE; number_artifact++;}
    else if (obj->pIndexData->vnum == 33984)  {ARTI_SWORD   = TRUE; number_artifact++;}
    else if (obj->pIndexData->vnum == 33985)  {ARTI_EYE     = TRUE; number_artifact++;}
    else if (obj->pIndexData->vnum == 33986)  {ARTI_SIGIL   = TRUE; number_artifact++;}
  }
  if (!ARTI_RING && number_artifact < max_arti && number_range(1,3) == 2)
  {
    location = get_rand_room();
    if ((pObjIndex = get_obj_index(33980)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }  
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
  if (!ARTI_BRACER && number_artifact < max_arti && number_range(1,3) == 2)
  {
    location = get_rand_room();
    if ((pObjIndex = get_obj_index(33981)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
  if (!ARTI_MAIL && number_artifact < max_arti && number_range(1,3) == 2)
  {
    location = get_rand_room();
    if ((pObjIndex = get_obj_index(33982)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
  if (!ARTI_SNAKE && number_artifact < max_arti && number_range(1,3) == 2)
  {
    location = get_rand_room();
    if ((pObjIndex = get_obj_index(33983)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
  if (!ARTI_SWORD && number_artifact < max_arti && number_range(1,3) == 2)
  {
    location = get_rand_room();
    if ((pObjIndex = get_obj_index(33984)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
  if (!ARTI_EYE && number_artifact < max_arti && number_range(1,3) == 2)
  {  
    location = get_rand_room();   
    if ((pObjIndex = get_obj_index(33985)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
  if (!ARTI_SIGIL && number_artifact < max_arti && number_range(1,3) == 2)
  {  
    location = get_rand_room();   
    if ((pObjIndex = get_obj_index(33986)) == NULL)
    {
      bug("Missing artifact.", 0);
      return;
    }
    obj = create_object(pObjIndex, 60);
    obj_to_room(obj, location);
    number_artifact++;
  }
}


/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{   
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  for ( obj = object_list; obj != NULL; obj = obj_next )
  {
    CHAR_DATA *rch;
    char *message;

    obj_next = obj->next;
    if ( obj->timer <= 0 || --obj->timer > 0 ) continue;

    switch ( obj->item_type )
    {
	default:              message = "$p vanishes.";         	break;
	case ITEM_FOUNTAIN:   message = "$p dries up.";         	break;
	case ITEM_CORPSE_NPC: message = "$p decays into dust."; 	break;
	case ITEM_CORPSE_PC:  message = "$p decays into dust."; 	break;
	case ITEM_FOOD:       message = "$p decomposes.";		break;
	case ITEM_TRASH:      message = "$p crumbles into dust.";	break;
	case ITEM_WEAPON:     message = "$p turns to fine dust and blows away."; break;
        case ITEM_WALL:       message = "$p flows back into the ground.";break;
        case ITEM_GRENADE:    message = "$p explodes in a shower of flames.";break;
    }

	if ( obj->carried_by != NULL && !IS_OBJ_STAT2(obj, ITEM_DAEMONSEED) )
	{
	    act( message, obj->carried_by, obj, NULL, TO_CHAR );
	}
	else if ( obj->in_room != NULL
	&&      ( rch = obj->in_room->people ) != NULL && !IS_OBJ_STAT2(obj, ITEM_DAEMONSEED) )
	{
	    act( message, rch, obj, NULL, TO_ROOM );
	    act( message, rch, obj, NULL, TO_CHAR );
	}

        if (IS_OBJ_STAT2(obj, ITEM_DAEMONSEED) && obj != NULL
	    && obj->in_obj == NULL && (locate_obj(obj))->people )
	{
	    char buf[MAX_STRING_LENGTH];
	    CHAR_DATA *vch;
	    int wdam;

	    sprintf(buf, "%s suddenly explodes in a ball of flame, incinerating you!\n\r",obj->short_descr);
	    buf[0] = UPPER(buf[0]);
	    if ((locate_obj(obj))->people == NULL) break;
	    for ( vch = (locate_obj(obj))->people; vch != NULL;vch=vch->next_in_room )
    	    {
		if (vch->class == 0 || (!IS_NPC(vch) && vch->level < 3)) continue;
	    if ( IS_SET(vch->in_room->room_flags,ROOM_SAFE) )
	    {
		stc("You are unaffected by the blast.\n\r",vch);
		continue;
	    }
	    wdam = obj->level + dice(12,50);
            damage(vch,vch,obj->level+ dice(12, 50),gsn_inferno); 
		send_to_char(buf, vch);
	    sprintf(buf,"The flames strike you incredibly hard![%d]\n\r",wdam);
		stc(buf,vch);
 	    }
	}

    /*
     * grenade code.
     */
    if (obj->item_type == ITEM_GRENADE && (locate_obj(obj))->people)
    {
      char buf[MAX_STRING_LENGTH];
      CHAR_DATA *vch;
      int wdam;

      for (vch = (locate_obj(obj))->people; vch != NULL; vch = vch->next_in_room)
      {
        wdam = number_range(100, 1000) * obj->value[2];
        wdam = UMIN(vch->hit - 1, wdam);
        if (wdam < 1) continue;
        if (wdam > 5000) wdam = 5000;
        if (IS_AFFECTED(vch, AFF_SANCTUARY)) wdam *= 0.5;
        vch->hit -= wdam;
        sprintf(buf, "The flames strike you incredibly hard![%d]\n\r",wdam);
        send_to_char(buf, vch);
        send_detonate(vch);
        sprintf(buf, "The flames strikes $n incredibly hard![%d]",wdam);
        act(buf, vch, NULL, NULL, TO_ROOM);
      }
    }

        /*
         * BUGFIX : The famous extract_obj bug....
         *          basicly we just make sure that we don't skip into the obj_free list
         *          by accident. (That's what caused it).
         */

        if (obj == object_list)
        {
          extract_obj(obj);
          obj_next = object_list;
        }
        else
        {
          OBJ_DATA *bugObj;

          for (bugObj = object_list; bugObj; bugObj = bugObj->next)
          {
            if (bugObj->next == obj) break;
          }
          /*
           * This shouldn't happen, but if it does, I want to make sure we never see that object again,
           * so we let extract_obj() try and handle it, terminating to avoid further corruption.
           */
          if (!bugObj)
          {
            bug("obj_update: obj %d not in object_list. Terminating obj_update.", obj->pIndexData->vnum);
            return;
          }
          extract_obj(obj);
          obj_next = bugObj->next;
        }

       /*
        * The old simple line that caused the bug... It's a long story, just rejoice.
        * Keeping the line - always nice to scare people with.
        */

//	if (obj != NULL) extract_obj( obj );
    }

    return;
}

 
void embrace_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *ch;
    CHAR_DATA       *victim;
    int blpr;  /* variable to check for amout of blood sucked. Shakti */
    char buf[MAX_STRING_LENGTH];

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
      
      if ( ( ch = d->character) == NULL )
	continue;

      if (IS_CLASS(ch, CLASS_DRONE) && IS_SET(ch->pcdata->jflags, JFLAG_DRONE_DISTORT))            
      {
        send_to_char("Your weave of distorted reality fades.\n\r", ch);
        REMOVE_BIT(ch->pcdata->jflags, JFLAG_DRONE_DISTORT);
      }

      if (IS_SET(ch->pcdata->jflags, JFLAG_MADNESS))
      {
        if (ch->hit < 0) break;

        switch (number_range(1, 6))
        {
          default: break;
          case 1:
            ch->fight_timer++;
            do_say(ch, "Arrgh, get it off me, get it off me!");
            do_cast(ch, "'dispel magic' self");
            break;
          case 2:
            ch->fight_timer++;
            act("$n screams out in anger, attacking everything wildly.", ch, NULL, NULL, TO_ROOM);
            send_to_char("You feel a need to kill everyone.\n\r", ch);
            do_berserk2(ch, "");
            break;
          case 3:
            ch->fight_timer++;
            send_to_char("The horror, the horror, you must flee this place instantly.\n\r", ch);
            do_flee(ch, "");
            break;
          case 4:
          case 5:
            act("$n get's a sane look in $s eyes for a second.", ch, NULL, NULL, TO_ROOM);
            send_to_char("You regain control over your body for a short while.\n\r", ch);
            break;
          case 6:
            act("$n returns to normal, the madness fleeing.", ch, NULL, NULL, TO_ROOM);
            REMOVE_BIT(ch->pcdata->jflags, JFLAG_MADNESS);
            send_to_char("Your world returns to normal.\n\r", ch);
            break;
        }
      }

      if (IS_SET(ch->pcdata->jflags, JFLAG_DRONE_HORRID))
      {
        int dam;

        if (ch->hit < 0) break;

        switch (number_range(1, 8))
        {
          default: break;
          case 1:
          case 2:
            ch->fight_timer++;
            act("A tentacle appears from out of nowhere and slashes at you.", ch, NULL, NULL, TO_CHAR);
            act("$n screams out in fear, eyes locked on some distant nightmare that only $e can see.", ch, NULL, NULL, TO_ROOM);
            dam = UMIN(ch->hit - 1, number_range(500, 1500));
            ch->hit -= dam;
            break;
          case 3:
          case 4:
            ch->fight_timer++;
            act("Flames burst from your chest, you try to put it out, but fail.", ch, NULL, NULL, TO_CHAR);
            act("$n clutches $s chest, trying to prevent some damage, that only $e can see.", ch, NULL, NULL, TO_ROOM);
            if (!IS_SET(ch->affected_by, AFF_FLAMING)) SET_BIT(ch->affected_by, AFF_FLAMING);
            dam = UMIN(ch->hit - 1, number_range(400, 800));
            ch->hit -= dam;
            break;
          case 5:
            ch->fight_timer++;
            act("Giants spiders suddenly appear on your body, spinning a spidery web around your body.", ch, NULL, NULL, TO_CHAR);
            act("$n looks with panic at $s own body, staring in disbelief.", ch, NULL, NULL, TO_ROOM);
            if (!IS_SET(ch->affected_by, AFF_WEBBED)) SET_BIT(ch->affected_by, AFF_WEBBED);
            dam = UMIN(ch->hit - 1, number_range(500, 1300));
            ch->hit -= dam;
            break;
          case 6:
          case 7:
            ch->fight_timer++;
            act("You roar in pain as sharp thorns sprout from your body.", ch, NULL, NULL, TO_CHAR);
            act("$n screams out in pain, staring at $s his body.", ch, NULL, NULL, TO_ROOM);
            dam = UMIN(ch->hit - 1, number_range(300, 1000));
            ch->hit -= dam;
            break;
          case 8:
            act("$n looks relieved, as if awakening from a long nightmare.", ch, NULL, NULL, TO_ROOM);
            REMOVE_BIT(ch->pcdata->jflags, JFLAG_DRONE_HORRID);
            send_to_char("Your nightmare fades.\n\r", ch);
            break;
        }
      }

      if ( IS_CLASS(ch, CLASS_MONK) && ch->fighting == NULL )
	ch->monkblock = 0;

      if ( IS_NPC(ch) || ch->embracing == NULL )
        continue;

	if (!IS_CLASS(ch,CLASS_VAMPIRE))
	  stop_embrace(ch,NULL);

	victim=ch->embracing;

/* Fix for embracing mobs by Shakti.					*/

	if (IS_NPC(victim) )
	{
		if (victim->level > 75 * ch->spl[2] )
		{
			stop_embrace(ch,victim);
			send_to_char("You cannot embrace someone so powerful!\n\r",ch);
			return;
		}
		
	/* To keep how much blood was sucked, and how much gained the same. Shakti */
	
		(blpr =number_range(30,40));
		victim->practice -=blpr;
		ch->pcdata->condition[COND_THIRST] +=blpr;
		
		sprintf(buf, "%s shudders in ecstacy as he drinks blood from %s's neck.\n\r", ch->name,victim->short_descr);
	    act(buf,ch,NULL,NULL,TO_ROOM);
	    sprintf(buf, "You shudder in ecstacy as you drink blood from %s's neck.\n\r",victim->short_descr);
	    send_to_char(buf,ch);
	    
		if (victim->practice < 0)
		{
			victim->practice = 0;
		}

		if (victim->practice == 0)
		
		{
	     raw_kill(victim);
	     sprintf(buf,"%s's body falls lifless to the ground!.\n\r",victim->short_descr);
	     act(buf,ch,NULL,NULL,TO_ROOM);
	     send_to_char(buf,ch);
	     stop_embrace(ch,victim);
		 return;
		}

		if ( ch->pcdata->condition[COND_THIRST] > (3000 / ch->generation))
		{
	 	  ch->pcdata->condition[COND_THIRST] = (3000 / ch->generation);
		  stc("Your bloodlust is sated.\n\r",ch);
		}

		continue;
	}



	if (get_char_world(ch,victim->name) == NULL)
	  {stop_embrace(ch,NULL);continue;}
	if (ch->in_room != victim->in_room)
	  stop_embrace(ch,victim);
	if( ch->in_room->vnum == ROOM_VNUM_ALTAR )
	  stop_embrace(ch,victim);
	if (victim->pcdata->condition[COND_THIRST] < 0)
	  victim->pcdata->condition[COND_THIRST] = 0;
       
	ch->pcdata->condition[COND_THIRST] += number_range(30,40);
    sprintf(buf, "%s shudders in ecstacy as he drinks blood from %s's neck.\n\r", ch->name,victim->name);
	act(buf,ch,NULL,NULL,TO_ROOM);
	sprintf(buf, "You shudder in ecstacy as you drink blood from %s's neck.\n\r",victim->name);
	send_to_char(buf,ch);
	sprintf(buf, "You feel some of your life slip away as %s drinks from your neck.\n\r", ch->name);
	send_to_char(buf,victim);
	ch->pcdata->condition[COND_THIRST] += number_range(35,40);
	victim->pcdata->condition[COND_THIRST] -= number_range(40,42);
	if( victim->pcdata->condition[COND_THIRST] > 0 ) 
	  victim->pcdata->condition[COND_THIRST]=0;

	if (ch->pcdata->condition[COND_THIRST] > 3000/ch->generation)

	{ch->pcdata->condition[COND_THIRST]=3000/ch->generation;
	send_to_char("Your bloodlust is sated.\n\r",ch);}
	if (victim->pcdata->condition[COND_THIRST] < 0)
	  victim->pcdata->condition[COND_THIRST]=0;
	if( !IS_NPC(victim) && victim->pcdata->condition[COND_THIRST] != 0 )
	  victim->pcdata->condition[COND_THIRST]=0;

    continue; 
  }
  return;
}

void ww_update( void )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *victim;
    float            dam = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
      {
      if (!IS_PLAYING(d) || (victim = d->character) == NULL
        || IS_NPC(victim) || IS_IMMORTAL(victim)
	|| victim->in_room == NULL || victim->pcdata->chobj != NULL
        || IS_CLASS(victim,CLASS_WEREWOLF))
	{
            continue;
        }
      if ( !IS_SET( d->character->in_room->room_flags, ROOM_BLADE_BARRIER ) )
        continue;

      act( "The scattered blades on the ground fly up into the air ripping into you.", d->character, NULL, NULL, TO_CHAR );
      act( "The scattered blades on the ground fly up into the air ripping into $n.", d->character, NULL, NULL, TO_ROOM );

      act( "The blades drop to the ground inert.", d->character, NULL, NULL, TO_CHAR );
      act( "The blades drop to the ground inert.", d->character, NULL, NULL, TO_ROOM );

      dam = number_range( 7, 14 );
      dam = dam / 100;
      dam = d->character->hit * dam;
      if ( dam < 100 ) dam = 100;
      d->character->hit = d->character->hit - dam;
      if ( d->character->hit < -10 ) d->character->hit = -10;
      update_pos( victim );
      }

    return;
}


/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_area;
    static  int     pulse_mobile;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int     pulse_ww;
    static  int     pulse_embrace;
    static  int     pulse_mudinfo;
    static  int     pulse_html;
    static  int     pulse_track;

    recycle_descriptors();


    if (--pulse_track <= 0)
    {
      pulse_track = 2;
      update_trackers();
    }

    if ( ++ragnarok_timer > PULSE_RAGNAROK && ragnarok)
    {
      ragnarok_stop();
      ragnarok_timer = 0;
    }
    if (pulse_doubleexp > 0)
    {
      if ( --pulse_doubleexp == 0)
      {
        global_exp = FALSE;
        exptimer = 0;
        pulse_doubleexp = -1;
        do_info(NULL,"#GH#Ra#Gp#Rp#Gy #GH#Ro#Gu#Rr #0(#7ENDED#0)#n");
      }
    }
    if (pulse_doubleqp > 0)
    {
      if ( --pulse_doubleqp == 0)
      {
        global_qp = FALSE;
        pulse_doubleqp = -1;
        qpstimer = 0;
        do_info(NULL,"#GQ#RU#GE#RS#GT#RO#GR#RS #GD#RE#GL#RI#GG#RH#GT #0(#7ENDED#0)#n");
      }
    }
    if ( --pulse_ww       <= 0 )
    {
        pulse_ww        = PULSE_WW;
        ww_update       ( );
    }
    if (arena_open)
    {
      pulse_arena = PULSE_ARENA - (45 * PULSE_PER_SECOND + 1);
      arena_open = FALSE;
    }
    if ( ++pulse_arena	  >= PULSE_ARENA - 45 * PULSE_PER_SECOND)
    {
      if (pulse_arena < PULSE_ARENA && !arenaopen) open_arena();
      else if (pulse_arena >= PULSE_ARENA)
      {
         pulse_arena = 0;
         close_arena();
      }
    }
    if ( --pulse_area     <= 0 )
    {
	pulse_area	= number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
        arti_update     ( );
	area_update	( );
        newbie_help();
        parse_who();
    }

    if ( --pulse_html   <= 0 )
    {
        pulse_html   = 180 * PULSE_AREA;
        parse_top    ( );
        parse_leader ( );
    }

    if ( --pulse_mudinfo <= 0)
    {
      pulse_mudinfo   = PULSE_THIRTY;
      update_mudinfo  ( );
      update_polls    ( );
      recycle_dummys  ( ); // every 30 secs is enough
      if (!global_exp)
      {
        exptimer++;
        if (!global_qp) chance_exp();  // don't want both running
      }
      if (!global_qp)
      {
        qpstimer++;
        if (!global_exp) chance_qps();  // don't want both running
      }
    }

    if ( --pulse_mobile   <= 0 )
    {
	pulse_mobile	= PULSE_MOBILE;
	mobile_update	( );
    }

    if ( --pulse_violence <= 0 )
    {
	pulse_violence	= PULSE_VIOLENCE;
	violence_update	( );
    }
    if ( --pulse_embrace <= 0)
    {
        pulse_embrace = PULSE_EMBRACE;
        embrace_update ( ); 
    }
    if ( --pulse_point    <= 0 )
    {
	pulse_point     = number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 );
	weather_update	( );
	char_update	( );
	obj_update	( );
        room_update	( );

        /*
         * The following is some excessive force.
         * If the mud crashes again within a pulse from last crash,
         * let it go down to avoid crashloops.
         */
        if(iDelete == 0)
            iDelete++;
        else if(iDelete == 1)
        {
            unlink(CRASH_TEMP_FILE);
            iDelete++;
        }

    }

    tail_chain( );
    return;
}



void update_morted_timer(CHAR_DATA *ch)
{
	if( ch->hit > 0 && ch->pcdata->mortal > 0 ) ch->pcdata->mortal = 0;
	else	 
	{
		ch->pcdata->mortal += 1;		
		if( ch->pcdata->mortal > 10 && !in_fortress(ch))
		{
			ch->hit = 100;
			update_pos(ch);
			char_from_room(ch);
			char_to_room(ch,get_room_index(3054));
			stc( "You recieve a heal from the gods.\n\r",ch);
			ch->pcdata->mortal = 0;
                        ch->fight_timer = 0;
		}
	}
}

void update_sit_safe_counter(CHAR_DATA *ch)
{
  if (IS_NPC(ch)) return;

  if (ch->pcdata->safe_counter > 0)
  {
    ch->pcdata->safe_counter--;
    if (ch->pcdata->safe_counter == 0)
      send_to_char("You can now pk again.\n\r", ch);
  }
  if (IS_SET(ch->in_room->room_flags, ROOM_SAFE) && ch->level > 2 && ch->level < 7 && getMight(ch) >= 150)
  {
    if (!IS_SET(ch->extra,EXTRA_AFK)) ch->pcdata->sit_safe += number_range(5,10);

    if ((ch->pcdata->sit_safe > 200 || ch->fight_timer != 0) && !ragnarok && !ch->fighting)
    {
      stc( "\nThe gods are tired of granting you refuge.\n\r",ch);
      char_from_room(ch);
      char_to_room(ch,get_room_index(ROOM_VNUM_TEMPLE));
    }
  }
  else
  {
    if (ch->pcdata->sit_safe > 0) ch->pcdata->sit_safe -=10;
    else ch->pcdata->sit_safe = 0;
  }
}

void update_drunks(CHAR_DATA *ch)
{
	// I strongly suggest adding more stuff to drunks -xiphias *grin*
	if (ch->pcdata->condition[COND_DRUNK] > 10 && number_range(1,10) == 1)
	{
		send_to_char("You hiccup loudly.\n\r",ch);
		act("$n hiccups.",ch,NULL,NULL,TO_ROOM);
	}
  if (ch->pcdata->condition[COND_DRUNK] > 0 && number_range(1,10) == 5)
  {   
    ch->pcdata->condition[COND_DRUNK]--;
    if (ch->pcdata->condition[COND_DRUNK] == 0)
      send_to_char("You feel sober again.\n\r", ch);
  }

}

void sex_update(CHAR_DATA *ch)
{
	if (ch->pcdata->stage[0] > 0 || ch->pcdata->stage[2] > 0)
	{
		CHAR_DATA *vch;
		if (ch->pcdata->stage[1] > 0 && ch->pcdata->stage[2] >= 225)
		{
			ch->pcdata->stage[2] += 1;
			if ( ( vch = ch->pcdata->partner ) != NULL &&
			!IS_NPC(vch) && vch->pcdata->partner == ch 
			&& ((vch->pcdata->stage[2] >= 200 && vch->sex == SEX_FEMALE) 
			|| (ch->pcdata->stage[2] >= 200 && ch->sex == SEX_FEMALE)))
			{
				if (ch->in_room != vch->in_room)return ;
				if (vch->pcdata->stage[2] >= 225 
				&& ch->pcdata->stage[2] >= 225 
				&& vch->pcdata->stage[2] < 240 
				&& ch->pcdata->stage[2] < 240)
				{
					ch->pcdata->stage[2] = 240;
					vch->pcdata->stage[2] = 240;
				}
				if (ch->sex == SEX_MALE && vch->pcdata->stage[2] >= 240)
				{
					act("You thrust deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_CHAR);
					act("$n thrusts deeply between your warm, damp thighs.",ch,NULL,vch,TO_VICT);
					act("$n thrusts deeply between $N's warm, damp thighs.",ch,NULL,vch,TO_NOTVICT);
					if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
						ch->pcdata->stage[2] = vch->pcdata->stage[2];
				}
				else if (ch->sex == SEX_FEMALE && vch->pcdata->stage[2] >= 240)
				{
					act("You squeeze your legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_CHAR);
					act("$n squeezes $s legs tightly around you, moaning loudly.",ch,NULL,vch,TO_VICT);
					act("$n squeezes $s legs tightly around $N, moaning loudly.",ch,NULL,vch,TO_NOTVICT);
					if (vch->pcdata->stage[2] > ch->pcdata->stage[2])
						ch->pcdata->stage[2] = vch->pcdata->stage[2];
				}
			}
			if (ch->pcdata->stage[2] >= 250)
			{
				if ( ( vch = ch->pcdata->partner ) != NULL &&
					!IS_NPC(vch) && vch->pcdata->partner == ch &&
					ch->in_room == vch->in_room)
				{
					vch->pcdata->stage[2] = 250;
					if (ch->sex == SEX_MALE)
					{
						stage_update(ch,vch,2,"xm-thrust");
						stage_update(vch,ch,2,"xf-squeeze");
					}
					else
					{
						stage_update(vch,ch,2,"xm-thrust");
						stage_update(ch,vch,2,"xf-squeeze");
					}
											
					ch->pcdata->stage[0] = 0;
					vch->pcdata->stage[0] = 0;
					
					if (!IS_EXTRA(ch, EXTRA_EXP))
					{
						send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",ch);
						SET_BIT(ch->extra, EXTRA_EXP);
						ch->exp += 100000;
					}
					if (!IS_EXTRA(vch, EXTRA_EXP))
					{
						send_to_char("Congratulations on achieving a simultanious orgasm!  Recieve 100000 exp!\n\r",vch);
						SET_BIT(vch->extra, EXTRA_EXP);
						vch->exp += 100000;
					}
				}
			}
		}
		else
		{
			if (ch->pcdata->stage[0] > 0 && ch->pcdata->stage[2] < 1 &&
				ch->position != POS_RESTING) 
			{
				if (ch->pcdata->stage[0] > 1)
					ch->pcdata->stage[0] -= 1;
				else
					ch->pcdata->stage[0] = 0;
			}
			else if (ch->pcdata->stage[2]>0 && ch->pcdata->stage[0] < 1)
			{
				if (ch->pcdata->stage[2] > 10)
					ch->pcdata->stage[2] -= 10;
				else
					ch->pcdata->stage[2] = 0;
				if (ch->sex == SEX_MALE && ch->pcdata->stage[2] == 0)
					send_to_char("You feel fully recovered.\n\r",ch);
			}
		}
	}
}

void update_safe_powers(CHAR_DATA *ch)
{
  int cost = 800;
  bool cheating = FALSE;
  char buf[MAX_STRING_LENGTH];

  /* Arene cheaters getting it the hard way */
  if (ch->in_room != NULL && IS_SET(ch->in_room->room_flags, ROOM_ARENA) && ch->level < 7)
  {
    if (IS_SET(ch->act, PLR_WIZINVIS)) {dropinvis(ch); cheating = TRUE;}
    if (IS_SET(ch->act, AFF_HIDE)) {dropinvis(ch); cheating = TRUE;}
    if (IS_SET(ch->newbits, NEW_DARKNESS)) {dropinvis(ch); cheating = TRUE;}
    if (IS_AFFECTED(ch, AFF_PEACE)) {REMOVE_BIT(ch->affected_by, AFF_PEACE); cheating = TRUE;}
    if (IS_AFFECTED(ch,AFF_SHADOWPLANE)) {REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE); cheating = TRUE;}
    if (IS_AFFECTED(ch,AFF_ETHEREAL)) {REMOVE_BIT(ch->affected_by, AFF_ETHEREAL); cheating = TRUE;}
    if (!IS_HEAD(ch,LOST_HEAD) && IS_EXTRA(ch,EXTRA_OSWITCH)) {do_humanform(ch, ""); cheating = TRUE;}
    if (cheating)
    {
      ch->hit = ch->hit/2; // Punish those bastards
      sprintf(buf, "%s is cheating in the arena!!!", ch->name);
      do_info(ch, buf);
    }
  }
  if (ch->position != POS_STANDING && ch->position != POS_STUNNED)
  {
    cost *= 4;
  }
  if (IS_CLASS(ch, CLASS_WEREWOLF) || IS_CLASS(ch, CLASS_HOBBIT)) cost *= 1.5;
  if (!IS_HEAD(ch,LOST_HEAD) && IS_EXTRA(ch,EXTRA_OSWITCH))
  {
    if (ch->move >= cost) ch->move -=cost;
    else do_humanform(ch,"");
  }
  if (IS_SET(ch->act, PLR_WIZINVIS))
  {
    if (ch->move >= cost) ch->move -=cost;
    else
    {
      REMOVE_BIT(ch->act, PLR_WIZINVIS);
      act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly fade back into existence.\n\r", ch );
    }
  }
  if (IS_SET(ch->act, AFF_HIDE))
  {
    if (ch->move >= cost) ch->move -= cost;
    else
    {
      REMOVE_BIT(ch->act, AFF_HIDE);
      act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly fade back into existence.\n\r", ch );
    }
  }
  if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_ANGEL))
  {
    if (IS_SET(ch->pcdata->powers[ANGEL_POWERS], ANGEL_EYE))
    {
      if(ch->move < cost*2 || ch->mana < cost*2)
      {
        REMOVE_BIT(ch->pcdata->powers[ANGEL_POWERS], ANGEL_EYE);
        send_to_char("The sinners have payed for their sins, time to rest.\n\r",ch);
      }
      else
      {
        ch->move -= cost*2;
        ch->mana -= cost*2;
      }
    }
  }
  if (IS_SET(ch->newbits, NEW_DARKNESS))
  {
    if (ch->mana >= cost) ch->mana -= cost;
    else
    {
      REMOVE_BIT(ch->newbits, NEW_DARKNESS);
      REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
      send_to_char("You cannot pay the upkeep and banish your globe of darkness.\n\r", ch );
      act("The globe of darkness around $n disappears.",ch,NULL,NULL,TO_ROOM);
    }
  }
  if (IS_AFFECTED(ch, AFF_PEACE))
  {
    if (ch->mana >= cost) ch->mana -= cost;
    else if (!IS_CLASS(ch, CLASS_ANGEL))
    {
      REMOVE_BIT(ch->affected_by, AFF_PEACE);
      act( "You lower your majesty.",  ch, NULL, NULL, TO_CHAR    );
      act( "$n looks less imposing.",  ch, NULL, NULL, TO_NOTVICT );
    }
    else
    {   
      REMOVE_BIT(ch->affected_by, AFF_PEACE);
      act( "God protects you no longer.",  ch, NULL, NULL, TO_CHAR    );
      act( "$n looks wicked.",  ch, NULL, NULL, TO_NOTVICT );
    }
  }
  if (IS_AFFECTED(ch,AFF_SHADOWPLANE))
  {
    if (ch->mana >= cost) ch->mana -= cost;
    else
    {
      REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
      send_to_char("You fade back into the real world.\n\r",ch);
      act("The shadows flicker and $n fades into existance.",ch,NULL,NULL,TO_ROOM);
    }
  }
  if (IS_AFFECTED(ch,AFF_ETHEREAL))
  {
    if (ch->mana >= cost) ch->mana -= cost;
    else
    {
      REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
      send_to_char("Your body becomes firm again.\n\r",ch);
      act("$n becomes sold again.",ch,NULL,NULL,TO_ROOM);
    }
  }

  /* Has nothing to do with safe powers, just needed it somewhere, so here it is */
  if (IS_SET(ch->newbits, NEW_VISION))
  {
    if (number_percent() < 15 && IS_SET(ch->itemaffect, ITEMA_VISION))
    {
      send_to_char("#pYour superior sight fades away.#n\n\r",ch);
      REMOVE_BIT(ch->itemaffect, ITEMA_VISION);
      REMOVE_BIT(ch->newbits, NEW_VISION);
    }
  }
  return;
}

void update_vampire_regen(CHAR_DATA *ch)
{
  if ( ch->position == POS_FIGHTING && ch->rage > 0 && ch->rage < 25 && !IS_ITEMAFF(ch, ITEMA_RAGER))
    ch->rage += 1;
  else if (ch->rage > 0 && !IS_ITEMAFF(ch, ITEMA_RAGER))
    ch->rage -= 1;
  if (ch->rage == 0) return;
  if (( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move) && ch->rage >0)
  {
    werewolf_regen(ch, 2);
    regen_limb(ch);
	}
  return;
}

void update_vampire(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93350 && ch->in_room->vnum <= 93359))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  return;
}

void update_cyborg(CHAR_DATA *ch)
{
  if (!IS_ITEMAFF(ch, ITEMA_DROIDSPEED))
  {
    if (ch->position != POS_FIGHTING && ch->pcdata->powers[DROID_SPEED] > 0 && number_range( 1,4 ) == 2)
    {
      ch->pcdata->powers[DROID_SPEED]--;
      stc("You feel the rush of speed subside.\n\r",ch);
    }
  }
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_knight (CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93300 && ch->in_room->vnum <= 93309))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (ch->pcdata->powers[POWER_TICK] > 0) ch->pcdata->powers[POWER_TICK]--;
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_shapeshifter (CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93310 && ch->in_room->vnum <= 93319))
  {  
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (ch->pcdata->powers[PHASE_COUNTER] > 0) ch->pcdata->powers[PHASE_COUNTER]--;
  if (ch->pcdata->powers[SHAPE_COUNTER] > 0) ch->pcdata->powers[SHAPE_COUNTER]--;
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_tanarri(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93330 && ch->in_room->vnum <= 93339))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  return;
}

void update_drider(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93350 && ch->in_room->vnum <= 93359))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  return;
}

void update_hobbit(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93470 && ch->in_room->vnum <= 93479))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch, 3);
  regen_limb(ch);
  return;
}

void update_lich_regen(CHAR_DATA *ch)
{
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_lich(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93360 && ch->in_room->vnum <= 93369))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (ch->position == POS_MEDITATING && ch->mana < ch->max_mana)
  {
    ch->mana += number_range(1000,2000);
    if (ch->mana > ch->max_mana)
      ch->mana = ch->max_mana;
  }
  if (IS_SET(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_FIRE) && number_range(1,50) == 4)
  {
    send_to_char("You can summon another fire golem.\n\r",ch);
    REMOVE_BIT(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_FIRE);
  }
  if (IS_SET(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_IRON) && number_range(1,50) == 4)
  {
    send_to_char("You can summon another iron golem.\n\r",ch);
    REMOVE_BIT(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_IRON);
  }
  if (IS_SET(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_CLAY) && number_range(1,50) == 4)
  {
    send_to_char("You can summon another clay golem.\n\r",ch);
    REMOVE_BIT(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_CLAY);
  }
  if (IS_SET(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_STONE) && number_range(1,50) == 4)
  {
    send_to_char("You can summon another stone golem.\n\r",ch);
    REMOVE_BIT(ch->pcdata->powers[GOLEMS_SUMMON], HAS_SUMMONED_STONE);
  }
  return;
}

void update_angel(CHAR_DATA *ch)
{
  if (ch->pcdata->powers[ANGEL_LOVE] > 3)
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move )
      werewolf_regen(ch, 2);
    regen_limb(ch);
  }
  if (ch->pcdata->powers[ANGEL_PEACE_COUNTER] > 0) ch->pcdata->powers[ANGEL_PEACE_COUNTER]--;
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93340 && ch->in_room->vnum <= 93349))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  return;
}


void update_monk(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93410 && ch->in_room->vnum <= 93419))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  } 
  if (!IS_ITEMAFF(ch, ITEMA_MONKCHI))
  {
    if (ch->position != POS_FIGHTING 	&& ch->chi[CURRENT] > 0	&& number_range( 1,4 ) == 2)
    {
      ch->chi[CURRENT]--;
      stc("You feel more relaxed.\n\r",ch);
      act("$n looks more relaxed.",ch,NULL,NULL,TO_ROOM);
    }
  }
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move )
    werewolf_regen(ch, 2);
  regen_limb(ch); 
  return;
}

void update_fae(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93370 && ch->in_room->vnum <= 93379))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);  
  }
  if (ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
  {
    if (ch->position == POS_MEDITATING && ch->fight_timer < 1 && !IS_SET(ch->act, AFF_HIDE)) werewolf_regen(ch,10);
    else werewolf_regen(ch,2);
  }
  regen_limb(ch);
  return;
}

void update_ninja(CHAR_DATA *ch)
{
  if (ch->position != POS_FIGHTING)
  {
    if (++ch->pcdata->powers[NINJA_COMBATTICK] > 3)
    {
      ch->pcdata->powers[NINJA_COMBO] = 0;
      if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_KNIFESHIELD))
      {
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_KNIFESHIELD);
        send_to_char("Your knife barrier fades from lack of use.\n\r", ch);
      }
      if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLUR))
      {
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLUR);
        send_to_char("The shroud of shadows fade for lack of combat.\n\r", ch);
      }
      if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
      {
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
        send_to_char("Your bloodrage subsides.\n\r", ch);
      }
      if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM))
      {
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM);
        send_to_char("You regain your normal form.\n\r", ch);
      }
      ch->pcdata->powers[NINJA_COMBATTICK] = 0;
    }
  }
  else  // is fighting
  {
    ch->pcdata->powers[NINJA_COMBATTICK] = 0;
    if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLUR))
    {
      if (ch->pcdata->stats[NINJA_POWER] < 100)
      {
        send_to_char("Your blurring magic fades due to lack of shadow points.\n\r", ch);
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLUR);
      }
      else ch->pcdata->stats[NINJA_POWER] -= 100;
    }
    if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
    {
      if (ch->pcdata->stats[NINJA_POWER] < 200)
      {
        send_to_char("Your bloodrage fades due to lack of shadow points.\n\r", ch);
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
      }
      else ch->pcdata->stats[NINJA_POWER] -= 200;
    }
    if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM))
    {
      if (ch->pcdata->stats[NINJA_POWER] < 200)
      {
        send_to_char("Your shadowform fades due to lack of shadow points.\n\r", ch);
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM);
      }
      else ch->pcdata->stats[NINJA_POWER] -= 200;
    }
    if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_KNIFESHIELD))
    {
      if (ch->pcdata->stats[NINJA_POWER] < 50)
      {
        send_to_char("Your knifeshield fades due to lack of shadow points.\n\r", ch);
        REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_KNIFESHIELD);
      }
      else ch->pcdata->stats[NINJA_POWER] -= 50;
    }
  }
  if (ch->pcdata->powers[NINJA_SOULAMMO] > 0) ch->pcdata->powers[NINJA_SOULAMMO]--;

  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93460 && ch->in_room->vnum <= 93469))  
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch,2);
  regen_limb(ch);
  return;
}

void update_werewolf(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93430 && ch->in_room->vnum <= 93439))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (!IS_ITEMAFF(ch, ITEMA_RAGER))
  {
    if (ch->position == POS_FIGHTING)
    {
      if (ch->rage < 200) ch->rage += number_range(5,10);
      if (ch->rage < 200 && ch->pcdata->powers[DISC_WERE_WOLF] > 3)	ch->rage += number_range(5,10);
      if (!IS_SET(ch->special, SPC_WOLFMAN) && ch->rage >= 100)	do_werewolf(ch,"");
    }
    else if (ch->rage > 0)
    {
      ch->rage -= 1;
      if (ch->rage < 100) do_unwerewolf(ch,"");
    }
  }
  if (ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch,2);
	regen_limb(ch);
  return;
}

void update_demon(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93420 && ch->in_room->vnum <= 93426))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 2);
  }
  regen_limb(ch);
}

void update_demon_regen(CHAR_DATA *ch)
{
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move )
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_drow(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93440 && ch->in_room->vnum <= 93446))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  return;
}

void update_highlander(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93320 && ch->in_room->vnum <= 93326))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (get_eq_char(ch, WEAR_WIELD) != NULL)
  {
    int wpn = (ch->wpn[1]/3);
    if ( ch->hit < ch->max_hit)
    {
      ch->hit += wpn;
      if (ch->hit > ch->max_hit)
        ch->hit = ch->max_hit;
    }
    if ( ch->mana < ch->max_mana)
    {
      ch->mana += wpn;
      if (ch->mana > ch->max_mana)
        ch->mana = ch->max_mana;
    }
    if ( ch->move < ch->max_move)
    {
      ch->move += wpn;
      if (ch->move > ch->max_move)
        ch->move = ch->max_move;
    }
  }
  if (ch->fighting == NULL && ch->pcdata->powers[SAMURAI_FOCUS] > 0)
    ch->pcdata->powers[SAMURAI_FOCUS]--;
  if (ch->fighting == NULL && ch->pcdata->powers[SAMURAI_LAST] > 0)
    ch->pcdata->powers[SAMURAI_LAST] = 0;
  if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move )
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void update_mage(CHAR_DATA *ch)
{
  if ( ch->in_room != NULL && ( ch->in_room->vnum >= 93400 && ch->in_room->vnum <= 93406))
  {
    if ( ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
      werewolf_regen(ch, 1);
  }
  if (ch->position == POS_MEDITATING && ch->mana < ch->max_mana)
  {
    ch->mana += number_range(1000,2000);
    if (ch->mana > ch->max_mana)
      ch->mana = ch->max_mana;
  }
  return;
}

void update_arti_regen(CHAR_DATA *ch)
{
  if (ch->hit < ch->max_hit || ch->mana < ch->max_mana || ch->move < ch->max_move)
    werewolf_regen(ch, 2);
  regen_limb(ch);
  return;
}

void regen_limb(CHAR_DATA *ch)
{
  if (ch->loc_hp[6] > 0)
  {
    int sn = skill_lookup( "clot" );
    (*skill_table[sn].spell_fun) (sn,ch->level,ch,ch);
  }
  else if ((ch->loc_hp[0] + ch->loc_hp[1] + ch->loc_hp[2] + ch->loc_hp[3] + ch->loc_hp[4] + ch->loc_hp[5]) != 0)
    reg_mend(ch);
  return;
}

void werewolf_regen( CHAR_DATA *ch, int multiplier )
{
  bool pestilence = FALSE;
  bool gaia = FALSE;
  int min = 5;
  int max = 10;
  int hit_gain = 0;
  int mana_gain = 0;
  int move_gain = 0;

  if (IS_NPC(ch)) return;

  if (ch->in_room)
  {
    /*
     * first see if the pestilence is there
     */
    if (has_area_affect(ch->in_room->area, AREA_AFF_PESTILENCE, 0))
    {
      /*
       * then check if the player is of the same religion
       */
      if (has_area_affect(ch->in_room->area, AREA_AFF_PESTILENCE, ch->pcdata->religion))
        pestilence = FALSE;

      /*
       * if they aren't, then half the regen rate
       */
      else pestilence = TRUE;
    }

    /*
     * see if the gaia blessing is there
     */
    if (has_area_affect(ch->in_room->area, AREA_AFF_GAIA, ch->pcdata->religion))
    {
      gaia = TRUE;
    }
  }


  if (IS_CLASS(ch, CLASS_WEREWOLF) && (ch->position == POS_SLEEPING || ch->position == POS_FIGHTING))
  {
    if (ch->position == POS_SLEEPING && ch->power[DISC_WERE_BEAR] > 3)
    {
      min += 1200;
      max += 1700;
      hit_gain += number_range(min,max);
      mana_gain += number_range(min/2,max/2);
      move_gain += number_range(min,max);
    }
    else if (ch->position == POS_FIGHTING && ch->power[DISC_WERE_BEAR] > 3)
    {
      min += 275;
      max += 475;
      hit_gain += number_range(min,max);
      mana_gain += number_range(min/2,max/2);
      move_gain += number_range(min,max);
    }
  }
  else if (ch->position == POS_SLEEPING)
  {
    min += 300;
    max += 500;
    hit_gain += number_range(min,max);
    mana_gain += number_range(min,max);
    move_gain += number_range(min,max);
  }
  else
  {
    min += 100;
    max += 200;
    hit_gain += number_range(min,max);
    mana_gain += number_range(min,max);
    move_gain += number_range(min,max);
  }
  if (pestilence)
  {
    hit_gain /= 2;
    mana_gain /= 2;
    move_gain /= 2;
  }
  if (gaia)
  {
    hit_gain *= 1.5;
    mana_gain *= 1.5;
    move_gain *= 1.5;
  }
  ch->hit = UMIN (ch->hit+(hit_gain*multiplier), ch->max_hit);
  ch->mana = UMIN (ch->mana+(mana_gain*multiplier), ch->max_mana);
  ch->move = UMIN (ch->move+(move_gain*multiplier), ch->max_move);
  update_pos(ch);
  if ( ch->hit >= ch->max_hit && ch->mana >= ch->max_mana && ch->move >= ch->max_move )
    send_to_char("Your body has completely regenerated itself.\n\r",ch);
  return;
}

void update_active_counters(CHAR_DATA *ch)
{
  /* we do not update idle players */
  if (ch->timer >= 6) return;

  if (ch->pcdata->time_tick < 250) ch->pcdata->time_tick++;
  if (ch->pcdata->bully_counter > 0)
  {
    ch->pcdata->bully_counter--;
    if (ch->pcdata->bully_counter == 0)
    {
      send_to_char("Bully flag removed, now be nice.\n\r", ch);
      REMOVE_BIT(ch->pcdata->jflags, JFLAG_BULLY);
    }
  }
  if (ch->pcdata->bully_points > 0)
    ch->pcdata->bully_points--;
}
