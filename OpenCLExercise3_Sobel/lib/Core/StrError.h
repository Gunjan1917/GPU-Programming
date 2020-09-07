/*
 * Copyright (c) 2010-2012 Steffen Kieß
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CORE_STRERROR_H_INCLUDED
#define CORE_STRERROR_H_INCLUDED

// Wrapper for strerror_r / strerror_s

#include <Core/Util.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#if OS_UNIX
// Under linux/g++ the XSI strerror_r cannot be used from C++

// Call the XSI strerror_r
int MY_XSI_strerror_r (int errnum, char *buf, size_t buflen);
#endif

#if OS_WIN
int MY_strerror_s (char *buf, size_t size, int errnum);
#endif

#ifdef __cplusplus
}
#endif

#endif // !CORE_STRERROR_H_INCLUDED
