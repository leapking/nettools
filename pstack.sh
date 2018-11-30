# use gdb to print stack

#!/bin/sh

PID=$1
SYMBOLFILE=$2

if [ x"$PID" == x -o ! -d /proc/$PID/task ]; then
        echo "Usage: $0 {PID} [symbol]"
        echo "PID $PID is must exist"
        exit 1
fi

if [ x"$SYMBOLFILE" != x -a -e $SYMBOLFILE ]; then
        echo "Usage: $0 {PID} [symbol]"
        echo "symbol file: $SYMBOLFILE is not exist"
        exit 1
fi

for thread in $(ls /proc/$PID/task)
do
echo "Thread: $thread"
if [ x"$SYMBOLFILE" != x ]; then
gdb -p $thread <<EOF 2>&1 |
set pagination off
symbol-file $SYMBOLFILE
bt
detach
quit
EOF
sed -n -e 's/(gdb) //' -e '/^#/p' -e '/^  /p'
else
gdb -p $thread <<EOF 2>&1 |
set pagination off
bt
detach
quit
EOF
sed -n -e 's/(gdb) //' -e '/^#/p' -e '/^  /p'
fi
done
