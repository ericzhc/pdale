/* Philippe Mabilleau ing.
* 05/08/2003
* Modif by: Jean-Denis Hatier
* Modif by: ELR  15/11/2005
*/

/*
* redboot-syscalls.c -- provide system call support for RedBoot
*
* Copyright (c) 1997, 2001, 2002 Red Hat, Inc.
*
* The authors hereby grant permission to use, copy, modify, distribute,
* and license this software and its documentation for any purpose, provided
* that existing copyright notices are retained in all copies and that this
* notice is included verbatim in any distributions. No written agreement,
* license, or royalty fee is required for any of the authorized uses.
* Modifications to this software may be copyrighted by their authors
* and need not follow the licensing terms described here, provided that
* the new terms are clearly indicated on the first page of each file where
* they apply.
*
*/

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <errno.h>

#include "serial.h"

int  _close (int  fd)
{
    return (0);
}

void  _exit (int  stat)
{
}

int  _stat (const char  *filename, struct stat  *st)
{
    errno = EIO;
    return (-1);
}

int  _fstat (int  file, struct stat  *st)
{
    memset (st, 0, sizeof(*st));
    st->st_mode = S_IFCHR;
    st->st_blksize = 1024;
    return (0);
}

int  _getpid (int n)
{
    return (1);
}

int  _gettimeofday (void  *tp, void  *tzp)
{
    return (-1);
}

int  isatty (int  fd)
{
    return (1);
}

int  _kill (int  pid, int  sig)
{
    if (pid == 1) {
        _exit(sig);
    }
    return (0);
}

off_t  _lseek (int  fd, off_t  offset, int  whence)
{
    errno = ESPIPE;
    return ((off_t)-1);
}

int  _open (const char  *buf, int  flags, int  mode)
{
    errno = EIO;
    return (-1);
}

int  _write (int  fd, const char  *buf, int  nbytes)
{
    int i;
    for (i = 0; i < nbytes; i++) {
        write_serial_byte (*(buf + i));
    }
    return (nbytes);
}

void  print (char  *ptr)
{
    while (*ptr != '\0')
    {
        write_serial_byte (*ptr++);
    }
}

void  _raise (void)
{
    return;
}

int  _read (int  fd, char  *buf, int  nbytes)
{
    int i;
    for (i = 0; i < nbytes; i++) {
        *(buf + i) = (char) read_serial_byte();
        if ((*(buf + i) == '\n') ||
            (*(buf + i) == '\r')) {
            i++;
            break;
        }
    }
    return (i);
}

extern  char  _heap[];      /* _heap  is set in the linker command file */
extern  char  _eheap[];     /* _eheap is set in the linker command file */
char    *heap_ptr;          /* modif ELR 15/11/2005                     */

char  *_sbrk (int nbytes)
{
    char  *base;


    if (heap_ptr == NULL) {
        heap_ptr = (char *)&_heap;
    }

    base = heap_ptr;
    if (heap_ptr + nbytes > _eheap) {
        errno = ENOMEM;
        return ((char *)-1);
    }

    heap_ptr += nbytes;
    return (base);
}

clock_t  _times (struct tms *tp)
{
   if (tp != NULL) {
      tp->tms_utime  = 0;
      tp->tms_stime  = 0;
      tp->tms_cutime = 0;
      tp->tms_cstime = 0;
   }
   return (0);
}

int  _rename (const char *oldpath, const char *newpath)
{
    errno = ENOSYS;
    return (-1);
}

int  _unlink (const char *pathname)
{
    errno = ENOSYS;
    return (-1);
}

int  _system (const char *command)
{
    if (command == NULL) {
        return (0);
    }
    errno = ENOSYS;
    return (-1);
}

void  *__get_memtop (void)
{
    return ((void *)&_eheap);
}
