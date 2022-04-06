
set -o errexit

tempFontFile=".fontrender_temp"
tempGrpFile=".grp_temp.png"

make blackt
make fontrender
make fontrender_multi


# TEST
# mkdir -p "out/scenes"
# cp -r "scenes/OP_YUMI.DAT" "out/scenes/"
# cp -r "scenes/ENDC000A.DAT" "out/scenes/"
# cp -r "scenes/ENDC000B.DAT" "out/scenes/"
# cp -r "scenes/ENDC000C.DAT" "out/scenes/"


function renderStringSystemC() {
#  outFile=$1
  message=$1
  colorR=$2
  colorG=$3
  colorB=$4
  outlineColorR=$5
  outlineColorG=$6
  outlineColorB=$7
  alignment=$8
  lineH=$9
  extraOffsetX=${10}
  extraOffsetY=${11}
  font=${12}
  hack=${13}
  
  outFile=$tempGrpFile
  
  if [ "$alignment" = "" ]; then
    alignment="--center"
  fi
  
  if [ "$lineH" = "" ]; then
    lineH=16
  fi
  
  if [ "$extraOffsetX" = "" ]; then
    extraOffsetX=0
  fi
  
  if [ "$extraOffsetY" = "" ]; then
    extraOffsetY=0
  fi
  
  if [ "$font" = "" ]; then
    font="system"
  fi
  
  printf "$message" > $tempFontFile
  ./fontrender_multi "font/$font/" "$tempFontFile" "font/$font/table.tbl" "$outFile"\
    $alignment --lineh $lineH\
    --recolor $colorR $colorG $colorB\
    --outline $outlineColorR $outlineColorG $outlineColorB\
    --xoff $((0+extraOffsetX)) --yoff $((2+extraOffsetY))\
    $hack
}

function blankGrpArea() {
  fileName=$1
  x=$2
  y=$3
  w=$4
  h=$5
  
#   convert -size ${w}x${h} xc:transparent\
#   "$fileName" -geometry +0+0 -composite\
#   PNG32:out/rsrc/grp/title_logo_full_convert.png
  convert "$fileName"\
  -size ${w}x${h} xc:black -geometry +${x}+${y} -compose dst-out -composite\
  "PNG32:$fileName"
}

function maskGrpArea() {
  fileName=$1
  x=$2
  y=$3
  maskFilename=$4
  
#   convert -size ${w}x${h} xc:transparent\
#   "$fileName" -geometry +0+0 -composite\
#   PNG32:out/rsrc/grp/title_logo_full_convert.png
  convert "$fileName"\
  "$maskFilename" -geometry +${x}+${y} -compose dst-out -composite\
  "PNG32:$fileName"
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
  
#   convert -size ${w}x${h} xc:transparent\
#   "$fileName" -geometry +0+0 -composite\
#   PNG32:out/rsrc/grp/title_logo_full_convert.png
  convert "$outfileName"\
  \( "$infileName" -crop ${srcW}x${srcH}+${srcX}+${srcY} +repage \)\
  -geometry +${dstX}+${dstY} -compose Copy -composite\
  "PNG32:$outfileName"
}

function overwriteCreditsBlockCenter() {
  fileName=$1
  boxX=$2
  boxY=$3
  boxW=$4
  boxH=$5
  noclear=$6
  
  srcFile=$tempGrpFile
  
  if [ "$noclear" = "" ]; then
    blankGrpArea $fileName $boxX $boxY $boxW $boxH
  fi
  
  boxCenterX=$(($boxX+(boxW/2)))
  boxCenterY=$(($boxY+(boxH/2)))
  # -region ${boxW}x${boxH}+${boxX}+${boxY}
#  convert "$fileName"\
#  "$srcFile" -geometry +${boxCenterX}+${boxCenterY} -gravity Center -composite\
#  "PNG32:$fileName"

  # imagemagick, with all its infinite useless options,
  # has no built-in ability to composite and center one image over another
  # at a specified point.
  # the only way to do it is to use the output from arcane image query functions
  # to assign it directly to the target output position.
  # wonderful!
  offsetX=$(($(convert $srcFile -format "%[fx:$xoffset-w]" info:)/2))
  offsetY=$(($(convert $srcFile -format "%[fx:$yoffset-h]" info:)/2))
#  echo $offsetX $offsetY
  dstX=$((boxCenterX+offsetX))
  dstY=$((boxCenterY+offsetY))
#  echo $dstX $dstY

  convert "$fileName"\
  "$srcFile" -geometry +${dstX}+${dstY} -composite\
  "PNG32:$fileName"
}

function overwriteCreditsBlockLeft() {
  fileName=$1
  boxX=$2
  boxY=$3
  boxW=$4
  boxH=$5
#  noclear=$6
  
  srcFile=$tempGrpFile
  
  
  if [ "$boxW" != "" ]; then
    blankGrpArea $fileName $boxX $boxY $boxW $boxH
  fi
  
  dstX=$boxX
  dstY=$boxY

  convert "$fileName"\
  "$srcFile" -geometry +${dstX}+${dstY} -composite\
  "PNG32:$fileName"
}

function overwriteCreditsBlockRight() {
  fileName=$1
  boxX=$2
  boxY=$3
  boxW=$4
  boxH=$5
  
  srcFile=$tempGrpFile
  
  if [ "$boxW" != "" ]; then
    blankGrpArea $fileName $boxX $boxY $boxW $boxH
  fi
  
#  echo $boxX $boxY $boxW $boxH
#  boxCenterX=$(($boxX+(boxW/2)))
#  boxCenterY=$(($boxY+(boxH/2)))
  offsetX=$(($(convert $srcFile -format "%[fx:$xoffset-w]" info:)))
#  offsetY=$(($(convert $srcFile -format "%[fx:$yoffset-h]" info:)))
#  echo $offsetX 
  dstX=$((boxX+(boxW-(-offsetX))))
#  dstY=$((boxCenterY+offsetY))
  dstY=$boxY
#  echo $dstX $dstY

  convert "$fileName"\
  "$srcFile" -geometry +${dstX}+${dstY} -composite\
  "PNG32:$fileName"
}

function makeOpCredits() {
  sceneBase=$1
  
  #==============
  # page 0
  #==============
  
  renderStringSystemC\
    "Scenario/Setting/Storyboards\nIzumi Takemoto"\
    0x22 0x22 0x22\
    0x11 0x11 0x11
  #cp "scenes/OP_YUMI.DAT/scene1_bgmap1.png" "test.png"
  #overwriteCreditsBlock "test.png" 36 40 120 32
  overwriteCreditsBlockCenter "${sceneBase}/scene1_bgmap1.png" 36 40 120 32
  
  #==============
  # page 1
  #==============
  
  op_page1_xBase=$((17))
  op_page1_yBase=$((16+10))
  
  blankGrpArea "${sceneBase}/scene2_bgmap2.png" 17 16 102 96
  
  renderStringSystemC\
    "Opening Song\n{Achichicchi}"\
    0xFF 0xFF 0xFF\
    0x11 0x11 0x11
#  overwriteCreditsBlockCenter "${sceneBase}/scene2_bgmap2.png" 17 16 102 96
  overwriteCreditsBlockCenter "${sceneBase}/scene2_bgmap2.png" $op_page1_xBase $op_page1_yBase 102 32
  
#   renderStringSystemC\
#     "Lyrics: Yasushi Akimoto\nComposer: Kazuya Motojima\nArrangement: Masaki Iwamoto\nVocals: Yumiko Takahashi"\
#     0xFF 0xFF 0xFF\
#     0x11 0x11 0x11\
#     --left
#   overwriteCreditsBlockLeft "${sceneBase}/scene2_bgmap2.png" 17 48 102 64
  
  # works, but too wide and covers graphic
#   renderStringSystemC\
#     "Lyrics:\nComposer:\nArrangement:\nVocals:"\
#     0xFF 0xFF 0xFF\
#     0x11 0x11 0x11\
#     --left
#   overwriteCreditsBlockLeft "${sceneBase}/scene2_bgmap2.png" 19 48
#   
#   renderStringSystemC\
#     "Yasushi Akimoto\nKazuya Motojima\nMasaki Iwamoto\nYumiko Takahashi"\
#     0xFF 0xFF 0xFF\
#     0x11 0x11 0x11\
#     --left
#   overwriteCreditsBlockLeft "${sceneBase}/scene2_bgmap2.png" 91 48
  
  renderStringSystemC\
    "Lyrics\n  Yasushi Akimoto\nMusic\n  Kazuya Motojima\nArrangement\n  Masaki Iwamoto\nVocals\n  Yumiko Takahashi"\
    0xFF 0xFF 0xFF\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene2_bgmap2.png" $op_page1_xBase $(($op_page1_yBase+32))
  
  #==============
  # page 2
  #==============
  
  renderStringSystemC\
    "Music\nMecano Associates"\
    0x22 0x22 0x22\
    0x11 0x11 0x11
  overwriteCreditsBlockCenter "${sceneBase}/scene3_bgmap1.png" 186 145 117 32
  
  #==============
  # page 3
  #==============
  
  # 小松 徹 is a complete enigma. searches turn up nothing except a couple of
  # other game arts games that are no help for determining the reading,
  # and "micropine" has no documented existence whatsoever besides this game
  renderStringSystemC\
    "Saturn Version Programming\nMiCROPiNE  Toru Komatsu"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --center 24
  overwriteCreditsBlockCenter "${sceneBase}/scene9_bgmap6.png" 88 87 144 48
  
  #==============
  # page 4
  #==============
  
  blankGrpArea "${sceneBase}/scene17_objmap0.png" 0 0 144 64
  
  renderStringSystemC\
    "Director\n  Hiroake Okabe\nAssistant Director\n  Masahiko Ikeya"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left 16 0 2
  overwriteCreditsBlockLeft "${sceneBase}/scene17_objmap0.png" 20 0
  
  #==============
  # page 5
  #==============
  
  # jumping through asinine hoops to create a transparent outline
  op_page5_string="Producers\nYoichi Miyaji\nSatoshi Uesaka"
  
  # copy no-text image over text to remove it
  copyGrpArea "${sceneBase}/scene21_bgmap0.png" 0 0 320 224 "${sceneBase}/scene21_bgmap0.png" 0 224
  
  renderStringSystemC\
    "$op_page5_string"\
    0x22 0x22 0x22\
    0x00 0x00 0x00\
    --center 16
  maskGrpArea "${sceneBase}/scene21_bgmap0.png" 49 361 "$tempGrpFile"
  renderStringSystemC\
    "$op_page5_string"\
    0x22 0x22 0x22\
    -1 -1 -1\
    --center 16 1 1
  overwriteCreditsBlockCenter "${sceneBase}/scene21_bgmap0.png" 46 360 85 48 noclear
  
  #==============
  # page 6
  #==============
  
  renderStringSystemC\
    "Design & Production\nGame Arts"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --center 24
  overwriteCreditsBlockCenter "${sceneBase}/scene23_bgmap1.png" 123 73 75 48
#   renderStringSystemC\
#     "Planning & Development"\
#     0x22 0x22 0x22\
#     0x11 0x11 0x11
#   overwriteCreditsBlockCenter "${sceneBase}/scene23_bgmap1.png" 126 74 65 14
#   renderStringSystemC\
#     "Game Arts"\
#     0x22 0x22 0x22\
#     0x11 0x11 0x11
#   overwriteCreditsBlockCenter "${sceneBase}/scene23_bgmap1.png" 123 99 75 13
}

function makeEdCredits() {
  sceneBase=$1
  
  #==============
  # copy over hand-modified subscenes from main resource folder
  # (the manual modifications are to add and display extra prerendered
  # zoom frames at the start of specific scenes where trying to do
  # the zoom effect in real time is too VDP1-intensive and eats up
  # almost all of the available drawing time for the frame,
  # causing parts of the overlay to flicker if not modified
  # to avoid this)
  #==============
  
  for file in rsrc/credits_subscenes/*; do
    cp "$file" "$sceneBase"
  done
  
  #==============
  # page -1
  #==============
  
  # copy no-text image over text to remove it
  copyGrpArea "${sceneBase}/scene1_bgmap2.png" 320 0 320 224 "${sceneBase}/scene1_bgmap2.png" 320 256
  
  renderStringSystemC\
    "Voice Performances"\
    0xFF 0xFF 0xFF\
    0x11 0x11 0x11\
    --center
  overwriteCreditsBlockCenter "${sceneBase}/scene1_bgmap2.png" $((518-4)) 366 64 16 noclear
  
  #==============
  # page 0
  #==============
  
#   renderStringSystemC\
#     "Scenario/Setting/Storyboards\nIzumi Takemoto"\
#     0x22 0x22 0x22\
#     0x11 0x11 0x11
#   #cp "scenes/OP_YUMI.DAT/scene1_bgmap1.png" "test.png"
#   #overwriteCreditsBlock "test.png" 36 40 120 32
#   overwriteCreditsBlockCenter "${sceneBase}/scene1_bgmap1.png" 36 40 120 32
  
  cp "${sceneBase}/scene2_bgmap1.png" "${sceneBase}/scene2_bgmap2.png"
  renderStringSystemC\
    "Yumimi Yoshizawa"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene2_bgmap2.png" 50 90
  renderStringSystemC\
    "Akiko Hiramatsu"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene2_bgmap2.png" 50 116
  
  #==============
  # page 1
  #==============
  
  cp "${sceneBase}/scene3_bgmap1.png" "${sceneBase}/scene3_bgmap5.png"
  renderStringSystemC\
    "Shinichi Matsuzaki"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene3_bgmap5.png" 166 90
  renderStringSystemC\
    "Toshihiko Seki"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene3_bgmap5.png" 166 116
  
  #==============
  # page 2
  #==============
  
  cp "${sceneBase}/scene4_bgmap1.png" "${sceneBase}/scene4_bgmap4.png"
  renderStringSystemC\
    "Sakurako Sakurazaki"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene4_bgmap4.png" 50 $((90+24))
  renderStringSystemC\
    "Emi Shinohara"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene4_bgmap4.png" 50 $((116+24))
  
  #==============
  # page 3
  #==============
  
  cp "${sceneBase}/scene5_bgmap1.png" "${sceneBase}/scene5_bgmap2.png"
  renderStringSystemC\
    "Prez"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene5_bgmap2.png" 166 $((90-24))
  renderStringSystemC\
    "Hideyuki Umezu"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene5_bgmap2.png" 166 $((116-24))
  
  #==============
  # page 4
  #==============
  
  cp "${sceneBase}/scene6_bgmap1.png" "${sceneBase}/scene6_bgmap4.png"
  renderStringSystemC\
    "Rie Morishita"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene6_bgmap4.png" 50 $((90+24))
  renderStringSystemC\
    "Kumiko Nishihara"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene6_bgmap4.png" 50 $((116+24))
  
  #==============
  # page 5
  #==============
  
  cp "${sceneBase}/scene7_bgmap1.png" "${sceneBase}/scene7_bgmap2.png"
  renderStringSystemC\
    "Mr. Kojima"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene7_bgmap2.png" 166 $((90-0))
  renderStringSystemC\
    "Kiyoyuki Yanada"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene7_bgmap2.png" 166 $((116-0))
  
  #==============
  # page 6
  #==============
  
  cp "${sceneBase}/scene8_bgmap1.png" "${sceneBase}/scene8_bgmap2.png"
  renderStringSystemC\
    "Mama"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene8_bgmap2.png" 50 $((90-12))
  renderStringSystemC\
    "Satoko Yasunaga"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene8_bgmap2.png" 50 $((116-12))
  
  #==============
  # page 7
  #==============
  
  cp "${sceneBase}/scene9_bgmap1.png" "${sceneBase}/scene9_bgmap2.png"
  # NOTE: this could refer to one student or multiple;
  # japanese doesn't distinguish and i can't be sure from just the voices
  renderStringSystemC\
    "Students"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene9_bgmap2.png" $((166-16)) $((90-0))
  renderStringSystemC\
    "Yoshikazu Iwanami"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockLeft "${sceneBase}/scene9_bgmap2.png" $((166-16)) $((116-0))
  
  #==============
  # page 8
  #==============
  
  cp "${sceneBase}/scene10_bgmap1.png" "${sceneBase}/scene10_bgmap2.png"
  renderStringSystemC\
    "Narration"\
    0xFF 0xFF 0xFF\
    0x11 0x11 0x11\
    --center
  overwriteCreditsBlockCenter "${sceneBase}/scene10_bgmap2.png" 0 76 320 16 noclear
  renderStringSystemC\
    "Tessho Genda"\
    0xFF 0xFF 0xFF\
    0x11 0x11 0x11\
    --center -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockCenter "${sceneBase}/scene10_bgmap2.png" 0 102 320 18 noclear
  
  #==============
  # page 9
  #==============
  
  cp "${sceneBase}/scene12_bgmap1.png" "${sceneBase}/scene12_bgmap4.png"
  renderStringSystemC\
    "Scenario/Setting/Storyboards\nLayout/Title"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --center
  overwriteCreditsBlockCenter "${sceneBase}/scene12_bgmap4.png" 0 91 320 32 noclear
  renderStringSystemC\
    "Izumi Takemoto"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --center -1 0 0\
    "system_big" "--KERNHACK"
  overwriteCreditsBlockCenter "${sceneBase}/scene12_bgmap4.png" 0 132 320 18 noclear
  
  #==============
  # page 10
  #==============
  
  blankGrpArea "${sceneBase}/scene13_bgmap2.png" 136 54 149 107
  
  ed_page10_offsetX=0
  ed_page10_offsetY=-8
  
  renderStringSystemC\
    "Director\nHiroake Okabe\n\nAssistant Director\nMasahiko Ikeya"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --right
  overwriteCreditsBlockRight "${sceneBase}/scene13_bgmap2.png" $((136+$ed_page10_offsetX)) $((54+$ed_page10_offsetY)) 149 64
  renderStringSystemC\
    "Saturn Version Programming\nMiCROPiNE  Toru Komatsu"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --right
  overwriteCreditsBlockRight "${sceneBase}/scene13_bgmap2.png" $((150+$ed_page10_offsetX)) $((127+16+$ed_page10_offsetY)) 136 34
  
  #==============
  # page 11
  #==============
  
  cp "${sceneBase}/scene14_bgmap1.png" "${sceneBase}/scene14_bgmap2.png"
  renderStringSystemC\
    "Graphics\n\nSusumu Banba\nTetsuya Amabiki\nSatoshi Oshiki\nSatoshi Yoshida"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene14_bgmap2.png" 70 46
  
  #==============
  # page 12
  #==============
  
  cp "${sceneBase}/scene15_bgmap1.png" "${sceneBase}/scene15_bgmap2.png"
  renderStringSystemC\
    "Kazuyo Nakazawa\nTeruhisa Ryu\nMiki Ukai"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene15_bgmap2.png" 174 88
  
  #==============
  # page 13
  #==============
  
  cp "${sceneBase}/scene16_bgmap1.png" "${sceneBase}/scene16_bgmap2.png"
  renderStringSystemC\
    "Coloring\n\nNobuo Horii\nMasatoshi Azumi\nAkiko Sato\nMeiko Wada\nTadao Mizuno"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene16_bgmap2.png" $((54-4)) 46
  
  #==============
  # page 14
  #==============
  
  # 池田　勇 = isamu ikeda, credited in e.g. grandia 2
  # 望月　りさ = risa mochizuki, credited in e.g. lunar eternal blue
  # 近藤　康弘 = yasuhiro kondou, credited in e.g. urusei yatsura dmf
  cp "${sceneBase}/scene17_bgmap1.png" "${sceneBase}/scene17_bgmap2.png"
  renderStringSystemC\
    "Isamu Ikeda\nRisa Mochizuki\nShinya Fujita\nYasuhiro Kondou\nSadao Kobayashi\nKazuhiro Nakazawa\nRyouju Hatae"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene17_bgmap2.png" $((200-16)) $((52+8))
  
  #==============
  # page 15
  #==============
  
  cp "${sceneBase}/scene18_bgmap1.png" "${sceneBase}/scene18_bgmap2.png"
  renderStringSystemC\
    "Programming\n\nHiroyuki Koyama\nJunichi Sakamoto\nYoshihisa Segawa\nHideki Kanai\nNaoki Kudou\nMitsuhiro Watariya"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene18_bgmap2.png" $((48-8)) $((50))
  
  #==============
  # page 16
  #==============
  
#  blankGrpArea "${sceneBase}/scene19_bgmap4.png" 136 112 102 63

  ed_page16_xOffset=28
  ed_page16_yOffset=-36
  
  cp "${sceneBase}/scene19_bgmap1.png" "${sceneBase}/scene19_bgmap4.png"
  renderStringSystemC\
    "Ending Song\n{Genki! Genki! Genki!}"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene19_bgmap4.png" $((123+$ed_page16_xOffset)) $((67+$ed_page16_yOffset))
#   renderStringSystemC\
#     "Ending Song\n{Genki! Genki! Genki!}"\
#     0x22 0x22 0x22\
#     0x11 0x11 0x11\
#     --center
#   overwriteCreditsBlockCenter "${sceneBase}/scene12_bgmap4.png" $((123+$ed_page16_xOffset)) $((67+$ed_page16_yOffset)) 0 0 noclear
  renderStringSystemC\
    "Lyrics\n  Yasushi Akimoto\nMusic\n  Kyouhei Tsutsumi\nArrangement\n  Nobuyuki Shimizu\nVocals\n  Yumiko Takahashi"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
#  overwriteCreditsBlockLeft "${sceneBase}/scene19_bgmap4.png" $((136+$ed_page16_xOffset)) $((112-16+$ed_page16_yOffset))
  overwriteCreditsBlockLeft "${sceneBase}/scene19_bgmap4.png" $((136+$ed_page16_xOffset)) $((99+$ed_page16_yOffset))
  
  #==============
  # page 17
  #==============
  
  blankGrpArea "${sceneBase}/scene20_bgmap2.png" 40 56 117 110
  
  renderStringSystemC\
    "Music & Sound Effects"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene20_bgmap2.png" $((40)) $((56))
  # 西　隆宏 = takahiro nishi
  renderStringSystemC\
    "Ari Kamijo\nTakahiro Nishi\nKenichi Kunishima\nAkio Matsuda"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene20_bgmap2.png" $((40)) $((82))
  renderStringSystemC\
    "Mecano Associates"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene20_bgmap2.png" $((40)) $((152))
  
  #==============
  # page 18
  #==============

  ed_page18_xOffset=-8
  ed_page18_yOffset=0
  
  cp "${sceneBase}/scene21_bgmap1.png" "${sceneBase}/scene21_bgmap2.png"
  renderStringSystemC\
    "Production\n\nMariko Uemura\nMasayuki Shimada"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene21_bgmap2.png" $((192+$ed_page18_xOffset)) $((42+$ed_page18_yOffset))
  # 須永　有三 = yuzo sunaga (e.g. alisa dragoon)
  renderStringSystemC\
    "Facilitation\n\nYuzo Sunaga"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene21_bgmap2.png" $((192+$ed_page18_xOffset)) $((130+$ed_page18_yOffset))
  
  #==============
  # page 19
  #==============

  ed_page19_xOffset=0
  ed_page19_yOffset=0
  
  cp "${sceneBase}/scene22_bgmap1.png" "${sceneBase}/scene22_bgmap2.png"
  # 岡部　利香 = rika okabe?
  # 小川　陽平 = youhei ogawa?
  renderStringSystemC\
    "Sales\n\nToshiyuki Uchida\nRika Okabe\nYumi Sugaya\nYouhei Ogawa"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --left
  overwriteCreditsBlockLeft "${sceneBase}/scene22_bgmap2.png" $((53+$ed_page19_xOffset)) $((53+$ed_page19_yOffset))
  
  #==============
  # page 20
  #==============
  
  blankGrpArea "${sceneBase}/scene23_bgmap4.png" 65 73 177 80
  
  renderStringSystemC\
    "Producers\nYoichi Miyaji\nSatoshi Uesaka\n\nExecutive Producer\nMitsuhiro Mazda"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --center
  overwriteCreditsBlockCenter "${sceneBase}/scene23_bgmap4.png" 65 $((73-8)) 177 $((80+16)) noclear
  
  #==============
  # page 21
  #==============
  
  blankGrpArea "${sceneBase}/scene26_bgmap2.png" 125 41 75 38
  
  renderStringSystemC\
    "Design & Production\nGame Arts"\
    0x22 0x22 0x22\
    0x11 0x11 0x11\
    --center 24
  overwriteCreditsBlockCenter "${sceneBase}/scene26_bgmap2.png" 125 40 75 48 noclear
#  overwriteCreditsBlockCenter "${sceneBase}/scene26_bgmap2.png" 0 40 320 48 noclear
  
}

makeOpCredits "out/scenes/OP_YUMI.DAT"
makeEdCredits "out/scenes/ENDC000A.DAT"
makeEdCredits "out/scenes/ENDC000B.DAT"
makeEdCredits "out/scenes/ENDC000C.DAT"

rm -f "$tempFontFile"
rm -f "$tempGrpFile"