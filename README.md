[![rebol-opencv](https://github.com/user-attachments/assets/6c630cbd-9ef8-41b3-9693-e2b5054712c0)](https://github.com/Oldes/Rebol-OpenCV)

[![Rebol-OpenCV CI](https://github.com/Oldes/Rebol-OpenCV/actions/workflows/main.yml/badge.svg)](https://github.com/Oldes/Rebol-OpenCV/actions/workflows/main.yml)
[![Gitter](https://badges.gitter.im/rebol3/community.svg)](https://app.gitter.im/#/room/#Rebol3:gitter.im)
[![Zulip](https://img.shields.io/badge/zulip-join_chat-brightgreen.svg)](https://rebol.zulipchat.com/)

# Rebol/OpenCV

Initial implementation of the OpenCV extension for [Rebol3](https://github.com/Oldes/Rebol3) (3.14.0 or newer).

So far it is considered just like a proof of concept and tested only on macOS with [OpenCV installed using homebrew](https://formulae.brew.sh/formula/opencv)!


When building on macOS, the setup expects, that OpenCV includes and libs are accessible. So it is recommanded to use something like:
```
ln -s /opt/homebrew/Cellar/opencv/4.6.0/include/opencv4/opencv2 /usr/local/include/opencv2
ln -s /opt/homebrew/Cellar/opencv/4.6.0/lib /usr/local/lib/opencv
```
To import the extension from Rebol on macOS, the Rebol must be signed with entitlemens containing `com.apple.security.cs.disable-library-validation` as `true`. 

Feature requests are welcome.

For list of currently supported extension commands and other values, read [Commands.md](Commands.md).

## Usage

All folowing examples expect, that OpenCV extension was imported using one of these methods:
1. Using direct path to the file: `cv: import %path/to/opencv.rebx`
2. Using extension in the default location: `cv: import 'opencv`

### Making a blank matrix and displaying it.

Matrices are one of the main datatypes used by OpenCV extension.

```rebol
mat: cv/Matrix 250x140 ;; creates an empty black image handle
cv/imshow mat          ;; open a window with default name "Image" displaying the image
cv/waitKey 0           ;; and wait for any key
```

### Closing a window

Window is closed using `destroyWindow "Window name"`.
It is possible to close all windows using `destroyAllWindows`

```rebol
cv/destroyWindow "Image"       ;; because "Image" is the default window's name.
```

### Loading a matrix from file

Instead of writting full paths to `cv` commands, like: `cv/imshow`, it is possible
to bind the code to the `cv` context using `with cv [...]`

```rebol
with cv [
    filename: %image/mask.png
    mat: imread filename
    imshow/name mat filename   ;; displaying the image in the window with file name's title
    waitKey 0
]
```

### Moving/resizing an opened window

Having the window from the previous example still open, it is possible to move it using `moveWindow`.

```rebol
cv/moveWindow filename 300x50
cv/waitKey 5000                ;; now there is only 5s wait time
```

Windows created using `namedWindow` may be resized using `resizeWindow`

```rebol
cv/namedWindow win: "Resized"  ;; creating a window with title/name "Resized"
cv/imshow/name mat win         ;; displaying an image in it
cv/resizeWindow win 500x280    ;; resized
cv/waitKey 5000
cv/destroyAllWindows           ;; closing both windows
```

### Getting matrix properties

Using still the matrix from above, resized using `resize` command so the output is not too long..

```rebol
with cv [
    mat: resize mat 10%
    probe get-property mat MAT_SIZE     ;; image size
    probe get-property mat MAT_TYPE     ;; CV type id
    probe get-property mat MAT_CHANNELS ;; number of channels (3 = RGB)
    probe get-property mat MAT_DEPTH    ;; CV depth id (0 = CV_8U)
    probe get-property mat MAT_BINARY   ;; raw binary data
    probe get-property mat MAT_VECTOR   ;; Rebol vector value
    probe get-property mat MAT_IMAGE    ;; Rebol image value
]
```
Above is now also possible with direct getters, like:
```rebol
with cv [
    mat: resize mat 10%
    print ["Image size:"         mat/size     ]
    print ["CV type:"            mat/type     ]
    print ["Channels:"           mat/channels ]
    print ["CV depth:"           mat/depth    ]
    print ["Rebol binary data:"  mat/binary   ]
    print ["Rebol vector value:" mat/vector   ]
    print ["Rebol image value:"  mat/image    ]
]
```

### Manually releasing matrices

Normally matrices are automatically released by Rebol's GC, but it is also possible to free them manually

```rebol
cv/free mat          ;; manually released matrix
```

It should be noted, that such a matrix is not usable anymore! This will fail:

```rebol
probe cv/imshow mat  ;; will return false!
```

### Color space conversion

```rebol
with cv [
    img: imread %image/mask.png
    hls:  cvtColor img none COLOR_BGR2HLS
    gray: cvtColor img none COLOR_BGR2GRAY
    namedWindow win1: "Original"
    namedWindow win2: "HLS"
    namedWindow win3: "Grayscale"
    imshow/name img  win1
    imshow/name hls  win2
    imshow/name gray win3
    moveWindow win1 0x0
    moveWindow win2 250x0
    moveWindow win3 500x0
    waitKey 0
]
```

### Image threshold

Having the grayscale version from above, we can applie a fixed-level threshold.

```rebol
with cv [
    namedWindow win4: "THRESH_BINARY"
    namedWindow win5: "THRESH_TRUNC"
    namedWindow win6: "THRESH_TOZERO"
    moveWindow  win4   0x150
    moveWindow  win5 250x150
    moveWindow  win6 500x150
    thresh1: threshold gray none  0  255 THRESH_BINARY
    thresh2: threshold gray none 100 255 THRESH_TRUNC
    thresh3: threshold gray none 200 255 THRESH_TOZERO
    imshow/name thresh1 win4
    imshow/name thresh2 win5
    imshow/name thresh3 win6
    waitKey 0
    destroyAllWindows
]
```

### Using computed binary threshold as an opacity channel

```rebol
image: cv/get-property img     cv/MAT_IMAGE   ;; get Rebol image
alpha: cv/get-property thresh1 cv/MAT_BINARY  ;; get Rebol binary with alpha values
image/alpha: alpha                            ;; replace image alpha with the new value
save %tmp/masked.png image                    ;; using Rebol's PNG codec to save the new image
```

### Detecting edges in the image and their dilatation

```rebol
with cv [
    src: imread "image/mask.png"
    namedWindow win1: "Canny"
    namedWindow win2: "Canny dilated"
    moveWindow win1 0x0
    moveWindow win2 250x0

    kernel: getStructuringElement MORPH_CROSS 3 -1 ;; preparing the kernel for dilatation 

    dst1: Canny src none 50 200                    ;; edge detecting
    dst2: dilate dst1 none kernel -1x-1 1          ;; dilating the edges
    imshow/name dst1 win1
    imshow/name dst2 win2
    waitKey 0
    destroyAllWindows
]
```

### Detecting edges using Gabor Filter

```rebol
with cv [
    lena: imread "image/lena.jpeg"
    size: get-property :lena MAT_SIZE
    gray: cvtColor :lena none COLOR_BGR2GRAY

    num-filters: 16
    sigma: 1.5    ;; The bandwidth or sigma controls the overall size of the Gabor envelope.
    theta: 0      ;; The theta controls the orientation of the Gabor function.
    lambd: 60     ;; The wavelength governs the width of the strips of the Gabor function.
    gamma: 0.75   ;; The aspect ratio or gamma controls the height of the Gabor function.
    psi: PI * 0.5 ;; The phase offset.

    theta-step: pi / num-filters
    kernels: make block! num-filters

    loop num-filters [
        ;- Generate new Gabor kernel
        kern: getGaborKernel 35x35 :sigma :theta :lambd :gamma :psi CV_64F
        normalize :kern :kern 1 0 NORM_L1 CV_64F ;- Brightness normalization
        ;- Store it for later use
        append kernels kern
        ;- Update the orientation for the next kernel
        theta: theta + theta-step
    ]

    temp: Matrix [:size CV_8UC1] ;; holds temporary filtered image
    dest: Matrix [:size CV_8UC1] ;; detected edges

    ;- Apply each Gabor kernel to filter the grayscale source
    foreach kern kernels [
        temp: filter2D :gray :temp -1 :kern -1x-1 0
        normalize :temp :temp 0 255 NORM_MINMAX
        ;- Compare our filter and cumulative image, taking the higher value
        max :dest :temp :dest
        ;- Display current detection state
        imshow/name :dest "Edges"
        waitKey 10
    ]
    waitKey 0
]
```

### Blurring images

```rebol
with cv [
    src: imread "image/mask.png"
    blured1: Matrix src
    blured2: Matrix src
    blured3: Matrix src
    namedWindow win1: "blur"
    namedWindow win2: "gaussianBlur"
    namedWindow win3: "medianBlur"
    moveWindow win1 0x0
    moveWindow win2 250x0
    moveWindow win3 500x0

    size: 1
    add-blur?: true
    forever [
        either add-blur? [
            size: size + 2
            add-blur?: size < 100
        ][
            size: size - 2
            add-blur?: size <= 1
        ]
        blur         :src :blured1 :size
        gaussianBlur :src :blured2 :size 0 0
        medianBlur   :src :blured3 :size
        imshow/name blured1 win1
        imshow/name blured2 win2
        imshow/name blured3 win3
        if 0 <= waitkey 50 [break]
    ]
    destroyAllWindows
]
```

### Applying a color map

```rebol
with cv [
    src: imread "image/lena.jpeg"
    result: Matrix :src
    for i 0 21 1 [
        applyColorMap :src :result :i 
        imshow :result
        setWindowTitle "Image" join "Colormap: " i
        if 0 < waitKey 1000 [break]
    ]
    destroyAllWindows
]
```

### Applying Sepia filter to an image

```rebol
with cv [
    src: imread "image/taj.jpg"
    kernel: #(float! [
        0.272 0.534 0.131
        0.349 0.686 0.168
        0.393 0.769 0.189
    ])
    sepia-filter: Matrix [3x3 :kernel]
    transform src src sepia-filter
    imshow src
    waitKey 0
    destroyAllWindows
]
```

### Using shared buffer

When constructing matrices from a Rebol's binary or vector value, the buffer may be shared.
```rebol
with cv [
    ;; allocate vector for a grayscale image of size 320x200
    data: make vector! [uint8! 64000]
    ;; make an OpenCV metrix using the shared data
    img: Matrix [320x200 :data]
    ;; do some animation...
    forever [
        ;; manipulate the matrix data using Rebol code with the direct access
        forall data [data/1: random 255]
        ;; display the modified image using OpenCV
        imshow img
        if 0 < waitKey 10 [break]
    ]
]
```

### Saving video from the camera

```rebol
with cv [
    ;; initialize video input from a file...
    ;cam: VideoCapture %test.mp4
    ;; or from a camera device...
    cam: VideoCapture 0
    unless cam [print "Failed to initialize VideoCapture" quit]

    ;; resolve input frame size...
    size: as-pair get-property cam CAP_PROP_FRAME_WIDTH
                  get-property cam CAP_PROP_FRAME_HEIGHT

    print ["Input frame size:" size]

    ;set-property cam CAP_PROP_POS_FRAMES 2000.0 ;; can be used to set position in the video (file input)

    if frame: read :cam [
        ;; initialize VideoWriter (when 0 is used as codec parameter, than the output will be MJPG)
        out: VideoWriter %tmp/out.avi 0 24 size
        unless out [print "Failed to initialize VideoWriter!" quit]

        ;; grab 50 frames maximum...
        loop 50 [ 
            read/into :cam :frame    ;; reusing existing frame
            write out :frame         ;; append the frame to the output video
            imshow :frame            ;; and also show it in the window
            if pollKey = 27 [break]  ;; exit on ESC key
            wait 0.01                ;; let Rebol breath as well
        ]
        destroyAllWindows
    ]
    free :out  ;; release VideoWriter
    free :cam  ;; release VideoCapture
]
```

VideoWriter expects integer for its codec input. It is possible to use function like this for the conversion:
```rebol
fourcc: func[
    "Converts fourcc codec integer to string and back"
    codec [any-string! binary! number!]][
    either number? codec [
        to string! reverse trim to binary! to integer! codec
    ][
        to integer! reverse to binary! codec
    ]
]
fourcc "avc1"    ;== 828601953
fourcc 828601953 ;== "avc1"
```

### Computing absolute difference between 2 video frames

`absdiff` is useful when tracking a moving objects (or to produce nice psychedelic video effects:).
```rebol
with cv [
    cam: VideoCapture 0
    unless cam [print "Failed to initialize VideoCapture" quit]
    if all [
        frame1: read :cam       ;; try to get the first frame
        frame2: Matrix :frame1  ;; make matrices with the same size and type
        result: Matrix :frame1  ;; for reuse later
    ][
        print "Press any key to quit."
        forever [ 
            read/into :cam :frame1          ;; get first frame
            if 0 < waitKey 50 [break]       ;; wait some time
            read/into :cam :frame2          ;; get second frame
            absdiff :frame1 :frame2 :result ;; compute absolute difference
            imshow :result                  ;; display it
            if 0 < waitKey 50 [break]       ;; for some time
        ]
        destroyAllWindows
    ]
    free :cam  ;; release VideoCapture
]
```

### Encode and decode QR codes

```rebol
with cv [
    mat: qrcode-encode "Hello Rebol, hello OpenCV!"
    mat: resize/with :mat 600% INTER_NEAREST
    ;; display the result...
    imshow :mat
    waitKey 0
    destroyAllWindows
    ;; save as PNG image...
    imwrite %tmp/test-qrcode.png :mat
    ;; decode QRCode from file:
    probe qrcode-decode %tmp/test-qrcode.png
]
```

### Using mouse callback

```rebol
;; Having mouse callback handler in a given context
ctx: context [
    color: 0.0.0
    on-mouse-trace: func [
        type  [integer!]
        x     [integer!]
        y     [integer!]
        flags [integer!]
    ][
        print [
            pickz [
                MOUSEMOVE
                LBUTTONDOWN
                RBUTTONDOWN
                MBUTTONDOWN
                LBUTTONUP
                RBUTTONUP
                MBUTTONUP
                LBUTTONDBLCLK
                RBUTTONDBLCLK
                MBUTTONDBLCLK
                MOUSEWHEEL
                MOUSEHWHEEL
            ] :type
            "position:" as-pair x y
            "flags:" flags
        ]
    ]
    on-mouse-draw: func [
        type  [integer!]
        x     [integer!]
        y     [integer!]
        flags [integer!]
    ][
        if type == cv/EVENT_RBUTTONDOWN [
            ;; Modify draw color on right button click
            color: random 200.200.200
            print ["New draw color:" color]
        ]
        if 1 = (flags & 1) [
            ;; Modify pixel if right mouse button is down
            img/(mcb/pos): color
        ]
        ;; Update the window with modified image
        cv/imshow/name img win
    ]
]

with cv [
    ;; Display white image...
    win: "Test mouse window"
    img: make image! 320x240
    imshow/name img win
    ;; Set first callback which prints inputs in console
    mcb: setMouseCallback win ctx 'on-mouse-trace
    waitKey 0
    ;; Replace the mouse callback with a very simple draw handler
    mcb: setMouseCallback win ctx 'on-mouse-draw
    waitKey 0
    ;; Last event data are stored in the cvMouseCallback handle
    print ["Last mouse position:" mcb/x mcb/y]
    destroyAllWindows
]
```


* * * *
This file was generated using [examples.r3](examples.r3) script.

## Extension commands:


#### `test`
Simple OpenCV test

#### `Matrix` `:spec`
Initialize new cvMat class
* `spec` `[pair! handle! image! block!]`

#### `VideoCapture` `:src`
Initialize new VideoCapture class
* `src` `[integer! file! string!]`

#### `VideoWriter` `:src` `:codec` `:fps` `:size`
Initialize new VideoWriter class
* `src` `[integer! file! string!]`
* `codec` `[integer!]` 4-character code of codec used to compress the frames (mp4ra.org/#/codecs)
* `fps` `[number!]` Framerate of the created video stream
* `size` `[pair!]` Size of the video frames

#### `free` `:class`
Release OpenCV handle
* `class` `[handle!]` Mat, VideoCapture or VideoWriter

#### `get-property` `:obj` `:property`
Returns a property value
* `obj` `[handle!]` VideoCapture handle
* `property` `[integer!]`

#### `set-property` `:obj` `:property` `:value`

* `obj` `[handle!]` VideoCapture handle
* `property` `[integer!]`
* `value` `[number!]`

#### `read` `:src`
Grabs, decodes and returns the next video frame
* `src` `[handle!]` VideoCapture
* `/into` Optional existing Mat handle
* `dst` `[handle!]` Mat

#### `write` `:dst` `:frame`
Writes the next video frame
* `dst` `[handle!]` VideoWriter
* `frame` `[image! handle!]`

#### `imread` `:src`

* `src` `[file! string!]` Loads an image from a file.
* `/image` As Rebol image instead of default cvMat
* `/with` Flag that can take values IMREAD_*
* `flags` `[integer!]` Default is IMREAD_UNCHANGED

#### `imreadmulti` `:src`

* `src` `[file! string!]` Loads a multi-page image from a file.
* `/image` As Rebol images instead of default cvMat
* `/with` Flag that can take values IMREAD_*
* `flags` `[integer!]` Default is IMREAD_UNCHANGED

#### `imwrite` `:name` `:image`
Saves an image to a specified file.
* `name` `[any-string!]`
* `image` `[image! handle!]` Image or cvMat handle
* `/with` Format-specific parameters encoded as pairs
* `params` `[block!]` integer pairs (words are resolved)

#### `resize` `:image` `:size`
Resizes an image.
* `image` `[image! handle!]` Image or cvMat handle
* `size` `[pair! percent!]`
* `/into`
* `target` `[handle!]` cvMat
* `/with`
* `interpolation` `[integer!]`

#### `bilateralFilter` `:src` `:dst` `:diameter` `:sigmaColor` `:sigmaSpace`
Applies the bilateral filter to an image.
* `src` `[handle!]` Source 8-bit or floating-point, 1-channel or 3-channel image.
* `dst` `[handle! none!]` Destination image of the same size and type as src.
* `diameter` `[integer!]` Diameter of each pixel neighborhood that is used during filtering. If it is non-positive, it is computed from sigmaSpace.
* `sigmaColor` `[number!]`
* `sigmaSpace` `[number!]`
* `/border` border mode used to extrapolate pixels outside of the image
* `type` `[integer!]` one of: [0 1 2 4 5 16]

#### `blur` `:src` `:dst` `:size`
Blurs an image using the normalized box filter.
* `src` `[handle!]` cvMat handle
* `dst` `[handle! none!]`
* `size` `[pair! integer!]` blurring kernel size
* `/border` border mode used to extrapolate pixels outside of the image
* `type` `[integer!]` one of: [0 1 2 4 5 16]

#### `dilate` `:src` `:dst` `:kernel` `:anchor` `:iterations`
Dilates an image by using a specific structuring element.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `kernel` `[handle!]`
* `anchor` `[pair! integer!]` position of the anchor within the element
* `iterations` `[integer!]` number of times dilation is applied

#### `erode` `:src` `:dst` `:kernel` `:anchor` `:iterations`
Erodes an image by using a specific structuring element.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `kernel` `[handle!]`
* `anchor` `[pair! integer!]` position of the anchor within the element
* `iterations` `[integer!]` number of times dilation is applied

#### `filter2D` `:src` `:dst` `:ddepth` `:kernel` `:anchor` `:delta`
Convolves an image with the kernel.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `ddepth` `[integer!]` desired depth of the destination image
* `kernel` `[handle!]` convolution kernel (or rather a correlation kernel), a single-channel floating point matrix
* `anchor` `[pair! integer!]` position of the anchor within the element
* `delta` `[number!]` value added to the filtered pixels before storing them in dst
* `/border` border mode used to extrapolate pixels outside of the image
* `type` `[integer!]` one of: [0 1 2 4 5 16]

#### `GaussianBlur` `:src` `:dst` `:size` `:sigmaX` `:sigmaY`
Blurs an image using a Gaussian filter.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `size` `[pair! integer!]` blurring kernel size
* `sigmaX` `[number!]`
* `sigmaY` `[number!]`
* `/border` border mode used to extrapolate pixels outside of the image
* `type` `[integer!]` one of: [0 1 2 4 5 16]

#### `getGaborKernel` `:ksize` `:sigma` `:theta` `:lambd` `:gamma` `:psi` `:ktype`
Returns Gabor filter coefficients.
* `ksize` `[pair!]` Size of the filter returned
* `sigma` `[number!]` Standard deviation of the gaussian envelope
* `theta` `[number!]` Orientation of the normal to the parallel stripes of a Gabor function.
* `lambd` `[number!]` Wavelength of the sinusoidal factor
* `gamma` `[number!]` Spatial aspect ratio
* `psi` `[number!]` Phase offset
* `ktype` `[integer!]` Type of filter coefficients. It can be CV_32F or CV_64F

#### `getStructuringElement` `:shape` `:ksize` `:anchor`
Returns a structuring element of the specified size and shape for morphological operations.
* `shape` `[integer!]` Element shape that could be one of MorphShapes (rect = 0, cross = 1, ellipse = 2)
* `ksize` `[pair! integer!]` Size of the structuring element
* `anchor` `[pair! integer!]` Anchor position within the element

#### `Laplacian` `:src` `:dst` `:ddepth` `:ksize` `:scale` `:delta`
Calculates the Laplacian of an image.
* `src` `[handle!]` Source image
* `dst` `[handle! none!]` Destination image of the same size and the same number of channels as src
* `ddepth` `[number!]` Desired depth of the destination image
* `ksize` `[number!]` Aperture size used to compute the second-derivative filters. The size must be positive and odd.
* `scale` `[number!]` Scale factor for the computed Laplacian values.
* `delta` `[number!]` Optional delta value that is added to the results prior to storing them in dst.

#### `medianBlur` `:src` `:dst` `:size`
Blurs an image using the median filter.
* `src` `[handle!]` input 1-, 3-, or 4-channel image; when ksize is 3 or 5, the image depth should be CV_8U, CV_16U, or CV_32F, for larger aperture sizes, it can only be CV_8U
* `dst` `[handle! none!]` destination array of the same size and type as src
* `size` `[number!]` aperture linear size; it must be odd and greater than 1, for example: 3, 5, 7...

#### `cvtColor` `:src` `:dst` `:code`
Converts an image from one color space to another.
* `src` `[handle!]` source cvMat handle
* `dst` `[handle! none!]` destination cvMat
* `code` `[integer!]`

#### `applyColorMap` `:src` `:dst` `:colormap`
Applies a GNU Octave/MATLAB equivalent colormap on a given image.
* `src` `[handle!]` source cvMat handle
* `dst` `[handle! none!]` destination cvMat
* `colormap` `[integer!]`

#### `threshold` `:src` `:dst` `:thresh` `:maxval` `:type`
Applies a fixed-level threshold to each array element.
* `src` `[handle!]`
* `dst` `[handle! none!]`
* `thresh` `[number!]`
* `maxval` `[number!]`
* `type` `[integer!]`

#### `absdiff` `:src1` `:src2` `:dst`
Calculates the per-element absolute difference between two arrays
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat

#### `add` `:src1` `:src2` `:dst`
Calculates the per-element sum of two arrays.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/mask`
* `m` `[handle!]` cvMat

#### `addWeighted` `:src1` `:alpha` `:src2` `:beta` `:gamma` `:dst`
Calculates the weighted sum of two arrays.
* `src1` `[handle!]` cvMat
* `alpha` `[number!]` weight of the first array elements.
* `src2` `[handle!]` cvMat
* `beta` `[number!]` weight of the second array elements.
* `gamma` `[number!]` scalar added to each sum.
* `dst` `[handle! none!]` cvMat

#### `bitwise-and` `:src1` `:src2` `:dst`
Computes bitwise conjunction of the two arrays (dst = src1 & src2)
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/mask`
* `m` `[handle!]` cvMat

#### `bitwise-not` `:src` `:dst`
Inverts every bit of an array.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/mask`
* `m` `[handle!]` cvMat

#### `bitwise-or` `:src1` `:src2` `:dst`
Calculates the per-element bit-wise disjunction of two arrays or an array and a scalar.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/mask`
* `m` `[handle!]` cvMat

#### `bitwise-xor` `:src1` `:src2` `:dst`
Calculates the per-element bit-wise "exclusive or" operation on two arrays or an array and a scalar.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/mask`
* `m` `[handle!]` cvMat

#### `convertScaleAbs` `:src` `:dst` `:alpha` `:beta`
Scales, calculates absolute values, and converts the result to 8-bit.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `alpha` `[number!]` default = 1
* `beta` `[number!]`

#### `divide` `:src1` `:src2` `:dst`
Calculates the per-element division of two arrays.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/scale` scalar factor
* `s` `[number!]` default = 1

#### `flip` `:src` `:dst` `:flipCode`
Flips a 2D array around vertical, horizontal, or both axes.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `flipCode` `[integer!]` a flag to specify how to flip the array; 0 means flipping around the x-axis and positive value (for example, 1) means flipping around y-axis. Negative value (for example, -1) means flipping around both axes

#### `invert` `:src` `:dst` `:flags`
Finds the inverse or pseudo-inverse of a matrix.
* `src` `[handle!]` Input floating-point M x N matrix.
* `dst` `[handle! none!]` Output matrix of N x M size and the same type as src.
* `flags` `[integer!]` Inversion method; One of DecompTypes (DECOMP_*)

#### `max` `:src1` `:src2` `:dst`
Calculates per-element maximum of two arrays.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat

#### `mean` `:src`
Calculates an average (mean) of array elements.
* `src` `[handle!]` cvMat

#### `min` `:src1` `:src2` `:dst`
Calculates per-element minimum of two arrays.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat

#### `minMaxLoc` `:src`
Finds the global minimum and maximum in an array.
* `src` `[handle!]` input single-channel array (cvMat)

#### `multiply` `:src1` `:src2` `:dst`
Calculates the per-element scaled product of two arrays.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/scale` scalar factor
* `s` `[number!]` default = 1

#### `normalize` `:src` `:dst` `:alpha` `:beta` `:norm_type`
Calculates the per-element scaled product of two arrays.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `alpha` `[number!]` norm value to normalize to or the lower range boundary in case of the range normalization
* `beta` `[number!]` upper range boundary in case of the range normalization; it is not used for the norm normalization
* `norm_type` `[integer!]` normalization type
* `/mask`
* `m` `[handle!]` optional operation mask

#### `subtract` `:src1` `:src2` `:dst`
Calculates the per-element difference between two arrays.
* `src1` `[handle!]` cvMat
* `src2` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `/mask`
* `m` `[handle!]` cvMat

#### `transform` `:src` `:dst` `:m`
Performs the matrix transformation of every array element.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `m` `[handle!]` transformation 2x2 or 2x3 floating-point matrix.

#### `convertTo` `:src` `:dst` `:type` `:alpha` `:beta`
Converts an array to another data type with optional scaling.
* `src` `[handle!]` cvMat
* `dst` `[handle! none!]` cvMat
* `type` `[integer! word!]` desired output matrix type or, rather, the depth since the number of channels are the same as the input has; if rtype is negative, the output matrix will have the same type as the input
* `alpha` `[number!]` scale factor
* `beta` `[number!]` delta added to the scaled values

#### `Canny` `:src` `:dst` `:threshold1` `:threshold2`
Finds edges in an image using the Canny algorithm
* `src` `[handle!]` 8-bit input image
* `dst` `[handle! none!]` output edge map; single channels 8-bit image, which has the same size as image
* `threshold1` `[number!]` first threshold for the hysteresis procedure
* `threshold2` `[number!]` second threshold for the hysteresis procedure

#### `startWindowThread`


#### `imshow` `:src`
Displays an image in the specified window.
* `src` `[image! handle!]` Image or cvMat handle
* `/name` Optional window name
* `window` `[any-string!]`

#### `pollKey`
Polls for a pressed key.

#### `waitKey` `:delay`
Waits for a pressed key.
* `delay` `[integer!]` In millisecons; infinitely when <=0

#### `namedWindow` `:name`
Creates a window.
* `name` `[any-string!]`

#### `resizeWindow` `:window` `:size`
Resizes the window to the specified size.
* `window` `[any-string!]`
* `size` `[pair!]`

#### `moveWindow` `:window` `:pos`
Moves the window to the specified position.
* `window` `[any-string!]`
* `pos` `[pair!]`

#### `getWindowImageSize` `:window`
Provides size of image in the window (not counting tarckbars and other controls).
* `window` `[any-string!]`

#### `getWindowImagePos` `:window`
Provides position (left-bottom) of image in the window.
* `window` `[any-string!]`

#### `getWindowProperty` `:name` `:property`
Provides parameters of a window.
* `name` `[any-string!]`
* `property` `[integer!]`

#### `setWindowProperty` `:name` `:property` `:value`
Changes parameters of a window dynamically.
* `name` `[any-string!]`
* `property` `[integer!]`
* `value` `[number!]`

#### `setWindowTitle` `:name` `:title`
Updates window title.
* `name` `[any-string!]` Name of the window
* `title` `[any-string!]` New title

#### `destroyAllWindows`
Destroys all of the HighGUI windows.

#### `destroyWindow` `:window`
Destroys the specified window.
* `window` `[any-string!]`

#### `createTrackbar` `:trackbarname` `:window` `:count`

* `trackbarname` `[any-string!]`
* `window` `[any-string!]`
* `count` `[integer!]` Maximal position of the slider. The minimal position is always 0.
* `/with`
* `ctx` `[object!]` Callback's context
* `func` `[word!]` Callback function name

#### `setTrackbarMax` `:trackbar` `:value`
Sets the trackbar maximum position.
* `trackbar` `[handle!]` cvTrackbar
* `value` `[integer!]` New maximum position.

#### `setTrackbarMin` `:trackbar` `:value`
Sets the trackbar minimum position.
* `trackbar` `[handle!]` cvTrackbar
* `value` `[integer!]` New minimum position.

#### `setTrackbarPos` `:trackbar` `:value`
Sets the trackbar position.
* `trackbar` `[handle!]` cvTrackbar
* `value` `[integer!]` New position.

#### `getTrackbarPos` `:trackbar`
Gets the trackbar position.
* `trackbar` `[handle!]` cvTrackbar

#### `selectROI` `:src`
Allows users to select a ROI on the given image.
* `src` `[image! handle!]` Image or cvMat handle

#### `setMouseCallback` `:window` `:context` `:word`
Sets mouse handler for the specified window.
* `window` `[any-string!]`
* `context` `[object!]` Function context
* `word` `[word! none!]` Function name

#### `getTickCount`
Returns the number of ticks.

#### `getTickFrequency`
Returns the number of ticks per second.

#### `getNumThreads`
Returns the number of threads used by OpenCV for parallel regions.

#### `getVersionString`
Returns library version string.

#### `getBuildInformation`
Returns full configuration time cmake output.

#### `useOptimized`
Returns the status of optimized code usage.

#### `setUseOptimized` `:onoff`
Enables or disables the optimized code.
* `onoff` `[logic!]`

#### `qrcode-encode` `:text`
Encode string to an image.
* `text` `[any-string!]`
* `/version` Symbol version of QR Code range
* `v` `[integer!]` 1 - 40
* `/mode` Encoding mode
* `m` `[integer!]`
* `/correction` Approximate error correction level (low, medium, quartile, high)
* `level` `[integer!]` 0 - 3 (default is 0)

#### `qrcode-decode` `:image`
Decode string from an image with a QRcode
* `image` `[image! handle! file!]` Image or cvMat handle


## Used handles and its getters / setters

#### __CVMAT__ - OpenCV Matrix

```rebol
;Refinement       Gets                Sets                          Description
/size             pair!               none                          "2D size of a matrix element"
/type             word!               none                          "Type of a matrix element"
/depth            word!               none                          "Depth of a matrix element"
/channels         integer!            none                          "Number of channels"
/binary           binary!             none                          "Binary"
/image            image!              none                          "Image"
/vector           vector!             none                          "Vector"
/total            integer!            none                          "Number of array elements (a number of pixels if the array represents an image)"
/is-submatrix     logic!              none                          "Returns true if the matrix is a submatrix of another matrix"
/width            integer!            none                          "Width of the Matrix"
/height           integer!            none                          "Height of the Matrix"
```

#### __CVVIDEOCAPTURE__ - Class for video capturing from video files, image sequences or cameras

```rebol
;Refinement       Gets                Sets                          Description
/pos-ms           decimal!            none                          "Current position of the video file in milliseconds"
/pos-frame        decimal!            none                          "0-based index of the frame to be decoded/captured next"
/pos-ratio        decimal!            none                          "Relative position of the video file: 0=start of the film, 1=end of the film"
/width            integer!            none                          "Width of the frames in the video stream"
/height           integer!            none                          "Height of the frames in the video stream"
/fps              decimal!            none                          "Frame rate"
/fourcc           decimal!            none                          "4-character code of codec"
/frames           integer!            none                          "Number of frames in the video file"
/format           word!               none                          "Format of the Mat objects"
```

#### __CVMOUSECALLBACK__ - Mouse callback state

```rebol
;Refinement       Gets                Sets                          Description
/x                integer!            none                          "Last mouseX position"
/y                integer!            none                          "Last mouseY position"
/pos              pair!               none                          "Last mouse position as pair"
/flags            integer!            none                          "Last mouse event flags"
```


## Other extension values:
```rebol
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

;; MouseEventTypes:
EVENT_MOUSEMOVE:     0
EVENT_LBUTTONDOWN:   1
EVENT_RBUTTONDOWN:   2
EVENT_MBUTTONDOWN:   3
EVENT_LBUTTONUP:     4
EVENT_RBUTTONUP:     5
EVENT_MBUTTONUP:     6
EVENT_LBUTTONDBLCLK: 7
EVENT_RBUTTONDBLCLK: 8
EVENT_MBUTTONDBLCLK: 9
EVENT_MOUSEWHEEL:    10
EVENT_MOUSEHWHEEL:   11
```
