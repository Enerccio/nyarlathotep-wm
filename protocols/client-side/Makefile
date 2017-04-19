.PHONY: all

PROTOCOL_SOURCES := $(wildcard *.xml)
PROTOCOL_HEADERS := $(PROTOCOL_SOURCES:.xml=.h)

%.h: %.xml
	wayland-scanner client-header $< ../../include/nwm/$@  
	
all: $(PROTOCOL_HEADERS) $(PROTOCOL_CODE)
	