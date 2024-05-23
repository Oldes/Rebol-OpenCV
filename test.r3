Rebol [
	title: "Basic OpenCV extension test"
]

system/options/quiet: false
system/options/log/rebol: 4

CI?: any [
	"true" = get-env "CI"
	"true" = get-env "GITHUB_ACTIONS"
	"true" = get-env "TRAVIS"
	"true" = get-env "CIRCLECI"
	"true" = get-env "GITLAB_CI"
]

if CI? [
	;; for the CI test the module is the build directory 
	system/options/modules: to-real-file %build/
	print ["Using modified modules location:" as-green system/options/modules]
	ls (system/options/modules)

	if system/platform = 'Windows [
		;; include a directory with OpenCV dlls in the PATH environment variable
		set-env "PATH" ajoin [
			probe to-local-file to-real-file rejoin [%build/ system/build/arch %/vc17/bin/]
			#";" get-env "PATH"
		]
	]
]

try [system/modules/opencv: none] ;; make sure that we use the local one

cv: import opencv

? cv
print cv/getVersionString
print cv/getBuildInformation

print as-green "Encode QRcode..."
mat: cv/qrcode-encode https://github.com/Oldes/Rebol-OpenCV/
probe mat
probe mat/vector
save %build/qrcode.png mat/image