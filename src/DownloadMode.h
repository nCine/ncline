#pragma once

class Settings;
class GitCommand;
class CMakeCommand;

class DownloadMode
{
  public:
	static void perform(GitCommand &git, CMakeCommand &cmake, const Settings &settings);
};
