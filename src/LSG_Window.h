#include "main.h"

#ifndef LSG_WINDOW_H
#define LSG_WINDOW_H

class LSG_Window
{
private:
    LSG_Window()  {}
    ~LSG_Window() {}

private:
    static SDL_Renderer* renderer;
    static SDL_Window*   window;

    #if defined _windows
        static std::vector<std::wstring> openFiles(bool   allowMultipleSelection = false);
        static std::vector<std::wstring> openFolders(bool allowMultipleSelection = false);
    #else
        static std::vector<std::string> openFiles(bool openFolder = false, bool allowMultipleSelection = false);
    #endif

public:
    static void          Close();
    static SDL_Size      GetMinimumSize();
    static SDL_Point     GetPosition();
    static SDL_Size      GetSize();
    static std::string   GetTitle();
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
        static std::wstring              OpenFile();
        static std::wstring              OpenFolder();
        static std::vector<std::wstring> OpenFiles();
        static std::vector<std::wstring> OpenFolders();
    #else
        static std::string              OpenFile();
        static std::string              OpenFolder();
        static std::vector<std::string> OpenFiles();
        static std::vector<std::string> OpenFolders();
    #endif

};

#endif
