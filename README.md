## Android port for this game
- zipper for armeabi-v7a is NOT present here, because of this: https://github.com/android/ndk/issues/1391
- code does NOT work because OGL-ES1 does NOT support programmable pipeline through shaders, and porting to OGL-ES2 requires the application to be rewritten.