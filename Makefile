# user defined values
include ./Make.conf

# application name
TARGET = CoAJnR

# global flags
CXXFLAGS   += -pipe
WARN_FLAGS	= -Wall
INCLUDE	   += -I./include
LIBPATH		=
LIBRARIES	=
BINPATH		=
TESTBINPATH =
EXTENSION	=

ifeq ($(shell uname -s),Darwin)

# ############################################################################ #
#	MAC OS X																   #
#																			   #
# TODO: the INCLUDE value is cheesy and should be replaced					   #
#		with something more general in the future							   #
#																			   #
  INCLUDE		 += -I/Library/Frameworks/Ogre.framework/Headers -I/Library/Frameworks/OIS.framework/Headers
  LIBPATH		 += 
  LIBRARIES		 += -framework Ogre -framework OIS -framework Cocoa
  BINPATH		  = ./bin/CoAJnR.app/Contents/MacOS
  TESTBINPATH	  = ./bin/Test.app/Contents/MacOS
#																			   #
# ############################################################################ #

else
  ifeq ($(shell uname -s),linux)

# ############################################################################ #
#	LINUX																	   #
#																			   #
	INCLUDE		 += 
	LIBPATH		 += 
	LIBRARIES	 +=
	BINPATH		  =
	TESTBINPATH	  =
#																			   #
# ############################################################################ #
	
  else	

# ############################################################################ #
#	WINDOWS																	   #
#																			   #
	INCLUDE		 += -I"$(OGRE_HOME)/include"  -I"$(OGRE_HOME)/include/OIS"
	LIBPATH		 += -L"$(OGRE_HOME)/bin/release"
	LIBRARIES	 += -lOgreMain -lOIS 
	BINPATH		  = ./bin
	TESTBINPATH	  = ./bin
	EXTENSION	  = .exe
#																			   #
# ############################################################################ #

  endif
endif

# combine the stuff
LIB		   = $(LIBRARIES) $(LIBPATH) $(EXTRA_LIBS)
CXXFLAGS  += $(EXTRA_CXXFLAGS) $(WARN_FLAGS) $(INCLUDE) $(EXTRA_INCLUDES)

SRC_FILES = $(wildcard src/*.cpp)
O_FILES	  = $(SRC_FILES:%.cpp=%.o)


all: libOgreBullet.a $(TARGET) test

$(TARGET): $(O_FILES)
	$(CXX) $(O_FILES) -o $(BINPATH)/$@$(EXTENSION) $(LIB)

test:
	$(CXX) $(CXXFLAGS) test/Test.cpp -o $(TESTBINPATH)/Test$(EXTENSION) $(LIB)

libOgreBullet.a:
	make -C dep/OgreBullet

clean:
	rm -f *.o src/*.o 
	
proper: clean
	rm -f $(BINPATH)/$(TARGET)$(EXTENSION) $(TESTBINPATH)/Test$(EXTENSION)
	make proper -C dep/OgreBullet
