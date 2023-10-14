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

SDL_Point LSG_Window::GetMousePosition()
{
	SDL_Point mousePosition = {};
	SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

	return mousePosition;
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
 * @throws runtime_error
 */
SDL_Renderer* LSG_Window::Open(const std::string& title, int width, int height)
{
	LSG_Window::window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		(SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE)
	);

    if (!LSG_Window::window)
        throw std::runtime_error(LSG_Text::Format("Failed to create a window: %s", SDL_GetError()));

	SDL_SetWindowMinimumSize(LSG_Window::window, LSG_Const::WindowMinSize, LSG_Const::WindowMinSize);

	LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, -1, SDL_RENDERER_ACCELERATED);

    if (!LSG_Window::renderer)
		LSG_Window::renderer = SDL_CreateRenderer(LSG_Window::window, -1, SDL_RENDERER_SOFTWARE);

    if (!LSG_Window::renderer)
        throw std::runtime_error(LSG_Text::Format("Failed to create a renderer: %s", SDL_GetError()));

	return LSG_Window::renderer;
}

#if defined _linux
std::vector<std::string> LSG_Window::openFiles(bool openFolder, bool allowMultipleSelection)
{
	if (strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv("DISPLAY", ":0", 1);

	if (!gtk_init_check(0, nullptr))
		return {};

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

	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), allowMultipleSelection);

	std::vector<std::string> filePaths;

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		GSList* paths = nullptr;

		for (paths = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog)); paths != nullptr; paths = paths->next)
		{
			auto selectedPath = (gchar*)paths->data;

			if (!selectedPath)
				continue;

			auto filePath = std::string(selectedPath);

			if (filePath.substr(0, 7) == "file://")
				filePath = filePath.substr(7);

			if (!filePath.empty())
				filePaths.push_back(filePath);

			g_free(selectedPath);
		}

		if (paths)
			g_slist_free(paths);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

	while (gtk_events_pending())
		gtk_main_iteration();

	return filePaths;
}
#elif defined _macosx
std::vector<std::string> LSG_Window::openFiles(bool openFolder, bool allowMultipleSelection)
{
	NSOpenPanel* panel = [NSOpenPanel openPanel];

	if (!panel)
		return {};

	[panel setAllowsMultipleSelection: (allowMultipleSelection ? YES : NO)];
	[panel setCanChooseDirectories: (openFolder ? YES : NO)];
	[panel setCanChooseFiles: (openFolder ? NO : YES)];

	if ([panel runModal] != NSOKButton)
		return {};

	const int MAX_FILE_PATH = 260;

	std::vector<std::string> filePaths;

	for (id url in [panel URLs])
	{
		CFURLRef selectedURL = (CFURLRef)url;

		if (!selectedURL)
			continue;

		char selectedPath[MAX_FILE_PATH] = {};

		if (!CFURLGetFileSystemRepresentation(selectedURL, TRUE, (UInt8*)selectedPath, MAX_FILE_PATH))
			continue;

		auto filePath = std::string(selectedPath);

		if (filePath.substr(0, 7) == "file://")
			filePath = filePath.substr(7);

		if (!filePath.empty())
			filePaths.push_back(filePath);
	}

	return filePaths;
}
#elif defined _windows
std::vector<std::wstring> LSG_Window::openFiles(const wchar_t* filter, bool allowMultipleSelection)
{
	const int MAX_FILE_PATH = 2048;

	OPENFILENAMEW browseDialog                = {};
	wchar_t       selectedPath[MAX_FILE_PATH] = {};

	// https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew

	browseDialog.lStructSize  = sizeof(browseDialog);
	browseDialog.lpstrFile    = selectedPath;
	browseDialog.lpstrFile[0] = '\0';
	browseDialog.nMaxFile     = sizeof(selectedPath);
	browseDialog.Flags        = (OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS);
	browseDialog.lpstrFilter  = L"All Files (*)\0*.*\00";
	browseDialog.nFilterIndex = 1;

	if (allowMultipleSelection)
		browseDialog.Flags |= OFN_ALLOWMULTISELECT;

	if (filter && (wcslen(filter) > 0))
		browseDialog.lpstrFilter = filter;

	if (!GetOpenFileNameW(&browseDialog))
		return {};

	std::wstring directory = L"";

	for (int i = 0; i < browseDialog.nFileOffset - 1; i++)
		directory.push_back(selectedPath[i]);

	std::vector<std::wstring> filePaths;

	std::wstring filePath = L"";

	for (int i = browseDialog.nFileOffset; i < MAX_FILE_PATH; i++)
	{
		if (selectedPath[i] != '\0') {
			filePath.push_back(selectedPath[i]);
			continue;
		}

		if (filePath.empty())
			break;

		if (directory.ends_with('\\'))
			filePaths.push_back(LSG_Text::FormatW(L"%s%s", directory.c_str(), filePath.c_str()));
		else
			filePaths.push_back(LSG_Text::FormatW(L"%s\\%s", directory.c_str(), filePath.c_str()));

		filePath.clear();
	}

	return filePaths;
}
#endif

#if defined _windows
std::wstring LSG_Window::OpenFile(const wchar_t* filter)
{
	auto files = LSG_Window::openFiles(filter);

	return (!files.empty() ? files[0] : L"");
}

std::vector<std::wstring> LSG_Window::OpenFiles(const wchar_t* filter)
{
	return LSG_Window::openFiles(filter, true);
}

std::vector<std::wstring> LSG_Window::openFolders(bool allowMultipleSelection)
{
	IFileOpenDialog* browseDialog = nullptr;

	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&browseDialog))) || !browseDialog)
		return {};

	// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/ne-shobjidl_core-_fileopendialogoptions
	// https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nn-shobjidl_core-ifileopendialog

	auto options = (FOS_DONTADDTORECENT | FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST | FOS_PICKFOLDERS);

	if (allowMultipleSelection)
		options |= FOS_ALLOWMULTISELECT;

	browseDialog->SetOptions(options);

	std::vector<std::wstring> folderPaths;

	IShellItemArray* shellItems     = nullptr;
	DWORD            shellItemCount = 0;

	if (SUCCEEDED(browseDialog->Show(nullptr)) && SUCCEEDED(browseDialog->GetResults(&shellItems)) && SUCCEEDED(shellItems->GetCount(&shellItemCount)))
	{
		for (DWORD i = 0; i < shellItemCount; i++)
		{
			IShellItem* shellItem = nullptr;

			if (FAILED(shellItems->GetItemAt(i, &shellItem)))
				continue;

			LPWSTR selectedPath = nullptr;

			if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &selectedPath)) && selectedPath)
				folderPaths.push_back(std::wstring(selectedPath));

			if (selectedPath)
				CoTaskMemFree(selectedPath);

			if (shellItem)
				shellItem->Release();
		}
	}

	if (shellItems)
		shellItems->Release();

	if (browseDialog)
		browseDialog->Release();

	return folderPaths;
}

std::wstring LSG_Window::OpenFolder()
{
	auto folders = LSG_Window::openFolders();

	return (!folders.empty() ? folders[0] : L"");
}

std::vector<std::wstring> LSG_Window::OpenFolders()
{
	return LSG_Window::openFolders(true);
}
#elif defined _linux || defined _macosx
std::string LSG_Window::OpenFile()
{
	auto files = LSG_Window::openFiles();

	return (!files.empty() ? files[0] : "");
}

std::vector<std::string> LSG_Window::OpenFiles()
{
	return LSG_Window::openFiles(false, true);
}

std::string LSG_Window::OpenFolder()
{
	auto folders = LSG_Window::openFiles(true);

	return (!folders.empty() ? folders[0] : "");
}

std::vector<std::string> LSG_Window::OpenFolders()
{
	return LSG_Window::openFiles(true, true);
}
#endif

void LSG_Window::Present()
{
	LSG_UI::Present(LSG_Window::renderer);
}

void LSG_Window::Render()
{
	auto backgroundColor = LSG_UI::GetBackgroundColor();

	SDL_SetRenderTarget(LSG_Window::renderer, nullptr);
	SDL_SetRenderDrawColor(LSG_Window::renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
	SDL_RenderClear(LSG_Window::renderer);

	LSG_UI::Render(LSG_Window::renderer);
}

#if defined _linux
std::string LSG_Window::SaveFile()
{
	if (strlen(std::getenv("DISPLAY")) == 0)
		SDL_setenv("DISPLAY", ":0", 1);

	if (!gtk_init_check(0, nullptr))
		return "";

	GtkWidget* dialog = gtk_file_chooser_dialog_new(
		"Save File",
		nullptr,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE_AS,
		GTK_RESPONSE_ACCEPT,
		nullptr
	);

	std::string filePath = "";

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar* selectedPath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		filePath = std::string(selectedPath);

		if (filePath.substr(0, 7) == "file://")
			filePath = filePath.substr(7);

		g_free(selectedPath);
	}

	gtk_widget_destroy(GTK_WIDGET(dialog));

	while (gtk_events_pending())
		gtk_main_iteration();

	return filePath;
}
#elif defined _macosx
std::string LSG_Window::SaveFile()
{
	NSSavePanel* panel = [NSSavePanel savePanel];

	if (!panel)
		return "";

	if ([panel runModal] != NSOKButton)
		return "";

	CFURLRef selectedURL = (CFURLRef)[panel URL];

	if (!selectedURL)
		return "";

	const int MAX_FILE_PATH = 260;
	char      selectedPath[MAX_FILE_PATH] = {};

	if (!CFURLGetFileSystemRepresentation(selectedURL, TRUE, (UInt8*)selectedPath, MAX_FILE_PATH))
		return "";

	auto filePath = std::string(selectedPath);

	if (filePath.substr(0, 7) == "file://")
		filePath = filePath.substr(7);

	return filePath;
}
#elif defined _windows
std::wstring LSG_Window::SaveFile(const wchar_t* filter)
{
	OPENFILENAMEW browseDialog           = {};
	wchar_t       selectedPath[MAX_PATH] = {};

	// https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew

	browseDialog.lStructSize  = sizeof(browseDialog);
	browseDialog.lpstrFile    = selectedPath;
	browseDialog.lpstrFile[0] = '\0';
	browseDialog.nMaxFile     = sizeof(selectedPath);
	browseDialog.Flags        = (OFN_CREATEPROMPT | OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS);
	browseDialog.lpstrFilter  = L"All Files (*)\0*.*\00";
	browseDialog.nFilterIndex = 1;

	if (filter && (wcslen(filter) > 0))
		browseDialog.lpstrFilter = filter;

	if (!GetSaveFileNameW(&browseDialog))
		return L"";
	
	return std::wstring(selectedPath);
}
#endif

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

	auto filePath = LSG_Text::GetFullPath(imageFile);
	auto texture  = IMG_LoadTexture(LSG_Window::renderer, filePath.c_str());

	if (!texture)
		throw std::runtime_error(LSG_Text::Format("Failed to load image file: %s", filePath.c_str()));

	return texture;
}

SDL_Texture* LSG_Window::ToTexture(SDL_Surface* surface)
{
	if (!surface)
		return nullptr;

	return SDL_CreateTextureFromSurface(LSG_Window::renderer, surface);
}

