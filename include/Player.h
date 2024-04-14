#pragma once

class Player { //Это класс игрока
public:
    int ***List_ships; //Это матрица - список кораблей
    char **Field;      //А это поле игрока
    int SIZE;          //Размер поля
    int Number_ships;  //Кол-во типов кораблей
    char Order;        //Порядок хода игрока: чет/нечет

    Player() {
        int i, j, k;
        SIZE=10;
        Number_ships=4;
        List_ships = new int**[Number_ships];
        for (i=0; i<Number_ships; i++) {
            List_ships[i] = new int*[Number_ships-i];
            for (j=0; j<Number_ships-i; j++) {
                List_ships[i][j] = new int[i+1];
                for(k=0; k<i+1; k++) List_ships[i][j][k]=-1;
            }
        }
        Field=new char*[SIZE];
        for (i=0; i<SIZE; i++){
            Field[i]=new char[SIZE];
            for (j=0; j<SIZE; j++) Field[i][j]=0;
        }
    }
    ~Player() {
        int i,j;
        for (i=0; i<Number_ships; i++) {
            for (j=0; j<Number_ships-i; j++)
                delete [] List_ships[i][j];
            delete [] List_ships[i];
        }
        delete [] List_ships;
        for (i=0; i<SIZE; i++)
            delete [] Field[i];
        delete [] Field;
        std::cout << "Класс Player: Деструктор закончил свою работу!" << std::endl;
    }
    void SetRecord(int x, char status){//Это метод записи
        Field[x/10][x%10]=status;
    }
    int GetRecord(int x) {//Это метод получения записи
        return Field[x/10][x%10];
    }
    void SetOrder(char x) { //Это метод установки порядка
        Order=x;
    }
    char GetOrder() { //Это метод получения порядка
        return Order;
    }
    bool Overwriting() {//Это метод проверки игрового поля
        int i,j,k,I=0,J;
        bool st=true; //Это флаг проверки игрового поля игрока на правильную расстановку кораблей: 1 - проблем нет, 0 - проблемы были обнаружены.
        bool p;       //Это флаг положения корабля: горизонталь или вертикаль.
        for (i=0; i<SIZE && st; i++) { //Начинаем обход поля по вертикале
            for (j=0; j<SIZE && st; j++) { //Начинаем обход поля по горизонтале
                    k=0;
                    while (i+k<SIZE && Field[i+k][j]==-1) k++; //Если корабль размещен по вертикале
                    if (k==1) {
                            while(j+k<SIZE && Field[i][j+k]==-1) k++; //Если корабль размещен по горизонтале
                            k*=-1;
                    }
                    if (k!=0 && k<=Number_ships && k>=(-1)*Number_ships) {//Далее, если корабль был зафиксирован, то начинаем его регистрацию
                        if(k<0) {p=false; k*=-1;} //Устанавливаем флаг: false значит горизонт, true значит вертикаль
                        else p=true;
                        for (I=0; I<(5-k);I++)
                             if(List_ships[k-1][I][0]==-1) {
                                 for(J=0; J<k; J++) {
                                    if(p) {
                                            List_ships[k-1][I][J]=(i+J)*10+j;
                                            Field[i+J][j]=(k-1)*10+I+11;
                                    }
                                    else  {
                                            List_ships[k-1][I][J]=i*10+(j+J);
                                            Field[i][j+J]=(k-1)*10+I+11;
                                    }
                                 }
                                 break;
                             }
                        if(I==5-k) st=false; //Проверяем также хватило ли места для этого корабля. Если нет - значит пользователь добавил кораблей больше требуемых
                    }
                    else if(k>4) st=false; //Если же корабль был зафиксирован, но его длина больше допустимой, фиксируем ошибку
                    if(Field[i][j]>0) { //Не стоит забывать про проверку на расстояние между кореблями, т.к. по правилам нельзя рядом ставить корабли
                        if ((i+1<SIZE && j+1<SIZE && Field[i+1][j+1]==-1) || (i-1>=0 && j-1>=0 && Field[i-1][j-1]==-1) || (i-1>=0 && j+1<SIZE && Field[i-1][j+1]==-1)
                            || (i+1<SIZE && j-1>=0 && Field[i+1][j-1]==-1)) {
                                st=false;
                        }
                    }


            }
        }
        for(i=0; i<4 && st; i++)
            for(j=0; j<4-i && st; j++) if (List_ships[i][j][0]==-1) st=false;
        if(!st) { //Если ошибка, то стоит вернуть полю прежний вид
            for(i=0; i<SIZE; i++)
                for(j=0; j<SIZE; j++)
                    if(Field[i][j]>0) Field[i][j]=-1;
            for (i=0; i<Number_ships; i++)
                for (j=0; j<Number_ships-i; j++)
                    for (k=0; k<i+1; k++) List_ships[i][j][k]=-1;
        }
        return st;
    }
    void Set_Zone(int x) {
        int i, j, X;
        int Shift[4]={1,9,10,11};
        for(i=0; i<(x/10+1); i++) {
                X=List_ships[x/10][x%10][i];
                for(j=0; j<4; j++) {
                    if (X+Shift[j]<100 && (X%10!=9 || (X+Shift[j])%10!=0)
                       && (X%10!=0 || (X+Shift[j])%10!=9) && Field[(X+Shift[j])/10][(X+Shift[j])%10]==0) Field[(X+Shift[j])/10][(X+Shift[j])%10]=2;
                    if (X-Shift[j]>=0  && (X%10!=9 || (X-Shift[j])%10!=0)
                       && (X%10!=0 || (X-Shift[j])%10!=9) && Field[(X-Shift[j])/10][(X-Shift[j])%10]==0) Field[(X-Shift[j])/10][(X-Shift[j])%10]=2;
                }
        }
    }
    int Hit_Registration(int x) { //Метод записи попадания
        int f=0;
        switch(GetRecord(x)) {
            case 0: {
                SetRecord(x,-1);
                break;
            }
            case 2: {
                SetRecord(x,-2);
                break;
            }
            default: {
                if (GetRecord(x)<0) f=-1; //Значит в ячейку уже стреляли
                else {
                    SetRecord(x,GetRecord(x)*(-1)); //Записываем попадание
                    if(Check_Ship(abs(GetRecord(x))-11)) {
                            Set_Zone(abs(GetRecord(x))-11);
                            f=2; //Попадание с уничтожением корабля
                    }
                    else f=1; //Попадание, но не уничтожение корабля
                }
            }
        }
        return f;
    }
    bool Check() {
        int i,j;
        for (i=0; i<Number_ships; i++) {
            for (j=0; j<Number_ships-i; j++) if(!Check_Ship(i*10+j)) return false;
        }
        return true;
    }
    bool Check_Ship(int x) {
        int X, k;
        for(k=0; k<((x/10)+1); k++) {
            X=List_ships[x/10][x%10][k];
            if(Field[X/10][X%10]>0) return false; //Если корабль ещё наплаву, выводит ложь
        }
        return true; //Если корабль уничтожен, выводит истину
    }
};
