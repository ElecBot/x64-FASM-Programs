#!/bin/bash
# Assemble the specified program in a given project directory

if [ -z "$1" ]
  then
	echo "Assembles assembly files and moves created files into bin folder of the project directory"
	echo "Currently must be run from same directory"
	echo "Currently has commented code for linking found object files together or seperately"
    echo "Program Useage: assemble.sh <project-directory> [FASM-Debug] [UEFI-BootFile]"
	echo "project-directory: manditory path with assembly files"
	echo "FASM-Debug: 0 = No Debug (Default), 1 / Other = Output FASM Debug as .lst file"
	echo "UEFI-BootFile: Put Useable bootx64.efi in root directory, 0 = Don't Output, 1 / Other = Output "
	exit 1
fi

PROJECT_DIRECTORY="${1%/}"
mkdir -p ./$PROJECT_DIRECTORY/bin

echo "Working In Directory: $PROJECT_DIRECTORY"

AssemblyArray=()
mapfile -d $'\0' AssemblyArray < <(find ./$PROJECT_DIRECTORY -maxdepth 1 -name "*.asm" -print0)
for i in "${AssemblyArray[@]}"
do
	file=$(basename -- $i)
	filename="${file%.*}"
	echo "Processing File: $filename"
	if [[ ! -z "$2" && "$2" -ne "0" ]]; then
		./fasm/fasm.x64 "./$PROJECT_DIRECTORY/$filename.asm" -s "./$PROJECT_DIRECTORY/$filename.fas"
		./fasm/tools/libc/bin/listing "./$PROJECT_DIRECTORY/$filename.fas" "./$PROJECT_DIRECTORY/bin/$filename.lst"
		mv "./$PROJECT_DIRECTORY/$filename.fas" "./$PROJECT_DIRECTORY/bin/$filename.fas"
	else
		./fasm/fasm.x64 "./$PROJECT_DIRECTORY/$filename.asm"
	fi
	if [ -f "./$PROJECT_DIRECTORY/$filename" ]; then
		chmod +x "./$PROJECT_DIRECTORY/$filename"
    	mv "./$PROJECT_DIRECTORY/$filename" "./$PROJECT_DIRECTORY/bin/$filename"
	fi
	if [ -f "./$PROJECT_DIRECTORY/$filename.efi" ]; then
		chmod +x "./$PROJECT_DIRECTORY/$filename.efi"
    	mv "./$PROJECT_DIRECTORY/$filename.efi" "./$PROJECT_DIRECTORY/bin/$filename.efi"
		if [[ ! -z "$3" && "$3" -ne "0" ]]; then
			cp "./$PROJECT_DIRECTORY/bin/$filename.efi" "./bootx64.efi"
		fi
	fi
done

ObjectArray=()
mapfile -d $'\0' ObjectArray < <(find ./$PROJECT_DIRECTORY -maxdepth 1 -name "*.o" -print0)
#echo "${#ObjectArray[@]}"
if [ ${#ObjectArray[@]} -gt 0 ]; then
	echo "${ObjectArray[0]}"
	filename=$(basename -- ${ObjectArray[0]})
	ExtensionRemoved="${filename%.*}"
	#ld -o ./$PROJECT_DIRECTORY/bin/$ExtensionRemoved $ObjectArray
	for i in "${ObjectArray[@]}"
	do
		filename=$(basename -- $i)
		#gcc -m32 -o ./$PROJECT_DIRECTORY/bin/${filename%.*} $i
		mv $i ./$PROJECT_DIRECTORY/bin/$filename
	done
fi



exit 0
