# x64-FASM-Programs
Various assembly programs created for modern consumer x64 computers and assembled with FASM.

Program projects within the bootable folder are targeted to run only on x86-64 based computers with UEFI. Some of these programs only support specific processors and/or processor brands.

Program projects within the linux folder are targeted to run only on x86-64 based computers with linux running and potentially other software / hardware descriptions.

Each project and project group may have additional README information.

There may be future support for some of the linux programs to be converted/ported to run on Windows or other operating systems but basic "command-line-like" scripts might not need many changes in order to run.

Small-ish programs will likely be pre-assembled but the assembler bash script(s) can always be used to help reassemble any program in the project folders. This bash script currently targets x86-64 linux and uses the x64 linux version of FASM included in the fasm folder of this repository so changes must be made to run the script with x86 linux, but the sources will still assemble to x86-64 versions. There may be future support for other operating systems to assemble at least the bootable programs. 

Flat assembler (FASM) was created and is maintained by Tomasz Grysztar.
Learn more about FASM here: https://flatassembler.net/
