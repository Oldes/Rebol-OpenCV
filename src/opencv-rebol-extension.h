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
	CMD_OPENCV_IMREAD,
	CMD_OPENCV_IMSHOW,
	CMD_OPENCV_POLLKEY,
	CMD_OPENCV_WAITKEY,
	CMD_OPENCV_NAMEDWINDOW,
	CMD_OPENCV_DESTROYALLWINDOWS,
};
enum ext_cmd_words {W_OPENCV_CMD_0,
};
enum ext_arg_words {W_OPENCV_ARG_0,
};


int cmd_init_words(RXIFRM *frm, void *ctx);
int cmd_test(RXIFRM *frm, void *ctx);
int cmd_imread(RXIFRM *frm, void *ctx);
int cmd_imshow(RXIFRM *frm, void *ctx);
int cmd_pollKey(RXIFRM *frm, void *ctx);
int cmd_waitKey(RXIFRM *frm, void *ctx);
int cmd_namedWindow(RXIFRM *frm, void *ctx);
int cmd_destroyAllWindows(RXIFRM *frm, void *ctx);

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);
MyCommandPointer Command[] = {
	cmd_init_words,
	cmd_test,
	cmd_imread,
	cmd_imshow,
	cmd_pollKey,
	cmd_waitKey,
	cmd_namedWindow,
	cmd_destroyAllWindows,
};

#define OPENCV_EXT_INIT_CODE \
	"REBOL [Title: {Rebol OpenCV Extension} Type: module Exports: []]\n"\
	"init-words: command [cmd-words [block!] arg-words [block!]]\n"\
	"test: command [\"Simple OpenCV test\"]\n"\
	"imread: command [src [file!]]\n"\
	"imshow: command [\"Displays an image in the specified window.\" src [image!] /name \"Optional window name\" window [any-string!]]\n"\
	"pollKey: command [\"Polls for a pressed key.\"]\n"\
	"waitKey: command [\"Waits for a pressed key.\" delay [integer!] \"In millisecons; infinitely when <=0\"]\n"\
	"namedWindow: command [\"Creates a window.\" name [any-string!]]\n"\
	"destroyAllWindows: command [\"Destroys all of the HighGUI windows.\"]\n"\
	"init-words words: [] []\n"\
	"protect/hide 'init-words\n"\
