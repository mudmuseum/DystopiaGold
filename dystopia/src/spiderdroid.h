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
 * Place to store class points (ch->pcdata->stats[x])
 */
#define DROID_POWER    8

/*
 * Class flags :)
 */
#define DROID_IMPLANTS  1  // bitvector for implants...
#define DROID_AMMO      2  // how much ammo has the droid loaded...
#define DROID_ROCKET    3  // how many rockets does the droid have loaded
#define DROID_SPEED     4  // how fast is the droid...

/*
 * Bits for implants
 */
#define IMPLANT_EYES               1  // truesight
#define IMPLANT_EXTRA_ARMS         2  // extra attacks
#define IMPLANT_RADAR              4  // scry power
#define IMPLANT_SUBMACHINEGUN      8  // sub-machinegun
#define IMPLANT_ROCKETLAUNCHER    16  // rocket launcher
#define IMPLANT_BODYARMOR         32  // extra toughness
#define IMPLANT_ARMWIRE           64  // extra parry ability
#define IMPLANT_360VISION        128  // allround vision == dodge
#define IMPLANT_ARMORY           256  // extra ammo
#define IMPLANT_STEAMENGINE      512  // extra punch == damcap/damage modifier
#define IMPLANT_BREATHCANNON    1024  // cannon, only works with steam engine.
#define IMPLANT_BATTLESTATION   2048  // the loved cubeform from supermud
#define IMPLANT_SHADOWGATE      4096  // darkness and shadowwalk power
#define IMPLANT_TUNEDENGINE     8192  // need steam-engine - doubles damcap bonus
#define IMPLANT_REGROWTH       16384  // regeneration skill
#define IMPLANT_LARVAFEET      32768  // enables walking
