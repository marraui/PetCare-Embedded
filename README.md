# PetCare-Embedded #

Código para el dispositivo embebido

## Build ##

El proyecto es construido utilizando [platform.io](https://platformio.org/) y [Visual Studio Code](https://code.visualstudio.com/) como IDE.

- Instalar [Visual Studio Code](https://code.visualstudio.com/)
- Instalar [platform.io](https://platformio.org/platformio-ide) para VS code
- Compila el código
- Conecta tu PC al ESP8266 a través de un cable usb (1)
- Monta el código (2)
- Monitorea el output del ESP8266 (3)

![image of instructions to build](https://github.com/marraui/PetCare-Embedded/blob/master/images/build.PNG)

## Uso ##

Una vez el código es montado en el ESP8266 este empezara a correr, tratando de conectarse a une red Wi-Fi, al no tener una red Wi-Fi guarda, creará su propia red a la cual te puedes conectar. Una vez te conectes serás reenviado a una página web donde podrás configurar la red Wi-Fi y además el correo de tu cuenta, el nombre de la mascota que usará el collar, y el tiempo que debería durar en hibernación despues de cada medición.

![wifi config image](https://github.com/marraui/PetCare-Embedded/blob/master/images/wifi-config.png?raw=true)

Una vez conectado el dispositivo, ponselo a tu mascota

![dog image wearing collar](https://github.com/marraui/PetCare-Embedded/blob/master/images/dog%20image.jpg?raw=true)
