#include "opencv2/opencv.hpp"

extern "C" {
	#include "opencv-rebol-extension.h"
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
#define ARG_Is_Trackbar(n)      FRM_IS_HANDLE(n, Handle_cvTrackbar)
#define ARG_Is_Image(n)         (RXA_TYPE(frm,n) == RXT_IMAGE)
#define ARG_Is_Pair(n)          (RXA_TYPE(frm,n) == RXT_PAIR)
#define ARG_Is_Integer(n)       (RXA_TYPE(frm,n) == RXT_INTEGER)
#define ARG_Is_None(n)          (RXA_TYPE(frm,n) == RXT_NONE)
#define ARG_Is_Not_None(n)      (RXA_TYPE(frm,n) != RXT_NONE)
#define ARG_Is_Block(n)         (RXA_TYPE(frm,n) == RXT_BLOCK)
#define ARG_Mat(n)              (ARG_Is_Mat(n) ? (Mat*)(RXA_HANDLE_CONTEXT(frm, n)->handle) : NULL)
#define ARG_VideoCapture(n)     (VideoCapture*)(RXA_HANDLE_CONTEXT(frm, n)->handle)
#define ARG_VideoWriter(n)      (VideoWriter*)(RXA_HANDLE_CONTEXT(frm, n)->handle)
#define ARG_Trackbar(n)         (ARG_Is_Trackbar(n) ? (CTX_TRACKBAR*)(RXA_HANDLE_CONTEXT(frm, n)->handle) : NULL)
#define ARG_MatType(n)          (RXA_TYPE(frm,n) == RXT_INTEGER ? RXA_INT32(frm,n) : RL_FIND_WORD(ext_arg_words, RXA_INT32(frm,n))-W_OPENCV_ARG_CV_8UC1)
#define ARG_Double(n)           (RXA_TYPE(frm,n) == RXT_DECIMAL ? RXA_DEC64(frm,n) : (double)RXA_INT64(frm,n))
#define ARG_Int(n)              (RXA_TYPE(frm,n) == RXT_INTEGER ? RXA_INT32(frm,n) : (int)RXA_DEC64(frm,n))
#define ARG_Size(n)             (RXA_TYPE(frm,n) == RXT_PAIR ? Size(PAIR_X(frm,n), PAIR_Y(frm,n)) : Size(RXA_INT32(frm,n), RXA_INT32(frm,n)));
#define ARG_Point(n)            (RXA_TYPE(frm,n) == RXT_PAIR ? Point(PAIR_X(frm,n), PAIR_Y(frm,n)) : Point(RXA_INT32(frm,n), RXA_INT32(frm,n)));
#define ARG_String(n)           (String((const char*)((REBSER*)RXA_ARG(frm, n).series)->data)) //TODO: only ansii yet!
#define ARG_BorderType(n)       (RXA_TYPE(frm, n) == RXT_INTEGER ? RXA_INT32(frm, n) : BORDER_DEFAULT)

// Special Mat initialization... if Mat at frame n is NULL (arg was none), than it creates a new Mat with same size and type as m at this frame
#define ARG_Mat_As(n,m)         (ARG_Is_Mat(n)\
								? (Mat*)(RXA_HANDLE_CONTEXT(frm, n)->handle)\
								: (m ? initRXHandle(frm,n,new Mat(m->size(),m->type()),Handle_cvMat),(Mat*)(RXA_HANDLE_CONTEXT(frm, n)->handle) : NULL))


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
	extern REBCNT Handle_cvTrackbar;

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
	void* releaseTrackbar(void* cls) {
		debug_print("GC cvTrackbar %p\n", cls);
		if (cls != NULL) {
			CTX_TRACKBAR *bar = (CTX_TRACKBAR*)cls;
			delete bar->name;
			delete bar->window;
		}
		return NULL;
	}
}

static int initRXHandleArg(RXIARG* val, void* handle, REBCNT type) {
	REBHOB* hob = RL_MAKE_HANDLE_CONTEXT(type);
	debug_print("new hob: %p handle: %p\n", hob, handle);
	if (hob == NULL) return RXR_FALSE;
	hob->handle = handle;
	val->handle.ptr = hob;
	val->handle.type = type;
	val->handle.flags = HANDLE_CONTEXT;
	return RXR_VALUE;
}

static int initRXHandle(RXIFRM *frm, int index, void* handle, REBCNT type) {
	RXA_TYPE(frm, index) = RXT_HANDLE;
	return initRXHandleArg(&RXA_ARG(frm, index), handle, type);
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

//;-----------------------------------------------------------------------
//;- Constructors                                                         
//;-----------------------------------------------------------------------

COMMAND cmd_Matrix(RXIFRM *frm, void *ctx) {
	Mat *mat = NULL;
	REBSER *bin = NULL;
	Size size = Size(0,0);
	int type = CV_8UC4;
	
	if (ARG_Is_Block(1)) {
		REBSER *blk = (REBSER *)RXA_SERIES(frm, 1);
		REBCNT n, t;
		RXIARG val;
		for(n = RXA_INDEX(frm, 1); (t = RL_GET_VALUE(blk, n, &val)); n++) {
			if (t == RXT_END) break;
			if (t == RXT_GET_WORD) {
				t = RL_GET_VALUE_RESOLVED(blk, n, &val);
			}
			if (t == RXT_PAIR) {
				size = Size((int)val.pair.x, (int)val.pair.y);
			}
			else if (t == RXT_WORD) {
				type = RL_FIND_WORD(ext_arg_words, val.int32a);
				if (type < W_OPENCV_ARG_CV_8UC1 || type > W_OPENCV_ARG_CV_USRC4) return RXR_FALSE;
				type -= W_OPENCV_ARG_CV_8UC1;
			}
			else if (t == RXT_TUPLE) {
				mat = new Mat(size, type, Scalar(val.bytes[3],val.bytes[2],val.bytes[1],val.bytes[4]));
				goto done;
			}
			else if (t == RXT_BINARY) {
				bin = (REBSER*)val.series;
			}
			// TODO: vector!
		}
		if (bin) {
			// It should be possible to create a matrix, which is using directly the Rebol binary data.
			// https://docs.opencv.org/3.4/d3/d63/classcv_1_1Mat.html#a9fa74fb14362d87cb183453d2441948f
			// mat = new Mat(size, type, bin->data);
			// But first I must resolve, how to guard these data from GC on the Rebol side.		
			// So for now I will rather copy the binary data into the new matrix instead.
			mat = new Mat(size, type);
			int matBytes = mat->elemSize() * mat->cols * mat->rows;
			int binBytes = SERIES_TAIL(bin) - val.index;
			if (binBytes < matBytes) matBytes = binBytes;
			//TODO: if the source data are shorter, they could be copied repeatedly!
			memcpy(mat->data, SERIES_SKIP(bin, val.index), matBytes);
			goto done;
		}
	}
	else if (ARG_Is_Image(1)) {
		mat = new_Mat_From_Image_Arg(frm, 1);
		goto done;
	}
	else if (ARG_Is_Mat(1)) {
		mat = ARG_Mat(1);
		if (!mat) return RXR_NONE;
		size = mat->size();
		type = mat->type();
	}
	else if (ARG_Is_Pair(1)) {
		size = ARG_Size(1);
	}
	mat = new Mat(size, type);
	
done:
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

//;-----------------------------------------------------------------------
//;- Accessors                                                            
//;-----------------------------------------------------------------------

COMMAND cmd_get_property(RXIFRM *frm, void *ctx) {
	int propid = RXA_INT32(frm, 2);
	double result = 0;

	if (!RXA_HANDLE_CONTEXT(frm, 1)) return RXR_FALSE;

	
	if (ARG_Is_VideoCapture(1)) {
		VideoCapture *cap = ARG_VideoCapture(1);
		result = cap->get(propid);
	}
	else if (ARG_Is_Mat(1)) {
		Mat *mat = ARG_Mat(1);
		if (!mat) return RXR_NONE;
		switch(propid){
			case MAT_SIZE: {
				Size size = mat->size();
				RXA_TYPE(frm,1) = RXT_PAIR;
				RXA_PAIR(frm,1).x = size.width;
				RXA_PAIR(frm,1).y = size.height;
				return RXR_VALUE;
			}
			case MAT_TYPE: {
				RXA_TYPE(frm,1) = RXT_WORD;
				RXA_ARG(frm,1).int64 = ext_arg_words[mat->type() + W_OPENCV_ARG_CV_8UC1];
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
				REBINT bits, dims = 1; // TODO: could be dims used to store number of rows?
				REBINT size = mat->cols * mat->rows * mat->channels();
				
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
				memcpy(vec->data, mat->data,  bytes); //TODO: expects that mat is continuous!
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

COMMAND cmd_read(RXIFRM *frm, void *ctx) {
	VideoCapture *cap;
	Mat *frame;

	if(!ARG_Is_VideoCapture(1))
		return RXR_FALSE;

	cap = ARG_VideoCapture(1);
	if (!cap->isOpened()) return RXR_NONE;

	frame = ARG_Mat(3); // destination
	if (!frame) {
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
		Mat *mat = ARG_Mat(2);
		if (!mat) return RXR_FALSE;
		writer->write(*mat);
	} else { // input is Rebol image
		Mat image, mat;
		RXIARG arg = RXA_ARG(frm, 2);
		image = Mat(arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		image.convertTo(mat, CV_8UC3);
		writer->write(mat);
	}
	EXCEPTION_CATCH

	return RXR_TRUE;
}

//;-----------------------------------------------------------------------
//;- Image file reading and writing                                       
//;- https://docs.opencv.org/4.6.0/d4/da8/group__imgcodecs.html           
//;-----------------------------------------------------------------------

COMMAND cmd_imread(RXIFRM *frm, void *ctx) {
	Mat image;
	String filename = ARG_String(1);
	int flags = (ARG_Is_Integer(4) ? RXA_INT32(frm,4) : IMREAD_UNCHANGED);

	EXCEPTION_TRY
	image = imread(filename, flags);
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

COMMAND cmd_imreadmulti(RXIFRM *frm, void *ctx) {
	vector<Mat> images;
	String filename = ARG_String(1);
	int flags = (ARG_Is_Integer(4) ? RXA_INT32(frm,4) : IMREAD_UNCHANGED);

	EXCEPTION_TRY
	if (!imreadmulti(filename, images, flags)) return RXR_FALSE;	
	EXCEPTION_CATCH

	int n;
	int num = images.size();
	REBSER *blk = (REBSER*)RL_MAKE_BLOCK(num);
	RXIARG val;

	if (ARG_Is_None(2)) {
		Mat *image;
		for (n = 0; n < num; n++) {
			image = new Mat(images[n]);
			initRXHandleArg(&val, image, Handle_cvMat);
			RL_SET_VALUE(blk, n, val, RXT_HANDLE);
		}
	} else {
		Mat image;
		REBSER* reb_image;
		for (n = 0; n < num; n++) {
			cvtColor(images[n], image, COLOR_BGR2BGRA);
			reb_image = (REBSER *)RL_MAKE_IMAGE(image.cols, image.rows);
			memcpy(reb_image->data, image.data, 4*image.cols*image.rows);
			val.width  = image.cols;
			val.height = image.rows;
			val.image  = reb_image;
			RL_SET_VALUE(blk, n, val, RXT_IMAGE);
		}
	}
	RXA_TYPE(frm, 1) = RXT_BLOCK;
	RXA_SERIES(frm, 1) = blk;
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

//;-----------------------------------------------------------------------
//;- Geometric Image Transformations                                      
//;- https://docs.opencv.org/4.6.0/da/d54/group__imgproc__transform.html  
//;-----------------------------------------------------------------------

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


//;-----------------------------------------------------------------------
//;- Image Filtering                                                      
//;- https://docs.opencv.org/4.6.0/d4/d86/group__imgproc__filter.html     
//;-----------------------------------------------------------------------

COMMAND cmd_bilateralFilter(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	int d             = ARG_Int(3);
	double sigmaColor = ARG_Double(4);
	double sigmaSpace = ARG_Double(5);
	int borderType    = ARG_BorderType(7);

	if(!src || !dst) return RXR_FALSE;

	EXCEPTION_TRY
	bilateralFilter(*src, *dst, d, sigmaColor, sigmaSpace, borderType);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_blur(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	Size ksize        = ARG_Size(3);
	Point anchor      = Point(-1, -1); // TODO!!
	int borderType    = ARG_BorderType(5);

	if(!src || !dst) return RXR_FALSE;

	EXCEPTION_TRY
	blur(*src, *dst, ksize, anchor, borderType);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_dilate(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	Mat *kernel       = ARG_Mat(3);
	Point anchor      = ARG_Point(4);
	int iterations    = ARG_Int(5);

	if(!src || !dst || !kernel) return RXR_FALSE;

	EXCEPTION_TRY
	dilate(*src, *dst, *kernel, anchor, iterations);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_GaussianBlur(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	Size ksize        = ARG_Size(3);
	double sigmaX     = ARG_Double(4);
	double sigmaY     = ARG_Double(5);
	int borderType    = ARG_BorderType(7);
	
	if (!dst || !src) return RXR_FALSE;

	EXCEPTION_TRY
	GaussianBlur(*src, *dst, ksize, sigmaX, sigmaY, borderType);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_getStructuringElement(RXIFRM *frm, void *ctx) {
	int shape     = ARG_Int(1);
	Size ksize    = ARG_Size(2);
	Point anchor  = ARG_Point(3);
	Mat *dst      = new Mat();

	EXCEPTION_TRY
	*dst = getStructuringElement(shape, ksize, anchor);
	EXCEPTION_CATCH

	return initRXHandle(frm, 1, dst, Handle_cvMat);
}

COMMAND cmd_Laplacian(RXIFRM *frm, void *ctx) {
	Mat *src       = ARG_Mat(1);
	Mat *dst       = ARG_Mat_As(2, src);
	int ddepth     = ARG_Int(3);
	int ksize      = ARG_Int(4);
	double scale   = ARG_Double(5);
	double delta   = ARG_Double(6);
	int borderType = BORDER_DEFAULT;
	
	if (!dst || !src) return RXR_FALSE;

	EXCEPTION_TRY
	Laplacian(*src, *dst, ddepth, ksize, scale, delta, borderType);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_medianBlur(RXIFRM *frm, void *ctx) {
	Mat *src       = ARG_Mat(1);
	Mat *dst       = ARG_Mat_As(2, src);
	int ksize      = ARG_Int(3);
	
	if (!dst || !src) return RXR_FALSE;

	EXCEPTION_TRY
	medianBlur(*src, *dst, ksize);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}


//;-----------------------------------------------------------------------
//;- Color Space Conversions                                              
//;- https://docs.opencv.org/4.6.0/d8/d01/group__imgproc__color__conversions.html
//;-----------------------------------------------------------------------

COMMAND cmd_cvtColor(RXIFRM *frm, void *ctx) {
	Mat *src       = ARG_Mat(1);
	Mat *dst       = ARG_Mat_As(2, src);
	int code       = ARG_Int(3);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	cvtColor(*src, *dst, code);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

//;-----------------------------------------------------------------------------------------------
//;- Color Maps                                                                                   
//;- https://docs.opencv.org/4.6.0/d3/d50/group__imgproc__colormap.html                           
//;-----------------------------------------------------------------------------------------------

COMMAND cmd_applyColorMap(RXIFRM *frm, void *ctx) {
	Mat *src       = ARG_Mat(1);
	Mat *dst       = ARG_Mat_As(2, src);
	int colormap   = ARG_Int(3);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	applyColorMap(*src, *dst, colormap);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

//;-----------------------------------------------------------------------------------------------
//;- Feature Detection                                                                            
//;- https://docs.opencv.org/4.6.0/dd/d1a/group__imgproc__feature.html                            
//;-----------------------------------------------------------------------------------------------

COMMAND cmd_Canny(RXIFRM *frm, void *ctx) {
	Mat *src       = ARG_Mat(1);
	Mat *dst       = ARG_Mat_As(2, src);
	double threshold1 = ARG_Double(3);
	double threshold2 = ARG_Double(4);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	Canny(*src, *dst, threshold1, threshold2);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}


//;-----------------------------------------------------------------------
//;- Image Thresholding                                                   
//;- https://docs.opencv.org/4.x/d7/d4d/tutorial_py_thresholding.html     
//;-----------------------------------------------------------------------

COMMAND cmd_threshold(RXIFRM *frm, void *ctx) {
	Mat *src = ARG_Mat(1);
	Mat *dst = ARG_Mat_As(2, src);
	double thresh = ARG_Double(3);
	double maxval = ARG_Double(4);
	int    type   = ARG_Int(5);
	double result;

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	result = threshold(*src, *dst, thresh, maxval, type);
	EXCEPTION_CATCH

	if (type == THRESH_OTSU || type == THRESH_TRIANGLE){
		RXA_TYPE(frm, 1) = RXT_DECIMAL;
		RXA_DEC64(frm, 1) = result;
	} else {
		RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	}
	return RXR_VALUE;
}


//;-----------------------------------------------------------------------
//;- Operations on arrays                                                 
//;- https://docs.opencv.org/4.6.0/d2/de8/group__core__array.html         
//;-----------------------------------------------------------------------

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
	Mat *src1 = ARG_Mat(1);
	Mat *src2 = ARG_Mat(2);
	Mat *dst  = ARG_Mat_As(3, src1);
	Mat *mask = NULL;
	bool newHandle = FALSE;
	double scale = 1;

	if (op < 10) {
		mask = ARG_Mat(5);
		if(!mask)
			mask = (Mat*)&noArray();
	} else {
		// multiply and divide don't have mask, but scale instead
		if (!ARG_Is_None(4)) scale = ARG_Double(5);
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

	RXA_ARG(frm, 1) = RXA_ARG(frm, 3);
	return RXR_VALUE;
}

COMMAND cmd_bitwise_and(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_AND);
}
COMMAND cmd_bitwise_or(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_OR);
}
COMMAND cmd_bitwise_xor(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_XOR);
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
	Mat *src  = ARG_Mat(1);
	Mat *dst  = ARG_Mat_As(2, src);
	Mat *mask = ARG_Mat(3);

	if (!src || !dst ) return RXR_NONE;

	EXCEPTION_TRY
	if (mask) bitwise_not(*src, *dst, *mask);
	else      bitwise_not(*src, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_absdiff(RXIFRM *frm, void *ctx) {
	//TODO: should work also with Scalar (Rebol's tuple)
	Mat *src1    = ARG_Mat(1);
	Mat *src2    = ARG_Mat(2);
	Mat *dst     = ARG_Mat_As(3, src1);

	if(!src1 || !src2 || !dst) return RXR_NONE;

	EXCEPTION_TRY
	absdiff(*src1, *src2, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 6);
	return RXR_VALUE;
}

COMMAND cmd_addWeighted(RXIFRM *frm, void *ctx) {
	Mat *src1    = ARG_Mat(1);
	double alpha = ARG_Double(2);
	Mat *src2    = ARG_Mat(3);
	double beta  = ARG_Double(4);
	double delta = ARG_Double(5); 
	Mat *dst     = ARG_Mat_As(6, src1);

	if(!src1 || !src2 || !dst) return RXR_NONE;

	EXCEPTION_TRY
	addWeighted(*src1, alpha, *src2, beta, delta, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 6);
	return RXR_VALUE;
}

COMMAND cmd_convertScaleAbs(RXIFRM *frm, void *ctx) {
	Mat *src     = ARG_Mat   (1);
	Mat *dst     = ARG_Mat_As(2, src);
	double alpha = ARG_Double(3);
	double beta  = ARG_Double(4);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	convertScaleAbs(*src, *dst, alpha, beta);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;	
}

COMMAND cmd_flip(RXIFRM *frm, void *ctx) {
	Mat *src     = ARG_Mat   (1);
	Mat *dst     = ARG_Mat_As(2, src);
	int flipCode = ARG_Int(3);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	flip(*src, *dst, flipCode);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;	
}

COMMAND cmd_invert(RXIFRM *frm, void *ctx) {
	Mat *src     = ARG_Mat   (1);
	Mat *dst     = ARG_Mat_As(2, src);
	int flipCode = ARG_Int(3);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	invert(*src, *dst, flipCode);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;	
}

COMMAND cmd_normalize(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	double alpha      = ARG_Double(3);
	double beta       = ARG_Double(4);
	int norm_type     = ARG_Int(5);
	Mat *mask         = ARG_Mat(7);

	if(!src || !dst) return RXR_FALSE;

	EXCEPTION_TRY
	if (mask)
		normalize(*src, *dst, alpha, beta, norm_type, dst->type(), *mask);
	else
		normalize(*src, *dst, alpha, beta, norm_type, dst->type());
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_transform(RXIFRM *frm, void *ctx) {
	Mat *src     = ARG_Mat   (1);
	Mat *dst     = ARG_Mat_As(2, src);
	Mat *m       = ARG_Mat   (3);

	if (!src || !dst || !m) return RXR_NONE;

	EXCEPTION_TRY
	transform(*src, *dst, *m);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;	
}


//;-----------------------------------------------------------------------
//;- Mat class                                                            
//;- https://docs.opencv.org/4.6.0/d3/d63/classcv_1_1Mat.html             
//;-----------------------------------------------------------------------

COMMAND cmd_convertTo(RXIFRM *frm, void *ctx) {
	Mat *src     = ARG_Mat   (1);
	Mat *dst     = ARG_Mat_As(2, src);
	int    rtype = ARG_MatType(3);
	double alpha = ARG_Double(4);
	double beta  = ARG_Double(5);

	if (!src || !dst) return RXR_NONE;

	EXCEPTION_TRY
	src->convertTo(*dst, rtype, alpha, beta);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;	
}


//;-----------------------------------------------------------------------
//;- High-level GUI                                                       
//;- https://docs.opencv.org/4.6.0/d7/dfc/group__highgui.html             
//;-----------------------------------------------------------------------

COMMAND cmd_startWindowThread(RXIFRM *frm, void *ctx) {
	startWindowThread();
	return RXR_UNSET;
}

COMMAND cmd_imshow(RXIFRM *frm, void *ctx) {
	Mat *image  = ARG_Mat(1);
	// check if name was provided or use default
	String name = (RXA_TYPE(frm, 3) == RXT_NONE) ? "Image" : ARG_String(3);

	if (image) {
		Size size = image->size();
		if (size.width == 0 || size.height == 0) return RXR_FALSE;
		EXCEPTION_TRY
		imshow(name, *image);
		EXCEPTION_CATCH
	} else if(ARG_Is_Image(1)) { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat( arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		imshow(name, image);
	} else {
		return RXR_FALSE;
	}
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
	pollKey(); // OpenCV bug on macOS... window would not resize without it!
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

COMMAND cmd_setWindowTitle(RXIFRM *frm, void *ctx) {
	setWindowTitle(ARG_String(1), ARG_String(2));
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


static void callbackTrackbar(int pos, void* userdata) {
	REBHOB* hob = (REBHOB*)userdata;
	CTX_TRACKBAR* bar = (CTX_TRACKBAR*)hob->data;
	//cout << *bar->name << ": " << pos << endl;
}

COMMAND cmd_createTrackbar(RXIFRM *frm, void *ctx) {
	String name   = ARG_String(1);
	String window = ARG_String(2);
	int count     = ARG_Int(3);
	
	REBHOB* hob = RL_MAKE_HANDLE_CONTEXT(Handle_cvTrackbar);
	CTX_TRACKBAR* bar = (CTX_TRACKBAR*)hob->data;
	bar->name   = new String(name);
	bar->window = new String(window);

	createTrackbar(name, window, &bar->value, count, callbackTrackbar, hob);

	RXA_HANDLE(frm, 1) = hob;
	RXA_HANDLE_TYPE(frm, 1) = hob->sym;
	RXA_HANDLE_FLAGS(frm, 1) = hob->flags;
	RXA_TYPE(frm, 1) = RXT_HANDLE;
	return RXR_VALUE;
}

COMMAND cmd_setTrackbarMax(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	setTrackbarMax(*bar->name, *bar->window, ARG_Int(2));
	return RXR_VALUE;
}
COMMAND cmd_setTrackbarMin(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	setTrackbarMin(*bar->name, *bar->window, ARG_Int(2));
	return RXR_VALUE;
}
COMMAND cmd_setTrackbarPos(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	setTrackbarPos(*bar->name, *bar->window, ARG_Int(2));
	return RXR_VALUE;
}
COMMAND cmd_getTrackbarPos(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_ARG(frm, 1).int64 = getTrackbarPos(*bar->name, *bar->window);
	return RXR_VALUE;
}

//;-----------------------------------------------------------------------
//;- Utilities                                                            
//;-----------------------------------------------------------------------

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
