################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/OBD/KLine.c \
../Core/Src/OBD/OBD.c \
../Core/Src/OBD/OBD_PID.c \
../Core/Src/OBD/can.c 

OBJS += \
./Core/Src/OBD/KLine.o \
./Core/Src/OBD/OBD.o \
./Core/Src/OBD/OBD_PID.o \
./Core/Src/OBD/can.o 

C_DEPS += \
./Core/Src/OBD/KLine.d \
./Core/Src/OBD/OBD.d \
./Core/Src/OBD/OBD_PID.d \
./Core/Src/OBD/can.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/OBD/%.o Core/Src/OBD/%.su Core/Src/OBD/%.cyclo: ../Core/Src/OBD/%.c Core/Src/OBD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L431xx -c -I../Core/Inc -I"C:/Users/miros/Desktop/bakalarka_latest/secured-vehicle-monitoring/iot_obd_monitoring_device/Drivers/BG77" -I"C:/Users/miros/Desktop/bakalarka_latest/secured-vehicle-monitoring/iot_obd_monitoring_device/Core/Inc/OBD" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-OBD

clean-Core-2f-Src-2f-OBD:
	-$(RM) ./Core/Src/OBD/KLine.cyclo ./Core/Src/OBD/KLine.d ./Core/Src/OBD/KLine.o ./Core/Src/OBD/KLine.su ./Core/Src/OBD/OBD.cyclo ./Core/Src/OBD/OBD.d ./Core/Src/OBD/OBD.o ./Core/Src/OBD/OBD.su ./Core/Src/OBD/OBD_PID.cyclo ./Core/Src/OBD/OBD_PID.d ./Core/Src/OBD/OBD_PID.o ./Core/Src/OBD/OBD_PID.su ./Core/Src/OBD/can.cyclo ./Core/Src/OBD/can.d ./Core/Src/OBD/can.o ./Core/Src/OBD/can.su

.PHONY: clean-Core-2f-Src-2f-OBD

