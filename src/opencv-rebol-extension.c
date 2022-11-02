//
// OpenCV Rebol extension
// ====================================
// Use on your own risc!

#include "opencv-rebol-extension.h"


RL_LIB *RL; // Link back to reb-lib from embedded extensions

//==== Globals ===============================//
u32*   ext_cmd_words;
u32*   ext_arg_words;
REBCNT Handle_cvVideoCapture;
REBCNT Handle_cvVideoWriter;
REBCNT Handle_cvMat;
REBCNT Handle_cvTrackbar;

REBDEC doubles[DOUBLE_BUFFER_SIZE];
RXIARG arg[ARG_BUFFER_SIZE];
extern MyCommandPointer Command[];
//============================================//

static const char* init_block = OPENCV_EXT_INIT_CODE;

int cmd_init_words(RXIFRM *frm, void *ctx) {
	ext_cmd_words = RL_MAP_WORDS(RXA_SERIES(frm,1));
	ext_arg_words = RL_MAP_WORDS(RXA_SERIES(frm,2));
	return RXR_NONE;
}

extern void* releaseVideoCapture(void* cls);
extern void* releaseVideoWriter(void* cls);
extern void* releaseMat(void* cls);
extern void* releaseTrackbar(void* cls);


RXIEXT const char *RX_Init(int opts, RL_LIB *lib) {
	RL = lib;
	REBYTE ver[8];
	RL_VERSION(ver);
	debug_print("RXinit opencv-extension; Rebol v%i.%i.%i\n", ver[1], ver[2], ver[3]);

	if (MIN_REBOL_VERSION > VERSION(ver[1], ver[2], ver[3])) {
		debug_print("Needs at least Rebol v%i.%i.%i!\n", MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD);
		return 0;
	}
	if (!CHECK_STRUCT_ALIGN) {
		trace("CHECK_STRUCT_ALIGN failed!");
		return 0;
	}
	Handle_cvVideoCapture = RL_REGISTER_HANDLE((REBYTE*)"cvVideoCapture", sizeof(void*), releaseVideoCapture);
	Handle_cvVideoWriter  = RL_REGISTER_HANDLE((REBYTE*)"cvVideoWriter",  sizeof(void*), releaseVideoWriter);
	//Handle_cvMat          = RL_REGISTER_HANDLE((REBYTE*)"cvMat", sizeof(CTX_MAT), releaseMat);
	Handle_cvMat          = RL_REGISTER_HANDLE((REBYTE*)"cvMat", sizeof(void*), releaseMat);
	Handle_cvTrackbar     = RL_REGISTER_HANDLE((REBYTE*)"cvTrackbar", sizeof(CTX_TRACKBAR), releaseTrackbar);
	return init_block;
}


RXIEXT int RX_Call(int cmd, RXIFRM *frm, void *ctx) {
	return Command[cmd](frm, ctx);
}
