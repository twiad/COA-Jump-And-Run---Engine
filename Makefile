# user defined values
include ./Make.conf

# application name
TARGET = CoAJnR

# global flags
CXXFLAGS        += -pipe
EXTRA_CXXFLAGS   = -DDEBUG
WARN_FLAGS	     = -Wall
INCLUDE	        += -I./include -I./dep/OgreBullet/Collisions/include -I./dep/OgreBullet/Dynamics/include -I./dep/PagedGeometry/include
LIBPATH		     = -L./dep/OgreBullet -L./dep/PagedGeometry
LIBRARIES	     = -lOgreBullet -lPagedGeometry
BINPATH		     =
TESTBINPATH      =
EXTENSION	     =

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
	INCLUDE		 += -I"$(OGRE_HOME)/include"  -I"$(OGRE_HOME)/include/OIS" -I"$(BULLET_HOME)/src" -I"c:/SDL/include/SDL"
	LIBPATH		 += -L"$(OGRE_HOME)/bin/release" -L"$(BULLET_HOME)/lib" -L"c:/SDL/lib"
	LIBRARIES	 += -lOgreMain -lOIS -lbulletdynamics -lbulletcollision -lbulletmath -lmingw32 -lSDLmain -lSDL -mwindows
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

all: libOgreBullet.a libPagedGeometry.a $(TARGET)

$(TARGET): $(O_FILES) libOgreBullet.a libPagedGeometry.a
	$(CXX) $(O_FILES) -o $(BINPATH)/$@$(EXTENSION) $(LIB)

test: $(TEST_O_FILES) libOgreBullet.a
	$(CXX) $(TEST_O_FILES) -o $(TESTBINPATH)/Test$(EXTENSION) $(LIB)

libOgreBullet.a:
	make -C dep/OgreBullet
	
libPagedGeometry.a:
	make -C dep/PagedGeometry

clean:
	rm -f *.o src/*.o 
	
clean-test:
	rm -rf test/*.o

proper: clean clean-test
	make proper -C dep/OgreBullet
	make proper -C dep/PagedGeometry
	rm -f $(BINPATH)/$(TARGET)$(EXTENSION) $(TESTBINPATH)/Test$(EXTENSION)
	rm -rf `find . -name "Ogre.log"`
