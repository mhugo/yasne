project( nes )


include_directories( /usr/include/SDL2 )
add_definitions( -ggdb )
add_executable( nes main.cpp cpu.cpp ppu.cpp apu.cpp nes_file_importer.cpp )
target_link_libraries( nes SDL2 readline )
