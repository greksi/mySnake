#include <iostream>
#include <time.h>    //��������� �����
#include <stdio.h>   //��� ����������� ����� ������
#include <windows.h> // ��� HANDLE, �������, �����, sleep()
#include <conio.h>   //��� kbhit, getch

using namespace std;

const char KEY_ARROW_UP = 72;
const char KEY_ARROW_RIGHT = 77;
const char KEY_ARROW_DOWN = 80;
const char KEY_ARROW_LEFT = 75;

 HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);


//CONSOLE_SCREEN_BUFFER_INFO csbInfo;
//GetConsoleScreenBufferInfo (hStdOut,  &csbInfo);
//csbiInfo.bVisible = false;




const char * gameTitle = "Simple Snake Game";
const char * gameWelcome = "*** Welcome to Simple Snake game! ***";
const int SCREEN_WIDTH = 78;        // to be able clear screen
const int SCREEN_HEIGHT = 18;       // to be able clear screen

enum eDirection {STOP,UP, DOWN, LEFT, RIGHT};
enum eGameStage {MENU, SETTINGS, GAME};
enum eGameStatus {START, PAUSE};
enum {KONEC, PLUS, MOVE};

eDirection dir;
eGameStage stage;
eGameStatus status;


bool isMenuPrint;
bool isFieldPrint;
bool isGameOver;
bool isPlay;

int fruitX, fruitY;



struct position {
    int x,y;
};

struct Snake            // ��������� ������
{
    COORD *ptrSnake;    //��������� �� ����� � �������
    int length;         //���������� �����
};

struct Temp
{
    int tempDx;
    int tempDy;
    eDirection tempDir;
} temp;

/*class food {
    position pos;
    char symbol;
public:
    food(): symbol('X'), pos() {
        pos.x = pos.y = -1;
    }

    void set_pos(int x, int y) {
        pos.x = x;
        pos.y = y;
    }

    void reposition(const field_cls & field) {
        pos.x = rand() % field.get_width();
        pos.y = rand() % field.get_height();
    }

    int get_x() const {return pos.x;}
    int get_y() const {return pos.y;}
    char get_symbol() const {return symbol;}
} food;*/




class Game //���������� ������, ���, ������������ �� X � Y, ��������, �����������
{
    public:
    Snake snake;
    int dx, dy, pause;

    Game()
    {
        snake.length = 2;                           //������� ����� � ������
        snake.ptrSnake = new COORD [2];             //������� �����
        snake.ptrSnake[1].X = SCREEN_WIDTH / 2 ;    //��������� ������� ������ �� X
        snake.ptrSnake[1].Y = SCREEN_HEIGHT / 2;    //��������� ������� ������ �� Y
        dx = 0;                                     //��������� ����������� ������ �� �
        dy = 0;                                     //��������� ����������� ������ �� �
        pause = 220;                                //�������� �������� ������������ ����
    }
};


void AddFruit(Game game) //������� ��������� ���
{
    int i;
    int n = game.snake.length;

    do
    {
        fruitX = rand() % SCREEN_WIDTH;
        fruitY = rand() % SCREEN_HEIGHT;

        for(i = 0; i < n; i++)
        {
            if(fruitX == game.snake.ptrSnake[i].X && game.snake.ptrSnake[i].Y) // �������� ����� ��� �� ������ �� ����
            break;
        }
    }
    while(i < n);

    SetConsoleCursorPosition(hStdOut, {fruitX, fruitY}); //��������� ������ � ��� �������
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY | FOREGROUND_RED);
    printf("%c", 6); //������ ���
}

void PrintMenu()
{
    system("cls");
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
    SetConsoleCursorPosition(hStdOut,{35,3}); cout << gameWelcome;
    SetConsoleCursorPosition(hStdOut,{25,8}); cout << "MENU";
    SetConsoleCursorPosition(hStdOut, {20,10}); cout << "Press 'Spase' to Start";
    SetConsoleCursorPosition(hStdOut, {20,12}); cout << "Press 'Esc' to Exit";
    isFieldPrint = false;
    isMenuPrint = true;
}

void PrintField(Game game)
{
    system("cls");
    isMenuPrint = false;
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    SetConsoleCursorPosition(hStdOut, {30, 1}); cout << "NABERITE 60 O4KOV";
    SetConsoleCursorPosition(hStdOut, {2, 19}); cout << "Score: 0";
    SetConsoleCursorPosition(hStdOut, {2, 21}); cout << "Speed: 0";
    SetConsoleCursorPosition(hStdOut, {2, 23}); cout << "Length: 0";
    SetConsoleCursorPosition(hStdOut, {65, 19}); cout << "Esc: Exit";
    SetConsoleCursorPosition(hStdOut, {65, 21}); //cout << "Space: Pause";
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);

    for(int i = 2; i < SCREEN_WIDTH; i++)
    {
        SetConsoleCursorPosition(hStdOut, {i, 2});
        cout << '+';                                //������ ������� �������������� �����
        SetConsoleCursorPosition(hStdOut, {i, 17});
        cout << '+';                                //������ ������ �������������� �����
    }

    for(int j = 3; j < SCREEN_HEIGHT; j++)
    {
        SetConsoleCursorPosition(hStdOut, {2,j});
        cout << '+';                                //������ ����� ������������ �����
        SetConsoleCursorPosition(hStdOut, {77,j});
        cout << '+';                                //������ ������ ������������ �����
    }

    isMenuPrint = false;

    isFieldPrint = true;
    AddFruit(game);

}

int Move(Game &gm)
{
    int &n = gm.snake.length;
    COORD head = gm.snake.ptrSnake[n - 1];  //������
    COORD tail = gm.snake.ptrSnake[0];      //�����
    COORD next;                             //����������� ����� ���� ������
    next.X = head.X + gm.dx;                //�������� ��������� ����� �� ����������� �� �
    next.Y = head.Y + gm.dy;                //�������� ��������� ����� �� ����������� �� �

    if(next.X < 3 || next.Y < 3 || next.X > SCREEN_WIDTH || next.Y > SCREEN_HEIGHT)//�� �������� �� � �����?
        return KONEC;

    if(n >4)
    {
        for(int i = 0; i < n; i++)
            if(next.X == gm.snake.ptrSnake[i].X && next.Y == gm.snake.ptrSnake[i].Y) //�� ������� �� �� ����?
                return KONEC;
    }
    if(next.X == fruitX && next.Y == fruitY)        //���� ������� �� ���
    {
        COORD *temp = new COORD[ ++n ]; //����� ����� ������� �� 1
        for(int i = 0; i < n; i++)
            temp[i] = gm.snake.ptrSnake[i]; //������������
        temp[n - 1] = next; //��������� ����
        delete [] gm.snake.ptrSnake;
        gm.snake.ptrSnake = temp;

        SetConsoleCursorPosition(hStdOut,head);
        SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); //����������� ��� ������� �����
        printf("%c",4);                                                                                         //������ ��������� ��� � ������
        SetConsoleCursorPosition(hStdOut,next);
        SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); //����������� ������ ������
        printf("%c",2);                 //������ ������ ������
        AddFruit(gm);
        return PLUS;
    }

    for(int i = 0; i < n - 1; i++)                                               //������� ����������� ������
        gm.snake.ptrSnake[i] = gm.snake.ptrSnake[i + 1];
    gm.snake.ptrSnake[n - 1] = next;
    SetConsoleCursorPosition(hStdOut,tail);                                     //����������� ����
    printf(" ");
    SetConsoleCursorPosition(hStdOut,head);
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_GREEN);     //������ ����� ���� � ������� ����
    printf("0");                                                                //������ ���� ������
    SetConsoleCursorPosition(hStdOut,next);
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); //������ ������ ���� � ����� ����
    printf("%c",2);                                                                                         //������ ������ ������
    return MOVE;

}



void Setup()
{
    SetConsoleTitle(gameTitle);
    isMenuPrint = false;
    isFieldPrint = false;
    isPlay = false;
    isGameOver = false;

    dir = STOP;

    stage = MENU;
    status = START;

}

void Input( Game &game)
{
    if (_kbhit())
    {
        switch(_getch())
        {
          case KEY_ARROW_UP: if(isPlay) dir = UP;
          break;

          case KEY_ARROW_DOWN: if(isPlay) dir = DOWN;
          break;

          case KEY_ARROW_RIGHT: if(isPlay) dir = RIGHT;
          break;

          case KEY_ARROW_LEFT: if(isPlay) dir = LEFT;
          break;

          case 32: switch(stage)
                   {
                        case MENU: stage = GAME;
                        break;

                        case GAME: status == START ? status = PAUSE : status = START;
                        break;
                   }
          break;

          case 27: isGameOver = true;
          break;
        }
    }
}

void Update(Game &game)
{
    if(stage == GAME)
    Move(game);
    Sleep(game.pause); //��������
}

void HandleDir(Game &game)
{
    switch(dir)
    {
       case LEFT: if(game.dx != 1) {game.dx = -1; game.dy = 0;}
       break;

       case RIGHT: if(game.dx != -1) {game.dx = 1; game.dy = 0;}
       break;

       case UP: if(game.dy != 1) {game.dy = -1; game.dx = 0;}
       break;

       case DOWN: if(game.dy != -1) {game.dy = 1; game.dx = 0;}
       break;

       case STOP: game.dy = 0; game.dx = 0;
       break;
    }
}

void HandleStage(Game &game)
{
    switch(stage)
    {
        case MENU: if(!isMenuPrint) PrintMenu();
        break;

        case GAME: if(!isFieldPrint) PrintField(game);
        break;
    }
}

void HandleStatus(Game &game)
{
    switch(status)
    {
        case PAUSE:
            if(isPlay)
            {
                temp.tempDx = game.dx;
                temp.tempDy = game.dy;
                temp.tempDir = dir;
                dir = STOP;
                isPlay = false;
                DWORD cCharsWritten;
                FillConsoleOutputCharacter(hStdOut,(TCHAR)' ', SCREEN_WIDTH - 63, {65, 21}, &cCharsWritten);
                SetConsoleCursorPosition(hStdOut, {65, 21});
                cout << "Space: Continue";
            }
        break;

        case START: if(!isPlay)
                    {
                        dir = temp.tempDir;
                        game.dx = temp.tempDx;
                        game.dy = temp.tempDy;
                        isPlay = true;
                        DWORD cCharsWritten;
                        FillConsoleOutputCharacter(hStdOut,(TCHAR)' ', SCREEN_WIDTH - 63, {65, 21}, &cCharsWritten);
                        SetConsoleCursorPosition(hStdOut, {65, 21});
                        cout << "Space: Pause";
                    }
        break;
    }
}



void Logic(Game &game)
{
    HandleStage(game);
    if(stage == GAME)
    {
        HandleStatus(game);
        HandleDir(game);
    }
}




int main()
{
     srand(static_cast<unsigned int>(time(0)));
    Game gm;
    Setup();


    while(!isGameOver)
    {
        Input(gm);
        Logic(gm);
        Update(gm);
    }
    return 0;
}


