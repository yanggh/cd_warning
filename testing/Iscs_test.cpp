#include "Iscs.h" 

int main(int argc, char** argv)
{
    std::thread  th1(ModBus_Deal);

    th1.join();

    return 0;
}
