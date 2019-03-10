#include <stdlib.h>

int binarySearch(int* nums, int left, int right, int target) {
  int mid = (right + left) / 2;
  if (nums[mid] == target) {
    return mid;
  } else if (nums[left] == target) {
    return left;
  } else if (nums[right] == target) {
    return right;
  }

  if (left == mid) {
    return -1;
  }

  if (nums[mid] > nums[left]) {
    return target > nums[left] && target < nums[mid]
               ? binarySearch(nums, left, mid, target)
               : binarySearch(nums, mid, right, target);
  } else {
    return target > nums[mid] && target < nums[right]
               ? binarySearch(nums, mid, right, target)
               : binarySearch(nums, left, mid, target);
  }
}

int search(int* nums, int numsSize, int target) {
  if (nums == NULL || numsSize == 0) {
    return -1;
  } else {
    return binarySearch(nums, 0, numsSize - 1, target);
  }
}