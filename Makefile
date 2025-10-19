# name of your application
APPLICATION = assignment2

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Add the module for the ST77XX display
USEMODULE += st7735

# Add the module for the LSM6DS3-TR-C IMU
USEMODULE += lsm6dsxx

# to print floats
USEMODULE += printf_float

# generate random numbers
USEMODULE += random

# mineplex
USEMODULE += mineplex

USEMODULE += benchmark
USEMODULE += ztimer
USEMODULE += ztimer_sec

# Use the default file system
USEMODULE += vfs_default
USEMODULE += constfs

# For LittleFS on real devices, the main stack size has to be
# increased:
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048



include $(RIOTBASE)/Makefile.include