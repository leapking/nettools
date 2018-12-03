#!usr/bin/perl
# 该脚本用于将pstack打印出来的堆栈信息去重
# 参数：pstack输出的文件
# 注意：比较时会自动忽略函数参数
# Author: leapking, 2018-11-28

# 统计出现的次数
# 在数组第一行后面追加"@Same:$num"用于记录重复次数
sub updateSameCnt
{
	my ($aref) = @_;
	chomp(@{$aref}[0]);

	if (@{$aref}[0] !~ /\@Same:/)
	{
		@{$aref}[0] = "@{$aref}[0] \@Same:1\n";
		return (0);
	}

	my @words = split(/\@Same:/, @{$aref}[0]);
	$words[1] = $words[1] + 1;
	@{$aref}[0] = "$words[0]\@Same:$words[1]\n";
}

# 比较两个堆栈信息是否一样
# 传递数组时，必须传递对数组的引用。注意对引用的使用。
sub diffStack
{
	my ($aref1, $aref2) = @_;

	if (@{$aref1} != @{$aref2})
	{
		return(1);
	}

	for($i = 1; $i < @{$aref1}; $i = $i + 1)
	{
		# 将(后面都内容都去掉后比较，忽略函数参数
		my @stack1 = split(/\(/, @{$aref1}[$i]);
		my @stack2 = split(/\(/, @{$aref2}[$i]);

		if($stack1[0] cmp $stack2[0])
		{
			return(1);
		}
	}
	return(0);
}

# 对堆栈进行去重
sub uniqAllStack()
{
	my $match = 1;
	my %uniqStack = ();

	print "========== Uniq All Stack ==========\n";
	foreach my $stackId (sort {$a<=>$b} keys %StackHash)
	{
		$match = 1;
		foreach my $uniqStackId (keys %uniqStack)
		{
			if (diffStack(\@{$StackHash{$stackId}}, \@{$uniqStack{$uniqStackId}}) == 0)
			{
				print "$stackId match as $uniqStackId\n";
				updateSameCnt(\@{$uniqStack{$uniqStackId}});
				$match = 0;
				last;
			}
		}

		if ($match == 1)
		{
			$uniqStack{$stackId} = $StackHash{$stackId};
			updateSameCnt(\@{$uniqStack{$stackId}});
		}
	}

	# output uniq stack
	foreach my $uniqStackId (sort {$a<=>$b} keys %uniqStack)
	{
		print "\n";
		print @{$uniqStack{$uniqStackId}};
	}
	print "\nuniq    stack num: ".keys(%uniqStack)."\n";
}

# 查询含某个关键字的堆栈
sub searchAllStack
{
	my $keyword = @_[0];
	my $matchCnt = 0;

	if (length($keyword) == 0)
	{
		return (1);
	}

	print "\n========== Search All Stack: $keyword ==========\n";
	foreach my $stackId (sort {$a<=>$b} keys %StackHash)
	{
		# search keyword from stack array
		if (grep(/$keyword/, @{$StackHash{$stackId}}) == 0)
		{
			next;
		}

		print "\n";
		print @{$StackHash{$stackId}};
		$matchCnt = $matchCnt + 1;
	}
	print "\nmatched stack num: $matchCnt\n";
}

# Main
if (@ARGV == 0 || ! -e "$ARGV[0]")
{
	print "Usage: $0 {StackFileName} [keyword]\n";
	print "uniq or search stackfile\n";
	exit(0);
}

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
	if ($line =~ /^#[0-9]+|Thread/) #判断何时需要换行
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
open(filein, $tmpfile) or die "failed to open \"$tmpfile\": $!";
while(<filein>)
{
	chomp($_);
	my $line = $_;

	if (length($line) == 0)
	{
		next;
	}

	if (/^Thread [0-9]+/)
	{
		my @words = split(' ', $line);
		$StackId = $words[1];			#将"Thread 24616"中的24616取到StackId，并作为hash key
	}

	push(@{$StackHash{$StackId}}, "$line\n");	#将hash值$StackHash{$StackId}直接作为数组使用，将每一行存入数组
}
close(filein);
unlink($tmpfile);

# 2. dispatch
uniqAllStack();
searchAllStack($ARGV[1]);
print "all     stack num: ".keys(%StackHash)."\n";
