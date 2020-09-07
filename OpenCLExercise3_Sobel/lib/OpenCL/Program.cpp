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

#include "Program.hpp"

#include <Core/Error.hpp>

#include <OpenCL/GetError.hpp>

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <fstream>

namespace OpenCL {
  static std::string logsToString (const std::vector<std::string>& logs) {
    std::stringstream str;
    for (size_t i = 0; i < logs.size (); i++) {
      str << "Build log for device " << i << ":";
      std::string log = logs[i];
      log = "\n" + log;
      if (log.length () > 0 && log[log.length () - 1] == '\n')
        log = log.substr (0, log.length () - 1);
      boost::replace_all (log, "\n", "\n> ");
      str << log << std::endl << std::endl;
    }
    return str.str ();
  }

  BuildError::BuildError (cl_int err, const char* errStr, const std::vector<std::string>& logs) :
    Error (err, errStr), logs_ (logs) {
  }

  BuildError::~BuildError () throw () {}

  std::string BuildError::message () const {
    std::stringstream str;
    str << "OpenCL Build Error: " << (errStr () ? errStr () : "empty") << ": " << getErrorString (err ());
    str << std::endl << logsToString (logs ());
    return str.str ();
  }

  cl::Program loadProgramSource (const cl::Context& context, const boost::filesystem::path& filename) {
    std::string source;
    {
      std::ifstream in (filename.string ().c_str ());
      Core::Error::check ("open", in);
      std::stringstream sstr;
      sstr << in.rdbuf ();
      Core::Error::check ("read", in);
      source = sstr.str ();
    }

    std::vector<std::pair<const char*, size_t> > sources;
    sources.push_back (std::make_pair (source.data (), source.length ()));
    cl::Program program (context, sources);
    return program;
  }

  std::vector<std::string> buildProgramGetMsgs (const cl::Program& program, const std::vector<cl::Device>& devices, const std::string& options) {
    std::vector<cl_device_id> devices2 (devices.size ());
    for (size_t i = 0; i < devices2.size (); i++)
      devices2[i] = devices[i] ();
    cl_int ret = clBuildProgram (program (), (cl_uint) devices2.size (), devices2.data (), options.c_str (), NULL, NULL);

    std::vector<std::string> logs;
    for (std::vector<cl::Device>::const_iterator it = devices.begin (); it != devices.end (); it++)
      logs.push_back (program.getBuildInfo<CL_PROGRAM_BUILD_LOG> (*it));
  
    if (ret != CL_SUCCESS)
      throw BuildError (ret, "clBuildProgram", logs);

    return logs;
  }

  void buildProgram (const cl::Program& program, const std::vector<cl::Device>& devices, const std::string& options, std::ostream& out) {
    std::vector<std::string> logs = buildProgramGetMsgs (program, devices, options);
    bool foundWarning = false;
    for (size_t i = 0; i < logs.size () && !foundWarning; i++)
      if (boost::trim_copy (logs[i]) != "")
        foundWarning = true;
    if (foundWarning)
      out << "Got warnings while compiling OpenCL code:" << std::endl << logsToString (logs) << std::flush;
  }
}
