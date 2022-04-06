
set -o errexit

make libmd
make ymm_scriptgen

mkdir -p script_raw

./ymm_scriptgen "scenes_raw" "script_raw/" "sat"
