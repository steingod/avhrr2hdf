#
# TYPE: 
# Makefile
# 
# PURPOSE:
# To create avhrr2hdf
#
# AUTHOR:
# Steinar Eastwood, met.no, 14.01.2003
#
# MODIFIED:
# Øystein Godøy, METNO/FOU, 05.05.2009: Simple adjustment to new
# libraries, no fancy changes as the format is liekly to be removed in the
# future...
#
# CVS_ID:
# $Id: Makefile,v 1.1 2009-05-07 11:13:32 steingod Exp $
# 

# This include statement reads the module specification of required
# libraries and sets varaibles needed for LDFLAGS and CFLAGS. All required
# information for the module should be put in this file, even if not all
# parts of the module requires them. Specify the requirements by setting
# the adequate FLAGS below.

FMUTILLIB=/disk1/software/lib
FMUTILINC=/disk1/software/include
FMIOLIB=/disk1/software/lib
FMIOINC=/disk1/software/include
OSIHDF5LIB=/disk1/software/lib
OSIHDF5INC=/disk1/software/include
PROJLIB=/usr/lib
PROJINC=/usr/include
HDF5LIB=/disk1/software/lib
HDF5INC=/disk1/software/include

# It is nice to include yourself.

SELFINC = ./

# Loader FLAGS required, specify as needed

LDFLAGS = \
	-L$(FMIOLIB) -lfmio \
	-L$(FMUTILLIB) -lfmutil \
	-L$(OSIHDF5LIB) -losihdf5 \
	-L$(HDF5LIB) -lhdf5 \
	-L$(PROJLIB) -lproj \
	-ltiff -lm -lz

# Turn on optimization or debugging if required.

OPT = -g -O2

# Set CFLAGS as needed.

CFLAGS =  \
	-I$(SELFINC) \
	-I$(PROJINC) \
	-I$(PHYSINC) \
	-I$(SATINC) \
	-I$(FMCOORDINC) \
	-I$(MEOS4INC) \
	-I$(MEOS5INC) \
	-I$(HDF5INC) \
	-I$(OSIHDF5INC) \
	$(OPT)


# List the object files. 

OBJS = \
	avhrr2hdf.o \
	func_avhrr2hdf.o

# Specify the name of the executable file
# This will be installed properly if make install is executed.

RUNFILE = avhrr2hdf

# Specify name of dependency files (e.g. header files)

DEPS = \
	avhrr2hdf.h 

$(RUNFILE): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS)  -o $(RUNFILE) $(LDFLAGS) 

# Specify requirements for the object generation.

$(OBJS): $(DEPS)

clean:
	-rm -f $(OBJS)

distclean:
	-rm -f $(OBJS)
	-rm -f $(RUNFILE)
	-rm -f ../bin/$(RUNFILE)

install:
ifdef RUNFILE
	install $(RUNFILE) ../bin
endif

