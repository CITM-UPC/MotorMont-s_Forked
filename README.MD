# MOTOR MONT-S GAME ENGINE

Version 1.0 of the Motor Montés. Developed with love and care. Forked from the Freakiest Engine ever

### The developers

- [Sergi Gantzer](https://github.com/sgantzer12).
- [Ivo Montés](https://github.com/Ivomm9).
- [Biel Liñán](https://github.com/Drauguer).

How to use the engine

- Right click - Rotate the camera 
- Right click + WASD - Fps camera movement
- Right click + Q - Up the camera
- Right click + E - Down the camera
- Right click + Shift + AWSD - fast fps camera movement
- Right click + Alt - Orbit selected object
- F - Focus on the selected GameObject
- Left click to select a GameObject

How to use the UI

- Select Mesh to open the Mesh Spawner and Create Triangles, Squares and Cubes
- Select File to quit
- Select Scene to save and load scenes in a custom .scene format
- Select Help to visit our GitHub page
- Select View to alter between console and assets Windows
- Select Play to play the scene, Stop to stop the scene (stop button will appear once you hit play)
- When left clicking a GameObject, it is selected, use the inspector on the right
- Drag a GameObject on top of another one in the Hierarchy parent

How to use the Assets Window

- Left click to select an asset
- Right click with an asset selected to delete or import said asset
- If importing a model, it will spawn textureless in 0, 0, 0.
- If importing a texture, you'll have to select which GameObject you want to apply the Texture to.
- Note that Assets folder is not being modified. When executing the code, it will clone all the assets inside the Library folder and use those. 
- When an asset is deleted from the Assets Window, the one from the Library folder is deleted as well.

NOTES

- For texture import, outside of the one in the Assets Window, you'll have to select a GameObject, then drag the texture into the scene to apply it.
- Parenting Works but creates a "clone" (it displays twice in the Hierarchy), outside of the visual artifact, it works.
- Street Environment is not loaded
- When Loading a scene, camera values are saved correctly (check the .scene file with notebook), but the frustum planes don't follow when moving the camera GameObject (the GameObject is moving, check by pressing F to focus once moved).
- When Saving/Loading a scene, the texture values are not saved, you need to reimport the textures manually.

EXTRAS 

- Asset import to the scene using right click on the Assets Window
- Super nice UI for visual clarity and confirmations