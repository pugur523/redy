#/!bin/bash

script_dir=$(cd $(dirname $0) && pwd)
build_scripts_dir=${script_dir}/src/build/scripts
out_bin_dir=${script_dir}/out/build/linux/x86_64/debug/bin

action=$1

shift 1
remain_args=$@

if [ "$action" == "build" ]; then
  ${build_scripts_dir}/build.py $remain_args
elif [ "$action" == "run" ]; then
  ${out_bin_dir}/redy $remain_args
elif [ "$action" == "test" ]; then
  ${out_bin_dir}/redy_test $remain_args
else
  exit 1
fi

exit 0
