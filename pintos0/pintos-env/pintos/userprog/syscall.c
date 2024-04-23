#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "user/syscall.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // Extract the system call number from the stack
  int syscall_number = *((int *)(f->esp));

  // Dispatch to appropriate handler based on the system call number
  switch (syscall_number) {
    case SYS_EXIT:
      {
        // Extract the exit status from the stack
        int status = *((int *)(f->esp + 4));
        thread_exit_with_status(status);
        break;
      }
    case SYS_PRINTF:
      {
        // Extract the pointer to the string from the stack
        char *format = (char *)(*((int *)(f->esp + 4)));
        
        // Call putbuf to print the string to stdout
        putbuf(format, strlen(format));
        f->eax = strlen(format); // Set the return value to the length of the string
        break;
      }
    default:
      printf ("Unknown system call!\n");
      thread_exit ();
  }
}

// Exit the current thread with a status code
void thread_exit_with_status(int status) {
  struct thread *cur = thread_current ();
  cur->exit_status = status; // Save the exit status
  thread_exit ();
}
