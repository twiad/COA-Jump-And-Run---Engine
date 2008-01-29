# application name
TARGET = CoAJnR

# global flags
CXXFLAGS		+= -pipe -ansi -D$(shell whoami)
EXTRA_CXXFLAGS	 = -DDEBUG -g
#EXTRA_CXXFLAGS	 = -O3
WARN_FLAGS		 = -Wall
INCLUDE			+= -Iinclude -Idep/PagedGeometry/include -Idep/rtils/include -Idep/threadpool/include
#LIBPATH			 = -L./dep/PagedGeometry
#LIBRARIES		 = -lPagedGeometry
BINPATH			 =
EXTENSION		 =


ifeq ($(shell uname -s),Darwin)

# ############################################################################ #
#	MAC OS X																   #
#																			   #
  PLATFORMDEF   = -DMAC -D__MAC__																			   #
  INCLUDE		 += -I/usr/local/include/bullet -I/opt/local/include -I/Developer/OgreSDK/Dependencies/include
  LIBPATH		 += -L/Developer/OgreSDK/Dependencies/lib/Release/ -L/opt/local/lib
  LIBRARIES		 += -framework Ogre -framework Cocoa -framework Carbon -lbulletcollision -lbulletdynamics -lbulletmath -lois -lboost_thread-mt
  BINPATH		  = ./bin/CoAJnR.app/Contents/MacOS
#																			   #
# ############################################################################ #

else
  ifeq ($(shell uname -s),Linux)

# ############################################################################ #
#	LINUX																	   #
#																			   #
    PLATFORMDEF   = -DLINUX -D__LINUX__																			   #
	INCLUDE		 += -I/usr/include/OGRE -I/usr/local/include/bullet
	LIBPATH		 += 
	LIBRARIES	 += -lboost_thread -lOgreMain -lOIS -lbulletdynamics -lbulletcollision -lbulletmath
	BINPATH		  = ./bin
#																			   #
# ############################################################################ #
	
  else	

# ############################################################################ #
#	WINDOWS																	   #
#
    PLATFORMDEF   = -DMINGW -D__MINGW__																			   #
	INCLUDE		 += -I"$(BOOST_HOME)" -I"$(OGRE_HOME)/include" -I"$(OGRE_HOME)/include/OIS" -I"$(BULLET_HOME)/src"
	LIBPATH		 += -L"$(BOOST_HOME)/stage/lib" -L"$(OGRE_HOME)/bin/release" -L"$(BULLET_HOME)/lib"
	LIBRARIES	 += -lboost_thread -lOgreMain -lOIS -lbulletdynamics -lbulletcollision -lbulletmath -lWinmm
	BINPATH		  = ./bin
	EXTENSION	  = .exe
#																			   #
# ############################################################################ #

  endif
endif

# combine the stuff
LIB		   = $(LIBPATH) $(LIBRARIES) $(EXTRA_LIBS)
CXXFLAGS  += $(EXTRA_CXXFLAGS) $(WARN_FLAGS) $(INCLUDE) $(EXTRA_INCLUDES) $(PLATFORMDEF)

SRC_FILES = $(wildcard src/*.cpp)
O_FILES	  = $(SRC_FILES:%.cpp=%.o)

TEST_SRC_FILES = $(wildcard test/*.cpp)
TEST_O_FILES = $(TEST_SRC_FILES:%.cpp=%.o)

#all: libOgreBullet.a libPagedGeometry.a $(TARGET)
all: $(TARGET)

#$(TARGET): $(O_FILES) libOgreBullet.a libPagedGeometry.a
$(TARGET): $(O_FILES)
	$(CXX) $(O_FILES) -o $(BINPATH)/$@$(EXTENSION) $(LIB)

#libOgreBullet.a:
#	make -C dep/OgreBullet
	
#libPagedGeometry.a:
#	make -C dep/PagedGeometry

run: $(TARGET)
	cd $(BINPATH)
	DYLD_INSERT_LIBRARIES=/usr/lib/libMallocDebug.A.dylib ./$(BINPATH)/$(TARGET)
	cd -

docs:
	doxygen
	
clean-docs:
	rm -rf doc/api

clean:
	rm -f *.o src/*.o .gdb_history
	
proper: clean clean-docs
#	make proper -C dep/OgreBullet
#	make proper -C dep/PagedGeometry
	rm -f $(BINPATH)/$(TARGET)$(EXTENSION) $(TESTBINPATH)/Test$(EXTENSION)
	rm -rf `find . -name "ogre.log"`
	
