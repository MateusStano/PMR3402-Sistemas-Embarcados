#include <ESP32Servo.h>
#include "BluetoothSerial.h" 

// SERVO
Servo myservo1;       // define servo 1 == servo fixo
Servo myservo2;       // define servo 2 == servo mangueira
int pos1 = 0;         // posicao servo 1
int pos2 = 0;         // posicao servo 2
int dir_servo1 = 0;   // direcao servo 1
int dir_servo2 = 0;   // direcao servo 2
int servoPin1 = 17;
int servoPin2 = 18;

// BOMBA
const int relay = 13;

// BLUETOOTH
BluetoothSerial ESP_BT; 
int incoming;

// Sensor
int Fire_analog = 12;
int firesensorAnalog;

// Constantes para os estados
const int estado_idle = 0;
const int estado_manual = 1;
const int estado_procurando_fogo = 2;
const int estado_fogo_encontrado = 3;
int codigoEvento, codigoAcao, estado, eventoInterno;

// Constantes para as ações
const int NENHUMA_ACAO = 0;
const int acao_modo_manual = 1;
const int acao_modo_automatico = 2;
const int acao_fogo_detect = 3;
const int acao_modo_idle = 4;
const int acao_cima = 5;
const int acao_baixo = 6;
const int acao_esquerda = 7;
const int acao_direita = 8;
const int acao_bomba = 9;

// Constantes para os eventos
const int NENHUM_EVENTO = 0;
const int MANUAL_ON = 1;
const int AUTOMATICO_ON = 2;
const int CIMA = 3;
const int BAIXO = 4;
const int ESQUERDA = 5;
const int DIREITA = 6;
const int BOMBA_ON = 7;
const int FOGO_DETECTADO = 8;

// Número de estados e eventos
const int NUM_ESTADOS = 5;
const int NUM_EVENTOS = 8;

// Matriz para ações e próximo estado
int acao_mTEstados[NUM_ESTADOS][NUM_EVENTOS];
int proximo_estado_mTEstados[NUM_ESTADOS][NUM_EVENTOS];

// Função para obter a ação correspondente ao estado e evento atual
int obterAcao(int estado, int evento) {
  return acao_mTEstados[estado][evento];
}

// Função para obter o próximo estado com base no estado atual e evento
int obterProximoEstado(int estado, int evento) {
  return proximo_estado_mTEstados[estado][evento];
}

// Função para inicializar a máquina de estados
void iniciaMaquinaEstados()
{
  int i, j;
  for (i=0; i < NUM_ESTADOS; i++) {
    for (j=0; j < NUM_EVENTOS; j++) {
      acao_mTEstados[i][j] = NENHUMA_ACAO;
      proximo_estado_mTEstados[i][j] = i;
    }
  }

  proximo_estado_mTEstados[estado_idle][MANUAL_ON] = estado_manual;
  acao_mTEstados[estado_idle][MANUAL_ON] = acao_modo_manual;

  proximo_estado_mTEstados[estado_idle][AUTOMATICO_ON] = estado_procurando_fogo;
  acao_mTEstados[estado_idle][AUTOMATICO_ON] = acao_modo_automatico;
  
  proximo_estado_mTEstados[estado_manual][AUTOMATICO_ON] = estado_procurando_fogo;
  acao_mTEstados[estado_manual][AUTOMATICO_ON] = acao_modo_automatico;

  proximo_estado_mTEstados[estado_manual][CIMA] = estado_manual;
  acao_mTEstados[estado_manual][CIMA] = acao_cima;

  proximo_estado_mTEstados[estado_manual][BAIXO] = estado_manual;
  acao_mTEstados[estado_manual][BAIXO] = acao_baixo;

  proximo_estado_mTEstados[estado_manual][ESQUERDA] = estado_manual;
  acao_mTEstados[estado_manual][ESQUERDA] = acao_esquerda;

  proximo_estado_mTEstados[estado_manual][DIREITA] = estado_manual;
  acao_mTEstados[estado_manual][DIREITA] = acao_direita;

  proximo_estado_mTEstados[estado_manual][BOMBA_ON] = estado_manual;
  acao_mTEstados[estado_manual][BOMBA_ON] = acao_bomba;
  
  proximo_estado_mTEstados[estado_procurando_fogo][MANUAL_ON] = estado_manual;
  acao_mTEstados[estado_procurando_fogo][MANUAL_ON] = acao_modo_manual;

  proximo_estado_mTEstados[estado_procurando_fogo][FOGO_DETECTADO] = estado_fogo_encontrado;
  acao_mTEstados[estado_procurando_fogo][FOGO_DETECTADO] = acao_fogo_detect;
}

// Função para executar ação com base no código da ação
int executarAcao(int codigoAcao) {
  int retval;
  retval = NENHUM_EVENTO;
  if (codigoAcao == NENHUMA_ACAO) return retval;
  switch(codigoAcao) {
    case acao_modo_manual:
      digitalWrite(relay, LOW);
      posicao_init_servos();
      break;
    case acao_modo_automatico:
      digitalWrite(relay, LOW);
      executa_varredura_servo1();
      break;
    case acao_fogo_detect:
      digitalWrite(relay, HIGH);
      executa_varredura_servo2;
      break;
    case acao_cima:
      pos2=pos2+10;
      myservo2.write(pos2);
      break;
    case acao_baixo:
      pos2=pos2-10;
      myservo2.write(pos2);
      break;
    case acao_esquerda:
      pos1=pos1+10;
      myservo1.write(pos1);
      break;
    case acao_direita:
      pos1=pos1-10;
      myservo1.write(pos1);
      break;
    case acao_bomba:
        if (digitalRead(relay) == HIGH){
            digitalWrite(relay, LOW);
        }
        if (digitalRead(relay) == LOW){
            digitalWrite(relay, HIGH);
        }
      break;
  }


  return retval;
}

void posicao_init_servos(){
  // posiciona os servos na posicao inicial neutra
  
  if (pos1 < 90){
    while (pos1 != 90){
      pos1++;
      myservo1.write(pos1);
      delay(15);         
    }
  }
  else if (pos1 > 90){
    while (pos1 != 90){
      pos1--;
      myservo1.write(pos1);
      delay(15);           
    }
  }
  if (pos2 < 90){
    while (pos2 != 90){
      pos2++;
      myservo2.write(pos2);
      delay(15);           
    }
  }
  else if (pos2 > 90){
    while (pos2 != 90){
      pos2--;
      myservo2.write(pos2);
      delay(15);           
    }
  }
}

void executa_varredura_servo1() {
  if (dir_servo1 == 0){
    pos1++;
    if (pos1 >= 179){
      dir_servo1 = 1;
    }
    myservo1.write(pos1);
    delay(50);
  }
  else {
    pos1--;
    if (pos1 <= 1){
      dir_servo1 = 0;
    }
    myservo1.write(pos1);
    delay(50);
  }
}

void executa_varredura_servo2() {
  if (dir_servo2 == 0){
    pos2++;
    if (pos1 >= 179){
      dir_servo2 = 1;
    }
    myservo2.write(pos2);
    delay(50);
  }
  else {
    pos2--;
    if (pos2 <= 1){
      dir_servo2 = 0;
    }
    myservo2.write(pos2);
    delay(50);
  }
}


int readEventosButtons() {
  if (ESP_BT.available()) {
      incoming = ESP_BT.read(); //Le dado recebido
  
      // Separa ID do botão do seu valor -> ID pode ser 10, 20, 30, etc, valor vale ou 1 ou 0
      int button = floor(incoming / 10);
      int value = incoming % 10;
      // print para debbug
      // Serial.print("Button:");Serial.print(button);Serial.print("\n");
      switch (button) {
        case 1:  // manual
          return MANUAL_ON;
          break;
        case 2:  // automatica
          return AUTOMATICO_ON;
          break;
        case 3: // cima 
          return CIMA;
          break;
        case 4: // baixo
          return BAIXO;
          break;
        case 5: // esquerda
          return ESQUERDA;
          break;
        case 6: // direita
          return DIREITA;
          break;
        case 7: // agua
          return BOMBA_ON;
          break;
      }
    }
 return NENHUM_EVENTO;
}

int obterEvento(int estadoAtual){
  // Eventos do sensor
  // Lê sensor e checa se fogo foi detectado
  // Apenas levanta esse evento para
  if (estadoAtual == estado_procurando_fogo) {
    int firesensorAnalog = analogRead(Fire_analog);
    if (firesensorAnalog < 1000){
      return FOGO_DETECTADO;
    }
  }
  // Checa se algum botão foi pressionado via bluetooth
  return readEventosButtons();
  }

void setup() {
  estado = estado_idle;
  eventoInterno = NENHUM_EVENTO;
  iniciaMaquinaEstados();
}

void loop() {
  if (eventoInterno == NENHUM_EVENTO) {
    // Obter evento externo
    codigoEvento = obterEvento(estado);
  } else {
    codigoEvento = eventoInterno;
  }
  if (codigoEvento != NENHUM_EVENTO) {
    codigoAcao = obterAcao(estado, codigoEvento);
    estado = obterProximoEstado(estado, codigoEvento);
    eventoInterno = executarAcao(codigoAcao);
    printf("Estado: %d Evento: %d Acao: %d\n", estado, codigoEvento, codigoAcao);
  }
}
