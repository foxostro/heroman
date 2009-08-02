###############################
#    Makefile for darkness    #
###############################

TARGET = bin/darkness
# SRCS = `ls *.cpp`
SRCS = Actor.cpp \
       ActorFactory.cpp \
       ActorSet.cpp \
       AnimationController.cpp \
       AnimationControllerFactory.cpp \
       AnimationSequence.cpp \
       Application.cpp \
       BlackBoard.cpp \
       BoundingBox.cpp \
       Camera.cpp \
       Component.cpp \
       ComponentModelSetOnPlayerNumber.cpp \
       ComponentPhysics.cpp \
       ComponentRenderAsModel.cpp \
       ComponentRenderAsPhysicsGeom.cpp \
       ComponentMovement.cpp \
       ComponentTriggerAbstract.cpp \
       ComponentTriggerPlaySound.cpp \
       ComponentUserControllable.cpp \
       Dimmer.cpp \
       Effect.cpp \
       File.cpp FileFuncs.cpp \
       FileName.cpp FileText.cpp \
       Fog.cpp Font.cpp \
       FrameTimer.cpp \
       Frustum.cpp \
       GameStateCredits.cpp \
       GameStateMachine.cpp \
       GameStateRun.cpp \
       GraphicsDevice.cpp \
       gui.cpp \
       Kernel.cpp \
       KeyFrame.cpp \
       LabelWidget.cpp \
       linux.cpp \
       logger.cpp \
       Map.cpp \
       mat3.cpp \
       mat4.cpp \
       Material.cpp \
       Menu.cpp \
       MenuMain.cpp \
       Mesh.cpp \
       ModelLoader.cpp \
       ModelLoaderMD2.cpp \
       ModelLoaderMD3.cpp \
       ModelLoaderMulti.cpp \
       ModelLoaderOBJ.cpp \
       ModelLoaderSingle.cpp \
       myassert.cpp \
       PerformanceLabel.cpp \
       PhysicsEngine.cpp \
       PropertyBag.cpp \
       ResourceBuffer.cpp \
       ScreenShot.cpp \
       SDLinput.cpp \
       searchfile.cpp \
       SkyBox.cpp \
       SoundSystem.cpp \
       StackWalker.cpp \
       StateMachine.cpp \
       stdafx.cpp \
       Terrain.cpp \
       TextureFactory.cpp \
       Tile.cpp \
       tstring.cpp \
       Widget.cpp \
       win32.cpp \
       World.cpp
OBJS = $(SRCS:%.cpp=%.o)

###############################
###############################

# Compiler flags
SDL_LIBS = /usr/lib -lSDL
CFLAGS  = `sdl-config --cflags` `ode-config --cflags` -pipe -g
LDFLAGS = `sdl-config --libs`   `ode-config --libs` -ljpeg -lGL -lGLU  -lSDL_image -lSDL_mixer -rdynamic

# Compiler settings
CC = g++
MAKEDEPEND = gcc -M $(CFLAGS) -o $*.d $<

###############################
###############################

.PHONY: all cleanall clean-deps clean depend

all: $(TARGET)

cleanall: clean clean-deps

clean-deps:
	@echo "Cleaning deps..."
	@rm -rf *.d *.d.bak *.P

clean:
	@echo "Cleaning..."
	@rm -f *.o *~ $(TARGET)

$(TARGET): $(OBJS) Makefile
	@echo "Linking..."
	@$(CC) $(LDFLAGS) -o $@ $(OBJS)

# Dependency generation info: <http://make.paulandlesley.org/autodep.html#gcc>
%.o: %.cpp
	@$(CC) $(CFLAGS) -MD -o $@ -c $< && echo $<
	@cp $*.d $*.P; \
             sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
                 -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
             rm -f $*.d

-include $(SRCS:.cpp=.P)


