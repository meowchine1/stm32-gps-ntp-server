################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/wiznet_ioLibrary/socket.c \
../Core/Src/wiznet_ioLibrary/w5500.c \
../Core/Src/wiznet_ioLibrary/w5500_port.c \
../Core/Src/wiznet_ioLibrary/wizchip_conf.c 

OBJS += \
./Core/Src/wiznet_ioLibrary/socket.o \
./Core/Src/wiznet_ioLibrary/w5500.o \
./Core/Src/wiznet_ioLibrary/w5500_port.o \
./Core/Src/wiznet_ioLibrary/wizchip_conf.o 

C_DEPS += \
./Core/Src/wiznet_ioLibrary/socket.d \
./Core/Src/wiznet_ioLibrary/w5500.d \
./Core/Src/wiznet_ioLibrary/w5500_port.d \
./Core/Src/wiznet_ioLibrary/wizchip_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/wiznet_ioLibrary/%.o Core/Src/wiznet_ioLibrary/%.su Core/Src/wiznet_ioLibrary/%.cyclo: ../Core/Src/wiznet_ioLibrary/%.c Core/Src/wiznet_ioLibrary/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/AC6/cubeideWS/ntpserv/Core/Src/wiznet_ioLibrary/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-wiznet_ioLibrary

clean-Core-2f-Src-2f-wiznet_ioLibrary:
	-$(RM) ./Core/Src/wiznet_ioLibrary/socket.cyclo ./Core/Src/wiznet_ioLibrary/socket.d ./Core/Src/wiznet_ioLibrary/socket.o ./Core/Src/wiznet_ioLibrary/socket.su ./Core/Src/wiznet_ioLibrary/w5500.cyclo ./Core/Src/wiznet_ioLibrary/w5500.d ./Core/Src/wiznet_ioLibrary/w5500.o ./Core/Src/wiznet_ioLibrary/w5500.su ./Core/Src/wiznet_ioLibrary/w5500_port.cyclo ./Core/Src/wiznet_ioLibrary/w5500_port.d ./Core/Src/wiznet_ioLibrary/w5500_port.o ./Core/Src/wiznet_ioLibrary/w5500_port.su ./Core/Src/wiznet_ioLibrary/wizchip_conf.cyclo ./Core/Src/wiznet_ioLibrary/wizchip_conf.d ./Core/Src/wiznet_ioLibrary/wizchip_conf.o ./Core/Src/wiznet_ioLibrary/wizchip_conf.su

.PHONY: clean-Core-2f-Src-2f-wiznet_ioLibrary

