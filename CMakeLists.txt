cmake_minimum_required(VERSION 3.12)

project (sdl2gui)

# libsdl2gui depends on the following third-party libraries:
# - sdl2, sdl2_image, sdl2_ttf, libxml2

set(EXT_LIB_DIR "${EXT_LIB_DIR}")
set(INC_DIR     "${EXT_LIB_DIR}/include")
set(LIB_DIR     "${EXT_LIB_DIR}/lib")
set(SRC_DIR     "${CMAKE_SOURCE_DIR}")
set(DIST_DIR    "${CMAKE_BINARY_DIR}/dist")

set(CMAKE_CXX_STANDARD          "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCS      "include" ${INC_DIR} "${INC_DIR}/libxml2")
set(TEST_INCS "include" ${INC_DIR})

if (WIN32)
    set(LIBS      SDL2.lib SDL2_image.lib SDL2_ttf.lib libxml2.lib)
    set(TEST_LIBS SDL2.lib)
else()
    set(LIB_PREP "lib")

    set(LIBS      "-lSDL2 -lSDL2_image -lSDL2_ttf -lxml2")
    set(TEST_LIBS "-lSDL2")
endif()

if (ANDROID)
    set(OS "_android")

    set(DLL_EXT "so")
    set(OUT_DIR "android/testsdl2gui")

    set(OS_LIBS "-landroid")
elseif (IOS)
    set(OS "_ios")

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -x objective-c++")

    set(CFG_DIR "$<CONFIG>-iphoneos/")
    set(DLL_EXT "a")
    set(OUT_DIR "${CFG_DIR}testsdl2gui.app")

    set(FONT_ARIAL "/System/Library/Fonts/Supplemental/Arial Unicode.ttf")
    set(INFO_PLIST "${SRC_DIR}/ios/MacOSXBundleInfo.plist.in")
    set(RC         "${SRC_DIR}/ios/LaunchScreen.storyboard")

    set(OS_LIBS      "-framework UIKit")
    set(OS_TEST_LIBS "iconv -framework AudioToolbox -framework AVFoundation -framework CoreBluetooth -framework CoreFoundation -framework CoreGraphics -framework CoreHaptics -framework CoreMotion -framework CoreServices -framework Foundation -framework GameController -framework ImageIO -framework Metal -framework OpenGLES -framework QuartzCore -framework UIKit")
elseif (APPLE)
    set(OS "_macosx")

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -x objective-c++")

    set(CFG_DIR "$<CONFIG>/")
    set(DLL_EXT "dylib")
    set(OUT_DIR "$<CONFIG>")

    set(OS_LIBS "-framework AppKit -framework Foundation")
elseif (LINUX)
    set(OS "_linux")

    set(DLL_EXT "so")
    set(OUT_DIR ".")

    set(OS_INCS "/usr/include/gtk-3.0" "/usr/include/pango-1.0" "/usr/include/glib-2.0" "/usr/lib/x86_64-linux-gnu/glib-2.0/include" "/usr/include/harfbuzz" "/usr/include/freetype2" "/usr/include/libpng16" "/usr/include/libmount" "/usr/include/blkid" "/usr/include/fribidi" "/usr/include/cairo" "/usr/include/pixman-1" "/usr/include/gdk-pixbuf-2.0" "/usr/include/x86_64-linux-gnu" "/usr/include/gio-unix-2.0" "/usr/include/atk-1.0" "/usr/include/at-spi2-atk/2.0" "/usr/include/at-spi-2.0" "/usr/include/dbus-1.0" "/usr/lib/x86_64-linux-gnu/dbus-1.0/include")
    set(OS_LIBS "-lgtk-3 -lgdk-3 -lz -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0")
elseif (WIN32)
    set(OS "_windows")

    set(CFG_DIR "$(Configuration)/")
    set(DLL_EXT "dll")
    set(OUT_DIR "$(Configuration)")
    set(RC      "${SRC_DIR}/windows/libsdl2gui.rc")

    set_property(DIRECTORY ${SRC_DIR} PROPERTY VS_STARTUP_PROJECT testsdl2gui)
endif()

file(GLOB INC_H "${SRC_DIR}/include/*.h")
file(GLOB IMG   "${SRC_DIR}/img/*")
file(GLOB UI    "${SRC_DIR}/ui/*")

source_group("Images"    FILES ${IMG})
source_group("Resources" FILES ${RC})
source_group("UI"        FILES ${UI})

# libsdl2gui

file(GLOB SOURCES_H   "${SRC_DIR}/src/*.h")
file(GLOB SOURCES_CPP "${SRC_DIR}/src/*.cpp")

if (IOS)
    add_library(sdl2gui STATIC ${SOURCES_CPP} ${SOURCES_H} ${INC_H})
else()
    add_library(sdl2gui SHARED ${SOURCES_CPP} ${SOURCES_H} ${INC_H} ${RC})
endif()

target_compile_definitions(sdl2gui PRIVATE ${OS} _CRT_SECURE_NO_WARNINGS NOMINMAX MAKEDLL)
target_include_directories(sdl2gui PRIVATE ${INCS} ${OS_INCS})
target_link_directories(sdl2gui    PRIVATE ${LIB_DIR})
target_link_libraries(sdl2gui      PRIVATE ${LIBS} ${OS_LIBS})

if (OS STREQUAL "_macosx")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND install_name_tool -change @rpath/SDL2_image.dylib  @rpath/libSDL2_image.dylib ${CFG_DIR}libsdl2gui.dylib)
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND install_name_tool -change @rpath/SDL2_ttf.dylib    @rpath/libSDL2_ttf.dylib   ${CFG_DIR}libsdl2gui.dylib)
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND install_name_tool -change /usr/lib/libxml2.2.dylib @rpath/libxml2.dylib       ${CFG_DIR}libsdl2gui.dylib)
endif()

if (WIN32)
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/bin\"     mkdir \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/include\" mkdir \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/lib\"     mkdir \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}sdl2gui.lib ${DIST_DIR}/lib/)
else()
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}${LIB_PREP}sdl2gui.${DLL_EXT} ${DIST_DIR}/lib/)
endif()

add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${SRC_DIR}/include/*.h ${DIST_DIR}/include/)

add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${SRC_DIR}/LICENSE                    ${DIST_DIR}/LICENSE.txt)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-sdl2.txt       ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-sdl2_ttf.txt   ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-sdl2_image.txt ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${EXT_LIB_DIR}/LICENSE-libxml2.txt    ${DIST_DIR}/)

# testsdl2gui

set(TEST_CPP "${SRC_DIR}/test/main.cpp")

if (ANDROID)
    add_library(testsdl2gui SHARED ${TEST_CPP} ${INC_H} ${IMG} ${UI})
elseif (IOS)
    add_executable(testsdl2gui MACOSX_BUNDLE ${TEST_CPP} ${INC_H} ${RC} ${IMG} ${UI})
else()
    add_executable(testsdl2gui ${TEST_CPP} ${INC_H} ${IMG} ${UI})
endif()

target_compile_definitions(testsdl2gui PRIVATE ${OS})
target_include_directories(testsdl2gui PRIVATE ${TEST_INCS} ${OS_TEST_INCS})
target_link_directories(testsdl2gui    PRIVATE ${LIB_DIR})
target_link_libraries(testsdl2gui      PRIVATE sdl2gui ${TEST_LIBS} ${OS_TEST_LIBS})

if (ANDROID)
    set_property(TARGET testsdl2gui PROPERTY ADDITIONAL_CLEAN_FILES "android;${DIST_DIR}")
elseif (IOS)
    set(CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.libsdl2gui.test")
    set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY    "1,2")

    set_target_properties(testsdl2gui PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST           "${INFO_PLIST}"
        MACOSX_BUNDLE_BUNDLE_NAME          "Test SDL2 GUI"
        MACOSX_BUNDLE_COPYRIGHT            "(c) 2021 Adam A. Jammary (Jammary Studio)"
        MACOSX_BUNDLE_GUI_IDENTIFIER       "com.libsdl2gui.test"
        MACOSX_BUNDLE_ICON_FILE            "icon"
        MACOSX_BUNDLE_BUNDLE_VERSION       "1.0.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING  "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )

    set_property(TARGET testsdl2gui PROPERTY ADDITIONAL_CLEAN_FILES "${DIST_DIR}")
elseif (WIN32)
    set_property(TARGET testsdl2gui PROPERTY LINK_FLAGS_DEBUG   "-SUBSYSTEM:CONSOLE,5.02")
    set_property(TARGET testsdl2gui PROPERTY LINK_FLAGS_RELEASE "-SUBSYSTEM:WINDOWS,5.02")

    set_property(TARGET testsdl2gui PROPERTY ADDITIONAL_CLEAN_FILES "${DIST_DIR}")
endif()

if (ANDROID)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/android .)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mkdir -p \"${OUT_DIR}/src/main/assets\")

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img ${OUT_DIR}/src/main/assets/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui  ${OUT_DIR}/src/main/assets/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mkdir -p \"${OUT_DIR}/libs/arm64-v8a\")

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f libtestsdl2gui.so ${OUT_DIR}/libs/arm64-v8a/libmain.so)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f libsdl2gui.so     ${OUT_DIR}/libs/arm64-v8a/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libSDL2.so       ${OUT_DIR}/libs/arm64-v8a/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libSDL2_image.so ${OUT_DIR}/libs/arm64-v8a/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libSDL2_ttf.so   ${OUT_DIR}/libs/arm64-v8a/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libxml2.so       ${OUT_DIR}/libs/arm64-v8a/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/android/gradlew --stop)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/android/gradlew assembleDebug WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/android)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${OUT_DIR}/build/outputs/apk/debug/testsdl2gui-debug.apk ${DIST_DIR}/bin/)
elseif (IOS)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img       ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mv -f  ${OUT_DIR}/img/icon* ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui        ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${FONT_ARIAL}        ${OUT_DIR}/ui/)

    add_custom_target(testsdl2gui_app ALL cp -rf ${OUT_DIR} ${DIST_DIR}/bin/ DEPENDS testsdl2gui)
elseif (WIN32)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui  ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}testsdl2gui.exe ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}sdl2gui.dll     ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/SDL2.dll       ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/SDL2_image.dll ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/SDL2_ttf.dll   ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libxml2.dll    ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/img   ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/ui    ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${DIST_DIR}/bin/*.dll ${OUT_DIR}/)
else()
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui  ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}testsdl2gui           ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}libsdl2gui.${DLL_EXT} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libSDL2.${DLL_EXT}       ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libSDL2_image.${DLL_EXT} ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libSDL2_ttf.${DLL_EXT}   ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_DIR}/libxml2.${DLL_EXT}       ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/img          ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/ui           ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${DIST_DIR}/bin/*.${DLL_EXT} ${OUT_DIR}/)
endif()

# install

install(DIRECTORY ${DIST_DIR}/bin/     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${DIST_DIR}/lib/     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY ${DIST_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libsdl2gui)
install(DIRECTORY ${DIST_DIR}/         DESTINATION ${CMAKE_INSTALL_PREFIX} FILES_MATCHING PATTERN "*.txt")

if (LINUX OR IOS OR APPLE)
    install(DIRECTORY ${DIST_DIR}/bin/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin PATTERN testsdl2gui PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

# unittest

if (WIN32)
    add_library(unittest SHARED "${SRC_DIR}/test/unittest.cpp")

    target_compile_definitions(unittest PRIVATE ${OS})
    target_include_directories(unittest PRIVATE ${TEST_INCS} ${OS_TEST_INCS})
    target_link_directories(unittest    PRIVATE ${LIB_DIR})
    target_link_libraries(unittest      PRIVATE sdl2gui ${TEST_LIBS} ${OS_TEST_LIBS})
endif()
