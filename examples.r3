Rebol [
    title: "OpenCV code examples and README generator"
    needs: 3.16.0 ;; using the new construction syntax 
]

cv: import 'opencv

examples: [
comment {
    All folowing examples expect, that OpenCV extension was imported using one of these methods:
    1. Using direct path to the file: `cv: import %path/to/opencv.rebx`
    2. Using extension in the default location: `cv: import 'opencv`
}
;---------------------------------------------------------------------------------------
example "Making a blank matrix and displaying it." {
    Matrices are one of the main datatypes used by OpenCV extension.

    ```rebol
    mat: cv/Matrix 250x140 ;; creates an empty black image handle
    cv/imshow mat          ;; open a window with default name "Image" displaying the image
    cv/waitKey 0           ;; and wait for any key
    ```
}
example "Closing a window" {
    Window is closed using `destroyWindow "Window name"`.
    It is possible to close all windows using `destroyAllWindows`

    ```rebol
    cv/destroyWindow "Image"       ;; because "Image" is the default window's name.
    ```
}
example "Loading a matrix from file" {
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
}
example "Moving/resizing an opened window" {
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
}
example "Getting matrix properties" {
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
}
example "Manually releasing matrices" {
    Normally matrices are automatically released by Rebol's GC, but it is also possible to free them manually

    ```rebol
    cv/free mat          ;; manually released matrix
    ```

    It should be noted, that such a matrix is not usable anymore! This will fail:

    ```rebol
    probe cv/imshow mat  ;; will return false!
    ```
}
;---------------------------------------------------------------------------------------
example "Color space conversion" {
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
}
example "Image threshold" {
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
}
example "Using computed binary threshold as an opacity channel" {
    ```rebol
    image: cv/get-property img     cv/MAT_IMAGE   ;; get Rebol image
    alpha: cv/get-property thresh1 cv/MAT_BINARY  ;; get Rebol binary with alpha values
    image/alpha: alpha                            ;; replace image alpha with the new value
    save %tmp/masked.png image                    ;; using Rebol's PNG codec to save the new image
    ```
}
;---------------------------------------------------------------------------------------

example "Detecting edges in the image and their dilatation" {
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
}

example "Detecting edges using Gabor Filter" {
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
}


;---------------------------------------------------------------------------------------
example "Blurring images" {
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
}
example "Applying a color map" {
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
}
example "Applying Sepia filter to an image" {
    ```rebol
    with cv [
        src: imread "image/taj.jpg"
        ; using a binary for the kernel, but it should be possible
        ; to use vector directly later once implemented!
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
}
example "Using shared buffer" {
    When constructing matrices from a Rebol's binary or vector value, the buffer may be shared.
    ```rebol
    with cv [
        ;; allocate vector for a grayscale image of size 320x200
        data: #(uint8! 64000)
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
}
example "Saving video from the camera" {
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
}

;---------------------------------------------------------------------------------------

example "Computing absolute difference between 2 video frames" {
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
}

;---------------------------------------------------------------------------------------

example "Encode and decode QR codes" {
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
}

;---------------------------------------------------------------------------------------
] ; end of examples block


on-code-error: func[err][
    print err
    print as-red "README.md NOT MODIFIED!"
    quit
]

output: make string! 30000
readme: func[val][ append output val  val ]

num: 0
emit-comment: func[str][
    replace/all str "^/    " LF
    print readme str
]
emit-heading: func[str][
    ++ num
    append insert trim/head/tail str "^/### " LF
    print readme str
]
emit-code: func[str][
    replace/all str "^/    " LF
    print readme str
    parse str [ any [
        thru "```rebol" copy blk to "```" 3 skip (
            try/except load blk :on-code-error
        )
    ]]
]

readme {
[![Rebol-OpenCV CI](https://github.com/Oldes/Rebol-OpenCV/actions/workflows/main.yml/badge.svg)](https://github.com/Oldes/Rebol-OpenCV/actions/workflows/main.yml)
[![Gitter](https://badges.gitter.im/rebol3/community.svg)](https://app.gitter.im/#/room/#Rebol3:gitter.im)

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
}

parse examples [ any [
      'comment set str: string! (emit-comment str)
    |['example set str: string! (emit-heading str)
               set str: string! (emit-code str)]
    | skip
]]

readme {

* * * *
This file was generated using [examples.r3](examples.r3) script.
}

write %README.md output


