/*
 * qemu-openpty.c
 *
 * Copyright (c) 2003-2008 Fabrice Bellard
 * Copyright (c) 2010 Red Hat, Inc.
 *
 * Wrapper function qemu_openpty() implementation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * This is not part of oslib-posix.c because this function
 * uses openpty() which often in -lutil, and if we add this
 * dependency to oslib-posix.o, every app will have to be
 * linked with -lutil.
 */

#include "qemu/osdep.h"
#include "qemu-common.h"

#if defined(__GLIBC__)
# include <pty.h>
#elif defined CONFIG_SOLARIS
# include <termios.h>
# include <stropts.h>
#else
# include <termios.h>
#endif


int qemu_openpty_raw(int *aslave, char *pty_name)
{
    int amaster;
    struct termios tty;
    char *pty_buf = NULL;
#define q_ptsname(x) ptsname(x)

    if (openpty(&amaster, aslave, pty_buf, NULL, NULL) < 0) {
        return -1;
    }

    /* Set raw attributes on the pty. */
    tcgetattr(*aslave, &tty);
    cfmakeraw(&tty);
    tcsetattr(*aslave, TCSAFLUSH, &tty);

    if (pty_name) {
        strcpy(pty_name, q_ptsname(amaster));
    }

    return amaster;
}
