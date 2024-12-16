#---------------------------------------------------------------------------------------------------
# CMake Build Configuration for the Ray Tracing Weekend Series
#
# See README.md for guidance.
#---------------------------------------------------------------------------------------------------

cmake_minimum_required ( VERSION 3.1.0...3.27.0 )

project ( RTWeekend VERSION 1.0.0 )

# Set to C++11
set ( CMAKE_CXX_STANDARD          11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )
set ( CMAKE_CXX_EXTENSIONS        OFF )

# Source

set ( EXTERNAL
  # src/external/stb_image.h
  src/external/stb_image_write.h
)

set ( SOURCE_ONE_WEEKEND
  src/part1/main.cpp
  include/color.h
  include/ray.h
  include/vec4.h
)

# set ( SOURCE_NEXT_WEEK
#   src/part2/main.cc
#   src/part2/aabb.h
#   src/part2/bvh.h
#   src/part2/camera.h
#   src/part2/color.h
#   src/part2/constant_medium.h
#   src/part2/hittable.h
#   src/part2/hittable_list.h
#   src/part2/interval.h
#   src/part2/material.h
#   src/part2/perlin.h
#   src/part2/quad.h
#   src/part2/ray.h
#   src/part2/rtw_stb_image.h
#   src/part2/rtweekend.h
#   src/part2/sphere.h
#   src/part2/texture.h
#   src/part2/vec3.h
# )

# set ( SOURCE_REST_OF_YOUR_LIFE
#   src/part3/main.cc
#   src/part3/aabb.h
#   src/part3/camera.h
#   src/part3/color.h
#   src/part3/constant_medium.h
#   src/part3/hittable.h
#   src/part3/hittable_list.h
#   src/part3/interval.h
#   src/part3/material.h
#   src/part3/onb.h
#   src/part3/pdf.h
#   src/part3/perlin.h
#   src/part3/quad.h
#   src/part3/ray.h
#   src/part3/rtw_stb_image.h
#   src/part3/rtweekend.h
#   src/part3/sphere.h
#   src/part3/texture.h
#   src/part3/vec3.h
# )

include_directories(src)

# Specific compiler flags below. We're not going to add options for all possible compilers, but if
# you're new to CMake (like we are), the following may be a helpful example if you're using a
# different compiler or want to set different compiler options.

message (STATUS "Compiler ID: " ${CMAKE_CXX_COMPILER_ID})
message (STATUS "Release flags: " ${CMAKE_CXX_FLAGS_RELEASE})
message (STATUS "Debug flags: " ${CMAKE_CXX_FLAGS_DEBUG})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # /wd #### - Disable warning
    # /we #### - treat warning as error
    add_compile_options("/W4")      # Enable level-4 warnings
    add_compile_options("/we 4265") # Class has virtual functions, but its non-trivial destructor is not virtual
    add_compile_options("/we 5204") # Class has virtual functions, but its trivial destructor is not virtual
    add_compile_options("/wd 4100") # unreferenced formal parameter
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wnon-virtual-dtor) # Class has virtual functions, but its destructor is not virtual
    add_compile_options(-Wreorder) # Data member will be initialized after [other] data member
    add_compile_options(-Wmaybe-uninitialized) # Variable improperly initialized
    add_compile_options(-Wunused-variable) # Variable is defined but unused
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wnon-virtual-dtor) # Class has virtual functions, but its destructor is not virtual
    add_compile_options(-Wreorder) # Data member will be initialized after [other] data member
    add_compile_options(-Wsometimes-uninitialized) # Variable improperly initialized
    add_compile_options(-Wunused-variable) # Variable is defined but unused
endif()

# Executables
include_directories(include)

add_executable(inOneWeekend
  ${EXTERNAL}
  ${SOURCE_ONE_WEEKEND}
)

# add_executable(theNextWeek       ${EXTERNAL} ${SOURCE_NEXT_WEEK})
# add_executable(theRestOfYourLife ${EXTERNAL} ${SOURCE_REST_OF_YOUR_LIFE})
# add_executable(cos_cubed         src/part3/cos_cubed.cc         )
# add_executable(cos_density       src/part3/cos_density.cc       )
# add_executable(integrate_x_sq    src/part3/integrate_x_sq.cc    )
# add_executable(pi                src/part3/pi.cc                )
# add_executable(estimate_halfway  src/part3/estimate_halfway.cc  )
# add_executable(sphere_importance src/part3/sphere_importance.cc )
# add_executable(sphere_plot       src/part3/sphere_plot.cc       )