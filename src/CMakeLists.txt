
add_executable(${This}
    main.cpp
    SmlParser.cpp
)

target_compile_options(${This} PRIVATE -Wall -Wextra -Wpedantic -Werror)