
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <sys/types.h>
#include <sys/reent.h>

/**
 * @file syscalls.c
 *
 * Implemented with the help from newlib source, FreeRTOS demos,
 * http://www.eetimes.com/discussion/guest-editor/4023922/Embedding-GNU-Newlib-Part-2 and
 * http://e2e.ti.com/support/microcontrollers/stellaris_arm_cortex-m3_microcontroller/f/473/t/44452.aspx#154238
 */

/* Linker script defines these */
extern unsigned long _HEAP_START;
extern unsigned long _HEAP_END;

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

  /* Always return data aligned on a 8 byte boundary */
  nextHeap = (caddr_t)(((unsigned long) (heap + increment) + 7) & ~7);

  /*
    Modded to not check the stack pointer, as FreeRTOS uses the stack pointer
    register to point at each individual tasks' stack. Instead, we just check
    that the incremented heap has not crossed _HEAP_END as defined in the linker
    script.
  */
  if (nextHeap > (caddr_t) &_HEAP_END)
  {
    return NULL;
  }
  else
  {
    prevHeap = heap;
    heap = nextHeap;
    return (caddr_t) prevHeap;
  }
}
