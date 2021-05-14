


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 690826

  This file contains the code responsible for opening and closing file
  streams. The actual handling of the data from that stream is handled
  by the functions in data.cpp. See the header file for additional comments.
  
  Each function you must implement has a TODO in its comment block. You may
  have to implement additional functions. You may implement additional
  functions not specified.
 */

#include <iostream>
#include <sstream>
#include "input.h"

/*
  Constructor for an InputSource.

  @param source
    A unique identifier for a source (i.e. the location).
*/
InputSource::InputSource(const std::string& source): source(source){
  //throw std::logic_error("InputSource::InputSource() has not been implemented!");
}

/*
  This function gets the location of the source

  @return
    A non-modifable value for the source passed into the construtor.
*/
std::string InputSource::getSource() const{
    return source;
}


/*
  Constructor for a file-based source.

  @param path
    The complete path for a file to import.

  @example
    InputFile input("data/areas.csv");
*/
InputFile::InputFile(const std::string& filePath) : InputSource(filePath), ifs(filePath) {
  //std::cout << "Filename: " << filePath << "\n";
}

/*
  This function opens a file stream to the file path retrievable from getSource()
  and return a reference to the stream.

  @return
    A standard input stream reference

  @throws
    std::runtime_error if there is an issue opening the file, with the message:
    InputFile::open: Failed to open file <file name>

  @example
    InputFile input("data/areas.csv");
    input.open();
*/

std::ifstream &InputFile::open(){

    if(ifs.is_open()){
        //ifs.close();
        return ifs;
    } else {
        throw std::runtime_error("InputFile::open: Failed to open file " + getSource());
    }

}

