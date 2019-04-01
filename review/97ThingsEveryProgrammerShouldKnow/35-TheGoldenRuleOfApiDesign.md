# API设计黄金法则

[The Golden Rule of API Design](https://97-things-every-x-should-know.gitbooks.io/97-things-every-programmer-should-know/content/en/thing_35/)

API设计很难，尤其是大型的设计。如果你正在为成百上千的用户设计一款API，你不得不思考未来你可能作出何种改变，以及这些改变是否会给客户的代码造成破坏。除此之外，你不得不考虑这些API的用户会如何影响你。如果你的API类中有一个类会调用它自己的内部方法，你就不得不推荐用户可以通过继承一个子类并重写此方法，但这可能会成为灾难。你无法改变这个方法，因为用户已经给它赋予了不同的含义。你在之后的内部实现也将受到用户的支配。