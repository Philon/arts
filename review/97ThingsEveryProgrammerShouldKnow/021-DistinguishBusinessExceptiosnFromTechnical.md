# 辨别来自技术中的业务异常

[Distinguish Business Exceptions from Technical](https://97-things-every-x-should-know.gitbooks.io/97-things-every-programmer-should-know/content/en/thing_21/)

导致运行时出错的事情根本上来说有两种原因：阻止我们使用程序的技术问题，以及组织我们滥用程序的业务逻辑。很多现代语言，如LISP、Java、Smalltalk和C#，运用异常来标记这两种情况。然而，这两种情况完全不同，要严格区分开。用相同级别的异常来描述他们会混淆潜在的来源，不要继承相同的异常类。
