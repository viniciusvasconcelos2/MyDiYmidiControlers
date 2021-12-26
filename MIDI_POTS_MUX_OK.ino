#include <MIDI.h> //biblioteca para o chip ATMEGA328
MIDI_CREATE_DEFAULT_INSTANCE();
////////////////////////////////////////ORGANIZANDO O MUX/////////////////////////////////////////////

/*canais do mux4051:    ch0 =     pino no mux => 13
 *                  ch1 =   pino no mux => 14
 *                  ch2 =   pino no mux => 15
 *                  ch3 =    pino no mux => 12
 *                  ch4 =   pino no mux => 1
                    ch5 =   pino no mux => 5
                    ch6 =   pino no mux => 2
                    ch7 =   pino no mux => 4
                                  
//  PINO 11 DO MUX É O MSB A s0
PINO 10 DO MUX É O B s1
PINO 9 DO MUX É O MSB A
PINO 6, 7,8 GND, 
PINO 16 VCC
 */
#define MIDI_CH  1 //CANAL MIDI

#define N_MUX 2 //QUANTIDADE DE MUX USADOS

//define os pinos de controle dos mux
#define s0 4
#define s1 3
#define s2 2
//define as saídas dos mux
#define x0 A0
#define x1 A1
//um #define para cada mux
#define N_POTS 4 /*número de POTS em cada *obs.: se forem diferentes a quantidade de POTS em cada MUX, colocar a quantidade maior.
exemplo: 3 POTS no mux1 e 5 pots no mux2 então usar o 
maior valor*/

int mux4051[8][3]={ //define um array bidimensional para selecionar os canais do mux
  {0,0,0},  //ch0
  {0,0,1},  //ch1
  {0,1,0},  //ch2
  {0,1,1},  //ch3
  {1,0,0},  //ch4
  {1,0,1},  //ch5
  {1,1,0},  //ch6
  {1,1,1}   //ch7
};

int mux[N_MUX] = {x0,x1};//colocar as saídas dos mux, ou seja, as entradas do arduino, *obs: se precisar liberar código mudar o tipo de variável
int UltimaLeitura[N_POTS];
int LeituraAtual[N_POTS];
byte LeituraMidi[N_POTS];


byte CC_POTS[N_MUX][N_POTS]= {//esse array associa a leitura dos POTS de cada MUX , a um CC
  {36,37,38,39},
  {40,41,42,43}
  };


 
void setup() {
  MIDI.begin(MIDI_CH); //inicializa a comunicação MIDI
Serial.begin(115200); //inicializa a comunicação SERIAL
delay(100);


/////////////////////////////////////////////////////////////////////////////////////////////////////inicializa os arrays 
for (int i = 0 ; i < N_POTS ; i++){//inicializa estes arrays com 0, é descenessário mas está funcionando então vou deixar aqui
UltimaLeitura[i]= 0;
LeituraAtual[i]=0;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
pinMode (x0,INPUT_PULLUP);//colocar inputs em pullup
pinMode (x1,INPUT_PULLUP);
pinMode (s0,OUTPUT);
pinMode (s1,OUTPUT);
pinMode (s2,OUTPUT);

void loop() {
const int tempo = 20;//pra não ficar digitando toda hora o tempo dentro do delay

for (int i = 0; i< N_MUX ; i++){//aqui começa a brincadeira varrendo os muxs
    for(int j = 0; j< N_POTS ; j++){//depois os POTS
      digitalWrite(4,mux4051[j][2]); //seleciona as portas dos mux para que sejam lidas
      digitalWrite(3,mux4051[j][1]);
      digitalWrite(2,mux4051[j][0]);

      UltimaLeitura[j] = analogRead(mux[i]);//aqui temos o que indicará se tem POTS variando sua medida, lendo o pino de saída do mux selecionado, pra saber se está tendo variação

      delay(tempo);
      LeituraAtual[j] = analogRead(mux[i]);

        while ( abs(LeituraAtual[j]- UltimaLeitura[j])>5){ //enquanto houver variação continue lendo
                          LeituraAtual[j] = analogRead(mux[i]);
                          delay(tempo);
                          UltimaLeitura[j] = LeituraAtual[j];
                          delay(tempo);
                          LeituraAtual[j] = analogRead(mux[i]);
                          LeituraMidi[j] = map(LeituraAtual[j],22,1023,0,127);
                          

                          /*Serial.print("LEITURA POT("); //descomentar se for debugar
                          Serial.print(i);
                          Serial.print(j);
                          Serial.print(") :   ");
                          Serial.println(LeituraMidi[j]);*/
                          
                        MIDI.sendControlChange( CC_POTS[i][j] , LeituraMidi[j] , MIDI_CH);//comentar se for debugar //enviar o CC
                        
                          /*Serial.print("cc enviado:  ");//descomentar se for debugar
                          Serial.print(CC_POTS[i][j]);
                          Serial.print("   velocity  ");
                          Serial.print(LeituraMidi[j]);
                          Serial.print("  channel  "); 
                          Serial.println(MIDI_CH);  */
                          delay(tempo);
      }

    }
      
    }
  
}
