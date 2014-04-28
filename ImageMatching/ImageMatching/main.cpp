#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <dirent.h>

using namespace cv;
using namespace std;

void loadPhotos(vector<Mat>* pictures, const char* pictureFolder)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(pictureFolder)) != NULL) {
		cout << "Loading images..." << endl;
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			//ignore directories above
			if (!(strcmp(ent->d_name, "..") == 0 || strcmp(ent->d_name, ".") == 0)){
				string filename = pictureFolder;
				filename.append(ent->d_name);
				Mat img = imread(filename.c_str());
				pictures->push_back(img);
				imshow("Display window", img);                   // Show our image inside it.
				printf("%s\n", ent->d_name);
			}
		}
		closedir(dir);
		cout << endl;
	}
	else {
		/* could not open directory */
		perror("Count not open directory");
		//return EXIT_FAILURE;
	}
}

int main(){
	vector<Mat>* keyframes = new vector<Mat>();
	vector<Mat>* hqPictures = new vector<Mat>();

	//read in keyframes
	
	//read in hqpictures


	Mat image;
	//loadPhotos(keyframes, "C:\\Users\\Ein.Ein-PC\\Documents\\Visual Studio 2013\\Projects\\PictureMatching\\PictureMatching\\keyframes\\");
	//image = *keyframes->begin();
	image = imread("C:\\Users\\Ein.Ein-PC\\Pictures\\3u0uau.jpg");

	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window", image);                   // Show our image inside it.

	waitKey(0);                                          // Wait for a keystroke in the window

	return 0;
}