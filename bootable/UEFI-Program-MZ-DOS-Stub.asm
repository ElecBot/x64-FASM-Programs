;MZ Executable (EXE) DOS Program Stub for use in the beginning of a Portable Executable (PE) UEFI Program
;This assembly program is assembled by FASM
;Total EXE File Size Target: 60-bytes / 30-words so that entire program stub (file) can exisit before PE's 0x3C address
;Program Size Target: 28-bytes / 14-words so that program image fits within total file size target with minimal file header
;Used MZ-DOS EXE format resources to minimize total file / program size including the semi-official:
; http://bytepointer.com/resources/dos_programmers_ref_exe_format.htm

;---------------;
;FASM Directives;
;---------------;
;The following directives dictate how FASM creates the assembled output file with the relevant headers

format MZ				;MZ EXE output format
use16					;Assemble the code restricted to the 16-bit version of x86 assembly code
entry main:start		;Program entrance point definition
stack	256				;Necessary program stack size, assists in stack pointer setup (default 4096)
						;Unused stack can be considered necessary undefined data section
heap	0				;Indicate additional memory that the program could take advantage of (default 65535)
						;A value of 0 indicates that only required memory is needed
						;Both previous values indicate a size in bytes as a multiple of 16 (paragraphs of data)
;----;
;Code;
;----;
segment main			;Main program segment definition
start:					;Starting marker for the program entry point

	;push	cs			;Copies code segment to data segments 
	;pop	ds			;Should not be necessary (commented out) since DOS should set ds = cs right before program start

	mov	ah,09h			;09h sub-function value in register ah tells DOS to display a string when 21h is triggered
	mov	dx,uefiStr		;Set data register to point to the string to print
	int	21h				;Trigger the 21h interrupt, all calls are in accordance with the DOS API

	mov	ax,4C00h		;Properly Quit The DOS Program with interrupt vector 21h
	int	21h				;DOS API Quit

;----------------;
;Initialized Data;
;----------------;
;Hardcoding the message string to be displayed on DOS
;DOS strings end with value 24h
;Remaining size at this point: 16-bytes / 8-words -> 15 Character Message Allowed

uefiStr db 'UEFI Prog Only!',24h

