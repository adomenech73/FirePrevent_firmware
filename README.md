# FirePrevent Firmware

El mantenimiento de una chimenea o fuego a tierra es una tarea importante de cara a la seguridad del hogar, FirePrevent es un sistema de monitorización y alerta sobre el estado de la chimenea que facilita su correcto mantenimiento.

El siguiente codigo compatible con arduino, se ha realizado para la plataforma de desarrollo Espressif, concretamente se ha utilizado como microcontrolador el dispositivo [WeMos D1 mini](http://www.wemos.cc/Products/d1_mini.html) al que se le añaden una pareja de K-termopar de unión fría con compensación [MAX6675](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/MAX6675.html).

La información obtenida con los dos sensores se envian a través de internet a un servicio de almacenamiento etiquetada con la dirección MAC del dispositivo para su posterior tratamiento.

Para su elaboración se han utilizado las librerias própias de la plataforma así como la libreria [WiFiManager](https://github.com/tzapu/WiFiManager) para facilitar la configuración inicial del dispositivo, la libreria [max6675](https://github.com/adafruit/MAX6675-library) para la interacción con los sensores y la libreria [LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) para la salida de los valores a través de un LCD I2C de [DFRobot](http://www.dfrobot.com/index.php)

![alt text](https://github.com/adomenech73/FirePrevent_firmware/raw/master/FirePrevent_bb.png "Sketch del dispositivo")
