#!/bin/bash
output_folder="."
release_name="MMKit-PH-doc"
release_folder="MMKit-doc"

printf "Copying doc files... "
mkdir $release_folder
cp -r ../doc/html $release_folder/.
cp ../doc/MMKit_layout.A8.pdf $release_folder/.
cp ../doc/MMKit_schematic.A6.pdf $release_folder/.
echo "done"

printf "Compressing... "
zip --quiet -r $output_folder/$release_name.zip $release_folder
echo "done"

printf "Cleaning up... "
rm -rf $release_folder
echo "done"

printf "\nDocumentation release is finished and was stored in '%s'\n" $output_folder
