


/*
  +---------------------------------------+
  | BETH YW? WELSH GOVERNMENT DATA PARSER |
  +---------------------------------------+

  AUTHOR: 690826

  This file contains all the helper functions for initialising and running
  Beth Yw? In languages such as Java, this would be a class, but we really
  don't need a class here. Classes are for modelling data, and so forth, but
  here the code is pretty much a sequential block of code (BethYw::run())
  calling a series of helper functions.

  TODO: This file contains numerous functions you must implement. Each one
  is denoted with a TODO in the block comment. Note that some code has been
  provided in some functions to get you started, but you should read through
  this code and make sure it is safe and complete. You may need to remove or
  modify the provided code to get your program to work fully. You may implement
  additional functions not specified.
*/

#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "lib_cxxopts.hpp"

#include "areas.h"
#include "datasets.h"
#include "bethyw.h"
#include "input.h"

/*
  Run Beth Yw?, parsing the command line arguments, importing the data,
  and outputting the requested data to the standard output/error.

  Hint: cxxopts.parse() throws exceptions you'll need to catch. Read the cxxopts
  documentation for more information.

  @param argc
    Number of program arguments

  @param argv
    Program arguments

  @return
    Exit code
*/
int BethYw::run(int argc, char *argv[]) {
  auto cxxopts = BethYw::cxxoptsSetup();
  auto args = cxxopts.parse(argc, argv);

  // Print the help usage if requested
  if (args.count("help")) {
    std::cerr << cxxopts.help() << std::endl;
    return 0;
  }

  // Parse data directory argument
  std::string dir = args["dir"].as<std::string>() + DIR_SEP;

  // Parse other arguments and import data

  try{
      BethYw::parseDatasetsArg(args);
  } catch (const std::invalid_argument &e){
      std::cerr << e.what() << "\n";
      exit(1);
  }

  auto datasetsToImport = BethYw::parseDatasetsArg(args);
  auto areasFilter      = BethYw::parseAreasArg(args);
  auto measuresFilter   = BethYw::parseMeasuresArg(args);
  auto yearsFilter      = BethYw::parseYearsArg(args);

  Areas data = Areas();

  BethYw::loadAreas(data, dir, areasFilter);

  BethYw::loadDatasets(data,
                       dir,
                       datasetsToImport,
                       areasFilter,
                       measuresFilter,
                       yearsFilter);

  if (args.count("json")) {
    // The output as JSON
    std::cout << data.toJSON() << std::endl;
  } else {
    // The output as tables
    std::cout << data << std::endl;
  }

  return 0;
}

/*
  This function sets up and returns a valid cxxopts object. You do not need to
  modify this function.

  @return
     A constructed cxxopts object

  @example
    auto cxxopts = BethYw::cxxoptsSetup();
    auto args = cxxopts.parse(argc, argv);
*/
cxxopts::Options BethYw::cxxoptsSetup() {
  cxxopts::Options cxxopts(
        "bethyw",
        "Student ID: " + STUDENT_NUMBER + "\n\n"
        "This program is designed to parse official Welsh Government"
        " statistics data files.\n");
    
  cxxopts.add_options()(
      "dir",
      "Directory for input data passed in as files",
      cxxopts::value<std::string>()->default_value("datasets"))(

      "d,datasets",
      "The dataset(s) to import and analyse as a comma-separated list of codes "
      "(omit or set to 'all' to import and analyse all datasets)",
      cxxopts::value<std::vector<std::string>>())(

      "a,areas",
      "The areas(s) to import and analyse as a comma-separated list of "
      "authority codes (omit or set to 'all' to import and analyse all areas)",
      cxxopts::value<std::vector<std::string>>())(

      "m,measures",
      "Select a subset of measures from the dataset(s) "
      "(omit or set to 'all' to import and analyse all measures)",
      cxxopts::value<std::vector<std::string>>())(

      "y,years",
      "Focus on a particular year (YYYY) or "
      "inclusive range of years (YYYY-ZZZZ)",
      cxxopts::value<std::string>()->default_value("0"))(

      "j,json",
      "Print the output as JSON instead of tables.")(

      "h,help",
      "Print usage.");

  return cxxopts;
}

/*
  Parse the datasets argument passed into the command line. 

  The datasets argument is optional, and if it is not included, all datasets 
  should be imported. If it is included, it should be a comma-separated list of 
  datasets to import. If the argument contains the value "all"
  (case-insensitive), all datasets should be imported.

  @param args
    Parsed program arguments

  @return
    A std::vector of BethYw::InputFileSource instances to import

  @throws
    std::invalid_argument if the argument contains an invalid dataset with
    message: No dataset matches key <input code>
 */
std::vector<BethYw::InputFileSource> BethYw::parseDatasetsArg(
    cxxopts::ParseResult& args) {

    // Retrieve all valid datasets, see datasets.h
    size_t numDatasets = InputFiles::NUM_DATASETS;
    auto &allDatasets = InputFiles::DATASETS;

    // Create the container for the return type
    std::vector<InputFileSource> datasetsToImport;

    /*
     * If there are arguments for datasets, we check does it include an "all" argument
     * if not, we then loop through datasets to check if the Code and the argument is the same/
     * If they are the same, we add that dataset into datasetsToImport. Otherwise we ignore it.
     * If there are invalid argument, we throw std::invalid_argument exception
     */
    if(args.count("datasets") > 0){
        // Retrieve the areas argument like so:
        auto temp = args["datasets"].as<std::vector<std::string>>();

        // Loop through all the argument
        for(size_t i = 0; i < temp.size(); i++){
            //If the argument contains "all" we clear the whole vector
            // and import all the datasets
            if(temp.at(i).compare("all") == 0){
                datasetsToImport.clear();
                for(size_t j = 0; j < numDatasets; j++){
                    datasetsToImport.push_back(allDatasets[j]);
                }
                break;
            } else {
                // If there is an invalid Argument we throw an exception
                // Otherwise, we add the dataset to the vector
                bool invalidArgument = true;
                for(size_t j = 0; j < numDatasets; j++){
                    if(temp.at(i).compare(allDatasets[j].CODE) == 0){
                        invalidArgument = false;
                        datasetsToImport.push_back(allDatasets[j]);
                    }
                }
                if(invalidArgument){
                    throw std::invalid_argument("No dataset matches key: " + temp.at(i));
                }
            }
        }
    } else {
        // Import all data if there are no arguments
        for(size_t j = 0; j < numDatasets; j++){
            datasetsToImport.push_back(allDatasets[j]);
        }
        return datasetsToImport;
    }
    return datasetsToImport;
}

/*
  Parses the areas command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all areas should be
  imported, i.e., the filter should be an empty set.

  Unlike datasets we can't check the validity of the values as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, we simply fetch the list of areas and later pass it to the
  Areas::populate() function.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific areas
    to import, or an empty set if all areas should be imported.
*/
std::unordered_set<std::string> BethYw::parseAreasArg(
    cxxopts::ParseResult& args) {
    // The unordered set you will return
    std::unordered_set<std::string> areas;

    /*
     * If there are arguments for areas,
     * we loop add the correspond argument to areaFilter.
     *
     * Otherwise, we return an empty set
     */
    if(args.count("areas")){
        // Retrieve the areas argument like so:
        auto temp = args["areas"].as<std::vector<std::string>>();
        // ...
        for(unsigned int i = 0; i < temp.size(); i++){
            // Check the arguement for "all" and
            // convert it to lowercase for comparison since it should accept in all cases
            std::string tempString = temp.at(i);
            std::transform(tempString.begin(), tempString.end(), tempString.begin(), ::tolower);
            /* Check if the argument contains "all" argument
             * If it exist, we clear the whole set and return an empty set.
             *
             * Otherwise, we insert it to the set
            */
            if(tempString.compare("all") == 0){
                areas.clear();
                return areas;
            } else {
                areas.insert(temp.at(i));
            }
        }
    } else {
        return areas;
    }
    return areas;
}

/*
  Parse the measures command line argument, which is optional. If it doesn't 
  exist or exists and contains "all" as value (any case), all measures should
  be imported.

  Unlike datasets we can't check the validity of the values as it depends
  on each individual file imported (which hasn't happened until runtime).
  Therefore, we simply fetch the list of areas and later pass it to the
  Areas::populate() function.

  The filtering of inputs should be case insensitive.

  @param args
    Parsed program arguments

  @return 
    An std::unordered_set of std::strings corresponding to specific measures
    to import, or an empty set if all measures should be imported.

*/
std::unordered_set<std::string>BethYw::parseMeasuresArg(
        cxxopts::ParseResult& args){
    // The unordered set you will return
    std::unordered_set<std::string> measures;

    /*
     * If there are arguments for measures,
     * we loop add the correspond argument to measuresFilter.
     *
     * Otherwise, we return an empty set
     */
    if(args.count("measures")){
        // Retrieve the areas argument like so:
        auto temp = args["measures"].as<std::vector<std::string>>();

        for(unsigned int i = 0; i < temp.size(); i++){
            // Check the arguement for "all" and
            // convert it to lowercase for comparison since it should accept in all cases
            std::string tempString = temp.at(i);
            std::transform(tempString.begin(), tempString.end(), tempString.begin(), ::tolower);
            /* Check if the argument contains "all" argument
             * If it exist, we clear the whole set and return an empty set.
             *
             * Otherwise, we insert it to the set
            */
            if(tempString.compare("all") == 0){
                measures.clear();
                return measures;
            }else {
                measures.insert(temp.at(i));
            }
        }
    } else {
        return measures;
    }
    return measures;
}


/*
  TODO: BethYw::parseYearsArg(args)

  Parse the years command line argument. Years is either a four digit year 
  value, or two four digit year values separated by a hyphen (i.e. either 
  YYYY or YYYY-ZZZZ).

  This should be parsed as two integers and inserted into a std::tuple,
  representing the start and end year (inclusive). If one or both values are 0,
  then there is no filter to be applied. If no year argument is given return
  <0,0> (i.e. to import all years). You will have to search
  the web for how to construct std::tuple objects! 

  @param args
    Parsed program arguments

  @return
    A std::tuple containing two unsigned ints

  @throws
    std::invalid_argument if the argument contains an invalid years value with
    the message: Invalid input for years argument
*/
std::tuple<unsigned int, unsigned int>BethYw::parseYearsArg(
        cxxopts::ParseResult& args){

    std::tuple<unsigned int, unsigned int> years;

    /*
     * If there are arguments for years,
     * we split the string into two int and check if it is a valid
     * years argument.
     *
     * Otherwise, we return <0,0>
     */
    if(args.count("years")){

        std::string str = args["years"].as<std::string>();

        std::vector<std::string> array;

        std::stringstream ss(str);

        // Splitting the string into the array vector
        while(ss.good()){
            std::string substr;
            std::getline(ss, substr, '-');
            array.push_back(substr);
        }

        // Checking where the value is a valid input
        // If not, we throw std::invalid_argument
        for(unsigned int i = 0; i < array.size(); i++){
            if(!BethYw::isNumber(array.at(i))){
                throw std::invalid_argument("Invalid input for years argument");
            }
        }

        /* Create tuple base on the size of the array.
         * If the size of the array is larger than 2, we
         * throw std::invalid_argument
         *
         * If the size of the array is 2, we create a tuple base
         * on those 2 values.
         *
         * If the size of the array is 1, we create a tuple base on
         * that single value.
         *
         * If the size is 0, we create <0,0>
        */
        if(array.size() > 2){
            throw std::invalid_argument("Invalid input for years argument");
        } else if(array.size() == 2){
            unsigned int start = std::stoi(array.at(0));
            unsigned int end = std::stoi(array.at(1));

            /* If the value is inside our range, in this case 4 digits year.
             * We create a tuple base on those values.
             *
             * Otherwise, we throw std::invalid_argument
            */
            if((start > 999 && start < 10000 && end > 999 && end < 10000) || (start == 0 && end == 0)){
                years = std::make_tuple(start, end);
            } else {
                throw std::invalid_argument("Invalid input for years argument");
            }
        } else if(array.size() == 1){
            unsigned int start = std::stoi(array.at(0));
            unsigned int end = std::stoi(array.at(0));

            /* If the value is inside our range, in this case 4 digits year.
             * We create a tuple base on those values.
             *
             * Otherwise, we throw std::invalid_argument
            */
            if((start > 999 && start < 10000 && end > 999 && end < 10000) || (start == 0 && end == 0)){
                years = std::make_tuple(start, end);
            } else {
                throw std::invalid_argument("Invalid input for years argument");
            }
        }
        else {
            years = std::make_tuple(0,0);
        }
    }

    return years;
}
/*
 * This function checks if the string input is a number
 *
 * @para str
 *      The string input
 *
 * @return
 *      True if the string is a number, false otherwise.
 */
bool BethYw::isNumber(const std::string& str){
    std::string::const_iterator it = str.begin();
    while(it != str.end() && std::isdigit(*it)){
        it++;
    }
    return !str.empty() && it == str.end();
}


/*
  Load the areas.csv file from the directory `dir`. Parse the file and
  create the appropriate Area objects inside the Areas object passed to
  the function in the `areas` argument.

  areas.csv is guaranteed to be formatted as:
    Local authority code,Name (eng),Name (cym)

  @param areas
    An Areas instance that should be modified (i.e. the populate() function
    in the instance should be called)

  @param dir
    Directory where the areas.csv file is

  @param areasFilter
    An unordered set of areas to filter, or empty to import all areas

  @return
    void
*/
void BethYw::loadAreas(Areas& areas, const std::string dir, const std::unordered_set<std::string>areasFilter){

    // A temp string for dir
    std::string dirTemp;

    std::stringstream ss;
    ss << dir << BethYw::InputFiles::AREAS.FILE;
    dirTemp = ss.str();

    InputFile inputFile(dirTemp);

    try{
        inputFile.open();
    }catch (const std::runtime_error &e) {
        std::cerr << "Error importing dataset:" << "\n";
        std::cerr <<  e.what() << "\n";
        exit(1);
    }


    auto &is = inputFile.open();

    auto type = InputFiles::AREAS.PARSER;

    auto cols = InputFiles::AREAS.COLS;

    try{
        areas.populate(is, type, cols, &areasFilter);
    } catch (const std::runtime_error &e){
        std::cerr << "Error importing dataset:" << "\n";
        std::cerr << "what(): " << e.what() << "\n";
    } catch(const std::out_of_range &e){
        std::cerr << "Error importing dataset:" << "\n";
        std::cerr << "what(): " << e.what() << "\n";
    }
}


/*
  Import datasets from `datasetsToImport` as files in `dir` into areas, and
  filtering them with the `areasFilter`, `measuresFilter`, and `yearsFilter`.

  The actual filtering will be done by the Areas::populate() function, thus 
  you need to merely pass pointers on to these flters.

  This function should promise not to throw an exception. If there is an
  error/exception thrown in any function called by thus function, catch it and
  output 'Error importing dataset:', followed by a new line and then the output
  of the what() function on the exception.

  @param areas
    An Areas instance that should be modified (i.e. datasets loaded into it)

  @param dir
    The directory where the datasets are

  @param datasetsToImport
    A vector of InputFileSource objects

  @param areasFilter
    An unordered set of areas (as authority codes encoded in std::strings)
    to filter, or empty to import all areas

  @param measuresFilter
    An unordered set of measures (as measure codes encoded in std::strings)
    to filter, or empty to import all measures

  @param yearsFilter
    An two-pair tuple of unsigned ints corresponding to the range of years 
    to import, which should both be 0 to import all years.

  @return
    void
*/
void BethYw::loadDatasets(Areas& areas,
                          const std::string dir,
                          const std::vector<BethYw::InputFileSource> datasetsToImport,
                          const std::unordered_set<std::string>areasFilter,
                          const std::unordered_set<std::string>measuresFilter,
                          const std::tuple<unsigned int, unsigned int> yearsFilter){

    // Loop through every datasetsToImport
    for(auto it = datasetsToImport.begin(); it != datasetsToImport.end(); it++){
        std::stringstream ss;
        // Get file directory for each datasets
        ss << dir << it->FILE;
        std::string dirTemp;
        dirTemp = ss.str();

        InputFile inputFile(dirTemp);

        try{
            inputFile.open();
        }catch (const std::runtime_error &e) {
            std::cerr << "Error importing dataset:" << "\n";
            std::cerr <<  e.what() << "\n";
            exit(1);
        }

        auto &is = inputFile.open();

        //auto type = InputFiles::AREAS.PARSER;
        auto type = it->PARSER;

        //auto cols = InputFiles::AREAS.COLS;
        auto cols = it->COLS;

        try{
            areas.populate(is, type, cols, &areasFilter, &measuresFilter, &yearsFilter);
        } catch (const std::runtime_error &e){
            std::cerr << "Error importing dataset:" << "\n";
            std::cerr << "what(): " << e.what() << "\n";
        } catch(const std::out_of_range &e){
            std::cerr << "Error importing dataset:" << "\n";
            std::cerr << "what(): " << e.what() << "\n";
        }

    }


}



