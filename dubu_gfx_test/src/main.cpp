#include <exception>

#include "Application.h"

void Run() {
	Application app;
	app.Run();
}

int main() {
	dubu::log::Register<dubu::log::ConsoleLogger>();

	Run();

	return 0;
}
