=============================================
=  FoofGame
=============================================

This folder contains the actual project files which can be loaded in the Eclipse IDE.

https://www.youtube.com/watch?v=88wKVMknyeQ


This is a game written in Java using the Eclipse IDE and the Android SDK. It is designed using OpenGL ES 1.0 for deployment to android. 

The game features a bunny protaganist who can navigate 2D levels by running, jumping, and shooting enemies. Collectible items offer extra points, health, or give the player new ablities. The goal is to save the bunnies trapped in the plastic hamster balls.


This game is not finished and is only in a stable proof of concept form. It will be revisited with OpenGL ES 2.0 and feature real 3D graphics.

The rendering engine behind the game pushes each layer of sprites as a single vertex batch to the graphics card for extremely efficient rendering. All memory management is handled by the application itself so dynamic memory by the Java runtime is kept to a minimum.