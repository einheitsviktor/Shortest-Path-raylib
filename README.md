# Shortest-Path-raylib

This is an improvement of my [Shortest-path GUI](https://github.com/einheitsviktor/Shortest-Path) implemented in Qt.
This time I use [raylib](https://www.raylib.com/) ("... a simple and easy-to-use library to enjoy videogames programming.") to make the interactions more convenient.

Regarding the logic I followed this [guide](https://www.redblobgames.com/pathfinding/a-star/introduction.html) very closely.

![Screenshot](Screenshot.png)

## Dependencies for Running Locally
* cmake >= 3.11
* make >= 4.1 (Linux)
  * Linux: make is installed by default on most Linux distros
* gcc/g++ >= 8.0
  * Linux: gcc / g++ is installed by default on most Linux distros

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run: `./Shortest-Path-raylib`

## Usage
- Use the left mouse button to move the start (green) or goal tile (red), or draw obstacles.
- Use the right mouse button to remove obstacles.
- Choose one of the traverse algorithms:
    - Breadth First Search
    - Dijkstra Search
    - A* Search
- Hit the Search button to execute the algorithm
- Toggle the Vector field button to show every predecessor of all visited tiles
