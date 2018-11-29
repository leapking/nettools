#!usr/bin/perl
# 该脚本用于将pstack打印出来的堆栈信息去重
# 参数：pstack输出文件
# 比较时自动忽略函数参数
# Author: leapking, 2018-11-28

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
open(fileout, ">$ARGV[0].".tmp);
open(filein, $ARGV[0]);
while(<filein>)
{
	chomp($_);
	$line = $_;
	$line =~ s/^\s+/\s/g;

	if ($line =~ /^#[0-9]+/)
	{
		print fileout "\n";
	}
	elsif ($line =~ /^Thread/)
	{
		print fileout "\n";
	}
	print fileout "$line";
}
print fileout "\n";
close(filein);
close(fileout);

%StackHash=();
%UniqStackHash=();

# 1. put all stack into array
open(filein, "$ARGV[0].".tmp);
$StackId;
while(<filein>)
{
	chomp($_);

	if (length($_) == 0)
	{
		next;
	}

	if (/^Thread [0-9]+$/)
	{
		my @words = split(/ /, $_);
		$StackId = $words[1];	     #将"Thread 24616"中的24616取到StackId，并作为hash key
		$_ = $_." same:1";
	}

	push(@{$StackHash{$StackId}}, $_);   #将hash值$StackHash{$StackId}直接作为数组使用。将每一行存入数组。
}
close(filein);

# 2. uniq stacks
my $match = 1;
foreach my $stackId (keys %StackHash)
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
foreach my $uniqStackId (keys %UniqStackHash)
{
	#print "$uniqStackId => ${$UniqStackHash{$uniqStackId}}[0]\n";
	#print @{$UniqStackHash{$uniqStackId}};
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
