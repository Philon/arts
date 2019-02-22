# [下一个排列](https://leetcode.com/problems/next-permutation/)

实现下一个排列，重新排列的数字按照字典序法，下一个排列数字刚好更大。

如果排列不存在，就尽可能按最小的顺序排列(即升序排序)。

这个替换木笔在原地进行，只能使用常数内存空间。

下面是几个例子。左边一列是输入，右边对应列是输出。

1,2,3 → 1,3,2  
3,2,1 → 1,2,3  
1,1,5 → 1,5,1

## 我的思路

刚看到题目的时候——懵逼了！根本看不懂什么意思。查了下关于[全排列](https://zh.wikipedia.org/wiki/全排列生成算法)才搞明白，简单来说，`1,2,3`这三个数可以看成`123`，如果只能用这三个数调换位置成新的数字，那么下一个比`123`大的数应该是`132`，所以结果是`1,3,2`。

而我的思路很简单，但是有bug，部分测试用例无法通过。

就是从数的末尾开始遍历，向前一个个比较，如果发现某个数比末尾小，就和末尾交换，如果没有发现，就把当前索引的数放到末尾，进行下一轮：

86751  
867`15`  
86`157`  
8`7156`

```c
void nextPermutation(int* nums, int numsSize) {
  // 当测试用例为4202320，就game over了
  int end = numsSize - 1;
  for (int i = end; i > 0; i--) {
    for (int j = i-1; j >= 0 && nums[end] > 0; j--) {
      if (nums[end] > nums[j]) {
        int tmp = nums[end];
        nums[end] = nums[j];
        nums[j] = tmp;
        qsort(&nums[j+1], numsSize - j - 1, sizeof(int), asc);
        return;
      }
    }

    int tmp = nums[i-1];
    for (int j = i; j < numsSize; j++) {
      nums[j-1] = nums[j];
    }
    nums[end] = tmp;
  }
}
```

---
以下是官方解答

## 概括

我们需要按字典序法找出比给定数字排列刚好大一点的新数字顺序。

## 解决方案

### 方案一： 暴力法

在这方法中，我们可以找出给定数组的全部可能的排列顺序，然后找出刚好比给定数组大一点的那一中排列。但这种方案显然很幼稚，找出全部的排列形式会花费非常长的时间，实现起来也很复杂。因此，改方案是不可能被接受了。我们直接把它排除正确方案。

- 时间复杂度：O(n!)，全部可能的排列形式有n!种
- 空间复杂度：O(n)，每个排列数组都需要存储空间

### 方案二、单通道法

首先我们要知道，任何降序排列的序列都不可能有下一个排列。例如，下面这个排列就不可能有下一个：

`[9, 5, 4, 3, 1]`

我们需要找到一对连续的两个数字`a[i]`和`a[i-1]`，从右看，一直满足`a[i] > a[i-1]`。现在，没有任何一个新排列可以大过右边这截了，因为它们显然是降序排列。因此我们需要重新排列这些数字直到最右边的a[i-1]。

现在，哪种才是我们要生成的下一个更大的数字？我们想要创建的是比现在这个刚好大一点的排列。所以，我们需要从排列中找出仅仅比`a[i-1]`大一点的数字，称为`a[j]`。

![](https://leetcode.com/media/original_images/31_nums_graph.png)

我们交换`a[i-1]`和`a[j]`。我们现在有了正确数字的索引i-1。但这仍然不是我们最终想要的那个排列，我们需要的排列应该是从`a[i-1]`的右边开始最小的排列形式。所以，我们还需要通过升序的方式把升序的数字排列一遍。

但是，再看已经扫描过的右边的这些数字，我们通过简单的递减索引就能发现`a[i]>a[i-1]`。因此，a[i-1]右边的这些数字已经是降序的了。而且，交换`a[i-1]`和`a[j]`也不会改变这种序列。所以，我们只需要简单的反转a[i-1]之后的这些数字即可得到字典序法的下一个排列。

![](https://i.loli.net/2019/02/22/5c6f9bc3de862.gif)

```java
public class Solution {
    public void nextPermutation(int[] nums) {
        int i = nums.length - 2;
        while (i >= 0 && nums[i + 1] <= nums[i]) {
            i--;
        }
        if (i >= 0) {
            int j = nums.length - 1;
            while (j >= 0 && nums[j] <= nums[i]) {
                j--;
            }
            swap(nums, i, j);
        }
        reverse(nums, i + 1);
    }

    private void reverse(int[] nums, int start) {
        int i = start, j = nums.length - 1;
        while (i < j) {
            swap(nums, i, j);
            i++;
            j--;
        }
    }

    private void swap(int[] nums, int i, int j) {
        int temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }
}
```
- 时间复杂度：O(n)。最坏情况下，仅需要扫描数组两边
- 空间复杂度：O(1)，没有额外分配空间。