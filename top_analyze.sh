#!/usr/bin/sh
# This script is use to analyze result file of cmd: `top`
# it can collect time and other info into csv format file, and draw picture with excel
# Author: leapking, 2019/01/13
# Move to: https://github.com/dbcheck/sys_analyze

topfile="$1"
type="${2:-mem}"
program="${3:-postgres}"

if [ $# -eq 0 ]; then
	echo "Usage: $0 {TopResFile} [mem|cpu|load] [Program]"
	exit 0
elif [ -z "$topfile" ] || [ ! -f $topfile ]; then
	echo "ERROR: '$topfile' is not a valid file"
	exit 1
elif ! grep -c top $topfile >/dev/null; then
	echo "ERROR: '$topfile' is not a valid 'top' result file"
	exit 1
fi

if [ x"$type" = "xmem" ]; then
        echo "DAYS\tTIME\tVIRT\tRES\t%MEM"
        awk 'function m2gb(size){if(size~/m/){gsub(/m/,"",size);size=sprintf("%.1f",size/1024)}return size} \
        BEGIN{info="";days=1;hour=0}/^top/||/'"$program"'\s*$/{if(/^top/){info=$3;gsub(/:.*$/,"",$3);if($3<hour)days++;hour=$3}else if(/'"$program"'\s*$/){info=info"\t"m2gb($5)"\t"m2gb($6)"\t"$10;gsub(/[%g]/,"",info);print days"\t"info;info=""}}' $topfile
elif [ x"$type" = "xcpu" ]; then
        echo "days\ttime\t%user\t%system\t%idle"
        awk -F '[, ]+' 'BEGIN{info="";days=1;hour=0}/^top|Cpu\(s\)/{if(/^top/){info=$3;gsub(/:.*$/,"",$3);if($3<hour)days++;hour=$3}else if(/Cpu\(s\)/){info=info"\t"$2"\t"$3"\t"$5;gsub(/[A-Za-z%,]/,"",info);print days"\t"info;info=""}}' $topfile
elif [ x"$type" = "xload" ]; then
        echo "days\ttime\taverage\tmax\tmin"
        awk 'BEGIN{days=1;hour=0}/^top/{time=$3;gsub(/:.*$/,"",$3);if($3<hour)days++;hour=$3;gsub(/^.*load average:/,"",$0);gsub(/,/,"",$0);print days"\t"time"\t"$1"\t"$2"\t"$3}' $topfile
else
	echo "Unknown type: $type"
fi
