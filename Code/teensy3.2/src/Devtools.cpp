#include <Devtools.h>

// ----------------------- DEBUG FUNCTIONS -------------------------
void Devtools::print_to_console(String message_to_print) // print String
{
	if (Devtools::do_print_to_console)
		Serial.print(message_to_print);
}

void Devtools::print_to_console(int int_to_print) // print int
{
	if (Devtools::do_print_to_console)
		Serial.print(int_to_print);
	else if (Devtools::do_send_to_processing)
		Serial.write(int_to_print);
}

void Devtools::print_to_console(float float_to_print) // print float
{
	if (Devtools::do_print_to_console)
		Serial.print(float_to_print);
}

void Devtools::println_to_console(String message_to_print)
{
	if (do_print_to_console)
		Serial.println(message_to_print);
}

void Devtools::println_to_console(int int_to_print) // print int
{
	if (do_print_to_console)
		Serial.println(int_to_print);
}

void Devtools::println_to_console(float float_to_print) // print float
{
	if (do_print_to_console)
		Serial.println(float_to_print);
}

