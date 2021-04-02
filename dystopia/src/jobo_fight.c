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
#include "merc.h"

void do_pkcall(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;
  CHAR_DATA *victim;

  if (IS_NPC(ch)) return;  
  if (get_ratio(ch) < 3000)
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("Your not engaged in combat.\n\r",ch);
    return;
  }
  do_say(ch,"TO ARMS!!! TO ARMS!!!!");
  if (IS_NPC(victim))
  {
    send_to_char("No effect.\n\r", ch);
    return;
  }
  for (gch=ch->in_room->people;gch!=NULL;gch = gch->next_in_room)
  {
    if (!IS_NPC(gch)) continue;
    if (gch->master) continue; // not on charmies
    if (gch == ch->fighting) continue;
    if (gch->fighting != NULL) continue;
    if (gch->level > get_ratio(ch)/2 || gch->level > 2500)
    {
      do_say(gch,"Fight your own fights you pansy.\n\r");
      continue;
    }
    /*
     * If the player was fighting a mob, it could actually be dead.
     */
    if (ch->fighting == NULL) return;
    if (number_range(1,100) > 33)
    {
      do_say(gch,"To arms brothers, tonight we will feast in the halls of midgaard");
      multi_hit(gch,ch->fighting,TYPE_UNDEFINED);
    }
  }
  WAIT_STATE(ch,12);
  return;
}

void do_pkmind(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (get_ratio(ch) < 4000)
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (IS_SET(ch->newbits, NEW_IRONMIND))
  {
    send_to_char("Your already focusing your mental powers.\n\r",ch);
    return;
  }
  SET_BIT(ch->newbits, NEW_IRONMIND);
  send_to_char("You focus your concentration on absorbing damage.\n\r",ch);
  WAIT_STATE(ch, 6);
  return;
}

/* Genstealing is in :-) */
  
void do_gensteal(CHAR_DATA *ch, char *argument)   
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];

  one_argument( argument, arg);
  if (IS_NPC(ch)) return;
  if (ch->in_room != NULL)
  {
    if (IS_SET (ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("Your in the arena.\n\r",ch);
      return;
    }
  }
  if (ragnarok)
  {
    send_to_char("Not while ragnarok is running.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Gensteal whom?\n\r",ch);
    return;
  }
  if ((victim=get_char_room(ch,arg))==NULL)
  {
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  if (ch == victim)
  {
    send_to_char("How stupid are you?!?\n\r",ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on the mobs.\n\r",ch);
    return;
  }
  if (ch->class != victim->class)
  {
    send_to_char("Only on people of the same class as yourself.\n\r",ch);
    return;
  }
  if (getMight(ch) < 150)
  {
    send_to_char("You need to be a cadet or better.\n\r", ch);
    return;
  }
  if (getMight(victim) < 150)
  {
    send_to_char("You can only gensteal Cadets or better.\n\r", ch);
    return;
  }
  if (victim->position > 1)
  {
    send_to_char("He resists your attempt to gensteal.\n\r",ch);
    return;
  }
  if (ch->generation < victim->generation)
  {
    send_to_char("Sorry, you are already a higher generation than they are.\n\r",ch);
    return;
  }
  if (victim->generation > 7)
  {
    send_to_char("They are worthless.\n\r",ch);
    return;
  }
  if (ch->generation == 1)
  {
    send_to_char("You are the highest possible generation.\n\r",ch);
    return;
  }
  if (is_safe(ch,victim)) return;
  if (in_fortress(ch) && arena2death)
  {
    arena2 = TRUE;
    if ((location = get_room_index(ROOM_VNUM_VICTORY)) == NULL) return;
    char_from_room(victim);
    char_to_room(victim, location);
    char_from_room(ch);
    char_to_room(ch, location);
    update_bets(ch, victim, FALSE);
  }
  else if (in_fortress(ch))
  {
    send_to_char("Your in The Forbidden Fortress.\n\r", ch);
    return;
  }
  act("Lightning flows through your body as you steal the powers of $N.\n\r", ch, NULL, victim, TO_CHAR);
  act("$n puts his hands on $N's head and lightning covers his body.\n\r", ch, NULL, victim, TO_NOTVICT);
  send_to_char("Your generation has been stolen.\n\r",victim);
  sprintf(buf, "#G%s #ohas beaten #L%s #oin combat, and has stolen their generation!#n",ch->pcdata->switchname,
    victim->pcdata->switchname);
  do_info(ch,buf);
  ch->generation -= 1;
  victim->generation += 1;
  ch->fight_timer +=5;
  victim->hit = 1;
  update_pos(victim);
  WAIT_STATE(ch, 12);
  if ((location = get_room_index(ROOM_VNUM_ALTAR)) == NULL)
    return;
  char_from_room(victim);  
  char_to_room(victim, location);
  victim->fight_timer = 0;
  players_gstolen++;
  sprintf(buf, "%s was genstolen by %s", victim->name, ch->name);
  log_string(buf);
  return;
}

void do_combatswitch( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  
  argument = one_argument(argument,arg);
  if (IS_NPC(ch)) return;
  
  if (ch->fighting == NULL)
  {
    send_to_char("Ah, but your not really fighting anyone!\n\r",ch);
    return;
  }
  if ((victim=get_char_room(ch,arg))==NULL)
  {
    send_to_char("They aren't here.\n\r",ch);
    return;
  }
  if (ch == victim)
  {
    send_to_char("How stupid are you?!?\n\r",ch);
    return;
  }
  if (ch->fighting == victim)
  {    
    send_to_char("Maybe you should just continue fighting them.\n\r",ch);
    return;
  }
  if (victim->fighting == NULL || victim->fighting != ch)
  {
    send_to_char("You failed.\n\r",ch);
    return;
  }
  if (number_range(1,3) == 1)
  {      
    send_to_char("You failed.\n\r",ch);
    WAIT_STATE(ch, 12);
    return;
  }
  ch->fighting = victim;
  send_to_char("Hehe, bet they didn't expect that to happen...\n\r",ch);
  WAIT_STATE(ch,6);
  return;
}

void do_artisteal(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int i;

  if (IS_NPC(ch)) return;

  one_argument(argument, arg);
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("That's a mobile.\n\r", ch);
    return;
  }
  if (victim->position != POS_MORTAL)
  {
    send_to_char("They need to be mortally wounded.", ch);
    return;
  }
  if (!IS_SET(victim->pcdata->tempflag, TEMP_ARTIFACT))
  {
    send_to_char("They do not have an artifact.\n\r", ch);
    return;
  }
  for (i = 0; i < MAX_WEAR; i++)
  {
    if ((obj = get_eq_char(victim, i)) != NULL)
    {
      if (IS_SET(obj->quest, QUEST_ARTIFACT))
      {
        obj_from_char(obj);
        obj_to_char(obj, ch);
        send_to_char("You steal the artifact from them.\n\r", ch); 
        act("$n steals $p from $N.", ch, obj, victim, TO_ROOM);
        return;
      }
    }
  }
  for (obj = victim->carrying; obj; obj = obj->next_content)
  {
    if (IS_SET(obj->quest, QUEST_ARTIFACT))
    {
      obj_from_char(obj);
      obj_to_char(obj, ch);
      send_to_char("You steal the artifact from them.\n\r", ch);
      act("$n steals $p from $N.", ch, obj, victim, TO_ROOM);
      return;
    }
  }
  send_to_char("Malfunction - please report this.\n\r", ch);
  return;
}

void ragnarokdecap(CHAR_DATA *ch, CHAR_DATA *victim)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_CLASS(victim, CLASS_VAMPIRE))           do_mortalvamp(victim,"");
  else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
  char_from_room(victim);
  char_to_room(victim,get_room_index(ROOM_VNUM_ALTAR));
  do_clearstats(victim,"");
  victim->level = 2;
  dropinvis(ch);
  players_decap++;  
  sprintf(buf,"%s was beheaded by %s, the ragnarok continues",victim->name, ch->name);     
  do_info(ch,buf);
  send_to_char("YOU HAVE BEEN KILLED!!!!\n\r",victim);
  do_call(victim, "all");
  do_train(victim,"avatar");
  do_restore(victim,"self");
  return;
}

void fortresskill(CHAR_DATA *ch, CHAR_DATA *victim)
{
  char buf[MAX_STRING_LENGTH];
  ROOM_INDEX_DATA *location;

  if (IS_NPC(ch) || IS_NPC(victim))
  {
    bug("Fortress failure", 0);
    return;
  }

  /*
   * Some class stuff.
   */
  if (IS_CLASS(victim, CLASS_VAMPIRE)) do_mortalvamp(victim,"");
  else if (IS_SET(victim->special, SPC_WOLFMAN)) do_unwerewolf(victim,"");
  if (!IS_CLASS(victim, CLASS_NINJA)) victim->rage = 0;

  /*
   * Remove mage powers
   */
  if (IS_ITEMAFF(victim, ITEMA_STEELSHIELD)) REMOVE_BIT(ch->itemaffect, ITEMA_STEELSHIELD);
  if (IS_ITEMAFF(victim, ITEMA_BEAST)) REMOVE_BIT(victim->itemaffect, ITEMA_BEAST);
  if (IS_ITEMAFF(victim, ITEMA_MAGESHIELD)) REMOVE_BIT(victim->itemaffect, ITEMA_MAGESHIELD);
  if (IS_ITEMAFF(victim, ITEMA_ILLUSIONS)) REMOVE_BIT(victim->itemaffect, ITEMA_ILLUSIONS);
  if (IS_ITEMAFF(victim, ITEMA_DEFLECTOR)) REMOVE_BIT(victim->itemaffect, ITEMA_DEFLECTOR);

  /*
   * cleanup
   */
  ch->fight_timer = 0;
  victim->fight_timer = 0;
  powerdown(victim);
  free_string(victim->morph);
  victim->morph = str_dup("");
  do_restore(ch, ch->name);
  do_restore(ch, victim->name);
  REMOVE_BIT(victim->extra, TIED_UP);
  REMOVE_BIT(victim->extra, GAGGED);
  REMOVE_BIT(victim->extra, BLINDFOLDED);

  /*
   * Tally the score
   */
  ch->pcdata->awins++;
  victim->pcdata->alosses++;  

  /*
   * Information
   */
  sprintf(buf, "%s was beaten in The Forbidden Fortress by %s.", victim->name, ch->name);
  do_info(ch, buf);
   
  /*
   * Out they go.
   */
  if ((location = get_room_index(ROOM_VNUM_ALTAR)) == NULL) return;
  char_from_room(victim);
  char_to_room(victim, location);
  char_from_room(ch);
  char_to_room(ch, location);
    
  do_call(ch, "all");  
  do_call(victim, "all");
  do_clearstats(victim, "");
  victim->level = 2;

  /*
   * Open the Fortress
   */
  arena2 = TRUE;
  return;   
}

