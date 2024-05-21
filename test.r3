Rebol [
	title: "Basic OpenCV extension test"
]

CI?: "true" = get-env "CI"

if CI? [
	;; for the CI test the module is the build directory 
	system/options/modules: to-real-file %build/
	;; include the directory in PATH environment variable
	;; (so OpenCV can find its libraries)
	set-env "PATH" ajoin [
		to-local-file system/options/modules
		pick [#";" #":"] system/platform = 'Windows
		get-env "PATH"
	]
]

try [system/modules/opencv: none] ;; make sure that we use the local one

cv: import opencv

? cv
print cv/getVersionString
print cv/getBuildInformation