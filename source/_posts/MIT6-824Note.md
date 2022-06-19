---
title: MIT6.824Note
tags:
  - go
  - MIT课程
categories:
  - 分布式系统
date: 2022-06-19 23:19:48
---


今年4月份到现在6月份，抽了许多时间，大体上把MIT 6.824的4个lab给做了。截止目前，除了Lab4的Challenge：`Garbage collection of state`还未成功通过，其余的已成功通过测试。不得不说，一套做下来的收获满满。这里就记录一下自己做实验的思路以及遇到的坑吧。

<!-- more -->

## Lab 1: MapReduce

现在回想起来，Lab 1是一个很简单的小任务。要求我们实现一个MapReduce，在`src/main/mrsequential.go`中展示了任务的线性实现顺序。在开始实验前，首先把*Rules*以及*Hints*先理解清楚。其实对于每一个实验*Rules*和*Hints*都是很好的提示。对于Lab 1我并未去实现其的`No-credit challenge exercises`。

这里，由于我是初学go，Lab 1也是我第一个go项目的使用，所以Lab 1的实现我参考了他人的实现。主要的思路是：Worker不断向Coordinator发送请求索取一个任务，每当Worker完成一个任务向Coordinator报告后并请求下一个任务；对于Coordinator，其需要维护整个当前阶段（Map、Reduce阶段）中任务队列内的所有任务状态，Coordinator内部有一个定时线程检查任务队列中所有任务的状态，如超时检查等。当所有任务结束后，对于Worker的请求，Coordinator通知Worker退出即可。这里需要讨论的空间是Worker对于完成任务的处理，这里我简单的由Worker将临时文件重命名为正式的结果文件，再向Coordinator汇报成功。其实这么做是不妥当的，但是在通过了100次Test后我就暂时不管这个了。

Lab 1所踩的坑基本都是集中在`Go`语言的语法上的问题，比如通道引起的线程阻塞等。这个Lab基本是一个牛刀小试，写完Lab 1对于`Go`的语法也有了一定的了解。

## Lab 2: Raft

Lab 2应该是这门课程的重头戏了，其要实现一个`Raft`。Lab 2是之后的实验Lab 3以及Lab 4的基础。如果要打算继续做下去，Lab 2的正确性将成为往后实验顺利进行的一块重要的基石。
在开始实验之前，强烈建议阅读：

* [Raft论文](https://raft.github.io/raft.pdf)至少要阅读到第5.4节
* [Raft的流程演示](http://thesecretlivesofdata.com/raft/)
* [Raft Visualization](https://raft.github.io/)

同时对于Lab 2的每一部分的`Hint`都应该认真阅读，在实验过程中[Raft论文](https://raft.github.io/raft.pdf)以及[Students' Guide to Raft](https://thesquareplanet.com/blog/students-guide-to-raft/)都能有极大的帮助。对于`Raft`论文中的规定，只要实验要求中未作出特殊说明，请一定要百分百遵守。一般而言，遵守[Students' Guide to Raft](https://thesquareplanet.com/blog/students-guide-to-raft/)的建议对实验的帮助非常之大。

我的心跳间隔为：80 ms，选举时间间隔为：150-300 ms。

一个对每一个Part都非常有帮助的tips：对于一个要长期执行的For循环，请一定要加上检查`killed()`。

Raft的Debug是十分困难的，所以对于程序运行的日志的输出一定要做好规范，针对每一个函数的出入口都要有一个日志的打印。这里推荐一个非常棒的[一个非常棒的并行调试工具](https://blog.josejg.com/debugging-pretty/)

### Part 2A: leader election

这部分主要是实现`Raft`中关于Leader的选举以及选举成功后发送Heartbeat的流程的实现。实现的主体是`Figure2`中的`RequestVote RPC`以及`AppendEntries RPC`，这里对于涉及到日志的部分，默认为空或0即可。

涉及Leader选举的部分在论文中的5.2以及5.4.1有提及，就Part 2A而言，实现5.2中涉及的即可。整体思路为：Raft启动后，开启一个循环的线程，每次醒来时首先检查是否为Leader，否则继续检查Raft在自己沉睡时是否有收到过Heartbeat，否则开启选举，然后继续沉睡随机时间。对于选举，遵守`Raft`的要求去实现即可，对于其的实现，我参照了官方课程中的Lecture 5 - Go, Thread and Raft，采用了条件变量等待投票过半成为Leader。

对于Heartbeat的发送，实现思路是：Raft启动后，开启一个定时的线程，检查自己是否为Leader，是则向其他所有节点发送`AppendEntries RPC`。这里对于Part 2A而言Entries应为空。

这里踩的坑主要有：在发送RPC时采用异步发送；发送RPC请求时，不要持有锁，否则当两两互相发送RPC请求时可能会造成死锁；选举计时器的沉睡时间要保证随机；不要向自己发送RPC请求。

### Part 2B: log

Part 2B要求实现日志的同步，在这里你需要实现论文`Figure2`中的全部内容。如果你顺利的实现了Part 2A，那么Part 2B中的日志的同步应该不算太难。对Part 2B的日志同步，我采用了和Part 2A选举的实现一样的框架，异步向每一个Follower发送心跳即发送`AppendEntries RPC`，使用条件变量等待半数以上的节点同意日志的同步，然后选取合适的index，更变`commitIndex`。对于日志的Apply，我采用的方案是，当`commitIndex`改变时，立刻判断其与`lastApplied`的大小，如果`commitIndex`大小大于`lastApplied`，则开启一个线程将日志中的`[lastApplied+1:commitIndex+1]`提交，也就是传入`applyCh`中，这里需要注意的是传入`applyCh`时，线程不应该持有锁。

在Part 2B中，选举也多了限制，除了Part 2A实现的限制条件外，我们还要实现论文中5.4.1提及的`Election restriction`。

这里说明一下在做Part 2B时的一点小疑惑：
在`AppendEntries RPC`提及
> Reply false if log doesn’t contain an entry at prevLogIndex
whose term matches prevLogTerm (§5.3)

以及
> If an existing entry conflicts with a new one (same index
but different terms), delete the existing entry and all that
follow it (§5.3)

基本上，只要**百分之百**的遵照`Raft`论文的要求，Part 2B应该能够成功通过，不过论文有一个小细节可能没有提及，关于重置`election timeout`的说明，在实验中，我基本上遵从收到正确的心跳重置、收到更高的`Term`重置、授予投票时重置，这里有一个与这些操作高度相关的操作为：更变自己的状态为Follower，所以我在实现时，将更变状态为Follower与重置`election timeout`绑定在一起。这里有一个隐藏得很深的坑：在收到投票请求时，在其通过了`Term`检查后，我会将自己转为Follower，之后继续判断是否授予投票。这里如果将更变状态为Follower与重置`election timeout`绑定，导致的错误是无法选举产生Leader。这里一定要将更变状态为Follower与重置`election timeout`解绑，只有在授予投票后，才重置`election timeout`，否则可能自己永远无法触发选举，只能给他人投票。

这里可能还会存在的小问题可能是关于`matchIndex`和`nextIndex`的计算，这里我的建议是：

```go
matchIndex = args.PrevLogIndex + len(args.Entries)
nextIndex = matchIndex + 1
```

其余的case基本上在Students' Guide to Raft都有提到。

Part 2B基本可以算是整个Lab 2最难的一部分，而且其的正确实现关系到后面的每一个实验。在Part 2B中即使通过了多次测试也不要太过于高兴，到Part 2C可能问题就会暴露出来。（我进入Part 2C前，跑了Part 2B两千次不出错，结果到了Part 2C还是发现了一个很小很小的错误。）

### Part 2C: persistence

要求实现持久化，即实现`persist()`以及`readPersist()`即可。每一次Raft的启动都调用一次`readPersist()`。而对于`persist()`的调用，只要有涉及论文中提及的持久化变量的变换就调用`persist()`即可。对于Part 2C我们还需要作出一些优化，也就是论文中提交的可能无需作出的优化：加速`nextIndex`的同步，这里具体参考[Students' Guide to Raft](https://thesquareplanet.com/blog/students-guide-to-raft/)。

这里的主要难点是在于对论文中`Figure8`的理解，我的理解是：commit的日志中如果不存在当前term的项，就无法提交。也就是：
>If there exists an N such that N > commitIndex, a majority
of matchIndex[i] ≥ N, and log[N].term == currentTerm:
set commitIndex = N

对于Part 2C主要的失败点是`TestFigure8Unreliable2C()`。这里的调试花了我不少的时间，最终的问题锁定在了一个很低级的问题上在对获取`N`时我对`matchIndex`数组进行了排序，忘记copy了，直接采用赋值排序了，导致了`matchIndex`全乱了。对`Go`语言的不熟悉。
错误类似于

```go
commits := rf.leaderStateRecord.matchIndex
sort.Ints(commits)
```

很惊奇的是，这样一个低级的错误居然过了2B。

### Part 2D: log compaction

先提及一点是`CondInstallSnapshot()`这个函数，Lab的要求中说不建议实现，直接返回true。
对于Part 2D，最重要的是梳理其过程：
首先是上层发现log太大了，就会调用`Snapshot()`函数进行日志压缩。Lab 2中由Tester调用。关于这一点我们只需实现`Snapshot()`使其能够对日志进行压缩保存即可。
其次是`InstallSnapshot RPC`请求，这个是由我们去调用的，当Leader发现一个Follower落后太多时，就会将自己的快照发送给Follower。论文中对这个实现有具体的说明。当Follwer收到Leader的`InstallSnapshot RPC`请求后，判断Term是否满足，若满足而后正确的流程应该是将`ApplyMsg`设置为快照有效，并将此次快照信息通过`applyCh`传给上层，上层接收到这个信息后，会调用`CondInstallSnapshot()`由Raft去判断是否应该安装这个快照，Raft接受安装后`CondInstallSnapshot()`返回true，上层也会进行快照安装。在Part 2D中，我将问题简化，`CondInstallSnapshot()`不做实现，直接返回true，Follwer收到Leader的`InstallSnapshot RPC`请求后，自行判断是否接受这个快照的安装（具体的判断由Term以及Index决定），以及在接受安装请求后将此次快照信息通过`applyCh`传给上层，注意向`applyCh`传递时，应该单独`go func {}()`去传递。（这里的化简对于Lab 2而言是可以接受的，到了Lab 3请一定要遵守上述的正确流程。）

Part 2D由于涉及日志的压缩，对于log的处理就不能向传统的`Go silce`一般处理了。这里我的处理是重新封装关于log的`Get`、`Set`、`Append`函数，在不更变太多代码的情况下，顺利的通过了Part 2D的测试。这里有一个很好的技巧是：将`snapshot`的`includeTerm`和`includeIndex`存储在`rf.log[0]`内，对于Part 2D的实现有不少的方便之处。

### Lab 2小总结

我在整个Lab 2完成后，跑了2k次的Test，无一错误。不过在重复跑了很多遍的2k次的完整的Test后，仍旧是出现了一次错误，在查看日志后发现发现，如果当前任期内一直没有命令到来，会导致无法提交之前Term的日志，这会导致`TestFigure8Unreliable2C()`的失败，因为这个测试要求在网络恢复的10秒后，所有的Server要达到一致。

## Lab 3: Fault-tolerant Key/Value Service

Lab 3基于Lab 2实现的`Raft`构建一个可容错的K/V服务。关于这一部分的一些参考可以阅读`Raft`作者的[博士论文](https://web.stanford.edu/~ouster/cgi-bin/papers/OngaroPhD.pdf)，具体的流程说明在第六章，同时对于其要求的线性一致性，可以参考官方课程Lecture 7的后一部分以及Lecture 8的前一部分。值得说明的，你无需一定要按照博士论文中所提到的流程具体实现，这是一个思路的参考。这张[Raft交互图](https://pdos.csail.mit.edu/6.824/notes/raft_diagram.pdf)也是一个参考。Lab 3的实现还是蛮自由的，这里就简单介绍一下我的思路吧。

本次的实验中，一些值得注意的事项有：

1. 数据库由一个`map`来代替，数据全部存储在内存中。
2. 单个客户端的请求是串行的。
3. 所有操作均由Leader进行回应。

Lab 3的重点我认为应该是对线性一致性的保证，这一点我们可以把所有的读写操作加入Raft日志以及请求去重保证。
对于客户端请求的去重，结合到本次实验的具体情况单个客户端不支持并行的请求，对每个客户端在发送请求时，除了携带最基本的参数外，还应该携带客户端的`clientId`和命令序号`sequenceNum`的标识。`clientId`在clerk生成时由`nrand()`生成，`int64`保证在本次实验中的`clientId`的唯一性。命令序号`sequenceNum`初始值为0，一个clerk在发送一个命令请求时自动将`sequenceNum`增加1。由于客户端的执行是串行，`clientId`和`sequenceNum`保证了请求的唯一性。而对于server端，在接收到client的读写请求后，要先经过去重判断，才能加入`rf.Start()`。
server端应该针对每一个`clientId`存储一个最新的处理结果，对重复的请求，直接回复这一个存储最新的处理结果。
同时在应用日志到状态机内时，也要有一次去重判断。

server端在启动时，应该开启一个线程监听`applyCh`，以便将命令应用到状态机中。

整个server端与client的交互流程：大致为clinet向Leader发送RPC请求，server端在Leader状态时接收RPC请求，并将此RPC请求挂起，等待`applyCh`中该命令应用到状态机后回复client。

### Part B: Key/value service with snapshots

将Part 2D中忽略的`CondInstallSnapshot()`实现后，需要我们实现的大致会有两个函数创建快照`snapshot()`以及读取快照`readSnapshot()`，在合适的地方执行这两个函数即可。

### 我的实现的一些小瑕疵

由于我在实现Lab 2时，将日志的提交与心跳的广播绑定到一起。我的思路是，心跳会更新`commitIndex`，只有`commitIndex`更新后才会提交日志，导致server对于每个操作的执行时间与心跳频率高度相关。在Lab 3中有一测试为`TestSpeed3A()`，其要求server处理每个命令的最长平均耗时不超过33ms，由于我的心跳频率为80ms，所以我的server处理每个命令的平均耗时为80ms，无法通过`TestSpeed3A()`。其他测试均测试1k次以上，无一错误。

## Lab 4: Sharded Key/Value Service

Lab 4要求实现一个`Multi-Raft`。我认为Lab 4与Lab 2的难度不相上下，Lab 2的难度可能是在各种可能错误的case的情况的考虑，而Lab 4的难度主要是来源于对于整个模式的设计。首先，必须要梳理整个实验的思路。实验要求将数据通过`key2shard()`将数据按键分布在不同的`shard`中。一个集群中运行着几个Raft服务组。一个Raft服务组管理几个`shard`。在本次Lab中，我们有`NShard = 10`个`shard`以及三个Raft服务组。这样就解决了单个Raft系统的性能受制于单机的性能（读写请求都由Leader节点处理）的问题。

### Part A: The Shard controller

这是一个`easy`等级的任务，主要是要实现一个分区控制器，指定哪一组的Raft服务`Servers`管理哪几块分区`Shard`。`Shard[i]`表示第`i`块分区由哪一个Raft服务组管理，辨别Raft服务组通过非0值`Gid`辨别，`Groups[gid]`表示`gid`Raft组中所有的Servers。
主要任务是，当`Group`加入或者离开集群后如何将分区均衡得分配到新的`Groups`中的服务组去，要求是尽量少的Raft组间的数据交换。这一块的思路就是削多补少。

这里有一个小细节，`Hint`也有所提及：
>In Go, map iteration order is not deterministic.

这里带来的影响是在重平衡的削多补少时，仅仅以某一种情况为例子：可能在同一个`ConfigNum`下，出现两种配置：

```go
Shard = [100,100,100,101,101,101]
Shard = [101,101,101,100,100,100]
```

解决的方法，与自己的具体实现有关。我的解决方案是，先对key即`gid`进行排序，再对`gid`拥有的`shard`数进行排序。

### Part B: Sharded Key/Value Server

这一块，我前前后后设计了三种方案，第一种方案是，对某一个Raft强制其所有分区共用一个Config配置，这是一个过于保守的方案，而且对于后面Challenge的实现不利，最后被抛弃。第二个方案是，对某一个Raft，10个分区允许其有不同的Config，但是这在实现是对这个10个分区，每一个分区都进行一次Query操作，很费时间，经常性丢失Leader。最终采用了折中的方案：kv维护一个config，这是10个分区中最低ConfigNum的配置，在Client端保证，发送请求时一定按照最新的配置发送请求，这样了保证Shard的独立性，也减少了Query操作次数。

关于Shard数据的迁移，最开始采用`Pull`模式，即由`Join`操作的Raft服务组向旧的分区所有者请求数据。但是在`TestJoinLeave`测试中，当某个`Group`离去时，就一直请求数据RPC失败。在查阅了资料后发现原来还有`Push`模式，由旧的分区所有者向新的分区所有者推送分区。体感这是一个很好的模式，减少了RPC交互次数，而且在Push成功后可以直接清除该分区中的数据，应该可以直接实现Challenge1（但是我始终是过不了，总是提示我too big）。

具体的其他细节，都应该是因人而异。一些需要注意的点是：分区数据的迁移的操作也不要忘了加入日志中去，同时要注意该项操作需要去重处理，以及注意Think about how the shardkv client and server should deal with ErrWrongGroup.
