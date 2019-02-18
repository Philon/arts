# 不要僵直钉死你的程序

[Don’t Nail Your Program into the Upright Position](https://97-things-every-x-should-know.gitbooks.io/97-things-every-programmer-should-know/content/en/thing_28/)

我有次写了一个C++恶搞比赛，我讽刺地建议了以下的异常处理策略：

> 凭借遍布我们整个代码库的`try...catch`的结构，我们有时能够阻止程序的终止。我们把这种状态认为是“直立钉尸”。

尽管我的草率，实际上我还是总结了从这位祥林嫂(Dame Bitter Experience——或者叫艰苦岁月女？🥵)膝上学到的一课。

这是我们自制的C++库，是应用程序的一个基类。多年以后它被千“猿”所指：没有谁的手是干净的。它包含的代码把任何事情的异常都做了避开处理。通过Yossarian的[第22条军规(Catch-22)](https://en.wikipedia.org/wiki/Catch-22)作为指导，我们决定，或者说倾向于这个类的一个实例要么总是活着，要么赶紧死掉。(*决定*暗示更多想法，而不仅仅是进入这只怪物的结构)

为此，我们把多个异常交织在一起处理。我们把Windows的结构化异常与原生的类型混合在一起处理(记住C++中的__try..__catch？我们都没有)。当有意外抛出时，我们尝试再次调用，更难压入参数。