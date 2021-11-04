---
title: KMP算法
tags:
  - 字符串
categories:
  - 数据结构
mathjax: true
date: 2021-07-21 22:58:58
---



 **KMP算法目的:**快速从主串中匹配出与模式串相同的子串

在梳理之前，先给几个约定吧。

* 语言：C++

* 字符串s，其子串的表示方法s[i]~[j]

* 默认已经了解KMP算法中的一些基本概念，如主串，模式串等。
* 证明并不严谨，只是呢，个人感觉如果知道了证明，就能够掌握住KMP算法的精髓
* 本文的举例子都挺辣眼睛的，就是方便理解，可不看，重要的是证明。

## Brute-Force

在说明KMP算法前，不得不先了解一下暴力算法

```c++
int Index(const string &mainString, const string &modeString)
{
    if (modeString == "")
        return 0;
    if (mainString.length() < modeString.length())
        return -1;
    
    int i = 0, j = 0, k = 0;
    while (i < mainString.length() && j < modeString.length())
    {
        if (mainString[i] == modeString[j])
        {
            ++i;
            ++j;
        }
        else
        {
            ++k;
            i = k;
            j = 0;
        }
    }
    if (j >= modeString.length())
        return k;
    else
        return -1;
}
```

每一次匹配不成功，我们都把主串游标和模式串游标移动回来。时间复杂度为$O(mn)$

## KMP算法的理解

首先明白暴力算法的过程与原理，先直接上KMP算法的代码

```c++
int Index(const string &mainString, const string &modeString)
    {
        if (modeString == "")
        	return 0;
    	if (mainString.length() < modeString.length())
        	return -1;
        vector<int> next = GetNext(needle);

        int modeStrLen = modeString.length(), mainStrLen = mainString.length(); //由于length()返回的是一个无符号的整形数，因此切记，如果在表达式中混用了带符号数和无符号数将可能产生意想不到的结果。例如，假设n是一个具有负值的int，则表达式s.length() < n的判断结果几乎肯定是true
        int modeIndex = 0, mainIndex = 0;
    
    	//KMP
        while (modeIndex < modeStrLen && mainIndex < mainStrLen)
        {
            if (modeIndex == -1 || mainString[mainIndex] == modeString[modeIndex])
            {
                ++modeIndex, ++mainIndex;
            }
            else
            {
                modeIndex = next[modeIndex];
            }
        }

        if (modeIndex == modeStrLen)
            return mainIndex - modeIndex;

        return -1;
    }

    vector<int> GetNext(const string &modeString)
    {
        int mainIndex = 0, pairIndex = -1, length = modeString.length();
        vector<int> next(length);
        next[0] = -1;

        //next的求法其实也就是进行一次对自己的KMP
        while (mainIndex < length - 1)
        {
            if (pairIndex == -1 || modeString[mainIndex] == modeString[pairIndex])
            {
                ++mainIndex, ++pairIndex;
                next[mainIndex] = pairIndex;
            }
            else
            {
                pairIndex = next[pairIndex];
            }
        }
        return next;
    }
```



KMP算法是一个很精妙的算法，其关键在于next数组，但next数组的获取又涉及KMP算法，因为next的获取其实也就是进行一次对modeString的KMP，所以如果先讲next数组，在讲KMP算法，可能效果不太好。我们先弄清楚KMP的运行逻辑与过程，然后再说明next数组的逻辑，再证明KMP算法的正确性。

### KMP算法的运行逻辑与过程

其实如果你已经理解KMP算法的运行逻辑与过程，那就大可不必看这一部分内容，直接看提炼总结即可。因为下面的说明实在是太啰嗦了。

#### 公共前后缀

next数组是理解KMP的核心所在，但我们这里先不去理解next数组是怎么来的，也就是不要先去管**GetNext函数**， 我们只要先明白next数组的含义即可。

在此之前先理解一个很简单的概念**公共前后缀**。

先给出定义，定义可以看一下，看例子就能很好理解了。

* 前缀：是指不包含最后一个字符的所有以第一个字符开头的连续子串
* 后缀：是指不包含第一个字符的所有以最后一个字符结尾的连续子串
* 公共前后缀：是指前缀==后缀

例子一：s = "a"，s没有前缀，也没有后缀，公共前后缀也没有，可以理解为“”

例子二：s = “ab”，s的前缀："a"，s的后缀："b"，没有公共前后缀

例子三：s = “aaabaa”，s的前缀："a"、"aa"、"aaa"、"aaab"、"aaaba"，s的后缀："a"、"aa"、"baa"、"abaa"、"aabaa"，公共前后缀："a"、"aa"。

**next[j]的含义：modeString[0]~[j-1]的最长公共前后缀的长度**

**当模式串modeString[j]与mainString[i]匹配失败时，模式串要检查modeString[next[j]]与mainString[i]是否匹配**

```c++
		while ()
        {
            if (modeString[j] == mainString[i])
            {
                ++i, ++j;
            }
            else
            {
                j = next[j];
            }
        }
    
```

无论有何种疑惑，总之请先记牢上面两行加粗的字即可。

#### 模拟推演KMP算法的运行

我们只关注**Index**函数即可。

设

主串为mainString = "FABDABABCAB"

模式串为modeString = "ABCAB"

| next数组 |      |
| -------- | ---- |
| next[0]  | -1   |
| next[1]  | 0    |
| next[2]  | 0    |
| next[3]  | 0    |
| next[4]  | 1    |

按照定义都可以手算出next数组，不过一个特殊的点是对next[0]，显然不存在modeString[0]~[-1]，可能我们一开始会设置为0，但是设置为-1更好。next[0] = -1也是一个暂时要死记的点。

主逻辑

```c++
        int modeIndex = 0, mainIndex = 0;
		while (modeIndex < modeStrLen && mainIndex < mainStrLen)
        {
            if (modeIndex == -1 || mainString[mainIndex] == modeString[modeIndex])
            {
                ++modeIndex, ++mainIndex;
            }
            else
            {
                modeIndex = next[modeIndex];
            }
        }
```



```
比较mainString[0] == modeString[0]; 'F' == 'A';

​	 modeIndex = next[modeIndex]; modeIndex = -1;

modeIndex == -1

​	++modeIndex, ++mainIndex; modeIndex = 0, mainIndex = 1;

比较mainString[1] == modeString[0]; 'A' == 'A';

​	++modeIndex, ++mainIndex; modeIndex = 1, mainIndex = 2;

比较mainString[2] == modeString[1]; 'B' == 'B';

​	++modeIndex, ++mainIndex; modeIndex = 2, mainIndex = 3;

比较mainString[3] == modeString[2]; 'D' == 'C';

​	modeIndex = next[modeIndex]; modeIndex = 0;

比较mainString[3] == modeString[0]; 'D' == 'A';

​	modeIndex = next[modeIndex]; modeIndex = -1;

省略

比较mainString[6] == modeString[2]; 'A' == 'C';

​	modeIndex = next[modeIndex]; modeIndex = 0;

比较mainString[6] == modeString[0]; 'A' == 'A';

之后一路比对成功

跳出循环 mainIndex = 11, modeIndex = 5

KMP是否在主串中找到模式串的逻辑是相同的都是判断是否遍历完毕了模式串。

那么在6那儿相等
```



#### 解释运行逻辑

手动去模拟完毕后，我们能有很多发现。

可以发现mainIndex只会增加，而不是像暴力算法那样会把mainIndex往回移动。

modeIndex的往回移动是基于next数组的。

**next[j]的含义：modeString[0]~[j-1]的最长公共前后缀的长度**

**当模式串modeString[j]与mainString[i]匹配失败时，模式串要检查modeString[next[j]]与mainString[i]是否匹配**

我们在看一个例子，再一次深刻了解next数组的作用

主串："AABAACAABAAD"

模式串："AABAAD"

假设此时判断到"C"=="D"，匹配失败了。

按照暴力算法，我们要重新匹配"ABAACAABAAD"与"AABAAD"，但我们有next数组，它告诉我们应该判断"C"=="B"，因为"AABAAC“与"AABAAD"中前面匹配成功的"AABAA"最长公共前后缀的长度为2，我们就把问题匹配"AABAAC“与"AABAAD"，变成了匹配"AAC“与"AAB"

**也就是说我们可以把公共前缀和公共后缀对齐，继续往下比较，而不用往回移动mainIndex，如果没有公共前后缀，那么modeIndex=0或-1；也就说前面的都不匹配了，模式串要从头匹配，如果头也不匹配，那么我们就丢弃，那么主串的游标就要往后走。**

0和-1也是一个很妙的处理，首先明确只有next[0]能得到-1。我们继续上面的"AAC“与"AAB"，'C'与'B'也不匹配，'C'与modeString[1]='A'也不匹配，'C'与modeString[0]='A'也不匹配，所以我们不能继续匹配'C'了必须将其跳过，所以跳过条件modeIndex == -1，刚刚好-1+1 = 0，则继续新的匹配。

#### 提炼总结

其实最关键的就是next数组，也就是说，如果在某个位置匹配k失败的时候，我们可以知道[0]~[k-1]的字符串里的最长公共前后缀，我们就可以把模式串的最长公共前缀以及主串的最长公共后缀**对齐**起来，然后再匹配即可。

容我再啰嗦一句，这个对齐操作。理解起来很简单，上个例子即可

"ABCABDABCABF"

"ABCABF"

在判断'D'=='F'时失败了，重新对齐最长公共前后缀，next[5] = 2

"ABCABDABCABF"

​        "ABCABF"

此时判断'D'=='F'

关键的关键点就是next数组引导了对齐操作。

好嘛，正好此时就能引出next数组了。

### next数组

先上求next数组的代码

```c++
    vector<int> GetNext(const string &modeString)
    {
        int mainIndex = 0, pairIndex = -1, length = modeString.length();
        vector<int> next(length);
        next[0] = -1;

        //next的求法其实也就是进行一次对自己的KMP
        while (mainIndex < length - 1)
        {
            if (pairIndex == -1 || modeString[mainIndex] == modeString[pairIndex])
            {
                ++mainIndex, ++pairIndex;
                next[mainIndex] = pairIndex;
            }
            else
            {
                pairIndex = next[pairIndex];
            }
        }
        return next;
    }
```

这段代码就是KMP算法的精髓之精髓所在了。为什么自身进行KMP能得到next数组呢？

### next数组的证明

首先无论如何，有两个固定的值。

next[0] = -1, next[1] = 0;

下面就上数学证明了。

我们要证明：在运行完毕上述代码后，得到的next数组满足**next[i]的定义：modeString[0]~[i-1]的最长公共前后缀的长度**

证：

我们记pairIndex是以自身为模式串的要与主串匹配的游标的位置,mainIndex是以自身为主串的要与模式串匹配游标的位置。（又开始啰嗦了。）

且
$$
\text{i = 0,next[i] = -1}
$$

$$
\text{i = 1,next[i] = 0}
$$



此时next满足定义。

当：
$$
\text{i>=2}
$$
我们有已知
$$
\text{next[0]$\backsim$next[i-1]}
$$
都是满足next数组的定义的。

那么已知next[i-1]意味着我们已经知道modeString[0]~[i-2]的最长公共前后缀的长度

可设
$$
\text{next[i-1]=k}
$$
必然有：
$$
\text{modeString[i-1-k]$\backsim$[i-2] == modeString[0]$\backsim$[k-1]}
$$
那么为了求出next[i]，我们先对齐对齐模式串的前缀以及主串的后缀令
$$
\text{pairIndex=k,mainIndex=i-1}
$$
然后判断
$$
\text{? modeString[mainIndex] == modeString[pairIndex]}
$$


如果不匹配：
$$
\text{modeString[mainIndex] != modeString[pairIndex]}
$$
重新对齐模式串的前缀以及主串的后缀，即令pairIndex = next[pairIndex]，直至匹配或者pairIndex = -1意味着没有公共前后缀。

如果匹配：
$$
\text{modeString[mainIndex] == modeString[pairIndex]}
$$

$$
\to\text{[0]$\backsim$[mainIndex]的最长公共前后缀的长度为pairIndex+1}
$$

即
$$
\text{next[i]=最后更新的pairIndex加上1}
$$
此时next[i]满足定义。

由数学归纳法，经上述步骤后所求的next一定满足定义

把上面的数学推导转化成代码就是求出next数组的代码。

得证。

### KMP算法的证明

把next数组的正确性证明完毕后，终于算是理解了KMP算法了，KMP算法的难以理解的地方就是在于next数组的求出与KMP算法的是互相交叉的，先讲KMP算法，就容易被next数组绕晕，先讲next数组，就容易一头雾水为什么要这样求。

好嘛，我们已经得到了一个关于模式串的next数组。再一次搬出next数组的定义

next数组满足**next[i]的定义：modeString[0]~[i-1]的最长公共前后缀的长度**

再附上KMP算法的主逻辑

```c++
        int modeIndex = 0, mainIndex = 0;
    
    	//KMP
        while (modeIndex < modeStrLen && mainIndex < mainStrLen)
        {
            if (modeIndex == -1 || mainString[mainIndex] == modeString[modeIndex])
            {
                ++modeIndex, ++mainIndex;
            }
            else
            {
                modeIndex = next[modeIndex];
            }
        }

        if (modeIndex == modeStrLen)
            return mainIndex - modeIndex;

        return -1;
```

证：

（1）

当：
$$
\text{modeIndex = -1}
$$
可得：
$$
\text{mainString[mainIndex] != modeString[0]}
$$
此时要判断
$$
\text{? mainString[mainIndex+1] == modeString[0]}
$$
（2）

当：
$$
\text{modeIndex $\neq$ -1}
$$
如果匹配：
$$
\text{ mainString[mainIndex] == modeString[modeIndex]}
$$
则继续往下判断：
$$
\text{? mainString[mainIndex+1] == modeString[modeIndex+1]}
$$
如果不匹配：
$$
\text{ mainString[mainIndex] != modeString[modeIndex]}
$$
我们已知next[modeIndex]，即可设
$$
\text{next[modeIndex]=k$\neq$-1}
$$
我们必然有：
$$
\text{mainString[i-k]$\backsim$[i-1] == modeString[0]$\backsim$[k-1]}
$$
由此我们就不需要匹配0~k-1了，只需判断
$$
\text{? mainString[mainIndex] == modeString[k]}
$$
基于证明步骤的（1）、（2）

对模式串，其长度为modeStrLen，与主串，其长度为mainStrLen

如果主串上有子串与模式串匹配，那么我们就可以将模式串从头遍历完毕，即使modeIndex == modeStrLen

如果主串上没有有子串与模式串匹配，那么我们就可以将主串串从头遍历完毕且我们没有遍历完毕模式串，即使modeIndex != modeStrLen

将上述推导转换为代码即可

得证！

## 结语

以上是一些不严谨的证明，是否严谨不重要，重要的是在推理证明的过程中理解了KMP算法，思路最重要。就我个人而言，网上也有很多帖子给了我很多新的认知，但是看一次记住了，用一次就忘一次。无论是图例还是结合例子解释都让我感觉只是当时理解了KMP，但是其实并没有完全理解KMP为什么要这么做，这么做为什么可以保证算法的正确性。这一次花了一些时间，自己进行了一下推导证明突然就感觉通透了。
