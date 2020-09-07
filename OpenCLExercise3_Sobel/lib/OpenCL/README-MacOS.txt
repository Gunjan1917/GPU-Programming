How to make this code run in Eclipse under MacOS:
- Install boost using MacPorts http://www.macports.org/ (http://stackoverflow.com/questions/104322/how-do-you-install-boost-on-macos)
- Update the project properties (Properties => C/C++ Build => Settings):
  - Add include path (GCC C++ Compiler => Includes): /opt/local/include
  - Add library path (GCC C++ Linker => Libraries): /opt/local/lib
  - Remove library (GCC C++ Linker => Libraries): OpenCL
  - Add other linker option (GCC C++ Linker => Miscellaneous): -framework OpenCL
