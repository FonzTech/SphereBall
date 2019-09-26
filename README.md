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