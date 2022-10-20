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

Basic image input:
```rebol
cv: import %opencv.rebx
cv/startWindowThread ;; is this needed?

with cv [
    namedWindow  win: "Some Window"
    resizeWindow win 600x300
    moveWindow   win 300x300 ;<-- bug in OpenCV! On macOS the window is not moved when there is no content

    print "Press any key inside the opened window!"
    waitKey 0                  ;; wait for any key in the opened window

    img: imread %image/mask.png
    if img [
        imshow img             ;; show the image using default name
        imshow/name img win    ;; show the image in the existing named window (resized)
        moveWindow win 300x100 ;; move the window into some location

        print "Press ESC in any window to close all of them!"
        forever [
            k: pollKey         ;; check if there was any key pressed
            if k = 27 [break]  ;; exit on ESC key
            wait 0.01          ;; let Rebol breath as well
        ]
    ]
    destroyAllWindows
]
```

Getting `cvMat` properties:
```rebol
with cv [
    img: imread %image/mask.png
    mat: resize img 10%
    probe get-property mat MAT_SIZE
    probe get-property mat MAT_TYPE
    probe get-property mat MAT_CHANNELS
    probe get-property mat MAT_BINARY
    probe get-property mat MAT_IMAGE
]
```


Video input example:
```rebol
cv: import %opencv.rebx
with cv [
    ;; initialize video input from a file...
    ;cam: VideoCapture %test.mp4
    ;; or from a camera device...
    cam: VideoCapture 0   ? cam   ;; should be a cvVideoCapture handle
    unless cam [quit]

    frame: read :cam      ? frame   ;; should be a cvMat handle
    frame-sc: resize :frame 25%     ;; creates new scaled cvMat handle
    if frame [
        forever [
            read/into :cam :frame                        ;; reusing existing frame
            resize/into :frame 25% :frame-sc             ;; resize input frame by 25% (reusing existing cvMat)
            blur/border :frame-sc 16x16 BORDER_REPLICATE ;; blur the frame using kernel of size 64x64 pixels
            imshow :frame-sc
            k: pollKey            ;; check if there was any key pressed
            if k = 27 [break]     ;; exit on ESC key
            wait 0.01             ;; let Rebol breath as well
        ]
        destroyAllWindows
        ;; try to save the last resolved frame into a file...
        print "Saving the last frame into test images..."
        prin "test.jpg   " probe imwrite      %tmp/test.jpg   :frame
        prin "test_9.png " probe imwrite/with %tmp/test_9.png :frame [IMWRITE_PNG_COMPRESSION 9]
        prin "test_0.png " probe imwrite/with %tmp/test_0.png :frame [IMWRITE_PNG_COMPRESSION 0]
        prin "test.webp  " probe imwrite/with %tmp/test.webp  :frame [IMWRITE_WEBP_QUALITY 80]
    ]
    print "closing.."
    free :cam
    free :frame
    print "done"
]
```

Using bilateral filter to remove noise from an image:
```rebol
cv: import %opencv.rebx
img: cv/imread %image/taj.jpg
filtered: cv/bilateralFilter img 15 75.0 75.0
cv/imwrite %image/taj-bfilter.jpg :filtered
```

Color space conversion:
```rebol
cv: import %opencv.rebx
img: cv/imread %image/taj-bfilter.jpg
hls: cv/cvtColor img cv/COLOR_BGR2HLS
gre: cv/cvtColor img cv/COLOR_BGR2GRAY
cv/imwrite %image/taj-hls.jpg  :hls
cv/imwrite %image/taj-grey.jpg :gre
```

Image threshold:
```rebol
cv: import %opencv.rebx
with cv [
    img:  imread %image/mask.png
    gray: cvtColor img COLOR_BGR2GRAY
    threshold :gray :gray 0 255 THRESH_BINARY_INV
    imshow/name img  "Source"
    imshow/name dest "Result"
    waitkey 0
]
```

Saving video from the camera:
```rebol
cv: import %opencv.rebx
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

    ;set-property cam CAP_PROP_POS_FRAMES 2000.0 // can be used to set position in the video (file input)

    if frame: read :cam [
        ;; initialize VideoWriter (when 0 is used as codec parameter, than the output will be MJPG)
        out: VideoWriter %out.avi 0 24 size
        unless out [print "Failed to initialize VideoWriter!" quit]

        ;; grab 100 frames maximum...
        loop 100 [ 
            read/into :cam :frame    ;; reusing existing frame
            write out :frame         ;; append the frame to the output video
            imshow :frame            ;; and also show it in the window
            if pollKey = 27 [break]  ;; exit on ESC key
            wait 0.01                ;; let Rebol breath as well
        ]
        destroyAllWindows
    ]
    free :out
    free :cam
    free :frame
    print "done"
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