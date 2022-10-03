#include <heltec.h>
#include <WiFi.h>
#include <WebServer.h>
#include "ClasseUsuario.h"
#include "LoRa.h"

#define max_datum_perMessage 100 //máximo qtd de BPM por mensagem

const byte localAddress = 0xBC;     // Endereco deste dispositivo LoRa
WebServer server(80);
String text = ""; //muito cansado para pensar em outra alternativa agora

struct BPM_data
{
  int qtd_data;
  static int BPM[max_datum_perMessage];
}; typedef struct BPM_data BPM_data;
int BPM_data::BPM[max_datum_perMessage];

BPM_data HearthMessage_decode(String BPM_str);

/*Put your SSID & Password*/
const char* ssid = "POCO F3";  // Enter SSID here
const char* password = "01020304";  //Enter Password here

//WebServer server(80);

void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Serial.println("Connecting to ");
  Serial.println(ssid);
  delay(500);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  for (int i = 0; i <= 9; i++) {
    //check wi-fi is connected to wi-fi network
    if (WiFi.status() == WL_CONNECTED) break;
    Serial.print(".");
    delay(1000);
  }
  //solução cachorra de dar restart no ESP caso não conecte no WIFI
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Conexão com WIFI não realizada após 10 segundos. Reiniciando...");
    ESP.restart();
  }
  //se conectou imprime os dados da conexão
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/data.txt", []() {
    server.send(200, "text/html", text);
  });

  server.on("/", handle_OnConnect);

  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  int packetSize = LoRa.parsePacket(); // se retorno diferente de 0 então tem uma mensagem disponivel
  yield();
  if (packetSize)
  {
    Usuario * ptr;
    message_pack incoming_message;
    incoming_message = onReceive(localAddress);
    Serial.println("Mensagem recebida de " +  String(incoming_message.sender));
    Serial.println(incoming_message.validade); 
    if (incoming_message.validade != 0) //se a mensagem for valida
    {
      ptr = Usuario::findUsuario(incoming_message.sender);
      if (ptr == NULL) //se eu não achar o ID eu crio um novo usuário
      {
        ptr = new Usuario(incoming_message.sender);
        Serial.println("Criação de novo usário: " + String(ptr->identifier)); 
        if (ptr == NULL) //alocação de memória falhou
        {
          Serial.println("Memória do ESP Estourada. Reiniciando em 3 segundos.");
          delay(3000);
          ESP.restart();
        }

      }
      BPM_data message = HearthMessage_decode(incoming_message.message);
   /* Serial.print("Qtd de BPM recebidos:");
      Serial.println(message.qtd_data);
      Serial.print("BPM recebidos");
      for (int i=0; i<message.qtd_data;i++)
      {Serial.print(message.BPM[i]);
      Serial.print("-");
      }
     */ 
      ptr->setBPM(message.qtd_data, message.BPM);
      envia_dado(localAddress, ptr->identifier, "ACK");
      text = criar_pagina(Usuario::qtd_obj, Usuario::allUsers_data_for_HTML());
      server.handleClient();
    }
  }
}

BPM_data HearthMessage_decode(String BPM_str)
{
  char str[5000];
  BPM_data decoded_message;
  decoded_message.qtd_data = 0;

  BPM_str.toCharArray(str, 1000);
  char * pch;
  pch = strtok (str, "-");
  while (pch != NULL && decoded_message.qtd_data < max_datum_perMessage)
  {
    String sementara = pch;
    decoded_message.BPM[decoded_message.qtd_data] = sementara.toInt();
    decoded_message.qtd_data++;
    pch = strtok (NULL, "-");
  }
  return decoded_message;
}
