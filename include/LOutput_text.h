#pragma once
extern SDL_Renderer *gRenderer;
extern TTF_Font* gFont;

class LOutput_text: public LTexture { //Класс текста
public:
     bool Load (std::string textureText, bool f) { //Функция рендеринга текстуры
        SDL_Surface *textSurface;
        free(); //Вначале избавляемся от ранее существовавшей текстуры
        //Рендеринг текстовой поверхности
        if(!f) textSurface = TTF_RenderUTF8_Solid(gFont, textureText.c_str(), {255,255,255});
        else   textSurface = TTF_RenderUTF8_Solid(gFont, textureText.c_str(), {255,0,0});
        if (textSurface == NULL) {
            std::cout << "Класс LOutput_text: Ошибка! Не удалось загрузить изображение." << std::endl;
            std::cout << "Подробнее: " << IMG_GetError() << std::endl;
        }
        else {
            //Создание текстуры из пикселей поверхности
            mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            if (mTexture == NULL) {
                std::cout << "Класс LOutput_text: Ошибка! Невозможно создать текстуру." << std::endl;
                std::cout << "Подробнее: " << IMG_GetError() << std::endl;
            }
            else { //Получение размеров изображения
                mWidth = textSurface->w;
                mHeight = textSurface->h;
            } //Избавляемся от старой поверхности
        }
        SDL_FreeSurface(textSurface);
        //Выводим успех
        return mTexture != NULL;
    }
     void Render(int x, int y) { //Функция рендеринга текстуры
        SDL_Rect renderQuad = { x, y, mWidth, mHeight }; //Установите пространство для рендеринга и выводите на экран
        SDL_RenderCopy(gRenderer, mTexture, NULL, &renderQuad); //Вывод на экран
    }
};

