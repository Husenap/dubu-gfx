#include <exception>
#include <iostream>

#include "Application.h"

int main() {
	Application app;

	try {
		app.Run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
