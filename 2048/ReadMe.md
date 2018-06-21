# 2048

[Wikipedia Page for 2048](https://en.wikipedia.org/wiki/2048_(video_game))
>"2048 is a single-player sliding block puzzle game designed by Italian web developer Gabriele Cirulli. The game's objective is to slide numbered tiles on a grid to combine them to create a tile with the number 2048. However, one can continue to play the game after reaching the goal, creating tiles with larger numbers."
>-Wikipedia

This is my 'recreation' of 2048 in C++ using SDL2 for graphics

**Requirements:**
- gcc compiler
- SDL2 library
- SDL2_ttf library

Run the Makefile to build.

## 2048 with graphics

2048 with graphics similar to the original. It has a high score feature that automatically stores your best score after a game or when the game is restarted.

![g2048](https://github.com/RohithS98/SDL-Experiments/blob/master/2048/G2048.png)

Use W, A, S, D or arrow keys to move. Use r to reset the game. High score can be deleted by deleting the bestscore.bin file

## 2048 Text
This version does not require any external libraries and can be played in the terminal.

![text2048](https://github.com/RohithS98/SDL-Experiments/blob/master/2048/T2048.png)

Use ./2048text to run the game

Enter W, A, S or D and enter to move the blocks. There is no feature to save high score in this version.
