################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/OpenCL/Device.cpp \
../lib/OpenCL/Error.cpp \
../lib/OpenCL/Event.cpp \
../lib/OpenCL/GetError.cpp \
../lib/OpenCL/Program.cpp 

OBJS += \
./lib/OpenCL/Device.o \
./lib/OpenCL/Error.o \
./lib/OpenCL/Event.o \
./lib/OpenCL/GetError.o \
./lib/OpenCL/Program.o 

CPP_DEPS += \
./lib/OpenCL/Device.d \
./lib/OpenCL/Error.d \
./lib/OpenCL/Event.d \
./lib/OpenCL/GetError.d \
./lib/OpenCL/Program.d 


# Each subdirectory must supply rules for building sources it contributes
lib/OpenCL/%.o: ../lib/OpenCL/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DOMPI_SKIP_MPICXX -DCL_USE_DEPRECATED_OPENCL_1_1_APIS -I"/home/vayalala/workspace2/OpenCLExercise3_Sobel/lib" -I/usr/include/mpi -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


