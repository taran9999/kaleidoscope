mkdir -p build
cd build
cmake ..

ln -sf compile_commands.json ../compile_commands.json

make
