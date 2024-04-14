#pragma once

class LButton: public LOutput_text { //����� ������
    SDL_Texture* mTexture_Active;
    bool f;
    int FixedX;
    int FixedY;
public:
    LButton() {
        mTexture_Active=NULL;
        f=false; //�� ��������� ������ ������ �����, �� ���� �� �������
        FixedX=0;
        FixedY=0;
    }
    ~LButton() {
        free();
    }
    void free() {
        if (mTexture_Active != NULL) {//�������� �� ������� �������� � ������
            SDL_DestroyTexture(mTexture_Active);
            mTexture_Active = NULL;
        }
    }
    bool Load (std::string textureText) {
        if(!LOutput_text::Load(textureText, false)) {
            return false;
        }
        free(); //������� ����������� �� ����� �������������� ��������
        //��������� ��������� �����������
        SDL_Surface* textSurface = TTF_RenderUTF8_Solid(gFont, textureText.c_str(), {255,0,0});
        if (textSurface == NULL) {
            std::cout << "����� LButton: ������! ��������� ����������� �� ����� ���� ����������!" << std::endl;
            std::cout << "���������: " << TTF_GetError() << std::endl;
        }
        else {
            //�������� �������� �� �������� �����������
            mTexture_Active = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            if (mTexture_Active == NULL) {
                std::cout << "����� LButton: ������! ���������� ������� �������� �� ������������� ������!" << std::endl;
                std::cout << "���������: " << SDL_GetError() << std::endl;
            } //�������� ������� �� �����, �.�. ��� ������� ����� ������������� ������
        }
        SDL_FreeSurface(textSurface);
        return mTexture_Active!=NULL;
    }
    void Render(int x, int y) {
        FixedX=x;
        FixedY=y;
        if(f) {
            SDL_Rect renderQuad = { x, y, mWidth, mHeight }; //���������� ������������ ��� ���������� � �������� �� �����
            SDL_RenderCopy(gRenderer, mTexture_Active, NULL, &renderQuad); //����� �� �����
            f=false;
        } else LOutput_text::Render(x,y);
    }
    bool Check(int x, int y) {
        if(FixedX<=x && x<=FixedX+getWidth() && FixedY<=y && y<=(FixedY+getHeight())) f=true;
        else f=false;
        return f;
    }
};
