versionnum="v1.0"

filename_iso="patch/auto_patch/Yumimi Mix Remix EN [${versionnum}] ISO.xdelta"
filenameredump="patch/redump_patch/Yumimi Mix Remix EN [${versionnum}] Redump.xdelta"
filenamesplitbin="patch/splitbin_patch/Yumimi Mix Remix EN [${versionnum}] SplitBin.xdelta"

mkdir -p patch
mkdir -p patch/auto_patch
mkdir -p patch/redump_patch
mkdir -p patch/splitbin_patch


./build.sh

#../discaster/discaster yumimi.dsc
rm -f "$filenameredump"
xdelta3 -e -f -B 387896544 -s "patch/exclude/yumimi.bin" "yumimi_build.bin" "$filenameredump"

# dump built image to iso and generate patch against that
bchunk -w "yumimi_build.bin" "yumimi_build.cue" "yumimi_build_iso_"
rm -f "$filename_iso"
xdelta3 -e -f -B 275396608 -s "patch/exclude/yumimi_01.iso" "yumimi_build_iso_01.iso" "$filename_iso"
#rm -f yumimi_build_iso_*
