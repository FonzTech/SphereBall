## About the project
SphereBall is a simple 2.5D platform game, which was born as a simple project to learn new things and experiment with 3D graphics. Then it evolved to something more serious, so I throught it would be nice to have it on my portfolio.

### Concept
This game is straightforward: you are a ball which have to reach the exit spot to unlock new levels. Avoid enemies and hazards, collect coins and treasures, score all the points you can.

### State of the art
This project is developed in my free time. My goal is to successfully experiment new things about 3D graphics. No precise roadmap exists. I just keep adding new features, from the gameplay side to the technical one, refactoring and improving existing code and, eventually, reach a fully playable state for the game. Then I will add the level editor.

## Technical Notes

### Dependencies
- [Irrlicht Engine 1.8.4](http://irrlicht.sourceforge.net/)
- [SFML](https://www.sfml-dev.org/) [Only audio is required, with OpenAL library]
- [JSON library from nlohmann](https://github.com/nlohmann/json)
- [Zipper library from sebastiandev](https://github.com/sebastiandev/zipper) [with minizip and zlib]

### How do I play this game?
I do not provide binaries at the moment. Currently, this repository has the Visual Studio 2017 solution in it, which is the IDE I currently use. I will migrate this project to CMake in the future, after the game is finished.

### Code structure
Core classes are found in the "Core" folder of the solution. There are some important notes on how they are arranged.

* The application uses the Multiple Render Target feature. RTTs are stored in the **SharedData** class, which is used to share specific functionalities among the entire system. 3D geometry is rendered on a collection of render targets (the `sceneRtts` array), while all the GUI, * *created by all of the* * `GameObject` * * subclasses* *, are rendered into a separate render target (the `guiRtt` texture). I wanted to avoid this, but since Irrlicht 1.8.4 ignore any `E_MATERIAL_TYPE` attached to the 2D renderer while on the OpenGL back-end, the engine rendered all of the GUI environment to all of the currently bound render targets, since there is no other way to specify the layer for the fragment. So, managing the binding of render targets are up to game objects implementations. For instance, the `MainMenu` draws its GUI to the above mentioned `guiRtt` render target. This situation must be considered together with the following, and second point, of this list.

* All of the GUI, which is not affected by any custom shader, are created by the `SharedData` class. GUI created by `GameObject` subclasses, like `MainMenu`, MUST `drawAll`, then `clear`, the GUI environment. After this, they should reset the currently bound render target to the previous one, which should be the `sceneRtts`. According to the documentation, doing `setRenderTarget(0)` should automatically reset to the correct previous one. But, in the case of an array of `IRenderTarget` being set previously, this will not work, as of Irrlicht 1.8.4.

* I tried to keep a loose coupling where possible, while focusing on performance (for instance, access members directly, instead of doing some kind of Java-ish thing). But you could see some kind of "circular dependency" in implementations (`cpp` files). For example, the `RoomManager` class needs the `GameObject` class for its creational pattern (start a level with player, collectibles, exit, enemies, etc...). But you could certainly see actual game objects (subclasses of `GmaeObject`) which access the collection of currently active game objects, stored in the `RoomManager` class.

* Zip functionality is not really needed at all. It could be scrapped away by investigating `getMesh` method in the `Utility` class implementation and append a `&& false` to the extension check. I just wanted to save space on this Git repo.
* Textures where scene and GUI are rendered are destroyed and re-created every frame, since window size (or internal resolution) can change in any moment. See the first routine in the main loop in the `Engine` class implementation.