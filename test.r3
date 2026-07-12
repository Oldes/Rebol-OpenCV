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
    system/options/modules: dirize to-real-file %build/
    print ["Using modified modules location:" as-green system/options/modules]
    ls (system/options/modules)
    either system/platform = 'Windows [
        set-env "PATH" ajoin [
            to-local-file to-real-file rejoin [%build/ system/build/arch %/vc17/bin/]
            #";" get-env "PATH"
        ]
    ][
        set-env "LD_LIBRARY_PATH" "/usr/local/lib:/lib:/usr/lib"
    ]
]

try [system/modules/opencv: none]

cv: import opencv

; ============================================================
; Helper: print a test group header
; ============================================================
header: func [
    "Print a formatted test group header."
    title [string!] "Group title."
][
    print ""
    print as-green ajoin [ "=== " title " ===" ]
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
    if error? try body [
        print as-green rejoin ["PASS: " label]
        return
    ]
    print as-red rejoin ["FAIL: " label " — expected error but none occurred"]
    quit/return 1
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
m4: cv/Matrix [100x100 v]
assert [handle? m4] "Matrix from vector data"
assert [m4/size = 100x100] "Matrix size from vector"

; --- from image ---
img: make image! 64x64
m5: cv/Matrix img
assert [handle? m5] "Matrix from image!"
assert [m5/size = 64x64] "Matrix size from image"

; --- copy from existing mat ---
m6: cv/Matrix m1
assert [handle? m6] "Matrix copy from handle"
assert [m6/size = m1/size] "Matrix copy same size"

; --- free ---
cv/free m6
assert-error "Access freed handle errors" [
    probe m6/size
]

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
kernel: cv/Matrix [3x3 kernel-data]
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
added: cv/add mat gray none
assert [handle? added] "add"
assert [added/size = mat/size] "add same size"

; subtract
subbed: cv/subtract mat gray none
assert [handle? subbed] "subtract"

; multiply
mult: cv/multiply mat mat none
assert [handle? mult] "multiply"

; divide
divd: cv/divide mat mat none
assert [handle? divd] "divide"

; absdiff
diff: cv/absdiff mat gray none
assert [handle? diff] "absdiff"

; addWeighted
weighted: cv/addWeighted mat 0.5 gray 0.5 0 none
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
cvt: cv/convertTo gray none CV_32F 1.0 0.0
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
assert-error "imread missing file errors" [
    cv/imread %image/nonexistent.png
]

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

lap: cv/Laplacian gray none CV_8U 3 1 0
assert [handle? lap] "Laplacian"
assert [lap/channels = 1] "Laplacian single channel"

; ============================================================
; Summary
; ============================================================
print ""
print as-green "========================================"
print as-green "All tests passed!"
print as-green "========================================"
