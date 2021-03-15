#include <iostream>
#include <time.h>    //случайные числа
#include <stdio.h>   //для стандартных ввода вывода
#include <windows.h> // для HANDLE, курсора, цвета, sleep()
#include <conio.h>   //для kbhit, getch

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

struct Snake            // структура змейка
{
    COORD *ptrSnake;    //указатель на точки в консоли
    int length;         //количество точек
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




class Game //координаты змейки, еды, передвижение по X и Y, задержка, направление
{
    public:
    Snake snake;
    int dx, dy, pause;

    Game()
    {
        snake.length = 2;                           //сколько точек в змейки
        snake.ptrSnake = new COORD [2];             //создали точки
        snake.ptrSnake[1].X = SCREEN_WIDTH / 2 ;    //начальная позиция змейки по X
        snake.ptrSnake[1].Y = SCREEN_HEIGHT / 2;    //начальная позиция змейки по Y
        dx = 0;                                     //начальное направление змейки по Х
        dy = 0;                                     //начальное направление змейки по У
        pause = 220;                                //начльная скорость передвижение змеи
    }
};


void AddFruit(Game game) //Функция разброски еды
{
    int i;
    int n = game.snake.length;

    do
    {
        fruitX = rand() % SCREEN_WIDTH;
        fruitY = rand() % SCREEN_HEIGHT;

        for(i = 0; i < n; i++)
        {
            if(fruitX == game.snake.ptrSnake[i].X && game.snake.ptrSnake[i].Y) // проверка чтобы еда не попала на змею
            break;
        }
    }
    while(i < n);

    SetConsoleCursorPosition(hStdOut, {fruitX, fruitY}); //переносим курсор в эту позицию
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY | FOREGROUND_RED);
    printf("%c", 6); //символ еды
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
        cout << '+';                                //Рисуем верхнюю горизонтальную стену
        SetConsoleCursorPosition(hStdOut, {i, 17});
        cout << '+';                                //Рисуем нижнию горизонтальную стену
    }

    for(int j = 3; j < SCREEN_HEIGHT; j++)
    {
        SetConsoleCursorPosition(hStdOut, {2,j});
        cout << '+';                                //Рисуем левую вертикальную стену
        SetConsoleCursorPosition(hStdOut, {77,j});
        cout << '+';                                //Рисуем правую вертикальную стену
    }

    isMenuPrint = false;

    isFieldPrint = true;
    AddFruit(game);

}

int Move(Game &gm)
{
    int &n = gm.snake.length;
    COORD head = gm.snake.ptrSnake[n - 1];  //голова
    COORD tail = gm.snake.ptrSnake[0];      //хвост
    COORD next;                             //последующая часть тела змейки
    next.X = head.X + gm.dx;                //проверка следующей точки по направлению по Х
    next.Y = head.Y + gm.dy;                //проверка следующей точки по направлению по У

    if(next.X < 3 || next.Y < 3 || next.X > SCREEN_WIDTH || next.Y > SCREEN_HEIGHT)//не уперлась ли в стену?
        return KONEC;

    if(n >4)
    {
        for(int i = 0; i < n; i++)
            if(next.X == gm.snake.ptrSnake[i].X && next.Y == gm.snake.ptrSnake[i].Y) //не наехали ли на себя?
                return KONEC;
    }
    if(next.X == fruitX && next.Y == fruitY)        //если наехали на еду
    {
        COORD *temp = new COORD[ ++n ]; //новый масив больший на 1
        for(int i = 0; i < n; i++)
            temp[i] = gm.snake.ptrSnake[i]; //перекопируем
        temp[n - 1] = next; //добавляем одну
        delete [] gm.snake.ptrSnake;
        gm.snake.ptrSnake = temp;

        SetConsoleCursorPosition(hStdOut,head);
        SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); //закрашиваем еду которую съели
        printf("%c",4);                                                                                         //символ съеденной еды в змейке
        SetConsoleCursorPosition(hStdOut,next);
        SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); //закрашиваем голову змейки
        printf("%c",2);                 //символ головы змейки
        AddFruit(gm);
        return PLUS;
    }

    for(int i = 0; i < n - 1; i++)                                               //простое перемещение змейки
        gm.snake.ptrSnake[i] = gm.snake.ptrSnake[i + 1];
    gm.snake.ptrSnake[n - 1] = next;
    SetConsoleCursorPosition(hStdOut,tail);                                     //закрашиваем след
    printf(" ");
    SetConsoleCursorPosition(hStdOut,head);
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_GREEN);     //красим хвост змеи в зеленый цвет
    printf("0");                                                                //символ тела змейки
    SetConsoleCursorPosition(hStdOut,next);
    SetConsoleTextAttribute(hStdOut, FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE); //красим голову змеи в белый цвет
    printf("%c",2);                                                                                         //символ головы змейки
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
    Sleep(game.pause); //Задержка
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


