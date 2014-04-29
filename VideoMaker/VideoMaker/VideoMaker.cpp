#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/highgui/highgui.hpp>  // Video write
#include <dirent.h>

using namespace std;
using namespace cv;

void loadPhotos(vector<Mat>* pictures, vector<string>* fileNames, const char* pictureFolder)
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
				fileNames->push_back(ent->d_name);
				printf("%s\n", ent->d_name);
			}
		}
		closedir(dir);
		cout << endl;
	}
	else {
		/* could not open directory */
		perror("Could not open directory");
		//return EXIT_FAILURE;
	}
}

int main(int argc, char *argv[])
{
	vector<Mat>* photos = new vector<Mat>();
	vector<string>* fileNames = new vector<string>();

	string fileRoot = "C:\\Users\\Nick\\Pictures";
	string pictureFolder = fileRoot + "\\FusionPictures\\";
	string outputFile = "video.avi";

	//load pictures from folder
	loadPhotos(photos, fileNames, pictureFolder.c_str());

	Size S(640, 480);
	VideoWriter outputVideo;
	outputVideo.open(outputFile, CV_FOURCC('M', 'P', '4', 'V'), 20.0, S, true);

	if (!outputVideo.isOpened())
	{
		cout << "Could not open the output video for write: ";
		return -1;
	}

	for (int i = 0; i < photos->size(); ++i)
	{
		Mat flippedPhoto;
		flip((*photos)[i], flippedPhoto, 1);
		outputVideo << flippedPhoto;
	}
}