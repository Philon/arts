# [移除元素](https://leetcode.com/problems/remove-element/)

给一个数字数组和一个值，采用[原地算法](https://zh.wikipedia.org/wiki/原地算法)删除所有和输入值相同的实例，返回新的长度。

不要分配额外的数组空间，必须在“原地”修改输入数组，即空间复杂度为O(1)。

为了元素可以被改变，不用去关心新长度之后的元素。

```
Example 1:
nums = [3,2,2,3], val = 3,
函数返回length = 2，前两个元素数值为2。

Example 2:
nums = [0,1,2,2,3,0,4,2], val = 2,
函数返回length = 5，前五个元素数值包含0, 1, 3, 0和 4.
```

澄清一下:

不明白为什么返回的是一个整型而是不是修改后的数组？

要知道输入的数组参数是一个引用，就意味着对输入数组的任何修改都会被调用者感知到。

你可以这么想：
```
// 数字集合通过引用被传递进去(即不需要拷贝)
int len = removeDuplicates(nums);

// 你在函数内对数组内容的任何修改都会被调用者感知到。
// 然后通过你返回的长度，它能后打印对应数量的元素。
for (int i = 0; i < len; i++) {
    print(nums[i]);
}
```

## 我的思路：双索引

这道题其实和第26题没有什么大的区别，都是两个索引各自为政即可。

以下是C的实现：
```c
int removeElement(int* nums, int numsSize, int val) {
  int j = 0;
  for (int i = 0; i < numsSize; i++) {
    if (nums[i] != val) {
      nums[j++] = nums[i];
    }
  }

  return j;
}
```
- 时间复杂度：O(n)，n是数组长度
- 空间复杂度：O(1)
