#/bin/bash

if [ -z "$IDF_PATH" ]
then
  echo "IDF_PATH not defined."
  exit 1
fi

cd $PWD/..

echo $PWD

examples_root=examples
examples_list=("websocket/server" "wifi/station")

build_dir=build/

for example in ${examples_list[@]}
do
  example_dir=$examples_root/$example
  echo "NEW EXAMPLE: $example_dir"
  idf.py -B $build_dir build -C . -DEXTRA_COMPONENT_DIRS=$example_dir -DSDKCONFIG=$example_dir/sdkconfig
  if [ $? -ne 0 ]
  then
    echo "Error compiling \"$example_dir\""
  else
    echo "\"$example_dir\" compiled succefully"
  fi
done