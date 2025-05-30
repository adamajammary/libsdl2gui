# libsdl2gui

## A free cross-platform user interface library using SDL2

Copyright (C) 2021 Adam A. Jammary (Jammary Studio)

libsdl2gui is a free cross-platform user interface library using SDL2.

## 3rd Party Libraries

Library | Version | License
------- | ------- | -------
[SDL2](https://github.com/libsdl-org/SDL) | [2.32.6](https://github.com/libsdl-org/SDL/releases/download/release-2.32.6/SDL2-2.32.6.tar.gz) | [zlib license](https://github.com/libsdl-org/SDL#Zlib-1-ov-file)
[SDL2_image](https://github.com/libsdl-org/SDL_image) | [2.8.8](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-2.8.8.tar.gz) | [zlib license](https://github.com/libsdl-org/SDL_image#Zlib-1-ov-file)
[SDL2_ttf](https://github.com/libsdl-org/SDL_ttf) | [2.24.0](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-2.24.0.tar.gz) | [zlib license](https://github.com/libsdl-org/SDL_ttf#Zlib-1-ov-file)
[libtiff](https://github.com/libsdl-org/libtiff) | [4.7.0](https://github.com/libsdl-org/libtiff/archive/refs/tags/v4.7.0.tar.gz) | [LibTIFF license](https://github.com/libsdl-org/libtiff?tab=License-1-ov-file#readme)
[libwebp](https://github.com/webmproject/libwebp) | [1.5.0](https://github.com/webmproject/libwebp/archive/refs/tags/v1.5.0.tar.gz) | [BSD-3-Clause license](https://github.com/webmproject/libwebp?tab=BSD-3-Clause-1-ov-file#readme)
[libXML2](https://github.com/GNOME/libxml2) | [2.14.3](https://github.com/GNOME/libxml2/archive/refs/tags/v2.14.3.tar.gz) | [MIT License](https://opensource.org/licenses/mit-license.html)

## Platform-dependent Include Headers

Platform | Header | Package
-------- | ------ | -------
Android | android/asset_manager_jni.h | [Android NDK](https://developer.android.com/ndk/downloads)
Android | sys/stat.h | [Android NDK](https://developer.android.com/ndk/downloads)
iOS | MediaPlayer/MediaPlayer.h | Media Player Framework
iOS | Photos/Photos.h | Photos Framework
iOS | PhotosUI/PhotosUI.h | PhotosUI Kit Framework
iOS | StoreKit/StoreKit.h | StoreKit Framework
iOS | UIKit/UIKit.h | UIKit Framework
iOS | UniformTypeIdentifiers/UniformTypeIdentifiers.h | Uniform Type Identifiers framework
Linux | gtk/gtk.h | libgtk-3-dev
macOS | AppKit/AppKit.h | AppKit Framework
macOS | UniformTypeIdentifiers/UniformTypeIdentifiers.h | Uniform Type Identifiers framework
Windows | shobjidl_core.h | Win32 API

## Compilers and C++20

libsdlgui uses modern [C++20](https://en.cppreference.com/w/cpp/compiler_support#cpp20) features and requires the following minimum compiler versions.

Compiler | Version
-------- | -------
CLANG | 14
GCC | 13
MSVC | 2019

## How to build

1. Build the [third-party libraries](#3rd-party-libraries) and place the them in a common directory.
   - You will also need [patchelf](https://github.com/NixOS/patchelf) if you are building on **Linux**.
1. Make sure you have [cmake](https://cmake.org/download/) installed.
1. Open a command prompt or terminal.
1. Create a **build** directory and enter it.
1. Run `cmake` to create a **Makefile**, **Xcode** project or **Visual Studio** solution based on your target platform.
1. After building, the **dist** directory will contain all the output resources in the **include**, **lib** and **bin** directories.

```bash
mkdir build
cd build
```

### Android

Make sure you have [Android SDK](https://developer.android.com/studio) and [Android NDK](https://developer.android.com/ndk/downloads) installed.

Make sure the correct Android SDK path is set as either

- an environment variable `ANDROID_HOME=/path/to/ANDROID_SDK` or
- a local property `sdk.dir=/path/to/ANDROID_SDK` in the **android/local.properties** file

> See [Android SDK Command-Line Tools](https://developer.android.com/tools) and [SDL2 Android README](https://wiki.libsdl.org/SDL2/README/android) for more details.

```bash
cmake .. -G "Unix Makefiles" \
-D ANDROID_ABI="arm64-v8a" \
-D ANDROID_HOME="/path/to/ANDROID_SDK" \
-D ANDROID_NDK="/path/to/ANDROID_NDK" \
-D ANDROID_PLATFORM="android-29" \
-D CMAKE_BUILD_TYPE=Release \
-D CMAKE_SYSTEM_NAME="Android" \
-D CMAKE_TOOLCHAIN_FILE="/path/to/ANDROID_NDK/build/cmake/android.toolchain.cmake" \
-D LSG_EXT_LIB_DIR="/path/to/libs"

make
```

#### ADB (Android Debug Bridge)

> See [ADB (Android Debug Bridge)](https://developer.android.com/tools/adb) for more details.

##### Install APK to device

```bash
/path/to/ANDROID_SDK/platform-tools/adb install dist/bin/testsdl2gui-arm64-v8a-debug.apk
```

##### Re-install (update) APK to device

```bash
/path/to/ANDROID_SDK/platform-tools/adb install -r dist/bin/testsdl2gui-arm64-v8a-debug.apk
```

##### Uninstall (remove) APK from device

```bash
/path/to/ANDROID_SDK/platform-tools/adb uninstall com.libsdl2gui.test
```

![Screenshot of Test project on Android](screenshots/android_480p.png)

### iOS

You can get the iOS SDK path with the following command: `xcrun --sdk iphoneos --show-sdk-path`

> See [SDL2 iOS README](https://wiki.libsdl.org/SDL2/README/ios) for more details.

```bash
/Applications/CMake.app/Contents/bin/cmake .. -G "Xcode" \
-D CMAKE_BUILD_TYPE=Release \
-D CMAKE_OSX_ARCHITECTURES="arm64" \
-D CMAKE_OSX_DEPLOYMENT_TARGET="16.5" \
-D CMAKE_OSX_SYSROOT="/path/to/IOS_SDK" \
-D CMAKE_SYSTEM_NAME="iOS" \
-D CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="YOUR_DEVELOPMENT_TEAM_ID" \
-D IOS_SDK="iphoneos" \
-D LSG_EXT_LIB_DIR="/path/to/libs"

xcodebuild IPHONEOS_DEPLOYMENT_TARGET="16.5" -project sdl2gui.xcodeproj -configuration Release -destination "generic/platform=iOS" -allowProvisioningUpdates
```

#### Xcode - Devices

> See [Xcode - Running your app on a device](https://developer.apple.com/documentation/xcode/running-your-app-in-simulator-or-on-a-device) for more details.

#### Install APP on a device

1. Connect the device to your Mac.
1. Open **Xcode**.
1. Select `Window > Devices and Simulators` from the main menu.
1. Select the device from the list on the left.
1. Click the `+` icon under **Installed Apps**.
1. Locate and select `dist/bin/testsdl2gui-arm64.app`.

The app should now be installed on the device with the name **testsdl2gui**.

> If the installation fails, most likely it means the app package was not signed correctly. Try opening `sdl2gui.xcodeproj` in Xcode to make sure all signing options have been set correctly.

![Screenshot of Test project on iPhone](screenshots/iphone_480p.png)
![Screenshot of Test project on iPad](screenshots/ipad_480p.png)

### macOS

You can get the macOS SDK path with the following command: `xcrun --sdk macosx --show-sdk-path`

```bash
/Applications/CMake.app/Contents/bin/cmake .. -G "Xcode" \
-D CMAKE_BUILD_TYPE=Release \
-D CMAKE_OSX_ARCHITECTURES="x86_64" \
-D CMAKE_OSX_DEPLOYMENT_TARGET="13.4" \
-D CMAKE_OSX_SYSROOT="/path/to/MACOSX_SDK" \
-D LSG_EXT_LIB_DIR="/path/to/libs"

xcodebuild MACOSX_DEPLOYMENT_TARGET="13.4" -project sdl2gui.xcodeproj -configuration Release
```

![Screenshot of Test project on macOS](screenshots/macos_480p.png)

### Linux

```bash
cmake .. -G "Unix Makefiles" \
-D CMAKE_BUILD_TYPE=Release \
-D LSG_EXT_LIB_DIR="/path/to/libs"

make
```

![Screenshot of Test project on Linux](screenshots/linux_480p.png)

### Windows

```bash
cmake .. -G "Visual Studio 17 2022" \
-D CMAKE_BUILD_TYPE=Release \
-D LSG_EXT_LIB_DIR="/path/to/libs"

devenv.com sdl2gui.sln -build "Release|x64"
```

![Screenshot of Test project on Windows](screenshots/windows_480p.png)

## Test project

You must call [LSG_Start](#lsg_start) before using other *LSG_\** methods, see the *test* project for examples.

```cpp
try {
  SDL_Renderer* renderer = LSG_Start("ui/main.xml");

  while (LSG_IsRunning()) {
    std::vector<SDL_Event> events = LSG_Run();

    myapp_handleEvents(events);
    myapp_render(renderer);

    if (LSG_IsRunning())
      LSG_Present();

    SDL_Delay(50);
  }

  LSG_Quit();
} catch (const std::exception& e) {
  LSG_ShowError(e.what());
  LSG_Quit();
}
```

## Start

The first step is to run [LSG_Start](#lsg_start) which

- creates a new [SDL_Window](https://wiki.libsdl.org/SDL2/SDL_CreateWindow)
- loads UI components from the XML file
- creates and returns an [SDL_Renderer](https://wiki.libsdl.org/SDL2/SDL_CreateRenderer)

```cpp
SDL_Renderer* renderer = LSG_Start("ui/main.xml");
```

## Handle Events

You can call [LSG_IsRunning](#lsg_isrunning) to make sure the library was initialized successfully and is available for *LSG_\** calls.

[LSG_Run](#lsg_run)

- Clears the render buffer
- Renders UI components (if XML file was loaded)
- Handles UI related events (like clicking, scrolling, sliding etc.)
- Returns a list of all [SDL Events](https://wiki.libsdl.org/SDL2/SDL_Event) available
  - including the ones already handled by the library
  - custom library events will be available as [SDL_UserEvent](https://wiki.libsdl.org/SDL2/SDL_UserEvent)

```cpp
void myapp_handleEvents(const std::vector<SDL_Event>& events)
{
  for (const SDL_Event& event : events) {
    if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_CLOSE))
      LSG_Quit();
    else if (event.type >= SDL_USEREVENT)
      myapp_handleUserEvent(event.user);
    else if (event.type == SDL_KEYUP)
      myapp_handleKeyEvent(event.key);
  }
}
```

[SDL_UserEvent](https://wiki.libsdl.org/SDL2/SDL_UserEvent)

- The `.code` property will contain the [LSG_EventType](#lsg_eventtype)
- The `.data1` property will contain the `id` XML-attribute of the UI component that triggered the event
- The `.data2` property may contain extra data, and depends on the type of UI component

```cpp
void myapp_handleUserEvent(const SDL_UserEvent& event)
{
  auto type = (LSG_EventType)event.code;
  auto id   = static_cast<const char*>(event.data1);

  if ((type == LSG_EVENT_ROW_ACTIVATED) || (type == LSG_EVENT_ROW_SELECTED) || (type == LSG_EVENT_ROW_UNSELECTED))
    auto& rows = *static_cast<std::vector<int>*>(event.data2); // 0-based row indices (-1 for unselected)
  else if (type == LSG_EVENT_SLIDER_VALUE_CHANGED)
    double sliderValue = *static_cast<double*>(event.data2); // Percent-based slider value: [0.0, 1.0]
  else if (type == LSG_EVENT_COMPONENT_KEY_ENTERED)
    SDL_Keycode key = *static_cast<SDL_Keycode*>(event.data2); // SDL key code (SDLK_ESCAPE, SDLK_0, SDLK_a, ...)

  if (event.data1)
    free(event.data1);

  if (event.data2)
  {
    if ((type == LSG_EVENT_ROW_ACTIVATED) || (type == LSG_EVENT_ROW_SELECTED) || (type == LSG_EVENT_ROW_UNSELECTED))
      delete static_cast<std::vector<int>*>(event.data2);
    else if (type == LSG_EVENT_SLIDER_VALUE_CHANGED)
      delete static_cast<double*>(event.data2);
    else if (type == LSG_EVENT_COMPONENT_KEY_ENTERED)
      delete static_cast<SDL_Keycode*>(event.data2);
  }
}
```

## Custom Render

You can also perform custom rendering like [SDL_RenderFillRect](https://wiki.libsdl.org/SDL2/SDL_RenderFillRect) by using the [SDL_Renderer](https://wiki.libsdl.org/SDL2/SDL_CreateRenderer) returned from [LSG_Start](#lsg_start).

> Make sure to render after calling [LSG_Run](#lsg_run) as it will clear anything in the render buffer.

```cpp
void myapp_render(SDL_Renderer* renderer)
{
    SDL_Size windowSize  = LSG_GetWindowSize();
    SDL_Rect destination = { ((windowSize.width - 100) / 2), ((windowSize.height - 100) / 2), 100, 100 };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 64);
    SDL_RenderFillRect(renderer, &destination);
}
```

## Present

Finally, you can run [LSG_Present](#lsg_present) to swap the background buffer and [present](https://wiki.libsdl.org/SDL2/SDL_RenderPresent) it to the screen.

## Quit

Make sure to call [LSG_Quit](#lsg_quit) to cleanup all resources and close the library.

## XML UI components

### \<button\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [orientation](#orientation) | [size](#size)

Triggers [LSG_EVENT_BUTTON_CLICKED](#handle-events) event.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
orientation="orientation"
background-color="color"
border="int"
border-color="color"
margin="int"
padding="int"
halign="alignment_horizontal"
valign="alignment_vertical"
spacing="int"
font-size="int" # default="14"
text-color="color"
```

### \<image\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [file_path](#file_path) | [size](#size)

```ini
id="string"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
halign="alignment_horizontal"
valign="alignment_vertical"

file="file_path"
fill="boolean"
```

### \<line\>

[color](#color) | [orientation](#orientation) | [size](#size)

```ini
id="string"
width="size"
height="size"
orientation="orientation"

color="color"
```

### \<list\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [size](#size) | [sort_order](#sort_order)

Triggers [LSG_EVENT_ROW_ACTIVATED](#handle-events), [LSG_EVENT_ROW_SELECTED](#handle-events) and [LSG_EVENT_ROW_UNSELECTED](#handle-events) events.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
show-row-border="boolean"
halign="alignment_horizontal"
valign="alignment_vertical"
font-size="int" # default="14"
text-color="color"

sort="sort_order"
```

### \<menu\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [size](#size)

Triggers [LSG_EVENT_MENU_SELECTED](#handle-events) event.

```ini
id="string"
enabled="boolean"
visible="boolean"
background-color="color"
halign="alignment_horizontal"
valign="alignment_vertical"
padding="int"
font-size="int" # default="14"
text-color="color"

title="string"
width="size"
```

### \<menu-sub\>

[boolean](#boolean)

```ini
id="string"
enabled="boolean"
visible="boolean"

title="string"
```

### \<menu-item\>

[boolean](#boolean) | [file_path](#file_path)

```ini
id="string"
enabled="boolean"
visible="boolean"

icon="file_path"
key="string"
```

### \<modal\>

[alignment](#alignment) | [color](#color) | [orientation](#orientation) | [size](#size)

```ini
id="string"
width="size"
height="size"
orientation="orientation"
background-color="color"
border="int"
border-color="color"
padding="int"
halign="alignment_horizontal"
valign="alignment_vertical"
spacing="int"
font-size="int" # default="14"
text-color="color"
title="string"

hide-close-icon="boolean"
max-width="int"
max-height="int"
min-width="int"
min-height="int"
```

### \<navigation\>

[boolean](#boolean) | [color](#color) | [size](#size)

Triggers [LSG_EVENT_NAVIGATE_BACK](#handle-events) and [LSG_EVENT_NAVIGATE_END](#handle-events), [LSG_EVENT_NAVIGATE_FORWARD](#handle-events) and [LSG_EVENT_NAVIGATE_HOME](#handle-events) events.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
font-size="int" # default="14"
text-color="color"

items-total="size_t"          # Number of total items that can be navigated, default is 0.
items-per-navigation="size_t" # Number of items to navigate by, default is 1.
```

### \<panel\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [orientation](#orientation) | [size](#size)

Triggers [LSG_EVENT_COMPONENT_CLICKED](#handle-events) and [LSG_EVENT_COMPONENT_DOUBLE_CLICKED](#handle-events) and [LSG_EVENT_COMPONENT_RIGHT_CLICKED](#handle-events) events.

```ini
id="string"
width="size"
height="size"
orientation="orientation"
background-color="color"
border="int"
border-color="color"
margin="int"
padding="int"
halign="alignment_horizontal"
valign="alignment_vertical"
spacing="int"
font-size="int" # default="14"
text-color="color"

scrollable="boolean"
```

### \<progress-bar\>

[boolean](#boolean) | [color](#color) | [percent](#percent) | [size](#size)

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
margin="int"
padding="int"

value="percent"
progress-color="color"
```

### \<slider\>

[boolean](#boolean) | [color](#color) | [orientation](#orientation) | [percent](#percent) | [size](#size)

Triggers [LSG_EVENT_SLIDER_VALUE_CHANGED](#handle-events) event.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
orientation="orientation"
background-color="color"
border="int"
border-color="color"
margin="int"
padding="int"

value="percent"
fill-progress="boolean"
progress-color="color"
thumb-color="color"
thumb-width="int" # minimum="10"
thumb-border-color="color"
thumb-border="int"
```

### \<table\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [size](#size) | [sort_order](#sort_order)

Triggers [LSG_EVENT_ROW_ACTIVATED](#handle-events), [LSG_EVENT_ROW_SELECTED](#handle-events) and [LSG_EVENT_ROW_UNSELECTED](#handle-events) events.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
show-column-border="boolean"
show-row-border="boolean"
halign="alignment_horizontal"
valign="alignment_vertical"
font-size="int" # default="14"
text-color="color"

sort="sort_order"
sort-column="int" # 0-based index
```

### \<text\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [size](#size)

```ini
id="string"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
halign="alignment_horizontal"
valign="alignment_vertical"
font-size="int" # default="14"
text-color="color"

bold="boolean"
italic="boolean"
strike-through="boolean"
underline="boolean"
wrap="boolean"
```

### \<text-input\>

[color](#color) | [size](#size)

```ini
id="string"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
font-size="int" # default="14"
padding="int"
text-color="color"

placeholder="string"
value="string"
```

### \<tiles\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [size](#size)

Triggers [LSG_EVENT_TILE_ACTIVATED](#handle-events), [LSG_EVENT_TILE_SELECTED](#handle-events) and [LSG_EVENT_TILE_UNSELECTED](#handle-events) events.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
spacing="int"
font-size="int" # default="14"
text-color="color"

text-halign="alignment_horizontal"
text-valign="alignment_vertical"
tile-size="size"
wrap="boolean" # default="true"
```

### \<toggle\>

[alignment](#alignment) | [boolean](#boolean) | [color](#color) | [size](#size)

Triggers [LSG_EVENT_TOGGLED_OFF](#handle-events) and [LSG_EVENT_TOGGLED_ON](#handle-events) events.

```ini
id="string"
enabled="boolean"
visible="boolean"
width="size"
height="size"
background-color="color"
border="int"
border-color="color"
halign="alignment_horizontal"
valign="alignment_vertical"
font-size="int" # default="14"
text-color="color"

on="boolean"
```

### \<window\>

[boolean](#boolean) | [file_path](#file_path)

```ini
title="string"
width="int"
height="int"
min-width="int"  # default="400"
min-height="int" # default="400"
x="int"
y="int"
maximized="boolean"
color-theme-file="file_path"
```

## XML UI component attributes

### alignment

```ini
value="left|center|right" # Horizontal default="left"
value="top|middle|bottom" # Vertical default="top"
```

### boolean

```ini
value="true|false" # default="false"
```

### color

```ini
value="#00FF00|#00FF0080" # Hex default="#00000000"
value="rgb(0,255,255)|rgba(0,255,0,0.5)" # RGB default="rgba(0,0,0,0)"
```

### file_path

```ini
value="/path/to/ui/dark.colortheme|C:/path/to/ui/dark.colortheme" # Absolute
value="ui/dark.colortheme" # Relative
```

### orientation

```ini
value="horizontal|vertical" # default="horizontal"
```

### percent

```ini
value="0.5" # 50% [0.0,1.0] default="0.0"
```

### size

```ini
value="100" # Absolute
value="10%" # Relative
```

### sort_order

```ini
value="ascending|descending" # default="ascending"
```

## Color Theme File

```ini
<id>.background-color=<color>
<id>.border-color=<color>
<id>.text-color=<color>
```

- `<id>` needs to correspond to the XML-attribute `id` of the UI component.
- `<color>` can have the same values as [color](#color) (without the string quotes).
  - Unlike XML, the `.colortheme` file does not wrap the values in string quotes, so `"#000000"` becomes `#000000`.
- `<id>` and `*-color` attribute are seperated by a `.` (dot) character.

```ini
Root.background-color=rgb(245, 245, 245)
Root.border-color=#000000
Root.text-color=#FF0000

Slider.progress-color=rgb(20, 130, 255)
Slider.thumb-color=rgb(128, 128, 128)
Slider.thumb-border-color=#000000
```

## API

### LSG_EventType

[Handle Events](#handle-events)

```cpp
enum LSG_EventType {
  LSG_EVENT_BUTTON_CLICKED,
  LSG_EVENT_BUTTON_PRESSED,
  LSG_EVENT_COMPONENT_CLICKED,
  LSG_EVENT_COMPONENT_DOUBLE_CLICKED,
  LSG_EVENT_COMPONENT_RIGHT_CLICKED,
  LSG_EVENT_COMPONENT_KEY_ENTERED,
  LSG_EVENT_COMPONENT_SCROLLED,
  LSG_EVENT_MENU_ITEM_SELECTED,
  LSG_EVENT_NAVIGATE_BACK,
  LSG_EVENT_NAVIGATE_END,
  LSG_EVENT_NAVIGATE_FORWARD,
  LSG_EVENT_NAVIGATE_HOME,
  LSG_EVENT_PAGE_NAVIGATED,
  LSG_EVENT_ROW_ACTIVATED, // ENTER or double-click
  LSG_EVENT_ROW_SELECTED,
  LSG_EVENT_ROW_UNSELECTED,
  LSG_EVENT_SLIDER_VALUE_CHANGED,
  LSG_EVENT_TABLE_COLUMN_RESIZED,
  LSG_EVENT_TEXT_INPUT_CLEARED,
  LSG_EVENT_TEXT_INPUT_COMPLETED, // ENTER
  LSG_EVENT_TILE_ACTIVATED, // ENTER or double-click
  LSG_EVENT_TILE_SELECTED,
  LSG_EVENT_TILE_UNSELECTED,
  LSG_EVENT_TOGGLED_OFF,
  LSG_EVENT_TOGGLED_ON
};
```

### LSG_ExifTagID

<https://www.media.mit.edu/pia/Research/deepview/exif.html>

<https://exiftool.org/TagNames/EXIF.html>

```cpp
enum LSG_ExifTagID
{
  LSG_EXIF_TAG_ID_CAMERA_FOCAL_LENGTH  = 0x920a,
  LSG_EXIF_TAG_ID_CAMERA_MAKE          = 0x010f,
  LSG_EXIF_TAG_ID_CAMERA_MODEL         = 0x0110,
  LSG_EXIF_TAG_ID_CAMERA_FSTOP         = 0x829d,
  LSG_EXIF_TAG_ID_CAMERA_EXPOSURE_TIME = 0x829a,
  LSG_EXIF_TAG_ID_CAMERA_ISO           = 0x8827,
  LSG_EXIF_TAG_ID_DATE_TIME_ORIGINAL   = 0x9003,
  LSG_EXIF_TAG_ID_DATE_TIME_OFFSET     = 0x9011,
  LSG_EXIF_TAG_ID_GPS_LATITUDE_REF     = 0x0001,
  LSG_EXIF_TAG_ID_GPS_LATITUDE         = 0x0002,
  LSG_EXIF_TAG_ID_GPS_LONGITUDE_REF    = 0x0003,
  LSG_EXIF_TAG_ID_GPS_LONGITUDE        = 0x0004,
  LSG_EXIF_TAG_ID_GPS_ALTITUDE_REF     = 0x0005,
  LSG_EXIF_TAG_ID_GPS_ALTITUDE         = 0x0006,
  LSG_EXIF_TAG_ID_OFFSET_GPS_INFO      = 0x8825,
  LSG_EXIF_TAG_ID_OFFSET_SUB_IFD       = 0x8769,
  LSG_EXIF_TAG_ID_ORIENTATION          = 0x0112,
  LSG_EXIF_TAG_ID_THUMB_JPEG_OFFSET    = 0x0201,
  LSG_EXIF_TAG_ID_THUMB_JPEG_SIZE      = 0x0202,
  LSG_EXIF_TAG_ID_THUMB_COMPRESSION    = 0x0103
};
```

### LSG_HAlign

```cpp
enum LSG_HAlign {
  LSG_HALIGN_LEFT,
  LSG_HALIGN_CENTER,
  LSG_HALIGN_RIGHT
};
```

### LSG_VAlign

```cpp
enum LSG_VAlign {
  LSG_VALIGN_TOP,
  LSG_VALIGN_MIDDLE,
  LSG_VALIGN_BOTTOM
};
```

### LSG_Orientation

```cpp
enum LSG_Orientation {
  LSG_ORIENTATION_HORIZONTAL,
  LSG_ORIENTATION_VERTICAL
};
```

### LSG_SortOrder

```cpp
enum LSG_SortOrder {
  LSG_SORT_ORDER_ASCENDING,
  LSG_SORT_ORDER_DESCENDING
};
```

### LSG_DEFAULT_FONT_SIZE

```cpp
const int LSG_DEFAULT_FONT_SIZE = 14;
```

### LSG_MAX_ROWS_PER_PAGE

```cpp
const int LSG_MAX_ROWS_PER_PAGE = 100;
```

### SDL_Size

```cpp
struct SDL_Size {
  int width  = 0;
  int height = 0;
};
```

### LSG_ButtonItem

```cpp
struct LSG_ButtonItem
{
  std::string id     = "";
  std::string text   = "";
  LSG_HAlign  halign = LSG_HALIGN_CENTER;
  LSG_VAlign  valign = LSG_VALIGN_MIDDLE;
};```

### LSG_ExifData

```cpp
struct LSG_ExifData
{
  LSG_ExifTags gps       = {};
  LSG_ExifTags tags      = {};
  SDL_Surface* thumbnail = nullptr;
};
```

### LSG_ImageOrientation

```cpp
struct LSG_ImageOrientation
{
  SDL_RendererFlip flip     = SDL_FLIP_NONE;
  double           rotation = 0.0;
};
```

### LSG_GPSCoordinate

```cpp
struct LSG_GPSCoordinate
{
  double degrees = 0.0;
  double minutes = 0.0;
  double seconds = 0.0;
  double decimal = 0.0;
};
```

### LSG_GPS

```cpp
struct LSG_GPS
{
  LSG_GPSCoordinate latitude  = {};
  LSG_GPSCoordinate longitude = {};
  double            altitude  = {};
};
```

### LSG_TableGroup

```cpp
struct LSG_TableGroup
{
  std::string   group = "";
  LSG_TableRows rows  = {};
};
```

### LSG_TileItem

```cpp
struct LSG_TileItem
{
  std::string image = "";
  std::string text  = "";
};
```

### LSG_ButtonItems

```cpp
using LSG_ButtonItems = std::vector<LSG_ButtonItem>;
```

### LSG_ExifTags

```cpp
using LSG_ExifTags = std::map<uint16_t, std::string>;
```

### LSG_Strings

```cpp
using LSG_Strings = std::vector<std::string>;
```

### LSG_TableRows

```cpp
using LSG_TableRows = std::vector<LSG_Strings>;
```

### LSG_TableGroups

```cpp
using LSG_TableGroups = std::vector<LSG_TableGroup>;
```

### LSG_TileItems

```cpp
using LSG_TileItems = std::vector<LSG_TileItem>;
```

### LSG_AddListItem

```cpp
void LSG_AddListItem(const std::string& id, const std::string& item);
```

Adds a new item to the list.

Parameters

- **id** \<list\> component ID
- **item** List item

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_AddListItem("List", "My new list item");
```

### LSG_AddPanelButton

```cpp
void LSG_AddPanelButton(const std::string& id, const LSG_ButtonItem& button);
```

Adds a new button to the panel.

Parameters

- **id** \<panel\> component ID
- **button** Button item

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_ButtonItem button = {
  .id     = "ButtonIdButton1",
  .text   = "Button 1",
  .halign = LSG_HALIGN_CENTER,
  .valign = LSG_VALIGN_MIDDLE
};

LSG_AddPanelButton("PanelIdButtons", button);
```

### LSG_AddSubMenuItem

```cpp
void LSG_AddSubMenuItem(const std::string& id, const std::string& item, const std::string& itemId);
```

Adds a new item to the sub-menu.

Parameters

- **id** \<menu-sub\> component ID
- **item** \<menu-item\> value
- **itemId** \<menu-item\> component ID

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_AddSubMenuItem("MenuIdColorThemes", "Dark",  "MenuIdColorThemeDark");
LSG_AddSubMenuItem("MenuIdColorThemes", "Light", "MenuIdColorThemeLight");
```

### LSG_AddTableGroup

```cpp
void LSG_AddTableGroup(const std::string& id, const LSG_TableGroup& group);
```

Adds a new group with rows to the table.

Parameters

- **id** \<table\> component ID
- **group** Table group and rows

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TableGroup group = {
  .group = "New Group",
  .rows = {
    { "New row 1 - Column A", "My new row 1 - Column B" },
    { "New row 2 - Column A", "My new row 2 - Column B" }
  }
};

LSG_AddTableGroup("TableWithGroups", group);
```

### LSG_AddTableRow

```cpp
void LSG_AddTableRow(const std::string& id, const LSG_Strings& columns);
```

Adds a new row to the table.

Parameters

- **id** \<table\> component ID
- **columns** Table row columns

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_Strings row = {
  "New row - Column A",
  "New row - Column B"
};

LSG_AddTableRow("Table", row);
```

### LSG_AddTile

```cpp
void LSG_AddTile(const std::string& id, const LSG_TileItem& tile);
```

Adds a new tile to the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **tile** Tile item

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TileItem tile = {
  .image = "/path/to/image/file1.jpg",
  .text  = "Image 1"
};

LSG_AddTile("Tiles", tile);
```

### LSG_ClearTextInput

```cpp
void LSG_ClearTextInput(const std::string& id);
```

Clears the text input value.

Parameters

- **id** \<text-input\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetBackgroundColor

```cpp
SDL_Color LSG_GetBackgroundColor(const std::string& id);
```

Returns the background color of the component.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetColorTheme

```cpp
std::string LSG_GetColorTheme();
```

Returns the currently applied color theme file, ex: "ui/dark.colortheme" or "" if none applied.

Exceptions

- runtime_error

### LSG_GetImageExif

```cpp
LSG_ExifData LSG_GetImageExif(const std::string& filePath);
```

Returns EXIF (Exchangeable Image File Format) data from the image file (if it exists).

Parameters

- **filePath** Image file path

Exceptions

- invalid_argument
- runtime_error

### LSG_GetImageGPS

```cpp
LSG_GPS LSG_GetImageGPS(const LSG_ExifTags& gps);
```

Returns GPS coordinates from the EXIF GPS tags (if they exist).

Parameters

- **gps** EXIF GPS tags

Exceptions

- runtime_error

### LSG_GetImageOrientation

```cpp
LSG_ImageOrientation LSG_GetImageOrientation(const LSG_ExifTags& tags);
```

Returns the orientation of the image from the EXIF tags (if it exists).

Parameters

- **tags** EXIF data tags

Exceptions

- runtime_error

### LSG_GetImageThumbnail

```cpp
SDL_Surface* LSG_GetImageThumbnail(const std::string& filePath, const SDL_Size& maxSize);
```

Returns a downscaled thumbnail of the original image.

Parameters

- **filePath** Image file path
- **maxSize** Max size of thumbnail

Exceptions

- invalid_argument
- runtime_error

### LSG_GetLastPage

```cpp
int LSG_GetLastPage(const std::string& id);
```

Returns the last 0-based page index of the list or table.

Parameters

- **id** \<list\> or \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetListItem

```cpp
std::string LSG_GetListItem(const std::string& id, int row);
```

Returns the item from the list.

Parameters

- **id** \<list\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

### LSG_GetListItemCount

```cpp
size_t LSG_GetListItemCount(const std::string& id);
```

Returns the number of items in the list.

Parameters

- **id** \<list\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetListItems

```cpp
LSG_Strings LSG_GetListItems(const std::string& id);
```

Returns all the items from the list.

Parameters

- **id** \<list\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetMargin

```cpp
int LSG_GetMargin(const std::string& id);
```

Returns the margin around a component.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetNavigationItemCount

```cpp
size_t LSG_GetNavigationItemCount(const std::string& id);
```

Returns the total number of items that can be navigated.

Parameters

- **id** \<navigation\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetNavigationPosition

```cpp
int LSG_GetNavigationPosition(const std::string& id);
```

Returns the current 0-based position of the navigation component.

Parameters

- **id** \<navigation\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPadding

```cpp
int LSG_GetPadding(const std::string& id);
```

Returns the padding inside a component.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPage

```cpp
int LSG_GetPage(const std::string& id);
```

Returns the current 0-based page index of the list table.

Parameters

- **id** \<list\> or \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPageListItem

```cpp
std::string LSG_GetPageListItem(const std::string& id, int row);
```

Returns the item on the current page of the list.

Parameters

- **id** \<list\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPageListItems

```cpp
LSG_Strings LSG_GetPageListItems(const std::string& id);
```

Returns the items on the current page of the list.

Parameters

- **id** \<list\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPageTableGroups

```cpp
LSG_TableRows LSG_GetPageTableGroups(const std::string& id);
```

Returns the groups on the current page of the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPageTableRow

```cpp
LSG_Strings LSG_GetPageTableRow(const std::string& id, int row);
```

Returns the columns on the current page of the table.

Parameters

- **id** \<table\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPageTableRows

```cpp
LSG_TableRows LSG_GetPageTableRows(const std::string& id);
```

Returns the rows on the current page of the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetPosition

```cpp
SDL_Point LSG_GetPosition(const std::string& id);
```

Returns the component position.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetProgressValue

```cpp
double LSG_GetProgressValue(const std::string& id);
```

Returns the value of the progress bar as a percent between 0 and 1.

Parameters

- **id** \<progress-bar\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetScrollHorizontal

```cpp
int LSG_GetScrollHorizontal(const std::string& id);
```

Returns the horizontal scroll offset of the component.

Parameters

- **id** \<list\>, \<panel\>, \<table\>, \<text\> or \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetScrollVertical

```cpp
int LSG_GetScrollVertical(const std::string& id);
```

Returns the vertical scroll offset of the component.

Parameters

- **id** \<list\>, \<panel\>, \<table\>, \<text\> or \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSelectedRows

```cpp
std::vector<int> LSG_GetSelectedRows(const std::string& id);
```

Returns the selected 0-based row indices (-1 for unselected) of the list or table.

Parameters

- **id** \<list\> or \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSelectedTiles

```cpp
std::vector<int> LSG_GetSelectedTiles(const std::string& id);
```

Returns the selected 0-based tile indices (-1 for unselected) of the tiles grid.

Parameters

- **id** \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSize

```cpp
SDL_Size LSG_GetSize(const std::string& id);
```

Returns the component size.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSliderValue

```cpp
double LSG_GetSliderValue(const std::string& id);
```

Returns the value of the slider as a percent between 0 and 1.

Parameters

- **id** \<slider\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSortColumn

```cpp
int LSG_GetSortColumn(const std::string& id);
```

Returns the sort column index of the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSortOrder

```cpp
LSG_SortOrder LSG_GetSortOrder(const std::string& id);
```

Returns the sort order of the list or table.

Parameters

- **id** \<list\> or \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetSpacing

```cpp
int LSG_GetSpacing(const std::string& id);
```

Returns the spacing between child components.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableColumnWidth

```cpp
int LSG_GetTableColumnWidth(const std::string& id, int column);
```

Returns the width of the table column.

Parameters

- **id** \<table\> component ID
- **column** 0-based column index

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableGroup

```cpp
LSG_TableGroup LSG_GetTableGroup(const std::string& id, const std::string& group);
```

Returns the group from the table.

Parameters

- **id** \<table\> component ID
- **group** The group name

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableGroups

```cpp
LSG_TableRows LSG_GetTableGroups(const std::string& id);
```

Returns all the groups from the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableHeader

```cpp
LSG_Strings LSG_GetTableHeader(const std::string& id);
```

Returns the header columns from the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableRow

```cpp
LSG_Strings LSG_GetTableRow(const std::string& id, int row);
```

Returns the columns from the table.

Parameters

- **id** \<table\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableRowCount

```cpp
size_t LSG_GetTableRowCount(const std::string& id);
```

Returns the number of rows in the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTableRows

```cpp
LSG_TableRows LSG_GetTableRows(const std::string& id);
```

Returns all the rows from the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetText

```cpp
std::string LSG_GetText(const std::string& id);
```

Returns the text value of the component.

Parameters

- **id** \<text\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTextInputValue

```cpp
std::string LSG_GetTextInputValue(const std::string& id);
```

Returns the text input value.

Parameters

- **id** \<text-input\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTile

```cpp
LSG_TileItem LSG_GetTile(const std::string& id, int index);
```

Returns the tile item from the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **index** 0-based tile index position

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTiles

```cpp
LSG_TileItems LSG_GetTiles(const std::string& id);
```

Returns all the tile items from the tiles grid.

Parameters

- **id** \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTilesCount

```cpp
size_t LSG_GetTilesCount(const std::string& id);
```

Returns the number of tile items in the tiles grid.

Parameters

- **id** \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetTitle

```cpp
std::string LSG_GetTitle(const std::string& id);
```

Returns the header title of the modal, menu or sub-menu.

Parameters

- **id** \<modal\>, \<menu\> or \<menu-sub\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_GetWindowMinimumSize

```cpp
SDL_Size LSG_GetWindowMinimumSize();
```

Returns the minimum window size.

Exceptions

- runtime_error

### LSG_GetWindowPosition

```cpp
SDL_Point LSG_GetWindowPosition();
```

Returns the window position.

Exceptions

- runtime_error

### LSG_GetWindowSize

```cpp
SDL_Size LSG_GetWindowSize();
```

Returns the window size.

Exceptions

- runtime_error

### LSG_GetWindowTitle

```cpp
std::string LSG_GetWindowTitle();
```

Returns the window title.

Exceptions

- runtime_error

### LSG_IsEnabled

```cpp
bool LSG_IsEnabled(const std::string& id);
```

Returns true if the component is enabled.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_IsMenuItemSelected

```cpp
bool LSG_IsMenuItemSelected(const std::string& id);
```

Returns true if the menu item is selected.

Parameters

- **id** \<menu-item\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_IsMenuOpen

```cpp
bool LSG_IsMenuOpen(const std::string& id);
```

Returns true if the menu is open.

Parameters

- **id** \<menu\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_IsPreferredDarkMode

```cpp
bool LSG_IsPreferredDarkMode();
```

Returns true if the platform prefers dark mode.

Exceptions

- runtime_error

### LSG_IsRunning

```cpp
bool LSG_IsRunning();
```

Returns true if the library has been initialized and window created.

### LSG_IsToggledOn

```cpp
bool LSG_IsToggledOn(const std::string& id);
```

Returns true if the toggle switch is toggled on.

Parameters

- **id** \<toggle\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_IsVisible

```cpp
bool LSG_IsVisible(const std::string& id);
```

Returns true if the component is visible.

Parameters

- **id** Component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_IsWindowMaximized

```cpp
bool LSG_IsWindowMaximized();
```

Returns true if the window is maximized.

Exceptions

- runtime_error

### LSG_Layout

```cpp
void LSG_Layout();
```

Recalculates and redraws the window layout.

Exceptions

- runtime_error

### LSG_NavigateBack

```cpp
void LSG_NavigateBack(const std::string& id, const std::string& text = "");
```

Navigates backwards, and displays an updated text label.

Parameters

- **id** \<navigation\> component ID
- **text** Optional text label, shows "[new_position] / [total_items]" by default.

Exceptions

- invalid_argument
- runtime_error

### LSG_NavigateEnd

```cpp
void LSG_NavigateEnd(const std::string& id, const std::string& text = "");
```

Navigates to the last item, and displays an updated text label.

Parameters

- **id** \<navigation\> component ID
- **text** Optional text label, shows "[last_position] / [total_items]" by default.

Exceptions

- invalid_argument
- runtime_error

### LSG_NavigateForward

```cpp
void LSG_NavigateForward(const std::string& id, const std::string& text = "");
```

Navigates forwards, and displays an updated text label.

Parameters

- **id** \<navigation\> component ID
- **text** Optional text label, shows "[new_position] / [total_items]" by default.

Exceptions

- invalid_argument
- runtime_error

### LSG_NavigateHome

```cpp
void LSG_NavigateHome(const std::string& id, const std::string& text = "");
```

Navigates to the first item, and displays an updated text label.

Parameters

- **id** \<navigation\> component ID
- **text** Optional text label, shows "1 / [total_items]" by default.

Exceptions

- invalid_argument
- runtime_error

### LSG_NavigateTo

```cpp
void LSG_NavigateTo(const std::string& id, int position, const std::string& text = "");
```

Navigates to the position, and displays an updated text label.

Parameters

- **id** \<navigation\> component ID
- **position** 0-based position
- **text** Optional text label, shows ""[new_position] / [total_items]" by default.

Exceptions

- invalid_argument
- runtime_error

### LSG_OpenFile

```cpp
std::string LSG_OpenFile(const LSG_Strings& filters = {}); // Android, Linux, macOS and Windows
```

> Only supported on Android, Linux, macOS and Windows.

Displays an Open File dialog where you can select a single file.

Returns the selected file path or an empty string if cancelled.

Parameters

- **filters** Optional filter by file type

Exceptions

- runtime_error

Android

```cpp
LSG_OpenFile({ "application/*", "audio/*", "image/*", "text/*", "video/*" });
```

Linux

```cpp
LSG_OpenFile({ "*.pdf", "*.txt" });
```

macOS

```cpp
LSG_OpenFile({ ".pdf", ".txt" });
```

Windows

```cpp
LSG_OpenFile({ "*.pdf", "*.txt" });
```

### LSG_OpenFiles

```cpp
LSG_Strings LSG_OpenFiles(const LSG_Strings& filters = {}); // Linux, macOS and Windows
```

> Only supported on Linux, macOS and Windows.

Displays an Open File dialog where you can select multiple files.

Returns the selected file paths or an empty list if cancelled.

Parameters

- **filters** Optional filter by file type

Exceptions

- runtime_error

See [LSG_OpenFile](#lsg_openfile) for examples.

### LSG_OpenFolder

```cpp
std::string LSG_OpenFolder();
```

> Only supported on Android, Linux, macOS and Windows.

Displays an Open Folder dialog where you can select a single folder.

Returns the selected folder path or an empty string if cancelled.

Exceptions

- runtime_error

### LSG_OpenFolders

```cpp
LSG_Strings LSG_OpenFolders();
```

> Only supported on Linux, macOS and Windows.

Displays an Open Folder dialog where you can select multiple folders.

Returns the selected folder paths or an empty list if cancelled.

Exceptions

- runtime_error

### LSG_OpenFile (iOS)

```cpp
void LSG_OpenFile(std::function<void(NSArray<NSURL*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

 Displays asynchronously a Document Picker dialog where you can select a single item file.

Parameters

- **resultsCallback** Callback function with an array containing the selected file, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

iOS

See [Accessing items outside the app's sandbox](https://developer.apple.com/documentation/uikit/providing-access-to-directories?language=objc) for more details.

```cpp
LSG_OpenFile([](NSArray<NSURL*>* urls) -> void {
  for (NSURL* url in urls) {
    if (![url startAccessingSecurityScopedResource])
      continue;

    NSData* bookmarkData = [url bookmarkDataWithOptions: NSURLBookmarkCreationMinimalBookmark includingResourceValuesForKeys: nil relativeToURL: nil error: nil];

    // TODO: save bookmark data for future access

    [url stopAccessingSecurityScopedResource];
  }
});
```

### LSG_OpenFiles (iOS)

```cpp
void LSG_OpenFiles(std::function<void(NSArray<<NSURL*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

Displays asynchronously a Document Picker dialog where you can select multiple item files.

Parameters

- **resultsCallback** Callback function with an array of selected files, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

See [LSG_OpenFile (iOS)](#lsg_openfile-ios) for examples.

### LSG_OpenFolder (iOS)

```cpp
void LSG_OpenFolder(std::function<void(NSArray<<NSURL*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

Displays asynchronously a Document Picker dialog where you can select a single folder.

Parameters

- **resultsCallback** Callback function with an array containing the selected folder, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

See [Accessing directory content outside the app's sandbox](https://developer.apple.com/documentation/uikit/providing-access-to-directories?language=objc) for more details.

```cpp
LSG_OpenFolder([](NSArray<NSURL*>* urls) -> void {
  for (NSURL* url in urls) {
    if (![url startAccessingSecurityScopedResource])
      continue;

    NSData* folderBookmarkData = [url bookmarkDataWithOptions: NSURLBookmarkCreationMinimalBookmark includingResourceValuesForKeys: nil relativeToURL: nil error: nil];

    // TODO: save bookmark data for future folder access

    NSArray<NSURL*>* files = [[NSFileManager defaultManager] contentsOfDirectoryAtURL: url includingPropertiesForKeys: nil options: NSDirectoryEnumerationSkipsHiddenFiles error: nil];

    for (NSURL* file in files) {
      NSData* fileBookmarkData = [file bookmarkDataWithOptions: NSURLBookmarkCreationMinimalBookmark includingResourceValuesForKeys: nil relativeToURL: nil error: nil];

      // TODO: save bookmark data for future file access
    }

    [url stopAccessingSecurityScopedResource];
  }
});
```

### LSG_OpenMediaFile

```cpp
void LSG_OpenMediaFile(std::function<void(NSArray<MPMediaItem*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

Displays asynchronously an Open Music dialog where you can select a single media file.

Parameters

- **resultsCallback** Callback function with an array containing the selected file, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

iOS

See [MPMediaItem](https://developer.apple.com/documentation/mediaplayer/mpmediaitem?language=objc), [valueForProperty:](https://developer.apple.com/documentation/mediaplayer/mpmediaentity/value(forproperty:)?language=objc) and [General media item property keys](https://developer.apple.com/documentation/mediaplayer/general-media-item-property-keys?language=objc) for more details.

```cpp
LSG_OpenMediaFile([](NSArray<MPMediaItem*>* items) -> void {
  for (MPMediaItem* item in items) {
    NSString* title = [item valueForProperty: MPMediaItemPropertyTitle];
    NSURL*    url   = [item valueForProperty: MPMediaItemPropertyAssetURL];
  }
});
```

### LSG_OpenMediaFiles

```cpp
void LSG_OpenMediaFiles(std::function<void(NSArray<<MPMediaItem*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

Displays asynchronously an Open Music dialog where you can select multiple media files.

Parameters

- **resultsCallback** Callback function with an array of selected files, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

See [LSG_OpenMediaFile](#lsg_openmediafile) for examples.

### LSG_OpenPhotoFile

```cpp
void LSG_OpenPhotoFile(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

Displays asynchronously an Open Photo dialog where you can select a single image file.

Parameters

- **resultsCallback** Callback function with an array containing the selected file, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

iOS

See [PHPickerResult](https://developer.apple.com/documentation/photokit/phpickerresult?language=objc), [NSItemProvider](https://developer.apple.com/documentation/foundation/nsitemprovider?language=objc) and [Fetching Assets](https://developer.apple.com/documentation/photokit/phasset/fetching_assets?language=objc) for more details.

```cpp
LSG_OpenPhotoFile([](NSArray<PHPickerResult*>* results) -> void {
  for (PHPickerResult* result in results) {
    NSString*       localIdentifier = [result assetIdentifier];
    NSItemProvider* provider        = [result itemProvider];
  }
});
```

### LSG_OpenPhotoFiles

```cpp
void LSG_OpenPhotoFiles(std::function<void(NSArray<PHPickerResult*>*)> resultsCallback); // iOS
```

> Only supported on iOS.

Displays asynchronously an Open Photo dialog where you can select multiple image files.

Parameters

- **resultsCallback** Callback function with an array of selected files, or an empty array if cancelled or denied access.

Exceptions

- runtime_error

See [LSG_OpenPhotoFile](#lsg_openphotofile) for examples.

### LSG_Present

```cpp
void LSG_Present();
```

Presents the render buffer to the screen/window.

Exceptions

- runtime_error

### LSG_Quit

```cpp
void LSG_Quit();
```

Cleans up allocated resources and closes the window.

### LSG_RemoveListItem

```cpp
void LSG_RemoveListItem(const std::string& id, int row);
```

Removes the item row from the list.

Parameters

- **id** \<list\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveListItem("List", 0);
```

### LSG_RemoveMenuItem

```cpp
void LSG_RemoveMenuItem(const std::string& id);
```

Removes the menu item.

Parameters

- **id** \<menu-item\> component ID

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveMenuItem("MenuIdColorThemeDark");
```

### LSG_RemovePageListItem

```cpp
void LSG_RemovePageListItem(const std::string& id, int row);
```

Removes the item on the current page of the list.

Parameters

- **id** \<list\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemovePageListItem("List", 0);
```

### LSG_RemovePageTableRow

```cpp
void LSG_RemovePageTableRow(const std::string& id, int row);
```

Removes the row on the current page of the table.

Parameters

- **id** \<table\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveTableRow("Table", 0);
```

### LSG_RemoveTableHeader

```cpp
void LSG_RemoveTableHeader(const std::string& id);
```

Removes the header columns from the table.

Parameters

- **id** \<table\> component ID

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveTableHeader("Table");
```

### LSG_RemoveTableGroup

```cpp
void LSG_RemoveTableGroup(const std::string& id, const std::string& group);
```

Removes the grouped rows from the table.

Parameters

- **id** \<table\> component ID
- **group** Table group name

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveTableGroup("TableWithGroups", "Quis Hendrerit");
```

### LSG_RemoveTableRow

```cpp
void LSG_RemoveTableRow(const std::string& id, int row);
```

Removes the row from the table.

Parameters

- **id** \<table\> component ID
- **row** 0-based row index

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveTableRow("Table", 0);
```

### LSG_RemoveTile

```cpp
void LSG_RemoveTile(const std::string& id, int index);
```

Removes the tile item from the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **index** 0-based tile index position

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_RemoveTile("Tiles", 0);
```

### LSG_Run

```cpp
std::vector<SDL_Event> LSG_Run();
```

Handles events and renders the UI components.

Returns a list of SDL2 events available during this run.

Exceptions

- runtime_error

### LSG_SaveFile

```cpp
std::string LSG_SaveFile(const LSG_Strings& filters = {}); // Android, Linux, macOS and Windows
```

> Only supported on Android, Linux, macOS and Windows.

Displays a Save File dialog where you can select a single file.

Returns the selected file path or an empty string if cancelled.

Parameters

- **filters** Optional filter by file type

Exceptions

- runtime_error

See [LSG_OpenFile](#lsg_openfile) for examples.

### LSG_ScrollHorizontal

```cpp
void LSG_ScrollHorizontal(const std::string& id, int scroll);
```

Scrolls the component horizontally by the specified offset.

Parameters

- **id** \<list\>, \<panel\>, \<table\>, \<text\> or \<tiles\> component ID
- **scroll** Horizontal scroll offset

Exceptions

- invalid_argument
- runtime_error

### LSG_ScrollVertical

```cpp
void LSG_ScrollVertical(const std::string& id, int scroll);
```

Scrolls the component vertically by the specified offset.

Parameters

- **id** \<list\>, \<panel\>, \<table\>, \<text\> or \<tiles\> component ID
- **scroll** Vertical scroll offset

Exceptions

- invalid_argument
- runtime_error

### LSG_ScrollToBottom

```cpp
void LSG_ScrollToBottom(const std::string& id);
```

Scrolls to the bottom of the component.

Parameters

- **id** \<list\>, \<panel\>, \<table\>, \<text\> or \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_ScrollToTop

```cpp
void LSG_ScrollToTop(const std::string& id);
```

Scrolls to the top of the component.

Parameters

- **id** \<list\>, \<panel\>, \<table\>, \<text\> or \<tiles\> component ID

Exceptions

- invalid_argument
- runtime_error

### LSG_SelectRow

```cpp
void LSG_SelectRow(const std::string& id, int row);
```

Selects the row in the list or table.

Parameters

- **id** \<list\> or \<table\> component ID
- **row** 0-based row index (-1 for unselected)

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SelectRow("List", 0);
```

### LSG_SelectRowByOffset

```cpp
void LSG_SelectRowByOffset(const std::string& id, int offset);
```

Selects a row relative to the currently selected row in the list or table.

Parameters

- **id** \<list\> or \<table\> component ID
- **offset** 0-based offset from current row index

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SelectRowByOffset("List", -2);
```

### LSG_SelectRows

```cpp
void LSG_SelectRows(const std::string& id, const std::vector<int>& rows);
```

Selects the rows in the list or table.

Parameters

- **id** \<list\> or \<table\> component ID
- **rows** 0-based row indices

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SelectRows("List", { 0, 1 });
```

### LSG_SelectTile

```cpp
void LSG_SelectTile(const std::string& id, int index);
```

Selects the tile item in the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **index** 0-based tile index position (-1 for unselected)

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SelectTile("Tiles", 0);
```

### LSG_SelectTiles

```cpp
void LSG_SelectTiles(const std::string& id, const std::vector<int>& indices);
```

Selects the tile items in the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **indices** 0-based tile index positions

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SelectTiles("Tiles", { 0, 1 });
```

### LSG_SetAlignmentHorizontal

```cpp
void LSG_SetAlignmentHorizontal(const std::string& id, LSG_HAlign alignment);
```

Sets the horizontal alignment of child components in containers like \<panel\> and \<button\>, or alignment of textured components like \<image\> and \<text\> relative to available space in their background component.

Parameters

- **id** Component ID
- **alignment** Horizontal alignment

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetAlignmentHorizontal("ButtonIdColorThemeDark", LSG_HALIGN_CENTER);
```

### LSG_SetAlignmentVertical

```cpp
void LSG_SetAlignmentVertical(const std::string& id, LSG_VAlign alignment);
```

Sets the vertical alignment of child components in containers like \<panel\> and \<button\>, or alignment of textured components like \<image\> and \<text\> relative to available space in their background component.

Parameters

- **id** Component ID
- **alignment** Vertical alignment

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetAlignmentVertical("ButtonIdColorThemeDark", LSG_VALIGN_MIDDLE);
```

### LSG_SetBackgroundColor

```cpp
void LSG_SetBackgroundColor(const std::string& id, const SDL_Color& color);
```

Sets the background color of a component.

Parameters

- **id** Component ID
- **color** Background color

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetBackgroundColor("Root", SDL_Color(255, 0, 0, 255));
```

### LSG_SetBorder

```cpp
void LSG_SetBorder(const std::string& id, int border);
```

Sets the border width of a component.

Parameters

- **id** Component ID
- **border** Border width in pixels

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetBorder("Root", 2);
```

### LSG_SetBorderColor

```cpp
void LSG_SetBorderColor(const std::string& id, const SDL_Color& color);
```

Sets the border color of a component.

Parameters

- **id** Component ID
- **color** Border color

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetBorderColor("Root", SDL_Color(255, 0, 0, 255));
```

### LSG_SetButtonSelected

```cpp
void LSG_SetButtonSelected(const std::string& id, bool selected = true);
```

Highlights the button as selected.

Parameters

- **id** \<button\> component ID
- **selected** true to select or false to unselect

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetButtonSelected("ButtonIdColorThemeDark", true);
```

### LSG_SetColorTheme

```cpp
void LSG_SetColorTheme(const std::string& colorThemeFile);
```

Tries to load and apply the color theme file.

Parameters

- **colorThemeFile** Color theme file. ex: "ui/dark.colortheme"

Exceptions

- runtime_error

### LSG_SetEnabled

```cpp
void LSG_SetEnabled(const std::string& id, bool enabled = true);
```

Enables or disables the component.

Parameters

- **id** Component ID
- **enabled** true to enable or false to disable

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetEnabled("MenuIdColorThemeDark", false);
LSG_SetEnabled("MenuIdColorThemeLight");
LSG_SetEnabled("ButtonIdColorThemeDark", false);
LSG_SetEnabled("ButtonIdColorThemeLight");
```

### LSG_SetFontSize

```cpp
void LSG_SetFontSize(const std::string& id, int size);
```

Sets the font size of a component and sub-components.

Parameters

- **id** Component ID
- **size** Font size

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetFontSize("TextIdColorTheme", 40);
```

### LSG_SetFontStyle

```cpp
void LSG_SetFontStyle(const std::string& id, int style);
```

Sets the font style of a component and sub-components.

Parameters

- **id** Component ID
- **size** Font style

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetFontStyle("TextIdColorTheme", (TTF_STYLE_BOLD | TTF_STYLE_ITALIC | TTF_STYLE_STRIKETHROUGH | TTF_STYLE_UNDERLINE));
LSG_SetFontStyle("TextIdColorTheme", TTF_STYLE_NORMAL);
```

### LSG_SetHeight

```cpp
void LSG_SetHeight(const std::string& id, int height, bool layout = true);
```

Sets the height of a component.

Parameters

- **id** Component ID
- **height** Height in pixels
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetHeight("ButtonIdColorThemeDark", 100);
```

### LSG_SetHeight (percent)

```cpp
void LSG_SetHeight(const std::string& id, double percent, bool layout = true);
```

Sets the height of a component as a percent between 0 and 1.

Parameters

- **id** Component ID
- **percent** [0.0 - 1.0]
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetHeight("ButtonIdColorThemeDark", 0.5);
```

### LSG_SetImage

```cpp
void LSG_SetImage(const std::string& id, const std::string& file, bool fill = false);
```

Sets the file path of an image.

Parameters

- **id** \<image\> component ID
- **file** Image file path
- **fill** Scale the image to fill the entire background

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetImage("ImageIdColorThemeDark", "img/dark-24.png");
```

### LSG_SetListItem

```cpp
void LSG_SetListItem(const std::string& id, int row, const std::string& item);
```

Updates and overwrites the item in the list.

Parameters

- **id** \<list\> component ID
- **row**  0-based row index
- **item** New list item value

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetListItem("List", 12, "My updated list item.");
```

### LSG_SetListItems

```cpp
void LSG_SetListItems(const std::string& id, const LSG_Strings& items);
```

Sets the items of the list.

Parameters

- **id** \<list\> component ID
- **items** List items

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_Strings listItems = {
  "Lorem ipsum dolor sit amet",
  "consectetur adipiscing elit",
  "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua",
  "Cursus sit amet dictum sit",
  "Turpis in eu mi bibendum neque egestas congue quisque",
  "Tellus in hac habitasse platea dictumst",
  "Sed elementum tempus egestas sed sed risus pretium quam vulputate",
  "Placerat duis ultricies lacus sed turpis tincidunt",
  "Amet purus gravida quis blandit turpis cursus in hac habitasse",
  "Rutrum tellus pellentesque eu tincidunt tortor aliquam nulla facilisi cras",
  "Vulputate ut pharetra sit amet aliquam id diam maecenas ultricies",
  "Eu augue ut lectus arcu bibendum at varius vel pharetra"
};

LSG_SetListItems("List", listItems);
```

### LSG_SetMargin

```cpp
void LSG_SetMargin(const std::string& id, int margin);
```

Sets the margin around a component.

Parameters

- **id** Component ID
- **margin** Margin in pixels

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetMargin("Root", 5);
```

### LSG_SetMenuItemIcon

```cpp
void LSG_SetMenuItemIcon(const std::string& id, const std::string& imageFile);
```

Sets the icon of the menu-item.

Parameters

- **id** \<menu-item\> component ID
- **imageFile** Image file path

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetMenuItemIcon("MenuIdAbout", "img/info-white-16.png");
```

### LSG_SetMenuItemSelected

```cpp
void LSG_SetMenuItemSelected(const std::string& id, bool selected = true);
```

Highlights the menu item as selected.

Parameters

- **id** \<menu-item\> component ID
- **selected** true to select or false to unselect

Exceptions

- invalid_argument
- runtime_error

### LSG_SetMenuItemValue

```cpp
void LSG_SetMenuItemValue(const std::string& id, const std::string& value);
```

Sets the text value of the menu-item.

Parameters

- **id** \<menu-item\> component ID
- **value** Text value

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetMenuItemValue("MenuIdQuit", "Quit\\tCtrl+Q");
```

### LSG_SetNavigationItemCount

```cpp
void DLL LSG_SetNavigationItemCount(const std::string& id, size_t itemsTotal, size_t itemsPerNavigation = 1);
```

Sets the item count of the navigation.

Parameters

- **id** <\navigation\> component ID
- **itemsTotal** Number of total items that can be navigated
- **itemsPerNavigation** Number of items to navigate by

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetNavigationItemCount("Navigation", 10, 1);
```

### LSG_SetOrientation

```cpp
void LSG_SetOrientation(const std::string& id, LSG_Orientation orientation, bool layout = true);
```

Sets the layout orientation of the children of a component.

Parameters

- **id** Component ID
- **orientation** Horizontal or vertical
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetOrientation("Root", LSG_ORIENTATION_VERTICAL);
```

### LSG_SetPadding

```cpp
void LSG_SetPadding(const std::string& id, int padding);
```

Sets the padding inside a component.

Parameters

- **id** Component ID
- **padding** Padding in pixels

Exceptions

- invalid_argument
- runtime_error

```cpp
LSG_SetPadding("Root", 10);
```

### LSG_SetPage

```cpp
void LSG_SetPage(const std::string& id, int page);
```

Navigates to and sets the page of the list or table.

Parameters

- **id** \<list\> or \<table\> component ID
- **page** 0-based page index

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetPage("List", 0);
```

### LSG_SetPageListItem

```cpp
void LSG_SetPageListItem(const std::string& id, int row, const std::string& item);
```

Updates and overwrites the item on the current page of the list.

Parameters

- **id** \<list\> component ID
- **row**  0-based row index
- **item** New list item value

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetPageListItem("List", 12, "My updated list item.");
```

### LSG_SetPageTableRow

```cpp
void LSG_SetPageTableRow(const std::string& id, int row, const LSG_Strings& columns);
```

Updates and overwrites the row columns on the current page of the table.

Parameters

- **id** \<table\> component ID
- **row** 0-based row index
- **columns** New row columns

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_Strings row = { "Updated Row", "My updated table row" };

LSG_SetPageTableRow("Table", 6, row);
```

### LSG_SetPanelButtons

```cpp
void LSG_SetPanelButtons(const std::string& id, const LSG_Buttons& buttons);
```

Replaces all child compomonents of the panel with the provided buttons.

Parameters

- **id** \<panel\> component ID
- **buttons** Button items

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_Buttons buttons = {
  { .id = "ButtonIdButton1", .text = "Button 1" },
  { .id = "ButtonIdButton2", .text = "Button 2" }
};

LSG_SetPanelButtons("PanelIdButtons", buttons);
```

### LSG_SetProgressValue

```cpp
void LSG_SetProgressValue(const std::string& id, double percent);
```

Sets the value of the progress bar as a percent between 0 and 1.

Parameters

- **id** \<progress-bar\> component ID
- **percent** [0.0-1.0]

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetProgressValue("ProgressBar", 0.5);
```

### LSG_SetSize

```cpp
void LSG_SetSize(const std::string& id, const SDL_Size& size, bool layout = true);
```

Sets the size of a component.

Parameters

- **id** Component ID
- **size** Width and height in pixels
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetSize("MenuIdMenu", SDL_Size(300, 100));
```

### LSG_SetSize (percent)

```cpp
void LSG_SetSize(const std::string& id, double width, double height, bool layout = true);
```

Sets the size of a component as a percent between 0 and 1.

Parameters

- **id** Component ID
- **width** [0.0 - 1.0]
- **height** [0.0 - 1.0]
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetSize("ButtonIdColorThemeDark", 0.25, 0.75);
```

### LSG_SetSliderValue

```cpp
void LSG_SetSliderValue(const std::string& id, double percent);
```

Sets the value of the slider as a percent between 0 and 1.

Parameters

- **id** \<slider\> component ID
- **percent** [0.0-1.0]

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetSliderValue("Slider", 0.5);
```

### LSG_SetSpacing

```cpp
void LSG_SetSpacing(const std::string& id, int spacing);
```

Sets the spacing between child components.

Parameters

- **id** Component ID
- **spacing** Spacing in pixels

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetSpacing("Root", 20);
```

### LSG_SetTableColumnWidth

```cpp
void LSG_SetTableColumnWidth(const std::string& id, int column, int width);
```

Sets the width of the table column.

Parameters

- **id** \<table\> component ID
- **column** 0-based column index
- **width** Width in pixels

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetTableColumnWidth("Table", 0, 25);
```

### LSG_SetTableGroup

```cpp
void LSG_SetTableGroup(const std::string& id, const LSG_TableGroup& group);
```

Sets the rows of the group in the table.

Parameters

- **id** \<table\> component ID
- **group** The group to update

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TableGroup tableGroup = {
    "Quis Hendrerit", {
      { "Adipiscing", "Elit pellentesque habitant morbi tristique senectus et" },
      { "Congue", "Sed egestas egestas fringilla phasellus faucibus scelerisque" },
      { "Consequat", "Ac felis donec et odio pellentesque diam volutpat commodo" }
    }
};

LSG_SetTableGroups("TableWithGroups", tableGroup);
```

### LSG_SetTableGroups

```cpp
void LSG_SetTableGroups(const std::string& id, const LSG_TableGroups& groups);
```

Sets the groups of the table.

Parameters

- **id** \<table\> component ID
- **groups** Groups with rows

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TableGroups tableGroups = {
  {
    "Quis Hendrerit", {
      { "Adipiscing", "Elit pellentesque habitant morbi tristique senectus et" },
      { "Congue", "Sed egestas egestas fringilla phasellus faucibus scelerisque" },
      { "Consequat", "Ac felis donec et odio pellentesque diam volutpat commodo" }
    }
  },
  {
    "Vestibulum", {
      { "Blandit", "Imperdiet nulla malesuada" },
      { "Cursus", "Pellentesque elit eget gravida" },
      { "Risus", "Sociis natoque penatibus" }
    }
  }
};

LSG_SetTableGroups("TableWithGroups", tableGroups);
```

### LSG_SetTableHeader

```cpp
void LSG_SetTableHeader(const std::string& id, const LSG_Strings& header);
```

Sets the header columns of the table.

Parameters

- **id** \<table\> component ID
- **header** Table header columns

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_Strings tableHeader = {
  "DOLOR",
  "MAGNA"
};

LSG_SetTableHeader("Table", tableHeader);
```

### LSG_SetTableRow

```cpp
void LSG_SetTableRow(const std::string& id, int row, const LSG_Strings& columns);
```

Updates and overwrites the row columns in the table.

Parameters

- **id** \<table\> component ID
- **row** 0-based row index
- **columns** New row columns

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_Strings row = { "Updated Row", "My updated table row" };

LSG_SetTableRow("Table", 6, row);
```

### LSG_SetTableRows

```cpp
void LSG_SetTableRows(const std::string& id, const LSG_TableRows& rows);
```

Sets the rows of the table.

Parameters

- **id** \<table\> component ID
- **rows** Table rows

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TableRows rows = {
  { "Adipiscing", "Elit pellentesque habitant morbi tristique senectus et" },
  { "Congue",     "Sed egestas egestas fringilla phasellus faucibus scelerisque" },
  { "Consequat",  "Ac felis donec et odio pellentesque diam volutpat commodo" },
  { "Blandit",    "Imperdiet nulla malesuada" },
  { "Cursus",     "Pellentesque elit eget gravida" },
  { "Risus",      "Sociis natoque penatibus" }
};

LSG_SetTableRows("Table", rows);
```

### LSG_SetText

```cpp
void LSG_SetText(const std::string& id, const std::string& value);
```

Sets the text value of the text.

Parameters

- **id** \<text\> component ID
- **value** Text value

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetText("TextIdColorTheme", "Color Theme");
```

### LSG_SetTextColor

```cpp
void LSG_SetTextColor(const std::string& id, const SDL_Color& color);
```

Sets the text color of a component and all sub-components.

Parameters

- **id** Component ID
- **color** Text color

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetTextColor("TextIdColorTheme", SDL_Color(255, 0, 0, 255));
```

### LSG_SetTextInputValue

```cpp
void LSG_SetTextInputValue(const std::string& id, const std::string& value);
```

Sets the text input value.

Parameters

- **id** \<text-input\> component ID
- **value** Text value

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetTextInputValue("TextInput", "Initial text input value");
```

### LSG_SetTile

```cpp
void LSG_SetTile(const std::string& id, int index, const LSG_TileItem& tile);
```

Updates and overwrites the tile item in the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **index** 0-based tile index position
- **tile** New tile item

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TileItem tile = {
  .image = "/path/to/image/file2.jpg",
  .text  = "Image 2"
};

LSG_SetTile("Tiles", 0, tile);
```

### LSG_SetTiles

```cpp
void LSG_SetTiles(const std::string& id, const LSG_TileItems& tiles);
```

Sets the tile items of the tiles grid.

Parameters

- **id** \<tiles\> component ID
- **tiles** Tile items

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_TileItems tiles = {
  { .image = "", .text = "" },
  { .image = "", .text = "No image" },
  { .image = "/path/to/image/file.jpg",  .text  = "" },
  { .image = "/path/to/image/file1.jpg", .text  = "Image 1" }
};

LSG_SetTiles("Tiles", tiles);
```

### LSG_SetTitle

```cpp
void LSG_SetTitle(const std::string& id, const std::string& title);
```

Sets the header title of the modal, menu or sub-menu.

Parameters

- **id** \<modal\>, \<menu\> or \<menu-sub\> component ID
- **title** Header title

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetTitle("ModalIdAbout", "SDL2 GUI Library");
```

### LSG_SetToggle

```cpp
void LSG_SetToggle(const std::string& id, bool on);
```

Toggles the switch on or off.

Parameters

- **id** \<toggle\> component ID
- **on** true for on or false for off

Exceptions

- invalid_argument
- runtime_error

### LSG_SetVisible

```cpp
void LSG_SetVisible(const std::string& id, bool visible = true, bool layout = true);
```

Shows or hides the component.

Parameters

- **id** Component ID
- **visible** true to show or false to hide
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetVisible("MenuIdMenu", false);
```

### LSG_SetWidth

```cpp
void LSG_SetWidth(const std::string& id, int width, bool layout = true);
```

Sets the width of a component.

Parameters

- **id** Component ID
- **width** Width in pixels
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetWidth("ButtonIdColorThemeDark", 300);
```

### LSG_SetWidth (percent)

```cpp
void LSG_SetWidth(const std::string& id, double percent, bool layout = true);
```

Sets the width of a component as a percent between 0 and 1.

Parameters

- **id** Component ID
- **percent** [0.0 - 1.0]
- **layout** Recalculates and redraws the window layout after applying the change

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SetWidth("ButtonIdColorThemeDark", 0.5);
```

### LSG_SetWindowMaximized

```cpp
void LSG_SetWindowMaximized(bool maximized = true);
```

Maximizes or restores the window.

Parameters

- **maximized** true to maximize or false to restore

Exceptions

- runtime_error

Example

```cpp
if (LSG_IsWindowMaximized())
  LSG_SetWindowMaximized(false);
else
  LSG_SetWindowMaximized();
```

### LSG_SetWindowMinimumSize

```cpp
void LSG_SetWindowMinimumSize(int width, int height);
```

Sets the minimum window size.

Parameters

- **width** Window width
- **height** Window height

Exceptions

- runtime_error

Example

```cpp
LSG_SetWindowMinimumSize(800, 800);
```

### LSG_SetWindowPosition

```cpp
void LSG_SetWindowPosition(int x, int y);
```

Sets the window position.

Parameters

- **x** Window horizontal positon
- **y** Window vertical position

Exceptions

- runtime_error

Example

```cpp
LSG_SetWindowPosition(100, 100);
```

### LSG_SetWindowSize

```cpp
void LSG_SetWindowSize(int width, int height);
```

Sets the window size.

Parameters

- **width** Window width
- **height** Window height

Exceptions

- runtime_error

Example

```cpp
LSG_SetWindowSize(1280, 720);
```

### LSG_SetWindowTitle

```cpp
void LSG_SetWindowTitle(const std::string& title);
```

Sets the window title.

Parameters

- **title** Window title

Exceptions

- runtime_error

Example

```cpp
LSG_SetWindowTitle("New Window");
```

### LSG_ShowColumnBorder

```cpp
void LSG_ShowColumnBorder(const std::string& id, bool show = true);
```

Shows or hides the border/rule between columns.

Parameters

- **id** \<table\> component ID
- **show** true to show or false to hide

Exceptions

- invalid_argument
- runtime_error

### LSG_ShowError

```cpp
void LSG_ShowError(const std::string& message);
```

Shows a modal dialog with an error icon and the error message.

Parameters

- **message** Error message

### LSG_ShowRowBorder

```cpp
void LSG_ShowRowBorder(const std::string& id, bool show = true);
```

Shows or hides the border/rule between rows.

Parameters

- **id** \<list\> or \<table\> component ID
- **show** true to show or false to hide

Exceptions

- invalid_argument
- runtime_error

### LSG_SortList

```cpp
void LSG_SortList(const std::string& id, LSG_SortOrder sortOrder);
```

Sorts the list items.

Parameters

- **id** \<list\> component ID
- **sortOrder** Ascending or descending

Exceptions

- invalid_argument
- runtime_error

Example

```cpp
LSG_SortList("List", LSG_SORT_ORDER_ASCENDING);
```

### LSG_SortTable

```cpp
void LSG_SortTable(const std::string& id, LSG_SortOrder sortOrder, int sortColumn);
```

Sorts the table rows.

Parameters

- **id** \<table\> component ID
- **sortOrder** Ascending or descending
- **sortColumn** Sort column index

Exceptions

- invalid_argument
- runtime_error

```cpp
LSG_SortTable("Table", LSG_SORT_ORDER_ASCENDING, 0);
LSG_SortTable("TableWithGroups", LSG_SORT_ORDER_DESCENDING, 1);
```

### LSG_Start

```cpp
SDL_Renderer* LSG_Start(const std::string& xmlFile);
```

Tries to initialize the library and open a new window based on layout from XML file.

Returns an SDL2 renderer.

Parameters

- **xmlFile** Window and UI component layout file. ex: "ui/main.xml"

Exceptions

- runtime_error

Example

```cpp
SDL_Renderer* renderer = LSG_Start("ui/main.xml");
```
