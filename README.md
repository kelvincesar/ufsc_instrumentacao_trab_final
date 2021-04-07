# Trabalho final - Amostragem de dados com interface ao LabVIEW

    UNIVERSIDADE FEDERAL DE SANTA CATARINA - JOINVILLE
    ESE410009-41010086ME (20203) - Instrumentação para Sistemas Eletrônicos

    Aluno: Kelvin César de Andrade
    Data: 04/2021

**Descrição:**
- Trabalho final para amostrar um sinal e transmitir para o software LabVIEW 
via UART;

- Neste projeto é realizado a amostragem de um sinal do sensor de tensão AC 
ZMPT101B via I2S e armazenado em um buffer circular. Outra task é criada
 no núcleo 1 para realizar a transmissão via UART das amostras coletadas.

- Core 0: amostragem do sinal via I2S.
- Core 1: transmissão dos dados via UART para o software LabVIEW.


**Equipamentos:**
- ESP32;
- Sensor de tensão AC ZMPT101B;
- Software LabVIEW.
