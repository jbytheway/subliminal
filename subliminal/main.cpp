#include <iostream>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem/operations.hpp>

#include <optimal/optionsparser.hpp>

#include <ffmsxx/service.hpp>
#include <ffmsxx/get_single_video_source.hpp>

#include "extract_subtitles.hpp"
#include "text_feedback.hpp"
#include "gtk_feedback.hpp"
#include "fatal.hpp"

namespace {

  struct Options {
    Options() :
      help{},
      quiet{}
    {}

    boost::filesystem::path data;
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
"  -d, --data DATA  Look for program data in DATA.  Default is to search for\n"
"                   a directory named 'data' in the executable's directory\n"
"                   or any parent thereof.\n"
"  -h, --help       Display this message.\n"
"  -o, --output     Save results in this file.  Default: stdout.\n"
"  -q, --quiet      Suppress various messages.\n"
"  -r, --raw V1     V1 is the video without subs.\n"
"  -s, --subs V2    V2 is the video with subs.\n"
<< std::endl;
  }

  Options get_options(int const argc, char* const* const argv)
  {
    optimal::OptionsParser parser;
    Options results;
    parser.addOption("data",   'd', &results.data);
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
  auto cwd = boost::filesystem::initial_path();

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

  boost::filesystem::path dataPath = options.data;

  if (dataPath.empty()) {
    boost::filesystem::path exe(argv[0]);
    if (!exe.has_root_path()) {
      /** \bug Searches too many directories when exe contains '..' members */
      exe = cwd/exe;
    }
    boost::filesystem::path exePath = exe.parent_path();
    while (!(exePath.empty() || boost::filesystem::exists(exePath/"data"))) {
      exePath = exePath.parent_path();
    }
    if (exePath.empty()) {
      SUBLIMINAL_FATAL(
        "couldn't locate data directory in any parent of exe directory.  "
        "Searched from " << exePath.parent_path()
      );
    }
    dataPath = exePath/"data";
  }

  ffmsxx::service ffms;

  auto raw_source = ffmsxx::get_single_video_source(ffms, options.raw);
  auto sub_source = ffmsxx::get_single_video_source(ffms, options.subs);
  subliminal::gtk_feedback feedback(dataPath, std::cout);

  subliminal::extract_subtitles(raw_source, sub_source, feedback);

  return 0;
}

