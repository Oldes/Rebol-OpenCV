//
// OpenCV Rebol extension
// ====================================
// Use on your own risc!

#include "opencv-rebol-extension.h"


RL_LIB *RL; // Link back to reb-lib from embedded extensions

//==== Globals ===============================//
u32* arg_words;
u32* type_words;
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
	arg_words = RL_MAP_WORDS(RXA_SERIES(frm,1));
	type_words = RL_MAP_WORDS(RXA_SERIES(frm,2));
	return RXR_NONE;
}

extern void* releaseVideoCapture(void* cls);
extern void* releaseVideoWriter(void* cls);
extern void* releaseMat(void* cls);
extern void* releaseTrackbar(void* cls);


int Common_mold(REBHOB *hob, REBSER *ser);

int cvMat_free(void* hndl);
int cvMat_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg);
int cvMat_set_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg);

int cvVideoCapture_free(void* hndl);
int cvVideoCapture_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg);
int cvVideoCapture_set_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg);

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

	REBHSP spec;
	spec.mold = Common_mold;

	spec.size      = sizeof(void*);
	//spec.flags     = HANDLE_REQUIRES_HOB_ON_FREE;
	spec.flags = 0;
	spec.free      = cvMat_free;
	spec.get_path  = cvMat_get_path;
	//spec.set_path  = cvMat_Set_path;
	Handle_cvMat  = RL_REGISTER_HANDLE_SPEC((REBYTE*)"cvMat", &spec);

	spec.free      = cvVideoCapture_free;
	spec.get_path  = cvVideoCapture_get_path;
	//spec.set_path  = cvVideoCapture_Set_path;
	Handle_cvVideoCapture  = RL_REGISTER_HANDLE_SPEC((REBYTE*)"cvVideoCapture", &spec);

//	Handle_cvVideoCapture = RL_REGISTER_HANDLE((REBYTE*)"cvVideoCapture", sizeof(void*), releaseVideoCapture);
	Handle_cvVideoWriter  = RL_REGISTER_HANDLE((REBYTE*)"cvVideoWriter",  sizeof(void*), releaseVideoWriter);
	//Handle_cvMat          = RL_REGISTER_HANDLE((REBYTE*)"cvMat", sizeof(CTX_MAT), releaseMat);
//	Handle_cvMat          = RL_REGISTER_HANDLE((REBYTE*)"cvMat", sizeof(void*), cvMat_free);
	Handle_cvTrackbar     = RL_REGISTER_HANDLE((REBYTE*)"cvTrackbar", sizeof(CTX_TRACKBAR), releaseTrackbar);
	return init_block;
}


RXIEXT int RX_Call(int cmd, RXIFRM *frm, void *ctx) {
	return Command[cmd](frm, ctx);
}
