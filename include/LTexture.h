#pragma once
extern SDL_Renderer *gRenderer;

class LTexture { //Класс текстуры изображения
    SDL_Rect clip;
public:
    SDL_Texture* mTexture; //Текстура
    int mWidth;            //Ширина текстуры
    int mHeight;           //Высота текстуры

    LTexture() {//Инициализация
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        clip.x = 0;
        clip.y = 0;
        clip.w = 0;
        clip.h = 0;
    }
    ~LTexture() {//Деструктор
        free();
    }
    bool Load(std::string path) { //Метод загрузки изображения виде текстуры
        //path - путь к изображению, которое надо загрузить как текстуру
        //Избавляемся от ранее существовавшей текстуры
        free();
        //Окончательная текстура
        SDL_Texture* newTexture = NULL;
        //Загрузить изображение по указанному пути
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (loadedSurface == NULL) {
            std::cout << "Класс LTexture: Ошибка! Не удалось загрузить изображение с " << path.c_str() << std::endl;
            std::cout << "Подробнее: " << IMG_GetError() << std::endl;
        }
        else {
            //Установка цветового ключа
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
            //Создание изображения из пикселей поверхности
            newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
            if (newTexture == NULL) {
                    std::cout << "Класс LTexture: Ошибка! Невозможно создать текстуру из " << path.c_str() << std::endl;
                    std::cout << "Подробнее: " << IMG_GetError() << std::endl;
            }
            else {
                //Получение размеров текстуры
                mWidth = loadedSurface->w;
                mHeight = loadedSurface->h;
            }
            //Избавиться от старой загруженной поверхности
            SDL_FreeSurface(loadedSurface);
        }
        //Вывод результата
        mTexture = newTexture;
        return mTexture != NULL;
    }
    void free() {//Освобождение текстуры, если она существует
        if (mTexture != NULL) {//Проверка на наличие текстуры в классе
            SDL_DestroyTexture(mTexture);
            mTexture = NULL;
            mWidth = 0;
            mHeight = 0;
        }
    }
    void Render(int x, int y) { //Функция рендеринга текстуры
        //Установите пространство для рендеринга и выводите на экран
        SDL_Rect renderQuad = { x, y, mWidth, mHeight };
        //Установка размеров рендеринга клипа
        if (clip.w!=0 && clip.h!=0) {
            renderQuad.w = clip.w;
            renderQuad.h = clip.h;
        }
        //Вывод на экран
        SDL_RenderCopy(gRenderer, mTexture, &clip, &renderQuad);
    }
    int getWidth() { //Метод получения ширины текстуры
        return mWidth;
    }
    int getHeight() { //Метод получения высоты текстуры
        return mHeight;
    }
    void setRect(int w,int h) {
        clip.w = w;
        clip.h = h;
    }
};

