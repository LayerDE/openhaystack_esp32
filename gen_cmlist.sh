#!/bin/bash
c_files='*.c'
cpp_files='*.cpp'
cd main
rm CMakeLists.txt
echo -n "idf_component_register(SRCS "> CMakeLists.txt
for c_file in $c_files
do
   echo -n "\"" >> CMakeLists.txt
   echo -n $c_file >> CMakeLists.txt
   echo -n "\"" >> CMakeLists.txt
   echo -n " " >> CMakeLists.txt
done
for cpp_file in $cpp_files
do
   echo -n "\"" >> CMakeLists.txt
   echo -n $cpp_file >> CMakeLists.txt
   echo -n "\"" >> CMakeLists.txt
   echo -n " " >> CMakeLists.txt
done
echo -en "\n\t\t\t\tINCLUDE_DIRS \".\")" >> CMakeLists.txt
