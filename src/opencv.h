//
// Project: Rebol/OpenCV extension
// SPDX-License-Identifier: MIT
// =============================================================================
// Private declarations of the OpenCV extension.
//
// Replaces the old common.h. Everything it duplicated (API_EXPORT, RL_API,
// debug_print/trace, APPEND_STRING, the COMMAND macro) now comes from
// reb-ext-common.h, part of the amalgamated rebol-extension.h.
//

#ifndef REBOL_OPENCV_EXT_H
#define REBOL_OPENCV_EXT_H

#include <stdlib.h>
#include <math.h>

// The generated header is plain C - its command declarations, the dispatch
// table and the word arrays must keep C linkage when seen from C++.
#ifdef __cplusplus
extern "C" {
#endif
#include "gen-opencv.h"
#ifdef __cplusplus
}
// Command definitions in the .cpp file must have C linkage as well.
#undef  COMMAND
#define COMMAND extern "C" int
#endif

#ifdef __cplusplus
# define CV_STRING cv::String
#else
# define CV_STRING void
#endif

// cvMat, cvVideoCapture and cvVideoWriter handles hold a pointer to the
// OpenCV object INSIDE their data block (spec size is sizeof(void*)), so the
// block allocated by Rebol is never replaced or freed by us.
#define HOB_PTR(hob)      (*(void**)((hob)->data))

typedef struct ctx_trackbar {
	int         value;
	CV_STRING  *name;
	CV_STRING  *window;
	RXICBI     *cbi;
	RXIARG     *args;
} CTX_TRACKBAR;

typedef struct ctx_mouseCallback {
	int         x;
	int         y;
	int         flags;
	CV_STRING  *window;
	RXICBI     *cbi;
	RXIARG     *args;
} CTX_MOUSECALLBACK;

// A handle argument of the given type which was not released yet.
// (FRM_IS_HANDLE from reb-ext-common.h checks only the type.)
#define ARG_Is_Live_Handle(n, t) \
	(FRM_IS_HANDLE(n, t) && IS_USED_HOB(RXA_HANDLE_CONTEXT(frm, n)))

#endif // REBOL_OPENCV_EXT_H
