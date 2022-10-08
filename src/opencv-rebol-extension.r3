REBOL [
	title: "OpenCV module builder"
	type: module
]

cmd-words: []
arg-words: []

commands: [
	; init-words is internal extension initialization (for mapping Rebol words to local equivalents)
	init-words: [cmd-words [block!] arg-words [block!]]
	;--------------------------

	test: [{Simple OpenCV test}]

	startWindowThread: []

	imread: [src [file!]]
	imwrite: [
		"Saves an image to a specified file."
		name [any-string!]
		image [image! handle!]
		/with "Format-specific parameters encoded as pairs"
		params [block!] "integer pairs (words are resolved)"
	]
	imshow: [
		"Displays an image in the specified window."
		src [image! handle!]
		/name "Optional window name"
		 window [any-string!]
	]

	bilateralFilter: [
		"Applies the bilateral filter to an image."
		image [image! handle!]
		diameter [integer!]
		sigmaColor [decimal!]
		sigmaSpace [decimal!]
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]

	blur: [
		"Blurs an image using the normalized box filter."
		image [image! handle!]
		size [pair!] "blurring kernel size"
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	

	pollKey: ["Polls for a pressed key."]
	waitKey: ["Waits for a pressed key." delay [integer!] "In millisecons; infinitely when <=0"]
	namedWindow: ["Creates a window." name [any-string!]]
	resizeWindow: [
		"Resizes the window to the specified size."
		window [any-string!]
		size   [pair!]
	]
	moveWindow: [
		"Moves the window to the specified position."
		window [any-string!]
		pos    [pair!]
	]
	destroyAllWindows: ["Destroys all of the HighGUI windows."]


	VideoCapture: [
		"Initialize new VideoCapture class"
		src [integer! file!]
	]
	read: [
		"Grabs, decodes and returns the next video frame"
		src [handle!] "VideoCapture"
		/into "Optional existing Mat handle"
		 dst [handle!] "Mat"
	]
	free: [
		"Release VideoCapture or Mat handle"
		class [handle!]
	]
	;--------------------------
]


header: {REBOL [Title: {Rebol OpenCV Extension} Type: module Exports: []]}
enum-commands:  "enum ext_commands {"
enum-cmd-words: "enum ext_cmd_words {W_OPENCV_CMD_0,"
enum-arg-words: "enum ext_arg_words {W_OPENCV_ARG_0,"
cmd-declares: ""
cmd-dispatch: ""
uname: none

foreach word cmd-words [
	word: uppercase form word
	replace/all word #"-" #"_"
	append enum-cmd-words ajoin ["^/^-W_OPENCV_CMD_" word #","]
]
foreach word arg-words [
	word: uppercase form word
	replace/all word #"-" #"_"
	append enum-arg-words ajoin ["^/^-W_OPENCV_ARG_" word #","]
]

foreach [name spec] commands [
	append header ajoin [lf name ": command "]
	new-line/all spec false
	append/only header mold spec

	name: form name
	replace/all name #"-" #"_"
	uname: uppercase copy name
	
	append enum-commands ajoin ["^/^-CMD_OPENCV_" uname #","]

	append cmd-declares ajoin ["^/int cmd_" name "(RXIFRM *frm, void *ctx);"]
	append cmd-dispatch ajoin ["^-cmd_" name ",^/"]
]

new-line/all cmd-words false
new-line/all arg-words false
append header rejoin [{^/init-words words: } mold cmd-words #" " mold arg-words]
append header {^/protect/hide 'init-words}
append header {
; imwrite params..
IMWRITE_JPEG_QUALITY: 1
IMWRITE_JPEG_PROGRESSIVE: 2
IMWRITE_JPEG_OPTIMIZE: 3
IMWRITE_JPEG_RST_INTERVAL: 4
IMWRITE_JPEG_LUMA_QUALITY: 5
IMWRITE_JPEG_CHROMA_QUALITY: 6
IMWRITE_JPEG_SAMPLING_FACTOR: 7
IMWRITE_PNG_COMPRESSION: 16
IMWRITE_PNG_STRATEGY: 17
IMWRITE_PNG_BILEVEL: 18
IMWRITE_PXM_BINARY: 32
IMWRITE_EXR_TYPE: 48
IMWRITE_EXR_COMPRESSION: 49
IMWRITE_WEBP_QUALITY: 64
IMWRITE_PAM_TUPLETYPE: 128
IMWRITE_TIFF_RESUNIT: 256
IMWRITE_TIFF_XDPI: 257
IMWRITE_TIFF_YDPI: 258
IMWRITE_TIFF_COMPRESSION: 259
IMWRITE_JPEG2000_COMPRESSION_X1000: 272

; ImwritePNGFlags
IMWRITE_PNG_STRATEGY_DEFAULT:      0
IMWRITE_PNG_STRATEGY_FILTERED:     1
IMWRITE_PNG_STRATEGY_HUFFMAN_ONLY: 2
IMWRITE_PNG_STRATEGY_RLE:          3
IMWRITE_PNG_STRATEGY_FIXED:        4

; BorderTypes
BORDER_CONSTANT: 0
BORDER_REPLICATE: 1
BORDER_REFLECT: 2
BORDER_WRAP: 3
BORDER_REFLECT_101: 4
BORDER_TRANSPARENT: 5
BORDER_REFLECT101: BORDER_REFLECT_101
BORDER_DEFAULT: BORDER_REFLECT_101
BORDER_ISOLATED: 16}

;print header

out: make string! 2000
append out reword {//
// auto-generated file, do not modify!
//

#include "common.h"

#define MIN_REBOL_VER 3
#define MIN_REBOL_REV 5
#define MIN_REBOL_UPD 4
#define VERSION(a, b, c) (a << 16) + (b << 8) + c
#define MIN_REBOL_VERSION VERSION(MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD)

$enum-commands
^};
$enum-cmd-words
^};
$enum-arg-words
^};

$cmd-declares

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);
MyCommandPointer Command[] = {
$cmd-dispatch};

#define OPENCV_EXT_INIT_CODE \} self


foreach line split header lf [
	replace/all line #"^"" {\"}
	append out ajoin [{^/^-"} line {\n"\}] 
]
append out "^/"


;print out

write %opencv-rebol-extension.h out
