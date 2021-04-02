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

void do_ninjaarmor( CHAR_DATA *ch, char *argument )
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 0;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Please specify which piece of ninja eq you wish to make: Ring Collar Robe Cap\n\r"
                 "Leggings Boots Gloves Sleeves Cloak Belt Bracer Mask Sword Dagger.\n\r",ch);
    return;
  }
  if (ch->practice < 60)
  {
    send_to_char("You don't have the 60 primal needed.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"mask"	)) vnum = 33093;
  else if (!str_cmp(arg,"dagger" )) vnum = 33080;
  else if (!str_cmp(arg,"sword"   )) vnum = 33081;
  else if (!str_cmp(arg,"ring"   )) vnum = 33082;
  else if (!str_cmp(arg,"collar"   )) vnum = 33083;
  else if (!str_cmp(arg,"robe"   )) vnum = 33085;
  else if (!str_cmp(arg,"cap"   )) vnum = 33086;
  else if (!str_cmp(arg,"leggings"   )) vnum = 33087;
  else if (!str_cmp(arg,"boots"   )) vnum = 33088;
  else if (!str_cmp(arg,"gloves"   )) vnum = 33091;
  else if (!str_cmp(arg,"sleeves"   )) vnum = 33089;
  else if (!str_cmp(arg,"cloak"   )) vnum = 33090;
  else if (!str_cmp(arg,"belt"   )) vnum = 33092;
  else if (!str_cmp(arg,"bracer"   )) vnum = 33084;
  else
  {
    do_ninjaarmor(ch,"");
    return;
  }
  if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
  {
    send_to_char("Missing object, please inform a God .\n\r",ch);
    return;
  }
  ch->practice -= 60;
  obj = create_object(pObjIndex, 50);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj->ownerid = ch->pcdata->playerid;
  obj_to_char(obj, ch);
  act("You make $p from the shadows.",ch,obj,NULL,TO_CHAR);
  act("$n forms $p from the shadows.",ch,obj,NULL,TO_ROOM);
  return;
}

void do_moonstrike(CHAR_DATA *ch, char *argument) 
{
  OBJ_DATA *obj;
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  int i;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_MOONSTRIKE))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_moonstrike, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1) &&
      IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_SOULREAPER_1))
  {
    act("You summon a #7**#0shadow#7**#n storm to shred the life from $N.", ch, NULL, victim, TO_CHAR);
    act("$n conjures forth a #7**#0shadow#7**#n vortex, and you are caught in it's shredding embrace.", ch, NULL, victim, TO_VICT);
    act("$n conjueres forth a #7**#0shadow#7**#n storm to shred $N.", ch, NULL, victim, TO_NOTVICT);
    for (i = 0; i < MAX_WEAR; i++)
    {
      if ((obj = get_eq_char(victim, i)) != NULL)
      {
        if (number_range(1, 7) == 2 && !IS_CLASS(victim, CLASS_FAE)) take_item(victim, obj);
      }
    }
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 24);
    return;
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_MOONSTRIKE_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_shadowthrust(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_SHADOWTHRUST))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_shadowthrust, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_CALTROPS_1) &&
      IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1))
  {
    if (!IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM))
    {
      act("You become one with the shadows, transforming your body into shadow matter.", ch, NULL, NULL, TO_CHAR);
      act("$n becomes one with the shadows, transforming $s body into shadow matter.", ch, NULL, NULL, TO_ROOM);
      SET_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM);
    }
    else
    {
      act("You reform your body, and leave the shadow existance.", ch, NULL, NULL, TO_CHAR);
      act("$n reforms $s body, returning from the shadows.", ch, NULL, NULL, TO_ROOM);
      REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM);
    }
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 12);
    return;
  }
  else if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1) &&
      IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_1) &&
      IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_2))
  {
    do_say(ch, "By the power of the shadows, you must die!");
    one_hit(ch, victim, gsn_shadowthrust, 1);
    one_hit(ch, victim, gsn_shadowthrust, 1);
    one_hit(ch, victim, gsn_shadowthrust, 1);
    one_hit(ch, victim, gsn_shadowthrust, 1);
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 12); 
    return;
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_SHADOWTHRUST_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_dirtthrow(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
     
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_DIRTTHROW))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_dirtthrow, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_CALTROPS_1))
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1);
    send_to_char("The shadows flicker.\n\r", ch);
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_gutcutter(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
 
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_GUTCUTTER))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }      
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_gutcutter, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_SHADOWTHRUST_1))
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_GUTCUTTER_1);
    send_to_char("The shadows form closer around you.\n\r", ch);
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_soulreaper(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_SOULREAPER))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_soulreaper, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_SHADOWTHRUST_1) &&
      IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_GUTCUTTER_1))
  {
    if (!IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLUR))
    {
      act("You shroud yourself in a blanket of shadows.", ch, NULL, NULL, TO_CHAR);
      act("$n shrouds $mself in a blanket of shadows.", ch, NULL, NULL, TO_ROOM);
      SET_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLUR);
    }
    else
      send_to_char("You are already blurred in shadows.\n\r", ch);
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 12);
    return;
  }
  else if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1))
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_SOULREAPER_1);
    send_to_char("The shadows form around you, ready to strike.\n\r", ch);
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_SOULREAPER_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_knifespin(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
 
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_KNIFESPIN))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_knifespin, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_CALTROPS_1))
  {
    if (!IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_KNIFESHIELD))
    {
      act("You throw the knife into the air and a barrier of knifes form around you.", ch, NULL, NULL, TO_CHAR);
      act("$n throws a knife into the air, and a barrier of knifes surround $m.", ch, NULL, NULL, TO_ROOM);
      SET_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_KNIFESHIELD);
    }
    else
      send_to_char("You already have a set of flying knifes around you.\n\r", ch);
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 6);
    return;
  }
  else if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_WAKASASHISLICE_1) &&
           IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_WAKASASHISLICE_2))
  {
    act("You throw back your head and howl in glee as you enter the bloodrage.", ch, NULL, NULL, TO_CHAR);
    act("$n throws back $s head and howl in glee as $e enters a bloodrage.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
    if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM))
    {
      REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_SHADOWFORM);
      send_to_char("You return from the shadows.\n\r", ch);
    }
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 12);
    return;
  }
  else if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1) &&
           IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_1))
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_2);
    send_to_char("The shadows grow more substantial.\n\r", ch);
  }
  else if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_DIRTTHROW_1))
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_1);
    send_to_char("The shadows grow more substantial.\n\r", ch);
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_wakasashislice(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
    
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_WAKASASHISLICE))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_wakasashi, 1);
  if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_KNIFESPIN_1))
  {
    do_say(ch, "Wakasashi!!");
    one_hit(ch, victim, gsn_wakasashi, 1);
    ch->pcdata->powers[NINJA_COMBO] = 0;
    WAIT_STATE(ch, 6); 
    return;
  }
  else if (IS_SET(ch->pcdata->powers[NINJA_COMBO], NCOMBO_WAKASASHISLICE_1))
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_WAKASASHISLICE_2);
    send_to_char("The shadows form around you, ready to strike.\n\r", ch);
  }
  else if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_WAKASASHISLICE_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_caltrops(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_CALTROPS))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
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
  if (is_safe(ch, victim)) return;
  if (IS_SET(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE))
  {
    send_to_char("Your bloodrage subsides.\n\r", ch);
    REMOVE_BIT(ch->pcdata->powers[NINJA_BITS], NPOWER_BLOODRAGE);
  }
  one_hit(ch, victim, gsn_caltrops, 1);
  if (ch->pcdata->powers[NINJA_COMBO] > 0)
  {
    ch->pcdata->powers[NINJA_COMBO] = 0;
    send_to_char("You feel the shadows subside.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->pcdata->powers[NINJA_COMBO], NCOMBO_CALTROPS_1);
    send_to_char("The shadows form around you.\n\r", ch);
  }
  WAIT_STATE(ch, 6);
  return;
}

void do_shadowlearn(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int i;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("                    #R{}#y***#R{} #0Shadow Powers #R{}#y***#R{}#n\n\r\n\r", ch);
    sprintf(buf, "              #yMartial Skill #0[#R");  // 140 mill exp for all
    for (i = 0; i < 25; i++)
    {
      if (ch->pcdata->powers[NINJA_MARTIAL] > i) strcat(buf, "*");
      else strcat(buf, " ");
    }
    strcat(buf, "#0]#n\n\r\n\r");
    send_to_char(buf, ch);
    send_to_char("   #RSilentwalk #0[#y10000#0]   #RVanish      #0[#y25000#0]   #RShadowwalk   #0[#y25000#0]#n\n\r", ch);
    send_to_char("   #RMienaku    #0[#y25000#0]   #RTruesight   #0[#y10000#0]   #RDemonic Pact #0[#y50000#0]#n\n\r", ch);
    send_to_char("   #RNight Pact #0[#y50000#0]   #RScry        #0[#y25000#0]   #RShield       #0[#y10000#0]#n\n\r", ch);
    send_to_char("   #RSoulseeker #0[#y25000#0]   #RAssassinate #0[#y25000#0]#n\n\r\n\r", ch);
    send_to_char("                 #0Ninja Attacks (500K exp to learn)#n\n\r\n\r", ch);
    send_to_char("      #RMoonstrike    Shadowthrust    Dirtthrow        Gutcutter#n\n\r", ch);
    send_to_char("      #RSoulreaper    Knifespin       Wakasashislice   Caltrops#n\n\r", ch);
    return;
  }
  if (!str_cmp(arg, "martial"))
  {
    if (ch->pcdata->powers[NINJA_MARTIAL] >= 25)
    {
      send_to_char("You are already a master of martial combat.\n\r", ch);
      return;
    }
    if (ch->practice < (ch->pcdata->powers[NINJA_MARTIAL] + 1) * 10)
    {
      send_to_char("You need 10 primal for each level.\n\r", ch);
      return;
    }
    ch->pcdata->powers[NINJA_MARTIAL]++;
    ch->practice -= ch->pcdata->powers[NINJA_MARTIAL] * 10;
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "silentwalk"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SILENTWALK))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 10000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 10000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SILENTWALK);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "vanish"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_HIDE))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 25000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 25000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_HIDE);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "shadowwalk"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SHADOWWALK))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 25000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 25000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SHADOWWALK);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "mienaku"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_MIENAKU))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 25000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 25000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_MIENAKU);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "truesight"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SIGHT))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 10000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 10000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SIGHT);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "demonic"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_TPACT))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 50000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 50000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_TPACT);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "night"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_DPACT))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 50000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 50000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_DPACT);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "scry"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SCRY))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 25000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 25000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SCRY);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "shield"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SHIELD))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 10000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 10000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SHIELD);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "soulseeker"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SOULSEEKERS))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 25000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 25000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SOULSEEKERS);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "assassinate"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_ASSASSINATE))
    {
      send_to_char("You already have that power.\n\r", ch);
      return;
    }
    if (ch->pcdata->stats[NINJA_POWER] < 25000)
    {
      send_to_char("You don't have enough class points to buy this power.\n\r", ch);
      return;
    }
    ch->pcdata->stats[NINJA_POWER] -= 25000;
    SET_BIT(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_ASSASSINATE);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "moonstrike"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_MOONSTRIKE))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_MOONSTRIKE);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "shadowthrust"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_SHADOWTHRUST))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_SHADOWTHRUST);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "dirtthrow"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_DIRTTHROW))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_DIRTTHROW);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "gutcutter"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_GUTCUTTER))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_GUTCUTTER);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "soulreaper"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_SOULREAPER))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_SOULREAPER);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "knifespin"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_KNIFESPIN))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_KNIFESPIN);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "wakasashislice"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_WAKASASHISLICE))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_WAKASASHISLICE);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "caltrops"))
  {
    if (IS_SET(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_CALTROPS))
    {
      send_to_char("You already know that attack style.\n\r", ch);
      return;
    }
    if (ch->exp < 500000)
    {
      send_to_char("You don't have 500.000 exp.\n\r", ch);
      return;
    }
    ch->exp -= 500000;
    SET_BIT(ch->pcdata->powers[NINJA_ATTACKS], NATTACK_CALTROPS);
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else do_shadowlearn(ch, "");
  return;
}

void do_soulseek(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *chroom;
  ROOM_INDEX_DATA *victimroom;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SOULSEEKERS))
  {
    send_to_char("You don't have the soulseeker power yet.\n\r", ch);
    return;
  }
  if (strlen(ch->pcdata->soultarget) < 3)
  {
    send_to_char("But your not targetting anyone.\n\r", ch);
    return;
  }
  if (ch->pcdata->powers[NINJA_SOULAMMO] < 1)
  {
    send_to_char("You have lost your lock on their soul.\n\r", ch);
    return;
  }
  if (ch->pcdata->stats[NINJA_POWER] < 500)
  {
    send_to_char("You don't have enough class points.\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, ch->pcdata->soultarget)) == NULL)
  {
    send_to_char("You cannot find them.\n\r", ch);
    return;
  }
  if ((victimroom = victim->in_room) == NULL)
  {
    send_to_char("They are hiding from you.\n\r", ch);
    return;
  }
  chroom = ch->in_room;
  char_from_room(ch);
  char_to_room(ch, victimroom);
  one_hit(ch, victim, gsn_soulseeker, 1);
  one_hit(ch, victim, gsn_soulseeker, 1);
  one_hit(ch, victim, gsn_soulseeker, 1);
  char_from_room(ch);
  char_to_room(ch, chroom);
  ch->pcdata->stats[NINJA_POWER] -= 500;
  WAIT_STATE(ch, 12);
}

void do_soultarget(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_SOULSEEKERS))
  {  
    send_to_char("You don't have the soulseeker power yet.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("NPC's have no soul to target.\n\r", ch);
    return;
  }
  free_string(ch->pcdata->soultarget);
  ch->pcdata->soultarget = str_dup(victim->name);
  ch->pcdata->powers[NINJA_SOULAMMO] = 5;
  send_to_char("You get a lock on their soul, they cannot escape your wrath now.\n\r", ch);
  send_to_char("You feel a dark presence in the very core of your being.\n\r", victim);
  WAIT_STATE(ch, 12);
}

void do_mienaku(CHAR_DATA *ch, char *argument)
{
  ROOM_INDEX_DATA *was_in;
  ROOM_INDEX_DATA *now_in;
  CHAR_DATA *victim;
  int attempt;

  WAIT_STATE(ch,4); /* to prevent spam fleeing */

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->powers[NINJA_SHADOW], NSHADOWS_MIENAKU))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (IS_EXTRA(ch, TIED_UP))
  {
    send_to_char("Not while tied up.\n\r",ch);
    return;
  }
  if ((victim = ch->fighting ) == NULL)
  {
    if (ch->position == POS_FIGHTING) ch->position = POS_STANDING;
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  if (ch->move < 1000)
  {
    send_to_char("You can't find the energy to leave.\n\r",ch);
    return;
  }
  if (is_webbed(ch)) return;
  ch->move -= 1000;
  was_in = ch->in_room;
  for (attempt = 0; attempt < 6; attempt++)
  {
    EXIT_DATA *pexit;
    int door;
    
    door = number_door( );
    if ((pexit = was_in->exit[door]) == 0 || !pexit->to_room || IS_SET(pexit->exit_info, EX_CLOSED)) 
      continue;
    move_char(ch, door);
    if ((now_in = ch->in_room) == was_in)
      continue;
    ch->in_room = was_in;
    act("$n escapes into the shadows!", ch, NULL, NULL, TO_ROOM );
    ch->in_room = now_in;
    if (!IS_NPC(ch))
      send_to_char("You escape into the shadows!\n\r", ch );
    stop_fighting(ch, TRUE);
    return;  
  }
  send_to_char("You were unable to escape!\n\r", ch);
  return;
}

void do_nfocus(CHAR_DATA *ch, char *argument)
{
  int sn;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_NINJA))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  sn = skill_lookup("ninja focus");
  if (sn  > 0) (*skill_table[sn].spell_fun) (sn, ch->pcdata->powers[NINJA_MARTIAL], ch, ch);
  WAIT_STATE(ch, 6);
}
