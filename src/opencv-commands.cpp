#include "opencv2/opencv.hpp"

#include "common.h"

#define COMMAND           extern "C" int
#define CV_STRING(frm, n) (String((const char*)((REBSER*)RXA_ARG(frm, n).series)->data)) //TODO: only ansii yet!
#define PAIR_X(frm, n)    (int)RXA_PAIR(frm,n).x
#define PAIR_Y(frm, n)    (int)RXA_PAIR(frm,2).y

#define FRM_IS_HANDLE(n, t)     (RXA_TYPE(frm,n) == RXT_HANDLE && RXA_HANDLE_TYPE(frm, n) == t)
#define ARG_Is_Mat(n)           FRM_IS_HANDLE(n, Handle_cvMat)
#define ARG_Is_VideoCapture(n)  FRM_IS_HANDLE(n, Handle_cvVideoCapture)
#define ARG_Is_Image(n)         (RXA_TYPE(frm,n) == RXT_IMAGE)
#define ARG_Mat(n)              (Mat*)(RXA_HANDLE_CONTEXT(frm, n)->handle)
#define ARG_VideoCapture(n)     (VideoCapture*)(RXA_HANDLE_CONTEXT(frm, n)->handle)

using namespace cv;
using namespace std;

extern REBCNT Handle_cvVideoCapture;
extern REBCNT Handle_cvMat;

// TODO: automatic releasing unreferenced handles is not working now!
extern "C" void* releaseVideoCapture(void* cls) {
	debug_print("GC VideoCapture class %p\n", cls);
	if (cls != NULL) {
		VideoCapture *cap = (VideoCapture*)cls;
		cap->release();
	}
	return NULL;
}
extern "C" void* releaseMat(void* cls) {
	debug_print("GC Mat class %p\n", cls);
	if (cls != NULL) {
		Mat *mat = (Mat*)cls;
		mat->release();
	}
	return NULL;
}

static int initRXHandle(RXIFRM *frm, int index, void* handle, REBCNT type) {
	REBHOB* hob = RL_MAKE_HANDLE_CONTEXT(type);
	debug_print("new hob: %p handle: %p\n", hob, handle);
	if (hob == NULL) return RXR_FALSE;
	hob->handle = handle;
	RXA_HANDLE(frm, 1) = hob;
	RXA_HANDLE_TYPE(frm, 1) = type;
	RXA_HANDLE_FLAGS(frm, 1) = HANDLE_CONTEXT;
	RXA_TYPE(frm, 1) = RXT_HANDLE;
	return RXR_VALUE;
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

	return initRXHandle(frm, 1, cap, Handle_cvVideoCapture);
}

COMMAND cmd_free(RXIFRM *frm, void *ctx) {
	REBCNT type = RXA_HANDLE_TYPE(frm, 1);
	if (type == Handle_cvVideoCapture) {
		VideoCapture *cap = ARG_VideoCapture(1);
		debug_print("free cap %p\n", cap);
		if (cap == NULL) return RXR_FALSE;
		cap->release();
	}
	else if (type == Handle_cvMat) {
		Mat *mat = ARG_Mat(1);
		debug_print("free mat %p\n", mat);
		if (mat == NULL) return RXR_FALSE;
		mat->release();
	}
	else {
		return RXR_NONE;
	}
	RXA_HANDLE_CONTEXT(frm, 1)->handle = NULL;
	return RXR_TRUE;
}

COMMAND cmd_read(RXIFRM *frm, void *ctx) {
	VideoCapture *cap;
	Mat *frame;

	if(!ARG_Is_VideoCapture(1))
		return RXR_FALSE;

	cap = ARG_VideoCapture(1);
	if (!cap->isOpened()) return RXR_NONE;

	if (FRM_IS_HANDLE(3, Handle_cvMat) && ARG_Mat(3)) {
		frame = ARG_Mat(3);
	} else {
		frame = new Mat();
		if (RXR_VALUE != initRXHandle(frm, 1, frame, Handle_cvMat))
			return RXR_FALSE;
	}

	cap->read(*frame);
	if (frame->empty()) return RXR_NONE;
	
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
	cvtColor(image, image, COLOR_BGR2BGRA);

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

	// Prepare optional parameters
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

	if (ARG_Is_Mat(2)) {
		if (!ARG_Mat(2)) return RXR_FALSE;
		result = imwrite(name, *ARG_Mat(2), params);
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

	if (ARG_Is_Mat(1)) {
		if (!ARG_Mat(1)) return RXR_FALSE;
		imshow(name, *ARG_Mat(1));
	} else { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.width, arg.height, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		imshow(name, image);
	}
	return RXR_TRUE;
}

COMMAND cmd_bilateralFilter(RXIFRM *frm, void *ctx) {
	int d             = RXA_INT32(frm, 2);
	double sigmaColor = RXA_DEC64(frm, 3);
	double sigmaSpace = RXA_DEC64(frm, 4);
	int borderType = RXA_TYPE(frm, 5) == RXT_INTEGER ? RXA_INT32(frm, 5) : BORDER_DEFAULT;

	if (ARG_Is_Mat(1)) {
		Mat *img = ARG_Mat(1);
		Mat tmp;
		if (!img) return RXR_FALSE;
		bilateralFilter(*img, tmp, d, sigmaColor, sigmaSpace, borderType);
		cvtColor(tmp, *img, COLOR_BGR2BGRA);
	} else { // input is Rebol image
		Mat image, src, tmp;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.width, arg.height, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		// Rebol image is stored as BGRA, but the filter accpets only BGR
		cvtColor(image, src, COLOR_BGRA2BGR);
		bilateralFilter(src, tmp, d, sigmaColor, sigmaSpace, borderType);
		// and convert it back to BGRA in the Rebol image data
		cvtColor(tmp, image, COLOR_BGR2BGRA);
	}
	return RXR_VALUE;
}

COMMAND cmd_blur(RXIFRM *frm, void *ctx) {
	Size ksize = Size(PAIR_X(frm, 2), PAIR_Y(frm, 2));
	Point anchor = Point(-1, -1);
	int borderType = RXA_TYPE(frm, 4) == RXT_INTEGER ? RXA_INT32(frm, 4) : BORDER_DEFAULT;

	if (ARG_Is_Mat(1)) {
		if (!ARG_Mat(1)) return RXR_FALSE;
		blur(*ARG_Mat(1), *ARG_Mat(1), ksize, anchor, borderType);
	} else { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.width, arg.height, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		blur(image, image, ksize, anchor, borderType);
	}
	return RXR_VALUE;
}

COMMAND cmd_cvtColor(RXIFRM *frm, void *ctx) {
	Mat *dst;
	int code = RXA_INT32(frm, 2);

	dst = new Mat();

	if (ARG_Is_Mat(1)) {
		if (!ARG_Mat(1)) return RXR_FALSE;
		cvtColor(*ARG_Mat(1), *dst, code);
	} else { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.width, arg.height, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		cvtColor(image, *dst, code);
	}
	return initRXHandle(frm, 1, dst, Handle_cvMat);
}

