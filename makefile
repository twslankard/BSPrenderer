#==========================================================================
# Tom's fabulous FLTK / OpenGL makefile!
#==========================================================================
#==========================================================================
# libraries to use
#==========================================================================

LDFLAGS	= -L/usr/X11R6/lib -lX11 -lXi -lXmu -lglut -lGL -lGLU


#==========================================================================
# compiler flags
#==========================================================================
CXX      = g++
CXXFLAGS = -g -O3 -Wall

CC	= $(CXX)
CFLAGS	= $(CXXFLAGS)

#==========================================================================
# source files
#==========================================================================
MAINSRC	= main.cpp
FORMSRC	= bitset.cpp BSP.cpp frustum.cpp image.cpp camera.cpp matrix.cpp PLANE.cpp VECTOR2D.cpp VECTOR3D.cpp VECTOR4D.cpp Timer.cpp md3.cpp node.cpp md3node.cpp transformnode.cpp lightingnode.cpp t3dmodel.cpp cquaternion.cpp lodnode.cpp

#==========================================================================
# file names:
#
# By default, the executable is named after the main source file
# and all of the object files are named after their corresponding .cpp files
#==========================================================================
APPNAME = quakebsp
EXEC	= $(MAINSRC:%.cpp=%)
FORMOBJ	= $(FORMSRC:%.cpp=%.o)

#==========================================================================
# build rules
#==========================================================================
all: $(APPNAME)
$(APPNAME): $(EXEC).o $(FORMOBJ)
	$(CXX) $(LIBPATH) -o $@ $(EXEC).o ${FORMOBJ} $(LDFLAGS)

#==========================================================================
# dependencies
#==========================================================================
include depend.mak

#==========================================================================
# additional rules
#==========================================================================

#depend -- rule to make dependency file
depend:
	-rm -f depend.mak
	g++ -MM $(CXXFLAGS) $(MAINSRC) $(FORMSRC) > depend.mak

#tidy -- removes temp files
tidy:
	-rm -f *~
	-rm -f *.o

#clean -- removes temp files and executable
clean: tidy
	-rm -f $(EXEC)

#remake -- removes executable and temp files and rebuilds the entire project
remake: clean all

#EOF
