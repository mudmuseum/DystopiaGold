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

void check_effect_pot  args ( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void random_cook_say   args ( ( CHAR_DATA *ch ) );
bool know_ingredient   args ( ( CHAR_DATA *ch, int value ) );

void do_hobbitarmor(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 0;
    
  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_HOBBIT))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Please specify which piece of hobbit armor you wish to make: Knife Mace\n\r"
                 "Bracer Charm Ring Vestment Hat Leggings Boots Gloves Sleeves Cloak Belt Mask Pot.\n\r",ch);
    return;
  }
  if ( ch->practice < 60 )
  {
    send_to_char("It costs 60 points of primal to create hobbit equipment.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"knife")) vnum = 33240;
  else if (!str_cmp(arg,"mace")) vnum = 33241;
  else if (!str_cmp(arg,"bracer")) vnum = 33242;
  else if (!str_cmp(arg,"charm")) vnum = 33243;
  else if (!str_cmp(arg,"ring")) vnum = 33244;
  else if (!str_cmp(arg,"vestment")) vnum = 33245;
  else if (!str_cmp(arg,"hat")) vnum = 33246;
  else if (!str_cmp(arg,"leggings")) vnum = 33247;
  else if (!str_cmp(arg,"boots")) vnum = 33248;
  else if (!str_cmp(arg,"gloves")) vnum = 33249;
  else if (!str_cmp(arg,"sleeves")) vnum = 33250;
  else if (!str_cmp(arg,"cloak")) vnum = 33251;
  else if (!str_cmp(arg,"belt")) vnum = 33252;
  else if (!str_cmp(arg,"mask")) vnum = 33253;
  else if (!str_cmp(arg,"pot")) vnum = 33254;
  else
  {
    do_hobbitarmor(ch,"");
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

void do_bettercook(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  int new_power;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_HOBBIT))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("What secrets of the kitchen would you like to explore? [spice, vegetables, meat, cake]\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"cake")) new_power = HOBBIT_CAKE;
  else if (!str_cmp(arg,"spice")) new_power = HOBBIT_SPICE;
  else if (!str_cmp(arg,"meat")) new_power = HOBBIT_MEAT;
  else if (!str_cmp(arg,"vegetables")) new_power = HOBBIT_VEGETABLES;
  else
  {
    send_to_char("No such dish.\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[new_power] > 3)
  {
    send_to_char("You already know all those secrets.\n\r",ch);
    return;
  }
  if (ch->practice < (ch->pcdata->powers[new_power] + 1) * 40)
  {
    send_to_char("You don't haven enough primal to learn this (40 primal per level).\n\r",ch);
    return;
  }
  ch->pcdata->powers[new_power]++;
  ch->pcdata->powers[HOBBIT_DISHES]++;
  ch->practice -= ch->pcdata->powers[new_power] * 40;
  send_to_char("You learn the ancient secret of a new ingredient.\n\r",ch);
  return;
}

void do_dishes(CHAR_DATA *ch, char *argument)
{
  int i;
  bool found = FALSE;

  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_HOBBIT))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  for (i = 1 ; i <= ch->pcdata->powers[HOBBIT_SPICE]; i++)
  {
    if (i == 1) send_to_char("You have uncovered the ancient secrets of : Pepper",ch);
    if (i == 2) send_to_char(" Chilli",ch);
    if (i == 3) send_to_char(" Garlic",ch);
    if (i == 4) send_to_char(" Curry",ch);
    found=TRUE;
  }
  if (found) send_to_char("\n\r",ch);
  for (i = 1 ; i <= ch->pcdata->powers[HOBBIT_VEGETABLES]; i++)
  {
    if (i == 1) send_to_char("You have uncovered the ancient secrets of : Carrot",ch);
    if (i == 2) send_to_char(" Potato",ch);
    if (i == 3) send_to_char(" Tomato",ch);
    if (i == 4) send_to_char(" Mushroom",ch);
    found=TRUE;
  }
  if (found) send_to_char("\n\r",ch);
  for (i = 1 ; i <= ch->pcdata->powers[HOBBIT_CAKE]; i++)
  {
    if (i == 1) send_to_char("You have uncovered the ancient secrets of : Sugar",ch);
    if (i == 2) send_to_char(" Chocolate",ch);
    if (i == 3) send_to_char(" Cream",ch);
    if (i == 4) send_to_char(" Cherry",ch);
    found=TRUE;
  }
  if (found) send_to_char("\n\r",ch);
  for (i = 1 ; i <= ch->pcdata->powers[HOBBIT_MEAT]; i++)
  {
    if (i == 1) send_to_char("You have uncovered the ancient secrets of : Pork",ch);
    if (i == 2) send_to_char(" Beef",ch);
    if (i == 3) send_to_char(" Herring",ch);
    if (i == 4) send_to_char(" Sausage",ch);
    found=TRUE;
  }
  if (found) send_to_char("\n\r",ch);
  if (!found) send_to_char("You know nothing!\n\r",ch);
  return;
}

void do_cook(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;
  int i = 0, value = 0, j;

  one_argument(argument,arg);

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_HOBBIT))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("Cook what? (ingredient or spill (to empty))\n\r",ch);
    return;
  }
  if ((obj = get_eq_char( ch, WEAR_SPECIAL )) == NULL)
  {
    send_to_char("your not ready to cook, get your pot out first.\n\r",ch);
    return;
  }
  if (obj->item_type != ITEM_COOKINGPOT)
  {
    send_to_char("your not ready to cook, get your pot out first.\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"spill"))
  {
    obj->value[0]=0;
    obj->value[1]=0;
    obj->value[2]=0;
    obj->value[3]=0;
    send_to_char("You spill the contents of the pot onto the floor!\n\r",ch);
    return;
  }
  if (ch->mana < 100)
  {
    send_to_char("You don't have enough mana to summon the blessings of the great chef.\n\r",ch);
    return;
  }
  if (ch->move < 100)
  {
    send_to_char("Your to tired to cook anything.\n\r",ch);
    return;
  }
  while (obj->value[i] != 0 && i < 4) i++;
  if (obj->value[i] == 0)
  {
    /* Bits, so the order in which we cook doesn't matter :) */
    if (!str_cmp(arg,"pepper")) value = 1;
    else if (!str_cmp(arg,"chilli")) value = 2;
    else if (!str_cmp(arg,"curry")) value = 4;
    else if (!str_cmp(arg,"garlic")) value = 8;
    else if (!str_cmp(arg,"beef")) value = 16;
    else if (!str_cmp(arg,"pork")) value = 32;
    else if (!str_cmp(arg,"sausage")) value = 64;
    else if (!str_cmp(arg,"herring")) value = 128;
    else if (!str_cmp(arg,"cream")) value = 256;
    else if (!str_cmp(arg,"sugar")) value = 512;
    else if (!str_cmp(arg,"chocolate")) value = 1024;
    else if (!str_cmp(arg,"cherry")) value = 2048;
    else if (!str_cmp(arg,"potato")) value = 4096;
    else if (!str_cmp(arg,"carrot")) value = 8192;
    else if (!str_cmp(arg,"mushroom")) value = 16384;
    else if (!str_cmp(arg,"tomato")) value = 32768;
    else
    {
      send_to_char("You know of no such ingredience.\n\r",ch);
      return;
    }
    if (!know_ingredient(ch,value)) return;
    for (j = 0; j < i; j++)
    {
      if (obj->value[j] == value)
      {
        send_to_char("That's already in the pot.\n\r", ch);
        return;
      }
    }
    obj->value[i] = value;
    sprintf(buf,"You throw the strange and exotic ingredient known as #s%s#n into the pot.\n\r", arg);
    send_to_char(buf,ch);
    ch->move -= 100;
    ch->mana -= 100;
    if (i == 3) check_effect_pot(ch,obj);
  }
  else
  {
    do_say(ch, "Hmmm, there really isn't any space in this pot for anything else, I'd better empty it.");
    do_cook(ch, "spill");
  }
  return;
}

void random_cook_say(CHAR_DATA *ch)
{
 int i = number_range(1,4);

  switch (i)
  {
    default: do_say(ch, "ooOOoo, I bet this will taste just wonderful!"); break;
    case (1): do_say(ch,"There can only be one..... And I'm Da Cook!");break;
    case (2): do_say(ch,"Get out of my kitchen or....");break;
    case (3): do_say(ch,"Boggles, wonder how this will taste");break;
    case (4): do_say(ch,"Soup?!?!?! This is no stinking soup, it's a super deluxe pot o'soup!");break;
    case (5): do_say(ch," ");break;
  }
  return;
}

void check_effect_pot(CHAR_DATA *ch, OBJ_DATA *obj)
{
  CHAR_DATA *victim;
  int i=0;
  int sn;
  int level = ch->pcdata->powers[HOBBIT_DISHES]*3;

  if (obj->pIndexData->vnum == 33255) // mastery item
    level += 15;

  // empty pot and get the number
  i += obj->value[0]; obj->value[0]=0;
  i += obj->value[1]; obj->value[1]=0;
  i += obj->value[2]; obj->value[2]=0;
  i += obj->value[3]; obj->value[3]=0;

  if (i == 16772) // food frenzy : mushroom, herring, cream, curry
  {
    victim = ch;
    sn = skill_lookup("food frenzy");
    if (sn  > 0) (*skill_table[sn].spell_fun) (sn,level,ch,victim);
    do_say(ch,"Fooooood, I need foooood!");
  }
  else if (i == 10273) // Clay ball : pepper carrot cherry pork
  {
    do_say(ch,"Ick, this is all sticky, here hold this.");
    if ((victim = ch->fighting) == NULL)
    {
      send_to_char("Noone is close enough to hold it.\n\r",ch);
      return;
    }
    sn = skill_lookup("clay ball");
    if (sn  > 0) (*skill_table[sn].spell_fun) (sn,level,ch,victim);
    WAIT_STATE(ch,8);
    return;
  }
  else if (i == 34834) // Sweet dreams : chilli tomato cherry beef
  {
    do_say(ch,"oooOOOooo, this rocks, try and give it a taste, I've never tasted anything like it");
    if ((victim = ch->fighting) == NULL)
    {
      send_to_char("Noone is close enough to taste it.\n\r",ch);
      return;
    }
    if (number_range(1,3)==2)
    {
      send_to_char("They refuse to take a sip, what a pansy.\n\r",ch);
      return;
    }
    send_to_char("Damn this tastes good, better sheath those weapons so you can get a better grip on the spoon.\n\r",victim);
    do_say(victim,"oooOOOooo, your right, it really tastes good");
    do_sheath(victim,"both");
    WAIT_STATE(victim,12);
    WAIT_STATE(ch,18);
    return;
  }
  else if (i == 17473) // Burning mocha : pepper mushroom chocolate sausage
  {
    if ((victim = ch->fighting) == NULL)
    {
      send_to_char("You spill the burning mocha on the floor, causing no damage.\n\r",ch);
      return;
    }
    WAIT_STATE(ch, 8);
    do_say(ch,"Take a sip of this!!!");
    one_hit(ch, victim, gsn_mocha, 1);
    one_hit(ch, victim, gsn_mocha, 1);
    if (level > 40) one_hit(ch, victim, gsn_mocha, 0);
    if (level > 50) one_hit(ch, victim, gsn_mocha, 0);
    if (victim == NULL) return;
    if (!IS_SET(victim->affected_by, AFF_FLAMING))
    {
      send_to_char("The burning mocha starts a fire.\n\r",ch);
      SET_BIT(victim->affected_by, AFF_FLAMING);
    }
    return;
  }
  else if (i == 4488) // Potato Grenade : garlic potato cream herring
  {
    CHAR_DATA *victim_next;
    int i;

    victim_next = ch->in_room->people;
    victim = victim_next;
    do_say(ch,"Eat my potato grenades, they are tasty as HeLL!!");
    while (victim_next != NULL)
    {
      if (level > 40) i = number_range(1,3);
      else i = number_range(1,2);
      victim_next = victim->next_in_room;
      if (IS_NPC(victim))
      {
         if (is_safe(ch,victim)) break;
         while (i-- > 0) one_hit(ch,victim,gsn_potato,1);
      }
      victim = victim_next;
    }
    WAIT_STATE(ch,14);
  }
  else if (i == 8516) // Foggie Woggie : curry carrot cream sausage
  {
    WAIT_STATE(ch, 24);
    do_say(ch,"Foggie Woggie, this sure is steamy");
    if (number_range(1,3) == 2) return;
    if (ch->in_room != NULL)
    {
      if (IS_SET (ch->in_room->room_flags, ROOM_ARENA))
      {
        send_to_char("Your in the arena.\n\r",ch);
        return;
      }
    }
    if (in_fortress(ch))
    {
      send_to_char("Your in The Forbidden Fortress.\n\r", ch);
      return;
    }
    act("A cloud of steam erupts from the pot, and when it disappears $n is gone.",ch,NULL,NULL,TO_ROOM);
    stc("You are swallowed by the pot.\n\r",ch);
    if (ch->fighting != NULL) stop_fighting( ch, TRUE );
    char_from_room(ch);
    char_to_room(ch, get_random_room(ch));
    stc("You stick your head out of the pot and is somewhere else.\n\r",ch);
    act("$n appears from inside a cooking pot.",ch,NULL,NULL,TO_ROOM);
    do_look(ch,"auto");
    return;
  }
  else if (i == 33314) // Hot Bread : chilli tomato sugar pork
  {
    do_say(ch,"Sweet Bread... yummi... wait, this is hot breads, oh well...");
    ch->hit += number_range(10*level, 20*level);
    if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
    send_to_char("You feel much better.\n\r",ch);
    WAIT_STATE(ch,8);
    return;
  }
  else if (i == 4632) // Stinky stinky : garlic potato sugar beef
  {
    if (ch->in_room == NULL) return;
    do_say(ch,"Stinky Stinky, I can't eat this... try and smell it people");
    for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
    {
      if (victim == ch) continue;
      if (is_safe(ch,victim)) continue;
      if (victim->position != POS_STANDING && number_range(1,4)!=2) continue;
      send_to_char("You take one sniff of the smelly stuff in the pot, and drop to the ground.\n\r",victim);
      act("$n's eyes turns blank, and $e drops to the ground!",victim,NULL,NULL,TO_ROOM);
      stop_fighting(victim, TRUE);
      victim->position = POS_SLEEPING;
    }
    WAIT_STATE(ch,16);
    return;
  }
  else
  {
    send_to_char("nothing happens.\n\r",ch);
    random_cook_say(ch);
  }
  return;
}

bool know_ingredient(CHAR_DATA *ch, int value)
{
  bool can_cook = TRUE;

  if (value == 1 && ch->pcdata->powers[HOBBIT_SPICE] < 1) can_cook = FALSE;
  if (value == 2 && ch->pcdata->powers[HOBBIT_SPICE] < 2) can_cook = FALSE;
  if (value == 4 && ch->pcdata->powers[HOBBIT_SPICE] < 4) can_cook = FALSE;
  if (value == 8 && ch->pcdata->powers[HOBBIT_SPICE] < 3) can_cook = FALSE;
  if (value == 16 && ch->pcdata->powers[HOBBIT_MEAT] < 2) can_cook = FALSE;
  if (value == 32 && ch->pcdata->powers[HOBBIT_MEAT] < 1) can_cook = FALSE;
  if (value == 64 && ch->pcdata->powers[HOBBIT_MEAT] < 4) can_cook = FALSE;
  if (value == 128 && ch->pcdata->powers[HOBBIT_MEAT] < 3) can_cook = FALSE;
  if (value == 256 && ch->pcdata->powers[HOBBIT_CAKE] < 3) can_cook = FALSE;
  if (value == 512 && ch->pcdata->powers[HOBBIT_CAKE] < 1) can_cook = FALSE;
  if (value == 1024 && ch->pcdata->powers[HOBBIT_CAKE] < 2) can_cook = FALSE;
  if (value == 2048 && ch->pcdata->powers[HOBBIT_CAKE] < 4) can_cook = FALSE;
  if (value == 4096 && ch->pcdata->powers[HOBBIT_VEGETABLES] < 2) can_cook = FALSE;
  if (value == 8192 && ch->pcdata->powers[HOBBIT_VEGETABLES] < 1) can_cook = FALSE;
  if (value == 16384 && ch->pcdata->powers[HOBBIT_VEGETABLES] < 4) can_cook = FALSE;
  if (value == 32768 && ch->pcdata->powers[HOBBIT_VEGETABLES] < 3) can_cook = FALSE;
  if (!can_cook) send_to_char("You don't know how to do that.\n\r",ch);
  return can_cook;
}

const struct flag_type hobbit_dishes[] =
{
  { "empty",         0,     TRUE },
  { "pepper",        1,     TRUE },
  { "chilli",        2,     TRUE },
  { "curry",         4,     TRUE },
  { "garlic",        8,     TRUE },
  { "beef",         16,     TRUE },
  { "pork",         32,     TRUE },
  { "sausage",      64,     TRUE },
  { "herring",     128,     TRUE },
  { "cream",       256,     TRUE },
  { "sugar",       512,     TRUE },
  { "chocolate",  1024,     TRUE },
  { "cherry",     2048,     TRUE },
  { "potato",     4096,     TRUE },
  { "carrot",     8192,     TRUE },
  { "mushroom",  16384,     TRUE },
  { "tomato",    32768,     TRUE },
  { "",              0,        0 }
};
