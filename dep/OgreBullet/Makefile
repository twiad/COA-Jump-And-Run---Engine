# user defined values
include ../../Make.conf

# global flags
CXXFLAGS   += -pipe
WARN_FLAGS	= -Wall
INCLUDE	   += -I./include -I./Collisions/include -I./Dynamics/include
LIBPATH		=
LIBRARIES	=

ifeq ($(shell uname -s),Darwin)

# ############################################################################ #
#	MAC OS X																   #
#																			   #
# TODO: the INCLUDE value is cheesy and should be replaced					   #
#		with something more general in the future							   #
#																			   #
  INCLUDE		 += -I/Library/Frameworks/Ogre.framework/Headers -I/Library/Frameworks/OIS.framework/Headers -I /usr/local/include/bullet -I/Library/Frameworks/SDL.framework/Headers
  LIBPATH		 += 
  LIBRARIES		 += -framework Ogre -framework OIS -framework Cocoa -framework SDL -lbulletcollision -lbulletdynamics -lbulletmath
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
	INCLUDE		 += -I"$(OGRE_HOME)/include" -I"$(BULLET_HOME)/src" -I"c:/SDL/include/SDL"
	LIBPATH		 += -L"$(OGRE_HOME)/bin/release" -L"$(BULLET_HOME)/lib" -L"c:/SDL/lib"
	LIBRARIES	 += -lOgreMain -lOIS -lbulletcollision -lbulletdynamics -lbulletmath -lOgreBullet -lSDLmain -lSDL
#																			   #
# ############################################################################ #

  endif
endif

# combine the stuff
LIB		   = $(LIBRARIES) $(LIBPATH) $(EXTRA_LIBS)
CXXFLAGS  += $(EXTRA_CXXFLAGS) $(WARN_FLAGS) $(INCLUDE) $(EXTRA_INCLUDES)

SRC_FILES = $(wildcard Collisions/src/*.cpp) $(wildcard Collisions/src/Debug/*.cpp) $(wildcard Collisions/src/Shapes/*.cpp) $(wildcard Collisions/src/Utils/*.cpp) $(wildcard Dynamics/src/*.cpp) $(wildcard Dynamics/src/Constraints/*.cpp) $(wildcard Dynamics/src/Prefabs/*.cpp)
O_FILES	  = $(SRC_FILES:%.cpp=%.o)


all: libOgreBullet.a

libOgreBullet.a: $(O_FILES)
	rm -f $@
	$(AR) rcs $@ $+
	ranlib $@

clean:
	rm -f *.o Collisions/src/*.o Collisions/src/Debug/*.o Collisions/src/Shapes/*.o Collisions/src/Utils/*.o Dynamics/src/*.o Dynamics/src/Constraints/*.o  Dynamics/src/Prefabs/*.o
	
proper: clean
	rm -rf libOgreBullet.a
