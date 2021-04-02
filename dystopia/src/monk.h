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

/*      Fight Style Things, and.. other.. stuff. */

#define FS_TRIP		1
#define FS_KICK		2
#define FS_BASH		4
#define FS_ELBOW	8
#define FS_KNEE		16
#define FS_HEADBUTT	32
#define FS_DISARM	64
#define FS_BITE		128
#define FS_DIRT		256
#define FS_GRAPPLE	512
#define FS_PUNCH	1024
#define FS_GOUGE	2048
#define FS_RIP		4096
#define FS_STAMP	8192
#define FS_BACKFIST	16384
#define FS_JUMPKICK	32768
#define FS_SPINKICK	65536
#define FS_HURL		131072
#define FS_SWEEP	262144
#define FS_CHARGE	524288
#define TECH_THRUST	1048576
#define TECH_SPIN	2097152
#define TECH_ELBOW	4194304
#define TECH_BACK	8388608
#define TECH_PALM	16777216
#define TECH_KNEE	33554432
#define TECH_SWEEP	67108864
#define TECH_SHIN	134217728
#define MONK_DEATH      268435456
#define MONK_HEAL	536870912

/* Combos */

#define COMB_THRUST1		1
#define COMB_THRUST2		2
#define COMB_SHIN		4
#define COMB_KNEE		8
#define COMB_REV1		16
#define COMB_SWEEP		32
/*
				64
				128
				256
				512
				1024
				2048
*/

/* Abilities */
#define AWARE		0
#define BODY		1
#define COMBAT		2
#define SPIRIT		3

/* Chi */
#define CURRENT		0
#define MAXIMUM		1

/* Monk Power Bit */
#define PMONK                   0

