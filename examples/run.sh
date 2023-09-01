#/bin/bash

examples_list=("log"
               "uc/adc_stream"
               "uc/gpio"
               "uc/pulse_counter"
               "peripherals/water_flow_sensor"
               "wave"
               "websocket/server"
               "wifi/station"
               "wifi/ap"
               "http/server"
               "http/server_cb"
               "http/login_wifi"
               "http/https_server"
               "echo_http_ws_serial")

# Set redirect
exec 3> /dev/null
# exec 3>&1

target=esp32

root=''
do_fullclean=0

if [ $# -lt 1 -o $# -gt 2 ]; then
  echo "Root path not defined"
  exit 1
fi

if [ $# -eq 2 ]; then
  do_fullclean=$2
fi

root=$1
cd $root
if [ $? -ne 0 ]; then
  echo "Root path provided not valid"
  exit 2
fi

if [ -z "$IDF_PATH" ]; then
  echo "'IDF_PATH' not defined."
  exit 3
fi

now() {
  date +"%Y/%m/%d %H:%M:%S"
}

instant() {
  date +%s%N
}

exec_time() {
  local -n arr1=$1
  shift
  local start=$(instant)
  $@
  local ret1=$?
  local end=$(instant)
  arr1=($ret1 $(($end - $start)))
}

nano_to_seconds() {
  local time=$1
  local nano=$(($time % 1000))
  time=$(($time/1000))
  local micro=$((time % 1000))
  time=$(($time/1000))
  local mili=$((time % 1000))
  local sec=$(($time/1000))
  printf "%d.%03d.%03d.%03d" $sec $mili $micro $nano
}

size_file() {
  stat --printf="%s" $1
}

print_header() {
  cat <<-EOF
----------------------------RUNNING EXAMPLES-----------------------------
Date: $(now)
IDF_Version: $(idf.py --version)
IDF_PATH: ${IDF_PATH}
Command: $@
Fullclean: $do_fullclean
-------------------------------------------------------------------------
EOF
}

printf_format() {
  local op_name=$1
  local line='...........................................'
  printf "%s %s" "$op_name" "${line:${#op_name}}"
}

print_table_header() {
  echo "+-----+-------------------------------------+----------+------------------+---------+"
  echo "|  Op |             project name            |  status  | time(s.ms.us.ns) | size(b) |"
  echo "+-----+-------------------------------------+----------+------------------+---------+"
}

print_success() {
  echo "  SUCCESS  |  $(nano_to_seconds $1)  | $(size_file $examples_root/$example/build/$(basename $2).bin)"
}

components=$PWD/components
examples_root=$PWD/examples

build() {
  local -n ans=$1
  exec_time ans idf.py build -C $examples_root/$2 -DEXTRA_COMPONENT_DIRS=$components >&3
}

clean() {
  if [ $do_fullclean -ne 1 ]; then
    idf.py clean -C $examples_root/$1 >&3
  else
    idf.py fullclean -C $examples_root/$1 >&3
  fi
}

clean_build_all() {
  local success=()
  local fail=()

  print_table_header

  for example in ${examples_list[@]}; do
    # idf.py set-target -C $examples_root/$example $target >&3
    if [ $1 -eq 1 -o $1 -eq 3 ]; then
      printf_format "|CLEAN| $example"

      clean $example
      if [ $? -ne 0 ]; then
        echo "  FAILED"
      else
        echo "  SUCCESS"
      fi
    fi
    if [ $1 -eq 2 -o $1 -eq 3 ]; then
      printf_format "|BUILD| $example"
      local ret;
      build ret $example
      if [ ${ret[0]} -ne 0 ]; then
        echo "  FAILED"
        fail+=($example)
      else
        print_success ${ret[1]} $example
        success+=($example)
      fi
    fi
  done
  echo "------------------------------------------------------------"
  echo "Success: " ${success[@]}
  echo "Fail   : " ${fail[@]}
}

# idf.py set-target $target >&3

print_header $0 $@
clean_build_all 3
