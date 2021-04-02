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

void do_spiderarmor( CHAR_DATA *ch, char *argument )
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 0;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Please specify which piece of spider armor you wish to make: whip ring\n\r"
                 "collar armor helmet leggings boots gloves sleeves cloak belt bracer mask.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"whip"        )) vnum = 33140;
  else if (!str_cmp(arg,"ring"     )) vnum = 33141;
  else if (!str_cmp(arg,"collar"   )) vnum = 33142;
  else if (!str_cmp(arg,"armor"    )) vnum = 33143;
  else if (!str_cmp(arg,"helmet"   )) vnum = 33144;
  else if (!str_cmp(arg,"leggings" )) vnum = 33145;
  else if (!str_cmp(arg,"boots"    )) vnum = 33146;
  else if (!str_cmp(arg,"gloves"   )) vnum = 33147;
  else if (!str_cmp(arg,"sleeves"  )) vnum = 33148;
  else if (!str_cmp(arg,"cloak"   )) vnum = 33149;
  else if (!str_cmp(arg,"belt"     )) vnum = 33150;
  else if (!str_cmp(arg,"bracer"    )) vnum = 33151;
  else if (!str_cmp(arg,"mask"      )) vnum = 33152;
  else
  {
    do_spiderarmor(ch,"");
    return;
  }
  if ( ch->practice < 150)
  {
    send_to_char("It costs 150 points of primal to create a piece of spider armour.\n\r",ch);
    return;
  }
  if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
  {
    send_to_char("Missing object, please inform an immortal.\n\r",ch);
    return;
  }
  ch->practice   -= 150;
  obj = create_object(pObjIndex, 50);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj->ownerid = ch->pcdata->playerid;
  obj_to_char(obj, ch);
  act("$p appears in your hands in a blast of flames.",ch,obj,NULL,TO_CHAR);
  act("$p appears in $n's hands in a blast of flames.",ch,obj,NULL,TO_ROOM);
  return;
}

void do_radarscan( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *chroom;
  ROOM_INDEX_DATA *victimroom;
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_RADAR))
  {
    send_to_char("You don't have the radar implant.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Scan for whom?\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim->in_room)
  {
    if (has_area_affect(victim->in_room->area, AREA_AFF_BADMOON, 0))
    {
      send_to_char("Some powerful magic prevent your scrying attempt.\n\r", ch);
      return;
    }
  }
  chroom = ch->in_room;
  victimroom = victim->in_room;
  char_from_room(ch);
  char_to_room(ch,victimroom);
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_AFFECTED(victim, AFF_SHADOWPLANE)))
  {
    REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    do_look(ch,"scry");
    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
  }
  else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && (IS_AFFECTED(victim, AFF_SHADOWPLANE)))
  {
    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
    do_look(ch,"scry");
    REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
  }
  else
    do_look(ch,"scry");
  char_from_room(ch);
  char_to_room(ch,chroom);
  if (!IS_NPC(victim)) ch->fight_timer += 5;
  return;
}

void do_battlestation(CHAR_DATA *ch, char *argument)
{
  char buf [MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_BATTLESTATION))
  {
    send_to_char("You don't have the battlestation implant.\n\r", ch);
    return;
  }
  if (IS_SET(ch->newbits, NEW_CUBEFORM))
  {
    if (ch->fighting)
    {
      send_to_char("Not while fighting.\n\r", ch);
      return;
    }
    REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    act("$n reverts to spiderform.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You turn into your normal form.\n\r",ch);
    ch->damroll -= 250;
    ch->hitroll -= 250;
    free_string(ch->morph);
    ch->morph=str_dup("");
    return;
  }
  else
  {
    act( "$n transforms into a powerful looking battlestation.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You change into a powerful battlestation.\n\r",ch);
    SET_BIT(ch->newbits, NEW_CUBEFORM);  
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf,"%s the battlestation", ch->name); 
    free_string(ch->morph);
    ch->morph = str_dup(buf);
    ch->damroll += 250;
    ch->hitroll += 250;
    if (ch->fighting) WAIT_STATE(ch, 12);
  }
  return;
}

void do_reload(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("What do you wish to reload : ammo or rockets ?\n\r", ch);
    return;
  }
  if (!str_cmp(arg, "ammo"))
  {
    if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_SUBMACHINEGUN))
    {
      send_to_char("You don't have a submachine gun.\n\r", ch);
      return;
    }
    if ((!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_ARMORY) && ch->pcdata->powers[DROID_AMMO] >= 2)
      || ch->pcdata->powers[DROID_AMMO] >= 4)
    {
      send_to_char("You are already fully reloaded.\n\r", ch);
      return;
    }
    send_to_char("*Click*\n\r", ch);
    WAIT_STATE(ch, 8);
    ch->pcdata->powers[DROID_AMMO]++;
    return;
  }
  else if (!str_cmp(arg, "rockets"))
  {
    if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_ROCKETLAUNCHER))
    {
      send_to_char("You don't have a rocket launcher.\n\r", ch);
      return;
    }
    if ((!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_ARMORY) && ch->pcdata->powers[DROID_ROCKET] >= 1)
      || ch->pcdata->powers[DROID_ROCKET] >= 2)
    {
      send_to_char("You are already fully reloaded.\n\r", ch);
      return;
    }
    send_to_char("*Ka-Chunk*\n\r", ch);
    WAIT_STATE(ch, 8); 
    ch->pcdata->powers[DROID_ROCKET]++;
    return;
  }
  else
  {
    do_reload(ch, "");
    return;
  }
  return;
}

void do_implant(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int cost, power;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {  
    send_to_char("Huh?\n\r", ch );
    return;
  }
  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("                 #p{{{#0-#p}}} #0Spiderdroid Implants #p{{{#0-#p}}}#n\n\r\n\r", ch);
    send_to_char(" #PEyes         #0[#p 25000#0]  #PExtra Arms      #0[#p100000#0]  #PRadar         #0[#p100000#0]#n\n\r", ch);
    send_to_char(" #PMachinegun   #0[#p200000#0]  #PRocket Launcher #0[#p150000#0]  #PBody Armor    #0[#p100000#0]#n\n\r", ch);
    send_to_char(" #PArmwires     #0[#p200000#0]  #P360 Vision      #0[#p 50000#0]  #PArmory        #0[#p200000#0]#n\n\r", ch);
    send_to_char(" #PSteam Engine #0[#p150000#0]  #PBreath Cannon   #0[#p100000#0]  #PBattlestation #0[#p400000#0]#n\n\r", ch);
    send_to_char(" #PShadow Gate  #0[#p100000#0]  #PTuned Engine    #0[#p100000#0]  #PRegrowth      #0[#p 25000#0]#n\n\r", ch);
    send_to_char(" #PLarvafeet    #0[#p200000#0]#n\n\r", ch);
    return;
  }
  if (!str_cmp(arg, "eyes"))
  {
    cost = 25000;
    power = IMPLANT_EYES;
  }
  else if (!str_cmp(arg, "extra") || !str_cmp(arg, "arms"))
  {
    cost = 100000;
    power = IMPLANT_EXTRA_ARMS;
  }
  else if (!str_cmp(arg, "radar"))
  {
    cost = 100000;
    power = IMPLANT_RADAR;
  }
  else if (!str_cmp(arg, "machinegun"))
  {
    cost = 200000;
    power = IMPLANT_SUBMACHINEGUN;
  }
  else if (!str_cmp(arg, "rocket") || !str_cmp(arg, "launcher"))
  {
    cost = 150000;
    power = IMPLANT_ROCKETLAUNCHER;
  }
  else if (!str_cmp(arg, "body") || !str_cmp(arg, "armor"))
  {
    cost = 100000;
    power = IMPLANT_BODYARMOR;
  }
  else if (!str_cmp(arg, "armwires"))
  {
    cost = 200000;
    power = IMPLANT_ARMWIRE;
  }
  else if (!str_cmp(arg, "360") || !str_cmp(arg, "vision"))
  {
    cost = 50000;
    power = IMPLANT_360VISION;
  }
  else if (!str_cmp(arg, "armory"))
  {
    cost = 200000;
    power = IMPLANT_ARMORY;
  }
  else if (!str_cmp(arg, "steam"))
  {
    cost = 150000;
    power = IMPLANT_STEAMENGINE;
  }
  else if (!str_cmp(arg, "breath") || !str_cmp(arg, "cannon"))
  {
    cost = 100000;
    power = IMPLANT_BREATHCANNON;
  }
  else if (!str_cmp(arg, "battlestation"))
  {
    cost = 400000;
    power = IMPLANT_BATTLESTATION;
  }
  else if (!str_cmp(arg, "larvafeet"))
  {
    cost = 200000;
    power = IMPLANT_LARVAFEET;
  }
  else if (!str_cmp(arg, "shadow") || !str_cmp(arg, "gate"))
  {
    cost = 100000;
    power = IMPLANT_SHADOWGATE;
  }
  else if (!str_cmp(arg, "tuned"))
  {
    cost = 100000;
    power = IMPLANT_TUNEDENGINE;
  }
  else if (!str_cmp(arg, "regrowth"))
  {
    cost = 25000;
    power = IMPLANT_REGROWTH;
  }
  else
  {
    do_implant(ch, "");
    return;
  }
  if (IS_SET(ch->pcdata->powers[DROID_IMPLANTS], power))
  {
    send_to_char("You already have that implant.\n\r", ch);
    return;
  }
  if (ch->pcdata->stats[DROID_POWER] < cost)
  {
    send_to_char("You do not have enough droid points to buy this implant.\n\r", ch);
    return;
  }
  if (ch->exp < 500 * cost)
  {
    send_to_char("You do not have enough exp to buy this implant.\n\r", ch);
    return;
  }
  ch->exp -= cost * 500;
  ch->pcdata->stats[DROID_POWER] -= cost;
  send_to_char("You gain a new implant.\n\r", ch);
  SET_BIT(ch->pcdata->powers[DROID_IMPLANTS], power);
  return;
}

void do_droidspeed(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {  
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->position != POS_FIGHTING)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (ch->pcdata->powers[DROID_SPEED] >= 6)
  {
    send_to_char("But you are already moving at max speed.\n\r", ch);
    return;
  }
  send_to_char("You feel a rush of power as your fighting speed picks up.\n\r", ch);
  ch->pcdata->powers[DROID_SPEED]++;
  WAIT_STATE(ch, 8);
  return;
}

void do_steam(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  int cost = 500;

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_BREATHCANNON))
  {
    send_to_char("You don't have that implant.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    if ((victim = ch->fighting) == NULL)
    {
      send_to_char("But you are not fighting anyone.\n\r", ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (ch->move < cost)
  {
    send_to_char("you don't have enough move.\n\r", ch);
    return;
  }
  if (is_safe(ch, victim)) return;
  one_hit(ch, victim, gsn_steam, 1);
  one_hit(ch, victim, gsn_steam, 1);
  if (!IS_AFFECTED(victim, AFF_FLAMING))
  {
    act("$n catches on fire.", victim, NULL, NULL, TO_ROOM);
    SET_BIT(victim->affected_by, AFF_FLAMING);
  }
  ch->move -= cost;
  WAIT_STATE(ch, 12);
  return;
}

void do_submachine(CHAR_DATA *ch, char *argument)
{  
  CHAR_DATA *victim;  
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_SUBMACHINEGUN))
  {
    send_to_char("You don't have that implant.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    if ((victim = ch->fighting) == NULL)
    {
      send_to_char("But you are not fighting anyone.\n\r", ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (ch->pcdata->powers[DROID_AMMO] <= 0)
  {
    send_to_char("*Click* You are out of ammo.\n\r", ch);
    do_say(ch, "Damn, out of ammo...");
    return;
  }
  if (is_safe(ch, victim)) return;
  ch->pcdata->powers[DROID_AMMO]--;
  one_hit(ch, victim, gsn_machinegun, 1);
  one_hit(ch, victim, gsn_machinegun, 1);
  one_hit(ch, victim, gsn_machinegun, 1);
  if (number_range(1, 5) == 3)
  {
    send_to_char("You are blessed by Lloth and fire of another volley.\n\r", ch);
    one_hit(ch, victim, gsn_machinegun, 1);
    one_hit(ch, victim, gsn_machinegun, 1);
  }
  act("$n reels back in shock.", victim, NULL, NULL, TO_ROOM);
  WAIT_STATE(victim, 12);
  WAIT_STATE(ch, 12);
  return;
}

void do_rocketblast(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;  
  CHAR_DATA *ich;
  CHAR_DATA *ich_next;
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DROID))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (!IS_SET(ch->pcdata->powers[DROID_IMPLANTS], IMPLANT_ROCKETLAUNCHER))
  {
    send_to_char("You don't have that implant.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    if ((victim = ch->fighting) == NULL)
    {
      send_to_char("But you are not fighting anyone.\n\r", ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (ch->pcdata->powers[DROID_ROCKET] <= 0)
  {
    send_to_char("You are out of rockets.\n\r", ch);
    return;
  }
  if (is_safe(ch, victim)) return;
  ch->pcdata->powers[DROID_ROCKET]--;
  one_hit(ch, victim, gsn_rocket, 1);
  one_hit(ch, victim, gsn_rocket, 1);
  ich_next = ch->in_room->people;
  ich = ich_next;
  while (ich_next != NULL)
  {
    ich_next = ich->next_in_room;
    if (ich != ch)
    {
      if (is_safe(ch,ich)) break;
      one_hit(ch, ich, gsn_rocket, 1);
    }
    ich = ich_next;
  }
  WAIT_STATE(ch, 12);
  return;
}
