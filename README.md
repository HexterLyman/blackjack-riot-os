# Blackjack 21 - RIOT OS Edition
Implementation of a simplified version of 21 written in C and programmed into a xiao-bella microcontroller.

## Overview
This project aims to showcase a simplified version of Blackjack 21 within the RIOT Operating System. With limited user input options, it has been decided that a cursor-like system was best used since we can take advantage of the microcontroller's in-built motion sensor.

## Setup and Configuration
### 1. Install and setup RIOT OS on your device
**https://guide.riot-os.org/getting-started/installing/**

Warning: Ensure that you can flash the microcontroller first before proceeding!
### 2. Clone this Repository
```
https://github.com/HexterLyman/blackjack-riot-os.git
```
### 3. Ensure that the files are within the same environment as RIOT OS 
Note: It is recommended to work in Linux / Unix systems or in WSL if in Windows  

### 4. Flash the code into the xiao-bella microcontroller



## Application Structure
<img width="2642" height="1480" alt="image" src="https://github.com/user-attachments/assets/d3a636ff-7e93-48f4-8f86-b1c0820f4892" />

1. The **Start Menu** is displayed upon powering the xiao-bella microcontroller.
2. Once the user presses the input button, the **Cutscene will play**, before showing the **Betting screen**.
3. The user is then prompted to increase, decrease the bet for the round before confirming the bet. The user is then redirected to the **Gameplay Screen**.
4. The cards are dealt to both the dealer and the player (user). The user can choose to either hit (left) or stand (right).
5. If the user chooses to stand, the dealer's turn begins and the outcome of the round is then determined. The user is then redirected to the **Gameover Screen**.
6. The user is then prompted to either play again, which will redirect to the **Betting screen**, or not, which will redirect to the **Highscore Screen**.
7. Upon viewing the highscore, the user can press the input button again and go back to the **Start Menu**. 


## Miscellaneous
- The *display.h* file is **NOT** inside the *Core* directory folder since it requires access to the *bin* folder which contains the necessary libraries i.e. *mineplex.h* in this case
- **DO NOT** remove the flasher code in main.c! It would still compile, but the code below it will not run.




