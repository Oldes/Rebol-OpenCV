//
// auto-generated file, do not modify!
//

#include "common.h"

#define SERIES_TEXT(s)   ((char*)SERIES_DATA(s))

#define MIN_REBOL_VER 3
#define MIN_REBOL_REV 14
#define MIN_REBOL_UPD 0
#define VERSION(a, b, c) (a << 16) + (b << 8) + c
#define MIN_REBOL_VERSION VERSION(MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD)


extern u32* arg_words;
extern u32* type_words;

enum ext_commands {
	CMD_OPENCV_INIT_WORDS,
	CMD_OPENCV_TEST,
	CMD_OPENCV_MATRIX,
	CMD_OPENCV_VIDEOCAPTURE,
	CMD_OPENCV_VIDEOWRITER,
	CMD_OPENCV_FREE,
	CMD_OPENCV_GET_PROPERTY,
	CMD_OPENCV_SET_PROPERTY,
	CMD_OPENCV_READ,
	CMD_OPENCV_WRITE,
	CMD_OPENCV_IMREAD,
	CMD_OPENCV_IMREADMULTI,
	CMD_OPENCV_IMWRITE,
	CMD_OPENCV_RESIZE,
	CMD_OPENCV_BILATERALFILTER,
	CMD_OPENCV_BLUR,
	CMD_OPENCV_DILATE,
	CMD_OPENCV_ERODE,
	CMD_OPENCV_FILTER2D,
	CMD_OPENCV_GAUSSIANBLUR,
	CMD_OPENCV_GETGABORKERNEL,
	CMD_OPENCV_GETSTRUCTURINGELEMENT,
	CMD_OPENCV_LAPLACIAN,
	CMD_OPENCV_MEDIANBLUR,
	CMD_OPENCV_CVTCOLOR,
	CMD_OPENCV_APPLYCOLORMAP,
	CMD_OPENCV_THRESHOLD,
	CMD_OPENCV_ABSDIFF,
	CMD_OPENCV_ADD,
	CMD_OPENCV_ADDWEIGHTED,
	CMD_OPENCV_BITWISE_AND,
	CMD_OPENCV_BITWISE_NOT,
	CMD_OPENCV_BITWISE_OR,
	CMD_OPENCV_BITWISE_XOR,
	CMD_OPENCV_CONVERTSCALEABS,
	CMD_OPENCV_DIVIDE,
	CMD_OPENCV_FLIP,
	CMD_OPENCV_INVERT,
	CMD_OPENCV_MAX,
	CMD_OPENCV_MEAN,
	CMD_OPENCV_MIN,
	CMD_OPENCV_MINMAXLOC,
	CMD_OPENCV_MULTIPLY,
	CMD_OPENCV_NORMALIZE,
	CMD_OPENCV_SUBTRACT,
	CMD_OPENCV_TRANSFORM,
	CMD_OPENCV_CONVERTTO,
	CMD_OPENCV_CANNY,
	CMD_OPENCV_STARTWINDOWTHREAD,
	CMD_OPENCV_IMSHOW,
	CMD_OPENCV_POLLKEY,
	CMD_OPENCV_WAITKEY,
	CMD_OPENCV_NAMEDWINDOW,
	CMD_OPENCV_RESIZEWINDOW,
	CMD_OPENCV_MOVEWINDOW,
	CMD_OPENCV_GETWINDOWIMAGESIZE,
	CMD_OPENCV_GETWINDOWIMAGEPOS,
	CMD_OPENCV_GETWINDOWPROPERTY,
	CMD_OPENCV_SETWINDOWPROPERTY,
	CMD_OPENCV_SETWINDOWTITLE,
	CMD_OPENCV_DESTROYALLWINDOWS,
	CMD_OPENCV_DESTROYWINDOW,
	CMD_OPENCV_CREATETRACKBAR,
	CMD_OPENCV_SETTRACKBARMAX,
	CMD_OPENCV_SETTRACKBARMIN,
	CMD_OPENCV_SETTRACKBARPOS,
	CMD_OPENCV_GETTRACKBARPOS,
	CMD_OPENCV_SELECTROI,
	CMD_OPENCV_GETTICKCOUNT,
	CMD_OPENCV_GETTICKFREQUENCY,
	CMD_OPENCV_GETNUMTHREADS,
	CMD_OPENCV_GETVERSIONSTRING,
	CMD_OPENCV_GETBUILDINFORMATION,
	CMD_OPENCV_USEOPTIMIZED,
	CMD_OPENCV_SETUSEOPTIMIZED,
	CMD_OPENCV_QRCODE_ENCODE,
	CMD_OPENCV_QRCODE_DECODE,
};


int cmd_init_words(RXIFRM *frm, void *ctx);
int cmd_test(RXIFRM *frm, void *ctx);
int cmd_Matrix(RXIFRM *frm, void *ctx);
int cmd_VideoCapture(RXIFRM *frm, void *ctx);
int cmd_VideoWriter(RXIFRM *frm, void *ctx);
int cmd_free(RXIFRM *frm, void *ctx);
int cmd_get_property(RXIFRM *frm, void *ctx);
int cmd_set_property(RXIFRM *frm, void *ctx);
int cmd_read(RXIFRM *frm, void *ctx);
int cmd_write(RXIFRM *frm, void *ctx);
int cmd_imread(RXIFRM *frm, void *ctx);
int cmd_imreadmulti(RXIFRM *frm, void *ctx);
int cmd_imwrite(RXIFRM *frm, void *ctx);
int cmd_resize(RXIFRM *frm, void *ctx);
int cmd_bilateralFilter(RXIFRM *frm, void *ctx);
int cmd_blur(RXIFRM *frm, void *ctx);
int cmd_dilate(RXIFRM *frm, void *ctx);
int cmd_erode(RXIFRM *frm, void *ctx);
int cmd_filter2D(RXIFRM *frm, void *ctx);
int cmd_GaussianBlur(RXIFRM *frm, void *ctx);
int cmd_getGaborKernel(RXIFRM *frm, void *ctx);
int cmd_getStructuringElement(RXIFRM *frm, void *ctx);
int cmd_Laplacian(RXIFRM *frm, void *ctx);
int cmd_medianBlur(RXIFRM *frm, void *ctx);
int cmd_cvtColor(RXIFRM *frm, void *ctx);
int cmd_applyColorMap(RXIFRM *frm, void *ctx);
int cmd_threshold(RXIFRM *frm, void *ctx);
int cmd_absdiff(RXIFRM *frm, void *ctx);
int cmd_add(RXIFRM *frm, void *ctx);
int cmd_addWeighted(RXIFRM *frm, void *ctx);
int cmd_bitwise_and(RXIFRM *frm, void *ctx);
int cmd_bitwise_not(RXIFRM *frm, void *ctx);
int cmd_bitwise_or(RXIFRM *frm, void *ctx);
int cmd_bitwise_xor(RXIFRM *frm, void *ctx);
int cmd_convertScaleAbs(RXIFRM *frm, void *ctx);
int cmd_divide(RXIFRM *frm, void *ctx);
int cmd_flip(RXIFRM *frm, void *ctx);
int cmd_invert(RXIFRM *frm, void *ctx);
int cmd_max(RXIFRM *frm, void *ctx);
int cmd_mean(RXIFRM *frm, void *ctx);
int cmd_min(RXIFRM *frm, void *ctx);
int cmd_minMaxLoc(RXIFRM *frm, void *ctx);
int cmd_multiply(RXIFRM *frm, void *ctx);
int cmd_normalize(RXIFRM *frm, void *ctx);
int cmd_subtract(RXIFRM *frm, void *ctx);
int cmd_transform(RXIFRM *frm, void *ctx);
int cmd_convertTo(RXIFRM *frm, void *ctx);
int cmd_Canny(RXIFRM *frm, void *ctx);
int cmd_startWindowThread(RXIFRM *frm, void *ctx);
int cmd_imshow(RXIFRM *frm, void *ctx);
int cmd_pollKey(RXIFRM *frm, void *ctx);
int cmd_waitKey(RXIFRM *frm, void *ctx);
int cmd_namedWindow(RXIFRM *frm, void *ctx);
int cmd_resizeWindow(RXIFRM *frm, void *ctx);
int cmd_moveWindow(RXIFRM *frm, void *ctx);
int cmd_getWindowImageSize(RXIFRM *frm, void *ctx);
int cmd_getWindowImagePos(RXIFRM *frm, void *ctx);
int cmd_getWindowProperty(RXIFRM *frm, void *ctx);
int cmd_setWindowProperty(RXIFRM *frm, void *ctx);
int cmd_setWindowTitle(RXIFRM *frm, void *ctx);
int cmd_destroyAllWindows(RXIFRM *frm, void *ctx);
int cmd_destroyWindow(RXIFRM *frm, void *ctx);
int cmd_createTrackbar(RXIFRM *frm, void *ctx);
int cmd_setTrackbarMax(RXIFRM *frm, void *ctx);
int cmd_setTrackbarMin(RXIFRM *frm, void *ctx);
int cmd_setTrackbarPos(RXIFRM *frm, void *ctx);
int cmd_getTrackbarPos(RXIFRM *frm, void *ctx);
int cmd_selectROI(RXIFRM *frm, void *ctx);
int cmd_getTickCount(RXIFRM *frm, void *ctx);
int cmd_getTickFrequency(RXIFRM *frm, void *ctx);
int cmd_getNumThreads(RXIFRM *frm, void *ctx);
int cmd_getVersionString(RXIFRM *frm, void *ctx);
int cmd_getBuildInformation(RXIFRM *frm, void *ctx);
int cmd_useOptimized(RXIFRM *frm, void *ctx);
int cmd_setUseOptimized(RXIFRM *frm, void *ctx);
int cmd_qrcode_encode(RXIFRM *frm, void *ctx);
int cmd_qrcode_decode(RXIFRM *frm, void *ctx);

enum cv_arg_words {W_ARG_0,
	W_ARG_SIZE,
	W_ARG_TYPE,
	W_ARG_DEPTH,
	W_ARG_CHANNELS,
	W_ARG_BINARY,
	W_ARG_IMAGE,
	W_ARG_VECTOR,
	W_ARG_TOTAL,
	W_ARG_IS_SUBMATRIX,
	W_ARG_WIDTH,
	W_ARG_HEIGHT,
	W_ARG_POS_MS,
	W_ARG_POS_FRAME,
	W_ARG_POS_RATIO,
	W_ARG_FPS,
	W_ARG_FOURCC,
	W_ARG_FRAMES,
	W_ARG_FORMAT
};
enum cv_type_words {W_TYPE_0,
	W_TYPE_CV_8U,
	W_TYPE_CV_8S,
	W_TYPE_CV_16U,
	W_TYPE_CV_16S,
	W_TYPE_CV_32S,
	W_TYPE_CV_32F,
	W_TYPE_CV_64F,
	W_TYPE_CV_16F,
	W_TYPE_CV_8UC1,
	W_TYPE_CV_8SC1,
	W_TYPE_CV_16UC1,
	W_TYPE_CV_16SC1,
	W_TYPE_CV_32SC1,
	W_TYPE_CV_32FC1,
	W_TYPE_CV_64FC1,
	W_TYPE_CV_16FC1,
	W_TYPE_CV_8UC2,
	W_TYPE_CV_8SC2,
	W_TYPE_CV_16UC2,
	W_TYPE_CV_16SC2,
	W_TYPE_CV_32SC2,
	W_TYPE_CV_32FC2,
	W_TYPE_CV_64FC2,
	W_TYPE_CV_16FC2,
	W_TYPE_CV_8UC3,
	W_TYPE_CV_8SC3,
	W_TYPE_CV_16UC3,
	W_TYPE_CV_16SC3,
	W_TYPE_CV_32SC3,
	W_TYPE_CV_32FC3,
	W_TYPE_CV_64FC3,
	W_TYPE_CV_16FC3,
	W_TYPE_CV_8UC4,
	W_TYPE_CV_8SC4,
	W_TYPE_CV_16UC4,
	W_TYPE_CV_16SC4,
	W_TYPE_CV_32SC4,
	W_TYPE_CV_32FC4,
	W_TYPE_CV_64FC4,
	W_TYPE_CV_16FC4
};

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);

#define OPENCV_EXT_INIT_CODE \
	"REBOL [Title: {Rebol OpenCV Extension} Type: module Exports: [] Require: 3.14.0]\n"\
	"init-words: command [args [block!] type [block!]]\n"\
	"test: command [\"Simple OpenCV test\"]\n"\
	"Matrix: command [\"Initialize new cvMat class\" spec [pair! handle! image! block!]]\n"\
	"VideoCapture: command [\"Initialize new VideoCapture class\" src [integer! file! string!]]\n"\
	"VideoWriter: command [\"Initialize new VideoWriter class\" src [integer! file! string!] codec [integer!] {4-character code of codec used to compress the frames (mp4ra.org/#/codecs)} fps [number!] \"Framerate of the created video stream\" size [pair!] \"Size of the video frames\"]\n"\
	"free: command [\"Release OpenCV handle\" class [handle!] \"Mat, VideoCapture or VideoWriter\"]\n"\
	"get-property: command [\"Returns a property value\" obj [handle!] \"VideoCapture handle\" property [integer!]]\n"\
	"set-property: command [obj [handle!] \"VideoCapture handle\" property [integer!] value [number!]]\n"\
	"read: command [\"Grabs, decodes and returns the next video frame\" src [handle!] \"VideoCapture\" /into \"Optional existing Mat handle\" dst [handle!] \"Mat\"]\n"\
	"write: command [\"Writes the next video frame\" dst [handle!] \"VideoWriter\" frame [image! handle!]]\n"\
	"imread: command [src [file! string!] \"Loads an image from a file.\" /image \"As Rebol image instead of default cvMat\" /with \"Flag that can take values IMREAD_*\" flags [integer!] \"Default is IMREAD_UNCHANGED\"]\n"\
	"imreadmulti: command [src [file! string!] \"Loads a multi-page image from a file.\" /image \"As Rebol images instead of default cvMat\" /with \"Flag that can take values IMREAD_*\" flags [integer!] \"Default is IMREAD_UNCHANGED\"]\n"\
	"imwrite: command [\"Saves an image to a specified file.\" name [any-string!] image [image! handle!] \"Image or cvMat handle\" /with \"Format-specific parameters encoded as pairs\" params [block!] \"integer pairs (words are resolved)\"]\n"\
	"resize: command [\"Resizes an image.\" image [image! handle!] \"Image or cvMat handle\" size [pair! percent!] /into target [handle!] \"cvMat\" /with interpolation [integer!]]\n"\
	"bilateralFilter: command [\"Applies the bilateral filter to an image.\" src [handle!] {Source 8-bit or floating-point, 1-channel or 3-channel image.} dst [handle! none!] {Destination image of the same size and type as src.} diameter [integer!] {Diameter of each pixel neighborhood that is used during filtering. If it is non-positive, it is computed from sigmaSpace.} sigmaColor [number!] sigmaSpace [number!] /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"blur: command [\"Blurs an image using the normalized box filter.\" src [handle!] \"cvMat handle\" dst [handle! none!] size [pair! integer!] \"blurring kernel size\" /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"dilate: command [{Dilates an image by using a specific structuring element.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" kernel [handle!] anchor [pair! integer!] \"position of the anchor within the element\" iterations [integer!] \"number of times dilation is applied\"]\n"\
	"erode: command [{Erodes an image by using a specific structuring element.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" kernel [handle!] anchor [pair! integer!] \"position of the anchor within the element\" iterations [integer!] \"number of times dilation is applied\"]\n"\
	"filter2D: command [\"Convolves an image with the kernel.\" src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" ddepth [integer!] \"desired depth of the destination image\" kernel [handle!] {convolution kernel (or rather a correlation kernel), a single-channel floating point matrix} anchor [pair! integer!] \"position of the anchor within the element\" delta [number!] {value added to the filtered pixels before storing them in dst} /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"GaussianBlur: command [\"Blurs an image using a Gaussian filter.\" src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" size [pair! integer!] \"blurring kernel size\" sigmaX [number!] sigmaY [number!] /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"getGaborKernel: command [\"Returns Gabor filter coefficients.\" ksize [pair!] \"Size of the filter returned\" sigma [number!] \"Standard deviation of the gaussian envelope\" theta [number!] {Orientation of the normal to the parallel stripes of a Gabor function.} lambd [number!] \"Wavelength of the sinusoidal factor\" gamma [number!] \"Spatial aspect ratio\" psi [number!] \"Phase offset\" ktype [integer!] {Type of filter coefficients. It can be CV_32F or CV_64F}]\n"\
	"getStructuringElement: command [{Returns a structuring element of the specified size and shape for morphological operations.} shape [integer!] {Element shape that could be one of MorphShapes (rect = 0, cross = 1, ellipse = 2)} ksize [pair! integer!] \"Size of the structuring element\" anchor [pair! integer!] \"Anchor position within the element\"]\n"\
	"Laplacian: command [\"Calculates the Laplacian of an image.\" src [handle!] \"Source image\" dst [handle! none!] {Destination image of the same size and the same number of channels as src} ddepth [number!] \"Desired depth of the destination image\" ksize [number!] {Aperture size used to compute the second-derivative filters. The size must be positive and odd.} scale [number!] \"Scale factor for the computed Laplacian values.\" delta [number!] {Optional delta value that is added to the results prior to storing them in dst.}]\n"\
	"medianBlur: command [\"Blurs an image using the median filter.\" src [handle!] {input 1-, 3-, or 4-channel image; when ksize is 3 or 5, the image depth should be CV_8U, CV_16U, or CV_32F, for larger aperture sizes, it can only be CV_8U} dst [handle! none!] \"destination array of the same size and type as src\" size [number!] {aperture linear size; it must be odd and greater than 1, for example: 3, 5, 7...}]\n"\
	"cvtColor: command [\"Converts an image from one color space to another.\" src [handle!] \"source cvMat handle\" dst [handle! none!] \"destination cvMat\" code [integer!]]\n"\
	"applyColorMap: command [{Applies a GNU Octave/MATLAB equivalent colormap on a given image.} src [handle!] \"source cvMat handle\" dst [handle! none!] \"destination cvMat\" colormap [integer!]]\n"\
	"threshold: command [{Applies a fixed-level threshold to each array element.} src [handle!] dst [handle! none!] thresh [number!] maxval [number!] type [integer!]]\n"\
	"absdiff: command [{Calculates the per-element absolute difference between two arrays} src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\"]\n"\
	"add: command [\"Calculates the per-element sum of two arrays.\" src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /mask m [handle!] \"cvMat\"]\n"\
	"addWeighted: command [\"Calculates the weighted sum of two arrays.\" src1 [handle!] \"cvMat\" alpha [number!] \"weight of the first array elements.\" src2 [handle!] \"cvMat\" beta [number!] \"weight of the second array elements.\" gamma [number!] \"scalar added to each sum.\" dst [handle! none!] \"cvMat\"]\n"\
	"bitwise-and: command [{Computes bitwise conjunction of the two arrays (dst = src1 & src2)} src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /mask m [handle!] \"cvMat\"]\n"\
	"bitwise-not: command [\"Inverts every bit of an array.\" src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /mask m [handle!] \"cvMat\"]\n"\
	"bitwise-or: command [{Calculates the per-element bit-wise disjunction of two arrays or an array and a scalar.} src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /mask m [handle!] \"cvMat\"]\n"\
	"bitwise-xor: command [{Calculates the per-element bit-wise \"exclusive or\" operation on two arrays or an array and a scalar.} src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /mask m [handle!] \"cvMat\"]\n"\
	"convertScaleAbs: command [{Scales, calculates absolute values, and converts the result to 8-bit.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" alpha [number!] \"default = 1\" beta [number!]]\n"\
	"divide: command [\"Calculates the per-element division of two arrays.\" src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /scale \"scalar factor\" s [number!] \"default = 1\"]\n"\
	"flip: command [{Flips a 2D array around vertical, horizontal, or both axes.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" flipCode [integer!] {a flag to specify how to flip the array; 0 means flipping around the x-axis and positive value (for example, 1) means flipping around y-axis. Negative value (for example, -1) means flipping around both axes}]\n"\
	"invert: command [\"Finds the inverse or pseudo-inverse of a matrix.\" src [handle!] \"Input floating-point M x N matrix.\" dst [handle! none!] {Output matrix of N x M size and the same type as src.} flags [integer!] \"Inversion method; One of DecompTypes (DECOMP_*)\"]\n"\
	"max: command [\"Calculates per-element maximum of two arrays.\" src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\"]\n"\
	"mean: command [\"Calculates an average (mean) of array elements.\" src [handle!] \"cvMat\"]\n"\
	"min: command [\"Calculates per-element minimum of two arrays.\" src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\"]\n"\
	"minMaxLoc: command [\"Finds the global minimum and maximum in an array.\" src [handle!] \"input single-channel array (cvMat)\"]\n"\
	"multiply: command [{Calculates the per-element scaled product of two arrays.} src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /scale \"scalar factor\" s [number!] \"default = 1\"]\n"\
	"normalize: command [{Calculates the per-element scaled product of two arrays.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" alpha [number!] {norm value to normalize to or the lower range boundary in case of the range normalization} beta [number!] {upper range boundary in case of the range normalization; it is not used for the norm normalization} norm_type [integer!] \"normalization type\" /mask m [handle!] \"optional operation mask\"]\n"\
	"subtract: command [{Calculates the per-element difference between two arrays.} src1 [handle!] \"cvMat\" src2 [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" /mask m [handle!] \"cvMat\"]\n"\
	"transform: command [{Performs the matrix transformation of every array element.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" m [handle!] \"transformation 2x2 or 2x3 floating-point matrix.\"]\n"\
	"convertTo: command [{Converts an array to another data type with optional scaling.} src [handle!] \"cvMat\" dst [handle! none!] \"cvMat\" type [integer! word!] {desired output matrix type or, rather, the depth since the number of channels are the same as the input has; if rtype is negative, the output matrix will have the same type as the input} alpha [number!] \"scale factor\" beta [number!] \"delta added to the scaled values\"]\n"\
	"Canny: command [\"Finds edges in an image using the Canny algorithm\" src [handle!] \"8-bit input image\" dst [handle! none!] {output edge map; single channels 8-bit image, which has the same size as image} threshold1 [number!] \"first threshold for the hysteresis procedure\" threshold2 [number!] \"second threshold for the hysteresis procedure\"]\n"\
	"startWindowThread: command []\n"\
	"imshow: command [\"Displays an image in the specified window.\" src [image! handle!] \"Image or cvMat handle\" /name \"Optional window name\" window [any-string!]]\n"\
	"pollKey: command [\"Polls for a pressed key.\"]\n"\
	"waitKey: command [\"Waits for a pressed key.\" delay [integer!] \"In millisecons; infinitely when <=0\"]\n"\
	"namedWindow: command [\"Creates a window.\" name [any-string!]]\n"\
	"resizeWindow: command [\"Resizes the window to the specified size.\" window [any-string!] size [pair!]]\n"\
	"moveWindow: command [\"Moves the window to the specified position.\" window [any-string!] pos [pair!]]\n"\
	"getWindowImageSize: command [{Provides size of image in the window (not counting tarckbars and other controls).} window [any-string!]]\n"\
	"getWindowImagePos: command [{Provides position (left-bottom) of image in the window.} window [any-string!]]\n"\
	"getWindowProperty: command [\"Provides parameters of a window.\" name [any-string!] property [integer!]]\n"\
	"setWindowProperty: command [\"Changes parameters of a window dynamically.\" name [any-string!] property [integer!] value [number!]]\n"\
	"setWindowTitle: command [\"Updates window title.\" name [any-string!] \"Name of the window\" title [any-string!] \"New title\"]\n"\
	"destroyAllWindows: command [\"Destroys all of the HighGUI windows.\"]\n"\
	"destroyWindow: command [\"Destroys the specified window.\" window [any-string!]]\n"\
	"createTrackbar: command [trackbarname [any-string!] window [any-string!] count [integer!] {Maximal position of the slider. The minimal position is always 0.} /with ctx [object!] \"Callback's context\" func [word!] \"Callback function name\"]\n"\
	"setTrackbarMax: command [\"Sets the trackbar maximum position.\" trackbar [handle!] \"cvTrackbar\" value [integer!] \"New maximum position.\"]\n"\
	"setTrackbarMin: command [\"Sets the trackbar minimum position.\" trackbar [handle!] \"cvTrackbar\" value [integer!] \"New minimum position.\"]\n"\
	"setTrackbarPos: command [\"Sets the trackbar position.\" trackbar [handle!] \"cvTrackbar\" value [integer!] \"New position.\"]\n"\
	"getTrackbarPos: command [\"Gets the trackbar position.\" trackbar [handle!] \"cvTrackbar\"]\n"\
	"selectROI: command [\"Allows users to select a ROI on the given image.\" src [image! handle!] \"Image or cvMat handle\"]\n"\
	"getTickCount: command [\"Returns the number of ticks.\"]\n"\
	"getTickFrequency: command [\"Returns the number of ticks per second.\"]\n"\
	"getNumThreads: command [{Returns the number of threads used by OpenCV for parallel regions.}]\n"\
	"getVersionString: command [\"Returns library version string.\"]\n"\
	"getBuildInformation: command [\"Returns full configuration time cmake output.\"]\n"\
	"useOptimized: command [\"Returns the status of optimized code usage.\"]\n"\
	"setUseOptimized: command [\"Enables or disables the optimized code.\" onoff [logic!]]\n"\
	"qrcode-encode: command [\"Encode string to an image.\" text [any-string!] /version \"Symbol version of QR Code range\" v [integer!] \"1 - 40\" /mode \"Encoding mode\" m [integer!] /correction {Approximate error correction level (low, medium, quartile, high)} level [integer!] \"0 - 3 (default is 0)\"]\n"\
	"qrcode-decode: command [\"Decode string from an image with a QRcode\" image [image! handle! file!] \"Image or cvMat handle\"]\n"\
	"init-words [size type depth channels binary image vector total is-submatrix width height pos-ms pos-frame pos-ratio fps fourcc frames format][CV_8U CV_8S CV_16U CV_16S CV_32S CV_32F CV_64F CV_16F CV_8UC1 CV_8SC1 CV_16UC1 CV_16SC1 CV_32SC1 CV_32FC1 CV_64FC1 CV_16FC1 CV_8UC2 CV_8SC2 CV_16UC2 CV_16SC2 CV_32SC2 CV_32FC2 CV_64FC2 CV_16FC2 CV_8UC3 CV_8SC3 CV_16UC3 CV_16SC3 CV_32SC3 CV_32FC3 CV_64FC3 CV_16FC3 CV_8UC4 CV_8SC4 CV_16UC4 CV_16SC4 CV_32SC4 CV_32FC4 CV_64FC4 CV_16FC4]\n"\
	"protect/hide 'init-words\n"\
	"\n"\
	"; imread flags..\n"\
	"IMREAD_UNCHANGED: -1\n"\
	"IMREAD_GRAYSCALE: 0\n"\
	"IMREAD_COLOR: 1\n"\
	"IMREAD_ANYDEPTH: 2\n"\
	"IMREAD_ANYCOLOR: 4\n"\
	"IMREAD_LOAD_GDAL: 8\n"\
	"IMREAD_REDUCED_GRAYSCALE_2: 16\n"\
	"IMREAD_REDUCED_COLOR_2: 17\n"\
	"IMREAD_REDUCED_GRAYSCALE_4: 32\n"\
	"IMREAD_REDUCED_COLOR_4: 33\n"\
	"IMREAD_REDUCED_GRAYSCALE_8: 64\n"\
	"IMREAD_REDUCED_COLOR_8: 65\n"\
	"IMREAD_IGNORE_ORIENTATION: 128\n"\
	"\n"\
	"; imwrite params..\n"\
	"IMWRITE_JPEG_QUALITY: 1\n"\
	"IMWRITE_JPEG_PROGRESSIVE: 2\n"\
	"IMWRITE_JPEG_OPTIMIZE: 3\n"\
	"IMWRITE_JPEG_RST_INTERVAL: 4\n"\
	"IMWRITE_JPEG_LUMA_QUALITY: 5\n"\
	"IMWRITE_JPEG_CHROMA_QUALITY: 6\n"\
	"IMWRITE_JPEG_SAMPLING_FACTOR: 7\n"\
	"IMWRITE_PNG_COMPRESSION: 16\n"\
	"IMWRITE_PNG_STRATEGY: 17\n"\
	"IMWRITE_PNG_BILEVEL: 18\n"\
	"IMWRITE_PXM_BINARY: 32\n"\
	"IMWRITE_EXR_TYPE: 48\n"\
	"IMWRITE_EXR_COMPRESSION: 49\n"\
	"IMWRITE_WEBP_QUALITY: 64\n"\
	"IMWRITE_PAM_TUPLETYPE: 128\n"\
	"IMWRITE_TIFF_RESUNIT: 256\n"\
	"IMWRITE_TIFF_XDPI: 257\n"\
	"IMWRITE_TIFF_YDPI: 258\n"\
	"IMWRITE_TIFF_COMPRESSION: 259\n"\
	"IMWRITE_JPEG2000_COMPRESSION_X1000: 272\n"\
	"\n"\
	"; ImwritePNGFlags\n"\
	"IMWRITE_PNG_STRATEGY_DEFAULT:      0\n"\
	"IMWRITE_PNG_STRATEGY_FILTERED:     1\n"\
	"IMWRITE_PNG_STRATEGY_HUFFMAN_ONLY: 2\n"\
	"IMWRITE_PNG_STRATEGY_RLE:          3\n"\
	"IMWRITE_PNG_STRATEGY_FIXED:        4\n"\
	"\n"\
	"; BorderTypes\n"\
	"BORDER_CONSTANT: 0\n"\
	"BORDER_REPLICATE: 1\n"\
	"BORDER_REFLECT: 2\n"\
	"BORDER_WRAP: 3\n"\
	"BORDER_REFLECT_101: 4\n"\
	"BORDER_TRANSPARENT: 5\n"\
	"BORDER_REFLECT101: BORDER_REFLECT_101\n"\
	"BORDER_DEFAULT: BORDER_REFLECT_101\n"\
	"BORDER_ISOLATED: 16\n"\
	"\n"\
	"; ColorConversionCodes\n"\
	"COLOR_BGR2BGRA: 0\n"\
	"COLOR_RGB2RGBA: COLOR_BGR2BGRA\n"\
	"COLOR_BGRA2BGR: 1\n"\
	"COLOR_RGBA2RGB: COLOR_BGRA2BGR\n"\
	"COLOR_BGR2RGBA: 2\n"\
	"COLOR_RGB2BGRA: COLOR_BGR2RGBA\n"\
	"COLOR_RGBA2BGR: 3\n"\
	"COLOR_BGRA2RGB: COLOR_RGBA2BGR\n"\
	"COLOR_BGR2RGB: 4\n"\
	"COLOR_RGB2BGR: COLOR_BGR2RGB\n"\
	"COLOR_BGRA2RGBA: 5\n"\
	"COLOR_RGBA2BGRA: COLOR_BGRA2RGBA\n"\
	"COLOR_BGR2GRAY: 6\n"\
	"COLOR_RGB2GRAY: 7\n"\
	"COLOR_GRAY2BGR: 8\n"\
	"COLOR_GRAY2RGB: COLOR_GRAY2BGR\n"\
	"COLOR_GRAY2BGRA: 9\n"\
	"COLOR_GRAY2RGBA: COLOR_GRAY2BGRA\n"\
	"COLOR_BGRA2GRAY: 10\n"\
	"COLOR_RGBA2GRAY: 11\n"\
	"COLOR_BGR2BGR565: 12\n"\
	"COLOR_RGB2BGR565: 13\n"\
	"COLOR_BGR5652BGR: 14\n"\
	"COLOR_BGR5652RGB: 15\n"\
	"COLOR_BGRA2BGR565: 16\n"\
	"COLOR_RGBA2BGR565: 17\n"\
	"COLOR_BGR5652BGRA: 18\n"\
	"COLOR_BGR5652RGBA: 19\n"\
	"COLOR_GRAY2BGR565: 20\n"\
	"COLOR_BGR5652GRAY: 21\n"\
	"COLOR_BGR2BGR555: 22\n"\
	"COLOR_RGB2BGR555: 23\n"\
	"COLOR_BGR5552BGR: 24\n"\
	"COLOR_BGR5552RGB: 25\n"\
	"COLOR_BGRA2BGR555: 26\n"\
	"COLOR_RGBA2BGR555: 27\n"\
	"COLOR_BGR5552BGRA: 28\n"\
	"COLOR_BGR5552RGBA: 29\n"\
	"COLOR_GRAY2BGR555: 30\n"\
	"COLOR_BGR5552GRAY: 31\n"\
	"COLOR_BGR2XYZ: 32\n"\
	"COLOR_RGB2XYZ: 33\n"\
	"COLOR_XYZ2BGR: 34\n"\
	"COLOR_XYZ2RGB: 35\n"\
	"COLOR_BGR2YCrCb: 36\n"\
	"COLOR_RGB2YCrCb: 37\n"\
	"COLOR_YCrCb2BGR: 38\n"\
	"COLOR_YCrCb2RGB: 39\n"\
	"COLOR_BGR2HSV: 40\n"\
	"COLOR_RGB2HSV: 41\n"\
	"COLOR_BGR2Lab: 44\n"\
	"COLOR_RGB2Lab: 45\n"\
	"COLOR_BGR2Luv: 50\n"\
	"COLOR_RGB2Luv: 51\n"\
	"COLOR_BGR2HLS: 52\n"\
	"COLOR_RGB2HLS: 53\n"\
	"COLOR_HSV2BGR: 54\n"\
	"COLOR_HSV2RGB: 55\n"\
	"COLOR_Lab2BGR: 56\n"\
	"COLOR_Lab2RGB: 57\n"\
	"COLOR_Luv2BGR: 58\n"\
	"COLOR_Luv2RGB: 59\n"\
	"COLOR_HLS2BGR: 60\n"\
	"COLOR_HLS2RGB: 61\n"\
	"COLOR_BGR2HSV_FULL: 66\n"\
	"COLOR_RGB2HSV_FULL: 67\n"\
	"COLOR_BGR2HLS_FULL: 68\n"\
	"COLOR_RGB2HLS_FULL: 69\n"\
	"COLOR_HSV2BGR_FULL: 70\n"\
	"COLOR_HSV2RGB_FULL: 71\n"\
	"COLOR_HLS2BGR_FULL: 72\n"\
	"COLOR_HLS2RGB_FULL: 73\n"\
	"COLOR_LBGR2Lab: 74\n"\
	"COLOR_LRGB2Lab: 75\n"\
	"COLOR_LBGR2Luv: 76\n"\
	"COLOR_LRGB2Luv: 77\n"\
	"COLOR_Lab2LBGR: 78\n"\
	"COLOR_Lab2LRGB: 79\n"\
	"COLOR_Luv2LBGR: 80\n"\
	"COLOR_Luv2LRGB: 81\n"\
	"COLOR_BGR2YUV: 82\n"\
	"COLOR_RGB2YUV: 83\n"\
	"COLOR_YUV2BGR: 84\n"\
	"COLOR_YUV2RGB: 85\n"\
	"COLOR_YUV2RGB_NV12: 90\n"\
	"COLOR_YUV2BGR_NV12: 91\n"\
	"COLOR_YUV2RGB_NV21: 92\n"\
	"COLOR_YUV2BGR_NV21: 93\n"\
	"COLOR_YUV420sp2RGB: COLOR_YUV2RGB_NV21\n"\
	"COLOR_YUV420sp2BGR: COLOR_YUV2BGR_NV21\n"\
	"COLOR_YUV2RGBA_NV12: 94\n"\
	"COLOR_YUV2BGRA_NV12: 95\n"\
	"COLOR_YUV2RGBA_NV21: 96\n"\
	"COLOR_YUV2BGRA_NV21: 97\n"\
	"COLOR_YUV420sp2RGBA: COLOR_YUV2RGBA_NV21\n"\
	"COLOR_YUV420sp2BGRA: COLOR_YUV2BGRA_NV21\n"\
	"COLOR_YUV2RGB_YV12: 98\n"\
	"COLOR_YUV2BGR_YV12: 99\n"\
	"COLOR_YUV2RGB_IYUV: 100\n"\
	"COLOR_YUV2BGR_IYUV: 101\n"\
	"COLOR_YUV2RGB_I420: COLOR_YUV2RGB_IYUV\n"\
	"COLOR_YUV2BGR_I420: COLOR_YUV2BGR_IYUV\n"\
	"COLOR_YUV420p2RGB: COLOR_YUV2RGB_YV12\n"\
	"COLOR_YUV420p2BGR: COLOR_YUV2BGR_YV12\n"\
	"COLOR_YUV2RGBA_YV12: 102\n"\
	"COLOR_YUV2BGRA_YV12: 103\n"\
	"COLOR_YUV2RGBA_IYUV: 104\n"\
	"COLOR_YUV2BGRA_IYUV: 105\n"\
	"COLOR_YUV2RGBA_I420: COLOR_YUV2RGBA_IYUV\n"\
	"COLOR_YUV2BGRA_I420: COLOR_YUV2BGRA_IYUV\n"\
	"COLOR_YUV420p2RGBA: COLOR_YUV2RGBA_YV12\n"\
	"COLOR_YUV420p2BGRA: COLOR_YUV2BGRA_YV12\n"\
	"COLOR_YUV2GRAY_420: 106\n"\
	"COLOR_YUV2GRAY_NV21: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV2GRAY_NV12: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV2GRAY_YV12: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV2GRAY_IYUV: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV2GRAY_I420: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV420sp2GRAY: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV420p2GRAY: COLOR_YUV2GRAY_420\n"\
	"COLOR_YUV2RGB_UYVY: 107\n"\
	"COLOR_YUV2BGR_UYVY: 108\n"\
	"COLOR_YUV2RGB_Y422: COLOR_YUV2RGB_UYVY\n"\
	"COLOR_YUV2BGR_Y422: COLOR_YUV2BGR_UYVY\n"\
	"COLOR_YUV2RGB_UYNV: COLOR_YUV2RGB_UYVY\n"\
	"COLOR_YUV2BGR_UYNV: COLOR_YUV2BGR_UYVY\n"\
	"COLOR_YUV2RGBA_UYVY: 111\n"\
	"COLOR_YUV2BGRA_UYVY: 112\n"\
	"COLOR_YUV2RGBA_Y422: COLOR_YUV2RGBA_UYVY\n"\
	"COLOR_YUV2BGRA_Y422: COLOR_YUV2BGRA_UYVY\n"\
	"COLOR_YUV2RGBA_UYNV: COLOR_YUV2RGBA_UYVY\n"\
	"COLOR_YUV2BGRA_UYNV: COLOR_YUV2BGRA_UYVY\n"\
	"COLOR_YUV2RGB_YUY2: 115\n"\
	"COLOR_YUV2BGR_YUY2: 116\n"\
	"COLOR_YUV2RGB_YVYU: 117\n"\
	"COLOR_YUV2BGR_YVYU: 118\n"\
	"COLOR_YUV2RGB_YUYV: COLOR_YUV2RGB_YUY2\n"\
	"COLOR_YUV2BGR_YUYV: COLOR_YUV2BGR_YUY2\n"\
	"COLOR_YUV2RGB_YUNV: COLOR_YUV2RGB_YUY2\n"\
	"COLOR_YUV2BGR_YUNV: COLOR_YUV2BGR_YUY2\n"\
	"COLOR_YUV2RGBA_YUY2: 119\n"\
	"COLOR_YUV2BGRA_YUY2: 120\n"\
	"COLOR_YUV2RGBA_YVYU: 121\n"\
	"COLOR_YUV2BGRA_YVYU: 122\n"\
	"COLOR_YUV2RGBA_YUYV: COLOR_YUV2RGBA_YUY2\n"\
	"COLOR_YUV2BGRA_YUYV: COLOR_YUV2BGRA_YUY2\n"\
	"COLOR_YUV2RGBA_YUNV: COLOR_YUV2RGBA_YUY2\n"\
	"COLOR_YUV2BGRA_YUNV: COLOR_YUV2BGRA_YUY2\n"\
	"COLOR_YUV2GRAY_UYVY: 123\n"\
	"COLOR_YUV2GRAY_YUY2: 124\n"\
	"COLOR_YUV2GRAY_Y422: COLOR_YUV2GRAY_UYVY\n"\
	"COLOR_YUV2GRAY_UYNV: COLOR_YUV2GRAY_UYVY\n"\
	"COLOR_YUV2GRAY_YVYU: COLOR_YUV2GRAY_YUY2\n"\
	"COLOR_YUV2GRAY_YUYV: COLOR_YUV2GRAY_YUY2\n"\
	"COLOR_YUV2GRAY_YUNV: COLOR_YUV2GRAY_YUY2\n"\
	"COLOR_RGBA2mRGBA: 125\n"\
	"COLOR_mRGBA2RGBA: 126\n"\
	"COLOR_RGB2YUV_I420: 127\n"\
	"COLOR_BGR2YUV_I420: 128\n"\
	"COLOR_RGB2YUV_IYUV: COLOR_RGB2YUV_I420\n"\
	"COLOR_BGR2YUV_IYUV: COLOR_BGR2YUV_I420\n"\
	"COLOR_RGBA2YUV_I420: 129\n"\
	"COLOR_BGRA2YUV_I420: 130\n"\
	"COLOR_RGBA2YUV_IYUV: COLOR_RGBA2YUV_I420\n"\
	"COLOR_BGRA2YUV_IYUV: COLOR_BGRA2YUV_I420\n"\
	"COLOR_RGB2YUV_YV12: 131\n"\
	"COLOR_BGR2YUV_YV12: 132\n"\
	"COLOR_RGBA2YUV_YV12: 133\n"\
	"COLOR_BGRA2YUV_YV12: 134\n"\
	"COLOR_BayerBG2BGR: 46\n"\
	"COLOR_BayerGB2BGR: 47\n"\
	"COLOR_BayerRG2BGR: 48\n"\
	"COLOR_BayerGR2BGR: 49\n"\
	"COLOR_BayerRGGB2BGR: COLOR_BayerBG2BGR\n"\
	"COLOR_BayerGRBG2BGR: COLOR_BayerGB2BGR\n"\
	"COLOR_BayerBGGR2BGR: COLOR_BayerRG2BGR\n"\
	"COLOR_BayerGBRG2BGR: COLOR_BayerGR2BGR\n"\
	"COLOR_BayerRGGB2RGB: COLOR_BayerBGGR2BGR\n"\
	"COLOR_BayerGRBG2RGB: COLOR_BayerGBRG2BGR\n"\
	"COLOR_BayerBGGR2RGB: COLOR_BayerRGGB2BGR\n"\
	"COLOR_BayerGBRG2RGB: COLOR_BayerGRBG2BGR\n"\
	"COLOR_BayerBG2RGB: COLOR_BayerRG2BGR\n"\
	"COLOR_BayerGB2RGB: COLOR_BayerGR2BGR\n"\
	"COLOR_BayerRG2RGB: COLOR_BayerBG2BGR\n"\
	"COLOR_BayerGR2RGB: COLOR_BayerGB2BGR\n"\
	"COLOR_BayerBG2GRAY: 86\n"\
	"COLOR_BayerGB2GRAY: 87\n"\
	"COLOR_BayerRG2GRAY: 88\n"\
	"COLOR_BayerGR2GRAY: 89\n"\
	"COLOR_BayerRGGB2GRAY: COLOR_BayerBG2GRAY\n"\
	"COLOR_BayerGRBG2GRAY: COLOR_BayerGB2GRAY\n"\
	"COLOR_BayerBGGR2GRAY: COLOR_BayerRG2GRAY\n"\
	"COLOR_BayerGBRG2GRAY: COLOR_BayerGR2GRAY\n"\
	"COLOR_BayerBG2BGR_VNG: 62\n"\
	"COLOR_BayerGB2BGR_VNG: 63\n"\
	"COLOR_BayerRG2BGR_VNG: 64\n"\
	"COLOR_BayerGR2BGR_VNG: 65\n"\
	"COLOR_BayerRGGB2BGR_VNG: COLOR_BayerBG2BGR_VNG\n"\
	"COLOR_BayerGRBG2BGR_VNG: COLOR_BayerGB2BGR_VNG\n"\
	"COLOR_BayerBGGR2BGR_VNG: COLOR_BayerRG2BGR_VNG\n"\
	"COLOR_BayerGBRG2BGR_VNG: COLOR_BayerGR2BGR_VNG\n"\
	"COLOR_BayerRGGB2RGB_VNG: COLOR_BayerBGGR2BGR_VNG\n"\
	"COLOR_BayerGRBG2RGB_VNG: COLOR_BayerGBRG2BGR_VNG\n"\
	"COLOR_BayerBGGR2RGB_VNG: COLOR_BayerRGGB2BGR_VNG\n"\
	"COLOR_BayerGBRG2RGB_VNG: COLOR_BayerGRBG2BGR_VNG\n"\
	"COLOR_BayerBG2RGB_VNG: COLOR_BayerRG2BGR_VNG\n"\
	"COLOR_BayerGB2RGB_VNG: COLOR_BayerGR2BGR_VNG\n"\
	"COLOR_BayerRG2RGB_VNG: COLOR_BayerBG2BGR_VNG\n"\
	"COLOR_BayerGR2RGB_VNG: COLOR_BayerGB2BGR_VNG\n"\
	"COLOR_BayerBG2BGR_EA: 135\n"\
	"COLOR_BayerGB2BGR_EA: 136\n"\
	"COLOR_BayerRG2BGR_EA: 137\n"\
	"COLOR_BayerGR2BGR_EA: 138\n"\
	"COLOR_BayerRGGB2BGR_EA: COLOR_BayerBG2BGR_EA\n"\
	"COLOR_BayerGRBG2BGR_EA: COLOR_BayerGB2BGR_EA\n"\
	"COLOR_BayerBGGR2BGR_EA: COLOR_BayerRG2BGR_EA\n"\
	"COLOR_BayerGBRG2BGR_EA: COLOR_BayerGR2BGR_EA\n"\
	"COLOR_BayerRGGB2RGB_EA: COLOR_BayerBGGR2BGR_EA\n"\
	"COLOR_BayerGRBG2RGB_EA: COLOR_BayerGBRG2BGR_EA\n"\
	"COLOR_BayerBGGR2RGB_EA: COLOR_BayerRGGB2BGR_EA\n"\
	"COLOR_BayerGBRG2RGB_EA: COLOR_BayerGRBG2BGR_EA\n"\
	"COLOR_BayerBG2RGB_EA: COLOR_BayerRG2BGR_EA\n"\
	"COLOR_BayerGB2RGB_EA: COLOR_BayerGR2BGR_EA\n"\
	"COLOR_BayerRG2RGB_EA: COLOR_BayerBG2BGR_EA\n"\
	"COLOR_BayerGR2RGB_EA: COLOR_BayerGB2BGR_EA\n"\
	"COLOR_BayerBG2BGRA: 139\n"\
	"COLOR_BayerGB2BGRA: 140\n"\
	"COLOR_BayerRG2BGRA: 141\n"\
	"COLOR_BayerGR2BGRA: 142\n"\
	"COLOR_BayerRGGB2BGRA: COLOR_BayerBG2BGRA\n"\
	"COLOR_BayerGRBG2BGRA: COLOR_BayerGB2BGRA\n"\
	"COLOR_BayerBGGR2BGRA: COLOR_BayerRG2BGRA\n"\
	"COLOR_BayerGBRG2BGRA: COLOR_BayerGR2BGRA\n"\
	"COLOR_BayerRGGB2RGBA: COLOR_BayerBGGR2BGRA\n"\
	"COLOR_BayerGRBG2RGBA: COLOR_BayerGBRG2BGRA\n"\
	"COLOR_BayerBGGR2RGBA: COLOR_BayerRGGB2BGRA\n"\
	"COLOR_BayerGBRG2RGBA: COLOR_BayerGRBG2BGRA\n"\
	"COLOR_BayerBG2RGBA: COLOR_BayerRG2BGRA\n"\
	"COLOR_BayerGB2RGBA: COLOR_BayerGR2BGRA\n"\
	"COLOR_BayerRG2RGBA: COLOR_BayerBG2BGRA\n"\
	"COLOR_BayerGR2RGBA: COLOR_BayerGB2BGRA\n"\
	"\n"\
	"; VideoCaptureProperties\n"\
	"CAP_PROP_POS_MSEC: 0\n"\
	"CAP_PROP_POS_FRAMES: 1\n"\
	"CAP_PROP_POS_AVI_RATIO: 2\n"\
	"CAP_PROP_FRAME_WIDTH: 3\n"\
	"CAP_PROP_FRAME_HEIGHT: 4\n"\
	"CAP_PROP_FPS: 5\n"\
	"CAP_PROP_FOURCC: 6\n"\
	"CAP_PROP_FRAME_COUNT: 7\n"\
	"CAP_PROP_FORMAT: 8\n"\
	"CAP_PROP_MODE: 9\n"\
	"CAP_PROP_BRIGHTNESS: 10\n"\
	"CAP_PROP_CONTRAST: 11\n"\
	"CAP_PROP_SATURATION: 12\n"\
	"CAP_PROP_HUE: 13\n"\
	"CAP_PROP_GAIN: 14\n"\
	"CAP_PROP_EXPOSURE: 15\n"\
	"CAP_PROP_CONVERT_RGB: 16\n"\
	"CAP_PROP_WHITE_BALANCE_BLUE_U: 17\n"\
	"CAP_PROP_RECTIFICATION: 18\n"\
	"CAP_PROP_MONOCHROME: 19\n"\
	"CAP_PROP_SHARPNESS: 20\n"\
	"CAP_PROP_AUTO_EXPOSURE: 21\n"\
	"CAP_PROP_GAMMA: 22\n"\
	"CAP_PROP_TEMPERATURE: 23\n"\
	"CAP_PROP_TRIGGER: 24\n"\
	"CAP_PROP_TRIGGER_DELAY: 25\n"\
	"CAP_PROP_WHITE_BALANCE_RED_V: 26\n"\
	"CAP_PROP_ZOOM: 27\n"\
	"CAP_PROP_FOCUS: 28\n"\
	"CAP_PROP_GUID: 29\n"\
	"CAP_PROP_ISO_SPEED: 30\n"\
	"CAP_PROP_BACKLIGHT: 32\n"\
	"CAP_PROP_PAN: 33\n"\
	"CAP_PROP_TILT: 34\n"\
	"CAP_PROP_ROLL: 35\n"\
	"CAP_PROP_IRIS: 36\n"\
	"CAP_PROP_SETTINGS: 37\n"\
	"CAP_PROP_BUFFERSIZE: 38\n"\
	"CAP_PROP_AUTOFOCUS: 39\n"\
	"CAP_PROP_SAR_NUM: 40\n"\
	"CAP_PROP_SAR_DEN: 41\n"\
	"CAP_PROP_BACKEND: 42\n"\
	"CAP_PROP_CHANNEL: 43\n"\
	"CAP_PROP_AUTO_WB: 44\n"\
	"CAP_PROP_WB_TEMPERATURE: 45\n"\
	"CAP_PROP_CODEC_PIXEL_FORMAT: 46\n"\
	"CAP_PROP_BITRATE: 47\n"\
	"CAP_PROP_ORIENTATION_META: 48\n"\
	"CAP_PROP_ORIENTATION_AUTO: 49\n"\
	"CAP_PROP_OPEN_TIMEOUT_MSEC: 53\n"\
	"CAP_PROP_READ_TIMEOUT_MSEC: 54\n"\
	"\n"\
	"; WindowPropertyFlags\n"\
	"WND_PROP_FULLSCREEN: 0\n"\
	"WND_PROP_AUTOSIZE: 1\n"\
	"WND_PROP_ASPECT_RATIO: 2\n"\
	"WND_PROP_OPENGL: 3\n"\
	"WND_PROP_VISIBLE: 4\n"\
	"WND_PROP_TOPMOST: 5\n"\
	"; WindowFlags\n"\
	"WINDOW_NORMAL: 0\n"\
	"WINDOW_AUTOSIZE: 1  ;0x00000001\n"\
	"WINDOW_OPENGL: 4096 ;0x00001000\n"\
	"WINDOW_FULLSCREEN: 1\n"\
	"WINDOW_FREERATIO: 256 ;0x00000100\n"\
	"WINDOW_KEEPRATIO: 0\n"\
	"WINDOW_GUI_EXPANDED: 0\n"\
	"WINDOW_GUI_NORMAL: 16\n"\
	"\n"\
	"; MatProperties\n"\
	"MAT_SIZE: 1\n"\
	"MAT_TYPE: 2\n"\
	"MAT_DEPTH: 3\n"\
	"MAT_CHANNELS: 4\n"\
	"MAT_BINARY: 5\n"\
	"MAT_IMAGE: 6\n"\
	"MAT_VECTOR: 7\n"\
	"\n"\
	"; InterpolationFlags\n"\
	"INTER_NEAREST: 0\n"\
	"INTER_LINEAR: 1\n"\
	"INTER_CUBIC: 2\n"\
	"INTER_AREA: 3\n"\
	"INTER_LANCZOS4: 4\n"\
	"INTER_LINEAR_EXACT: 5\n"\
	"INTER_NEAREST_EXACT: 6\n"\
	"\n"\
	"; ThresholdTypes\n"\
	"THRESH_BINARY: 0\n"\
	"THRESH_BINARY_INV: 1\n"\
	"THRESH_TRUNC: 2\n"\
	"THRESH_TOZERO: 3\n"\
	"THRESH_TOZERO_INV: 4\n"\
	"THRESH_MASK: 7\n"\
	"THRESH_OTSU: 8\n"\
	"THRESH_TRIANGLE: 16\n"\
	"\n"\
	"; Depths:\n"\
	"CV_8U:   0\n"\
	"CV_8S:   1\n"\
	"CV_16U:  2\n"\
	"CV_16S:  3\n"\
	"CV_32S:  4\n"\
	"CV_32F:  5\n"\
	"CV_64F:  6\n"\
	"\n"\
	"; DecompTypes:\n"\
	"DECOMP_LU: 0\n"\
	"DECOMP_SVD: 1\n"\
	"DECOMP_EIG: 2\n"\
	"DECOMP_CHOLESKY: 3\n"\
	"DECOMP_QR: 4\n"\
	"DECOMP_NORMAL: 16\n"\
	"\n"\
	"; MorphShapes:\n"\
	"MORPH_RECT: 0\n"\
	"MORPH_CROSS: 1\n"\
	"MORPH_ELLIPSE: 2\n"\
	"\n"\
	"; MorphTypes:\n"\
	"MORPH_ERODE: 0\n"\
	"MORPH_DILATE: 1\n"\
	"MORPH_OPEN: 2\n"\
	"MORPH_CLOSE: 3\n"\
	"MORPH_GRADIENT: 4\n"\
	"MORPH_TOPHAT: 5\n"\
	"MORPH_BLACKHAT: 6\n"\
	"MORPH_HITMISS: 7\n"\
	"\n"\
	"; NormTypes:\n"\
	"NORM_INF: 1\n"\
	"NORM_L1: 2\n"\
	"NORM_L2: 4\n"\
	"NORM_L2SQR: 5\n"\
	"NORM_HAMMING: 6\n"\
	"NORM_HAMMING2: 7\n"\
	"NORM_TYPE_MASK: 7\n"\
	"NORM_RELATIVE: 8\n"\
	"NORM_MINMAX: 32\n"\
	"\n"\
	"; ColormapTypes:\n"\
	"COLORMAP_AUTUMN: 0\n"\
	"COLORMAP_BONE: 1\n"\
	"COLORMAP_JET: 2\n"\
	"COLORMAP_WINTER: 3\n"\
	"COLORMAP_RAINBOW: 4\n"\
	"COLORMAP_OCEAN: 5\n"\
	"COLORMAP_SUMMER: 6\n"\
	"COLORMAP_SPRING: 7\n"\
	"COLORMAP_COOL: 8\n"\
	"COLORMAP_HSV: 9\n"\
	"COLORMAP_PINK: 10\n"\
	"COLORMAP_HOT: 11\n"\
	"COLORMAP_PARULA: 12\n"\
	"COLORMAP_MAGMA: 13\n"\
	"COLORMAP_INFERNO: 14\n"\
	"COLORMAP_PLASMA: 15\n"\
	"COLORMAP_VIRIDIS: 16\n"\
	"COLORMAP_CIVIDIS: 17\n"\
	"COLORMAP_TWILIGHT: 18\n"\
	"COLORMAP_TWILIGHT_SHIFTED: 19\n"\
	"COLORMAP_TURBO: 20\n"\
	"COLORMAP_DEEPGREEN: 21\n"\
	"\n"

#ifdef  USE_TRACES
#include <stdio.h>
#define debug_print(fmt, ...) do { printf(fmt, __VA_ARGS__); } while (0)
#define trace(str) puts(str)
#else
#define debug_print(fmt, ...)
#define trace(str) 
#endif

