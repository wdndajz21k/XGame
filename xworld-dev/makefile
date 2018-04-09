# make pandora's all projects

MAKE = cd $@ && make clean && make

all: XWorld ZoneCenter WorldServer
.PHONY: all

clean:
	rm -rf Build
.PHONY: clean

XWorld: 
	$(MAKE)
.PHONY: XWorld

WorldServer: XWorld
	$(MAKE)
.PHONY: WorldServer

ZoneCenter: XWorld
	$(MAKE)
.PHONY: ZoneCenter
