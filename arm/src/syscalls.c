
#include <sys/types.h>

/**
 * @file syscalls.c
 *
 * Implemented with the help from newlib source, FreeRTOS demos,
 * http://www.eetimes.com/discussion/guest-editor/4023922/Embedding-GNU-Newlib-Part-2 and
 * http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/473/t/44452.aspx#154238
 */

/* Linker script defines these */
extern unsigned int _HEAP_START;
extern unsigned int _HEAP_END;

/* Register name faking */
register caddr_t stack_ptr __asm("sp");

/*
 * _sbrk is a function for growing heap space, used by malloc()
 */
caddr_t _sbrk(int increment)
{
  static caddr_t heap = NULL;
  caddr_t prevHeap;
  caddr_t nextHeap;

  /* First allocation */
  if (heap == NULL)
  {
    heap = (caddr_t) &_HEAP_START;
  }

  prevHeap = heap;

  /* Always return data aligned on a 8 byte boundary */
  nextHeap = (caddr_t)(((unsigned int)(heap + increment) + 7) & ~7);

  /* Check enough space and there is no collision with stack coming the other
   * way, if stack is above start of heap */
  if ((((caddr_t)&_HEAP_START < stack_ptr) && (nextHeap > stack_ptr)) ||
      (nextHeap > (caddr_t)&_HEAP_END))
  {
    return NULL;
  }
  else
  {
    heap = nextHeap;
    return (caddr_t) prevHeap;
  }
}
