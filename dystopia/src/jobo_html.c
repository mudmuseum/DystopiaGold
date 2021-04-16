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
 * who_html.c
 *
 * This piece of code allows the mud to create a html file, that looks
 * a bit like the 'who screen'. By calling this function every few ticks,
 * the mud can keep an updated listing which can be accessed from the web.
 *
 * Code by Brian Graversen aka Jobo
 */

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#define HTML_DIR   "../public_html/"

char * plr_class_name(int arg)
{
  static char buf[512];

  buf[0] = '\0';

  switch(arg)
  {
  case CLASS_DEMON:
    strcat(buf, " <font color=\"#FF0000\">[</font><font color=\"#666666\">Demon</font><font color=\"#FF0000\">]</font>");
    break;
  case CLASS_MAGE:
    strcat(buf, " <font color=\"#666666\">{{</font><font color=\"#33CCFF\">Battlemage</font><font color=\"#666666\">}}</font>");
    break;
  case CLASS_WEREWOLF:
    strcat(buf, " <font color=\"#FFFF00\">((</font><font color=\"#3333FF\">Werewolf</font><font color=\"#FFFF00\">))</font>");
    break;
  case CLASS_VAMPIRE:
    strcat(buf, " <font color=\"#666666\">&lt;&lt;</font><font color=\"#FF0000\">Vampire</font><font color=\"#666666\">>></font>");
    break;
  case CLASS_SAMURAI:
    strcat(buf, " <font color=\"#33CCFF\">-=</font><font color=\"#FF0000\">Samu</font><font color=\"#FFFF00\">rai</font><font color=\"#33CCFF\">=-</font>");
    break;
  case CLASS_DROW:
    strcat(buf, " <font color=\"#CC33CC\">.o0</font><font color=\"#666666\">Drow</font><font color=\"#CC33CC\">0o.</font>");
    break;
  case CLASS_MONK:
    strcat(buf, " <font color=\"#666666\">.x[</font><font color=\"#3333FF\">Monk</font><font color=\"#666666\">]x.</font>");
    break;
  case CLASS_NINJA:
    strcat(buf, " <font color=\"#33CCFF\">***</font><font color=\"#FFFF00\">Ninja</font><font color=\"#33CCFF\">***</font>");
    break;
  case CLASS_LICH:
    strcat(buf, " <font color=\"#33FF33\">>*&lt;</font><font color=\"#FFFFFF\">Lich</font><font color=\"#33FF33\">>*&lt;</font>");
    break;
  case CLASS_SHAPESHIFTER:
    strcat(buf, " <font color=\"#666666\">[</font><font color=\"#CC33CC\">*</font><font color=\"#666666\">]</font><font color=\"#FF0000\">Shapeshifter</font><font color=\"#666666\">[</font><font color=\"#CC33CC\">*</font><font color=\"#666666\">]</font>");
    break;
  case CLASS_TANARRI:
    strcat(buf, " <font color=\"#FFFF00\">{</font><font color=\"#FF0000\">Tanar'ri</font><font color=\"#FFFF00\">}</font>");
    break;
  case CLASS_ANGEL:
    strcat(buf, " <font color=\"#FFFF00\">.x</font><font color=\"#666666\">(</font><font color=\"#FFFFFF\">Angel</font><font color=\"#666666\">)</font><font color=\"#FFFF00\">x.</font>");
    break;
  case CLASS_UNDEAD_KNIGHT:
    strcat(buf, " <font color=\"#666666\">|[</font><font color=\"#3366FF\">Undead Knight</font><font color=\"#666666\">]|</font>");
    break;
  case CLASS_DROID:
    strcat(buf, " <font color=\"#CC33CC\">{</font><font color=\"#666666\">-</font><font color=\"#CC33CC\">}</font><font color=\"#666666\">Spider Droid</font><font color=\"#CC33CC\">{</font><font color=\"#666666\">-</font><font color=\"#CC33CC\">}</font><font color=\"#FFFFFF\"></font>");
    break;
  case CLASS_FAE:
    strcat(buf, " <font color=\"#33FF33\">&lt;&lt;</font><font color=\"#FF0000\">(</font><font color=\"#FFFF00\">Fae</font><font color=\"#FF0000\">)</font><font color=\"#33FF33\">>></font>");
    break;
  case CLASS_DRONE:
    strcat(buf, " <font color=\"#33FF33\">&lt;</font><font color=\"#666666\">=</font><font color=\"#FFFF00\">{</font><font color=\"#666666\">Drone</font><font color=\"#FFFF00\">}</font><font color=\"#666666\">=</font><font color=\"#33FF33\">></font>");
    break;
  case CLASS_GIANT:
    strcat(buf, " <font color=\"#33CCFF\">&lt;></font><font color=\"#33FF33\">Giant</font><font color=\"#33CCFF\">&lt;></font>");
    break;
  case CLASS_HOBBIT:
    strcat(buf, " <font color=\"#FFCC00\">(</font><font color=\"#66FFFF\">*</font><font color=\"#FFCC00\">)</font><font color=\"#FF0000\">Hobbit</font><font color=\"#FFCC00\">(</font><font color=\"#66FFFF\">*</font><font color=\"#FFCC00\">)</font>");
    break;
  }
  return (buf[0] != '\0') ? buf+1 : "none";
}

char * plr_kingdom_name(int arg)
{
  static char buf[512];

  buf[0] = '\0';

  switch(arg)
  {
  case 1:
    strcat(buf, " <font color=\"#33FF33\">=</font><font color=\"#999999\">Loa</font><font color=\"#33FF33\">=</font>");
    break;
  case 2:
    strcat(buf, " <font color=\"#3333FF\">-</font><font color=\"#FFFF00\">X</font><font color=\"#3333FF\">-S</font><font color=\"#999999\">tars</font><font color=\"#3333FF\">-</font><font color=\"#FFFF00\">X</font><font color=\"#3333FF\">-</font>");
    break;
  case 3:
    strcat(buf, " <font color=\"#66FFFF\">::</font><font color=\"#FFCC00\">T</font><font color=\"#FFFF00\">he </font><font color=\"#FFCC00\">O</font><font color=\"#FFFF00\">ne </font><font color=\"#FFCC00\">T</font><font color=\"#FFFF00\">rue</font><font color=\"#66FFFF\">::</font>");
    break;
  case 4:
    strcat(buf, " <font color=\"#33FF33\">=</font><font color=\"#FFFF00\">*</font><font color=\"#33FF33\">=</font><font color=\"#999999\">W</font><font color=\"#33FF33\">ild=</font><font color=\"#FFFF00\">*</font><font color=\"#33FF33\">=</font>");
    break;
  case 5: 
    strcat(buf, " <font color=\"#FF0000\">/></font><font color=\"#FFFFFF\">G</font><font color=\"#999999\">ahr</font><font color=\"#FF0000\">&lt;\\</font>");
    break;
 }
  return (buf[0] != '\0') ? buf+1 : " ";
}

void parse_who()
{
  CHAR_DATA *ch;
  DESCRIPTOR_DATA *d;
  FILE *fp;
  char whofile[200];
  char buf[MAX_STRING_LENGTH];
  char fadein_data[MAX_STRING_LENGTH];
  char fadeout_data[MAX_STRING_LENGTH];
  char body_data[4 * MAX_STRING_LENGTH]; // should be enough
  int pcount = 0, pcountv = 0;

  sprintf(whofile, "%s%s", HTML_DIR, "who.html");
  if ((fp = fopen(whofile, "w")) == NULL)
  {
    log_string("Error writing to who.html");
    return;
  }

  /*
   * Now let's fill out the body of the file.
   */
  sprintf(body_data, "\n");
  for (d = descriptor_list; d; d = d->next)
  {
    if (d->connected != CON_PLAYING) continue;
    if ((ch = d->character) == NULL) continue;
    if (ch->level > 6) continue; // we don't list the immortals.
    pcount++;

    if (IS_SET(ch->act, PLR_WIZINVIS) || IS_SET(ch->act, AFF_HIDE) || IS_SET(ch->newbits, NEW_DARKNESS)) continue;
    pcountv++;

    strcat(body_data, "<tr>\n");
    sprintf(buf, "<td>%s</td>\n", ch->name);
    strcat(body_data, buf);
    sprintf(buf, "<td>%s</td>\n", plr_class_name(ch->class));
    strcat(body_data, buf);
    sprintf(buf, "<td>%s</td>\n", plr_kingdom_name(ch->pcdata->religion));
    strcat(body_data, buf);
    strcat(body_data, "</tr>\n");
  }

  /*
   * Setting up the header and foot of the file
   */
  sprintf(fadein_data, "<html>\n");
  strcat(fadein_data, "<body text=\"#FFFFFF\" bgcolor=\"#FFFFFF\" link=\"#000000\" vlink=\"#000000\" alink=\"#0055FF\">\n");
  strcat(fadein_data, "<table><br>\n");
  strcat(fadein_data, "<table border=0 cellspacing=0 cellpadding=0 width=100% bgcolor=\"#000000\"><tr><td>\n");
  strcat(fadein_data, "<table width=100% border=0 cellspacing=1 cellpadding=3><tr><td colspan=1 bgcolor=\"#999999\">\n");
  strcat(fadein_data, "<font size=4><center><b><font color=\"#000000\">Players Online</font></b></center></td></tr>\n");
  strcat(fadein_data, "<tr><td bgcolor=\"#000000\"><font size=3>\n");
  strcat(fadein_data, "<p><table WIDTH=\"100%\">\n\n<tr>\n");
  strcat(fadein_data, "<td><b><u>Name</u></b></td>\n");
  strcat(fadein_data, "<td><b><u>Class</u></b></td>\n");
  strcat(fadein_data, "<td><b><u>Religion</u></b></td>\n</tr>\n\n");
  
  sprintf(fadeout_data, "</table>\n");
  sprintf(buf, "<p>%d/%d Visible Players Online\n", pcountv, pcount);
  strcat(fadeout_data, buf);
  strcat(fadeout_data, "<p><b><u>Mudinfo Data for this Week</u></b>\n");
  strcat(fadeout_data, "<br><table WIDTH=\"50%\">\n");
  sprintf(buf, "<tr><td>Average Players Online</td>   <td>%d</td></tr>\n",
    (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS])/mudinfo[MUDINFO_UPDATED]);
  strcat(fadeout_data, buf);
  sprintf(buf, "<tr><td>Peak Players Online</td>      <td>%d</td></tr>\n", mudinfo[MUDINFO_PEAK_USERS]);
  strcat(fadeout_data, buf);
  sprintf(buf, "<tr><td>Mccp Usage</td>               <td>%d %%</td></tr>\n",
    100 * mudinfo[MUDINFO_MCCP_USERS] / (mudinfo[MUDINFO_MCCP_USERS] + mudinfo[MUDINFO_OTHER_USERS]));
  strcat(fadeout_data, buf);
  strcat(fadeout_data, "</table>\n");

  strcat(fadeout_data, "<p>This Page is updated every 60 seconds");
  strcat(fadeout_data, "</td></tr>\n");
  strcat(fadeout_data, "</table></td></tr>\n");
  strcat(fadeout_data, "</table>\n");
  strcat(fadeout_data, "</table>\n");
  strcat(fadeout_data, "</body>\n");
  strcat(fadeout_data, "</html>\n");

  /*
   * Write it all to the file, and return.
   */
  fprintf(fp, "%s", fadein_data);
  fprintf(fp, "%s", body_data);
  fprintf(fp, "%s", fadeout_data);

  fclose (fp);
  return;
}

void parse_top()
{
  FILE *fp;
  char topfile[200];
  char buf[MAX_STRING_LENGTH];
  char fadein_data[MAX_STRING_LENGTH];
  char fadeout_data[MAX_STRING_LENGTH];
  char body_data[4 * MAX_STRING_LENGTH]; // should be enough
  int i;

  sprintf(topfile, "%s%s", HTML_DIR, "top.html");
  if ((fp = fopen(topfile, "w")) == NULL)
  {
    log_string("Error writing to top.html");
    return;
  }

  sprintf(fadein_data, "<html>\n");
  strcat(fadein_data, "<body text=\"#000000\" bgcolor=\"#FFFFFF\" link=\"#000000\" vlink=\"#000000\" alink=\"#0055FF\">\n");
  strcat(fadein_data, "<table><br>\n");
  strcat(fadein_data, "<table><pre>\n");
  strcat(fadein_data, " <b><>==<>==<>==<>==<>==<>==<>==  Top PKscore  ==<>==<>==<>==<>==<>==<>==<></b>\n");

  sprintf(fadeout_data, " <b><>==<>==<>==<>==<>==<>==<>==  Top PKscore  ==<>==<>==<>==<>==<>==<>==<></b>\n");
  strcat(fadeout_data, "</pre>\n</table>\n</table>\n</body>\n</html>\n<p>\n");
  strcat(fadeout_data, "<b>This page is updated once every 3 hours</b><br>\n");

  sprintf(body_data, "\n");
  for (i = 1; i <= MAX_TOP_PLAYERS / 2; i++)
  {
    sprintf(buf, "  %2d.  %-15s      %5d       %2d.  %-15s      %5d\n",
      i, top_board[i].name, top_board[i].pkscore,
      i + MAX_TOP_PLAYERS / 2, top_board[i + MAX_TOP_PLAYERS / 2].name,
      top_board[i + MAX_TOP_PLAYERS / 2].pkscore);
    strcat(body_data, buf);
  }
  strcat(body_data, "\n");

  fprintf(fp, "%s", fadein_data);
  fprintf(fp, "%s", body_data);
  fprintf(fp, "%s", fadeout_data);

  fclose (fp);
  return;
}

void parse_leader()
{
  FILE *fp;
  char leaderfile[200];
  char buf[MAX_STRING_LENGTH];
  char fadein_data[MAX_STRING_LENGTH];
  char fadeout_data[MAX_STRING_LENGTH];
  char body_data[4 * MAX_STRING_LENGTH]; // should be enough

  sprintf(leaderfile, "%s%s", HTML_DIR, "leader.html");
  if ((fp = fopen(leaderfile, "w")) == NULL)
  {
    log_string("Error writing to leader.html");
    return;
  }

  sprintf(fadein_data, "<html>\n");
  strcat(fadein_data, "<body text=\"#000000\" bgcolor=\"#FFFFFF\" link=\"#000000\" vlink=\"#000000\" alink=\"#0055FF\">\n");
  strcat(fadein_data, "<table><br>\n");
  strcat(fadein_data, "<table><pre>\n");
  strcat(fadein_data, " <b><>==<>==<>==<>==<>==<>   Leader Board   <>==<>==<>==<>==<>==<></b>\n");

  sprintf(fadeout_data, " <b><>==<>==<>==<>==<>==<>   Leader Board   <>==<>==<>==<>==<>==<></b>\n");
  strcat(fadeout_data, "</pre>\n</table>\n</table>\n</body>\n</html>\n<p>\n");
  strcat(fadeout_data, "<b>This page is updated once every 3 hours</b><br>\n");
  
  sprintf(body_data, "\n");

  strcat(body_data, "   Most Player Kills  --->   ");
  sprintf(buf, "<b>%-13s</b>", leader_board.pk_name);
  strcat(body_data, buf);
  sprintf(buf, " with %d Kills\n", leader_board.pk_number);
  strcat(body_data, buf);

  strcat(body_data, "   Most Hours Played  --->   ");
  sprintf(buf, "<b>%-13s</b>", leader_board.time_name);
  strcat(body_data, buf);
  sprintf(buf, " with %d Hours\n", leader_board.time_number);
  strcat(body_data, buf);

  strcat(body_data, "   Most Qps Earned    --->   ");
  sprintf(buf, "<b>%-13s</b>", leader_board.quest_name);
  strcat(body_data, buf);
  sprintf(buf, " with %d Qps\n", leader_board.quest_number);
  strcat(body_data, buf);

  strcat(body_data, "   Most Mobs Killed   --->   ");
  sprintf(buf, "<b>%-13s</b>", leader_board.mobkills_name);
  strcat(body_data, buf);
  sprintf(buf, " with %d Kills\n", leader_board.mobkills_number);
  strcat(body_data, buf);

  strcat(body_data, "   Highest PK score   --->   ");
  sprintf(buf, "<b>%-13s</b>", leader_board.pkscore_name);
  strcat(body_data, buf);
  sprintf(buf, " with %d Points\n", leader_board.pkscore_number);
  strcat(body_data, buf);

  strcat(body_data, "   Most Arena Wins    --->   ");
  sprintf(buf, "<b>%-13s</b>", leader_board.arena_name);
  strcat(body_data, buf);
  sprintf(buf, " with %d Wins\n", leader_board.arena_number);
  strcat(body_data, buf);

  strcat(body_data, "\n");

  fprintf(fp, "%s", fadein_data);
  fprintf(fp, "%s", body_data);
  fprintf(fp, "%s", fadeout_data);
   
  fclose (fp);
  return;
}
