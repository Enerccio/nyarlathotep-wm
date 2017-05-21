################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libds/funcops.c \
../src/libds/hmap.c 

OBJS += \
./src/libds/funcops.o \
./src/libds/hmap.o 

C_DEPS += \
./src/libds/funcops.d \
./src/libds/hmap.d 


# Each subdirectory must supply rules for building sources it contributes
src/libds/%.o: ../src/libds/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/usr2/fast/projects/pinion/nyarlathotep-wm/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


