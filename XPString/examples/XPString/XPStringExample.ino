#include <XPString.h>
#define BUF_LEN 200 // Size of the XPString buffer.

void setup()
{
    //Serial.begin(9600);

    char buf[BUF_LEN]; // Our XPString's backing buffer.
    XPString myString(buf, BUF_LEN); // Create a new XPString, using the created buffer.

    size_t len = myString.length();
    Serial.println(len); // Prints: 0

    myString += "Hello"; // Concatenate a string.
    Serial.println(myString); // Prints: "Hello"
    myString += ", world!";
    Serial.println(myString); // Prints: "Hello, world!"

    myString = "Testing other string functions";

    Serial.println(myString.starts_with("Testing")); // Prints: 1 (true)
    Serial.println(myString.starts_with("foo")); // Prints: 0 (false)
    Serial.println(myString.ends_with("functions")); // Prints: 1 (true)

    Serial.println(myString.index_of("other")); // Prints: 8
    Serial.println(myString.index_of("blah")); // Prints: -1 (String was not found)


    myString.reset(); // Same as doing myString = ""
    // Print values into the string:
    myString.println(100); // int
    myString.print(" is greater than "); // string
    myString.print(3.14f); // float
    Serial.println(myString); // Prints: "100 is greater than 3.14"

    // Separate the string into parts:
    myString = "example,of,comma,separated,values";

    #ifdef DEF_XPARTS
    XParts parts = myString.split(",");  // Split myString using "," as delimiter
    while (parts.has_next()) 
    { // While there are parts available...
       Serial.print("part: ");
       char *p = parts.get_next(); // Get the next part.
       Serial.println(p);
    }
    #endif
    //This prints:
    //      part: example
    //      part: of
    //      part: comma
    //      part: separated
    //      part: values
    // The contents of myString will be modified by split().  myString should not be modified by the user before has_next() returns false.

    myString = "You can also remove any part of the string.";
    myString.remove(13, 7);
    Serial.println(myString); // Prints: "You can also any part of the string."
    myString.remove(29);
    Serial.println(myString); // Prints: "You can also any part of the"
    myString.remove(0); // myString is now empty

    char smallbuf[10];
    XPString otherString(smallbuf, 10);
    otherString = "Hello!";

    // Append contents of otherString to myString:
    myString.append(otherString);
    Serial.println(myString); // Prints: "Hello!"

    // c_str() returns the String's backing buffer.
    const char *backing_buf = myString.c_str();

    myString.reset();
    // use myString for other stuff...
}

void loop()
{
}
