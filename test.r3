Rebol [
	Title:   "Rebol/OpenCV extension test"
	Purpose: {
		Exercises the extension's commands, the path accessors of the cvMat
		and cvVideoCapture handles, the in-memory codecs and the Rebol codecs
		registered by the module.

		Writes its temporary files into %build/.

		Usage:
			r3 test.r3               ;; dynamic build
			r3 test.r3 --static      ;; the self-contained build
			r3 test.r3 --torture     ;; with recycle/torture (finds GC errors)

		Exits with 1 if any test failed, so it can gate CI.
	}
]

print ["Running test on Rebol build:" mold to-block system/build]
system/options/quiet: false
system/options/log/rebol: 4

CI?: any [
	"true" = get-env "CI"
	"true" = get-env "GITHUB_ACTIONS"
	"true" = get-env "TRAVIS"
	"true" = get-env "CIRCLECI"
	"true" = get-env "GITLAB_CI"
]

if CI? [
	system/options/modules: dirize to-real-file %./
	print ["Using modified modules location:" as-green system/options/modules]
	ls (system/options/modules)
]

;; make sure that we load a fresh extension - the module directory may hold a
;; previously installed copy
try [system/modules/opencv: none]
try [system/modules/opencv-static: none]

args: system/options/args
static?: did find args "--static"
if find args "--torture" [recycle/torture] ;; to detect GC related errors

;; `import` takes its name literally (`import opencv`), so an expression
;; must be in parentheses
cv: import (either static? ['opencv-static]['opencv])

make-dir %build/

;;=============================================================================
;; Minimal self-contained harness
;;=============================================================================

test-count: 0
fail-count: 0
skip-count: 0
failed: copy []

group: func ["Starts a named group of tests" name [string!]][
	print ajoin [lf as-yellow "== " as-yellow name]
]

;; NOTE: plain `func` with explicit locals - `function` would collect the
;; counter set-words as locals and the totals would never move.
;;
;; A test block must EVALUATE to true; anything else counts as a failure and
;; is printed as the detail, so a check can hand back a string saying what
;; went wrong. Never use `return` inside one - it would return from
;; `--test--` itself and the result would never be counted.
--test--: func [
	"Passes when the code block evaluates to true"
	name [string!]
	code [block!]
	/local result
][
	test-count: test-count + 1
	result: try code
	case [
		error? :result [
			fail-count: fail-count + 1
			append failed name
			print [as-red "[FAIL]" name]
			print [as-red "      " mold/flat :result]
		]
		:result = true [
			print [as-green "[ ok ]" name]
		]
		true [
			fail-count: fail-count + 1
			append failed name
			print [as-red "[FAIL]" name "=>" mold/flat/part :result 60]
		]
	]
	:result
]

--skip--: func ["Counts a test which cannot run here" name [string!]][
	skip-count: skip-count + 1
	print [as-purple "[skip]" name]
]

--rejects--: func [
	"Checks that the code is refused with an error"
	label [string!]
	code  [block!]
][
	--test-- ajoin [label " is refused"] compose/only [error? try (code)]
]

;; Compares a result with what it should be and hands back the actual value
;; when they differ, so the failure line shows it.
is?: func ["Compares a value with the expected one" value expected][
	any [value == expected  ajoin ["got " mold/flat/part value 60]]
]

summary: does [
	print ajoin [lf as-yellow "-----------------------------------------------------------------------------"]
	print [
		"tests:" test-count
		as-green ajoin ["passed: " test-count - fail-count - skip-count]
		either fail-count > 0 [as-red ajoin ["failed: " fail-count]][ajoin ["failed: " fail-count]]
		as-purple ajoin ["skipped: " skip-count]
	]
	if fail-count > 0 [
		print as-red ajoin ["^/Failed tests:^/  " mold/flat failed]
	]
	print ""
	quit/return either fail-count > 0 [1][0]
]

;;=============================================================================
group "Basic info & utilities"
;;=============================================================================

--test-- "module imported"                [module? cv]
--test-- "getVersionString returns string" [string? cv/getVersionString]
--test-- "version returns tuple"           [tuple? cv/version]
--test-- "version has 3 parts"             [is? length? cv/version 3]
--test-- "version matches getVersionString" [
	any [
		did find/match cv/getVersionString form cv/version
		reform ["version" cv/version "string" mold cv/getVersionString]
	]
]
--test-- "getBuildInformation returns string" [string? cv/getBuildInformation]

inf: cv/info
print ["OpenCV info:" mold inf]
--test-- "info returns block"                  [block? inf]
--test-- "info/version matches version"        [is? inf/version cv/version]
--test-- "info/cpus is positive"               [all [integer? inf/cpus  inf/cpus > 0]]
--test-- "info/threads matches getNumThreads"  [is? inf/threads cv/getNumThreads]
--test-- "info/optimized matches useOptimized" [is? inf/optimized cv/useOptimized]
--test-- "info/parallel is a string"           [string? inf/parallel]
--test-- "info/cpu-features is a string"       [string? inf/cpu-features]
foreach key [video-backends video-cameras video-streams video-writers][
	--test-- ajoin ["info/" key " is a block of words"] compose [
		all [block? b: select inf (to lit-word! key)  parse b [any word!]]
	]
]
--test-- "the built-in MJPEG writer is listed" [did find inf/video-writers 'CV_MJPEG]

original-log: cv/getLogLevel
--test-- "getLogLevel returns integer"     [integer? original-log]
--test-- "setLogLevel returns the previous level" [
	is? cv/setLogLevel cv/LOG_LEVEL_ERROR original-log
]
--test-- "getLogLevel reads the new level" [is? cv/getLogLevel cv/LOG_LEVEL_ERROR]
--test-- "setLogLevel restores the level" [
	cv/setLogLevel original-log
	is? cv/getLogLevel original-log
]
--rejects-- "setLogLevel above verbose" [cv/setLogLevel 7]
--rejects-- "setLogLevel below silent"  [cv/setLogLevel -1]
--test-- "a rejected level leaves the level alone" [is? cv/getLogLevel original-log]

original-threads: cv/getNumThreads
--test-- "setNumThreads returns the previous count" [is? cv/setNumThreads 0 original-threads]
;; NOTE: what getNumThreads reports after `setNumThreads 0` depends on the
;; parallel backend (1 with pthreads, the TBB arena size with TBB), so it is
;; not tested here.
--test-- "a negative count resets to the default" [
	cv/setNumThreads -1
	is? cv/getNumThreads original-threads
]
--test-- "results do not depend on the thread count" [
	big: cv/resize cv/imread %image/lena.jpeg 1000x1000  ;; runs before `mat` is loaded
	cv/setNumThreads 0
	a: cv/GaussianBlur big none 9x9 3.0 3.0
	cv/setNumThreads -1
	b: cv/GaussianBlur big none 9x9 3.0 3.0
	a/binary = b/binary
]
cv/setNumThreads original-threads
--test-- "getTickCount returns integer"    [integer? cv/getTickCount]
--test-- "getTickFrequency returns decimal" [decimal? cv/getTickFrequency]
--test-- "getNumThreads returns integer"   [integer? cv/getNumThreads]
--test-- "useOptimized returns logic"      [logic? cv/useOptimized]

original-opt: cv/useOptimized
--test-- "setUseOptimized flips the flag" [
	cv/setUseOptimized not original-opt
	cv/useOptimized = not original-opt
]
--test-- "setUseOptimized restores the flag" [
	cv/setUseOptimized original-opt
	cv/useOptimized = original-opt
]

;;=============================================================================
group "Matrix creation & properties"
;;=============================================================================

m1: cv/Matrix 320x240
--test-- "Matrix from pair returns handle" [handle? m1]
--test-- "Matrix size from pair"           [is? m1/size 320x240]
--test-- "Matrix width"                    [is? m1/width 320]
--test-- "Matrix height"                   [is? m1/height 240]
--test-- "Matrix channels (default BGRA)"  [is? m1/channels 4]
--test-- "Matrix depth CV_8U"              [m1/depth = 'CV_8U]
--test-- "Matrix is not submatrix"         [not m1/is-submatrix]
--test-- "Matrix total elements (320*240)" [is? m1/total 76800]
--test-- "Matrix binary data accessible"   [binary? m1/binary]
--test-- "Matrix vector accessible"        [vector? m1/vector]
--test-- "Matrix image! accessible"        [image? m1/image]

m2: cv/Matrix [100x200 CV_8UC1]
--test-- "Matrix from block spec"          [handle? m2]
--test-- "Matrix size from block"          [is? m2/size 100x200]
--test-- "Matrix single channel"           [is? m2/channels 1]
--test-- "Matrix depth CV_8U from block"   [m2/depth = 'CV_8U]

m3: cv/Matrix [50x50 CV_32FC3]
--test-- "Matrix float type"               [handle? m3]
--test-- "Matrix type CV_32FC3"            [m3/type = 'CV_32FC3]

v: make vector! [uint8! 30000]
m4: cv/Matrix [100x100 :v]
--test-- "Matrix from vector data"         [handle? m4]
--test-- "Matrix size from vector"         [is? m4/size 100x100]

;; literal binary is copied, not shared
spec: [2x2 CV_8UC1 #{01020304}]
mb: cv/Matrix spec
--test-- "Matrix from literal binary has the data"   [is? mb/binary #{01020304}]
--test-- "Matrix leaves the source binary untouched" [is? spec/3 #{01020304}]
--test-- "Short data is repeated" [
	mr: cv/Matrix [2x2 CV_8UC1 #{0102}]
	is? mr/binary #{01020102}
]
--test-- "Shared binary starts at its index" [
	b: next #{FF01020304}
	ms: cv/Matrix [2x2 CV_8UC1 :b]
	is? to block! ms/vector [1 2 3 4]
]

img: make image! 64x64
m5: cv/Matrix img
--test-- "Matrix from image!"              [handle? m5]
--test-- "Matrix size from image"          [is? m5/size 64x64]

m6: cv/Matrix m1
--test-- "Matrix copy from handle"         [handle? m6]
--test-- "Matrix copy same size"           [is? m6/size m1/size]

sub: cv/Matrix [:m1 [10x20 30x40]]
--test-- "Submatrix from get-word handle and rectangle" [handle? sub]
--test-- "Submatrix has the rectangle's size"           [is? sub/size 30x40]
--test-- "Submatrix reports is-submatrix"               [sub/is-submatrix]
--test-- "Submatrix from reduced spec (handle value, not get-word)" [
	sub2: cv/Matrix reduce [m1 [10x20 30x40]]
	is? sub2/size 30x40
]
--rejects-- "Submatrix without rectangle"  [cv/Matrix [:m1]]
--rejects-- "Submatrix outside the source" [cv/Matrix [:m1 [300x200 100x100]]]
--rejects-- "Submatrix with negative size" [cv/Matrix [:m1 [0x0 -1x10]]]
--rejects-- "Matrix with invalid block"    [cv/Matrix [invalid-spec]]

cv/free m6
--test-- "Freed handle reads as none"      [none? m6/size]

;;=============================================================================
group "Image I/O"
;;=============================================================================

mat: cv/imread %image/lena.jpeg
--test-- "imread loads image"              [handle? mat]
--test-- "imread mat converts to image"    [image? mat/image]
--test-- "imread grayscale" [
	mat-gray: cv/imread/with %image/lena.jpeg cv/IMREAD_GRAYSCALE
	all [handle? mat-gray  1 = mat-gray/channels]
]
--test-- "imread/image returns rebol image" [image? cv/imread/image %image/lena.jpeg]
--test-- "imread missing file returns none" [none? cv/imread %image/nonexistent.png]

--test-- "imwrite saves file" [
	cv/imwrite %build/test-save.png mat
	did exists? %build/test-save.png
]
--test-- "imread round-trip same size" [
	mat-loaded: cv/imread %build/test-save.png
	is? mat-loaded/size mat/size
]

;;=============================================================================
group "Multi-page TIFF"
;;=============================================================================

pages: cv/imreadmulti %image/2page.tiff
--test-- "imreadmulti returns 2 pages"     [is? length? pages 2]
--test-- "imreadmulti pages are handles"   [all [handle? pages/1  handle? pages/2]]
pages: cv/imreadmulti/image %image/2page.tiff
--test-- "imreadmulti/image returns 2 pages" [is? length? pages 2]
--test-- "imreadmulti/image pages are images" [all [image? pages/1  image? pages/2]]

;;=============================================================================
group "Color conversion"
;;=============================================================================

gray: cv/cvtColor mat none cv/COLOR_BGR2GRAY
--test-- "cvtColor BGR2GRAY is single channel" [is? gray/channels 1]
--test-- "cvtColor BGR2HLS is 3 channel" [
	hls: cv/cvtColor mat none cv/COLOR_BGR2HLS
	is? hls/channels 3
]
--test-- "cvtColor BGR2HSV is 3 channel" [
	hsv: cv/cvtColor mat none cv/COLOR_BGR2HSV
	is? hsv/channels 3
]

;;=============================================================================
group "Image transforms"
;;=============================================================================

--test-- "resize by percent" [
	resized: cv/resize mat 50%
	resized/size = (mat/size * 50%)
]
--test-- "resize explicit size" [
	resized: cv/resize mat 100x100
	is? resized/size 100x100
]
--test-- "resize with INTER_NEAREST" [handle? cv/resize/with mat 50% cv/INTER_NEAREST]
--test-- "flip horizontal keeps the size" [
	flipped: cv/flip mat none 1
	is? flipped/size mat/size
]
--test-- "flip vertical"                   [handle? cv/flip mat none 0]
--test-- "flip both axes"                  [handle? cv/flip mat none -1]

;;=============================================================================
group "Image filtering"
;;=============================================================================

--test-- "blur keeps the size" [
	blurred: cv/blur mat none 5
	is? blurred/size mat/size
]
--test-- "GaussianBlur"    [handle? cv/GaussianBlur mat none 5x5 1.0 0.0]
--test-- "medianBlur"      [handle? cv/medianBlur mat none 5]
--test-- "bilateralFilter" [handle? cv/bilateralFilter mat none 9 75 75]
--test-- "filter2D sharpen" [
	kernel-data: #(float! [
		 0 -1  0
		-1  5 -1
		 0 -1  0
	])
	kernel: cv/Matrix [3x3 :kernel-data]
	sharp: cv/filter2D mat none -1 kernel -1x-1 0
	cv/free kernel
	handle? sharp
]

;;=============================================================================
group "Edge detection & thresholding"
;;=============================================================================

edges: cv/Canny gray none 50 200
--test-- "Canny output is single channel"  [is? edges/channels 1]
--test-- "Canny output same size"          [is? edges/size gray/size]
--test-- "threshold binary is single channel" [
	thresh: cv/threshold gray none 128 255 cv/THRESH_BINARY
	is? thresh/channels 1
]
--test-- "threshold binary inverse" [handle? cv/threshold gray none 128 255 cv/THRESH_BINARY_INV]
--test-- "threshold trunc"          [handle? cv/threshold gray none 100 255 cv/THRESH_TRUNC]

;;=============================================================================
group "Arithmetic & bitwise ops"
;;=============================================================================

--test-- "add keeps the size" [
	added: cv/add mat mat none
	is? added/size mat/size
]
--rejects-- "add with different channel counts" [cv/add mat gray none]
--test-- "subtract"     [handle? cv/subtract mat mat none]
--test-- "multiply"     [handle? cv/multiply mat mat none]
--test-- "divide"       [handle? cv/divide mat mat none]
--test-- "absdiff"      [handle? cv/absdiff mat mat none]
--test-- "addWeighted"  [handle? cv/addWeighted mat 0.5 mat 0.5 0 none]
--test-- "bitwise-and"  [handle? cv/bitwise-and mat mat none]
--test-- "bitwise-or"   [handle? cv/bitwise-or mat mat none]
--test-- "bitwise-xor"  [handle? cv/bitwise-xor mat mat none]
--test-- "bitwise-not"  [handle? cv/bitwise-not mat none]

--test-- "bitwise-not/mask leaves unmasked pixels" [
	src:  cv/Matrix [2x1 CV_8UC1 #{0000}]
	dst:  cv/Matrix [2x1 CV_8UC1 #{1111}]
	mask: cv/Matrix [2x1 CV_8UC1 #{FF00}]
	cv/bitwise-not/mask src dst mask
	is? dst/binary #{FF11}
]

;; masked op into a new destination: unmasked elements must be zero
src:  cv/Matrix [64x64 CV_8UC1 #{01}]          ;; filled with 1
mb:   append/dup copy #{} 0 4096  mb/1: 255
mask: cv/Matrix [64x64 CV_8UC1 :mb]            ;; only the first element set
junk: cv/add src src none                      ;; OpenCV-allocated, all 2
cv/free junk                                   ;; leave non-zero memory behind
res:  cv/add/mask src src none mask
--test-- "masked element computed"                       [is? res/binary/1 2]
--test-- "unmasked element of a new destination is zero" [is? res/binary/2 0]

;;=============================================================================
group "Matrix analysis"
;;=============================================================================

--test-- "mean has 3 values (BGR)" [
	means: cv/mean mat
	all [block? means  3 = length? means]
]
--test-- "minMaxLoc returns block" [block? cv/minMaxLoc gray]
--test-- "normalize"               [handle? cv/normalize gray none 0 255 cv/NORM_MINMAX]
--test-- "convertScaleAbs"         [handle? cv/convertScaleAbs gray none 1.0 0]
--test-- "convertTo changes type" [
	cvt: cv/convertTo gray none cv/CV_32F 1.0 0.0
	cvt/type = 'CV_32FC1
]
--test-- "max" [handle? cv/max mat mat none]
--test-- "min" [handle? cv/min mat mat none]

;;=============================================================================
group "Color maps"
;;=============================================================================

--test-- "applyColorMap JET is 3 channel" [
	cmapped: cv/applyColorMap gray none cv/COLORMAP_JET
	is? cmapped/channels 3
]
--test-- "applyColorMap HOT" [handle? cv/applyColorMap gray none cv/COLORMAP_HOT]

;;=============================================================================
group "Morphological operations"
;;=============================================================================

kernel-el: cv/getStructuringElement cv/MORPH_RECT 5 -1x-1
--test-- "getStructuringElement" [handle? kernel-el]
--test-- "dilate" [handle? cv/dilate gray none kernel-el -1x-1 1]
--test-- "erode"  [handle? cv/erode  gray none kernel-el -1x-1 1]
cv/free kernel-el

;;=============================================================================
group "QR code"
;;=============================================================================

qr-data: "https://github.com/Oldes/Rebol-OpenCV/"
qr: cv/qrcode-encode qr-data
qr-big: cv/resize/with qr 600% cv/INTER_NEAREST  ;; resized so it is decodable
--test-- "qrcode-encode returns handle"  [handle? qr]
--test-- "qrcode-decode from cvMat"      [is? cv/qrcode-decode qr-big qr-data]
--test-- "qrcode-decode from image!"     [is? cv/qrcode-decode qr-big/image qr-data]
--test-- "qrcode-decode from file" [
	cv/imwrite %build/test-qrcode.png qr-big
	is? cv/qrcode-decode %build/test-qrcode.png qr-data
]

;;=============================================================================
group "get-property constants"
;;=============================================================================

--test-- "get-property MAT_SIZE"     [is? cv/get-property mat cv/MAT_SIZE mat/size]
--test-- "get-property MAT_CHANNELS" [is? cv/get-property mat cv/MAT_CHANNELS mat/channels]
--test-- "get-property MAT_TYPE"     [(cv/get-property mat cv/MAT_TYPE) = mat/type]
--test-- "get-property MAT_DEPTH"    [(cv/get-property mat cv/MAT_DEPTH) = mat/depth]
--test-- "get-property MAT_DEPTH is a word" [word? cv/get-property mat cv/MAT_DEPTH]
--test-- "get-property MAT_BINARY"   [binary? cv/get-property mat cv/MAT_BINARY]
--test-- "get-property MAT_IMAGE"    [image?  cv/get-property mat cv/MAT_IMAGE]
--test-- "get-property MAT_VECTOR"   [vector? cv/get-property mat cv/MAT_VECTOR]

;;=============================================================================
group "Released handles"
;;=============================================================================

cv/free m1
--rejects-- "resize of a freed handle" [cv/resize m1 50%]
--rejects-- "imencode of a released handle" [cv/imencode %.png m6]

;; a released handle must not come back as a newly allocated one
m: cv/Matrix 4x4
cv/free m
n: cv/Matrix 8x8   ;; used to get the same HOB as `m`
--test-- "released handle stays released after a new allocation" [none? m/size]
--test-- "new handle is not affected"                            [is? n/size 8x8]
--rejects-- "released handle after a new allocation"             [cv/imencode %.png m]
recycle
--test-- "released handle stays released after recycle"         [none? m/size]
--test-- "new handle survives recycle"                           [is? n/size 8x8]

;;=============================================================================
group "Gabor kernel & Laplacian"
;;=============================================================================

gabor: cv/getGaborKernel 31x31 5.0 0.0 10.0 0.5 0 cv/CV_64F
--test-- "Gabor kernel correct size"  [is? gabor/size 31x31]
--test-- "Gabor kernel type CV_64FC1" [gabor/type = 'CV_64FC1]
--test-- "Laplacian single channel" [
	lap: cv/Laplacian gray none cv/CV_8U 3 1 0
	is? lap/channels 1
]

;;=============================================================================
group "Vectors (element type travels in the value since ABI 1)"
;;=============================================================================

--test-- "CV_8UC1 vector length (100x200x1)" [
	vec8: m2/vector
	all [vector? vec8  20000 = length? vec8]
]
--test-- "CV_32FC3 vector length (50x50x3)" [
	vecf: m3/vector
	all [vector? vecf  7500 = length? vecf]
]
--test-- "CV_32FC3 vector holds decimals" [decimal? first vecf]
--test-- "Matrix from float32 vector is CV_32FC1" [
	fdata: make vector! [decimal! 32 [1.0 2.0 3.0 4.0]]
	mf: cv/Matrix [2x2 :fdata]
	mf/type = 'CV_32FC1
]

;;=============================================================================
group "Video"
;;=============================================================================

frame: cv/resize mat 64x48
out: cv/VideoWriter %build/test-video.avi 0 24 64x48   ;; 0 = MJPG
--test-- "VideoWriter opens" [handle? out]
if handle? out [
	loop 5 [cv/write out frame]
	cv/free out                                        ;; flushes the file
]

cap: cv/VideoCapture %build/test-video.avi
either handle? cap [
	--test-- "capture width"  [is? cap/width 64]
	--test-- "capture height" [is? cap/height 48]
	;; backends derive fps from the AVI header (rate/scale or usec per frame),
	;; so it may come back as e.g. 23.9998 instead of 24.0
	--test-- "capture fps" [
		any [0.01 > abs (cap/fps - 24.0)  ajoin ["got " cap/fps]]
	]
	;; fourcc is backend specific: FFmpeg/MJPEG report the file's codec (MJPG),
	;; AVFoundation reports its decoded output format (BGR3)
	--test-- "capture fourcc is MJPG or BGR3" [
		fourcc: to string! reverse skip to binary! to integer! cap/fourcc 4
		any [did find ["MJPG" "BGR3"] fourcc  ajoin ["got " mold fourcc]]
	]
	--test-- "capture frame count"           [is? cap/frames 5]
	--test-- "capture starts at frame 0"     [is? cap/pos-frame 0.0]
	--test-- "pos-frame advances after read" [
		cv/read cap
		is? cap/pos-frame 1.0
	]
	cv/free cap
][
	--test-- "VideoCapture opens the written file" [handle? cap]
]

;;=============================================================================
group "imencode / imdecode"
;;=============================================================================

png: cv/imencode %.png mat
--test-- "imencode returns binary"        [binary? png]
--test-- "imencode %.png writes PNG"      [is? copy/part png 4 #{89504E47}]
--test-- "imencode accepts extension without dot" [is? copy/part cv/imencode "png" mat 4 #{89504E47}]
--test-- "imencode accepts codec word"    [is? copy/part cv/imencode 'png mat 4 #{89504E47}]
--test-- "imencode 'jpeg writes JPEG"     [is? copy/part cv/imencode 'jpeg mat 2 #{FFD8}]
--test-- "imencode 'tiff writes TIFF"     [is? copy/part cv/imencode 'tiff mat 4 #{49492A00}]
--test-- "imencode %.webp writes WebP"    [is? copy/part cv/imencode %.webp mat 4 #{52494646}]
--rejects-- "imencode with unknown codec word" [cv/imencode 'xyz mat]
--rejects-- "imencode with unknown format"     [cv/imencode %.xyz mat]

--test-- "imdecode returns cvMat" [
	m: cv/imdecode png
	all [handle? m  m/size = mat/size]
]
--test-- "PNG round trip is lossless"      [mat/binary = m/binary]
--test-- "imdecode/image equals mat/image" [mat/image = cv/imdecode/image png]
--test-- "imdecode honours the binary index" [
	m: cv/imdecode next join #{00} png
	is? m/size mat/size
]
--test-- "imencode from image!" [
	m: cv/imdecode cv/imencode %.png mat/image
	is? m/size mat/size
]
--test-- "imencode uses the quality parameter" [
	jpg-lo: cv/imencode/with %.jpg mat reduce [cv/IMWRITE_JPEG_QUALITY 10]
	jpg-hi: cv/imencode/with %.jpg mat reduce [cv/IMWRITE_JPEG_QUALITY 95]
	(length? jpg-lo) < length? jpg-hi
]
--test-- "16-bit image is scaled to 8 bits" [
	m16: cv/Matrix [4x4 CV_16UC1 #{FFFF}]
	img: cv/imdecode/image cv/imencode %.png m16
	is? first img/1 255
]
--test-- "imdecode of garbage returns none"      [none? cv/imdecode #{00010203}]
--test-- "imdecode of empty binary returns none" [none? cv/imdecode #{}]

;;=============================================================================
group "Rebol codecs"
;;=============================================================================

foreach name [cv-png cv-jpeg cv-bmp cv-tiff cv-webp][
	--test-- ajoin ["codec registered: " name] compose [object? select system/codecs (to lit-word! name)]
]
--test-- "cv-* codecs take no suffixes"  [not find system/catalog/file-types 'cv-png]
--test-- "cv-* codecs do not identify"   [not function? try [:system/codecs/cv-png/identify]]

bin: encode 'cv-png mat/image
--test-- "encode 'cv-png"                [is? copy/part bin 4 #{89504E47}]
--test-- "decode 'cv-png round trip"     [mat/image = decode 'cv-png bin]
--test-- "encode 'cv-png from cvMat"     [is? copy/part encode 'cv-png mat 4 #{89504E47}]
--test-- "encode/as integer sets the quality" [
	jpg-lo: encode/as 'cv-jpeg mat/image 10
	jpg-hi: encode/as 'cv-jpeg mat/image 95
	(length? jpg-lo) < length? jpg-hi
]
--test-- "encode/as block passes parameters" [
	jpg-blk: encode/as 'cv-jpeg mat/image reduce [cv/IMWRITE_JPEG_QUALITY 10]
	is? length? jpg-blk length? jpg-lo
]
--rejects-- "decode of invalid data" [decode 'cv-png #{00010203}]

;; plain names are registered only where the core has no codec
foreach name [png jpeg bmp tiff webp][
	codec: select system/codecs name
	either all [codec  string? codec/title  find codec/title "(OpenCV)"][
		file: join %build/test-codec. name
		--test-- ajoin ["save via suffix: " name] compose [
			save (file) mat/image
			did exists? (file)
		]
		--test-- ajoin ["encoding? identifies " name] compose [
			(to lit-word! name) = encoding? read (file)
		]
		--test-- ajoin ["load via suffix: " name] compose [
			img: load (file)
			is? img/size mat/size
		]
	][
		--skip-- ajoin ["plain " name " codec (the core has its own)"]
	]
]

;;=============================================================================
group "HighGUI (not part of the static build)"
;;=============================================================================

either static? [
	--test-- "GUI command errors in static build" [
		error? err: try [cv/destroyAllWindows]
	]
	--test-- "error reports missing highgui" [did find form err "no highgui"]
][
	--skip-- "GUI tests (need a display)"
]

summary
