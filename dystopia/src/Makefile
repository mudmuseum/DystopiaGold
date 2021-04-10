# CC      = gcc
CC      = aarch64-linux-gnu-gcc
C_FLAGS = -Wall -g
L_FLAGS = -lz -lcrypt -lpthread
# STATIC  = -static

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o bit.o arena.o mccp.o \
	board.o build.o clan.o comm.o daemon.o db.o angel.o religion.o socials.o soundex.o \
	demon.o drow.o fight.o kav_wiz.o handler.o kav_fight.o kav_info.o drone.o giant.o \
	interp.o mage.o magic.o mem.o monk.o monk2.o ninja.o olc.o upgrade.o \
	olc_act.o olc_save.o save.o special.o string.o spiderdroid.o jope.o msp.o lich.o \
	update.o vamp.o ww.o shapeshifter.o tanarri.o undead_knight.o fae.o jobo_update.o \
        desc_map.o samurai.o wizutil.o const.o hobbit.o jobo_king.o jobo_save.o jobo_comm.o \
	jobo_html.o jobo_quest.o jobo_fight.o jobo_act.o jobo_wiz.o jobo_util.o jobo_shop.o

merc: $(O_FILES)
	rm -f dystopia
	$(CC)  -o dystopia $(O_FILES) $(L_FLAGS) # $(STATIC)
	chmod g+w dystopia
	chmod g+w $(O_FILES)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean :
	rm -f *.o
	rm -f dystopia
	rm -f *.*~
