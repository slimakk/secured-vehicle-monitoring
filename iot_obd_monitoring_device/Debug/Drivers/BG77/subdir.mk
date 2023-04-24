################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BG77/bg77.c 

OBJS += \
./Drivers/BG77/bg77.o 

C_DEPS += \
./Drivers/BG77/bg77.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BG77/%.o Drivers/BG77/%.su Drivers/BG77/%.cyclo: ../Drivers/BG77/%.c Drivers/BG77/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I"C:/Users/miros/Desktop/bakalarka_latest/secured-vehicle-monitoring/iot_obd_monitoring_device/Drivers/BG77" -I"C:/Users/miros/Desktop/bakalarka_latest/secured-vehicle-monitoring/iot_obd_monitoring_device/Core/Inc/OBD" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BG77

clean-Drivers-2f-BG77:
	-$(RM) ./Drivers/BG77/bg77.cyclo ./Drivers/BG77/bg77.d ./Drivers/BG77/bg77.o ./Drivers/BG77/bg77.su

.PHONY: clean-Drivers-2f-BG77

