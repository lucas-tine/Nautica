/*    MULTIPLICADOR DE DOIS DÍGITOS PARA ARDUÍNO
 * 
 * AUTOR: LUCAS TINÉ 
 * ÚLTIMA ATUALIZAÇÃO: 18/11/18
 */

// displays 1 e 2 dispostos idealmente da esquerda para a direita 
// Parâmetros 


#define DISPLAY1          0  // terminal catodo comum do display 1 
#define DISPLAY2          8  // terminal catodo comum do display 2 
#define BUTTON1           10 
#define BUTTON2           11  
#define CATODO_ANODO      12 // controle externo do tipo de display, isolado indica catodo comum, ligado ao terra indica anodo comum
                             

#define TEMPO_CLICK       180// tempo mínimo entre cliques consecutivos (ms) e máximo entre dois simultâneos 
#define INTERVALO         180// duração do intervalo vazio entre o resultado e uma nova entrada de valores
#define FRAME_RATE        50 // taxa de atualização na exibição de dois digitos simultâneos (HZ)

#define FORCE_DISPLAY1    1
#define FORCE_DISPLAY2    2
#define FORCE_AMBOS       3

//      PROTÓTIPOS DAS FUNÇÕES DECLARADAS

typedef unsigned char Byte;

void displayNum (Byte, Byte, Byte); 
void displayProduto (Byte, Byte);

//      DECLARAÇÃO DE VARIÁVEIS

//garante que cada click será contado apenas uma vez
Byte bt_mute = 1; 

//guardam os números fornecidos pelos botões
Byte digito1 = 0;
Byte digito2 = 0;

Byte display_atual = 1; // indica o display a ser ligado no instante


char ligar;   // valores diferentes dependendo do tipo de display escolhido
char desligar;




void setup() { 
  pinMode (BUTTON1, INPUT_PULLUP);
  pinMode (BUTTON2, INPUT_PULLUP);
  pinMode (CATODO_ANODO, INPUT_PULLUP);

  DDRD = 0b11111111; // todos os terminais de 0-8 então em modo OUTPUT

  pinMode (DISPLAY1, OUTPUT);
  pinMode (DISPLAY2, OUTPUT);

  // Adaptando o controle ao tipo de display:

  ligar = (digitalRead(CATODO_ANODO) == HIGH)?  LOW : HIGH;
  desligar = (digitalRead(CATODO_ANODO) == HIGH)? HIGH : LOW;

  displayNum('-','-',FORCE_AMBOS); //inicialmente, os displays mostram traços
}




void loop() {
  
// o clique no botão é considerado valido apenas se o mesmo não estiver "mutado"
  if ( digitalRead(BUTTON1) == LOW && !bt_mute && digitalRead(BUTTON2) == HIGH ){
      delay(TEMPO_CLICK); 
      
      if ( digitalRead(BUTTON2) == HIGH){  //verificar se apenas um botão recebeu clique mesmo depois de TEMPO_CLICK
          bt_mute = 1; // botão mutado até que seja solto
          digito1++;
          if (digito1 > 9) // a contagem retorna de 9 para 0
              digito1 = 0;
          displayNum (digito1,digito1,FORCE_DISPLAY1);
      }    
  }

  if ( digitalRead(BUTTON2) == LOW && !bt_mute && digitalRead(BUTTON1) == HIGH ){ 
     delay(TEMPO_CLICK);

      if ( digitalRead(BUTTON1) == HIGH){ //verificar se realmente apenas um botão recebeu clique
          bt_mute = 1; // botão mutado até que seja solto
          digito2++;
          if (digito2 > 9) // 9 -> 0
              digito2 = 0;
          displayNum (digito2,digito2,FORCE_DISPLAY2);
      }
  }

// Um clique em ambos os botões exibe o produto entre os números escolhidos
  if (digitalRead(BUTTON1) == LOW && digitalRead(BUTTON2) == LOW && !bt_mute ){  
      displayProduto (digito1, digito2);

      digitalWrite(DISPLAY1, desligar);
      digitalWrite(DISPLAY2, desligar);

      // excluindo os números antigos
      digito1 = 0;    
      digito2 = 0;
      
      delay(INTERVALO);
      
      displayNum('-','-',FORCE_AMBOS); // Estado inicial
      bt_mute = 1; //garante que click que terminou DisplayProduto() não será detectado no loop principal
  }

// se ambos os botões estão soltos, são ambos desmutados
  if ( digitalRead(BUTTON1) == HIGH && digitalRead(BUTTON2) == HIGH){
      bt_mute = 0;
  }
}


//      DECLARAÇÃO DE FUNÇÕES  



void displayNum (Byte num1, Byte num2, Byte display_force = 0){ // Controle e decodificação dos displays  
  Byte digito_atual;

  // Os displays se alternam a cada ciclo
  if (display_atual == 1)
      digito_atual = num1;
  else
      digito_atual = num2;

  digitalWrite (DISPLAY2, desligar);
  
  switch (digito_atual){ // Decodificação de digito_atual para display, no formato (0b)abcdefg1

    case 1:
    digito_atual = 0b01100001;
    break;
    
    case 2:
    digito_atual = 0b11011011;                                                                                                               
    break;
    
    case 3:
    digito_atual = 0b11110011;
    break;
    
    case 4:
    digito_atual = 0b01100111;
    break;
    
    case 5:
    digito_atual = 0b10110111;
    break;
    
    case 6:
    digito_atual = 0b10111111;
    break;
    
    case 7:
    digito_atual = 0b11100001;
    break; 
    
    case 8:
    digito_atual = 0b11111111;
    break; 
    
    case 9:
    digito_atual = 0b11110111;
    break;
    
    case 0:
    digito_atual = 0b11111101;
    break;

    case '-':
    digito_atual = 0b00000011;
    break;  
  }  

  if (digitalRead (CATODO_ANODO) == LOW)
      PORTD = digito_atual^(0b11111111) ; // inversão bit-a-bit no caso de trabalharmos com Anodo Comum
  else
      PORTD = digito_atual;

  if (display_force != 0) // indica um determinado display dentre os dois a ser usado no momento
      display_atual = display_force;

  if (display_force == FORCE_AMBOS){ 
      digitalWrite (DISPLAY1, ligar);
      digitalWrite (DISPLAY2, ligar);
  }
  else if (display_atual == 1){
      digitalWrite (DISPLAY1, ligar);
      display_atual = 2; // garante que os displays se alternarão se a função for chamada várias vezes
  }
  else {
      digitalWrite (DISPLAY2, ligar);
      display_atual = 1; //displays se alternam se a função for chamada em loop    
  } 
}





void displayProduto (Byte fator1, Byte fator2){ // Mostra no display o produto entre os números selecionados
  
    Byte produto = fator1 * fator2;
    bt_mute = 1;

    //extraindo unidade e dezena do produto
    Byte unidade = produto % 10;
    Byte dezena = (produto-(produto % 10)) / 10 ;
    
    while (true){ //mostrar o produto nos displays enquanto nenhum botão for pressionado
        
        displayNum (dezena, unidade );
        delay (1000/FRAME_RATE); // tempo entre as atualizações do conjunto de displays  
        
        if ( digitalRead(BUTTON1) == HIGH && digitalRead(BUTTON2) == HIGH){ // desmutar botões quando forem soltos
            bt_mute = 0;
        }

        // encerrar a exibição quando um dos botões for novamente pressionado
        else if ( (digitalRead(BUTTON2) == LOW && !bt_mute)||(digitalRead(BUTTON1) == LOW && !bt_mute) ){
            break;
        }
    } 
}
