#pragma once

/// The settings parsed from the command line arguments
class Settings
{
  public:
	enum class Mode
	{
		SET,
		DOWNLOAD,
		CONF,
		BUILD,
		DIST,

		HELP,
		VERSION
	};

	enum class Target
	{
		LIBS,
		ENGINE,
		GAME
	};

	enum class BuildType
	{
		DEBUG,
		RELEASE
	};

	bool parseArguments(int argc, char **argv);

	inline Mode mode() const { return mode_; }
	inline Target target() const { return target_; }
	inline BuildType buildType() const { return buildType_; }
	inline bool downloadArtifact() const { return downloadArtifact_; }
	inline bool clean() const { return clean_; }

  private:
	Mode mode_ = Mode::HELP;
	Target target_ = Target::LIBS;
	BuildType buildType_ = BuildType::RELEASE;
	bool downloadArtifact_ = false;
	bool clean_ = false;
};
