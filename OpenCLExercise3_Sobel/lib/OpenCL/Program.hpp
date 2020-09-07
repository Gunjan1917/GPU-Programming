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

#ifndef OPENCL_PROGRAM_HPP_INCLUDED
#define OPENCL_PROGRAM_HPP_INCLUDED

#include <Core/Util.hpp>

#include <OpenCL/Error.hpp>
#include <OpenCL/cl-patched.hpp>

#include <boost/filesystem/path.hpp>

#include <iostream>

namespace OpenCL {
  class BuildError : public Error {
  private:
    std::vector<std::string> logs_;

  public:
    BuildError (cl_int err, const char* errStr, const std::vector<std::string>& logs);
    ~BuildError () throw ();

    const std::vector<std::string>& logs () const { return logs_; }

    virtual std::string message () const;
  };

  cl::Program loadProgramSource (const cl::Context& context, const boost::filesystem::path& filename);
  // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."
  static inline cl::Program loadProgramSource (const cl::Context& context, const char* filename) {
    return loadProgramSource (context, (boost::filesystem::path) filename);
  }

  std::vector<std::string> buildProgramGetMsgs (const cl::Program& program, const std::vector<cl::Device>& devices, const std::string& options = "");
  void buildProgram (const cl::Program& program, const std::vector<cl::Device>& devices, const std::string& options = "", std::ostream& out = std::cerr);
}

#endif // !OPENCL_PROGRAM_HPP_INCLUDED
