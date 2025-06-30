#! /usr/bin/env bash

set -e

VERSION="1.0.0"
RELEASEDIR="release_tmp"
RELEASENAME="bibneat-v$VERSION"

mkdir -p $RELEASEDIR

declare -a zip_files=(
    "LICENSE"
    "INSTALL.md" 
    "CMakeLists.txt" 
    "CMakePresets.json" 
    "vcpkg.json" 
    "CMakeUserPresets.json.example" 
)

declare -a zip_dirs=(
    "src"
)

for f in "${zip_files[@]}"
do
    cp $f $RELEASEDIR
done

for d in "${zip_dirs[@]}"
do
    cp -r $d $RELEASEDIR
done

(cd $RELEASEDIR && zip -r -q -9 "../releases/$RELEASENAME.zip" . -x **/.DS_Store)

zip -T releases/$RELEASENAME.zip
zipinfo -t releases/$RELEASENAME.zip

(cd $RELEASEDIR && tar -czf ../releases/$RELEASENAME.tar.gz --no-mac-metadata --totals --exclude .DS_Store .)

echo "Found `tar -tzf releases/$RELEASENAME.tar.gz | wc -l` files in tar.gz archive"

rm -rf $RELEASEDIR