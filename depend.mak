main.o: main.cpp main.h VECTOR3D.h camera.h BSP.h bitset.h frustum.h \
  PLANE.h matrix.h VECTOR4D.h node.h boundingbox.h md3node.h md3.h \
  VECTOR2D.h cquaternion.h t3dmodel.h Timer.h lodnode.h transformnode.h \
  lightingnode.h
bitset.o: bitset.cpp bitset.h
BSP.o: BSP.cpp image.h BSP.h bitset.h frustum.h PLANE.h VECTOR3D.h \
  matrix.h VECTOR4D.h
frustum.o: frustum.cpp frustum.h PLANE.h VECTOR3D.h matrix.h VECTOR4D.h
image.o: image.cpp image.h
camera.o: camera.cpp camera.h VECTOR3D.h
matrix.o: matrix.cpp matrix.h VECTOR4D.h VECTOR3D.h
PLANE.o: PLANE.cpp PLANE.h VECTOR3D.h
VECTOR2D.o: VECTOR2D.cpp VECTOR2D.h
VECTOR3D.o: VECTOR3D.cpp VECTOR3D.h
VECTOR4D.o: VECTOR4D.cpp VECTOR4D.h VECTOR3D.h
Timer.o: Timer.cpp Timer.h
md3.o: md3.cpp image.h md3.h VECTOR2D.h VECTOR3D.h cquaternion.h \
  t3dmodel.h boundingbox.h Timer.h
node.o: node.cpp node.h VECTOR3D.h frustum.h PLANE.h matrix.h VECTOR4D.h \
  boundingbox.h
md3node.o: md3node.cpp md3node.h boundingbox.h VECTOR3D.h node.h \
  frustum.h PLANE.h matrix.h VECTOR4D.h md3.h VECTOR2D.h cquaternion.h \
  t3dmodel.h Timer.h
transformnode.o: transformnode.cpp transformnode.h node.h VECTOR3D.h \
  frustum.h PLANE.h matrix.h VECTOR4D.h boundingbox.h
lightingnode.o: lightingnode.cpp lightingnode.h node.h VECTOR3D.h \
  frustum.h PLANE.h matrix.h VECTOR4D.h boundingbox.h
t3dmodel.o: t3dmodel.cpp t3dmodel.h VECTOR2D.h VECTOR3D.h
cquaternion.o: cquaternion.cpp cquaternion.h
lodnode.o: lodnode.cpp lodnode.h node.h VECTOR3D.h frustum.h PLANE.h \
  matrix.h VECTOR4D.h boundingbox.h
