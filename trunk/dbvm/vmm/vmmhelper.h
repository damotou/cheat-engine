#ifndef VMMHELPER_H_
#define VMMHELPER_H_

#include "common.h"
#include "vmreadwrite.h"
#include "vmxcontrolstructures.h"

extern volatile unsigned char *MSRBitmap;

unsigned long long vmmstart;
unsigned long long nextstack;
PPDPTE_PAE pagedirlvl4;
PPDPTE_PAE pagedirptrvirtual;
PPDE2MB_PAE pagedirvirtual; //wrong name, should be pagedir
PPDE2MB_PAE pagedirvirtual2;
PPDE2MB_PAE pagedirvirtual3;
PPDE2MB_PAE pagedirvirtual4;


int debugmode,breakpointset;
int globals_have_been_configured;


//global int 1 redirection vars
int int1redirection;
int int1redirection_idtbypass;
UINT64 int1redirection_idtbypass_cs;
UINT64 int1redirection_idtbypass_rip;

int int3redirection;
int int3redirection_idtbypass;
UINT64 int3redirection_idtbypass_cs;
UINT64 int3redirection_idtbypass_rip;

int int14redirection;
int int14redirection_idtbypass;
UINT64 int14redirection_idtbypass_cs;
UINT64 int14redirection_idtbypass_rip;


typedef struct _vmregisters /* DO NOT CHANGE THIS ORDER */
{
  UINT64         r15;
  UINT64         r14;
  UINT64         r13;
  UINT64         r12;
  UINT64         r11;
  UINT64         r10;
  UINT64         r9;
  UINT64         r8;
  UINT64         rbp;
  UINT64         rsi;
  UINT64         rdi;
  UINT64         rdx;
  UINT64         rcx;
  UINT64         rbx;
  UINT64         rax;
} VMRegisters;

#ifdef DEBUG
typedef struct _vmstate
{
  VMRegisters registers;
  UINT64 rsp;
  UINT64 rip;
  UINT64 rflags;
  WORD es;
  WORD cs;
  WORD ss;
  WORD ds;
  WORD fs;
  WORD gs;
  WORD ldtr;
  WORD tr;

  UINT64 es_base;
  UINT64 cs_base;
  UINT64 ss_base;
  UINT64 ds_base;
  UINT64 fs_base;
  UINT64 gs_base;
  UINT64 ldtr_base;
  UINT64 tr_base;

  unsigned long long efer;

  DWORD exit_reason;
  DWORD exit_interruptioninfo;
  DWORD exit_interruptionerror;
  DWORD idtvector_information;
  DWORD idtvector_error;

  //exit
  WORD exit_cs;
  WORD exit_ss;
  UINT64 exit_cs_base;
  UINT64 exit_ss_base;
  UINT64 exit_rip;
  UINT64 exit_rsp;
  UINT64 exit_rflags;

} VMState, *PVMState;

VMState vmstates[4];
int vmstates_pos;

#endif

typedef volatile struct _cpuinfo
{
  DWORD active;
  DWORD cpunr;
  DWORD apicid;
  DWORD hastoterminate;
  DWORD hasIF;
  UINT64 guestCR0;
  UINT64 guestCR3;
  char  isboot;
  char  command;
  char  vmxsetup;
  char  invalidcs;

  int   int1happened; //set if it has just redirected a interrupt, cleared by vmcall query command
  int   int3happened; //'  '  '
  int   int14happened; //'  '  '

  /* the TS elements are used for transition from protected mode to real and back, since that changes the TS temporarily */
  WORD  TSsegment;
  UINT64 TSbase;
  DWORD TSlimit;
  DWORD TSaccessRights;



  unsigned long long sysenter_CS; //what the guest sees
  unsigned long long sysenter_EIP;
  unsigned long long sysenter_ESP;

  int   hidden_sysenter_modification;
  unsigned long long actual_sysenter_CS; //what is actually is
  unsigned long long actual_sysenter_EIP;
  unsigned long long actual_sysenter_ESP;
  unsigned long long efer;


  void* vmxon_region;
  void* vmcs_region;

  UINT64 virtualTLB_PA;
  void  *virtualTLB;
  void  *virtualTLB_FreeSpot;
  DWORD virtualTLB_Max; //max size of the virtual TLB, when hit, flush...
  UINT64 *virtualTLB_Lookup; //used to lookup the original physical address of a entry
  int   virtualTLB_whiped;

  unsigned char *virtualTLB_guest_PDPTR_lookup;  //memory to keep the original guest's pagedirptr addresses in. (so changes can be detected)
  unsigned char *virtualTLB_guest_PD_lookup; //memory to keep the original guests's pagedir(s) addresses in. (so changes can be detected)

  UINT64 AvailableVirtualAddress;  //used for mapping physical memory, last 4 bits define the cpunr

  UINT64 Previous_Interuptability_State; //used for the block/unblock interrupts vmcall
  int    Previous_CLI;

  struct {
    UINT64 IDTBase;
    WORD IDTLimit;
    UINT64 GDTBase;
    WORD GDTLimit;
  } RealMode;

  struct {
    int cr3_change_callback; //=1 when a callback has been registered
    int called_callback;

    int calling_convention;
    WORD callback_cs;
    WORD callback_ss;
    UINT64 callback_rip;
    UINT64 callback_rsp;

    UINT64 rip;

    //saved state before the callback as made
    UINT64 rax;
    UINT64 rbx;
    UINT64 rcx;
    UINT64 rdx;
    UINT64 rsi;
    UINT64 rdi;
    UINT64 rbp;
    UINT64 rsp;
    UINT64 r8;
    UINT64 r9;
    UINT64 r10;
    UINT64 r11;
    UINT64 r12;
    UINT64 r13;
    UINT64 r14;
    UINT64 r15;

    UINT64 rflags;
    UINT64 interruptability_state;

    ULONG cs_selector;
    ULONG cs_limit;
    ULONG cs_base;
    ULONG cs_accessrights;

    ULONG ss_selector;
    ULONG ss_limit;
    ULONG ss_base;
    ULONG ss_accessrights;

    ULONG ds_selector;
    ULONG ds_limit;
    ULONG ds_base;
    ULONG ds_accessrights;

    ULONG es_selector;
    ULONG es_limit;
    ULONG es_base;
    ULONG es_accessrights;

    ULONG fs_selector;
    ULONG fs_limit;
    ULONG fs_base;
    ULONG fs_accessrights;

    ULONG gs_selector;
    ULONG gs_limit;
    ULONG gs_base;
    ULONG gs_accessrights;

    UINT64 newcr3;
    int changedcr3;
  } cr3_callback;

  struct
  {
    int Active; //set to 1 when active
    QWORD CR3; //Holds the CR3 value to watch taskswitch to and from
    QWORD DEBUGCTL; //Holds the DebugCTL value to set when inside the target process
    QWORD DS_AREA; //Holds the DS_AREA to set when
    QWORD OriginalDebugCTL; //When inside the target process this holds the debugctl that was set before entering. Return this on readMSR (and set with writeMSR when inside the process)
    QWORD OriginalDS_AREA; //When inside the target process this holds the DS_AREA that was set before entering. Return this with readMSR ('''')
  } Ultimap;

  struct
  {
      int Active;
      QWORD LastIgnoredPageFault;
  } IgnorePageFaults;


  //Host-only fields:
  //field for exception handling
  struct
  {
      QWORD RIP;  //if set go to this address on return from any interrupt
      QWORD RSP;  //if set go to this address on return from any interrupt
  } OnInterrupt;



  unsigned char LastInterrupt;
  unsigned char LastInterruptHasErrorcode;
  WORD LastInterruptErrorcode;

} tcpuinfo, *pcpuinfo; //allocated when the number of cpu's is known
tcpuinfo cpuinfo[32];




typedef struct
{
  union
  {
    ULONG	Selectorvalue;
	  struct {
      ULONG	RPL      : 2;
      ULONG TI       : 1;
      ULONG	Selector : 29;
		};
	};
} Selector_Field, *PSelector_Field;


typedef struct _regCR0
{
  union{
    unsigned long long CR0;
    struct {
      unsigned PE         :1; //0
      unsigned MP         :1; //1
      unsigned EM         :1; //2
      unsigned TS         :1; //3
      unsigned ET         :1; //4
      unsigned NE         :1; //5
      unsigned reserved1  :10; //6-15
      unsigned WP         :1; //16
      unsigned reserved2  :1; //17
      unsigned AM         :1; //18
      unsigned reserved   :10; //19-28
      unsigned NW         :1; //29
      unsigned CD         :1; //30
      unsigned PG         :1; //31
    };
  };
} __attribute__((__packed__)) regCR0,*PregCR0;

typedef struct _regCR4
{
  union{
    unsigned long long CR4;
    struct {
      unsigned VME        :1;
      unsigned PVI        :1;
      unsigned TSD        :1;
      unsigned DE         :1;
      unsigned PSE        :1;
      unsigned PAE        :1;
      unsigned MCE        :1;
      unsigned PGE        :1;
      unsigned PCE        :1;
      unsigned OSFXSR     :1;
      unsigned OSXMMEXCPT :1;
      unsigned reserved   :2;
      unsigned VMXE       :1;
      UINT64   reserved2  :50;
    };
  };
} __attribute__((__packed__)) regCR4,*PregCR4;

//PAE setting bits(when used without regCR4)
#define CR4_VME         (1<<0)
#define CR4_PVI         (1<<1)
#define CR4_TSD         (1<<2)
#define CR4_DE          (1<<3)
#define CR4_PSE         (1<<4)
#define CR4_PAE         (1<<5)
#define CR4_MCE         (1<<6)
#define CR4_PGE         (1<<7)
#define CR4_PCE         (1<<8)
#define CR4_OSFXSR      (1<<9)
#define CR4_OSXMMEXCPT  (1<<10)
#define CR4_VMXE        (1<<13)

typedef struct _regDR7
{
  union{
    unsigned long long DR7;
    struct {
      unsigned L0        :1; //0
      unsigned G0        :1; //1
      unsigned L1        :1; //2
      unsigned G1        :1; //3
      unsigned L2        :1; //4
      unsigned G2        :1; //5
      unsigned L3        :1; //6
      unsigned G3        :1; //7
      unsigned LE        :1; //8
      unsigned GE        :1; //9
      unsigned reserved  :3; //001  //10-11-12
      unsigned GD        :1; //13...
      unsigned reserved2 :2; //00
      unsigned RW0       :2;
      unsigned LEN0      :2;
      unsigned RW1       :2;
      unsigned LEN1      :2;
      unsigned RW2       :2;
      unsigned LEN2      :2;
      unsigned RW3       :2;
      unsigned LEN3      :2;
      unsigned reserved3 :32;
    };
  };
} __attribute__((__packed__)) regDR7,*PregDR7;


typedef struct _pferrorcode /* errorcode returned for pagefaults */
{
  union{
    unsigned long long errorcode;
    struct {
      unsigned P          :1;  /* 0=non present page, 1=page level protection violation */
      unsigned W          :1;  /* 0=read exception, 1=write exception */
      unsigned US         :1;  /* 0=processor was in supervisor mode, 1=processor was in usermode */
      unsigned RSVD       :1;  /* 0=Not caused by reserved bits set, 1=Caused because reserved bits are set */
      unsigned ID         :1;  /* 0=Not caused by an instruction fetch, 1=Caused by an instruction fetch */
      unsigned reserved   :27;
    };
  };
} __attribute__((__packed__)) PFerrorcode,*PPFerrorcode;


UINT64 IA32_VMX_CR0_FIXED0,IA32_VMX_CR0_FIXED1;
UINT64 IA32_VMX_CR4_FIXED0,IA32_VMX_CR4_FIXED1;
TIA32_VMX_BASIC IA32_VMX_BASIC;
UINT64 IA32_VMX_PINBASED_CTLS;
UINT64 IA32_VMX_PROCBASED_CTLS;
UINT64 IA32_VMX_SECONDARY_PROCBASED_CTLS;
UINT64 IA32_VMX_EXIT_CTLS;
UINT64 IA32_VMX_ENTRY_CTLS;
TIA32_VMX_MISC IA32_VMX_MISC;


void CheckGuest(void);
void displayVMmemory(pcpuinfo currentcpuinfo);
void displayPhysicalMemory();
void setupTSS8086(void);
void setupVMX(pcpuinfo currentcpuinfo);
void launchVMX(pcpuinfo currentcpuinfo);
int vmexit(tcpuinfo *cpu, UINT64 *registers);

void sendvmstate(pcpuinfo currentcpuinfo, VMRegisters *registers);
char *getVMInstructionErrorString(void);

void ShowCurrentInstruction(pcpuinfo currentcpuinfo);
void ShowCurrentInstructions(pcpuinfo currentcpuinfo);
void displayPreviousStates(void);

#define ISREALMODE ((vmread(vm_cr0_fakeread) & 1)==0)

//check if ia32e mode is active for the guest(bit 9)
#define IS64BITPAGING ((vmread(vm_entry_controls) & IA32E_MODE_GUEST) != 0)
#define IS64BITCODE (IS64BITPAGING && ((vmread(vm_guest_cs_access_rights) >> 13) & 1))

#endif /*VMMHELPER_H_*/
