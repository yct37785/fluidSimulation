#include "Engine.h"

int main()
{
    Engine engine;
    engine.Init();
    engine.Run();
    engine.Exit();

    // display memory leak report
    _CrtDumpMemoryLeaks();
}
