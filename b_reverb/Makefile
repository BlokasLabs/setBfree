MODNAME=Organ Reverb
include ../common.mak
include ../git2lv2.mk

LOADLIBES = -lm
override CFLAGS+=`pkg-config --cflags lv2` -fvisibility=hidden

targets=reverb.o

LV2NAME=b_reverb

ifeq ($(LV2AVAIL), yes)
  targets+=lv2
endif

all: $(targets)

lv2: manifest.ttl $(LV2NAME)$(LIB_EXT) $(LV2NAME).ttl

manifest.ttl:
	sed "s/@LV2NAME@/$(LV2NAME)/;s/@LIB_EXT@/$(LIB_EXT)/;s/@MODGUITTL@/$(MODGUITTL)/" manifest.ttl.in > manifest.ttl

$(LV2NAME).ttl: $(LV2NAME).ttl.in Makefile
	sed "s/@VERSION@/lv2:microVersion $(LV2MIC) ;lv2:minorVersion $(LV2MIN) ;/g;s/@MODBRAND@/$(MODBRAND)/;s/@MODLABEL@/$(MODLABEL)/" \
		$(LV2NAME).ttl.in > $(LV2NAME).ttl

$(LV2NAME)$(LIB_EXT): lv2.c reverb.o
	$(CC) $(CPPFLAGS) $(CFLAGS) \
		-o $(LV2NAME)$(LIB_EXT) lv2.c reverb.o \
	  -shared $(LV2LDFLAGS) $(LDFLAGS)

%.o: %c %.h ../src/cfgParser.h

install: $(targets)
ifeq ($(LV2AVAIL)$(INSTALL_EXTRA_LV2), yesyes)
	install -d $(DESTDIR)$(lv2dir)/$(LV2NAME)
	install -m755 $(LV2NAME)$(LIB_EXT) $(DESTDIR)$(lv2dir)/$(LV2NAME)
	install -m644 manifest.ttl $(LV2NAME).ttl $(DESTDIR)$(lv2dir)/$(LV2NAME)
ifneq ($(MOD),)
	install -m644 modgui.ttl $(DESTDIR)$(lv2dir)/$(LV2NAME)
	install -d $(DESTDIR)$(lv2dir)/$(LV2NAME)/modgui
	cp -r modgui/* $(DESTDIR)$(lv2dir)/$(LV2NAME)/modgui
endif
endif

uninstall:
	rm -f $(DESTDIR)$(lv2dir)/$(LV2NAME)/*.ttl
	rm -f $(DESTDIR)$(lv2dir)/$(LV2NAME)/$(LV2NAME)$(LIB_EXT)
	rm -rf $(DESTDIR)$(lv2dir)/$(LV2NAME)/modgui
	-rmdir $(DESTDIR)$(lv2dir)/$(LV2NAME)

clean:
	rm -f *.o manifest.ttl $(LV2NAME)$(LIB_EXT) $(LV2NAME).ttl

.PHONY: clean all install uninstall lv2
