


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 690826

  The file contains the Areas class implementation. Areas are the top
  level of the data structure in Beth Yw? for now.

  Areas is also responsible for importing data from a stream (using the
  various populate() functions) and creating the Area and Measure objects.

  This file contains numerous functions you must implement. Each function you
  must implement has a TODO block comment. 
*/

#include <stdexcept>
#include <iostream>
#include <string>
#include <stdexcept>
#include <tuple>
#include <unordered_set>
#include <sstream>

#include "lib_json.hpp"

#include "datasets.h"
#include "areas.h"
#include "measure.h"

/*
  An alias for the imported JSON parsing library.
*/
using json = nlohmann::json;

/*
  Constructor for an Areas object.

  @example
    Areas data = Areas();
*/
Areas::Areas() {
  //throw std::logic_error("Areas::Areas() has not been implemented!");
}

/*
  This function adds a particular Area to the Areas object.

  If an Area already exists with the same local authority code, overwrite all
  data contained within the existing Area with those in the new
  Area (i.e. they should be combined, but the new Area's data should take
  precedence, e.g. replace a name with the same language identifier).

  @param localAuthorityCode
    The local authority code of the Area

  @param area
    The Area object that will contain the Measure objects

  @return
    void
*/
void Areas::setArea(std::string localAuthorityCode, Area &area){
    areasContainer[localAuthorityCode] = area;
}


/*
  This function retrieves an Area instance with a given local authority code.

  @param localAuthorityCode
    The local authority code to find the Area instance of

  @return
    An Area object

  @throws
    std::out_of_range if an Area with the set local authority code does not
    exist in this Areas instance
*/
Area& Areas::getArea(std::string localAuthorityCode){
    if(areasContainer.find(localAuthorityCode) == areasContainer.end()){
        throw std::out_of_range("No area found matching " + localAuthorityCode);
    } else {
        return areasContainer[localAuthorityCode];
    }
}

/*
   This function gets the areas container in the Areas object

   @return
        the AreasContainer
 */
AreasContainer& Areas::getAreaContainer() {
    return this->areasContainer;
}


/*
  This function retrieves the number of Areas within the container.

  @return
    The number of Area instances
*/
unsigned int Areas::size() const{
    return areasContainer.size();
}


/*
  This function specifically parses the compiled areas.csv file of local 
  authority codes, and their names in English and Welsh.

  This is a simple dataset that is a comma-separated values file (CSV), where
  the first row gives the name of the columns, and then each row is a set of
  data.

  Once the data is parsed, Area objects will be created accordingly and
  insert them in to areasContainer.

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityCodeCSV(
    std::istream &is,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter) {

    // Getting the first line of the csv file which is the heading
    std::string str;
    std::getline(is, str);
    std::stringstream sstream(str);
    std::vector<std::string> headings;
    while(sstream.good()){
        std::string substr;
        getline(sstream, substr, ',');
        headings.push_back(substr);
    }

    // There should be only three elements in our headings vector, if there are
    // more than 3, there are not enough columns in cols
    if(headings.size() > 3){
        throw std::out_of_range("Not enought columns in cols");
    } else if(headings.at(0).compare(cols.at(BethYw::SourceColumn::AUTH_CODE)) != 0 &&
        headings.at(1).compare(cols.at(BethYw::SourceColumn::AUTH_NAME_ENG)) != 0 &&
        headings.at(2).compare(cols.at(BethYw::SourceColumn::AUTH_NAME_CYM)) != 0){
        throw std::runtime_error("Parsing error occurs: due to malformed file");
        // 0 = Local Authority Code, 1 = Name (eng), 2 = Name (cym)
    }

    /* Start reading every line of the csv file and create
     * area's object accordingly
    */
    while(is.good()) {
        std::getline(is, str);

        // Vector for storing data
        std::vector<std::string> values;      // Index: 0 = area code, 1 = eng, 2 = cym
        std::stringstream ss(str);

        // Reading data and store it in our values vector
        while (ss.good()) {
            std::string substr;
            getline(ss, substr, ',');
            values.push_back(substr);
        }

        // Check if it is in our areafilters, if not, we ignore, it. Otherwise, we create area object
        if (areasFilter != NULL && areasFilter->find(values[0]) == areasFilter->end() && !areasFilter->empty()) {
            continue;
        } else {
            Area *area = new Area(values[0]);
            //std::string langCodeEnglish  = cols.at(BethYw::SourceColumn::AUTH_NAME_ENG);
            std::string langCodeEnglish = "eng";
            area->setName(langCodeEnglish, values[1]);

            //std::string langCodeWelsh  = cols.at(BethYw::SourceColumn::AUTH_NAME_CYM);
            std::string langCodeWelsh = "cym";
            area->setName(langCodeWelsh, values[2]);

            areasContainer[values[0]] = *area;
        }
    }
}

/*
  This function creates data according to Json file by extracting the local authority
  code, English name (the files only contain the English names), and each measure by
  year.

  If there are an Area object that does not exist in the Areas container, a
  new area object would be created

  If areasFilter is a non-empty set only include areas matching the filter. If
  measuresFilter is a non-empty set only include measures matching the filter.
  If yearsFilter is not equal to <0,0>, only import years within the range
  specified by the tuple (inclusive).

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings of areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings of measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as the range of years to be imported (inclusively)

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromWelshStatsJSON(std::istream& is,
                                       const BethYw::SourceColumnMapping& cols,
                                       const StringFilterSet * const areasFilter,
                                       const StringFilterSet * const measuresFilter,
                                       const YearFilterTuple * const yearsFilter){

    json j;
    is >> j;

    // Loop through every line of the Json file
    for (auto& el : j["value"].items()) {
        auto &data = el.value();

        // Getting the local Authority Code
        std::string localAuthorityCode = data[cols.at(BethYw::SourceColumn::AUTH_CODE)];

        // Boolean to check if the line read is the data we need from the areasFilter
        bool isInArea = false;

        /* If areasFilter is empty or if the current area is
         * in the areasFilter, we set isInArea to true and
         * if the data is not currently in our areasContainer,
         * we set the local authority code and name of the area object.
         *
         * Otherwise, we do nothing.
         *
        */
        if(areasFilter->empty()){
            isInArea = true;
            if(areasContainer.find(localAuthorityCode) == areasContainer.end()){
                areasContainer[localAuthorityCode].setLocalAuthorityCode(localAuthorityCode);
                areasContainer[localAuthorityCode].setName("eng", data[cols.at(BethYw::SourceColumn::AUTH_NAME_ENG)]);
            }
        } else {
            if(areasFilter->find(localAuthorityCode) != areasFilter->end()){
                isInArea = true;
                if(areasContainer.find(localAuthorityCode) == areasContainer.end()){
                    areasContainer[localAuthorityCode].setLocalAuthorityCode(localAuthorityCode);
                    areasContainer[localAuthorityCode].setName("eng", data[cols.at(BethYw::SourceColumn::AUTH_NAME_ENG)]);
                }
            }
        }


        std::string measureCode = "";
        /*
         * Since some datasets does not have MEASURE_CODE in the JSON file,
         * we replace it with SINGLE_MEASURE_CODE that was set in the datasets.h
         */
        if(cols.find(BethYw::SourceColumn::MEASURE_CODE) == cols.end()){
            measureCode = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE);
        } else {
            if(data[cols.at(BethYw::SourceColumn::MEASURE_CODE)].empty()){
                throw std::out_of_range("There are not enough columns in cols");
            } else {
                measureCode = data[cols.at(BethYw::SourceColumn::MEASURE_CODE)];
            }
        }
        // Convert the measure code to lowercase
        std::transform(measureCode.begin(), measureCode.end(), measureCode.begin(), ::tolower);

        // Boolean to check if the line read is part of the measuresFilter
        bool isInMeasures = false;

        // We read measures data only if it is in our areasFilter
        if(isInArea){
            /* If measuresFilter is empty or if the current measure is
            * in the measuresFilter, we set isInMeasures to true and
            * if the data is not currently in our area object,
            * we create a new Measure object and add it to the current
             * area object.
            *
            * Otherwise, we do nothing.
            *
            */
            if(measuresFilter->empty()){
                //std::cout << "I shouldnt end up here" << "\n";
                isInMeasures = true;
                if(areasContainer[localAuthorityCode].measures.find(measureCode) == areasContainer[localAuthorityCode].measures.end()){
                    std::string label = "";
                    /*
                    * Since some datasets does not have MEASURE_NAME in the JSON file,
                    * we replace it with SINGLE_MEASURE_NAME that was set in the datasets.h
                    */
                    if(cols.find(BethYw::SourceColumn::MEASURE_NAME) == cols.end()){
                        label = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME);
                    } else {
                        if(data[cols.at(BethYw::SourceColumn::MEASURE_NAME)].empty()){
                            throw std::out_of_range("There are not enough columns in cols");
                        } else {
                            label = data[cols.at(BethYw::SourceColumn::MEASURE_NAME)];
                        }
                    }
                    //std::string label = data[cols.at(BethYw::SourceColumn::MEASURE_NAME)];
                    Measure *measure = new Measure(measureCode, label);
                    areasContainer[localAuthorityCode].setMeasure(measureCode, *measure);
                }
            } else {
                if(measuresFilter->find(measureCode) != measuresFilter->end()) {
                    isInMeasures = true;
                    if (areasContainer[localAuthorityCode].measures.find(measureCode) == areasContainer[localAuthorityCode].measures.end()) {
                        std::string label;
                        /*
                        * Since some datasets does not have MEASURE_NAME in the JSON file,
                        * we replace it with SINGLE_MEASURE_NAME that was set in the datasets.h
                        */
                        if(cols.find(BethYw::SourceColumn::MEASURE_NAME) == cols.end()){
                            label = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME);
                        } else {
                            if(data[cols.at(BethYw::SourceColumn::MEASURE_NAME)].empty()){
                                throw std::out_of_range("There are not enough columns in cols");
                            } else {
                                label = data[cols.at(BethYw::SourceColumn::MEASURE_NAME)];
                            }
                        }
                        Measure *measure = new Measure(measureCode, label);
                        areasContainer[localAuthorityCode].setMeasure(measureCode, *measure);
                    }
                }
            }
        }

        /*
         * If isInMeasures and isInArea is true,
         * We start reading the year data
         */
        if(isInMeasures && isInArea){
            std::string yearInString = data[cols.at(BethYw::SourceColumn::YEAR)];
            int year = std::stoi(yearInString);
            int minYear = 0;
            int maxYear = 0;
            std::tie(minYear, maxYear) = *yearsFilter;
            /*
             * If year is within the yearFilter time frame and if the yearFilter is <0,0>,
             * we save the data to our measure object
             */
            if((year >= minYear && year <= maxYear) || (minYear == 0 && maxYear == 0)){
                //std::cout << "LocalAuthorityCode: " << localAuthorityCode << "\n";
                //std::cout << "MeasureCode: " << measureCode << "\n";
                //std::cout << "Value: " << data[cols.at(BethYw::SourceColumn::VALUE)] << "\n";

                /*
                if(typeof(data[cols.at(BethYw::SourceColumn::VALUE)].value()).name()){

                }
                 */
                double measureValue;
                std::string stringCompare = "string";

                if(stringCompare.compare(data[cols.at(BethYw::SourceColumn::VALUE)].type_name()) == 0){
                    std::string dataInString = data[cols.at(BethYw::SourceColumn::VALUE)];

                    measureValue = std::stod(dataInString);
                } else {
                    measureValue = data[cols.at(BethYw::SourceColumn::VALUE)];
                }

                areasContainer[localAuthorityCode].getMeasure(measureCode).setValue(year, measureValue);

                //std::cout << "Result: " << areasContainer[localAuthorityCode].getMeasure(measureCode)<< "\n\n";
            }
        }
    }
}

/*
  This function imports CSV files that contain a single measure. The 
  CSV file consists of columns containing the authority code and years.
  Each row contains an authority code and values for each year (or no value
  if the data doesn't exist).

  @param is
    The input stream from InputSource

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of strings for measures to import, or an empty 
    set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @see
    See datasets.h for details of how the variable cols is organised

  @see
    See bethyw.cpp for details of how the variable areasFilter is created

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file)
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populateFromAuthorityByYearCSV(std::istream& is,
                                       const BethYw::SourceColumnMapping& cols,
                                       const StringFilterSet * const areasFilter,
                                       const StringFilterSet * const measuresFilter,
                                       const YearFilterTuple * const yearsFilter){

    // Getting the first line of the csv file which is the heading: Authority code + year
    std::string str;
    std::getline(is, str);
    std::vector<std::string> headings;
    std::stringstream stream(str);
    while(stream.good()){
        std::string substr;
        getline(stream, substr, ',');
        headings.push_back(substr);
    }

    // Getting the year value from yearsFilter
    int minYear = 0;
    int maxYear = 0;
    std::tie(minYear, maxYear) = *yearsFilter;

    // Reading data from every line
    while(is.good()){
        std::getline(is, str);

        // Vector for storing all values from the line
        std::vector<std::string> values;      // Index: 0 = area code, numbers after 0 correspond to year
        std::stringstream ss(str);

        //Split the line into values vector
        while(ss.good()){
            std::string substr;
            getline(ss, substr, ',');
            values.push_back(substr);
        }

        std::string localAuthorityCode = values[0];

        /* Check if the area is in the areasFilter
         * If yes, we add data to the area object
         */
        if(areasFilter != NULL && areasFilter->find(localAuthorityCode) == areasFilter->end() && !areasFilter->empty()){
            continue;
        } else {
            // Starting from index 1 because we have the area code already
            for(unsigned int i = 1; i < values.size(); i++){
                int year = std::stoi(headings.at(i));
                //std::cout << "Year: " << year << "\n";
                //std::cout << "MinYear: " << minYear << "\n";
                //std::cout << "MaxYear: " << maxYear << "\n\n";
                if((year >= minYear && year <= maxYear) || (minYear == 0 && maxYear == 0)){
                    if(cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE).empty()){
                        throw std::out_of_range("There are not enough columns in cols");
                    } else {
                        // We get the data base on SINGLE_MEASURE_CODE and convert it to lowercase
                        std::string measureCode = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE);
                        std::transform(measureCode.begin(), measureCode.end(), measureCode.begin(), ::tolower);
                        // Getting the name of the label from SINGLE_MEASURE_NAME
                        std::string measureLabel = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_NAME);
                        /*
                         * If the current measureCode is part in the measureFilter or if the
                         * measureFilter is empty, we add the data to the measure object
                         */
                        if(measuresFilter->find(measureCode) != measuresFilter->end() || measuresFilter->empty()){
                            /*
                             * If the measure object is not created before,
                             * we create a new measure object and add it
                             * to the area object
                             */
                            if(areasContainer[localAuthorityCode].measures.find(measureCode) != areasContainer[localAuthorityCode].measures.end()){
                                double measureValue = std::stod(values.at(i));
                                // Adding value to the Measure object
                                areasContainer[localAuthorityCode].getMeasure(measureCode).setValue(year, measureValue);
                            } else {
                                double measureValue = std::stod(values.at(i));
                                Measure *measure = new Measure(measureCode, measureLabel);
                                measure->setValue(year, measureValue);
                                areasContainer[localAuthorityCode].setMeasure(measureCode, *measure);
                            }
                        }
                    }
                }
            }
            /*
            for(auto it = std::next(values.begin()); it != values.end(); it++){
                int value = std::stoi(*it);
                if(year >= minYear && year <= maxYear){
                    if(cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE).empty()){
                        throw std::out_of_range("There are not enough columns in cols");
                    } else {
                        // We get the data base on SINGLE_MEASURE_CODE
                        std::string measureCode = cols.at(BethYw::SourceColumn::SINGLE_MEASURE_CODE);
                        double measureValue = std::stod(*it);
                        // Adding value to the Measure object
                        areasContainer[localAuthorityCode].getMeasure(measureCode).setValue(year, measureValue);
                    }
                }
            }
             */
        }
    }

}

/*
  Parse data from an standard input stream, that is of a particular type,
  and with a given column mapping, filtering for specific areas, measures,
  and years, and fill the container.

  This overloaded function includes pointers to the three filters for areas,
  measures, and years.

  @param is
    The input stream from InputSource

  @param type
    A value from the BethYw::SourceDataType enum which states the underlying
    data file structure

  @param cols
    A map of the enum BethyYw::SourceColumnMapping (see datasets.h) to strings
    that give the column header in the CSV file

  @param areasFilter
    An umodifiable pointer to set of umodifiable strings for areas to import,
    or an empty set if all areas should be imported

  @param measuresFilter
    An umodifiable pointer to set of umodifiable strings for measures to import,
    or an empty set if all measures should be imported

  @param yearsFilter
    An umodifiable pointer to an umodifiable tuple of two unsigned integers,
    where if both values are 0, then all years should be imported, otherwise
    they should be treated as a the range of years to be imported

  @return
    void

  @throws 
    std::runtime_error if a parsing error occurs (e.g. due to a malformed file),
    the stream is not open/valid/has any contents, or an unexpected type
    is passed in.
    std::out_of_range if there are not enough columns in cols
*/
void Areas::populate(
    std::istream &is,
    const BethYw::SourceDataType &type,
    const BethYw::SourceColumnMapping &cols,
    const StringFilterSet * const areasFilter,
    const StringFilterSet * const measuresFilter,
    const YearFilterTuple * const yearsFilter)
     {
    if (type == BethYw::AuthorityCodeCSV) {
        populateFromAuthorityCodeCSV(is, cols, areasFilter);
    } else if(type == BethYw::WelshStatsJSON){
        populateFromWelshStatsJSON(is, cols, areasFilter, measuresFilter, yearsFilter);
    } else if(type == BethYw::AuthorityByYearCSV){
        populateFromAuthorityByYearCSV(is, cols, areasFilter, measuresFilter, yearsFilter);
    } else {
        throw std::runtime_error("Areas::populate: Unexpected data type");
    }
}

/*
  This function converts this Areas object, and all its containing Area instances, and
  the Measure instances within those, to values.
  
  The JSON should be formatted as such:
    {
    "<localAuthorityCode1>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               },
    "<localAuthorityCode2>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    ...
    "<localAuthorityCodeN>" : {
                              "names": { "<languageCode1>": "<languageName1>",
                                         "<languageCode2>": "<languageName2>" 
                                         …
                                         "<languageCodeN>": "<languageNameN>" }, 
                              "measures" : { "<year1>": <value1>,
                                             "<year2>": <value2>,
                                             …
                                            "<yearN>": <valueN> }
                               }
    }
  
  @return
    std::string of JSON
*/
std::string Areas::toJSON() const {
    json j;
    if(areasContainer.empty()){
        j = json ({});
    } else {
        // Print out every area object in areasContainer
        for(auto it = areasContainer.begin(); it != areasContainer.end(); it++){
            // Print out all the measures we have in this area object
            for(auto jt = it->second.measures.begin(); jt != it->second.measures.end(); jt++){
                std::map<int, double> list = jt->second.getAllValue();
                // Print out all the values we have in this measure object
                for(auto zt = list.begin(); zt != list.end(); zt++){
                    j[it->first]["measures"][jt->second.getCodename()][std::to_string(zt->first)] = zt->second;
                }
            }
            // Printing out all the names in this area object
            for(auto jt = it->second.lang.begin(); jt != it->second.lang.end(); jt++){
                j[it->first]["names"][jt->first] = jt->second;
            }
        }
    }
  return j.dump();
}

/*
  Overload the << operator to print all of the imported data.

  Output should be formatted like the following to pass the tests. Areas should
  be printed, ordered alphabetically by their local authority code. Measures 
  within each Area should be ordered alphabetically by their codename.

  See the coursework specification for more information, although for reference
  here is a quick example of how output should be formatted:

    <English name of area 1> / <Welsh name of area 1> (<authority code 1>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>


    <English name of area 2> / <Welsh name of area 2> (<authority code 2>)
    <Measure 1 name> (<Measure 1 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 code>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x code>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

    ...

    <English name of area y> / <Welsh name of area y> (<authority code y>)
    <Measure 1 name> (<Measure 1 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 1> <diff 1> <diffp 1>

    <Measure 2 name> (<Measure 2 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 2> <diff 2> <diffp 2>

    <Measure 3 name> (<Measure 3 codename>)
     <year 1>  <year 2> <year 3> ...  <year n>
    <value 1>  <year 2> <year 3> ... <value n> <mean 3> <diff 3> <diffp 3>

    ...

    <Measure x name> (<Measure x codename>)
     <year 1>  <year 2> <year 3> ...  <year n>  Average    Diff.   % Diff.
    <value 1>  <year 2> <year 3> ... <value n> <mean x> <diff x> <diffp x>

  With real data, your output should start like this for the command
  bethyw --dir <dir> -p popden -y 1991-1993 (truncated for readability):

    Isle of Anglesey / Ynys Môn (W06000001)
    Land area (area) 
          1991       1992       1993    Average    Diff.  % Diff. 
    711.680100 711.680100 711.680100 711.680100 0.000000 0.000000 

    Population density (dens) 
         1991      1992      1993   Average    Diff.  % Diff. 
    97.126504 97.486216 98.038430 97.550383 0.911926 0.938905 

    Population (pop) 
            1991         1992         1993      Average      Diff.  % Diff. 
    69123.000000 69379.000000 69772.000000 69424.666667 649.000000 0.938906 


    Gwynedd / Gwynedd (W06000002)
    Land area (Area)
    ...

  @param os
    The output stream to write to

  @param areas
    The Areas object to write to the output stream

  @return
    Reference to the output stream

  @example
    Areas areas();
    std::cout << areas << std::end;
*/
std::ostream& operator<<(std::ostream& os, Areas& areas){
    for(auto it = areas.areasContainer.begin(); it != areas.areasContainer.end(); it++){
        os << it->second << "\n";
        if(it->second.measures.size() > 0){
            for(auto jt = it->second.measures.begin(); jt != it->second.measures.end(); jt++){
                os << jt->second << "\n";
            }
        } else {
            os << "<no measures>" << "\n\n";
        }
    }
    return os;
}


