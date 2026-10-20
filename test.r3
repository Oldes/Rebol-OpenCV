Rebol [
    title: "Comprehensive OpenCV extension test"
]

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
    ;either system/platform = 'Windows [
    ;    set-env "PATH" ajoin [
    ;        to-local-file to-real-file rejoin [%build/ system/build/arch %/vc17/bin/]
    ;        #";" get-env "PATH"
    ;    ]
    ;][
    ;    set-env "LD_LIBRARY_PATH" "/usr/local/lib:/lib:/usr/lib"
    ;]
]

try [system/modules/opencv: none]

args: system/options/args
static?: did find args "--static"
if find args "--torture" [ recycle/torture ] ;; to detect GC related errors

cv: import (either static? ['opencv-static]['opencv])

; ============================================================
; Helper: print a test group header
; ============================================================
header: func [
    "Print a formatted test group header."
    title [string!] "Group title."
][
    print ""
    print as-blue ajoin [ "=== " title " ===" ]
]

; ============================================================
; Helper: check that an expression is true
; ============================================================
assert: func [
    "Fail with message if value is not true."
    value "Value to test for truth."
    label [string!] "Test label."
][
    if not value [
        print as-red rejoin ["FAIL: " label]
        quit/return 1
    ]
    print as-green rejoin ["PASS: " label]
]

; ============================================================
; Helper: check that two values are equal
; ============================================================
assert-equal: func [
    "Fail if a and b are not equal."
    a "Expected value."
    b "Actual value."
    label [string!] "Test label."
][
    if a <> b [
        print as-red rejoin ["FAIL: " label " — expected: " mold a ", got: " mold b]
        quit/return 1
    ]
    print as-green rejoin ["PASS: " label]
]

; ============================================================
; Helper: check that an expression raises an error
; ============================================================
assert-error: func [
    "Succeed only if evaluating body raises an error."
    label [string!] "Test label."
    body [block!] "Code that should error."
][
    either error? try body [
        print as-green rejoin ["PASS: " label]
    ][
        print as-red rejoin ["FAIL: " label " — expected error but none occurred"]
        quit/return 1
    ]
]

; ============================================================
; 1. Basic info and utilities
; ============================================================
header "Basic Info & Utilities"

assert [string? cv/getVersionString] "getVersionString returns string"
assert [string? cv/getBuildInformation] "getBuildInformation returns string"
assert [integer? cv/getTickCount] "getTickCount returns integer"
assert [decimal? cv/getTickFrequency] "getTickFrequency returns decimal"
assert [integer? cv/getNumThreads] "getNumThreads returns integer"
assert [logic? cv/useOptimized] "useOptimized returns logic"

; setUseOptimized
original-opt: cv/useOptimized
cv/setUseOptimized not original-opt
assert [cv/useOptimized = not original-opt] "setUseOptimized flips flag"
cv/setUseOptimized original-opt
assert [cv/useOptimized = original-opt] "setUseOptimized restores flag"

; ============================================================
; 2. Matrix creation and properties
; ============================================================
header "Matrix Creation & Properties"

; --- from pair (blank image) ---
m1: cv/Matrix 320x240
assert [handle? m1] "Matrix from pair returns handle"
assert [m1/size  = 320x240] "Matrix size from pair"
assert [m1/width  = 320] "Matrix width"
assert [m1/height = 240] "Matrix height"
assert [m1/channels = 3] "Matrix channels (default BGR)"
assert [m1/depth = 'CV_8U] "Matrix depth CV_8U"
assert [not m1/is-submatrix] "Matrix is not submatrix"
assert [m1/total = 76800] "Matrix total elements (320*240)"
assert [binary? m1/binary] "Matrix binary data accessible"
assert [vector? m1/vector] "Matrix vector accessible"
assert [image? m1/image] "Matrix image! accessible"

; --- from block spec ---
m2: cv/Matrix [100x200 CV_8UC1]
assert [handle? m2] "Matrix from block spec"
assert [m2/size = 100x200] "Matrix size from block"
assert [m2/channels = 1] "Matrix single channel"
assert [m2/depth = 'CV_8U] "Matrix depth CV_8U"

; --- from block with float type ---
m3: cv/Matrix [50x50 CV_32FC3]
assert [handle? m3] "Matrix float type"
assert [m3/type = 'CV_32FC3] "Matrix type CV_32FC3"

; --- from block with vector data ---
v: make vector! [uint8! 30000]
m4: cv/Matrix [100x100 :v]
assert [handle? m4] "Matrix from vector data"
assert [m4/size = 100x100] "Matrix size from vector"

; --- from literal binary (copied, not shared) ---
spec: [2x2 CV_8UC1 #{01020304}]
mb: cv/Matrix spec
assert-equal #{01020304} mb/binary "Matrix from literal binary has the data"
assert-equal #{01020304} spec/3 "Matrix leaves the source binary untouched"

; --- shorter data is repeated to fill the matrix ---
mr: cv/Matrix [2x2 CV_8UC1 #{0102}]
assert-equal #{01020102} mr/binary "Short data is repeated"

; --- shared binary honours its index ---
b: next #{FF01020304}
ms: cv/Matrix [2x2 CV_8UC1 :b]
assert-equal [1 2 3 4] to block! ms/vector "Shared binary starts at its index"

; --- from image ---
img: make image! 64x64
m5: cv/Matrix img
assert [handle? m5] "Matrix from image!"
assert [m5/size = 64x64] "Matrix size from image"

; --- copy from existing mat ---
m6: cv/Matrix m1
assert [handle? m6] "Matrix copy from handle"
assert [m6/size = m1/size] "Matrix copy same size"

; --- submatrix (region of interest) ---
sub: cv/Matrix [:m1 [10x20 30x40]]
assert [handle? sub] "Submatrix from get-word handle and rectangle"
assert [sub/size = 30x40] "Submatrix has the rectangle's size"
assert [sub/is-submatrix] "Submatrix reports is-submatrix"

sub2: cv/Matrix reduce [m1 [10x20 30x40]]
assert [handle? sub2] "Submatrix from reduced spec (handle value, not get-word)"
assert [sub2/size = 30x40] "Reduced submatrix has the rectangle's size"

assert-error "Submatrix without rectangle errors"  [cv/Matrix [:m1]]
assert-error "Submatrix outside the source errors" [cv/Matrix [:m1 [300x200 100x100]]]
assert-error "Submatrix with negative size errors" [cv/Matrix [:m1 [0x0 -1x10]]]

; --- free ---
cv/free m6
assert [none? m6/size] "Freed handle reads as none"

; ============================================================
; 3. Image I/O
; ============================================================
header "Image I/O"

mat: cv/imread %image/lena.jpeg
assert [handle? mat] "imread loads image"
assert [image? mat/image] "imread mat converts to image"

mat-gray: cv/imread/with %image/lena.jpeg cv/IMREAD_GRAYSCALE
assert [handle? mat-gray] "imread grayscale"
assert [mat-gray/channels = 1] "imread grayscale is single channel"

mat-rebol: cv/imread/image %image/lena.jpeg
assert [image? mat-rebol] "imread/image returns rebol image"

; imwrite
cv/imwrite %build/test-save.png mat
assert [exists? %build/test-save.png] "imwrite saves file"

; imread the saved file back
mat-loaded: cv/imread %build/test-save.png
assert [handle? mat-loaded] "imread round-trip loads handle"
assert [mat-loaded/size = mat/size] "imread round-trip same size"

; ============================================================
; 4. Color conversion
; ============================================================
header "Color Conversion"

gray: cv/cvtColor mat none cv/COLOR_BGR2GRAY
assert [handle? gray] "cvtColor BGR2GRAY"
assert [gray/channels = 1] "cvtColor BGR2GRAY is single channel"

hls: cv/cvtColor mat none cv/COLOR_BGR2HLS
assert [handle? hls] "cvtColor BGR2HLS"
assert [hls/channels = 3] "cvtColor BGR2HLS is 3 channel"

hsv: cv/cvtColor mat none cv/COLOR_BGR2HSV
assert [handle? hsv] "cvtColor BGR2HSV"
assert [hsv/channels = 3] "cvtColor BGR2HSV is 3 channel"

; ============================================================
; 5. Image transforms
; ============================================================
header "Image Transforms"

; resize
resized: cv/resize mat 50%
assert [handle? resized] "resize by percent"
assert [resized/size = (mat/size * 50%)] "resize by percent correct size"

resized-px: cv/resize mat 100x100
assert [resized-px/size = 100x100] "resize explicit size"

resized-nn: cv/resize/with mat 50% cv/INTER_NEAREST
assert [handle? resized-nn] "resize with INTER_NEAREST"

; flip
flipped-h: cv/flip mat none 1     ; horizontal
assert [handle? flipped-h] "flip horizontal"
assert [flipped-h/size = mat/size] "flip same size"

flipped-v: cv/flip mat none 0     ; vertical
assert [handle? flipped-v] "flip vertical"

flipped-b: cv/flip mat none -1    ; both
assert [handle? flipped-b] "flip both axes"

; ============================================================
; 6. Image filtering
; ============================================================
header "Image Filtering"

; blur
blurred: cv/blur mat none 5
assert [handle? blurred] "blur"
assert [blurred/size = mat/size] "blur same size"

; GaussianBlur
gblur: cv/GaussianBlur mat none 5x5 1.0 0.0
assert [handle? gblur] "GaussianBlur"

; medianBlur
mblur: cv/medianBlur mat none 5
assert [handle? mblur] "medianBlur"

; bilateralFilter
bf: cv/bilateralFilter mat none 9 75 75
assert [handle? bf] "bilateralFilter"

; filter2D — sharpen kernel
kernel-data: #(float! [
    0 -1 0
   -1 5 -1
    0 -1 0
])
kernel: cv/Matrix [3x3 :kernel-data]
sharp: cv/filter2D mat none -1 kernel -1x-1 0
assert [handle? sharp] "filter2D sharpen"
cv/free kernel

; ============================================================
; 7. Edge detection and thresholding
; ============================================================
header "Edge Detection & Thresholding"

; Canny on grayscale
edges: cv/Canny gray none 50 200
assert [handle? edges] "Canny edge detection"
assert [edges/channels = 1] "Canny output is single channel"
assert [edges/size = gray/size] "Canny output same size"

; threshold
thresh: cv/threshold gray none 128 255 cv/THRESH_BINARY
assert [handle? thresh] "threshold binary"
assert [thresh/channels = 1] "threshold single channel"

thresh-inv: cv/threshold gray none 128 255 cv/THRESH_BINARY_INV
assert [handle? thresh-inv] "threshold binary inverse"

thresh-trunc: cv/threshold gray none 100 255 cv/THRESH_TRUNC
assert [handle? thresh-trunc] "threshold trunc"

; ============================================================
; 8. Arithmetic and bitwise operations
; ============================================================
header "Arithmetic & Bitwise Ops"

; add
added: cv/add mat mat none
assert [handle? added] "add"
assert [added/size = mat/size] "add same size"
assert-error "add with different channel counts errors" [
    cv/add mat gray none
]

; subtract
subbed: cv/subtract mat mat none
assert [handle? subbed] "subtract"

; multiply
mult: cv/multiply mat mat none
assert [handle? mult] "multiply"

; divide
divd: cv/divide mat mat none
assert [handle? divd] "divide"

; absdiff
diff: cv/absdiff mat mat none
assert [handle? diff] "absdiff"

; addWeighted
weighted: cv/addWeighted mat 0.5 mat 0.5 0 none
assert [handle? weighted] "addWeighted"

; bitwise-and
bw-and: cv/bitwise-and mat mat none
assert [handle? bw-and] "bitwise-and"

; bitwise-or
bw-or: cv/bitwise-or mat mat none
assert [handle? bw-or] "bitwise-or"

; bitwise-xor
bw-xor: cv/bitwise-xor mat mat none
assert [handle? bw-xor] "bitwise-xor"

; bitwise-not
bw-not: cv/bitwise-not mat none
assert [handle? bw-not] "bitwise-not"

; bitwise-not/mask only touches masked pixels
src:  cv/Matrix [2x1 CV_8UC1 #{0000}]
dst:  cv/Matrix [2x1 CV_8UC1 #{1111}]
mask: cv/Matrix [2x1 CV_8UC1 #{FF00}]
cv/bitwise-not/mask src dst mask
assert-equal #{FF11} dst/binary "bitwise-not/mask leaves unmasked pixels"

; masked op into a new destination: unmasked elements must be zero
src:  cv/Matrix [64x64 CV_8UC1 #{01}]          ;; filled with 1
mb:   append/dup copy #{} 0 4096  mb/1: 255
mask: cv/Matrix [64x64 CV_8UC1 :mb]            ;; only the first element set
junk: cv/add src src none                      ;; OpenCV-allocated, all 2
cv/free junk                                   ;; leave non-zero memory behind
res:  cv/add/mask src src none mask
assert-equal 2 res/binary/1 "masked element computed"
assert-equal 0 res/binary/2 "unmasked element of a new destination is zero"

; ============================================================
; 9. Matrix analysis
; ============================================================
header "Matrix Analysis"

; mean
means: cv/mean mat
assert [block? means] "mean returns block"
assert [3 = length? means] "mean has 3 values (BGR)"

; minMaxLoc on single-channel
mminmax: cv/minMaxLoc gray
assert [block? mminmax] "minMaxLoc returns block"

; normalize
normed: cv/normalize gray none 0 255 cv/NORM_MINMAX
assert [handle? normed] "normalize"

; convertScaleAbs
csa: cv/convertScaleAbs gray none 1.0 0
assert [handle? csa] "convertScaleAbs"

; convertTo
cvt: cv/convertTo gray none cv/CV_32F 1.0 0.0
assert [handle? cvt] "convertTo"
assert [cvt/type = 'CV_32FC1] "convertTo changes type"

; max / min
maxed: cv/max mat mat none
assert [handle? maxed] "max"
mined: cv/min mat mat none
assert [handle? mined] "min"

; ============================================================
; 10. Color map
; ============================================================
header "Color Maps"

cmapped: cv/applyColorMap gray none cv/COLORMAP_JET
assert [handle? cmapped] "applyColorMap JET"
assert [cmapped/channels = 3] "color map is 3 channel"

cmapped2: cv/applyColorMap gray none cv/COLORMAP_HOT
assert [handle? cmapped2] "applyColorMap HOT"

; ============================================================
; 11. Morphological operations
; ============================================================
header "Morphological Operations"

kernel-el: cv/getStructuringElement cv/MORPH_RECT 5 -1x-1
assert [handle? kernel-el] "getStructuringElement"

dilated: cv/dilate gray none kernel-el -1x-1 1
assert [handle? dilated] "dilate"

eroded: cv/erode gray none kernel-el -1x-1 1
assert [handle? eroded] "erode"

cv/free kernel-el

; ============================================================
; 12. QR code encode/decode
; ============================================================
header "QR Code"

qr-data: "https://github.com/Oldes/Rebol-OpenCV/"
qr: cv/qrcode-encode qr-data
assert [handle? qr] "qrcode-encode returns handle"

; resize the qr so it's decodable
qr-big: cv/resize/with qr 600% cv/INTER_NEAREST

; decode from cvMat
decoded: cv/qrcode-decode qr-big
assert [decoded = qr-data] "qrcode-decode from cvMat"

; decode from Rebol image
qr-img: qr-big/image
decoded2: cv/qrcode-decode qr-img
assert [decoded2 = qr-data] "qrcode-decode from image!"

; decode from file
cv/imwrite %build/test-qrcode.png qr-big
decoded3: cv/qrcode-decode %build/test-qrcode.png
assert [decoded3 = qr-data] "qrcode-decode from file"

; ============================================================
; 13. get-property with property constants
; ============================================================
header "get-property Constants"

assert [cv/get-property mat cv/MAT_SIZE     = mat/size]  "get-property MAT_SIZE"
assert [cv/get-property mat cv/MAT_CHANNELS = mat/channels] "get-property MAT_CHANNELS"
assert [cv/get-property mat cv/MAT_DEPTH    = mat/depth]  "get-property MAT_DEPTH"
assert [binary? cv/get-property mat cv/MAT_BINARY] "get-property MAT_BINARY"
assert [image?  cv/get-property mat cv/MAT_IMAGE]  "get-property MAT_IMAGE"
assert [vector? cv/get-property mat cv/MAT_VECTOR]  "get-property MAT_VECTOR"

; ============================================================
; 14. Edge cases and error handling
; ============================================================
header "Edge Cases & Errors"

; Matrix from invalid spec
assert-error "Matrix with invalid block errors" [
    cv/Matrix [invalid-spec]
]

; imread missing file
assert [none? cv/imread %image/nonexistent.png] "imread missing file returns none"

; operation on freed handle
cv/free m1
assert-error "Resize freed handle errors" [
    cv/resize m1 50%
]

; ============================================================
; 15. Multi-page TIFF
; ============================================================
header "Multi-page TIFF"

pages: cv/imreadmulti %image/2page.tiff
assert [block? pages] "imreadmulti returns block"
assert [2 = length? pages] "imreadmulti has 2 pages"
assert [handle? pages/1] "imreadmulti page 1 is handle"
assert [handle? pages/2] "imreadmulti page 2 is handle"

pages-img: cv/imreadmulti/image %image/2page.tiff
assert [block? pages-img] "imreadmulti/image returns block"
assert [2 = length? pages-img] "imreadmulti/image has 2 pages"
assert [image? pages-img/1] "imreadmulti/image page 1 is image"
assert [image? pages-img/2] "imreadmulti/image page 2 is image"

; ============================================================
; 16. Gabor kernel
; ============================================================
header "Gabor Kernel"

gabor: cv/getGaborKernel 31x31 5.0 0.0 10.0 0.5 0 cv/CV_64F
assert [handle? gabor] "getGaborKernel"
assert [gabor/size = 31x31] "Gabor kernel correct size"
assert [gabor/type = 'CV_64FC1] "Gabor kernel type CV_64FC1"

; ============================================================
; 17. Laplacian
; ============================================================
header "Laplacian"

lap: cv/Laplacian gray none cv/CV_8U 3 1 0
assert [handle? lap] "Laplacian"
assert [lap/channels = 1] "Laplacian single channel"

; ============================================================
; 18. Vectors (element type travels in the value since ABI 1)
; ============================================================
header "Vectors"

vec8: m2/vector
assert [vector? vec8] "CV_8UC1 matrix as vector"
assert-equal 20000 length? vec8 "CV_8UC1 vector length (100x200x1)"

vecf: m3/vector
assert [vector? vecf] "CV_32FC3 matrix as vector"
assert-equal 7500 length? vecf "CV_32FC3 vector length (50x50x3)"
assert [decimal? first vecf] "CV_32FC3 vector holds decimals"

;; matrix made from a float vector keeps the vector's element type
fdata: make vector! [decimal! 32 [1.0 2.0 3.0 4.0]]
mf: cv/Matrix [2x2 :fdata]
assert-equal 'CV_32FC1 mf/type "Matrix from float32 vector is CV_32FC1"

; ============================================================
; 19. Video writing and capture properties
; ============================================================
header "Video"

frame: cv/resize mat 64x48
out: cv/VideoWriter %build/test-video.avi 0 24 64x48   ;; 0 = MJPG
assert [handle? out] "VideoWriter opens"
loop 5 [cv/write out frame]
cv/free out                                            ;; flushes the file

cap: cv/VideoCapture %build/test-video.avi
assert [handle? cap] "VideoCapture opens the written file"
assert-equal 64   cap/width  "capture width"
assert-equal 48   cap/height "capture height"
;; backends derive fps from the AVI header (rate/scale or usec per frame),
;; so it may come back as e.g. 23.9998 instead of 24.0
assert [0.01 > abs (cap/fps - 24.0)] "capture fps"
;; fourcc is backend specific: FFmpeg/MJPEG report the file's codec (MJPG),
;; AVFoundation reports its decoded output format (BGR3)
fourcc: to binary! to integer! cap/fourcc
assert [find ["MJPG" "BGR3"] to string! reverse skip fourcc 4] "capture fourcc is MJPG or BGR3"
assert-equal 5    cap/frames "capture frame count"
assert [0.0 = cap/pos-frame] "capture starts at frame 0"
cv/read cap
assert [1.0 = cap/pos-frame] "pos-frame advances after read"
cv/free cap

; ============================================================
; 20. Encoding and decoding in memory
; ============================================================
header "imencode / imdecode"

png: cv/imencode %.png mat
assert [binary? png] "imencode returns binary"
assert-equal #{89504E47} copy/part png 4 "imencode %.png writes PNG"
assert-equal #{89504E47} copy/part cv/imencode "png" mat 4 "imencode accepts extension without dot"
assert-equal #{89504E47} copy/part cv/imencode 'png mat 4 "imencode accepts codec word"
assert-equal #{FFD8} copy/part cv/imencode 'jpeg mat 2 "imencode 'jpeg writes JPEG"
assert-equal #{49492A00} copy/part cv/imencode 'tiff mat 4 "imencode 'tiff writes TIFF"
assert-error "imencode with unknown codec word errors" [cv/imencode 'xyz mat]

m: cv/imdecode png
assert [handle? m] "imdecode returns cvMat"
assert-equal mat/size m/size "imdecode keeps the size"
assert-equal mat/binary m/binary "PNG round trip is lossless"

img: cv/imdecode/image png
assert [image? img] "imdecode/image returns image"
assert-equal mat/image img "imdecode/image equals mat/image"

m: cv/imdecode next join #{00} png
assert-equal mat/size m/size "imdecode honours the binary index"

m: cv/imdecode cv/imencode %.png mat/image
assert-equal mat/size m/size "imencode from image!"

jpg-lo: cv/imencode/with %.jpg mat reduce [cv/IMWRITE_JPEG_QUALITY 10]
jpg-hi: cv/imencode/with %.jpg mat reduce [cv/IMWRITE_JPEG_QUALITY 95]
assert [(length? jpg-lo) < length? jpg-hi] "imencode uses the quality parameter"
assert-equal #{FFD8} copy/part jpg-hi 2 "imencode %.jpg writes JPEG"

assert-equal #{49492A00} copy/part cv/imencode %.tiff mat 4 "imencode %.tiff writes TIFF"
assert-equal #{52494646} copy/part cv/imencode %.webp mat 4 "imencode %.webp writes WebP"

m16: cv/Matrix [4x4 CV_16UC1 #{FFFF}]
img: cv/imdecode/image cv/imencode %.png m16
assert-equal 255 first img/1 "16-bit image is scaled to 8 bits"

assert [none? cv/imdecode #{00010203}] "imdecode of garbage returns none"
assert [none? cv/imdecode #{}] "imdecode of empty binary returns none"
assert-error "imencode with unknown format errors" [cv/imencode %.xyz mat]
assert-error "imencode with released handle errors" [cv/imencode %.png m6]

;; a released handle must not come back as a newly allocated one
m: cv/Matrix 4x4
cv/free m
n: cv/Matrix 8x8   ;; used to get the same HOB as `m`
assert [none? m/size] "released handle stays released after a new allocation"
assert-equal 8x8 n/size "new handle is not affected"
assert-error "released handle errors after a new allocation" [cv/imencode %.png m]
recycle
assert [none? m/size] "released handle stays released after recycle"
assert-equal 8x8 n/size "new handle survives recycle"

; ============================================================
; 21. Rebol codecs
; ============================================================
header "Rebol codecs"

foreach name [cv-png cv-jpeg cv-bmp cv-tiff cv-webp] [
	assert [object? select system/codecs name] join "codec registered: " name
]
assert [not find system/catalog/file-types 'cv-png] "cv-* codecs take no suffixes"
assert [not function? try [:system/codecs/cv-png/identify]] "cv-* codecs do not identify"

bin: encode 'cv-png mat/image
assert-equal #{89504E47} copy/part bin 4 "encode 'cv-png"
assert-equal mat/image decode 'cv-png bin "decode 'cv-png round trip"
assert-equal #{89504E47} copy/part encode 'cv-png mat 4 "encode 'cv-png from cvMat"

jpg-lo: encode/as 'cv-jpeg mat/image 10
jpg-hi: encode/as 'cv-jpeg mat/image 95
assert [(length? jpg-lo) < length? jpg-hi] "encode/as integer sets the quality"
jpg-blk: encode/as 'cv-jpeg mat/image reduce [cv/IMWRITE_JPEG_QUALITY 10]
assert-equal length? jpg-lo length? jpg-blk "encode/as block passes parameters"

assert-error "decode of invalid data errors" [decode 'cv-png #{00010203}]

;; plain names are registered only where the core has no codec
foreach name [png jpeg bmp tiff webp] [
	codec: select system/codecs name
	if all [codec  string? codec/title  find codec/title "(OpenCV)"] [
		print ["Core has no" name "codec - using OpenCV"]
		file: join %build/test-codec. name
		save file mat/image
		assert-equal name encoding? read file join "encoding? identifies " name
		img: load file
		assert-equal mat/size img/size join "save/load via suffix: " name
	]
]

; ============================================================
; 22. HighGUI (not part of the static build)
; ============================================================
header "HighGUI"

either static? [
    assert [error? err: try [cv/destroyAllWindows]] "GUI command errors in static build"
    assert [find form err "no highgui"] "error reports missing highgui"
][
    print "GUI tests need a display - skipped"
]

; ============================================================
; Summary
; ============================================================
print ""
print as-green "========================================"
print as-green "All tests passed!"
print as-green "========================================"
