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

int   calculate_dam              args (( CHAR_DATA *ch, CHAR_DATA *victim, int type ));
char *get_keywords_from_mob      args (( MOB_INDEX_DATA *pMob ));
char *get_description_from_mob   args (( MOB_INDEX_DATA *pMob ));

void do_reform( CHAR_DATA *ch, char *argument )
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE) && !IS_CLASS(ch, CLASS_SAMURAI))
  {
    send_to_char("Huh?\n\r", ch );
    return;
  }
  if (ch->power[DISC_FAE_ARCANE] < 1 && IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("You need level 1 discipline in arcane.\n\r",ch);
    return;
  }
  ch->loc_hp[0] = 0;
  ch->loc_hp[1] = 0;
  ch->loc_hp[2] = 0;
  ch->loc_hp[3] = 0;
  ch->loc_hp[4] = 0;
  ch->loc_hp[5] = 0;
  ch->loc_hp[6] = 0;

  if (IS_CLASS(ch, CLASS_FAE))
  {
    ch->pcdata->powers[FAE_PLASMA_TICK] = 0;
    ch->pcdata->powers[FAE_WILL_TICK] = 0;
    ch->pcdata->powers[FAE_ENERGY_TICK] = 0;
    ch->pcdata->powers[FAE_MATTER_TICK] = 0;
    ch->pcdata->powers[FAE_PLASMA_GROWTH] = 0;
    ch->pcdata->powers[FAE_WILL_GROWTH] = 0;
    ch->pcdata->powers[FAE_ENERGY_GROWTH] = 0;
    ch->pcdata->powers[FAE_MATTER_GROWTH] = 0;
    ch->pcdata->powers[FAE_SHIELD] = 0;
  }
  send_to_char("#pYou reform your body, regrowing lost limbs.#n\n\r",ch);
  return;
}

void do_elementalform( CHAR_DATA *ch, char *argument )
{
  char buf [MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->power[DISC_FAE_NATURE] < 10)
  {
    send_to_char("You are not a true master of nature.\n\r",ch);
    return;
  }
  if (IS_SET(ch->newbits, NEW_CUBEFORM))
  {
    REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    if (IS_SET(ch->affected_by, AFF_PASS_DOOR)) REMOVE_BIT(ch->affected_by, AFF_PASS_DOOR);
    act( "$n turns into a Fae.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You turn into your normal form.\n\r",ch);
    ch->damroll -=350;
    ch->hitroll -=350;
    free_string(ch->morph);
    ch->morph=str_dup("");
    return;
  }
  if (ch->move < 2000 || ch->mana < 2000)
  {
    send_to_char("You need 2000 mana and 2000 move  to change into elemental form.\n\r",ch);
    return;
  }
  else
  {
    act( "$n transforms into an elemental storm.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You become one with the elements, letting your rage guide you.\n\r",ch);
    if (!IS_SET(ch->affected_by, AFF_PASS_DOOR)) SET_BIT(ch->affected_by, AFF_PASS_DOOR);
    SET_BIT(ch->newbits, NEW_CUBEFORM);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf,"#oAn #sElemental #oStorm #0(#g%s#0)#n",ch->name);
    free_string(ch->morph);
    ch->morph = str_dup(buf);
    ch->move -= 2000;
    ch->mana -= 2000;
    ch->damroll +=350;
    ch->hitroll +=350;
    return;
  }
  return;
}

void do_channel(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    do_channels(ch, argument);
    return;
  }

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char("Start channeling what, and at what speed?\n\r",ch);
    return;
  }
  if (!is_number(arg2) || (atoi(arg2) < 1 || atoi(arg2) > 3))
  {
    send_to_char("The speed should be a number between 1 and 3.\n\r",ch);
    return;
  }
  if (!str_prefix(arg1,"matter"))
  {
    if (atoi(arg2) > ch->pcdata->powers[FAE_MATTER])
    {
      send_to_char("You can't channel faster than your current level.\n\r",ch);
      return;
    }
    ch->pcdata->powers[FAE_MATTER_GROWTH] = atoi(arg2);
    if (ch->pcdata->powers[FAE_MATTER_TICK] == 0)
      ch->pcdata->powers[FAE_MATTER_TICK] = atoi(arg2);
    send_to_char("#pOk, you start channeling matter#n.\n\r",ch);
  }
  else if (!str_prefix(arg1,"will")) 
  {
    if (atoi(arg2) > ch->pcdata->powers[FAE_WILL])
    {
      send_to_char("You can't channel faster than your current level.\n\r",ch);
      return;
    }
    ch->pcdata->powers[FAE_WILL_GROWTH] = atoi(arg2);
    if (ch->pcdata->powers[FAE_WILL_TICK] == 0)
      ch->pcdata->powers[FAE_WILL_TICK] = atoi(arg2);
    send_to_char("#pOk, you start concentrating your willpower#n.\n\r",ch);
  }
  else if (!str_prefix(arg1,"plasma"))
  {
    if (atoi(arg2) > ch->pcdata->powers[FAE_PLASMA])
    {
      send_to_char("You can't channel faster than your current level.\n\r",ch);
      return;
    }
    ch->pcdata->powers[FAE_PLASMA_GROWTH] = atoi(arg2);
    if (ch->pcdata->powers[FAE_PLASMA_TICK] == 0)    
      ch->pcdata->powers[FAE_PLASMA_TICK] = atoi(arg2);
    send_to_char("#pOk, you start channeling plasma.#n\n\r",ch);
  }
  else if (!str_prefix(arg1,"energy"))
  {
    if (atoi(arg2) > ch->pcdata->powers[FAE_ENERGY])
    {
      send_to_char("You can't channel faster than your current level.\n\r",ch);
      return;
    }
    ch->pcdata->powers[FAE_ENERGY_GROWTH] = atoi(arg2);
    if (ch->pcdata->powers[FAE_ENERGY_TICK] == 0)
      ch->pcdata->powers[FAE_ENERGY_TICK] = atoi(arg2);
    send_to_char("#pOk, you start channeling energy.#n\n\r",ch);
  }
  else send_to_char("You cannot channel that.\n\r",ch);
  return;
}

void do_infuse(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int dam,i,sn;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (arg1[0] == '\0')
  {
    send_to_char("Infuse what into whom?\n\r",ch);
    return;
  }
  if (!str_prefix(arg1,"matter"))
  {
    if (ch->pcdata->powers[FAE_MATTER_TICK] == 0)
    {
      send_to_char("Your not charging matter.\n\r",ch);
      return;
    }
    if (ch->pcdata->powers[FAE_SHIELD] > 0)
    {
      send_to_char("Your shield still exists.\n\r",ch);
      return;
    }
    send_to_char("You weave a shield of charged matter from the fabric of the arcane.\n\r",ch);
    act("$n mutters a series of arcane words and weaves a shimmering shield around $m.",  ch, NULL, NULL, TO_ROOM);
    ch->pcdata->powers[FAE_SHIELD] = ch->pcdata->powers[FAE_MATTER_TICK]*7; // turns on the shield.
    ch->pcdata->powers[FAE_MATTER_TICK] = 0;
    ch->pcdata->powers[FAE_MATTER_GROWTH] = 0;
    WAIT_STATE(ch,12);
    return;
  }
  if (arg2[0] == '\0' && ch->fighting != NULL) victim = ch->fighting;
  else if ((victim = get_char_room(ch, arg2)) == NULL)
  {
    send_to_char("They are not here.\n\r", ch );
    return;
  }
  if (is_safe(ch,victim)) return;
  if (!str_prefix(arg1,"energy"))
  {
    if (ch->pcdata->powers[FAE_ENERGY_TICK] == 0)
    {
      send_to_char("Your not charging energy.\n\r",ch);
      return;
    }
    act("#pYour infuse $N's body with static energy, causing great pain.#n" ,ch,NULL,victim,TO_CHAR);
    act("#p$n#p touches you and chilling energies flow through your body.#n",ch,NULL,victim,TO_VICT);   
    act("#p$n#p touches $N, sending sparks flying.#n" ,ch,NULL,victim,TO_NOTVICT);
    dam = calculate_dam(ch,victim, FAE_ENERGY);
    damage(ch, victim, dam, gsn_lightning);
    ch->pcdata->powers[FAE_ENERGY_GROWTH] = 0;
    ch->pcdata->powers[FAE_ENERGY_TICK] = 0;
    WAIT_STATE(ch,6);
    return;
  }
  else if (!str_prefix(arg1,"plasma"))
  {
    if (ch->pcdata->powers[FAE_PLASMA_TICK] == 0)
    {
      send_to_char("Your not charging plasma.\n\r",ch);
      return;
    }
    act("#pYour body pulsates as you channel pure plasma into $N.#n" ,ch,NULL,victim,TO_CHAR);
    act("#p$n#p's body pulsates as $e channels pure plasma into you.#n",ch,NULL,victim,TO_VICT);
    act("#p$n#p's body pulsates as $e channels pure plasma into $N.#n" ,ch,NULL,victim,TO_NOTVICT);
    dam = calculate_dam(ch,victim, FAE_PLASMA);
    for (i = 0; i < ch->pcdata->powers[FAE_PLASMA_TICK]; i++)
    {
      damage(ch, victim, dam, gsn_plasma);
      dam *= 0.9;
    }
    ch->pcdata->powers[FAE_PLASMA_GROWTH] = 0;
    ch->pcdata->powers[FAE_PLASMA_TICK] = 0;
    WAIT_STATE(ch,12);
    return;
  }
  else if (!str_prefix(arg1,"will"))
  {
    if (ch->pcdata->powers[FAE_WILL_TICK] == 0)
    {
      send_to_char("Your not trying to concentrate your will.\n\r",ch);
      return;
    }
    act("#pYou bend the laws of gravity with your will and slam $N#p with stones, following up with a few spells.#n" ,ch,NULL,victim,TO_CHAR);
    act("#p$n#p stares at you while muttering a few arcane words.#n",ch,NULL,victim,TO_VICT);
    act("#pStones fly from the ground and slams into you while\n\r$n#p continues with the spellcasting.#n",ch,NULL,victim,TO_VICT);
    act("#pstones fly from the gound and slams into $N while\n\r$n#p mutters some arcane phrases.#n" ,ch,NULL,victim,TO_NOTVICT);
    for (i = 0; i < ch->pcdata->powers[FAE_WILL_TICK]; i+=2)
    {
      dam = calculate_dam(ch,victim, FAE_WILL);
      damage(ch, victim, dam, gsn_telekinetic);
    }
    if ((sn = skill_lookup( "curse" ) ) > 0) (*skill_table[sn].spell_fun) (sn,50,ch,victim);
    if ((sn = skill_lookup( "web" ) ) > 0) (*skill_table[sn].spell_fun) (sn,50,ch,victim);
    ch->pcdata->powers[FAE_WILL_GROWTH] = 0;
    ch->pcdata->powers[FAE_WILL_TICK] = 0;
    WAIT_STATE(ch,12);
    return;
  }
  else send_to_char("You have no idea how to infuse that.\n\r",ch);
  return;
}

void do_conjurebow(CHAR_DATA *ch, char *argument)
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;   
  AFFECT_DATA paf;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 33260;
  int cost;

  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char("How much qps do you want to spend on the bow (max 20K) ?\n\r",ch);
    return;
  }
  if (!is_number(arg))
  {
    send_to_char("A number please.\n\r",ch);
    return;
  }
  cost = atoi(arg);
  if (cost < 1000 || cost > 20000)
  {
    send_to_char("between 1K and 20K please.\n\r",ch);
    return;
  }
  if (ch->pcdata->quest < cost)
  {
    send_to_char("You don't have that many qps.\n\r",ch);
    return;
  }
  if ((pObjIndex = get_obj_index( vnum )) == NULL)
  {
    send_to_char("Missing object, please inform a God.\n\r",ch);
    return;
  }
  obj = create_object(pObjIndex, 50);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj->ownerid = ch->pcdata->playerid;
  if (cost > 19000)
  {
    obj->value[1] = 60;
    obj->value[2] = 90;
  }
  else if (cost > 15000)
  {
    obj->value[1] = 50;
    obj->value[2] = 80;
  }
  else if (cost > 10000)
  {
    obj->value[1] = 50;
    obj->value[2] = 70;
  }
  else if (cost > 5000)
  {
    obj->value[1] = 40;
    obj->value[2] = 60;
  }
  else
  {
    obj->value[1] = 30;
    obj->value[2] = 50;
  }

  paf.type           = 0;
  paf.duration       = -1; 
  paf.location       = APPLY_HITROLL;
  paf.modifier       = cost/40;
  paf.bitvector      = 0;
  affect_to_obj(obj, &paf);

  paf.type           = 0;
  paf.duration       = -1;
  paf.location       = APPLY_DAMROLL;
  paf.modifier       = cost/40;
  paf.bitvector      = 0;
  affect_to_obj(obj, &paf);

  paf.type           = 0;
  paf.duration       = -1;
  paf.location       = APPLY_AC;
  paf.modifier       = -cost/30;
  paf.bitvector      = 0;
  affect_to_obj(obj, &paf);

  obj_to_char(obj, ch);
  ch->pcdata->quest -= cost;
  act("You stretch your mind into the plane of energy and draw $p directly from the weave of energy.",ch,obj,NULL,TO_CHAR);
  act("$p appears in $n's hands in a flash of energy.",ch,obj,NULL,TO_ROOM);
  return;
}

void do_ancients(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  int new_power;
  int cost = 2000000; // 2 million

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;
  if( !IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("What?\n\r",ch);
    return;
  }
  if (arg[0] == '\0')
  {
    sprintf(buf,"#pPlasma  #G[#y%d#G]   #pMatter    #G[#y%d#G]   #pEnergy   #G[#y%d#G]   #pWill   #G[#y%d#G]#n\n\r",
      ch->pcdata->powers[FAE_PLASMA], ch->pcdata->powers[FAE_MATTER],
      ch->pcdata->powers[FAE_ENERGY], ch->pcdata->powers[FAE_WILL]);
    send_to_char(buf,ch);
    send_to_char("\n\r#p        Which ancient would you like to improve?#n\n\r",ch);
    return;
  }
  if (!str_cmp(arg,"plasma")) new_power = FAE_PLASMA;
  else if (!str_cmp(arg,"will")) new_power = FAE_WILL;
  else if (!str_cmp(arg,"energy")) new_power = FAE_ENERGY;
  else if (!str_cmp(arg,"matter")) new_power = FAE_MATTER;
  else
  {
    send_to_char("No such ancients.\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[new_power] > 7)
  {
    send_to_char("You know it all, your the champ.\n\r",ch);
    return;
  }
  if (ch->wpn[6] < (ch->pcdata->powers[new_power]+1)*100)
  {
    send_to_char("Your blasting powers are not good enough.\n\r",ch);
    return;
  }
  cost = cost * (ch->pcdata->powers[new_power]+1) * (ch->pcdata->powers[new_power]+1);
  if (ch->exp < cost)
  {
    sprintf(buf,"You need %d more exp to gain that ancient.\n\r", cost - ch->exp);
    send_to_char(buf,ch);
    return;
  }
  ch->exp -= cost;
  ch->pcdata->powers[new_power]++;
  send_to_char("Ok.\n\r",ch);
  return;
}

void do_will(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[FAE_WILL] < 8)
  {
    send_to_char("You have not mastered your own will yet.\n\r",ch);
    return;
  }
  if (IS_SET(ch->affected_by, AFF_FLYING))
  {
    REMOVE_BIT(ch->affected_by, AFF_FLYING);
    send_to_char("You release your control and float slowly to the ground.\n\r",ch);
  }
  else
  {
    SET_BIT(ch->affected_by, AFF_FLYING);
    send_to_char("You concentrate on the forces of gravity, and slowly float into the air.\n\r",ch);
  }
  WAIT_STATE(ch, 8);
  return;
}

void do_matter(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[FAE_MATTER] < 8)
  {
    send_to_char("Your control over matter is to weak.\n\r",ch);
    return;
  }
  if (IS_AFFECTED(ch, AFF_SANCTUARY))
  {
    REMOVE_BIT(ch->affected_by, AFF_SANCTUARY);
    send_to_char("No longer needing the shield, you disband it.\n\r",ch);
  }
  else
  {
    SET_BIT(ch->affected_by, AFF_SANCTUARY);
    send_to_char("You bend the fabric of space to form a shimmering shield around you.\n\r",ch);
  }
  WAIT_STATE(ch,8);
  return;
}  

void do_energy(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[FAE_ENERGY] < 8)
  {
    send_to_char("You cannot harness the energy yet.\n\r",ch);
    return;
  }
  if (IS_SET(ch->itemaffect, ITEMA_CHAOSSHIELD))
  {
    if(IS_ITEMAFF(ch, ITEMA_FIRESHIELD))
      REMOVE_BIT(ch->itemaffect, ITEMA_FIRESHIELD);
    if(IS_ITEMAFF(ch, ITEMA_ACIDSHIELD))
      REMOVE_BIT(ch->itemaffect, ITEMA_ACIDSHIELD);
    if(IS_ITEMAFF(ch, ITEMA_SHOCKSHIELD))
      REMOVE_BIT(ch->itemaffect, ITEMA_SHOCKSHIELD);
    if(IS_ITEMAFF(ch, ITEMA_ICESHIELD))
      REMOVE_BIT(ch->itemaffect, ITEMA_ICESHIELD);
    if(IS_ITEMAFF(ch, ITEMA_CHAOSSHIELD))
      REMOVE_BIT(ch->itemaffect, ITEMA_CHAOSSHIELD);
    REMOVE_BIT(ch->itemaffect, ITEMA_CHAOSHANDS);
    send_to_char("Your shields flicker and die.\n\r",ch);
  }
  else   
  {
    if(!IS_ITEMAFF(ch, ITEMA_FIRESHIELD))
      SET_BIT(ch->itemaffect, ITEMA_FIRESHIELD);
    if(!IS_ITEMAFF(ch, ITEMA_ACIDSHIELD))
      SET_BIT(ch->itemaffect, ITEMA_ACIDSHIELD);
    if(!IS_ITEMAFF(ch, ITEMA_SHOCKSHIELD))
      SET_BIT(ch->itemaffect, ITEMA_SHOCKSHIELD);
    if(!IS_ITEMAFF(ch, ITEMA_ICESHIELD))
      SET_BIT(ch->itemaffect, ITEMA_ICESHIELD);
    if(!IS_ITEMAFF(ch, ITEMA_CHAOSSHIELD))
      SET_BIT(ch->itemaffect, ITEMA_CHAOSSHIELD);
    SET_BIT(ch->itemaffect, ITEMA_CHAOSHANDS);
    send_to_char("You form shields of different colors, each representing the different energies in the universe.\n\r",ch);
  }
  WAIT_STATE(ch, 8);
  return;
}  

void do_plasma2(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (ch->pcdata->powers[FAE_PLASMA] < 8)
  {
    send_to_char("The plasma refuse to do your bidding.\n\r",ch);
    return;
  }
  if (IS_AFFECTED(ch, AFF_PROTECT) || IS_AFFECTED(ch, AFF_PROTECT_GOOD))
  {
    if (IS_SET(ch->affected_by, AFF_PROTECT)) REMOVE_BIT(ch->affected_by, AFF_PROTECT);
    if (IS_SET(ch->affected_by, AFF_PROTECT_GOOD)) REMOVE_BIT(ch->affected_by, AFF_PROTECT_GOOD);
    send_to_char("You call off your shield of protection.\n\r",ch);
  }
  else
  {
    if (IS_GOOD(ch))
    {
      SET_BIT(ch->affected_by, AFF_PROTECT);
      send_to_char("You summon a mystical force to protect you from evil.\n\r",ch);
    }
    else if (IS_EVIL(ch))
    {
      SET_BIT(ch->affected_by, AFF_PROTECT_GOOD);
      send_to_char("You summon a mystical force to protect you from good.\n\r",ch);
    }
    else send_to_char("Nothing happens.\n\r",ch);
  }
  WAIT_STATE(ch, 8);
  return;
}  

int calculate_dam(CHAR_DATA *ch, CHAR_DATA *victim, int type)
{
  int dam = 0;

  dam += char_damroll(ch)/5;
  if (type == FAE_PLASMA)
  {
    dam *= 1 + ch->pcdata->powers[FAE_PLASMA]/4;
    dam *= ch->pcdata->powers[FAE_PLASMA_TICK]/1.3;
  }
  else if (type == FAE_ENERGY)
  {
    dam *= 1 + ch->pcdata->powers[FAE_ENERGY]/2;
    dam *= ch->pcdata->powers[FAE_ENERGY_TICK]*1.5;
  }
  else if (type == FAE_WILL)
  {
    dam *= 1 + ch->pcdata->powers[FAE_WILL]/4;
    dam *= ch->pcdata->powers[FAE_WILL_TICK]/1.8;
  }
  dam = number_range(dam/3, dam/2);
  if (dam > ch->damcap[DAM_CAP]) dam = ch->damcap[DAM_CAP] + number_range(-100,100);
  if (IS_AFFECTED(victim, AFF_SANCTUARY)) dam /= 2;
  return dam;
}

void do_djinn(CHAR_DATA *ch, char *argument)
{
  MOB_INDEX_DATA *pMobIndex;
  MOB_TRIGGER *mProg;
  CHAR_DATA *mob;
  ROOM_INDEX_DATA *pRoom;
  int vnum;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (ch->power[DISC_FAE_NATURE] < 8)
  {
    send_to_char("You need at least level 8 in nature to summon The Djinn.\n\r",ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_DJINN))
  {
    send_to_char("You can only summon the djinn once every 24 hours.\n\r", ch);
    return;
  }
  if ((pRoom = get_room_index(3001)) == NULL) return;
  if (pRoom->area == ch->in_room->area)
  {
    send_to_char("This power cannot function in midgaard.\n\r", ch);
    return;
  }
  if ((pMobIndex = get_mob_index(20)) == NULL)
  {
    send_to_char("Bug, please report this to an immortal.\n\r", ch);
    return;
  }
  SET_TIMER(ch, TIMER_DJINN, 24);
  mob = create_mobile(pMobIndex);
  mob->decay_pulse = number_range(4, 8);

  for (vnum = ch->in_room->area->lvnum; vnum <= ch->in_room->area->uvnum; vnum++)
  {
    if ((pMobIndex = get_mob_index(vnum)) != NULL)
    {
      if (trigger_free == NULL)
      {
        mProg             = alloc_perm(sizeof(*mProg));
      }
      else
      {
        mProg             = trigger_free;
        trigger_free      = trigger_free->next;
      }
      mProg->type         = MTRIG_SAY;
      mProg->vnum         = 0;
      mProg->keywords     = str_dup(get_keywords_from_mob(pMobIndex));
      mProg->roomOutput   = str_dup(get_description_from_mob(pMobIndex));
      mProg->chOutput     = str_dup("");
      mProg->next         = mob->triggers;
      mob->triggers       = mProg;
    }
  }
  char_to_room(mob, ch->in_room);
  do_say(ch, "Oh mighty djinn of the seven ages, I bid you to join my service");
  act("$n appears in a puff of smoke.", mob, NULL, NULL, TO_ROOM);
  do_say(mob, "I come to do your bidding master.");
  return;
}

void do_genie(CHAR_DATA *ch, char *argument)
{
  MOB_INDEX_DATA *pMobIndex;
  MOB_TRIGGER *mProg;
  CHAR_DATA *mob;

  if (IS_NPC(ch)) return;
  if (!IS_CLASS(ch, CLASS_FAE))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }
  if (ch->power[DISC_FAE_ARCANE] < 8)
  {
    send_to_char("You need at least level 8 in arcane to summon The Genie.\n\r",ch);
    return;
  }
  if (!TIME_UP(ch, TIMER_GENIE))
  {
    send_to_char("You can only summon The Genie once every 24 hours.\n\r", ch);
    return;
  }
  if ((pMobIndex = get_mob_index(21)) == NULL)
  {
    send_to_char("Bug, please report this to an immortal.\n\r", ch);
    return;
  }
  SET_TIMER(ch, TIMER_GENIE, 24);
  mob = create_mobile(pMobIndex);
  mob->decay_pulse = number_range(4, 8);

/*
  if (trigger_free == NULL)
  {
    mProg             = alloc_perm(sizeof(*mProg));
  }
  else
  {
    mProg             = trigger_free;
    trigger_free      = trigger_free->next;
  }
  mProg->type         = MTRIG_SCRY;
  mProg->vnum         = 0;
  mProg->keywords     = str_dup("&scry*");
  mProg->roomOutput   = str_dup("");
  mProg->chOutput     = str_dup("");
  mProg->next         = mob->triggers;
  mob->triggers       = mProg;
*/

  if (trigger_free == NULL)
  {
    mProg             = alloc_perm(sizeof(*mProg));
  }
  else
  {
    mProg             = trigger_free;
    trigger_free      = trigger_free->next;
  }
  mProg->type         = MTRIG_LOCATE;
  mProg->vnum         = 0;
  mProg->keywords     = str_dup("&locate*");
  mProg->roomOutput   = str_dup("I suggest you walk the following path, '%s'.");
  mProg->chOutput     = str_dup("I'm afraid I cannot locate that person master.");
  mProg->next         = mob->triggers;
  mob->triggers       = mProg;

  char_to_room(mob, ch->in_room);
  do_say(ch, "Oh mighty genie, bloodson to the Prince of the East, head my call");
  act("$n appears in a puff of smoke.", mob, NULL, NULL, TO_ROOM);
  do_say(mob, "Ask your questions mortal, before I bid you goodbye.");
  return;
}

char *get_keywords_from_mob(MOB_INDEX_DATA *pMob)
{
  static char buf[MAX_STRING_LENGTH];
  char *ptr, *ptr2, *ptr3;

  buf[0] = '\0';

  ptr = pMob->player_name;
  ptr2 = buf;
  ptr3 = "&what*|";

  while ((*ptr2 = *ptr3) != '\0')
    ++ptr2, ++ptr3;

  while (*ptr != '\0')
  {
    if (*ptr != ' ') *ptr2 = *ptr;
    else
    {
      *ptr2 = '*';
    }
    ptr++; ptr2++;
  }
  *ptr2 = '*'; ptr2++; *ptr2 = '\0';

  return buf;
}

char *mob_might(MOB_INDEX_DATA *pMob)
{
  static char buf[MAX_STRING_LENGTH];

  if (pMob->level < 100)
    sprintf(buf, "a pitiful creature");
  else if (pMob->level < 250)
    sprintf(buf, "a dangerous creature");
  else if (pMob->level < 500)
    sprintf(buf, "a fierce beast");
  else if (pMob->level < 1000)
    sprintf(buf, "a mighty beast");
  else
    sprintf(buf, "a powerful being");

  return buf;
}
    
char *mob_toughness(MOB_INDEX_DATA *pMob)
{
  static char buf[MAX_STRING_LENGTH];

  if (pMob->toughness < - 50)
    sprintf(buf, "a frail body");
  else if (pMob->toughness < 0)
    sprintf(buf, "a weak body");
  else if (pMob->toughness < 25)
    sprintf(buf, "a normal toughness");
  else if (pMob->toughness < 50)
    sprintf(buf, "increased toughness");
  else
    sprintf(buf, "superior toughness");
  return buf;
}
   
char *mob_attacks(MOB_INDEX_DATA *pMob)
{
  static char buf[MAX_STRING_LENGTH];

  if (pMob->extra_attack < 1)
    sprintf(buf, "a few");
  else if (pMob->extra_attack < 5)
    sprintf(buf, "some");
  else if (pMob->extra_attack < 10)  
    sprintf(buf, "many");
  else if (pMob->extra_attack < 15)
    sprintf(buf, "increadible many");
  else
    sprintf(buf, "a huge amount of");

  return buf;
}
 
char *mob_damage(MOB_INDEX_DATA *pMob)
{
  static char buf[MAX_STRING_LENGTH];
  
  if (pMob->dam_modifier < 2)
    sprintf(buf, "a normal amount of");
  else if (pMob->dam_modifier == 2)
    sprintf(buf, "an increased amount of");
  else if (pMob->dam_modifier > 2)
    sprintf(buf, "a huge amount of");

  return buf;
} 

char *get_description_from_mob(MOB_INDEX_DATA *pMob)
{
  static char buf[MAX_STRING_LENGTH];

  sprintf(buf, "%s is %s with %s. It has %s attacks, each dealing %s damage.",
    pMob->short_descr, mob_might(pMob), mob_toughness(pMob), mob_attacks(pMob), mob_damage(pMob));
  return buf;
}

char *cut_after_tracking(char *str)
{
  static char buf[MAX_STRING_LENGTH];
  char *ptr;
  bool found = FALSE;

  buf[0] = '\0';
  ptr = buf;

  if (!is_contained2(" tracking ", str)) return NULL;

  for (;;)
  {
    while (*str != ' ') str++;
    str++;

    while (*str != '\0' && *str != ' ')
    {
      *ptr = *str;
      ptr++; str++;
    }
    *ptr = '\0';

    if (found) return buf;
    if (!str_cmp(buf, "tracking"))
      found = TRUE;

    buf[0] = '\0';
    ptr = buf;
  }
  return buf;
}

char *cut_after_locate(char *str)
{
  static char buf[MAX_STRING_LENGTH];
  char *ptr;
  bool found = FALSE;

  buf[0] = '\0';
  ptr = buf;

  if (!is_contained2(" locate ", str)) return NULL;

  for (;;)
  {
    while (*str != ' ') str++;
    str++;

    while (*str != '\0' && *str != ' ')
    {
      *ptr = *str;
      ptr++; str++;
    }
    *ptr = '\0';

    if (found) return buf;
    if (!str_cmp(buf, "locate"))
      found = TRUE;

    buf[0] = '\0';
    ptr = buf;
  }
  return buf;
}

char *cut_after_scry(char *str)
{
  static char buf[MAX_STRING_LENGTH];
  char *ptr;
  bool found = FALSE;
  
  buf[0] = '\0';
  ptr = buf;
  
  if (!is_contained2(" scry ", str)) return NULL;
  
  for (;;)
  {
    while (*str != ' ') str++;
    str++;
    
    while (*str != '\0' && *str != ' ')
    {
      *ptr = *str;
      ptr++; str++;
    }
    *ptr = '\0';
 
    if (found) return buf;
    if (!str_cmp(buf, "scry"))
      found = TRUE;
      
    buf[0] = '\0';
    ptr = buf;
  }
  return buf;
}
