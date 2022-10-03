// Funcao que retira o dado da queue
int retira_dado()
{
  return pilha_dados.pop();
}

// Funcao que esvazia a pilha
void pilha_to_string() {
  int quantidade_de_dados = pilha_dados.getCount();
  for (int i=0; i<quantidade_de_dados; i++)
  {
    string_de_dados += String(pilha_dados.getDataByIndex(i)) + "-";
  }
}

// Funcao que transfere os dados da pilha do ble pra pilha de envio
void ble_to_envio(){
  if (!pilha_dados_BLE.isEmpty()) {
    while (!pilha_dados_BLE.isEmpty()) {
      int dado = pilha_dados_BLE.pop();
      pilha_dados.push(dado);
      //Serial.println("Passei o dado" + String(dado) + "para a pilha de envio");
    }
  }
}

// Funcao para receber mensagem
void onReceive(int packetSize)
{
  if (packetSize == 0) return;          // Se nenhuma mesnagem foi recebida, retorna nada
  Serial.println(ack);
  Serial.print("\nentrei aqui\n");
  // Leu um pacote, vamos decodificar
  int recipient = LoRa.read();          // Endereco de quem ta recebendo
  byte sender = LoRa.read();            // Endereco do remetente
  byte incomingMsgId = LoRa.read();     // Mensagem
  byte incomingLength = LoRa.read();    // Tamanho da mensagem

  packSize = String(packetSize, DEC);

  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length())
  {
    // check length for error
    Serial.println("erro!: o tamanho da mensagem nao condiz com o conteudo!");
    return;
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF)
  {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // Caso a mensagem seja para este dispositivo, imprime os detalhes
  /*Serial.println("Recebido do dispositivo: 0x" + String(sender, HEX));
    Serial.println("Enviado para: 0x" + String(recipient, HEX));
    Serial.println("ID da mensagem: " + String(incomingMsgId));
    Serial.println("Tamanho da mensagem: " + String(incomingLength));
    Serial.println("Mensagem: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
    delay(500);*/

  ack = incoming;
  //Serial.print(ack);
  printText(incoming);
}

// Funcao que envia uma mensagem LoRa
void envia_dado(String outgoing)
{
  LoRa.beginPacket();                   // Inicia o pacote da mensagem
  LoRa.write(destination);              // Adiciona o endereco de destino
  LoRa.write(localAddress);             // Adiciona o endereco do remetente
  LoRa.write(msgCount);                 // Contador da mensagem
  LoRa.write(outgoing.length());        // Tamanho da mensagem em bytes
  LoRa.print(outgoing);                 // Vetor da mensagem
  LoRa.endPacket();                     // Finaliza o pacote e envia
  msgCount++;                           // Contador do numero de mensagnes enviadas
}

void printText(String string_dados) {
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0 , 0 , "Enviando: ");
  if (string_dados != "ACK") {
    Heltec.display->drawString(0 , 16 , string_dados);
  }
  else
  {
    Heltec.display->drawString(0 , 32 , string_dados);
  }
  Heltec.display->display();
}
