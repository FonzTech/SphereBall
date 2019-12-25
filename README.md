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

* The application uses the Multiple Render Target feature. RTTs are stored in the **SharedData** class, which is used to share specific functionalities among the entire system. 3D geometry is rendered on a collection of render targets (the `sceneRtts` array), while all the GUI, *created by all of the* `GameObject` *subclasses*, are rendered into a separate render target (the `guiRtt` texture). I wanted to avoid this, but since Irrlicht 1.8.4 ignore any `E_MATERIAL_TYPE` attached to the 2D renderer while on the OpenGL back-end, the engine rendered all of the GUI environment to all of the currently bound render targets, since there is no other way to specify the layer for the fragment. So, managing the binding of render targets are up to game objects implementations. For instance, the `MainMenu` draws its GUI to the above mentioned `guiRtt` render target. This situation must be considered together with the following, and second point, of this list.

* All of the GUI, which is not affected by any custom shader, are created by the `SharedData` class. GUI created by `GameObject` subclasses, like `MainMenu`, MUST `drawAll`, then `clear`, the GUI environment. After this, they should reset the currently bound render target to the previous one, which should be the `sceneRtts`. According to the documentation, doing `setRenderTarget(0)` should automatically reset to the correct previous one. But, in the case of an array of `IRenderTarget` being set previously, this will not work, as of Irrlicht 1.8.4.

* I tried to keep a loose coupling where possible, while focusing on performance (for instance, access members directly, instead of doing some kind of Java-ish thing). But you could see some kind of "circular dependency" in implementations (`cpp` files). For example, the `RoomManager` class needs the `GameObject` class for its creational pattern (start a level with player, collectibles, exit, enemies, etc...). But you could certainly see actual game objects (subclasses of `GmaeObject`) which access the collection of currently active game objects, stored in the `RoomManager` class.

* Zip functionality is not really needed at all. It could be scrapped away by investigating `getMesh` method in the `Utility` class implementation and append a `&& false` to the extension check. I just wanted to save space on this Git repo.
* Textures where scene and GUI are rendered are destroyed and re-created every frame, since window size (or internal resolution) can change in any moment. See the first routine in the main loop in the `Engine` class implementation.
* Post-processing effects are computed into the `scene.fs` fragment shader. Since there are multiple effects which can occur during the scene, a "mask-technique" is used to decide what kind of effect must be applied. For instance, the wave heat effect is applied when the post-processing mask (which is a render target, after all) contains some red pixels. More specifically, when the red component satisfies the following conditional: `x >= y - 0.01 && x < y + 0.01`, where `x` is the color component value and `y` is the target for the effect to apply (in the case of heat waves, the red component must be equal to `0.02`). All component values are restrict into a `]0,1[` range, because `0` is a value which can be potentially be catched as an effect, since it's the starting value. and `1` can't be reached in the `inRange` function (this function uses the `step` function to avoid branching; more specifically, the above mentioned conditional where the inclusion in the right side is not considered).
* In relation to the above, "Red" effects are computed first, then the "Green" ones. The "Blue" component does only serve as a coefficient passthrough for the relative effect to apply. "Alpha" component should be always `1`, otherwise clunky effects will be introduced. Here's a list of post-processing effects mapped to color components with their trigger value:
  - red: `0.02` = heat wave;
  - red: `0.04` = glass;
  - green: `0.02` = simple overlap (no special effect; "red" effects are applied behind the object masked with this value);