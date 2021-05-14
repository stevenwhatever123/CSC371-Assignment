


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 690826

  This file contains the implementation of the Measure class. Measure is a
  very simple class that needs to contain a few member variables for its name,
  codename, and a Standard Library container for data. The data you need to 
  store is values, organised by year. I'd recommend storing the values as 
  doubles.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <string>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "measure.h"

Measure::Measure(){

}

/*
  Construct a single Measure, that has values across many years.

  All StatsWales JSON files have a codename for measures. You should convert 
  all codenames to lowercase.

  @param codename
    The codename for the measure

  @param label
    Human-readable (i.e. nice/explanatory) label for the measure
*/
Measure::Measure(std::string code, const std::string &label) {
    // Converting code string to lowercase
    std::transform(code.begin(), code.end(), code.begin(), ::tolower);
    this->codename = code;
    this->label = label;
    //throw std::logic_error("Measure::Measure() has not been implemented!");
}

/*
  This function retrieves the code for the Measure.

  @return
    The codename for the Measure

*/
std::string Measure::getCodename() const{
    return this->codename;
}


/*
  This function retrieves the human-friendly label for the Measure.

  @return
    The human-friendly label for the Measure
*/
std::string Measure::getLabel() const{
    return this->label;
}


/*
  This function change the label for the Measure.

  @param label
    The new label for the Measure


*/
void Measure::setLabel(const std::string &label){
    this->label = label;
}

/*
  TODO: Measure::getValue(key)

  This function retrieves a Measure's value base on a given year.

  @param key
    The year to find the value for

  @return
    The value stored for the given year

  @throws
    std::out_of_range if year does not exist in Measure with the message
    No value found for year <year>

  @return
    The value
*/
double Measure::getValue(int key){
    // If the size of value is empty or the key does not exist
    // An exception will be thrown
    if(values.size() < 1 || this->values.find(key) == this->values.end()){
        throw std::out_of_range("No value found for year " + std::to_string(key));
    } else {
        return this->values[key];
    }
}

/*
   This function retrieves all values (i.e. key and value) from this Measure object

   @return
        std::map<int, double> All data
 */
std::map<int, double> Measure::getAllValue() const{
    return this->values;
}


/*
  This function adds a particular year's value to the Measure object.
  If the value already exists for the year, replace it.

  @param key
    The year to insert a value at

  @param value
    The value for the given year

  @return
    void
*/
void Measure::setValue(int key, double value){
    this->values[key] = value;
}


/*
  This function retrieves the number of years data we have for this measure.

  @return
    The size of the measure
*/
unsigned int Measure::size() const{
    return this->values.size();
}


/*
  TODO: Measure::getDifference()

  This function calculates the difference between the first and last year imported.

  @return
    The difference/change in value from the first to the last year, or 0 if it
    cannot be calculated

*/
double Measure::getDifference() const{
    if(this->values.size() < 1){
        return 0;
    } else {
        return this->values.rbegin()->second - this->values.begin()->second;
    }
}


/*
  TODO: Measure::getDifferenceAsPercentage()

  This function calculates the difference between the first and last year imported
  as a percentage.

  @return
    The difference/change in value from the first to the last year as a decminal
    value, or 0 if it cannot be calculated
*/
double Measure::getDifferenceAsPercentage() const{
    if(this->values.size() < 1){
        return 0;
    } else {
        double firstYearValue = this->values.begin()->second;
        double finalYearValue = this->values.rbegin()->second;
        double result = (finalYearValue - firstYearValue)/firstYearValue * 100;

        return result;
    }
}


/*
  TODO: Measure::getAverage()

  This function calculates the average/mean value for all the values.

  @return
    The average value for all the years, or 0 if it cannot be calculated
*/
double Measure::getAverage() const{
    double sum = 0;
    int size = this->values.size();
    for(auto it = this->values.begin(); it != this->values.end(); it++){
        sum += it->second;
    }
    return sum/size;
}

/*
   This function rounds up the given value to how many decimal Place

   @para value
        The value to be converted

   @para decimalPlace
        The number of decimal place for the value to be convert

   @return
        The value
 */
double roundUp(double value, int decimalPlace){
    const double multipler = std::pow(10.0, decimalPlace);
    return std::ceil(value * multipler) / multipler;
}


/*
  Overload the << operator to print all of the Measure's imported data.

  We align the year and value outputs by padding the outputs with spaces,
  i.e. the year and values should be right-aligned to each other so they
  can be read as a table of numerical values.

  Years should be printed in chronological order. Three additional columns
  should be included at the end of the output, correspodning to the average
  value across the years, the difference between the first and last year,
  and the percentage difference between the first and last year.

  If there is no data in this measure, print the name and code, and 
  on the next line print: <no data>

  See the coursework specification for more information.

  @param os
    The output stream to write to

  @param measure
    The Measure to write to the output stream

  @return
    Reference to the output stream
*/
std::ostream& operator<<(std::ostream& os, const Measure& measure){
    os << measure.getLabel() << " (" << measure.getCodename() << ")\n";
    // Check for the largest value in the data
    double largestValue = 0;
    for(auto it = measure.values.begin(); it != measure.values.end(); it++){
        if(it->second > largestValue){
            largestValue = it->second;
        }
    }

    // Converting all values to n decimal place
    int numberOfDigits = 8; // 7 = dot + 6 decimal place
    int largestValueInt = (int) largestValue;
    while(largestValueInt /= 10){
        numberOfDigits++;
    }


    // Printing out all years first
    for(auto it = measure.values.begin(); it != measure.values.end(); it++){
        // Lining up all values to the right hand side
        os << std::right << std::setw(numberOfDigits) << it->first << " ";
    }
    int numberOfDigitsDifference = 8; // 7 = dot + 6 decimal place
    int largestValueIntDifference = (int) measure.getDifference();
    while(largestValueIntDifference /= 10){
        numberOfDigitsDifference++;
    }

    int numberOfDigitsPercent = 8; // 7 = dot + 6 decimal place
    int largestValueIntPercent = (int) measure.getDifferenceAsPercentage();
    while(largestValueIntPercent /= 10){
        numberOfDigitsPercent++;
    }

    // Headings for final three column
    os << std::right << std::fixed << std::setw(numberOfDigits) << "Average" << " ";
    os << std::right << std::fixed << std::setw(numberOfDigitsDifference) << "Diff." << " ";
    os << std::right << std::fixed << std::setw(numberOfDigitsPercent) << "%Diff." << " ";
    os << "\n";

    // Printing all values
    for(auto it = measure.values.begin(); it != measure.values.end(); it++){
        os << std::right << std::setprecision(6) << it->second << " ";
    }
    // Values of final three column
    os << std::right << std::setprecision(6) << measure.getAverage() << " ";
    os << std::right << std::setprecision(6) << measure.getDifference() << " ";
    os << std::right << std::setprecision(6) << measure.getDifferenceAsPercentage() << " ";
    os << "\n";

    return os;
}


/*
  Overload the == operator for two Measure objects. Two Measure objects
  are only equal when their codename, label and data are all equal.

  @param lhs
    A Measure object

  @param rhs
    A second Measure object

  @return
    true if both Measure objects have the same codename, label and data; false
    otherwise
*/
bool operator==(const Measure& lhs, const Measure& rhs){
    bool sameCodename = lhs.getCodename() == rhs.getCodename();

    bool sameLabel = lhs.getLabel() == rhs.getLabel();

    bool sameData = lhs.values == rhs.values;

    return sameCodename && sameLabel && sameData;
}

