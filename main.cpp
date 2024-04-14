#include <iostream>
#include <string>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "include/Player.h"
#include "include/Player_BOT.h"
#include "include/LTexture.h"
#include "include/LOutput_text.h"
#include "include/LButton.h"
#include "include/List_Records.h"

using namespace std;

//Задаем глобальные переменные и т.д.
SDL_DisplayMode displayMode;       //Очень пригодится, если хотим иметь приложение на весь экран. Этот объект нужно создать до инициализации самого SDL.
SDL_Window *gWindow;               //Окно графического интерфейса
SDL_Renderer *gRenderer;           //Рендер
TTF_Font* gFont;                   //Шрифт
const double fps = 60, deltaTime = 1000 / fps;
std::string filename = "data/";
const Uint8* keyboard_state_array = SDL_GetKeyboardState(NULL);
SDL_Event e;
int SIZE_NAME=16;
List_Records ListRec; //Создание класса для рекордов

enum flag{
              warning=-1,      //Флаг: Пользователь получает предупреждение
              waiting,      //Флаг: Ожидание действий от пользователя ...
              ex,           //Флаг: Пользователь выходит из приложения
              menu,         //Флаг: Пользователь возвращается в меню
              fight,        //Флаг: Пользователь начинает бой
              win_1,        //Флаг: Победа игрока 1
              win_2,
              revansh };      //Флаг: Победа игрока 2

void Close(){ //Функция закрытия
        if(gWindow!=NULL) {
            //Дестрой
            SDL_DestroyRenderer(gRenderer);
            SDL_DestroyWindow(gWindow);
            gWindow = NULL;
            gRenderer = NULL;
            TTF_CloseFont(gFont);
            gFont = NULL;
            //Выход из SDL
            TTF_Quit();
            IMG_Quit();
            SDL_Quit();
        }
    }
bool init() { //Метод инициализации SDL
       //Инициализируем SDL
       if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            cout << "Ошибка! SDL_INIT_VIDEO не инициализировалось." << endl;
            cout << "Подробнее: " << SDL_GetError() << endl;
            return false;
       }
       //Получаем параметры монитора с которым работаем.
       //Если в первый аргумент записать 0, то функция обратиться к главному монитору. Все полученные параметры можно считать с объекта displayMode.
       //SDL_GetDesktopDisplayMode(0,&displayMode);
       displayMode.w=1536;
       displayMode.h=864;
       if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            cout << "Внимание! Линейная фильтрация текстур не включена!" << endl;
            return false;
        }
        //Далее создаем окно
        gWindow = SDL_CreateWindow("Sea battel",     //Загаловок окна
                                  0,                //Позиция окна на мониторе по x
                                  0,                //Позиция окна на мониторе по y
                                  displayMode.w,    //Ширина окна
                                  displayMode.h,    //Высота окна
                                  SDL_WINDOW_SHOWN);//Флаги окна. Полный список есть на вики.
        if (gWindow == nullptr){
                cout << "Ошибка! Не удалось создать окно." << endl;
                cout << "Подробнее: " << SDL_GetError() << endl;
                return false;
        }
        //Далее создаем рендер
        gRenderer = SDL_CreateRenderer(gWindow,    //Окно в котором мы будем работать.
                                     -1,    //Индекс драйвера который будет использовать рендер. Если поставить -1, то рендер будет использовать первый подходящий драйвер.
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //Флаги рендера. Полный список есть на вики.
                                    //Будем использовать SDL_RENDERER_ACCELERATED отвечающий за аппаратное ускорение и
                                    //SDL_RENDERER_PRESENTVSYNC отвечающий за вертикальную синхронизацию.
        if (gRenderer == nullptr){
            cout << "Ошибка! Не удалось создать рендер." << endl;
            cout << "Подробнее: " << SDL_GetError() << endl;
            return false;
        }
        //Инициализируется цвет, средство визуализации
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        //Инициализируется загрузка PNG
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
                cout << "Ошибка! SDL_image не удалось инициализировать." << endl;
                cout << "Подробнее: " << SDL_GetError() << endl;
                return false;
        }
        //Инициализация SDL_ttf
        if (TTF_Init() == -1) {
                cout << "Ошибка! SDL_ttf не удалось инициализировать." << endl;
                cout << "Подробнее: " << SDL_GetError() << endl;
                return false;
        }
        //Далее делаем иконку для окна. Уж если делать все красиво, то до конца!
        //Важно, лучше иконку брать формата png, т.к. в bmp нет прозрачности.
        SDL_Surface *icon=IMG_Load("data/icon.png"); //Это поверхность с изображением иконки
        if (icon==nullptr) {
            cout << "Ошибка! Не удалось загрузить иконку." << endl;
            cout << "Подробнее: " << SDL_GetError() << endl;
            return false;
        }
        SDL_SetWindowIcon(gWindow, icon);
        SDL_FreeSurface(icon);
        //Загружаем также шрифт
        gFont = TTF_OpenFont((filename+"/fonts/SkylineBeachNbpRegular.ttf").c_str(), 50);
        if (gFont == NULL) {
            cout << "Ошибка! Не удалось загрузить шрифт." << endl;
            cout << "Подробнее: " << TTF_GetError() << endl;
            return false;
        }
       //Инициализация SDL закончена! P.S. Это только начало, а я уже устал.
       return true;
    }
int Menu(string name) { //Это метод вывода "Главного меню"
        int menu=-1;
        int i,x=0,y=0, NUMMENU=6;
        LButton tmp_text[5];   //Текстуры для кнопок
        LOutput_text tmp_name[2]; //Текстура для строки с имененем пользователя
        string text[NUMMENU]={"Добро пожаловать,",
                              "> НАЧАТЬ СРАЖЕНИЕ",
                              "> РЕКОРДЫ",
                              "> ИНФОРМАЦИЯ",
                              "> СМЕНИТЬ ИМЯ",
                              "> ВЫХОД"};
        //Загрузка эмблемы игры
        LTexture emblem;
        LTexture background;
        SDL_Delay(250);
        if (!emblem.Load(filename + "Emblema.png") || !background.Load(filename + "Background_MENU.jpg")) { //Адрес фона
            std::cout << "Ошибка! Не удалось загрузить текстуру листа спрайта эмблемы или фона." << endl;
            return 4;
        }
        else {
                emblem.setRect(508,193);
                background.setRect(displayMode.w, displayMode.h);
        }
        tmp_name[0].Load(text[0], false);
        tmp_name[1].Load(name+"!", false);
        for(i=1; i<NUMMENU; i++) tmp_text[i-1].Load(text[i]);
		do {
                SDL_RenderClear(gRenderer);
                background.Render(0, 0);
                emblem.Render(20, displayMode.h/2-320);
                tmp_name[0].Render(63, 2*displayMode.h/3 - 260);
                tmp_name[1].Render(63, 2*displayMode.h/3 - 210);
                SDL_GetMouseState(&x, &y);
                for(i=0; i<(NUMMENU-1); i++) {
                        tmp_text[i].Check(x,y);
                        tmp_text[i].Render(63, 2*displayMode.h/3 - (210-60*(i+1)));
                }
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) { menu = 5; break; }
					else if (e.type == SDL_KEYDOWN) {
						if (keyboard_state_array[SDL_SCANCODE_1]) { //Старт новой игры
						    menu = 1;
						}
						else if (keyboard_state_array[SDL_SCANCODE_2]) { //Статистика
							menu = 2;
						}
						else if (keyboard_state_array[SDL_SCANCODE_3]) { //Информация
						    menu = 3;
						}
						else if (keyboard_state_array[SDL_SCANCODE_4]) { //Сменить имя
						    menu = 4;
						}
						else if (keyboard_state_array[SDL_SCANCODE_5]) {  //Выход
                            menu = 5;
						}
						if (menu!=-1) break;
					}
					else if (e.type == SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                        for(i=1; i<NUMMENU; i++) if(tmp_text[i-1].Check(x,y)) {
                            menu=i;
                            break;
                        }
                        if (menu!=-1) break;
                    }
				}
				SDL_Delay(deltaTime);
                SDL_RenderPresent(gRenderer);
        } while (menu==-1);
        cout << "Сейчас пользователь захотел " << text[menu] << endl;
        return menu;
}
string PresNamePlayer(int *menu) {//Функция ввода имени игрока
    int x=0, y=0;
    LButton button;
    LTexture background;
    LOutput_text tmp_text[2];
    string print_text="Введите своё имя, капитан: ";
	string text;
	string vevod_text;
	char is_text_entered = 0;
	SDL_Delay(250);
	button.Load(">Ввести имя");
	if(!background.Load(filename + "Background_ScreenSaver.png")) {
            std::cout << "Ошибка! Не удалось загрузить текстуру листа спрайта эмблемы или фона." << endl;
            std::cout << "Подробнее: " << SDL_GetError() << endl;
            *menu=5;
            return "FALSE";
	} else background.setRect(displayMode.w, displayMode.h);
	tmp_text[1].Load("Неккоректный размер имени. (НАДО: от 1 до " + to_string(SIZE_NAME) + " байт)", true);
	do {
		SDL_RenderClear(gRenderer);
		background.Render(0, 0);
		SDL_GetMouseState(&x, &y);
		SDL_StartTextInput();
		while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                    *menu=5;
                    is_text_entered = 1;
            }
			else if (e.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_RETURN]) {
				if(text.length()<=SIZE_NAME && text.length()!=0) is_text_entered = 1; else is_text_entered = 2;
			}
			else if (e.type == SDL_TEXTINPUT) {
				text += e.text.text;
				is_text_entered = 0;
			}
			else if (e.type == SDL_KEYDOWN && keyboard_state_array[SDL_SCANCODE_BACKSPACE]) {
				if (text.length() > 0) {
                    text.pop_back();
                    if ((unsigned int)text.back()>=4294967248) text.pop_back();
				}
				is_text_entered = 0;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                    if(button.Check(x,y)) {
                            if((text.length()<=SIZE_NAME && text.length()!=0)) is_text_entered = 1;
                            else is_text_entered = 2;
                    }
			}
		}
		SDL_StopTextInput();
		if(print_text!=" ") {
            if (text.length() > 0) vevod_text=print_text+text;
            else vevod_text=print_text;
            tmp_text[0].Load(vevod_text, false);
            tmp_text[0].Render(displayMode.w/2-500, displayMode.h/2-150);
            if (is_text_entered==2) tmp_text[1].Render(displayMode.w/2-500, displayMode.h/2-100);
            button.Check(x,y);
            button.Render(displayMode.w/2-500, displayMode.h/2-50);
		}
		SDL_Delay(deltaTime);
		SDL_RenderPresent(gRenderer);
	} while (is_text_entered!=1);
	SDL_RenderClear(gRenderer);
	//if(text=="") text="Безымянный игрок";
	cout << "Игрок сменил/установил имя " << text << "." << endl;
	return text;
}
void Information(int *menu) {
    LTexture background;
    LButton button;
    bool f=false;
    int x,y;
    SDL_Delay(250);
    if(!background.Load(filename + "Background_Information.png")) {
            std::cout << "Ошибка! Не удалось загрузить текстуру листа спрайта эмблемы или фона." << endl;
            std::cout << "Подробнее: " << SDL_GetError() << endl;
            *menu=5;
            return;
	} else background.setRect(displayMode.w, displayMode.h);
	button.Load("> МЕНЮ");
    do {
        SDL_RenderClear(gRenderer);
        background.Render(0, 0);
        SDL_GetMouseState(&x, &y);
        button.Check(x,y);
        button.Render(20, displayMode.h-80);
        while (SDL_PollEvent(&e)!= 0) {
                if(e.type==SDL_QUIT) {
                    *menu=5;
                    f=true;
                }
                else if (e.type == SDL_KEYDOWN) { //Если пользователь воспользовался быстрыми клавишами действий
                    if (keyboard_state_array[SDL_SCANCODE_RETURN]) f=true; //Пользователь захотел вернуться в главное меню
                }
                else if (e.type==SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                        if(button.Check(x,y)) f=true;
                }
        }
        SDL_Delay(deltaTime);
		SDL_RenderPresent(gRenderer);
    } while(!f);
}
void Screensaver(int *menu) {
    LTexture background;
    int i, NUMMENU=4;
    LOutput_text tmp_text[NUMMENU];
    bool f=false;
    SDL_Delay(250);
    if(!background.Load(filename + "Background_ScreenSaver.png")) {
            std::cout << "Ошибка! Не удалось загрузить текстуру листа спрайта эмблемы или фона." << endl;
            std::cout << "Подробнее: " << SDL_GetError() << endl;
            *menu=5;
            return;
	} else background.setRect(displayMode.w, displayMode.h);
	tmp_text[0].Load("Название: «Морской бой»", false);
	tmp_text[1].Load("Разработчик: Целихович А.Л.", false);
	tmp_text[2].Load("Группа: о712б", false);
	tmp_text[3].Load("Нажмите ENTER для продолжения ...", false);
    do {
        SDL_RenderClear(gRenderer);
        background.Render(0, 0);
        for(i=0; i<NUMMENU; i++) tmp_text[i].Render(displayMode.w/2-500, displayMode.h/2-150+50*i);
        while (SDL_PollEvent(&e)!= 0) {
                if(e.type==SDL_QUIT) {
                    *menu=5;
                    f=true;
                }
                else if (e.type == SDL_KEYDOWN) { //Если пользователь воспользовался быстрыми клавишами действий
                    if (keyboard_state_array[SDL_SCANCODE_RETURN]) f=true; //Пользователь захотел вернуться в главное меню
                }
        }
        SDL_Delay(deltaTime);
		SDL_RenderPresent(gRenderer);
    } while(!f);
}
int Menu_PAUSE() { //Это метод вывода "Главного меню"
        int menu=-1;
        int i,x=0,y=0, NUMMENU=6;
        LButton tmp_text[5];   //Текстуры для кнопок
        LOutput_text tmp_name; //Текстура для строки с имененем пользователя
        string text[NUMMENU]={"ИГРА ПРИОСТАНОВЛЕНА!",
                              "> ПРОДОЛЖИТЬ ИГРУ",
                              "> НОВАЯ ИГРА",
                              "> РЕКОРДЫ",
                              "> ИНФОРМАЦИЯ",
                              "> ВОЗВРАТ В МЕНЮ"};
        //Загрузка эмблемы игры
        cout << "Пользователь вызвал Меню-паузу." << endl;
        LTexture emblem;
        LTexture background;
        SDL_Delay(250);
        if(!background.Load(filename + "Background_PAUSE.png") || !emblem.Load(filename + "Emblema.png")) {
            std::cout << "Ошибка! Не удалось загрузить текстуру листа спрайта эмблемы или фона." << endl;
            std::cout << "Подробнее: " << SDL_GetError() << endl;
            return 6;
        } else {
            background.setRect(displayMode.w, displayMode.h);
            emblem.setRect(508, 193);
        }
        tmp_name.Load(text[0], false);
        for(i=1; i<NUMMENU; i++) tmp_text[i-1].Load(text[i]);
		do {
                SDL_RenderClear(gRenderer);
                background.Render(0, 0);
                emblem.Render(20, displayMode.h/2-320);
                tmp_name.Render(63, 2*displayMode.h/3 - 260);
                SDL_GetMouseState(&x, &y);
                for(i=0; i<(NUMMENU-1); i++) {
                    tmp_text[i].Check(x,y);
                    tmp_text[i].Render(63, 2*displayMode.h/3 - (210-60*i));
                }
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) { menu = 6; break; }
					else if (e.type == SDL_KEYDOWN) {
						if (keyboard_state_array[SDL_SCANCODE_1]) { //Возврат
						    menu = 1;
						}
						else if (keyboard_state_array[SDL_SCANCODE_2]) { //Новая игра
							menu = 2;
						}
						else if (keyboard_state_array[SDL_SCANCODE_3]) { //Статистика
						    menu = 3;
						}
						else if (keyboard_state_array[SDL_SCANCODE_4]) { //Информация
						    menu = 4;
						}
						else if (keyboard_state_array[SDL_SCANCODE_5]) {  //Выход
                            menu = 5;
						}
						if (menu!=-1) break;
					}
					else if (e.type == SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                        for(i=1; i<NUMMENU; i++) if(tmp_text[i-1].Check(x,y)) {
                            menu=i;
                            break;
                        }
                        if (menu!=-1) break;
                    }
				}
				SDL_Delay(deltaTime);
                SDL_RenderPresent(gRenderer);
        } while (menu==-1);
        if(menu!=6) cout << "Сейчас пользователь захотел " << text[menu] << endl;
        return menu;
}
int Game(string name) {
    int x, y;
    int k, i, j;
    int motion=1;
    time_t Fixtime=0, Pastime;
    flag Flag=waiting;
    int SIZE=10, NUMMENU=6;
    Player Player_1; //Создание класса игрока 1 - пользователь
    Player_Bot Player_2; //Создание класса игрока 2 - бот
    LButton Button[2]; //Это текстуры кнопок
    LOutput_text tmp_text[NUMMENU]; //Это текстуры выводимого текста
    LTexture Empty[4]; //Это текстуры ячеек
    LTexture Backgroung_Game[3]; //Это текстуры фонов
    LTexture Destroy; //Это текстура креста для уничтоженных кораблей в списке
    string file[2];
    SDL_Delay(250);
    //Загружаем ресурсы
    for(i=0; i<3; i++) {
        switch(i) {
            case 0: file[0]="Empty.png"; file[1]="Background_Preparation.png"; break;
            case 1: file[0]="Ship.png";  file[1]="Background_Game1.png"; break;
            case 2: file[0]="Shot.png";  file[1]="Background_Game2.png"; break;
        }
        if (!Empty[i].Load(filename + file[0]) || !Backgroung_Game[i].Load(filename + file[1])) { //Адрес фона
            cout << "Ошибка! Не удалось загрузить текстуру листа спрайта ячейки." << endl;
            return 1;
        }
    }
    if(!Destroy.Load(filename + "Destroy.png") || !Empty[3].Load(filename+"NotShip.png")) {
            cout << "Ошибка! Не удалось загрузить текстуру листа спрайта ячейки." << endl;
            return 1;
    } else {
        for(i=0; i<3; i++) {
            Empty[i].setRect(48,48);
            Backgroung_Game[i].setRect(displayMode.w,displayMode.h);
        }
        Empty[3].setRect(48,48);
        Destroy.setRect(72,31);
    }
    //Начинаем с этапа подготовка
    Player_2.ReistrName();
    Player_2.Program_Creation(); //Бот заполняет поле
    //Затем загружаем фразы
    tmp_text[0].Load("Подготовительный этап!", false);
    tmp_text[1].Load("Вы: "+ name, false);
    tmp_text[2].Load("Враг: "+ Player_2.GetName(), false);
    tmp_text[3].Load("Ошибка! Некорректное размещение кораблей.", true);
    tmp_text[4].Load("Пожалуйста, расставьте свои корабли.", false);
    Button[0].Load("> ПАУЗА");
    Button[1].Load("> Начать сражение");
    do {
        SDL_RenderClear(gRenderer);
		Backgroung_Game[0].Render(0, 0); //Рендерим фон
		//Далее рандерим поля
		for(i=0; i<SIZE; i++)
            for(j=0; j<SIZE; j++) {
                if(Player_1.GetRecord(i*10+j)==-1) Empty[1].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                else Empty[0].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                Empty[0].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
            }
        //Затем рендерим фразы
        SDL_GetMouseState(&x, &y);
        for(i=0; i<2; i++) Button[i].Check(x,y);
        Button[0].Render(20, displayMode.h-80);
        Button[1].Render(displayMode.w-380,  displayMode.h-80);
        tmp_text[0].Render(displayMode.w/2-452, 20);
        tmp_text[1].Render(displayMode.w/2-500,displayMode.h/2-175-tmp_text[1].getHeight());
        tmp_text[2].Render(displayMode.w-744, displayMode.h/2-175-tmp_text[2].getHeight());
        tmp_text[4].Render(displayMode.w/2-452, 70);
        if(Flag==warning) tmp_text[3].Render(displayMode.w/2-452, 120);
        //Наконец счетиывание действий пользователя
        while (SDL_PollEvent(&e)!= 0) {
            if(e.type==SDL_QUIT) Flag=ex; //Если пользователь захотел выйти
            else if (e.type == SDL_KEYDOWN) { //Если пользователь воспользовался быстрыми клавишами действий
                    if (keyboard_state_array[SDL_SCANCODE_ESCAPE]) Flag=menu; //Пользователь захотел вернуться в главное меню
                    else if (keyboard_state_array[SDL_SCANCODE_RETURN]) { //Пользователь захотел начать игру, то есть закончил подготовку
                            if(Player_1.Overwriting()) Flag=fight; //Если все в порядке, то начинаем игру
                            else Flag=warning; //Нет дает пользователю возможность исправить расстановку кораблей
                    }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                    if(Button[0].Check(x,y)) Flag=menu;
                    else if(Button[1].Check(x,y)) {
                                if (Player_1.Overwriting()) Flag=fight;
                                else Flag=warning;
                         }
                    else for(i=0; i<SIZE && x!=-1; i++)
                        for(j=0; j<SIZE && x!=-1; j++) if((displayMode.w/2-500+(j*48))<=x && (displayMode.w/2-500+((j+1)*48))>x &&
                                                        (displayMode.h/2-170+(i*48))<=y && (displayMode.h/2-170+((i+1)*48))>y) {
                                                            if(Player_1.GetRecord(i*10+j)==0) Player_1.SetRecord(i*10+j, -1);
                                                            else Player_1.SetRecord(i*10+j, 0);
                                                            x=-1;
                                                            Flag=waiting;
                                                        }
            }
        }
        SDL_Delay(deltaTime);
		SDL_RenderPresent(gRenderer);
		if(Flag==menu) {
            Flag=fight;
            do {
                switch(Menu_PAUSE()) {
                    case 1: Flag=waiting; break;
                    case 2: Flag=revansh; break;
                    case 3: {
                        ListRec.Rating(&i);
                        if(i==5) Flag=ex;
                        else Flag=fight;
                        break;
                    }
                    case 4: {
                        Information(&i);
                        if(i==5) Flag=ex;
                        else Flag=fight;
                        break;
                    }
                    case 5: Flag=menu; break;
                    case 6: Flag=ex;
                }
            } while(Flag==fight);
		}
    } while(Flag<=0);
    if(Flag==fight) {
        tmp_text[0].Load("Сражайтесь, капитаны!", false);
        tmp_text[4].Load("Сейчас ход капитана "+name+"!", false);
        tmp_text[5].Load("Сейчас ход капитана "+Player_2.GetName()+"!", false);
        Player_1.SetOrder(rand()%2);
        Player_2.SetOrder(1-Player_1.GetOrder());
        while(Flag==fight || Flag==waiting) { //Если пользователь захотел начать игру
            SDL_RenderClear(gRenderer);
            if(motion%2==Player_1.GetOrder()) Backgroung_Game[2].Render(0, 0);
            else Backgroung_Game[1].Render(0, 0);
            tmp_text[3].Load("Ход #"+ to_string(motion%2==0? motion/2:(motion/2+1)), false);
            //Рендерим поля
            for(i=0; i<SIZE; i++) {
                for(j=0; j<SIZE; j++) {
                    if(Player_1.GetRecord(i*10+j)<0) {
                        if(Player_1.GetRecord(i*10+j)<-2) Empty[1].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_1.GetRecord(i*10+j)==-2) Empty[3].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        Empty[2].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                    } else {
                        if(Player_1.GetRecord(i*10+j)==0) Empty[0].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_1.GetRecord(i*10+j)!=2) Empty[1].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        else Empty[3].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                    }
                    if(Player_2.GetRecord(i*10+j)<0) {
                        if(Player_2.GetRecord(i*10+j)<-2) Empty[1].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_2.GetRecord(i*10+j)==-2) Empty[3].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        Empty[2].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                    } else {
                        if(Player_2.GetRecord(i*10+j)==2) Empty[3].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        else Empty[0].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                    }
                }
            }
            //Затем рендерим фразы и кнопки
            tmp_text[0].Render(displayMode.w/2-452, 20);
            tmp_text[1].Render(displayMode.w/2-500,displayMode.h/2-175-tmp_text[1].getHeight());
            tmp_text[2].Render(displayMode.w-744, displayMode.h/2-175-tmp_text[2].getHeight());
            tmp_text[3].Render(displayMode.w/2-452, 70);
            if(motion%2==Player_1.GetOrder()) tmp_text[4].Render(displayMode.w/2-452, 120);
            else tmp_text[5].Render(displayMode.w/2-452, 120);
            SDL_GetMouseState(&x, &y);
            Button[0].Check(x,y);
            Button[0].Render(20, displayMode.h-80);
            //Под конец рендерим крестики в списках уничтожения кораблей
            for (i=3, k=0; i>=0; i--) {
                for (j=0; j<4-i; j++, k++) {
                    if(Player_1.Check_Ship(i*10+j)) {
                        if(k%5==0) Destroy.Render(100, 380+130*(k%2));
                        else Destroy.Render(k%2==0? 60: 160,  k<5? 330+45*(4-i): k<8? 370+45*(4-i): 370+45*(5-i));
                    }
                    if(Player_2.Check_Ship(i*10+j)) {
                        if(k%5==0) Destroy.Render(displayMode.w-172, 380+130*(k%2));
                        else Destroy.Render(k%2==0? displayMode.w-132: displayMode.w-232,  k<5? 330+45*(4-i): k<8? 370+45*(4-i): 370+45*(5-i));
                    }
                }
            }
            Flag=fight;
            while (SDL_PollEvent(&e)!= 0) {
                if(e.type==SDL_QUIT) Flag=ex; //Если пользователь захотел выйти
                else if (e.type == SDL_KEYDOWN) { //Если пользователь воспользовался быстрыми клавишами действий
                        if (keyboard_state_array[SDL_SCANCODE_ESCAPE]) Flag=menu; //Пользователь захотел вернуться в главное меню
                }
                else if (e.type==SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                        if (Button[0].Check(x,y)) Flag=menu;
                        else if (motion%2==Player_1.GetOrder() && Flag==fight) {
                             for(i=0; i<SIZE && motion%2==Player_1.GetOrder(); i++)
                             for(j=0; j<SIZE && motion%2==Player_1.GetOrder(); j++) {
                                if((displayMode.w/2+20+(j*48))<=x && (displayMode.w/2+20+((j+1)*48))>x &&
                                   (displayMode.h/2-170+(i*48))<=y && (displayMode.h/2-170+((i+1)*48))>y) {
                                                                if (Player_2.GetRecord(i*10+j)>=0) {
                                                                    cout << endl << endl << "ХОД ЗА ИГРОКОМ №1" << endl;
                                                                    cout << "Пользователь сделал ход в ячейку " << i*10+j << endl;
                                                                    if(Player_2.Hit_Registration(i*10+j)==0) {
                                                                        motion++;
                                                                        Flag=waiting;
                                                                    }
                                                                }
                                }
                            }
                        }
                }
            }
            SDL_Delay(deltaTime);
            SDL_RenderPresent(gRenderer);
            //Далее идет ход компьютера, если его очередь
            if(motion%2==Player_2.GetOrder() && Flag==fight) {
                if(Fixtime==0) {
                    Fixtime=time(nullptr);
                    Pastime=3;
                } else if (time(nullptr)-Fixtime >= Pastime) {
                    Fixtime=0;
                    do { x=Player_2.Program_Makemove(); } while(Player_1.GetRecord(x)<0 || Player_1.GetRecord(x)==2);
                    cout << "Компьютер сделал ход в ячейку " << x << endl;
                    if (!Player_2.Progress(x, Player_1.Hit_Registration(x))) motion++;
                }
            }
            if(Player_1.Check()) Flag=win_2;
            else if(Player_2.Check()) Flag=win_1;
            if(Flag==menu) {
                if(Fixtime!=0) Pastime-=time(nullptr)-Fixtime;
                Flag=fight;
                do {
                    switch(Menu_PAUSE()) {
                        case 1: Flag=waiting; break;
                        case 2: Flag=revansh; break;
                        case 3: {
                            ListRec.Rating(&i);
                            if(i==5) Flag=ex;
                            else Flag=fight;
                            break;
                        }
                        case 4: {
                            Information(&i);
                            if(i==5) Flag=ex;
                            else Flag=fight;
                            break;
                        }
                        case 5: Flag=menu; break;
                        case 6: Flag=ex;
                    }
                } while(Flag==fight);
                if(Fixtime!=0) Fixtime=time(nullptr);
            }
        }
    }
    //Далее стоит вывести информацию кто победил
    if(Flag==4 || Flag==5) {
        tmp_text[0].Load("Игра окончена!", true);
        if(Flag==win_1) {
            tmp_text[3].Load("Победил капитан "+name+"!", false);
            ListRec.SetRecord(name.c_str(), motion%2==0?motion/2:(motion/2+1));
        }
        else {
            tmp_text[3].Load("Победил капитан "+Player_2.GetName()+"!", false);
            ListRec.SetRecord(Player_2.GetName().c_str(), motion%2==0?motion/2:(motion/2+1));
        }
        tmp_text[4].Load("Сейчас Вы можете вернуться в меню.", false);
        Button[0].Load("> МЕНЮ");
        Flag=waiting;
        do {
            SDL_RenderClear(gRenderer);
            Backgroung_Game[1].Render(0, 0);
            //Рендерим фразы
            SDL_GetMouseState(&x, &y);
            Button[0].Check(x,y);
            Button[0].Render(20, displayMode.h-80);
            tmp_text[0].Render(displayMode.w/2-452, 20);
            tmp_text[1].Render(displayMode.w/2-500,displayMode.h/2-175-tmp_text[1].getHeight());
            tmp_text[2].Render(displayMode.w-744, displayMode.h/2-175-tmp_text[2].getHeight());
            tmp_text[3].Render(displayMode.w/2-452, 70);
            tmp_text[4].Render(displayMode.w/2-452, 120);
            //Рендерим поля
            for(i=0; i<SIZE; i++)
                for(j=0; j<SIZE; j++) {
                    if(Player_1.GetRecord(i*10+j)<0) {
                        if(Player_1.GetRecord(i*10+j)<-2) Empty[1].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_1.GetRecord(i*10+j)==-2) Empty[3].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        Empty[2].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                    } else {
                        if(Player_1.GetRecord(i*10+j)==0) Empty[0].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_1.GetRecord(i*10+j)!=2) Empty[1].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                        else Empty[3].Render(displayMode.w/2-500+(j*48), displayMode.h/2-170+(i*48));
                    }
                    if(Player_2.GetRecord(i*10+j)<0) {
                        if(Player_2.GetRecord(i*10+j)<-2) Empty[1].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_2.GetRecord(i*10+j)==-2) Empty[3].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        Empty[2].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                    } else {
                        if(Player_2.GetRecord(i*10+j)==2) Empty[3].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        else if(Player_2.GetRecord(i*10+j)>2) Empty[1].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                        else Empty[0].Render(displayMode.w/2+20+(j*48), displayMode.h/2-170+(i*48));
                    }
                }
            //Рендерим список
            for (i=3, k=0; i>=0; i--) {
                for (j=0; j<4-i; j++, k++) {
                    if(Player_1.Check_Ship(i*10+j)) {
                        if(k%5==0) Destroy.Render(100, 380+130*(k%2));
                        else Destroy.Render(k%2==0? 60: 160,  k<5? 330+45*(4-i): k<8? 370+45*(4-i): 370+45*(5-i));
                    }
                    if(Player_2.Check_Ship(i*10+j)) {
                        if(k%5==0) Destroy.Render(displayMode.w-172, 380+130*(k%2));
                        else Destroy.Render(k%2==0? displayMode.w-132: displayMode.w-232,  k<5? 330+45*(4-i): k<8? 370+45*(4-i): 370+45*(5-i));
                    }
                }
            }
            while (SDL_PollEvent(&e)!= 0) {
                if(e.type==SDL_QUIT) Flag=ex; //Если пользователь захотел выйти
                else if (e.type == SDL_KEYDOWN) { //Если пользователь воспользовался быстрыми клавишами действий
                    if (keyboard_state_array[SDL_SCANCODE_RETURN]) Flag=menu; //Пользователь захотел вернуться в главное меню
                }
                else if (e.type==SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                        if(Button[0].Check(x,y)) Flag=menu;
                }
            }
            SDL_Delay(deltaTime);
            SDL_RenderPresent(gRenderer);
        } while(Flag==waiting);
    }
    return Flag;
}

int main(int argc, char **argv) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    gWindow=NULL;
    gRenderer=NULL;
    gFont=NULL;
    string name;
	int menu=0; //Принимает меню
	if (!init()) { //Инициализаруем
		cout << "Ошибка! SDL не инициализировался." << endl;
		return 0;
	}
	else {//Если все проинициализировалось, то начинаем работу
		//Задаем имя игроку
		Screensaver(&menu);
		if(menu!=5) name=PresNamePlayer(&menu);
		while(menu!=5) {
                if(menu!=1) menu=Menu(name);
                switch(menu) {
                    case 1: {
                        switch(Game(name)) {
                            case 1: menu=5; break;
                            case 6: menu=1; break;
                            default: menu=0;
                        }
                        break;
                    }
                    case 2: ListRec.Rating(&menu); break;
                    case 3: Information(&menu); break;
                    case 4: name=PresNamePlayer(&menu); break;
                    case 5: break;
                }
		}
	}
	Close();
    return 0;
}
