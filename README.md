# 🪙 Coin Collector

**Coin Collector** is a 2D game built with **C** and **Raylib**, where your goal is to collect coins, reach the target score, and complete each level before time runs out!

## 🎮 About the Game

Coin Collector is a lightweight 2D game project developed in C using the Raylib graphics library. It features a simple game flow with menus, level selection, gameplay, and win or game-over screens.

## ✨ Features

- 🪙 Collect coins and increase your score
- ⏱️ Race against the timer
- 🎯 Reach the target score to win
- 🗺️ Level selection and file-based levels
- 🏠 Main menu
- 🏆 Win and Game Over screens
- ⚡ Lightweight native C application
- 🎨 Powered by Raylib

## 🛠️ Built With

- **C** — Game logic and programming
- **Raylib** — Graphics, input, and game framework
- **GCC** — C compiler

## 📋 Requirements

- GCC or another compatible C compiler
- Raylib installed and configured
- A supported desktop environment

## 🚀 Build and Run

### 1. Clone the repository

```bash
git clone https://github.com/PyPh4ntom/coin-collector.mobile.git
cd coin-collector.mobile
```

### 2. Compile the game

On Linux, if your source file is named `main.c`:

```bash
gcc main.c -o coin-collector -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

> **Note:** The compile command may need adjustments depending on your source filenames, Raylib installation, and graphics backend.

### 3. Run the game

```bash
./coin-collector
```

## 🕹️ Gameplay

1. Start the game from the main menu.
2. Select a level.
3. Collect coins to increase your score.
4. Reach the target score before the timer expires.
5. Complete the level and aim for a new high score!

*Controls and gameplay details may vary depending on the current version.*

## 📂 Project Structure

The project may contain files organized like this:

```text
coin-collector/
├── src/          # Source code
├── levels/       # Level data files
├── assets/       # Game assets
├── README.md
└── .gitignore
```

*The structure above is illustrative; it may differ from the actual repository.*

## 🗺️ Roadmap

- [ ] Add more levels
- [ ] Improve graphics and animations
- [ ] Add sound effects and background music
- [ ] Add a high-score system
- [ ] Improve gameplay and performance

## 👨‍💻 Developer

**PyPhantom**

- GitHub: [@PyPh4ntom](https://github.com/PyPh4ntom)

## 📜 License

No license has been specified yet. All rights are reserved by default unless a license is added to the repository.

---

⭐ If you like the project, consider giving the repository a star!

**Made with C, Raylib, and ❤️ by PyPhantom.**
