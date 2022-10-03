 #include "ClasseUsuario.h"
 
 //construtor
    Usuario::Usuario(byte id)
    {
      if (qtd_obj == 0)
      {
        head = this;
      }
      else
      {
        tail->prox = this;
      }
      tail = this;
      
      qtd_obj++;
      identifier = id;
      pos_firstBPM = 0;
      pos_lastBPM = -1;
      qtd_BPM = 0;
      prox = NULL; 
      Serial.println("Obj:" + String(qtd_obj) +  "criado"); 
    }
    
    // destrutor
    Usuario::~Usuario()
    {
      qtd_obj--; // na vdd não era necessário declarar o destrutor, está aqui só por finalidade educacional
    //essa classe não suporta a destruição de objetos, precisaria adaptar o codigo para acertar os ponteiros de head e tail
    } 

    //procura se há o usuário com o id fornecido. Se achar retorna o endereço, caso não retorna NULL
    Usuario* Usuario::findUsuario(byte id)
    {
      int i;
      Usuario *obj = head;
      while (obj != NULL)
      {
        if (obj->identifier == id) return obj;
        else obj = obj->prox;
      }
      return NULL;
    }

  void Usuario::setBPM(int qtd, int* ptr)
  {
    int i;
    for (i=0;i<qtd;i++)
    {
      if(qtd_BPM < max_data_bpm) // ainda há espaço vago no vetor de BPM
      {
       BPM[pos_lastBPM+1]=*(ptr+i);
       pos_lastBPM++;
       qtd_BPM++;
      }
      else //vetor BPM já está cheio, usaremos lógica circular
      {
        BPM[pos_firstBPM]=*(ptr+i); //o primeiro cronologico é apagado e escrevemos o novo valor
        pos_lastBPM = pos_firstBPM;
        pos_firstBPM = (pos_lastBPM+1)%max_data_bpm;
        //Serial.println(pos_firstBPM);
      }
    }
     /*for (int i=0; i<max_data_bpm;i++)
      {Serial.print(this->BPM[i]);
      Serial.print("-");
      };
     Serial.println("");*/
  }

int * Usuario::allUsers_data_for_HTML()
{
  static int *ptr=NULL;
  static int last_qtd_obj;
 
  if(ptr==NULL)
  {
    last_qtd_obj=Usuario::qtd_obj;
    ptr=(int*)malloc(sizeof(int)* last_qtd_obj * max_data_bpm);
  }
  else{
  if(last_qtd_obj != Usuario::qtd_obj)
  {
    last_qtd_obj=Usuario::qtd_obj;
    ptr=(int*)realloc(ptr,sizeof(int)* last_qtd_obj * max_data_bpm);
  }
  }
  int j=0;
  for (j=0;j<max_data_bpm;j++)
  {
    *(ptr+j)=head->BPM[(head->pos_firstBPM + j)%max_data_bpm];
   // Serial.print(*(ptr+j));
  //  Serial.print("-");
   }
  Usuario * next = head->prox;
  int i=1;
  int offset;
  while(next != NULL)
  {
  offset=i*max_data_bpm;
  for (j=0;j<max_data_bpm;j++)
  {
    *(ptr+offset+j)= next->BPM[(next->pos_firstBPM + j)%max_data_bpm];
  }
  i++;
  next=next->prox;
  }
  return ptr;
}  


int Usuario::qtd_obj=0;
Usuario* Usuario::head=NULL;
Usuario* Usuario::tail=NULL;
