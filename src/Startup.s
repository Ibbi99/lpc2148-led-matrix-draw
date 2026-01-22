; Startup for LPC214x (Keil template), stacks adjusted for safety

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80
F_Bit           EQU     0x40

; #stack sizes (bytes) - safer than the tiny defaults
UND_Stack_Size  EQU     0x00000040
SVC_Stack_Size  EQU     0x00000100
ABT_Stack_Size  EQU     0x00000040
FIQ_Stack_Size  EQU     0x00000040
IRQ_Stack_Size  EQU     0x00000080
USR_Stack_Size  EQU     0x00000400

ISR_Stack_Size  EQU     (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                         FIQ_Stack_Size + IRQ_Stack_Size)

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   USR_Stack_Size
__initial_sp    SPACE   ISR_Stack_Size
Stack_Top

Heap_Size       EQU     0x00000000
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

VPBDIV          EQU     0xE01FC100
VPBDIV_SETUP    EQU     1
VPBDIV_Val      EQU     0x00000000

PLL_BASE        EQU     0xE01FC080
PLLCON_OFS      EQU     0x00
PLLCFG_OFS      EQU     0x04
PLLSTAT_OFS     EQU     0x08
PLLFEED_OFS     EQU     0x0C
PLLCON_PLLE     EQU     (1<<0)
PLLCON_PLLC     EQU     (1<<1)
PLLSTAT_PLOCK   EQU     (1<<10)

PLL_SETUP       EQU     0
PLLCFG_Val      EQU     0x00000023

MAM_BASE        EQU     0xE01FC000
MAMCR_OFS       EQU     0x00
MAMTIM_OFS      EQU     0x04

MAM_SETUP       EQU     1
MAMCR_Val       EQU     0x00000002
MAMTIM_Val      EQU     0x00000004

                PRESERVE8

                AREA    RESET, CODE, READONLY
                ARM

Vectors         LDR     PC, Reset_Addr
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP
                LDR     PC, [PC, #-0x0FF0]
                LDR     PC, FIQ_Addr

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SWI_Addr        DCD     SWI_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handler
                DCD     0
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler

Undef_Handler   B       Undef_Handler
SWI_Handler     B       SWI_Handler
PAbt_Handler    B       PAbt_Handler
DAbt_Handler    B       DAbt_Handler
IRQ_Handler     B       IRQ_Handler
FIQ_Handler     B       FIQ_Handler

                EXPORT  Reset_Handler
Reset_Handler

; #VPBDIV setup (left as-is)
                IF      VPBDIV_SETUP <> 0
                LDR     R0, =VPBDIV
                LDR     R1, =VPBDIV_Val
                STR     R1, [R0]
                ENDIF

; #PLL (disabled)
                IF      PLL_SETUP <> 0
                ; (kept from template)
                ENDIF

; #MAM setup
                IF      MAM_SETUP <> 0
                LDR     R0, =MAM_BASE
                MOV     R1, #MAMTIM_Val
                STR     R1, [R0, #MAMTIM_OFS]
                MOV     R1, #MAMCR_Val
                STR     R1, [R0, #MAMCR_OFS]
                ENDIF

; #stack pointers
                LDR     R0, =Stack_Top

                MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #UND_Stack_Size

                MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #ABT_Stack_Size

                MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #FIQ_Stack_Size

                MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

                MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #SVC_Stack_Size

                MSR     CPSR_c, #Mode_USR
                IF      :DEF:__MICROLIB
                EXPORT  __initial_sp
                ELSE
                MOV     SP, R0
                SUB     SL, SP, #USR_Stack_Size
                ENDIF

; #enter C
                IMPORT  __main
                LDR     R0, =__main
                BX      R0

                IF      :DEF:__MICROLIB
                EXPORT  __heap_base
                EXPORT  __heap_limit
                ELSE
                AREA    |.text|, CODE, READONLY
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap
                LDR     R0, =Heap_Mem
                LDR     R1, =(Stack_Mem + USR_Stack_Size)
                LDR     R2, =(Heap_Mem + Heap_Size)
                LDR     R3, =Stack_Mem
                BX      LR
                ENDIF

                END
