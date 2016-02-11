CONTIKI_PROJECT = drone_project
all: drone sensor

CONTIKI_WITH_RIME = 1
CONTIKI = ../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
include $(CONTIKI)/Makefile.include


