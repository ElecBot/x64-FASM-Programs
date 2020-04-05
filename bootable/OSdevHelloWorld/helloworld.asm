;x64 UEFI Very Basic Hello World Print Program using FASM

format pe64 dll efi ;FASM Method to Create a Bootable EFI
entry main ;Label name of the Entry Point
 
section '.text' code executable readable ;Declare that this section named text is the executable read-only code

include 'uefi.inc' ;Borrowed UEFI library (File in Same Directory)
 
main: ;Entry Point Label
    ; initialize UEFI library
    InitializeLib ;Library "Function Call"
    jc @f
 
    ; call uefi function to print to screen
    uefi_call_wrapper ConOut, OutputString, ConOut, _hello ;Helper Function for calling a UEFI function
 		
		xor	rax, rax		;Clear the counter register
infloop:						;Infinite Loop Label
		inc rax					;Infinite Loop 64-bit counter
		jmp infloop			;Always jump to the Infinite Loop Label

@@: mov eax, EFI_SUCCESS ;Exit the Program with a Success
    retn
 
section '.data' data readable writeable ;Defines the readable Data Section With the Hello World String
 
_hello                                  du 'Hello World',13,10,0 ;Hello World String
 
section '.reloc' fixups data discardable
