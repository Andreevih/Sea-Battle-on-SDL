#pragma once
extern std::string filename;
extern SDL_DisplayMode displayMode;
extern SDL_Event e;
extern const Uint8* keyboard_state_array;
extern const double deltaTime;

class List_Records { //Это класс рекордов
    FILE* File;
    int N;
    struct Record {  //Структура записи
        char Name[16]; //Имя пользователя
        int Moves;   //Кол-во ходов
    };
public:
    List_Records() {
        Creat();
    }
    ~List_Records() {
        Clear();
        std::cout << "Класс List_Record: Деструктор завершил свою работу!" << std::endl;
    }
    bool Creat() { //Метод создания файла для записи рекордов
        if((File=fopen("lib/Record.bin","wb+"))==NULL){ //Создание файла
            std::cout << "Класс List_Record: Ошибка создания файла!" << std::endl;
            return false;
        } else {
            N=0;
            fclose(File); //Закрываем файл
            return true;
        }
    }
    bool Clear() { //Метод очистки файла
        if((File=fopen("lib/Record.bin","wb+"))==NULL){ //Создание файла
            std::cout << "Класс List_Record: Ошибка создания файла!" << std::endl;
            return false;
        } else {
            N=0;
            fclose(File); //Закрываем файл
            remove("lib/Record.bin");
            return true;
        }
    }
    bool SetRecord(const char* n, int m) {
        Record NewRec;
        Record Rec;
        FILE* File_Help;
        int i;
        bool f=false;
        if((File=fopen("lib/Record.bin","rb+"))==NULL){ //Открытие файла
            std::cout << "Класс List_Record: Ошибка открытия файла!" << std::endl;
            return false;
        } else {
            for(i=0; i<16; i++) NewRec.Name[i]=n[i];
            NewRec.Moves=m;
            if(N<10) N++; //Увеличиваем число записей, вкл. новую
            if(N==1) fwrite(&NewRec,sizeof(Record),1,File); //Если записей нет
            else {
                if((File_Help=fopen("lib/Record_Help.bin","wb+"))==NULL){
                    std::cout << "Класс List_Record: Ошибка при создании вспомогательного файла!" << std::endl;
                    fclose(File); //Закрываем файл
                    return false;
                }
                for(i=0; i<(N-1); i++) {
                    fseek(File,i*sizeof(Record),SEEK_SET); //Ищем следующую запись
                    fread(&Rec, sizeof(Record),1,File); //Считали текущую запись
                    if(Rec.Moves>NewRec.Moves && !f) {
                        fwrite(&NewRec,sizeof(Record),1,File_Help);
                        f=true;
                    }
                    fwrite(&Rec,sizeof(Record),1,File_Help);
                }
                if(!f) fwrite(&NewRec,sizeof(Record),1,File_Help);
                fclose(File); //Закрываем файл
                if((File=fopen("lib/Record.bin","wb+"))==NULL) {
                    std::cout << "Класс List_Record: Ошибка создания файла!" << std::endl;
                    fclose(File_Help);
                    remove("lib/Record_Help.bin");
                    return false;
                } else {
                    for(i=0; i<N; i++) {
                        fseek(File_Help,i*sizeof(Record),SEEK_SET); //Ищем следующую запись
                        fread(&Rec, sizeof(Record),1,File_Help); //Считали текущую запись
                        fwrite(&Rec,sizeof(Record),1,File);
                    }
                }
                fclose(File_Help);
                remove("lib/Record_Help.bin");
            }

            fclose(File); //Закрываем файл
            return true;
        }
    }
    bool PrintFile() { //Метод печати файла ввиде текстового документа
        FILE* Pr;
        Record Rec;
        std::string motion;
        int i;
        if((File=fopen("lib/Record.bin","rb+"))==NULL) {
            std::cout << "Класс List_Record: Ошибка открытия файла!" << std::endl;
            return false;
        } else {
            if((Pr=fopen("Record.txt","w"))==NULL) {
                std::cout << "Класс List_Record: Ошибка создания файла!" << std::endl;
                fclose(File);
                return false;
            } else {
                for(i=0; i<N; i++) {
                    fseek(File,i*sizeof(Record),SEEK_SET); //Ищем следующую запись
                    fread(&Rec, sizeof(Record),1,File); //Считали текущую запись
                    fputs(Rec.Name, Pr);
                    motion=std::to_string(Rec.Moves);
                    fputs(" ", Pr);
                    fputs(motion.c_str(), Pr);
                    fputs("\n",Pr);
                }
                fclose(Pr);
                fclose(File);
            }
        }
        return true;
    }
    void Rating(int *menu) { //Метод вывода на экран
       LTexture background;
       LButton button[3];
       LOutput_text tmp_text[2*(N+1)];
       LOutput_text Warn;
       Record Rec;
       bool f=false;
       int x,y,i;
       SDL_Delay(250);
       if(!background.Load(filename + "Background_Record.png")) {
            std::cout << "Класс List_Record: Ошибка! Не удалось загрузить лист спрайтов логотипа или фоновую текстуру." << std::endl;
            std::cout << "Подробнее: " << SDL_GetError() << std::endl;
            *menu=5;
            return;
       } else background.setRect(displayMode.w, displayMode.h);
       button[0].Load("> МЕНЮ");
       button[1].Load("> Очистить");
       button[2].Load("> Экспорт");
       Warn.Load("Список пуст",true);
       tmp_text[0].Load("Имя капитана", false);
       tmp_text[1].Load("Ходы", false);
       if((File=fopen("lib/Record.bin","rb+"))==NULL){ //Открытие файла
            std::cout << "Класс List_Record: Ошибка открытия файла!" << std::endl;
            *menu=5;
            return;
        } else {
            for(i=0; i<N; i++) {
                fseek(File,i*sizeof(Record),SEEK_SET); //Ищем следующую запись
                fread(&Rec, sizeof(Record),1,File); //Считали текущую запись
                tmp_text[i*2+2].Load(std::to_string(i+1)+". "+Rec.Name,false);
                tmp_text[i*2+3].Load(std::to_string(Rec.Moves),false);
            }
        }
        fclose(File);
        do {
            SDL_RenderClear(gRenderer);
            background.Render(0, 0);
            SDL_GetMouseState(&x, &y);
            for(i=0; i<3; i++) button[i].Check(x,y);
            button[0].Render(20, displayMode.h-80);
            tmp_text[0].Render(480, 150);
            tmp_text[1].Render(1000, 150);
            if(N==0) Warn.Render(500, 200);
            else {
                button[1].Render(240, 450);
                button[2].Render(240, 500);
                for(i=0; i<N; i++) {
                    tmp_text[i*2+2].Render(480,200+50*i);
                    tmp_text[i*2+3].Render(1000,200+50*i);
                }
            }
            while (SDL_PollEvent(&e)!= 0) {
                    if(e.type==SDL_QUIT) {
                        *menu=5;
                        f=true;
                    }
                    else if (e.type == SDL_KEYDOWN) { //Если пользователь воспользовался быстрыми клавишами действий
                        if (keyboard_state_array[SDL_SCANCODE_RETURN]) f=true; //Пользователь захотел вернуться в главное меню
                    }
                    else if (e.type==SDL_MOUSEBUTTONDOWN) { //Пользователь сделал действие мышкой
                            if(button[0].Check(x,y)) f=true;
                            else if(button[1].Check(x,y)) {
                                    if(N!=0) Clear();
                            }
                            else if(button[2].Check(x,y)) {
                                    if(N!=0) PrintFile();
                            }
                    }
            }
            SDL_Delay(deltaTime);
            SDL_RenderPresent(gRenderer);
        } while(!f);
    }
};

