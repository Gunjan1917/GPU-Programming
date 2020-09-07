/*
 * Copyright (c) 2010-2014 Steffen Kieß
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

#include "Error.hpp"

#include <Core/Exception.hpp>

#include <OpenCL/GetError.hpp>
#include <OpenCL/cl-patched.hpp>

#include <sstream>

namespace OpenCL {
  Error::Error (cl_int err, const char * errStr) : err_(err), errStr_(errStr) {}
  Error::~Error () throw() {}

  std::string Error::message () const {
    std::stringstream str;
    str << "OpenCL Error: " << (errStr_ ? errStr_ : "empty") << ": " << getErrorString (err ());
    return str.str ();
  }
}

namespace cl {
  NORETURN errorHandler (cl_int err, const char * errStr) {
    throw OpenCL::Error (err, errStr);
  }
}
