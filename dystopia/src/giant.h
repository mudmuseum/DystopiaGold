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

/* Headers for Giants */

/*
 * ch->pcdata->rank
 */
#define FOOT_10         1
#define FOOT_15         2
#define FOOT_20		3
#define FOOT_25         4
#define FOOT_30         5

/*
 * ch->pcdata->powers
 */
#define GIANT_ATTACK    1
#define GIANT_FRENZY    2
#define GIANT_GIFTS     3
#define GGIFTS_GAINED   4
#define GIANT_STANDFIRM 5

/*
 * ch->pcdata->stats
 */
#define GIANT_POINTS    8  

/*
 * bit vector for giant gifts
 */
#define GGIFT_LEATHERSKIN    1
#define GGIFT_REVIVAL        2
#define GGIFT_EARTHPUNCH     4
#define GGIFT_STANDFIRM      8
#define GGIFT_STONESHAPE    16
#define GGIFT_LONGLEGS      32
