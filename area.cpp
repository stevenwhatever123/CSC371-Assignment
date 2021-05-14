


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 690826

  This file contains the implementation for the Area class. Area is a relatively
  simple class that contains a local authority code, a container of names in
  different languages (perhaps stored in an associative container?) and a series
  of Measure objects (also in some form of container).

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <vector>

#include "area.h"

/*
  Construct an Area with a given local authority code.

  @param localAuthorityCode
    The local authority code of the Area

*/
Area::Area(std::string localAuthorityCode) : localAuthorityCode(localAuthorityCode){
  //throw std::logic_error("Area::Area() has not been implemented!");

}

/*
    Empty Constructor for map/vector initialization
 */
Area::Area(){

}

/*
  This function retrieves the local authority code for this Area.
  
  @return
    The Area's local authority code
*/
std::string Area::getLocalAuthorityCode() const{
    return localAuthorityCode;
}

/*
   This function sets the local authority code for this Area.
 */
void Area::setLocalAuthorityCode(std::string localAuthorityCode){
    this->localAuthorityCode = localAuthorityCode;
}


/*
  This function gets the name for the Area in a specific language.

  @param lang
    A three-leter language code in ISO 639-3 format, e.g. cym or eng

  @return
    The name for the area in the given language

  @throws
    std::out_of_range if lang does not correspond to a language of a name stored
    inside the Area instance
*/
std::string Area::getName(std::string lang) const{
    if(this->lang.find(lang) != this->lang.end()){
        return this->lang.find(lang)->second;
    } else {
        throw std::out_of_range("Lang does not correspond to a language of a name"
                                "stored inside the Area instance");
    }
}

/*
  This function sets a name for the Area in a specific language.

  @param lang
    A three-letter (alphabetical) language code in ISO 639-3 format,
    e.g. cym or eng, which should be converted to lowercase

  @param name
    The name of the Area in `lang`

  @throws
    std::invalid_argument if lang is not a three letter alphabetic code
*/
void Area::setName(std::string lang, std::string name){
    std::transform(lang.begin(), lang.end(), lang.begin(), ::tolower);
    if(lang.length() != 3 || containsNumber(lang)){
        throw std::invalid_argument("Area::setName: Language code must be three alphabetical letters only");
    } else {
        this->lang[lang] = name;
    }
}

/*
    This function checks if the string input contains a number

    @para lang
        A string for checking

    @return
        True if the string contains a number, false otherwise.
 */
bool Area::containsNumber(std::string lang){
    for(auto it = lang.begin(); it != lang.end(); it++){
        if(std::isdigit(*it)){
            return true;
        }
    }
    return false;
}


/*
  This function retrieves a Measure object, given its codename.

  @param key
    The codename for the measure you want to retrieve

  @return
    A Measure object

  @throws
    std::out_of_range if there is no measure with the given code, throwing
    the message:
    No measure found matching <codename>
*/
Measure& Area::getMeasure(std::string key){
    if(this->measures.find(key) == this->measures.end()){
        throw std::out_of_range("No measure found matching " + key);
    } else {
        //return this->measures.find(key)->second;
        return this->measures[key];
    }
}


/*
  This function adds a particular Measure to this Area object.
  The Measure's codename will be converted to lowercase.

  If a Measure already exists with the same codename in this Area, it will overwrite any
  values contained within the existing Measure with those in the new Measure
  passed into this function. The resulting Measure stored inside the Area
  instance should be a combination of the two Measures instances.

  @param codename
    The codename for the Measure

  @param measure
    The Measure object

  @return
    void
*/
void Area::setMeasure(std::string codename, Measure& measure){
    // Converting the codename to lowercase
    std::transform(codename.begin(), codename.end(), codename.begin(), ::tolower);
    this->measures[codename] = measure;
    //this->measures.insert({codename, measure});
}

/*
  This function gets the number of Measures we have for this Area

  @return
    The size of the Area (i.e., the number of Measures)
*/
unsigned int Area::size() const{
    return this->measures.size();
}


/*
  Overload the stream output operator as a free/global function.

  Output the name of the Area in English and Welsh, followed by the local
  authority code. Then output all the measures for the area (see the coursework
  worksheet for specific formatting).

  If the Area only has only one name, output this. If the area has no names,
  output the name "Unnamed".

  Measures should be ordered by their Measure codename. If there are no measures
  output the line "<no measures>" after you have output the area names.

  See the coursework specification for more examples.

  @param os
    The output stream to write to

  @param area
    Area to write to the output stream

  @return
    Reference to the output stream

  @example
    Area area("W06000023");
    area.setName("eng", "Powys");
    std::cout << area << std::endl;
*/
std::ostream& operator<<(std::ostream& os, const Area& area){
    if(area.lang.size() == 0){
        os << "Unnamed";
    } else  if(area.lang.size() == 1){
        os << area.lang.begin()->second;
    } else {
        os << area.lang.rbegin()->second<< " / "
           << area.lang.begin()->second;
    }
    os << " (" << area.getLocalAuthorityCode() << ")";

    return os;
}


/*
  Overload the == operator for two Area objects as a global/free function. Two
  Area objects are only equal when their local authority code, all names, and
  all data are equal.

  @param lhs
    An Area object

  @param rhs
    A second Area object

  @return
    true if both Area instanes have the same local authority code, names
    and data; false otherwise.

  @example
    Area area1("MYCODE1");
    Area area2("MYCODE1");

    bool eq = area1 == area2;
*/
bool operator==(const Area& lhs, const Area& rhs){
    // Boolean for both having the same local authority code
    bool sameLocalAuthorityCode = lhs.getLocalAuthorityCode() == rhs.getLocalAuthorityCode();

    //Boolean to check if both areas has the same name
    bool sameNames = true;
    //Vector for storing all names from lhs
    std::vector<std::string> lhsTemp;
    //Vector for storing all names from rhs
    std::vector<std::string> rhsTemp;

    /*
     * Storing all name from both sides to the temp vector
     */

    for(auto it = lhs.lang.begin(); it != lhs.lang.end(); it++){
        if(std::find(lhsTemp.begin(), lhsTemp.end(), it->second) == lhsTemp.end()){
            lhsTemp.push_back(it->second);
        }
    }

    for(auto jt = rhs.lang.begin(); jt != rhs.lang.end(); jt++){
        if(std::find(lhsTemp.begin(), lhsTemp.end(), jt->second) == rhsTemp.end()){
            rhsTemp.push_back(jt->second);
        }
    }

    /*
     * Comparing both temp vector by looping through both vector and set sameNames to false
     * if one of the element does not appear on the other side.
     */

    for(auto it = lhsTemp.begin(); it != lhsTemp.end(); it++){
        for(auto jt = rhsTemp.begin(); jt != rhsTemp.end(); jt++){
            if(std::find(lhsTemp.begin(), lhsTemp.end(), *jt) == lhsTemp.end()){
                sameNames = false;
            }
        }
    }

    // Boolean for both having the same data
    bool sameData = lhs.measures == rhs.measures;

    return sameLocalAuthorityCode && sameNames && sameData;
}
