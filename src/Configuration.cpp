#include "Configuration.h"
#include "version.h"
#include <iostream>
#include <fstream>
#include <cpptoml.h>

namespace Names {

const char *configFile = "ncline.ini";

namespace CMake {
	const char *table = "cmake";
	const char *executable = "executable";
	const char *withNinja = "ninja";
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

bool Configuration::withNinja() const
{
	return cmakeSection_->get_as<bool>(Names::CMake::withNinja).value_or(false);
}

void Configuration::setWithNinja(bool value)
{
	cmakeSection_->insert(Names::CMake::withNinja, value);
}

void Configuration::save()
{
	//std::cout << *root_;

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
	cmakeSection_ = root_->get_table(Names::CMake::table);
	if (cmakeSection_ == nullptr)
	{
		cmakeSection_ = cpptoml::make_table();
		root_->insert(Names::CMake::table, cmakeSection_);
	}
}
