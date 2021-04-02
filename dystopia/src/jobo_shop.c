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

DECLARE_SHOP_FUN( shopspec_generic		);
DECLARE_SHOP_FUN( shopspec_healer		);

const struct shop_type shop_table [] =
{
  { "shopspec_generic",         shopspec_generic          },
  { "shopspec_healer",          shopspec_healer           },

  /* end of table */
  { "", 0 }
};

void do_shop(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *keeper;
  char arg[MAX_INPUT_LENGTH];
  
  if (IS_NPC(ch)) return;
  argument = one_argument(argument, arg);
  if ((keeper = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("You cannot seem to find that shopkeeper.\n\r", ch);
    return;
  }
  if (!IS_NPC(keeper))
  {
    send_to_char("That's not a shopkeeper.\n\r", ch);
    return;
  }
  if (keeper->shop_fun != 0)
  {
    (*keeper->shop_fun)(keeper, ch, argument);
    return;
  }
  send_to_char("That's not a shopkeeper", ch);
  return;
}

SHOP_FUN *shop_lookup(const char *name)
{
  int cmd;

  for (cmd = 0; *shop_table[cmd].shop_name; cmd++)
    if (!str_cmp(name, shop_table[cmd].shop_name))
      return shop_table[cmd].shop_fun;
  return 0;
}

char *shop_string(SHOP_FUN *fun)
{
  int cmd;

  for (cmd = 0; *shop_table[cmd].shop_fun; cmd++)
    if (fun == shop_table[cmd].shop_fun)
      return shop_table[cmd].shop_name;

  return 0;
}

void shopspec_generic(CHAR_DATA *keeper, CHAR_DATA *ch, char *argument)
{
  return;
}

void shopspec_healer(CHAR_DATA *keeper, CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument(argument, arg);

  if (!str_cmp(arg, "list"))
  {
    do_say(keeper, "Welcome traveller, may I be of some service?");
    sprintf(buf, "\n\r%s offers the following spells:\n\r\n\r",
      keeper->short_descr);
    send_to_char(buf, ch);
    send_to_char("* Healing       :    10 qps.\n\r", ch);
    send_to_char("* Darkbless     :    25 qps.\n\r", ch);
    return;
  }
  else if (!str_cmp(arg, "buy"))
  {
    char item[MAX_INPUT_LENGTH];

    one_argument(argument, item);

    if (!str_cmp(item, "healing"))
    {
      int cost = 10;

      if (ch->pcdata->quest < cost)
      {
        do_say(keeper, "You don't have enough questpoints");
        return;
      }
      ch->hit += number_range(1000, 2000);
      if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
      ch->mana += number_range(1000, 2000);
      if (ch->mana > ch->max_mana) ch->mana = ch->max_mana;
      ch->move += number_range(1000, 2000);
      if (ch->move > ch->max_move) ch->move = ch->max_move;
      do_say(keeper, "May you wounds heal and your heart grow strong");
      ch->pcdata->quest -= cost;
      WAIT_STATE(ch, 6);
    }
    else if (!str_cmp(item, "darkbless"))
    {
      AFFECT_DATA af;
      int sn, cost = 25;

      if (ch->pcdata->quest < cost)
      {
        do_say(keeper, "You don't have enough questpoints");
        return;
      }
      if ((sn = skill_lookup("darkblessing")) < 1)
      {
        do_say(keeper, "Theres something wrong with my magic, please tell an immortal about this");
        return;
      }
      if (is_affected(ch, sn ))
      {
        do_say(keeper, "You are already blessed");
        return;
      }

      af.type      = sn;
      af.duration  = 50;
      af.location  = APPLY_HITROLL;
      af.modifier  = keeper->level/10;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      af.location  = APPLY_DAMROLL;
      af.modifier  = keeper->level/10;
      affect_to_char(ch, &af);

      do_say(keeper, "A blessing on you my child");
      ch->pcdata->quest -= cost;
      return;
    }
    else
    {
      do_say(keeper, "I'm afraid I don't sell that here.");
      return;
    }
  }
  else
  {
    do_say(keeper, "I'm afraid I don't understand");
    return;
  }
}
