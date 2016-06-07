#include <iostream>
#include <string>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "parser/FLOPPYParser.h"

extern "C" {
#include "floppy.h"
#include "bufman.h"
#include "heap.h"
}

int main()
{
   std::cout << "Hello World!!!!!";
}
