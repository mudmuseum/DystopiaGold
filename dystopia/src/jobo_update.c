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
#include <unistd.h>
#include <time.h>
#include "merc.h"


void update_trackers()
{
  CHAR_DATA *ch;
  CHAR_DATA *victim;
  char *path;
  char buf[MAX_STRING_LENGTH];
  bool found;

  for (ch = char_list; ch; ch = ch->next)
  {
    if (!IS_NPC(ch)) continue;
    if (!ch->tracking) continue;
    if (ch->hunting == NULL)
    {
      ch->hunting = str_dup("");
      ch->hunt_pointer = 0;
      ch->hunt_playerid = 0;
      ch->tracking = FALSE;
      sprintf(buf, "Update_trackers: %s NULL hunter.", ch->name);
      bug(buf, 0);
      continue;
    }
    if (strlen(ch->hunting) < ch->hunt_pointer)
    {
      sprintf(buf, "%s %d", ch->hunting, ch->hunt_pointer);
      log_string(buf);
      free_string(ch->hunting);
      ch->hunting = str_dup("");
      ch->hunt_pointer = 0;
      ch->hunt_playerid = 0;
      ch->tracking = FALSE;
      sprintf(buf, "Update_trackers: %s out of bound.", ch->name);
      bug(buf, 0);
      continue;
    }
    switch(ch->hunting[ch->hunt_pointer])
    {
      default:
        free_string(ch->hunting);
        ch->hunting = str_dup("");
        ch->hunt_pointer = 0;
        ch->hunt_playerid = 0;
        ch->tracking = FALSE;
        sprintf(buf, "Update_trackers: %s bad direction.", ch->name);
        bug(buf, 0);
        continue;
      case 'n':
        interpret(ch, "north");
        break;
      case 's':
        interpret(ch, "south");
        break;
      case 'e':
        interpret(ch, "east");
        break;
      case 'w':
        interpret(ch, "west");
        break;
      case 'u':
        interpret(ch, "up");
        break;
      case 'd':
        interpret(ch, "down");
        break;
      case '\0':
        found = FALSE;
        for (victim = ch->in_room->people; victim && !found; victim = victim->next_in_room)
        {
          if (IS_NPC(victim)) continue;
          if (victim->pcdata->playerid != ch->hunt_playerid) continue;
          free_string(ch->hunting);
          ch->hunting = str_dup("");
          ch->hunt_pointer = 0;
          ch->hunt_playerid = 0;
          ch->tracking = FALSE;
          found = TRUE;
        }
        if (found) continue;
        for (victim = char_list; victim && !found; victim = victim->next)
        {
          if (IS_NPC(victim)) continue;
          if (victim->pcdata->playerid != ch->hunt_playerid) continue;
          if (!victim->in_room || !ch->in_room) continue;
          if (victim->in_room->area != ch->in_room->area) continue;
          if ((path = pathfind(ch, victim)) == NULL) continue;
          free_string(ch->hunting);
          ch->hunting = str_dup(path);
          ch->hunt_pointer = 0;
          found = TRUE;
        }
        if (found) continue;
        do_say(ch, "I lost the trail, sorry master.");
        free_string(ch->hunting);
        ch->hunting = str_dup("");
        ch->hunt_pointer = 0;
        ch->hunt_playerid = 0;
        ch->tracking = FALSE;
        break;
    }
    ch->hunt_pointer++;
  }
}

void update_edge(CHAR_DATA *ch)
{
  CHAR_DATA *gch;

  for (gch = char_list; gch; gch = gch->next)
  {
    if (IS_NPC(gch)) continue;
    REMOVE_BIT(gch->pcdata->tempflag, TEMP_EDGE);
  }
  SET_BIT(ch->pcdata->tempflag, TEMP_EDGE);
}
