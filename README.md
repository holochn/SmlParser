# SmlParser
A Parser for SML (Smart Message Language) in C++ including an implementation for esp32.
In the example data are read from UART_NUM_2 and fed into the parser. Some interesting 
data are printed to the screen. 
This example uses data from an smart electric meter by ISK.

To start create an object of class `SmlParser`, which accepts a pointer to an 
unsigned char array as well as the pointer to the position in the array where you want to 
start to parse as parameters.

`unsigned char my_buffer[256];
SmlParser myParser = SmlParser(&my_buffer, 0);`

To start parsing just call method `parseSml()`:
`myParser.parseSml();`

To get the parsed data, create e.g. an `SmlListEntry` object:
`SmlListEntry myEntry = myParser.getElementByObis(some_ORBIS_number);
printf("%s\n", myEntry.sValue);`

You can adapt the logging by changing the log level of class `SmlLogger`.
Pick one of your favorites:
* Verbose
* Debug
* Info
* Warning
* Error

Example:
`SmlLogLevel SmlLogger::logLevel{SmlLogLevel::Warning};`
