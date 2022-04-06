
set -o errexit

tempFontFile=".fontrender_temp"


function outlineSolidPixels() {
#  convert "$1" \( +clone -channel A -morphology EdgeOut Diamond:1 -negate -threshold 15% -negate +channel +level-colors \#000024 \) -compose DstOver -composite "$2"
  convert "$1" \( +clone -channel A -morphology EdgeOut Square:1 -negate -threshold 15% -negate +channel +level-colors \#000024 \) -compose DstOver -composite "$2"
}

function renderString() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px_outline/" "$tempFontFile" "font/12px_outline/table.tbl" "$1.png"
#  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
  ./fontrender "font/orig/" "$tempFontFile" "font/table.tbl" "$1.png"
}

function renderStringNarrow() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px_outline/" "$tempFontFile" "font/12px_outline/table.tbl" "$1.png"
#  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
  ./fontrender "font/12px/" "$tempFontFile" "font/12px/table.tbl" "$1.png"
  outlineSolidPixels "$1.png" "$1.png"
}

function renderStringScene() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px_outline/" "$tempFontFile" "font/12px_outline/table.tbl" "$1.png"
#  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
  ./fontrender "font/scene/" "$tempFontFile" "font/scene/table.tbl" "$1.png"
}

function renderStringChoice() {
  printf "$2" > $tempFontFile
  ./fontrender "font/choice/" "$tempFontFile" "font/choice/table.tbl" "$1.png"
}

function renderStringSystem() {
  printf "$2" > $tempFontFile
  ./fontrender_multi "font/system/" "$tempFontFile" "font/system/table.tbl" "$1.png"\
    --left --lineh 16\
    --recolor 0xFF 0xFF 0xFF\
    --outline 0x00 0x00 0x00
}



make blackt && make fontrender
make blackt && make fontrender_multi

# renderStringChoice render1 "Save"
# renderStringChoice render2 "Load"

# ※※　注意　※※
# 「ゆみみみっくす」でデータ
# をセーブするには、
# バックアップＲＡＭ（　　）
# の、「全部消す」（初期化）が
# 必要です。
# STARTキーを押すと、強制的に
# 「全部消す」（初期化）を
# します。
renderStringSystem render1 " --- WARNING --- \nIn order to save data for\n{Yumimi Mix,} it is necessary\nto fully clear (format) the\nBackup RAM (            ).\nPressing the START Button\nwill forcibly clear (format)\nit."

# ※※　注意　※※
# バックアップＲＡＭの容量が
# たりません。
# 保存データ管理画面で、
# データの移動か消去を行なっ
# て、５以上の空き容量を作っ
# てくさださい。
# STARTキーを押すと、強制的に
# 「ゆみみみっくす」を開始で
# きますが、SAVEはできません。
#renderStringSystem render1 " --- WARNING --- \nThere is not enough space in\nthe Backup RAM. Please use the\nsave data management screen\nto move or erase data and\ncreate at least 5 empty blocks.\nYou can force {Yumimi Mix} to\nstart by pressing the START\nButton, but you will be\nunable to save."

rm $tempFontFile