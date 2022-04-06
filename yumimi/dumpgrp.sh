
set -o errexit

make blackt
make libmd
make sat_8bpp_imgconv
make sat_4bpp_colorize
make sat_4bpp_decolorize
make talldmp_md

mkdir -p "rsrc/orig"

./sat_8bpp_imgconv "rsrc_raw/grp/ymmpuz_title_tiles.bin" "rsrc_raw/grp/ymmpuz_title_tilemap.bin" 40 28 "rsrc/orig/ymmpuz_title.png" -p "rsrc_raw/pal/ymmpuz_title.pal"
./sat_8bpp_imgconv "rsrc_raw/grp/ymmpuz_kabe0_tiles.bin" "rsrc_raw/grp/ymmpuz_kabe0_tilemap.bin" 40 28 "rsrc/orig/ymmpuz_kabe0.png" -p "rsrc_raw/pal/ymmpuz_kabe0.pal"
./sat_8bpp_imgconv "rsrc_raw/grp/ymmpuz_osimai_tiles.bin" "rsrc_raw/grp/ymmpuz_osimai_tilemap.bin" 40 28 "rsrc/orig/ymmpuz_osimai.png" -p "rsrc_raw/pal/ymmpuz_osimai.pal"
./sat_8bpp_imgconv "rsrc_raw/grp/ymmpuz_tease_tiles.bin" "rsrc_raw/grp/ymmpuz_tease_tilemap.bin" 40 28 "rsrc/orig/ymmpuz_tease.png" -p "rsrc_raw/pal/ymmpuz_tease.pal"

./sat_4bpp_colorize "scenes/OP_YUMI.DAT/scene0_bgmap0.png" "rsrc_raw/pal/op_logo_mod.pal" "rsrc/orig/title_logo.png"
#./sat_4bpp_decolorize "rsrc/orig/title_logo.png" "rsrc_raw/pal/op_logo_mod.pal" "test.png"

./talldmp_md "rsrc_raw/grp/interface_block.bin" 2 38 "rsrc/orig/interface.png" -s 0x800
./sat_4bpp_colorize "rsrc/orig/interface.png" "rsrc_raw/pal/interface_mod.pal" "rsrc/orig/interface.png"
# ./sat_4bpp_decolorize "rsrc/orig/interface.png" "rsrc_raw/pal/interface_mod.pal" "rsrc/orig/interface.png"
# ./tallundmp_md "rsrc/orig/interface.png" "test.bin"
# ./talldmp_md "test.bin" 2 38 "test.png"

# ※※　注意　※※
# 「ゆみみみっくす」でデータ
# をセーブするには、
# バックアップＲＡＭ（　　）
# の、「全部消す」（初期化）が
# 必要です。
# STARTキーを押すと、強制的に
# 「全部消す」（初期化）を
# します。
./sat_4bpp_colorize "scenes/C000C.CUT/scene0_bgmap1.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/backup_format.png"
# 本体
./sat_4bpp_colorize "scenes/C000C.CUT/scene0_objmap0.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/backup_format_option0.png"
# カートリッジ
./sat_4bpp_colorize "scenes/C000C.CUT/scene0_objmap1.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/backup_format_option1.png"

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
./sat_4bpp_colorize "scenes/C000B.CUT/scene0_bgmap1.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/backup_spacelow.png"

# 始めから
# 続きから
# リプレイ
# おまけ
#
# 本体
# カートリッジ
./sat_4bpp_colorize "scenes/C000A.CUT/scene0_bgmap1.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/mainmenu_1.png"
# 内蔵
./sat_4bpp_colorize "scenes/C000A.CUT/scene0_bgmap2.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/mainmenu_2.png"
./sat_4bpp_colorize "scenes/C000A.CUT/scene0_bgmap3.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/mainmenu_3.png"
./sat_4bpp_colorize "scenes/C000A.CUT/scene0_bgmap4.png" "rsrc_raw/pal/backup_format.pal" "rsrc/orig/mainmenu_4.png"

# 〒１７１
# 東京都豊島区南池袋
# 　　　　　２−９−９
# 第一池袋ホワイトビル
# 　　　　　　　７Ｆ
# （株）ゲームアーツ宛
./sat_4bpp_colorize "scenes/C021.DAT/scene0_bgmap1.png" "rsrc_raw/pal/advertise.pal" "rsrc/orig/advertise.png"

