WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
CFLAGS+= -DUIP_CONF_IPV6

PROJECT_SOURCEFILES += null_sniplayRDC.c

CONTIKI_PROJECT = sniplay

all: $(CONTIKI_PROJECT)  

CONTIKI = /* enter the contiki folder path here */

include $(CONTIKI)/Makefile.include
