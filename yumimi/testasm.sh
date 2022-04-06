
set -o errexit

# nonzero to boot to credits
#ENDING_DEBUG_ON=0

BASE_PWD=$PWD
PATH=".:$PATH"

ARMIPS="./armipsh/build/armips"

mkdir -p out
  
echo "*******************************************************************************"
echo "Building armipsh..."
echo "*******************************************************************************"
  
cd armipsh
  mkdir -p build && cd build
#  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  make
cd $BASE_PWD
  
echo "*******************************************************************************"
echo "Building ASM..."
echo "*******************************************************************************"

mkdir -p out/asm

# copy original executables
cp "out/files/SATANIME.BIN" "out/asm"

# build  
$ARMIPS asm/SATANIME.BIN.asm -temp out/asm/temp.txt -sym out/asm/symbols.sym -sym2 out/asm/symbols.sym2

# copy to output folder
cp "out/asm/SATANIME.BIN" "out/files"
