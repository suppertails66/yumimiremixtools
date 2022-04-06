
set -o errexit

function ripScene() {
#  echo "$1"
  OUTNAME=$2$(basename $1)
  mkdir -p "$OUTNAME"
  ./ymm_scenerip "$1" "$OUTNAME/" "sat"
}

make blackt
make libmd
make ymm_scenerip

#ripScene C002.DAT out/

#for file in {../../build_origfiles/*.DAT,../../build_origfiles/*.CUT}; do
#  echo $file
#done;

#for file in ../../build_origfiles/*.DAT; do
# ripScene $file out/
#done;

#for file in scenes_raw/OP_YUMI.DAT; do
#for file in scenes_raw/C102.DAT; do
#for file in scenes_raw/C101B.DAT; do
#for file in scenes_raw/C000C.CUT; do
#for file in {scenes_raw/*.DAT,scenes_raw/*.CUT}; do
# for file in scenes_raw/{C000A.CUT,C000B.CUT,C000C.CUT,C021.DAT,OP_YUMI.DAT,ENDC000A.DAT,ENDC000B.DAT,ENDC000C.DAT}; do
#   echo "dumping $file"
#   ripScene $file "scenes_unpacked/"
# done;


#for file in scenes_raw/C231.DAT; do
#for file in scenes_raw/C119.DAT; do
#for file in scenes_raw/C869A.DAT; do
for file in scenes_raw/C856.DAT; do
  echo "dumping $file"
  ripScene $file "scenes_unpacked/"
done;