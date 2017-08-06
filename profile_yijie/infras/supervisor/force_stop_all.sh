#!/bin/bash

supvr_config_d=/data/release/server/infras/supervisor/supvr.conf.d

for file in `ls ${supvr_config_d}`
do
    if [ "${file}" != "supvr_event_callback.bin.supvr.conf" ];then
        real_file_dir=$(dirname `readlink -f ${supvr_config_d}/${file}`)
        echo ${real_file_dir}
        ${real_file_dir}/admin.sh force_stop
    fi
done

