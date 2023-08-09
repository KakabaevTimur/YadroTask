#include "room.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
	{
		return 1;
	}

    try
    {
        Room room{argv[1]};
        room.start();
    }
    catch(const std::exception& e)
    {
        return 1;
    }
}