#include "room.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
	{
		return 1;
	}

    Room room{argv[1]};
    room.start();
}