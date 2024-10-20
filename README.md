# X16 Crazy Boulders C edition
**Tested on real hardware and R47 emulator.**

![CrazyBoulders screenshot](/image/CrazyBoulders500x390.png)



This is a Boulder Dash style game for the Commander X16.

Use the arrow keys or joystick to control the player.

Find the door to the next level within the time allotted, avoiding getting squashed by the boulders, while collecting enough diamonds to open the door.

-----------------------------------------------------------------------------
Update 22nd Febuary 2024

Added option to continue or exit to basic after each game.

Added code to enable use of joystick 1 or joystick 2 in addition to keyboard.

-----------------------------------------------------------------------------

The original version of Crazy Boulders by Dustan Strakl was programmed in Basic see https://www.8bitcoding.com/p/crazy-boulders.html.

8BitCoding has lots of useful information on programming the X16.

This C version is an exercise in writing a game in C using the cc65 compiler.

Using Dustan's game design made it easer to concentrate on the coding while learning how game mechanics work.

The excellent libX16.lib by Chris Love provides functions vload and load_file to give the C equivalent of the basic commands VLOAD and LOAD.
