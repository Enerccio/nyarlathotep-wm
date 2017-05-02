################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/decoration.c \
../src/necronomicon.c \
../src/registry.c \
../src/render.c 

OBJS += \
./src/decoration.o \
./src/necronomicon.o \
./src/registry.o \
./src/render.o 

C_DEPS += \
./src/decoration.d \
./src/necronomicon.d \
./src/registry.d \
./src/render.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/src/school/nwm/nyarlathotep-wm/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


