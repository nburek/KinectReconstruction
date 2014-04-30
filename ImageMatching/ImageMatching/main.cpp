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
	
	vector<int>* matchIndices = new vector<int>();
	vector<vector< DMatch >>* matchVec = new vector<vector< DMatch >>();
	findTopFiveFLANNMatches(hqPicDescriptors->front(), keyframeDescriptors, matchVec, matchIndices);
	//findTopFiveBFMatches(hqPicDescriptors->front(), keyframeDescriptors, matchVec, matchIndices);

	Mat img_matches;
	Mat img1 = hqPictures->front();
	//Note: compensating for current dataset: will probably want to remove the +1
	Mat img2 = (*keyframes)[*(matchIndices->begin()+1)];
	vector<KeyPoint> keypoints1 = hqPicFeatures->front();
	vector<KeyPoint> keypoints2 = (*keyframeFeatures)[*(matchIndices->begin() + 1)];
	vector<DMatch> matches = (*matchVec)[*(matchIndices->begin() + 1)];

	//BF Matcher
	//drawMatches(img1, keypoints1, img2, keypoints1, matches, img_matches);

	//FLANN Matcher
	drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches, Scalar::all(-1), Scalar::all(-1),	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	imshow("Good Matches", img_matches);

	waitKey(0);

	return 0;
}