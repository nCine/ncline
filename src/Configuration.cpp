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
	const char *executable = "executable";
	const char *ninjaExecutable = "ninja_executable";
	const char *withNinja = "ninja";
	const char *withMinGW = "mingw";
	const char *vsVersion = "vs_version";
}

namespace nCine {
	const char *table = "ncine";
	const char *branch = "branch";
	const char *compiler = "compiler";
	const char *compilerGCC = "gcc";
	const char *compilerClang = "clang";
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
	bool valueFound = false;

	auto gitExe = gitSection_->get_as<std::string>(Names::Git::executable);
	if (gitExe)
	{
		value = *gitExe;
		valueFound = true;
	}

	return valueFound;
}

bool Configuration::cmakeExecutable(std::string &value) const
{
	bool valueFound = false;

	auto cmakeExe = cmakeSection_->get_as<std::string>(Names::CMake::executable);
	if (cmakeExe)
	{
		value = *cmakeExe;
		valueFound = true;
	}

	return valueFound;
}

bool Configuration::ninjaExecutable(std::string &value) const
{
	bool valueFound = false;

	auto ninjaExe = cmakeSection_->get_as<std::string>(Names::CMake::ninjaExecutable);
	if (ninjaExe)
	{
		value = *ninjaExe;
		valueFound = true;
	}

	return valueFound;
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

bool Configuration::branchName(std::string &value) const
{
	bool valueFound = false;

	auto name = ncineSection_->get_as<std::string>(Names::nCine::branch);
	if (name)
	{
		value = *name;
		valueFound = true;
	}

	return valueFound;
}

void Configuration::setBranchName(const std::string &value)
{
	ncineSection_->insert(Names::nCine::branch, value);
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
