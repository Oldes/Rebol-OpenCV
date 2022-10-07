//
// auto-generated file, do not modify!
//

#include "common.h"

#define MIN_REBOL_VER 3
#define MIN_REBOL_REV 5
#define MIN_REBOL_UPD 4
#define VERSION(a, b, c) (a << 16) + (b << 8) + c
#define MIN_REBOL_VERSION VERSION(MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD)

enum ext_commands {
	CMD_OPENCV_INIT_WORDS,
	CMD_OPENCV_TEST,
	CMD_OPENCV_STARTWINDOWTHREAD,
	CMD_OPENCV_IMREAD,
	CMD_OPENCV_IMWRITE,
	CMD_OPENCV_IMSHOW,
	CMD_OPENCV_BLUR,
	CMD_OPENCV_POLLKEY,
	CMD_OPENCV_WAITKEY,
	CMD_OPENCV_NAMEDWINDOW,
	CMD_OPENCV_RESIZEWINDOW,
	CMD_OPENCV_MOVEWINDOW,
	CMD_OPENCV_DESTROYALLWINDOWS,
	CMD_OPENCV_VIDEOCAPTURE,
	CMD_OPENCV_READ,
	CMD_OPENCV_FREE,
};
enum ext_cmd_words {W_OPENCV_CMD_0,
};
enum ext_arg_words {W_OPENCV_ARG_0,
};


int cmd_init_words(RXIFRM *frm, void *ctx);
int cmd_test(RXIFRM *frm, void *ctx);
int cmd_startWindowThread(RXIFRM *frm, void *ctx);
int cmd_imread(RXIFRM *frm, void *ctx);
int cmd_imwrite(RXIFRM *frm, void *ctx);
int cmd_imshow(RXIFRM *frm, void *ctx);
int cmd_blur(RXIFRM *frm, void *ctx);
int cmd_pollKey(RXIFRM *frm, void *ctx);
int cmd_waitKey(RXIFRM *frm, void *ctx);
int cmd_namedWindow(RXIFRM *frm, void *ctx);
int cmd_resizeWindow(RXIFRM *frm, void *ctx);
int cmd_moveWindow(RXIFRM *frm, void *ctx);
int cmd_destroyAllWindows(RXIFRM *frm, void *ctx);
int cmd_VideoCapture(RXIFRM *frm, void *ctx);
int cmd_read(RXIFRM *frm, void *ctx);
int cmd_free(RXIFRM *frm, void *ctx);

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);
MyCommandPointer Command[] = {
	cmd_init_words,
	cmd_test,
	cmd_startWindowThread,
	cmd_imread,
	cmd_imwrite,
	cmd_imshow,
	cmd_blur,
	cmd_pollKey,
	cmd_waitKey,
	cmd_namedWindow,
	cmd_resizeWindow,
	cmd_moveWindow,
	cmd_destroyAllWindows,
	cmd_VideoCapture,
	cmd_read,
	cmd_free,
};

#define OPENCV_EXT_INIT_CODE \
	"REBOL [Title: {Rebol OpenCV Extension} Type: module Exports: []]\n"\
	"init-words: command [cmd-words [block!] arg-words [block!]]\n"\
	"test: command [\"Simple OpenCV test\"]\n"\
	"startWindowThread: command []\n"\
	"imread: command [src [file!]]\n"\
	"imwrite: command [\"Saves an image to a specified file.\" name [any-string!] image [image! handle!] /with \"Format-specific parameters encoded as pairs\" params [block!] \"integer pairs (words are resolved)\"]\n"\
	"imshow: command [\"Displays an image in the specified window.\" src [image! handle!] /name \"Optional window name\" window [any-string!]]\n"\
	"blur: command [\"Blurs an image using the normalized box filter.\" src [image! handle!] size [pair!] \"blurring kernel size\" /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"pollKey: command [\"Polls for a pressed key.\"]\n"\
	"waitKey: command [\"Waits for a pressed key.\" delay [integer!] \"In millisecons; infinitely when <=0\"]\n"\
	"namedWindow: command [\"Creates a window.\" name [any-string!]]\n"\
	"resizeWindow: command [\"Resizes the window to the specified size.\" window [any-string!] size [pair!]]\n"\
	"moveWindow: command [\"Moves the window to the specified position.\" window [any-string!] pos [pair!]]\n"\
	"destroyAllWindows: command [\"Destroys all of the HighGUI windows.\"]\n"\
	"VideoCapture: command [\"Initialize new VideoCapture class\" src [integer! file!]]\n"\
	"read: command [\"Grabs, decodes and returns the next video frame\" src [handle!] \"VideoCapture\" /into \"Optional existing Mat handle\" dst [handle!] \"Mat\"]\n"\
	"free: command [\"Release VideoCapture or Mat handle\" class [handle!]]\n"\
	"init-words words: [] []\n"\
	"protect/hide 'init-words\n"\
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
