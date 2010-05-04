#include <iostream>

#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/fusion/include/io.hpp>

#include <optimal/optionsparser.hpp>

#include <ffmsxx/service.hpp>
#include <ffmsxx/error.hpp>
#include <ffmsxx/get_single_video_source.hpp>

#include "extract_subtitles.hpp"
#include "text_feedback.hpp"
#include "gtk_feedback.hpp"
#include "fatal.hpp"
#include "transform_params.hpp"

namespace {

  struct Options {
    Options() :
      chunking_threshold{10, 3},
      frame_interval{1},
      gtk{true},
      help{},
      quiet{}
    {}

    boost::optional<int> alignment_frame;
    typedef boost::optional<int> OInt;
    std::pair<OInt, OInt> bounds;
    boost::fusion::vector<int, int> chunking_threshold;
    boost::filesystem::path data;
    int frame_interval;
    bool gtk;
    bool help;
    bool quiet;
    std::string raw;
    std::string subs;
    boost::filesystem::path output;
    subliminal::transform_params start_params;
  };

  void usage()
  {
    std::cout <<
"Subliminal is a free subtitle extraction system\n"
"\n"
"  subliminal [OPTIONS] --output O --raw V1 --subs V2\n"
"\n"
"  -a, --align N    Use frame N for determining alignment.\n"
"  -b, --begin B    Analyze from frame B\n"
"  -c, --chunking (X Y)\n"
"                   Consider portions of the image to be part of the same\n"
"                   subtitle if they are within X, Y pixels in the x, y\n"
"                   directions respectively.\n"
"  -d, --data DATA  Look for program data in DATA.  Default is to search for\n"
"                   a directory named 'data' in the executable's directory\n"
"                   or any parent thereof.\n"
"  -e, --end E      Analyze only before frame E\n"
"  -f, --frame-interval N\n"
"                   Analyze every Nth frame.  Default 1, but increase to go\n"
"                   faster at the expense of timing precision.\n"
"  -g-, --no-gtk    Don't use GTK windows for illustrating progress.\n"
"  -h, --help       Display this message.\n"
"  -o, --output O   Save results in files with names starting O.\n"
"  -q, --quiet      Suppress various messages.\n"
"  -r, --raw V1     V1 is the video without subs.\n"
"  -s, --subs V2    V2 is the video with subs.\n"
"  -t, --transform (XP XT YP YT LP LT)\n"
"                   Start alignment search at specified transform, where\n"
"                   each capital variable is a floating point value,\n"
"                   X and Y are for spatial coordinates, L is luminosity,\n"
"                   P is a quantity added, and T a scale factor.\n"
<< std::endl;
  }

  Options get_options(int const argc, char* const* const argv)
  {
    optimal::OptionsParser parser;
    Options results;
    parser.addOption("align",          'a', &results.alignment_frame);
    parser.addOption("begin",          'b', &results.bounds.first);
    parser.addOption("chunking",       'c', &results.chunking_threshold);
    parser.addOption("data",           'd', &results.data);
    parser.addOption("end",            'e', &results.bounds.second);
    parser.addOption("frame-interval", 'f', &results.frame_interval);
    parser.addOption("gtk",            'g', &results.gtk);
    parser.addOption("help",           'h', &results.help);
    parser.addOption("output",         'o', &results.output);
    parser.addOption("quiet",          'q', &results.quiet);
    parser.addOption("raw",            'r', &results.raw);
    parser.addOption("subs",           's', &results.subs);
    parser.addOption("transform",      't', &results.start_params);

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

  if (options.raw.empty() || options.subs.empty() || options.output.empty()) {
    std::cerr <<
      "Error: must specify two video files and output (see --help)" <<
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

  try {
    ffmsxx::service ffms;

    auto raw_source = ffmsxx::get_single_video_source(ffms, options.raw);
    auto sub_source = ffmsxx::get_single_video_source(ffms, options.subs);

    boost::scoped_ptr<subliminal::visual_feedback> feedback;
    if (options.gtk) {
      feedback.reset(new subliminal::gtk_feedback(dataPath, std::cout));
    } else {
      feedback.reset(new subliminal::text_feedback(std::cout));
    }

    subliminal::extract_subtitles_options extract_options{
      options.alignment_frame, options.start_params,
      options.chunking_threshold, options.frame_interval,
      options.bounds, options.quiet
    };

    subliminal::output out(options.output);

    subliminal::extract_subtitles(
      raw_source, sub_source, extract_options, out, *feedback
    );
  } catch (ffmsxx::error const& e) {
    std::cerr << "Video error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

