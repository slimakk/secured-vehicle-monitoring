################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/OBD/CAN.c \
../Core/Src/OBD/KLine.c 

OBJS += \
./Core/Src/OBD/CAN.o \
./Core/Src/OBD/KLine.o 

C_DEPS += \
./Core/Src/OBD/CAN.d \
./Core/Src/OBD/KLine.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/OBD/CAN.o: ../Core/Src/OBD/CAN.c Core/Src/OBD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/OLED -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/OBD/%.o Core/Src/OBD/%.su: ../Core/Src/OBD/%.c Core/Src/OBD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I../Drivers/OLED -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-OBD

clean-Core-2f-Src-2f-OBD:
	-$(RM) ./Core/Src/OBD/CAN.d ./Core/Src/OBD/CAN.o ./Core/Src/OBD/CAN.su ./Core/Src/OBD/KLine.d ./Core/Src/OBD/KLine.o ./Core/Src/OBD/KLine.su

.PHONY: clean-Core-2f-Src-2f-OBD

