project( nes )


include_directories( /usr/local/include/SDL2 )
add_definitions( -ggdb )
add_executable( nes main.cpp cpu.cpp ppu.cpp nes_file_importer.cpp )
target_link_libraries( nes SDL2 readline )
