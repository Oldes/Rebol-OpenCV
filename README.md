# Rebol/OpenCV

Initial implementation of the OpenCV extension for [Rebol3](https://github.com/Siskin-framework/Rebol)

## Usage

```rebol
cv: import %/Users/oldes/GIT/Builder/tree/rebol/Rebol-OpenCV/build/opencv-macos-arm64.rebx
img: cv/imread %/Users/oldes/GIT/Builder/assets/siskin-512.png
if img [
    with cv [
        imshow/name img "Test image"
        forever [
            k: pollKey        ;; check if there was any key pressed
            if k = 27 [break] ;; exit on ESC key
            wait 0.0001       ;; let Rebol breath as well
        ]
        destroyAllWindows
    ]
]
```
