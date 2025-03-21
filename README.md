# **FileSystemVizualizer_Linux**



## **Description**

Filesystem Visualizer is a C program similar to Midnight Commander (mc), using the ncurses library. It offers navigating the Linux file system in an interactive terminal interface.

## **Features**

- View directory contents

- Navigate through files and directories

- Copy files

- Move files

- Delete files

## **Tehnologies Used** 

- *C* for logical implementation

- *ncurses* for terminal-based graphical interface

- *sys/stat.h, dirent.h, unistd.h* for file system manipulation

---

## Installation and Execution


### Requirements

- A Linux system

- A GCC compiler

- The ncurses library installed


### Clone the Repositoyry

```git clone https://github.com/username/FileSystemVizualizer_Linux.git```

```cd FileSystemVizualizer_Linux```

### Compilation

```gcc -o filesystem_visualizer main.c -lncurses```

### Execution

```./filesystem_visualizer```

### Usage

- Arrow keys up/down - Navigate through files

- Enter - Access a directory

- c - Copy a file

- m - Move a file

- d - Delete a file

- q - Exit


--- 

## Code Structure

- afiseaza_mesaj_unic - Displays a message on the screen

- confirmare_stergere - Confirms file deletion

- confirmare_mutare - Confirms file movement

- copie_fisier - Copies a file

- init_interface_ncurses and cleanup_interface_ncurses - Initializes and cleans up the ncurses interface

- interfata_antet - Displays the interface header

- continut_director - Displays the contents of a directory

- navigare_fisiere - Enables file navigation and manipulation

---

## Screenshots

![image](https://github.com/user-attachments/assets/73ff4853-f80d-4056-91d8-a9db47883ba2)


![image](https://github.com/user-attachments/assets/f78efbb0-53a4-45cf-8a44-af2bc08ea224)


![image](https://github.com/user-attachments/assets/8010cdec-ac46-469f-a5d6-d8bb117b7d15)


---


