#include <Devtools.h>

String Devtools::endOfLine = "";

// ----------------------- DEBUG FUNCTIONS -------------------------
void Devtools::print_to_console(String msg) // print String
{
	if (Devtools::do_print_to_console)
		endOfLine += msg;
}

void Devtools::print_to_console(int printInt) // print int
{
	if (Devtools::do_print_to_console)
		endOfLine += printInt;
	else if (Devtools::do_send_to_processing)
		Serial.write(printInt);
}

void Devtools::print_to_console(float printFloat) // print float
{
	if (Devtools::do_print_to_console)
		endOfLine += printFloat;
}

void Devtools::println_to_console(String msg)
{
	if (do_print_to_console)
		endOfLine += msg;
	printLine();
}

void Devtools::println_to_console(int printInt) // print int
{
	if (do_print_to_console)
		endOfLine += printInt;
	printLine();
}

void Devtools::println_to_console(float printFloat) // print float
{
	if (do_print_to_console)
		endOfLine += printFloat;
	printLine();
}

// print end of Line:
void Devtools::printLine()
{
	Serial.println(endOfLine);
	endOfLine = "";
}
