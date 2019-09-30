# [平衡二叉树](https://leetcode.com/problems/balanced-binary-tree/)

给一个二叉树，确定它是否为高度平衡。所谓高度平衡是指，子树每个节点的深度相差不超过1。

```
Example 1:
Given the following tree [3,9,20,null,null,15,7]:

    3
   / \
  9  20
    /  \
   15   7
Return true.

Example 2:
Given the following tree [1,2,2,3,3,null,null,4,4]:

       1
      / \
     2   2
    / \
   3   3
  / \
 4   4
Return false.
```

## 这道题让我很困惑

主要是对题意的理解，所谓高度均衡的二叉树：
- 我第一反应是——叶节点的高度相差不能超过1
- 官方的说法是——子树节点的高度相差不能超过1

乍一看貌似没区别，但实际上如果不咬文嚼字的话，很可能在这个测试用例上栽跟头：
```
[1,2,2,3,3,3,3,4,4,4,4,4,4,null,null,5,5]

             1
       2     |      2
    3     3  |   3     3
  4  4  4  4 | 4  4  #  #
5  5         |
```

可以看到最左的深度是5，最右的深度是3。

如果按照第一种理解，子节点最大相差2，理所当然要返回false，**可官方答案是true**。

我到论坛里看了下，发现确实有人对此提出了质疑，但官方的解释也很直接，**子树节点高度相差不超过1**，如果根节点的左右子树来看，左边是5，右边是4......

不论如何，我把两种理解，对应的思路都写下来。

## 解题思路：叶节点高度均衡

如果是比较二叉树当中最深叶节点很最浅也节点的差距，最简单粗暴的办法：先遍历树，存下每个叶的深度，然后比较它们之间的差距。

当然，下面的代码还有优化空间，比如for嵌套遍历可以改为最大/最小值查找，将复杂度从O(N²)将为O(N)
```cpp
class Solution {
public:
  bool isBalanced(TreeNode* root) {
    vector<int> leaf;
    depth(root, leaf, 0);
    for (int i = 0; i < leaf.size(); i++) {
      for (int j = i+1; j < leaf.size(); j++) {
        if (abs(leaf[i] - leaf[j]) > 1) {
          return false;
        }
      }
    }

    return true;
  }

  void depth(TreeNode* node, vector<int>& leaf, int d) {
    if (node) {
      depth(node->left, leaf, d+1);
      depth(node->right, leaf, d+1);
    } else {
      leaf.push_back(d);
    }
  }
};
```

## 解题思路：子树节点高度均衡

如果是比较左右两颗子树的最大深度的话，那就是“纯递归遍历”。首先递归遍历节点的深度，然后在遍历左右树是否均衡，判断均衡的方法很简单，比较它们的子节点深度是否相差超过2(递归)。

懒得写if else，就高了个长长的三元运算😄

```cpp
class Solution {
public:
  bool isBalanced(TreeNode* root) {
    return root ? isBalanced(root->left) && isBalanced(root->right) && abs(depth(root->left)-depth(root->right)) < 2 : true;
  }

  int depth(TreeNode* node) {
    return node ? max(depth(node->left), depth(node->right))+1 : 0;
  }
};
```