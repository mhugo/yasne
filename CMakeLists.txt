project( nes )


add_definitions( -ggdb )
add_executable( nes main.cpp cpu.cpp nes_file_importer.cpp )
