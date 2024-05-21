Rebol [
	title: "Basic OpenCV extension test"
]

CI?: "true" = get-env "CI"
;; for the CI test the module is in current directory 
if CI? [system/options/modules: to-real-file %build/]

system/modules/opencv: none ;; make sure that we use the local one 
cv: import opencv

? cv
print cv/getVersionString
print cv/getBuildInformation