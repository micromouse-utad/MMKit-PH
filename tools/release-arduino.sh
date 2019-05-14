#!/bin/bash
output_folder="."
release_name="MMKit-PH-arduino"

printf "Copying source files... "
cp -r ../src mmkit_ph
echo "done"

printf "Refactoring code... "
mkdir mmkit_ph/src
mv mmkit_ph/hardware mmkit_ph/src/hardware
sed -i 's$#include "hardware/$#include "src/hardware/$g' mmkit_ph/*.*
sed -i 's$#include "../$#include "../../$g' mmkit_ph/src/hardware/*.*
echo "done"

printf "Compressing... "
zip --quiet -r $output_folder/$release_name.zip mmkit_ph
echo "done"

printf "Cleaning up... "
rm -rf mmkit_ph
echo "done"

printf "\nArduino release is finished and was stored in '%s'\n" $output_folder
