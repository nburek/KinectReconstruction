GLM and GLFW needed to run code 

If code does not output a model, check a printout that occurs at the beginning of the program run.  This print out provides an analysis of what version of GLSL your graphics card is running.  My machine is running 3.3 so if it doesn't yours doesn't match up be sure to change the version of GLSL in the shader programs.

The lines for loading the OBJ and camera matrix are occur in scenediffuse.cpp, within the initScene method.