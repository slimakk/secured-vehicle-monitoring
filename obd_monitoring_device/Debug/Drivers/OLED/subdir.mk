################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/OLED/ssd1306.c \
../Drivers/OLED/ssd1306_fonts.c 

OBJS += \
./Drivers/OLED/ssd1306.o \
./Drivers/OLED/ssd1306_fonts.o 

C_DEPS += \
./Drivers/OLED/ssd1306.d \
./Drivers/OLED/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/OLED/%.o Drivers/OLED/%.su: ../Drivers/OLED/%.c Drivers/OLED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I"D:/Bakalarka/secured-vehicle-monitoring/obd_monitoring_device/Core/Inc/OBD" -I../Drivers/OLED -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-OLED

clean-Drivers-2f-OLED:
	-$(RM) ./Drivers/OLED/ssd1306.d ./Drivers/OLED/ssd1306.o ./Drivers/OLED/ssd1306.su ./Drivers/OLED/ssd1306_fonts.d ./Drivers/OLED/ssd1306_fonts.o ./Drivers/OLED/ssd1306_fonts.su

.PHONY: clean-Drivers-2f-OLED

