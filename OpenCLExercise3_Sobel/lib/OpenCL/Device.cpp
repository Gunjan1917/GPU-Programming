/*
 * Copyright (c) 2014 Steffen Kieß
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

#include "Device.hpp"

#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
#define CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV       0x4000
#endif
#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV
#define CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV       0x4001
#endif

namespace OpenCL {
  void printDeviceInfo(std::ostream& stream, const cl::Device& device) {
    stream << "Running on " << device.getInfo<CL_DEVICE_NAME> ();
    if ((" " + device.getInfo<CL_DEVICE_EXTENSIONS> () + " ").find (" cl_nv_device_attribute_query ") != std::string::npos) {
      cl_uint major = -1;
      cl_uint minor = -1;
      device.getInfo<cl_uint> (CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, &major);
      device.getInfo<cl_uint> (CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, &minor);
      stream << " (" << major << "." << minor << ")";
    }
    stream << std::endl;
  }
}
