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

    img: imread %/Users/oldes/GIT/Builder/assets/siskin-512.png
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

Video input example:
```rebol
cv: import %opencv.rebx
with cv [
    ;; initialize video input from a file...
    ;cam: VideoCapture %test.mp4
    ;; or from a camera device...
    cam: VideoCapture 0   ? cam   ;; should be a cvVideoCapture handle
    unless cam [quit]

    frame: read :cam      ? frame ;; should be a cvMat handle
    if frame [
        forever [
            read/into :cam :frame ;; reusing existing frame
            blur/border frame 64x64 BORDER_REPLICATE  ;; blur the frame using kernel of size 64x64 pixels
            imshow :frame
            k: pollKey            ;; check if there was any key pressed
            if k = 27 [break]     ;; exit on ESC key
            wait 0.01             ;; let Rebol breath as well
        ]
        destroyAllWindows
        ;; try to save the last resolved frame into a file...
        print "Saving the last frame into test images..."
        prin "test.jpg   " probe imwrite %test.jpg :frame
        prin "test_9.png " probe imwrite/with %test_9.png :frame [IMWRITE_PNG_COMPRESSION 9]
        prin "test_0.png " probe imwrite/with %test_0.png :frame [IMWRITE_PNG_COMPRESSION 0]
        prin "test.webp  " probe imwrite/with %test.webp  :frame [IMWRITE_WEBP_QUALITY 80]
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