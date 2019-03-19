int searchInsert(int* nums, int numsSize, int target) {
  int left = 0;
  int right = numsSize - 1;

  while (left <= right) {
    int i = (left + right) / 2;
    if (nums[i] > target) {
      right = i - 1;
    } else if (nums[i] < target) {
      left = i + 1;
    } else {
      return i;
    }
  }

  return left;
}