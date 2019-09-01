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

		HELP
	};

	enum class DownloadMode
	{
		LIBS,
		ENGINE,
		GAME
	};

	enum class ConfMode
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

	enum class BuildMode
	{
		LIBS,
		ENGINE,
		GAME
	};

	bool parseArguments(int argc, char **argv);
	void print();

	inline Mode mode() const { return mode_; }
	inline DownloadMode downloadMode() const { return downloadMode_; }
	inline ConfMode confMode() const { return confMode_; }
	inline BuildType buildType() const { return buildType_; }
	inline BuildMode buildMode() const { return buildMode_; }

  private:
	Mode mode_ = Mode::HELP;
	DownloadMode downloadMode_ = DownloadMode::LIBS;
	ConfMode confMode_ = ConfMode::LIBS;
	BuildType buildType_ = BuildType::RELEASE;
	BuildMode buildMode_ = BuildMode::LIBS;
};
