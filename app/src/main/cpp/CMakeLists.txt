# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Declares and names the project.

project("sphereball")

add_definitions( -D_IRR_ANDROID_PLATFORM_=1 )

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        app-glue

        # Sets the library as a shared library.
        STATIC

        # Provides a relative path to your source file(s).
        # Associated headers in the same location as their source
        # file are automatically included.
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c )

add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        # Associated headers in the same location as their source
        # file are automatically included.
        Alarm.cpp
        Camera.cpp
        Coin.cpp
        Editor.cpp
        Engine.cpp
        EngineObject.cpp
        EventManager.cpp
        Exit.cpp
        Fire.cpp
        Fruit.cpp
        GUIImageSceneNode.cpp
        GameObject.cpp
        Hourglass.cpp
        Hud.cpp
        Key.cpp
        MainMenu.cpp
        Model.cpp
        Pickup.cpp
        Pill.cpp
        Player.cpp
        RoomManager.cpp
        ScreenQuadSceneNode.cpp
        ShaderCallback.cpp
        SharedData.cpp
        SkyBox.cpp
        Solid.cpp
        SoundManager.cpp
        Spikes.cpp
        Teleporter.cpp
        Utility.cpp
        android_tools.cpp
        main.cpp )

# Specifies a path to native header files.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
        ${ANDROID_NDK}/sources/android/native_app_glue/
        D:/Development/C++/irrlicht-ogl-es/include/
        D:/Development/C++/nlohmann-json/
        D:/Development/C++/SFML-2.5.1/include
        D:/Development/C++/zipper-1.0.0/include
)
#>>>    NOTE: The above should be YOUR Irrlicht include directory. This is mine.
#>>>          The path starts in the "app" directory of your project.

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( native-lib
        app-glue
        android
        EGL
        log
        GLESv1_CM
        GLESv2
        z
        m
        ${log-lib}
        D:/Development/C++/irrlicht-ogl-es/source/Irrlicht/Android/obj/local/${ANDROID_ABI}/libIrrlicht.a
        D:/AndroidSDK/ndk-bundle/sources/third_party/sfml/lib/${ANDROID_ABI}/libsfml-system.so
        D:/AndroidSDK/ndk-bundle/sources/third_party/sfml/lib/${ANDROID_ABI}/libsfml-audio.so
        D:/AndroidSDK/ndk-bundle/sources/third_party/zipper/lib/${ANDROID_ABI}/libZipper.a)
#>>>    NOTE: Just like above, this should be your path. This one is mine.
#>>>          Links to the appropriate ABI. CAREFUL: You must have >ALL< Irrlicht ABIs built ("arm64-v8a", "armeabi", "armeabi-v7a", "mips64", "x86", "x86_64") otherwise you may run into installation errors on your virtual device.