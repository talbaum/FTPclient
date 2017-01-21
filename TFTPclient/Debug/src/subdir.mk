################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/connectionHandler.cpp \
../src/echoClient.cpp \
../src/encDec.cpp 

OBJS += \
./src/connectionHandler.o \
./src/echoClient.o \
./src/encDec.o 

CPP_DEPS += \
./src/connectionHandler.d \
./src/echoClient.d \
./src/encDec.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


