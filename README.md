* TestConn.java: a JDBC demo

* TestThread.java: a JDBC demo with multi thread

* gitlog2csv.sh: convert git log to csv file

* glibc_fixbug_pthread_kill.c: test and fix pthread PANIC

* glibc_mem_sbrk.c: check malloc_trim to free memory to system fast

* mtrace_hook.c: trace memory alloc and free with hook

* mtrace_wrap.c: trace memory alloc and free with wrap

* dnstest.c: test dns

* net_ing.c: a simple demo of ping [About ICMP](https://leapking.github.io/2018/03/03/linux_network_ICMP/)

* net_tcp_client.c: CS app of client

* net_tcp_server_block.c: CS app of server

* pstack.sh: use gdb to print stack

* pstack_analyze.perl: uniq or search keyword in pstack result

* top_analyze.sh: extract memory,cpu or load average info to csv file, make chart with Excel

-----------------

## Linux

* [Linux基础](https://linuxtools-rst.readthedocs.io/zh_CN/latest/base/index.html)
* [Shell脚本学习指南](http://c.biancheng.net/shell/)

## Glibc Memory

* [C学习笔记——malloc内存分配](https://blog.csdn.net/a10615/article/details/41063063)
* [C/C++内存分配](https://www.cnblogs.com/hanerfan/p/4545370.html)
* [Linux C/C++ Memory Leak Detection Tool](https://www.cnblogs.com/LittleHann/p/4353145.html)
* [glibc内存分配与回收过程图解](https://blog.csdn.net/think_ycx/article/details/77160286)
* [理解 glibc malloc：主流用户态内存分配器实现原理](https://blog.csdn.net/maokelong95/article/details/51989081)
* [理解 glibc malloc：malloc() 与 free() 原理图解](https://blog.csdn.net/maokelong95/article/details/52006379)
* [Glibc内存管理--ptmalloc2源代码分析（十）](https://mqzhuang.iteye.com/blog/1014291)
* [Glibc内存管理--ptmalloc2源代码分析](https://blog.csdn.net/yazhouren/article/details/38554465)
* [malloc_chunk边界标记法和空间复用](https://blog.csdn.net/sim120/article/details/39373229)
* [glibc malloc 内存管理 分析](https://blog.csdn.net/hshl1214/article/details/7295405)
* [malloc.c](http://gee.cs.oswego.edu/pub/misc/malloc.c)

## Database

* [数据库系统概论](http://chinadb.ruc.edu.cn/home)

### PostgreSQL

* [PostgreSQL教程](https://www.yiibai.com/postgresql/)
* [PostgreSQL 179个场景](http://www.voidcn.com/article/p-smcsfuef-bpa.html)
* [Postgresql内存上下文分析](https://blog.csdn.net/u014539401/article/details/51893272)
* [PostgreSQL 9种索引的原理和应用场景](https://yq.aliyun.com/articles/111793?utm_content=m_24486)
* [深入浅出PostgreSQL B-Tree索引结构](https://github.com/digoal/blog/blob/master/201605/20160528_01.md?spm=a2c4e.11153940.blogcont111793.15.564b5bf0TDmC6a&file=20160528_01.md)
* [PostgreSQL 数据去重方法大全](https://github.com/digoal/blog/blob/master/201706/20170602_01.md)
* [postgresl系列之性能优化](https://blog.csdn.net/qq_31156277/article/details/84669803)

## GDB

* [《100个gdb小技巧》](https://www.kancloud.cn/wizardforcel/gdb-tips-100/146708)
* [Debugging programs with multiple threads](https://ftp.gnu.org/old-gnu/Manuals/gdb/html_node/gdb_24.html)
* [gdb调试info threads只能显示一个进程问题](https://blog.csdn.net/bingqingsuimeng/article/details/50536735)
* [关于gdb “Unable to find libthread_db matching inferior's thread library”的解决办法](https://bbs.csdn.net/topics/391832947)
* [allow accessing thread-local (TLS) variables](https://bugs.chromium.org/p/nativeclient/issues/detail?id=3011)

## Consensus Algorithm

### Raft

* [Raft](https://raft.github.io/)
* [RaftScope](https://github.com/ongardie/raftscope)
* [分布式系统的Raft算法](http://www.jdon.com/artichect/raft.html)
* [Raft一致性算法](http://blog.csdn.net/cszhouwei/article/details/38374603)
* [Raft算法的学习与理解](http://bingotree.cn/?p=611)

### Paxos

* [PaxosLease算法实现——用于Paxos自身选主](http://bingotree.cn/?p=604)
* [PAXOS算法理解](http://bingotree.cn/?p=595)
* [Paxos选举多次决议的算法实现](http://bingotree.cn/?p=607)
* [PAXOS的初次学习](http://bingotree.cn/?p=588)
* [对Raft与Paxos的关系的理解](http://bingotree.cn/?p=614)

## LevelDB & RocksDB

* [LevelDB性能分析和表现](http://database.51cto.com/art/201106/267852.htm)
* [What are the keys used in the blockchain levelDB (ie what are the key:value pairs)?](http://bitcoin.stackexchange.com/questions/28168/what-are-the-keys-used-in-the-blockchain-leveldb-ie-what-are-the-keyvalue-pair)
* [Benchmarking LevelDB vs. RocksDB vs. HyperLevelDB vs. LMDB Performance for InfluxDB](https://influxdata.com/blog/benchmarking-leveldb-vs-rocksdb-vs-hyperleveldb-vs-lmdb-performance-for-influxdb/)
