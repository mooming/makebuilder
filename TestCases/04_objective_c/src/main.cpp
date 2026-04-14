#include <iostream>

extern "C" void ObjCLib_Function();
extern void ObjCPPLib_Function();

int main()
{
    ObjCLib_Function();
    ObjCPPLib_Function();
    std::cout << "Hello from OCApp!" << std::endl;
    return 0;
}
