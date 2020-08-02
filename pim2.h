/*
 *   06 DE DEZEMBRO DE 2018, MANAUS - AM / UNIVERSIDADE PAULISTA - UNIP
 *   SUP TEC ANÁLISE DES SISTEMAS / TURMA: DS1A34 / TURNO: MATUTINO
 *
 *   - AUTOR:           WILLIAM BENJAMIM M SAMPAIO / RA: D7534B-7
 *   - CONTRIBUIRAM:    FELIPE MATEUS B NASCIMENTO / RA: N372HD-8
 *                      JOSE R C CARDOSO JUNIOR    / RA: N256BJ-3
 *                      LEVY DE LIRA GOMES         / RA: D7533H-9
 *                      LISVANETE RODRIGUES GARCIA / RA: N2572H-7
 */

#include <ctype.h>      // CONTEM ALGUMAS FUNÇÕES UTEIS PARA VALIDAÇÕA DE CARACTERE
#include <gtk/gtk.h>    // BIBLIOTECA PADRÃO DA API GTK+
#include <mysql.h>      // BIBLIOTECA DE PARA CONEXÃO COM MYSQL
#include <stdio.h>      // BIBLIOTECA PADRÃO DE E/S
#include <stdlib.h>     // BIBLIOTECA PADRÃO DO C
#include <string.h>     // CONTEM ALGUMAS FUNÇÕES UTEIS PARA MANIPULAÇÃO DE STRING
#include <time.h>       // CONTEM ALGUMAS FUNÇÕES UTEIS COM TEMPO

#define GLADE_FILE      "glade/window_main.glade"
// DIRETORIO E NOME DO ARQUIVO .glade, RESPONSAVEL POR MONTAR A GUI DO SISTEMA
#define KEY_FILE        "chaves_de_acesso/chaves.txt"
// DIRETORIO E NOME DO ARQUIVO COM AS CHAVES DE ACESSO DOS CONVIDADOS
#define SYSCONFIG_FILE  "config/sysconfig"
// DIRETORIO E NOME DO ARQUIVO COM AS CONFIGURAÇÕES DO SISTEMA

#define QTD_FILEIRA         21
// QUANTIDADE DE FILEIRAS
#define QTD_COLUNA          15
// QUANTIDADE DE ASSENTOS POR FILEIRA
#define QTD_TOTAL           QTD_FILEIRA * QTD_COLUNA
// TOTAL DE LUGARES
#define QTD_RES_CON_FIL     2
// TOTAL DE FILEIRAS RESERVADAS PARA CONVIDADOS
#define QTD_RES_PCD         15
// TOTAL DE ASSENTOS RESERVADOS PARA PESSOAS COM DEFICIÊNCIA (5%)
#define QTD_COMUM           QTD_TOTAL - ((QTD_RES_CON_FIL * QTD_COLUNA) + QTD_RES_PCD)
// TOTAL DE ASSENTOS RESERVADOS PARA PESSOAS COMUNS

/*  ESTRUTURA DE DADOS - ASSENTO
 *    - CONTÉM TODOS OS ATRIBUTOS DA "CLASSE" ASSENTO:
 *
 *      (STRING) TEXT: CONTEM O TEXTO DO ASSENTO
 *      (INT)    TYPE: CONTEM O TIPO DE ASSENTO.
 *          TIPO 1 - CONVIDADOS
 *          TIPO 2 - PESSOAS COM DEFICIÊNCIA
 *          TIPO 3 - PARTICIPANTES COMUM
 *      (INT)    USED: RECEBE "TRUE" OU "FALSE":
 *          FALSE - ASSENTO DISPONIVEL
 *          TRUE  - ASSENTO INDISPONIVEL, OCUPADO
 */
typedef struct
{
    char TEXT[5];
    //  O TEXTO DO ASSENTO, EX: A-1, F-10, etc.
    int  TYPE;
    //  TIPO = 1 - CONVIDADO
    //  TIPO = 2 - PCD
    //  TIPO = 3 - PARTICIPANTE COMUM
    int  USED;
    //  USED = FALSE: ASSENTO NÃO OCUPADO, DISPONIVEL
    //  USED = TRUE : ASSENTO OCUPADO
} ASSENTO;

/*  ESTRUTURA DE DADOS - AUDITORIO
 *    - CONTÉM TODOS OS ATRIBUTOS DA "CLASSE" ASSENTO:
 *
 *      (ASSENTO) LUGAR: MATRIZ COM O TOTAL DE POSIÇÕES REPRESENTANDO OS ASSENTOS.
                         CADA POSIÇÃO CONTEM OS ATRIBUTOS DO TIPO (ASSENTO).
 *      (INT)     _TTL: CONTEM O TOTAL DE ASSENTO.
 *      (INT)     _CON: CONTEM O TOTAL DE ASSENTO PARA OS CONVIDADOS.
 *      (INT)     _PCD: CONTEM O TOTAL DE ASSENTO PARA AS PESSOAS COM DEFICIENCIA.
 *      (INT)     _COM: CONTEM O TOTAL DE ASSENTO PARA OS PARTICIPANTES COMUNS.
 *      (INT)     _CON_OK: CONTEM O TOTAL DE ASSENTO OCUPADOS PELOS CONVIDADOS.
 *      (INT)     _PCD_OK: CONTEM O TOTAL DE ASSENTO OCUPADOS PELAS PESSOAS COM DEFICIENCIA.
 *      (INT)     _COM_OK: CONTEM O TOTAL DE ASSENTO OCUPADOS PELOS PARTICIPANTES COMUNS.
 */
typedef struct
{
    ASSENTO lugar[QTD_FILEIRA][QTD_COLUNA];
    int _TTL;
    int _CON;
    int _PCD;
    int _COM;
    int _CON_OK;
    int _PCD_OK;
    int _COM_OK;
} AUDITORIO;

/*  ESTRUTURA DE DADOS - BANCODEDADOS
 *    - ARMAZENA TODOS OS ATRIBUTOS DE CONEXÃO AO BANCO DE DADOS:
 *
 *      (STRING) HOST: HOSTNAME.
 *      (STRING) USER: USERNAME.
 *      (STRING) PASS: PASSWORD.
 *      (STRING) DATB: DATABASE.
 *      (INT)    PORT: PORT.
 */
typedef struct
{
    char HOST[60];
    char USER[60];
    char PASS[60];
    char DATB[60];
    char PORT[10];
} BANCODEDADOS;

int _start(void);
void _initGTK(void);
void _finiGTK(void);
void _firstExec(void);
void _quit(void);
void _ConstructAuditorio(AUDITORIO *audi, int fileira, int assento);
void _progressBar(GtkWidget **obj, double progress, int step);

char* addNewUser(AUDITORIO *audi, int tipo);
char* dataHora(void);

void createInviteKey(void);
void saveTxtInvitKey(void);
void validatInvitKey(void);

void hardReset(void);
void setPag0(void);
void goToPage0(void);
void goToPage1(void);
void goToPage2(void);
void goToPage3(void);
void goToPage4(void);
void goToPage5(void);
void goToPage6(void);
void goToPage7(void);
void goToPage8(void);
void goToPage9(void);
void goToPage10(void);

void cadConvidado(void);
void cadConviProgressBar(void);
void cadParticipante(void);
void cadPartiProgressBar(void);

int getAssentosTtl(AUDITORIO *audi);
int getAssentosCon(AUDITORIO *audi);
int getAssentosPcd(AUDITORIO *audi);
int getAssentosCom(AUDITORIO *audi);
int getAssentosConOk(AUDITORIO *audi);
int getAssentosPcdOk(AUDITORIO *audi);
int getAssentosComOk(AUDITORIO *audi);
int setAssentosConOk(AUDITORIO *audi, int num);
int setAssentosPcdOk(AUDITORIO *audi, int num);
int setAssentosComOk(AUDITORIO *audi, int num);

int getAuditorioBackup(AUDITORIO *audi);

int getConfigDb(BANCODEDADOS *db);

void configDb_clicked(void);
void testarDb_clicked(void);
void resetaDb_clicked(void);
void limparDb_clicked(void);

int consultaDocDb(char *cpf);
int consultaInviteKeyDb(char *key);
int consultaKeyDb(char *key);

void adminExit(void);
void adminLogin(void);

int validarNome(char *nome);
int validarTele(char *tel);
int validarMail(char *mail);
int validarDocu(char *doc);

// GLOBAL VARS
GtkWidget   *assistant,
            /* VARIAVEL QUE RECEBE O TELA PRINCIPAL DO PROGRAMA */
            *pag0_btn1,
            *pag0_btn2,
            *pag0_btn3,
            *pag0_lbl1,
            *pag0_lbl2,
            *pag0_lbl3,
            *pag0_lbl4,
            *pag0_lbl5,
            *pag0_lbl6,
            *pag0_img1,
            *pag0_img2,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 0 */
            *pag1_btn1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 1 */
            *pag2_btn1,
            *pag2_btn2,
            *pag2_input1,
            *pag2_input2,
            *pag2_input3,
            *pag2_input4,
            *pag2_rbtn1,
            *pag2_rbtn2,
            *pag2_rbtn3,
            *pag2_label1,
            *pag2_pbar1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 2 */
            *pag3_btn1,
            *pag3_btn2,
            *pag3_input1,
            *pag3_input2,
            *pag3_input3,
            *pag3_input4,
            *pag3_label1,
            *pag3_pbar1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 3 */
            *pag4_btn1,
            *pag4_btn2,
            *pag4_input1,
            *pag4_label1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 4 */
            *pag5_btn1,
            *pag5_label1,
            *pag5_label2,
            *pag5_label3,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 5 */
            *pag6_btn1,
            *pag6_btn2,
            *pag6_btn3,
            *pag6_input1,
            *pag6_input2,
            *pag6_label1,
            *pag6_label2,
            *pag6_pbar1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 6 */
            *pag7_btn1,
            *pag7_btn2,
            *pag7_btn3,
            *pag7_btn4,
            *pag7_btn5,
            *pag7_btn6,
            *pag7_btn7,
            *pag7_label1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 7 */
            *pag8_btn1,
            *pag8_btn2,
            *pag8_btn3,
            *pag8_btn4,
            *pag8_btn5,
            *pag8_btn6,
            *pag8_btn7,
            *pag8_input1,
            *pag8_input2,
            *pag8_input3,
            *pag8_input4,
            *pag8_input5,
            *pag8_label1,
            *pag8_pbar1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 8 */
            *pag9_btn1,
            /* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 9 */
            *pag10_btn1,
            *pag10_label1,
            *pag10_label2,
            *pag10_label3,
            *pag10_label4;
/* VARIAVEIS QUE RECEBEM OS OBJETOS DA INTERFACE DA PAG. 10 */
GtkBuilder  *builder;
/* VARIAVEL QUE DE CONSTRUÇÃO DO INTERFACE */
GError *err = NULL;
/* RECEBE O LOG DE ERRO DO GTK */
AUDITORIO Audi;
/* DECLARA UMA VARIAVEL DO TIPO AUDITORIO */
BANCODEDADOS Db;
/* DECLARA UMA VARIAVEL DO TIPO BANCODEDADOS */
char ultimaLugar[5];
/*  GUARDA O ULTIMO LUGAR CADASTRADO */
char ultimaChave[6];
/* GUARDA A ULTIMA CHAVE GERADA */

/*
*       **********  **      **  **      **    ********    ******    **********    ********
*       **********  **      **  ***     **   *********  **********  **********   *********
*       **          **      **  ****    **  ***         **      **  **          **
*       *******     **      **  *****   **  **          **      **  *******     *******
*       *******     **      **  **  **  **  **          **      **  *******      *********
*       **          **      **  **   ** **  ***         **      **  **                  **
*       **           *********  **    ****   *********  **********  **********   ********
*       **            ******    **     ***    ********    ******    **********   *******
*/

int _start(void)
{
    /*
        ESTA FUNCÃO INICIA TODO O PROGRAMA
    */
    _ConstructAuditorio(&Audi.lugar,QTD_FILEIRA,QTD_COLUNA);
    _initGTK();
    _firstExec();
    _finiGTK();
}

void _initGTK(void)
{
    /*
        ESTA FUNÇÃO CAPTURA E ARMAZENA TODOS OS OBJETOS DA INTERFACE
        QUE ESTA EM "glade/window_main.glade".
    */
    gtk_init (NULL, NULL);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, GLADE_FILE, &err);
    assistant = GTK_WIDGET(gtk_builder_get_object(builder, "assistant"));
    gtk_window_set_title (GTK_WINDOW (assistant), "SISTEMA DE CADASTRAMENTO");
    gtk_window_fullscreen(assistant);
    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect (G_OBJECT (assistant), "close", G_CALLBACK (goToPage6), NULL);
    g_signal_connect (G_OBJECT (assistant), "show",  G_CALLBACK (setPag0), NULL);

    pag0_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
    pag0_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));
    pag0_btn3 = GTK_WIDGET(gtk_builder_get_object(builder, "button3"));
    pag0_lbl1 = GTK_WIDGET(gtk_builder_get_object(builder, "label56"));
    pag0_lbl2 = GTK_WIDGET(gtk_builder_get_object(builder, "label57"));
    pag0_lbl3 = GTK_WIDGET(gtk_builder_get_object(builder, "label59"));
    pag0_lbl4 = GTK_WIDGET(gtk_builder_get_object(builder, "label37"));
    pag0_lbl5 = GTK_WIDGET(gtk_builder_get_object(builder, "label58"));
    pag0_lbl6 = GTK_WIDGET(gtk_builder_get_object(builder, "label10"));
    pag0_img1 = GTK_WIDGET(gtk_builder_get_object(builder, "image10"));
    pag0_img2 = GTK_WIDGET(gtk_builder_get_object(builder, "image11"));
    g_signal_connect(pag0_btn1,"clicked", G_CALLBACK(goToPage1), NULL);
    g_signal_connect(pag0_btn2,"clicked", G_CALLBACK(goToPage2), NULL);
    g_signal_connect(pag0_btn3,"clicked", G_CALLBACK(goToPage4), NULL);

    pag1_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button4"));
    g_signal_connect(pag1_btn1,"clicked", G_CALLBACK(goToPage0), NULL);

    pag2_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button40"));
    pag2_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button7"));
    pag2_input1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry11"));
    pag2_input2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry12"));
    pag2_input3 = GTK_WIDGET(gtk_builder_get_object(builder, "entry13"));
    pag2_input4 = GTK_WIDGET(gtk_builder_get_object(builder, "entry14"));
    pag2_rbtn1 =  GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton4"));
    pag2_rbtn2 =  GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton5"));
    pag2_rbtn3 =  GTK_WIDGET(gtk_builder_get_object(builder, "radiobutton1"));
    pag2_label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label32"));
    pag2_pbar1 = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar4"));
    g_signal_connect(pag2_btn1,"clicked", G_CALLBACK(hardReset), NULL);
    g_signal_connect(pag2_btn2,"clicked", G_CALLBACK(cadParticipante), NULL);
    g_signal_connect(pag2_input1,"changed", G_CALLBACK(cadPartiProgressBar), NULL);
    g_signal_connect(pag2_input2,"changed", G_CALLBACK(cadPartiProgressBar), NULL);
    g_signal_connect(pag2_input3,"changed", G_CALLBACK(cadPartiProgressBar), NULL);
    g_signal_connect(pag2_input4,"changed", G_CALLBACK(cadPartiProgressBar), NULL);
    g_signal_connect(pag2_rbtn1,"clicked", G_CALLBACK(cadPartiProgressBar), NULL);
    g_signal_connect(pag2_rbtn1,"clicked", G_CALLBACK(cadPartiProgressBar), NULL);

    pag3_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button29"));
    pag3_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button39"));
    pag3_input1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry7"));
    pag3_input2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry8"));
    pag3_input3 = GTK_WIDGET(gtk_builder_get_object(builder, "entry9"));
    pag3_input4 = GTK_WIDGET(gtk_builder_get_object(builder, "entry10"));
    pag3_label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label31"));
    pag3_pbar1 = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar3"));
    g_signal_connect(pag3_btn1,"clicked", G_CALLBACK(hardReset), NULL);
    g_signal_connect(pag3_btn2,"clicked", G_CALLBACK(cadConvidado), NULL);
    g_signal_connect(pag3_input1,"changed", G_CALLBACK(cadConviProgressBar), NULL);
    g_signal_connect(pag3_input2,"changed", G_CALLBACK(cadConviProgressBar), NULL);
    g_signal_connect(pag3_input3,"changed", G_CALLBACK(cadConviProgressBar), NULL);
    g_signal_connect(pag3_input4,"changed", G_CALLBACK(cadConviProgressBar), NULL);

    pag4_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button46"));
    pag4_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button47"));
    pag4_input1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry15"));
    pag4_label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label93"));
    g_signal_connect(pag4_btn1,"clicked", G_CALLBACK(hardReset), NULL);
    g_signal_connect(pag4_btn2,"clicked", G_CALLBACK(validatInvitKey), NULL);

    pag5_btn1 =  GTK_WIDGET(gtk_builder_get_object(builder, "button5"));
    pag5_label1 =  GTK_WIDGET(gtk_builder_get_object(builder, "label7"));
    pag5_label2 =  GTK_WIDGET(gtk_builder_get_object(builder, "label36"));
    pag5_label3 =  GTK_WIDGET(gtk_builder_get_object(builder, "label39"));
    g_signal_connect(pag5_btn1,"clicked", G_CALLBACK(hardReset), NULL);

    pag6_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button9"));
    pag6_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button11"));
    pag6_btn3 = GTK_WIDGET(gtk_builder_get_object(builder, "button12"));
    pag6_input1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry16"));
    pag6_input2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
    pag6_label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label12"));
    pag6_label2 = GTK_WIDGET(gtk_builder_get_object(builder, "label17"));
    pag6_pbar1 = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar1"));
    g_signal_connect(pag6_btn1,"clicked", G_CALLBACK(hardReset), NULL);
    g_signal_connect(pag6_btn2,"clicked", G_CALLBACK(adminLogin), NULL);
    g_signal_connect(pag6_btn3,"clicked", G_CALLBACK(adminExit), NULL);

    pag7_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button15"));
    pag7_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button14"));
    pag7_btn3 = GTK_WIDGET(gtk_builder_get_object(builder, "button13"));
    pag7_btn4 = GTK_WIDGET(gtk_builder_get_object(builder, "button21"));
    pag7_btn5 = GTK_WIDGET(gtk_builder_get_object(builder, "button25"));
    pag7_btn6 = GTK_WIDGET(gtk_builder_get_object(builder, "button22"));
    pag7_btn7 = GTK_WIDGET(gtk_builder_get_object(builder, "button6"));
    pag7_label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label41"));
    g_signal_connect(pag7_btn1,"clicked", G_CALLBACK(goToPage10), NULL);
    g_signal_connect(pag7_btn2,"clicked", G_CALLBACK(createInviteKey), NULL);
    g_signal_connect(pag7_btn3,"clicked", G_CALLBACK(goToPage8), NULL);
    g_signal_connect(pag7_btn4,"clicked", G_CALLBACK(hardReset), NULL);
    g_signal_connect(pag7_btn5,"clicked", G_CALLBACK(goToPage9), NULL);
    g_signal_connect(pag7_btn6,"clicked", G_CALLBACK(_quit), NULL);
    g_signal_connect(pag7_btn7,"clicked", G_CALLBACK(saveTxtInvitKey), NULL);

    pag8_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button17"));
    pag8_btn2 = GTK_WIDGET(gtk_builder_get_object(builder, "button19"));
    pag8_btn3 = GTK_WIDGET(gtk_builder_get_object(builder, "button20"));
    pag8_btn4 = GTK_WIDGET(gtk_builder_get_object(builder, "button23"));
    pag8_btn5 = GTK_WIDGET(gtk_builder_get_object(builder, "button24"));
    pag8_btn6 = GTK_WIDGET(gtk_builder_get_object(builder, "button16"));
    pag8_input1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry2"));
    pag8_input2 = GTK_WIDGET(gtk_builder_get_object(builder, "entry3"));
    pag8_input3 = GTK_WIDGET(gtk_builder_get_object(builder, "entry4"));
    pag8_input4 = GTK_WIDGET(gtk_builder_get_object(builder, "entry5"));
    pag8_input5 = GTK_WIDGET(gtk_builder_get_object(builder, "entry6"));
    pag8_label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label35"));
    pag8_pbar1 = GTK_WIDGET(gtk_builder_get_object(builder, "progressbar2"));
    g_signal_connect(pag8_btn1,"clicked", G_CALLBACK(resetaDb_clicked), NULL);
    g_signal_connect(pag8_btn2,"clicked", G_CALLBACK(goToPage7), NULL);
    g_signal_connect(pag8_btn3,"clicked", G_CALLBACK(testarDb_clicked), NULL);
    g_signal_connect(pag8_btn4,"clicked", G_CALLBACK(configDb_clicked), NULL);
    g_signal_connect(pag8_btn5,"clicked", G_CALLBACK(limparDb_clicked), NULL);
    g_signal_connect(pag8_btn6,"clicked", G_CALLBACK(_quit), NULL);

    pag9_btn1 = GTK_WIDGET(gtk_builder_get_object(builder, "button8"));
    g_signal_connect(pag9_btn1,"clicked", G_CALLBACK(goToPage7), NULL);

    pag10_btn1 =  GTK_WIDGET(gtk_builder_get_object(builder, "button10"));
    pag10_label1 =  GTK_WIDGET(gtk_builder_get_object(builder, "label43"));
    pag10_label2 =  GTK_WIDGET(gtk_builder_get_object(builder, "label44"));
    pag10_label3 =  GTK_WIDGET(gtk_builder_get_object(builder, "label45"));
    pag10_label4 =  GTK_WIDGET(gtk_builder_get_object(builder, "label47"));
    g_signal_connect(pag10_btn1,"clicked", G_CALLBACK(goToPage7), NULL);
}

void _finiGTK(void)
{
    /*
        ESTA FUNÇÃO RENDERIZA(EXIBE) NA TELA O PROGRAMA
    */
    g_object_unref(builder);
    gtk_widget_show(assistant);
    gtk_main();
}

void _firstExec(void)
{
    /* FUNÇÃO EXECUTADA SEMPRE QUE O PROGRAMA E INICIADO */
    FILE *sysconfig, *audconfig;
    int STATUS;
    sysconfig = fopen(SYSCONFIG_FILE,"r");
    if(sysconfig == NULL)
    {
        goToPage8();
    }
    else
    {
        getConfigDb(&Db);
    }
    fclose(sysconfig);
    getAuditorioBackup(&Audi);
}

void _quit(void)
{
    /* FUNÇÃO QUE FECHA O SOFTWARE */
    return gtk_main_quit();
}

void _ConstructAuditorio(AUDITORIO *audi, int fileira, int assento)
{
    /* FUNÇÃO CONFIGURA TODO O AUDITORIO */
    int row, col;
    for(row = 0; row < fileira; row++)
    {
        for(col = 0; col < assento; col++)
        {
            switch(row)
            {
            case 0:
                strcpy(audi->lugar[row][col].TEXT,"A-");
                break;
            case 1:
                strcpy(audi->lugar[row][col].TEXT,"B-");
                break;
            case 2:
                strcpy(audi->lugar[row][col].TEXT,"C-");
                break;
            case 3:
                strcpy(audi->lugar[row][col].TEXT,"D-");
                break;
            case 4:
                strcpy(audi->lugar[row][col].TEXT,"E-");
                break;
            case 5:
                strcpy(audi->lugar[row][col].TEXT,"F-");
                break;
            case 6:
                strcpy(audi->lugar[row][col].TEXT,"G-");
                break;
            case 7:
                strcpy(audi->lugar[row][col].TEXT,"H-");
                break;
            case 8:
                strcpy(audi->lugar[row][col].TEXT,"I-");
                break;
            case 9:
                strcpy(audi->lugar[row][col].TEXT,"J-");
                break;
            case 10:
                strcpy(audi->lugar[row][col].TEXT,"K-");
                break;
            case 11:
                strcpy(audi->lugar[row][col].TEXT,"L-");
                break;
            case 12:
                strcpy(audi->lugar[row][col].TEXT,"M-");
                break;
            case 13:
                strcpy(audi->lugar[row][col].TEXT,"N-");
                break;
            case 14:
                strcpy(audi->lugar[row][col].TEXT,"O-");
                break;
            case 15:
                strcpy(audi->lugar[row][col].TEXT,"P-");
                break;
            case 16:
                strcpy(audi->lugar[row][col].TEXT,"Q-");
                break;
            case 17:
                strcpy(audi->lugar[row][col].TEXT,"R-");
                break;
            case 18:
                strcpy(audi->lugar[row][col].TEXT,"S-");
                break;
            case 19:
                strcpy(audi->lugar[row][col].TEXT,"T-");
                break;
            case 20:
                strcpy(audi->lugar[row][col].TEXT,"U-");
                break;
            }
            switch(col)
            {
            case 0:
                strcat(audi->lugar[row][col].TEXT,"1");
                break;
            case 1:
                strcat(audi->lugar[row][col].TEXT,"2");
                break;
            case 2:
                strcat(audi->lugar[row][col].TEXT,"3");
                break;
            case 3:
                strcat(audi->lugar[row][col].TEXT,"4");
                break;
            case 4:
                strcat(audi->lugar[row][col].TEXT,"5");
                break;
            case 5:
                strcat(audi->lugar[row][col].TEXT,"6");
                break;
            case 6:
                strcat(audi->lugar[row][col].TEXT,"7");
                break;
            case 7:
                strcat(audi->lugar[row][col].TEXT,"8");
                break;
            case 8:
                strcat(audi->lugar[row][col].TEXT,"9");
                break;
            case 9:
                strcat(audi->lugar[row][col].TEXT,"10");
                break;
            case 10:
                strcat(audi->lugar[row][col].TEXT,"11");
                break;
            case 11:
                strcat(audi->lugar[row][col].TEXT,"12");
                break;
            case 12:
                strcat(audi->lugar[row][col].TEXT,"13");
                break;
            case 13:
                strcat(audi->lugar[row][col].TEXT,"14");
                break;
            case 14:
                strcat(audi->lugar[row][col].TEXT,"15");
                break;
            }
            if(row < QTD_RES_CON_FIL)
            {
                audi->lugar[row][col].TYPE = 1;
            }
            else
            {
                audi->lugar[row][col].TYPE = 0;
            }
            audi->lugar[row][col].USED = FALSE;
        }
    }
    audi->_TTL = QTD_TOTAL;
    audi->_CON = QTD_RES_CON_FIL * QTD_COLUNA;
    audi->_PCD = QTD_RES_PCD;
    audi->_COM = QTD_COMUM;
    audi->_CON_OK = 0;
    audi->_PCD_OK = 0;
    audi->_COM_OK = 0;
    return;
}

void _progressBar(GtkWidget **obj, double progress, int step)
{
    /* RECEBE COMO PARAMETRO O OBJETO DO TIPO BARRA DE PROGRESSO E DEFINE A % CONCLUIDA */
    while (gtk_events_pending ())
        gtk_main_iteration ();
    g_usleep(step*1000);
    char *message = g_strdup_printf ("%.0f%%", progress);
    gtk_progress_bar_set_fraction (obj, progress / 100.0);
    gtk_progress_bar_set_text (obj, message);
}

char* addNewUser(AUDITORIO *audi, int tipo)
{
    /* ADCIONA MAIS UM USUARIO AO AUDITÓRIO */
    int row, col;
    if(tipo == 1)
    {
        for(row = 0; row < QTD_RES_CON_FIL; row++)
        {
            for(col = 0; col < QTD_COLUNA; col++)
            {
                if(audi->lugar[row][col].TYPE == 1 && audi->lugar[row][col].USED == FALSE)
                {
                    audi->lugar[row][col].USED = TRUE;
                    audi->_CON_OK++;
                    return audi->lugar[row][col].TEXT;
                }
            }
        }
    }
    else if(tipo == 2)
    {
        for(row = QTD_RES_CON_FIL; row < QTD_FILEIRA; row++)
        {
            for(col = 0; col < QTD_COLUNA; col++)
            {
                if(audi->lugar[row][col].TYPE != 1 && audi->lugar[row][col].USED == FALSE)
                {
                    audi->lugar[row][col].USED = TRUE;
                    audi->_PCD_OK++;
                    return audi->lugar[row][col].TEXT;
                }
            }
        }
    }
    else
    {
        for(row = QTD_RES_CON_FIL; row < QTD_FILEIRA; row++)
        {
            for(col = 0; col < QTD_COLUNA; col++)
            {
                if(audi->lugar[row][col].TYPE != 1 && audi->lugar[row][col].USED == FALSE)
                {
                    audi->lugar[row][col].USED = TRUE;
                    audi->_COM_OK++;
                    return audi->lugar[row][col].TEXT;
                }
            }
        }
    }
    return 1;
}

char* dataHora(void)
{
    /* RETORNA UMA STRING COM A DATA E HORA ATUAIS */
    return __DATE__ " - " __TIME__;
}

void createInviteKey(void)
{
    /* CRIA E INSERE NO BANCO DE DADOS AS CHAVES PARA CADASTRO DOS CONVIADDOS */
    MYSQL* conn;
    char query[60];
    int key;
    int ttl = getAssentosCon(&Audi);
    srand(321 * time(NULL));
    key = 10000 + rand() % 90000;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            mysql_query(conn,"DELETE FROM invite");
            mysql_query(conn,"ALTER TABLE invite AUTO_INCREMENT = 1");
            for(int i=0; i<ttl; i++)
            {
                sprintf(query,"INSERT INTO invite (invitKey) VALUES ('%d');",key+i);
                mysql_query(conn,query);
            }
            return mysql_close(conn);
        }
    }
}

void saveTxtInvitKey(void)
{
    /* SALVA TODAS AS CHAVES DE CADASTRO DE CONVIDADOS */
    FILE *chaves;
    MYSQL *conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    chaves = fopen(KEY_FILE,"w+");
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[] = "SELECT * FROM invite";
            qstate = mysql_query(conn,query);
            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    fprintf(chaves,"%s-> %s | %s\n",row[0],row[1],row[2]);
                    Sleep(30);
                }
            }
        }
    }
    fclose(chaves);
}

void validatInvitKey(void)
{
    /* VERIFICA DE CHAVE DE CADASTRO DE CONVIDADO É VALIDA */
    MYSQL *conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    char query[60];
    char key[6];
    strcpy(key,gtk_entry_get_text(GTK_ENTRY(pag4_input1)));
    for(int i=0; i<strlen(key); i++)
    {
        if(!isdigit(key[i]))
        {
            gtk_entry_set_text(GTK_ENTRY(pag4_input1),"");
            return gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">CHAVE DE ACESSO INVALIDA!</span>"));
        }
    }
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            sprintf(query,"SELECT invitStatus FROM invite WHERE invitKey = '%s'",key);
            qstate = mysql_query(conn,query);
            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        if(strcmp(row[0],"FALSE") == 0)
                        {
                            mysql_close(conn);
                            return goToPage3();
                        }
                        else
                        {
                            mysql_close(conn);
                            gtk_entry_set_text(GTK_ENTRY(pag4_input1),"");
                            return gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">CHAVE DE ACESSO JA UTILIZADA!</span>"));
                        }
                    }
                    else
                    {
                        mysql_close(conn);
                        gtk_entry_set_text(GTK_ENTRY(pag4_input1),"");
                        return gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">CHAVE DE ACESSO INVALIDA!</span>"));
                    }
                }
            }
            else
            {
                return gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">%s</span>",mysql_error(conn)));
            }
        }
        else
        {
            return gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">%s</span>",mysql_error(conn)));
        }
    }
    else
    {
        return gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">%s</span>",mysql_error(conn)));
    }
}

void hardReset(void)
{
    /* RESETA O ESTADO DOS OBJETOS DA INTERFACE */
    gtk_entry_set_text(GTK_ENTRY(pag2_input1),"");
    gtk_entry_set_text(GTK_ENTRY(pag2_input2),"");
    gtk_entry_set_text(GTK_ENTRY(pag2_input3),"");
    gtk_entry_set_text(GTK_ENTRY(pag2_input4),"");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pag2_rbtn3),TRUE);
    gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    gtk_entry_set_text(GTK_ENTRY(pag3_input1),"");
    gtk_entry_set_text(GTK_ENTRY(pag3_input2),"");
    gtk_entry_set_text(GTK_ENTRY(pag3_input3),"");
    gtk_entry_set_text(GTK_ENTRY(pag3_input4),"");
    gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    gtk_entry_set_text(GTK_ENTRY(pag4_input1),"");
    gtk_label_set_text(GTK_LABEL(pag4_label1),"");
    gtk_entry_set_text(GTK_ENTRY(pag6_input1),"");
    gtk_entry_set_text(GTK_ENTRY(pag6_input2),"");
    gtk_label_set_markup(GTK_LABEL(pag6_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
    goToPage0();
}

void setPag0(void)
{
    /* CONFIGURA A PAGINA INICIAL */
    gtk_label_set_markup(GTK_LABEL(pag0_lbl1),g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\%s</span>",gtk_label_get_text(GTK_LABEL(pag0_lbl1))));
    gtk_label_set_markup(GTK_LABEL(pag0_lbl2),g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\%s</span>",gtk_label_get_text(GTK_LABEL(pag0_lbl2))));
    gtk_label_set_markup(GTK_LABEL(pag0_lbl3),g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\%s</span>",gtk_label_get_text(GTK_LABEL(pag0_lbl3))));
    char strLbl1[50];
    char strLbl2[50];
    char strLbl3[50];
    sprintf(strLbl1,"%d LUGARES",getAssentosTtl(&Audi));
    sprintf(strLbl2,"%d / %d LUGARES",(getAssentosComOk(&Audi)+getAssentosPcdOk(&Audi)),(getAssentosCom(&Audi))+(getAssentosPcd(&Audi)));
    sprintf(strLbl3,"%d / %d LUGARES",getAssentosConOk(&Audi),getAssentosCon(&Audi));
    gtk_label_set_markup(GTK_LABEL(pag0_lbl4), g_markup_printf_escaped("<span weight=\"bold\" background=\"yellow\" foreground=\"red\" size=\"x-large\">\%s</span>",strLbl1));
    gtk_label_set_markup(GTK_LABEL(pag0_lbl5), g_markup_printf_escaped("<span weight=\"bold\" background=\"yellow\" foreground=\"red\" size=\"x-large\">\%s</span>",strLbl2));
    gtk_label_set_markup(GTK_LABEL(pag0_lbl6), g_markup_printf_escaped("<span weight=\"bold\" background=\"yellow\" foreground=\"red\" size=\"x-large\">\%s</span>",strLbl3));
    gtk_widget_hide(GTK_WIDGET(pag0_img1));
    gtk_widget_hide(GTK_WIDGET(pag0_img2));
    if((getAssentosComOk(&Audi)+getAssentosPcdOk(&Audi)) == (getAssentosCom(&Audi))+(getAssentosPcd(&Audi)))
    {
        gtk_widget_hide(GTK_WIDGET(pag0_btn2));
        gtk_widget_show(GTK_WIDGET(pag0_img1));
    }
    if(getAssentosConOk(&Audi) == getAssentosCon(&Audi))
    {
        gtk_widget_hide(GTK_WIDGET(pag0_btn3));
        gtk_widget_show(GTK_WIDGET(pag0_img2));
    }
    return;
}

void goToPage0(void)
{
    for(int i=0;i<QTD_FILEIRA;i++)
    {
        for(int j=0;j<QTD_COLUNA;j++)
        {
            g_print("%d",Audi.lugar[i][j].USED);
        }
        g_print("\n");
    }
    /* VAI PRA PAGINA 0 */
    getConfigDb(&Db);
    getAuditorioBackup(&Audi);
    setPag0();
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 0);
    return;
}

void goToPage1(void)
{
    /* VAI PRA PAGINA 1 */
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 1);
    return;
}

void goToPage2(void)
{
    /* VAI PRA PAGINA 2 */
    gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 2);
    return;
}

void goToPage3(void)
{
    /* VAI PRA PAGINA 3 */
    gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 3);
    return;
}

void goToPage4(void)
{
    /* VAI PRA PAGINA 4 */
    gtk_label_set_markup(GTK_LABEL(pag4_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 4);
    return;
}

void goToPage5(void)
{
    /* VAI PRA PAGINA 5 */
    gtk_label_set_markup(GTK_LABEL(pag5_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\%s</span>",dataHora()));
    gtk_label_set_markup(GTK_LABEL(pag5_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\ASSENTO:</span>\n<span weight=\"bold\" foreground=\"red\" size=\"45000\">%s</span>",ultimaLugar));
    gtk_label_set_markup(GTK_LABEL(pag5_label3), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\SENHA DE ACESSO: </span><span weight=\"bold\" foreground=\"blue\" size=\"x-large\">\%s</span>",ultimaChave));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 5);
    return;
}

void goToPage6(void)
{
    /* VAI PRA PAGINA 6 */
    gtk_label_set_markup(GTK_LABEL(pag6_label1), g_markup_printf_escaped("<span weight=\"bold\" background=\"white\" foreground=\"purple\" size=\"xx-large\">%s</span>",gtk_label_get_text(GTK_LABEL(pag6_label1))));
    gtk_label_set_markup(GTK_LABEL(pag6_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 6);
    return;
}

void goToPage7(void)
{
    /* VAI PRA PAGINA 7 */
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 7);
    return;
}

void goToPage8(void)
{
    /* VAI PRA PAGINA 8 */
    gtk_entry_set_text(GTK_ENTRY(pag8_input1),Db.HOST);
    gtk_entry_set_text(GTK_ENTRY(pag8_input2),Db.PORT);
    gtk_entry_set_text(GTK_ENTRY(pag8_input3),Db.DATB);
    gtk_entry_set_text(GTK_ENTRY(pag8_input4),Db.USER);
    gtk_entry_set_text(GTK_ENTRY(pag8_input5),Db.PASS);
    gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 8);
    return;
}

void goToPage9(void)
{
    /* VAI PRA PAGINA 9 */
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 9);
    return;
}

void goToPage10(void)
{
    /* VAI PRA PAGINA 10 */
    if((getAssentosComOk(&Audi) + getAssentosPcdOk(&Audi)) == 0)
    {
        return;
    }
    srand(time(0));
    char assento[5];
    char nome[60];
    char tele[12];
    int cont = 0;
    int ganhador = 1 + rand() % (getAssentosComOk(&Audi) + getAssentosPcdOk(&Audi));
    for(int row = QTD_RES_CON_FIL; row < QTD_FILEIRA; row++)
    {
        for(int col = 0; col < QTD_COLUNA; col++)
        {
            cont++;
            if(cont == ganhador)
            {
                strcpy(assento,Audi.lugar[row][col].TEXT);
                break;
            }
        }
    }
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[60];
            sprintf(query,"SELECT name, tel FROM user WHERE local = '%s'",assento);
            qstate = mysql_query(conn,query);
            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        strcpy(nome,row[0]);
                        strcpy(tele,row[1]);
                    }
                    else
                    {
                        mysql_close(conn);
                        return g_print("%s",mysql_error(conn));
                    }
                }
            }
            else
            {
                g_print("%s",mysql_error(conn));
                mysql_close(conn);
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
    char str1[] = "<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\NOME: </span>\n<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">\%s</span>\n";
    char str2[] = "<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\TEL.: </span>\n<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">\%s</span>\n";
    gtk_label_set_markup(GTK_LABEL(pag10_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\%s</span>",dataHora()));
    gtk_label_set_markup(GTK_LABEL(pag10_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"black\" size=\"x-large\">\ASSENTO:</span>\n<span weight=\"bold\" foreground=\"red\" size=\"45000\">%s</span>",assento));
    gtk_label_set_markup(GTK_LABEL(pag10_label3), g_markup_printf_escaped(str1,nome));
    gtk_label_set_markup(GTK_LABEL(pag10_label4), g_markup_printf_escaped(str2,tele));
    gtk_assistant_set_current_page(GTK_ASSISTANT(assistant), 10);
    return;
}

void cadConvidado(void)
{
    /* CADASTRA USUARIO NA BASE-DE-DADOS */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    char query[250];
    strcpy(query,"");
    char NOME[60];
    int  TIPO = 1;
    char TELE[12];
    char MAIL[60];
    char DOCU[12];
    char key[6];
    strcpy(NOME, gtk_entry_get_text(GTK_ENTRY(pag3_input1)));
    strcpy(TELE, gtk_entry_get_text(GTK_ENTRY(pag3_input2)));
    strcpy(MAIL, gtk_entry_get_text(GTK_ENTRY(pag3_input3)));
    strcpy(DOCU, gtk_entry_get_text(GTK_ENTRY(pag3_input4)));
    TIPO = 1;
    if(validarNome(NOME) == 0 && validarTele(TELE) == 0 && validarMail(MAIL) == 0 && validarDocu(DOCU) == 0)
    {
        if(conn = mysql_init(0))
        {
            if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
            {
                if(consultaDocDb(DOCU) == 1)
                {
                    return gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">ERRO! DOCUMENTO JA CADASTRADA.</span>"));
                }
                else
                {
                    do
                    {
                        srand(time(NULL));
                        sprintf(key,"%d",10000 + rand() % 99999);
                        strcpy(ultimaChave,key);
                    }
                    while(consultaKeyDb(key) == 1);
                }
                if(consultaKeyDb(key) == 0)
                {
                    strcpy(ultimaLugar,addNewUser(&Audi, TIPO));
                    sprintf(query, "INSERT INTO user VALUES (default, %d, '%s', '%s', '%s', '%s', '%s', '%s')",
                            TIPO,
                            NOME,
                            DOCU,
                            TELE,
                            MAIL,
                            key,
                            ultimaLugar);
                }
                qstate = mysql_query(conn,query);
                sprintf(query,"UPDATE invite SET invitStatus= 'TRUE' WHERE invitKey = '%s'",gtk_entry_get_text(GTK_ENTRY(pag4_input1)));
                qstate = mysql_query(conn,query);
                if(!qstate)
                {
                    goToPage5();
                    mysql_close(conn);
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
}

void cadConviProgressBar(void)
{
    /* DEFINE A % DA BARRA DE PROGRESSO */
    char NOME[60];
    char TELE[12];
    char MAIL[60];
    char DOCU[12];
    int state[4] = {FALSE,FALSE,FALSE,FALSE};
    int i, progress = 0;
    strcpy(NOME, gtk_entry_get_text(GTK_ENTRY(pag3_input1)));
    strcpy(TELE, gtk_entry_get_text(GTK_ENTRY(pag3_input2)));
    strcpy(MAIL, gtk_entry_get_text(GTK_ENTRY(pag3_input3)));
    strcpy(DOCU, gtk_entry_get_text(GTK_ENTRY(pag3_input4)));
    // VERIFICA A ENTRADA DO NOME, SE É VALIDA.
    if(validarNome(NOME) == 0)
    {
        state[0] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA A ENTRADA DO TELEFONE, SE É VALIDA.
    if(validarTele(TELE) == 0)
    {
        state[1] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA A ENTRADA DO E-MAIL, SE É VALIDA.
    if(validarMail(MAIL) == 0)
    {
        state[2] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA A ENTRADA DO DOCUMENTO, SE É VALIDA.
    if(validarDocu(DOCU) == 0)
    {
        state[3] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // FINALMENTE VERIFICA QUANTOS CAMPOS SAO VALIDOS E ATUALIZA A BARRA DE PROGRESSO.
    for(int i = 0; i < 4; i++)
    {
        if(state[i] == TRUE)
        {
            progress += 25;
            gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
        }
    }
    _progressBar(pag3_pbar1, progress, 0);
    if(progress == 100)
    {
        gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\">TUDO OK, CLIQUE EM PROSSEGUIR!</span>"));
    }
    else
    {
        if(state[0] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM NOME VALIDO!</span>"));
        }
        else if(state[1] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM TELEFONE VALIDO!</span>"));
        }
        else if(state[2] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM E-MAIL VALIDO!</span>"));
        }
        else if(state[3] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag3_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM DOCUMENTO VALIDO!</span>"));
        }
    }
}

void cadParticipante(void)
{
    /* CADASTRA USUARIO NA BASE-DE-DADOS */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    char query[250];
    strcpy(query,"");
    char NOME[60];
    int  TIPO = 1;
    char TELE[12];
    char MAIL[60];
    char DOCU[12];
    char key[6];
    strcpy(NOME, gtk_entry_get_text(GTK_ENTRY(pag2_input1)));
    strcpy(TELE, gtk_entry_get_text(GTK_ENTRY(pag2_input2)));
    strcpy(MAIL, gtk_entry_get_text(GTK_ENTRY(pag2_input3)));
    strcpy(DOCU, gtk_entry_get_text(GTK_ENTRY(pag2_input4)));
    if(gtk_toggle_button_get_active(GTK_RADIO_BUTTON(pag2_rbtn2)))
    {
        TIPO = 3;
    }
    else
    {
        TIPO = 2;
    }
    if(validarNome(NOME) == 0 && validarTele(TELE) == 0 && validarMail(MAIL) == 0 && validarDocu(DOCU) == 0)
    {
        if(conn = mysql_init(0))
        {
            if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
            {
                if(consultaDocDb(DOCU) == 1)
                {
                    return gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">ERRO! DOCUMENTO JA CADASTRADA.</span>"));
                }
                else
                {
                    do
                    {
                        srand(time(NULL));
                        sprintf(key,"%d",10000 + rand() % 99999);
                        strcpy(ultimaChave,key);
                    }
                    while(consultaKeyDb(key) == 1);
                }
                if(consultaKeyDb(key) == 0)
                {
                    strcpy(ultimaLugar,addNewUser(&Audi, TIPO));
                    sprintf(query, "INSERT INTO user VALUES (default, %d, '%s', '%s', '%s', '%s', '%s', '%s')",
                            TIPO,
                            NOME,
                            DOCU,
                            TELE,
                            MAIL,
                            key,
                            ultimaLugar);
                }
                qstate = mysql_query(conn,query);
                if(!qstate)
                {
                    goToPage5();
                    mysql_close(conn);
                }
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }
    }
}

void cadPartiProgressBar(void)
{
    /* DEFINE A % DA BARRA DE PROGRESSO */
    char NOME[60];
    char TELE[12];
    char MAIL[60];
    char DOCU[12];
    int state[5] = {FALSE,FALSE,FALSE,FALSE,FALSE};
    int i, progress = 0;
    strcpy(NOME, gtk_entry_get_text(GTK_ENTRY(pag2_input1)));
    strcpy(TELE, gtk_entry_get_text(GTK_ENTRY(pag2_input2)));
    strcpy(MAIL, gtk_entry_get_text(GTK_ENTRY(pag2_input3)));
    strcpy(DOCU, gtk_entry_get_text(GTK_ENTRY(pag2_input4)));
    // VERIFICA A ENTRADA DO NOME, SE É VALIDA.
    if(validarNome(NOME) == 0)
    {
        state[0] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA ESTADO DOS BUTOES - PCD
    if(gtk_toggle_button_get_active(GTK_RADIO_BUTTON(pag2_rbtn1)) || gtk_toggle_button_get_active(GTK_RADIO_BUTTON(pag2_rbtn2)))
    {
        state[1] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA A ENTRADA DO TELEFONE, SE É VALIDA.
    if(validarTele(TELE) == 0)
    {
        state[2] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA A ENTRADA DO E-MAIL, SE É VALIDA.
    if(validarMail(MAIL) == 0)
    {
        state[3] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // VERIFICA A ENTRADA DO DOCUMENTO, SE É VALIDA.
    if(validarDocu(DOCU) == 0)
    {
        state[4] = TRUE;
        gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
    }
    // FINALMENTE VERIFICA QUANTOS CAMPOS SAO VALIDOS E ATUALIZA A BARRA DE PROGRESSO.
    for(int i = 0; i < 5; i++)
    {
        if(state[i] == TRUE)
        {
            progress += 20;
            gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\"> </span>"));
        }
    }
    _progressBar(pag2_pbar1, progress, 0);
    if(progress == 100)
    {
        gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\">TUDO OK, CLIQUE EM PROSSEGUIR!</span>"));
    }
    else
    {
        if(state[0] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM NOME VALIDO!</span>"));
        }
        else if(state[1] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DESCOLHA SIM OU NAO!</span>"));
        }
        else if(state[2] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM TELEFONE VALIDO!</span>"));
        }
        else if(state[3] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM E-MAIL VALIDO!</span>"));
        }
        else if(state[4] == FALSE)
        {
            gtk_label_set_markup(GTK_LABEL(pag2_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"blue\" size=\"x-large\">DIGITE UM DOCUMENTO VALIDO!</span>"));
        }
    }
}

int getAssentosTtl(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE VAGAS PARA O AUDITORIO */
    return audi->_TTL;
}

int getAssentosCon(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE VAGAS PARA PARTICIPANTES CONVIDADOS */
    return audi->_CON;
}

int getAssentosPcd(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE VAGAS PARA PARTICIPANTES COM DEFICIENCIA */
    return audi->_PCD;
}

int getAssentosCom(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE VAGAS PARA PARTICIPANTES */
    return audi->_COM;
}

int getAssentosConOk(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE PARTICIPANTES CONVIDADOS CADASTRADOS NO EVENTO */
    return audi->_CON_OK;
}

int getAssentosPcdOk(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE PARTICIPANTES COM DEFICIECIA CADASTRADOS NO EVENTO */
    return audi->_PCD_OK;
}

int getAssentosComOk(AUDITORIO *audi)
{
    /* PEGA O TOTAL DE PARTICIPANTES CADASTRADOS NO EVENTO */
    return audi->_COM_OK;
}

int setAssentosConOk(AUDITORIO *audi, int num)
{
    if(audi->_CON_OK = num)
    {
        int cont = 0;
        for(int i=0; i<QTD_FILEIRA; i++)
        {
            for(int j=0; j<QTD_COLUNA; j++)
            {
                if(cont < num)
                {
                    audi->lugar[i][j].USED = TRUE;
                }
                cont++;
            }
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

int setAssentosPcdOk(AUDITORIO *audi, int num)
{
    if(audi->_PCD_OK = num)
    {
        int cont = 0;
        for(int i=0; i<QTD_FILEIRA; i++)
        {
            for(int j=0; j<QTD_COLUNA; j++)
            {
                if(cont < num)
                {
                    audi->lugar[i][j].USED = TRUE;
                }
                cont++;
            }
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

int setAssentosComOk(AUDITORIO *audi, int num)
{
    if(audi->_COM_OK = num)
    {
        int cont = 0;
        for(int i=0; i<QTD_FILEIRA; i++)
        {
            for(int j=0; j<QTD_COLUNA; j++)
            {
                if(cont < num)
                {
                    audi->lugar[i][j].USED = TRUE;
                }
                cont++;
            }
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

int getAuditorioBackup(AUDITORIO *audi)
{
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    int cont, con = 0, pcd = 0, com = 0;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            strcpy(stderr,"SELECT type FROM user;");
            qstate = mysql_query(conn,stderr);
            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(strcmp(row[0],"1") == 0)
                    {
                        con++;
                    }
                    else if(strcmp(row[0],"2") == 0)
                    {
                        pcd++;
                    }
                    else
                    {
                        com++;
                    }
                }
                mysql_close(conn);
            }
            else
            {
                g_print("Erro: %s\n",mysql_error(conn));
            }
        }
        else
        {
            g_print("Erro: %s\n",mysql_error(conn));
        }
    }
    else
    {
        g_print("Erro: %s\n",mysql_error(conn));
    }
    setAssentosConOk(&Audi, con);
    setAssentosPcdOk(&Audi, pcd);
    setAssentosComOk(&Audi, com);
    cont = 0;
    for(int row=0;row<QTD_RES_CON_FIL;row++)
    {
        for(int col=0;col<QTD_COLUNA;col++)
        {
            if(cont < con)
            {
                audi->lugar[row][col].USED = TRUE;
            }
            else
            {
                audi->lugar[row][col].USED = FALSE;
            }
            cont++;
        }
    }
    cont = 0;
    for(int row=QTD_RES_CON_FIL;row<QTD_FILEIRA;row++)
    {
        for(int col=0;col<QTD_COLUNA;col++)
        {
            if(cont < (pcd + com))
            {
                audi->lugar[row][col].USED = TRUE;
            }
            else
            {
                audi->lugar[row][col].USED = FALSE;
            }
            cont++;
        }
    }
    return 0;
}

int getConfigDb(BANCODEDADOS *db)
{
    /* PEGA AS CONFIGURAÇÕES DE CONEXÃO AO BANCO DE DADOS */
    FILE *sysconfig;
    sysconfig = fopen(SYSCONFIG_FILE,"r");
    if(sysconfig == NULL)
    {
        return 1;
    }
    fscanf(sysconfig,"%s",db->HOST);
    fscanf(sysconfig,"\n%s",db->PORT);
    fscanf(sysconfig,"\n\n%s",db->DATB);
    fscanf(sysconfig,"\n\n\n%s",db->USER);
    fscanf(sysconfig,"\n\n\n\n%s",db->PASS);
    if(strcmp(db->PASS,"#null#") == 0)
    {
        strcpy(db->PASS,"");
    }
    fclose(sysconfig);
    return 0;
}

void configDb_clicked(void)
{
    /* PEGA O TOTAL DE VAGAS PARA PARTICIPANTES */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    FILE *sysconfig;
    _progressBar(pag8_pbar1, 10, 100);
    char texto[80];
    char HOST[60];
    char PORT[60];
    char DATB[60];
    char USER[60];
    char PASS[60];
    strcpy(HOST,gtk_entry_get_text(GTK_ENTRY(pag8_input1)));
    strcpy(PORT,gtk_entry_get_text(GTK_ENTRY(pag8_input2)));
    strcpy(DATB,gtk_entry_get_text(GTK_ENTRY(pag8_input3)));
    strcpy(USER,gtk_entry_get_text(GTK_ENTRY(pag8_input4)));
    strcpy(PASS,gtk_entry_get_text(GTK_ENTRY(pag8_input5)));
    _progressBar(pag8_pbar1, 50, 100);
    if(strcmp(HOST,"") != 0 && strcmp(DATB,"") != 0 && strcmp(USER,"") != 0 && strcmp(PORT,"") != 0)
    {
        if(conn = mysql_init(0))
        {
            _progressBar(pag8_pbar1, 60, 100);
            if(conn = mysql_real_connect(conn,HOST,USER,PASS,DATB,atoi(PORT),NULL,0))
            {
                mysql_close(conn);
                _progressBar(pag8_pbar1, 75, 100);
                sysconfig = fopen(SYSCONFIG_FILE,"w+");
                fprintf(sysconfig, "%s", HOST);
                fprintf(sysconfig,"\n");
                fprintf(sysconfig, "%s", PORT);
                fprintf(sysconfig,"\n");
                fprintf(sysconfig, "%s", DATB);
                fprintf(sysconfig,"\n");
                fprintf(sysconfig, "%s", USER);
                fprintf(sysconfig,"\n");
                if(strcmp(PASS,"") == 0)
                {
                    strcpy(PASS,"#null#");
                }
                fprintf(sysconfig, "%s", PASS);
                fprintf(sysconfig,"\n");
                fclose(sysconfig);
                gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\">CONFIGURADO COM SUCESSO!</span>"));
                _progressBar(pag8_pbar1, 100, 100);
            }
            else
            {
                gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">FALHA AO TENTAR ESTABELECER CONEXAO!</span>"));
                _progressBar(pag8_pbar1, 0, 100);
            }
        }
    }
    else
    {
        gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"orange\" size=\"x-large\">OS CAMPOS SINALIZADOS COM (*) SAO OBRIGATORIOS.</span>"));
        _progressBar(pag8_pbar1, 0, 100);
    }
    _progressBar(pag8_pbar1, 0, 100);
    getConfigDb(&Db);
    return;
}

void testarDb_clicked(void)
{
    /* TESTA A CONEXÃO COM A BASE DE DADOS */
    _progressBar(pag8_pbar1, 25, 200);
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    char texto[80];
    char HOST[60];
    char PORT[60];
    char DATB[60];
    char USER[60];
    char PASS[60];
    strcpy(HOST,gtk_entry_get_text(GTK_ENTRY(pag8_input1)));
    strcpy(PORT,gtk_entry_get_text(GTK_ENTRY(pag8_input2)));
    strcpy(DATB,gtk_entry_get_text(GTK_ENTRY(pag8_input3)));
    strcpy(USER,gtk_entry_get_text(GTK_ENTRY(pag8_input4)));
    strcpy(PASS,gtk_entry_get_text(GTK_ENTRY(pag8_input5)));
    _progressBar(pag8_pbar1, 50, 100);
    if(strcmp(HOST,"") != 0 && strcmp(DATB,"") != 0 && strcmp(USER,"") != 0 && strcmp(PORT,"") != 0)
    {
        _progressBar(pag8_pbar1, 50, 200);
        if(conn = mysql_init(0))
        {
            _progressBar(pag8_pbar1, 75, 200);
            if(conn = mysql_real_connect(conn,HOST,USER,PASS,DATB,atoi(PORT),NULL,0))
            {
                _progressBar(pag8_pbar1, 100, 200);
                gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\">CONEXAO ESTABELECIDA COM SUCESSO!</span>"));
                _progressBar(pag8_pbar1, 0, 200);
                mysql_close(conn);
            }
            else
            {
                gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">FALHA AO TENTAR ESTABELECER CONEXAO!</span>"));
                _progressBar(pag8_pbar1, 0, 200);
            }
        }
        else
        {
            gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">FALHA AO TENTAR CRIAR OBJETO DE CONEXAO!</span>"));
            _progressBar(pag8_pbar1, 0, 200);
        }
    }
    else
    {
        gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"orange\" size=\"x-large\">OS CAMPOS SINALIZADOS COM (*) SAO OBRIGATORIOS.</span>"));
        _progressBar(pag8_pbar1, 0, 200);
    }
}

void resetaDb_clicked(void)
{
    /* ATUALIZA OS CAMPOS COM AS CONFIGURAÇÕES DE CONEXÃO COM O BANCO DE DADOS */
    gtk_entry_set_text(GTK_ENTRY(pag8_input1),Db.HOST);
    gtk_entry_set_text(GTK_ENTRY(pag8_input2),Db.PORT);
    gtk_entry_set_text(GTK_ENTRY(pag8_input3),Db.DATB);
    gtk_entry_set_text(GTK_ENTRY(pag8_input4),Db.USER);
    gtk_entry_set_text(GTK_ENTRY(pag8_input5),Db.PASS);
    gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
}

void limparDb_clicked(void)
{
    /* LIMPA TODOS OS CAMPOS PARA A CONFIGURAÇÃO DO BANCO DE DADOS */
    gtk_entry_set_text(GTK_ENTRY(pag8_input1),"");
    gtk_entry_set_text(GTK_ENTRY(pag8_input2),"");
    gtk_entry_set_text(GTK_ENTRY(pag8_input3),"");
    gtk_entry_set_text(GTK_ENTRY(pag8_input4),"");
    gtk_entry_set_text(GTK_ENTRY(pag8_input5),"");
    gtk_label_set_markup(GTK_LABEL(pag8_label1), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"green\" size=\"x-large\"> </span>"));
}

int consultaDocDb(char *cpf)
{
    /* VERIFICA SE O DOCUMENTO FORNECIDO PELO USUARIO JA EXISTE NA BASE-DE-DADOS */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[60];
            sprintf(query,"SELECT iduser FROM user WHERE doc = '%s'",cpf);
            qstate = mysql_query(conn,query);

            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        mysql_close(conn);
                        return 1;
                    }
                    else
                    {
                        mysql_close(conn);
                        return 0;
                    }
                }
            }
            else
            {
                mysql_close(conn);
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int consultaInviteKeyDb(char *key)
{
    /* VERIFICA A CHAVE DE ACESSO GERADA PELO SISTEMA JA EXISTE NA BASE-DE-DADOS */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[60];
            sprintf(query,"SELECT idinvite FROM invite WHERE invitKey = '%s'",key);
            qstate = mysql_query(conn,query);

            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        mysql_close(conn);
                        return 1;
                    }
                    else
                    {
                        mysql_close(conn);
                        return 0;
                    }
                }
            }
            else
            {
                g_print("%s",mysql_error(conn));
                mysql_close(conn);
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

int consultaKeyDb(char *key)
{
    /* VERIFICA A CHAVE DE ACESSO GERADA PELO SISTEMA JA EXISTE NA BASE-DE-DADOS */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;

    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[60];
            sprintf(query,"SELECT iduser FROM user WHERE acessKey = '%s'",key);
            qstate = mysql_query(conn,query);

            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        mysql_close(conn);
                        return 1;
                    }
                    else
                    {
                        mysql_close(conn);
                        return 0;
                    }
                }
            }
            else
            {
                g_print("%s",mysql_error(conn));
                mysql_close(conn);
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

void adminExit(void)
{
    /* VALIDA AS INFORMAÇÕES PARA SAIR DO PROGRAMA */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[60];
            sprintf(query,"SELECT idadmin FROM admin WHERE admin = '%s' AND pass = '%s'",
                    gtk_entry_get_text(GTK_ENTRY(pag6_input1)),
                    gtk_entry_get_text(GTK_ENTRY(pag6_input2)));
            qstate = mysql_query(conn,query);
            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        mysql_close(conn);
                        return _quit();
                    }
                }
                mysql_close(conn);
                return gtk_label_set_markup(GTK_LABEL(pag6_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">LOGIN OU SENHA INVALIDO, OU INEXISTENTE!</span>"));
            }
            else
            {
                mysql_close(conn);
                return;
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

void adminLogin(void)
{
    /* VALIDA AS INFORMAÇÕES PARA ACESSO DO PAINEL ADMINISTRATIVO */
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;
    int qstate;
    if(conn = mysql_init(0))
    {
        if(conn = mysql_real_connect(conn,Db.HOST,Db.USER,Db.PASS,Db.DATB,atoi(Db.PORT),NULL,0))
        {
            char query[60];
            sprintf(query,"SELECT idadmin FROM admin WHERE admin = '%s' AND pass = '%s'",
                    gtk_entry_get_text(GTK_ENTRY(pag6_input1)),
                    gtk_entry_get_text(GTK_ENTRY(pag6_input2)));
            qstate = mysql_query(conn,query);
            if(!qstate)
            {
                res = mysql_store_result(conn);
                while(row = mysql_fetch_row(res))
                {
                    if(row[0])
                    {
                        mysql_close(conn);
                        return goToPage7();
                    }
                }
                mysql_close(conn);
                return gtk_label_set_markup(GTK_LABEL(pag6_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">LOGIN OU SENHA INVALIDO, OU INEXISTENTE!</span>"));
            }
            else
            {
                mysql_close(conn);
                return gtk_label_set_markup(GTK_LABEL(pag6_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">%s</span>",mysql_error(conn)));
            }
        }
        else
        {
            return gtk_label_set_markup(GTK_LABEL(pag6_label2), g_markup_printf_escaped("<span weight=\"bold\" foreground=\"red\" size=\"x-large\">%s</span>",mysql_error(conn)));
        }
    }
    else
    {
        return gtk_label_set_text(GTK_LABEL(pag6_label2),mysql_error(conn));
    }
}

int validarNome(char *nome)
{
    /* VALIDA UMA STRING NOME */
    int i;
    if(strlen(nome) >= 3)
    {
        for(int i=0; i<strlen(nome); i++)
        {
            if(nome[i] == ' ')
            {
                continue;
            }
            if(!isalpha(nome[i]))
            {
                return 1;
            }
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

int validarTele(char *tel)
{
    /* VALIDA TELEFONE DE CONTATO */
    int i;
    if(strlen(tel) == 11)
    {
        for(int i=0; i<strlen(tel); i++)
        {
            if(!isdigit(tel[i]))
            {
                return 1;
            }
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

int validarMail(char *mail)
{
    /* VALIDA UMA STRING DE E-MAIL */
    int i, arrouba, ponto;
    if(strlen(mail) >= 5)
    {
        for(int i=0; i<strlen(mail); i++)
        {
            if(mail[i] == '@')
            {
                arrouba = TRUE;
            }
            if(mail[i]=='.' && arrouba==TRUE)
            {
                ponto = TRUE;
            }
        }
        if(arrouba==TRUE && ponto==TRUE)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}

int validarDocu(char *doc)
{
    /* VALIDA UMA STRING DE DOCUMNETO */
    int i, ehnum, ehletra;
    if(strlen(doc) == 11)
    {
        for(i = 0; i < strlen(doc) ; i++)
        {
            if(!isdigit(doc[i]))
            {
                return 1;
            }
        }
        return 0;
    }
    else if(strlen(doc) == 8)
    {
        for(i = 0; i < strlen(doc) ; i++)
        {
            if(!isdigit(doc[i]))
            {
                return 1;
            }
        }
        return 0;
    }
    else if(strlen(doc) == 7)
    {
        for(i = 0; i < strlen(doc) ; i++)
        {
            if(!isalnum(doc[i]))
            {
                return 1;
            }
            else
            {
                if(isdigit(doc[i]))
                {
                    ehnum = TRUE;
                }
                if(isalpha(doc[i]))
                {
                    ehletra = TRUE;
                    doc[i] = toupper(doc[i]);
                }
            }
        }
        if(ehnum==TRUE && ehletra==TRUE)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
}
