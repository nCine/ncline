#include <iostream>
#include <fstream>
#include <string>
#include <cpptoml.h>
#include "Configuration.h"
#include "version.h"

namespace Names {

const char *configFile = "ncline.ini";

const char *withColors = "colors";

namespace Executables {
	const char *table = "executables";
	const char *git = "git";
	const char *cmake = "cmake";
	const char *ninja = "ninja";
	const char *emcmake = "emcmake";
	const char *doxygen = "doxygen";
}

namespace CMake {
	const char *table = "cmake";
	const char *withNinja = "ninja";
	const char *withMinGW = "mingw";
	const char *vsVersion = "vs_version";
	const char *prefixPath = "prefix_path";
}

namespace nCine {
	const char *table = "ncine";
	const char *platform = "platform";
	const char *platformDesktop = "desktop";
	const char *platformAndroid = "android";
	const char *platformEmscripten = "emscripten";
	const char *compiler = "compiler";
	const char *compilerGCC = "gcc";
	const char *compilerClang = "clang";
	const char *cmakeArguments = "cmake_arguments";
	const char *branch = "branch";
	const char *ncineDir = "ncine_dir";
	const char *gameName = "game_name";
}

namespace Android {
	const char *table = "android";
	const char *architecture = "architecture";
	const char *archARMv7a = "armeabi-v7a";
	const char *archARM64 = "arm64-v8a";
	const char *archX86_64 = "x86_64";
	const char *sdkDir = "sdk_dir";
	const char *ndkDir = "ndk_dir";
	const char *gradleDir = "gradle_dir";
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
	return retrieveString(executablesSection_, Names::Executables::git, value);
}

void Configuration::setGitExecutable(const std::string &value)
{
	executablesSection_->insert(Names::Executables::git, value);
}

bool Configuration::cmakeExecutable(std::string &value) const
{
	return retrieveString(executablesSection_, Names::Executables::cmake, value);
}

void Configuration::setCMakeExecutable(const std::string &value)
{
	executablesSection_->insert(Names::Executables::cmake, value);
}

bool Configuration::ninjaExecutable(std::string &value) const
{
	return retrieveString(executablesSection_, Names::Executables::ninja, value);
}

void Configuration::setNinjaExecutable(const std::string &value)
{
	executablesSection_->insert(Names::Executables::ninja, value);
}

bool Configuration::emcmakeExecutable(std::string &value) const
{
	return retrieveString(executablesSection_, Names::Executables::emcmake, value);
}

void Configuration::setEmcmakeExecutable(const std::string &value)
{
	executablesSection_->insert(Names::Executables::emcmake, value);
}

bool Configuration::doxygenExecutable(std::string &value) const
{
	return retrieveString(executablesSection_, Names::Executables::doxygen, value);
}

void Configuration::setDoxygenExecutable(const std::string &value)
{
	executablesSection_->insert(Names::Executables::doxygen, value);
}

Configuration::Platform Configuration::platform() const
{
	auto name = ncineSection_->get_as<std::string>(Names::nCine::platform);
	if (name)
	{
		if (*name == Names::nCine::platformDesktop)
			return Platform::DESKTOP;
		else if (*name == Names::nCine::platformAndroid)
			return Platform::ANDROID;
		else if (*name == Names::nCine::platformEmscripten)
			return Platform::EMSCRIPTEN;
	}
	return Platform::DESKTOP;
}

void Configuration::setPlatform(Platform platform)
{
	switch (platform)
	{
		case Platform::DESKTOP:
			ncineSection_->insert(Names::nCine::platform, Names::nCine::platformDesktop);
			break;
		case Platform::ANDROID:
			ncineSection_->insert(Names::nCine::platform, Names::nCine::platformAndroid);
			break;
		case Platform::EMSCRIPTEN:
			ncineSection_->insert(Names::nCine::platform, Names::nCine::platformEmscripten);
			break;
		case Platform::UNSPECIFIED:
			ncineSection_->insert(Names::nCine::platform, "");
			break;
	}
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

Configuration::AndroidArch Configuration::androidArch() const
{
	auto name = androidSection_->get_as<std::string>(Names::Android::architecture);
	if (name)
	{
		if (*name == Names::Android::archARMv7a)
			return AndroidArch::ARMEABI_V7A;
		else if (*name == Names::Android::archARM64)
			return AndroidArch::ARM64_V8A;
		else if (*name == Names::Android::archX86_64)
			return AndroidArch::X86_64;
	}
	return AndroidArch::UNSPECIFIED;
}

void Configuration::setAndroidArch(AndroidArch arch)
{
	switch (arch)
	{
		case AndroidArch::ARMEABI_V7A:
			androidSection_->insert(Names::Android::architecture, Names::Android::archARMv7a);
			break;
		case AndroidArch::ARM64_V8A:
			androidSection_->insert(Names::Android::architecture, Names::Android::archARM64);
			break;
		case AndroidArch::X86_64:
			androidSection_->insert(Names::Android::architecture, Names::Android::archX86_64);
			break;
		case AndroidArch::UNSPECIFIED:
			androidSection_->insert(Names::Android::architecture, "");
			break;
	}
}

bool Configuration::androidSdkDir(std::string &value) const
{
	return retrieveString(androidSection_, Names::Android::sdkDir, value);
}

void Configuration::setAndroidSdkDir(const std::string &value)
{
	androidSection_->insert(Names::Android::sdkDir, value);
}

bool Configuration::androidNdkDir(std::string &value) const
{
	return retrieveString(androidSection_, Names::Android::ndkDir, value);
}

void Configuration::setAndroidNdkDir(const std::string &value)
{
	androidSection_->insert(Names::Android::ndkDir, value);
}

bool Configuration::gradleDir(std::string &value) const
{
	return retrieveString(androidSection_, Names::Android::gradleDir, value);
}

void Configuration::setGradleDir(const std::string &value)
{
	androidSection_->insert(Names::Android::gradleDir, value);
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

bool Configuration::hasCMakePrefixPath() const
{
	return hasString(cmakeSection_, Names::CMake::prefixPath);
}

bool Configuration::cmakePrefixPath(std::string &value) const
{
	return retrieveString(cmakeSection_, Names::CMake::prefixPath, value);
}

void Configuration::setCMakePrefixPath(const std::string &value)
{
	cmakeSection_->insert(Names::CMake::prefixPath, value);
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
	executablesSection_ = root_->get_table(Names::Executables::table);
	if (executablesSection_ == nullptr)
	{
		executablesSection_ = cpptoml::make_table();
		root_->insert(Names::Executables::table, executablesSection_);
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

	androidSection_ = root_->get_table(Names::Android::table);
	if (androidSection_ == nullptr)
	{
		androidSection_ = cpptoml::make_table();
		root_->insert(Names::Android::table, androidSection_);
	}
}
