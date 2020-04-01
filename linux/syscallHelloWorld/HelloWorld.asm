;Linux Executable 64-bit Program with Executable and Linkable "File" Format (ELF) to be assembled with FASM
;Prints / displays the well known "Hello World!" string in a linux terminal using a write system call (syscall)
;Linux system call useage changes based on the system architecture which changes the calling convention and call numbers:
; http://man7.org/linux/man-pages/man2/syscall.2.html 

format ELF64 ; x86-64 architecture version of ELF

;Fundamental x86-64 linux system call numbers extracted from the proper unistd.h/unistd_64.h definition file 
syscall_write = 1
syscall_exit = 60

;Standard output file value extracted from linux LibC Manual: https://www.gnu.org/software/libc/manual/html_mono/libc.html
STDOUT_FILENO = 1

public _start
_start:

;The following syscalls utilize the generic x86-64 calling convention alongside the syscall convention.
; Generic calling convention: https://en.wikipedia.org/wiki/X86_calling_conventions#System_V_AMD64_ABI

;Print the string to the terminal
	mov	eax, syscall_write 
	mov	edi, STDOUT_FILENO
	mov	esi, helloStr
	mov	edx, helloStr_length
	syscall					;An x86-64 instruction to processes the system call

;Exit the program
	mov	eax, syscall_exit	
	xor	edi, edi		;Clear the system call return code to zero
	syscall

helloStr:	;Label to reference the beginning of the string
	db 'Hello World!',10	; String where each character is a byte and 10 represents a new line
helloStr_length = $ - helloStr ; Capture length from current position ($) to string label

;End of Program

