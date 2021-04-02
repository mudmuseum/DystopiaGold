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


AREA_AFFECT * area_affect_free = NULL;
POLL_DATA   * poll_list        = NULL;
CHANGE_DATA * change_list      = NULL;
CHANGE_DATA * change_last      = NULL;
CHANGE_DATA * change_free      = NULL;

bool gFound;

bool examine_room   args (( ROOM_INDEX_DATA *pRoom, ROOM_INDEX_DATA *tRoom, AREA_DATA *pArea, int steps ));
void examine_steps  args (( ROOM_INDEX_DATA *pRoom, ROOM_INDEX_DATA *tRoom, AREA_DATA *pArea, int steps ));

/*
 * Is astr contained within bstr ?
 *
 */
bool is_contained( const char *astr, const char *bstr )
{
  int i, alen, blen, count;
   
  alen = strlen(astr);
  blen = strlen(bstr);
  if (alen > blen) return FALSE;
  for (i = 0; i <= (blen - alen); i++)
  {
    count = 0;
    while (count < alen && UPPER(astr[count]) == UPPER(bstr[i+count])) count++;
    if (count == alen) return TRUE;
  }
  return FALSE;
}

/*
 * A case sensitive version
 */
bool is_contained2( const char *astr, const char *bstr )
{
  int i, alen, blen, count;

  alen = strlen(astr);
  blen = strlen(bstr);
  if (alen > blen) return FALSE;
  for (i = 0; i <= (blen - alen); i++)
  {
    count = 0;
    while (count < alen && astr[count] == bstr[i+count]) count++;
    if (count == alen) return TRUE;
  }
  return FALSE;
}

int strlen2(const char *s)
{
  int i, b, count=0;

  if (s[0] == '\0') return 0;
  b = strlen(s);
  for (i = 0; i < b; i++)
  {
    if (s[i] == '#') count++;
  }
  return (b + 7 * count);
}

char *get_exits(CHAR_DATA *ch)
{
  extern char *const dir_name[]; 
  static char buf[MAX_STRING_LENGTH];
  EXIT_DATA *pexit;
  bool found;
  int door;

  if (!check_blind(ch)) return "";

  buf[0] = '\0';    
  sprintf(buf, "#0[#GExits#9:#C");
     
  found = FALSE;
  for (door = 0; door <= 5; door++)
  { 
    if ((pexit = ch->in_room->exit[door]) != NULL && pexit->to_room != NULL)
    {
      found = TRUE;
      if (pexit && IS_SET(pexit->exit_info, EX_CLOSED))
      {
        strcat(buf, " #0(#C");
        strcat(buf, dir_name[door]);
        strcat(buf, "#0)#C");
      }   
      else       
      {
        strcat(buf, " ");
        strcat(buf, dir_name[door]);
      }
    }
  }
  if (!found)
    strcat(buf, " none");
  strcat(buf, "#0]#n\n\r");
  
  return buf;
}

void win_prize( CHAR_DATA *ch )
{
  int i,vnum;
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pIndex;
  
  if (IS_NPC(ch)) return;
  i = number_range(1,100);
  if (i < 35) vnum = OBJ_VNUM_PROTOPLASM;
  else if (i < 40) vnum = 33840;
  else if (i < 45) vnum = 33841;
  else if (i < 50) vnum = 30052;
  else if (i < 65) vnum = 33851;
  else if (i < 75) vnum = 33852;
  else if (i < 80) vnum = 33853;
  else if (i < 85) vnum = 33854;
  else if (i < 90) vnum = 33855;
  else if (i < 95) vnum = 33856;
  else vnum = 33857;
  if ((pIndex = get_obj_index(vnum)) == NULL)
  {
    bug("BAD PRIZE!!",0);
    return;
  }
  obj = create_object(pIndex, 50);
  if (vnum == OBJ_VNUM_PROTOPLASM)
  {
    obj->level = 1;
    free_string(obj->short_descr);
    free_string(obj->name);
    free_string(obj->description);
    obj->short_descr = str_dup("A prize token");
    obj->description = str_dup("A token lies on the floor");
    obj->name = str_dup("prize token");
    obj->value[0] = number_range(100,300);
    obj->item_type = ITEM_QUEST;
  }
  obj_to_char(obj,ch);
  return;
}

void do_clearstats2( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    if (IS_NPC(ch)) return;

    powerdown(ch); /* remove class shit */

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->wear_loc != WEAR_NONE )
        {obj_from_char(obj); obj_to_char(obj,ch);}
    }
     
    while ( ch->affected )
      affect_remove( ch, ch->affected );
      
    if (IS_SET(ch->affected_by, AFF_POLYMORPH)) REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    if (IS_SET(ch->affected_by, AFF_ETHEREAL)) REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);  
    if (IS_EXTRA(ch,EXTRA_DRAGON)) REMOVE_BIT(ch->extra, EXTRA_DRAGON);
    
    ch->affected_by      = 0;
    ch->armor            = 100;
    ch->hit              = UMAX( 1, ch->hit  );
    ch->mana             = UMAX( 1, ch->mana );
    ch->move             = UMAX( 1, ch->move );
    ch->hitroll          = 0;
    ch->damroll          = 0;
    ch->saving_throw     = 0;
    ch->pcdata->mod_str  = 0;
    ch->pcdata->mod_int  = 0;
    ch->pcdata->mod_wis  = 0;
    ch->pcdata->mod_dex  = 0;
    ch->pcdata->mod_con  = 0;
    ch->pcdata->followers = 0;
    if (IS_SET(ch->newbits, NEW_DFORM)) REMOVE_BIT(ch->newbits, NEW_DFORM);
    if (IS_POLYAFF(ch, POLY_ZULOFORM)) REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
    if (IS_SET(ch->newbits, NEW_CUBEFORM)) REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
    save_char_obj( ch );
    send_to_char("Your stats have been cleared.  Please rewear your equipment.\n\r",ch);
    return;
}

/*
 *  If an item have a bad char in it's short/long/name desc, the mud
 *  will freeze when the char with that item tries to log on, so use
 *  this check on any renaming of items (glamour/reshape/etc).
 */
bool has_bad_chars(CHAR_DATA *ch, char *argument)
{
  int i;
  
  if ( argument[0] == '\0' ) return FALSE;
  for (i = 0; argument[i] != '\0' ; i++)  
  {
    if (argument[i] == '~') return TRUE;
  }
  return FALSE;
}

bool is_webbed(CHAR_DATA *ch)
{
  if (IS_AFFECTED(ch, AFF_WEBBED))
  {
    send_to_char("Not while webbed.\n\r", ch);
    return TRUE;
  }
  if (IS_AFFECTED(ch, AFF_THORNS))
  {
    send_to_char("Not while rooted by brambles.\n\r", ch);
    return TRUE;
  }
  return FALSE;
}

void ragnarok_stop()
{
  DESCRIPTOR_DATA *d;
  
  ragnarok = FALSE;
  do_info(NULL,"#CPeace has been restored in the realms, the time of ragnarok is no more#n");
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->character && d->connected == CON_PLAYING)
    {
      d->character->fight_timer = 0;
      d->character->pcdata->safe_counter = 5;
      do_call(d->character,"all");
      do_restore(d->character,"self");
    }
  }
  return;
}

void logout_message(CHAR_DATA *ch)
{
  static char * const he_she  [] = { "XX", "he",  "she" };
  static char * const him_her [] = { "XX", "him", "her" };
  static char * const his_her [] = { "XX", "his", "her" };

  DESCRIPTOR_DATA *d;
  char buf[400]; // that should be plenty.
  const char *dmess;
  const char *i;
  char *ptr2;
  char *ptr;
  int size;

  size = strlen2(ch->pcdata->logoutmessage);
  if (size > 380)
  {
    bug("Bad logoutmessage.",0);
    return;
  }
    
  ptr2  = "#C<- #RLeaves #C->#n ";
  ptr   = buf;
  dmess = ch->pcdata->logoutmessage;
      
  while ((*ptr = *ptr2) != '\0')
    ++ptr, ++ptr2;

  while (*dmess != '\0')
  {
    if ( *dmess != '$' )
    {
      *ptr++ = *dmess++;
      continue;
    }
    ++dmess;
    switch (*dmess)
    {
      default:  i = ""; break;
      case 'n': i = ch->name; break;
      case 'e': i = he_she  [URANGE(1, ch->sex, 2)]; break;
      case 'm': i = him_her [URANGE(1, ch->sex, 2)]; break;
      case 's': i = his_her [URANGE(1, ch->sex, 2)]; break;
    }
    ++dmess;
    /* copying the data into the pointer */
    while ((*ptr = *i) != '\0')
      ++ptr, ++i;
  }
  *ptr++ = '\n';
  *ptr++ = '\r';  

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->lookup_status != STATUS_DONE) continue;
    if (d->connected != CON_PLAYING ) continue;
    write_to_buffer( d, buf, ptr - buf );
  }
  return;
}

void tie_message(CHAR_DATA *ch, CHAR_DATA *victim)
{
  static char * const he_she  [] = { "XX", "he",  "she" };
  static char * const him_her [] = { "XX", "him", "her" };
  static char * const his_her [] = { "XX", "his", "her" };
  DESCRIPTOR_DATA *d;
  char buf[400]; // that should be plenty.
  const char *dmess;
  const char *i;
  char *ptr2;
  char *ptr;
  int size;

  size = strlen2(ch->pcdata->decapmessage);
  if (size > 380)
  {
    bug("Bad tiemessage.",0);
    return;
  }

  ptr2  = "#C<- #RTie #C->#n ";
  ptr   = buf;
  dmess = ch->pcdata->tiemessage;  

  while ((*ptr = *ptr2) != '\0')
    ++ptr, ++ptr2;

  while (*dmess != '\0')
  {
    if ( *dmess != '$' )
    {
      *ptr++ = *dmess++;
      continue;
    }
    ++dmess;
    switch (*dmess)
    {
      default:  i = ""; break;
      case 'n': i = ch->name; break;
      case 'e': i = he_she  [URANGE(1, ch->sex, 2)]; break;
      case 'm': i = him_her [URANGE(1, ch->sex, 2)]; break;
      case 's': i = his_her [URANGE(1, ch->sex, 2)]; break;
      case 'N': i = victim->name; break;
      case 'S': i = his_her [URANGE(1, victim->sex, 2)]; break;
      case 'M': i = him_her [URANGE(1, victim->sex, 2)]; break;
      case 'E': i = he_she  [URANGE(1, victim->sex, 2)]; break;
    }
    ++dmess;
    /* copying the data into the pointer */
    while ((*ptr = *i) != '\0')
      ++ptr, ++i;
  }
  *ptr++ = '\n';
  *ptr++ = '\r';

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->lookup_status != STATUS_DONE) continue;
    if (d->connected != CON_PLAYING ) continue;
    write_to_buffer( d, buf, ptr - buf );
  }
  return;
}

void login_message(CHAR_DATA *ch)
{
  static char * const he_she  [] = { "XX", "he",  "she" };
  static char * const him_her [] = { "XX", "him", "her" };
  static char * const his_her [] = { "XX", "his", "her" };

  DESCRIPTOR_DATA *d;
  char buf[400]; // that should be plenty. 
  const char *dmess;
  const char *i; 
  char *ptr2;
  char *ptr;
  int size;
    
  size = strlen2(ch->pcdata->loginmessage); 
  if (size > 380)
  {
    bug("Bad loginmessage.",0);
    return;
  }

  ptr2  = "#C<- #REnters #C->#n ";
  ptr   = buf;
  dmess = ch->pcdata->loginmessage;

  while ((*ptr = *ptr2) != '\0')
    ++ptr, ++ptr2;
   
  while (*dmess != '\0')
  {
    if ( *dmess != '$' )
    {
      *ptr++ = *dmess++;
      continue;
    }
    ++dmess;
    switch (*dmess)
    {
      default:  i = ""; break;
      case 'n': i = ch->name; break;
      case 'e': i = he_she  [URANGE(1, ch->sex, 2)]; break;
      case 'm': i = him_her [URANGE(1, ch->sex, 2)]; break;
      case 's': i = his_her [URANGE(1, ch->sex, 2)]; break;
    }
    ++dmess;
    /* copying the data into the pointer */
    while ((*ptr = *i) != '\0')
      ++ptr, ++i;
  }
  *ptr++ = '\n';
  *ptr++ = '\r';
    
  for (d = descriptor_list; d; d = d->next)
  {
    if (d->lookup_status != STATUS_DONE) continue;
    if (d->connected != CON_PLAYING ) continue;
    write_to_buffer( d, buf, ptr - buf );
  }
  return;
}

void avatar_message(CHAR_DATA *ch)
{
  static char * const he_she  [] = { "XX", "he",  "she" };
  static char * const him_her [] = { "XX", "him", "her" };
  static char * const his_her [] = { "XX", "his", "her" };

  DESCRIPTOR_DATA *d;   
  char buf[400]; // that should be plenty.
  const char *dmess;
  const char *i;
  char *ptr2;
  char *ptr;
  int size;
      
  size = strlen2(ch->pcdata->avatarmessage);
  if (size > 380)
  {
    bug("Bad loginmessage.",0);
    return;
  }
  
  ptr2  = "#C<- #RAvatar #C->#n ";
  ptr   = buf;
  dmess = ch->pcdata->avatarmessage;
   
  while ((*ptr = *ptr2) != '\0')
    ++ptr, ++ptr2;
    
  while (*dmess != '\0')
  {
    if ( *dmess != '$' )
    {
      *ptr++ = *dmess++;
      continue;
    }
    ++dmess;
    switch (*dmess) 
    {
      default:  i = ""; break;
      case 'n': i = ch->name; break;
      case 'e': i = he_she  [URANGE(1, ch->sex, 2)]; break;
      case 'm': i = him_her [URANGE(1, ch->sex, 2)]; break;
      case 's': i = his_her [URANGE(1, ch->sex, 2)]; break;
    }
    ++dmess;
    /* copying the data into the pointer */
    while ((*ptr = *i) != '\0')
      ++ptr, ++i;
  }
  *ptr++ = '\n';
  *ptr++ = '\r';
   
  for (d = descriptor_list; d; d = d->next)
  {
    if (d->lookup_status != STATUS_DONE) continue;
    if (d->connected != CON_PLAYING ) continue;
    write_to_buffer( d, buf, ptr - buf );
  }
  return;
}

void powerdown(CHAR_DATA *ch)
{
  if (IS_NPC(ch)) return;

  if (IS_CLASS(ch, CLASS_DEMON))
  {
    if (IS_POLYAFF(ch, POLY_ZULOFORM))  /* demonform */
    {
      REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      free_string( ch->morph );
      ch->morph = str_dup( "" );
    }
  }
  else if (IS_CLASS(ch, CLASS_MONK)) /* spiritpower and monkflame */
  {
    if (IS_SET(ch->newbits, NEW_MONKFLAME)) REMOVE_BIT(ch->newbits, NEW_MONKFLAME);
    if (IS_SET(ch->newbits, NEW_POWER))     REMOVE_BIT(ch->newbits, NEW_POWER);
  }
  else if (IS_CLASS(ch, CLASS_WEREWOLF))
  {
    if (IS_SET(ch->special, SPC_WOLFMAN))
      do_unwerewolf(ch, "");
  }
  else if (IS_CLASS(ch, CLASS_DROW))
  {
    if (IS_SET(ch->newbits,NEW_DFORM)) /* spiderform */
    {
      free_string(ch->morph);
      ch->morph = str_dup("");
      REMOVE_BIT(ch->newbits,NEW_DFORM);
      REMOVE_BIT(ch->newbits,THIRD_HAND);
      REMOVE_BIT(ch->newbits,FOURTH_HAND);
    }
  }
  else if (IS_CLASS(ch, CLASS_VAMPIRE))
  {
    if (IS_EXTRA(ch, EXTRA_DRAGON)) /* dragonform */
    {
      REMOVE_BIT(ch->extra,EXTRA_DRAGON);
      free_string( ch->morph );
      ch->morph = str_dup( "" );
    }
    if (IS_POLYAFF(ch, POLY_ZULOFORM)) /* zuloform */
    {
      REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      free_string( ch->morph );
      ch->morph = str_dup( "" );
    }
  }
  else if (IS_CLASS(ch, CLASS_GIANT))
  {
    if (IS_SET(ch->newbits, NEW_CUBEFORM)) /* dawnstrength */
    {
      REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      free_string(ch->morph);
      ch->morph=str_dup("");
    }
  }
  else if (IS_CLASS(ch, CLASS_TANARRI))
  {
    if (ch->pcdata->powers[TANARRI_FURY_ON] == 1)  /* fury */
      ch->pcdata->powers[TANARRI_FURY_ON] = 0;
  }
  else if (IS_CLASS(ch, CLASS_DRONE)) /* nightmare aura */
  {
    if (ch->pcdata->powers[DRONE_NIGHTMARE] == 1)
      ch->pcdata->powers[DRONE_NIGHTMARE] = 0;
  }
  else if (IS_CLASS(ch, CLASS_SHAPESHIFTER))
  {
    if (IS_SET(ch->affected_by, AFF_POLYMORPH)) /* reset form */
    {
      ch->pcdata->powers[SHAPE_FORM]=0;
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      free_string(ch->morph);
      ch->morph=str_dup("");
    }
  }
  else if (IS_CLASS(ch, CLASS_DROID))
  {
    if (IS_SET(ch->newbits, NEW_CUBEFORM))  /* _THE_ cubeform */
    {
      REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);  
      free_string(ch->morph);
      ch->morph=str_dup("");
    }
  }
  else if (IS_CLASS(ch, CLASS_UNDEAD_KNIGHT))
  {
    if (IS_SET(ch->pcdata->powers[AURAS], MIGHT_AURA))  /* aura of might */
      REMOVE_BIT(ch->pcdata->powers[AURAS], MIGHT_AURA);
  }
  else if (IS_CLASS(ch, CLASS_ANGEL))
  {
    if (IS_SET(ch->newbits, NEW_CUBEFORM)) /* godly favor */
    {
      REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      free_string(ch->morph);
      ch->morph=str_dup("");
    }
  }
  else if (IS_CLASS(ch, CLASS_FAE))
  {
    if (IS_SET(ch->newbits, NEW_CUBEFORM))  /* elemental form */
    {
      REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
      REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
      if (IS_SET(ch->affected_by, AFF_PASS_DOOR)) REMOVE_BIT(ch->affected_by, AFF_PASS_DOOR);
      free_string(ch->morph);
      ch->morph=str_dup("");
    }
  }

  /* and a little bit of everything */
  REMOVE_BIT(ch->pcdata->jflags, JFLAG_DRONE_HORRID);
  return;
}

int get_ratio(CHAR_DATA *ch)
{
  long ratio;

  if (IS_NPC(ch)) return 0;
  if ((ch->pkill + ch->pdeath) == 0) ratio = 0; // to avoid divide by zero.
  else if (ch->pkill > ch->pdeath)
    ratio = ch->pkill * 100 * ((ch->pkill * ch->pkill) - (ch->pdeath * ch->pdeath))/((ch->pkill + ch->pdeath) * (ch->pkill + ch->pdeath));
  else if (ch->pkill > 0)
    ratio = (-100) * (ch->pdeath - ch->pkill) / ch->pkill;
  else
    ratio = (-100) * ch->pdeath;
  return (int) ratio;
}

bool multicheck(CHAR_DATA *ch)
{
  CHAR_DATA *gch;
  char buf[MAX_STRING_LENGTH];

  if (ch->level > 6) return FALSE;

  for (gch = char_list; gch; gch = gch->next)
  {
    if (IS_NPC(gch)) continue;
    if (gch == ch) continue;
    if (gch->level > 6) continue;
    if (strlen(gch->lasthost) > 2)
    {
      if (gch->desc)
      {
        if (!str_cmp(gch->desc->host, ch->desc->host))
        {
          sprintf(buf,"%s has connected from the same IP as %s", ch->name, gch->name);
          log_string(buf);
          do_info(ch, buf);
          return TRUE;
        }
      }
      else if (!str_cmp(gch->lasthost, ch->desc->host))
      {
        sprintf(buf,"%s has connected from the same IP as %s", ch->name, gch->name);
        log_string(buf);
        return TRUE;
      }
    }
  }
  return FALSE;
}

bool reachedDecapLimit( CHAR_DATA *ch )
{
  AFFECT_DATA *paf;
  OBJ_DATA *obj;
  int limit = 0, objhps = 0, spellhps = 0;
  int hps, i;

  if (IS_NPC(ch)) return TRUE;
  if (ch->level > 6) return FALSE;

  if (!IS_CLASS(ch, CLASS_SAMURAI))
  {
    for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_HIT)
        spellhps += paf->modifier;
    }
  }

  for (i = 0; i < MAX_WEAR; i++)
  {
    if ((obj = get_eq_char(ch, i)) == NULL) continue;
    if (IS_CLASS(ch, CLASS_SAMURAI) && obj->pIndexData->vnum != 33177) continue;
    for (paf = obj->pIndexData->affected; paf; paf = paf->next)
    {
      if (paf->location == APPLY_HIT)
        objhps += paf->modifier;
    }
    for (paf = obj->affected; paf; paf = paf->next)
    {
      if (paf->location == APPLY_HIT)
        objhps += paf->modifier;
    }
  }

  hps = (ch->max_hit - (spellhps + objhps));

  if (hps > 10000) limit += 10;
  else limit += hps/1000;
  if ((hps -= 10000) > 10000) limit += 20;
  else if (hps > 0) limit += 2 * hps/1000;
  if ((hps -= 10000) > 10000) limit += 30;
  else if (hps > 0) limit += 3 * hps/1000;
  if ((hps -= 10000) > 10000) limit += 40;
  else if (hps > 0) limit += 4 * hps/1000;
  if ((hps -= 10000) > 10000) limit += 50;
  else if (hps > 0) limit += 5 * hps/1000;
  if ((hps -= 10000) > 10000) limit += 60;
  else if (hps > 0) limit += 6 * hps/1000;

  if (is_upgrade(ch)) limit *= (2 + ch->pcdata->upgrade_level);

  /*
   * For those with no skill
   */
  limit += ch->pdeath;

  if (limit > ch->pkill) return FALSE;
  else return TRUE;
}

void death_info(char *str)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];

  if (str[0] == '\0') return;

  sprintf(buf, "#C<- #RDeath #C->#n %s\n\r", str);

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character != NULL)
      send_to_char(buf, d->character);
  }
  return;
}

void avatar_info(char *str)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
    
  if (str[0] == '\0') return;

  sprintf(buf, "#C<- #RAvatar #C->#n %s\n\r", str); 

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character != NULL)
      send_to_char(buf, d->character);
  }  
  return;
}

void leave_info(char *str)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
      
  if (str[0] == '\0') return;
 
  sprintf(buf, "#C<- #RLeaves #C->#n %s\n\r", str);
  
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character != NULL)
      send_to_char(buf, d->character);
  }
  return;
}

void enter_info(char *str)
{
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
      
  if (str[0] == '\0') return;
 
  sprintf(buf, "#C<- #REnter #C->#n %s\n\r", str);
  
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character != NULL)
      send_to_char(buf, d->character);
  }
  return;
}

int getMight(CHAR_DATA *ch)
{
  AFFECT_DATA *paf;
  OBJ_DATA *obj;
  int spellhps = 0;
  int objhps = 0;
  int might, temp, i;

  if (!IS_CLASS(ch, CLASS_SAMURAI))
  {
    for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_HIT)
        spellhps += paf->modifier;
    }
  }

  for (i = 0; i < MAX_WEAR; i++)
  {
    if ((obj = get_eq_char(ch, i)) == NULL) continue;
    if (IS_CLASS(ch, CLASS_SAMURAI) && obj->pIndexData->vnum != 33177) continue;
    for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
    {
      if (paf->location == APPLY_HIT)
        objhps += paf->modifier;
    }
    for (paf = obj->affected; paf; paf = paf->next) 
    {
      if (paf->location == APPLY_HIT)                       
        objhps += paf->modifier;
    }
  }

  might = (ch->max_hit - (spellhps + objhps))/100;
  
  for (i = 0; i < 5; i++)  might += UMIN(2, ch->spl[i]/100);
  for (i = 0; i < 13; i++) might += UMIN(4, ch->wpn[i]/50);
  for (i = 1; i < 11; i++) might += UMIN(4, ch->stance[i]/50);
  if (IS_SET(ch->newbits, NEW_MASTERY)) might += 2;

  if (IS_SET(ch->pcdata->jflags, JFLAG_SS5)) might += 250;
  else if (IS_SET(ch->pcdata->jflags, JFLAG_SS4)) might += 200;
  else if (IS_SET(ch->pcdata->jflags, JFLAG_SS3)) might += 150;
  else if (IS_SET(ch->pcdata->jflags, JFLAG_SS2)) might += 100;
  else if (IS_SET(ch->pcdata->jflags, JFLAG_SS1)) might += 50;

  if (might >= 150)
  {
    if (is_upgrade(ch)) might *= 2;
    if (ch->pcdata->rune_count > 0) might += UMIN(150, ch->pcdata->rune_count * 10);
    if ((temp = get_ratio(ch)) >= 500) might += UMIN(100, temp / 50);  // 10 points for each pkpower.
    if (ch->pcdata->upgrade_level > 0) might += ch->pcdata->upgrade_level * 120;
  }
  return might;
}

int getMightMod(CHAR_DATA *ch)
{
  int might;

  if (IS_NPC(ch)) return 0;
  might = getMight(ch);
  if (might < 150) return 0;
  else if (might <=  350) return 10;
  else if (might <=  500) return 20;
  else if (might <=  750) return 30;
  else if (might <= 1000) return 40;
  else if (might <= 1250) return 50;
  else if (might <= 1500) return 60;
  else if (might <= 1750) return 70;
  else if (might <= 2000) return 80;
  else if (might <= 2250) return 90;
  else if (might <= 2500) return 100;
  else if (might <= 2750) return 110;
  else if (might <= 3000) return 120;
  else if (might <= 3250) return 130;
  else if (might <= 3500) return 140;
  else return 150;
}

void forge_affect(OBJ_DATA *obj, int value)
{
  AFFECT_DATA paf;

  paf.type           = 0;
  paf.duration       = -1;
  paf.location       = APPLY_HITROLL;
  paf.modifier       = value;   
  paf.bitvector      = 0;
  affect_to_obj(obj, &paf);
    
  paf.type           = 0;
  paf.duration       = -1;
  paf.location       = APPLY_DAMROLL;
  paf.modifier       = value;
  paf.bitvector      = 0;
  affect_to_obj(obj, &paf);
}

void dump_last_command()
{
  FILE *fp;
  char buf[MAX_STRING_LENGTH];

  fp = fopen("../txt/crash.txt","a");
  if (cmd_done)
    fprintf (fp,"Last command typed : %s  (thread count : %d) (command executed without flaws)\n",last_command, thread_count);
  else
    fprintf (fp,"Last command typed : %s  (thread count : %d) (crash happended during this command)\n",last_command, thread_count);
  fflush(fp);
  fclose(fp);

  /*
   * creates a note to the immortals
   */
  sprintf(buf, "It seems we have crashed, the last command typed was\n\r\n\r");
  strcat(buf, last_command);
  strcat(buf, "\n\r\n\rPlease remember that this doesn't mean that this caused the crash.\n\r\n\rRegards,\n\r\n\rThe Crash Code");
  make_note("Immortal", "Crash Code", "imm", "We Crashed", 7, buf);
} 

void update_revision(CHAR_DATA *ch)
{
  if (IS_NPC(ch)) return;
  if (ch->pcdata->revision == CURRENT_REVISION) return;

  /*
   * We don't end cases with break, since we want the player to be fully updated.
   */
  switch (ch->pcdata->revision)
  {
    default:
      break;
  }
  return;
}

bool in_fortress(CHAR_DATA *ch)
{
  if (!ch->in_room) return FALSE;
  if (ch->in_room->vnum >= 78001 && ch->in_room->vnum <= 78010) return TRUE;
  return FALSE;
}

bool in_arena(CHAR_DATA *ch)
{
  if (!ch->in_room) return FALSE;
  if (ch->in_room->vnum >= 50 && ch->in_room->vnum <= 70) return TRUE;
  return FALSE;
}

void increase_total_output(int clenght)
{
  total_output += clenght;
}

void update_mudinfo()
{
  DESCRIPTOR_DATA *d;
  int i, pcount = 0;

  /*
   * Each week, the data is stored to a file, and
   * the variable cleared.
   */
  if (mudinfo[MUDINFO_UPDATED] > 20160)
  {
    write_mudinfo_database();
    for (i = 0; i < (MUDINFO_MAX - 2); i++)
    {
      mudinfo[i] = 0;
    }
    log_string("Mudinfo database updated.");
  }

  /* Increase update count */
  mudinfo[MUDINFO_UPDATED]++;

  /* Outdate the output data */

  if (total_output > mudinfo[MUDINFO_DATA_PEAK]) mudinfo[MUDINFO_DATA_PEAK] = total_output;

  /* The stored data */
  if (mudinfo[MUDINFO_BYTE] > 1048576) // 1 megabyte
  {
    mudinfo[MUDINFO_MBYTE]++;
    mudinfo[MUDINFO_BYTE] -= 1048576;
  }
  mudinfo[MUDINFO_BYTE] += total_output;
  
  /* The temp data */
  if (mudinfo[MUDINFO_BYTE_S] > 1048576) // 1 megabyte
  {
    mudinfo[MUDINFO_MBYTE_S]++;
    mudinfo[MUDINFO_BYTE_S] -= 1048576;
  }
  mudinfo[MUDINFO_BYTE_S] += total_output;
 
  /* We clear the counter */
  total_output = 0;

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->lookup_status == STATUS_DONE) 
    {
      if (d->character)
      {
        if (d->character->level < 7)
        {
          pcount++;
          if (d->out_compress) mudinfo[MUDINFO_MCCP_USERS]++;
          else mudinfo[MUDINFO_OTHER_USERS]++;
          if (IS_SET(d->character->act, PLR_SOUND)) mudinfo[MUDINFO_MSP_USERS]++;
        }
      }
    }
  }  

  if (pcount > mudinfo[MUDINFO_PEAK_USERS]) mudinfo[MUDINFO_PEAK_USERS] = pcount;

  save_mudinfo();
}

void recycle_descriptors()
{
  DESCRIPTOR_DATA *dclose;
  DESCRIPTOR_DATA *dclose_next;

  for (dclose = descriptor_list; dclose; dclose = dclose_next)
  {
    dclose_next = dclose->next;
    if (dclose->lookup_status != STATUS_CLOSED) continue;

    /*
     * First let's get it out of the descriptor list.
     */
    if ( dclose == descriptor_list )
    {
      descriptor_list = descriptor_list->next;
    }
    else   
    { 
      DESCRIPTOR_DATA *d;
       
      for (d = descriptor_list; d && d->next != dclose; d = d->next)
        ;
      if (d != NULL)
        d->next = dclose->next;
      else
      {
        bug( "Recycle_descriptors: dclose not found.", 0 );
        continue;
      }
    }

    /*
     * Clear out that memory
     */
    free_string( dclose->host );
    free_mem( dclose->outbuf, dclose->outsize );
 
    /*
     * Mccp
     */
    if (dclose->out_compress)
    {
      deflateEnd(dclose->out_compress);
      free_mem(dclose->out_compress_buf, COMPRESS_BUF_SIZE);
      free_mem(dclose->out_compress, sizeof(z_stream));
    }

    /*     
     * Bye bye mr. Descriptor.
     */
    close( dclose->descriptor );

    /*   
     * And then we recycle
     */
    dclose->next        = descriptor_free;
    descriptor_free     = dclose;
  }
}

void affect_to_area(AREA_DATA *pArea, AREA_AFFECT *paf)
{
  AREA_AFFECT *paf_new;

  if (area_affect_free == NULL)
  {
    paf_new = alloc_perm( sizeof(*paf_new) );
  }
  else
  {
    paf_new          = area_affect_free;
    area_affect_free = area_affect_free->next;
  }

  *paf_new        = *paf;
  paf_new->next   = pArea->affects;
  pArea->affects  = paf_new;
  
  return;
}

void area_affect_remove(AREA_DATA *pArea, AREA_AFFECT *paf)
{
  if (pArea->affects == NULL)
  {
    bug( "Area_affect_remove: no affect.", 0 );
    return;
  }
  if (paf == pArea->affects)
  {
    pArea->affects = paf->next;
  }
  else
  {
    AREA_AFFECT *prev;

    for (prev = pArea->affects; prev; prev = prev->next)
    {
      if (prev->next == paf)
      {
        prev->next = paf->next;
        break;
      }
    }
    if (!prev)
    {
      bug( "Area_affect_remove: cannot find paf.", 0);
      return;
    }
  }

  paf->next        = area_affect_free;
  area_affect_free = paf;

  return;
} 

/*
 * A religion of 0 will result in any religion is true,
 * but a religion != 0 will only result in true if the spell
 * has that given religion.
 */
bool has_area_affect(AREA_DATA *pArea, int affect, int religion)
{
  AREA_AFFECT *paf;

  for (paf = pArea->affects; paf; paf = paf->next)
  {
    if (paf->type == affect)
    {
      if (religion != 0 && paf->religion != religion) continue;
      else return TRUE;
    }
  }
  return FALSE;
}

int get_next_playerid()
{
  top_playerid++;
  save_coreinfo();
  return top_playerid;
}

/*
 * Writes a string straight to stderr
 */
void log_string2(const char *str)
{
  char *strtime;

  strtime = ctime(&current_time);
  strtime[strlen(strtime)-1] = '\0';
  fprintf(stderr, "%s :: %s\n", strtime, str);

  return;
}

void newbie_help()
{
  DESCRIPTOR_DATA *d;

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->connected != CON_PLAYING) continue;
    if (!d->character) continue;
    if ((get_age(d->character) - 17) >= 2) continue;
    if (IS_SET(d->character->pcdata->jflags, JFLAG_NONEWBIE)) continue;

    // Ok, let's give them a few hints
    switch(number_range(1, 11))
    {
      default: break;
      case 1:
        send_to_char("#C[#0NEWBIE#C]#y You can turn this channel on/off by typing 'newbiehint'.#n\n\r", d->character);
        break;
      case 2:
        send_to_char("#C[#0NEWBIE#C]#y Using MCCP will give you an extra 25% exp. Type 'help mccp' for details.#n\n\r", d->character);
        break;
      case 3:
        send_to_char("#C[#0NEWBIE#C]#y This mud doesn't use status, but a different system. type 'help paradoxstats' for details.#n\n\r", d->character);
        break;
      case 4:
        send_to_char("#C[#0NEWBIE#C]#y Theres a help file on the WHO-screen. Type 'help who' for details.#n\n\r", d->character);
        break;
      case 5:
        send_to_char("#C[#0NEWBIE#C]#y It helps alot when fighting mobs to have a high unarmed skill. Fight barehanded to increase it.#n\n\r", d->character);
        break;
      case 6:
        send_to_char("#C[#0NEWBIE#C]#y Go North, Up, South from recall to enter the newbie arena (it's a good place to start)#n\n\r", d->character);
        break;
      case 7:
        send_to_char("#C[#0NEWBIE#C]#y You need 2000 hitpoints to train avatar, but then you can use the selfclass command to gain a class.#n\n\r", d->character);
        break;
      case 8:
        send_to_char("#C[#0NEWBIE#C]#y Remember to read 'help changes' often, and the noteboard 'help noteboard'.#n\n\r", d->character);
        break;
      case 9:
        send_to_char("#C[#0NEWBIE#C]#y The muds homepage is http://www.daimi.au.dk/~jobo/dystopia, browse it for more details.#n\n\r", d->character);
        break;
      case 10:
        send_to_char("#C[#0NEWBIE#C]#y When you get a class, try typing 'powers', this will give a list of class commands.#n\n\r", d->character);
        break;
      case 11:
        send_to_char("#C[#0NEWBIE#C]#y If you forge a gemstone/hilt on an item, it will lose it's old affect as well as gain a new.#n\n\r", d->character);
        break;
    }
  }
}

int get_spend_qps(CHAR_DATA *ch)
{
  int qps = 0;
  int i;

  if (is_upgrade(ch))
  {
    qps += 35000; // initial upgrade

    /* And then for each upgrade the player already had */
    for (i = 0; i < ch->pcdata->upgrade_level; i++)
    {
      qps += 35000 * (i+1);
    }
  }
  qps += ch->pcdata->rune_count * 8000;    // rune eq
  qps += ch->pcdata->cursed_count * 4000;  // cursed eq
  return qps;
}

void recycle_dummys()
{
  DUMMY_ARG *dummy;
  DUMMY_ARG *dummy_next;

  for (dummy = dummy_list; dummy; dummy = dummy_next)
  {
    dummy_next = dummy->next;
    if (dummy->status == 1) continue;  // being used

    if (dummy == dummy_list)
    {
      dummy_list = dummy_list->next;
    }
    else
    {
      DUMMY_ARG *prev;

      /* we find the prev dummy arg */
      for (prev = dummy_list; prev && prev->next != dummy; prev = prev->next)
        ;
      if (prev != NULL)
        prev->next = dummy->next;
      else
      {
        bug( "Recycle_dymmys: dummy not found.", 0 );
        continue;
      }

      /* recycle */
      dummy->next = dummy_free;
      dummy_free = dummy;
    }
  }
}

void chance_qps()
{
  char buf[MAX_STRING_LENGTH];
  int chance;

  if (qpstimer <= ccenter[CCENTER_QD_TIME]) // 3 hours minimum
    return;
  chance = (qpstimer - ccenter[CCENTER_QD_TIME])/4;  // 1 for each 2 minutes beyond 6 hours
  if (number_percent() < UMIN(chance, 95))
  {
    global_qp = TRUE;
    sprintf(buf, "#GQ#RU#GE#RS#GT#RO#GR#RS #GD#RE#GL#RI#GG#RH#GT #0(#7STARTED#0)#n");
    do_info(NULL, buf);
    pulse_doubleqp = number_range(30, 60) * PULSE_AREA;
  }
}

void chance_exp()
{
  char buf[MAX_STRING_LENGTH];
  int chance;

  if (exptimer <= ccenter[CCENTER_HH_TIME]) // 3 hours minimum
    return;
  chance = (exptimer - ccenter[CCENTER_HH_TIME])/4;  // 1 for each 2 minutes beyond 6 hours
  if (number_percent() < UMIN(chance, 95))
  {
    global_exp = TRUE;
    sprintf(buf, "#GH#Ra#Gp#Rp#Gy #GH#Ro#Gu#Rr #0(#7STARTED#0)#n");
    do_info(NULL, buf);
    pulse_doubleexp = number_range(30, 60) * PULSE_AREA;
  }
}

void check_help_soundex(char *argument, CHAR_DATA *ch)
{
  HELP_DATA *pHelp;
  char buf[MAX_STRING_LENGTH];
  char tbuf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  char keyword[MAX_INPUT_LENGTH];
  char *str;
  bool found = FALSE;

  one_argument(argument, arg);

  if (arg[0] == '\0') return;

  sprintf(buf, "\n\r[Perhaps:");
  for (pHelp = first_help; pHelp; pHelp = pHelp->next)
  {
    if (pHelp->level > ch->level) continue;

    str = pHelp->keyword;
    str = one_argument(str, keyword);
    while (keyword[0] != '\0')
    {
      if (SoundexMatch(GetSoundexKey(arg), GetSoundexKey(keyword)) > 75)
      {
        found = TRUE;
        sprintf(tbuf, " %s", keyword);
        strcat(buf, tbuf);
      }
      str = one_argument(str, keyword);
    }
  }
  strcat(buf, "]\n\r");
  if (found) send_to_char(buf, ch);
}

/*
 * New system to replace status, called fair fight, it measures the
 * difference between two players, giving them points for their
 * stances, powers, and stats. If they are within each others range,
 * the call will return TRUE, if not FALSE. Call for fair_fight when
 * you need to see if a fight is fair (ie. decapping).
 */
bool fair_fight(CHAR_DATA *ch, CHAR_DATA *victim)
{
  int iAggr, iDef;

  if (IS_NPC(ch) || IS_NPC(victim)) return TRUE;

  /*
   * All the people that shouldn't be fighting anyway
   */
  if (ch == victim) return FALSE;
  if (ch->level != 3 || victim->level != 3) return FALSE;

  iAggr = getMight(ch);
  iDef  = getMight(victim);
  if (iDef < 150 || iAggr < 150) return FALSE;

  if (reachedDecapLimit(ch)) return FALSE;
  if (!str_cmp(ch->pcdata->retaliation, victim->name)) return TRUE;
  if (!str_cmp(ch->pcdata->last_decap[0], victim->name)) return FALSE;

  /* 2000 extra hps per paradox counter */
  if (iAggr >= 150) iAggr += ch->pcdata->mean_paradox_counter * 20;
  if (iDef >= 150) iDef += victim->pcdata->mean_paradox_counter * 20;

  /*
   * Checking to see if they are in range.
   */
  if (iAggr * 0.80 > iDef) return FALSE;

  /*
   * They passed the test, FIGHT children.
   */
  return TRUE;
}

void special_decap_message(CHAR_DATA *ch, CHAR_DATA *victim)
{  
  static char * const he_she  [] = { "XX", "he",  "she" };
  static char * const him_her [] = { "XX", "him", "her" };
  static char * const his_her [] = { "XX", "his", "her" };
  DESCRIPTOR_DATA *d;
  char buf[400]; // that should be plenty.
  const char *dmess;
  const char *i;
  char *ptr2;
  char *ptr;
  int size;
      
  size = strlen2(ch->pcdata->decapmessage);
  if (size > 380)
  {
    bug("Bad decapmessage.",0);
    return;
  }
  
  ptr2  = "#C<- #RDeath #C->#n ";
  ptr   = buf;
  dmess = ch->pcdata->decapmessage;
  
  while ((*ptr = *ptr2) != '\0')
    ++ptr, ++ptr2;

  while (*dmess != '\0')
  {
    if ( *dmess != '$' )
    {
      *ptr++ = *dmess++;
      continue; 
    }
    ++dmess;
    switch (*dmess)
    {
      default:  i = ""; break;
      case 'n': i = ch->name; break;
      case 'e': i = he_she  [URANGE(1, ch->sex, 2)]; break;
      case 'm': i = him_her [URANGE(1, ch->sex, 2)]; break;
      case 's': i = his_her [URANGE(1, ch->sex, 2)]; break;
      case 'N': i = victim->name; break;
      case 'S': i = his_her [URANGE(1, victim->sex, 2)]; break;
      case 'M': i = him_her [URANGE(1, victim->sex, 2)]; break;
      case 'E': i = he_she  [URANGE(1, victim->sex, 2)]; break;
    }
    ++dmess;
    /* copying the data into the pointer */
    while ((*ptr = *i) != '\0')
      ++ptr, ++i;
  }
  *ptr++ = '\n';
  *ptr++ = '\r';
     
  for (d = descriptor_list; d; d = d->next)
  {
    if (d->lookup_status != STATUS_DONE) continue;
    if (d->connected != CON_PLAYING ) continue;
    write_to_buffer( d, buf, ptr - buf );
  }  
  return;
}

void update_polls()
{ 
  POLL_DATA *poll;
  POLL_DATA *poll_next;
  
  for (poll = poll_list; poll; poll = poll_next)
  {
    poll_next = poll->next;
    if (poll->expire < current_time) complete_poll(poll);
  }
}

/* 
 * It's not perfect, but it will do - Jobo
 */
void complete_poll(POLL_DATA *poll)
{
  VOTE_DATA *vote;
  POLL_DATA *prev; 
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  bool found = FALSE;  
  int i;
  
  for (vote = poll->votes; vote; vote = vote->next)
  {
    free_string(vote->phost);
    free_string(vote->pname);
  }
  if (poll == poll_list)
  {
    poll_list = poll->next;
    found = TRUE;
  }
  else
  {
    for (prev = poll_list; prev; prev = prev->next)
    {
      if (prev->next != poll) continue;
      found = TRUE;  
      prev->next = poll->next;
    }
  }
  if (!found)
  { 
    bug("Poll_complete: poll not found", 0);
    return;
  }

  sprintf(buf, "Polling on %s completed.\n\r\n\r", poll->name);
  for (i = 0; i < MAX_VOTE_OPTIONS; i++)
  {
    if (str_cmp(poll->options[i], "<null>"))
    {
      sprintf(buf2, "Option '%s' got %d votes.\n\r", poll->options[i], poll->vcount[i]);
      strcat(buf, buf2);
    }
    free_string(poll->options[i]);
  }
  strcat(buf, "\n\rRegards, The Polling Code\n\r");
    
  make_note("Announce", "Polling Code", "all", "Poll Completed", 7, buf);
  free_string(poll->name);
  save_polls();
  do_info(NULL, "Poll completed, check the announce board for details.");
}

/*
 * This function handles the actual removing of the change
 */
bool remove_change(int i)
{
  CHANGE_DATA *change; 
  bool found = FALSE;
     
  for (change = change_list; change; change = change->next)
  {
    if (--i > 0) continue;
    found = TRUE;
    
    /* clearing out the strings */
    free_string(change->imm);
    free_string(change->text);
    free_string(change->date);
   
    /* update the pointer to the last change if needed */
    if (change == change_last) change_last = change->prev;
  
    /* handle the special case of the first change */
    if (change == change_list)
    { 
      change_list = change->next;
      if (change->next) change->next->prev = NULL;
    }
    else
    { 
      change->prev->next = change->next;
      if (change->next)  
        change->next->prev = change->prev;
    }
  
    /* Handle the free list */
    change->next = change_free;
    change->prev = NULL;
    if (change_free) change_free->prev = change;
    change_free = change;
  
    /* terminate the loop */
    break;
  }
  
  /* did we remove anything ? */
  return found;
} 

char *get_dystopia_banner(char *title, int size)
{
  int tSize = strlen(title);
  int patternsize, bufferspaces = 0, blockcount, i;
  static char buf[200];

  buf[0] = '\0';

  /* calculating the amount of patterns on each side of the title */
  patternsize = (size - tSize)/2;
  if (!(patternsize % 4))
    patternsize -= 4;
  else
    while(patternsize % 4) patternsize--;
  patternsize /= 4;

  /* calculating buffer spaces */
  while ((tSize + bufferspaces) % 4) bufferspaces++;
  blockcount = bufferspaces/2;

  if (patternsize < 1)
  {
    strcat(buf, "#0<>== #G");
    strcat(buf, title);
    strcat(buf, " #0==<>#n");  
  }
  else
  {
    /* first add patterns */
    strcat(buf, "#0<>==");
    for (i = 1; i < patternsize; i++)
      strcat(buf, "<>==");
    /* add the title */
    if (tSize)
    {
      strcat(buf, "#G ");
      while (bufferspaces > blockcount)
      {
        bufferspaces--;
        strcat(buf, " ");
      }
    }
    else strcat(buf, "<>");
    strcat(buf, title);
    if (tSize)
    {
      strcat(buf, "#0 ");
      while (bufferspaces > 0)
      {
        bufferspaces--;
        strcat(buf, " ");
      }
    }
    /* add secondary patterns */
    for (i = 0; i < patternsize; i++)
      strcat(buf, "==<>");
    strcat(buf, "#n");
  }
  return buf;
}

int calc_ratio(int a, int b)
{
  int ratio;

  if (b == 0 && a > 0) ratio = 100;
  else if ((a + b) != 0) ratio = (100*a)/(a+b);
  else ratio = 0;
  return ratio;
}

char *strip_ansi(char *str)
{
  static char buf[200];
  char *ptr;

  buf[0] = '\0';
  ptr = buf;

  while (*str != '\0')
  {
    if (*str != '#') *ptr++ = *str++;
    else if (*(++str) != '\0') str++;
  }
  *ptr = '\0';
  return buf;
}


char *line_indent(char *text, int wBegin, int wMax)
{
  static char buf[MAX_STRING_LENGTH];
  char *ptr;
  char *ptr2;
  int count = 0;
  bool stop = FALSE;
  int wEnd;

  buf[0] = '\0';   
  ptr = text;
  ptr2 = buf;

  while (!stop)
  {
    if (count == 0)
    {
      if (*ptr == '\0') wEnd = wMax - wBegin;
      else if (strlen(ptr) < (wMax - wBegin)) wEnd = wMax - wBegin;
      else   
      {
        int x = 0;

        while (*(ptr + (wMax - wBegin - x)) != ' ') x++;
        wEnd = wMax - wBegin - (x - 1);
        if (wEnd < 1) wEnd = wMax - wBegin;
      }
    }
    if (count == 0 && *ptr == ' ') ptr++;
    else if (++count != wEnd)
    {
      if ((*ptr2++ = *ptr++) == '\0') stop = TRUE;
    }
    else if (*ptr == '\0')
    {
      stop = TRUE;
      *ptr2 = '\0';
    }
    else
    {
      int k;
      
      count = 0;
      *ptr2++ = '\n';
      *ptr2++ = '\r';
      for (k = 0; k < wBegin; k++)
        *ptr2++ = ' ';
    }
  }
  return buf;
}

void init_vt100(DESCRIPTOR_DATA *d, char *xbuf)
{
  CHAR_DATA *ch;
  char buf[MAX_STRING_LENGTH];
  int i;

  if ((ch = d->character) == NULL)
    return;
  if ((i = atoi(xbuf)) < 10)
  {
    send_to_char("VT100 Failed.\n\r", ch);
    return;
  }  
  ch->pcdata->vt100_size = i;
  SET_BIT(ch->act, PLR_VT100);
  sprintf(buf, "\e[%d;1H%s%s\e[1;1H%s%s\e[1;%dr",
    i, VT_CLEAR_LINE, VT_SAVECURSOR, VT_SETWIN_CLEAR, VT_CLEAR_SCREEN, i - 2);
  send_to_char(buf, ch);
  send_to_char("VT100 Initialized.\n\r", ch);
  return;
}

/*
 * The mobs vnum is included for bug-reports.
 */
bool will_trigger(char *say, char *trigger, int vnum)
{
  char buf[50];
  char *ptr;
  bool found;

  buf[0] = '\0';
  ptr = buf;

  while (*trigger != '\0')
  {
    switch(*trigger)
    {
      default:
        bug("Will_trigger: Bad keyword on mob %d.", vnum);
        return FALSE;
      case '&':  // all keywords must be included
        trigger++;
        while (*trigger != '\0' && *trigger != '&' && *trigger != '!' && *trigger != '|')
        {
          if (*trigger != '*')
            *ptr++ = *trigger;
          else
          {
            *ptr = '\0';
            if (!is_contained(buf, say)) return FALSE;
            else
            {
              buf[0] = '\0';
              ptr = buf;
            }
          }
          trigger++;
        }
        break;
      case '|':  // at least one must be included
        trigger++; found = FALSE;
        while (*trigger != '\0' && *trigger != '&' && *trigger != '!' && *trigger != '|')
        {
          if (*trigger != '*')   
            *ptr++ = *trigger;
          else
          { 
            *ptr = '\0';
            if (is_contained(buf, say)) found = TRUE;
            buf[0] = '\0';
            ptr = buf;
          }   
          trigger++;
        }
        if (!found) return FALSE;
        break;
      case '!':  // none of these may be included
        trigger++;
        while (*trigger != '\0' && *trigger != '&' && *trigger != '!' && *trigger != '|')
        {
          if (*trigger != '*')   
            *ptr++ = *trigger;
          else
          {
            *ptr = '\0';
            if (is_contained(buf, say)) return FALSE;
            else
            {
              buf[0] = '\0';
              ptr = buf;  
            }
          }
          trigger++;  
        }
        break;
    }
  }
  return TRUE;
}

void mob_text(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *victim;
  MOB_TRIGGER *mProg;
  char *arg;
  char buf[MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;

  for (vch = ch->in_room->people; vch; vch = vch_next)
  {
    int runs = 0;

    vch_next = vch->next_in_room;
    if (!IS_NPC(vch)) continue;

    while (runs < 2)
    {
      if (runs == 0) mProg = vch->pIndexData->triggers;
      else mProg = vch->triggers;

      for ( ; mProg; mProg = mProg->next)
      {
        if (will_trigger(argument, mProg->keywords, vch->pIndexData->vnum))
        {
          switch(mProg->type)
          {
            default:
              bug("Mob_text: Bad type on mob %d.", vch->pIndexData->vnum);
              break;
            case MTRIG_SAY:
              do_say(vch, mProg->roomOutput);
              break;
            case MTRIG_LOCATE:
              if ((arg = cut_after_locate(argument)) == NULL)
              {
                do_say(vch, mProg->chOutput);
                return;
              }
              else if ((victim = get_char_area(ch, arg)) == NULL)
              {
                do_say(vch, mProg->chOutput);
                return;
              }
              else
              {
                if ((arg = pathfind(ch, victim)) == NULL)
                {
                  do_say(vch, mProg->chOutput);
                  return;
                }
                else
                {
                  sprintf(buf, mProg->roomOutput, arg);
                  do_say(vch, buf);
                  return;
                }
              }
              break;
            case MTRIG_HUNTER:
              if ((arg = cut_after_tracking(argument)) == NULL)
              {
                do_say(vch, mProg->chOutput);
                return;
              }
              else if ((victim = get_char_area(ch, arg)) == NULL)
              {
                do_say(vch, mProg->chOutput);
                return;
              }
              if (IS_NPC(victim))
              {
                do_say(vch, mProg->chOutput);
                return;
              }
              else  
              {
                if ((arg = pathfind(ch, victim)) == NULL)
                {
                  do_say(vch, mProg->chOutput);
                  return;
                }
                else
                {
                  free_string(vch->hunting);
                  vch->hunting = str_dup(arg);
                  vch->tracking = TRUE;
                  vch->hunt_pointer = 0;
                  vch->hunt_playerid = victim->pcdata->playerid;
                  do_say(vch, mProg->roomOutput);
                  return;
                }
              }
              break;
            case MTRIG_SCRY:
              break;
          }
          return;
        }
      }
      runs++;
    }
  }
}

void trigger_remove(CHAR_DATA *ch, MOB_TRIGGER *mProg)
{
  if (ch->triggers == NULL)
  {
    bug("Trigger_remove: no trigger.", 0);
    return;
  }
  if (mProg == ch->triggers)
  {
    ch->triggers = ch->triggers->next;
  }
  else
  {
    MOB_TRIGGER *prev;

    for (prev = ch->triggers; prev; prev = prev->next)
    {
      if (prev->next == mProg)
      {
        prev->next = mProg->next;
        break;
      }
    }
    if (!prev)
    {
      bug("Trigger_remove: cannot find mProg.", 0);
      return;
    }
  }
  mProg->next   = trigger_free;
  trigger_free  = mProg;

  return;
}

void update_aggressive_flags(CHAR_DATA *ch, CHAR_DATA *victim)
{
  int iDef, iAggr;

  /* first we check if we even care */
  if (IS_NPC(ch) || IS_NPC(victim)) return;
  if (ch->level > 6) return;
  if (victim->pcdata->aggress_towards == ch->pcdata->playerid) return;
  if (ch->pcdata->aggress_from == victim->pcdata->playerid) return;
  if (!str_cmp(victim->name, ch->pcdata->retaliation)) return;
  if (in_arena(ch) || in_fortress(ch)) return;
  if (IS_SET(ch->pcdata->tempflag, TEMP_AGGRESSIVE)) return;
  if (ch->class == victim->class && ch->generation >= victim->generation) return;
  if (ragnarok) return;

  iDef = getMight(victim);
  iAggr = getMight(ch);

  /* If the player aggresses someone other than
   * the person who attacked them, they don't deserve
   * the gang protection, since they are asking for it.
   */
  ch->pcdata->aggress_from = 0;

  /* This will take care of bully players */
  if (iDef < iAggr * 0.60)
  {
    victim->pcdata->aggress_from = ch->pcdata->playerid;
    ch->pcdata->bully_points += 50;
    if (ch->pcdata->bully_points > 400)
    {
      SET_BIT(ch->pcdata->jflags, JFLAG_BULLY);
      ch->pcdata->bully_counter += 240;  // roughly 2 hours
      ch->pcdata->bully_points = 0;
      send_to_char("#PTake that, bully!#n\n\r", ch);
    }
  }

  /* this will take care of really small players attacking large players */
  if (iAggr < iDef * 0.60)
    ch->pcdata->aggress_towards = victim->pcdata->playerid;

  /* aggro counter up a little, untill I can fix it */
  ch->pcdata->agg_counter = UMAX(2, ch->pcdata->agg_counter);
  victim->pcdata->agg_counter = UMAX(2, victim->pcdata->agg_counter);
}

void strip_aggression(CHAR_DATA *ch)
{
  if (IS_NPC(ch)) return;

  ch->pcdata->agg_counter     = 0;
  ch->pcdata->aggress_towards = 0;
  ch->pcdata->aggress_from    = 0;
}

char *pathfind(CHAR_DATA *ch, CHAR_DATA *victim)
{
  int const exit_names [] = { 'n', 'e', 's', 'w', 'u', 'd' };
  ROOM_INDEX_DATA *pRoom;
  AREA_DATA *pArea;
  static char path[MAX_STRING_LENGTH]; // should be plenty.
  int iPath = 0, vnum, door;
  bool found;
                  
  if (!ch->in_room || !victim->in_room) return NULL;
  if (ch->in_room == victim->in_room) return NULL;
  if ((pArea = ch->in_room->area) != victim->in_room->area) return NULL;
     
  /* initialize all rooms in the area */
  for (vnum = pArea->lvnum; vnum < pArea->uvnum; vnum++)
  {  
    if ((pRoom = get_room_index(vnum)))
    {
      pRoom->visited = FALSE;
      pRoom->steps = 999;
      for (door = 0; door < 6; door++)
      {
        if (pRoom->exit[door] == NULL) continue;
        pRoom->exit[door]->color = FALSE;
      }
    }
  }
    
  /* initialize variables */
  pRoom = ch->in_room;
  gFound = FALSE;
   
  /* In the first run, we only count steps, no coloring is done */
  examine_steps(pRoom, victim->in_room, pArea, 0);
       
  /* If the target room was never found, we return NULL */
  if (!gFound) return NULL;

  /* in the second run, we color the shortest path using the step counts */
  if (!examine_room(pRoom, victim->in_room, pArea, 0))
    return NULL;   
      
  /* then we follow the trace */
  while (pRoom != victim->in_room)
  {  
    found = FALSE;
    for (door = 0; door < 6 && !found; door++)
    {  
      if (pRoom->exit[door] == NULL) continue;
      if (pRoom->exit[door]->to_room == NULL) continue;
      if (!pRoom->exit[door]->color) continue;
  
      pRoom->exit[door]->color = FALSE;
      found = TRUE;
      path[iPath] = exit_names[door];
      iPath++;
      pRoom = pRoom->exit[door]->to_room;
    }  
    if (!found)
    {
      bug("Pathfind: Fatal Error in %d.", pRoom->vnum);
      return NULL;
    }
  }
      
  path[iPath] = '\0';
  return path;
}    

bool examine_room(ROOM_INDEX_DATA *pRoom, ROOM_INDEX_DATA *tRoom, AREA_DATA *pArea, int steps)
{
  int door;
      
  /* been here before, out of the area or can we get here faster */
  if (pRoom->area != pArea) return FALSE;
  if (pRoom->visited) return FALSE;
  if (pRoom->steps < steps) return FALSE;
   
  /* Have we found the room we are searching for */
  if (pRoom == tRoom)
    return TRUE;
    
  /* mark the room so we know we have been here */
  pRoom->visited = TRUE;
      
  /* Depth first traversel of all exits */
  for (door = 0; door < 6; door++)
  {   
    if (pRoom->exit[door] == NULL) continue;
    if (pRoom->exit[door]->to_room == NULL) continue;
     
    /* assume we are walking the right way */
    pRoom->exit[door]->color = TRUE;
 
    /* recursive return */
    if (examine_room(pRoom->exit[door]->to_room, tRoom, pArea, steps + 1))
      return TRUE;
  
    /* it seems we did not walk the right way */
    pRoom->exit[door]->color = FALSE;
  }
  return FALSE;
}
    
void examine_steps(ROOM_INDEX_DATA *pRoom, ROOM_INDEX_DATA *tRoom, AREA_DATA *pArea, int steps)
{
  int door;
      
  /* Have we found the room we are searching for */
  if (pRoom == tRoom)
  {   
    if (pRoom->steps > steps) pRoom->steps = steps;
    gFound = TRUE;
    return;
  }
    
  /* been here before, or out of the area */
  if (pRoom->area != pArea) return;
  if (pRoom->steps != 999)
  {
    if (pRoom->steps > steps) pRoom->steps = steps;
    return;
  }
   
  /* update the rooms step count*/
  pRoom->steps = steps;
    
  /* Depth first traversel of all exits */
  for (door = 0; door < 6; door++)
  {
    if (pRoom->exit[door] == NULL) continue;
    if (pRoom->exit[door]->to_room == NULL) continue;
  
    examine_steps(pRoom->exit[door]->to_room, tRoom, pArea, steps + 1);
  }
    
  return;  
}
