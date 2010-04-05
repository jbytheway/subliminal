#include <iostream>

#include <boost/algorithm/string/join.hpp>

#include <optimal/optionsparser.hpp>

#include <ffmsxx/service.hpp>
#include <ffmsxx/get_single_video_source.hpp>

namespace {

  struct Options {
    Options() :
      help{},
      quiet{}
    {}

    bool help;
    bool quiet;
    boost::filesystem::path raw;
    boost::filesystem::path subs;
    boost::filesystem::path output;
  };

  void usage()
  {
    std::cout <<
"Subliminal is a free subtitle extraction system\n"
"\n"
"  subliminal [OPTIONS] --raw V1 --subs V2\n"
"\n"
"  -h, --help    Display this message.\n"
"  -o, --output  Save results in this file.  Default: stdout.\n"
"  -q, --quiet   Suppress various messages.\n"
"  -r, --raw     Version of the video without subs.\n"
"  -s, --subs    Version of the video with subs.\n"
<< std::endl;
  }

  Options get_options(int const argc, char* const* const argv)
  {
    optimal::OptionsParser parser;
    Options results;
    parser.addOption("help",   'h', &results.help);
    parser.addOption("output", 'o', &results.output);
    parser.addOption("quiet",  'q', &results.quiet);
    parser.addOption("raw",    'r', &results.raw);
    parser.addOption("subs",   's', &results.subs);

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
      "There is NO WARRANTY, to the extent permitted by law.\n" << std::endl;
  }

  if (options.raw.empty() || options.subs.empty()) {
    std::cerr << "Error: must specify two video files (see --help)" <<
      std::endl;
    return EXIT_FAILURE;
  }

  ffmsxx::service ffms;

  auto raw_indexer = ffmsxx::get_single_video_source(ffms, options.raw);

  return 0;
}

