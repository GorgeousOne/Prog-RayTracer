add_executable(example example.cpp)

target_link_libraries(example ${FRAMEWORK_NAME} ${LIBRARIES})
add_dependencies(example glfw ${FRAMEWORK_NAME} ${COPY_BINARY})

install(TARGETS example DESTINATION .)

add_executable(raytracer raytracer.cpp)

target_link_libraries(raytracer ${FRAMEWORK_NAME} ${LIBRARIES}
        glfw
        ${GLFW_LIBRARIES}
        )
add_dependencies(raytracer glfw ${FRAMEWORK_NAME} ${COPY_BINARY})

install(TARGETS raytracer DESTINATION .)

# Build tests
add_executable(tests
        tests.cpp
        ../framework/Shape.hpp ../framework/Shape.cpp
        ../framework/Sphere.hpp ../framework/Sphere.cpp
        ../framework/Box.h ../framework/Box.cpp
        ../framework/Ray.hpp
        ../framework/HitPoint.hpp
        ../framework/Material.hpp ../framework/Material.cpp
        )
target_link_libraries(tests
        ${GLFW_LIBRARIES}
        )

