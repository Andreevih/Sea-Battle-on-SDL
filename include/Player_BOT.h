#pragma once

class Player_Bot: public Player { //Это класс игрока, управляемого компьютером
    int *Search;
    int SIZE_Search;
    std::string name;
public:
    Player_Bot() {
       SIZE_Search=0;
       Search=new int[Number_ships-1];
       for (int i=0; i<Number_ships-1; i++) Search[i]=0;
    }
    ~Player_Bot() {
       delete [] Search;
       std::cout << "Класс Player_BOT: Деструктор закончил свою работу!" << std::endl;
    }
    void ReistrName() {
        int x, i, N=10;
        std::string Var[N]={"Врунгель",
                           "НЕМО",
                           "Воробей",
                           "поБЕДА",
                           "РАЗРАБ",
                           "ИИ++",
                           "Jhon Silver",
                           "Good boy",
                           "Отличник",
                           "Смоллет",
                          };
        x=rand()%10;
        for(i=0; i<N; i++)
            if(x==i) {
                name=Var[i];
                break;
            }
    }
    std::string GetName() {
        return name;
    }
    void Program_Creation() { //Это метод, программа заполняет поле кораблями
        int x,y,i,j,k;
        int I, J;
        for(i=0; i<Number_ships; i++)
            for(j=0; j<Number_ships-i; j++) {
                do {
                    //Этап 1: Выбираем начальную ячейку и проверяем, не заполнена ли она кораблем
                    do { x=rand()%100; } while(Field[x/10][x%10]!=0); //Генерируем номер начальной ячейки, от которой программа будет ставить корабль
                    //Этап 2: Выбрать расположение корабля относительно начальной ячейки так, чтобы не нарушить правила игры
                    y=rand()%4; //Генерируем решение, как расположить корабль из 4 вариантов
                    for(k=0; k<i+1 && y!=4; k++) { //Далее проверяем пространство вокруг ячеек
                        switch(y) {
                            case 0: I=x/10-k; J=x%10; break;
                            case 1: I=x/10+k; J=x%10; break;
                            case 2: I=x/10;   J=x%10-k; break;
                            case 3: I=x/10;   J=x%10+k; break;
                        }
                        //Проверяем не вышли ли за пределы поля
                        if(I<0 || I>=SIZE || J<0 || J>=SIZE) y=4;
                        else switch(I) {
                            case 0: {
                                if(Field[I+1][J]!=0 ||
                                   (J!=0 && (Field[I][J-1]!=0  ||
                                             Field[I+1][J-1]!=0)) ||
                                   (J!=9 && (Field[I][J+1]!=0 ||
                                             Field[I+1][J+1]!=0))) y=4;
                                break;
                            }
                            case 9: {
                                if(Field[I-1][J]!=0 ||
                                   (J!=0 && (Field[I][J-1]!=0  ||
                                             Field[I-1][J-1]!=0)) ||
                                   (J!=9 && (Field[I][J+1]!=0 ||
                                             Field[I-1][J+1]!=0))) y=4;
                                break;
                            }
                            default: {
                                if(Field[I-1][J]!=0 ||
                                   Field[I+1][J]!=0 ||
                                   (J!=0 && (Field[I][J-1]!=0     ||
                                             Field[I-1][J-1]!=0   ||
                                             Field[I+1][J-1]!=0)) ||
                                   (J!=9 && (Field[I][J+1]!=0   ||
                                             Field[I-1][J+1]!=0 ||
                                             Field[I+1][J+1]!=0))) y=4;
                            }
                        }
                    }
                } while(y==4);
                //Этап 3: Если все в порядке, то заполняем пространство кораблем и регестрируем его
                for(k=0; k<i+1; k++) {
                        switch(y) {
                            case 0: I=x/10-k; J=x%10; break;
                            case 1: I=x/10+k; J=x%10; break;
                            case 2: I=x/10;   J=x%10-k; break;
                            case 3: I=x/10;   J=x%10+k; break;
                        }
                        SetRecord(I*10+J, i*10+j+11);
                        List_ships[i][j][k]=I*10+J;
                }
            }
    }
    int Program_Makemove() { //Это метод, когда программа делает ход
        int x;
        switch(SIZE_Search) {
            case 0: x=rand()%100; break; //Если нет информации о нахождении корабля, то делает ход на любую клетку
            case 1: { //Если известна только одна ячейка с кораблем, то поиски ведутся по 4 направлениям
                    do {
                        x=rand()%4;
                        switch(x) {
                            case 0: x=(Search[0]/10-1)*10+Search[0]%10; break;
                            case 1: x=(Search[0]/10+1)*10+Search[0]%10; break;
                            case 2: x=Search[0]-1; break;
                            case 3: x=Search[0]+1; break;
                        }
                    } while(x<0 || x>99 || (Search[0]%10==9 && x%10==0) || (Search[0]%10==0 && x%10==9));
                    break;
                }
            default: { //Если известны две и более ячейек с кораблем, то поиски ведутся по 2 направлениям
                do {
                    x=rand()%2;
                    switch(x) {
                        case 0: {
                            x=2*Search[0]-Search[1];
                            if(x<0 || (Search[0]%10==0 && x%10==9)) x=-1;
                            break;
                        }
                        case 1: {
                            //for(i=0; i<SIZE_Search; i++); i--;
                            x=Search[SIZE_Search-1]+Search[1]-Search[0];
                            if(x>99 || (Search[SIZE_Search-1]%10==9 && x%10==0)) x=-1;
                        }
                    }
                } while(x==-1);
                break;
            }
        }
        return x;
    }
    bool Progress (int x, int f) {//Это метод записи прогресса игрока в уничтожении кораблей
        int i,j;
        switch(f) {
            case 1: { //Если попали, но не уничтожили корабль
                Search[SIZE_Search]=x; //Записываем координату корабля
                SIZE_Search+=1;
                //Далее делаем сортировку по возрастанию методом пузырька
                for(i=0; i<SIZE_Search-1; i++)
                    for(j=SIZE_Search-2; j>=i; j--)
                        if(Search[j]>Search[j+1]) {
                            x=Search[j];
                            Search[j]=Search[j+1];
                            Search[j+1]=x;
                        }
                break;
            }
            case 2: { //Если попали и уничтожили корабль
                for(i=0; i<Number_ships-1;i++) Search[i]=0;
                SIZE_Search=0;
            }
        }
        if(f>0) return true;
        else return false;
    }
};

