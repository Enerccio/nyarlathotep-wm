################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/animator.c \
../src/configuration.c \
../src/keyboard_manager.c \
../src/log.c \
../src/mouse_manager.c \
../src/pinion.c \
../src/render.c \
../src/windows.c \
../src/workspace.c 

OBJS += \
./src/animator.o \
./src/configuration.o \
./src/keyboard_manager.o \
./src/log.o \
./src/mouse_manager.o \
./src/pinion.o \
./src/render.o \
./src/windows.o \
./src/workspace.o 

C_DEPS += \
./src/animator.d \
./src/configuration.d \
./src/keyboard_manager.d \
./src/log.d \
./src/mouse_manager.d \
./src/pinion.d \
./src/render.d \
./src/windows.d \
./src/workspace.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr2/fast/projects/pinion/wlc/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


