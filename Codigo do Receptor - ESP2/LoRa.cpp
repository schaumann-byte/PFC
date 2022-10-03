#include "LoRa.h"

// Funcao para receber mensagem
message_pack onReceive(byte localAddress)
{
  message_pack incoming_data;
 
  // Leu um pacote, vamos decodificar
  incoming_data.recipient = LoRa.read();          // Endereco do destinatário
  incoming_data.sender = LoRa.read();            // Endereco do remetente
  incoming_data.incomingMsgId = LoRa.read();     // ID da Mensagem
  incoming_data.incomingLength = LoRa.read();    // Tamanho da mensagem

  // if the recipient isn't this device or broadcast,
  if (incoming_data.recipient != localAddress && incoming_data.recipient != 0xFF)
  {
    Serial.println("This message is not for me.");
    incoming_data.validade=0;
    // skip rest of function
    return incoming_data;
  }
  
  incoming_data.message = "";

  while (LoRa.available())
  {
    incoming_data.message += (char)LoRa.read();
  }

  if (incoming_data.incomingLength != incoming_data.message.length())
  {
    // check length for error
    Serial.println("erro!: o tamanho da mensagem nao condiz com o conteudo!");
    incoming_data.validade=0;
    return incoming_data;
  }
  incoming_data.validade=1;
  return incoming_data;
}

void envia_dado(byte remetente,byte destinatario, String outgoing)
{
  static int msgCount=0;
  while (LoRa.beginPacket() == 0) {delay(10);};
  //LoRa.beginPacket();                   // Inicia o pacote da mensagem
  LoRa.write(destinatario);              // Adiciona o endereco de destino
  LoRa.write(remetente);             // Adiciona o endereco do remetente
  LoRa.write(msgCount);                 // Contador da mensagem
  LoRa.write(outgoing.length());        // Tamanho da mensagem em bytes
  LoRa.print(outgoing);                 // Vetor da mensagem
  LoRa.endPacket();                     // Finaliza o pacote e envia
  msgCount++;                           // Contador do numero de mensagnes enviadas
}
