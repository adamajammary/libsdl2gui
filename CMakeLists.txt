cmake_minimum_required(VERSION 3.21)

project (sdlui)

set(EXT_LIB_DIR "${LSG_EXT_LIB_DIR}")
set(INC_DIR     "${EXT_LIB_DIR}/include")
set(LIB_DIR     "${EXT_LIB_DIR}/lib")
set(SRC_DIR     "${CMAKE_SOURCE_DIR}")
set(DIST_DIR    "${CMAKE_BINARY_DIR}/dist")

set(CMAKE_CXX_STANDARD          "20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# libsdlui depends on the following third-party libraries:
# - sdl3, sdl3_image, sdl3_ttf, libxml2

if (ANDROID)
    set(EXT_LIB_SDL  libSDL3.so)
    set(EXT_LIB_SDLI libSDL3_image.so)
    set(EXT_LIB_SDLT libSDL3_ttf.so)
    set(EXT_LIB_XML  libxml2.so)
elseif (IOS)
    set(EXT_LIB_SDL  libSDL3.a)
    set(EXT_LIB_SDLI libSDL3_image.a)
    set(EXT_LIB_PNG  libpng16.a)
    set(EXT_LIB_WEBP libwebp.a)
    set(EXT_LIB_WPDM libwebpdemux.a)
    set(EXT_LIB_WPM  libwebpmux.a)
    set(EXT_LIB_SDLT libSDL3_ttf.a)
    set(EXT_LIB_FT   libfreetype.a)
    set(EXT_LIB_XML  libxml2.a)
elseif (APPLE)
    set(EXT_LIB_SDL  libSDL3.0.dylib)
    set(EXT_LIB_SDLI libSDL3_image.0.dylib)
    set(EXT_LIB_SDLT libSDL3_ttf.0.dylib)
    set(EXT_LIB_XML  libxml2.16.dylib)
elseif (WIN32)
    set(EXT_LIB_SDL  SDL3.dll)
    set(EXT_LIB_SDLL SDL3.lib)
    set(EXT_LIB_SDLI SDL3_image.dll)
    set(EXT_LIB_SDIL SDL3_image.lib)
    set(EXT_LIB_SDLT SDL3_ttf.dll)
    set(EXT_LIB_SDTL SDL3_ttf.lib)
    set(EXT_LIB_XML  libxml2.dll)
    set(EXT_LIB_XMLL libxml2.lib)
else()
    set(EXT_LIB_SDL  libSDL3.so.0.4.14)
    set(EXT_LIB_SDLI libSDL3_image.so.0.4.4)
    set(EXT_LIB_SDLT libSDL3_ttf.so.0.2.2)
    set(EXT_LIB_XML  libxml2.so.16)
endif()

find_file(LIB_SDL  ${EXT_LIB_SDL}  PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_file(LIB_SDLI ${EXT_LIB_SDLI} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_file(LIB_SDLT ${EXT_LIB_SDLT} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
find_file(LIB_XML  ${EXT_LIB_XML}  PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)

if (IOS)
    find_file(LIB_FT   ${EXT_LIB_FT}   PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_PNG  ${EXT_LIB_PNG}  PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_WEBP ${EXT_LIB_WEBP} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_WPDM ${EXT_LIB_WPDM} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_WPM  ${EXT_LIB_WPM}  PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
elseif (WIN32)
    find_file(LIB_SDLL ${EXT_LIB_SDLL} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_SDIL ${EXT_LIB_SDIL} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_SDTL ${EXT_LIB_SDTL} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
    find_file(LIB_XMLL ${EXT_LIB_XMLL} PATHS "${LIB_DIR}" NO_CACHE REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
endif()

if (NOT EXISTS "${SRC_DIR}/fonts")
    file(MAKE_DIRECTORY "${SRC_DIR}/fonts")

    set(DEJAVU_SANS_TTF "dejavu-fonts-ttf-2.37")

    file(DOWNLOAD "https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/${DEJAVU_SANS_TTF}.zip" "${SRC_DIR}/fonts/${DEJAVU_SANS_TTF}.zip")
    file(DOWNLOAD "https://github.com/notofonts/noto-cjk/raw/refs/heads/main/Sans/OTC/NotoSansCJK-Regular.ttc"         "${SRC_DIR}/fonts/NotoSansCJK-Regular.ttc")
    file(DOWNLOAD "https://raw.githubusercontent.com/notofonts/noto-cjk/refs/heads/main/Sans/LICENSE"                  "${SRC_DIR}/fonts/LICENSE-Noto.txt")

    file(ARCHIVE_EXTRACT INPUT "${SRC_DIR}/fonts/${DEJAVU_SANS_TTF}.zip" DESTINATION "${SRC_DIR}/fonts/")

    file(RENAME "${SRC_DIR}/fonts/${DEJAVU_SANS_TTF}/ttf/DejaVuSans.ttf" "${SRC_DIR}/fonts/DejaVuSans.ttf")
    file(RENAME "${SRC_DIR}/fonts/${DEJAVU_SANS_TTF}/LICENSE"            "${SRC_DIR}/fonts/LICENSE-DejaVu.txt")

    file(REMOVE_RECURSE "${SRC_DIR}/fonts/${DEJAVU_SANS_TTF}")
    file(REMOVE         "${SRC_DIR}/fonts/${DEJAVU_SANS_TTF}.zip")
endif()

set(INCS      "${SRC_DIR}/include" "${INC_DIR}" "${INC_DIR}/libxml2")
set(TEST_INCS "${SRC_DIR}/include" "${INC_DIR}")

if (WIN32)
    set(LIBS      "${LIB_SDLL}" "${LIB_SDIL}" "${LIB_SDTL}" "${LIB_XMLL}")
    set(TEST_LIBS "${LIB_SDLL}")
else()
    set(LIBS      "${LIB_SDL}" "${LIB_SDLI}" "${LIB_SDLT}" "${LIB_XML}")
    set(TEST_LIBS "${LIB_SDL}")

    if (IOS)
        set(LIBS ${LIBS} "${LIB_FT}" "${LIB_PNG}" "${LIB_WEBP}" "${LIB_WPDM}" "${LIB_WPM}")
    endif()
endif()

if (ANDROID)
    set(OS "_android")

    set(DLL_EXT "so")
    set(OUT_DIR "android/testsdlui")

    set(OS_LIBS "-landroid")
elseif (IOS)
    set(OS "_ios")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")

    set(CFG_DIR "$<CONFIG>-${IOS_SDK}/")
    set(DLL_EXT "a")
    set(OUT_DIR "${CFG_DIR}testsdlui.app")

    set(INFO_PLIST "${SRC_DIR}/ios/MacOSXBundleInfo.plist.in")
    set(RC         "${SRC_DIR}/ios/LaunchScreen.storyboard")

    set(OS_LIBS      "-framework MediaPlayer" "-framework Photos" "-framework PhotosUI" "-framework StoreKit" "-framework UIKit" "-framework UniformTypeIdentifiers")
    set(OS_TEST_LIBS "-framework AudioToolbox" "-framework AVFoundation" "-framework CoreBluetooth" "-framework CoreFoundation" "-framework CoreGraphics" "-framework CoreHaptics" "-framework CoreMotion" "-framework CoreServices" "-framework Foundation" "-framework GameController" "-framework ImageIO" "-framework Metal" "-framework OpenGLES" "-framework QuartzCore" "-framework UIKit")
elseif (APPLE)
    set(OS "_macosx")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")

    set(CFG_DIR "$<CONFIG>/")
    set(DLL_EXT "dylib")
    set(OUT_DIR "$<CONFIG>")

    set(OS_LIBS "-framework AppKit" "-framework Foundation" "-framework UniformTypeIdentifiers")
elseif (WIN32)
    set(OS "_windows")

    set(CFG_DIR "$(Configuration)/")
    set(DLL_EXT "dll")
    set(OUT_DIR "$(Configuration)")
    set(RC      "${SRC_DIR}/windows/libsdlui.rc")

    set_property(DIRECTORY ${SRC_DIR} PROPERTY VS_STARTUP_PROJECT testsdlui)
else()
    set(OS "_linux")

    set(DLL_EXT "so")
    set(OUT_DIR ".")

    execute_process(COMMAND pkg-config --cflags --keep-system-cflags gtk+-3.0 OUTPUT_VARIABLE OS_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND pkg-config --libs   --keep-system-libs   gtk+-3.0 OUTPUT_VARIABLE OS_LIBS   OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

file(GLOB INC_H "${SRC_DIR}/include/*.h")
file(GLOB IMG   "${SRC_DIR}/img/*")
file(GLOB UI    "${SRC_DIR}/ui/*")

source_group("Images"    FILES ${IMG})
source_group("Resources" FILES ${RC})
source_group("UI"        FILES ${UI})

# libsdlui

file(GLOB SOURCES_H   "${SRC_DIR}/src/*.h")
file(GLOB SOURCES_CPP "${SRC_DIR}/src/*.cpp")

if (IOS)
    add_library(sdlui STATIC ${SOURCES_CPP} ${SOURCES_H} ${INC_H})
else()
    add_library(sdlui SHARED ${SOURCES_CPP} ${SOURCES_H} ${INC_H} ${RC})
endif()

set_property(TARGET sdlui PROPERTY COMPILE_FLAGS ${OS_CFLAGS})

target_compile_definitions(sdlui PRIVATE ${OS} _CRT_SECURE_NO_WARNINGS NOMINMAX MAKEDLL)
target_include_directories(sdlui PRIVATE ${INCS})
target_link_libraries(sdlui      PRIVATE ${OS_LIBS} ${LIBS})

# dist dir

if (WIN32)
    add_custom_command(TARGET sdlui POST_BUILD COMMAND if exist \"${DIST_DIR}\" rmdir /s /q \"${DIST_DIR}\")

    add_custom_command(TARGET sdlui POST_BUILD COMMAND if not exist \"${DIST_DIR}/bin\"     mkdir \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdlui POST_BUILD COMMAND if not exist \"${DIST_DIR}/include\" mkdir \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdlui POST_BUILD COMMAND if not exist \"${DIST_DIR}/lib\"     mkdir \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdlui POST_BUILD COMMAND cp -f ${CFG_DIR}sdlui.lib ${DIST_DIR}/lib/)
    add_custom_command(TARGET sdlui POST_BUILD COMMAND cp -f ${CFG_DIR}sdlui.dll ${DIST_DIR}/lib/)
else()
    add_custom_command(TARGET sdlui POST_BUILD COMMAND rm -rf \"${DIST_DIR}\")

    add_custom_command(TARGET sdlui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/bin\")
    add_custom_command(TARGET sdlui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/include\")
    add_custom_command(TARGET sdlui POST_BUILD COMMAND mkdir -p \"${DIST_DIR}/lib\")

    add_custom_command(TARGET sdlui POST_BUILD COMMAND cp -f ${CFG_DIR}libsdlui.${DLL_EXT} ${DIST_DIR}/lib/)
endif()

# includes

add_custom_command(TARGET sdlui POST_BUILD COMMAND cp -f ${INC_H} ${DIST_DIR}/include/)

# licenses

if (EXISTS "${EXT_LIB_DIR}/LICENSE-sdl2.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-sdl2.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-sdl2_ttf.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-sdl2_image.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-libtiff.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-libwebp.txt")
    list(APPEND LICENSES "${EXT_LIB_DIR}/LICENSE-libxml2.txt")
endif()

list(LENGTH LICENSES NR_OF_LICENSES)

if (NR_OF_LICENSES GREATER 0)
    add_custom_command(TARGET sdlui POST_BUILD COMMAND cp -f ${LICENSES} ${DIST_DIR}/)
endif()

add_custom_command(TARGET sdlui POST_BUILD COMMAND cp -f ${SRC_DIR}/LICENSE ${DIST_DIR}/LICENSE-libsdlui.txt)

list(APPEND LICENSES "${DIST_DIR}/LICENSE-libsdlui.txt")

# testsdlui

set(TEST_CPP "${SRC_DIR}/test/main.cpp")

if (ANDROID)
    add_library(testsdlui SHARED ${TEST_CPP} ${INC_H} ${IMG} ${UI})
elseif (IOS)
    add_executable(testsdlui MACOSX_BUNDLE ${TEST_CPP} ${INC_H} ${RC} ${IMG} ${UI})
else()
    add_executable(testsdlui ${TEST_CPP} ${INC_H} ${IMG} ${UI})
endif()

target_compile_definitions(testsdlui PRIVATE ${OS})
target_include_directories(testsdlui PRIVATE ${OS_TEST_INCS} ${TEST_INCS})
target_link_libraries(testsdlui      PRIVATE sdlui ${OS_LIBS} ${LIBS} ${OS_TEST_LIBS} ${TEST_LIBS})

if (IOS)
    set(CMAKE_XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.libsdlui.test")
    set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY    "1,2")

    set_target_properties(testsdlui PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST           "${INFO_PLIST}"
        MACOSX_BUNDLE_BUNDLE_NAME          "SDL UI Library"
        MACOSX_BUNDLE_COPYRIGHT            "(c) 2021 Adam A. Jammary (Jammary Studio)"
        MACOSX_BUNDLE_GUI_IDENTIFIER       "com.libsdlui.test"
        MACOSX_BUNDLE_ICON_FILE            "icon"
        MACOSX_BUNDLE_BUNDLE_VERSION       "1.7.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING  "1.7.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.7.0"
    )
elseif (WIN32)
    set_property(TARGET testsdlui PROPERTY LINK_FLAGS_DEBUG   "-SUBSYSTEM:CONSOLE,5.02")
    set_property(TARGET testsdlui PROPERTY LINK_FLAGS_RELEASE "-SUBSYSTEM:WINDOWS,5.02")
endif()

if (ANDROID)
    set_property(TARGET testsdlui PROPERTY ADDITIONAL_CLEAN_FILES "android;${DIST_DIR}")
else()
    set_property(TARGET testsdlui PROPERTY ADDITIONAL_CLEAN_FILES "${DIST_DIR}")
endif()

if (ANDROID)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/android .)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND mkdir -p \"${OUT_DIR}/src/main/assets\")

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/fonts ${OUT_DIR}/src/main/assets/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img   ${OUT_DIR}/src/main/assets/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui    ${OUT_DIR}/src/main/assets/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND mkdir -p \"${OUT_DIR}/libs/${ANDROID_ABI}\")

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f libtestsdlui.so ${OUT_DIR}/libs/${ANDROID_ABI}/libmain.so)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f libsdlui.so     ${OUT_DIR}/libs/${ANDROID_ABI}/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${LIB_SDL} ${LIB_SDLI} ${LIB_SDLT} ${LIB_XML} ${OUT_DIR}/libs/${ANDROID_ABI}/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND chmod a+x ${CMAKE_BINARY_DIR}/android/gradlew)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/android/gradlew --stop)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND ${CMAKE_BINARY_DIR}/android/gradlew assembleDebug WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/android)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND mv -f ${OUT_DIR}/build/outputs/apk/debug/testsdlui-debug.apk ${DIST_DIR}/bin/testsdlui-${ANDROID_ABI}-debug.apk)
elseif (IOS)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/fonts     ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img       ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND mv -f  ${OUT_DIR}/img/icon* ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui        ${OUT_DIR}/)

    add_custom_target(testsdlui_app ALL mv -f ${OUT_DIR} ${DIST_DIR}/bin/testsdlui-${CMAKE_OSX_ARCHITECTURES}.app DEPENDS testsdlui)
elseif (WIN32)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/fonts ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img   ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui    ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${CFG_DIR}testsdlui.exe ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${CFG_DIR}sdlui.dll     ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${LIB_SDL} ${LIB_SDLI} ${LIB_SDLT} ${LIB_XML} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/fonts ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/img   ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/ui    ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f  ${DIST_DIR}/bin/*.dll ${OUT_DIR}/)
else()
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/fonts ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/img   ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${SRC_DIR}/ui    ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${CFG_DIR}testsdlui           ${DIST_DIR}/bin/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${CFG_DIR}libsdlui.${DLL_EXT} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f ${LIB_SDL} ${LIB_SDLI} ${LIB_SDLT} ${LIB_XML} ${DIST_DIR}/bin/)

    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/fonts        ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/img          ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -rf ${DIST_DIR}/bin/ui           ${OUT_DIR}/)
    add_custom_command(TARGET testsdlui POST_BUILD COMMAND cp -f  ${DIST_DIR}/bin/*.${DLL_EXT} ${OUT_DIR}/)
endif()

if (OS STREQUAL "_linux")
    add_custom_target(testsdlui_rpath ALL DEPENDS testsdlui)

    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/${EXT_LIB_SDL})
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/${EXT_LIB_SDLI})
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/${EXT_LIB_SDLT})
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/${EXT_LIB_XML})
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/libsdlui.so)
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND patchelf --set-rpath '\$$ORIGIN' ${DIST_DIR}/bin/testsdlui)
elseif (OS STREQUAL "_macosx")
    add_custom_target(testsdlui_rpath ALL DEPENDS testsdlui)

    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND install_name_tool -change @rpath/${EXT_LIB_SDL}   @executable_path/${EXT_LIB_SDL}   ${DIST_DIR}/bin/testsdlui)
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND install_name_tool -change @rpath/${EXT_LIB_SDLI}  @executable_path/${EXT_LIB_SDLI}  ${DIST_DIR}/bin/testsdlui)
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND install_name_tool -change @rpath/${EXT_LIB_SDLT}  @executable_path/${EXT_LIB_SDLT}  ${DIST_DIR}/bin/testsdlui)
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND install_name_tool -change @rpath/${EXT_LIB_XML}   @executable_path/${EXT_LIB_XML}   ${DIST_DIR}/bin/testsdlui)
    add_custom_command(TARGET testsdlui_rpath POST_BUILD COMMAND install_name_tool -change @rpath/libsdlui.dylib   @executable_path/libsdlui.dylib   ${DIST_DIR}/bin/testsdlui)
endif()

# install

foreach(LICENSE ${LICENSES})
    cmake_path(NORMAL_PATH LICENSE OUTPUT_VARIABLE LICENSE_NORMALIZED)
    install(FILES ${LICENSE_NORMALIZED} DESTINATION ${CMAKE_INSTALL_PREFIX})
endforeach()

file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/bin/testsdlui)
file(REMOVE         ${CMAKE_INSTALL_PREFIX}/include/libsdlui)

install(DIRECTORY ${DIST_DIR}/bin/     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/testsdlui)
install(DIRECTORY ${DIST_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/libsdlui)
install(DIRECTORY ${DIST_DIR}/lib/     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if (OS STREQUAL "_linux" OR OS STREQUAL "_macosx")
    install(FILES ${DIST_DIR}/bin/testsdlui DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/testsdlui PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

# unittest

if (WIN32)
    add_library(unittest SHARED "${SRC_DIR}/test/unittest.cpp")

    target_compile_definitions(unittest PRIVATE ${OS})
    target_include_directories(unittest PRIVATE ${TEST_INCS})
    target_link_libraries(unittest      PRIVATE sdlui)
endif()
