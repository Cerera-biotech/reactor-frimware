#define FIRMWARE_VERSION 2

void print_welcome()
{
    printf(" __                                   \n|  \\                                  \n| $$  ______   ______ ____    ______  \n| $$ |      \\ |      \\    \\  /      \\ \n| $$  \\$$$$$$\\| $$$$$$\\$$$$\\|  $$$$$$\\\n| $$ /      $$| $$ | $$ | $$| $$  | $$\n| $$|  $$$$$$$| $$ | $$ | $$| $$__/ $$\n| $$ \\$$    $$| $$ | $$ | $$| $$    $$\n \\$$  \\$$$$$$$ \\$$  \\$$  \\$$| $$$$$$$ \n                            | $$      \n                            | $$      \n                             \\$$      \n");
    printf("version: %d\n", FIRMWARE_VERSION);
}