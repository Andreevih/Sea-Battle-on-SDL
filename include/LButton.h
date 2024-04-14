#pragma once

class LButton: public LOutput_text { //Класс кнопки
    SDL_Texture* mTexture_Active;
    bool f;
    int FixedX;
    int FixedY;
public:
    LButton() {
        mTexture_Active=NULL;
        f=false; //По умолчанию кнопка белого цвета, то есть не активна
        FixedX=0;
        FixedY=0;
    }
    ~LButton() {
        free();
    }
    void free() {
        if (mTexture_Active != NULL) {//Проверка на наличие текстуры в классе
            SDL_DestroyTexture(mTexture_Active);
            mTexture_Active = NULL;
        }
    }
    bool Load (std::string textureText) {
        if(!LOutput_text::Load(textureText, false)) {
            return false;
        }
        free(); //Вначале избавляемся от ранее существовавшей текстуры
        //Рендеринг текстовой поверхности
        SDL_Surface* textSurface = TTF_RenderUTF8_Solid(gFont, textureText.c_str(), {255,0,0});
        if (textSurface == NULL) {
            std::cout << "Класс LButton: Ошибка! Текстовая поверхность не может быть отображена!" << std::endl;
            std::cout << "Подробнее: " << TTF_GetError() << std::endl;
        }
        else {
            //Создание текстуры из пикселей поверхности
            mTexture_Active = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            if (mTexture_Active == NULL) {
                std::cout << "Класс LButton: Ошибка! Невозможно создать текстуру из отрисованного текста!" << std::endl;
                std::cout << "Подробнее: " << SDL_GetError() << std::endl;
            } //Получать размеры не нужно, т.к. это сделает метод родительского класса
        }
        SDL_FreeSurface(textSurface);
        return mTexture_Active!=NULL;
    }
    void Render(int x, int y) {
        FixedX=x;
        FixedY=y;
        if(f) {
            SDL_Rect renderQuad = { x, y, mWidth, mHeight }; //Установите пространство для рендеринга и выводите на экран
            SDL_RenderCopy(gRenderer, mTexture_Active, NULL, &renderQuad); //Вывод на экран
            f=false;
        } else LOutput_text::Render(x,y);
    }
    bool Check(int x, int y) {
        if(FixedX<=x && x<=FixedX+getWidth() && FixedY<=y && y<=(FixedY+getHeight())) f=true;
        else f=false;
        return f;
    }
};
