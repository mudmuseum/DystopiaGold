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

void giantattack    args ((CHAR_DATA *ch, int attacktype));

void do_giantarmor(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 0;

  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Please specify which piece of giant armor you wish to make: Branch Bracer\n\r"
                 "Collar Ring Plate Helmet Leggings Boots Gloves Sleeves Cloak Belt Visor.\n\r",ch);
    return;
  }
  if ( ch->practice < 60 )
  {
    send_to_char("It costs 60 points of primal to create giant equipment.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"ring")) vnum = 33280;
  else if (!str_cmp(arg,"bracer")) vnum = 33281;
  else if (!str_cmp(arg,"collar")) vnum = 33282;
  else if (!str_cmp(arg,"plate")) vnum = 33283;
  else if (!str_cmp(arg,"helmet")) vnum = 33284;
  else if (!str_cmp(arg,"belt")) vnum = 33285;
  else if (!str_cmp(arg,"visor")) vnum = 33286;
  else if (!str_cmp(arg,"gloves")) vnum = 33287;
  else if (!str_cmp(arg,"sleeves")) vnum = 33288;
  else if (!str_cmp(arg,"boots")) vnum = 33289;
  else if (!str_cmp(arg,"leggings")) vnum = 33290;
  else if (!str_cmp(arg,"cloak")) vnum = 33291;
  else if (!str_cmp(arg,"branch")) vnum = 33292;
  else
  {
    do_giantarmor(ch,"");
    return;
  }
  if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
  {
    send_to_char("Missing object, please inform a God.\n\r",ch);
    return;
  }
  obj = create_object(pObjIndex, 50);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj->ownerid = ch->pcdata->playerid;
  obj_to_char(obj, ch);
  ch->practice -= 60;
  act("$p appears in your hands.",ch,obj,NULL,TO_CHAR);
  act("$p appears in $n's hands.",ch,obj,NULL,TO_ROOM);
  return;
}

void do_ggrow( CHAR_DATA *ch, char *argument )
{
  char buf[MAX_STRING_LENGTH];
  int cost = 12000000;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->rank >= FOOT_30)
  {
    send_to_char("Your quite a tall giant already.\n\r",ch);
    return;
  }
  if (ch->exp < cost * (ch->pcdata->rank+1))
  {
    sprintf(buf,"You need %d more exp to grow.\n\r", cost * (ch->pcdata->rank+1) - ch->exp);
    send_to_char(buf,ch);
    return;
  }
  ch->pcdata->rank++;
  ch->exp -= ch->pcdata->rank * cost;
  send_to_char("You grow and grow, you can almost touch the stars.\n\r",ch);
  return;
}

void do_thwack(CHAR_DATA *ch, char *argument)
{
  giantattack(ch,1);
  return;
}

void do_smack(CHAR_DATA *ch, char *argument)
{
  giantattack(ch,2);
  return;
}

void do_bash(CHAR_DATA *ch, char *argument)
{
  giantattack(ch,3);
  return;
}

void giantattack(CHAR_DATA *ch, int attacktype)
{
  CHAR_DATA *victim;
     
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->move < 1000)
  {
    send_to_char("You're not up to it, you ain't got the stamina in ya.\n\r",ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  ch->move -= 1000;
  if (attacktype == ch->pcdata->powers[GIANT_ATTACK])
  {
    do_say(ch,"Oi Oi, I smell a tiny humaaaan, thwack, smack, bash you skull, humaaan.");
    one_hit(ch,victim,gsn_thwack,1);
    one_hit(ch,victim,gsn_smack,1);
    one_hit(ch,victim,gsn_bash,1);
  }
  else
  {
    if (attacktype == 1) one_hit(ch,victim,gsn_thwack,1);
    if (attacktype == 2) one_hit(ch,victim,gsn_smack,1);
    if (attacktype == 3) one_hit(ch,victim,gsn_bash,1);
  }
  WAIT_STATE(ch,12);
  return;  
}  

void do_dawnstrength(CHAR_DATA *ch, char *argument)
{
  char buf [MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->pcdata->rank == 0)
  {
    send_to_char("You're too short, piss off.\n\r",ch);
    return;
  }
  if (IS_SET(ch->newbits, NEW_CUBEFORM))
  {
    REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    act( "$n lets the strenght of old fade away.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You let the strength of old fade away.\n\r",ch);
    ch->damroll -= ch->pcdata->rank * 100;
    ch->hitroll -= ch->pcdata->rank * 100;
    free_string(ch->morph);
    ch->morph=str_dup("");
    return;
  }
  else
  {
    act( "$n grows to enormous heights.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You call upon the strength of old to guide you..\n\r",ch);
    SET_BIT(ch->newbits, NEW_CUBEFORM);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf,"%s the giant",ch->name);
    free_string(ch->morph);
    ch->morph = str_dup(buf);
    ch->damroll += ch->pcdata->rank * 100;
    ch->hitroll += ch->pcdata->rank * 100;
    return;
  }
  return;
}

void do_giantgift(CHAR_DATA *ch, char *argument)
{
  char buf [MAX_STRING_LENGTH];
  int cost = 100000;
  int max_gifts = 6;

  if (IS_NPC(ch)) return;  
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[GGIFTS_GAINED] >= max_gifts)
  {
    send_to_char("You already have all giant gifts.\n\r",ch);
    return;
  }
  if (ch->pcdata->stats[GIANT_POINTS] < cost)
  {
    sprintf(buf,"You still need %d class points for the next gift.\n\r", cost - ch->pcdata->stats[GIANT_POINTS]);
    send_to_char(buf,ch);
    return;
  }
  ch->pcdata->stats[GIANT_POINTS] -= cost;
  if (ch->pcdata->powers[GGIFTS_GAINED] < 1)
  {
    SET_BIT(ch->pcdata->powers[GIANT_GIFTS], GGIFT_LEATHERSKIN);
    send_to_char("You are gifted with leather skin, preventing damage taken.\n\r",ch);
  }
  else if (ch->pcdata->powers[GGIFTS_GAINED] < 2)
  {
    SET_BIT(ch->pcdata->powers[GIANT_GIFTS], GGIFT_STONESHAPE);
    send_to_char("You are gifted with the ability to shape stone onto tools of war.\n\r",ch);
  }
  else if (ch->pcdata->powers[GGIFTS_GAINED] < 3)
  {
    SET_BIT(ch->pcdata->powers[GIANT_GIFTS], GGIFT_REVIVAL);
    send_to_char("You are gifted the power of revival, the healing of nature.\n\r",ch);
  }
  else if (ch->pcdata->powers[GGIFTS_GAINED] < 4)
  {
    SET_BIT(ch->pcdata->powers[GIANT_GIFTS], GGIFT_EARTHPUNCH);
    send_to_char("You are granted the power to strike the earth with a mighty punch, shaking everyone in the room.\n\r",ch);
  }
  else if (ch->pcdata->powers[GGIFTS_GAINED] < 5)
  {
    SET_BIT(ch->pcdata->powers[GIANT_GIFTS], GGIFT_STANDFIRM);
    send_to_char("You are granted the power to stand firm, thus taking less damage in combat.\n\r",ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[GIANT_GIFTS], GGIFT_LONGLEGS);
    send_to_char("You are granted the gift of long legs, thus being able to step in front of a fleeing opponent. (auto)\n\r",ch);
  }
  ch->pcdata->powers[GGIFTS_GAINED]++;
  return;
}

void do_gsweep(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *location;

  if (IS_NPC(ch)) return;
  if (IS_CLASS(ch, CLASS_MONK))
  {
    do_sweep(ch, argument);
    return;
  }
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You're not fighting anyone you can sweep.\n\r",ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobiles.\n\r", ch);
    return;
  }
  do_say(ch,"Oi Oi, Im gonna crush you little skull, Humaaan.");
  one_hit(ch, victim, gsn_backfist, 0);
  one_hit(ch, victim, gsn_headbutt, 0);
  special_hurl(ch,victim);
  if (victim->in_room == NULL || ch->in_room == NULL) return;
  if (ch->in_room != victim->in_room)
  {
    stop_fighting(ch, TRUE);
    stop_fighting(victim, TRUE);
    act("$n walks after $N.", ch, victim, NULL, TO_ROOM);
    send_to_char("You follow the tiny humaaan.\n\r",ch);
    send_to_char("The Giant walks after you.....\n\r",victim);
    location = victim->in_room;
    char_from_room(ch);
    char_to_room(ch, location);
    do_look(ch,"auto");
  }
  victim->position = POS_STANDING;
  one_hit(ch, victim, gsn_bash, 1);
  WAIT_STATE(ch,24);
  return;
}

void do_revival(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))  
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[GIANT_GIFTS], GGIFT_REVIVAL))
  {
    send_to_char("You don't have that gift.\n\r",ch);
    return;
  }
  ch->hit += number_range(300,600);
  if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
  send_to_char("You feel revived.\n\r",ch);
  WAIT_STATE(ch,12);
  return;
}

void do_earthpunch(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))  
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[GIANT_GIFTS], GGIFT_EARTHPUNCH))
  { 
    send_to_char("You don't have that gift.\n\r",ch);
    return;
  }
  send_to_char("You slam your fist into the ground creating a minor earthquake.\n\r",ch);
  act("$n slams $m fist into the ground, creating a minor earthquake.", ch, NULL, NULL, TO_ROOM);
  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if (IS_NPC(gch)) continue;
    if (ch == gch) continue;
    if (is_safe(ch,gch)) continue;
    if (gch->fighting != NULL)
    {
      one_hit(ch,gch,gsn_bash,0);
      send_to_char("You lose your balance.\n\r",gch);
      do_stance(gch,"");
    }
    else
    {
      send_to_char("You drop to the ground, stunned.\n\r",gch);
      stop_fighting(gch, TRUE);
      gch->position = POS_STUNNED;
    }
  }
  WAIT_STATE(ch,24);
  return;
}

void do_standfirm(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[GIANT_GIFTS], GGIFT_STANDFIRM))
  {
    send_to_char("You don't have that gift.\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[GIANT_STANDFIRM] == 0)
  {
    send_to_char("You slam your feet hard into the ground, and refuse to budge.\n\r",ch);
    ch->pcdata->powers[GIANT_STANDFIRM] = 1;
  }
  else
  {
    send_to_char("You relax from your standfirm.\n\r",ch);
    ch->pcdata->powers[GIANT_STANDFIRM] = 0;
  }
  return;
}

void do_stoneshape(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int cost = 50;

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[GIANT_GIFTS], GGIFT_STONESHAPE))
  {
    send_to_char("You don't have that gift.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Stoneshape what?\n\r",ch);
    return;
  }
  if ((obj = get_obj_carry(ch,arg)) == NULL)
  {
    send_to_char("You dont have that item.\n\r",ch);
    return;
  }
  if ((obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR)
    || obj->chobj != NULL || IS_SET(obj->quest, QUEST_GIANTSTONE))
  {
    send_to_char("You are unable to stoneshape this item.\n\r",ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf, "It costs %d quest points to use this power, and you don't have that much.\n\r", cost);
    send_to_char(buf, ch);
    return;
  }
  if (obj->item_type == ITEM_WEAPON)
  {
    obj->value[1] += number_range(1,3);
    obj->value[2] += number_range(3,5);
  }
  forge_affect(obj, number_range(1,5));
  SET_BIT(obj->quest, QUEST_GIANTSTONE);
  ch->pcdata->quest -= cost;
  send_to_char("You shape the ancient stone of the world onto the item, making it more poweful.\n\r",ch);
  return;
}

void do_deathfrenzy(CHAR_DATA *ch, char *argument)
{  
  CHAR_DATA *victim;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_GIANT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->hit > ch->max_hit * 0.25)
  {
    send_to_char("Your still in good health.\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[GIANT_FRENZY] == 1)
  {
    send_to_char("Your already raging.\n\r",ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("But, your not fighting anyone, who in the world do you plan on getting pissed at?\n\r",ch);
    return;
  }
  send_to_char("You enter your deathfrenzy.\n\r",ch);
  ch->pcdata->powers[GIANT_FRENZY] = 1;
  do_say(ch,"You Humaaans think you so smart, me bash little humaaan skull, you not so smart no more!");
  multi_hit(ch,victim, TYPE_UNDEFINED);
  return;
}
