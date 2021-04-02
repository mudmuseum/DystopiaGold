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

/*
 * A quick hack to get support for the mud sound protocol by Jobo.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#define SOUND_URL  "http://www.daimi.au.dk/~jobo/dystopia/download/sounds/"

void do_sound(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;

  if (IS_SET(ch->act, PLR_SOUND))
  {
    REMOVE_BIT(ch->act, PLR_SOUND);
    send_to_char("Sound disabled.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->act, PLR_SOUND);
    send_to_char("Sound enabled.\n\r", ch);
  }
  return;
}

void send_portal(CHAR_DATA *ch)
{
  char buf[200];

  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(portal.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_redspell(CHAR_DATA *ch)
{
  char buf[200];   

  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(red.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_purplespell(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(purple.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_greenspell(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(green.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_yellowspell(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(yellow.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_bluespell(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(blue.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_howl(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(howl.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_detonate(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(detonate.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_eagle(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(eagle.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_thunder(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(thunder.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_teleport(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(teleport.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_decap(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(decap.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}

void send_losthead(CHAR_DATA *ch)
{
  char buf[200];     
  
  if (!IS_SET(ch->act, PLR_SOUND)) return;
  sprintf(buf, "!!SOUND(losthead.wav U=%s)", SOUND_URL);
  send_to_char(buf, ch);
  return;
}
