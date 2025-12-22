#include "main.h"

#ifndef LSG_WINDOW_H
#define LSG_WINDOW_H

class LSG_Window
{
private:
    LSG_Window()  {}
    ~LSG_Window() {}

private:
    static float         dpiScale;
    static SDL_Renderer* renderer;
    static SDL_SysWMinfo sysWmInfo;
    static SDL_Window*   window;

public:
    static void          Close();
    static int           GetDPIScaled(int value);
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
    static SDL_Texture*  RotateTexture(SDL_Texture* texture, const LSG_ImageOrientation& orientation, const SDL_Size& size, uint32_t format);
    static void          SetDPIScale();
    static void          SetMaximized(bool maximized = true);
    static void          SetMinimumSize(int width, int height);
    static void          SetPosition(int x, int y);
    static void          SetSize(int width, int height);
    static void          SetTitle(const std::string& title);
    static void          ShowMessage(const std::string& message, uint32_t flags = SDL_MESSAGEBOX_ERROR);
    static SDL_Texture*  ToTexture(const std::string& imageFile);
    static SDL_Texture*  ToTexture(SDL_Surface* surface);
    static SDL_Texture*  ToTextureEmpty(SDL_Surface* surface);

    #if defined _windows
        static std::wstring              OpenFile(const LSG_Strings& filters);
        static std::vector<std::wstring> OpenFiles(const LSG_Strings& filters);
        static std::wstring              OpenFolder();
        static std::vector<std::wstring> OpenFolders();
        static std::wstring              SaveFile(const LSG_Strings& filters);
    #elif defined _linux || defined _macosx
        static std::string OpenFile(const LSG_Strings& filters);
        static LSG_Strings OpenFiles(const LSG_Strings& filters);
        static std::string OpenFolder();
        static LSG_Strings OpenFolders();
        static std::string SaveFile(const LSG_Strings& filters);
    #elif defined _android
        static float       getDPIScale();
        static std::string OpenFile(const LSG_Strings& filters);
        static std::string OpenFolder();
        static std::string SaveFile(const LSG_Strings& filters);
    #elif defined _ios
        static void OpenFileDocuments(std::function<void(NSArray<NSURL*>*)> resultsCallback, bool allowMultipleSelection);
        static void OpenFileMedia(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback, bool allowMultipleSelection);
        static void OpenFilePhotos(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback, bool allowMultipleSelection);
        static void OpenFolder(std::function<void(NSArray<NSURL*>*)> resultsCallback);
    #endif

    #if defined _windows && defined _DEBUG
        static void OpenTest();
    #endif

private:
    #if defined _windows
        static std::vector<std::wstring> getFiltersWide(const LSG_Strings& filters);
        static std::vector<std::wstring> openFiles(bool allowMultipleSelection, const LSG_Strings& filters);
        static std::vector<std::wstring> openFolders(bool allowMultipleSelection);
    #elif defined _linux || defined _macosx
        static LSG_Strings openFiles(bool openFolder, bool allowMultipleSelection, const LSG_Strings& filters);
    #elif defined _android
        static std::string pickFile(const LSG_Strings& filters, bool saveFile = false);
    #endif
};

#endif
