#include "LSG_Window.h"

SDL_Renderer* LSG_Window::renderer = nullptr;
SDL_Window*   LSG_Window::window   = nullptr;

void LSG_Window::Close()
{
	if (LSG_Window::renderer) {
		SDL_DestroyRenderer(LSG_Window::renderer);
		LSG_Window::renderer = nullptr;
	}

	if (LSG_Window::window) {
		SDL_DestroyWindow(LSG_Window::window);
		LSG_Window::window = nullptr;
	}

	SDL_Quit();
}

SDL_Size LSG_Window::GetMinimumSize()
{
	SDL_Size size = {};
	SDL_GetWindowMinimumSize(LSG_Window::window, &size.width, &size.height);

	return size;
}

SDL_Point LSG_Window::GetPosition()
{
	SDL_Point position = {};
	SDL_GetWindowPosition(LSG_Window::window, &position.x, &position.y);

	return position;
}

SDL_Size LSG_Window::GetSize()
{
	SDL_Size size = {};
	SDL_GetRendererOutputSize(LSG_Window::renderer, &size.width, &size.height);

	return size;
}

std::string LSG_Window::GetTitle()
{
	return SDL_GetWindowTitle(LSG_Window::window);
}

bool LSG_Window::IsMaximized()
{
	return (SDL_GetWindowFlags(LSG_Window::window) & SDL_WINDOW_MAXIMIZED);
}

/**
 * @throws exception
 */
SDL_Renderer* LSG_Window::Open(const std::string& title, int width, int height)
{
	const uint32_t FLAGS = (SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

	LSG_Window::window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, FLAGS);

    if (!LSG_Window::window)
        throw std::exception(std::format("Failed to create a window: {}", SDL_GetError()).c_str());

	SDL_SetWindowMinimumSize(LSG_Window::window, LSG_WINDOW_MIN_SIZE, LSG_WINDOW_MIN_SIZE);

	LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, -1, SDL_RENDERER_ACCELERATED);

    if (!LSG_Window::renderer)
		LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, -1, SDL_RENDERER_SOFTWARE);

    if (!LSG_Window::renderer)
        throw std::exception(std::format("Failed to create a renderer: {}", SDL_GetError()).c_str());

	return LSG_Window::renderer;
}

#if defined _linux
std::string LSG_Window::OpenFile(bool openFolder)
{
	if (strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv("DISPLAY", ":0", 1);

	if (!gtk_init_check(0, nullptr))
		return "";

	GtkWidget* dialog = gtk_file_chooser_dialog_new(
		(openFolder ? "Select a folder" : "Select a file"),
		nullptr,
		(openFolder ? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN,
		GTK_RESPONSE_ACCEPT,
		nullptr
	);

	std::string filePath = "";

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char* selectedPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		if (selectedPath) {
			filePath = std::string(selectedPath);
			g_free(selectedPath);
		}
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

	while (gtk_events_pending())
		gtk_main_iteration();

	if (filePath.substr(0, 7) == "file://")
		filePath = filePath.substr(7);

	return filePath;
}
#elif defined _macosx
std::string LSG_Window::OpenFile(bool openFolder)
{
	NSOpenPanel* panel = [NSOpenPanel openPanel];

	if (!panel)
		return "";

	[panel setAllowsMultipleSelection: NO];
	[panel setCanChooseDirectories: (openFolder ? YES : NO)];
	[panel setCanChooseFiles: (openFolder ? NO : YES)];

	if ([panel runModal] != NSOKButton)
		return "";

	CFURLRef selectedURL = (CFURLRef)[[panel URLs]firstObject];

	if (!selectedURL)
		return "";

	std::string filePath               = "";
	char        selectedPath[MAX_PATH] = {};

	if (CFURLGetFileSystemRepresentation(selectedURL, TRUE, (UInt8*)selectedPath, MAX_PATH))
		filePath = std::string(selectedPath);

	if (filePath.substr(0, 7) == "file://")
		filePath = filePath.substr(7);

	return filePath;
}
#elif defined _windows
std::wstring LSG_Window::OpenFile()
{
	OPENFILENAMEW browseDialog           = {};
	std::wstring  filePath               = L"";
	wchar_t       selectedPath[MAX_PATH] = {};

	// https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew

	browseDialog.lStructSize  = sizeof(browseDialog);
	browseDialog.lpstrFile    = selectedPath;
	browseDialog.lpstrFile[0] = '\0';
	browseDialog.nMaxFile     = sizeof(selectedPath);
	browseDialog.Flags        = (OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS);

	if (GetOpenFileNameW(&browseDialog))
		filePath = std::wstring(selectedPath);

	if (filePath.substr(0, 7) == L"file://")
		filePath = filePath.substr(7);

	return filePath;
}
#endif

#if defined _windows
std::wstring LSG_Window::OpenFolder()
{
	IFileDialog* browseDialog = nullptr;

	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&browseDialog))) || !browseDialog)
		return L"";

	// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions

	browseDialog->SetOptions(FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_PICKFOLDERS);

	std::wstring folderPath = L"";
	IShellItem*  shellItem  = nullptr;

	if (SUCCEEDED(browseDialog->Show(nullptr)) && SUCCEEDED(browseDialog->GetResult(&shellItem)))
	{
		LPWSTR selectedPath = nullptr;

		if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &selectedPath)) && selectedPath)
			folderPath = std::wstring(selectedPath);

		if (selectedPath)
			CoTaskMemFree(selectedPath);
	}

	if (shellItem)
		shellItem->Release();

	if (browseDialog)
		browseDialog->Release();

	return folderPath;
}
#else
std::string LSG_Window::OpenFolder()
{
	return LSG_Window::OpenFile(true);
}
#endif

void LSG_Window::Present()
{
	SDL_RenderPresent(LSG_Window::renderer);
}

void LSG_Window::Render()
{
	auto backgroundColor = LSG_UI::GetBackgroundColor();

	SDL_SetRenderTarget(LSG_Window::renderer, nullptr);
	SDL_SetRenderDrawColor(LSG_Window::renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
	SDL_RenderClear(LSG_Window::renderer);

	LSG_UI::Render(LSG_Window::renderer);
}

void LSG_Window::SetMaximized(bool maximized)
{
	if (maximized)
		SDL_MaximizeWindow(LSG_Window::window);
	else
		SDL_RestoreWindow(LSG_Window::window);
}

void LSG_Window::SetMinimumSize(int width, int height)
{
	SDL_SetWindowMinimumSize(LSG_Window::window, width, height);
}

void LSG_Window::SetPosition(int x, int y)
{
	SDL_SetWindowPosition(LSG_Window::window, x, y);
}

void LSG_Window::SetSize(int width, int height)
{
	SDL_SetWindowSize(LSG_Window::window, width, height);
}

void LSG_Window::SetTitle(const std::string& title)
{
	SDL_SetWindowTitle(LSG_Window::window, title.c_str());
}

void LSG_Window::ShowMessage(const std::string& message, uint32_t flags)
{
	SDL_ShowSimpleMessageBox(flags, LSG_Window::GetTitle().c_str(), message.c_str(), LSG_Window::window);
}

SDL_Texture* LSG_Window::ToTexture(const std::string& imageFile)
{
	if (imageFile.empty())
		return nullptr;

	#if defined _windows && defined _DEBUG
		auto filePath = (imageFile.size() > 1 && imageFile[1] != ':' ? std::format("Debug/{}", imageFile) : imageFile);
		auto texture  = IMG_LoadTexture(LSG_Window::renderer, filePath.c_str());
	#else
		auto texture = IMG_LoadTexture(LSG_Window::renderer, imageFile.c_str());
	#endif

	if (!texture)
		throw std::exception(std::format("Failed to load Image file: {}", imageFile).c_str());

	return texture;
}

SDL_Texture* LSG_Window::ToTexture(SDL_Surface* surface)
{
	if (!surface)
		return nullptr;

	return SDL_CreateTextureFromSurface(LSG_Window::renderer, surface);
}

