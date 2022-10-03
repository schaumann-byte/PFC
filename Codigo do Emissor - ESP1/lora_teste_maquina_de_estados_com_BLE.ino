#include <heltec.h>
#include <SSVQueueStackArray.h>
#include "BLEDevice.h"

#define ESTADO_ENVIO 0
#define ESTADO_AGUARDA_ACK 1

volatile char estado = ESTADO_ENVIO;

int t_ini;

#define BAND    915E6

String string_de_dados;   // dado a ser enviado

byte localAddress = 0xBB;     // Endereco deste dispositivo LoRa
byte msgCount = 0;            // Contador de mensagens enviadas
byte destination = 0xBC;      // Endereco do dispositivo para enviar a mensagem (0xFF envia para todos devices)

float lastSendTime;           // TimeStamp da ultima mensagem enviada

float intervalACK = 2000;

float firstNonACKtime;
float intervalACKmax = 15000;

int flag_firstACK = 0;
String packSize = "";
String ack;

SSVQueueStackArray <int> pilha_dados;  //O default é criar uma queue(FIFO)
SSVQueueStackArray <int> pilha_dados_BLE;  //O default é criar uma queue(FIFO)

// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000180d-0000-1000-8000-00805f9b34fb");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("2a37");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
      Serial.print("Connected");
    }

    void onDisconnect(BLEClient* pclient) {
      connected = false;
      Serial.println("onDisconnect");
    }
};

/**
   Scan for BLE servers and find the first one that advertises the service we are looking for.
*/
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    /**
        Called for each advertising BLE server.
    */
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;

      } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  //Finção que recebe o packet do LoRa
  onReceive(LoRa.parsePacket());
  yield();
  /*Serial.print("Dado recebido: ");
  Serial.println(ack);*/

  //Função que gere as notificações do BLE
  BLE_loop();

  ble_to_envio();

  //Maquina de Estados
  switch (estado) {
    case ESTADO_ENVIO:
      {
        if (!pilha_dados.isEmpty()) {
          //Serial.println("entrei no IF DE ENVIO");
          string_de_dados = "";

          pilha_to_string(); //Esvazia a pilha e coloca os dados na string

          envia_dado(string_de_dados); //envio da string de dados

          printText(string_de_dados);

          Serial.println("Enviando " + string_de_dados);
          lastSendTime = millis();            // Timestamp da ultima mensagem
          ack = "";

          estado = ESTADO_AGUARDA_ACK;

          //teste sem ack
          
           //pilha_dados.resetData();
           //estado = ESTADO_ENVIO;
           
        }
        break;
      }

    case ESTADO_AGUARDA_ACK:
      {
        if (ack != "ACK") {
          if (float(millis()) - lastSendTime > intervalACK)  // Não 1: Nao recebeu o ACK e o intervalo de tempo é maior que 500ms
          {
            if (flag_firstACK == 0)
            {
              firstNonACKtime = millis();
              //Serial.println("Essa é a primeira vez que entra sem receber ack: " + String(firstNonACKtime));
            }
            Serial.print("Nao recebi ACK");
            Heltec.display->clear();
            Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
            Heltec.display->setFont(ArialMT_Plain_16);
            Heltec.display->drawString(0 , 0 , "Não recebi ACK");
            Heltec.display->display();
            flag_firstACK += 1;

            estado = ESTADO_ENVIO;
          }
          else if (float(millis()) - firstNonACKtime > intervalACKmax && flag_firstACK != 0)
          {
            //Serial.println("entrou no intervalo de 15s");
            pilha_dados.resetData();
            estado = ESTADO_ENVIO;
            flag_firstACK = 0;
          }
        }
        else {  // Recebeu o ACK
          //Serial.println("entrei no else do ACK");
          flag_firstACK = 0;
          pilha_dados.resetData();
          estado = ESTADO_ENVIO;
        }
        break;
      }
  }
  delay(50);
}
