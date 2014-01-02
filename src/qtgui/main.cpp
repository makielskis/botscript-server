// Copyright (c) 2013, makielski.net
// Licensed under the MIT license
// https://raw.github.com/makielski/botscript/master/COPYING

#include <fstream>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include <QtGui>
#include <QApplication>

#include "./Window.h"
#include "./BotThread.h"

#include "../bs_server_options.h"
#include "../ws_server_options.h"

#include "../botscript_server_version.h"

class QApplication;

using boost::filesystem::path;
using namespace botscript_server;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {
  Q_INIT_RESOURCE(systray);

  std::string exe_dir = path(argv[0]).parent_path().generic_string();
  chdir(exe_dir.c_str());

  // Prepare configurations with default values.
  std::string file;
  ws_server_options wss_options("127.0.0.1", "9003");
  bs_server_options bss_options(false, true, "packages");

  // Prepare program description.
  std::string config_file;
  po::options_description desc("\n\tMakielskis Bot v" + version()
                               + "\n\nOptions", 100);
  desc.add_options()
      ("help", "produce help message")
      ("config,c", po::value<std::string>(&file)->default_value("config.ini"),
          "name of a file for configuration.");
  wss_options.configure_description(desc);
  bss_options.configure_description(desc);

  // Positional argument: config file.
  po::positional_options_description p;
  p.add("config", -1);

  // Read cmdline arguments.
  po::variables_map vm;
  auto parsed = po::command_line_parser(argc, argv)
                  .options(desc)
                    .positional(p)
                    .allow_unregistered()
                  .run();
  po::store(parsed, vm);
  po::notify(vm);

  // Print help and exit if desired.
  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  // List unrecognized options.
  auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
  for (const auto& opt : unrecog) {
    std::cout << "Unrecognized option: " << opt << "\n";
  }

  // Read config file.
  std::ifstream ifs(file.c_str());
  if (!ifs) {
    std::cout << "Configuration file " << file << " not found, skipping\n";
  } else {
    po::store(po::parse_config_file(ifs, desc), vm);
    po::notify(vm);
  }

  // Read program options.
  wss_options.parse(vm);
  bss_options.parse(vm);

  // Print options.
  std::cout << "Used options:\n" << wss_options << bss_options << "\n";

  QApplication app(argc, argv);
  app.setOrganizationName("Makielski");
  app.setApplicationName("makielskisbot");
  QApplication::setQuitOnLastWindowClosed(false);

  BotThread server(wss_options, bss_options);
  server.start();

  Window window(server);
  window.hide();

  QObject::connect(&app, SIGNAL(aboutToQuit()), &window, SLOT(stop()));

  return app.exec();
}
