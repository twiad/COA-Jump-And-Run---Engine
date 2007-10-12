# user defined values
include ../../Make.conf

# global flags
CXXFLAGS   += -pipe -g
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
	INCLUDE		 += -I"O:\Personal\eclipse\Ogre145\OgreMain\include"
	LIBPATH		 += -L"O:\Personal\eclipse\Ogre145\Samples\Common\bin\Release"
	LIBRARIES	 += -lOgreMain -lOIS
#																			   #
# ############################################################################ #

  endif
endif

# combine the stuff
LIB		   = $(LIBRARIES) $(LIBPATH) $(EXTRA_LIBS)
CXXFLAGS  += $(EXTRA_CXXFLAGS) $(WARN_FLAGS) $(INCLUDE) $(EXTRA_INCLUDES)

SRC_FILES = $(wildcard src/*.cpp)
O_FILES	  = $(SRC_FILES:%.cpp=%.o)


all: libPagedGeometry.a

libPagedGeometry.a: $(O_FILES)
	rm -f $@
	$(AR) rcs $@ $+
	ranlib $@

clean:
	rm -f *.o src/*.o
	
proper: clean
	rm -rf libPagedGeometry.a
