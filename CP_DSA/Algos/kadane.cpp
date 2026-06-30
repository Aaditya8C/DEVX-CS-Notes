int maxSubarraySum(vector<int> &nums)
{
    int maxSum = nums[0], cur = nums[0];
    for (int i = 1; i < nums.size(); i++)
    {
        cur = max(nums[i], cur + nums[i]);
        maxSum = max(maxSum, cur);
    }
    return maxSum;
}
