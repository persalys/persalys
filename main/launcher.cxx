//                                               -*- C++ -*-
/**
 *  @brief Windows launcher
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <boost/version.hpp>
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

  std::unordered_map<std::string, std::string> env;
#if BOOST_VERSION >= 108800
  for (const auto entry : boost::process::environment::current())
    env.emplace(entry.key().string(), entry.value().string());
#else
  for (const auto entry : boost::this_process::environment())
    env.emplace(entry.get_name(), entry.to_string());
#endif

  // localization
  if (lang.size())
    env["LANG"] = lang;

  // disable OpenGL
  if (!opengl)
    env["PERSALYS_NO_GL"] = "1";

#ifdef _WIN32
  // Python PATH
  const std::string path_key = env.count("Path") ? "Path" : "PATH";
  const std::string old_path = env[path_key];
  const std::string new_path = python_dir.string() + ";" + (python_dir / "Scripts").string() + (old_path.size() ? std::string(";") + old_path : "");
  env[path_key] = new_path;
  if (verbose)
    std::cout << path_key << "=" << new_path << std::endl;

  // PYTHONHOME
  const std::string PYTHONHOME = python_dir.string();
  env["PYTHONHOME"] = PYTHONHOME;
  if (verbose)
    std::cout << "PYTHONHOME=" << PYTHONHOME << std::endl;

  // PYTHONUSERBASE
  const std::string env_appdata = env["APPDATA"];
  const fs::path pythonuserbase_dir = env_appdata.size() ? (fs::canonical(env_appdata) / "Persalys") : fs::path();
  if (!pythonuserbase_dir.empty())
  {
    env["PYTHONUSERBASE"] = pythonuserbase_dir.string();
    if (verbose)
      std::cout << "PYTHONUSERBASE=" << pythonuserbase_dir.string() << std::endl;

    // we cannot load from PYTHONUSERBASE if the directory doesnt exist prior to app launch
    try
    {
      fs::create_directories(pythonuserbase_dir / "Python314" / "site-packages");
    }
    catch (const std::filesystem::filesystem_error &)
    {
    }
  }

  // PYTHONPATH
  const std::string PYTHONPATH = (python_dir / "python314.zip").string() + ";" + persalys_dir.parent_path().string()
    + (pythonuserbase_dir.empty() ? "" : ";" + (pythonuserbase_dir / "Python314" / "site-packages").string());
  env["PYTHONPATH"] = PYTHONPATH;
  if (verbose)
    std::cout << "PYTHONPATH=" << PYTHONPATH << std::endl;

  // Paraview plugin dir
  const std::string PV_PLUGIN_PATH = (persalys_dir / "BagPlotViewsAndFilters").string();
  env["PV_PLUGIN_PATH"] = PV_PLUGIN_PATH;
  if (verbose)
    std::cout << "PV_PLUGIN_PATH=" << PV_PLUGIN_PATH << std::endl;

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
    std::cout << "PERSALYS_HTML_PATH=" << PERSALYS_HTML_PATH << std::endl;
#endif

// fork process
const std::string persalys_exe = (persalys_dir / "persalys.exe").string();
if (verbose)
  std::cout << "persalys_exe=" << persalys_exe << std::endl;
#if BOOST_VERSION >= 108800
  boost::asio::io_context ctx;
  bp::process proc(ctx, persalys_exe, {}, bp::process_environment(env));
  if (wait)
    return proc.wait();
  else
    proc.detach();
#else
  std::vector<std::string> env_vector;
  env_vector.reserve(env.size());
  for (const auto & entry : env)
    env_vector.push_back(entry.first + "=" + entry.second);
  bp::child proc(persalys_exe, env_vector);
  if (wait)
  {
    proc.wait();
    return proc.exit_code();
  }
  else
    proc.detach();
#endif
  return EXIT_SUCCESS;
}
