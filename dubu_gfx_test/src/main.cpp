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
		std::cerr << "====== FATAL ERROR ======" << std::endl;
		std::cerr << e.what() << std::endl;
		std::cerr << "====== FATAL ERROR ======" << std::endl;
		return 1;
	}

	return 0;
}
