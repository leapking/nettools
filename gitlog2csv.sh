#!/usr/bin/sh

ORIGIN="origin1 origin2 origin3"
OPT_GREP="con1, con2, con3"
OPT_GREP_V="COPYRIGHT README"
TIME_START="2018-01-01 00:00:00"
TIME_END="2018-11-01 00:00:00"
OUT_FILE="gitlog.txt"

## get git log to text
:>$OUT_FILE
for ori in $ORIGIN
do
    echo $ori;

    #%h:  abbreviated commit hash
    #%an: author name
    #%ad: author date (format respects --date= option)
    #%s:  subject
    #%B:  raw body (unwrapped subject and body)
    git log origin/$ori --name-only --date=iso --since="$TIME_START" --before="$TIME_END" --pretty=format:'"CCCCC"%h"|"%an"|"%ad"|"%s"|"%B"|"' >>$OUT_FILE
done

## convert \n to \a
tr "\n" "\a" <$OUT_FILE >${OUT_FILE}.tmp #用\a代替换行，以后在Excel里再替换成真正的换行
sed -i "s/CCCCC/\n/g" ${OUT_FILE}.tmp    #将CCCCC替换成换行
sed -i '/^"$/d' ${OUT_FILE}.tmp          #去掉只有双引号的行
sed -i 's/\a\a/\a/g' ${OUT_FILE}.tmp     #去掉重复的\a
sed -i 's/"\a/"/g' ${OUT_FILE}.tmp       #去掉开始的\a
sed -i 's/\a"/"/g' ${OUT_FILE}.tmp       #去掉结束的\a

## grep
grep -E "$OPT_GREP" ${OUT_FILE}.tmp >$OUT_FILE
grep -v "$OPT_GREP_V" $OUT_FILE >${OUT_FILE}.tmp
mv ${OUT_FILE}.tmp $OUT_FILE

## uniq
cat >uniq.pl <<EOF
#/bin/perl

open (IN, "$OUT_FILE");
while (my \$line = <IN>)
{
    my @key = split(/\|/, \$line);
    \$hash{\$key[0]} = \$line;
}
close \$IN;

open (OUT, ">$OUT_FILE");
foreach my \$key (keys %hash)
{
    print OUT "\$hash{\$key}";
}
close \$OUT;
print kyes %hash;
EOF

perl uniq.pl
rm uniq.pl
