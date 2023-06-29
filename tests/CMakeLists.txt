file(GLOB_RECURSE files "./*.cc")

set(i 0)

foreach(filename ${files})
    get_filename_component(no_extension ${filename} NAME_WE)

    set(exec_name ${no_extension}_${i})

    add_executable(${exec_name} ${filename})

    target_link_libraries(${exec_name} libwreq)

    add_test(NAME ${exec_name} COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${exec_name}")

    math(EXPR i "${i} + 1")
endforeach()
