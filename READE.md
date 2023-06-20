# Sistema Embarcado de Combate a Incêndio

Este é um sistema embarcado desenvolvido para auxiliar no combate a incêndios. O sistema utiliza um dispositivo baseado no ESP32, que controla um servo motor fixo e um servo motor conectado a uma mangueira de combate a incêndio.

## Funcionalidades

- Modo Manual: Permite controlar manualmente os movimentos dos servos e ativar/desativar a bomba d'água.
- Modo Automático: Realiza uma varredura automatizada com o servo fixo em busca de sinais de fogo. Quando detectado, aciona o servo da mangueira e a bomba d'água.

## Requisitos

- Placa ESP32
- Servo motor fixo
- Servo motor para a mangueira
- Sensor de fogo analógico
- Módulo Bluetooth

## Instalação

1. Conecte o servo motor fixo ao pino definido como `servoPin1` no código.
2. Conecte o servo motor da mangueira ao pino definido como `servoPin2` no código.
3. Conecte o sensor de fogo analógico ao pino definido como `Fire_analog` no código.
4. Conecte o módulo Bluetooth à placa ESP32.

## Configuração

1. Faça o upload do código para a placa ESP32 utilizando a IDE do Arduino ou outro método adequado.
2. Alimente a placa ESP32 com a energia necessária.

## Uso

1. Ligue o sistema embarcado pressionando o botão de ligar/desligar.
2. O sistema iniciará no modo Idle, aguardando comandos.
3. Para ativar o Modo Manual, pressione o botão correspondente no aplicativo Bluetooth.
4. No Modo Manual, utilize os botões de direção para mover o servo fixo (cima, baixo, esquerda, direita) e o botão de água para ativar/desativar a bomba.
5. Para ativar o Modo Automático, pressione o botão correspondente no aplicativo Bluetooth.
6. No Modo Automático, o sistema realizará uma varredura com o servo fixo em busca de sinais de fogo. Quando detectado, o servo da mangueira será acionado automaticamente, direcionando a água para o foco de incêndio.
7. Para retornar ao Modo Manual ou ao Modo Idle, utilize os botões correspondentes no aplicativo Bluetooth.
