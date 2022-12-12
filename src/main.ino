#include <Keypad.h>
#include <string.h>

// Configuração do keypad
const byte rows = 4;
const byte cols = 3;
char keys[rows][cols] = {
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};
byte rowPins[rows] = {8, 7, 6, 5};
byte colPins[cols] = {4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// Definição dos arrays de senha
char senhaOriginal[4];
char senhaTeste[4];

int sensorPin = A0; // Pino do sensor LDR

// Valores de estados iniciais
int igual = 1;
int tentativas = 0;
int sensorValue = 0;

void setup() {
    /*
    Funcao de inicializacao do sistema.
    Essa funcao simplesmente define os pinos e espera que o usuário defina a senha.
    */

    // Definindo os pinos
    pinMode(12, OUTPUT); // LED verde
    pinMode(11, OUTPUT); // LED vermelho
    pinMode(9, OUTPUT); // LED 'laser'
    pinMode(10, OUTPUT); // Buzzer
    Serial.begin(9600);

    // Um LED verde acende sinalizando que o software está aguardado que o usuário aperte '#'
    digitalWrite(12, HIGH);

    // Aguardando o sinal de que o botão '#' foi pressionado
    // O botão '#' deve ser pressionado para indicar a entrada de uma senha
    while(keypad.waitForKey() != '#') {}
    
    // O LED verde desliga quando o botão '#' é pressionado
    digitalWrite(12, LOW);

    // Recebendo dígito por dígito da senha do usuário
    for(int i = 0; i < 4; i++) { // Note que a senha possui 4 dígitos
        digitalWrite(11, HIGH); // LED vermelho acende indicando a espera por um dígito
        senhaOriginal[i] = keypad.waitForKey();
        digitalWrite(11, LOW); // LED vermelho apaga confirmando que o dígito foi capturado
        delay(100);
    }

    /*
    Após o fim da definição da senha os dois LEDs piscam duas vezes, indicando que a senha foi
    definida com sucesso.
    */
    for (int i = 0; i < 2; i++) {
        delay(500);
        digitalWrite(12, HIGH);
        digitalWrite(11, HIGH);
        delay(500);
        digitalWrite(12, LOW);
        digitalWrite(11, LOW);
    }

}

void loop() {
    /*
    Loop principal do programa. Recebe input da senha e dos sensores, além de controlar os LEDs e
    o buzzer.
    */

    igual = 1;

    /*
    O usuário deve pressionar '*' e em seguida digitar a senha previamente definida
    para que o sistema seja finalmente iniciado.
    */

    // Esperando a tecla '*'
    while(keypad.waitForKey() != '*') {}

    // Esperando a entrada da senha, da mesma forma que foi feito na função `setup`.
    for(int i = 0; i < 4; i++) {
        digitalWrite(11, HIGH);
        senhaTeste[i] = keypad.waitForKey();
        digitalWrite(11, LOW);
        delay(100);
    }

    // Verificando a igualdade entre as senhas digitadas...
    for (int i = 0; i < 4; i++) {
        if (!(senhaOriginal[i] == senhaTeste[i])) {
            igual = 0; // No primeiro caractere errado a senha já é considerada incorreta...
        }
    }

    // O loop a seguir é iniciado caso a senha esteja correta
    while (igual) {
        // Liga o laser:
        digitalWrite(9, HIGH);

        /*
        O usuário possui a opção de desativar o alarme a qualquer momento pressionando '*'
        e digitando a senha em seguida...
        */

        // Caso o sistema continue ativado (o usuário ainda não optou por desativa-lo)
        while(keypad.getKey() != '*') {
            // É feita a leitura do valor obtido pelo sensor
            sensorValue = analogRead(sensorPin);
            Serial.println(sensorValue); // Para fins de debugging o valor é exibido no terminal

            /*
            Para valores abaixo de um dado limiar o buzzer é acionado, gerando um aviso sonoro.
            O limiar definido foi de 700
            */

            if (sensorValue < 700) /* Verificação do valor em relação ao limiar */ {
                Serial.println("Alarme ativado!");
                digitalWrite(10, HIGH); // O buzzer é ativado
            }

        } // Espera senha para desativar

        // Aguardando a entrada da senha pelo usuário
        // O funcionamento é o mesmo do observado em `setup`
        for (int i = 0; i < 4; i++) {
            digitalWrite(11, HIGH);
            senhaTeste[i] = keypad.waitForKey();
            digitalWrite(11, LOW);
            delay(100);
        }

        // Verificação da senha correta
        bool senha_correta;
        senha_correta = (senhaOriginal[0] == senhaTeste[0]) && (senhaOriginal[1] == senhaTeste[1])
                     && (senhaOriginal[2] == senhaTeste[2]) && (senhaOriginal[3] == senhaTeste[3]);

        if (senha_correta) {
            // Em caso de senha correta o loop atual é encerrado
            digitalWrite(10, LOW);
            igual = 0; // Tal variável é definida como 0 para que o loop seja encerrado
            tentativas = 0; // Os erros são zerados para futuras iterações
        } else {
            // Em caso de erro a tentativa falha é contabilizada
            tentativas++;
        }

        if (tentativas >= 4) {
            // Em 4 tentativas falhas o sistema ativa o buzzer
            digitalWrite(10, HIGH);
        }
    }

    // Caso a senha esteja incorreta o laser se mantém apagado e o programa volta ao início do loop
    digitalWrite(9, LOW); // O laser apaga também em casos de desativação do sistema pelo usuário
}
