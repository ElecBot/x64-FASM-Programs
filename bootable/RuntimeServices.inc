;========================;
;Runtime Services Section;
;========================;
;Section of UEFI FASM Library
;Updated April 10th 2020

;-------------------;
;Numerical Constants;
;-------------------;
;EFI Runtime Services
	EFI_RUNTIME_SERVICES_SIGNATURE	=	0x56524553544e5552
	EFI_RUNTIME_SERVICES_REVISION	=	EFI_SPECIFICATION_VERSION

;------------------;
;Symbolic Constants;
;------------------;
	EFI_PHYSICAL_ADDRESS	equ		UINT64
	EFI_VIRTUAL_ADDRESS		equ		UINT64

;----------;
;Structures;
;----------;
struc _EFI_RUNTIME_SERVICES {
	.Hdr					_EFI_TABLE_HEADER
;Time Services
	.GetTime				FUNCTION		;EFI_GET_TIME
	.SetTime				FUNCTION		;EFI_SET_TIME
	.GetWakeupTime			FUNCTION		;EFI_GET_WAKEUP_TIME
	.SetWakeupTime			FUNCTION		;EFI_SET_WAKEUP_TIME
;Virtual Memory Services
	.SetVirtualAddressMap	FUNCTION		;EFI_SET_VIRTUAL_ADDRESS_MAP
	.ConverterPointer		POINTER			;EFI_CONVERT_POINTER (Function or Pointer?)
;Variable Services
	.GetVariable			FUNCTION		;EFI_GET_VARIABLE
	.GetNextVariableName	FUNCTION		;EFI_GET_NEXT_VARIABLE_NAME
	.SetVariable			FUNCTION		;EFI_SET_VARIABLE
;Miscellaneous Services
	.GetNextHighMonotonicCount		FUNCTION	;EFI_GET_NEXT_HIGH_MONO_COUNT
	.ResetSystem					FUNCTION	;EFI_RESET_SYSTEM
;UEFI 2.0 Capsule Services
	.UpdateCapsule					FUNCTION	;EFI_UPDATE_CAPSULE
	.QueryCapsuleCapabilities		FUNCTION	;EFI_QUERY_CAPSULE_CAPABILITIES
;Miscellaneous UEFI 2.0 Service
	.QueryVariableInfo		FUNCTION		;EFI_QUERY_VARIABLE_INFO
}

;----;
;Data;
;----;
;Read/Write Uninitialized Data Used With Macros
section '.bss' data readable writeable
RuntimeServicesPtr:		POINTER		;EFI_RUNTIME_SERVICES

;------;
;Macros;
;------;
VirtualStructure _EFI_RUNTIME_SERVICES, EFI_RUNTIME_SERVICES

macro RuntimeServicesInitialize {
	mov rbx,[SystemTablePtr]
	mov	rbx,[rbx + EFI_SYSTEM_TABLE.RuntimeServicesPtr]
	mov [RuntimeServicesPtr],rbx
}

macro RuntimeServicesFunction function*, [data] { ;Set Function in Register
common
	mov rbx,[RuntimeServicesPtr]
	mov rbx,[rbx + EFI_RUNTIME_SERVICES.#function]
	CallFunction rbx, data
}






