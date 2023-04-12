
add_executable(${This}
    main.cpp
    SML_Parser.cpp
)

target_compile_options(${This} PRIVATE -Wall -Wextra -Wpedantic -Werror)