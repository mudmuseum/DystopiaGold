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


bool   is_rel_power       args ((CHAR_DATA *ch, int power));
int    get_rel_price      args ((CHAR_DATA *ch, int power));
char * get_player_health  args ((CHAR_DATA *ch));

char * get_player_health(CHAR_DATA *ch)
{
  static char buf[512];
     
  buf[0] = '\0';

  if (ch->hit >= ch->max_hit) strcat(buf, " perfect");
  else if (ch->hit >= ch->max_hit * 0.75) strcat(buf, " fine");
  else if (ch->hit >= ch->max_hit * 0.50) strcat(buf, " fair");
  else strcat(buf, " poor");
  return (buf[0] != '\0') ? buf+1 : "no";
}

void do_forsake(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (argument[0] == '\0')
  {
    send_to_char("Syntax : forsake <your password>\n\r", ch);
    send_to_char("WARNING : You will lose all religion powers.\n\r", ch);
    return;
  }
  if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
  {
    send_to_char("Illegal password.\n\r", ch);
    WAIT_STATE(ch, 12);  
    return;
  }
  if (ch->pcdata->relrank == 0)
  {
    send_to_char("You cannot forsake your religion unless you are at rank 2 or better.\n\r", ch);
    return;
  }

  if (ch->pcdata->religion == RELIGION_GAHR)
    sprintf(buf, "%s has forsaken The Mad God", ch->name);
  else if (ch->pcdata->religion == RELIGION_WILD)
    sprintf(buf, "%s has forsaken the Call of the Wild", ch->name);
  else if (ch->pcdata->religion == RELIGION_STARS)
    sprintf(buf, "%s has forsaken the Path of the Stars", ch->name);
  else if (ch->pcdata->religion == RELIGION_LOA)
    sprintf(buf, "%s has forsaken the Serpent God", ch->name);
  else if (ch->pcdata->religion == RELIGION_ONE)
    sprintf(buf, "%s has forsaken the crusade of The One True", ch->name);
  else
    sprintf(buf, "Bogus religion - forsaken");

  do_info(ch, buf);

  ch->pcdata->religion = 0;
  ch->pcdata->faith = 0;
  ch->pcdata->current_faith = 0;
  ch->pcdata->rel_powers = 0;

  return;
}

void do_worship(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int i, start;
  bool found = FALSE;

  one_argument(argument,arg);

  if (IS_NPC(ch)) return;
  if (ch->level > 6) start = 0;
  else start = 1;
  if (IS_SET(ch->special, SPC_HAS_CHOSEN_RELIGION) && ch->level < 7)
  {
    send_to_char("You are already worhipping a deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->newbits, NEW_MASTERY))
  {
    send_to_char("You need to master yourself before you can dedicate yourself to one god.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Which religion do you wish to worship?\n\r",ch);
    for (i = start; i <= MAX_RELIGIONS; i++)
    {
      sprintf(buf,"%s\n\r",religion_table[i].truename);
      send_to_char(buf,ch);
    }
    return;
  }
  for (i = start; i <= MAX_RELIGIONS && !found; i++)
  {
    if (!str_cmp(arg, religion_table[i].truename)) found = TRUE;
  }
  if (!found)
  {
    do_worship(ch,"");
    return;
  }
  else
  {
    ch->pcdata->religion = i-1;
    SET_BIT(ch->special, SPC_HAS_CHOSEN_RELIGION);
    sprintf(buf, "You are now known as %s %s among your fellow worshippers.\n\r",
      religion_table[ch->pcdata->religion].rankname[0],religion_table[ch->pcdata->religion].name);
    send_to_char(buf,ch);
    return;
  }
  return;
}

void do_setdecap(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument) || !is_contained2("$N", argument))
  {
    send_to_char("You really need to include both $n and $N in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETDECAP)) SET_BIT(ch->pcdata->jflags, JFLAG_SETDECAP);
  free_string(ch->pcdata->decapmessage);
  ch->pcdata->decapmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}

void do_settie(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
   
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {   
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument) || !is_contained2("$N", argument))
  {
    send_to_char("You really need to include both $n and $N in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETTIE)) SET_BIT(ch->pcdata->jflags, JFLAG_SETTIE);
  free_string(ch->pcdata->tiemessage);
  ch->pcdata->tiemessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}

void do_setlogout(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
   
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument))
  {
    send_to_char("You really need to include $n in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETLOGOUT)) SET_BIT(ch->pcdata->jflags, JFLAG_SETLOGOUT);
  free_string(ch->pcdata->logoutmessage);
  ch->pcdata->logoutmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}

void do_setlogin(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;
   
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument))       
  {
    send_to_char("You really need to include $n in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETLOGIN)) SET_BIT(ch->pcdata->jflags, JFLAG_SETLOGIN);
  free_string(ch->pcdata->loginmessage);
  ch->pcdata->loginmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}

void do_setavatar(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  int cost = 1000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->jflags, JFLAG_NOSET))
  {
    send_to_char("Your not allowed to use custom settings.\n\r", ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    sprintf(buf,"You need %d more qps to use this command.\n\r", cost - ch->pcdata->quest);
    send_to_char(buf,ch);
    return;
  }
  smash_tilde(argument);
  if (strlen(argument) < 10 || strlen(argument) > 90)
  {
    send_to_char("Between 10 and 90 chars... thanks.\n\r",ch);
    return;
  }
  if (!is_contained2("$n", argument))
  {
    send_to_char("You really need to include $n in the message.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->jflags, JFLAG_SETAVATAR)) SET_BIT(ch->pcdata->jflags, JFLAG_SETAVATAR);
  free_string(ch->pcdata->avatarmessage);
  ch->pcdata->avatarmessage = str_dup(argument);
  ch->pcdata->quest -= cost;
  send_to_char("done.\n\r",ch);
  return;
}


/*
 * Here comes the immortal commands in regard to religions
 */

/*
 * Use this call with caution, you CAN crash the mud with illegal calls.
 */
void do_relset(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int id;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (IS_NPC(ch)) return;
  if (ch->level < MAX_LEVEL)
  {
    send_to_char("You don't have permission to change this.\n\r",ch);
    return;
  }
  if (arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0')
  {
    send_to_char("#yThe syntax for relset is #G: #nrelset <id> <field> <value>#n\n\r\n\r",ch);
    send_to_char("#0* #RId is the number of the religion (1-5)\n\r",ch);
    send_to_char("#0* #RField is one of : #Gdecapmessage, name, truename, channel,#n\n\r",ch);
    send_to_char("                    #Grank1, rank2, rank3, rank4 or rank5.#n\n\r",ch);
    send_to_char("#0* #RValue is the new value for that field.#n\n\r",ch);
    return;
  }
  if (!is_number(arg1))
  {
    do_relset(ch,"");
    return;
  }
  if ((id = atoi(arg1)) < 1 || id > 5)
  {
    send_to_char("Between 1 and 5 please.\n\r",ch);
    return;
  }
  if (!str_cmp(arg2, "decapmessage"))
  {
    free_string(religion_table[id].decapmessage);
    religion_table[id].decapmessage = str_dup(argument);
  }
  else if (!str_cmp(arg2, "name"))
  {
    free_string(religion_table[id].name);
    religion_table[id].name = str_dup(argument);
  }
  else if (!str_cmp(arg2, "truename"))
  {
    free_string(religion_table[id].truename);
    religion_table[id].truename = str_dup(argument);
  }
  else if (!str_cmp(arg2, "channel"))
  {
    free_string(religion_table[id].channel);
    religion_table[id].channel = str_dup(argument);
  }
  else if (!str_cmp(arg2, "rank1"))
  {
    free_string(religion_table[id].rankname[0]);
    religion_table[id].rankname[0] = str_dup(argument);
  }
  else if (!str_cmp(arg2, "rank2"))
  {
    free_string(religion_table[id].rankname[1]);
    religion_table[id].rankname[1] = str_dup(argument);
  }
  else if (!str_cmp(arg2, "rank3"))
  {
    free_string(religion_table[id].rankname[2]);
    religion_table[id].rankname[2] = str_dup(argument);
  }
  else if (!str_cmp(arg2, "rank4"))
  {
    free_string(religion_table[id].rankname[3]);
    religion_table[id].rankname[3] = str_dup(argument);
  }
  else if (!str_cmp(arg2, "rank5"))
  {
    free_string(religion_table[id].rankname[4]);
    religion_table[id].rankname[4] = str_dup(argument);
  }
  else
  {
    send_to_char("No such field.\n\r",ch);
    return;
  }
  save_religions();
  send_to_char("Ok.\n\r",ch);
  return;
}

void do_setrank(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  int value;

  argument = one_argument(argument,arg);

  if (IS_NPC(ch)) return;
  if (ch->level < MAX_LEVEL)
  {
    send_to_char("You don't have permission to change this.\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Set the rank of whom ?\n\r",ch);
    return;
  }
  if ((victim = get_char_world(ch,arg)) == NULL)
  {
    send_to_char("They are not here.\n\r",ch);
    return;
  }
  if (!is_number(argument))
  {
    send_to_char("Not a number.\n\r",ch);
    return;
  }
  if ((value = atoi(argument)) < 1 || value > 5)
  {
    send_to_char("From one to five, please.\n\r",ch);
    return;
  }
  victim->pcdata->relrank = value-1;
  send_to_char("Ok.\n\r",ch);
  return;
}

void check_faith(CHAR_DATA *ch)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0) return;
  if (ch->pcdata->faith < 100)
  {
    if (ch->pcdata->relrank > 0)
    {
      ch->pcdata->relrank = 0;
      sprintf(buf,"You have beed degraded to the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[0],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
  }
  else if (ch->pcdata->faith < 200)
  {
    if (ch->pcdata->relrank > 1)
    {
      ch->pcdata->relrank = 1;
      sprintf(buf,"You have beed degraded to the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[1],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
    else if (ch->pcdata->relrank < 1)
    {
      ch->pcdata->relrank = 1;
      sprintf(buf,"You have gained the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[1],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
  }
  else if (ch->pcdata->faith < 300)
  {
    if (ch->pcdata->relrank > 2)
    {
      ch->pcdata->relrank = 2;
      sprintf(buf,"You have beed degraded to the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[2],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
    else if (ch->pcdata->relrank < 2)
    {
      ch->pcdata->relrank = 2;
      sprintf(buf,"You have gained the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[2],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
  }
  else if (ch->pcdata->faith < 400)
  {
    if (ch->pcdata->relrank > 3)
    {
      ch->pcdata->relrank = 3;
      sprintf(buf,"You have beed degraded to the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[3],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
    else if (ch->pcdata->relrank < 3)
    {
      ch->pcdata->relrank = 3;
      sprintf(buf,"You have gained the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[3],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
  }
  else
  {
    if (ch->pcdata->relrank < 4)
    {
      ch->pcdata->relrank = 4;
      sprintf(buf,"You have gained the title of %s %s\n\r",
        religion_table[ch->pcdata->religion].rankname[4],religion_table[ch->pcdata->religion].name);
      send_to_char(buf,ch);
    }
  }
  return;
}

bool rel_oppose(CHAR_DATA *ch, CHAR_DATA *victim)
{
  if (IS_NPC(ch) || IS_NPC(victim)) return FALSE;
  if (ch->pcdata->religion == 0 || victim->pcdata->religion == 0) return FALSE;
  if (ch->pcdata->religion == 1)
  {
    if (victim->pcdata->religion == 2 || victim->pcdata->religion == 3) return TRUE;
  }
  else if (ch->pcdata->religion == 2)
  {
    if (victim->pcdata->religion == 1 || victim->pcdata->religion == 4) return TRUE;
  }   
  else if (ch->pcdata->religion == 3)
  {
    if (victim->pcdata->religion == 1 || victim->pcdata->religion == 5) return TRUE;
  }
  else if (ch->pcdata->religion == 4)
  {
    if (victim->pcdata->religion == 2 || victim->pcdata->religion == 5) return TRUE;
  }
  else if (ch->pcdata->religion == 5)
  {
    if (victim->pcdata->religion == 3 || victim->pcdata->religion == 4) return TRUE;
  }
  return FALSE;
}

void do_relbuy(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  char tempbuf[MAX_STRING_LENGTH];
  int cost, power;

  if (IS_NPC(ch)) return;

  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    sprintf(buf, "Power                 name          price\n\r");
    if (!IS_SET(ch->pcdata->rel_powers, REL_DARMOR))
    {
      sprintf(tempbuf, "Divine Armor         (darmor)        %3d\n\r", get_rel_price(ch, REL_DARMOR));
      if (!is_rel_power(ch, REL_DARMOR)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_HOLYWORD))
    {
      sprintf(tempbuf, "Holyword             (holyword)      %3d\n\r", get_rel_price(ch, REL_HOLYWORD));
      if (!is_rel_power(ch, REL_HOLYWORD)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_GZONE))
    {
      sprintf(tempbuf, "Gods Zone            (gzone)         %3d\n\r", get_rel_price(ch, REL_GZONE));
      if (!is_rel_power(ch, REL_GZONE)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_GAIA))
    {
      sprintf(tempbuf, "Gaia's Blessing      (gaia)          %3d\n\r", get_rel_price(ch, REL_GAIA));
      if (!is_rel_power(ch, REL_GAIA)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_THORNS))
    {
      sprintf(tempbuf, "Thorns and Brambles  (thorns)        %3d\n\r", get_rel_price(ch, REL_THORNS));
      if (!is_rel_power(ch, REL_THORNS)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_CALLWILD))
    {
      sprintf(tempbuf, "Call of the Wild     (callwild)      %3d\n\r", get_rel_price(ch, REL_CALLWILD));
      if (!is_rel_power(ch, REL_CALLWILD)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_DEJA))
    {
      sprintf(tempbuf, "Deja-Vu              (deja)          %3d\n\r", get_rel_price(ch, REL_DEJA));
      if (!is_rel_power(ch, REL_DEJA)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_PESTILENCE))
    {
      sprintf(tempbuf, "Pestilence           (pestilence)    %3d\n\r", get_rel_price(ch, REL_PESTILENCE));
      if (!is_rel_power(ch, REL_PESTILENCE)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_MADNESS))
    {
      sprintf(tempbuf, "Induce Madness       (madness)       %3d\n\r", get_rel_price(ch, REL_MADNESS));
      if (!is_rel_power(ch, REL_MADNESS)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_AREAHEAL))
    {
      sprintf(tempbuf, "Area Heal            (areaheal)      %3d\n\r", get_rel_price(ch, REL_AREAHEAL));
      if (!is_rel_power(ch, REL_AREAHEAL)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_BANISHMENT))
    {
      sprintf(tempbuf, "Banishment           (banishment)    %3d\n\r", get_rel_price(ch, REL_BANISHMENT));
      if (!is_rel_power(ch, REL_BANISHMENT)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_STARSIGHT))
    {
      sprintf(tempbuf, "Starsight            (starsight)     %3d\n\r", get_rel_price(ch, REL_STARSIGHT));
      if (!is_rel_power(ch, REL_STARSIGHT)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_RPURGE))
    {
      sprintf(tempbuf, "Realms Purge         (rpurge)        %3d\n\r", get_rel_price(ch, REL_RPURGE));
      if (!is_rel_power(ch, REL_RPURGE)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_DBATTLE))
    {
      sprintf(tempbuf, "Divine Battle        (dbattle)       %3d\n\r", get_rel_price(ch, REL_DBATTLE));
      if (!is_rel_power(ch, REL_DBATTLE)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_GFURY))
    {
      sprintf(tempbuf, "Godly Fury           (gfury)         %3d\n\r", get_rel_price(ch, REL_GFURY));
      if (!is_rel_power(ch, REL_GFURY)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_BADMOON))
    {
      sprintf(tempbuf, "Bad Moon             (badmoon)       %3d\n\r", get_rel_price(ch, REL_BADMOON));
      if (!is_rel_power(ch, REL_BADMOON)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_HDESIRE))
    {
      sprintf(tempbuf, "The Hearts Desire    (hdesire)       %3d\n\r", get_rel_price(ch, REL_HDESIRE));
      if (!is_rel_power(ch, REL_HDESIRE)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_DSCRY))
    {
      sprintf(tempbuf, "Divine Scry          (dscry)         %3d\n\r", get_rel_price(ch, REL_DSCRY));
      if (!is_rel_power(ch, REL_DSCRY)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_SUBVERT))
    {
      sprintf(tempbuf, "Subvert              (subvert)       %3d\n\r", get_rel_price(ch, REL_SUBVERT));
      if (!is_rel_power(ch, REL_SUBVERT)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_REBIRTH))
    {
      sprintf(tempbuf, "Rebirth              (rebirth)       %3d\n\r", get_rel_price(ch, REL_REBIRTH));
      if (!is_rel_power(ch, REL_REBIRTH)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_CHAOS))
    {
      sprintf(tempbuf, "Chaotic Landscape    (landscape)     %3d\n\r", get_rel_price(ch, REL_CHAOS));
      if (!is_rel_power(ch, REL_CHAOS)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_NOVA))
    {
      sprintf(tempbuf, "Supernova            (supernova)     %3d\n\r", get_rel_price(ch, REL_NOVA));
      if (!is_rel_power(ch, REL_NOVA)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    if (!IS_SET(ch->pcdata->rel_powers, REL_FARMOR))
    {
      sprintf(tempbuf, "Armor of True Faith  (farmor)        %3d\n\r", get_rel_price(ch, REL_FARMOR));
      if (!is_rel_power(ch, REL_FARMOR)) ADD_COLOUR(ch, tempbuf, GREY);
      strcat(buf, tempbuf);
    }
    send_to_char(buf, ch);
    sprintf(buf, "\n\rFaith  : %d/%d\n\r", ch->pcdata->current_faith, ch->pcdata->faith);
    send_to_char(buf, ch);
    return;
  }
  if (!str_cmp(arg, "darmor"))
  {
    power = REL_DARMOR;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "holyword"))
  {
    power = REL_HOLYWORD;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "gzone"))
  {
    power = REL_GZONE;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "gaia"))
  {
    power = REL_GAIA;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "thorns"))
  {
    power = REL_THORNS;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "callwild"))
  {
    power = REL_CALLWILD;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "deja"))
  {
    power = REL_DEJA;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "pestilence"))
  {
    power = REL_PESTILENCE;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "madness"))
  {
    power = REL_MADNESS;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "areaheal"))
  {
    power = REL_AREAHEAL;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "banishment"))
  {
    power = REL_BANISHMENT;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "starsight"))
  {
    power = REL_STARSIGHT;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "rpurge"))
  {
    power = REL_RPURGE;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "dbattle"))
  {
    power = REL_DBATTLE;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "gfury"))
  {
    power = REL_GFURY;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "supernova"))
  {
    power = REL_NOVA;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "badmoon"))
  {
    power = REL_BADMOON;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "hdesire"))
  {
    power = REL_HDESIRE;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "dscry"))
  {
    power = REL_DSCRY;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "subvert"))
  {
    power = REL_SUBVERT;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "rebirth"))
  {
    power = REL_REBIRTH;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "landscape"))
  {
    power = REL_CHAOS;
    cost = get_rel_price(ch, power);
  }
  else if (!str_cmp(arg, "farmor"))
  {
    power = REL_FARMOR;
    cost = get_rel_price(ch, power);
  }
  else
  {
    send_to_char("No such religion power.\n\r", ch);
    return;
  }
  if (IS_SET(ch->pcdata->rel_powers, power))
  {
    send_to_char("You already have that religion power.\n\r", ch);
    return;
  }
  if (ch->pcdata->current_faith < cost)
  {
    send_to_char("Sorry, you cannot afford that religion power.\n\r", ch);
    return;
  }
  ch->pcdata->current_faith -= cost;
  SET_BIT(ch->pcdata->rel_powers, power);
  send_to_char("Ok.\n\r", ch);
  return;
}

bool is_rel_power(CHAR_DATA *ch, int power)
{
  switch(ch->pcdata->religion)
  {
    case RELIGION_LOA:
      if (power == REL_HDESIRE) return TRUE;
      if (power == REL_DSCRY) return TRUE;
      if (power == REL_BADMOON) return TRUE;
      if (power == REL_SUBVERT) return TRUE;
      break;
    case RELIGION_STARS:
      if (power == REL_STARSIGHT) return TRUE;
      if (power == REL_BANISHMENT) return TRUE;  
      if (power == REL_AREAHEAL) return TRUE;
      if (power == REL_NOVA) return TRUE;
      break;
    case RELIGION_ONE:
      if (power == REL_GFURY) return TRUE;
      if (power == REL_DBATTLE) return TRUE;  
      if (power == REL_RPURGE) return TRUE;
      if (power == REL_FARMOR) return TRUE;
      break;
    case RELIGION_WILD:
      if (power == REL_THORNS) return TRUE;
      if (power == REL_CALLWILD) return TRUE;  
      if (power == REL_GAIA) return TRUE;
      if (power == REL_REBIRTH) return TRUE;
      break;
    case RELIGION_GAHR:
      if (power == REL_PESTILENCE) return TRUE;
      if (power == REL_MADNESS) return TRUE;  
      if (power == REL_DEJA) return TRUE;
      if (power == REL_CHAOS) return TRUE;
      break;
    default:
      bug("Bad Religion", 0);
      break;
  }

  /*
   * The powers that all can get
   */
  if (power == REL_DARMOR)    return TRUE;
  if (power == REL_HOLYWORD)  return TRUE;
  if (power == REL_GZONE)     return TRUE;

  /*
   * None of the above, so it's not a power
   */
  return FALSE;
}

int get_rel_price(CHAR_DATA *ch, int power)
{
  int price;
  int rank;

  switch(power)
  {
    case REL_DARMOR:
    case REL_HOLYWORD:
      rank = 0;
      price = 20;
      break;
    case REL_RPURGE:
    case REL_GZONE:
    case REL_GAIA:
    case REL_DEJA:
    case REL_AREAHEAL:
    case REL_BADMOON:
      rank = 1;
      price = 40;
      break;
    case REL_THORNS:
    case REL_CALLWILD:
    case REL_PESTILENCE:
    case REL_MADNESS:
    case REL_BANISHMENT:
    case REL_STARSIGHT:
    case REL_DBATTLE:
    case REL_GFURY:
    case REL_HDESIRE:
    case REL_DSCRY:
      rank = 2;
      price = 60;
      break;
    case REL_SUBVERT:
    case REL_REBIRTH:
    case REL_CHAOS:
    case REL_NOVA:
    case REL_FARMOR:
      rank = 3;
      price = 80;
      break;
    default: // bad power ??
      rank = 10;
      price = 2000;
      break;
  }

  if (rank > ch->pcdata->relrank) price *= 2;
  if (!is_rel_power(ch, power))   price *= 2;
  return price;
}

void do_gzone(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT af;
  AREA_DATA *pArea;
  int cost = 10000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_GZONE))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_GZONE, 0))
  {
    send_to_char("This holy enchantment has already been invoked here.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
  {
    if (number_range(1, 4) != 2)
    {
      int dam;
   
      dam = number_range(10, 20);
      dam *= ch->pcdata->current_faith; // wicked :)
      ch->hit -= dam;
      if (ch->hit < 1) ch->hit = 1;
      ch->mana -= cost;
      
      send_to_char("Your enchantment backfires, the spell is spoiled.\n\r", ch);
      WAIT_STATE(ch, 24);
      return;
    }
  }

  af.type      = AREA_AFF_GZONE;
  af.duration  = ch->pcdata->faith/10;
  af.level     = ch->pcdata->faith;
  af.religion  = ch->pcdata->religion;
  affect_to_area(pArea, &af);

  act("$n invokes the name of $s god, as $e lays the God's Zone enchantment.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You lay the God's Zone enchantment on this area.\n\r", ch);
  ch->mana -= cost;
  WAIT_STATE(ch, 36);
  return;
}

void do_darmor(CHAR_DATA *ch, char *argument)
{
  int sn, cost = 5000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_DARMOR))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  sn = skill_lookup("darmor");   
  if (sn  > 0) (*skill_table[sn].spell_fun) (sn, 50, ch, ch);
  WAIT_STATE(ch, 6);
  ch->mana -= cost;
  return;
}

void do_holyword(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;
  int cost = 5000;
  int dam;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_HOLYWORD))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (IS_SET(ch->in_room->room_flags,ROOM_SAFE) && !ragnarok)
  {   
    send_to_char("You cannot fight in a safe room.\n\r", ch );
    return;
  }
  send_to_char("You utter a holyword.\n\r", ch);
  act("$n utters a holyword.", ch, NULL, NULL, TO_ROOM);
  for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
  {
    if (!rel_oppose(ch, gch))
    {
      if (gch == ch) continue;
      send_to_char("You are unaffected.\n\r", gch);
      continue;
    }
    if (IS_AFFECTED(gch, AFF_FARMOR))
    {
      act("The holy attack fizzles as it strikes $n's armor of faith.", gch, NULL, NULL, TO_ROOM);
      act("The holy attack fizzles as it strikes your armor of faith.", gch, NULL, NULL, TO_CHAR);
      continue;
    }
    send_to_char("The perversion of the holyword strikes the very core of your soul.\n\r", gch);
    act("$N howls in pain, as the agony of $n's holyword strikes $s soul.", ch, NULL, gch, TO_NOTVICT);
    act("$N howls in pain, as the agony of your holyword strikes $s soul.", ch, NULL, gch, TO_CHAR);
    dam = UMIN(gch->hit - 1, number_range(ch->pcdata->faith * 10, ch->pcdata->faith * 20));
    if (dam > 3000) dam = number_range(2900, 3000);
    gch->hit -= dam;
    WAIT_STATE(gch, 12);
  }
  ch->mana -= cost;
  WAIT_STATE(ch, 24);
  return;
}

void do_thorns(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT af;
  AREA_DATA *pArea;
  int cost = 15000;  
  
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_THORNS))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_THORNS, 0))
  {
    send_to_char("This holy enchantment has already been invoked here.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
  {
    if (number_range(1, 4) != 2)
    {
      int dam;
   
      dam = number_range(10, 20);
      dam *= ch->pcdata->current_faith; // wicked :)
      ch->hit -= dam;
      if (ch->hit < 1) ch->hit = 1;
      ch->mana -= cost;
      
      send_to_char("Your enchantment backfires, the spell is spoiled.\n\r", ch);
      WAIT_STATE(ch, 24);
      return;
    }
  }

  af.type      = AREA_AFF_THORNS;
  af.duration  = ch->pcdata->faith/10;
  af.level     = ch->pcdata->faith;
  af.religion  = ch->pcdata->religion;
  affect_to_area(pArea, &af);

  act("$n invokes the name of $s god, as $e lays the Thorns and Brambles enchantment.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You lay the Thorns and Brambles enchantment on this area.\n\r", ch);
  ch->mana -= cost;
  WAIT_STATE(ch, 36);
  return;
}

void do_rpurge(CHAR_DATA *ch, char *argument)
{
  int cost = 10000;
  AREA_AFFECT *paf;
  AREA_AFFECT *paf_next;
  AREA_DATA *pArea;
  bool found = FALSE;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }  
  if (!IS_SET(ch->pcdata->rel_powers, REL_RPURGE))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }

  for (pArea = area_first; pArea; pArea = pArea->next)
  {
    for (paf = pArea->affects; paf; paf = paf_next)
    {
      paf_next = paf->next;

      found = TRUE;

      if ((ch->pcdata->faith > paf->level && number_percent() > 25) || number_percent() > 75)
      {
        send_to_char("Found and destroyed a magical enchantment.\n\r", ch);
        area_affect_remove(pArea, paf);
      }
      else send_to_char("Found, but failed to destroy magical enchantment.\n\r", ch);
    }
  }
  if (!found)
  {
    send_to_char("No enchantments currently active.\n\r", ch);
    return;
  }
  WAIT_STATE(ch, 18);
  ch->mana -= cost;
  return;
}


void do_deja(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int cost = 10000;
  char *action;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_DEJA))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_DEJAVU))
  {
    send_to_char("Sorry, you can only use this power once every 2 hours.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobiles.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {
    act("The unholy attack fizzles as it strikes $n's armor of faith.", victim, NULL, NULL, TO_ROOM);
    act("The unholy attack fizzles as it strikes your armor of faith.", victim, NULL, NULL, TO_CHAR);
    ch->mana -= cost;
    SET_TIMER(ch, TIMER_DEJAVU, 2);
    return;
  }
  if (!victim->desc)
  {
    send_to_char("Not on linkdead players.\n\r", ch);
    return;
  }
  if (!victim->desc->inlast || strlen(victim->desc->inlast) < 2)
  {
    send_to_char("But they haven't done anything yet that they can relive.\n\r", ch);
    return;
  }
  act("$n invokes the name of $s god, as he forces $N to relive $S past.", ch, NULL, victim, TO_ROOM);
  act("You invoke your god, and $N relives $S past.", ch, NULL, victim, TO_CHAR);
  act("$n invokes the name of $s god, and you get a strange sense of deja-vu.", ch, NULL, victim, TO_VICT);

  action = str_dup(victim->desc->inlast);
  interpret(victim, action);
  free_string(action);

  ch->mana -= cost;
  SET_TIMER(ch, TIMER_DEJAVU, 2);
  WAIT_STATE(ch, 6);
  return;
}

void do_holysymbol(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int pcost = 500;
  int qcost = 1000;
  int type;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (ch->practice < pcost)
  {
    sprintf(buf, "Sorry, this costs %d primal, which you dont have.\n\r", pcost);
    send_to_char(buf, ch);
    return;
  }
  if (ch->pcdata->quest < qcost)   
  {
    sprintf(buf, "Sorry, this costs %d qps, which you dont have.\n\r", qcost);
    send_to_char(buf, ch);
    return;
  }
  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("#0Which type of holy symbol do you wish to create ?#n\n\r\n\r", ch);
    send_to_char("#G   Ring     Bodyart    Wield    Float#n\n\r", ch);
    send_to_char("#G   Mask     Special    Medal    Amulet#n\n\r", ch);
    return;
  }
  if (!str_cmp(arg, "ring"))         type = ITEM_WEAR_FINGER;
  else if (!str_cmp(arg, "bodyart")) type = ITEM_WEAR_BODYART;
  else if (!str_cmp(arg, "wield"))   type = ITEM_WIELD;
  else if (!str_cmp(arg, "float"))   type = ITEM_WEAR_FLOAT;
  else if (!str_cmp(arg, "mask"))    type = ITEM_WEAR_FACE;
  else if (!str_cmp(arg, "special")) type = ITEM_WEAR_SPECIAL;
  else if (!str_cmp(arg, "medal"))   type = ITEM_WEAR_MEDAL;
  else if (!str_cmp(arg, "amulet"))  type = ITEM_WEAR_NECK;
  else
  {
    do_holysymbol(ch, "");
    return;
  }
  if ((pObjIndex = get_obj_index(OBJ_VNUM_HOLYSYMBOL)) == NULL)
  {
    send_to_char("Missing object, please inform a God.\n\r",ch);
    return;
  }
  obj = create_object(pObjIndex, 50);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj->ownerid = ch->pcdata->playerid;
  obj->wear_flags = type + 1;
  obj->value[0] = ch->pcdata->religion;
  obj_to_char(obj, ch);
  ch->practice      -= pcost;
  ch->pcdata->quest -= qcost;
  act("$p appears in your hands.",ch,obj,NULL,TO_CHAR);
  act("$p appears in $n's hands.",ch,obj,NULL,TO_ROOM);
  return;
}

void do_areaheal(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;
  char buf[MAX_STRING_LENGTH];
  int cost = 10000;
  int heal;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_AREAHEAL))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  act("$n invokes the name of $s god, and calls for a blessing on everyone.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You call upon your god to heal those who are true of heart.\n\r", ch);
  sprintf(buf, "You feel a warm feeling run through your body, as %s fills you with healing.\n\r", religion_table[ch->pcdata->religion].truename);
  heal = number_range(ch->pcdata->faith * 15, ch->pcdata->faith * 30);
  for (gch = char_list; gch; gch = gch->next)
  {  
    if (IS_NPC(gch)) continue;                             // no monsters
    if (rel_oppose(ch, gch)) continue;                     // no anti religions
    if (!gch->in_room) continue;                           // no room-less
    if (gch->in_room->area != ch->in_room->area) continue; // only same area
    gch->hit += number_range(heal / 2, 3 * heal / 2);
    if (gch->hit > gch->max_hit) gch->hit = gch->max_hit;
    send_to_char(buf, gch);
    send_to_char("Your healing energies find a target.\n\r", ch);
  }
  ch->mana -= cost;
  WAIT_STATE(ch, 12);
  return;
}

void do_badmoon(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT af;
  AREA_DATA *pArea;
  int cost = 10000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_BADMOON))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_BADMOON))
  {
    send_to_char("You can only use this power once every 12 hours.\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_BADMOON, 0))
  {
    send_to_char("This holy enchantment has already been invoked here.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
  {
    if (number_range(1, 4) != 2)
    {
      int dam;

      dam = number_range(10, 20);
      dam *= ch->pcdata->current_faith; // wicked :)
      ch->hit -= dam;
      if (ch->hit < 1) ch->hit = 1;
      ch->mana -= cost;

      send_to_char("Your enchantment backfires, the spell is spoiled.\n\r", ch);
      WAIT_STATE(ch, 24);
      return;
    }
  }

  af.type      = AREA_AFF_BADMOON;
  af.duration  = ch->pcdata->faith/10;
  af.level     = ch->pcdata->faith;
  af.religion  = ch->pcdata->religion;
  affect_to_area(pArea, &af);

  act("$n invokes the name of $s god, as $e lays the Bad Moon enchantment.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You lay the Bad Moon enchantment on this area.\n\r", ch);
  ch->mana -= cost;
  SET_TIMER(ch, TIMER_BADMOON, 12);
  WAIT_STATE(ch, 12);
  return;
}

void do_relpowers(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  { 
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (ch->pcdata->rel_powers == 0)
  {
    send_to_char("You don't have any religion powers.\n\r", ch);
    return;
  }
  send_to_char("You have the following religion powers :\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_DARMOR))        send_to_char(" * Divine Armor\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_HOLYWORD))      send_to_char(" * Holyword\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_RPURGE))        send_to_char(" * Realms Purge\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_GZONE))         send_to_char(" * God's Zone\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_GAIA))          send_to_char(" * Gaia's Blessing\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_DEJA))          send_to_char(" * Deja-vu\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_AREAHEAL))      send_to_char(" * Area-Heal\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_BADMOON))       send_to_char(" * Bad Moon\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_THORNS))        send_to_char(" * Thorns and Brambles\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_CALLWILD))      send_to_char(" * Call of the Wild\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_PESTILENCE))    send_to_char(" * Pestilence\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_MADNESS))       send_to_char(" * Induce Madness\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_BANISHMENT))    send_to_char(" * Banishment\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_STARSIGHT))     send_to_char(" * Starsight\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_DBATTLE))       send_to_char(" * Divine Battle\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_GFURY))         send_to_char(" * Godly Fury\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_HDESIRE))       send_to_char(" * The Hearts Desire\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_DSCRY))         send_to_char(" * Divine Scry\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_SUBVERT))       send_to_char(" * Subvert\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_REBIRTH))       send_to_char(" * Rebirth\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_CHAOS))         send_to_char(" * Chaotic Landscape\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_NOVA))          send_to_char(" * Supernova\n\r", ch);
  if (IS_SET(ch->pcdata->rel_powers, REL_FARMOR))        send_to_char(" * Armor of True Faith\n\r", ch);
  return;
}

void do_hdesire(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int cost = 15000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_HDESIRE))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_HDESIRE))
  {
    send_to_char("Sorry, you can only use this power once every 2 hours.\n\r", ch);
    return;
  }
  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Whom do you wish to read the hearts desires of?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch);
    return;
  }
  if (IS_NPC(victim) || victim->level > 6)
  {
    send_to_char("Mobiles have no desires.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {  
    act("The spell fizzles as it strikes some holy barrier.", ch, NULL, NULL, TO_CHAR);
    act("The armor of faith shimmers for a second, repelling some attack.", victim, NULL, NULL, TO_CHAR);
    SET_TIMER(ch, TIMER_HDESIRE, 2);
    ch->mana -= cost;
    return;
  }
  sprintf(buf, "%s score", arg);
  do_asperson(ch, buf);
  if (victim->pcdata->religion != 0)
  {
    sprintf(buf, "%s relpowers", arg);
    do_asperson(ch, buf);
  }
  ch->mana -= cost;
  send_to_char("\n\rYou read their hearts desires.\n\r", ch);
  SET_TIMER(ch, TIMER_HDESIRE, 2);
  WAIT_STATE(ch, 12);
  return;
}

void do_dscry(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  ROOM_INDEX_DATA *chroom;
  ROOM_INDEX_DATA *victimroom;
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  int cost = 15000;
  
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_DSCRY))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  one_argument(argument, arg);
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
  if (!IS_NPC(victim) && victim->level > 6)
  {
    send_to_char("Hardly.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {
    act("The scry attempt fizzles as it strikes some holy barrier.", ch, NULL, NULL, TO_CHAR);
    act("The armor of faith shimmers for a second, repelling some attack.", victim, NULL, NULL, TO_CHAR);
    ch->mana -= cost;
    return;
  }
  chroom = ch->in_room;
  victimroom = victim->in_room;
  char_from_room(ch);
  char_to_room(ch, victimroom);
  if (IS_AFFECTED(ch, AFF_SHADOWPLANE) && (!IS_AFFECTED(victim, AFF_SHADOWPLANE)))
  {
    REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
    do_look(ch, "scry");
    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
  }
  else if (!IS_AFFECTED(ch, AFF_SHADOWPLANE) && (IS_AFFECTED(victim, AFF_SHADOWPLANE)))
  {
    SET_BIT(ch->affected_by, AFF_SHADOWPLANE);
    do_look(ch, "scry");
    REMOVE_BIT(ch->affected_by, AFF_SHADOWPLANE);
  }
  else do_look(ch,"scry");
  char_from_room(ch);
  char_to_room(ch, chroom);
  sprintf(buf, "\n\r%s is in %s health, and is currenlty located in '%s'\n\r",
    victim->name, get_player_health(victim), victim->in_room->area->name);
  send_to_char(buf, ch);
  ch->mana -= cost;
  return;
}

void do_pestilence(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT af;
  AREA_DATA *pArea;
  int cost = 15000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_PESTILENCE))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_PESTILENCE, 0))
  {
    send_to_char("This unholy enchantment has already been invoked here.\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_PESTILENCE))
  {
    send_to_char("Sorry, you can only use this power once every 4 hours.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
  {
    if (number_range(1, 4) != 2)
    {
      int dam;
   
      dam = number_range(10, 20);
      dam *= ch->pcdata->current_faith; // wicked :)
      ch->hit -= dam;
      if (ch->hit < 1) ch->hit = 1;
      ch->mana -= cost;
      
      send_to_char("Your enchantment backfires, the spell is spoiled.\n\r", ch);
      WAIT_STATE(ch, 24);
      return;
    }
  }

  af.type      = AREA_AFF_PESTILENCE;
  af.duration  = ch->pcdata->faith/10;
  af.level     = ch->pcdata->faith;
  af.religion  = ch->pcdata->religion;
  affect_to_area(pArea, &af);
   
  act("$n invokes the name of $s god, as $e lays the Pestilence enchantment.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You lay the Pestilence enchantment on this area.\n\r", ch);
  ch->mana -= cost;
  SET_TIMER(ch, TIMER_PESTILENCE, 4);
  WAIT_STATE(ch, 36);
  return;
}  

void do_banishment(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT *paf;
  AREA_AFFECT *paf_next;
  AREA_DATA *pArea;
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  bool found = FALSE;
  int cost = 15000;

  if (IS_NPC(ch)) return;
  one_argument(argument, arg);
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_BANISHMENT))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("Banish the area in which whom is standing?\n\r", ch);
    return;
  }
  if ((pArea = victim->in_room->area) == NULL)
  {   
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  for (paf = pArea->affects; paf; paf = paf_next)
  {
    paf_next = paf->next;

    found = TRUE;

    if ((ch->pcdata->faith > paf->level && number_percent() > 25) || number_percent() > 75)
    {
      send_to_char("Found and destroyed a magical enchantment.\n\r", ch);
      area_affect_remove(pArea, paf);
    }
    else send_to_char("Found, but failed to destroy magical enchantment.\n\r", ch);
  }
  if (!found)
  {
    send_to_char("No enchantments currently active.\n\r", ch);
    return;
  }
  WAIT_STATE(ch, 12);
  ch->mana -= cost;
  return;
}

void do_starsight(CHAR_DATA *ch, char *argument)
{
  int cost = 15000;   
  int fps = 1;
  
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_BANISHMENT))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (IS_SET(ch->itemaffect, ITEMA_VISION))
  {
    send_to_char("You already have this divine sight.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana to cast this spell.\n\r", ch);
    return;
  }
  if (ch->pcdata->current_faith < fps)
  {
    send_to_char("You don't have enough fps to power the spell.\n\r", ch);
    return;
  }
  send_to_char("Your eyes flicker with divine vision.\n\r", ch);
  SET_BIT(ch->itemaffect, ITEMA_VISION);
  ch->pcdata->current_faith -= fps;
  ch->mana -= cost;
  return;
}

void do_gaia(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT af;
  AREA_DATA *pArea;
  int cost = 10000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_GAIA))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL)) 
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_GAIA, 0))
  {
    send_to_char("This unholy enchantment has already been invoked here.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
  {
    if (number_range(1, 4) != 2)
    {
      int dam;
   
      dam = number_range(10, 20);
      dam *= ch->pcdata->current_faith; // wicked :)
      ch->hit -= dam;
      if (ch->hit < 1) ch->hit = 1;
      ch->mana -= cost;
      
      send_to_char("Your enchantment backfires, the spell is spoiled.\n\r", ch);
      WAIT_STATE(ch, 24);
      return;
    }
  }

  af.type      = AREA_AFF_GAIA;
  af.duration  = ch->pcdata->faith/10;
  af.level     = ch->pcdata->faith;
  af.religion  = ch->pcdata->religion;
  affect_to_area(pArea, &af);
   
  act("$n invokes the name of $s god, as $e lays the Gaia's Blessing enchantment.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You lay the Gaia's Blessing enchantment on this area.\n\r", ch);
  ch->mana -= cost;
  WAIT_STATE(ch, 36);
  return;
}

void do_dbattle(CHAR_DATA *ch, char *argument)
{
  ROOM_INDEX_DATA *location;
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int cost = 15000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_DBATTLE))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (ch->in_room->vnum >= ROOM_VNUM_REL_HALL1 &&
      ch->in_room->vnum <= ROOM_VNUM_REL_HALL3)
  {
    send_to_char("Say, what?!?\n\r", ch);
    return;
  }
  if (in_fortress(ch))
  {
    send_to_char("Hehe, not in the fortress silly.\n\r", ch);
    return;
  }
  if (ch->in_room != NULL)
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("Your are in the arena.\n\r",ch);  
      return;
    }
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("Not on mobiles.\n\r", ch);
    return;
  }
  if (victim->pcdata->religion == 0)
  {
    send_to_char("They have no faith, and are hardly worthy of your time.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {
    act("The spell fizzles as it strikes $N's armor of faith.", ch, NULL, victim, TO_CHAR);
    act("Your armor of faith shimmers as it repels an attack.", victim, NULL, NULL, TO_CHAR);
    ch->mana -= cost;
    return;
  }

  sprintf(buf, "I challenge you, %s, to a holy duel, on your faith.", victim->name);
  do_say(ch, buf);

  if ((location = get_room_index(ROOM_VNUM_REL_HALL1)) != NULL
       && location->people == NULL)
  {
    char_from_room(victim);
    char_from_room(ch);
    char_to_room(victim, location);
    char_to_room(ch, location);
  }
  else if ((location = get_room_index(ROOM_VNUM_REL_HALL2)) != NULL
       && location->people == NULL)
  {
    char_from_room(victim);
    char_from_room(ch);
    char_to_room(victim, location);
    char_to_room(ch, location);
  }
  else if ((location = get_room_index(ROOM_VNUM_REL_HALL3)) != NULL
       && location->people == NULL)
  {
    char_from_room(victim);
    char_from_room(ch);
    char_to_room(victim, location);
    char_to_room(ch, location);
  }
  else
  {
    send_to_char("Sorry, the halls are currently being used.\n\r", ch);
    return;
  }
  ch->mana -= cost;
  WAIT_STATE(ch, 6);
  return;
}  

void do_madness(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *gch;
  int cost = 15000;
  char buf[MAX_STRING_LENGTH];
  bool found = FALSE;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_MADNESS))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_MADNESS))
  {
    send_to_char("Sorry, you can only use this power once every 2 hours.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  act("$n twists reality and challenges your sanity with horrid images.", ch, NULL, NULL, TO_ROOM);
  for (gch = ch->in_room->people; gch; gch = gch->next)
  {
    if (IS_NPC(gch)) continue;
    if (!gch->fighting || gch->fighting != ch) continue;
    if ((gch->pcdata->faith > ch->pcdata->faith) && number_range(1,3) == 2) continue;
    if (IS_AFFECTED(gch, AFF_FARMOR))
    {
      act("$n's armor of faith shimmers for a second.", gch, NULL, NULL, TO_ROOM);
      act("Your armor of faith protects you from harm.", gch, NULL, NULL, TO_CHAR);
      continue;
    }
    found = TRUE;
    sprintf(buf, "%s is struck by madness!\n\r", gch->name);
    send_to_char(buf, ch);
    send_to_char("You are struck by madness.\n\r", gch);
    SET_BIT(gch->pcdata->jflags, JFLAG_MADNESS);
  }
  if (!found) send_to_char("Noone seems to be affected.\n\r", ch);
  ch->mana -= cost;
  WAIT_STATE(ch, 12);
  SET_TIMER(ch, TIMER_MADNESS, 2);
  return;
}

void do_callwild(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  AFFECT_DATA af;
  char buf[MAX_STRING_LENGTH];
  int cost = 15000;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_CALLWILD))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_CALLWILD))
  {
    send_to_char("You can only call on the wild every 2 mud hours.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("They are but a creature of the wild themselfs.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {
    act("The spell fizzles as it strikes some holy barrier.", ch, NULL, NULL, TO_CHAR);
    act("The armor of faith shimmers for a second, repelling some attack.", victim, NULL, NULL, TO_CHAR);
    SET_TIMER(ch, TIMER_CALLWILD, 2);
    ch->mana -= cost;
    return;
  }
  if (!IS_SET(victim->affected_by, AFF_THORNS))
  {
    SET_BIT(victim->affected_by, AFF_THORNS);
    send_to_char("You are rooted to the ground by vines and thorny brambles.\n\r", victim);
    sprintf(buf, "The will answers your prayer and roots %s to the ground.\n\r", victim->name);
    send_to_char(buf, ch);
  }
  one_hit(ch, victim, gsn_thorns, 1);
  one_hit(ch, victim, gsn_thorns, 1);

  af.type      = skill_lookup("reserved");
  af.duration  = 20;
  af.location  = APPLY_DAMROLL;
  af.modifier  = number_range(-100, -200);
  af.bitvector = 0;
  affect_to_char(victim, &af );

  af.type      = skill_lookup("reserved");
  af.duration  = 20;
  af.location  = APPLY_HITROLL;
  af.modifier  = number_range(-100, -200);
  af.bitvector = 0;
  affect_to_char(victim, &af );

  send_to_char("You are cursed by the forces of nature.\n\r", victim);
  send_to_char("The forces of nature curse the heathen.\n\r", ch);

  SET_TIMER(ch, TIMER_CALLWILD, 2);
  ch->mana -= cost;
  return;
}

void do_gfury(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int cost = 15000;
  int count = 1;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_GFURY))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_GFURY))
  {
    send_to_char("Sorry, you can only use this power once every 2 hours.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("They are hardly worthy of your wrath.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {
    act("The spell fizzles as it strikes some holy barrier.", ch, NULL, NULL, TO_CHAR);
    act("The armor of faith shimmers for a second, repelling some attack.", victim, NULL, NULL, TO_CHAR);
    ch->mana -= cost;
    SET_TIMER(ch, TIMER_GFURY, 2);
    return;
  }
  do_say(ch, "Time to admit your sins faithless. The time of reckoning is here!");
  count += ch->pcdata->faith / 15;
  count -= victim->pcdata->faith / 15;
  if (rel_oppose(ch, victim)) count *= 2;
  if (count < 2) count = 2;
  if (count > 7) count = 7;
  while (--count >= 0) one_hit(ch, victim, gsn_faith, 1);
  ch->mana -= cost;
  SET_TIMER(ch, TIMER_GFURY, 2);
  return;
}

void do_subvert(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT *paf;
  AREA_DATA *pArea;
  bool found = FALSE;
  int cost = 20000;
  int fps = 1;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_SUBVERT))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (ch->pcdata->religion != RELIGION_LOA) fps *= 2;
  if (ch->pcdata->current_faith < fps)
  {
    send_to_char("You do not have enough faith points to complete the subversion.\n\r", ch);
    return;
  }
  act("$n praises the almighty loa.", ch, NULL, NULL, TO_ROOM);
  act("You praise the almight loa.", ch, NULL, NULL, TO_CHAR);
  for (paf = pArea->affects; paf; paf = paf->next)
  {
    if (paf->religion == ch->pcdata->religion) continue;

    found = TRUE;
    if ((ch->pcdata->faith > paf->level && number_percent() > 25) || number_percent() > 75)
    {
      paf->religion = ch->pcdata->religion;
      send_to_char("You subvert an area affect.\n\r", ch);
    }
    else
      send_to_char("Failed the subversion of an area affect.\n\r", ch);
  }
  if (!found)
  {
    send_to_char("No affects found in this area.\n\r", ch);
    return;
  }
  ch->pcdata->current_faith -= fps;
  ch->mana -= cost;
  return;
}

void do_rebirth(CHAR_DATA *ch, char *argument)
{
  int cost = 20000;
  int fps = 1;
  AFFECT_DATA af;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_REBIRTH))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (ch->pcdata->religion != RELIGION_WILD) fps *= 2;
  if (ch->pcdata->current_faith < fps)
  {
    send_to_char("You do not have enough faith points to be reborn.\n\r", ch);
    return;
  }
  if (has_timer(ch)) return;
  ch->pcdata->current_faith -= fps;
  ch->loc_hp[0] = 0;
  ch->loc_hp[1] = 0;
  ch->loc_hp[2] = 0;
  ch->loc_hp[3] = 0;
  ch->loc_hp[4] = 0;
  ch->loc_hp[5] = 0;
  ch->loc_hp[6] = 0;
  ch->mana = ch->max_mana;
  ch->move = ch->max_move;
  ch->hit = ch->max_hit;

  af.type      = skill_lookup("bless");
  af.duration  = 20;
  af.location  = APPLY_HITROLL;
  af.modifier  = number_range(100, 150);
  af.bitvector = AFF_DARMOR;
  affect_to_char(ch, &af);
  af.location  = APPLY_DAMROLL;
  af.modifier  = number_range(100, 150);
  affect_to_char(ch, &af);

  SET_BIT(ch->itemaffect, ITEMA_VISION);
  SET_BIT(ch->newbits, NEW_VISION);
  act("$n calls to the mother of nature, and $s body is instantly healed.", ch, NULL, NULL, TO_ROOM);
  send_to_char("Your eyes flicker, and everything is made clear.\n\r",ch);
  send_to_char("Your body regenerates all wounds, and you feel as fresh as ever.\n\r", ch);
  return;
}

void do_landscape(CHAR_DATA *ch, char *argument)
{
  AREA_AFFECT af;
  AREA_DATA *pArea;
  int cost = 20000;
  int fps = 1;
    
  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_CHAOS))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  } 
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->pcdata->religion != RELIGION_GAHR) fps *= 2;
  if (ch->pcdata->current_faith < fps)
  {
    send_to_char("You do not have enough faith points to invoke the chaotic landscape.\n\r", ch);
    return;
  }
  if ((pArea = ch->in_room->area) == NULL)
  {
    send_to_char("You cannot cast this spell in this place.\n\r", ch);
    return;
  }
  if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
  { 
    send_to_char("This unholy enchantment has already been invoked here.\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }

  af.type      = AREA_AFF_CHAOS;
  af.duration  = ch->pcdata->faith/10;
  af.level     = ch->pcdata->faith;
  af.religion  = ch->pcdata->religion;
  affect_to_area(pArea, &af);

  act("$n invokes the name of $s god, as $e lays the Chaotic Landscape enchantment.", ch, NULL, NULL, TO_ROOM);
  send_to_char("You lay the Chaotic Landscape enchantment on this area.\n\r", ch);

  ch->mana -= cost;
  ch->pcdata->current_faith -= fps;
  WAIT_STATE(ch, 12);
  return;
}

void do_supernova(CHAR_DATA *ch, char *argument)
{
  int fps = 1, i, count, cost = 20000;
  CHAR_DATA *victim;
  OBJ_DATA *wield1;
  OBJ_DATA *wield2;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_NOVA))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (ch->pcdata->religion != RELIGION_STARS) fps *= 2;
  if (ch->pcdata->current_faith < fps)
  {
    send_to_char("You do not have enough faith points to create a supernova.\n\r", ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_SUPERNOVA))
  {
    send_to_char("Sorry, you can only use this power once every 4 hours.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You are not fighting anyone.\n\r", ch);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("They are hardly worthy targets of a supernova.\n\r", ch);
    return;
  }
  if (IS_AFFECTED(victim, AFF_FARMOR))
  {
    act("The spell fizzles as it strikes some holy barrier.", ch, NULL, NULL, TO_CHAR);
    act("The armor of faith shimmers for a second, repelling some attack.", victim, NULL, NULL, TO_CHAR);
    ch->mana -= cost;
    SET_TIMER(ch, TIMER_SUPERNOVA, 4);
    return;
  }
  count = URANGE(4, ch->max_hit/10000, 8);
  for (i = 0; i < count; i++)
    one_hit(ch, victim, gsn_nova, 1);

  if ((wield1 = get_eq_char(victim, WEAR_WIELD)) != NULL && number_range(1, 3) != 2)
    take_item(victim, wield1);
  if ((wield2 = get_eq_char(victim, WEAR_HOLD)) != NULL && number_range(1, 3) != 2)
    take_item(victim, wield2);

  ch->mana -= cost;
  ch->pcdata->current_faith -= fps;
  SET_TIMER(ch, TIMER_SUPERNOVA, 4);
  return;
}

void do_farmor(CHAR_DATA *ch, char *argument)
{
  int sn, cost = 20000, fps = 1;

  if (IS_NPC(ch)) return;
  if (ch->pcdata->religion == 0)
  {
    send_to_char("You are not worshipping any deity.\n\r",ch);
    return;
  }
  if (!IS_SET(ch->pcdata->rel_powers, REL_FARMOR))
  {
    send_to_char("You don't have that religion power.\n\r", ch);
    return;
  }
  if (!IS_SET(ch->itemaffect, ITEMA_HOLYSYMBOL))
  {
    send_to_char("Where is your holy symbol?\n\r", ch);
    return;
  }
  if (ch->mana < cost)
  {
    send_to_char("You don't have enough mana.\n\r", ch);
    return;
  }
  if (ch->pcdata->religion != RELIGION_ONE) fps *= 2;
  if (ch->pcdata->current_faith < fps)
  {
    send_to_char("You do not have enough faith points to invoke this warding.\n\r", ch);
    return;
  }
  sn = skill_lookup("faith armor");
  if (sn  > 0) (*skill_table[sn].spell_fun) (sn, 50, ch, ch);
  WAIT_STATE(ch, 6);
  ch->mana -= cost;
  ch->pcdata->current_faith -= fps;
  return;
}
