//
// Project: Rebol/OpenCV extension
// SPDX-License-Identifier: MIT
// =============================================================================
// Command implementations and handle callbacks.
//
// One function per command; the enum, the declarations, the dispatch table
// and the `_init` handler are all generated from opencv.reb.
//

#include "opencv2/opencv.hpp"
#include "opencv.h"

static const char* ERR_MAT_TYPE   = "Invalid matrix type!";
static const char* ERR_MAT_SPEC   = "Invalid matrix specification!";
static const char* ERR_MAT_SIZE   = "Invalid or missing matrix size!";
static const char* ERR_MAT_VECTOR = "Vector does not match the matrix size or type!";
static const char* ERR_MAT_SHORT  = "Data series is too short for the matrix!";
static const char* ERR_INVALID_MAT = "Invalid or released cvMat handle!";

static char err_buff[255]; // temporary buffer used to pass an exception messages to Rebol side

#define EXCEPTION_TRY   try {
#define EXCEPTION_CATCH } catch( cv::Exception& e ) {\
	snprintf(err_buff, sizeof(err_buff), "%s", e.err.c_str());\
	RETURN_ERROR(err_buff);}
	// must use the temporary buffer above, because the error String could be freed before used on the Rebol side

#define HOB_LIVE(hob) ((hob) && IS_USED_HOB(hob) && (hob)->data)

#define PAIR_X(frm, n)    (int)RXA_PAIR(frm,n).x
#define PAIR_Y(frm, n)    (int)RXA_PAIR(frm,n).y

#define ARG_Is_Mat(n)           ARG_Is_Live_Handle(n, Handle_cvMat)
#define ARG_Is_VideoCapture(n)  ARG_Is_Live_Handle(n, Handle_cvVideoCapture)
#define ARG_Is_VideoWriter(n)   ARG_Is_Live_Handle(n, Handle_cvVideoWriter)
#define ARG_Is_Trackbar(n)      ARG_Is_Live_Handle(n, Handle_cvTrackbar)
#define ARG_Is_Image(n)         (RXA_TYPE(frm,n) == RXT_IMAGE)
#define ARG_Is_Pair(n)          (RXA_TYPE(frm,n) == RXT_PAIR)
#define ARG_Is_Integer(n)       (RXA_TYPE(frm,n) == RXT_INTEGER)
#define ARG_Is_None(n)          (RXA_TYPE(frm,n) == RXT_NONE)
#define ARG_Is_Not_None(n)      (RXA_TYPE(frm,n) != RXT_NONE)
#define ARG_Is_Block(n)         (RXA_TYPE(frm,n) == RXT_BLOCK)
#define ARG_Is_File(n)          (RXA_TYPE(frm,n) == RXT_FILE)
#define ARG_Mat(n)              (ARG_Is_Mat(n) ? (Mat*)HOB_PTR(RXA_HANDLE_CONTEXT(frm, n)) : NULL)
#define ARG_VideoCapture(n)     ((VideoCapture*)HOB_PTR(RXA_HANDLE_CONTEXT(frm, n)))
#define ARG_VideoWriter(n)      ((VideoWriter*)HOB_PTR(RXA_HANDLE_CONTEXT(frm, n)))
#define ARG_Trackbar(n)         (ARG_Is_Trackbar(n) ? (CTX_TRACKBAR*)(RXA_HANDLE_CONTEXT(frm, n)->data) : NULL)
#define ARG_MatType(n)          (RXA_TYPE(frm,n) == RXT_INTEGER ? RXA_INT32(frm,n) : cvTypeFromWord(RXA_WORD(frm,n)))
// reb-ext-common.h's ARG_Double reads a decimal only; this one also takes
// an integer!, as the `number!` arguments in the specs allow.
#undef  ARG_Double
#define ARG_Double(n)           (RXA_TYPE(frm,n) == RXT_DECIMAL ? RXA_DEC64(frm,n) : (double)RXA_INT64(frm,n))
#define ARG_Int(n)              (RXA_TYPE(frm,n) == RXT_INTEGER ? RXA_INT32(frm,n) : (int)RXA_DEC64(frm,n))
#define ARG_Size(n)             (RXA_TYPE(frm,n) == RXT_PAIR ? Size(PAIR_X(frm,n), PAIR_Y(frm,n)) : Size(RXA_INT32(frm,n), RXA_INT32(frm,n)))
#define ARG_Point(n)            (RXA_TYPE(frm,n) == RXT_PAIR ? Point(PAIR_X(frm,n), PAIR_Y(frm,n)) : Point(RXA_INT32(frm,n), RXA_INT32(frm,n)))
#define ARG_String(n)           (String((const char*)((REBSER*)RXA_ARG(frm, n).series)->data)) //TODO: only ansii yet!
#define ARG_BorderType(n)       (RXA_TYPE(frm, n) == RXT_INTEGER ? RXA_INT32(frm, n) : BORDER_DEFAULT)

// Special Mat initialization... if Mat at frame n is NULL (arg was none), than it creates a new Mat with same size and type as m at this frame
#define ARG_Mat_As(n,m)         (ARG_Is_Mat(n)\
								? (Mat*)HOB_PTR(RXA_HANDLE_CONTEXT(frm, n))\
								: (m ? initRXHandle(frm,n,new Mat(m->size(),m->type()),Handle_cvMat, NULL),(Mat*)HOB_PTR(RXA_HANDLE_CONTEXT(frm, n)) : NULL))

enum MatProperties {
	MAT_SIZE = 1,
	MAT_TYPE,
	MAT_DEPTH,
	MAT_CHANNELS,
	MAT_BINARY,
	MAT_IMAGE,
	MAT_VECTOR,
};

// The `type` word list in opencv.reb holds 8 depths x 4 channel counts, in
// the order `depth + (channels - 1) * 8` - which is the OpenCV 4 type id.
// OpenCV 5 widened the channel shift from 3 to 5 bits (CV_CN_SHIFT), so a
// type id is NOT that index any more: always convert through these helpers.
#define TYPE_WORD_DEPTHS 8   // CV_8U .. CV_16F

// Word -> CV type, or -1 when the word is not a type word.
static int cvTypeFromWord(REBCNT word) {
	int idx = (int)RL_FIND_WORD(Opencv_type_words, word) - W_OPENCV_TYPE_CV_8UC1;
	if (idx < 0 || idx >= 4 * TYPE_WORD_DEPTHS) return -1;
	return CV_MAKETYPE(idx % TYPE_WORD_DEPTHS, idx / TYPE_WORD_DEPTHS + 1);
}

// CV type -> word id, or 0 for a type the word list cannot name.
static REBCNT typeWordFromCv(int type) {
	int depth = CV_MAT_DEPTH(type);
	int cn    = CV_MAT_CN(type);
	if (depth >= TYPE_WORD_DEPTHS || cn < 1 || cn > 4) return 0;
	return Opencv_type_words[W_OPENCV_TYPE_CV_8UC1 + depth + (cn - 1) * TYPE_WORD_DEPTHS];
}

// CV depth -> word id, or 0.
static REBCNT depthWordFromCv(int depth) {
	if (depth < 0 || depth >= TYPE_WORD_DEPTHS) return 0;
	return Opencv_type_words[W_OPENCV_TYPE_CV_8U + depth];
}

// TRUE for a type the extension can build a matrix of.
#define IS_KNOWN_CV_TYPE(t) ((t) >= 0 && CV_MAT_DEPTH(t) < TYPE_WORD_DEPTHS && CV_MAT_CN(t) <= 4)

using namespace cv;
using namespace std;

// Creates a context handle of the given type holding `handle` and stores it
// in `val`. The pointer goes INTO the data block allocated by Rebol.
static int initRXHandleArg(RXIARG* val, void* handle, REBCNT type, REBSER* ser) {
	REBHOB* hob = RL_MAKE_HANDLE_CONTEXT(type);
	if (hob == NULL) return RXR_FALSE;
	debug_print("new hob: %p handle: %p\n", (void*)hob, handle);
	HOB_PTR(hob) = handle;
	hob->series = ser;
	val->handle.ptr   = hob;
	val->handle.type  = type;
	val->handle.flags = hob->flags;
	return RXR_VALUE;
}

static int initRXHandle(RXIFRM *frm, int index, void* handle, REBCNT type, REBSER* ser) {
	RXA_TYPE(frm, index) = RXT_HANDLE;
	return initRXHandleArg(&RXA_ARG(frm, index), handle, type, ser);
}

static Mat* new_Mat_From_Image_Arg(RXIFRM *frm, int index) {
	RXIARG arg = RXA_ARG(frm, index);
	Mat *mat = new Mat(arg.height, arg.width, CV_8UC4);
	mat->data = ((REBSER*)arg.series)->data;
	return mat;
}

// Copies the matrix elements, row by row when not continuous.
static size_t copy_mat_data(Mat *mat, REBYTE *dst) {
	size_t elemSize = mat->elemSize();
	size_t bytes = elemSize * mat->cols * mat->rows;

	if (mat->isContinuous()) {
		memcpy(dst, mat->data, bytes);
	} else {
		unsigned char *bp = mat->data;
		size_t wb = elemSize * mat->cols;
		size_t st = mat->step;
		for (int j = 0; j < mat->rows; j++) {
			memcpy(dst, bp, wb);
			bp  += st;
			dst += wb;
		}
	}
	return bytes;
}

// For byte series (binary and image), whose tail the caller adjusts.
static void mat2ser(Mat *mat, REBSER *bin, RXIARG *arg) {
	SERIES_TAIL(bin) = (REBLEN)copy_mat_data(mat, bin->data);
	arg->series = bin;
	arg->index = 0;
}

// Makes a new Rebol vector! with a copy of the matrix elements.
// Returns FALSE for a depth without a vector equivalent (CV_16F).
static REBOOL mat2vector(Mat *mat, RXIARG *arg) {
	REBCNT vt;
	switch (mat->depth()) {
		case CV_8U:  vt = VTUI08; break;
		case CV_8S:  vt = VTSI08; break;
		case CV_16U: vt = VTUI16; break;
		case CV_16S: vt = VTSI16; break;
		case CV_32S: vt = VTSI32; break;
		case CV_32F: vt = VTSF32; break;
		case CV_64F: vt = VTSF64; break;
		default: return FALSE;
	}
	if (!RL_MAKE_VECTOR(arg, vt, (REBINT)(mat->total() * mat->channels()), 1)) return FALSE;
	copy_mat_data(mat, ((REBSER*)arg->vector.series)->data);
	return TRUE;
}

extern "C" {

	int Common_mold(REBHOB *hob, REBSER *str) {
		int len;
		if (!str) return 0;
		SERIES_TAIL(str) = 0;
		APPEND_STRING(str, "0#%lx", (unsigned long)(uintptr_t)(HOB_LIVE(hob) ? HOB_PTR(hob) : NULL));
		return len;
	}

	int cvMat_free(void* hndl) {
		REBHOB *hob = (REBHOB*)hndl;
		if (!hob || !hob->data) return 0;
		Mat *mat = (Mat*)HOB_PTR(hob);
		debug_print("GC Mat class %p\n", (void*)mat);
		if (mat) {
			delete mat;
			HOB_PTR(hob) = NULL;
		}
		UNMARK_HOB(hob);
		return 0;
	}

	int cvMat_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
		Size size;
		Mat tmp;
		if (!HOB_LIVE(hob)) return PE_NONE;
		Mat *mat = (Mat*)HOB_PTR(hob);
		if (!mat) return PE_NONE;
		REBSER* ser = hob->series;
		REBSER* bin;
		int channels;

		word = RL_FIND_WORD(Opencv_arg_words, word);

		switch(word){
		case W_OPENCV_ARG_SIZE:
			*type = RXT_PAIR;
			size = mat->size();
			arg->dec32a = size.width;
			arg->dec32b = size.height;
			break;
		case W_OPENCV_ARG_WIDTH:
		case W_OPENCV_ARG_HEIGHT:
			*type = RXT_INTEGER;
			size = mat->size();
			arg->uint64 = (word == W_OPENCV_ARG_WIDTH) ? size.width : size.height;
			break;
		case W_OPENCV_ARG_TYPE:
			arg->int64 = typeWordFromCv(mat->type());
			*type = arg->int64 ? RXT_WORD : RXT_NONE;
			break;
		case W_OPENCV_ARG_DEPTH:
			arg->int64 = depthWordFromCv(mat->depth());
			*type = arg->int64 ? RXT_WORD : RXT_NONE;
			break;
		case W_OPENCV_ARG_CHANNELS:
			*type = RXT_INTEGER;
			arg->uint64 = mat->channels();
			break;
		case W_OPENCV_ARG_BINARY:
			*type = RXT_BINARY;
			if (ser) {
				arg->series = ser;
				arg->index = 0;
			} else {
				bin = (REBSER *)RL_MAKE_STRING(mat->elemSize() * mat->cols * mat->rows, FALSE);
				mat2ser(mat, bin, arg);
			}
			break;
		case W_OPENCV_ARG_IMAGE:
			*type = RXT_IMAGE;
			channels = mat->channels();
			if (channels == 1) {
				cvtColor(*mat, tmp, COLOR_GRAY2BGRA);
			}
			else if (channels == 3 || channels == 4) {
				cvtColor(*mat, tmp, COLOR_BGR2BGRA);
			}
			else {
				*type = RXT_NONE;
				break;
			}
			bin = (REBSER *)RL_MAKE_IMAGE(tmp.cols, tmp.rows);
			mat2ser(&tmp, bin, arg);
			// The tail in an image value is not in bytes, but in pixels!
			SERIES_TAIL(bin) = tmp.cols * tmp.rows;
			arg->width  = tmp.cols;
			arg->height = tmp.rows;
			arg->image_index = 0;
			break;
		case W_OPENCV_ARG_VECTOR:
			// Always a copy: a shared buffer is a byte series with no
			// element type, which is not a vector.
			*type = mat2vector(mat, arg) ? RXT_VECTOR : RXT_NONE;
			break;
		case W_OPENCV_ARG_TOTAL:
			*type = RXT_INTEGER;
			arg->uint64 = mat->total();
			break;
		case W_OPENCV_ARG_IS_SUBMATRIX:
			*type = RXT_LOGIC;
			arg->int32a = mat->isSubmatrix();
			break;
		default:
			return PE_BAD_SELECT;
		}
		return PE_USE;
	}

	int cvVideoCapture_free(void* hndl) {
		REBHOB *hob = (REBHOB*)hndl;
		if (!hob || !hob->data) return 0;
		VideoCapture *cap = (VideoCapture*)HOB_PTR(hob);
		debug_print("GC VideoCapture class %p\n", (void*)cap);
		if (cap) {
			cap->release();
			delete cap;
			HOB_PTR(hob) = NULL;
		}
		UNMARK_HOB(hob);
		return 0;
	}

	int cvVideoCapture_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
		if (!HOB_LIVE(hob)) return PE_NONE;
		VideoCapture *cap = (VideoCapture*)HOB_PTR(hob);
		if (!cap) return PE_NONE;

		// Each field names its CAP_PROP_* explicitly. The word ids come from
		// the order of the `handles:` fields in the spec (shared with cvMat),
		// so they cannot be turned into property ids by arithmetic.
		switch (RL_FIND_WORD(Opencv_arg_words, word)) {
		case W_OPENCV_ARG_POS_MS:
			*type = RXT_DECIMAL;
			arg->dec64 = cap->get(CAP_PROP_POS_MSEC);
			break;
		case W_OPENCV_ARG_POS_FRAME:
			*type = RXT_DECIMAL;
			arg->dec64 = cap->get(CAP_PROP_POS_FRAMES);
			break;
		case W_OPENCV_ARG_POS_RATIO:
			*type = RXT_DECIMAL;
			arg->dec64 = cap->get(CAP_PROP_POS_AVI_RATIO);
			break;
		case W_OPENCV_ARG_WIDTH:
			*type = RXT_INTEGER;
			arg->int64 = (i64)cap->get(CAP_PROP_FRAME_WIDTH);
			break;
		case W_OPENCV_ARG_HEIGHT:
			*type = RXT_INTEGER;
			arg->int64 = (i64)cap->get(CAP_PROP_FRAME_HEIGHT);
			break;
		case W_OPENCV_ARG_FPS:
			*type = RXT_DECIMAL;
			arg->dec64 = cap->get(CAP_PROP_FPS);
			break;
		case W_OPENCV_ARG_FOURCC:
			*type = RXT_DECIMAL;
			arg->dec64 = cap->get(CAP_PROP_FOURCC);
			break;
		case W_OPENCV_ARG_FRAMES:
			*type = RXT_INTEGER;
			arg->int64 = (i64)cap->get(CAP_PROP_FRAME_COUNT);
			break;
		case W_OPENCV_ARG_FORMAT:
			arg->int64 = typeWordFromCv((int)cap->get(CAP_PROP_FORMAT));
			*type = arg->int64 ? RXT_WORD : RXT_NONE;
			break;
		default:
			return PE_BAD_SELECT;
		}
		return PE_USE;
	}

	int cvVideoWriter_free(void* hndl) {
		REBHOB *hob = (REBHOB*)hndl;
		if (!hob || !hob->data) return 0;
		VideoWriter *writer = (VideoWriter*)HOB_PTR(hob);
		debug_print("GC VideoWriter class %p\n", (void*)writer);
		if (writer) {
			writer->release();
			delete writer;
			HOB_PTR(hob) = NULL;
		}
		UNMARK_HOB(hob);
		return 0;
	}

	int cvTrackbar_free(void* hndl) {
		REBHOB *hob = (REBHOB*)hndl;
		if (!hob || !hob->data) return 0;
		CTX_TRACKBAR *bar = (CTX_TRACKBAR*)hob->data;
		debug_print("GC cvTrackbar %p\n", (void*)bar);
		delete bar->name;
		delete bar->window;
		if (bar->cbi)  FREE_MEM(bar->cbi);
		if (bar->args) FREE_MEM(bar->args);
		CLEARS(bar);
		UNMARK_HOB(hob);
		return 0;
	}

	int cvMouseCallback_free(void* hndl) {
		REBHOB *hob = (REBHOB*)hndl;
		if (!hob || !hob->data) return 0;
		CTX_MOUSECALLBACK *mcb = (CTX_MOUSECALLBACK*)hob->data;
		debug_print("GC MouseCallback class %p\n", (void*)mcb);
		// It's not possible to remove the callback,
		// because it could remove another one, which is already used.
		// OpenCV api does not provide any API to get existing callback for a test.
		delete mcb->window;
		if (mcb->cbi)  FREE_MEM(mcb->cbi);
		if (mcb->args) FREE_MEM(mcb->args);
		CLEARS(mcb);
		UNMARK_HOB(hob);
		return 0;
	}

	int cvMouseCallback_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
		if (!HOB_LIVE(hob)) return PE_NONE;
		CTX_MOUSECALLBACK *mcb = (CTX_MOUSECALLBACK*)hob->data;
		if (!mcb) return PE_NONE;

		word = RL_FIND_WORD(Opencv_arg_words, word);
		switch (word) {
		case W_OPENCV_ARG_X:
			*type = RXT_INTEGER;
			arg->int64 = mcb->x;
			break;
		case W_OPENCV_ARG_Y:
			*type = RXT_INTEGER;
			arg->int64 = mcb->y;
			break;
		case W_OPENCV_ARG_POS:
			*type = RXT_PAIR;
			arg->dec32a = (float)mcb->x;
			arg->dec32b = (float)mcb->y;
			break;
		case W_OPENCV_ARG_FLAGS:
			*type = RXT_INTEGER;
			arg->int64 = mcb->flags;
			break;
		default:
			return PE_BAD_SELECT;
		}
		return PE_USE;
	}
}


COMMAND cmd_opencv_test(RXIFRM *frm, void *ctx) {
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

static int vecType2cvType[12] = {
	CV_8S,  //	VTSI08
	CV_16S, //	VTSI16
	CV_32S, //	VTSI32
	-1,     //	VTSI64
	CV_8U,  //	VTUI08
	CV_16U, //	VTUI16
	-1,     //	VTUI32
	-1,     //	VTUI64
	-1,     //	VTSF08
	-1,     //	VTSF16
	CV_32F, //	VTSF32
	CV_64F  //	VTSF64
};

COMMAND cmd_opencv_Matrix(RXIFRM *frm, void *ctx) {
	Mat *mat = NULL;
	REBSER *bin = NULL;
	REBSER *blk;
	REBCNT n, t;
	REBOOL resolved;
	REBOOL sharedBin = FALSE;
	RXIARG val;
	Size size = Size(0,0);
	int type = CV_8UC4;
	int binBytes = 0, matBytes = 0;
	REBCNT binIndex = 0; // `val` is reused by the loop, so keep the data position
	
	if (ARG_Is_Block(1)) {
		blk = (REBSER *)RXA_SERIES(frm, 1);

		for(n = RXA_INDEX(frm, 1); (t = RL_GET_VALUE(blk, n, &val)); n++) {
			if (t == RXT_END) break;
			if (t == RXT_GET_WORD || t == RXT_GET_PATH) {
				resolved = TRUE;
				t = RL_GET_VALUE_RESOLVED(blk, n, &val);
			} else {
				resolved = FALSE;
			}
			if (t == RXT_PAIR) {
				size = Size((int)val.pair.x, (int)val.pair.y);
			}
			else if (t == RXT_WORD) {
				type = cvTypeFromWord(val.int32a);
				if (type < 0) goto err_type;
			}
			else if (t == RXT_TUPLE) {
				if (RXT_END != RL_GET_VALUE(blk, ++n, &val)) goto err_spec;
				if (size.width <= 0 || size.height <= 0) goto err_size;
				mat = new Mat(size, type, Scalar(val.bytes[3],val.bytes[2],val.bytes[1],val.bytes[4]));
				goto done;
			}
			else if (t == RXT_BINARY) {
				bin = (REBSER*)val.series;
				binIndex = val.index;
				binBytes = SERIES_TAIL(bin) - val.index;
				sharedBin = resolved; // use shared series only if argument was passed by word or path
			}
			else if (t == RXT_VECTOR) {
				// Current Rebol's vector has only one dimension,
				// so there must be used size of the resulting matrix!
				// The CV type is computed from the vector type and sizes.
				bin = (REBSER*)val.series;
				if (size.width <= 0 || size.height <= 0) goto err_size;
				// The element type travels in the value's info since ABI 1.
				int vecType = (int)RXI_VECTOR_TYPE(val.vector.info);
				if (vecType < 0 || vecType > VTSF64 || (type = vecType2cvType[vecType]) < 0) goto err_vect;

				int depth = (bin->tail - val.index) / (size.width * size.height);
				binIndex = val.index;
				binBytes = (SERIES_TAIL(bin) - val.index) * (RXI_VECTOR_BITS(val.vector.info) / 8);
				// `depth` here is the number of channels per element
				if (depth < 1 || depth > 4) goto err_vect;
				type = CV_MAKETYPE(type, depth);
				sharedBin = resolved; // use shared series only if argument was passed by word or path
			}
			else if (t == RXT_HANDLE) {
				Mat *src;
				REBSER *rect;
				RXIARG pos, sz;

				if (val.handle.type != Handle_cvMat || !IS_USED_HOB(val.handle.hob)) {
					trace("Expected cvMat handle.");
					goto err_spec;
				}
				src = (Mat*)HOB_PTR(val.handle.hob);
				if (!src) {
					trace("Invalid matrix argument.");
					goto err_spec;
				}
				// the rectangle block follows the handle
				t = RL_GET_VALUE(blk, ++n, &val);
				if (t == RXT_GET_WORD || t == RXT_GET_PATH) {
					t = RL_GET_VALUE_RESOLVED(blk, n, &val);
				}
				if (t != RXT_BLOCK) {
					trace("Expected block!");
					goto err_spec;
				}
				rect = (REBSER*)val.series;
				if (RXT_PAIR != RL_GET_VALUE(rect, val.index, &pos) || RXT_PAIR != RL_GET_VALUE(rect, val.index+1, &sz)) {
					trace("Expected block with 2 pairs!");
					goto err_spec;
				}
				// A rectangle outside the source throws - report it as an error
				// rather than let the exception escape the extension.
				EXCEPTION_TRY
				mat = new Mat(*src, Rect(pos.pair.x,pos.pair.y,sz.pair.x,sz.pair.y));
				EXCEPTION_CATCH
				goto done;
			}
			else goto err_spec;
		}
		if (bin) {
			if (size.width <= 0 || size.height <= 0) goto err_size;
			if (!IS_KNOWN_CV_TYPE(type)) goto err_type;
			// It is possible to create a matrix, which is using directly the Rebol binary data.
			// https://docs.opencv.org/3.4/d3/d63/classcv_1_1Mat.html#a9fa74fb14362d87cb183453d2441948f

			matBytes =  size.width * size.height * (int)CV_ELEM_SIZE(type);

			if (sharedBin) {
				//trace("shared binary");
				if(binBytes < matBytes) goto err_shortBin;
				mat = new Mat(size, type, SERIES_SKIP(bin, binIndex));
			}
			else {
				// Not shared: the matrix gets its own copy of the data, kept
				// alive by the handle. Data shorter than the matrix is repeated
				// to fill it.
				REBYTE *src = SERIES_SKIP(bin, binIndex);
				REBYTE *bp;
				int left;
				REBSER *ser = (REBSER *)RL_MAKE_STRING(matBytes, FALSE);
				if (!ser) RETURN_ERROR(ERR_MAT_SIZE);
				SERIES_TAIL(ser) = matBytes;
				bp = ser->data;
				if (binBytes <= 0) {
					memset(bp, 0, matBytes);
				} else {
					for (left = matBytes; left > 0; left -= binBytes, bp += binBytes) {
						memcpy(bp, src, MIN(left, binBytes));
					}
				}
				mat = new Mat(size, type, ser->data);
				bin = ser; // the handle keeps the copy, not the caller's series
			}
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
		if (size.width < 0 || size.height < 0) goto err_size;
		mat = new Mat(size, type, Scalar(0,0,0));
		goto done;
	}
	mat = new Mat(size, type);
	
done:
	return initRXHandle(frm, 1, mat, Handle_cvMat, bin);
err_size:
	RETURN_ERROR(ERR_MAT_SIZE);
err_spec:
	RETURN_ERROR(ERR_MAT_SPEC);
err_shortBin:
	RETURN_ERROR(ERR_MAT_SHORT);
err_type:
	RETURN_ERROR(ERR_MAT_TYPE);
err_vect:
	RETURN_ERROR(ERR_MAT_VECTOR);
}

COMMAND cmd_opencv_VideoCapture(RXIFRM *frm, void *ctx) {
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

	return initRXHandle(frm, 1, cap, Handle_cvVideoCapture, NULL);
}

COMMAND cmd_opencv_VideoWriter(RXIFRM *frm, void *ctx) {
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

	return initRXHandle(frm, 1, writer, Handle_cvVideoWriter, NULL);
}

COMMAND cmd_opencv_free(RXIFRM *frm, void *ctx) {
	REBCNT type = RXA_HANDLE_TYPE(frm, 1);
	if (type) {
		// calling Rebol's release function, which will call appropriate
		// GC callback to release OpenCV resources.
		RL_FREE_HANDLE_CONTEXT((REBHOB*)RXA_HANDLE_CONTEXT(frm, 1));
		//UNMARK_HOB((REBHOB*)RXA_HANDLE_CONTEXT(frm, 1));
	}
	else {
		return RXR_NONE;
	}
	return RXR_TRUE;
}

//;-----------------------------------------------------------------------
//;- Accessors                                                            
//;-----------------------------------------------------------------------

COMMAND cmd_opencv_get_property(RXIFRM *frm, void *ctx) {
	int propid = RXA_INT32(frm, 2);
	double result = 0;

	if (!RXA_HANDLE_CONTEXT(frm, 1)) return RXR_FALSE;

	
	if (ARG_Is_VideoCapture(1)) {
		VideoCapture *cap = ARG_VideoCapture(1);
		result = cap->get(propid);
	}
	else if (ARG_Is_Mat(1)) {
		REBHOB* hob = RXA_HANDLE_CONTEXT(frm, 1);
		if (!hob) return RXR_NONE;
		Mat *mat = (Mat*)HOB_PTR(hob);
		if (!mat) return RXR_NONE;
		REBSER* ser = hob->series;

		switch(propid){
			case MAT_SIZE: {
				Size size = mat->size();
				RXA_TYPE(frm,1) = RXT_PAIR;
				RXA_PAIR(frm,1).x = size.width;
				RXA_PAIR(frm,1).y = size.height;
				return RXR_VALUE;
			}
			case MAT_TYPE: {
				RXA_ARG(frm,1).int64 = typeWordFromCv(mat->type());
				if (!RXA_ARG(frm,1).int64) return RXR_NONE;
				RXA_TYPE(frm,1) = RXT_WORD;
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
				if (ser) {
					RXA_SERIES(frm, 1) = ser;
					RXA_INDEX(frm, 1) = 0;
				} else {
					REBSER *bin = (REBSER *)RL_MAKE_STRING(mat->elemSize() * mat->cols * mat->rows, FALSE);
					mat2ser(mat, bin, &RXA_ARG(frm,1));
				}
				RXA_TYPE  (frm, 1) = RXT_BINARY;
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

				REBSER *bin = (REBSER *)RL_MAKE_IMAGE(tmp.cols, tmp.rows);
				mat2ser(&tmp, bin, &RXA_ARG(frm,1));
				// The tail in an image value is not in bytes, but in pixels!
				SERIES_TAIL(bin) = tmp.cols * tmp.rows;
				RXA_TYPE(frm, 1) = RXT_IMAGE;
				RXA_IMAGE_WIDTH(frm, 1)  = tmp.cols;
				RXA_IMAGE_HEIGHT(frm, 1) = tmp.rows;
				RXA_IMAGE_INDEX(frm, 1)  = 0;
				return RXR_VALUE;
			}
			case MAT_VECTOR: {
				// Always a copy: a shared buffer is a byte series with no
				// element type, which is not a vector.
				if (!mat2vector(mat, &RXA_ARG(frm,1))) return RXR_NONE;
				RXA_TYPE  (frm, 1) = RXT_VECTOR;
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

COMMAND cmd_opencv_set_property(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_read(RXIFRM *frm, void *ctx) {
	VideoCapture *cap;
	Mat *frame;

	if(!ARG_Is_VideoCapture(1))
		return RXR_FALSE;

	cap = ARG_VideoCapture(1);
	if (!cap->isOpened()) return RXR_NONE;

	frame = ARG_Mat(3); // destination
	if (!frame) {
		frame = new Mat();
		if (RXR_VALUE != initRXHandle(frm, 1, frame, Handle_cvMat, NULL))
			return RXR_FALSE;
	}

	EXCEPTION_TRY
	cap->read(*frame);
	EXCEPTION_CATCH

	if (frame->empty()) return RXR_NONE;
	
	return RXR_VALUE;
}

COMMAND cmd_opencv_write(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_imread(RXIFRM *frm, void *ctx) {
	Mat image;
	String filename = ARG_String(1);
	int flags = (ARG_Is_Integer(4) ? RXA_INT32(frm,4) : IMREAD_UNCHANGED);

	EXCEPTION_TRY
	image = imread(filename, flags);
	if (image.empty()) return RXR_NONE;
	if (ARG_Is_None(2)) {
		Mat *result = new Mat(image);
		if (RXR_VALUE != initRXHandle(frm, 1, result, Handle_cvMat, NULL))
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
		RXA_IMAGE_INDEX(frm, 1) = 0;
	}
	
	EXCEPTION_CATCH

	return RXR_VALUE;
}

COMMAND cmd_opencv_imreadmulti(RXIFRM *frm, void *ctx) {
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
			initRXHandleArg(&val, image, Handle_cvMat, NULL);
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
			val.image_index = 0;
			RL_SET_VALUE(blk, n, val, RXT_IMAGE);
		}
	}
	RXA_TYPE(frm, 1) = RXT_BLOCK;
	RXA_SERIES(frm, 1) = blk;
	return RXR_VALUE;
}

COMMAND cmd_opencv_imwrite(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_resize(RXIFRM *frm, void *ctx) {
	Mat *src;
	Mat *dst;
	Size size;
	int interpolation = INTER_LINEAR;
	bool newHandle = FALSE;

	if (ARG_Is_Mat(1)) {
		src = ARG_Mat(1);
	} else if (ARG_Is_Image(1)) { // input is Rebol image
		src = new_Mat_From_Image_Arg(frm, 1); // delete before return!
	} else RETURN_ERROR(ERR_INVALID_MAT);

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
		return initRXHandle(frm, 1, dst, Handle_cvMat, NULL);
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

COMMAND cmd_opencv_bilateralFilter(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_blur(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_dilate(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_erode(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	Mat *kernel       = ARG_Mat(3);
	Point anchor      = ARG_Point(4);
	int iterations    = ARG_Int(5);

	if(!src || !dst || !kernel) return RXR_FALSE;

	EXCEPTION_TRY
	erode(*src, *dst, *kernel, anchor, iterations);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_opencv_filter2D(RXIFRM *frm, void *ctx) {
	Mat *src          = ARG_Mat(1);
	Mat *dst          = ARG_Mat_As(2, src);
	int ddepth        = ARG_Int(3);
	Mat *kernel       = ARG_Mat(4);
	Point anchor      = ARG_Point(5);
	double delta      = ARG_Double(6);
	int borderType    = ARG_BorderType(8);

	if(!src || !dst || !kernel) return RXR_FALSE;

	EXCEPTION_TRY
	filter2D(*src, *dst, ddepth, *kernel, anchor, delta, borderType);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_opencv_GaussianBlur(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_getGaborKernel(RXIFRM *frm, void *ctx) {
	Size 	ksize  = ARG_Size(1);
	double 	sigma  = ARG_Double(2);
	double 	theta  = ARG_Double(3);
	double 	lambd  = ARG_Double(4);
	double 	gamma  = ARG_Double(5);
	double 	psi    = ARG_Double(6);
	int 	ktype  = ARG_Int(7);
	Mat    *kernel = new Mat();

	EXCEPTION_TRY
	*kernel = getGaborKernel(ksize, sigma, theta, lambd, gamma, psi, ktype);
	EXCEPTION_CATCH

	return initRXHandle(frm, 1, kernel, Handle_cvMat, NULL);
}

COMMAND cmd_opencv_getStructuringElement(RXIFRM *frm, void *ctx) {
	int shape     = ARG_Int(1);
	Size ksize    = ARG_Size(2);
	Point anchor  = ARG_Point(3);
	Mat *dst      = new Mat();

	EXCEPTION_TRY
	*dst = getStructuringElement(shape, ksize, anchor);
	EXCEPTION_CATCH

	return initRXHandle(frm, 1, dst, Handle_cvMat, NULL);
}

COMMAND cmd_opencv_Laplacian(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_medianBlur(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_cvtColor(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_applyColorMap(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_Canny(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_threshold(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_bitwise_and(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_AND);
}
COMMAND cmd_opencv_bitwise_or(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_OR);
}
COMMAND cmd_opencv_bitwise_xor(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, BITWISE_XOR);
}
COMMAND cmd_opencv_add(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_ADD);
}
COMMAND cmd_opencv_subtract(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_SUBTRACT);
}
COMMAND cmd_opencv_multiply(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_MULTIPLY);
}
COMMAND cmd_opencv_divide(RXIFRM *frm, void *ctx) {
	return mat_math_op(frm, ctx, MATH_DIVIDE);
}

COMMAND cmd_opencv_bitwise_not(RXIFRM *frm, void *ctx) {
	Mat *src  = ARG_Mat(1);
	Mat *dst  = ARG_Mat_As(2, src);
	// Slot 3 is the /mask refinement itself, its argument is in slot 4.
	Mat *mask = RXA_REF(frm, 3) ? ARG_Mat(4) : NULL;

	if (!src || !dst ) return RXR_NONE;

	EXCEPTION_TRY
	if (mask) bitwise_not(*src, *dst, *mask);
	else      bitwise_not(*src, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 2);
	return RXR_VALUE;
}

COMMAND cmd_opencv_absdiff(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_addWeighted(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_convertScaleAbs(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_flip(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_invert(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_max(RXIFRM *frm, void *ctx) {
	Mat *src1 = ARG_Mat(1);
	Mat *src2 = ARG_Mat(2);
	Mat *dst  = ARG_Mat_As(3, src1);

	if (!src1 || !src2) return RXR_NONE;

	EXCEPTION_TRY
	max(*src1, *src2, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 3);
	return RXR_VALUE;	
}

COMMAND cmd_opencv_mean(RXIFRM *frm, void *ctx) {
	Scalar scalar;
	REBINT channels;
	RXIARG val;
	REBSER *ser;
	Mat *src = ARG_Mat(1);

	if (!src) return RXR_NONE;

	EXCEPTION_TRY
	scalar = mean(*src);
	EXCEPTION_CATCH

	channels = src->channels();
	if (channels == 1) {
		RXA_TYPE(frm, 1) = RXT_DECIMAL;
		RXA_DEC64(frm, 1) = scalar[0];
	} else {
		ser = (REBSER*)RL_MAKE_BLOCK(channels);
		if(!ser) return RXR_NONE;

		for(int i=0; i < channels; i++) {
			val.dec64 = scalar[i];
			RL_SET_VALUE(ser, i, val, RXT_DECIMAL);
		}
		RXA_TYPE(frm, 1) = RXT_BLOCK;
		RXA_SERIES(frm, 1) = ser;
		RXA_INDEX(frm, 1) = 0;
		SERIES_TAIL(ser) = channels;
	}
	return RXR_VALUE;	
}

COMMAND cmd_opencv_min(RXIFRM *frm, void *ctx) {
	Mat *src1 = ARG_Mat(1);
	Mat *src2 = ARG_Mat(2);
	Mat *dst  = ARG_Mat_As(3, src1);

	if (!src1 || !src2) return RXR_NONE;

	EXCEPTION_TRY
	min(*src1, *src2, *dst);
	EXCEPTION_CATCH

	RXA_ARG(frm, 1) = RXA_ARG(frm, 3);
	return RXR_VALUE;	
}

COMMAND cmd_opencv_minMaxLoc(RXIFRM *frm, void *ctx) {
	Mat *src = ARG_Mat(1);
	double minVal; 
	double maxVal; 
	Point minLoc; 
	Point maxLoc;

	if (!src) return RXR_NONE;

	EXCEPTION_TRY
	minMaxLoc(*src, &minVal, &maxVal, &minLoc, &maxLoc);
	EXCEPTION_CATCH

	REBSER *blk = (REBSER*)RL_MAKE_BLOCK(4);
	RXIARG val;

	val.dec64 = minVal;
	RL_SET_VALUE(blk, 0, val, RXT_DECIMAL);
	val.dec64 = maxVal;
	RL_SET_VALUE(blk, 1, val, RXT_DECIMAL);
	val.pair.x = (float)minLoc.x;
	val.pair.y = (float)minLoc.y;
	RL_SET_VALUE(blk, 2, val, RXT_PAIR);
	val.pair.x = (float)maxLoc.x;
	val.pair.y = (float)maxLoc.y;
	RL_SET_VALUE(blk, 3, val, RXT_PAIR);

	RXA_TYPE(frm, 1) = RXT_BLOCK;
	RXA_SERIES(frm, 1) = blk;
	RXA_INDEX(frm, 1) = 0;
	return RXR_VALUE;
}

COMMAND cmd_opencv_normalize(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_transform(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_convertTo(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_startWindowThread(RXIFRM *frm, void *ctx) {
	startWindowThread();
	return RXR_UNSET;
}

COMMAND cmd_opencv_imshow(RXIFRM *frm, void *ctx) {
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

COMMAND cmd_opencv_pollKey(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_ARG(frm, 1).int64 = pollKey();
	return RXR_VALUE;
}

COMMAND cmd_opencv_waitKey(RXIFRM *frm, void *ctx) {
	RXA_ARG(frm, 1).int64 = waitKey(RXA_ARG(frm, 1).int32a); // Wait for a keystroke in the window
	return RXR_VALUE;
}


COMMAND cmd_opencv_namedWindow(RXIFRM *frm, void *ctx) {
	namedWindow(ARG_String(1), WINDOW_NORMAL );
	pollKey(); // OpenCV bug on macOS... window would not resize without it!
	return RXR_UNSET;
}

COMMAND cmd_opencv_resizeWindow(RXIFRM *frm, void *ctx) {
	resizeWindow(ARG_String(1), PAIR_X(frm,2), PAIR_Y(frm,2));
	return RXR_UNSET;
}

COMMAND cmd_opencv_moveWindow(RXIFRM *frm, void *ctx) {
	moveWindow(ARG_String(1), PAIR_X(frm,2), PAIR_Y(frm,2));
	return RXR_UNSET;
}

COMMAND cmd_opencv_getWindowImageSize(RXIFRM *frm, void *ctx) {
	Rect rect = getWindowImageRect(ARG_String(1));
	RXA_TYPE(frm, 1) = RXT_PAIR;
	RXA_PAIR(frm,1).x = rect.width;
	RXA_PAIR(frm,1).y = rect.height;
	return RXR_VALUE;
}

COMMAND cmd_opencv_getWindowImagePos(RXIFRM *frm, void *ctx) {
	Rect rect = getWindowImageRect(ARG_String(1));
	RXA_TYPE(frm, 1) = RXT_PAIR;
	RXA_PAIR(frm,1).x = rect.x;
	RXA_PAIR(frm,1).y = rect.y;
	return RXR_VALUE;
}

COMMAND cmd_opencv_getWindowProperty(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_DECIMAL;
	RXA_DEC64(frm, 1) = getWindowProperty(ARG_String(1), ARG_Int(2));
	return RXR_VALUE;
}

COMMAND cmd_opencv_setWindowProperty(RXIFRM *frm, void *ctx) {
	setWindowProperty(ARG_String(1), ARG_Int(2), ARG_Int(3));
	return RXR_UNSET;
}

COMMAND cmd_opencv_setWindowTitle(RXIFRM *frm, void *ctx) {
	setWindowTitle(ARG_String(1), ARG_String(2));
	return RXR_UNSET;
}

COMMAND cmd_opencv_destroyAllWindows(RXIFRM *frm, void *ctx) {
	destroyAllWindows();
	pollKey();
	return RXR_UNSET;
}

COMMAND cmd_opencv_destroyWindow(RXIFRM *frm, void *ctx) {
	destroyWindow(ARG_String(1));
	return RXR_UNSET;
}


static void rebTrackbarCallback(int pos, void* userdata) {
	REBHOB* hob = (REBHOB*)userdata;
	CTX_TRACKBAR* bar = (CTX_TRACKBAR*)hob->data;
	if (!bar) {trace("null trackbar handle!"); return;}
	RXIARG *args = bar->args;
	RXICBI *cbi  = bar->cbi;
	if (!args || !cbi) {trace("!args || !cbi"); return;}

	RXI_COUNT(args) = 1;
	RXI_TYPE(args, 1) = RXT_INTEGER;
	args[1].int64 = pos;

	RL_CALLBACK(cbi);
}

COMMAND cmd_opencv_createTrackbar(RXIFRM *frm, void *ctx) {
	REBHOB* hob = RL_MAKE_HANDLE_CONTEXT(Handle_cvTrackbar);
	debug_print("new hob: %p handle: %p (cvTrackbar)\n", hob, hob->data);
	if (hob == NULL) return RXR_FALSE;
	
	CTX_TRACKBAR* bar = (CTX_TRACKBAR*)hob->data;
	bar->name   = new String((const char*)((REBSER*)RXA_ARG(frm, 1).series)->data);
	bar->window = new String((const char*)((REBSER*)RXA_ARG(frm, 2).series)->data);
	EXCEPTION_TRY
	if (ARG_Is_None(4)) {
		// version without callback
		createTrackbar(*bar->name, *bar->window, &bar->value, ARG_Int(3));
	} else {
		// used callback
		bar->cbi    = (RXICBI *)MAKE_MEM(sizeof(RXICBI));
		bar->args   = (RXIARG *)MAKE_MEM(sizeof(RXIARG) * 4);

		CLEAR(bar->cbi,  sizeof(RXICBI));
		CLEAR(bar->args, sizeof(RXIARG) * 4);

		bar->cbi->obj  = (REBSER*)RXA_OBJECT(frm, 5);
		bar->cbi->word = RXA_WORD(frm, 6);
		bar->cbi->args = bar->args;
		
		createTrackbar(*bar->name, *bar->window, &bar->value, ARG_Int(3), rebTrackbarCallback, hob);
	}
	EXCEPTION_CATCH

	RXA_HANDLE(frm, 1) = hob;
	RXA_HANDLE_TYPE(frm, 1) = hob->sym;
	RXA_HANDLE_FLAGS(frm, 1) = hob->flags;
	RXA_TYPE(frm, 1) = RXT_HANDLE;

	return RXR_VALUE;
}

COMMAND cmd_opencv_setTrackbarMax(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	setTrackbarMax(*bar->name, *bar->window, ARG_Int(2));
	return RXR_VALUE;
}
COMMAND cmd_opencv_setTrackbarMin(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	setTrackbarMin(*bar->name, *bar->window, ARG_Int(2));
	return RXR_VALUE;
}
COMMAND cmd_opencv_setTrackbarPos(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	setTrackbarPos(*bar->name, *bar->window, ARG_Int(2));
	return RXR_VALUE;
}
COMMAND cmd_opencv_getTrackbarPos(RXIFRM *frm, void *ctx) {
	CTX_TRACKBAR* bar = ARG_Trackbar(1);
	if (!bar) return RXR_FALSE;
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_ARG(frm, 1).int64 = getTrackbarPos(*bar->name, *bar->window);
	return RXR_VALUE;
}


COMMAND cmd_opencv_selectROI(RXIFRM *frm, void *ctx) {
	Mat *image  = ARG_Mat(1);
	Rect rect;

	if (image) {
		Size size = image->size();
		if (size.width == 0 || size.height == 0) return RXR_FALSE;
		EXCEPTION_TRY
		rect = selectROI(*image);
		EXCEPTION_CATCH
	} else if(ARG_Is_Image(1)) { // input is Rebol image
		Mat image;
		RXIARG arg = RXA_ARG(frm, 1);
		image = Mat( arg.height, arg.width, CV_8UC4);
		image.data = ((REBSER*)arg.series)->data;
		rect = selectROI(image);
	} else {
		return RXR_FALSE;
	}

	REBSER *blk = (REBSER*)RL_MAKE_BLOCK(2);
	RXIARG val;

	val.pair.x = (float)rect.x;
	val.pair.y = (float)rect.y;
	RL_SET_VALUE(blk, 0, val, RXT_PAIR);
	val.pair.x = (float)rect.width;
	val.pair.y = (float)rect.height;
	RL_SET_VALUE(blk, 1, val, RXT_PAIR);


	RXA_TYPE(frm, 1) = RXT_BLOCK;
	RXA_SERIES(frm, 1) = blk;
	RXA_INDEX(frm, 1) = 0;
	return RXR_VALUE;
}

static void rebMouseCallback(int event, int x, int y, int flags, void* userdata) {
	REBHOB* hob = (REBHOB*)userdata;
    CTX_MOUSECALLBACK* mcb = (CTX_MOUSECALLBACK*)hob->data;
    //debug_print("mouse %p %p %p %p\n", hob, mcb, mcb->args, mcb->cbi);
	if (!mcb) {trace("null mouseCallback handle!"); return;}
	RXIARG *args = mcb->args;
	RXICBI *cbi  = mcb->cbi;
	if (!args || !cbi) {trace("!args || !cbi"); return;}

	args[1].int64 = event;
	args[2].int64 = mcb->x = x;
	args[3].int64 = mcb->y = y;
	args[4].int64 = mcb->flags = flags;

	RL_CALLBACK(cbi);
}

COMMAND cmd_opencv_setMouseCallback(RXIFRM *frm, void *ctx) {
	REBHOB* hob = RL_MAKE_HANDLE_CONTEXT(Handle_cvMouseCallback);
	if (hob == NULL) return RXR_FALSE;

	if (ARG_Is_None(3)) {
		setMouseCallback(ARG_String(1), NULL);
		return RXR_UNSET;
	}
	
	CTX_MOUSECALLBACK* mcb = (CTX_MOUSECALLBACK*)hob->data;
	mcb->window = new String((const char*)((REBSER*)RXA_ARG(frm, 1).series)->data);
	mcb->cbi    = (RXICBI*)MAKE_MEM(sizeof(RXICBI));
	mcb->args   = (RXIARG*)MAKE_MEM(sizeof(RXIARG) * 5);
	CLEAR(mcb->cbi,  sizeof(RXICBI));
	CLEAR(mcb->args, sizeof(RXIARG) * 5);
	mcb->cbi->obj  = (REBSER*)RXA_OBJECT(frm, 2);
	mcb->cbi->word = RXA_WORD(frm, 3);
	mcb->cbi->args = mcb->args;

	//debug_print("cmd_setMouseCallback %p %p %p %p\n", hob, mcb, mcb->args, mcb->cbi);

	RXI_COUNT(mcb->args) = 4;
	RXI_TYPE(mcb->args, 1) = RXT_INTEGER;
	RXI_TYPE(mcb->args, 2) = RXT_INTEGER;
	RXI_TYPE(mcb->args, 3) = RXT_INTEGER;
	RXI_TYPE(mcb->args, 4) = RXT_INTEGER;

	setMouseCallback(*mcb->window, rebMouseCallback, hob);

	RXA_HANDLE(frm, 1) = hob;
	RXA_HANDLE_TYPE(frm, 1) = hob->sym;
	RXA_HANDLE_FLAGS(frm, 1) = hob->flags;
	RXA_TYPE(frm, 1) = RXT_HANDLE;
	return RXR_VALUE;
}

//;-----------------------------------------------------------------------
//;- Utilities                                                            
//;-----------------------------------------------------------------------

COMMAND cmd_opencv_getTickCount(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_INT64(frm, 1) = getTickCount();
	return RXR_VALUE;
}
COMMAND cmd_opencv_getTickFrequency(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_DECIMAL;
	RXA_DEC64(frm, 1) = getTickFrequency();
	return RXR_VALUE;
}
COMMAND cmd_opencv_getNumThreads(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_INTEGER;
	RXA_INT64(frm, 1) = getNumThreads();
	return RXR_VALUE;
}
COMMAND cmd_opencv_getVersionString(RXIFRM *frm, void *ctx) {
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
COMMAND cmd_opencv_getBuildInformation(RXIFRM *frm, void *ctx) {
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
COMMAND cmd_opencv_useOptimized(RXIFRM *frm, void *ctx) {
	RXA_TYPE(frm, 1) = RXT_LOGIC;
	RXA_LOGIC(frm, 1) = useOptimized();
	return RXR_VALUE;
}
COMMAND cmd_opencv_setUseOptimized(RXIFRM *frm, void *ctx) {
	setUseOptimized(RXA_LOGIC(frm, 1));
	return RXR_VALUE;
}


COMMAND cmd_opencv_qrcode_encode(RXIFRM *frm, void *ctx) {
	Mat *result;
	String text = ARG_String(1);

	result = new Mat();
	if (RXR_VALUE != initRXHandle(frm, 1, result, Handle_cvMat, NULL))
		return RXR_FALSE;
	
    QRCodeEncoder::Params params;
    if (RXA_TYPE(frm, 3) == RXT_INTEGER) params.version = RXA_INT32(frm, 3);
    if (RXA_TYPE(frm, 5) == RXT_INTEGER) params.mode = (QRCodeEncoder::EncodeMode)RXA_INT32(frm, 5);
    if (RXA_TYPE(frm, 7) == RXT_INTEGER) params.correction_level = (QRCodeEncoder::CorrectionLevel)RXA_INT32(frm, 7);

    EXCEPTION_TRY
    Ptr<QRCodeEncoder> encoder = QRCodeEncoder::create(params);
    encoder->encode(text, *result);
    EXCEPTION_CATCH
    return RXR_VALUE;
}

COMMAND cmd_opencv_qrcode_decode(RXIFRM *frm, void *ctx) {
	Mat *img = ARG_Mat(1);
	String str;
	vector<Point> corners;
	QRCodeDetector qrcode;

	EXCEPTION_TRY
	if (img == NULL) {
		Mat tmp;
		if (ARG_Is_Image(1)) {
			RXIARG arg = RXA_ARG(frm, 1);
			tmp = Mat(arg.height, arg.width, CV_8UC4);
			tmp.data = ((REBSER*)arg.series)->data;
		}
		else if (ARG_Is_File(1)) {
			String file = ARG_String(1);
			tmp = imread(file);
			if (tmp.empty()) return RXR_NONE;
		}
		else return RXR_NONE;
		str = qrcode.detectAndDecode(tmp, corners);	
	} else {
		str = qrcode.detectAndDecode(*img, corners);
	}
	EXCEPTION_CATCH
	
	int len = str.length();
	if (len == 0) return RXR_NONE;

	REBSER *ser = (REBSER *)RL_MAKE_STRING(len , FALSE);
	memcpy(ser->data, str.c_str(),  len);
	SERIES_TAIL(ser) = len;
	RXA_TYPE  (frm, 1) = RXT_STRING;
	RXA_SERIES(frm, 1) = ser;
	RXA_INDEX (frm, 1) = 0;
	return RXR_VALUE;
}