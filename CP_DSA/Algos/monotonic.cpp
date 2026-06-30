vector<int> nextGreater(vector<int> &nums)
{
    vector<int> res(nums.size(), -1);
    stack<int> st;
    for (int i = 0; i < nums.size(); i++)
    {
        while (!st.empty() && nums[st.top()] < nums[i])
            res[st.top()] = nums[i], st.pop();
        st.push(i);
    }
    return res;
}
