#include<iostream>
#include"memorypool.h"
int main()
{
    try
    {
        MemoryPool pool(2 * 1024 * 1024,1000);
        //�ڴ�������
        int* p1 = reinterpret_cast<int*>(pool.Allocate(sizeof(int)));
        char* p2 = reinterpret_cast<char*>(pool.Allocate(sizeof(char)));
        double* p3 = reinterpret_cast<double*>(pool.Allocate(sizeof(double)));
        void* p4 = reinterpret_cast<void*>(pool.Allocate(1 * 1024 * 1024));
        void* p5 = reinterpret_cast<void*>(pool.Allocate(1 * 1024 * 1024));
        std::string* p6 = reinterpret_cast<std::string*>(pool.Allocate(sizeof"ashiftmaker"));
        pool.printStatus();

        //�ͷ�һ����
        pool.deAllocate(p1);
        pool.deAllocate(p3);
        pool.deAllocate(p4);
        pool.deAllocate(p6);
        pool.printStatus();

        //�ٷ���
        char* p7 = reinterpret_cast<char*>(pool.Allocate(sizeof(char)));
        float* p8 = reinterpret_cast<float*>(pool.Allocate(sizeof(float)));
        void* p9 = reinterpret_cast<void*>(pool.Allocate(3 * 1024 * 1024));
        pool.printStatus();

        //�����쳣
        try {
            MemoryPool invalidPool(999,100); // 10MB�޷���999����
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << "\n";
        };
        try {
            pool.deAllocate(nullptr); // �����ͷſ�ָ��
        }
        catch (const std::exception& e) {
            std::cout << "�쳣: " << e.what() << "\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "�쳣: " << e.what() << "\n";
    }
}

  