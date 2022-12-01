/*    TRABALHO DE CONCLUSÃO DE CURSO


      Nomes: Arthur Vinicius Lauxen, Giovani Kuckert, Julio Cesar Sironi Padilha


      Programação do µControlador ESP8266
*/

/*-------------- DECLARAÇÃO DE BIBLIOTECAS ----------------*/
#include <elapsedMillis.h>
//#include "display.h"
#include <Bitmap.h>
#include <DMDESP.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>
#include <fonts/ElektronMart6x12.h>
#include <fonts/Mono5x7.h>
#include <fonts/ElektronMart6x16.h>

//-------------------------------------------------------------------------------
/*-------------- DECLARAÇÃO DA PAGINA HTML ----------------*/
#include "PageIndex.h"

//-------------------------------------------------------------------------------
/*-------------- DECLARAÇÃO DA REDE E SENHA ----------------*/
const char*   rede    =   "Sironi";
const char*   senha   =   "192306jja";

//-------------------------------------------------------------------------------
/*-------------- CONFIGURAÇÕES DO IP DO SERVIDOR ----------------*/
IPAddress ip(192,168,1,155);                              //FAIXA DE IP DISPONÍVEL DO ROTEADOR
IPAddress gateway(192,168,1,1);                           //GATEWAY DE CONEXÃO
IPAddress subnet(255,255,255,0);                          //MASCARA DE REDE
IPAddress DNS1 (8, 8, 8, 8);
IPAddress DNS2 (8, 8, 8, 8);
ESP8266WebServer server(80);

//-------------------------------------------------------------------------------
/*-------------- CONFIGURAÇÕES DO SERVIDOR NTP ----------------*/
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//-------------------------------------------------------------------------------
/*-------------- CONFIGURAÇÕES DO DISPLAY DE LED ----------------*/
#define DISPLAYS_WIDE 2 //--> Colunas de paineis
#define DISPLAYS_HIGH 1 //--> Linhas de paineis
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);
//display disp(90,16, &Disp);

//-------------------------------------------------------------------------------
/*-------------- CONFIGURAÇÕES DO SENSOR DE TEMPERATURA ----------------*/
#define DHTPIN  4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//-------------------------------------------------------------------------------
/*-------------- VÁRIAVEIS PARA O CONTROLE DO PAINEL ----------------*/
int     temperatura;
String  hora;
String  data;
int     flag = 0;
long    interval = 20000;
elapsedMillis timeElapsed;

//-------------------------------------------------------------------------------
/*-------------- VÁRIAVEIS PARA O CONTROLE DA MENSAGEM ----------------*/
char   *standardText[] = {"COORDENACAO DE ELETRONICA"};
char   *Text[] = {"Eletronica"};
String  Incoming_Text = "";

//===============================================================================
/*-------------- FUNÇÃO PARA INICIAR A PÁGINA HTML ----------------*/
void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

//===============================================================================
/*-------------- FUNÇÃO PARA RECEBER A MENSAGEM DA PÁGINA HTML ----------------*/
void handle_Incoming_Text() {
  Incoming_Text = server.arg("TextContents");
  server.send(200, "text/plane", "");
  Process_Incoming_Text();
}

//===============================================================================
/*-------------- FUNÇÃO PARA ATRIBUIR A MENSAGEM PARA OUTRA VARIAVEL ----------------*/
void Process_Incoming_Text() {
  delay(500);
  Serial.println("Incoming text : ");
  Serial.println(Incoming_Text);
  Serial.println();
  int str_len = Incoming_Text.length() + 1;
  char char_array[str_len];
  Incoming_Text.toCharArray(char_array, str_len);
  strcpy(Text[0], char_array);
  Incoming_Text = "";
}

//===============================================================================
void setup() {
  Serial.begin(115200);                                   //Inicia a serial
  delay(100);

  Disp.start();                                           //Inicia o display
  Disp.setBrightness(50);                                 //Define o brilho do display
  Disp.setFont(Mono5x7);                                  //Define a fonte a ser usada
  
  WiFi.begin(rede, senha);                                //Inicia o servidor
  WiFi.config(ip, gateway, subnet, DNS1, DNS2);

  Serial.print("Conectando a ");
  Serial.print(rede);

  while (WiFi.status() != WL_CONNECTED) {                 //ENQUANTO STATUS FOR DIFERENTE DE CONECTADO
    delay(500);                                           //INTERVALO DE 500 MILISEGUNDOS
    Serial.print(".");                                    //ESCREVE O CARACTER NA SERIAL
  }
  
  Serial.println("");
  Serial.print("Conectado a rede sem fio ");
  Serial.println(rede);
  server.begin();
  Serial.println("Servidor iniciado");

  server.on("/", handleRoot);                             //Chama a função handleRoot
  server.on("/setText", handle_Incoming_Text);            //Chama a função handle_Incoming_Text

  Serial.print("IP para se conectar ao NodeMCU: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());                         //Mostra o IP do web server na serial
  
  timeClient.begin();                                     //Inicia o servior NTP
  timeClient.setTimeOffset(-10800);                       //Define o fuso horário do servidor NTP

  dht.begin();                                            //Inicia o sensor DHT
}

//===============================================================================
void loop() {
  
  server.handleClient();                                  //Atualiza o web server
  Disp.loop();                                            //Atualiza o display
  timeClient.update();                                    //Atualiza o servidor NTP

  temperatura = dht.readTemperature();                    //Realiza a leitura da temperatura

  hora = timeClient.getFormattedTime();                   //Realiza a leitura da hora
  
  time_t epochTime = timeClient.getEpochTime();           //
  struct tm *ptm = gmtime ((time_t *)&epochTime);         //
  int currentDay = ptm->tm_mday;                          //Realiza a leitura da data
  int currentMonth = ptm->tm_mon+1;                       //
  data=String(currentDay) + "/" + String(currentMonth);   //

  if(timeElapsed > interval){                             //
    flag+=1;                                              //
    timeElapsed = 0;                                      //
  }                                                       //Controla o tempo de exibição de cada etapa
                                                          //
  if (flag > 4){                                          // 
    flag = 0;                                             //
  }                                                       //

  if(flag == 0){                                          //
     Serial.println(hora);                                //
     Disp.clear();                                        //Exibe a hora no display
     Disp.drawText(9, 5, hora);                           //
  }                                                       //

  if(flag == 1){                                          //
     Serial.println(standardText[0]);                     //Exibe a mensagem padrão no display                          
     Scrolling_standardText(5, 50);                       //
  }                                                       //

  if(flag == 2){                                          //
     Serial.println(data);                                //
     Disp.clear();                                        //Exibe a data no display
     Disp.drawText(18, 5, data);                          //
  }                                                       //

   if(flag == 3){                                         //
     Serial.print(temperatura);                           //
     Serial.println(" *C");                               //
     Disp.clear();                                        //Exibe a temperatura no display
     Disp.drawText(22, 5, (String)temperatura);           //
     Disp.drawText(33, 5 , "*C");                         //
  }                                                       //

  if(flag == 4){                                          //
     Serial.println(Text[0]);                             //Exibe a mensagem no display                          
     Scrolling_Text(5, 50);                               //
  }                                                       //
}

//===============================================================================
/*-------------- FUNÇÃO PARA DESLOCAMENTO DO TEXTO ----------------*/
void Scrolling_Text(int y, uint8_t scrolling_speed) {
  static uint32_t pM;
  static uint32_t x;
  int width = Disp.width();
  Disp.setFont(Mono5x7);
  int fullScroll = Disp.textWidth(Text[0]) + width;
  if((millis() - pM) > scrolling_speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      return;
    }
    Disp.drawText(width - x, y, Text[0]);
  }  
}

//===============================================================================
/*-------------- FUNÇÃO PARA DESLOCAMENTO DO TEXTO PADRÃO ----------------*/
void Scrolling_standardText(int y, uint8_t scrolling_speed) {
  static uint32_t pM;
  static uint32_t x;
  int width = Disp.width();
  Disp.setFont(Mono5x7);
  int fullScroll = Disp.textWidth(standardText[0]) + width;
  if((millis() - pM) > scrolling_speed) { 
    pM = millis();
    if (x < fullScroll) {
      ++x;
    } else {
      x = 0;
      return;
    }
    Disp.drawText(width - x, y, standardText[0],1);
  }  
}

//===============================================================================
