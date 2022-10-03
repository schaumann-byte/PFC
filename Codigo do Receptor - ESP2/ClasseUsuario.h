#include <heltec.h>
#define max_data_bpm 30

class Usuario
{  
    private:
    //int BPM[max_data_bpm]={0};
    static Usuario *head;
    static Usuario *tail;
    Usuario *prox;
    int pos_firstBPM;  //primeiro elemento BPM ordem cronologica
    int pos_lastBPM;   //ultimo elemento BPM ordem cronologica
    int qtd_BPM;       //qtd de bpm armazenados

    public:
    //construtor
    Usuario(byte id);
    // destrutor
    ~Usuario();
   
    byte identifier;
    static int qtd_obj;
    //procura se há o usuário com o id fornecido. Se achar retorna o endereço, caso não retorna NULL
    static Usuario* findUsuario(byte id);
    int BPM[max_data_bpm]={0}; // so pra teste
    void setBPM(int qtd, int* ptr);
    static int * allUsers_data_for_HTML();

};
