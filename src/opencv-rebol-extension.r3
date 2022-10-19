REBOL [
	title: "OpenCV module builder"
	type: module
]

cmd-words: []
arg-words: []

commands: [
	; init-words is internal extension initialization (for mapping Rebol words to local equivalents)
	init-words: [cmd-words [block!] arg-words [block!]]
	test: [{Simple OpenCV test}]

	;-----------------------------------------------------------------------
	;- Constructors                                                         
	;-----------------------------------------------------------------------
	Matrix: [
		"Initialize new cvMat class"
		spec [pair! handle! image! vector!]
		/as type [integer!]
	]
	VideoCapture: [
		"Initialize new VideoCapture class"
		src [integer! file! string!]
	]
	VideoWriter: [
		"Initialize new VideoWriter class"
		src    [integer! file! string!]
		codec  [integer!] "4-character code of codec used to compress the frames (mp4ra.org/#/codecs)"
		fps    [number!]  "Framerate of the created video stream"
		size   [pair!]    "Size of the video frames"
	]
	free: [
		"Release OpenCV handle"
		class [handle!] "Mat, VideoCapture or VideoWriter"
	]

	;-----------------------------------------------------------------------
	;- Accessors                                                            
	;-----------------------------------------------------------------------
	get-property: [
		"Returns a property value"
		obj [handle!] "VideoCapture handle"
		property [integer!]
	]
	set-property: [
		obj [handle!] "VideoCapture handle"
		property [integer!]
		value [decimal!]
	]
	read: [
		"Grabs, decodes and returns the next video frame"
		src [handle!] "VideoCapture"
		/into "Optional existing Mat handle"
		 dst [handle!] "Mat"
	]
	write: [
		"Writes the next video frame"
		dst [handle!] "VideoWriter"
		frame [image! handle!]
	]

	;-----------------------------------------------------------------------
	;- Image file reading and writing                                       
	;- https://docs.opencv.org/4.6.0/d4/da8/group__imgcodecs.html           
	;-----------------------------------------------------------------------
	imread: [
		src [file! string!]
	]
	imwrite: [
		"Saves an image to a specified file."
		name [any-string!]
		image [image! handle!] "Image or cvMat handle"
		/with "Format-specific parameters encoded as pairs"
		params [block!] "integer pairs (words are resolved)"
	]

	;-----------------------------------------------------------------------
	;- Geometric Image Transformations                                      
	;- https://docs.opencv.org/4.6.0/da/d54/group__imgproc__transform.html  
	;-----------------------------------------------------------------------
	;convertMaps
	;getAffineTransform
	;getPerspectiveTransform
	;getRectSubPix
	;getRotationMatrix2D 
	;invertAffineTransform 
	;linearPolar 
	;logPolar 
	;remap 
	resize: [
		"Resizes an image."
		image [image! handle!] "Image or cvMat handle"
		size  [pair! percent!]
		/into
		target [handle!] "cvMat"
		/with
		interpolation [integer!]
	]
	;warpAffine 
	;warpPerspective 
	;warpPolar 

	;-----------------------------------------------------------------------
	;- Image Filtering                                                      
	;- https://docs.opencv.org/4.6.0/d4/d86/group__imgproc__filter.html     
	;-----------------------------------------------------------------------
	bilateralFilter: [
		"Applies the bilateral filter to an image."
		image [image! handle!] "Image or cvMat handle"
		diameter [integer!]
		sigmaColor [decimal!]
		sigmaSpace [decimal!]
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	blur: [
		"Blurs an image using the normalized box filter."
		image [image! handle!] "Image or cvMat handle"
		size [pair!] "blurring kernel size"
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	;boxFilter
	;buildPyramid
	;dilate
	;erode
	;filter2D
	;GaussianBlur
	;getDerivKernels
	;getGaborKernel
	;getGaussianKernel
	;getStructuringElement
	;Laplacian
	;medianBlur
	;morphologyEx
	;pyrDown
	;pyrMeanShiftFiltering
	;pyrUp
	;Scharr
	;sepFilter2D
	;Sobel
	;spatialGradient
	;sqrBoxFilter

	;-----------------------------------------------------------------------
	;- Color Space Conversions                                              
	;- https://docs.opencv.org/4.6.0/d8/d01/group__imgproc__color__conversions.html
	;-----------------------------------------------------------------------
	cvtColor: [
		"Converts an image from one color space to another."
		image [image! handle!] "Image or cvMat handle"
		code [integer!]
	]

	;-----------------------------------------------------------------------
	;- Image Thresholding                                                   
	;- https://docs.opencv.org/4.x/d7/d4d/tutorial_py_thresholding.html     
	;-----------------------------------------------------------------------
	threshold: [
		"Applies a fixed-level threshold to each array element."
		src    [handle!]
		dst    [handle!]
		thresh [number!]
		maxval [number!]
		type   [integer!]
	]
	;adaptiveThreshold

	;-----------------------------------------------------------------------
	;- Operations on arrays                                                 
	;- https://docs.opencv.org/4.6.0/d2/de8/group__core__array.html         
	;-----------------------------------------------------------------------
	;absdiff
	add: [
		"Calculates the per-element sum of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	addWeighted: [
		"Calculates the weighted sum of two arrays."
		src1 [handle!] "cvMat"
		alpha [number!] "weight of the first array elements."
		src2 [handle!] "cvMat"
		beta [number!] "weight of the second array elements."
		gamma [number!]	"scalar added to each sum."
		dst [handle!] "cvMat"
	]
	;batchDistance	
	bitwise-and: [
		"Computes bitwise conjunction of the two arrays (dst = src1 & src2)"
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	bitwise-not: [
		"Inverts every bit of an array."
		src [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	bitwise-or: [
		"Calculates the per-element bit-wise disjunction of two arrays or an array and a scalar."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	bitwise-xor: [
		{Calculates the per-element bit-wise "exclusive or" operation on two arrays or an array and a scalar.}
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	;borderInterpolate
	;calcCovarMatrix 
	;cartToPolar
	;checkRange 
	;compare
	;completeSymm 
	;convertFp16
	;convertScaleAbs
	;copyMakeBorder
	;copyTo
	;countNonZero
	;dct 
	;determinant 
	;dft 
	divide: [
		"Calculates the per-element division of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/scale "scalar factor"
		s    [number!] "default = 1"
	]
	;eigen
	;eigenNonSymmetric
	;exp 
	;extractChannel 
	;findNonZero 
	;flip 
	;gemm
	;getOptimalDFTSize
	;hconcat
	;idct
	;idft 
	;inRange
	;insertChannel 
	;invert
	;log 
	;LUT
	;magnitude 
	;Mahalanobis
	;max 
	;mean
	;meanStdDev 
	;merge 
	;min 
	;minMaxIdx
	;minMaxLoc
	;mixChannels 
	;mulSpectrums
	multiply: [
		"Calculates the per-element scaled product of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/scale "scalar factor"
		s    [number!] "default = 1"
	]
	;mulTransposed 
	;norm 
	;normalize 
	;patchNaNs
	;...
	subtract: [
		"Calculates the per-element difference between two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		/into
		dst  [handle!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	;...
	

	;-----------------------------------------------------------------------
	;- Mat class                                                            
	;- https://docs.opencv.org/4.6.0/d3/d63/classcv_1_1Mat.html             
	;-----------------------------------------------------------------------
	convertTo: [
		"Converts an array to another data type with optional scaling."
		src   [handle!] "cvMat"
		dst   [handle!] "cvMat"
		type  [integer!] "desired output matrix type or, rather, the depth since the number of channels are the same as the input has; if rtype is negative, the output matrix will have the same type as the input"
		alpha [number!] "scale factor"
		beta  [number!] "delta added to the scaled values"
	]

	;-----------------------------------------------------------------------
	;- High-level GUI                                                       
	;- https://docs.opencv.org/4.6.0/d7/dfc/group__highgui.html             
	;-----------------------------------------------------------------------
	startWindowThread: []
	imshow: [
		"Displays an image in the specified window."
		src [image! handle!] "Image or cvMat handle"
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
	getWindowProperty: [
		"Provides parameters of a window."
		name [any-string!]
		property [integer!]
	]
	setWindowProperty: [
		"Changes parameters of a window dynamically."
		name [any-string!]
		property [integer!]
		value [number!]
	]
	destroyAllWindows: ["Destroys all of the HighGUI windows."]
	destroyWindow: ["Destroys the specified window." window [any-string!]]


	;-----------------------------------------------------------------------
	;- Utilities                                                            
	;-----------------------------------------------------------------------
	getTickCount: ["Returns the number of ticks."]
	getTickFrequency: ["Returns the number of ticks per second."]
	getNumThreads: ["Returns the number of threads used by OpenCV for parallel regions."]
	getVersionString: ["Returns library version string."]
	getBuildInformation: ["Returns full configuration time cmake output."]
	useOptimized: ["Returns the status of optimized code usage."]
	setUseOptimized: ["Enables or disables the optimized code." onoff [logic!]]
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
BORDER_ISOLATED: 16

; ColorConversionCodes
COLOR_BGR2BGRA: 0
COLOR_RGB2RGBA: COLOR_BGR2BGRA
COLOR_BGRA2BGR: 1
COLOR_RGBA2RGB: COLOR_BGRA2BGR
COLOR_BGR2RGBA: 2
COLOR_RGB2BGRA: COLOR_BGR2RGBA
COLOR_RGBA2BGR: 3
COLOR_BGRA2RGB: COLOR_RGBA2BGR
COLOR_BGR2RGB: 4
COLOR_RGB2BGR: COLOR_BGR2RGB
COLOR_BGRA2RGBA: 5
COLOR_RGBA2BGRA: COLOR_BGRA2RGBA
COLOR_BGR2GRAY: 6
COLOR_RGB2GRAY: 7
COLOR_GRAY2BGR: 8
COLOR_GRAY2RGB: COLOR_GRAY2BGR
COLOR_GRAY2BGRA: 9
COLOR_GRAY2RGBA: COLOR_GRAY2BGRA
COLOR_BGRA2GRAY: 10
COLOR_RGBA2GRAY: 11
COLOR_BGR2BGR565: 12
COLOR_RGB2BGR565: 13
COLOR_BGR5652BGR: 14
COLOR_BGR5652RGB: 15
COLOR_BGRA2BGR565: 16
COLOR_RGBA2BGR565: 17
COLOR_BGR5652BGRA: 18
COLOR_BGR5652RGBA: 19
COLOR_GRAY2BGR565: 20
COLOR_BGR5652GRAY: 21
COLOR_BGR2BGR555: 22
COLOR_RGB2BGR555: 23
COLOR_BGR5552BGR: 24
COLOR_BGR5552RGB: 25
COLOR_BGRA2BGR555: 26
COLOR_RGBA2BGR555: 27
COLOR_BGR5552BGRA: 28
COLOR_BGR5552RGBA: 29
COLOR_GRAY2BGR555: 30
COLOR_BGR5552GRAY: 31
COLOR_BGR2XYZ: 32
COLOR_RGB2XYZ: 33
COLOR_XYZ2BGR: 34
COLOR_XYZ2RGB: 35
COLOR_BGR2YCrCb: 36
COLOR_RGB2YCrCb: 37
COLOR_YCrCb2BGR: 38
COLOR_YCrCb2RGB: 39
COLOR_BGR2HSV: 40
COLOR_RGB2HSV: 41
COLOR_BGR2Lab: 44
COLOR_RGB2Lab: 45
COLOR_BGR2Luv: 50
COLOR_RGB2Luv: 51
COLOR_BGR2HLS: 52
COLOR_RGB2HLS: 53
COLOR_HSV2BGR: 54
COLOR_HSV2RGB: 55
COLOR_Lab2BGR: 56
COLOR_Lab2RGB: 57
COLOR_Luv2BGR: 58
COLOR_Luv2RGB: 59
COLOR_HLS2BGR: 60
COLOR_HLS2RGB: 61
COLOR_BGR2HSV_FULL: 66
COLOR_RGB2HSV_FULL: 67
COLOR_BGR2HLS_FULL: 68
COLOR_RGB2HLS_FULL: 69
COLOR_HSV2BGR_FULL: 70
COLOR_HSV2RGB_FULL: 71
COLOR_HLS2BGR_FULL: 72
COLOR_HLS2RGB_FULL: 73
COLOR_LBGR2Lab: 74
COLOR_LRGB2Lab: 75
COLOR_LBGR2Luv: 76
COLOR_LRGB2Luv: 77
COLOR_Lab2LBGR: 78
COLOR_Lab2LRGB: 79
COLOR_Luv2LBGR: 80
COLOR_Luv2LRGB: 81
COLOR_BGR2YUV: 82
COLOR_RGB2YUV: 83
COLOR_YUV2BGR: 84
COLOR_YUV2RGB: 85
COLOR_YUV2RGB_NV12: 90
COLOR_YUV2BGR_NV12: 91
COLOR_YUV2RGB_NV21: 92
COLOR_YUV2BGR_NV21: 93
COLOR_YUV420sp2RGB: COLOR_YUV2RGB_NV21
COLOR_YUV420sp2BGR: COLOR_YUV2BGR_NV21
COLOR_YUV2RGBA_NV12: 94
COLOR_YUV2BGRA_NV12: 95
COLOR_YUV2RGBA_NV21: 96
COLOR_YUV2BGRA_NV21: 97
COLOR_YUV420sp2RGBA: COLOR_YUV2RGBA_NV21
COLOR_YUV420sp2BGRA: COLOR_YUV2BGRA_NV21
COLOR_YUV2RGB_YV12: 98
COLOR_YUV2BGR_YV12: 99
COLOR_YUV2RGB_IYUV: 100
COLOR_YUV2BGR_IYUV: 101
COLOR_YUV2RGB_I420: COLOR_YUV2RGB_IYUV
COLOR_YUV2BGR_I420: COLOR_YUV2BGR_IYUV
COLOR_YUV420p2RGB: COLOR_YUV2RGB_YV12
COLOR_YUV420p2BGR: COLOR_YUV2BGR_YV12
COLOR_YUV2RGBA_YV12: 102
COLOR_YUV2BGRA_YV12: 103
COLOR_YUV2RGBA_IYUV: 104
COLOR_YUV2BGRA_IYUV: 105
COLOR_YUV2RGBA_I420: COLOR_YUV2RGBA_IYUV
COLOR_YUV2BGRA_I420: COLOR_YUV2BGRA_IYUV
COLOR_YUV420p2RGBA: COLOR_YUV2RGBA_YV12
COLOR_YUV420p2BGRA: COLOR_YUV2BGRA_YV12
COLOR_YUV2GRAY_420: 106
COLOR_YUV2GRAY_NV21: COLOR_YUV2GRAY_420
COLOR_YUV2GRAY_NV12: COLOR_YUV2GRAY_420
COLOR_YUV2GRAY_YV12: COLOR_YUV2GRAY_420
COLOR_YUV2GRAY_IYUV: COLOR_YUV2GRAY_420
COLOR_YUV2GRAY_I420: COLOR_YUV2GRAY_420
COLOR_YUV420sp2GRAY: COLOR_YUV2GRAY_420
COLOR_YUV420p2GRAY: COLOR_YUV2GRAY_420
COLOR_YUV2RGB_UYVY: 107
COLOR_YUV2BGR_UYVY: 108
COLOR_YUV2RGB_Y422: COLOR_YUV2RGB_UYVY
COLOR_YUV2BGR_Y422: COLOR_YUV2BGR_UYVY
COLOR_YUV2RGB_UYNV: COLOR_YUV2RGB_UYVY
COLOR_YUV2BGR_UYNV: COLOR_YUV2BGR_UYVY
COLOR_YUV2RGBA_UYVY: 111
COLOR_YUV2BGRA_UYVY: 112
COLOR_YUV2RGBA_Y422: COLOR_YUV2RGBA_UYVY
COLOR_YUV2BGRA_Y422: COLOR_YUV2BGRA_UYVY
COLOR_YUV2RGBA_UYNV: COLOR_YUV2RGBA_UYVY
COLOR_YUV2BGRA_UYNV: COLOR_YUV2BGRA_UYVY
COLOR_YUV2RGB_YUY2: 115
COLOR_YUV2BGR_YUY2: 116
COLOR_YUV2RGB_YVYU: 117
COLOR_YUV2BGR_YVYU: 118
COLOR_YUV2RGB_YUYV: COLOR_YUV2RGB_YUY2
COLOR_YUV2BGR_YUYV: COLOR_YUV2BGR_YUY2
COLOR_YUV2RGB_YUNV: COLOR_YUV2RGB_YUY2
COLOR_YUV2BGR_YUNV: COLOR_YUV2BGR_YUY2
COLOR_YUV2RGBA_YUY2: 119
COLOR_YUV2BGRA_YUY2: 120
COLOR_YUV2RGBA_YVYU: 121
COLOR_YUV2BGRA_YVYU: 122
COLOR_YUV2RGBA_YUYV: COLOR_YUV2RGBA_YUY2
COLOR_YUV2BGRA_YUYV: COLOR_YUV2BGRA_YUY2
COLOR_YUV2RGBA_YUNV: COLOR_YUV2RGBA_YUY2
COLOR_YUV2BGRA_YUNV: COLOR_YUV2BGRA_YUY2
COLOR_YUV2GRAY_UYVY: 123
COLOR_YUV2GRAY_YUY2: 124
COLOR_YUV2GRAY_Y422: COLOR_YUV2GRAY_UYVY
COLOR_YUV2GRAY_UYNV: COLOR_YUV2GRAY_UYVY
COLOR_YUV2GRAY_YVYU: COLOR_YUV2GRAY_YUY2
COLOR_YUV2GRAY_YUYV: COLOR_YUV2GRAY_YUY2
COLOR_YUV2GRAY_YUNV: COLOR_YUV2GRAY_YUY2
COLOR_RGBA2mRGBA: 125
COLOR_mRGBA2RGBA: 126
COLOR_RGB2YUV_I420: 127
COLOR_BGR2YUV_I420: 128
COLOR_RGB2YUV_IYUV: COLOR_RGB2YUV_I420
COLOR_BGR2YUV_IYUV: COLOR_BGR2YUV_I420
COLOR_RGBA2YUV_I420: 129
COLOR_BGRA2YUV_I420: 130
COLOR_RGBA2YUV_IYUV: COLOR_RGBA2YUV_I420
COLOR_BGRA2YUV_IYUV: COLOR_BGRA2YUV_I420
COLOR_RGB2YUV_YV12: 131
COLOR_BGR2YUV_YV12: 132
COLOR_RGBA2YUV_YV12: 133
COLOR_BGRA2YUV_YV12: 134
COLOR_BayerBG2BGR: 46
COLOR_BayerGB2BGR: 47
COLOR_BayerRG2BGR: 48
COLOR_BayerGR2BGR: 49
COLOR_BayerRGGB2BGR: COLOR_BayerBG2BGR
COLOR_BayerGRBG2BGR: COLOR_BayerGB2BGR
COLOR_BayerBGGR2BGR: COLOR_BayerRG2BGR
COLOR_BayerGBRG2BGR: COLOR_BayerGR2BGR
COLOR_BayerRGGB2RGB: COLOR_BayerBGGR2BGR
COLOR_BayerGRBG2RGB: COLOR_BayerGBRG2BGR
COLOR_BayerBGGR2RGB: COLOR_BayerRGGB2BGR
COLOR_BayerGBRG2RGB: COLOR_BayerGRBG2BGR
COLOR_BayerBG2RGB: COLOR_BayerRG2BGR
COLOR_BayerGB2RGB: COLOR_BayerGR2BGR
COLOR_BayerRG2RGB: COLOR_BayerBG2BGR
COLOR_BayerGR2RGB: COLOR_BayerGB2BGR
COLOR_BayerBG2GRAY: 86
COLOR_BayerGB2GRAY: 87
COLOR_BayerRG2GRAY: 88
COLOR_BayerGR2GRAY: 89
COLOR_BayerRGGB2GRAY: COLOR_BayerBG2GRAY
COLOR_BayerGRBG2GRAY: COLOR_BayerGB2GRAY
COLOR_BayerBGGR2GRAY: COLOR_BayerRG2GRAY
COLOR_BayerGBRG2GRAY: COLOR_BayerGR2GRAY
COLOR_BayerBG2BGR_VNG: 62
COLOR_BayerGB2BGR_VNG: 63
COLOR_BayerRG2BGR_VNG: 64
COLOR_BayerGR2BGR_VNG: 65
COLOR_BayerRGGB2BGR_VNG: COLOR_BayerBG2BGR_VNG
COLOR_BayerGRBG2BGR_VNG: COLOR_BayerGB2BGR_VNG
COLOR_BayerBGGR2BGR_VNG: COLOR_BayerRG2BGR_VNG
COLOR_BayerGBRG2BGR_VNG: COLOR_BayerGR2BGR_VNG
COLOR_BayerRGGB2RGB_VNG: COLOR_BayerBGGR2BGR_VNG
COLOR_BayerGRBG2RGB_VNG: COLOR_BayerGBRG2BGR_VNG
COLOR_BayerBGGR2RGB_VNG: COLOR_BayerRGGB2BGR_VNG
COLOR_BayerGBRG2RGB_VNG: COLOR_BayerGRBG2BGR_VNG
COLOR_BayerBG2RGB_VNG: COLOR_BayerRG2BGR_VNG
COLOR_BayerGB2RGB_VNG: COLOR_BayerGR2BGR_VNG
COLOR_BayerRG2RGB_VNG: COLOR_BayerBG2BGR_VNG
COLOR_BayerGR2RGB_VNG: COLOR_BayerGB2BGR_VNG
COLOR_BayerBG2BGR_EA: 135
COLOR_BayerGB2BGR_EA: 136
COLOR_BayerRG2BGR_EA: 137
COLOR_BayerGR2BGR_EA: 138
COLOR_BayerRGGB2BGR_EA: COLOR_BayerBG2BGR_EA
COLOR_BayerGRBG2BGR_EA: COLOR_BayerGB2BGR_EA
COLOR_BayerBGGR2BGR_EA: COLOR_BayerRG2BGR_EA
COLOR_BayerGBRG2BGR_EA: COLOR_BayerGR2BGR_EA
COLOR_BayerRGGB2RGB_EA: COLOR_BayerBGGR2BGR_EA
COLOR_BayerGRBG2RGB_EA: COLOR_BayerGBRG2BGR_EA
COLOR_BayerBGGR2RGB_EA: COLOR_BayerRGGB2BGR_EA
COLOR_BayerGBRG2RGB_EA: COLOR_BayerGRBG2BGR_EA
COLOR_BayerBG2RGB_EA: COLOR_BayerRG2BGR_EA
COLOR_BayerGB2RGB_EA: COLOR_BayerGR2BGR_EA
COLOR_BayerRG2RGB_EA: COLOR_BayerBG2BGR_EA
COLOR_BayerGR2RGB_EA: COLOR_BayerGB2BGR_EA
COLOR_BayerBG2BGRA: 139
COLOR_BayerGB2BGRA: 140
COLOR_BayerRG2BGRA: 141
COLOR_BayerGR2BGRA: 142
COLOR_BayerRGGB2BGRA: COLOR_BayerBG2BGRA
COLOR_BayerGRBG2BGRA: COLOR_BayerGB2BGRA
COLOR_BayerBGGR2BGRA: COLOR_BayerRG2BGRA
COLOR_BayerGBRG2BGRA: COLOR_BayerGR2BGRA
COLOR_BayerRGGB2RGBA: COLOR_BayerBGGR2BGRA
COLOR_BayerGRBG2RGBA: COLOR_BayerGBRG2BGRA
COLOR_BayerBGGR2RGBA: COLOR_BayerRGGB2BGRA
COLOR_BayerGBRG2RGBA: COLOR_BayerGRBG2BGRA
COLOR_BayerBG2RGBA: COLOR_BayerRG2BGRA
COLOR_BayerGB2RGBA: COLOR_BayerGR2BGRA
COLOR_BayerRG2RGBA: COLOR_BayerBG2BGRA
COLOR_BayerGR2RGBA: COLOR_BayerGB2BGRA

; VideoCaptureProperties
CAP_PROP_POS_MSEC: 0
CAP_PROP_POS_FRAMES: 1
CAP_PROP_POS_AVI_RATIO: 2
CAP_PROP_FRAME_WIDTH: 3
CAP_PROP_FRAME_HEIGHT: 4
CAP_PROP_FPS: 5
CAP_PROP_FOURCC: 6
CAP_PROP_FRAME_COUNT: 7
CAP_PROP_FORMAT: 8
CAP_PROP_MODE: 9
CAP_PROP_BRIGHTNESS: 10
CAP_PROP_CONTRAST: 11
CAP_PROP_SATURATION: 12
CAP_PROP_HUE: 13
CAP_PROP_GAIN: 14
CAP_PROP_EXPOSURE: 15
CAP_PROP_CONVERT_RGB: 16
CAP_PROP_WHITE_BALANCE_BLUE_U: 17
CAP_PROP_RECTIFICATION: 18
CAP_PROP_MONOCHROME: 19
CAP_PROP_SHARPNESS: 20
CAP_PROP_AUTO_EXPOSURE: 21
CAP_PROP_GAMMA: 22
CAP_PROP_TEMPERATURE: 23
CAP_PROP_TRIGGER: 24
CAP_PROP_TRIGGER_DELAY: 25
CAP_PROP_WHITE_BALANCE_RED_V: 26
CAP_PROP_ZOOM: 27
CAP_PROP_FOCUS: 28
CAP_PROP_GUID: 29
CAP_PROP_ISO_SPEED: 30
CAP_PROP_BACKLIGHT: 32
CAP_PROP_PAN: 33
CAP_PROP_TILT: 34
CAP_PROP_ROLL: 35
CAP_PROP_IRIS: 36
CAP_PROP_SETTINGS: 37
CAP_PROP_BUFFERSIZE: 38
CAP_PROP_AUTOFOCUS: 39
CAP_PROP_SAR_NUM: 40
CAP_PROP_SAR_DEN: 41
CAP_PROP_BACKEND: 42
CAP_PROP_CHANNEL: 43
CAP_PROP_AUTO_WB: 44
CAP_PROP_WB_TEMPERATURE: 45
CAP_PROP_CODEC_PIXEL_FORMAT: 46
CAP_PROP_BITRATE: 47
CAP_PROP_ORIENTATION_META: 48
CAP_PROP_ORIENTATION_AUTO: 49
CAP_PROP_OPEN_TIMEOUT_MSEC: 53
CAP_PROP_READ_TIMEOUT_MSEC: 54

; WindowPropertyFlags
WND_PROP_FULLSCREEN: 0
WND_PROP_AUTOSIZE: 1
WND_PROP_ASPECT_RATIO: 2
WND_PROP_OPENGL: 3
WND_PROP_VISIBLE: 4
WND_PROP_TOPMOST: 5
; WindowFlags
WINDOW_NORMAL: 0
WINDOW_AUTOSIZE: 1  ;0x00000001
WINDOW_OPENGL: 4096 ;0x00001000
WINDOW_FULLSCREEN: 1
WINDOW_FREERATIO: 256 ;0x00000100
WINDOW_KEEPRATIO: 0
WINDOW_GUI_EXPANDED: 0
WINDOW_GUI_NORMAL: 16

; MatProperties
MAT_SIZE: 1
MAT_TYPE: 2
MAT_DEPTH: 3
MAT_CHANNELS: 4
MAT_BINARY: 5
MAT_IMAGE: 6
MAT_VECTOR: 7

; InterpolationFlags
INTER_NEAREST: 0
INTER_LINEAR: 1
INTER_CUBIC: 2
INTER_AREA: 3
INTER_LANCZOS4: 4
INTER_LINEAR_EXACT: 5
INTER_NEAREST_EXACT: 6

; ThresholdTypes
THRESH_BINARY: 0
THRESH_BINARY_INV: 1
THRESH_TRUNC: 2
THRESH_TOZERO: 3
THRESH_TOZERO_INV: 4
THRESH_MASK: 7
THRESH_OTSU: 8
THRESH_TRIANGLE: 16

; Depths:
CV_8U:   0
CV_8S:   1
CV_16U:  2
CV_16S:  3
CV_32S:  4
CV_32F:  5
CV_64F:  6
}

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
