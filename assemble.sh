#!/bin/bash
# Assemble the specified program in a given project directory

if [ -z "$1" ]
  then
    echo "You are missing the project folder directory parameter. Quitting Now!"
		exit 1
fi

PROJECT_DIRECTORY="$1"
mkdir -p ./$PROJECT_DIRECTORY/bin

AssemblyArray=()
mapfile -d $'\0' AssemblyArray < <(find ./$PROJECT_DIRECTORY -maxdepth 1 -name "*.asm" -print0)
for i in "${AssemblyArray[@]}"
do
	echo "$i"
	./fasm/fasm.x64 $i
	ExtensionRemoved="${i%.*}"
	#echo "$ExtensionRemoved"
	if [ -f $ExtensionRemoved ]; then
		chmod +x $ExtensionRemoved
		filename=$(basename -- $ExtensionRemoved)
    mv $ExtensionRemoved ./$PROJECT_DIRECTORY/bin/$filename
	fi
	if [ -f "$ExtensionRemoved.efi" ]; then
		chmod +x "$ExtensionRemoved.efi"
		filename=$(basename -- "$ExtensionRemoved.efi")
    mv "$ExtensionRemoved.efi" ./$PROJECT_DIRECTORY/bin/$filename
	fi
done

ObjectArray=()
mapfile -d $'\0' ObjectArray < <(find ./$PROJECT_DIRECTORY -maxdepth 1 -name "*.o" -print0)
#echo "${#ObjectArray[@]}"
if [ ${#ObjectArray[@]} -gt 0 ]; then
	echo "${ObjectArray[0]}"
	filename=$(basename -- ${ObjectArray[0]})
	ExtensionRemoved="${filename%.*}"
	ld -o ./$PROJECT_DIRECTORY/bin/$ExtensionRemoved $ObjectArray
	for i in "${ObjectArray[@]}"
	do
		filename=$(basename -- $i)
		mv $i ./$PROJECT_DIRECTORY/bin/$filename
	done
fi



exit 0
