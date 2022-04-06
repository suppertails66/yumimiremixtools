
echo "*******************************************************************************"
echo "Setting up environment..."
echo "*******************************************************************************"

set -o errexit

BASE_PWD=$PWD
PATH=".:$PATH"

DISCASTER="../discaster/discaster"
ARMIPS="./armipsh/build/armips"

mkdir -p out

echo "*******************************************************************************"
echo "Copying game files..."
echo "*******************************************************************************"

#rm -r -f out/files
#if [ ! -e out/files ]; then
  mkdir -p out/files
  cp -r disc/files/* out/files
#fi

cp -r "scenes" "out"

echo "*******************************************************************************"
echo "Building tools..."
echo "*******************************************************************************"

make

#if [ ! -e $ARMIPS ]; then
  
  echo "********************************************************************************"
  echo "Building armipsh..."
  echo "********************************************************************************"
  
  cd armipsh
    mkdir -p build && cd build
#    cmake -DCMAKE_BUILD_TYPE=Debug ..
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
  cd $BASE_PWD
  
#fi

echo "*******************************************************************************"
echo "Building font..."
echo "*******************************************************************************"

mkdir -p out/font

ymm_fontbuild "font/scene/" "out/font/font_scene"

ymm_genchrompal 0x00 0x00 0x00 0xFF 0xFF 0xFF "out/font/font_scene_pal_std.pal" "out/font/font_scene_pal_std.png" --trans --force 2 0x22 0x22 0x22

ymm_genchrompal 0x00 0x00 0x00 0xFF 0xFF 0xFF "out/font/font_scene_pal_karaokeback.pal" "out/font/font_scene_pal_karaokeback.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0xFF 0x00 0x88 "out/font/font_scene_pal_karaokefront.pal" "out/font/font_scene_pal_karaokefront.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0xFF 0xCC 0xE8 "out/font/font_scene_pal_karaokefade1.pal" "out/font/font_scene_pal_karaokefade1.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0xFF 0x99 0xD1 "out/font/font_scene_pal_karaokefade2.pal" "out/font/font_scene_pal_karaokefade2.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0xFF 0x66 0xBA "out/font/font_scene_pal_karaokefade3.pal" "out/font/font_scene_pal_karaokefade3.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0xFF 0x33 0xA3 "out/font/font_scene_pal_karaokefade4.pal" "out/font/font_scene_pal_karaokefade4.png" --trans --force 2 0x22 0x22 0x22

echo "*******************************************************************************"
echo "Converting graphics..."
echo "*******************************************************************************"

function remapPalette() {
  oldFile=$1
  palFile=$2
  newFile=$3
  
  if [ "$newFile" == "" ]; then
    newFile=$oldFile
  fi
  
  convert "$oldFile" -dither None -remap "$palFile" PNG32:$newFile
}

function cropImg() {
  oldFile=$1
  cropX=$2
  cropY=$3
  cropW=$4
  cropH=$5
  newFile=$6
  
  convert "$oldFile" -crop ${cropW}x${cropH}+${cropX}+${cropY} +repage   PNG32:$newFile
}

function do4bppConversionSprite() {
  fileBase=$1
  paletteName=$2
  
  sat_4bpp_decolorize "$fileBase.png" "$paletteName" "$fileBase.png" -t
  sat_4bpp_grpmake "$fileBase.png" "$fileBase.bin"
}

function copyGrpArea() {
  infileName=$1
  srcX=$2
  srcY=$3
  srcW=$4
  srcH=$5
  outfileName=$6
  dstX=$7
  dstY=$8
  
  convert "$outfileName"\
  \( "$infileName" -crop ${srcW}x${srcH}+${srcX}+${srcY} +repage \)\
  -geometry +${dstX}+${dstY} -compose Copy -composite\
  "PNG32:$outfileName"
}

cp -r "rsrc" "out"

#=====
# title menu
#=====

cp "out/rsrc/orig/mainmenu_3.png" "out/rsrc/mainmenu_3.png"
cp "out/rsrc/orig/mainmenu_4.png" "out/rsrc/mainmenu_4.png"

# copy shared menu options from 1 to 2
copyGrpArea "out/rsrc/mainmenu_1.png" 80 80 104 24 "out/rsrc/mainmenu_2.png" 80 80
copyGrpArea "out/rsrc/mainmenu_1.png" 80 152 80 24 "out/rsrc/mainmenu_2.png" 80 152
# generate grayed-out options for alt menu used
# when no save files exist
cropImg "out/rsrc/mainmenu_1.png" 80 104 104 24 "out/rsrc/mainmenu_opt1_disabled.png"
cropImg "out/rsrc/mainmenu_1.png" 80 128 88 24 "out/rsrc/mainmenu_opt2_disabled.png"
ymm_erase_pixel_pattern "out/rsrc/mainmenu_opt1_disabled.png" "out/rsrc/mainmenu_opt1_disabled.png"
ymm_erase_pixel_pattern "out/rsrc/mainmenu_opt2_disabled.png" "out/rsrc/mainmenu_opt2_disabled.png"
# copy from 1 to 2
copyGrpArea "out/rsrc/mainmenu_opt1_disabled.png" 0 0 104 24 "out/rsrc/mainmenu_2.png" 80 104
copyGrpArea "out/rsrc/mainmenu_opt2_disabled.png" 0 0 88 24 "out/rsrc/mainmenu_2.png" 80 128

# copy common "internal"/"cartridge" labels from mainmenu_1
# to other graphics as needed
# "internal"
copyGrpArea "out/rsrc/mainmenu_1.png" 336 224 40 16 "out/rsrc/mainmenu_2.png" 336 224
copyGrpArea "out/rsrc/mainmenu_1.png" 336 224 40 16 "out/rsrc/mainmenu_3.png" 336 224
copyGrpArea "out/rsrc/mainmenu_1.png" 336 224 40 16 "out/rsrc/mainmenu_4.png" 336 224
copyGrpArea "out/rsrc/mainmenu_1.png" 336 224 40 16 "out/rsrc/mainmenu_4.png" 64 80
#copyGrpArea "out/rsrc/mainmenu_1.png" 336 224 40 16 "out/rsrc/backup_format_option0.png" 0 0
# "cartridge"
copyGrpArea "out/rsrc/mainmenu_1.png" 376 224 40 16 "out/rsrc/mainmenu_2.png" 376 224
copyGrpArea "out/rsrc/mainmenu_1.png" 376 224 40 16 "out/rsrc/mainmenu_3.png" 376 224
copyGrpArea "out/rsrc/mainmenu_1.png" 376 224 40 16 "out/rsrc/mainmenu_4.png" 376 224
copyGrpArea "out/rsrc/mainmenu_1.png" 376 224 40 16 "out/rsrc/mainmenu_4.png" 128 80
#copyGrpArea "out/rsrc/mainmenu_1.png" 376 224 40 16 "out/rsrc/backup_format_option1.png" 0 0

sat_4bpp_decolorize "out/rsrc/mainmenu_1.png" "rsrc_raw/pal/backup_format.pal" "out/rsrc/mainmenu_1.png"
cp "out/rsrc/mainmenu_1.png" "out/scenes/C000A.CUT/scene0_bgmap1.png"

sat_4bpp_decolorize "out/rsrc/mainmenu_2.png" "rsrc_raw/pal/backup_format.pal" "out/rsrc/mainmenu_2.png"
cp "out/rsrc/mainmenu_2.png" "out/scenes/C000A.CUT/scene0_bgmap2.png"

sat_4bpp_decolorize "out/rsrc/mainmenu_3.png" "rsrc_raw/pal/backup_format.pal" "out/rsrc/mainmenu_3.png"
cp "out/rsrc/mainmenu_3.png" "out/scenes/C000A.CUT/scene0_bgmap3.png"

sat_4bpp_decolorize "out/rsrc/mainmenu_4.png" "rsrc_raw/pal/backup_format.pal" "out/rsrc/mainmenu_4.png"
cp "out/rsrc/mainmenu_4.png" "out/scenes/C000A.CUT/scene0_bgmap4.png"

#=====
# other menu stuff
#=====

sat_4bpp_decolorize "out/rsrc/title_logo.png" "rsrc_raw/pal/op_logo_mod.pal" "out/rsrc/title_logo.png"
convert "out/scenes/OP_YUMI.DAT/scene0_bgmap0.png"\
  "out/rsrc/title_logo.png" -geometry +320+0 -composite\
  PNG32:"out/scenes/OP_YUMI.DAT/scene0_bgmap0.png"

sat_4bpp_decolorize "out/rsrc/backup_format.png" "rsrc_raw/pal/backup_format.pal" "out/rsrc/backup_format.png"
cp "out/rsrc/backup_format.png" "out/scenes/C000C.CUT/scene0_bgmap1.png"
copyGrpArea "out/rsrc/backup_format.png" 320 224 40 16 "out/scenes/C000C.CUT/scene0_objmap0.png" 0 0
copyGrpArea "out/rsrc/backup_format.png" 360 224 40 16 "out/scenes/C000C.CUT/scene0_objmap1.png" 0 0

sat_4bpp_decolorize "out/rsrc/backup_spacelow.png" "rsrc_raw/pal/backup_format.pal" "out/rsrc/backup_spacelow.png"
cp "out/rsrc/backup_spacelow.png" "out/scenes/C000B.CUT/scene0_bgmap1.png"

sat_4bpp_decolorize "out/rsrc/advertise.png" "rsrc_raw/pal/advertise.pal" "out/rsrc/advertise.png"
cp "out/rsrc/advertise.png" "out/scenes/C021.DAT/scene0_bgmap1.png"

#=====
# in-game interface
#=====

sat_4bpp_decolorize "out/rsrc/interface.png" "rsrc_raw/pal/interface_mod.pal" "out/rsrc/interface.png"
tallundmp_md "out/rsrc/interface.png" "out/rsrc/interface.bin"
datpatch "rsrc_raw/interface_block.bin" "out/rsrc/interface_block.bin" "out/rsrc/interface.bin" 0x800

#=====
# pause menu
#=====

function doPauseMenuGrpConv() {
  oldFile=$1
  cropX=$2
  cropY=$3
  cropW=$4
  cropH=$5
  fileBase=$6
  
  cropImg "$oldFile" "$cropX" "$cropY" "$cropW" "$cropH" "$fileBase.png"
  do4bppConversionSprite "$fileBase" "out/rsrc/pause_menu_std.pal"
}
  
ymm_genchrompal 0x00 0x00 0x00 0xFF 0xFF 0xFF "out/rsrc/pause_menu_std.pal" "out/rsrc/pause_menu_pal_std.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0xFF 0x88 0x00 "out/rsrc/pause_menu_selected.pal" "out/rsrc/pause_menu_pal_selected.png" --trans --force 2 0x22 0x22 0x22
ymm_genchrompal 0x00 0x00 0x00 0x88 0x88 0x88 "out/rsrc/pause_menu_off.pal" "out/rsrc/pause_menu_pal_off.png" --trans --force 2 0x22 0x22 0x22

doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x00)) $((64)) $((10)) "out/rsrc/pause_menu_honorifics"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x10)) $((56)) $((10)) "out/rsrc/pause_menu_subtitles"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x20)) $((80)) $((10)) "out/rsrc/pause_menu_signs"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x30)) $((96)) $((10)) "out/rsrc/pause_menu_sceneskip"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x40)) $((152)) $((10)) "out/rsrc/pause_menu_sceneskipdesc"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x50)) $((16)) $((10)) "out/rsrc/pause_menu_on"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x60)) $((24)) $((10)) "out/rsrc/pause_menu_off"
doPauseMenuGrpConv "out/rsrc/pause_menu.png" $((0x00)) $((0x70)) $((16)) $((16)) "out/rsrc/pause_menu_cursor"

#=====
# yumimi puzzle
#=====

remapPalette "out/rsrc/ymmpuz_title.png" "rsrc/orig/ymmpuz_title.png"
sat_8bpp_tileconv "out/rsrc/ymmpuz_title.png" "out/rsrc/ymmpuz_title_tiles.bin" "out/rsrc/ymmpuz_title_tilemap.bin" -p "rsrc_raw/pal/ymmpuz_title.pal"

convert -size 320x224 xc:#0083D5\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry -40-40 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry -40+24 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry -40+88 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry -40+152 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry -40+216 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +24-8 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +24+56 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +24+120 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +24+184 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +88-40 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +88+24 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +88+88 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +88+152 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +88+216 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +152-8 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +152+56 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +152+120 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +152+184 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +216-40 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +216+24 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +216+88 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +216+152 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +216+216 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +280-8 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +280+56 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +280+120 -composite\
  "out/rsrc/ymmpuz_kabe0_stamp.png" -geometry +280+184 -composite\
  "PNG32:out/rsrc/ymmpuz_kabe0.png"
remapPalette "out/rsrc/ymmpuz_kabe0.png" "rsrc/orig/ymmpuz_kabe0.png"
sat_8bpp_tileconv "out/rsrc/ymmpuz_kabe0.png" "out/rsrc/ymmpuz_kabe0_tiles.bin" "out/rsrc/ymmpuz_kabe0_tilemap.bin" -p "rsrc_raw/pal/ymmpuz_kabe0.pal"

remapPalette "out/rsrc/ymmpuz_osimai.png" "rsrc/orig/ymmpuz_osimai.png"
sat_8bpp_tileconv "out/rsrc/ymmpuz_osimai.png" "out/rsrc/ymmpuz_osimai_tiles.bin" "out/rsrc/ymmpuz_osimai_tilemap.bin" -p "rsrc_raw/pal/ymmpuz_osimai.pal"

remapPalette "out/rsrc/ymmpuz_tease.png" "rsrc/orig/ymmpuz_tease.png"
sat_8bpp_tileconv "out/rsrc/ymmpuz_tease.png" "out/rsrc/ymmpuz_tease_tiles.bin" "out/rsrc/ymmpuz_tease_tilemap.bin" -p "rsrc_raw/pal/ymmpuz_tease.pal"

#=====
# layering error fixes
#=====

cp out/rsrc/C119_fix/* "out/scenes/C119.DAT"
cp out/rsrc/C856_fix/* "out/scenes/C856.DAT"
cp out/rsrc/C869A_fix/* "out/scenes/C869A.DAT"

echo "*******************************************************************************"
echo "Generating credits sequences..."
echo "*******************************************************************************"

makecredits.sh

echo "*******************************************************************************"
echo "Building script and generating scenes..."
echo "*******************************************************************************"

mkdir -p out/scripttxt
#mkdir -p out/scriptwrap
mkdir -p out/script

ymm_scriptimport

# subtitle wrapping is done dynamically, so we have nothing to wrap here
#ymm_scriptwrap "out/scripttxt/spec_scene.txt" "out/scriptwrap/spec_scene.txt" "table/yumimi_scenes_en.tbl" "out/font/fontwidth_scene.bin" 0x50

ymm_scriptbuild "out/scripttxt/" "out/script/"

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

echo "*******************************************************************************"
echo "Building disc..."
echo "*******************************************************************************"

$DISCASTER "yumimi.dsc"

echo "*******************************************************************************"
echo "Success!"
echo "*******************************************************************************"
