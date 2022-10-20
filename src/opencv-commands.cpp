#include "opencv2/opencv.hpp"

extern "C" {
	#include "common.h"
}

static char* err_buff[255]; // temporary buffer used to pass an exception messages to Rebol side

#define EXCEPTION_TRY   try {
#define EXCEPTION_CATCH } catch( cv::Exception& e ) {\
	snprintf((char*)err_buff, 254,"%s", e.err.c_str());\
	RXA_SERIES(frm, 1) = (void*)err_buff;\
	return RXR_ERROR;}
	// must use the temporary buffer above, because the error String could be freed before used on the Rebol side

#define COMMAND           extern "C" int
#define PAIR_X(frm, n)    (int)RXA_PAIR(frm,n).x
#define PAIR_Y(frm, n)    (int)RXA_PAIR(frm,n).y

#define FRM_IS_HANDLE(n, t)     (RXA_TYPE(frm,n) == RXT_HANDLE && RXA_HANDLE_TYPE(frm, n) == t)
#define ARG_Is_Mat(n)           FRM_IS_HANDLE(n, Handle_cvMat)
#define ARG_Is_VideoCapture(n)  FRM_IS_HANDLE(n, Handle_cvVideoCapture)
#define ARG_Is_VideoWriter(n)   FRM_IS_HANDLE(n, Handle_cvVideoWriter)
#define ARG_Is_Image(n)         (RXA_TYPE(frm,n) == RXT_IMAGE)
#define ARG_Is_Pair(n)          (RXA_TYPE(frm,n) == RXT_PAIR)
#define ARG_Is_Integer(n)       (RXA_TYPE(frm,n) == RXT_INTEGER)
#define ARG_Is_None(n)          (RXA_TYPE(frm,n) == RXT_NONE)
#define ARG_Mat(n)              (Mat*)(RXA_HANDLE_CONTEXT(frm, n)->handle)
#define ARG_VideoCapture(n)     (VideoCapture*)(RXA_HANDLE_CONTEXT(frm, n)->handle)
#define ARG_VideoWriter(n)      (VideoWriter*)(RXA_HANDLE_CONTEXT(frm, n)->handle)
#define ARG_Double(n)           (RXA_TYPE(frm,n) == RXT_DECIMAL ? RXA_DEC64(frm,n) : (double)RXA_INT64(frm,n))
#define ARG_Int(n)              (RXA_TYPE(frm,n) == RXT_INTEGER ? RXA_INT32(frm,n) : (int)RXA_DEC64(frm,n))
#define ARG_Size(n)             Size(PAIR_X(frm,n), PAIR_Y(frm,n))
#define ARG_String(n)           (String((const char*)((REBSER*)RXA_ARG(frm, n).series)->data)) //TODO: only ansii yet!

enum MatProperties {
	MAT_SIZE = 1,
	MAT_TYPE,
	MAT_DEPTH,
	MAT_CHANNELS,
	MAT_BINARY,
	MAT_IMAGE,
	MAT_VECTOR,
};

using namespace cv;
using namespace std;

extern "C" {
	extern REBCNT Handle_cvVideoCapture;
	extern REBCNT Handle_cvVideoWriter;
	extern REBCNT Handle_cvMat;

	void* releaseVideoCapture(void* cls) {
		debug_print("GC VideoCapture class %p\n", cls);
		if (cls != NULL) {
			VideoCapture *cap = (VideoCapture*)cls;
			cap->release();
		}
		return NULL;
	}
	void* releaseVideoWriter(void* cls) {
		debug_print("GC VideoWriter class %p\n", cls);
		if (cls != NULL) {
			VideoWriter *cap = (VideoWriter*)cls;
			cap->release();
		}
		return NULL;
	}
	void* releaseMat(void* cls) {
		debug_print("GC Mat class %p\n", cls);
		if (cls != NULL) {
			Mat *mat = (Mat*)cls;
			mat->release();
		}
		return NULL;
	}
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

static Mat* new_Mat_From_Image_Arg(RXIFRM *frm, int index) {
	RXIARG arg = RXA_ARG(frm, index);
	Mat *mat = new Mat(arg.height, arg.width, CV_8UC4);
	mat->data = ((REBSER*)arg.series)->data;
	return mat;
}

COMMAND cmd_test(RXIFRM *frm, void *ctx) {
	Mat src;
    // use default camera as video source
    VideoCapture cap(0);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    // get one frame from camera to know frame size and type
    cap >> src;
    // check if we succeeded
    if (src.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        return -1;
    }
    bool isColor = (src.type() == CV_8UC3);
    //--- INITIALIZE VIDEOWRITER
    VideoWriter writer;
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');  // select desired codec (must be available at runtime)
    double fps = 25.0;                          // framerate of the created video stream
    string filename = "./live.avi";             // name of the output video file
    writer.open(filename, codec, fps, src.size(), isColor);
    // check if we succeeded
    if (!writer.isOpened()) {
        cerr << "Could not open the output video file for write\n";
        return -1;
    }
    //--- GRAB AND WRITE LOOP
    cout << "Writing videofile: " << filename << endl
         << "Press any key to terminate" << endl;
    for (;;)
    {
        // check if we succeeded
        if (!cap.read(src)) {
            cerr << "ERROR! blank frame grabbed\n";
            continue;
        }
        // encode the frame into the videofile stream
        writer.write(src);
        // show live and wait for a key with timeout long enough to show images
        imshow("Live", src);
        if (waitKey(5) >= 0)
            break;
    }

//	Mat image;
//	image = imread("/Users/oldes/GIT/Builder/assets/siskin-512.png", IMREAD_UNCHANGED);
//	if(image.empty()) return RXR_NONE;
//
//	auto detector = SiftFeatureDetector::create();
//	vector <cv::KeyPoint> keypoints;
//	detector->detect(image, keypoints);
//	// Show the keypoints on the image.
//	Mat image_with_keypoints ;
//	drawKeypoints (image , keypoints , image_with_keypoints );
//
//	imshow("Display window", image_with_keypoints);
//	moveWindow("Display window", 300, 300);
//	int k = waitKey(0); // Wait for a keystroke in the window
	return RXR_UNSET;
}

COMMAND cmd_Matrix(RXIFRM *frm, void *ctx) {
	Mat *mat;
	Size size;
	int type;
	
	if (ARG_Is_Image(1)) {
		mat = new_Mat_From_Image_Arg(frm, 1);
	} else {
		if(ARG_Is_Mat(1)) {
			if (!ARG_Mat(1)) return RXR_NONE;
			mat = ARG_Mat(1);
			size = mat->size();
			type = mat->type();
		}
		else {
			size = ARG_Size(1);
		}
		if (ARG_Is_Integer(3)) {
			type = ARG_Int(3);
		}
		mat = new Mat(size, type, Scalar(0,0,0));
	}

	return initRXHandle(frm, 1, mat, Handle_cvMat);
}

COMMAND cmd_VideoCapture(RXIFRM *frm, void *ctx) {
	VideoCapture *cap;
	int deviceID;

	EXCEPTION_TRY

	cap = new VideoCapture();

	if (RXA_TYPE(frm, 1) == RXT_INTEGER) {
		cap->open(ARG_Int(1), CAP_ANY);
	} else {
		cap->open(ARG_String(1), CAP_ANY);
	}

	EXCEPTION_CATCH
	
	debug_print("cap %p\n", cap);
	if (!cap->isOpened()) {
		cap->release();
		return RXR_FALSE;
	}

//	cout<<cap->getBackendName()<<endl;

	return initRXHandle(frm, 1, cap, Handle_cvVideoCapture);
}

COMMAND cmd_VideoWriter(RXIFRM *frm, void *ctx) {
	VideoWriter *writer;

	String name  = ARG_String(1);
	int    codec = ARG_Int(2);
	double fps   = ARG_Double(3);
	Size   size  = ARG_Size(4);

	EXCEPTION_TRY

	if (!codec) codec = VideoWriter::fourcc('M', 'J', 'P', 'G');

	writer = new VideoWriter();

	writer->open(name, codec, fps, size, true);
	if (!writer->isOpened()) {
		puts("Not opened!");
		writer->release();
		return RXR_FALSE;
	}

	EXCEPTION_CATCH

	return initRXHandle(frm, 1, writer, Handle_cvVideoWriter);
}

COMMAND cmd_get_property(RXIFRM *frm, void *ctx) {
	int propid = RXA_INT32(frm, 2);
	double result = 0;

	if (!RXA_HANDLE_CONTEXT(frm, 1)) return RXR_FALSE;

	
	if (ARG_Is_VideoCapture(1)) {
		VideoCapture *cap = ARG_VideoCapture(1);
		result = cap->get(propid);
	}
	else if (ARG_Is_Mat(1) && ARG_Mat(1)) {
		Mat *mat = ARG_Mat(1);
		switch(propid){
			case MAT_SIZE: {
				Size size = mat->size();
				RXA_TYPE(frm,1) = RXT_PAIR;
				RXA_PAIR(frm,1).x = size.width;
				RXA_PAIR(frm,1).y = size.height;
				return RXR_VALUE;
			}
			case MAT_TYPE: {
				RXA_TYPE(frm,1) = RXT_INTEGER;
				RXA_ARG(frm,1).int64 = mat->type();
				return RXR_VALUE;
			}
			case MAT_DEPTH: {
				RXA_TYPE(frm,1) = RXT_INTEGER;
				RXA_ARG(frm,1).int64 = mat->depth();
				return RXR_VALUE;
			}
			case MAT_CHANNELS: {
				RXA_TYPE(frm,1) = RXT_INTEGER;
				RXA_ARG(frm,1).int64 = mat->channels();
				return RXR_VALUE;
			}
			case MAT_BINARY: {
				int bytes = mat->elemSize() * mat->cols * mat->rows;
				REBSER *bin = (REBSER *)RL_MAKE_STRING(bytes, FALSE);
				//TODO: expects, that data are continuous!
				memcpy(bin->data, mat->data,  bytes);
				SERIES_TAIL(bin) = bytes;
				RXA_SERIES(frm, 1) = bin;
				RXA_TYPE  (frm, 1) = RXT_BINARY;
				RXA_INDEX (frm, 1) = 0;
				return RXR_VALUE;
			}
			case MAT_IMAGE: {
				Mat tmp;
				int channels = mat->channels();
				if (channels == 1)
					cvtColor(*mat, tmp, COLOR_GRAY2BGRA);
				else if (channels == 3 || channels == 4)
					cvtColor(*mat, tmp, COLOR_BGR2BGRA);
				else return RXR_NONE;

				REBSER *reb_image = (REBSER *)RL_MAKE_IMAGE(tmp.cols, tmp.rows);
				memcpy(reb_image->data, tmp.data, tmp.elemSize()*tmp.cols*tmp.rows);

				RXA_TYPE(frm, 1) = RXT_IMAGE;
				RXA_ARG(frm, 1).width  = tmp.cols;
				RXA_ARG(frm, 1).height = tmp.rows;
				RXA_ARG(frm, 1).image  = reb_image;
				return RXR_VALUE;
			}
			case MAT_VECTOR: {
				REBINT type; // int = 0, decimal = 1
				REBINT sign; // 0 = signed, 1 = unsigned
				REBINT bits, dims;
				REBINT size = mat->cols * mat->rows * mat->channels();
				cout << mat->depth() << endl;
				switch(mat->depth()) {
					case CV_8U:  type = 0; sign = 1; bits = 8;  break;
					case CV_8S:  type = 0; sign = 0; bits = 8;  break;
					case CV_16U: type = 0; sign = 1; bits = 16; break;
					case CV_16S: type = 0; sign = 0; bits = 16; break;
					case CV_32S: type = 0; sign = 0; bits = 32; break;
					case CV_32F: type = 1; sign = 0; bits = 32; break;
					case CV_64F: type = 1; sign = 0; bits = 64; break;
				}
				int bytes = mat->elemSize() * mat->cols * mat->rows;
				REBSER *vec = (REBSER *)RL_MAKE_VECTOR(type, sign, dims, bits, size);
				memcpy(vec->data, mat->data,  bytes);
				SERIES_TAIL(vec) = size;
				RXA_SERIES(frm, 1) = vec;
				RXA_TYPE  (frm, 1) = RXT_VECTOR;
				RXA_INDEX (frm, 1) = 0;
				return RXR_VALUE;
			}
		}
	}
	else {
		return RXR_NONE;
	}
	RXA_TYPE(frm, 1) = RXT_DECIMAL;
	RXA_DEC64(frm, 1) = result;
	return RXR_VALUE;
}

COMMAND cmd_set_property(RXIFRM *frm, void *ctx) {
	int propid   = RXA_INT32(frm, 2);
	double value = RXA_DEC64(frm, 3);
	bool result = FALSE;

	if (!RXA_HANDLE_CONTEXT(frm, 1)) return RXR_FALSE;

	EXCEPTION_TRY
	if (ARG_Is_VideoCapture(1)) {
		VideoCapture *cap = ARG_VideoCapture(1);
		result = cap->set(propid, value);
	}
	EXCEPTION_CATCH
	
	RXA_TYPE(frm, 1) = RXT_LOGIC;
	RXA_LOGIC(frm, 1) = result;
	return RXR_VALUE;
}

COMMAND cmd_free(RXIFRM *frm, void *ctx) {
	REBCNT type = RXA_HANDLE_TYPE(frm, 1);
	
	EXCEPTION_TRY
	if (type == Handle_cvMat) {
		Mat *mat = ARG_Mat(1);
		debug_print("Free Mat %p\n", mat);
		if (mat == NULL) return RXR_FALSE;
		mat->release();
	}
	else if (type == Handle_cvVideoCapture) {
		VideoCapture *cap = ARG_VideoCapture(1);
		debug_print("Free VideoCapture %p\n", cap);
		if (cap == NULL) return RXR_FALSE;
		cap->release();
	}
	else if (type == Handle_cvVideoWriter) {
		VideoWriter *writer = ARG_VideoWriter(1);
		debug_print("Free VideoWriter %p\n", writer);
		if (writer == NULL) return RXR_FALSE;
		writer->release();
	}
	else {
		return RXR_NONE;
	}
	RXA_HANDLE_CONTEXT(frm, 1)->handle = NULL;
	EXCEPTION_CATCH

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

	EXCEPTION_TRY
	cap->read(*frame);
	EXCEPTION_CATCH

	if (frame->empty()) return RXR_NONE;
	
	return RXR_VALUE;
}

COMMAND cmd_write(RXIFRM *frm, void *ctx) {
	VideoWriter *writer;

	if(!ARG_Is_VideoWriter(1))
		return RXR_FALSE;

	writer = ARG_VideoWriter(1);
	if (!writer || !writer->isOpened()) return RXR_NONE;

	EXCEPTION_TRY
	if (ARG_Is_Mat(2)) {
		if (!ARG_Mat(2)) return RXR_FALSE;
		writer->write(*ARG_Mat(2));
	} else { // input is Rebol image
		Mat image , mat;
		RXIARG arg = RXA_ARG(frm, 2);
		image = Mat(arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		image.convertTo(mat, CV_8UC3);
		writer->write(mat);
	}
	EXCEPTION_CATCH

	return RXR_TRUE;
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
	namedWindow(ARG_String(1), WINDOW_NORMAL );
	return RXR_UNSET;
}

COMMAND cmd_resizeWindow(RXIFRM *frm, void *ctx) {
	resizeWindow(ARG_String(1), PAIR_X(frm,2), PAIR_Y(frm,2));
	return RXR_UNSET;
}

COMMAND cmd_moveWindow(RXIFRM *frm, void *ctx) {
	moveWindow(ARG_String(1), PAIR_X(frm,2), PAIR_Y(frm,2));
	return RXR_UNSET;
}

COMMAND cmd_getWindowProperty(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_DECIMAL;
	RXA_DEC64(frm, 1) = getWindowProperty(ARG_String(1), ARG_Int(2));
	return RXR_VALUE;
}

COMMAND cmd_setWindowProperty(RXIFRM *frm, void *ctx) {
	setWindowProperty(ARG_String(1), ARG_Int(2), ARG_Int(3));
	return RXR_UNSET;
}

COMMAND cmd_destroyAllWindows(RXIFRM *frm, void *ctx) {
	destroyAllWindows();
	pollKey();
	return RXR_UNSET;
}

COMMAND cmd_destroyWindow(RXIFRM *frm, void *ctx) {
	destroyWindow(ARG_String(1));
	return RXR_UNSET;
}


COMMAND cmd_startWindowThread(RXIFRM *frm, void *ctx) {
	startWindowThread();
	return RXR_UNSET;
}


COMMAND cmd_imread(RXIFRM *frm, void *ctx) {
	Mat image;
	String filename = ARG_String(1);

	EXCEPTION_TRY
	image = imread(filename, IMREAD_UNCHANGED);
	if (image.empty()) return RXR_NONE;
	if (ARG_Is_None(2)) {
		Mat *result = new Mat(image);
		if (RXR_VALUE != initRXHandle(frm, 1, result, Handle_cvMat))
			return RXR_FALSE;
	}
	else {
		REBSER* reb_image;
		cvtColor(image, image, COLOR_BGR2BGRA);
		reb_image = (REBSER *)RL_MAKE_IMAGE(image.cols, image.rows);
		memcpy(reb_image->data, image.data, 4*image.cols*image.rows);

		RXA_TYPE(frm, 1) = RXT_IMAGE;
		RXA_ARG(frm, 1).width  = image.cols;
		RXA_ARG(frm, 1).height = image.rows;
		RXA_ARG(frm, 1).image  = reb_image;
	}
	
	EXCEPTION_CATCH

	return RXR_VALUE;
}

COMMAND cmd_imwrite(RXIFRM *frm, void *ctx) {
	String name = ARG_String(1);
	bool result = false;
	vector<int> params = vector<int>();

	EXCEPTION_TRY
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
	} else if(ARG_Is_Image(2)) { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 2);
		image = Mat(arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		result = imwrite(name, image, params);
	}
	EXCEPTION_CATCH

	return result ? RXR_VALUE : RXR_FALSE;
}

COMMAND cmd_imshow(RXIFRM *frm, void *ctx) {
	// check if name was provided or use default
	String name = (RXA_TYPE(frm, 3) == RXT_NONE) ? "Image" : ARG_String(3);

	if (ARG_Is_Mat(1)) {
		if (!ARG_Mat(1)) return RXR_FALSE;
		imshow(name, *ARG_Mat(1));
	} else if(ARG_Is_Image(1)) { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		//printf("img %i %i\n", arg.width, arg.height);
		image = Mat( arg.height, arg.width, CV_8UC4);
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

	EXCEPTION_TRY
	if (ARG_Is_Mat(1)) {
		Mat *img = ARG_Mat(1);
		Mat tmp;
		if (!img) return RXR_FALSE;
		bilateralFilter(*img, tmp, d, sigmaColor, sigmaSpace, borderType);
		cvtColor(tmp, *img, COLOR_BGR2BGRA);
	} else if(ARG_Is_Image(1)) { // input is Rebol image
		Mat image, src, tmp;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		// Rebol image is stored as BGRA, but the filter accpets only BGR
		cvtColor(image, src, COLOR_BGRA2BGR);
		bilateralFilter(src, tmp, d, sigmaColor, sigmaSpace, borderType);
		// and convert it back to BGRA in the Rebol image data
		cvtColor(tmp, image, COLOR_BGR2BGRA);
	}
	EXCEPTION_CATCH

	return RXR_VALUE;
}

COMMAND cmd_blur(RXIFRM *frm, void *ctx) {
	Size ksize = Size(PAIR_X(frm, 2), PAIR_Y(frm, 2));
	Point anchor = Point(-1, -1);
	int borderType = RXA_TYPE(frm, 4) == RXT_INTEGER ? RXA_INT32(frm, 4) : BORDER_DEFAULT;

	EXCEPTION_TRY
	if (ARG_Is_Mat(1)) {
		if (!ARG_Mat(1)) return RXR_FALSE;
		blur(*ARG_Mat(1), *ARG_Mat(1), ksize, anchor, borderType);
	} else if(ARG_Is_Image(1)) { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		blur(image, image, ksize, anchor, borderType);
	}
	EXCEPTION_CATCH

	return RXR_VALUE;
}

COMMAND cmd_cvtColor(RXIFRM *frm, void *ctx) {
	Mat *dst;
	int code = RXA_INT32(frm, 2);

	dst = new Mat();

	EXCEPTION_TRY
	if (ARG_Is_Mat(1)) {
		if (!ARG_Mat(1)) return RXR_FALSE;
		cvtColor(*ARG_Mat(1), *dst, code);
	} else if(ARG_Is_Image(1)) { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat(arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		cvtColor(image, *dst, code);
	}
	EXCEPTION_CATCH

	return initRXHandle(frm, 1, dst, Handle_cvMat);
}

COMMAND cmd_resize(RXIFRM *frm, void *ctx) {
	Mat *src;
	Mat *dst;
	Size size;
	int interpolation = INTER_LINEAR;
	bool newHandle = FALSE;

	if (ARG_Is_Mat(1) && ARG_Mat(1)) { 
		src = ARG_Mat(1);
	} else if (ARG_Is_Image(1)) { // input is Rebol image
		src = new_Mat_From_Image_Arg(frm, 1); // delete before return!
	} else return RXR_NONE;

	if (ARG_Is_Mat(4) && ARG_Mat(4)) { // /into handle
		dst = ARG_Mat(4);
	} else {
		newHandle = TRUE;
		dst = new Mat();
	}

	if (ARG_Is_Pair(2)) {
		size = ARG_Size(2);
	} else {
		double scale = RXA_DEC64(frm, 2);
		size = src->size();
		size.width *= scale;
		size.height *= scale;
	}

	if (ARG_Is_Integer(6)) {
		interpolation = ARG_Int(6);
	}
	
	EXCEPTION_TRY
	resize(*src, *dst, size, 0, 0, interpolation);
	EXCEPTION_CATCH

	if (ARG_Is_Image(1)) {
		// if input was Rebol image, release the temporary Mat created from it
		delete src;
	}

	if (newHandle) {
		return initRXHandle(frm, 1, dst, Handle_cvMat);
	}
	else {
		// requested output to given existing array
		RXA_ARG(frm, 1) = RXA_ARG(frm, 4);
		return RXR_VALUE;
	}
}

COMMAND cmd_threshold(RXIFRM *frm, void *ctx) {
	Mat *src;
	Mat *dst;

	if (ARG_Is_Mat(1) && ARG_Mat(1)) { 
		src = ARG_Mat(1);
	} else return RXR_NONE;

	if (ARG_Is_Mat(2) && ARG_Mat(2)) { 
		dst = ARG_Mat(2);
	} else return RXR_NONE;

	EXCEPTION_TRY
	RXA_TYPE(frm, 1) = RXT_DECIMAL;
	RXA_DEC64(frm, 1) = threshold(*src, *dst, ARG_Double(3), ARG_Double(4), ARG_Int(5));
	EXCEPTION_CATCH
	return RXR_VALUE;
}


COMMAND cmd_addWeighted(RXIFRM *frm, void *ctx) {
	Mat *src1    = ARG_Mat(1);
	double alpha = ARG_Double(2);
	Mat *src2    = ARG_Mat(3);
	double beta  = ARG_Double(4);
	double delta = ARG_Double(5); 
	Mat *dst     = ARG_Mat(6);

	if(!src1 || !src2 || !dst) return RXR_NONE;

	EXCEPTION_TRY
	addWeighted(*src1, alpha, *src2, beta, delta, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 6);
	return RXR_VALUE;
}


enum  MatMathOp {
  BITWISE_AND = 0,
  BITWISE_OR  = 1,
  BITWISE_XOR = 2,
  MATH_ADD,
  MATH_SUBTRACT,
  MATH_MULTIPLY = 10,
  MATH_DIVIDE,
};

static int mat_math_op(RXIFRM *frm, void *ctx, int op) {
	Mat *src1;
	Mat *src2;
	Mat *dst;
	Mat *mask = NULL;
	bool newHandle = FALSE;
	double scale = 1;

	src1 = ARG_Mat(1);
	src2 = ARG_Mat(2);

	if (ARG_Is_Mat(4)) { // /into handle
		dst = ARG_Mat(4);
	} else {
		newHandle = TRUE;
		dst = new Mat();
	}
	if (op < 10) {
		if (ARG_Is_Mat(6)) { // mask
			mask = ARG_Mat(6);
		}
		if(!mask)
			mask = (Mat*)&noArray();
	} else {
		// multiply and divide don't have mask, but scale instead
		scale = ARG_Double(6);
	}

	if (!src1 || !src2 || !dst ) return RXR_NONE;
	

	EXCEPTION_TRY
	switch (op){
		case BITWISE_AND:   bitwise_and(*src1, *src2, *dst, *mask); break;
		case BITWISE_OR:    bitwise_or(*src1, *src2, *dst, *mask); break;
		case BITWISE_XOR:   bitwise_xor(*src1, *src2, *dst, *mask); break;
		case MATH_ADD:      add(*src1, *src2, *dst, *mask); break;
		case MATH_SUBTRACT: subtract(*src1, *src2, *dst, *mask); break;
		case MATH_MULTIPLY: multiply(*src1, *src2, *dst, scale); break;
		case MATH_DIVIDE:   divide(*src1, *src2, *dst, scale); break;
	}
	EXCEPTION_CATCH

	if (newHandle) {
		return initRXHandle(frm, 1, dst, Handle_cvMat);
	}
	else {
		// requested output to given existing array
		RXA_ARG(frm, 1) = RXA_ARG(frm, 4);
		return RXR_VALUE;
	}	
}

COMMAND cmd_bitwise_and(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_AND);
}
COMMAND cmd_bitwise_or(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_AND);
}
COMMAND cmd_bitwise_xor(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_AND);
}
COMMAND cmd_add(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_ADD);
}
COMMAND cmd_subtract(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_SUBTRACT);
}
COMMAND cmd_multiply(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_MULTIPLY);
}
COMMAND cmd_divide(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_DIVIDE);
}

COMMAND cmd_bitwise_not(RXIFRM *frm, void *ctx) {
	Mat *src;
	Mat *dst;
	Mat *mask = NULL;
	bool newHandle = FALSE;

	src = ARG_Mat(1);

	if (ARG_Is_Mat(3)) { // /into handle
		dst = ARG_Mat(3);
	} else {
		newHandle = TRUE;
		dst = new Mat();
	}

	if (ARG_Is_Mat(5)) { // mask
		mask = ARG_Mat(5);
	}

	if (!src || !dst ) return RXR_NONE;
	if (!mask) mask = (Mat*)&noArray();

	EXCEPTION_TRY
	bitwise_not(*src, *dst, *mask);
	EXCEPTION_CATCH

	if (newHandle) {
		return initRXHandle(frm, 1, dst, Handle_cvMat);
	}
	else {
		// requested output to given existing array
		RXA_ARG(frm, 1) = RXA_ARG(frm, 3);
		return RXR_VALUE;
	}
}

COMMAND cmd_convertTo(RXIFRM *frm, void *ctx) {
	Mat *src;
	Mat *dst;

	src = ARG_Mat(1);
	dst = ARG_Mat(2);
	if (!src || !dst) return RXR_NONE;

	int    rtype = ARG_Int(3);
	double alpha = ARG_Double(4);
	double beta  = ARG_Double(5);

	EXCEPTION_TRY
	src->convertTo(*dst, rtype, alpha, beta);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;	
}

// Utility commands:

COMMAND cmd_getTickCount(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_INT64(frm, 1) = getTickCount();
	return RXR_VALUE;
}
COMMAND cmd_getTickFrequency(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_DECIMAL;
	RXA_DEC64(frm, 1) = getTickFrequency();
	return RXR_VALUE;
}
COMMAND cmd_getNumThreads(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_INT64(frm, 1) = getNumThreads();
	return RXR_VALUE;
}
COMMAND cmd_getVersionString(RXIFRM *frm, void *ctx) {
	String version = getVersionString();
	int len = version.length();
	REBSER *ser = (REBSER *)RL_MAKE_STRING(len , FALSE);
	memcpy(ser->data, version.c_str(),  len);
	SERIES_TAIL(ser) = len;
	RXA_TYPE  (frm, 1) = RXT_STRING;
	RXA_SERIES(frm, 1) = ser;
	RXA_INDEX (frm, 1) = 0;
	return RXR_VALUE;
}
COMMAND cmd_getBuildInformation(RXIFRM *frm, void *ctx) {
	String str = getBuildInformation();
	int len = str.length();
	REBSER *ser = (REBSER *)RL_MAKE_STRING(len , FALSE);
	memcpy(ser->data, str.c_str(),  len);
	SERIES_TAIL(ser) = len;
	RXA_TYPE  (frm, 1) = RXT_STRING;
	RXA_SERIES(frm, 1) = ser;
	RXA_INDEX (frm, 1) = 0;
	return RXR_VALUE;
}
COMMAND cmd_useOptimized(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_LOGIC;
	RXA_LOGIC(frm, 1) = useOptimized();
	return RXR_VALUE;
}
COMMAND cmd_setUseOptimized(RXIFRM *frm, void *ctx) {
	setUseOptimized(RXA_LOGIC(frm, 1));
	return RXR_VALUE;
}