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
	imshow: [
		"Displays an image in the specified window."
		src [image! handle!]
		/name "Optional window name"
		 window [any-string!]
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
