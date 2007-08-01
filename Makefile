# user defined values
include ./Make.conf

# application name
TARGET = CoAJnR

# global flags
CXXFLAGS   += -pipe
WARN_FLAGS	= -Wall
INCLUDE	   += -I./include -I./dep/OgreBullet/Collisions/include -I./dep/OgreBullet/Dynamics/include
LIBPATH		= -L./dep/OgreBullet
LIBRARIES	= -lOgreBullet
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
  INCLUDE		 += -I/Library/Frameworks/Ogre.framework/Headers -I/Library/Frameworks/OIS.framework/Headers -I/usr/local/include/bullet -I/Library/Frameworks/SDL.framework/Headers
  LIBPATH		 += 
  LIBRARIES		 += -framework Ogre -framework OIS -framework Cocoa  -lbulletcollision -lbulletdynamics -lbulletmath -framework SDL
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

TEST_SRC_FILES = $(wildcard test/*.cpp)
TEST_O_FILES = $(TEST_SRC_FILES:%.cpp=%.o)

all: libOgreBullet.a $(TARGET) test

$(TARGET): $(O_FILES) libOgreBullet.a
	$(CXX) $(O_FILES) -o $(BINPATH)/$@$(EXTENSION) $(LIB)

test: $(TEST_O_FILES) libOgreBullet.a
	$(CXX) $(TEST_O_FILES) -o $(TESTBINPATH)/Test$(EXTENSION) $(LIB)

libOgreBullet.a:
	make -C dep/OgreBullet

clean:
	rm -f *.o src/*.o 
	
clean-test:
	rm -rf test/*.o

proper: clean clean-test
	make proper -C dep/OgreBullet
	rm -f $(BINPATH)/$(TARGET)$(EXTENSION) $(TESTBINPATH)/Test$(EXTENSION)
