#include <iostream>
#include <fstream>
#include <string>
#include <cpptoml.h>
#include "Configuration.h"
#include "version.h"

namespace Names {

const char *configFile = "ncline.ini";

const char *withColors = "colors";

namespace Git {
	const char *table = "git";
	const char *executable = "executable";
}

namespace CMake {
	const char *table = "cmake";
	const char *cmakeExecutable = "cmake_executable";
	const char *ninjaExecutable = "ninja_executable";
	const char *emcmakeExecutable = "emcmake_executable";
	const char *withEmscripten = "emscripten";
	const char *withNinja = "ninja";
	const char *withMinGW = "mingw";
	const char *vsVersion = "vs_version";
}

namespace nCine {
	const char *table = "ncine";
	const char *compiler = "compiler";
	const char *compilerGCC = "gcc";
	const char *compilerClang = "clang";
	const char *cmakeArguments = "cmake_arguments";
	const char *branch = "branch";
	const char *ncineDir = "ncine_dir";
	const char *gameName = "game_name";
}

}

Configuration &config()
{
	static Configuration instance;
	return instance;
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Configuration::Configuration()
{
	try
	{
		root_ = cpptoml::parse_file(Names::configFile);
	}
	catch (cpptoml::parse_exception)
	{
		root_ = cpptoml::make_table();
	}
	retrieveSections();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool Configuration::withColors() const
{
#if defined(_WIN32) || defined(__APPLE__)
	const bool defaultValue = false;
#else
	const bool defaultValue = true;
#endif

	return root_->get_as<bool>(Names::withColors).value_or(defaultValue);
}

void Configuration::setWithColors(bool value)
{
	root_->insert(Names::withColors, value);
}

bool Configuration::gitExecutable(std::string &value) const
{
	return retrieveString(gitSection_, Names::Git::executable, value);
}

void Configuration::setGitExecutable(const std::string &value)
{
	gitSection_->insert(Names::Git::executable, value);
}

bool Configuration::cmakeExecutable(std::string &value) const
{
	return retrieveString(cmakeSection_, Names::CMake::cmakeExecutable, value);
}

void Configuration::setCMakeExecutable(const std::string &value)
{
	cmakeSection_->insert(Names::CMake::cmakeExecutable, value);
}

bool Configuration::ninjaExecutable(std::string &value) const
{
	return retrieveString(cmakeSection_, Names::CMake::ninjaExecutable, value);
}

void Configuration::setNinjaExecutable(const std::string &value)
{
	cmakeSection_->insert(Names::CMake::ninjaExecutable, value);
}

bool Configuration::emcmakeExecutable(std::string &value) const
{
	return retrieveString(cmakeSection_, Names::CMake::emcmakeExecutable, value);
}

void Configuration::setEmcmakeExecutable(const std::string &value)
{
	cmakeSection_->insert(Names::CMake::emcmakeExecutable, value);
}

bool Configuration::withEmscripten() const
{
	return cmakeSection_->get_as<bool>(Names::CMake::withEmscripten).value_or(false);
}

void Configuration::setWithEmscripten(bool value)
{
	cmakeSection_->insert(Names::CMake::withEmscripten, value);
}

bool Configuration::withNinja() const
{
	return cmakeSection_->get_as<bool>(Names::CMake::withNinja).value_or(false);
}

void Configuration::setWithNinja(bool value)
{
	cmakeSection_->insert(Names::CMake::withNinja, value);
}

bool Configuration::withMinGW() const
{
	return cmakeSection_->get_as<bool>(Names::CMake::withMinGW).value_or(false);
}

void Configuration::setWithMinGW(bool value)
{
	cmakeSection_->insert(Names::CMake::withMinGW, value);
}

unsigned int Configuration::vsVersion() const
{
	return cmakeSection_->get_as<unsigned int>(Names::CMake::vsVersion).value_or(2019);
}

void Configuration::setVsVersion(unsigned int version)
{
	if (version != 2019 && version != 2017)
		cmakeSection_->insert(Names::CMake::vsVersion, 2019);
	else
		cmakeSection_->insert(Names::CMake::vsVersion, version);
}

Configuration::Compiler Configuration::compiler() const
{
#ifndef __APPLE__
	auto name = ncineSection_->get_as<std::string>(Names::nCine::compiler);
	if (name)
	{
		if (*name == Names::nCine::compilerGCC)
			return Compiler::GCC;
		else if (*name == Names::nCine::compilerClang)
			return Compiler::CLANG;
	}
#endif
	return Compiler::UNSPECIFIED;
}

void Configuration::setCompiler(Compiler compiler)
{
	switch (compiler)
	{
		case Compiler::GCC:
			ncineSection_->insert(Names::nCine::compiler, Names::nCine::compilerGCC);
			break;
		case Compiler::CLANG:
			ncineSection_->insert(Names::nCine::compiler, Names::nCine::compilerClang);
			break;
		case Compiler::UNSPECIFIED:
			ncineSection_->insert(Names::nCine::compiler, "");
			break;
	}
}

bool Configuration::engineCMakeArguments(std::string &value) const
{
	return retrieveString(ncineSection_, Names::nCine::cmakeArguments, value);
}

void Configuration::setEngineCMakeArguments(const std::string &value)
{
	ncineSection_->insert(Names::nCine::cmakeArguments, value);
}

bool Configuration::branchName(std::string &value) const
{
	return retrieveString(ncineSection_, Names::nCine::branch, value);
}

void Configuration::setBranchName(const std::string &value)
{
	ncineSection_->insert(Names::nCine::branch, value);
}

bool Configuration::hasEngineDir() const
{
	return hasString(ncineSection_, Names::nCine::ncineDir);
}

bool Configuration::engineDir(std::string &value) const
{
	return retrieveString(ncineSection_, Names::nCine::ncineDir, value);
}

void Configuration::setEngineDir(const std::string &value)
{
	ncineSection_->insert(Names::nCine::ncineDir, value);
}

bool Configuration::hasGameName() const
{
	return hasString(ncineSection_, Names::nCine::gameName);
}

bool Configuration::gameName(std::string &value) const
{
	return retrieveString(ncineSection_, Names::nCine::gameName, value);
}

void Configuration::setGameName(const std::string &value)
{
	ncineSection_->insert(Names::nCine::gameName, value);
}

void Configuration::print() const
{
	std::cout << *root_;
}

void Configuration::save()
{
	std::ofstream file;
	file.open(Names::configFile);
	file << *root_;
	file.close();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool Configuration::hasString(const std::shared_ptr<cpptoml::table> &section, const char *name) const
{
	auto string = section->get_as<std::string>(name);
	return string.operator bool();
}

bool Configuration::retrieveString(const std::shared_ptr<cpptoml::table> &section, const char *name, std::string &dest) const
{
	bool valueFound = false;

	auto string = section->get_as<std::string>(name);
	if (string)
	{
		dest = *string;
		valueFound = true;
	}

	return valueFound;
}

void Configuration::retrieveSections()
{
	gitSection_ = root_->get_table(Names::Git::table);
	if (gitSection_ == nullptr)
	{
		gitSection_ = cpptoml::make_table();
		root_->insert(Names::Git::table, gitSection_);
	}

	cmakeSection_ = root_->get_table(Names::CMake::table);
	if (cmakeSection_ == nullptr)
	{
		cmakeSection_ = cpptoml::make_table();
		root_->insert(Names::CMake::table, cmakeSection_);
	}

	ncineSection_ = root_->get_table(Names::nCine::table);
	if (ncineSection_ == nullptr)
	{
		ncineSection_ = cpptoml::make_table();
		root_->insert(Names::nCine::table, ncineSection_);
	}
}
