#include "opencv2/opencv.hpp"

#include "common.h"

#define COMMAND           extern "C" int
#define CV_STRING(frm, n) (String((const char*)((REBSER*)RXA_ARG(frm, n).series)->data)) //TODO: only ansii yet!
#define PAIR_X(frm, n)    (int)RXA_PAIR(frm,n).x
#define PAIR_Y(frm, n)    (int)RXA_PAIR(frm,2).y
#define FRM_IS_HANDLE(n, t) (RXA_TYPE(frm,n) == RXT_HANDLE && RXA_HANDLE_TYPE(frm, n) == t)

using namespace cv;
using namespace std;

extern REBCNT Handle_cvVideoCapture;
extern REBCNT Handle_cvMat;

// TODO: automatic releasing unreferenced handles is not working now!
extern "C" void* releaseVideoCapture(void* cls) {
	printf("cls %p\n", cls);
	if (cls != NULL) {
		VideoCapture *cap = (VideoCapture*)cls;
		cap->release();
	}
	return NULL;
}
extern "C" void* releaseMat(void* cls) {
	printf("cls %p\n", cls);
	if (cls != NULL) {
		Mat *mat = (Mat*)cls;
		mat->release();
	}
	return NULL;
}

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
	moveWindow("Display window", 300, 300);
	int k = waitKey(0); // Wait for a keystroke in the window
	return RXR_UNSET;
}

COMMAND cmd_VideoCapture(RXIFRM *frm, void *ctx) {
	VideoCapture *cap;
	int deviceID;

	cap = new VideoCapture();

	if (RXA_TYPE(frm, 1) == RXT_INTEGER) {
		cap->open(RXA_INT32(frm,1), CAP_ANY);
	} else {
		cap->open(CV_STRING(frm,1), CAP_ANY);
	}	
	
	debug_print("cap %p\n", cap);
	if (!cap->isOpened()) {
		cap->release();
		return RXR_FALSE;
	}

//	cout<<cap->getBackendName()<<endl;

	RXA_HANDLE(frm, 1) = cap;
	RXA_HANDLE_TYPE(frm, 1) = Handle_cvVideoCapture;
	RXA_HANDLE_FLAGS(frm, 1) = HANDLE_CONTEXT;
	RXA_TYPE(frm, 1) = RXT_HANDLE;
	return RXR_VALUE;
}

COMMAND cmd_free(RXIFRM *frm, void *ctx) {
	REBCNT type = RXA_HANDLE_TYPE(frm, 1);
	if (type == Handle_cvVideoCapture) {
		VideoCapture *cap;
		cap = (VideoCapture*)RXA_HANDLE(frm, 1);
		debug_print("free cap %p\n", cap);
		if (cap == NULL) return RXR_FALSE;
		cap->release();
		RXA_HANDLE(frm, 1) = NULL;
		return RXR_TRUE;
	}
	else if (type == Handle_cvMat) {
		Mat *mat = (Mat*)RXA_HANDLE(frm, 1);
		debug_print("free mat %p\n", mat);
		if (mat == NULL) return RXR_FALSE;
		mat->release();
		RXA_HANDLE(frm, 1) = NULL;
		return RXR_TRUE;
	}
	return RXR_NONE;
}

COMMAND cmd_read(RXIFRM *frm, void *ctx) {
	VideoCapture *cap;
	Mat *frame;

	if(!FRM_IS_HANDLE(1, Handle_cvVideoCapture))
		return RXR_FALSE;

	cap = (VideoCapture*)RXA_HANDLE(frm, 1);
	if (!cap->isOpened()) return RXR_NONE;

	if (FRM_IS_HANDLE(3, Handle_cvMat)) {
		frame = (Mat*)RXA_HANDLE(frm, 3);
	} else {
		frame = new Mat();
	}

	cap->read(*frame);
	if (frame->empty()) return RXR_NONE;
	//imshow("Live", *frame);
	RXA_HANDLE(frm, 1) = frame;
	RXA_HANDLE_TYPE(frm, 1) = Handle_cvMat;
	RXA_HANDLE_FLAGS(frm, 1) = HANDLE_CONTEXT;
	RXA_TYPE(frm, 1) = RXT_HANDLE;
	return RXR_VALUE;
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
	namedWindow(CV_STRING(frm,1), WINDOW_NORMAL );
	return RXR_UNSET;
}

COMMAND cmd_resizeWindow(RXIFRM *frm, void *ctx) {
	resizeWindow(CV_STRING(frm,1), PAIR_X(frm,2), PAIR_Y(frm,2));
	return RXR_UNSET;
}

COMMAND cmd_moveWindow(RXIFRM *frm, void *ctx) {
	moveWindow(CV_STRING(frm,1), PAIR_X(frm,2), PAIR_Y(frm,2));
	return RXR_UNSET;
}

COMMAND cmd_destroyAllWindows(RXIFRM *frm, void *ctx) {
	destroyAllWindows();
	pollKey();
	return RXR_UNSET;
}


COMMAND cmd_startWindowThread(RXIFRM *frm, void *ctx) {
	startWindowThread();
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

COMMAND cmd_imwrite(RXIFRM *frm, void *ctx) {
	String name = CV_STRING(frm, 1);
	bool result = false;
	vector<int> params = vector<int>();

	if (RXA_TYPE(frm, 4) == RXT_BLOCK) {
		REBSER *cmds;
		REBCNT index, type;
		RXIARG arg1,arg2;
		cmds = (REBSER*)RXA_SERIES(frm, 4);
		index = RXA_INDEX(frm, 4);
		while (index+1 < cmds->tail
			&& RXT_INTEGER == RL_GET_VALUE_RESOLVED(cmds, index++, &arg1)
			&& RXT_INTEGER == RL_GET_VALUE_RESOLVED(cmds, index++, &arg2)) {
			params.push_back(arg1.int32a);
    		params.push_back(arg2.int32a);
		}
	}

	if (FRM_IS_HANDLE(2, Handle_cvMat)) {
		result = imwrite(name, *(Mat*)RXA_HANDLE(frm, 2), params);
	} else { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 2);
		image = Mat(arg.width, arg.height, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		result = imwrite(name, image, params);
	}
	return result ? RXR_VALUE : RXR_FALSE;
}

COMMAND cmd_imshow(RXIFRM *frm, void *ctx) {
	// check if name was provided or use default
	String name = (RXA_TYPE(frm, 3) == RXT_NONE) ? "Image" : CV_STRING(frm, 3);

	if (FRM_IS_HANDLE(1, Handle_cvMat)) {
		imshow(name, *(Mat*)RXA_HANDLE(frm, 1));
	} else { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.width, arg.height, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		imshow(name, image);
	}
	return RXR_TRUE;
}
