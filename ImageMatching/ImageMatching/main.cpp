#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
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
				if (img.empty())
				{
					cout << "Cannot load image!" << endl;
				}
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

//To be defined later--supposed to handle matching of all high quality images
void bulkSIFTMatching(){

}

//a bit more complicated, but technically faster than performing a sort and picking top 5
void pickTopFive(vector<vector< DMatch >>* matchVec, int matchIndices[5]){
	int amounts[5];

	for (int i = 1; i < matchVec->size(); i++){
		int matchCounts = (*matchVec)[i].size();
			
		if (matchCounts > amounts[0]){
			matchIndices[0] = i;
			amounts[0] = matchCounts;
		}
		else if (matchCounts > amounts[1]){
			matchIndices[1] = i;
			amounts[1] = matchCounts;
		}
		else if (matchCounts > amounts[2]){
			matchIndices[2] = i;
			amounts[2] = matchCounts;
		}
		else if (matchCounts > amounts[3]){
			matchIndices[3] = i;
			amounts[3] = matchCounts;
		}
		else if (matchCounts > amounts[4]){
			matchIndices[4] = i;
			amounts[4] = matchCounts;
		}
		
	}
}

void findTopFiveMatches(Mat hqDesc, vector<Mat>* keyframeDesc, int matchIndices[5]){
	FlannBasedMatcher matcher;
	int index = 0;

	vector<vector< DMatch >>* matchVec = new vector<vector< DMatch >>();
	//Calculate matches between high quality image and 
	for (vector<Mat>::iterator it = keyframeDesc->begin(); it != keyframeDesc->end(); ++it){
		vector< DMatch > matches;

		Mat kfDesc = *it;
		matcher.match(kfDesc, hqDesc, matches);
		matchVec->push_back(matches);
		index++;
	}
	//pickTopFive
	pickTopFive(matchVec, matchIndices);
	index = 0;
}

int main(){
	vector<Mat>* keyframes = new vector<Mat>();
	vector<Mat>* hqPictures = new vector<Mat>();

	//load keyframes
	loadPhotos(keyframes, "C:\\Pictures\\keyframes\\");
	//load hqpictures
	loadPhotos(hqPictures, "C:\\Pictures\\hqpictures\\");

	//Perform SIFT on all images
	double threshold      = 0.05;
	double edgeThreshold = 5.0;
	double magnification  = 3.0;

	// SIFT feature detector and feature extractor
	SiftFeatureDetector siftDetector(threshold, edgeThreshold);
	SiftDescriptorExtractor siftExtractor(magnification);

	// Feature detection
	cout << "Detecting features for keyframes" << endl;
	vector<vector<KeyPoint>>* keyframeFeatures = new vector<vector<KeyPoint>>();
	vector<vector<KeyPoint>>* hqPicFeatures = new vector<vector<KeyPoint>>();
	for (vector<Mat>::iterator it = keyframes->begin(); it != keyframes->end(); ++it){
		vector<KeyPoint> featPoints;
		siftDetector.detect(*it, featPoints);
		keyframeFeatures->push_back(featPoints);
	}

	cout << "Detecting features for high quality images" << endl;
	for (vector<Mat>::iterator it = hqPictures->begin(); it != hqPictures->end(); ++it){
		vector<KeyPoint> featPoints;
		siftDetector.detect(*it, featPoints);
		hqPicFeatures->push_back(featPoints);
	}
	
	// Compute feature descriptors.
	cout << "Computing features for keyframes" << endl;

	vector<Mat>* keyframeDescriptors = new vector<Mat>();
	vector<Mat>* hqPicDescriptors = new vector<Mat>();
	int index = 0;
	for (vector<Mat>::iterator it = keyframes->begin(); it != keyframes->end(); ++it){
		Mat descript;
		vector<KeyPoint> feat = (*keyframeFeatures)[index];
		siftDetector.compute(*it, feat, descript);
		keyframeDescriptors->push_back(descript);
		index++;
	}

	index = 0;
	cout << "Computing features for high quality images" << endl;
	for (vector<Mat>::iterator it = hqPictures->begin(); it != hqPictures->end(); ++it){
		Mat descript;
		vector<KeyPoint> feat = (*hqPicFeatures)[index];
		siftDetector.compute(*it, feat, descript);
		hqPicDescriptors->push_back(descript);
		index++;
	}

	/*Different matchers
		1. Flann
		2. Brute Force
	*/
	
	int matchIndices[5] = { 0, 0, 0, 0, 0 };
	//findTopFiveMatches(hqPictures->front(), keyframeDescriptors, matchIndices);

	
	//Example Code: http://fahmifahim.com/2012/12/11/opencv-sift-implementation-in-opencv-2-4/
	/*FlannBasedMatcher matcher;
	vector<vector< DMatch >>* matchVec = new vector<vector< DMatch >>();
	index = 0;
	for (vector<Mat>::iterator it = keyframeDescriptors->begin(); it != keyframeDescriptors->end(); ++it){
		vector< DMatch > matches;
		matcher.match(*it, (*hqPicDescriptors)[index], matches);
		matchVec->push_back(matches);
		index++;
	}

	Mat img_matches;
	drawMatches(keyframes->front(), keyframeDescriptors->front(), hqPictures->front(), hqPicDescriptors->front(), matchVec->front(), img_matches);
	imshow("Matches", img_matches);

	waitKey(0);
	*/
	//-- Show detected matches
	imshow("Matches", img_matches);


	//Determine good matches

	/*
	double max_dist = 0; double min_dist = 100;
	Mat descriptors = keyframeDescriptors->front();
	vector<Point2d*>* minMaxDistances = new vector<Point2d*>();

	index = 0;
	//-- Quick calculation of max and min distances between keypoints
	for (vector<vector< DMatch >>::iterator it = matchVec->begin(); it != matchVec->end(); ++it)
	{
		Mat descriptors = (*keyframeDescriptors)[index];	//probably not needed
		vector< DMatch > matches = matchVec->front();

		for (int i = 0; i < matches.size(); i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		Point2d* minMaxDist = new Point2d(min_dist, max_dist);
		minMaxDistances->push_back(minMaxDist);

		printf("-- Max dist : %f \n", max_dist);
		printf("-- Min dist : %f \n", min_dist);
	}
	
	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	vector<vector< DMatch >>* betterMatches = new vector<vector< DMatch >>();
	for (vector<vector< DMatch >>::iterator it = matchVec->begin(); it != matchVec->end(); ++it)
	{
		vector< DMatch > good_matches;
		vector< DMatch > matches = *it;

		for (int i = 0; i < matchVec->size(); i++)
		{
			if (matches[i].distance <= max(2 * min_dist, 0.02))
			{
				good_matches.push_back(matches[i]);
			}
		}
		betterMatches->push_back(good_matches);
	}*/

	//-- Draw only "good" matches
	/*Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	imshow("Good Matches", img_matches);

	for (int i = 0; i < (int)good_matches.size(); i++)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}

	waitKey(0);
	*/


	// Store description to "descriptors.des".
	/*FileStorage fs;
	fs.open("descriptors.des", FileStorage::WRITE);
	cout << "Opened file to store the features." << endl;
	fs << "descriptors" << descriptors;
	cout << "Finished writing file." << endl;
	fs.release();
	cout << "Released file." << endl;
	*/
	return 0;
}