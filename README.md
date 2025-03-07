# Multiplayer Minesweeper
A competitive multiplayer implementation of the classic Minesweeper game.

## Features
- **Classic Gameplay:** Minesweeper mechanics with an intuitive GUI.
- **Multiplayer:** Go head-to-head against a friend.

## Getting Started

### Prerequisites
Ensure the following dependencies are installed on your system:

- Your choice of C++ compiler (the build instructions assume that choice was GCC)
- SDL2: For window creation, input handling, and rendering.
  - SDL_ttf for font file support and text rendering
  - SDL_image for image loading
- Boost.Asio: For server and client networking.

### Build Instructions
1. Clone the repository:
`git clone https://github.com/RoghanBehm/minesweeper.git`

2. Install required libraries
- g++: [Installation guide](https://gcc.gnu.org/install/)
- SDL2: [Installation guide](https://wiki.libsdl.org/SDL2/Installation)
  - SDL_ttf: [Github](https://github.com/libsdl-org/SDL_image)
  - SDL_image: [Github](https://github.com/libsdl-org/SDL_ttf)
- Boost.Asio: [Boost installation](https://www.boost.org/users/download/)

3. Build the server:
`g++ -o server server/server.cpp include/serialize.cpp -I include`

4. Build the client:
`cd {your_path}/minesweeper`
`make`

## How to Play
To play on a local network server instance:
1. Start the server:
`./server`
2. Connect as a client:
`bin/minesweeper`
3. Start sweeping for mines. Players can reveal tiles with left click and flag tiles with right click.

At this stage, P2P over internet hosting requires the address value in main.cpp to be changed to the host server address, and port 8000 to be forwarded.



