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

/*
 * Only used in alternative reality, giving hr/dr bonus (alternate power)
 */
void do_visage(CHAR_DATA *ch, char *argument)
{
  int sn;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_DISTORTED_REALITY] < 1 || ch->pcdata->powers[DRONE_MINDCONTROL] < 1)
  {
    send_to_char("You need level 1 in distorted realities and mindcontrol.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_DRONEWORLD))
  {
    send_to_char("You haven't shifted reality yet, you cannot grow a visage here.\n\r", ch);
    return;
  }
  if (ch->mana < 2000)
  {
    send_to_char("You don't have 2000 mana.\n\r", ch);
    return;
  }
  sn = skill_lookup("visage");
  if (sn  > 0) (*skill_table[sn].spell_fun) (sn, 50, ch, ch);
  WAIT_STATE(ch, 6);
  ch->mana -= 2000;
  return;
}


/*
 * Only used in alternative reality, attack
 */
void do_memorytwist(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int i;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_ALTERNATE_REALITY] < 3 || ch->pcdata->powers[DRONE_WILLPOWER] < 1)
  {
    send_to_char("You need level 3 alternative realities and level 1 willpower.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_DRONEWORLD))
  {
    send_to_char("You haven't shifted reality yet, you cannot twist the memory of anyone in this reality.\n\r", ch);
    return;
  }
  if (ch->move < 1000)
  {
    send_to_char("You don't have 1000 move.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }
  ch->move -= 1000;
  for (i = number_range(0, 2); i < 3; i++)
    one_hit(ch, victim, gsn_telekinetic, 0);
  WAIT_STATE(ch, 12);
  return;
}

void do_nightmare(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_HORRID_REALITY] < 3
   || ch->pcdata->powers[DRONE_ALTERNATE_REALITY] < 3
   || ch->pcdata->powers[DRONE_DISTORTED_REALITY] < 3)
  {
    send_to_char("You haven't mastered all aspects of reality.\n\r", ch);
    return;
  }
  if (ch->pcdata->powers[DRONE_NIGHTMARE] == 1)
  {
    ch->hitroll -= 250;
    ch->damroll -= 250;
    send_to_char("You quench your nightmarish aura.\n\r",ch);
    ch->pcdata->powers[DRONE_NIGHTMARE] = 0;
  }
  else
  {
    ch->hitroll += 250;
    ch->damroll += 250;
    send_to_char("You unleash your nighmarish aura.\n\r",ch);
    ch->pcdata->powers[DRONE_NIGHTMARE] = 1;
  }
  return;
}

void do_horrid(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  argument = one_argument(argument, arg1);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_HORRID_REALITY] < 3)
  {
    send_to_char("You don't have the power to create a horrid reality yet.\n\r",ch);
    return;
  }
  if (ch->mana < 2000)
  {
    send_to_char("You don't have the 2000 mana required.\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg1)) == NULL)
  {
    if (ch->fighting != NULL) victim = ch->fighting;
    else
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
  }
  if (ch == victim)
  {
    send_to_char("Your really don't want that.\n\r", ch);
    return;
  }
  if (is_safe(ch, victim)) return;
  if (IS_NPC(victim))
  {
    send_to_char("It lacks the mental abilities to understand a change in reality.\n\r", ch);
    return;
  }
  if (IS_SET(victim->pcdata->jflags, JFLAG_DRONE_HORRID))
  {
    send_to_char("They are still deep in their nightmare.\n\r", ch);
    return;
  }
  act("You weave a horrid reality and let $N be the center of it.", ch, NULL, victim, TO_CHAR);
  act("$n sucks you into a horrid reality, consisting of everything you have ever feared.", ch, NULL, victim, TO_VICT);
  SET_BIT(victim->pcdata->jflags, JFLAG_DRONE_HORRID);
  if (!victim->fighting && victim->position == POS_STANDING)
  {
    victim->position = POS_FIGHTING;
    victim->fighting = ch;
  }
  ch->mana -= 2000;
  WAIT_STATE(ch, 12);
}

void do_distort(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_DISTORTED_REALITY] < 3)
  {
    send_to_char("You don't have the power to distort reality yet.\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_DRONE_DISTORT))
  {
    send_to_char("Your have already distorted reality to suit your needs.\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_DRONE_DISTORT))
  {
    send_to_char("Sorry, you can only use this power once every 12 hours.\n\r", ch);
    return;
  }
  if (ch->mana < 3000)
  {
    send_to_char("You don't have the 3000 mana required.\n\r", ch);
    return;
  }
  ch->mana -= 3000;
  SET_TIMER(ch, TIMER_DRONE_DISTORT, 12);
  SET_BIT(ch->pcdata->jflags, JFLAG_DRONE_DISTORT);
  act("$n weaves reality into something distorted and perverted.", ch, NULL, NULL, TO_ROOM);
  act("You weave reality to suit your needs, creating a pocket dimension where you are a god.", ch, NULL, NULL, TO_CHAR);
  return;
}

void do_mindseye(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg1);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_PSIONICS] < 2)
  {
    send_to_char("You don't have acces to that psionic.\n\r",ch);
    return;
  }
  if (arg1[0] == '\0')
  {
    send_to_char(" #G<<<#0- - - - --------========#y{ #0Drone Mindseye #y}#0========-------- - - - -#G>>>#n\n\r", ch);
    send_to_char("    Mindseye allows a drone to force players that looks at him or her\n\r", ch);
    send_to_char("    to execute a special command, chosen by the Drone itself.\n\r\n\r", ch);
    send_to_char("    'Readaura' gives an unblockable readable of the person looking.\n\r", ch);
    send_to_char("    'Level'    gives an insight into the lookers skills.\n\r", ch);
    send_to_char("    'Custom'   allows for a custom forced command of the drones choice.\n\r", ch);
    send_to_char("    'Off'      turns off the power.\n\r\n\r", ch);
    send_to_char("    Syntax : mindseye <custom/readaura/off> <custom command>\n\r", ch);
    send_to_char(" #G<<<#0- - - - --------========#y{ #0Drone Mindseye #y}#0========-------- - - - -#G>>>#n\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "custom"))
  {
    if (strlen(argument) < 3 || strlen(argument) > 80)
    {
      send_to_char("Between 3 and 80 chars please.\n\r", ch);
      return;
    }
    free_string(ch->poweraction);
    ch->poweraction = str_dup(argument);
    ch->pcdata->powers[DRONE_EYE] = DRONE_EYE_CUSTOM;
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg1, "level"))
  {
    ch->pcdata->powers[DRONE_EYE] = DRONE_EYE_LEVEL;
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg1, "readaura"))
  {
    ch->pcdata->powers[DRONE_EYE] = DRONE_EYE_READAURA;
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg1, "off"))
  {
    ch->pcdata->powers[DRONE_EYE] = DRONE_EYE_OFF;
    send_to_char("Ok.\n\r", ch);
    return;
  }
  else do_mindseye(ch, "");
  return;
}

void do_psionicburst(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_PSIONICS] < 1)
  {
    send_to_char("You don't have acces to that psionic.\n\r",ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    if (ch->fighting != NULL) victim = ch->fighting;
    else
    {
      send_to_char("They aren't here?\n\r", ch );
      return;
    }
  }
  if (ch == victim)
  {
    send_to_char("Your really don't want that.\n\r", ch);
    return;
  }
  if (victim->position != POS_STANDING)
  {
    send_to_char("Your blast can only affect awake and relaxed minds.\n\r", ch);
    return;
  }
  if (is_safe(ch, victim)) return;
  do_say(ch, "#yLook into my eyes and relax, you feel your eyelids growing heavy.#n");
  do_say(ch, "#yNow repeat after me - I'm sleepy#n");
  do_say(victim, "#GI'm sleepy#n");
  do_say(ch, "#yI think I'll take a nap#n");
  do_say(victim, "#GI think I'll take a nap#n");
  do_say(ch, "#yzzZZzz#n");
  do_say(victim, "#GzzZZzz#n");
  victim->position = POS_SLEEPING;
  WAIT_STATE(ch, 12);
  WAIT_STATE(victim, 12);
  return;
}

void do_realityshift(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  CHAR_DATA *gch;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_ALTERNATE_REALITY] < 3)
  {
    send_to_char("You haven't mastered the alternate realities yet.\n\r",ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r",ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Nay, you cannot create a new reality for mobs.\n\r", ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_DRONEWORLD))
  {
    send_to_char("Your already in an alternate reality.\n\r", ch);
    return;
  }
  if (ch->mana < 5000)
  {
    send_to_char("You don't have the 5000 mana required.\n\r", ch);
    return;
  }
  for (gch = ch->in_room->people; gch; gch = gch->next)
  {
    if (gch == ch || gch == victim) continue;
    if (gch->fighting == ch)
    {
      gch->fighting = NULL;
      gch->position = POS_STANDING;
    }
  }
  ch->mana -= 5000;
  SET_BIT(ch->pcdata->jflags, JFLAG_DRONEWORLD);
  SET_BIT(victim->pcdata->jflags, JFLAG_DRONEWORLD);
  act("$n creates an alternate reality, sucking $N with $m into it.", ch, NULL, victim, TO_NOTVICT);
  act("You create an alternate reality, sucking $N with you into it.", ch, NULL, victim, TO_CHAR);
  act("$n creates an alternate reality, sucking you with $m into it.", ch, NULL, victim, TO_VICT);
  WAIT_STATE(ch, 12);
  return;
}

void do_mindwarp(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument (argument, arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->pcdata->powers[DRONE_PSIONICS] < 3)
  {
    send_to_char("You haven't learned that psionic yet.\n\r",ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    if (ch->fighting != NULL) victim = ch->fighting;
    else
    {
      send_to_char("They aren't here?\n\r", ch );
      return;
    }
  }
  if (ch == victim)
  {
    send_to_char("Your really don't want that.\n\r", ch);
    return;
  }
  if (is_safe(ch, victim)) return;
  if (ch->fighting && ch->fighting != victim)
  {
    send_to_char("You cannot concentrate on them, your already fighting someone else.\n\r",ch);
    return;
  }
  if (ch->mana < 500)
  {
    send_to_char("You don't have the 500 mana required.\n\r", ch);
    return;
  }
  if (number_range(1,3) == 2)
  {
    act("$n unleashed a powerful psionic attack at $N, but $N resists the attack.", ch, NULL, victim, TO_NOTVICT);
    act("$N resists your psionic attack.", ch, NULL, victim, TO_CHAR);
    act("You feel a sudden intenese pain in your head, but you force it away with the power of your will.",ch,NULL,victim,TO_VICT);
    WAIT_STATE(ch, 24);
    return;
  }
  ch->mana -= 500;
  act("$N get's a distant look in $S eyes and $n uses the chance to land a devastating attack.", ch, NULL, victim, TO_NOTVICT);
  act("You unleash your most potent psionic attack, paralyzing $N.", ch, NULL, victim, TO_CHAR);
  act("You abuses the pause to land a devastating attack.", ch, NULL, victim, TO_CHAR);
  act("You suddenly feel yourself unable to move, your mind being held in $n's irongrip.", ch, NULL, victim, TO_VICT);
  multi_hit(ch, victim, TYPE_UNDEFINED);
  WAIT_STATE(ch, 18);
  return;
}

void do_dronearmor(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 0;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Please specify which piece of drone armor you wish to make: Tentacle Ring\n\r"
                 "Collar Wristbands Plate Helmet Leggings Boots Gauntlets Paddings Cape Girdle Visor.\n\r",ch);
    return;
  }
  if ( ch->practice < 150 )
  {
    send_to_char("It costs 150 points of primal to create drone equipment.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"tentacle")) vnum = 33300;
  else if (!str_cmp(arg,"ring")) vnum = 33301;
  else if (!str_cmp(arg,"collar")) vnum = 33302;
  else if (!str_cmp(arg,"wristbands")) vnum = 33303;
  else if (!str_cmp(arg,"plate")) vnum = 33304;
  else if (!str_cmp(arg,"helmet")) vnum = 33305;
  else if (!str_cmp(arg,"leggings")) vnum = 33306;
  else if (!str_cmp(arg,"boots")) vnum = 33307;
  else if (!str_cmp(arg,"gauntlets")) vnum = 33308;
  else if (!str_cmp(arg,"paddings")) vnum = 33309;
  else if (!str_cmp(arg,"cape")) vnum = 33310;
  else if (!str_cmp(arg,"girdle")) vnum = 33311;
  else if (!str_cmp(arg,"visor")) vnum = 33312;
  else
  {
    do_dronearmor(ch,"");
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
  ch->practice -= 150;
  act("$p appears in your hands.",ch,obj,NULL,TO_CHAR);
  act("$p appears in $n's hands.",ch,obj,NULL,TO_ROOM);
  return;  
}

void do_abstract(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int cost = 20000000;
  int i;

  argument = one_argument(argument, arg1);
  one_argument(argument, arg2);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (arg1[0] == '\0')
  {
    sprintf(buf, " #G- - - - - - - - --------========#y{ #0Drone Realities #y}#G========-------- - - - - - - - -#n\n\r");
    send_to_char(buf, ch);
    send_to_char("\n\r", ch);
    sprintf(buf, "  #PHorrid Realities #R[");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_HORRID_REALITY] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n    #PDistorted Realities #R[");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_DISTORTED_REALITY] > i) strcat(buf, "#0*");
      else strcat(buf, " "); 
    }
    strcat(buf, "#R]#n    #PAlternate Realities #R[");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_ALTERNATE_REALITY] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n\n\r");
    send_to_char(buf, ch);
    send_to_char("\n\r", ch);

    sprintf(buf, " #G- - - - - - - - --------========#y{ #0Drone Psychics  #y}#G========-------- - - - - - - - -#n\n\r");
    send_to_char(buf, ch);
    send_to_char("\n\r", ch);
    sprintf(buf, "  #PWillpower#R        [");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_WILLPOWER] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n    #PMindcontrol         #R[");
    for (i = 0; i < 3; i++)  
    {
      if (ch->pcdata->powers[DRONE_MINDCONTROL] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n    #PPsionics            #R[");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_PSIONICS] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n\n\r");
    send_to_char(buf, ch); 
    send_to_char("\n\r", ch);

    sprintf(buf, " #G- - - - - - - - --------========#y{ #0Drone Combat    #y}#G========-------- - - - - - - - -#n\n\r");   
    send_to_char(buf, ch);
    send_to_char("\n\r", ch);
    sprintf(buf, "  #PEvasion#R          [");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_EVASION] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n    #PFortitude           #R[");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_FORTITUDE] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n    #PFocus               #R[");
    for (i = 0; i < 3; i++)
    {
      if (ch->pcdata->powers[DRONE_FOCUS] > i) strcat(buf, "#0*");
      else strcat(buf, " ");
    }
    strcat(buf, "#R]#n\n\r");
    send_to_char(buf, ch);
    send_to_char("\n\r", ch);

    sprintf(buf, " #G- - - - - - - - --------===================================-------- - - - - - - - -#n\n\r");
    send_to_char(buf, ch);
    send_to_char("                   #PSyntax #0:#G abstract #G<#0reality#G/#0psyche#G/#0combat#G> #G<#0type#G>#n\n\r", ch);
    send_to_char(buf, ch);
  }
  else if (!str_cmp(arg1, "reality"))
  {
    if (!str_cmp(arg2, "horrid"))
    {
      if (ch->pcdata->powers[DRONE_HORRID_REALITY] >= 3)
      {
        send_to_char("You have mastered this reality already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_HORRID_REALITY] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_HORRID_REALITY] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_HORRID_REALITY]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_HORRID_REALITY];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg2, "distorted"))
    {
      if (ch->pcdata->powers[DRONE_DISTORTED_REALITY] >= 3)
      {
        send_to_char("You have mastered this reality already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_DISTORTED_REALITY] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_DISTORTED_REALITY] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_DISTORTED_REALITY]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_DISTORTED_REALITY];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg2, "alternate"))
    {
      if (ch->pcdata->powers[DRONE_ALTERNATE_REALITY] >= 3)
      {
        send_to_char("You have mastered this reality already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_ALTERNATE_REALITY] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_ALTERNATE_REALITY] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_ALTERNATE_REALITY]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_ALTERNATE_REALITY];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else
    {
      send_to_char("No such reality.\n\r", ch);
      return;
    }
  }
  else if (!str_cmp(arg1, "psyche"))
  {
    if (!str_cmp(arg2, "willpower"))
    {
      if (ch->pcdata->powers[DRONE_WILLPOWER] >= 3)
      {
        send_to_char("You have mastered this psyche already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_WILLPOWER] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_WILLPOWER] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_WILLPOWER]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_WILLPOWER];
      send_to_char("Ok.\n\r", ch);  
      return;
    }
    else if (!str_cmp(arg2, "mindcontrol"))
    {
      if (ch->pcdata->powers[DRONE_MINDCONTROL] >= 3)
      {
        send_to_char("You have mastered this psyche already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_MINDCONTROL] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_MINDCONTROL] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_MINDCONTROL]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_MINDCONTROL];
      send_to_char("Ok.\n\r", ch);
      return;
    } 
    else if (!str_cmp(arg2, "psionics"))
    {
      if (ch->pcdata->powers[DRONE_PSIONICS] >= 3)
      {
        send_to_char("You have mastered this psyche already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_PSIONICS] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_PSIONICS] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_PSIONICS]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_PSIONICS];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else
    {
      send_to_char("No such psyche.\n\r", ch);
      return;
    }
  }
  else if (!str_cmp(arg1, "combat"))
  {
    if (!str_cmp(arg2, "evasion"))
    {
      if (ch->pcdata->powers[DRONE_EVASION] >= 3)  
      {
        send_to_char("You have mastered this art of combat already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_EVASION] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_EVASION] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_EVASION]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_EVASION];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg2, "fortitude"))
    {
      if (ch->pcdata->powers[DRONE_FORTITUDE] >= 3)
      {
        send_to_char("You have mastered this art of combat already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_FORTITUDE] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_FORTITUDE] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_FORTITUDE]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_FORTITUDE];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else if (!str_cmp(arg2, "focus"))
    {
      if (ch->pcdata->powers[DRONE_FOCUS] >= 3)
      {
        send_to_char("You have mastered this art of combat already.\n\r", ch);
        return;
      }
      if (ch->exp < cost * (ch->pcdata->powers[DRONE_FOCUS] + 1))
      {
        sprintf(buf, "You need %d more exp to buy this power.\n\r", cost * (ch->pcdata->powers[DRONE_FOCUS] + 1) - ch->exp);
        send_to_char(buf,ch);
        return;
      }
      ch->pcdata->powers[DRONE_FOCUS]++;
      ch->exp -= cost * ch->pcdata->powers[DRONE_FOCUS];
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else
    {
      send_to_char("No such combat skill.\n\r", ch);
      return;
    }
  }
  else do_abstract(ch, "");
  return;
}

void do_mentalblock(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_DRONE))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (ch->pcdata->powers[DRONE_WILLPOWER] < 3)
  {
    send_to_char("You need willpower level 3 to create a mental block.\n\r", ch);
    return;
  }
  if (IS_SET(ch->newbits, NEW_MENTALBLOCK))
  {
    REMOVE_BIT(ch->newbits, NEW_MENTALBLOCK);
    send_to_char("You let your mental blocking fade.\n\r", ch);
    return;
  }
  SET_BIT(ch->newbits, NEW_MENTALBLOCK);
  send_to_char("You create a mental block, preventing anyone from controlling your actions.\n\r", ch);
  return;
}
