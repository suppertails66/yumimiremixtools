
set -o errexit

function outlineSolidPixels() {
  convert "$1" \( +clone -channel A -morphology EdgeOut Diamond:1 -negate -threshold 15% -negate +channel +level-colors \#000024 \) -compose DstOver -composite "$2"
}

outlineSolidPixels $1 $2
