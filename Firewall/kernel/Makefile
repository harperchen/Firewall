chrdev-objs := output.o hashTable.o lineTable.o utils.o hook.o filter.o nat.o
obj-m += chrdev.o

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean