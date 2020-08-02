#include <Ethernet.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define pinRele 12
#define pinBuzz 13

#define USER "root"
#define PASS ""

byte        mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress   hostName(127,0,0,1);

EthernetClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor cur = MySQL_Cursor(&conn);

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
int contador = 0;

const byte ROWS = 4; // Quatro Linhas
const byte COLS = 4; // Quatro Colunas

// Definindo quais são os caracteres correspondentes no teclado
char Keys[ROWS][COLS] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'},
};

// Conectando os pinos do arduino as suas respectivas linhas
byte rowPins[ROWS] = { 8, 7, 6, 5 };
// Conectando os pinos do arduino as suas respectivas colunas
byte colPins[COLS] = { 3, 2, 1, 0};
// Criando o objeto
Keypad kpd = Keypad( makeKeymap(Keys), rowPins, colPins, ROWS, COLS );

//Definição de qual será a senha
char senha[5] = {'0', '0', '0', '0', '0'};
//char palavra[7] = {'0', '0', '0', '0', '0', '0', '0'};
//int fechadura = 13;

void setup()
{
    pinMode( pinRele, OUTPUT);
    pinMode( pinBuzz, OUTPUT);
    digitalWrite( pinRele, HIGH);
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("Inicializando...");
    Ethernet.begin(mac_addr);
    if (conn.connect(hostName, 3306, USER, PASS))
    {
        lcd.print("Conectado!");
    }
    else
    {
        lcd.print("Erro! Erro!");
    }
    delay(1000);
    lcd.clear();
    lcd.print("DIGITE A SENHA: ");
}

void loop()
{
    row_values *row = NULL;
    long head_count = 0;
    char key = kpd.getKey();

    if(key)
    {
        lcd.setCursor(0, 0);
        // DEFINE O CURSOR PARA O LCD
        lcd.print("Digite a Senha: ");
        // IMPRIME NO LCD A STRING
        lcd.setCursor(contador, 1);
        lcd.print(key);
        // DEFINE O CURSOR PARA O LCD
        lcd.print(key);
        // IMPRIME NO LCD O CARACTERE EM KEY
        tone(pinBuzz,2000,100);
        // EMITE UM SINAL SONORO
        senha[contador] = key;
        // ATRIBUI O CARACTERE EM KEY A POSIÇÃO DEFINIDA PELO CONTADOR NA STRING SENHA
        contador++;
        // ICREMENTA O CONTADOR
        if(contador == 5)
        {
            // CASO CONTADOR SEJA IGUAL A 5...
            char str[] = "SELECT iduser FROM pim2.user WHERE acessKey = '%s'";
            char query[60];
            sprintf(query,str,senha);

            cur.execute(query);
            cur.get_columns();
            do
            {
                row = cur.get_next_row();
                if (row != NULL)
                {
                    libera_catraca();
                    contador = 0;
                }
                else
                {
                    lcd.clear();
                    lcd.print("Senha Incorreta!");
                    delay(1000);
                    contador = 0;
                }
            }
            while(row != NULL);
            cur.close();
        }
    }
}

void libera_catraca(void)
{
    lcd.clear();
    tone(pinBuzz,1000,100);
    digitalWrite(pinRele, LOW);
    lcd.setCursor(0, 0);
    lcd.print("FECHA EM:");
    lcd.setCursor(0, 1);
    for(int i = 10; i > 0; i--)
    {
        lcd.print(i);
        delay(1000);
    }
    digitalWrite(pinRele, HIGH);
    return;
}
