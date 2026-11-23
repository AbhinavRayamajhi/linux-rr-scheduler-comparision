# Kernel
KDIR := /lib/modules/$(shell uname -r)/build
# Release
RDIR := $(CURDIR)/release
# Debug
DDIR := $(CURDIR)/debug

.PHONY: all debug clean

all:
	mkdir -p $(RDIR)
	cp -u sched_switch_hook.c $(RDIR)
	$(MAKE) -C $(KDIR) M=$(RDIR) modules

debug:
	mkdir -p $(DDIR)
	cp -u sched_switch_hook.c $(DDIR)
	$(MAKE) -C $(KDIR) M=$(DDIR) modules

clean:
	$(MAKE) -C $(KDIR) M=$(RDIR) clean
	$(MAKE) -C $(KDIR) M=$(DDIR) clean
