# get main configuration from inc file
include ../../Makefile.inc

# I'm using multiple context for glew
CXXFLAGS+=-DGLEW_MX -DGLEW_STATIC -fpermissive -DXGE_EXPORTS -I./ -I../ -I../../libs -I../../libs/FreeImage -I../../libs/glew 

# all source code in this directory
SRC_CPP=$(shell ls *.cpp)
OBJS=$(SRC_CPP:.cpp=.o) juce_amalgamated.o


# ////////////////////////////////////////////////
# LINUX  configuration
ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
CXXFLAGS+=-I "/usr/include" -I "/usr/include/freetype2"
LIB_NAME=libxge.so
LDFLAGS+=-shared  -lz -lfreeimage -lGL -lGLU -lfreetype -lrt -lX11 -lGL -lGLU -lXinerama -lasound
endif

# ////////////////////////////////////////////////
# DARWIN configuration
ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
LIB_NAME=libxge.dylib
LDFLAGS+=-w -dynamiclib -lz ../../libs/FreeImage/libFreeImage.a -framework Carbon -framework AGL -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreMIDI -framework WebKit -framework DiscRecording -framework QuartzCore -framework QTKit -framework QuickTime
endif


all: ${LIB_NAME}
	@echo "Library done"

${LIB_NAME}: $(OBJS) 
	g++ $(LDFLAGS) -o $@ ${OBJS} ${LIBS} ../../libs/tinyxml/libtinyxml.a ../../libs/qhull/libqhull.a ../../libs/glew/libglew.a  


ifeq ($(PLATFORM_NAME),PLATFORM_Linux)
juce_amalgamated.o: 
	$(CXX) -c $(CXXFLAGS) -o $@ ../../libs/juce/juce_amalgamated.cpp
endif

ifeq ($(PLATFORM_NAME),PLATFORM_Darwin)
juce_amalgamated.o:
	$(CXX) -c $(CXXFLAGS) -o $@ ../../libs/juce/juce_amalgamated.mm
endif

depend:
	makedepend -I../ -Y. *.cpp *.c *.h


clean:
	rm -Rf Debug/ Release/ *.o *.a *.user *.dylib *.so


# DO NOT DELETE

DllMain.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
DllMain.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
DllMain.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
DllMain.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
DllMain.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
DllMain.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
DllMain.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
DllMain.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
DllMain.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
DllMain.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
DllMain.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
GL.engine.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
GL.engine.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
GL.engine.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
GL.engine.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
GL.engine.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
GL.engine.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
GL.engine.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
GL.engine.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
GL.engine.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
GL.engine.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
GL.engine.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
GL.engine.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
GL.engine.fbo.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
GL.engine.fbo.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
GL.engine.fbo.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
GL.engine.fbo.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
GL.engine.fbo.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
GL.engine.fbo.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
GL.engine.fbo.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
GL.engine.fbo.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
GL.engine.fbo.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
GL.engine.fbo.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
GL.engine.fbo.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
GL.engine.fbo.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
GL.engine.shader.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
GL.engine.shader.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
GL.engine.shader.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
GL.engine.shader.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
GL.engine.shader.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
GL.engine.shader.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h
GL.engine.shader.o: ../xge/ball.h ../xge/box.h ../xge/ray.h
GL.engine.shader.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
GL.engine.shader.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
GL.engine.shader.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h
GL.engine.shader.o: ../xge/graph.h ../xge/graph.hxx ../xge/engine.h
GL.engine.shader.o: ../xge/viewer.h ../xge/octree.h ../xge/unwrapper.h
GL.engine.shader.o: ../xge/bake.h ../xge/plasm.h
GL.engine.tessellator.o: ../xge/xge.h ../xge/config.h ../xge/utils.h
GL.engine.tessellator.o: ../xge/log.h ../xge/clock.h ../xge/keyboard.h
GL.engine.tessellator.o: ../xge/spinlock.h ../xge/thread.h
GL.engine.tessellator.o: ../xge/spinlock.hxx ../xge/mempool.h
GL.engine.tessellator.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
GL.engine.tessellator.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
GL.engine.tessellator.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h
GL.engine.tessellator.o: ../xge/ball.h ../xge/box.h ../xge/ray.h
GL.engine.tessellator.o: ../xge/quaternion.h ../xge/localsystem.h
GL.engine.tessellator.o: ../xge/vector.h ../xge/color4f.h ../xge/frustum.h
GL.engine.tessellator.o: ../xge/texture.h ../xge/manipulator.h ../xge/batch.h
GL.engine.tessellator.o: ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
GL.engine.tessellator.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
GL.engine.tessellator.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
archive.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
archive.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
archive.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
archive.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
archive.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
archive.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
archive.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
archive.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
archive.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
archive.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
archive.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
bake.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
bake.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
bake.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
bake.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
bake.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
bake.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
bake.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
bake.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
bake.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
bake.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
bake.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
ball.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
ball.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
ball.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
ball.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
ball.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
ball.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
ball.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
ball.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
ball.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
ball.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
ball.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
batch.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
batch.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
batch.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
batch.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
batch.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
batch.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
batch.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
batch.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
batch.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
batch.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
batch.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
batch.obj.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
batch.obj.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
batch.obj.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
batch.obj.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
batch.obj.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
batch.obj.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
batch.obj.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
batch.obj.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
batch.obj.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
batch.obj.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
batch.obj.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
batch.obj.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
batch.ply.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
batch.ply.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
batch.ply.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
batch.ply.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
batch.ply.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
batch.ply.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
batch.ply.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
batch.ply.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
batch.ply.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
batch.ply.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
batch.ply.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
batch.ply.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
batch.ptx.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
batch.ptx.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
batch.ptx.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
batch.ptx.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
batch.ptx.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
batch.ptx.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
batch.ptx.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
batch.ptx.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
batch.ptx.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
batch.ptx.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
batch.ptx.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
batch.ptx.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
box.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
box.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
box.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
box.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
box.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
box.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
box.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
box.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
box.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
box.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
box.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
encoder.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
encoder.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
encoder.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
encoder.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
encoder.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
encoder.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
encoder.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
encoder.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
encoder.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
encoder.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
encoder.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
filesystem.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
filesystem.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
filesystem.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
filesystem.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
filesystem.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
filesystem.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
filesystem.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
filesystem.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
filesystem.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
filesystem.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
filesystem.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
filesystem.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
frustum.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
frustum.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
frustum.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
frustum.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
frustum.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
frustum.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
frustum.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
frustum.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
frustum.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
frustum.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
frustum.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
graph.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
graph.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
graph.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
graph.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
graph.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
graph.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
graph.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
graph.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
graph.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
graph.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
graph.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
graph.import.svg.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
graph.import.svg.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
graph.import.svg.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
graph.import.svg.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
graph.import.svg.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
graph.import.svg.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h
graph.import.svg.o: ../xge/ball.h ../xge/box.h ../xge/ray.h
graph.import.svg.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
graph.import.svg.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
graph.import.svg.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h
graph.import.svg.o: ../xge/graph.h ../xge/graph.hxx ../xge/engine.h
graph.import.svg.o: ../xge/viewer.h ../xge/octree.h ../xge/unwrapper.h
graph.import.svg.o: ../xge/bake.h ../xge/plasm.h
keyboard.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
keyboard.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
keyboard.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
keyboard.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
keyboard.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
keyboard.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
keyboard.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
keyboard.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
keyboard.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
keyboard.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
keyboard.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
keyboard.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
log.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
log.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
log.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
log.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
log.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
log.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
log.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
log.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
log.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
log.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
log.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
manipulator.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
manipulator.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
manipulator.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
manipulator.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
manipulator.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
manipulator.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
manipulator.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
manipulator.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
manipulator.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
manipulator.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
manipulator.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
manipulator.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
mat.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
mat.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
mat.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
mat.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
mat.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
mat.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
mat.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
mat.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
mat.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
mat.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
mat.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
mempool.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
mempool.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
mempool.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
mempool.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
mempool.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
mempool.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
mempool.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
mempool.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
mempool.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
mempool.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
mempool.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
octree.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
octree.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
octree.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
octree.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
octree.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
octree.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
octree.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
octree.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
octree.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
octree.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
octree.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
plane.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
plane.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
plane.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
plane.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
plane.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
plane.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
plane.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
plane.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
plane.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
plane.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
plane.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
plasm.boolop.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
plasm.boolop.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
plasm.boolop.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
plasm.boolop.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
plasm.boolop.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
plasm.boolop.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
plasm.boolop.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
plasm.boolop.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
plasm.boolop.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
plasm.boolop.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
plasm.boolop.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
plasm.boolop.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
plasm.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
plasm.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
plasm.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
plasm.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
plasm.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
plasm.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
plasm.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
plasm.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
plasm.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
plasm.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
plasm.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
plasm.xml.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
plasm.xml.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
plasm.xml.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
plasm.xml.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
plasm.xml.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
plasm.xml.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
plasm.xml.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
plasm.xml.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
plasm.xml.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
plasm.xml.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
plasm.xml.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
plasm.xml.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
quaternion.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
quaternion.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
quaternion.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
quaternion.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
quaternion.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
quaternion.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
quaternion.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
quaternion.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
quaternion.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
quaternion.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
quaternion.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
quaternion.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
ray.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
ray.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
ray.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
ray.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
ray.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
ray.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
ray.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
ray.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
ray.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
ray.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
ray.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
stdafx.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
stdafx.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
stdafx.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
stdafx.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
stdafx.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
stdafx.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
stdafx.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
stdafx.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
stdafx.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
stdafx.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
stdafx.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
texture.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
texture.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
texture.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
texture.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
texture.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
texture.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
texture.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
texture.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
texture.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
texture.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
texture.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
thread.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
thread.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
thread.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
thread.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
thread.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
thread.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
thread.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
thread.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
thread.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
thread.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
thread.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
unwrapper.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
unwrapper.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
unwrapper.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
unwrapper.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
unwrapper.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
unwrapper.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
unwrapper.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
unwrapper.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
unwrapper.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
unwrapper.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
unwrapper.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
unwrapper.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
utils.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
utils.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
utils.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
utils.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
utils.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
utils.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
utils.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
utils.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
utils.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
utils.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
utils.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
vec.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
vec.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
vec.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
vec.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
vec.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
vec.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
vec.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
vec.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
vec.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
vec.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
vec.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
vector.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
vector.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
vector.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
vector.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
vector.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
vector.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
vector.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
vector.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
vector.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
vector.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
vector.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
viewer.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
viewer.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
viewer.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
viewer.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
viewer.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
viewer.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
viewer.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
viewer.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
viewer.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
viewer.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
viewer.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
archive.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
archive.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
archive.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
archive.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
archive.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
archive.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
archive.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
archive.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
archive.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
archive.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
archive.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
bake.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
bake.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
bake.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
bake.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
bake.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
bake.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
bake.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
bake.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
bake.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
bake.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
bake.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
ball.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
ball.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
ball.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
ball.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
ball.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
ball.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
ball.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
ball.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
ball.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
ball.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
ball.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
batch.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
batch.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
batch.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
batch.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
batch.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
batch.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
batch.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
batch.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
batch.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
batch.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
batch.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
box.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
box.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
box.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
box.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
box.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
box.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
box.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
box.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
box.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
box.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
box.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
clock.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
clock.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
clock.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
clock.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
clock.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
clock.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
clock.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
clock.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
clock.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
clock.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
clock.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
color4f.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
color4f.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
color4f.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
color4f.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
color4f.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
color4f.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
color4f.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
color4f.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
color4f.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
color4f.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
color4f.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
encoder.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
encoder.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
encoder.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
encoder.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
encoder.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
encoder.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
encoder.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
encoder.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
encoder.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
encoder.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
encoder.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
engine.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
engine.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
engine.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
engine.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
engine.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
engine.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
engine.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
engine.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
engine.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
engine.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
engine.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
filesystem.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
filesystem.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
filesystem.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
filesystem.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
filesystem.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
filesystem.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
filesystem.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
filesystem.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
filesystem.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
filesystem.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
filesystem.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
filesystem.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
frustum.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
frustum.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
frustum.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
frustum.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
frustum.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
frustum.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
frustum.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
frustum.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
frustum.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
frustum.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
frustum.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
graph.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
graph.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
graph.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
graph.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
graph.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
graph.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
graph.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
graph.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
graph.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
graph.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
graph.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
keyboard.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
keyboard.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
keyboard.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
keyboard.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
keyboard.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
keyboard.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
keyboard.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
keyboard.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
keyboard.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
keyboard.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
keyboard.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
keyboard.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
localsystem.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
localsystem.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
localsystem.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
localsystem.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
localsystem.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
localsystem.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
localsystem.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
localsystem.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
localsystem.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
localsystem.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
localsystem.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
localsystem.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
log.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
log.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
log.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
log.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
log.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
log.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
log.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
log.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
log.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
log.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
log.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
manipulator.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
manipulator.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
manipulator.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
manipulator.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
manipulator.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
manipulator.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
manipulator.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
manipulator.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
manipulator.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
manipulator.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
manipulator.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
manipulator.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
mat.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
mat.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
mat.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
mat.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
mat.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
mat.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
mat.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
mat.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
mat.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
mat.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
mat.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
mempool.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
mempool.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
mempool.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
mempool.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
mempool.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
mempool.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
mempool.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
mempool.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
mempool.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
mempool.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
mempool.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
mouseevent.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
mouseevent.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
mouseevent.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
mouseevent.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
mouseevent.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
mouseevent.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
mouseevent.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
mouseevent.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
mouseevent.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
mouseevent.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
mouseevent.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
mouseevent.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
octree.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
octree.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
octree.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
octree.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
octree.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
octree.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
octree.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
octree.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
octree.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
octree.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
octree.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
pick.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
pick.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
pick.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
pick.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
pick.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
pick.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
pick.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
pick.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
pick.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
pick.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
pick.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
plane.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
plane.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
plane.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
plane.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
plane.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
plane.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
plane.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
plane.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
plane.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
plane.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
plane.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
plasm.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
plasm.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
plasm.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
plasm.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
plasm.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
plasm.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
plasm.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
plasm.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
plasm.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
plasm.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
plasm.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
quaternion.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
quaternion.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
quaternion.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
quaternion.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
quaternion.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
quaternion.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
quaternion.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
quaternion.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
quaternion.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
quaternion.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
quaternion.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
quaternion.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
ray.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
ray.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
ray.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
ray.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
ray.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
ray.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
ray.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
ray.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
ray.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
ray.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
ray.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
spinlock.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
spinlock.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
spinlock.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
spinlock.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
spinlock.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
spinlock.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
spinlock.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
spinlock.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
spinlock.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
spinlock.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
spinlock.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
spinlock.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
texture.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
texture.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
texture.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
texture.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
texture.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
texture.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
texture.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
texture.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
texture.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
texture.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
texture.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
thread.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
thread.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
thread.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
thread.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
thread.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
thread.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
thread.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
thread.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
thread.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
thread.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
thread.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
triangle.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
triangle.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
triangle.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
triangle.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
triangle.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
triangle.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
triangle.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
triangle.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
triangle.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
triangle.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
triangle.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
triangle.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
unwrapper.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
unwrapper.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h
unwrapper.o: ../xge/thread.h ../xge/spinlock.hxx ../xge/mempool.h
unwrapper.o: ../xge/mempool.hxx ../xge/archive.h ../xge/encoder.h
unwrapper.o: ../xge/filesystem.h ../xge/mouseevent.h ../xge/vec.h
unwrapper.o: ../xge/triangle.h ../xge/plane.h ../xge/mat.h ../xge/ball.h
unwrapper.o: ../xge/box.h ../xge/ray.h ../xge/quaternion.h
unwrapper.o: ../xge/localsystem.h ../xge/vector.h ../xge/color4f.h
unwrapper.o: ../xge/frustum.h ../xge/texture.h ../xge/manipulator.h
unwrapper.o: ../xge/batch.h ../xge/pick.h ../xge/graph.h ../xge/graph.hxx
unwrapper.o: ../xge/engine.h ../xge/viewer.h ../xge/octree.h
unwrapper.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
utils.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
utils.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
utils.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
utils.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
utils.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
utils.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
utils.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
utils.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
utils.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
utils.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
utils.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
vec.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
vec.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
vec.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
vec.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
vec.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
vec.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
vec.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
vec.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
vec.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
vec.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
vec.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
vector.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
vector.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
vector.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
vector.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
vector.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
vector.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
vector.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
vector.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
vector.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
vector.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
vector.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
viewer.o: ../xge/xge.h ../xge/config.h ../xge/utils.h ../xge/log.h
viewer.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
viewer.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
viewer.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
viewer.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
viewer.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
viewer.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
viewer.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
viewer.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
viewer.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
viewer.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
xge.o: ../xge/config.h ../xge/utils.h ../xge/xge.h ../xge/log.h
xge.o: ../xge/clock.h ../xge/keyboard.h ../xge/spinlock.h ../xge/thread.h
xge.o: ../xge/spinlock.hxx ../xge/mempool.h ../xge/mempool.hxx
xge.o: ../xge/archive.h ../xge/encoder.h ../xge/filesystem.h
xge.o: ../xge/mouseevent.h ../xge/vec.h ../xge/triangle.h ../xge/plane.h
xge.o: ../xge/mat.h ../xge/ball.h ../xge/box.h ../xge/ray.h
xge.o: ../xge/quaternion.h ../xge/localsystem.h ../xge/vector.h
xge.o: ../xge/color4f.h ../xge/frustum.h ../xge/texture.h
xge.o: ../xge/manipulator.h ../xge/batch.h ../xge/pick.h ../xge/graph.h
xge.o: ../xge/graph.hxx ../xge/engine.h ../xge/viewer.h ../xge/octree.h
xge.o: ../xge/unwrapper.h ../xge/bake.h ../xge/plasm.h
