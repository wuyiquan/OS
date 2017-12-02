#include <iostream>
#include <fstream>
using namespace std;
fstream fin("/var/log/kern.log"); //打开文件
    string readline;
    while (getline(fin, readline)) //逐行读取，直到结束
    {
          cout << readline << endl;
    }

}