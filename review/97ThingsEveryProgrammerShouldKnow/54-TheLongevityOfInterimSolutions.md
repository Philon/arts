# 持久的临时方案

[The Longevity of Interim Solutions](https://97-things-every-x-should-know.gitbooks.io/97-things-every-programmer-should-know/content/en/thing_54/)

**我们为何要创建临时解决方案？**

通常，总有一些紧迫的问题需要解决。它可能是开发团队内部的问题，例如用某些工具去填补工具链的空白。它也可能是外部问题，对最终用户可见，例如通过变通办法处理缺失的功能。

在很多系统和团队里，你会发现一些系统会有隔离软件，它们被当作随时会变更的草案，即不符合标准，也未形成代码指南。而且可以肯定，你会听到开发者总在抱怨它们。最初创建这些的原因多种多样，但一个临时方案的成功非常简单：它有用！

临时解决方案，无论如何，它获得了惯性（或者说动能，取决于你的观点）。因为它就在那里，最终有用且被广泛接受，并没什么紧急要做的事情。每当权衡利弊来决定哪种行为价值更大时，就会出现很多排名较高的，适合于整合到系统的临时解决方案。为什么？因为它就在那里，它正常运行，它被接受了。唯一的明显缺点是它不符合约定好的标准和指南——除了一些利基市场，但这并不重要。

所以临时解决方案仍然存在，永远。

如果临时解决方案出了问题呢，它大概率不会出现在产品质量认证的条目里。那怎么办？当然是迅速用一个临时更新来替代这次临时方案啦，这也将收获好评。它将和最初的临时方案一样健壮…只是更新了一个版本而已。

这有什么问题吗？

答案取决于你的项目，以及你个人在产品代码标准中的利益。当一个系统包含了太多的临时解决方案，它的熵或者内部复杂度就会上升，可维护性就会下降。不论如何，这可能是个首先要问的错误问题。请记住我们在探讨的是解决方案，而不是你更喜欢的那个方案——它不大可能是所有人都喜欢的方案——但重做这个方案的动机很弱。

所以当我们发现问题是该做什么呢？

1. 避免一开始就创建临时方案。
2. 改变那些影响项目经理决策的因素。
3. 远离它。

现在让我们进一步来探讨这些选项：

1. 很多时候，回避并不是一种选项。实际的问题就摆在眼前，而且标准又那么严格。你可能会伤精费神地去尝试改变标准——尽管那是单调乏味的努力——且这种改变通常不会对你手头的问题有任何影响。
2. 这些因素源自项目文化，那些抗拒改变的本能。如果项目非常小的情况下倒是可能成功——尤其是项目中只有你自己——以及在和高层反映之前就已经清理了混乱。当然，如果项目已经混乱到停滞的地步，(方案)也可能推行成功，不过通常需要一些时间来接受。
3. 如果前面两种选项都不行，那就自动进入这个选项。

你会创建很多解决方案；它们其中一些是临时的，绝大部分是有用的。克服临时方案的最佳途径是让它们变得多余，提供更为优雅和有用的方案。愿你能平静地接受那些无法改变的事物，而鼓足勇气去改变那些可以改变的，并拥有甄别这些区别的智慧。