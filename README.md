# Remote image to ASCII on embedded Linux system.

# Task
Using an embedded Linux system equipped with camera, display and internet connection show the camera feed as ASCII. The processing of image into text should happen on another device (also a Linux system).
Everything should be logged.

# Description
System consists of 5 modules:
 - clock,
 - camera,
 - img2txt,
 - console,
 - logger.

which communicate together to achieve the desired result.

## Communication graph
![FullSchema](https://user-images.githubusercontent.com/13040204/159347886-77a29dd2-8e2c-452f-b4a3-35a6af8a042d.png)

# Usage
Can be ran on a single Linux system.

Additional requirements:
 - executable of [`img2txt`](http://imagetoasciiart.sourceforge.net) in the main directory
 - fake camera file, `test.mp4` in the camera directory

(both can be found present in the commit history, but were removed due to copyright)

To run:
 - compile all 5 executables using provided `compile.sh` files
 - run them
