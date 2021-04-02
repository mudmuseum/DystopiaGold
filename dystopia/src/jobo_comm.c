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
 * Communication storage system. Ye Jobo.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void init_channel_storage()
{
  int i, j;

  for (i = 1; i < STORE_MAX; i++)
  {
    for (j = 1; j <= MAX_STORAGE; j++)
    {
      old_messages[i].last[j] = str_dup("");
    }
  }
}

void update_channel_storage(char *buf, int chan)
{
  char xbuf[MAX_STRING_LENGTH];
  char tbuf[20];
  int i;

  if (chan != STORE_TELL)
  {
    sprintf(tbuf, "%-18.18s", buf);
    sprintf(xbuf, "%s %s", tbuf, line_indent(&buf[19], 15, 85));
  }
  else sprintf(xbuf, buf);

  if (chan >= STORE_MAX)
  {
    bug("update_channel_storage : Bad channel", 0);
    return;
  }

  for (i = MAX_STORAGE; i > 1; i--)
  {
    free_string(old_messages[chan].last[i]);
    old_messages[chan].last[i] = str_dup(old_messages[chan].last[i-1]);
  }
  free_string(old_messages[chan].last[1]);
  old_messages[chan].last[1] = str_dup(xbuf);
}

void do_lastmessage(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];

  one_argument(argument, arg);

  if (IS_NPC(ch)) return;
  if (!str_cmp(arg, "chat")) showchannel(ch, STORE_CHAT);
  else if (!str_cmp(arg, "newbie")) showchannel(ch, STORE_NEWBIE);
  else if (!str_cmp(arg, "flame")) showchannel(ch, STORE_FLAME);
  else if (!str_cmp(arg, "roleplay")) showchannel(ch, STORE_ROLEPLAY);
  else if (!str_cmp(arg, "tell") && ch->level > 6) showchannel(ch, STORE_TELL);
  else if (ch->level < 7) send_to_char("Show the last chat/newbie/flame/roleplay messages ?\n\r", ch);
  else send_to_char("Show the last chat/newbie/flame/roleplay/tell messages ?\n\r", ch);
  return;
}

void showchannel(CHAR_DATA *ch, int chan)
{
  char buf[MAX_STRING_LENGTH];
  int i;

  for (i = MAX_STORAGE; i > 0; i--)
  {
    if (strlen(old_messages[chan].last[i]) < 1) continue;
    sprintf(buf, "%s\n\r", old_messages[chan].last[i]);
    send_to_char(buf, ch);
  }
}

void talk_channel( CHAR_DATA *ch, char *argument, int channel, int sub_channel, const char *verb )
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if (!IS_NPC(ch) && getMight(ch) < 150 &&
     (channel == CHANNEL_CHAT || channel == CHANNEL_MUSIC || channel == CHANNEL_YELL))
  {
    send_to_char("Newbies are restricted to use the newbie channel.\n\r",ch);
    return;
  }
  if (!IS_NPC(ch) && (get_age(ch) - 17) < 2 && (channel == CHANNEL_FLAME || channel == CHANNEL_ROLEPLAY))
  {
    send_to_char("You must be at least 4 hours old to use this channel.\n\r", ch);
    return;
  }
  if (RTIMER(ch->in_room, RTIMER_SILENCE) != 0)
  {
    send_to_char("Something prevents you from speaking in this room.\n\r", ch);
    return;
  }
  if ( argument[0] == '\0' )
  {
    sprintf( buf, "%s what?\n\r", verb );
    buf[0] = UPPER(buf[0]);
    send_to_char(buf,ch);
    return;
  }
  if ( IS_HEAD(ch, LOST_TONGUE) )
  {
    sprintf( buf, "You can't %s without a tongue!\n\r", verb );
    send_to_char( buf, ch );
    return;
  }
  if ( IS_EXTRA(ch, GAGGED) )
  {
    sprintf( buf, "You can't %s with a gag on!\n\r", verb );
    send_to_char( buf, ch );
    return;
  }
  REMOVE_BIT(ch->deaf, channel);
  switch ( channel )
  {
    default:
      if (ch->flag4 == 1)
      {
        sprintf( buf, "You whine '#1%s#n'.\n\r",  argument );
        send_to_char( buf, ch );
        sprintf( buf, "%s #Pwhines#n '#1$t#n'.",ch->name );
      }
      else if (channel == CHANNEL_FLAME)
      {
        sprintf( buf, "You %s '#G%s#n'.\n\r", verb, argument );
        send_to_char( buf, ch );
        sprintf( buf, "%s %ses '#G$t#n'.",ch->name, verb );
      }
      else if (channel == CHANNEL_CHAT)
      {
        sprintf( buf, "You %s '#1%s#n'.\n\r", verb, argument );
        send_to_char( buf, ch );
        if (ch->trust > 6)
          sprintf( buf, "#y(#G*#y)#C%s#y(#G*#y)#n '#1$t#n'.", ch->name);
        else if (IS_NPC(ch))
          sprintf( buf, "%s chats '#1$t#n'.", ch->short_descr);
        else if (IS_CLASS(ch, CLASS_WEREWOLF))
          sprintf( buf, "%s barks '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_MAGE))
          sprintf( buf, "%s chants '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_SHAPESHIFTER))
          sprintf( buf, "%s whispers '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_VAMPIRE))
          sprintf( buf, "%s snarls '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_DRONE))
          sprintf( buf, "%s drools '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_ANGEL))
          sprintf( buf, "%s preaches '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_TANARRI))
          sprintf( buf, "%s booms '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_LICH))
          sprintf( buf, "%s squicks '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_HOBBIT))
          sprintf( buf, "%s burps '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_DEMON))
          sprintf( buf, "%s growls '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_DROID))
          sprintf( buf, "%s chitters '#1$t#n'.",ch->name);
        else if (IS_CLASS(ch, CLASS_GIANT))
          sprintf( buf, "%s rumbles '#1$t#n'.",ch->name);
        else  
          sprintf( buf, "%s %ss '#1$t#n'.", ch->name, verb );
      }
      else
      {
        sprintf( buf, "You %s '#1%s#n'.\n\r", verb, argument );
        send_to_char( buf, ch );
        sprintf( buf, "%s %ss '#1$t#n'.",ch->name,     verb );
      }
      break;
    case CHANNEL_IMMTALK:
      sprintf( buf, "#y.:#P%s#y:.#C $t.#n",ch->name);
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
    case CHANNEL_CLASS:
      switch(sub_channel)
      {
        case CC_ANGEL:
          sprintf( buf, "#0[#7%s#0]#C '$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_DEMON:
          sprintf( buf, "#0[#R%s#0]#C '$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_MAGE:
          sprintf( buf, "#n{{#0%s#n}}#C '$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_SHIFTER:
          sprintf( buf, "#G*#C(>#R%s#C<)#G* #C'$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_DROID:
          sprintf( buf, "#p{#0-#p}#0%s#p{#0-#p} #C'$t'.#n",ch->name);
          act( buf,ch,argument,NULL,TO_CHAR);
          break;
        case CC_WW:
          sprintf( buf, "#y((#L%s#y))#C '$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_NINJA:
          sprintf(buf, "#y-*(#0%s#y)*-#C '$t'.#n",ch->name );
          act(buf, ch, argument, NULL, TO_CHAR);
          break;
        case CC_SAMURAI:
          sprintf( buf, "#C-=#R%s#C=-  '$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_KNIGHT:
          sprintf( buf, "#0.x.#7%s#0.x.#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_FAE:
          sprintf( buf, "#G>>#R(#y%s#R)#G<<#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_GIANT:
          sprintf( buf, "#G<:>#o%s#G<:>#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_HOBBIT:
          sprintf( buf, "#o(#c*#o)#R%s#o(#c*#o)#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_TANARRI:
          sprintf( buf, "#y{#R%s#y}#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_LICH:
          sprintf( buf, "#G>*<#7%s#G>*<#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_DRONE:
          sprintf( buf, "#G<#0=#y{#0%s#y}#0=#G> #C'$t'.#n", ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_DROW:
          sprintf( buf, "#P.o0#0%s#P0o.#C '$t'.#n",ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_MONK:
          sprintf( buf, "#0.x[#l%s#0]x. #C '$t'.#n",ch->name );
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
        case CC_VAMPIRE:
          sprintf( buf, "#R<<#0%s#R>>#C $t.#n" , ch->name);
          act( buf, ch, argument, NULL, TO_CHAR );
          break;
      }
      break;      
    case CHANNEL_NEWBIE:
      if (getMight(ch) < 150)
        sprintf( buf, "%s the newbie chats #7'#R$t#7'.#n",ch->name);
      else
        sprintf( buf, "%s the newbie helper chats #7'#R$t#7'.#n",ch->name);
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
    case CHANNEL_DKTALK:
      sprintf( buf, "#R*#7*#R* #7%s #R*#7*#R*#7 '$t'.#n",ch->name);
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
    case CHANNEL_ROLEPLAY:
      sprintf( buf, "#o(#R%s#o) #n'#y$t#n'.", ch->name);
      act( buf, ch, argument, NULL, TO_CHAR ); 
      break;
    case CHANNEL_RELIGION:
      sprintf( buf, religion_table[ch->pcdata->religion].channel, ch->name );
      act( buf, ch, argument, NULL, TO_CHAR );
      break;
  }

  /* silenced, and they don't know it :) */
  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SILENCE)) return;

  /* anti spamming */
  if (!IS_NPC(ch) && (channel == CHANNEL_CHAT || channel == CHANNEL_NEWBIE))
  {
    if (!str_cmp(ch->pcdata->last_global, argument)) return;
    free_string(ch->pcdata->last_global);
    ch->pcdata->last_global = str_dup(argument);
  }

  for ( d = descriptor_list; d != NULL; d = d->next )
  {
    CHAR_DATA *gch;

    if (d->connected != CON_PLAYING) continue;
    if ((gch = d->character) == NULL) continue;
    if (gch == ch) continue;
    if (IS_SET(gch->deaf, channel)) continue;

    if (channel == CHANNEL_IMMTALK && !IS_IMMORTAL(gch)) continue;
    if (channel == CHANNEL_CLASS && !IS_IMMORTAL(gch) && (gch->class != ch->class || gch->level < 3))
      continue;
    if (channel == CHANNEL_RELIGION && !IS_IMMORTAL(gch) &&
      gch->pcdata->religion != ch->pcdata->religion) continue;
    if (channel == CHANNEL_YELL && gch->in_room && gch->in_room->area != ch->in_room->area)
      continue;

    if (!IS_NPC(gch) && !IS_NPC(ch))
    {
      bool ignore = FALSE;
      int i;
 
      for (i = 0; i < MAX_IGNORE; i++)
        if (gch->pcdata->ignore[i] == ch->pcdata->playerid) ignore = TRUE;
  
      if (ignore) continue;
    }

    act(buf, ch, argument, gch, TO_VICT);
  }
  return;  
}

void delete_last_line_in_note(CHAR_DATA *ch)
{
  char buf[4 * MAX_STRING_LENGTH];
  char *ptr;
  bool found = FALSE;
  int nCount = 0;
    
  buf[0] = '\0';
   
  if (IS_NPC(ch)) return;
  if (ch->pcdata->in_progress->text == NULL)
  {
    send_to_char("No note to delete lines in.\n\r", ch);
    return;
  }
  if (strlen(ch->pcdata->in_progress->text) < 1)
  {
    send_to_char("Empty note, nothing to delete.\n\r", ch);
    return;
  }
  strcpy(buf, ch->pcdata->in_progress->text);
  ptr = buf;
  while (*ptr != '\0')
  {
    if (*ptr == '\n') nCount++;
    ptr++;
  }
  if (nCount == 1)
  {
    free_string(ch->pcdata->in_progress->text);
    ch->pcdata->in_progress->text = NULL;
    send_to_char("Entire note deleted.\n\r", ch);
    return;
  }
  else
  {
    while (*ptr != '\n' || !found)
    {
      if (*ptr == '\n') found = TRUE;
      ptr--;
    }
  }
  ptr++;
  *ptr = '\0';
  free_string(ch->pcdata->in_progress->text);
  ch->pcdata->in_progress->text = str_dup(buf);
  send_to_char("Line deleted.\n\r", ch);
}

