
add_executable(${This}
    main.cpp
    SmlLexer.cpp
    SmlParser.cpp
)

add_compile_options(-Os)

target_compile_options(${This} PRIVATE -Wall -Wextra -Wpedantic -Werror)