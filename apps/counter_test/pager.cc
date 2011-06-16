#include <l4/message.h>
#include <l4/ipc.h>
#include <l4/thread.h>
#include <l4/sigma0.h>
#include <l4io.h>
#include <bitops.h>

void pager_loop (void)
{
    L4_ThreadId_t tid;
    L4_MsgTag_t tag;
    L4_Msg_t msg;
 
    L4_Word_t page_bits = 
      lsBit(L4_PageSizeMask ((L4_KernelInterfacePage_t*)
			     L4_KernelInterface ()));
   
    for (;;)
    {
	tag = L4_Wait (&tid);

	for (;;)
	{

	    L4_Store (tag, &msg);

	    printf ("Pager got msg from %p (%p, %p, %p)\n",
		    (void *) tid.raw, (void *) tag.raw,
		    (void *) L4_Get (&msg, 0), (void *) L4_Get (&msg, 1));

	    if (L4_UntypedWords (tag) != 2 || L4_TypedWords (tag) != 0 ||
		!L4_IpcSucceeded (tag))
	    {
		printf ("malformed pagefault IPC from %p (tag=%p)\n",
			(void *) tid.raw, (void *) tag.raw);
		return;
	    }

	    L4_Word_t faddr = L4_Get (&msg, 0);
	    /* L4_Word_t fip   = L4_Get (&msg, 1); */

	    /* This is really ugly, we just touch this address to bring 
	       the page into our address space */
	    volatile char* dummy = (char *)faddr;
	    *dummy;

	    /* Send mapitem, note that this is a nop between threads in the 
	       the same address space */
	    L4_Clear (&msg);
	    L4_Append (&msg, L4_MapItem (L4_FpageLog2 (faddr, page_bits) +
					 L4_FullyAccessible, faddr));
	    L4_Load (&msg);
	    tag = L4_ReplyWait (tid, &tid);
	}
    }
}
