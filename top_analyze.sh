#!/usr/bin/sh
# This script is use to analyze result file of cmd: `top`
# it can collect time and other info into csv format file, and draw picture with excel
# Author: leapking, 2019/01/13

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
        echo "TIME\tVIRT\tRES\t%MEM"
        awk 'function m2gb(size){if(size~/m/){gsub(/m/,"",size);size=sprintf("%.1f",size/1024)}return size} \
        BEGIN{info=""}/^top/||/'"$program"'\s*$/{if(/^top/){info=$3;}else if(/'"$program"'/){info=info"\t"m2gb($5)"\t"m2gb($6)"\t"$10;gsub(/[%g]/,"",info);print info;info=""}}' $topfile
elif [ x"$type" = "xcpu" ]; then
        echo "time\t%user\t%system\t%idle"
        awk -F '[, ]+' 'BEGIN{info=""}/^top|Cpu\(s\)/{if($1~/top/){info=$3;}else if(/Cpu\(s\)/){info=info"\t"$2"\t"$3"\t"$5;gsub(/[A-Za-z%,]/,"",info);print info;info=""}}' $topfile
elif [ x"$type" = "xload" ]; then
        echo "time\taverage\tmax\tmin"
        awk '/^top/{time=$3;gsub(/^.*load average:/,"",$0);gsub(/,/,"",$0);print time"\t"$1"\t"$2"\t"$3}' $topfile
else
	echo "Unknown type: $type"
fi
