#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>

using namespace std;

//AÇÕES POSSÍVEIS
enum Acao { Atacar, Defender, Parado, Aranha};
enum Habilidade {Vida, Ataque, Defesa, Sair};
enum Identificar { Pers, Aran, Mago};

//UPGRADES
int _maestria = -1; //PONTOS PARA O RESET
int _andar = 0;

int _vida = 1;
int _ataque = 1;
int _defesa = 1;
int _andarMago = 24;
bool _batalhaMago = false;

//ARANHA
Acao acaoAranha;
int _corAranha;
int _mod = 2; //MUDA QUANDO O PERSONAGEM ATACA A ARANHA

struct Status
{
    int vidaMaxima;
    int vidaAtual;
    int ataque;
    int defesa;
    int dano; //dano recebido de volta
};

//PRINTAR PERSONAGENS
void printPersonagem(int, int, int);
void printAranha(int, int, bool, bool);
void printMago(int, int, bool);

//PRINTAR TELA DE UPGRADE
void printVida(int, int, bool);
void printEscudo(int, int, bool);
void printEspada(int, int, bool);
void tutorialUpgrade(int, int, Habilidade);

//FUNCOES BASICAS DE FUNCIONAMENTO
void configuracoes(void);
void clear(void);
void hideCursor();
void gotoxy(int,int);
void color(int);

void aranhaParada(void);
void aranhaAtacando(void);
void printStatus(int, int, Status*);
void upgradeAranha(Status*);

void printQuadro(int x, int y)
{
    gotoxy(x, y + 0); printf("┌─────────────────────────┐");
    gotoxy(x, y + 1); printf("│                         │");
    gotoxy(x, y + 2); printf("└─────────────────────────┘");
}

void printAcao(int x, int y, Acao acao)
{
    int cor = 6; //COR DA AÇÃO
    int corOff = 15; //COR DA AÇÃO NÃO SELECIONADA [BRANCO]

    if (acao == Aranha)
    {
        //AÇÃO ALEATORIA DA ARANHA
        switch (rand() % _mod) //ARANHA ATACA MAIS QUANDO É ATACADA
        {
            case 0: //DEFENDER
                acao = Acao::Defender;
                acaoAranha = Acao::Defender;
                aranhaParada();
                break;
            case 1: 
            case 2:
                acao = Acao::Atacar;
                acaoAranha = Acao::Atacar;
                //printAranha(42, 2, false, true);
                aranhaAtacando();
                break;
        }
    }

    if (acao == Parado)
    {
        cor = corOff;
        printPersonagem(4, 0, 8);
    }
    else
    {
        if (acao == Defender)
        {
            int aux = cor;
            cor = corOff;
            corOff = aux;
        }
    }
    
    color(cor);
    printQuadro(x, y);
    gotoxy(x + 1, y + 1);
    printf("       A T A C A R       ");

    color(corOff);
    printQuadro(x, y + 3);
    gotoxy(x + 1, y + 4);
    printf("     D E F E N D E R     ");
}

void printMaestria(int cor)
{
    int x = 48;
    int y = 1;

    color(cor); //6
    gotoxy(x, y + 0); printf("┌────────────────────┐");
    gotoxy(x, y + 1); printf("│                    │");
    gotoxy(x, y + 2); printf("│  M A E S T R I A   │");
    gotoxy(x, y + 3); printf("│                    │");
    gotoxy(x, y + 4); printf("│                    │");
    gotoxy(x, y + 5); printf("│                    │");
    gotoxy(x, y + 6); printf("└────────────────────┘");
    gotoxy(x + 10, y + 4); printf("%d", _maestria);
}

void printHabilidade(Status *personagem, Habilidade selecionado, int cor)
{
    int x = 25;
    int y = 18;
    
    color(cor);
    gotoxy(x, y + 0); printf("┌─────────────────┐");
    gotoxy(x, y + 2); printf("│─────────────────│");
    gotoxy(x, y + 3); printf("│    A T U A L    │");
    gotoxy(x, y + 4); printf("│                 │");
    gotoxy(x, y + 5); printf("│       ~•~       │");
    gotoxy(x, y + 6); printf("│  P R Ó X I M O  │");
    gotoxy(x, y + 7); printf("│                 │");
    gotoxy(x, y + 8); printf("└─────────────────┘");
    
    if (selecionado == Vida)
    {
        gotoxy(x, y + 1); printf("│     V I D A     │"); //w ; w+w
        gotoxy(x + 9, y + 4); printf("%d", (*personagem).vidaMaxima);
        gotoxy(x + 9, y + 7); printf("%d", ((4 * (_vida + 1)) + 6));
    }
    else if (selecionado == Ataque)
    {
        gotoxy(x, y + 1); printf("│   A T A Q U E   │"); //a ; a+a
        gotoxy(x + 9, y + 4); printf("%d", (*personagem).ataque);
        gotoxy(x + 9, y + 7); printf("%d", (2 * (_ataque + 1)));
    }
    else if (selecionado == Defesa)
    {
        gotoxy(x, y + 1); printf("│   D E F E S A   │"); //d ; d+d
        gotoxy(x + 9, y + 4); printf("%d", (*personagem).defesa);
        gotoxy(x + 9, y + 7); printf("%d", ((_defesa + 1)));
    }
}

void resetPersonagem(Status *personagem)
{
    (*personagem).vidaMaxima = (4 * _vida) + 6;
    (*personagem).vidaAtual = (*personagem).vidaMaxima;
    (*personagem).ataque = (2 * _ataque);
    (*personagem).defesa = (1 * _defesa);
    _batalhaMago = false;
}

void printUpgrade(bool vida, bool espada, bool escudo)
{
    printVida(26, 4, vida);
    printEspada(2, 1, espada);
    printEscudo(48, 10, escudo);
}

void upgradePersonagem(Status *personagem, Status *aranha)
{
    Habilidade selecionado = Habilidade::Vida;
    char comando;
    int cor = 2;

    bool sair = false;
    
    clear();
    printUpgrade(true, false, false);

    do
    {
        tutorialUpgrade(10, 29, selecionado);
        resetPersonagem(personagem);
        printHabilidade(personagem, selecionado, cor);

        if (cor == 0)
        {   
            printMaestria(cor);
        }
        else
        {
            printMaestria(6);
        }

        comando = getch();
        clear();

        if (comando == 'w' || comando == 'W')
        {
            if (selecionado == Vida)
            {
                if (_maestria >= _vida )
                {
                    _maestria -= _vida;
                    _vida++;
                }
            }

            selecionado = Habilidade::Vida;
            printUpgrade(true, false, false);

            sair = false;
            cor = 2;
        }
        else if (comando == 'a' || comando == 'A')
        {
            if (selecionado == Ataque)
            {
                if (_maestria >= _ataque )
                {
                    _maestria -= _ataque;
                    _ataque++;
                }
            }

            selecionado = Habilidade::Ataque;
            printUpgrade(false, true, false);

            sair = false;
            cor = 3;
        }
        else if (comando == 'd' || comando == 'D')
        {
            if (selecionado == Defesa)
            {
                if (_maestria >= _defesa )
                {
                    _maestria -= _defesa;
                    _defesa++;
                }
            }

            selecionado = Habilidade::Defesa;
            printUpgrade(false, false, true);

            sair = false;
            cor = 12;
        }
        else if (comando == 's' || comando == 'S')
        {
            if (selecionado == Sair)
            {
                sair = true;
            }
            
            selecionado = Habilidade::Sair;
            printUpgrade(false, false, false);

            cor = 0;
        }

    } while (!sair);

    clear();
    //RESETANDO A TELA ---------------------------------------------------------
    Sleep(1000);    
    resetPersonagem(personagem);
    printStatus(1, 20, personagem);

    _andar = 0;
    upgradeAranha(aranha);
}

void upgradeAranha(Status *aranha)
{
    if (_andar == _andarMago) //BOSS FINAL
    {
        (*aranha).vidaMaxima = (*aranha).vidaAtual = (4 * _vida) + 10;
        (*aranha).ataque = 5 + _ataque;
        (*aranha).defesa = _defesa - 3;
        (*aranha).dano = 0;
        printMago(41, 0, false);
        aranhaParada();
        printStatus(43, 20, aranha);
        _batalhaMago = true;
    }
    else
    {
        (*aranha).vidaMaxima = (*aranha).vidaAtual = 5 + (_andar * 2);
        (*aranha).ataque = 1 + (_andar * 0.4);
        (*aranha).defesa = 1 + (_andar * 0.1);
        (*aranha).dano = 0;

        _corAranha = (rand() % 15) + 1;
        aranhaParada();
        printStatus(43, 20, aranha);
    }

    _maestria++;
    _andar++;
}

void printStatus(int x, int y, Status *status)
{
    int vida = 4;
    int vidaPerdida = 8;

    color(vida);
    gotoxy(x, y + 0); printf("┌─────────────────────────┐");
    gotoxy(x, y + 1); printf("│         V I D A         │");
    gotoxy(x, y + 2); printf("│   ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥ ♥   │");
    gotoxy(x, y + 3); printf("└─────────────────────────┘");

    if ((*status).dano < 1)
    {
        (*status).dano = 0;
    }
    
    (*status).vidaAtual -= (*status).dano;
    (*status).dano = 0;
    int perdeu;

    if ((*status).vidaAtual < (*status).vidaMaxima && (*status).vidaAtual > 0)
    {
        perdeu = (10 - ((*status).vidaAtual * 10) / (*status).vidaMaxima);

        color(vidaPerdida);
        for (int i = 22, j = 1; j < perdeu && i != 2; i -= 2, j++)
        {
            gotoxy(x + i, y + 2);
            printf("♥");
        }
    }
} 

void estaVivo(Status *status, Identificar identificar, Status *aranha)
{
    if ((*status).vidaAtual <= 0)
    {
        if (identificar == Pers)
        {
            upgradePersonagem(status, aranha);
        }
        else if (identificar == Aran)
        {
            upgradeAranha(status);
        }
        else if (identificar == Mago)
        {
            _batalhaMago = false;
            _andar++;
            upgradeAranha(status);
        }
    }
}

//LÓGICA DO GAME
void rodada(Acao acaoPersonagem, Status *personagem, Status *aranha)
{
    if (acaoPersonagem == Atacar)
    {
        if (acaoAranha == Atacar)
        {
            (*personagem).dano = (*aranha).ataque;
            (*aranha).dano = (*personagem).ataque;
        }
        else
        {
            (*aranha).dano = (*personagem).ataque - (*aranha).defesa;
        }
    }
    else
    {
        if (acaoAranha == Atacar)
        {
            (*personagem).dano = (*aranha).ataque - (*personagem).defesa;
        }
    }

    printStatus(1, 20, personagem);
    printStatus(43, 20, aranha);
    estaVivo(personagem, Identificar::Pers, aranha);

    if (_batalhaMago)
    {
        estaVivo(aranha, Identificar::Mago, aranha);
    }
    else
    {
        estaVivo(aranha, Identificar::Aran, aranha);
    }

    //testar atributos
    // gotoxy(44, 19); printf("                         ");
    // color(_corAranha); gotoxy(44, 19); printf("V: %d A: %d D: %d", (*aranha).vidaAtual, (*aranha).ataque, (*aranha).defesa);
}

//'ANIMAÇÕES' -------------------------------------------
void limparEspacoPersonagem()
{
    for (int y = 0; y < 20; y++)
    {
        for (int x = 2; x < 35; x++)
        {
            gotoxy(x, y);
            printf(" ");
        }
    }
}

void limparEspacoAranha()
{
    for (int y = 1; y < 18; y++)
    {
        for (int x = 47; x < 71; x++)
        {
            gotoxy(x, y);
            printf(" ");
        }
    }
}

void personagemParado()
{
    printAcao(1, 24, Acao::Parado);
}

void aranhaParada()
{
    if (_batalhaMago)
    {
        printMago(41, 0, false);
    }
    else
    {
        printAranha(42, 2, false, false);
    }
}

void aranhaAtacando()
{
    if (_batalhaMago)
    {
        printMago(41, 0, true);
    }
    else
    {
        printAranha(42, 2, false, true);
    }
}

void personagemAtacando()
{
    printPersonagem(7, 0, 8);

    if (_batalhaMago)
    {
        printMago(41, 0, false);
    }
    else
    {
        printAranha(42, 2, true, true);
    }

    Sleep(50);

    personagemParado();
    aranhaParada();
    _mod = 3;
}

void personagemDefendendo()
{
    printPersonagem(2, 0, 8);
    Sleep(50);
    personagemParado();
    _mod = 2;
}

void printAndar(int x, int y)
{
    color(7);
    gotoxy(x, y + 0); printf("┌───────────┐");
    gotoxy(x, y + 1); printf("│ A N D A R │");
    gotoxy(x, y + 2); printf("│           │");
    gotoxy(x, y + 3); printf("└───────────┘");

    if (_andar > 99)
    {
        x--;
    }
    
    gotoxy(x + 6, y + 2); printf("%d", _andar);
}

int main()
{
    configuracoes();

    //STATUS INICIAL PERSONAGENS
    Status personagem;
    personagem.vidaMaxima = personagem.vidaAtual = 10;
    personagem.ataque = 2;
    personagem.defesa = 1;
    personagem.dano = 0;

    //STATUS ARANHA
    Status aranha;
    upgradeAranha(&aranha);

    //FAZER A ENTRADA COM A HISTÓRIA

    personagemParado();
    printStatus(1, 20, &personagem);

    aranhaParada();
    printAcao(43, 24, Acao::Aranha);
    printStatus(43, 20, &aranha);
    
    char acao;

    do
    {
        printAndar(29, 20);

        acao = getch();

        switch(acao)
        {
            case 'A': //ATACAR
            case 'a':
                printAcao(1, 24, Acao::Atacar);
                rodada(Acao::Atacar, &personagem, &aranha); 
                personagemAtacando();
                printAcao(43, 24, Acao::Aranha);
                break;

            case 'D': //DEFENDER
            case 'd':
                printAcao(1, 24, Acao::Defender);
                rodada(Acao::Defender, &personagem, &aranha);
                personagemDefendendo();
                printAcao(43, 24, Acao::Aranha);
                break;
        }

    } while (acao != 27); //ENCERRA QUANDO APERTA ESC
    
    //getch();
    return 0;
}

//▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓

// Só tristeza :/
void printPersonagem(int x, int y, int cor)
{
    limparEspacoPersonagem();
    int cE = 9; //cor da espada
    int cC = 6; //cor do cabo da espada
    int cT = 14; //cor do tecido no capacete
    int cM = 7; //cor das mãos?
    int cP = 12; //cor da pedra

    color(cor);
    gotoxy(x, y); 
    color(cT); printf("       _,.");
    gotoxy(x, y + 1); 
    printf("     ,` -.)");
    gotoxy(x, y + 2); 
    printf("    ( _/"); color(cor); printf("-"); color(cT); printf("\\\\"); color(cor); printf("-._");
    gotoxy(x, y + 3); 
    color(cT); printf("   /,"); color(cor); printf("|`--._,-^|            "); color(cE); printf(","); color(cor);
    gotoxy(x, y + 4); 
    color(cT); printf("   \\_"); color(cor); printf("| |`-._/||          "); color(cE); printf(",'|"); color(cor);
    gotoxy(x, y + 5); 
    printf("     |  `-, / |         "); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 6); 
    printf("     |     || |        "); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 7); 
    printf("      `r-._||/   __   "); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 8); 
    printf(" ___,-<_     )`-/  `."); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 9); 
    printf("{   \\   `- -'   \\   "); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 10); 
    printf("|    |           |."); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 11); 
    printf("|    /           /"); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 12); 
    printf(" \\_/' \\         |"); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 13); 
    printf("  |    |   _,^-'"); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 14); 
    printf("  |    , ``  "); color(cC); printf("(\\"); color(cE); printf("/  /"); color(cor);
    gotoxy(x, y + 15); 
    printf("   \\,.->._    "); color(cC); printf("\\"); color(cP); printf(" ○"); color(cC); printf(" ~."); color(cor);
    gotoxy(x, y + 16); 
    printf("   (  /   `-."); color(cM); printf("_"); color(14); printf("//"); color(cC); printf("^`´"); color(cor);
    gotoxy(x, y + 17); 
    printf("    `Y-.____"); color(cM); printf("(__}"); color(cor);
    gotoxy(x, y + 18); 
    printf("     |     "); color(cM); printf("{__)");
    gotoxy(x, y + 19); 
    printf("           "); color(cC); printf("()");
    
}

void printAranha(int x, int y, bool animacao, bool ataque)
{
    limparEspacoAranha();
    int olhos = 6;

    color(15);

    if(ataque)
    {
        olhos = 4;
    }

    if (animacao)
    {
        gotoxy(x, y - 2); printf("               |       ");
        gotoxy(x, y - 1); printf("               |       ");
        gotoxy(x, y);     printf("               |       ");
        gotoxy(x, y + 1); printf("               |       ");
        gotoxy(x, y + 2); printf("               |       ");
        gotoxy(x, y + 3); printf("               |       ");
        gotoxy(x, y + 4); printf("               |       ");
        y += 2; //ABAIXAR A ARANHA
    }
    else
    {
        gotoxy(x, y - 2); printf("               (       ");
        gotoxy(x, y - 1); printf("                )       ");
        gotoxy(x, y);     printf("               (       ");
        gotoxy(x, y + 1); printf("                )       ");
        gotoxy(x, y + 2); printf("               (       ");
    }

    color(_corAranha);
    gotoxy(x, y + 3); printf("        /\\  .-\"\"\"-.  /\\");
    gotoxy(x, y + 4); printf("       //\\\\/  ,,,  \\//\\\\");
    gotoxy(x, y + 5); printf("       |/\\| ,;;;;;, |/\\\\|");
    gotoxy(x, y + 6); printf("       |/\\| ,;;;;;, |/\\|");
    gotoxy(x, y + 7); printf("      //  \\/   .   \\/  \\");
    gotoxy(x, y + 8); printf("     (| ,-_| \\ | / |_-, |)");
    gotoxy(x, y + 9); printf("       //`__\\.-.-./__`\\\\");
    gotoxy(x, y + 10); printf("      // /.-("); color(olhos); printf("() ()"); color(_corAranha); printf(")-.\\ \\\\");
    gotoxy(x, y + 11); printf("     (\\ |)   '---'   (| /)");
    gotoxy(x, y + 12); printf("      ` (|   ´   `   |) `");
    gotoxy(x, y + 13); printf("        \\)           (/");
}

void printMago(int x, int y, bool ataque)
{
    limparEspacoAranha();
    int cC = 1;
    int cP = 15;
    int cB = 8;
    int cM = 0;
    int cV = 14;
    int cS = 12; //kkk
    int cO = cP;

    if (ataque)
    {
        cM = 13;
        cO = 4;
    }

    color(cC);
    gotoxy(x, y + 0); printf("                    /\\");
    gotoxy(x, y + 1); printf("                   /○ \\");
    gotoxy(x, y + 2); printf("                  | ☼ °|");
    gotoxy(x, y + 3); printf("                --:'''':--");
    gotoxy(x, y + 4); printf("                  "); color(cP); printf(":"); color(cO); printf("`"); color(cP); printf("¿"); color(cO); printf("´"); color(cP); printf(" :");
    gotoxy(x, y + 5); printf("                  "); color(cC); printf("_"); color(cB); printf(":\"\":"); color(cP); printf("\\"); color(cC); printf("__"); color(cM);
    gotoxy(x, y + 6); printf(" '  '        "); color(cC); printf("___.' "); color(cB); printf("::: "); color(cC); printf("°  `."); color(cM);
    gotoxy(x, y + 7); printf(". *"); color(cV); printf(" ═════"); color(cP); printf("<<=)"); color(cC); printf("    ☼      \\  ○`:"); color(cM);
    gotoxy(x, y + 8); printf(" .  '      "); color(cC); printf("'-'-'\\_   °  /'._.'");
    gotoxy(x, y + 9); printf("                  \\"); color(cS); printf("====:"); color(cC); printf(". "); color(cP); printf("\"\""); color(cC);
    gotoxy(x, y + 10); printf("                 .'☼    \\");
    gotoxy(x, y + 11); printf("                :     ○ :");
    gotoxy(x, y + 12); printf("               / ° :    \\");
    gotoxy(x, y + 13); printf("              :   .   ☼  '.");
    gotoxy(x, y + 14); printf("              :☼ : :    ° :");
    gotoxy(x, y + 15); printf("              :__:-:__.;--'");
    gotoxy(x, y + 16); printf("              "); color(cS); printf("'-'   '-'");
}

void tutorialUpgrade(int x, int y, Habilidade habilidade)
{
    gotoxy(x, y);

    switch (habilidade)
    {
        case Vida:
            color(10);
            printf("Aperte 'w' para gastar %d pontos de maestria em Vida", _vida);
            break;
        case Ataque:
            color(3);
            printf("Aperte 'a' para gastar %d pontos de maestria em Ataque", _ataque);
            break;
        case Defesa:
            color(12);
            printf("Aperte 'd' para gastar %d pontos de maestria em Defesa", _defesa);
            break;
        case Sair:
            color(0);
            printf("                                                            ");
    }
}

void printVida(int x, int y, bool colorido) //18x15
{
    int cA = 15; //cor das asas
    int cC = 15; //cor do cajado
    int cS = 15; //cor das serpentes

    if (colorido)
    {
        cA = 3;
        cC = 14;
        cS = 2;
    }

    color(cA); 
    gotoxy(x, y - 3); printf(" _____  "); color(cC); printf("_"); color(cA); printf("  _____");
    gotoxy(x, y - 2); printf("(___  \\"); color(cC); printf("( )"); color(cA); printf("/  ___)");
    gotoxy(x, y - 1); printf("  (___ "); color(cC); printf("| |"); color(cA); printf(" ___)"); color(cS); 
    gotoxy(x, y + 0); printf("   /\")`"); color(cC); printf("| |"); color(cS); printf("'(\"\\");
    gotoxy(x, y + 1); printf("  | |  "); color(cC); printf("| |"); color(cS); printf("  | |");
    gotoxy(x, y + 2); printf("   \\ \\_"); color(cC); printf("| |"); color(cS); printf("_/ /");
    gotoxy(x, y + 3); printf("    `._"); color(cC); printf("!"); color(cS); printf("' _.'");
    gotoxy(x, y + 4); printf("      / .'\\");
    gotoxy(x, y + 5); printf("     | / "); color(cC); printf("|"); color(cS); printf(" |");
    gotoxy(x, y + 6); printf("      \\"); color(cC); printf("|"); color(cS); printf("/ /");
    gotoxy(x, y + 7); printf("       /.<");
    gotoxy(x, y + 8); printf("      ("); color(cC); printf("| |"); color(cS); printf(")"); color(cC);
    gotoxy(x, y + 9); printf("       | '");
    gotoxy(x, y + 10); printf("       | |");
    gotoxy(x, y + 11); printf("       `-'");
}

void printEscudo(int x, int y, bool colorido) //23x17
{
    int cS = 15; //cor do simbolo
    int cF = 15; //cor da faixa
    int cP = 15; //cor padrão
    int cB = 15; //cor do brilho

    if (colorido)
    {
        cS = 6;
        cF = 12;
        cP = 7;
        cB = 8;
    }

    color(cP);
    gotoxy(x, y + 0); printf(" ____________________ ");
    gotoxy(x, y + 1); printf("|"); color(cB); printf("<><><>  "); color(cF); printf("|  |"); color(cB); printf("  <><><>"); color(cP); printf("|");
    gotoxy(x, y + 2); printf("|"); color(cB); printf("<>      "); color(cF); printf("|  |"); color(cB); printf("      <>"); color(cP); printf("|");
    gotoxy(x, y + 3); printf("|"); color(cB); printf("<>      "); color(cF); printf("|  |"); color(cB); printf("      <>"); color(cP); printf("|");
    gotoxy(x, y + 4); printf("|"); color(cB); printf("<>   "); color(cS); printf(".--------.   "); color(cB); printf("<>"); color(cP); printf("|");
    gotoxy(x, y + 5); printf("|     "); color(cS); printf("|   ()   |     "); color(cP); printf("|");
    gotoxy(x, y + 6); printf("|"); color(cF); printf("_____"); color(cS); printf("| (O\\/O) |"); color(cF); printf("_____"); color(cP); printf("|");
    gotoxy(x, y + 7); printf("|     "); color(cS); printf("\\   /\\   /     "); color(cP); printf("|");
    gotoxy(x, y + 8); printf("|"); color(cF); printf("------"); color(cS); printf("\\  \\/  /"); color(cF); printf("------"); color(cP); printf("|");
    gotoxy(x, y + 9); printf("|       "); color(cS); printf("'.__.'       "); color(cP); printf("|");
    gotoxy(x, y + 10); printf("|        "); color(cF); printf("|  |        "); color(cP); printf("|");
    gotoxy(x, y + 11); printf(":        "); color(cF); printf("|  |        "); color(cP); printf(":");
    gotoxy(x, y + 12); printf(" \\       "); color(cF); printf("|  |       "); color(cP); printf("/ ");
    gotoxy(x, y + 13); printf("  \\"); color(cB); printf("<>    "); color(cF); printf("|  |    "); color(cB); printf("<>"); color(cP); printf("/  ");
    gotoxy(x, y + 14); printf("   \\"); color(cB); printf("<>   "); color(cF); printf("|  |   "); color(cB); printf("<>"); color(cP); printf("/   ");
    gotoxy(x, y + 15); printf("    `\\"); color(cB); printf("<> "); color(cF); printf("|  | "); color(cB); printf("<>"); color(cP); printf("/'    ");
    gotoxy(x, y + 16); printf("      `-."); color(cF); printf("|__|"); color(cP); printf(".-`     ");
}

void printEspada(int x, int y, bool colorido)
{
    int cE = 15; //cor da energia
    int cL = 15; //cor da lamina
    int cP = 15; //cor da pedra
    int cC = 15; //cor do cabo
    int cS = 15; //cor de segurar?

    if (colorido)
    {
        cE = 3;
        cL = 1;
        cP = 12;
        cC = 6;
        cS = 14;
    }

    color(cE);
    gotoxy(x, y + 0); printf("        )         ");
    gotoxy(x, y + 1); printf("          (            ");
    gotoxy(x, y + 2); printf("        '    }      ");
    gotoxy(x, y + 3); printf("      (    '      ");
    gotoxy(x, y + 4); printf("     '      (   ");
    gotoxy(x, y + 5); printf("      )  "); color(cL); printf("^    "); color(cE); printf(") ");
    gotoxy(x, y + 6); printf("    '   "); color(cL); printf("/|\\    "); color(cE); printf("`");
    gotoxy(x, y + 7); printf("   )   "); color(cL); printf("/ | \\  "); color(cE); printf("` )   ");
    gotoxy(x, y + 8); printf("  {    "); color(cL); printf("| | |  "); color(cE); printf("{   ");
    gotoxy(x, y + 9); printf(" }     "); color(cL); printf("| | |  "); color(cE); printf(".");
    gotoxy(x, y + 10); printf("  '    "); color(cL); printf("| | |    "); color(cE); printf(")");
    gotoxy(x, y + 11); printf(" (     "); color(cL); printf("| | |     "); color(cE); printf(".");
    gotoxy(x, y + 12); printf("  .    "); color(cL); printf("| | |    "); color(cE); printf("(");
    gotoxy(x, y + 13); printf("}      "); color(cL); printf("| | |    "); color(cE); printf(".        ");
    gotoxy(x, y + 14); printf(" (     "); color(cL); printf("| | |     "); color(cE); printf("}");
    gotoxy(x, y + 15); printf(" '     "); color(cL); printf("| | |    "); color(cE); printf("' ");
    gotoxy(x, y + 16); printf("  }    "); color(cL); printf("| | |   "); color(cE); printf("}");
    gotoxy(x, y + 17); printf("    .  "); color(cL); printf("| | | "); color(cE); printf("'");
    gotoxy(x, y + 18); printf("       "); color(cL); printf("J | L"); color(cC);
    gotoxy(x, y + 19); printf(" /|    "); color(cL); printf("J_|_L    "); color(cC); printf("|\\");
    gotoxy(x, y + 20); printf(" \\ \\___/ "); color(cP); printf("o "); color(cC); printf("\\___/ /");
    gotoxy(x, y + 21); printf("  \\_____ _ _____/");
    gotoxy(x, y + 22); printf("        "); color(cS); printf("|-|");
    gotoxy(x, y + 23); printf("        |-|");
    gotoxy(x, y + 24); printf("        |-|");
    gotoxy(x, y + 25); printf("       "); color(cC); printf(","); color(cS); printf("'-'"); color(cC); printf(".");
    gotoxy(x, y + 26); printf("       '---'");
}

//▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓

void configuracoes()
{
    SetConsoleOutputCP(65001);
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT sr;
    COORD consoleSize;
    consoleSize.X = 71; 
    consoleSize.Y = 30;

    sr.Top=sr.Left=0;
    sr.Right=70; sr.Bottom=29;
    SetConsoleWindowInfo(console, TRUE, &sr);
    SetConsoleScreenBufferSize(console, consoleSize);

    hideCursor();
    srand(time(NULL)); //ALEATÓRIO
}

//Procedimento que limpa a tela
//Nome: clear
//Tipo: Vazio

//Parâmetros: 0
//Variáveis: 0
void clear()
{
    COORD coordScreen = {0, 0}; 
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; 
    DWORD dwConSize; 
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
     GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
  
    FillConsoleOutputCharacter(hConsole, TEXT(' '),
                   dwConSize, coordScreen, &cCharsWritten);
  
    GetConsoleScreenBufferInfo(hConsole, &csbi);
  
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
                               dwConSize, coordScreen, &cCharsWritten);
    
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void hideCursor()
{
  CONSOLE_CURSOR_INFO cursor = {1, FALSE};
  SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

//Procedimento que posiciona o texto na tela
//Nome: gotoxy
//Tipo: Vazio

//Parâmetros: 2
//• Inteiro coluna: Valor em relação ao eixo X na tela
//• Inteiro linha: Valor em relação ao eixo Y na tela

//Variáveis: 0
void gotoxy(int coluna, int linha)
{
  COORD coord;
  coord.X = coluna;
  coord.Y = linha;
  SetConsoleCursorPosition(GetStdHandle( STD_OUTPUT_HANDLE ), coord);
}

//Procedimento que troca a cor do texto na tela
//Nome: color
//Tipo: Vazio

//Parâmetros: 1
//• Inteiro cor: número que representa determinada cor dada pelo usuário

//Variáveis: 0
void color(int cor)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor);
}