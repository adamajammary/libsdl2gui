#include "main.h"

#ifndef LSG_WINDOW_H
#define LSG_WINDOW_H

class LSG_Window
{
private:
    LSG_Window()  {}
    ~LSG_Window() {}

public:
    static const int MinSize = 400;

private:
    static SDL_Renderer* renderer;
    static SDL_Window*   window;

    #if defined _windows
        static std::vector<std::wstring> openFiles(bool allowMultipleSelection, const std::wstring& filter);
        static std::vector<std::wstring> openFolders(bool allowMultipleSelection);
    #elif defined _linux || defined _macosx
        static std::vector<std::string> openFiles(bool openFolder, bool allowMultipleSelection, const std::vector<std::string>& filters);
    #endif

public:
    static void          Close();
    static float         GetDPI();
    static SDL_Size      GetMinimumSize();
    static SDL_Point     GetMousePosition();
    static SDL_Point     GetPosition();
    static SDL_Size      GetSize();
    static SDL_FPoint    GetSizeScale();
    static std::string   GetTitle();
    static void          InitRenderTarget(SDL_Texture** renderTarget, const SDL_Size& textureSize);
    static bool          IsMaximized();
    static SDL_Renderer* Open(const std::string& title, int width, int height);
    static void          Present();
    static void          Render();
    static void          SetMaximized(bool maximized = true);
    static void          SetMinimumSize(int width, int height);
    static void          SetPosition(int x, int y);
    static void          SetSize(int width, int height);
    static void          SetTitle(const std::string& title);
    static void          ShowMessage(const std::string& message, uint32_t flags = SDL_MESSAGEBOX_ERROR);
    static SDL_Texture*  ToTexture(const std::string& imageFile);
    static SDL_Texture*  ToTexture(SDL_Surface* surface);

    #if defined _windows
        static std::wstring              OpenFile(const std::wstring& filter);
        static std::vector<std::wstring> OpenFiles(const std::wstring& filter);
        static std::wstring              OpenFolder();
        static std::vector<std::wstring> OpenFolders();
        static std::wstring              SaveFile(const std::wstring& filter);
    #elif defined _linux || defined _macosx
        static std::string              OpenFile(const std::vector<std::string>& filters);
        static std::vector<std::string> OpenFiles(const std::vector<std::string>& filters);
        static std::string              OpenFolder();
        static std::vector<std::string> OpenFolders();
        static std::string              SaveFile(const std::vector<std::string>& filters);
    #endif

    #if defined _windows && defined _DEBUG
        static void OpenTest();
    #endif

};

#endif
