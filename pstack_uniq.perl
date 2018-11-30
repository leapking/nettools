#!usr/bin/perl
# 该脚本用于将pstack打印出来的堆栈信息去重
# 参数：pstack输出的文件
# 注意：比较时会自动忽略函数参数
# Author: leapking, 2018-11-28

# 统计出现的次数，在数组第一行后面追加"same:$num"
sub updateSameCnt
{
	my ($aref) = @_;
	chomp(@{$aref}[0]);
	my @words = split(/same:/, @{$aref}[0]);

	if (length($words[1]) == 0)
	{
		$words[1] = 1;
	}
	$words[1] = $words[1] + 1;

	@{$aref}[0] = "$words[0]same:$words[1]";
}

# 传递数组时，必须传递对数组的引用。注意对引用的使用。
sub diffStack
{
	my ($aref1, $aref2) = @_;

	#print "arry1: ".@{$aref1};
	#print " arry2: ".@{$aref2};
	#print "\n";

	if (@{$aref1} != @{$aref2})
	{
		return(1);
	}

	for($i = 1; $i < @{$aref1}; $i = $i + 1)
	{
		my $str1 = @{$aref1}[$i];
		my $str2 = @{$aref2}[$i];

		$str1 =~ s/\(.*$//g;
		$str2 =~ s/\(.*$//g;

		#print "------------------------\n";
		#print "$str1\n";
		#print "$str2\n";

		if($str1 cmp $str2)
		{
			return(1);
		}
	}

	updateSameCnt($aref2);
	return(0);
}

# Main
# 0. 移除非正常的换行
$file = "$ARGV[0]";
$tmpfile = "$file.tmp";
open(filein, $file) or die "failed to open \"$file\": $!";
open(fileout, ">$tmpfile") or die "failed to open \"$tmpfile\": $!";
while(<filein>)
{
	chomp($_);
	my $line = $_;
	$line =~ s/^\s+/\s/g;

	if ($line =~ /^#[0-9]+|Thread/)
	{
		print fileout "\n";
	}
	print fileout "$line";
}
close(filein);
close(fileout);

# 1. 将堆栈装入数组，再组织成hash结构
$StackId = 0;
%StackHash = ();
open(filein, $tmpfile);
while(<filein>)
{
	chomp($_);
	my $line = $_;

	if (length($line) == 0)
	{
		next;
	}

	if (/^Thread [0-9]+$/)
	{
		my @words = split(' ', $line);
		$StackId = $words[1];	      #将"Thread 24616"中的24616取到StackId，并作为hash key
		$line = $line." same:1";      #在第一行后面追加"same:1"，标示出现次数
	}

	push(@{$StackHash{$StackId}}, $line); #将hash值$StackHash{$StackId}直接作为数组使用，将每一行存入数组
}
close(filein);
unlink($tmpfile);

# 2. 对堆栈进行去重
$match = 1;
%UniqStackHash = ();
foreach my $stackId (sort {$a<=>$b} keys %StackHash)
{
	$match = 1;
	#print "$stackId => ${$StackHash{$stackId}}[0]\n";

	if (keys %UniqStackHash == 0)
	{
		$UniqStackHash{$stackId} = $StackHash{$stackId};
		next;
	}

	foreach my $uniqStackId (keys %UniqStackHash)
	{
		if (diffStack(\@{$StackHash{$stackId}}, \@{$UniqStackHash{$uniqStackId}}) == 0)
		{
			print "$stackId match as $uniqStackId\n";
			$match = 0;
			last;
		}
	}

	if ($match == 1)
	{
		$UniqStackHash{$stackId} = $StackHash{$stackId};
	}
}

# 3. output uniq stack
foreach my $uniqStackId (sort {$a<=>$b} keys %UniqStackHash)
{
	#print "$uniqStackId => ${$UniqStackHash{$uniqStackId}}[0]\n";
	#print @{$UniqStackHash{$uniqStackId}};

	print "\n";
	foreach $line (@{$UniqStackHash{$uniqStackId}})
	{
		print "$line\n";
	}
}

print "\n";
print "all  stack num: ".keys %StackHash;
print "\n";
print "uniq stack num: ".keys %UniqStackHash;
print "\n";
