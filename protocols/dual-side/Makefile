.PHONY: all

PROTOCOL_SOURCES := $(wildcard *.xml)
PROTOCOL_HEADERS := $(PROTOCOL_SOURCES:.xml=.h)
PROTOCOL_CODE := $(PROTOCOL_SOURCES:.xml=.inc)  

%.h: %.xml
	wayland-scanner client-header $< ../../include/nwm/$@  
	wayland-scanner server-header $< ../../generated/headers/$@ 

%.inc: %.xml
	wayland-scanner code $< ../../generated/includes/$@ 
	
decorators.inc : decorators.xml
	wayland-scanner code $< ../../necronomicon/src/$@
	wayland-scanner code $< ../../generated/includes/$@ 
	
all: $(PROTOCOL_HEADERS) $(PROTOCOL_CODE)
	
