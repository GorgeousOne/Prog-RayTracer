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
        ../framework/shape.hpp ../framework/shape.cpp
        ../framework/sphere.hpp ../framework/sphere.cpp
        ../framework/box.hpp ../framework/box.cpp
        ../framework/ray.hpp
        ../framework/hitpoint.hpp
        ../framework/material.hpp ../framework/material.cpp
        ../framework/scene.hpp ../framework/scene.cpp
        ../framework/ambientLight.hpp
        ../framework/pointLight.hpp
        ../framework/camera.hpp
        )
target_link_libraries(tests
        ${GLFW_LIBRARIES}
        )

