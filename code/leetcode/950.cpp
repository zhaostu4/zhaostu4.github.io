/* 题目描述:
牌组中的每张卡牌都对应有一个唯一的整数。你可以按你想要的顺序对这套卡片进行排序。
最初，这些卡牌在牌组里是正面朝下的（即，未显示状态）。
现在，重复执行以下步骤，直到显示所有卡牌为止：
    从牌组顶部抽一张牌，显示它，然后将其从牌组中移出。
    如果牌组中仍有牌，则将下一张处于牌组顶部的牌放在牌组的底部。
    如果仍有未显示的牌，那么返回步骤 1。否则，停止行动。
返回能以递增顺序显示卡牌的牌组顺序。
答案中的第一张牌被认为处于牌堆顶部。

输入：[17,13,11,2,3,5,7]
输出：[2,13,3,11,5,17,7]
解释：
我们得到的牌组顺序为 [17,13,11,2,3,5,7]（这个顺序不重要），然后将其重新排序。
重新排序后，牌组以 [2,13,3,11,5,17,7] 开始，其中 2 位于牌组的顶部。
我们显示 2，然后将 13 移到底部。牌组现在是 [3,11,5,17,7,13]。
我们显示 3，并将 11 移到底部。牌组现在是 [5,17,7,13,11]。
我们显示 5，然后将 17 移到底部。牌组现在是 [7,13,11,17]。
我们显示 7，并将 13 移到底部。牌组现在是 [11,17,13]。
我们显示 11，然后将 17 移到底部。牌组现在是 [13,17]。
我们展示 13，然后将 17 移到底部。牌组现在是 [17]。
我们显示 17。
由于所有卡片都是按递增顺序排列显示的，所以答案是正确的。

*/
/*
主要思路： 首先，因为割张取拍，因此偶数(0,2,4,6...)编号必定有序递增；
		其次，奇数(1,3,5,7...);不断后移，偶数项拿完之后必定，剩余奇数相再排列；直到只剩最后一张牌为止。

		解决思路;
		1.数组排序：选出最小一半，排在偶数号未上。
		2.对剩余数目，选出最小一半，排在相对偶数未上，剩下奇数位再排序。
		3.循环直到最后一位。
*/

#include <iostream>
#include <vector>


using namespace std;

void print_vector(std::vector<int> v){
	for(auto i : v) {		
		cout<<i<<endl;
	}
};
//main function
vector<int> deckRevealedIncreasing(std::vector<int>& deck) {
		std::vector<int> result;
		//首先冒泡排序先选出最小的前半位；
		for (int i = 0; i < deck.size(); ++i)
		{
			/* code */
			for (int j= 0; j < deck.size()-i-1; ++j)
			{
				/* code */
				if(deck[j]>deck[j+1]){
					int temp=deck[j];
					deck[j]=deck[j+1];
					deck[j+1]=temp;
				}
			}
		}
		// for (int i = 0; i < deck.size()/2; ++i)
		// {
		// 	/* code */
		// 	result.push_back(deck[i]);
		// }
		//迭代插入
		for(int i=0;i<deck.size();++i)//每次处理一半的数据
		{
			//先插一半
			for (int i = 0; i < deck.size()/2; ++i)
			{
				/* code */
				result.insert(result.begin()+i,deck[i]);

			}
			//再插剩下的一半的一半
		}
		print_vector(deck);
		return result;

    };

int main(int argc, char const *argv[]) {
	//input number
	std::vector<int> input={17,13,11,2,3,5,7};
	//output number
	std::vector<int> output={2,13,3,11,5,17,7};
	//my result number
	auto  my_result=deckRevealedIncreasing(input);
	print_vector(input);
	

	//print_vector(my_result);
	return 0;
}