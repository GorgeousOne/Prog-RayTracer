set(CMAKE_CXX_STANDARD 17)

add_executable(example
	example.cpp
        ../framework/shape.hpp ../framework/shape.cpp
        ../framework/sphere.hpp ../framework/sphere.cpp
        ../framework/box.hpp ../framework/box.cpp
		../framework/triangle.hpp ../framework/triangle.cpp
		../framework/composite.hpp ../framework/composite.cpp

        ../framework/ray.hpp ../framework/ray.cpp
        ../framework/hitPoint.hpp
        ../framework/material.hpp ../framework/material.cpp
        ../framework/scene.hpp ../framework/scene.cpp
        ../framework/light.hpp
        ../framework/pointLight.hpp
        ../framework/camera.hpp
        ../framework/printVec3.hpp ../framework/printVec3.cpp

        ../framework/pixel.hpp ../framework/pixel.cpp
        ../framework/pixel.hpp ../framework/ppmwriter.cpp
		../framework/renderer.hpp ../framework/renderer.cpp
	)

target_link_libraries(example ${FRAMEWORK_NAME} ${LIBRARIES})
add_dependencies(example glfw ${FRAMEWORK_NAME} ${COPY_BINARY})

install(TARGETS example DESTINATION .)

# Build tests
#add_executable(tests
#       tests.cpp
#       ../framework/shape.hpp ../framework/shape.cpp
#       ../framework/sphere.hpp ../framework/sphere.cpp
#       ../framework/box.hpp ../framework/box.cpp
#	../framework/triangle.hpp ../framework/triangle.cpp
#	../framework/composite.hpp ../framework/composite.cpp
#
#		../framework/ray.hpp
#       ../framework/hitPoint.hpp
#       ../framework/material.hpp ../framework/material.cpp
#       ../framework/scene.hpp ../framework/scene.cpp
#       ../framework/light.hpp
#       ../framework/pointLight.hpp
#       ../framework/camera.hpp
#       ../framework/printVec3.hpp ../framework/printVec3.cpp
#
#       ../framework/pixel.hpp ../framework/pixel.cpp
#       ../framework/pixel.hpp ../framework/ppmwriter.cpp
#       ../framework/renderer.hpp ../framework/renderer.cpp
#       )
#target_link_libraries(tests
#       ${GLFW_LIBRARIES}
#       )


# Moviemaker
add_executable(moviemaker
		../moviemaker/moviemaker.cpp
        ../moviemaker/bodypart.h ../moviemaker/bodypart.cpp
		../moviemaker/animation.h ../moviemaker/animation.cpp

		../framework/shape.hpp ../framework/shape.cpp
		../framework/sphere.hpp ../framework/sphere.cpp
		../framework/box.hpp ../framework/box.cpp
		../framework/triangle.hpp ../framework/triangle.cpp
		../framework/composite.hpp ../framework/composite.cpp

		../framework/ray.hpp ../framework/ray.cpp
		../framework/hitPoint.hpp
		../framework/material.hpp ../framework/material.cpp
		../framework/scene.hpp ../framework/scene.cpp
		../framework/light.hpp
		../framework/pointLight.hpp
		../framework/camera.hpp
		../framework/printVec3.hpp ../framework/printVec3.cpp

		../framework/pixel.hpp ../framework/pixel.cpp
		../framework/pixel.hpp ../framework/ppmwriter.cpp
		../framework/renderer.hpp ../framework/renderer.cpp
		)

target_link_libraries(moviemaker ${FRAMEWORK_NAME} ${LIBRARIES})
add_dependencies(moviemaker glfw ${FRAMEWORK_NAME} ${COPY_BINARY})

install(TARGETS moviemaker DESTINATION .)