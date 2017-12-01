WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CFLAGS+= -DUIP_CONF_IPV6

PROJECT_SOURCEFILES += null_replayerRDC.c

CONTIKI_PROJECT = replayer

all: $(CONTIKI_PROJECT)  

CONTIKI = /* set the contiki path */

include $(CONTIKI)/Makefile.include
