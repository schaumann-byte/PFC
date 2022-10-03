#include <heltec.h>
#define BAND    915E6

struct message_pack { //struct que serve para organizar os dados recebidos
  int recipient;          // Endereco do destinatário
  byte sender;            // Endereco do remetente
  byte incomingMsgId;     // ID da Mensagem
  byte incomingLength;    // Tamanho da Mensagem
  byte validade;
  String message;         // Mensagem propriamente dita
};
typedef struct message_pack message_pack;

// Funcao para receber mensagem
message_pack onReceive(byte localAddress);


void envia_dado(byte remetente,byte destinatario, String outgoing);
