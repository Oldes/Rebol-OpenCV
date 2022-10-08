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
	CMD_OPENCV_BILATERALFILTER,
	CMD_OPENCV_BLUR,
	CMD_OPENCV_CVTCOLOR,
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
int cmd_bilateralFilter(RXIFRM *frm, void *ctx);
int cmd_blur(RXIFRM *frm, void *ctx);
int cmd_cvtColor(RXIFRM *frm, void *ctx);
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
	cmd_bilateralFilter,
	cmd_blur,
	cmd_cvtColor,
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
	"bilateralFilter: command [\"Applies the bilateral filter to an image.\" image [image! handle!] diameter [integer!] sigmaColor [decimal!] sigmaSpace [decimal!] /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"blur: command [\"Blurs an image using the normalized box filter.\" image [image! handle!] size [pair!] \"blurring kernel size\" /border {border mode used to extrapolate pixels outside of the image} type [integer!] \"one of: [0 1 2 4 5 16]\"]\n"\
	"cvtColor: command [\"Converts an image from one color space to another.\" image [image! handle!] code [integer!]]\n"\
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
