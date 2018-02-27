//#include <iostream>

//using namespace std;

//int main()
//{
//    cout << "Hello World!" << endl;
//    return 0;
//}

#include<stdio.h>
#include <iostream>

//int main()
//{
//    int a[5]={1,2,3,4,5};
//    int *ptr=(int *)(&a+1);
//    printf("%d,%d",*(a+1),*(ptr-1));
//    return 0;
//}


class CTest
{
public:
    CTest():m_count(1)
    {
    }
    int count ()
    {
        return m_count++;
    }

    void count (const int count)
    {
        m_count=count;
    }

    void set(CTest test)
    {
        m_count=test.count();
    }
private:
    int m_count;
};

int main ()
{
    CTest a;
    a.count (10);
    CTest b;
    b.set (a);
    printf ("%d %d\n", a.count (), b.count ());
    printf ("%d %d\n", a.count (), b.count ());
}
