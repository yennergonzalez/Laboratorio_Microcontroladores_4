# Laboratorio Microcontroladores 4: STM32: GPIO, ADC, comunicaciones, Iot


En este laboratorio se implementó un sismógrafo utilizando el giroscopio de una placa STM32F429, además el sismógrafo detecta el nivel actual de las baterías que lo alimentan y transmite tanto la rotación en sus ejes como el nivel de la batería hacia la plataforma IoT Thingsboard.

La transmisión se puede deshabilitar al presionar el botón USER de la placa de desarrollo y se cuenta con dos LEDs para indicar el estado actual de la transmisión (habilitada o deshabilitada) y el estado de la batería (alta o baja).

## Autores
Yenner Josué González Araya - B83375
Gabriel Barahona Otoya - B70896



## Cargar el firmware a la placa de desarrollo

Para cargar el firmware en la placa se deben tener los **contenidos** del directorio src/firmware de este repositorio en un directorio dentro de la siguiente ruta, correspondiente a los ejemplos de la biblioteca libopencm3: .../libopencm3-examples/examples/stm32/stm32f429i-discovery/[DIRECTORIO-DESTINO]

Estando en este directorio se debe ejecutar la siguiente serie de comandos en la terminal:

`make`
`arm-none-eabi-objcopy -O binary lab4.elf  lab4.bin`
`st-flash --reset write lab4.bin 0x8000000`


## Enviar datos hacia Thingsboard

Para habilitar la transmisión de datos hacia Thingsboard se debe navegar en la terminal hacia la ruta que contiene el archivo `Script.py` de este repositorio (ubicado en src/python-iot-script/) y ejecutar el siguiente comando:

`python3 Script.py`
