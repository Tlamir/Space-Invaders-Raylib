﻿# Space-Invaders-Raylib

This is a simple Space Invaders clone game developed using C++ and the Raylib library. The game features player movement, enemy attacks, and a scoring system. Players can shoot bullets to destroy enemies and bunkers while avoiding enemy bullets.

![Alt Text](https://i.giphy.com/media/v1.Y2lkPTc5MGI3NjExbXV6ZGF4cGw3ZHp1NmVtdWViMXJrdms4ZG1icHV5YWdqYWZqcjkyOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/LmzjR5n69ySN9eW8t2/giphy.gif)

## Features

- Player movement using arrow keys
- Shooting bullets with the spacebar
- Enemies that attack the player
- Bunkers that can be destroyed
- Game over and victory screens
- Level progression

## Requirements

- C++ compiler (e.g., g++)
- Raylib library

## Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Tlamir/Space-Invaders-Raylib.git
   cd Space-Invaders-Raylib
   ```

2. **Install Raylib:**
   Follow the installation instructions for Raylib based on your operating system. You can find the instructions [here](https://github.com/raysan5/raylib#installation).

3. **Compile the project:**
   Use the following command to compile the project:
   ```bash
   g++ main.cpp -o Space-Invaders-Raylib -lraylib -lm -lpthread -ldl -lrt -lX11
   ```

4. **Run the game:**
   After compiling, run the game with:
   ```bash
   ./Space-Invaders-Raylib
   ```

## Controls

- **Left Arrow**: Move left
- **Right Arrow**: Move right
- **Spacebar**: Shoot bullet
- **Enter**: Start game / Restart game / Proceed to next level

## Gameplay Tip
- Don't chase enemies. Make them come to you
