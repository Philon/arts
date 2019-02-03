# [移除已排序数组的重复项](https://leetcode.com/problems/remove-duplicates-from-sorted-array/)

给一个排好序的数字数组，采用[原地算法](https://zh.wikipedia.org/wiki/原地算法)删除每个副本至剩下一个，返回新数组长度。

不要分配额外的数组空间，必须在“原地”修改输入数组，即空间复杂度为O(1)。
```
Example 1:
nums = [1,1,2],
函数返回length = 2，前两个元素数值分别为1和2。
本题不会去管超出你返回长度之后的元素。

Example 2:
nums = [0,0,1,1,1,2,2,3,3,4],
函数返回length = 5，前五个元素数值分别为0、1、2、3和4。
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

设置两个索引`i`和`j`，刚开始都指向数组起始：
- `i`负责无脑遍历数组，每次都i++
- `j`每次都和`i`指向的元素做对比，即`nums[i] != nums[j]`
- 如果两个元素相同，说明是副本，直接略过，否则将nums[i]拷贝到j的下一个，即`nums[++j]`

以下是C的实现：
```c
int removeDuplicates(int* nums, int numsSize) {
  int j = 0;
  for (int i = 1; i < numsSize; i++) {
    if (nums[j] != nums[i]) {
      nums[++j] = nums[i];
    }
  }

  return numsSize > 1 ? j + 1 : numsSize;
}
```
- 时间复杂度：O(n)，n是数组长度
- 空间复杂度：O(1)
