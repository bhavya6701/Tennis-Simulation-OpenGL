# COMP371: Computer Graphics - Quiz 2

#### Name: Bhavya Ruparelia

#### Student ID: 40164863

## Build Instructions

### Command Line

1. Download and Install CMake
2. Open a terminal
3. Run cmake:

```
cd <source_folder>
cmake -S . -B <build_folder>
cmake --build <build_folder> --target install
```

To generate an Xcode project from the command line use:

```
cd <source_folder>
cmake -S . -B <build_folder> -G Xcode
cmake --build <build_folder> --target install
```

**NOTE:** In the case of cmake the `<source_folder>` is the topmost folder with
a CMakeLists.txt file.

**NOTE:** The `<build_folder>` can be anything but is usually just named `build`.

### CMake GUI

You can use the CMake GUI to configure and generate the Visual Studio Project.

1. Download and Install CMake
2. Open the CMake GUI
3. Set the `Where is the source code:` field to the source folder
4. Set the `Where to build the binaries:` field to different location (e.g.: source_folder/build)
5. Click Configure
6. Select the project generator (this will be the project file that compiles program)
7. Click Generate

#### Windows

Once the project is created you can build the **ALL_BUILD** project followed by
building the **INSTALL** project.

#### Xcode

Once the project is created you can build the **ALL_BUILD** target followed by
the **INSTALL** target.

#### Unix Make File

In the terminal:

```
cd <build_folder>
make install
```

## Running

Once built and installed, all the relevant files will be in the **dist** folder
at the top level of the source folder.

### Linux/macOS

In the terminal type:

```
./quiz2
```

### Windows

Click on the **quiz2.exe** from the File Explorer.

## Implementation Specifications

1. **Cubical Poles for Tennis Net**
   Three cubical poles holds the tennis net in place.
   Two poles are be placed at the two ends of the tennis net, and one pole is be placed in the middle.
   Textures are applied to each pole and the top row of the tennis net with repeat texture parameter for better
   visual appearance.

2. **Alphabets on Rackets**
   Four alphabets from Quiz 1 are brought together and placed on top of two rackets.
   The front face of each alphabet is made transparent using suitable alpha values to create a visually appealing
   effect.

3. **Centering the Models**
   The two racket models and corresponding alphabets are centered on either side of the tennis net.
   The rackets and alphabets are modeled as one entity to transform them together, creating a unified appearance.

4. **Textures**
   Distinct textures are applied to each alphabet to make them visually distinctive.
   The racket structure have a metallic texture, and the arm have a tattooed texture, enhancing the realism of
   the scene.

5. **Texture for Sky Cube**
   A suitable texture is applied to the inside of the sky cube, improving the visual experience when looking inside
   the cube.

6. **Lighting with Spotlight and Phong Illumination**
   A spotlight is added as the main light source, using the Phong illumination model for realistic lighting
   effects.
   The spotlight is placed 30 units out on the z-axis, facing the model.
   The 'L' key allows users to toggle the spotlight on and off.
   Ambient light is enabled when the main light source is off.

7. **Multiple Cameras**
   One camera is added onto each racket model's front to provide an unobscured view.
   The 'M' key allows user to loop through the available cameras, including the default main camera and the two
   camera views with the racket models.
   The 'R' key from Quiz 1 reset back to the main camera among other functions.

8. **Circling Camera with Spotlight**
   Another camera is added to circle around the center of the scene using arrow keys.
   A spotlight is attached to this camera, moving with the camera and toggleable using a key.
   The circling camera's Y and look-at point remains constant for a smooth circling effect.

9. **Rendering Shadows**
   Shadows are rendered using two-pass shadow algorithms.
   Users can toggle the spotlight from the circling camera for shadow computation using a key.

10. **OpenGL Version and Comments**
    The application uses OpenGL 4.1 and onwards for compatibility and modern features.

11. **Note:**
    - 'Shift + 1' and 'Shift + 2' keys toggle the racket models.
    - The 'L' key toggles the spotlight on and off.
    - The 'M' key loops through the available cameras.
    - The '0' key changes camera to circling camera.
    - The 'P' key toggles the shadow.
    - The 'R' key resets the camera to the main camera.
