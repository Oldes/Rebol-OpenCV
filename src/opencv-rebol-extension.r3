REBOL [
	title: "OpenCV module builder"
	type: module
]

type-words: [
	;@@ order of these is important!
	;- depths
	CV_8U
	CV_8S
	CV_16U
	CV_16S
	CV_32S
	CV_32F
	CV_64F
	CV_16F
	;- types:
	CV_8UC1
	CV_8SC1
	CV_16UC1
	CV_16SC1
	CV_32SC1
	CV_32FC1
	CV_64FC1
	CV_16FC1
	CV_8UC2
	CV_8SC2
	CV_16UC2
	CV_16SC2
	CV_32SC2
	CV_32FC2
	CV_64FC2
	CV_16FC2
	CV_8UC3
	CV_8SC3
	CV_16UC3
	CV_16SC3
	CV_32SC3
	CV_32FC3
	CV_64FC3
	CV_16FC3
	CV_8UC4
	CV_8SC4
	CV_16UC4
	CV_16SC4
	CV_32SC4
	CV_32FC4
	CV_64FC4
	CV_16FC4
]

commands: [
	; init-words is internal extension initialization (for mapping Rebol words to local equivalents)
	init-words: [args [block!] type [block!]] ;; used internaly only!
	test: [{Simple OpenCV test}]

	;-----------------------------------------------------------------------------------------------
	;- Constructors                                                                                 
	;-----------------------------------------------------------------------------------------------
	Matrix: [
		"Initialize new cvMat class"
		spec [pair! handle! image! block!]
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

	;-----------------------------------------------------------------------------------------------
	;- Accessors                                                                                    
	;-----------------------------------------------------------------------------------------------
	get-property: [
		"Returns a property value"
		obj [handle!] "VideoCapture handle"
		property [integer!]
	]
	set-property: [
		obj [handle!] "VideoCapture handle"
		property [integer!]
		value [number!]
	]
	read: [
		"Grabs, decodes and returns the next video frame"
		src [handle!] "VideoCapture"
		/into "Optional existing Mat handle"
		 dst [handle!] "Mat"
	]
	write: [
		"Writes the next video frame"
		dst   [handle!] "VideoWriter"
		frame [image! handle!]
	]

	;-----------------------------------------------------------------------------------------------
	;- Image file reading and writing                                                               
	;- https://docs.opencv.org/4.6.0/d4/da8/group__imgcodecs.html                                   
	;-----------------------------------------------------------------------------------------------
	imread: [
		src [file! string!] "Loads an image from a file."
		/image "As Rebol image instead of default cvMat"
		/with "Flag that can take values IMREAD_*"
		flags [integer!] "Default is IMREAD_UNCHANGED"
	]
	imreadmulti: [
		src [file! string!] "Loads a multi-page image from a file."
		/image "As Rebol images instead of default cvMat"
		/with "Flag that can take values IMREAD_*"
		flags [integer!] "Default is IMREAD_UNCHANGED"
	]
	imwrite: [
		"Saves an image to a specified file."
		name  [any-string!]
		image [image! handle!] "Image or cvMat handle"
		/with "Format-specific parameters encoded as pairs"
		params [block!] "integer pairs (words are resolved)"
	]

	;-----------------------------------------------------------------------------------------------
	;- Geometric Image Transformations                                                              
	;- https://docs.opencv.org/4.6.0/da/d54/group__imgproc__transform.html                          
	;-----------------------------------------------------------------------------------------------
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

	;-----------------------------------------------------------------------------------------------
	;- Image Filtering                                                                              
	;- https://docs.opencv.org/4.6.0/d4/d86/group__imgproc__filter.html                             
	;-----------------------------------------------------------------------------------------------
	bilateralFilter: [
		"Applies the bilateral filter to an image."
		src        [handle!] "Source 8-bit or floating-point, 1-channel or 3-channel image."
		dst        [handle! none!] "Destination image of the same size and type as src."
		diameter   [integer!] "Diameter of each pixel neighborhood that is used during filtering. If it is non-positive, it is computed from sigmaSpace."
		sigmaColor [number!]
		sigmaSpace [number!]
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	blur: [
		"Blurs an image using the normalized box filter."
		src [handle!] "cvMat handle"
		dst [handle! none!]
		size [pair! integer!] "blurring kernel size"
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	;boxFilter
	;buildPyramid
	dilate: [
		"Dilates an image by using a specific structuring element."
		src    [handle!] "cvMat"
		dst    [handle! none!] "cvMat"
		kernel [handle!]
		anchor [pair! integer!]	"position of the anchor within the element"
		iterations [integer!]   "number of times dilation is applied"
	]
	erode: [
		"Erodes an image by using a specific structuring element."
		src    [handle!] "cvMat"
		dst    [handle! none!] "cvMat"
		kernel [handle!]
		anchor [pair! integer!]	"position of the anchor within the element"
		iterations [integer!]   "number of times dilation is applied"
	]
	filter2D: [
		"Convolves an image with the kernel."
		src    [handle!] "cvMat"
		dst    [handle! none!] "cvMat"
		ddepth [integer!] "desired depth of the destination image"
		kernel [handle!] "convolution kernel (or rather a correlation kernel), a single-channel floating point matrix"
		anchor [pair! integer!]	"position of the anchor within the element"
		delta  [number!] "value added to the filtered pixels before storing them in dst"
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	GaussianBlur: [
		"Blurs an image using a Gaussian filter."
		src    [handle!] "cvMat"
		dst    [handle! none!] "cvMat"
		size   [pair! integer!]   "blurring kernel size"
		sigmaX [number!]
		sigmaY [number!]
		/border "border mode used to extrapolate pixels outside of the image"
		type [integer!] "one of: [0 1 2 4 5 16]"
	]
	;getDerivKernels
	getGaborKernel: [
		"Returns Gabor filter coefficients."
		ksize [pair!]    "Size of the filter returned"
		sigma [number!]  "Standard deviation of the gaussian envelope"
		theta [number!]  "Orientation of the normal to the parallel stripes of a Gabor function."
		lambd [number!]  "Wavelength of the sinusoidal factor"
		gamma [number!]  "Spatial aspect ratio"
		psi   [number!]  "Phase offset"
		ktype [integer!] "Type of filter coefficients. It can be CV_32F or CV_64F"
	]
	;getGaussianKernel
	getStructuringElement: [
		"Returns a structuring element of the specified size and shape for morphological operations."
		shape  [integer!] "Element shape that could be one of MorphShapes (rect = 0, cross = 1, ellipse = 2)"
		ksize  [pair! integer!] "Size of the structuring element"
		anchor [pair! integer!] "Anchor position within the element"
	]
	Laplacian: [
		"Calculates the Laplacian of an image."
		src    [handle!] "Source image"
		dst    [handle! none!] "Destination image of the same size and the same number of channels as src"
		ddepth [number!] "Desired depth of the destination image"
		ksize  [number!] "Aperture size used to compute the second-derivative filters. The size must be positive and odd."
		scale  [number!] "Scale factor for the computed Laplacian values."
		delta  [number!] "Optional delta value that is added to the results prior to storing them in dst."
		;/border "border mode used to extrapolate pixels outside of the image"
		;type [integer!] "one of: [0 1 2 4 5 16]"
	]
	medianBlur: [
		"Blurs an image using the median filter."
		src   [handle!] "input 1-, 3-, or 4-channel image; when ksize is 3 or 5, the image depth should be CV_8U, CV_16U, or CV_32F, for larger aperture sizes, it can only be CV_8U"
		dst   [handle! none!] "destination array of the same size and type as src"
		size  [number!] "aperture linear size; it must be odd and greater than 1, for example: 3, 5, 7..."
	]
	;morphologyEx
	;pyrDown
	;pyrMeanShiftFiltering
	;pyrUp
	;Scharr
	;sepFilter2D
	;Sobel
	;spatialGradient
	;sqrBoxFilter

	;-----------------------------------------------------------------------------------------------
	;- Color Space Conversions                                                                      
	;- https://docs.opencv.org/4.6.0/d8/d01/group__imgproc__color__conversions.html                 
	;-----------------------------------------------------------------------------------------------
	cvtColor: [
		"Converts an image from one color space to another."
		src  [handle!] "source cvMat handle"
		dst  [handle! none!] "destination cvMat"
		code [integer!]
	]

	;-----------------------------------------------------------------------------------------------
	;- Color Maps                                                                                   
	;- https://docs.opencv.org/4.6.0/d3/d50/group__imgproc__colormap.html                           
	;-----------------------------------------------------------------------------------------------

	applyColorMap: [
		"Applies a GNU Octave/MATLAB equivalent colormap on a given image."
		src  [handle!] "source cvMat handle"
		dst  [handle! none!] "destination cvMat"
		colormap [integer!]
	]

	;-----------------------------------------------------------------------------------------------
	;- Image Thresholding                                                                           
	;- https://docs.opencv.org/4.x/d7/d4d/tutorial_py_thresholding.html                             
	;-----------------------------------------------------------------------------------------------
	threshold: [
		"Applies a fixed-level threshold to each array element."
		src    [handle!]
		dst    [handle! none!]
		thresh [number!]
		maxval [number!]
		type   [integer!]
	]
	;adaptiveThreshold

	;-----------------------------------------------------------------------------------------------
	;- Operations on arrays                                                                         
	;- https://docs.opencv.org/4.6.0/d2/de8/group__core__array.html                                 
	;-----------------------------------------------------------------------------------------------
	absdiff: [
		"Calculates the per-element absolute difference between two arrays"
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
	]
	add: [
		"Calculates the per-element sum of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	addWeighted: [
		"Calculates the weighted sum of two arrays."
		src1  [handle!] "cvMat"
		alpha [number!] "weight of the first array elements."
		src2  [handle!] "cvMat"
		beta  [number!] "weight of the second array elements."
		gamma [number!]	"scalar added to each sum."
		dst   [handle! none!] "cvMat"
	]
	;batchDistance	
	bitwise-and: [
		"Computes bitwise conjunction of the two arrays (dst = src1 & src2)"
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	bitwise-not: [
		"Inverts every bit of an array."
		src  [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	bitwise-or: [
		"Calculates the per-element bit-wise disjunction of two arrays or an array and a scalar."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	bitwise-xor: [
		{Calculates the per-element bit-wise "exclusive or" operation on two arrays or an array and a scalar.}
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
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
	convertScaleAbs: [
		"Scales, calculates absolute values, and converts the result to 8-bit."
		src   [handle!] "cvMat"
		dst   [handle! none!] "cvMat"
		alpha [number!] "default = 1"
		beta  [number!]
	]
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
		dst  [handle! none!] "cvMat"
		/scale "scalar factor"
		s    [number!] "default = 1"
	]
	;eigen
	;eigenNonSymmetric
	;exp 
	;extractChannel 
	;findNonZero 
	flip: [
		"Flips a 2D array around vertical, horizontal, or both axes."
		src   [handle!] "cvMat"
		dst   [handle! none!] "cvMat"
		flipCode [integer!]	"a flag to specify how to flip the array; 0 means flipping around the x-axis and positive value (for example, 1) means flipping around y-axis. Negative value (for example, -1) means flipping around both axes"
	] 
	;gemm
	;getOptimalDFTSize
	;hconcat
	;idct
	;idft 
	;inRange
	;insertChannel 
	invert: [
		"Finds the inverse or pseudo-inverse of a matrix."
		src   [handle!] "Input floating-point M x N matrix."
		dst   [handle! none!] "Output matrix of N x M size and the same type as src."
		flags [integer!] "Inversion method; One of DecompTypes (DECOMP_*)"
	]
	;log 
	;LUT
	;magnitude 
	;Mahalanobis
	max: [
		"Calculates per-element maximum of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
	] 
	mean: [
		"Calculates an average (mean) of array elements."
		src [handle!] "cvMat"
	] 
	;meanStdDev 
	;merge 
	min: [
		"Calculates per-element minimum of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
	]
	;minMaxIdx
	minMaxLoc: [
		"Finds the global minimum and maximum in an array."
		src [handle!] "input single-channel array (cvMat)"
	]
	;mixChannels 
	;mulSpectrums
	multiply: [
		"Calculates the per-element scaled product of two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
		/scale "scalar factor"
		s    [number!] "default = 1"
	]
	;mulTransposed 
	;norm 
	normalize: [
		"Calculates the per-element scaled product of two arrays."
		src   [handle!]       "cvMat"
		dst   [handle! none!] "cvMat"
		alpha [number!] "norm value to normalize to or the lower range boundary in case of the range normalization"
		beta  [number!] "upper range boundary in case of the range normalization; it is not used for the norm normalization"
		norm_type [integer!] "normalization type"
		/mask
		m    [handle!] "optional operation mask"
	] 
	;patchNaNs
	;...
	subtract: [
		"Calculates the per-element difference between two arrays."
		src1 [handle!] "cvMat"
		src2 [handle!] "cvMat"
		dst  [handle! none!] "cvMat"
		/mask
		m    [handle!] "cvMat"
	]
	transform: [
		"Performs the matrix transformation of every array element."
		src [handle!] "cvMat"
		dst [handle! none!] "cvMat"
		m  [handle!] "transformation 2x2 or 2x3 floating-point matrix."
	]
	;...
	

	;-----------------------------------------------------------------------------------------------
	;- Mat class                                                                                    
	;- https://docs.opencv.org/4.6.0/d3/d63/classcv_1_1Mat.html                                     
	;-----------------------------------------------------------------------------------------------
	convertTo: [
		"Converts an array to another data type with optional scaling."
		src   [handle!] "cvMat"
		dst   [handle! none!] "cvMat"
		type  [integer! word!] "desired output matrix type or, rather, the depth since the number of channels are the same as the input has; if rtype is negative, the output matrix will have the same type as the input"
		alpha [number!] "scale factor"
		beta  [number!] "delta added to the scaled values"
	]

	;-----------------------------------------------------------------------------------------------
	;- Feature Detection                                                                            
	;- https://docs.opencv.org/4.6.0/dd/d1a/group__imgproc__feature.html                            
	;-----------------------------------------------------------------------------------------------
	Canny: [
		"Finds edges in an image using the Canny algorithm"
		src   [handle!] "8-bit input image"
		dst   [handle! none!] "output edge map; single channels 8-bit image, which has the same size as image"
		threshold1 [number!] "first threshold for the hysteresis procedure"
		threshold2 [number!] "second threshold for the hysteresis procedure"
	]

	;-----------------------------------------------------------------------------------------------
	;- High-level GUI                                                                               
	;- https://docs.opencv.org/4.6.0/d7/dfc/group__highgui.html                                     
	;-----------------------------------------------------------------------------------------------
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
	getWindowImageSize: [
		"Provides size of image in the window (not counting tarckbars and other controls)."
		window [any-string!]
	]
	getWindowImagePos: [
		"Provides position (left-bottom) of image in the window."
		window [any-string!]
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
	setWindowTitle: [
		"Updates window title."
		name   [any-string!] "Name of the window"
		title  [any-string!] "New title"
	]
	destroyAllWindows: ["Destroys all of the HighGUI windows."]
	destroyWindow: ["Destroys the specified window." window [any-string!]]

	createTrackbar: [
		trackbarname [any-string!]
		window [any-string!]
		count  [integer!] "Maximal position of the slider. The minimal position is always 0."
		/with
		ctx  [object!] "Callback's context"
		func [word!]   "Callback function name"
	]
	setTrackbarMax: [
		"Sets the trackbar maximum position."
		trackbar [handle!] "cvTrackbar"
		value  [integer!] "New maximum position."
	]
	setTrackbarMin: [
		"Sets the trackbar minimum position."
		trackbar [handle!] "cvTrackbar"
		value  [integer!] "New minimum position."
	]
	setTrackbarPos: [
		"Sets the trackbar position."
		trackbar [handle!] "cvTrackbar"
		value  [integer!] "New position."
	]
	getTrackbarPos: [
		"Gets the trackbar position."
		trackbar [handle!] "cvTrackbar"
	]

	selectROI: [
		"Allows users to select a ROI on the given image."
		src [image! handle!] "Image or cvMat handle"
	]


	;-----------------------------------------------------------------------------------------------
	;- Utilities                                                                                    
	;-----------------------------------------------------------------------------------------------
	getTickCount: ["Returns the number of ticks."]
	getTickFrequency: ["Returns the number of ticks per second."]
	getNumThreads: ["Returns the number of threads used by OpenCV for parallel regions."]
	getVersionString: ["Returns library version string."]
	getBuildInformation: ["Returns full configuration time cmake output."]
	useOptimized: ["Returns the status of optimized code usage."]
	setUseOptimized: ["Enables or disables the optimized code." onoff [logic!]]

	;-----------------------------------------------------------------------------------------------
	;- Other                                                                                        
	;-----------------------------------------------------------------------------------------------
	qrcode-encode: [
		"Encode string to an image."
		text [any-string!]
		/version "Symbol version of QR Code range"
		v [integer!] "1 - 40"
		/mode    "Encoding mode"
		m [integer!] 
		/correction "Approximate error correction level (low, medium, quartile, high)"
		level [integer!] "0 - 3 (default is 0)"
	]
]

ext-values: {
; imread flags..
IMREAD_UNCHANGED: -1
IMREAD_GRAYSCALE: 0
IMREAD_COLOR: 1
IMREAD_ANYDEPTH: 2
IMREAD_ANYCOLOR: 4
IMREAD_LOAD_GDAL: 8
IMREAD_REDUCED_GRAYSCALE_2: 16
IMREAD_REDUCED_COLOR_2: 17
IMREAD_REDUCED_GRAYSCALE_4: 32
IMREAD_REDUCED_COLOR_4: 33
IMREAD_REDUCED_GRAYSCALE_8: 64
IMREAD_REDUCED_COLOR_8: 65
IMREAD_IGNORE_ORIENTATION: 128

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

; DecompTypes:
DECOMP_LU: 0
DECOMP_SVD: 1
DECOMP_EIG: 2
DECOMP_CHOLESKY: 3
DECOMP_QR: 4
DECOMP_NORMAL: 16

; MorphShapes:
MORPH_RECT: 0
MORPH_CROSS: 1
MORPH_ELLIPSE: 2

; MorphTypes:
MORPH_ERODE: 0
MORPH_DILATE: 1
MORPH_OPEN: 2
MORPH_CLOSE: 3
MORPH_GRADIENT: 4
MORPH_TOPHAT: 5
MORPH_BLACKHAT: 6
MORPH_HITMISS: 7

; NormTypes:
NORM_INF: 1
NORM_L1: 2
NORM_L2: 4
NORM_L2SQR: 5
NORM_HAMMING: 6
NORM_HAMMING2: 7
NORM_TYPE_MASK: 7
NORM_RELATIVE: 8
NORM_MINMAX: 32

; ColormapTypes:
COLORMAP_AUTUMN: 0
COLORMAP_BONE: 1
COLORMAP_JET: 2
COLORMAP_WINTER: 3
COLORMAP_RAINBOW: 4
COLORMAP_OCEAN: 5
COLORMAP_SUMMER: 6
COLORMAP_SPRING: 7
COLORMAP_COOL: 8
COLORMAP_HSV: 9
COLORMAP_PINK: 10
COLORMAP_HOT: 11
COLORMAP_PARULA: 12
COLORMAP_MAGMA: 13
COLORMAP_INFERNO: 14
COLORMAP_PLASMA: 15
COLORMAP_VIRIDIS: 16
COLORMAP_CIVIDIS: 17
COLORMAP_TWILIGHT: 18
COLORMAP_TWILIGHT_SHIFTED: 19
COLORMAP_TURBO: 20
COLORMAP_DEEPGREEN: 21
}

handles: make map! [
	cvMat: [
		"OpenCV Matrix"
		size           pair!     none  "2D size of a matrix element"
		type           word!     none  "Type of a matrix element"
		depth          word!     none  "Depth of a matrix element"
		channels       integer!  none  "Number of channels"
		binary         binary!   none  "Binary"
		image          image!    none  "Image"
		vector         vector!   none  "Vector"
		total          integer!  none  "Number of array elements (a number of pixels if the array represents an image)"
		is-submatrix   logic!    none  "Returns true if the matrix is a submatrix of another matrix"
		width          integer!  none  "Width of the Matrix"
		height         integer!  none  "Height of the Matrix"
	]
	cvVideoCapture: [
		"Class for video capturing from video files, image sequences or cameras"
		pos-ms         decimal!  none  "Current position of the video file in milliseconds"
		pos-frame      decimal!  none  "0-based index of the frame to be decoded/captured next"
		pos-ratio      decimal!  none  "Relative position of the video file: 0=start of the film, 1=end of the film"
		width          integer!  none  "Width of the frames in the video stream"
		height         integer!  none  "Height of the frames in the video stream"
		fps            decimal!  none  "Frame rate"
		fourcc         decimal!  none  "4-character code of codec"
		frames         integer!  none  "Number of frames in the video file"
		format         word!     none  "Format of the Mat objects"
		;TODO: add the rest...
	]
]

arg-words:   copy []
handles-doc: copy {}

foreach [name spec] handles [
	append handles-doc ajoin [
		LF LF "#### __" uppercase form name "__ - " spec/1 LF
		LF "```rebol"
		LF ";Refinement       Gets                Sets                          Description"
	]
	foreach [name gets sets desc] next spec [
		append handles-doc rejoin [
			LF
			#"/" pad name 17
			pad mold gets 20
			pad mold sets 30
			#"^"" desc #"^""
		]
		append arg-words name
	]
	append handles-doc "^/```"
]
;print handles-doc
arg-words: unique arg-words
probe arg-words


;-------------------------------------- ----------------------------------------
reb-code: {REBOL [Title: {Rebol OpenCV Extension} Type: module Exports: [] Require: 3.14.0]}
enu-commands:  "" ;; command name enumerations
cmd-declares:  "" ;; command function declarations
cmd-dispatch:  "" ;; command functionm dispatcher

cv-arg-words: "enum cv_arg_words {W_ARG_0"
cv-type-words: "enum cv_type_words {W_TYPE_0"

;- generate C and Rebol code from the command specifications -------------------
foreach [name spec] commands [
	append reb-code ajoin [lf name ": command "]
	new-line/all spec false
	append/only reb-code mold spec

	name: form name
	replace/all name #"-" #"_"
	replace/all name #"?" #"q"
	
	append enu-commands ajoin ["^/^-CMD_OPENCV_" uppercase copy name #","]

	append cmd-declares ajoin ["^/int cmd_" name "(RXIFRM *frm, void *ctx);"]
	append cmd-dispatch ajoin ["^-cmd_" name ",^/"]
]

;- additional Rebol initialization code ----------------------------------------

foreach word arg-words [
	word: uppercase form word
	replace/all word #"-" #"_"
	replace/all word #"?" #"Q"
	append cv-arg-words ajoin [",^/^-W_ARG_" word]
]

foreach word type-words [
	word: uppercase form word
	replace/all word #"-" #"_"
	append cv-type-words ajoin [",^/^-W_TYPE_" word]
]

append cv-arg-words "^/};"
append cv-type-words "^/};"
append reb-code ajoin [{
init-words } mold/flat arg-words mold/flat type-words {
protect/hide 'init-words
} ext-values
]

;append reb-code {}

;print reb-code

;- convert Rebol code to C-string ----------------------------------------------
init-code: copy ""
foreach line split reb-code lf [
	replace/all line #"^"" {\"}
	append init-code ajoin [{\^/^-"} line {\n"}] 
]


;-- C file templates -----------------------------------------------------------
header: {//
// auto-generated file, do not modify!
//

#include "common.h"

#define SERIES_TEXT(s)   ((char*)SERIES_DATA(s))

#define MIN_REBOL_VER 3
#define MIN_REBOL_REV 14
#define MIN_REBOL_UPD 0
#define VERSION(a, b, c) (a << 16) + (b << 8) + c
#define MIN_REBOL_VERSION VERSION(MIN_REBOL_VER, MIN_REBOL_REV, MIN_REBOL_UPD)


extern u32* arg_words;
extern u32* type_words;

enum ext_commands {$enu-commands
};

$cmd-declares

$cv-arg-words
$cv-type-words

typedef int (*MyCommandPointer)(RXIFRM *frm, void *ctx);

#define OPENCV_EXT_INIT_CODE $init-code

#ifdef  USE_TRACES
#include <stdio.h>
#define debug_print(fmt, ...) do { printf(fmt, __VA_ARGS__); } while (0)
#define trace(str) puts(str)
#else
#define debug_print(fmt, ...)
#define trace(str) 
#endif

}
;;------------------------------------------------------------------------------
ctable: {//
// auto-generated file, do not modify!
//
#include "opencv-rebol-extension.h"
MyCommandPointer Command[] = {
$cmd-dispatch};
}

;- output generated files ------------------------------------------------------
write %opencv-rebol-extension.h reword :header self
write %opencv-commands-table.c  reword :ctable self




;; README documentation...
doc: clear ""
hdr: clear ""
arg: clear ""
cmd: desc: a: t: s: readme: r: none
parse commands [
	any [
		quote init-words: skip
		|
		set cmd: set-word! into [
			(clear hdr clear arg r: none)
			(append hdr ajoin [LF LF "#### `" cmd "`"])
			set desc: opt string!
			any [
				set a word!
				set t opt block!
				set s opt string!
				(
					unless r [append hdr ajoin [" `:" a "`"]]
					append arg ajoin [LF "* `" a "`"] 
					if t [append arg ajoin [" `" mold t "`"]]
					if s [append arg ajoin [" " s]]
				)
				|
				set r refinement!
				set s opt string!
				(
					append arg ajoin [LF "* `/" r "`"] 
					if s [append arg ajoin [" " s]]
				)
			]
			(
				append doc hdr
				append doc LF
				append doc any [desc ""]
				append doc arg
			)
		]
	]
]
;try/with [
	readme: "## Extension commands:"
	append readme ajoin [
		LF doc
		LF LF
		LF "## Used handles and its getters / setters" 
		handles-doc
		LF LF
		LF "## Other extension values:"
		LF "```rebol"
		trim/tail ext-values
		LF "```"
		LF
	]
	probe write %../Commands.md head readme
;] :print

