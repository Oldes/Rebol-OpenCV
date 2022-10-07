#include "opencv2/opencv.hpp"

#include "reb-host.h"
#include "host-lib.h"

#define COMMAND           extern "C" int
#define CV_STRING(frm, n) (String((const char*)((REBSER*)RXA_ARG(frm, n).series)->data))

using namespace cv;
using namespace std;


COMMAND cmd_test(RXIFRM *frm, void *ctx) {
	Mat image;
	image = imread("/Users/oldes/GIT/Builder/assets/siskin-512.png", IMREAD_UNCHANGED);
	if(image.empty()) return RXR_NONE;

	auto detector = SiftFeatureDetector::create();
	vector <cv::KeyPoint> keypoints;
	detector->detect(image, keypoints);
	// Show the keypoints on the image.
	Mat image_with_keypoints ;
	drawKeypoints (image , keypoints , image_with_keypoints );

	imshow("Display window", image_with_keypoints);
	int k = waitKey(0); // Wait for a keystroke in the window
	return RXR_UNSET;
}


COMMAND cmd_pollKey(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_ARG(frm, 1).int64 = pollKey();
	return RXR_VALUE;
}

COMMAND cmd_waitKey(RXIFRM *frm, void *ctx) {
	RXA_ARG(frm, 1).int64 = waitKey(RXA_ARG(frm, 1).int32a); // Wait for a keystroke in the window
	return RXR_VALUE;
}


COMMAND cmd_namedWindow(RXIFRM *frm, void *ctx) {
	namedWindow(CV_STRING(frm,1), 0 );
	return RXR_UNSET;
}

COMMAND cmd_destroyAllWindows(RXIFRM *frm, void *ctx) {
	destroyAllWindows();
	pollKey();
	return RXR_UNSET;
}


COMMAND cmd_imread(RXIFRM *frm, void *ctx) {
	Mat image;
	REBSER* reb_image;

    image = imread(CV_STRING(frm,1), IMREAD_UNCHANGED);
    if (image.empty()) return RXR_NONE;
    image.convertTo(image, CV_8UC4);

    reb_image = (REBSER *)RL_MAKE_IMAGE(image.cols, image.rows);
    memcpy(reb_image->data, image.data, 4*image.cols*image.rows);

	RXA_TYPE(frm, 1) = RXT_IMAGE;
	RXA_ARG(frm, 1).width  = image.cols;
	RXA_ARG(frm, 1).height = image.rows;
	RXA_ARG(frm, 1).image  = reb_image;

	return RXR_VALUE;
}

COMMAND cmd_imshow(RXIFRM *frm, void *ctx) {
	Mat image;
	String name;
	RXIARG arg;

	// only image is allowed so no need to validate the input arg
	arg = RXA_ARG(frm, 1);
	image = Mat(arg.width, arg.height, CV_8UC4);
	image.data = ((REBSER*)arg.series)->data;

	// check if name was provided or use default
	name = (RXA_TYPE(frm, 3) == RXT_NONE) ? "Image" : CV_STRING(frm, 3);

	imshow(name, image);

	return RXR_TRUE;
}
