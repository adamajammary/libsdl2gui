cmake_minimum_required(VERSION 3.12)

project (testsdl2gui)
project (sdl2gui)

# libsdl2gui depends on the following third-party libraries:
# - sdl2, sdl2_image, sdl2_ttf, libxml2

set(EXT_LIB_DIR "${EXT_LIB_DIR}")
set(INC_DIR     "${EXT_LIB_DIR}/include")
set(LIB_DIR     "${EXT_LIB_DIR}/lib")

set(CMAKE_CXX_STANDARD          "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (ANDROID)
    set(OS "_android")
elseif (IOS)
    set(OS "_ios")

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_MACOSX_BUNDLE  OFF)

    set(IOS_LIBS "iconv -framework AudioToolbox -framework AVFAudio -framework CoreBluetooth -framework CoreFoundation -framework CoreGraphics -framework CoreHaptics -framework CoreMotion -framework Foundation -framework GameController -framework ImageIO -framework Metal -framework MobileCoreServices -framework OpenGLES -framework QuartzCore -framework UIKit")
elseif (APPLE)
    set(OS "_macosx")

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -x objective-c++")

    set(MACOSX_LIBS "-framework AppKit")
elseif (LINUX)
    set(OS "_linux")

    set(GTK_INC /usr/include/gtk-3.0 /usr/include/pango-1.0 /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/harfbuzz /usr/include/freetype2 /usr/include/libpng16 /usr/include/libmount /usr/include/blkid /usr/include/fribidi /usr/include/cairo /usr/include/pixman-1 /usr/include/gdk-pixbuf-2.0 /usr/include/x86_64-linux-gnu /usr/include/gio-unix-2.0 /usr/include/atk-1.0 /usr/include/at-spi2-atk/2.0 /usr/include/at-spi-2.0 /usr/include/dbus-1.0 /usr/lib/x86_64-linux-gnu/dbus-1.0/include)
    set(GTK_LIB -lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0)
elseif (WIN32)
    set(OS "_windows")
    set(RC "windows/libsdl2gui.rc")

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT testsdl2gui)
endif()

if (WIN32)
    set(LIBS      SDL2.lib SDL2_image.lib SDL2_ttf.lib libxml2.lib)
    set(TEST_LIBS SDL2.lib)
else()
    set(LIBS      -lSDL2 -lSDL2_image -lSDL2_ttf -lxml2)
    set(TEST_LIBS -lSDL2)
endif()

file(GLOB INC_H       "include/*.h")
file(GLOB IMG         "img/*")
file(GLOB SOURCES_H   "src/*.h")
file(GLOB SOURCES_CPP "src/*.cpp")
file(GLOB TEST_H      "test/*.h")
file(GLOB TEST_CPP    "test/*.cpp")
file(GLOB UI          "ui/*")

if (IOS)
    add_library(sdl2gui STATIC ${SOURCES_CPP} ${SOURCES_H} ${INC_H} ${RC})
else()
    add_library(sdl2gui SHARED ${SOURCES_CPP} ${SOURCES_H} ${INC_H} ${RC})
endif()

source_group("Resources" FILES ${RC})
target_compile_definitions(sdl2gui PRIVATE ${OS} _CRT_SECURE_NO_WARNINGS NOMINMAX MAKEDLL)
target_include_directories(sdl2gui PRIVATE "include" ${INC_DIR} "${INC_DIR}/libxml2" ${GTK_INC})
target_link_directories(sdl2gui PRIVATE ${LIB_DIR})
target_link_libraries(sdl2gui PRIVATE ${LIBS} ${GTK_LIB} ${MACOSX_LIBS})

add_executable(testsdl2gui ${TEST_CPP} ${TEST_H} ${INC_H} ${IMG} ${UI})
source_group("Images" FILES ${IMG})
source_group("UI" FILES ${UI})
target_compile_definitions(testsdl2gui PRIVATE ${OS})
target_include_directories(testsdl2gui PRIVATE "include" ${INC_DIR})
target_link_directories(testsdl2gui PRIVATE ${LIB_DIR})
target_link_libraries(testsdl2gui PRIVATE sdl2gui ${TEST_LIBS} ${IOS_LIBS})

if (WIN32)
    set_target_properties(testsdl2gui PROPERTIES LINK_FLAGS_DEBUG   "-SUBSYSTEM:CONSOLE,5.02")
    set_target_properties(testsdl2gui PROPERTIES LINK_FLAGS_RELEASE "-SUBSYSTEM:WINDOWS,5.02")
endif()

set(BUILD_DIR "${CMAKE_BINARY_DIR}")
set(SRC_DIR   "${CMAKE_SOURCE_DIR}")
set(DIST_DIR  "${BUILD_DIR}/dist")

if (IOS)
    set(DLL_EXT "a")
    set(OUT_DIR "${BUILD_DIR}/out")
    set(CFG_DIR "Release-iphoneos/")
elseif (APPLE)
    set(DLL_EXT "dylib")
    set(OUT_DIR "${BUILD_DIR}/out")
    set(CFG_DIR "Release/")
elseif (WIN32)
    set(DLL_EXT "dll")
    set(OUT_DIR "${BUILD_DIR}/$(Configuration)")
else()
    set(DLL_EXT "so")
    set(OUT_DIR "${BUILD_DIR}/out")
endif()

if (WIN32)
    set(TEST_EXE "testsdl2gui.exe")
else()
    set(LIB_PREP "lib")
    set(TEST_EXE "testsdl2gui")
endif()

if (OS STREQUAL "_macosx")
    add_custom_command(TARGET sdl2gui PRE_LINK COMMAND install_name_tool -id @rpath/libSDL2.dylib       ${LIB_DIR}/libSDL2.dylib)
    add_custom_command(TARGET sdl2gui PRE_LINK COMMAND install_name_tool -id @rpath/libSDL2_image.dylib ${LIB_DIR}/libSDL2_image.dylib)
    add_custom_command(TARGET sdl2gui PRE_LINK COMMAND install_name_tool -id @rpath/libSDL2_ttf.dylib   ${LIB_DIR}/libSDL2_ttf.dylib)
    add_custom_command(TARGET sdl2gui PRE_LINK COMMAND install_name_tool -id @rpath/libxml2.dylib       ${LIB_DIR}/libxml2.dylib)

    add_custom_command(TARGET sdl2gui PRE_LINK COMMAND install_name_tool -change @executable_path/../Frameworks/libSDL2.dylib @rpath/libSDL2.dylib ${LIB_DIR}/libSDL2_image.dylib)
    add_custom_command(TARGET sdl2gui PRE_LINK COMMAND install_name_tool -change @executable_path/../Frameworks/libSDL2.dylib @rpath/libSDL2.dylib ${LIB_DIR}/libSDL2_ttf.dylib)
endif()

if (WIN32)
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/bin\"     mkdir \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/include\" mkdir \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/lib\"     mkdir \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${OUT_DIR}/*.lib ${DIST_DIR}/lib/)
else()
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir \"${OUT_DIR}\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}${LIB_PREP}sdl2gui* ${OUT_DIR}/)
endif()

add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/${LIB_PREP}SDL2.${DLL_EXT}       ${OUT_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/${LIB_PREP}SDL2_image.${DLL_EXT} ${OUT_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/${LIB_PREP}SDL2_ttf.${DLL_EXT}   ${OUT_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libxml2.${DLL_EXT}               ${OUT_DIR}/)

add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${OUT_DIR}/*.${DLL_EXT}                  ${DIST_DIR}/bin/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${OUT_DIR}/${LIB_PREP}sdl2gui.${DLL_EXT} ${DIST_DIR}/lib/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${SRC_DIR}/include/*.h                   ${DIST_DIR}/include/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${SRC_DIR}/LICENSE                       ${DIST_DIR}/LICENSE.txt)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-sdl2.txt          ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-sdl2_ttf.txt      ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-sdl2_image.txt    ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-libxml2.txt       ${DIST_DIR}/)

add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ../img ${OUT_DIR}/)
add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ../ui  ${OUT_DIR}/)

if (NOT WIN32)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}${TEST_EXE} ${OUT_DIR}/)
endif()

add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${OUT_DIR}/img         ${DIST_DIR}/bin/)
add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${OUT_DIR}/ui          ${DIST_DIR}/bin/)
add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${OUT_DIR}/${TEST_EXE} ${DIST_DIR}/bin/)

install(DIRECTORY ${DIST_DIR}/bin/     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PATTERN ${TEST_EXE} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(DIRECTORY ${DIST_DIR}/lib/     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${DIST_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libsdl2gui)
install(DIRECTORY ${DIST_DIR}/         DESTINATION ${CMAKE_INSTALL_PREFIX} FILES_MATCHING PATTERN "*.txt")
