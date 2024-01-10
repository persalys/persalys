//                                               -*- C++ -*-
/**
 *  @brief Windows launcher
 *
 *  Copyright 2015-2023 EDF-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define BOOST_PROCESS_USE_STD_FS
#include <boost/process.hpp>
#include <boost/program_options.hpp>

#include <filesystem>
#include <cstdlib>
#include <iostream>

namespace po = boost::program_options;
namespace fs = std::filesystem;
namespace bp = boost::process;

int main(int argc, char *argv[])
{
  // handle options
  std::string lang;
  bool opengl = true;
  bool mesa = true;
  bool wait = false;
  bool verbose = false;
  po::options_description desc ("Allowed options");
  desc.add_options ()
    ("help,h", "print usage message")
    ("lang", po::value<std::string>(&lang), "Language en|fr")
    ("mesa", po::value<bool>(&mesa), "Enable software renderer 1|0")
    ("opengl", po::value<bool>(&opengl), "Enable OpenGL renderer 1|0")
    ("wait", po::value<bool>(&wait), "Wait process 1|0")
    ("verbose,V", po::value<bool>(&verbose), "Verbose mode 1|0");
  po::variables_map vm;
  po::store (po::command_line_parser (argc, argv).options (desc).run (), vm);
  po::notify (vm);
  if (verbose)
    std::cout << "lang=" << lang << " opengl=" << opengl << " mesa=" << mesa << " wait=" << wait << std::endl;

  // set env
  fs::path persalys_dir = fs::canonical(argv[0]).parent_path();
  fs::path python_dir = persalys_dir.parent_path().parent_path().parent_path();
  if (verbose)
  {
    std::cout << "persalys_dir=" << persalys_dir << std::endl;
    std::cout << "python_dir=" << python_dir << std::endl;
  }
  auto env = boost::this_process::environment();

  // localization
  if (lang.size())
    env["LANG"] = lang;

  // disable OpenGL
  if (!opengl)
    env["PERSALYS_NO_GL"] = "1";

#ifdef _WIN32
  // Python PATH
  const char* env_path = std::getenv("PATH");
  const std::string PATH = python_dir.string() + ";" + (python_dir / "Scripts").string() + (env_path ? std::string(";") + env_path : "");
  env["PATH"] = PATH;
  if (verbose)
    std::cout << "PATH=" << PATH <<std::endl;

  // PYTHONPATH
  const char* env_pythonpath = std::getenv("PYTHONPATH");
  const std::string PYTHONPATH = (python_dir / "python311.zip").string() + ";" + persalys_dir.parent_path().string() + (env_pythonpath ? std::string(";") + env_pythonpath : "");
  env["PYTHONPATH"] = PYTHONPATH;
  if (verbose)
    std::cout << "PYTHONPATH=" << PYTHONPATH <<std::endl;

  // PYTHONHOME
  const std::string PYTHONHOME = python_dir.string();
  env["PYTHONHOME"] = PYTHONHOME;
  if (verbose)
    std::cout << "PYTHONHOME=" << PYTHONHOME <<std::endl;

  // Paraview plugin dir
  const std::string PV_PLUGIN_PATH = (persalys_dir / "BagPlotViewsAndFilters").string();
  env["PV_PLUGIN_PATH"] = PV_PLUGIN_PATH;
  if (verbose)
    std::cout << "PV_PLUGIN_PATH=" << PV_PLUGIN_PATH <<std::endl;

  // matplotlib
  env["MPL_BACKEND"] = "TkAgg";

  // mesa / llvmpipe
  if (mesa)
  {
    // mesa driver
    env["GALLIUM_DRIVER"] = "llvmpipe";
    // tweak qt opengl library
    env["QT_OPENGL_DLL"] = "opengl32";
  }

  // doc dir
  const std::string PERSALYS_HTML_PATH = (persalys_dir / "doc" / "html").string();
  env["PERSALYS_HTML_PATH"] = PERSALYS_HTML_PATH;
  if (verbose)
    std::cout << "PERSALYS_HTML_PATH=" << PERSALYS_HTML_PATH <<std::endl;
#endif

  // fork process
  bp::child proc("persalys", env);
  if (wait)
  {
    proc.wait();
    return proc.exit_code();
  }
  else
    proc.detach();
  return EXIT_SUCCESS;
}
