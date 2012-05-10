#include<iostream>

using namespace std;

class Solution {
public:
    int divide(int dividend, int divisor) {
        // Start typing your C/C++ solution below
        // DO NOT write int main() function
        int ret = 0;
        
        bool neg = false;
        
        if (dividend < 0)
        {
            dividend = -dividend;
            neg = !neg;
        }
        
        if (divisor < 0)
        {
            divisor = -divisor;
            neg = !neg;
        }

        int did = divisor;
		while( dividend > did)
			did = did << 1;


		while(did >= divisor)
		{
			if (dividend >= did)
			{
				dividend -= did;
				ret |= 1;
			}
            did = did >> 1;
            ret = ret << 1;
		}

		ret = ret >> 1;
		return neg ? -ret : ret;
    }
};

int main(void)
{
	Solution x;
	cout << x.divide(10, 2);
	return 0;
}