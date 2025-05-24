cmake_minimum_required(VERSION 3.21)

project (sdl2gui)

set(EXT_LIB_DIR "${LSG_EXT_LIB_DIR}")
set(INC_DIR     "${EXT_LIB_DIR}/include")
set(LIB_DIR     "${EXT_LIB_DIR}/lib")
set(SRC_DIR     "${CMAKE_SOURCE_DIR}")
set(DIST_DIR    "${CMAKE_BINARY_DIR}/dist")

set(CMAKE_CXX_STANDARD          "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# libsdl2gui depends on the following third-party libraries:
# - sdl2, sdl2_image, sdl2_ttf, libtiff, libwebp, libxml2

if (ANDROID)
    set(EXT_LIB_SDL  libSDL2.so)
    set(EXT_LIB_SDLI libSDL2_image.so)
    set(EXT_LIB_SDLT libSDL2_ttf.so)
    set(EXT_LIB_XML  libxml2.so)
elseif (IOS)
    set(EXT_LIB_SDL  libSDL2.a)
    set(EXT_LIB_SDLI libSDL2_image.a)
    set(EXT_LIB_SDLT libSDL2_ttf.a)
    set(EXT_LIB_XML  libxml2.a)
elseif (APPLE)
    set(EXT_LIB_SDL  libSDL2-2.0.0.dylib)
    set(EXT_LIB_SDLI libSDL2_image-2.0.0.800.8.dylib)
    set(EXT_LIB_SDLT libSDL2_ttf-2.0.0.2400.0.dylib)
    set(EXT_LIB_XML  libxml2.16.dylib)
elseif (WIN32)
    set(EXT_LIB_SDL  SDL2.dll)
    set(EXT_LIB_SDLI SDL2_image.dll)
    set(EXT_LIB_SDLT SDL2_ttf.dll)
    set(EXT_LIB_XML  libxml2.dll)
else()
    set(EXT_LIB_SDL  libSDL2-2.0.so.0)
    set(EXT_LIB_SDLI libSDL2_image-2.0.so.0)
    set(EXT_LIB_SDLT libSDL2_ttf-2.0.so.0)
    set(EXT_LIB_XML  libxml2.so.16)
endif()

find_file(LIB_SDL  ${EXT_LIB_SDL}  PATHS ${LIB_DIR} NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_file(LIB_SDLI ${EXT_LIB_SDLI} PATHS ${LIB_DIR} NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_file(LIB_SDLT ${EXT_LIB_SDLT} PATHS ${LIB_DIR} NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_file(LIB_XML  ${EXT_LIB_XML}  PATHS ${LIB_DIR} NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)

set(INCS      "-I${SRC_DIR}/include -I${INC_DIR} -I${INC_DIR}/libxml2")
set(TEST_INCS "-I${SRC_DIR}/include -I${INC_DIR}")

if (WIN32)
    set(LIBS      "SDL2.lib SDL2_image.lib SDL2_ttf.lib libxml2.lib")
    set(TEST_LIBS "SDL2.lib")
else()
    set(LIB_PREP "lib")

    if (IOS)
        set(LIBS "-lSDL2 -lSDL2_image -lwebpdecoder -lwebpdemux -lSDL2_ttf -lfreetype -lxml2")
    else()
        set(LIBS "-lSDL2 -lSDL2_image -lSDL2_ttf -lxml2")
    endif()
    
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

    set(CFG_DIR "$<CONFIG>-${IOS_SDK}/")
    set(DLL_EXT "a")
    set(OUT_DIR "${CFG_DIR}testsdl2gui.app")

    set(FONT_ARIAL "/System/Library/Fonts/Supplemental/Arial Unicode.ttf")
    set(INFO_PLIST "${SRC_DIR}/ios/MacOSXBundleInfo.plist.in")
    set(RC         "${SRC_DIR}/ios/LaunchScreen.storyboard")

    set(OS_LIBS      "-framework MediaPlayer -framework Photos -framework PhotosUI -framework StoreKit -framework UIKit -framework UniformTypeIdentifiers")
    set(OS_TEST_LIBS "-framework AudioToolbox -framework AVFoundation -framework CoreBluetooth -framework CoreFoundation -framework CoreGraphics -framework CoreHaptics -framework CoreMotion -framework CoreServices -framework Foundation -framework GameController -framework ImageIO -framework Metal -framework OpenGLES -framework QuartzCore -framework UIKit")
elseif (APPLE)
    set(OS "_macosx")

    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_FLAGS      "${CMAKE_CXX_FLAGS} -x objective-c++")

    set(CFG_DIR "$<CONFIG>/")
    set(DLL_EXT "dylib")
    set(OUT_DIR "$<CONFIG>")

    set(OS_LIBS "-framework AppKit -framework Foundation -framework UniformTypeIdentifiers")
elseif (WIN32)
    set(OS "_windows")

    set(CFG_DIR "$(Configuration)/")
    set(DLL_EXT "dll")
    set(OUT_DIR "$(Configuration)")
    set(RC      "${SRC_DIR}/windows/libsdl2gui.rc")

    set_property(DIRECTORY ${SRC_DIR} PROPERTY VS_STARTUP_PROJECT testsdl2gui)
else()
    set(OS "_linux")

    set(DLL_EXT "so")
    set(OUT_DIR ".")

    execute_process(COMMAND pkg-config --cflags --keep-system-cflags gtk4 OUTPUT_VARIABLE OS_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND pkg-config --libs   --keep-system-libs   gtk4 OUTPUT_VARIABLE OS_LIBS   OUTPUT_STRIP_TRAILING_WHITESPACE)
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
#target_include_directories(sdl2gui PRIVATE ${INCS})
#target_link_directories(sdl2gui    PRIVATE "/usr/lib/x86_64-linux-gnu" ${LIB_DIR})
target_link_libraries(sdl2gui      PRIVATE "${OS_LIBS} -L${LIB_DIR} ${LIBS}")

#target_link_options(sdl2gui PRIVATE "${OS_LIBS} -L${LIB_DIR} ${LIBS}")

set_property(TARGET sdl2gui PROPERTY COMPILE_FLAGS "${OS_CFLAGS} ${INCS}")
#set_property(TARGET sdl2gui PROPERTY LINK_FLAGS    "${OS_LIBS} -L${LIB_DIR} ${LIBS}")
#set_property(TARGET sdl2gui PROPERTY LINK_OPTIONS  "${OS_LIBS} -L${LIB_DIR} ${LIBS}")

# dist dir

if (WIN32)
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if exist \"${DIST_DIR}\" rmdir /s /q \"${DIST_DIR}\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/bin\"     mkdir \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/include\" mkdir \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND if not exist \"${DIST_DIR}/lib\"     mkdir \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}sdl2gui.lib ${DIST_DIR}/lib/)
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}sdl2gui.dll ${DIST_DIR}/lib/)
else()
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND rm -rf \"${DIST_DIR}\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}${LIB_PREP}sdl2gui.${DLL_EXT} ${DIST_DIR}/lib/)
endif()

# includes

add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${INC_H} ${DIST_DIR}/include/)

# licenses

if (EXISTS "${EXT_LIB_DIR}/LICENSE-sdl2.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-sdl2.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-sdl2_ttf.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-sdl2_image.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-libtiff.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-libwebp.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-libxml2.txt")
endif()

add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${LICENSES}        ${DIST_DIR}/)
add_custom_command(TARGET sdl2gui POST_BUILD COMMAND cp -f ${SRC_DIR}/LICENSE ${DIST_DIR}/LICENSE-libsdl2gui.txt)

list(APPEND LICENSES "${DIST_DIR}/LICENSE-libsdl2gui.txt")

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
#target_include_directories(testsdl2gui PRIVATE ${OS_TEST_INCS} ${TEST_INCS})
#target_link_directories(testsdl2gui    PRIVATE "/usr/lib/x86_64-linux-gnu" ${LIB_DIR})
target_link_libraries(testsdl2gui      PRIVATE "${OS_TEST_LIBS} -L${LIB_DIR} ${TEST_LIBS} libsdl2gui.so")

#target_link_options(testsdl2gui PRIVATE sdl2gui "${OS_TEST_LIBS} -L${LIB_DIR} ${TEST_LIBS}")

set_property(TARGET testsdl2gui PROPERTY COMPILE_FLAGS "${OS_TEST_INCS} ${TEST_INCS}")
#set_property(TARGET testsdl2gui PROPERTY LINK_OPTIONS  "${OS_TEST_LIBS} -L${LIB_DIR} ${TEST_LIBS}" sdl2gui)

if (IOS)
    set(CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.libsdl2gui.test")
    set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY    "1,2")

    set_target_properties(testsdl2gui PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST           "${INFO_PLIST}"
        MACOSX_BUNDLE_BUNDLE_NAME          "SDL2 GUI Library"
        MACOSX_BUNDLE_COPYRIGHT            "(c) 2021 Adam A. Jammary (Jammary Studio)"
        MACOSX_BUNDLE_GUI_IDENTIFIER       "com.libsdl2gui.test"
        MACOSX_BUNDLE_ICON_FILE            "icon"
        MACOSX_BUNDLE_BUNDLE_VERSION       "1.0.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING  "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    )
elseif (WIN32)
    set_property(TARGET testsdl2gui PROPERTY LINK_FLAGS_DEBUG   "-SUBSYSTEM:CONSOLE,5.02")
    set_property(TARGET testsdl2gui PROPERTY LINK_FLAGS_RELEASE "-SUBSYSTEM:WINDOWS,5.02")
endif()

if (ANDROID)
    set_property(TARGET testsdl2gui PROPERTY ADDITIONAL_CLEAN_FILES "android;${DIST_DIR}")
else()
    set_property(TARGET testsdl2gui PROPERTY ADDITIONAL_CLEAN_FILES "${DIST_DIR}")
endif()

if (ANDROID)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/android .)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mkdir -p \"${OUT_DIR}/src/main/assets\")

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img ${OUT_DIR}/src/main/assets/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui  ${OUT_DIR}/src/main/assets/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mkdir -p \"${OUT_DIR}/libs/${ANDROID_ABI}\")

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f libtestsdl2gui.so ${OUT_DIR}/libs/${ANDROID_ABI}/libmain.so)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f libsdl2gui.so     ${OUT_DIR}/libs/${ANDROID_ABI}/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_SDL} ${LIB_SDLI} ${LIB_SDLT} ${LIB_XML} ${OUT_DIR}/libs/${ANDROID_ABI}/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND chmod a+x ${CMAKE_BINARY_DIR}/android/gradlew)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/android/gradlew --stop)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/android/gradlew assembleDebug WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/android)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mv -f ${OUT_DIR}/build/outputs/apk/debug/testsdl2gui-debug.apk ${DIST_DIR}/bin/testsdl2gui-${ANDROID_ABI}-debug.apk)
elseif (IOS)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img       ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND mv -f  ${OUT_DIR}/img/icon* ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui        ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${FONT_ARIAL}        ${OUT_DIR}/ui/)

    add_custom_target(testsdl2gui_app ALL mv -f ${OUT_DIR} ${DIST_DIR}/bin/testsdl2gui-${CMAKE_OSX_ARCHITECTURES}.app DEPENDS testsdl2gui)
elseif (WIN32)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui  ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}testsdl2gui.exe ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}sdl2gui.dll     ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_SDL} ${LIB_SDLI} ${LIB_SDLT} ${LIB_XML} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/img   ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/ui    ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${DIST_DIR}/bin/*.dll ${OUT_DIR}/)
else()
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui  ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}testsdl2gui           ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${CFG_DIR}libsdl2gui.${DLL_EXT} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f ${LIB_SDL} ${LIB_SDLI} ${LIB_SDLT} ${LIB_XML} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/img          ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/ui           ${OUT_DIR}/)
    add_custom_command(TARGET testsdl2gui POST_BUILD COMMAND cp -f  ${DIST_DIR}/bin/*.${DLL_EXT} ${OUT_DIR}/)
endif()

if (OS STREQUAL "_linux")
    add_custom_target(testsdl2gui_rpath ALL DEPENDS testsdl2gui)

    add_custom_command(TARGET testsdl2gui_rpath COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/libsdl2gui.so)
    add_custom_command(TARGET testsdl2gui_rpath COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/testsdl2gui)
elseif (OS STREQUAL "_macosx")
    add_custom_target(testsdl2gui_rpath ALL DEPENDS testsdl2gui)

    add_custom_command(TARGET testsdl2gui_rpath COMMAND install_name_tool -change @rpath/${EXT_LIB_SDL}  @executable_path/${EXT_LIB_SDL}  ${DIST_DIR}/bin/libsdl2gui.dylib)
    add_custom_command(TARGET testsdl2gui_rpath COMMAND install_name_tool -change @rpath/${EXT_LIB_SDLI} @executable_path/${EXT_LIB_SDLI} ${DIST_DIR}/bin/libsdl2gui.dylib)
    add_custom_command(TARGET testsdl2gui_rpath COMMAND install_name_tool -change @rpath/${EXT_LIB_SDLT} @executable_path/${EXT_LIB_SDLT} ${DIST_DIR}/bin/libsdl2gui.dylib)
    add_custom_command(TARGET testsdl2gui_rpath COMMAND install_name_tool -change @rpath/${EXT_LIB_XML}  @executable_path/${EXT_LIB_XML}  ${DIST_DIR}/bin/libsdl2gui.dylib)

    add_custom_command(TARGET testsdl2gui_rpath COMMAND install_name_tool -change @rpath/libsdl2gui.dylib @executable_path/libsdl2gui.dylib ${DIST_DIR}/bin/testsdl2gui)
    add_custom_command(TARGET testsdl2gui_rpath COMMAND install_name_tool -change @rpath/${EXT_LIB_SDL}   @executable_path/${EXT_LIB_SDL}   ${DIST_DIR}/bin/testsdl2gui)
endif()

# install

foreach(LICENSE ${LICENSES})
    cmake_path(NORMAL_PATH LICENSE OUTPUT_VARIABLE LICENSE_NORMALIZED)
    install(FILES ${LICENSE_NORMALIZED} DESTINATION ${CMAKE_INSTALL_PREFIX})
endforeach()

install(DIRECTORY ${DIST_DIR}/bin/     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/testsdl2gui)
install(DIRECTORY ${DIST_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libsdl2gui)
install(DIRECTORY ${DIST_DIR}/lib/     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if (NOT ANDROID AND NOT WIN32)
    install(DIRECTORY ${DIST_DIR}/bin/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/testsdl2gui PATTERN testsdl2gui PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

# unittest

if (WIN32)
    add_library(unittest SHARED "${SRC_DIR}/test/unittest.cpp")

    target_compile_definitions(unittest PRIVATE ${OS})
    target_include_directories(unittest PRIVATE ${TEST_INCS} ${OS_TEST_INCS})
    target_link_directories(unittest    PRIVATE ${LIB_DIR})
    target_link_libraries(unittest      PRIVATE sdl2gui ${TEST_LIBS} ${OS_TEST_LIBS})
endif()
