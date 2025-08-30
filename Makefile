obj-m += lg_magic.o

KDIR ?= /lib/modules/$(KVER)/build
KVER ?= $(shell uname -r)
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
