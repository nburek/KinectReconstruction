#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
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
void pickTopFive(vector<vector< DMatch >>* matchVec, vector<int>* matchIndices){
	vector<int>* amounts = new vector<int>();
	for (int i = 0; i < 5; i++){
		amounts->push_back(0);
	}
	

	for (int i = 0; i < matchVec->size(); i++){
		int matchCounts = (*matchVec)[i].size();
			
		if (matchCounts > (*amounts)[0]){
			matchIndices->insert(matchIndices->begin(), i);
			amounts->insert(amounts->begin(), matchCounts);
		}
		else if (matchCounts > (*amounts)[1]){
			matchIndices->insert(matchIndices->begin()+1, i);
			amounts->insert(amounts->begin()+1, matchCounts);
		}
		else if (matchCounts > (*amounts)[2]){
			matchIndices->insert(matchIndices->begin() + 2, i);
			amounts->insert(amounts->begin() + 2, matchCounts);
		}
		else if (matchCounts > (*amounts)[3]){
			matchIndices->insert(matchIndices->begin() + 3, i);
			amounts->insert(amounts->begin() + 3, matchCounts);
		}
		else if (matchCounts > (*amounts)[4]){
			matchIndices->insert(matchIndices->begin() + 4, i);
			amounts->insert(amounts->begin() + 4, matchCounts);
		}
		
	}
	matchIndices->resize(5);

}

void findTopFiveFLANNMatches(Mat hqDesc, vector<Mat>* keyframeDesc, vector<vector< DMatch >>* matchVec, vector<int>* matchIndices){
	FlannBasedMatcher matcher;
	int index = 0;

	//Calculate matches between high quality image and 
	for (vector<Mat>::iterator it = keyframeDesc->begin(); it != keyframeDesc->end(); ++it){
		vector< DMatch > matches;

		//calculate initial matches
		Mat kfDesc = *it;
		matcher.match(hqDesc, kfDesc, matches);

		//determine good matches
		double max_dist = 0; double min_dist = 100;

		//-- Quick calculation of max and min distances between keypoints
		for (int i = 0; i < hqDesc.rows; i++)
		{
			double dist = matches[i].distance;
			if (dist < min_dist) min_dist = dist;
			if (dist > max_dist) max_dist = dist;
		}

		std::vector< DMatch > good_matches;
		for (int i = 0; i < hqDesc.rows; i++)
		{
			if (matches[i].distance <= max(2 * min_dist, 0.02))
			{
				good_matches.push_back(matches[i]);
			}
		}


		matchVec->push_back(good_matches);
		index++;
	}
	//pickTopFive
	pickTopFive(matchVec, matchIndices);
	index = 0;
}

void findTopFiveBFMatches(Mat hqDesc, vector<Mat>* keyframeDesc, vector<vector< DMatch >>* matchVec, vector<int>* matchIndices){
	BFMatcher matcher;
	int index = 0;

	//Calculate matches between high quality image and 
	for (vector<Mat>::iterator it = keyframeDesc->begin(); it != keyframeDesc->end(); ++it){
		vector< DMatch > matches;

		//calculate initial matches
		Mat kfDesc = *it;
		matcher.match(hqDesc, kfDesc, matches);

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
	loadPhotos(keyframes, "C:\\Users\\Nick\\Pictures\\\FinalKeyframes\\");
	//load hqpictures
	loadPhotos(hqPictures, "C:\\Users\\Nick\\Pictures\\HiResTest\\");

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
	
	//int matchIndices[5] = { 0, 0, 0, 0, 0 };
	vector<int>* matchIndices = new vector<int>();
	vector<vector< DMatch >>* matchVec = new vector<vector< DMatch >>();
	findTopFiveFLANNMatches(hqPicDescriptors->front(), keyframeDescriptors, matchVec, matchIndices);
	//findTopFiveBFMatches(hqPicDescriptors->front(), keyframeDescriptors, matchVec, matchIndices);

/*	
	namedWindow("Top Result", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Top Result", (*keyframes)[matchIndices->front()]);                   // Show our image inside it.
	namedWindow("High Quality image", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("High Quality image", hqPicDescriptors->front());                   // Show our image inside it.
	*/

	Mat img_matches;
	Mat img1 = hqPictures->front();
	Mat img2 = (*keyframes)[*(matchIndices->begin()+1)];	//true match
	vector<KeyPoint> keypoints1 = hqPicFeatures->front();
	vector<KeyPoint> keypoints2 = (*keyframeFeatures)[*(matchIndices->begin() + 1)];
	vector<DMatch> matches = (*matchVec)[*(matchIndices->begin() + 1)];



	/* Find Homography of best match */
	Mat descriptors_object = (*keyframeDescriptors)[*(matchIndices->begin() + 1)];
	Mat descriptors_scene = hqPicDescriptors->front();
	//FlannBasedMatcher matcher;
	//std::vector< DMatch > matches;
	//matcher.match(descriptors_object, descriptors_scene, matches);


	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for (int i = 0; i < matches.size(); i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	//-- Localize the object
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.push_back(keypoints1[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints2[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(scene, obj, CV_RANSAC);
	cout << "Homography: " << H;


	//BF Matcher
	//drawMatches(img1, keypoints1, img2, keypoints1, matches, img_matches);

	//FLANN Matcher
	drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	imshow("Good Matches", img_matches);

	Mat transformedImg;
	warpPerspective(img1, transformedImg, H, Size(2064,1161));
	imwrite("C:\\Users\\Nick\\Pictures\\HiResTrans.jpg", transformedImg);

	//-- Show detected matches
	//imshow("Matches", img_matches);

	waitKey(0);

	
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
	//imshow("Matches", img_matches);


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