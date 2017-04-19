.PHONY: all client server
	
client:
	$(MAKE) -C client-side all
	$(MAKE) -C dual-side all
	
server:
	$(MAKE) -C dual-side all
	$(MAKE) -C server-side all
	
all: client server