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

#ifndef OPENCL_ERROR_HPP_INCLUDED
#define OPENCL_ERROR_HPP_INCLUDED

// Provide an error class for OpenCL

#include <Core/Exception.hpp>
#include <Core/Util.hpp>

#include <OpenCL/cl-patched.hpp>

namespace OpenCL {
  class Error : public Core::Exception {
  private:
    cl_int err_;
    const char * errStr_;

  public:
    Error (cl_int err, const char * errStr = NULL);
    ~Error () throw ();

    cl_int err () const { return err_; }
    const char* errStr () const { return errStr_; }

    virtual std::string message () const;
  };
}

#endif // !OPENCL_ERROR_HPP_INCLUDED
