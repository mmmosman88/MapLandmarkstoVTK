
cmake_minimum_required(VERSION 2.8)

PROJECT(MapLandmarkstoVTK.cxx)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(MapLandmarkstoVTK MACOSX_BUNDLE MapLandmarkstoVTK)

if(VTK_LIBRARIES)
  target_link_libraries(MapLandmarkstoVTK ${VTK_LIBRARIES})
else()
  target_link_libraries(MapLandmarkstoVTK vtkHybrid vtkWidgets)
endif()
