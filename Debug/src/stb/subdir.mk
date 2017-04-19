################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/stb/stb.c 

OBJS += \
./src/stb/stb.o 

C_DEPS += \
./src/stb/stb.d 


# Each subdirectory must supply rules for building sources it contributes
src/stb/%.o: ../src/stb/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -Djournal_d -DLOG_LEVEL=1 -I/usr2/fast/projects/pinion/xwlc/wlc/include -I"/usr2/fast/projects/pinion/nyarlathotep-wm/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


