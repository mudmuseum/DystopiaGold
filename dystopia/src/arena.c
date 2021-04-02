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

#define ARENA_LVNUM     50 // lower vnum for the arena
#define ARENA_HVNUM     67 // upper vnum for the arena
#define ARENA_PLAYERS    8 // max players in the arena

int next_arena_room;

void do_arenastats(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int hp_stats = 0;
  int mana_stats = 0;
  int move_stats = 0;

  if (IS_NPC(ch)) return;
  if (ch->in_room != NULL)
  {
    if (IS_SET (ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("Your in the arena.\n\r",ch);
      return;
    }
  }
  send_to_char("#G            People in the arena#n\n\r\n\r",ch);
  send_to_char("#RName                Health   Stamina     Mana#n\n\r",ch);
  send_to_char("#0----------------------------------------------#n\n\r",ch);
  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    if (d->character != NULL)
    {
      if (d->character->in_room != NULL)
      {
        if (!IS_SET(d->character->in_room->room_flags, ROOM_ARENA)) continue;
        if (d->character->max_hit > 0)  hp_stats   = 100 * d->character->hit / d->character->max_hit;
        if (d->character->max_move > 0) move_stats = 100 * d->character->move / d->character->max_move;
        if (d->character->max_mana > 0) mana_stats = 100 * d->character->mana / d->character->max_mana;
        sprintf(buf,"%-15s    %3d/100   %3d/100   %3d/100\n\r", d->character->name, hp_stats, move_stats, mana_stats);
        send_to_char(buf,ch);
      }
    }
  }
  return;
}

void do_arenaset(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  char arg4[MIL];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );
  argument = one_argument( argument, arg4 );

  if (IS_NPC(ch)) return;
  if (arg1[0] == '\0')
  {
    send_to_char("Syntax : arenaset <player1> <player2> <player3> <player4>\n\r",ch);
    return;
  }
  sprintf(buf,"%s 50", arg1);
  do_transfer(ch, buf);
  do_restore(ch,arg1);
  if (arg2[0] != '\0')
  {
    sprintf(buf,"%s 58", arg2);
    do_transfer(ch, buf); 
    do_restore(ch,arg2);
  }
  if (arg3[0] != '\0')
  {    
    sprintf(buf,"%s 65", arg3);
    do_transfer(ch, buf);
    do_restore(ch,arg3);
  }
  if (arg4[0] != '\0')
  {    
    sprintf(buf,"%s 67", arg4);
    do_transfer(ch, buf);
    do_restore(ch,arg4);
  }
  sprintf(buf,"The fighters have entered the arena, the battle has begun");
  do_info(ch,buf);
  return;
}

void open_arena()
{
  arenaopen = TRUE;
  next_arena_room = ARENA_LVNUM; // first person to join will be put in this room.

  if (number_range(1,10) > 3)
  {
    arena_base = FALSE;
    do_info(NULL,"The Arena is now open for EVERYONE '#Larenajoin#n'");
  }
  else
  {
    do_info(NULL,"The Arena is now open for #CVeterans#n and lower rank '#Larenajoin#n'");
    arena_base = TRUE;
  }
  return;
}

void close_arena()
{
  CHAR_DATA *gch;
  CHAR_DATA *vch;
  int arenaplayers = 0;

  arenaopen = FALSE;

  for (vch=char_list;vch!=NULL;vch=vch->next)
  {
    if (IS_NPC(vch)) continue;
    if (vch->in_room != NULL)
    {
      if (IS_SET (vch->in_room->room_flags, ROOM_ARENA))
      {
        REMOVE_BIT(vch->act, PLR_FREEZE);
        gch = vch;
        arenaplayers++;
      }
    }
  }
  /* if there was only one player, remove him */
  if (arenaplayers <= 1)
  {
    if (arenaplayers) do_transfer(gch,"self 3054");
    do_info(NULL, "The Arena fight was cancelled due to lack of players!");
    return;
  }
  else do_info(NULL, "The arena is now closed, let the games begin!");
  return;
}

void do_arenajoin(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int arenapeople=0;
  DESCRIPTOR_DATA *d;

  if (IS_NPC(ch)) return;
  if (ch->fight_timer > 0)
  {
    send_to_char("You have a fighttimer.\n\r",ch);
    return;
  }
  if (!arenaopen)
  {
    send_to_char("The arena is closed.\n\r",ch);
    return;
  }
  if (arena_base && getMight(ch) > 750)
  {
    send_to_char("Your a bit to big for this arena.\n\r", ch);
    return;
  }
  if (ch->in_room != NULL)
  {
    if (IS_SET (ch->in_room->room_flags, ROOM_ARENA) || IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
    {
      send_to_char("This room is not connected to the astral sphere.\n\r",ch);
      return;
    }
  }
  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    if ( d->character != NULL )
    {
      if (!d->connected == CON_PLAYING) continue;
      if (d->character->in_room != NULL)
      {
        if (IS_SET (d->character->in_room->room_flags, ROOM_ARENA)) arenapeople++;
      }
    }
  }
  if (arenapeople > ARENA_PLAYERS)
  {
    send_to_char("The arena is crowded atm.\n\r",ch);
    return;
  }
  char_from_room(ch);
  char_to_room(ch, get_room_index(next_arena_room));
  next_arena_room += (ARENA_HVNUM - ARENA_LVNUM) / ARENA_PLAYERS;
  sprintf(buf,"%s has joined the arena!",ch->name);
  do_info(ch,buf);
  do_restore(ch,"self");
  strip_aggression(ch);
  SET_BIT(ch->act, PLR_FREEZE);
  return;
}

void do_resign(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  CHAR_DATA *gch;
  int found = 0;
  ROOM_INDEX_DATA *location;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  if (ch->in_room != NULL)
  {
    if (!IS_SET(ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("Your not in the arena.\n\r",ch);
      return;
    }
  }
  sprintf(buf,"%s resigns from the arena",ch->name);
  do_info(ch,buf);
  if ((location = get_room_index(ROOM_VNUM_ALTAR)) == NULL) return;
  char_from_room(ch);
  char_to_room(ch, location);
  ch->fight_timer = 0;
  do_restore(ch, "self");
  do_call(ch, "all");
  ch->pcdata->alosses++;
  for (victim = char_list; victim != NULL; victim = victim->next)
  {
    if (IS_NPC(victim)) continue;
    if (victim->in_room != NULL && IS_SET(victim->in_room->room_flags, ROOM_ARENA) && victim->pcdata->chobj == NULL)
    {
      gch = victim;
      found++;
    }
  }
  if (found == 1)
  {
    sprintf(buf,"#C%s #oemerges victorious from the #Rarena#n",gch->name);
    gch->pcdata->awins++;
    do_info(gch,buf);
    if ((location = get_room_index(ROOM_VNUM_ALTAR)) == NULL) return;
    char_from_room(gch);
    char_to_room(gch, location);
    gch->fight_timer = 0;
    do_restore(gch, "self");
    win_prize(gch);
  }
  return;
}

/*
 * The challenge system, uses specific vnums of rooms, so don't remove those.
 */

void do_challenge(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  
  argument = one_argument(argument, arg);
  one_argument(argument, arg2);

  if (IS_NPC(ch)) return;
  if (arg[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Syntax : Challenge <person> <death/spar>\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_CHALLENGE))
  {
    send_to_char("You can only use challenge once every 2 mud hours.\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("You can't challenge monsters.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("Ehm, no.\n\r", ch);
    return;
  }
  if (victim->level != 3)
  {
    send_to_char("You can only challenge avatars.\n\r", ch);
    return;
  }
  if (victim == ch->challenger)
  {
    send_to_char("You have already challenged them.\n\r", ch);
    return;
  }
  if (!str_cmp(arg2, "death")) ch->deathmatch = TRUE;
  else if (!str_cmp(arg2, "spar")) ch->deathmatch = FALSE;
  else
  {
    do_challenge(ch, "");
    return;
  }

  if (ch->deathmatch)
  {
    bool canDecap, canKillYou;

    if (!str_cmp(ch->pcdata->last_decap[0], victim->name)) canDecap = FALSE;
    else canDecap = fair_fight(ch, victim);
    if (!str_cmp(victim->pcdata->last_decap[0], ch->name)) canKillYou = FALSE;
    else canKillYou = fair_fight(victim, ch);
  
    if (!canKillYou || !canDecap)
    {
      send_to_char("You cannot challenge someone in a deathmatch if you cannot kill eachother.\n\r", ch);
      return;
    }
  }

  ch->challenger = victim;
  send_to_char("You challenge them.\n\r", ch);
  SET_TIMER(ch, TIMER_CHALLENGE, 2);
  if (ch->deathmatch) sprintf(buf, "You have been challenged to a #Gdeathmatch#n by %s. Type agree %s to start the fight.\n\r", ch->name, ch->name);
  else sprintf(buf, "You have been challenged to a #Gspar#n by %s. Type agree %s to start the fight.\n\r", ch->name, ch->name);
  send_to_char(buf, victim);
  WAIT_STATE(ch, 8);
  return;
}

void do_decline(CHAR_DATA *ch, char *argument)
{    
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (arg[0] == '\0')
  {
    send_to_char("Decline whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("You can't decline a monster, since it can't challenge you.\n\r", ch);
    return;
  }
  if (victim->challenger != ch)
  {
    send_to_char("They aren't challenging you. (they may have cancelled the challenge)\n\r", ch);
    return;
  }
  victim->challenger = NULL;
  send_to_char("You decline their challenge.\n\r", ch);
  send_to_char("Your challenge has been declined.\n\r", victim);
  return;
}

void do_accept2(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  bool canDecap, canKillYou;
  ROOM_INDEX_DATA *location;

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!arena2)
  {
    send_to_char("The Forbidden Fortress is currently being used by someone else.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Accept who's challenge?\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("You cannot accept a challenge from a monster.\n\r", ch);
    return;
  }
  if (victim->challenger != ch)
  {
    send_to_char("You haven't been challenged by them.\n\r", ch);
    return;
  }
  if (victim->level != 3)
  {
    send_to_char("They have to be avatar.\n\r", ch);
    return;
  }
  if (victim->fight_timer > 0)
  {
    send_to_char("They have a fighttimer currently, you'll have to wait.\n\r", ch);
    return;
  }
  if (ch->fight_timer > 0)
  {
    send_to_char("Not with a fight timer.\n\r", ch);
    return;
  }
  if (IS_SET(victim->extra,EXTRA_AFK))
  {
    send_to_char("They are AFK!\n\r", ch);
    return;
  }
  if (victim->position != POS_STANDING)
  {
    send_to_char("They are not standing, you'll have to wait.\n\r", ch);
    return;
  }
  if (ch->in_room != NULL)
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_ARENA) || IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
    {
      send_to_char("This room is not connected to the astral sphere.\n\r",ch);
      return;
    }
  }  
  if (victim->in_room != NULL)
  {
    if (IS_SET (victim->in_room->room_flags, ROOM_ARENA) || IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
    {
      send_to_char("Their room is not connected to the astral sphere.\n\r",ch);
      return;
    }
  }

  if (victim->deathmatch)
  {
    if (!str_cmp(ch->pcdata->last_decap[0], victim->name)) canDecap = FALSE;
    else canDecap = fair_fight(ch, victim);
    if (!str_cmp(victim->pcdata->last_decap[0], ch->name)) canKillYou = FALSE;
    else canKillYou = fair_fight(victim, ch);

    if (!canKillYou || !canDecap)
    {
      send_to_char("You cannot accept a deathmatch if you cannot kill eachother.\n\r", ch);
      return;
    }
  }

  if ((location = get_room_index(ROOM_VNUM_FORTRESS1)) == NULL)
  {
    bug("Fortress Missing.", 0);
    return;
  }
  char_from_room(ch);
  char_to_room(ch, location);

  if ((location = get_room_index(ROOM_VNUM_FORTRESS2)) == NULL)
  {
    bug("Fortress Missing.", 0);
    return;
  }
  char_from_room(victim);
  char_to_room(victim, location);

  do_restore(victim, victim->name);
  do_restore(ch, ch->name);

  if (!victim->deathmatch)
    sprintf(buf, "%s and %s enter #CThe Forbidden Fortress#n to test their skills", ch->name, victim->name);
  else
    sprintf(buf, "%s and %s enter #CThe Forbidden Fortress#n to duel for their lives", ch->name, victim->name);
  do_info(ch, buf);

  strip_aggression(ch);
  strip_aggression(victim);

  arena2 = FALSE;
  if (victim->deathmatch) arena2death = TRUE;
  else arena2death = FALSE;
  victim->challenger = NULL;
  return;
}

void do_fortressstats(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int hp_stats = 0;
  int mana_stats = 0;
  int move_stats = 0;

  if (IS_NPC(ch)) return;
  if (in_fortress(ch))
  {
    send_to_char("Hehe.\n\r", ch);
    return;
  }
  send_to_char("#G            Forbidden Fortress#n\n\r\n\r",ch);
  send_to_char("#RName                Health   Stamina     Mana#n\n\r",ch);
  send_to_char("#0----------------------------------------------#n\n\r",ch);
  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    if (d->character != NULL)
    {
      if (d->character->in_room != NULL)
      {
        if (!in_fortress(d->character)) continue;
        if (d->character->max_hit > 0)  hp_stats   = 100 * d->character->hit / d->character->max_hit;
        if (d->character->max_move > 0) move_stats = 100 * d->character->move / d->character->max_move;
        if (d->character->max_mana > 0) mana_stats = 100 * d->character->mana / d->character->max_mana;
        sprintf(buf,"%-15s    %3d/100   %3d/100   %3d/100\n\r", d->character->name, hp_stats, move_stats, mana_stats);
        send_to_char(buf,ch);
      }
    }
  }
  return;
}

void do_bet(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;
  CHAR_DATA *vch;
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int amount, max_bet = 1000;

  argument = one_argument(argument, arg);
  one_argument(argument, arg2);

  if (IS_NPC(ch)) return;

  if (in_fortress(ch))
  {
    send_to_char("You cannot bet on this fight.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Syntax : bet <person> <amount>\n\r", ch);
    return;
  }
  if ((amount = atoi(arg2)) < 100)
  {
    send_to_char("You must bet at least 100 qps.\n\r", ch);
    return;
  }
  if ((gch = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("Bet on whom?\n\r", ch);
    return;
  }
  if (IS_NPC(gch))
  {
    send_to_char("It is not possible to bet on mobiles.\n\r", ch);
    return;
  }
  if (!in_fortress(gch) && !tourney)
  {
    send_to_char("They are not in the fortress.\n\r", ch);
    return;
  }
  if (!in_arena(gch) && tourney)
  {
    send_to_char("They are not in the arena.\n\r", ch);
    return;
  }
  if (!arena2death && !tourney)
  {
    send_to_char("It is not a deathmatch, you can only bet on deathmatches.\n\r", ch);
    return;
  }
  if (ch->pcdata->betting_char > 0)
  {
    send_to_char("But you already have a wager on this fight.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < amount)
  {
    send_to_char("But you don't have that many questpoints.\n\r", ch);
    return;
  }
  if (amount > max_bet)
  {
    sprintf(buf, "You can only bet %d questpoints, that is the maximum.\n\r", max_bet);
    send_to_char(buf, ch);
    return;
  }
  for (vch = char_list; vch; vch = vch->next)
  {
    if (IS_NPC(vch)) continue;
    if (!in_fortress(vch)) continue;
    if (vch->hit < vch->max_hit * 0.75)
    {
      send_to_char("Not all players have more than 3/4th of their hps left.\n\r", ch);
      return;
    }
  }
  ch->pcdata->quest -= amount;
  ch->pcdata->betting_amount = amount;
  ch->pcdata->betting_char = gch->pcdata->playerid;
  sprintf(buf, "%s places %d qps on %s as the winner of the deathmatch.",
    ch->name, amount, gch->name);
  do_info(ch, buf);
  return;
}

void update_bets(CHAR_DATA *ch, CHAR_DATA *victim, bool payoff)
{
  CHAR_DATA *gch;
  char buf[MAX_STRING_LENGTH];

  for (gch = char_list; gch; gch = gch->next)
  {
    if (IS_NPC(gch)) continue;
    if (payoff && gch->pcdata->betting_char == ch->pcdata->playerid)
    {
      sprintf(buf, "Congratulations, you win %d quest points.\n\r", gch->pcdata->betting_amount*2);
      send_to_char(buf, gch);
      gch->pcdata->quest += gch->pcdata->betting_amount*2;
      sprintf(buf, "ARENA: %s wins %d qps for betting on %s.",
        gch->name, gch->pcdata->betting_amount*2, ch->name);
      log_string(buf);
    }
    else if (!payoff && gch->pcdata->betting_char == ch->pcdata->playerid)
    {
      sprintf(buf, "Since the fight ended without a kill, your money is returned.\n\r");
      send_to_char(buf, gch);
      gch->pcdata->quest += gch->pcdata->betting_amount;
    }
    gch->pcdata->betting_amount = 0;
    gch->pcdata->betting_char = 0;
  }
}
