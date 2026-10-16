//
// Project: Rebol/OpenCV extension
// SPDX-License-Identifier: MIT
// =============================================================================
// Entry points of the OpenCV extension.
//
//   REB_EXT defined ... standalone opencv-<arch>.rebx
//   REB_EXT absent .... compiled into the host
//
// One-time setup lives in Opencv_Init(), called from the generated `_init`
// command when the module body evaluates - not from the entry points.
//

#include "opencv.h"

#ifdef REB_EXT
// Standalone builds are their own binary and must supply the storage.
// Embedded builds use host-lib.c's definition, declared extern by reb-lib.h.
RL_LIB *RL;
#endif

static char *init_block = OPENCV_EXT_INIT_CODE;

// Symbols of the registered handle types. Declared in the generated header
// (from the spec's `c-header:`), defined here.
REBCNT Handle_cvMat           = 0;
REBCNT Handle_cvVideoCapture  = 0;
REBCNT Handle_cvVideoWriter   = 0;
REBCNT Handle_cvTrackbar      = 0;
REBCNT Handle_cvMouseCallback = 0;


// Returns plain TRUE/FALSE, NOT an RXR_* code: RXR_FALSE is 3, which is
// truthy in C. The generated handler maps the result onto RXR_TRUE/RXR_FALSE.
int Opencv_Init(void) {
	REBHSP spec;
	CLEARS(&spec);

	// Every free callback takes the HOB, not the raw data pointer.
	spec.flags = HANDLE_REQUIRES_HOB_ON_FREE;
	spec.mold  = Common_mold;

	// The pointer to the OpenCV object is stored in the data block.
	spec.size     = sizeof(void*);
	spec.free     = cvMat_free;
	spec.get_path = cvMat_get_path;
	Handle_cvMat  = RL_REGISTER_HANDLE_SPEC(cb_cast("cvMat"), &spec);
	if (!Handle_cvMat) return FALSE;

	spec.free     = cvVideoCapture_free;
	spec.get_path = cvVideoCapture_get_path;
	Handle_cvVideoCapture = RL_REGISTER_HANDLE_SPEC(cb_cast("cvVideoCapture"), &spec);
	if (!Handle_cvVideoCapture) return FALSE;

	spec.free     = cvVideoWriter_free;
	spec.get_path = NULL;
	Handle_cvVideoWriter = RL_REGISTER_HANDLE_SPEC(cb_cast("cvVideoWriter"), &spec);
	if (!Handle_cvVideoWriter) return FALSE;

	// These two keep their whole state inline in the data block.
	spec.size     = sizeof(CTX_TRACKBAR);
	spec.free     = cvTrackbar_free;
	spec.get_path = NULL;
	Handle_cvTrackbar = RL_REGISTER_HANDLE_SPEC(cb_cast("cvTrackbar"), &spec);
	if (!Handle_cvTrackbar) return FALSE;

	spec.size     = sizeof(CTX_MOUSECALLBACK);
	spec.free     = cvMouseCallback_free;
	spec.get_path = cvMouseCallback_get_path;
	Handle_cvMouseCallback = RL_REGISTER_HANDLE_SPEC(cb_cast("cvMouseCallback"), &spec);
	if (!Handle_cvMouseCallback) return FALSE;

	return TRUE;
}


#ifdef REB_EXT

/***********************************************************************
**  Standalone extension library
***********************************************************************/

RXIEXT const char *RX_Init(int opts, RL_LIB *lib) {
	REBYTE ver[8];
	RL = lib;
	RL_VERSION(ver);

	if (MIN_REBOL_VERSION > VERSION(ver[1], ver[2], ver[3])) {
		debug_print("Needs at least Rebol v%i.%i.%i!\n", MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD);
		return 0;
	}
	if (!CHECK_STRUCT_ALIGN) {
		trace("CHECK_STRUCT_ALIGN failed!");
		return 0;
	}
	return init_block;
}

RXIEXT int RX_Quit(int opts) {
	return 0;
}

// Reports the RL_API ABI this was built against, so `load-extension`
// can refuse an incompatible host. An absent symbol means ABI 0.
RXIEXT int RX_Abi(void) {
	return RL_ABI_VERSION;
}

// Resolved by name, so the spelling is fixed. The bounds-checked
// dispatcher is generated into gen-opencv.c.
RXIEXT int RX_Call(int cmd, RXIFRM *frm, void *ctx) {
	return Opencv_RX_Call(cmd, frm, ctx);
}

#endif
