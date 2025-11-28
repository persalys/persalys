//                                               -*- C++ -*-
/**
 *  @brief Intermediate coupling step
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/CouplingPhysicalModel.hxx"
#include "persalys/BaseTools.hxx"
#include "persalys/FileMemoizeFunction.hxx"

#include <openturns/PersistentObjectFactory.hxx>

using namespace OT;

namespace PERSALYS
{

CLASSNAMEINIT(CouplingPhysicalModel)

static Factory<CouplingPhysicalModel> Factory_CouplingPhysicalModel;
static Factory<PersistentCollection<CouplingStep> > Factory_PersistentCollectionCouplingStep;

/* Default constructor */
CouplingPhysicalModel::CouplingPhysicalModel(const OT::String & name,
    const CouplingStepCollection & steps)
  : PythonPhysicalModel(name)
{
  setSteps(steps);
}

/* Default constructor */
CouplingPhysicalModel::CouplingPhysicalModel(const OT::String & name,
    const InputCollection & inputs,
    const OutputCollection & outputs,
    const CouplingStepCollection & steps)
  : PythonPhysicalModel(name)
{
  PhysicalModelImplementation::setInputs(inputs);
  PhysicalModelImplementation::setOutputs(outputs);
  setSteps(steps);
}

/* Virtual constructor */
CouplingPhysicalModel* CouplingPhysicalModel::clone() const
{
  return new CouplingPhysicalModel(*this);
}

/** Accessor to the steps */
CouplingStepCollection CouplingPhysicalModel::getSteps() const
{
  return steps_;
}

void CouplingPhysicalModel::setSSHHostname(const String & hostname)
{
  hostname_ = hostname;
  updateCode();
}

String CouplingPhysicalModel::getStepsMacro(const String & offset) const
{
  OSS oss;
  oss << offset << "steps = []\n";
  for (UnsignedInteger i = 0; i < steps_.getSize(); ++ i)
  {
    const CouplingStep step(steps_[i]);
    const CouplingInputFileCollection inputFiles(step.getInputFiles());
    oss << offset << "input_files = []\n";
    for (UnsignedInteger j = 0; j < inputFiles.getSize(); ++ j)
    {
      const CouplingInputFile inputFile(inputFiles[j]);
      if (inputFile.getPath().empty())
        continue;
      oss << offset << "input_file" << j << " = persalys.CouplingInputFile(r'"
          << inputFile.getPath() << "')\n";
      oss << offset << "input_file" << j << ".setConfiguredPath(r'"
          << inputFile.getConfiguredPath() << "')\n";
      if (!inputFile.getVariableNames().isEmpty())
        oss << offset << "input_file" << j << ".setVariables("
            << Parameters::GetOTDescriptionStr(inputFile.getVariableNames()) << ", "
            << Parameters::GetOTDescriptionStr(inputFile.getTokens()) << ", "
            << Parameters::GetOTDescriptionStr(inputFile.getFormats()) << ")\n";
      oss << offset << "input_files.append(input_file" << j << ")\n";
    }
    const CouplingResourceFileCollection resourceFiles(step.getResourceFiles());
    oss << offset << "resource_files = []\n";
    for (UnsignedInteger j = 0; j < resourceFiles.getSize(); ++ j)
    {
      const CouplingResourceFile resourceFile(resourceFiles[j]);
      if (resourceFile.getPath().empty())
        continue;
      oss << offset << "resource_file" << j << " = persalys.CouplingResourceFile(r'"
          << resourceFile.getPath() << "')\n";
      oss << offset << "resource_files.append(resource_file" << j << ")\n";
    }
    const CouplingOutputFileCollection outputFiles(step.getOutputFiles());
    oss << offset << "output_files = []\n";
    for (UnsignedInteger j = 0; j < outputFiles.getSize(); ++ j)
    {
      const CouplingOutputFile outputFile(outputFiles[j]);
      if (outputFile.getPath().empty())
        continue;
      oss << offset << "output_file" << j << " = persalys.CouplingOutputFile(r'"
          << outputFile.getPath() << "')\n";
      if (!outputFile.getVariableNames().isEmpty())
        oss << offset << "output_file" << j
            << ".setVariables("
            << Parameters::GetOTDescriptionStr(outputFile.getVariableNames())
            << ", " << Parameters::GetOTDescriptionStr(outputFile.getTokens())
            << ", " << Parameters::GetOTPointStr(outputFile.getSkipTokens())
            << ", " << Parameters::GetOTPointStr(outputFile.getSkipLines())
            << ", " << Parameters::GetOTPointStr(outputFile.getSkipColumns()) << ")\n";
      oss << offset << "output_files.append(output_file" << j << ")\n";
    }
    // escape backslashes and single quotes
    oss << offset << "step" << i << " = persalys.CouplingStep(r'"
        << step.getCommand() << "', input_files, resource_files, output_files)\n";
    oss << offset << "step" << i << ".setIsShell(" << (step.getIsShell() ? "True" : "False") << ")\n";
    if(!step.getCode().empty())
      oss << offset << "step" << i << ".setCode(\"" << step.getEscapedCode() << "\")\n";
    oss << offset << "step" << i << ".setEnvironment("
        << Parameters::GetOTDescriptionStr(step.getEnvironmentKeys()) << ", "
        << Parameters::GetOTDescriptionStr(step.getEnvironmentValues()) << ")\n";

    oss << offset << "steps.append(step" << i << ")\n";
  }
  return oss;
}

void CouplingPhysicalModel::setSteps(const CouplingStepCollection & steps)
{
  steps_ = steps;
  updateCode();
}

Description CouplingPhysicalModel::getStepsOutputNames(const CouplingStepCollection &steps) const
{
  Description outputNames;

  // retrieve output variables
  for (UnsignedInteger i = 0; i < steps.getSize(); ++ i)
  {
    const CouplingStep step(steps[i]);
    const CouplingOutputFileCollection outputFiles(step.getOutputFiles());
    for (UnsignedInteger j = 0; j < outputFiles.getSize(); ++ j)
    {
      const CouplingOutputFile outputFile(outputFiles[j]);
      if (outputFile.getPath().empty())
        continue;
      const Description variableNames(outputFile.getVariableNames());
      for (UnsignedInteger k = 0; k < variableNames.getSize(); ++ k)
      {
        if (!outputNames.contains(variableNames[k]))
          outputNames.add(variableNames[k]);
      }
    }
    if (!step.getCode().empty())
      for (UnsignedInteger j = 0; j < step.getPPOutputs().getSize(); ++ j)
        if (!outputNames.contains(step.getPPOutputs()[j]))
          outputNames.add(step.getPPOutputs()[j]);
  }

  return outputNames;
}

Description CouplingPhysicalModel::getStepsInputNames(const CouplingStepCollection &steps, const Description &outputNames) const
{
  Description inputNames;

  // retrieve input variables
  for (UnsignedInteger i = 0; i < steps.getSize(); ++ i)
  {
    const CouplingStep step(steps[i]);
    const CouplingInputFileCollection inputFiles(step.getInputFiles());
    for (UnsignedInteger j = 0; j < inputFiles.getSize(); ++ j)
    {
      const CouplingInputFile inputFile(inputFiles[j]);
      if (inputFile.getPath().empty())
        continue;
      const Description variableNames(inputFile.getVariableNames());
      for (UnsignedInteger k = 0; k < variableNames.getSize(); ++ k)
      {
        if (!inputNames.contains(variableNames[k]) && !outputNames.contains(variableNames[k]))
          inputNames.add(variableNames[k]);
      }
    }
    if (!step.getCode().empty())
      for (UnsignedInteger j = 0; j < step.getPPInputs().getSize(); ++ j)
        if (!inputNames.contains(step.getPPInputs()[j]) &&
            !outputNames.contains(step.getPPInputs()[j]))
          inputNames.add(step.getPPInputs()[j]);
  }

  return inputNames;
}

String CouplingPhysicalModel::writeLocalCode(const Description & inputNames, 
  const Description & outputNames) const
{
  const String inputNamesStr(Parameters::GetOTDescriptionStr(inputNames, false, false));

  OSS code;
  code << "import tempfile\n";
  code << "import openturns.coupling_tools as otct\n";
  code << "import persalys\n";
  code << "import shutil\n";
  code << "import os\n";
  code << "import re\n";
  code << "import hashlib\n";
  code << "import struct\n";
  code << "from pathlib import Path\n\n";
  code << "FUNC_PATTERN = re.compile(r'def\\s+(\\w+)\\(.*?\\):')\n";
  code << "ARGS_PATTERN = re.compile(r'def\\s+\\w+\\(([\\w, ]*)\\):')\n";
  code << "RETURN_PATTERN = re.compile(r'return\\s+([\\w, ]+)')\n\n";
  code << "def _exec(" << inputNamesStr << "):\n";
  code << getStepsMacro("    ");
  code << "    all_vars = dict(zip(" << Parameters::GetOTDescriptionStr(inputNames) << ", [" << inputNamesStr << "]))\n";
  code << "    checksum = hashlib.sha1()\n";
  code << "    for value in all_vars.values():\n";
  code << "        checksum.update(hex(struct.unpack('<Q', struct.pack('<d', value))[0]).encode())\n";
  code << "    global workdir\n";
  if (!workDir_.empty())
    code << "    workdir = Path(r'" << workDir_ << "') / ('persalys_' + checksum.hexdigest())\n";
  else
    code << "    workdir = Path(tempfile.gettempdir()) / ('persalys_' + checksum.hexdigest())\n";
  code << "    workdir = workdir.resolve()\n";
  code << "    if not workdir.exists():\n";
  code << "        workdir.mkdir(parents=True, exist_ok=True)\n";
  code << "    def _require_var(var_name):\n";
  code << "        if var_name not in all_vars:\n";
  code << "            raise KeyError(f\"Variable '{var_name}' is undefined\")\n";
  code << "        return all_vars[var_name]\n";
  code << "    for step in steps:\n";
  code << "        for input_file in step.getInputFiles():\n";
  code << "            if not input_file.getPath():\n";
  code << "                continue\n";
  code << "            input_values = [_require_var(varname) for varname in input_file.getVariableNames()]\n";
  code << "            formats = input_file.getFormats()\n";
  code << "            if formats.isBlank():\n";
  code << "                formats = None\n";
  code << "            target = workdir / input_file.getConfiguredPath()\n";
  code << "            target.parent.mkdir(parents=True, exist_ok=True)\n";
  code << "            otct.replace(input_file.getPath(), str(target), input_file.getTokens(), input_values, formats=formats, encoding=step.getEncoding())\n";
  code << "        for resource_file in step.getResourceFiles():\n";
  code << "            if not resource_file.getPath():\n";
  code << "                continue\n";
  code << "            src_path = Path(resource_file.getPath())\n";
  code << "            if src_path.is_file():\n";
  code << "                shutil.copy2(src_path, workdir / src_path.name)\n";
  code << "            elif src_path.is_dir():\n";
  code << "                shutil.copytree(src_path, workdir / src_path.name, dirs_exist_ok=True)\n";
  code << "            else:\n";
  code << "                raise FileNotFoundError(resource_file.getPath())\n";
  code << "        if len(step.getCommand()) > 0:\n";
  code << "            timeout = step.getTimeOut()\n";
  code << "            if timeout <= 0:\n";
  code << "                timeout = None\n";
  code << "            if len(step.getEnvironmentKeys()) == 0:\n";
  code << "                otct.execute(step.getCommand(), cwd=str(workdir), shell=step.getIsShell(), capture_output=True, timeout=timeout)\n";
  code << "            else:\n";
  code << "                env = os.environ.copy()\n";
  code << "                for key, val in zip(step.getEnvironmentKeys(), step.getEnvironmentValues()):\n";
  code << "                    env[key] = val\n";
  code << "                otct.execute(step.getCommand(), cwd=str(workdir), shell=step.getIsShell(), capture_output=True, timeout=timeout, env=env)\n";
  code << "        for output_file in step.getOutputFiles():\n";
  code << "            if not output_file.getPath():\n";
  code << "                continue\n";
  code << "            outfile = workdir / output_file.getPath()\n";
  code << "            outfile_str = str(outfile)\n";
  code << "            for varname, token, skip_tok, skip_line, skip_col in zip(output_file.getVariableNames(), output_file.getTokens(), output_file.getSkipTokens(), output_file.getSkipLines(), output_file.getSkipColumns()):\n";
  code << "                token_esc = re.escape(token)\n";
  code << "                all_vars[varname] = otct.get_value(outfile_str, token=token_esc, skip_token=int(skip_tok), skip_line=int(skip_line), skip_col=int(skip_col), encoding=step.getEncoding())\n";
  code << "        if step.getCode():\n";
  code << "            script = step.getCode()\n";
  code << "            func_match = FUNC_PATTERN.search(script)\n";
  code << "            if func_match is None:\n";
  code << "                raise RuntimeError('Could not find extra processing function name')\n";
  code << "            script_funcname = func_match.group(1)\n";
  code << "            args_match = ARGS_PATTERN.search(script)\n";
  code << "            if args_match is not None and args_match.group(1):\n";
  code << "                script_invars = [name for name in args_match.group(1).replace(' ', '').split(',') if name]\n";
  code << "            else:\n";
  code << "                script_invars = []\n";
  code << "            return_match = RETURN_PATTERN.search(script)\n";
  code << "            if return_match is not None and return_match.group(1):\n";
  code << "                script_outvars = [name for name in return_match.group(1).replace(' ', '').split(',') if name]\n";
  code << "            else:\n";
  code << "                script_outvars = []\n";
  code << "            exec_script = script+'\\nscript_output__ = '+script_funcname+'('+ ', '.join([str(all_vars[var]) for var in script_invars]) + ')\\n'\n";
  code << "            local_dict = globals()\n";
  code << "            exec(exec_script, globals(), local_dict)\n";
  code << "            script_output__ = local_dict['script_output__']\n";
  code << "            if len(script_outvars) == 1:\n";
  code << "                script_output__ = [script_output__]\n";
  code << "            for var, value in zip(script_outvars, script_output__):\n";
  code << "                all_vars[var] = value\n";

  if (cleanupWorkDirectory_)
    code << "    shutil.rmtree(workdir)\n";

  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << "    " << outputNames[i] << " = all_vars['" << outputNames[i] << "']\n";
  }
  code << "    return " << Parameters::GetOTDescriptionStr(outputNames, false, false) << "\n";
  return code;
}

String CouplingPhysicalModel::writeRemoteCode(const Description & inputNames, const Description & outputNames) const
{
  const String inputNamesStr(Parameters::GetOTDescriptionStr(inputNames, false, false));

  OSS code;
  code << "from tempfile import gettempdir\n";
  code << "import openturns.coupling_tools as otct\n";
  code << "import persalys\n";
  code << "import shutil\n";
  code << "import os\n";
  code << "import re\n";
  code << "import hashlib\n";
  code << "import struct\n";
  code << "from pathlib import Path, PurePosixPath\n";
  code << "import paramiko\n";
  code << "\n";
  code << "FUNC_PATTERN = re.compile(r'def\\s+(\\w+)\\(.*?\\):')\n";
  code << "ARGS_PATTERN = re.compile(r'def\\s+\\w+\\(([\\w, ]*)\\):')\n";
  code << "RETURN_PATTERN = re.compile(r'return\\s+([\\w, ]+)')\n\n";
  code << "def _exec(" << inputNamesStr << "):\n";
  code << getStepsMacro("    ");
  code << "    all_vars = dict(zip(" << Parameters::GetOTDescriptionStr(inputNames) << ", [" << inputNamesStr << "]))\n";
  code << "    checksum = hashlib.sha1()\n";
  code << "    for value in all_vars.values():\n";
  code << "        checksum.update(hex(struct.unpack('<Q', struct.pack('<d', value))[0]).encode())\n";
  code << "    global workdir\n";
  code << "    workdir = Path(gettempdir()) / ('persalys_' + checksum.hexdigest())\n";
  code << "    workdir = workdir.resolve()\n";
  code << "    if not workdir.exists():\n";
  code << "        workdir.mkdir(parents=True, exist_ok=True)\n";
  code << "    def _require_var(var_name):\n";
  code << "        if var_name not in all_vars:\n";
  code << "            raise KeyError(f\"Variable '{var_name}' is undefined\")\n";
  code << "        return all_vars[var_name]\n";
  code << "    hostname = r'" << hostname_ << "'\n";
  if (!workDir_.empty())
    code << "    remote_base = PurePosixPath(r'" << workDir_ << "')\n";
  else
    code << "    remote_base = PurePosixPath('/tmp')\n";
  code << "    remote_workdir = remote_base / ('persalys_' + checksum.hexdigest())\n";
  code << "    ssh = paramiko.SSHClient()\n";
  code << "    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())\n";
  code << "    ssh.connect(hostname)\n";
  code << "    sftp = ssh.open_sftp()\n";
  code << "    def _remote_mkdir_p(path: PurePosixPath):\n";
  code << "        # use Unix mkdir -p on the remote host, ensure proper quoting\n";
  code << "        path_str = str(path)\n";
  code<<R"(        safe = path_str.replace("'", "'\\''"))" << "\n";
  code<<R"(        cmd = f"mkdir -p '{safe}'")" << "\n";
  code << "        stdin, stdout, stderr = ssh.exec_command(cmd)\n";
  code << "        ret = stdout.channel.recv_exit_status()\n";
  code << "        if ret != 0:\n";
  code << "            raise RuntimeError(f\"Remote mkdir failed for {path_str} with code {ret}: {stderr.read().decode()}\")\n";
  code << "    _remote_mkdir_p(remote_workdir)\n";
  code << "    for step in steps:\n";
  code << "        for input_file in step.getInputFiles():\n";
  code << "            if not input_file.getPath():\n";
  code << "                continue\n";
  code << "            input_values = [_require_var(varname) for varname in input_file.getVariableNames()]\n";
  code << "            formats = input_file.getFormats()\n";
  code << "            if formats.isBlank():\n";
  code << "                formats = None\n";
  code << "            target = workdir / input_file.getConfiguredPath()\n";
  code << "            target.parent.mkdir(parents=True, exist_ok=True)\n";
  code << "            otct.replace(input_file.getPath(), str(target), input_file.getTokens(), input_values, formats=formats, encoding=step.getEncoding())\n";
  code << "            remote_target = remote_workdir / input_file.getConfiguredPath()\n";
  code << "            _remote_mkdir_p(remote_target.parent)\n";
  code << "            sftp.put(str(target), str(remote_target))\n";
  code << "        for resource_file in step.getResourceFiles():\n";
  code << "            if not resource_file.getPath():\n";
  code << "                continue\n";
  code << "            src_path = Path(resource_file.getPath())\n";
  code << "            if src_path.is_file():\n";
  code << "                local_res = workdir / src_path.name\n";
  code << "                shutil.copy2(src_path, local_res)\n";
  code << "                remote_res = remote_workdir / src_path.name\n";
  code << "                _remote_mkdir_p(remote_res.parent)\n";
  code << "                sftp.put(str(local_res), str(remote_res))\n";
  code << "            elif src_path.is_dir():\n";
  code << "                dst_dir = workdir / src_path.name\n";
  code << "                shutil.copytree(src_path, dst_dir, dirs_exist_ok=True)\n";
  code << "                for root, dirs, files in os.walk(dst_dir):\n";
  code << "                    rel_root = Path(root).relative_to(workdir)\n";
  code<<R"(                    remote_root = remote_workdir / PurePosixPath(str(rel_root).replace('\\', '/')))" << "\n";
  code << "                    _remote_mkdir_p(remote_root)\n";
  code << "                    for f in files:\n";
  code << "                        local_f = Path(root) / f\n";
  code << "                        remote_f = remote_root / f\n";
  code << "                        sftp.put(str(local_f), str(remote_f))\n";
  code << "            else:\n";
  code << "                raise FileNotFoundError(resource_file.getPath())\n";
  code << "        if len(step.getCommand()) > 0:\n";
  code << "            timeout = step.getTimeOut()\n";
  code << "            if timeout <= 0:\n";
  code << "                timeout = None\n";
  code << "            cmd = step.getCommand()\n";
  code << "            # emulate cwd and optional shell using bash -lc\n";
  code << "            remote_cmd = f\"cd {remote_workdir} && {cmd}\"\n";
  code << "            env = {}\n";
  code << "            if len(step.getEnvironmentKeys()) > 0:\n";
  code << "                for key, val in zip(step.getEnvironmentKeys(), step.getEnvironmentValues()):\n";
  code << "                    env[key] = val\n";
  code << "            # paramiko exec_command supports an environment dict on Unix servers\n";
  code << "            stdin, stdout, stderr = ssh.exec_command(\n";
  code << "                f\"sh -c '{remote_cmd}'\",\n";
  code << "                timeout=timeout,\n";
  code << "                environment=env,\n";
  code << "            )\n";
  code << "            ret = stdout.channel.recv_exit_status()\n";
  code << "            if ret != 0:\n";
  code << "                raise RuntimeError(f\"Remote command failed with code {ret}: {stderr.read().decode()}\\n{stdout.read().decode()}\")\n";
  code << "        for output_file in step.getOutputFiles():\n";
  code << "            if not output_file.getPath():\n";
  code << "                continue\n";
  code << "            remote_out = remote_workdir / output_file.getPath()\n";
  code << "            local_out = workdir / output_file.getPath()\n";
  code << "            local_out.parent.mkdir(parents=True, exist_ok=True)\n";
  code << "            _remote_mkdir_p(remote_out.parent)\n";
  code << "            try:\n";
  code << "                sftp.get(str(remote_out), str(local_out))\n";
  code << "            except IOError:\n";
  code << "                raise FileNotFoundError(str(remote_out))\n";
  code << "            outfile_str = str(local_out)\n";
  code << "            for varname, token, skip_tok, skip_line, skip_col in zip(output_file.getVariableNames(), output_file.getTokens(), output_file.getSkipTokens(), output_file.getSkipLines(), output_file.getSkipColumns()):\n";
  code << "                token_esc = re.escape(token)\n";
  code << "                all_vars[varname] = otct.get_value(outfile_str, token=token_esc, skip_token=int(skip_tok), skip_line=int(skip_line), skip_col=int(skip_col), encoding=step.getEncoding())\n";
  code << "        if step.getCode():\n";
  code << "            script = step.getCode()\n";
  code << "            func_match = FUNC_PATTERN.search(script)\n";
  code << "            if func_match is None:\n";
  code << "                raise RuntimeError('Could not find extra processing function name')\n";
  code << "            script_funcname = func_match.group(1)\n";
  code << "            args_match = ARGS_PATTERN.search(script)\n";
  code << "            if args_match is not None and args_match.group(1):\n";
  code << "                script_invars = [name for name in args_match.group(1).replace(' ', '').split(',') if name]\n";
  code << "            else:\n";
  code << "                script_invars = []\n";
  code << "            return_match = RETURN_PATTERN.search(script)\n";
  code << "            if return_match is not None and return_match.group(1):\n";
  code << "                script_outvars = [name for name in return_match.group(1).replace(' ', '').split(',') if name]\n";
  code << "            else:\n";
  code << "                script_outvars = []\n";
  code << "            exec_script = script+'\\nscript_output__ = '+script_funcname+'('+ ', '.join([str(all_vars[var]) for var in script_invars]) + ')\\n'\n";
  code << "            local_dict = globals()\n";
  code << "            exec(exec_script, globals(), local_dict)\n";
  code << "            script_output__ = local_dict['script_output__']\n";
  code << "            if len(script_outvars) == 1:\n";
  code << "                script_output__ = [script_output__]\n";
  code << "            for var, value in zip(script_outvars, script_output__):\n";
  code << "                all_vars[var] = value\n";
  code << "    sftp.close()\n";
  if (cleanupWorkDirectory_)
  {
    code << "    try:\n";
    code<<R"(        safe_remote = str(remote_workdir).replace("'", "'\\''"))" << "\n";
    code << "        stdin, stdout, stderr = ssh.exec_command(f\"rm -rf '{safe_remote}'\")\n";
    code << "        stdout.channel.recv_exit_status()\n";
    code << "    except Exception:\n";
    code << "        pass\n";
  }
  code << "    ssh.close()\n";
  code << "    shutil.rmtree(workdir, ignore_errors=True)\n";
  for (UnsignedInteger i = 0; i < outputNames.getSize(); ++ i)
  {
    code << "    " << outputNames[i] << " = all_vars['" << outputNames[i] << "']\n";
  }
  code << "    return " << Parameters::GetOTDescriptionStr(outputNames, false, false) << "\n";

  return code;
}

void CouplingPhysicalModel::updateCode()
{
  const CouplingStepCollection steps = getSteps();
  const Description outputNames{getStepsOutputNames(steps)};
  const Description inputNames{getStepsInputNames(steps, outputNames)};

  OT::String code;
  if (hostname_.empty())
    code = writeLocalCode(inputNames, outputNames);
  else
    code = writeRemoteCode(inputNames, outputNames);

  PythonPhysicalModel::setCode(code);

  notify("stepsChanged");
}

Function CouplingPhysicalModel::generateFunction(const Description & outputNames) const
{
  return FileMemoizeFunction(PythonPhysicalModel::generateFunction(outputNames), getCacheInputFile(), getCacheOutputFile());
}


String CouplingPhysicalModel::getHTMLDescription() const
{
  OSS oss;
  oss << PhysicalModelImplementation::getHTMLDescription();
  oss << "<h3>Outputs</h3><p>";
  oss << R"(<table style="width:100%" border="1" cellpadding="5">)";
  oss << "<tr>";
  oss << "  <th>Name</th>";
  oss << "  <th>Description</th>";
  oss << "</tr>";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++i)
  {
    oss << "<tr>";
    oss << "  <td>" << getOutputNames()[i] << "</td>";
    const String desc(getOutputs()[i].getDescription());
    oss << "  <td>" << (desc.empty() ? "-" : desc) << "</td>";
    oss << "</tr>";
  }
  oss << "</table></p>";
  for (UnsignedInteger i = 0; i < steps_.getSize(); ++i)
  {
    const CouplingStep step(steps_[i]);
    oss << "<h3>Step " << i << "</h3>";
    oss << "<section><p>";
    oss << step.getCommand();
    oss << "</p></section>";
  }
  return oss;
}


String CouplingPhysicalModel::getPythonScript() const
{
  OSS oss;

  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
    oss << getInputs()[i].getPythonScript();

  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
    oss << getOutputs()[i].getPythonScript();

  oss << "inputs = [";
  for (UnsignedInteger i = 0; i < getInputDimension(); ++ i)
  {
    String inputName(getInputs()[i].getName());
    inputName = std::regex_replace(inputName, std::regex("[^0-9a-zA-Z_]"), "_");
    oss << inputName;
    if (i < getInputDimension() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << "outputs = [";
  for (UnsignedInteger i = 0; i < getOutputDimension(); ++ i)
  {
    String outputName(getOutputs()[i].getName());
    std::replace(outputName.begin(), outputName.end(), '.', '_');
    oss << outputName;
    if (i < getOutputDimension() - 1)
      oss << ", ";
  }
  oss << "]\n";

  oss << getStepsMacro();
  oss << getName() + " = persalys." << getClassName() << "('" << getName() << "', inputs, outputs, steps)\n";
  oss << getName() + ".setCleanupWorkDirectory(" << (getCleanupWorkDirectory() ? "True" : "False") << ")\n";
  oss << getName() + ".setCacheFiles(r'" << getCacheInputFile()
      << "', r'" << getCacheOutputFile() << "')\n";
  oss << getName() + ".setSSHHostname('" << getSSHHostname() << "')\n";
  oss << PhysicalModelImplementation::getCopulaPythonScript();

  return oss;
}


/** String converter */
String CouplingPhysicalModel::__repr__() const
{
  OSS oss;
  oss << PhysicalModelImplementation::__repr__()
      << " steps=" << getSteps()
      << " hostname=" << hostname_;
  return oss;
}

/** Whether the work dir is discarded */
void CouplingPhysicalModel::setCleanupWorkDirectory(const Bool cleanupWorkDirectory)
{
  cleanupWorkDirectory_ = cleanupWorkDirectory;
  updateCode();
}

Bool CouplingPhysicalModel::getCleanupWorkDirectory() const
{
  return cleanupWorkDirectory_;
}

void CouplingPhysicalModel::setCacheFiles(const OT::FileName & inputFile, const OT::FileName & outputFile)
{
  cacheInputFile_   = inputFile;
  cacheOutputFile_  = outputFile;
}

void CouplingPhysicalModel::setWorkDir(const OT::FileName & workDir)
{
  workDir_ = workDir;
  updateCode();
}


OT::FileName CouplingPhysicalModel::getWorkDir() const
{
  return workDir_;
}

OT::FileName CouplingPhysicalModel::getCacheInputFile() const
{
  return cacheInputFile_;
}

OT::FileName CouplingPhysicalModel::getCacheOutputFile() const
{
  return cacheOutputFile_;
}

/* Method save() stores the object through the StorageManager */
void CouplingPhysicalModel::save(Advocate & adv) const
{
  PythonPhysicalModel::save(adv);
  adv.saveAttribute("steps_", steps_);
  adv.saveAttribute("cleanupWorkDirectory_", cleanupWorkDirectory_);
  adv.saveAttribute("cacheInputFile_", cacheInputFile_);
  adv.saveAttribute("cacheOutputFile_", cacheOutputFile_);
  adv.saveAttribute("workDir_", workDir_);
  adv.saveAttribute("hostname_", hostname_);
}


/* Method load() reloads the object from the StorageManager */
void CouplingPhysicalModel::load(Advocate & adv)
{
  PythonPhysicalModel::load(adv);
  adv.loadAttribute("steps_", steps_);
  adv.loadAttribute("cleanupWorkDirectory_", cleanupWorkDirectory_);
  adv.loadAttribute("cacheInputFile_", cacheInputFile_);
  adv.loadAttribute("cacheOutputFile_", cacheOutputFile_);
  adv.loadAttribute("workDir_", workDir_);
  if (adv.hasAttribute("hostname_"))
    adv.loadAttribute("hostname_", hostname_);
}


}
