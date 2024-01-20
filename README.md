# Assigment2
This project is a Game Engine made for UPC videogames Master´s Degree

- [Game Engine Github Repository](https://github.com/Assigment2Company/Assigment2)

## Installation

Download GameEngine.zip file in [release](https://github.com/Assigment2Company/Assigment2/releases), unzip it and execute the Engine.exe file.

## Engine Guide

- Camera
  - Hold Right-Click and use “WASD” for a fps-like movement and free look around must be enabled.
  - Use “Q” and Use “E” for moving the camera Up and Down.
  - While Right clicking, move the mouse in any direction for free look.
  - Use the mouse Wheel for zooming in and out.
  - Left ALT + Left Click to Rotate de center of the scene.
  - Holding SHIFT duplicates de movement speed.
- Project Panel
  - Dragging a .gltf file inside the engine should import the file into project panel. (Click Refresh if the new asset is not appearing). 
  **All model files (.gltf, .bin, textures...) should always be in the same folder**.
  - Dragging an asset inside the scene should create a GameObject in the Hierarchy panel with an meshRenderer component.
- Hierarchy Panel  
  - Right click in hierarchy panel will open a popup where you can create, duplicate and delete a gameobject.
  - Dragging a GameObject into another GameObjet will set it as the child of the second object.
- Inspector Panel
  - Click Button "Add Component" in the inspector panel when a gameObject is selected to add a new component.
  - Every gameObject has a transform tha can be modified using the transform section in the inspector panel.
  - The name of a gameObject can be modified in the inspector panel.
- Lighting Panel
  - In this panel you are able to modify: ambient color, brightness and KD (Diffuse Attenuation Coefficient).
- Main Menu bar
  - File
    - Save Scene
    - Load Scene
    - Quit
  - View
    - QuadTree
      - CheckBox to enable or disable Frustum Culling.
      - CheckBox to show QuadTree in the Scene.
      - Shows QuadTree structure (can be updated using Refresh button).
    - Debug
      - Activate or Deactivate Grid and world Guizmo.
  - Window
    - Panels
      - From here every panel of the engine can be opened or closed.
    - Help
      - About
        - Show License and some information about the Engine.
