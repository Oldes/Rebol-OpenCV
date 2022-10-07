# Rebol/OpenCV

Initial implementation of the OpenCV extension for [Rebol3](https://github.com/Oldes/Rebol3)

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
            imshow :frame
            k: pollKey            ;; check if there was any key pressed
            if k = 27 [break]     ;; exit on ESC key
            wait 0.01             ;; let Rebol breath as well
        ]  
    ]
    print "closing.."
    free :cam
    free :frame
    print "done"
]
```
