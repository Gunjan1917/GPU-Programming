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

#ifndef CORE_IMAGE_HPP_INCLUDED
#define CORE_IMAGE_HPP_INCLUDED

#include <boost/filesystem/path.hpp>

#include <ostream>

namespace Core {
  void writeImagePGM (std::ostream& stream, const uint8_t* data, size_t width, size_t height);
  void writeImagePGM (const boost::filesystem::path& filename, const uint8_t* data, size_t width, size_t height);
  void writeImagePGM (const boost::filesystem::path& filename, const std::vector<uint8_t>& data, size_t width, size_t height);
  void imageFloatToByte (const std::vector<float>& input, std::vector<uint8_t>& output);
  void writeImagePGM (const boost::filesystem::path& filename, const std::vector<float>& data, size_t width, size_t height);
  static inline void writeImagePGM (const char* filename, const std::vector<float>& data, size_t width, size_t height) { writeImagePGM ((boost::filesystem::path) filename, data, width, height); } // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."
  static inline void writeImagePGM (const std::string& filename, const std::vector<float>& data, size_t width, size_t height) { writeImagePGM ((boost::filesystem::path) filename, data, width, height); } // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."

  void writeImagePPM (std::ostream& stream, const uint8_t* data, size_t width, size_t height);
  void writeImagePPM (const boost::filesystem::path& filename, const uint8_t* data, size_t width, size_t height);
  void writeImagePPM (const boost::filesystem::path& filename, const std::vector<uint8_t>& data, size_t width, size_t height);
  void imageFloatToByteCol (const std::vector<float>& input, std::vector<uint8_t>& output);
  void writeImagePPM (const boost::filesystem::path& filename, const std::vector<float>& data, size_t width, size_t height);
  static inline void writeImagePPM (const char* filename, const std::vector<float>& data, size_t width, size_t height) { writeImagePPM ((boost::filesystem::path) filename, data, width, height); } // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."
  static inline void writeImagePPM (const std::string& filename, const std::vector<float>& data, size_t width, size_t height) { writeImagePPM ((boost::filesystem::path) filename, data, width, height); } // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."

  void readImagePGM (std::istream& stream, std::vector<float>& data, size_t& width, size_t& height);
  void readImagePGM (const boost::filesystem::path& filename, std::vector<float>& data, size_t& width, size_t& height);
  static inline void readImagePGM (const char* filename, std::vector<float>& data, size_t& width, size_t& height) { readImagePGM ((boost::filesystem::path) filename, data, width, height); } // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."
  static inline void readImagePGM (const std::string& filename, std::vector<float>& data, size_t& width, size_t& height) { readImagePGM ((boost::filesystem::path) filename, data, width, height); } // Workaround eclipse 3.7.2-1 (eclipse-cdt 8.0.2-1) bug (without this eclipse shows an error "Invalid arguments ..."
}

#endif // !CORE_IMAGE_HPP_INCLUDED
