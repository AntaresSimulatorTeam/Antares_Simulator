#!/bin/bash

if [ $# -neq "2" ]; then
  echo "$0 <build dir> <output name>"
fi

build_dir=$1
output_name=$2

echo "Starting recording in ${build_dir}"
ClangBuildAnalyzer --stop ${build_dir}
ClangBuildAnalyzer --start ${build_dir}

echo "Running build ..."
cmake --build ${build_dir} --target clean
time cmake --build ${build_dir} -j 18
echo "Build finished"

binary_file_name="${output_name}.bin"
analysis_file_name="${output_name}.txt"
echo "Stopping recording and generating output file ${analysis_file_name}"
ClangBuildAnalyzer --stop ${build_dir} ${binary_file_name}
ClangBuildAnalyzer --analyze ${binary_file_name} > ${analysis_file_name}
