# global flags
CXXFLAGS   += -pipe
WARN_FLAGS	= -Wall
INCLUDE	   += -I./include
LIBPATH		=
LIBRARIES	=

ifeq ($(shell uname -s),Darwin)

# ############################################################################ #
#	MAC OS X																   #
#																			   #
# TODO: the INCLUDE value is cheesy and should be replaced					   #
#		with something more general in the future							   #
#																			   #
  INCLUDE		 += -I/Library/Frameworks/Ogre.framework/Headers -I/Library/Frameworks/OIS.framework/Headers -I /usr/local/include/bullet
  LIBPATH		 += 
  LIBRARIES		 += -framework Ogre -framework OIS -framework Cocoa -lbulletcollision -lbulletdynamics -lbulletmath
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
#																			   #
# ############################################################################ #
	
  else	

# ############################################################################ #
#	WINDOWS																	   #
#																			   #
	INCLUDE		 += -I"$(OGRE_HOME)/include"
	LIBPATH		 += -L"$(OGRE_HOME)/bin/release"
	LIBRARIES	 += -lOgreMain -lOIS 
#																			   #
# ############################################################################ #

  endif
endif

# combine the stuff
LIB		   = $(LIBRARIES) $(LIBPATH) $(EXTRA_LIBS)
CXXFLAGS  += $(EXTRA_CXXFLAGS) $(WARN_FLAGS) $(INCLUDE) $(EXTRA_INCLUDES)

SRC_FILES = $(wildcard src/*.cpp) $(wildcard src/Debug/*.cpp) $(wildcard src/Shapes/*.cpp) $(wildcard src/Utils/*.cpp)
O_FILES	  = $(SRC_FILES:%.cpp=%.o)


all: libOgreBulletCollisions.a

libOgreBulletCollisions.a: $(O_FILES)
	rm -f $@
	$(AR) rcs $@ $+
	ranlib $@

clean:
	rm -f *.o src/*.o src/Debug/*.o src/Shapes/*.o src/Utils/*.o
	
proper: clean
	rm -rf libOgreBulletCollisions.a
