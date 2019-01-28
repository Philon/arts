int removeDuplicates(int* nums, int numsSize) {
  int j = 0;
  for (int i = 1; i < numsSize; i++) {
    if (nums[j] != nums[i]) {
      nums[++j] = nums[i];
    }
  }

  return numsSize > 1 ? j + 1 : numsSize;
}