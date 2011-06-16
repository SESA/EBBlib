#include <l4/thread.h>
#include <l4/kip.h>
#include <l4io.h>
#include <types.h>
#include <ebb.h>

L4_ThreadId_t vcpus[NUM_VCPUS];

L4_Word_t vcpu_stacks[NUM_VCPUS][1024];

extern void vcpu_func(void);

#define UTCBaddress(x)							\
  ((void*)(((L4_Word_t)L4_MyLocalId().raw + utcbsize * (x)) \
	   & ~(utcbsize - 1)))

void init_vcpus() {
  uval i;
  L4_KernelInterfacePage_t* kip = 
    (L4_KernelInterfacePage_t*)L4_KernelInterface ();
  L4_Word_t utcbsize = L4_UtcbSize(kip);

  for (i = 0; i < NUM_VCPUS; i++) {
    vcpus[i] = L4_GlobalId(L4_ThreadNo(L4_Myself()) + 1 + i, 1);
    if (!L4_ThreadControl(vcpus[i], L4_Myself(), 
			  L4_Myself(), L4_Myself(), UTCBaddress(1+i))) {
      printf("ThreadControl failed\n");
      return;
    }

    L4_Set_UserDefinedHandle(vcpus[i], (L4_Word_t)ebb_table[i+1]);


    printf ("New thread with ip:%lx / sp:%lx\n",
	    (L4_Word_t)vcpu_func,
	    (L4_Word_t)&vcpu_stacks[i][1023]);
    L4_Start(vcpus[i], (L4_Word_t)&vcpu_stacks[i][1023],
	     (L4_Word_t)vcpu_func);
  }

}
