#include <exception>
#include <iostream>

#include "Application.h"

void Run() {
	Application app;
	app.Run();
}

int main() {
	try {
		Run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
