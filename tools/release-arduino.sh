#!/bin/bash
output_folder="."
release_name="MMKit-PH"

printf "Copying source files... "
cp -r ../src mmkit_ph
echo "done"

printf "Compressing... "
zip --quiet -r $output_folder/$release_name.zip mmkit_ph
echo "done"

printf "Cleaning up... "
rm -rf mmkit_ph
echo "done"

printf "\nRelease is finished and was stored in '%s'\n" $output_folder
