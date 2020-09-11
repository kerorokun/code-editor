# code-editor
<img src="https://github.com/kerorokun/code-editor/blob/master/images/screenshot.png" width="900" />
<img src="https://github.com/kerorokun/code-editor/blob/master/images/dracula_screenshot.png" width="900" />
A code editor I wrote in C/C++ and OpenGL back from March 2018 - July 2018. Unfortunately, I forgot to save the final completed project so this is a snapshot from the most recent saved repo I could find back in June 2, 2018.

There were a few inspirations for this project:
1. Make a minimal Emacs (as that was my main editor of choice back then) that used graphics acceleration
2. Learn more about data structures and C in a non-trivial project
3. Learn OpenGL
4. Experiment with a procedural coding style and the associated patterns (as I had only learned OOP in school)

## Features
Features in this specific build
* File loading and saving
* Undoing and redoing history
* Cursor operations (move left, right, up, down, next empty space, previous empty space, next word, previous word)
* Basic evaluation buffer similar to the one in Emacs
* Panelling
* Basic multithreading if desired
* Gap Buffer string representation (similar to the one used in the Emacs) to allow for efficient text modification
* Custom generic dynamic array
* Batched 2D Graphics Library backend using OpenGL

Features not available in this current build but were available in the final build
* Automatic code formatting
* Themes
* Slight more expanded evaluation buffer
* Expanded multithreading
* Word search using Trie structure
* LOTS of bug fixes
