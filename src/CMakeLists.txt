
add_executable(${This}
    main.cpp
    SmlParser.cpp
    SmlListEntry.cpp
)

add_compile_options(-Os)

target_compile_options(${This} PRIVATE -Wall -Wextra -Wpedantic -Werror)