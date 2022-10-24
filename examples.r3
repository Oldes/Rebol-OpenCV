Rebol [
    title: "OpenCV code examples and README generator"
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
        probe get-property mat MAT_IMAGE    ;; Rebol image value
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
    append insert trim/head/tail str ajoin ["^/### " num ". "] LF
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
# Rebol/OpenCV

Initial implementation of the OpenCV extension for [Rebol3](https://github.com/Oldes/Rebol3).

So far it is considered just like a proof of concept and tested only on macOS with [OpenCV installed using homebrew](https://formulae.brew.sh/formula/opencv)!


When building on macOS, the setup expects, that OpenCV includes and libs are accessible. So it is recommanded to use something like:
```
ln -s /opt/homebrew/Cellar/opencv/4.6.0/include/opencv4/opencv2 /usr/local/include/opencv2
ln -s /opt/homebrew/Cellar/opencv/4.6.0/lib /usr/local/lib/opencv
```
To import the extension from Rebol on macOS, the Rebol must be signed with entitlemens containing `com.apple.security.cs.disable-library-validation` as `true`. 

Feature requests are welcome.

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

