#include <iostream>

#include <boost/algorithm/string/join.hpp>

#include <optimal/optionsparser.hpp>

namespace {

  struct Options {
    Options() :
      help{},
      quiet{}
    {}

    bool help;
    bool quiet;
  };

  void usage()
  {
    std::cout <<
"Subliminal is a free subtitle extraction system\n"
"\n"
"  subliminal [OPTIONS]\n"
"\n"
"  -h, --help   Display this message.\n"
"  -q, --quiet  Supress various messages.\n"
<< std::endl;
  }

  Options get_options(int const argc, char* const* const argv)
  {
    optimal::OptionsParser parser;
    Options results;
    parser.addOption("help",  'h', &results.help);
    parser.addOption("quiet", 'q', &results.quiet);

    if (parser.parse(argc, argv)) {
      std::cerr <<
        "Error(s) parsing options:\n  " <<
        boost::algorithm::join(parser.getErrors(), "\n  ") << "\n\n" <<
        "Try --help for usage instructions.\n";
      exit(EXIT_FAILURE);
    }

    return results;
  }

}

int main(int argc, char** argv)
{
  Options options = get_options(argc, argv);

  if (options.help) {
    usage();
    return EXIT_SUCCESS;
  }

  if (!options.quiet) {
    std::cout <<
      "Subliminal, a subtitle extraction system\n"
      "Copyright (C) 2010 John Bytheway\n"
      "License GPLv3+: GNU GPL version 3 or later\n"
      "This is free software: you are free to change and redistribute it.\n"
      "There is NO WARRANTY, to the extent permitted by law." << std::endl;
  }

  return 0;
}

