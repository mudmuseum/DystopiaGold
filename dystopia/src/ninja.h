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
 * Ninja header file.
 */

#define NINJA_POWER               8   // Yes they use class points

/*
 * ch->pcdata->powers[x]
 */
#define NINJA_COMBO               1   // What combos have been used ?
#define NINJA_SHADOW              2   // Skills in shadows
#define NINJA_MARTIAL             3   // Skills in martial arts
#define NINJA_ATTACKS             4   // Learned attack types
#define NINJA_BITS                5   // Active ninja powers
#define NINJA_COMBATTICK          6   // Makes sure combat powers fades
#define NINJA_SOULAMMO            7   // How many charges left on the soulseekers ?

/*
 * Combo counter bits
 */
#define NCOMBO_MOONSTRIKE_1         1
#define NCOMBO_MOONSTRIKE_2         2
#define NCOMBO_SHADOWTHRUST_1       4
#define NCOMBO_SHADOWTHRUST_2       8
#define NCOMBO_DIRTTHROW_1         16
#define NCOMBO_DIRTTHROW_2         32
#define NCOMBO_GUTCUTTER_1         64
#define NCOMBO_GUTCUTTER_2        128
#define NCOMBO_SOULREAPER_1       256
#define NCOMBO_SOULREAPER_2       512
#define NCOMBO_KNIFESPIN_1       1024
#define NCOMBO_KNIFESPIN_2       2048
#define NCOMBO_WAKASASHISLICE_1  4096
#define NCOMBO_WAKASASHISLICE_2  8192
#define NCOMBO_CALTROPS_1       16384
#define NCOMBO_CALTROPS_2       32768

/*
 * Skills in shadows (costs class points to buy)
 */
#define NSHADOWS_SILENTWALK        1  // supersneak (auto)
#define NSHADOWS_HIDE              2  // invis power (vanish)
#define NSHADOWS_SHADOWWALK        4  // shadowwalk power
#define NSHADOWS_MIENAKU           8  // superflee
#define NSHADOWS_SIGHT            16  // truesight
#define NSHADOWS_TPACT            32  // pact for toughness
#define NSHADOWS_DPACT            64  // pact for damage modifier
#define NSHADOWS_SCRY            128  // scrying power
#define NSHADOWS_SHIELD          256  // shadow shield
#define NSHADOWS_SOULSEEKERS     512  // long range attack (tracks down a target)
#define NSHADOWS_ASSASSINATE    1024  // assassinating power

/*
 * Learned Attack Types
 */
#define NATTACK_MOONSTRIKE        1  // must be night
#define NATTACK_SHADOWTHRUST      2
#define NATTACK_DIRTTHROW         4
#define NATTACK_GUTCUTTER         8
#define NATTACK_SOULREAPER       16
#define NATTACK_KNIFESPIN        32  // must be wearing pierce
#define NATTACK_WAKASASHISLICE   64  // must be wearing slice
#define NATTACK_CALTROPS        128

/*
 * Ninja powers (each power costs class points to maintain)
 */
#define NPOWER_KNIFESHIELD        1  // free attacks
#define NPOWER_BLUR               2  // blurring effect, negates attacks
#define NPOWER_BLOODRAGE          4  // more attacks/damage, negates at next combo command
#define NPOWER_SHADOWFORM         8  // high toughness (% change of great modifier), low damage (poor modifier)

// shadowform and bloodrage not possibly together....
