Pipeline 

1. 3D Reconstruction: KinectFusionExplorer - D2D
2. Kinect video subsampling: VideoMaker
3. Matching: ImageMatching


4. Camera position verifying: OBJ Loader
**********************************************************
Requirements:
GLM: http://glm.g-truc.net/0.9.5/index.html
GLFW: http://www.glfw.org/download.html
GLSL 3.3
**********************************************************

To load an object, please modify line 53 in scenediffuse.cpp
To load a file contain camera matrices, please modify line 41 in scenediffuse.cpp
To properly set up libraries:

Once the libraries and file directories are properly set up the program should run without issue. If the program crashes for reasons other than improper library set up, please refer to the Troubleshooting notes below.
When run, the program will output information regarding the current version of GLSL running on your graphics card.
It will then prompt you to choose whether or not you wish to load a file of camera matrices.
Any answer other than 'y' will result in simply viewing the object that you choose to load.
If you respond with 'y' the program will load all of the camera matrices from the designated file and output the total number it's currently holding (for the user's reference).
After the mesh has successfully loaded, the user will then be prompted to enter the index of a camera matrice.

Troubleshooting:
If program crashes, put a break point at line 88 in the main function, which calls dumpGLInfo(). This line outputs information regarding the current version of GLSL running on your system. If the GLSL version running on your system is earlier than 3.3 then the program will fail. To rectify this, the versions within the shader programs (diffuse.vert and diffuse.frag) need to be modified accordingly. Note that any changes to earlier versions of GLSL may cause the shader programs to become unusable.

