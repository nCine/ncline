#pragma once

class Settings;
class GitCommand;

class DownloadMode
{
  public:
	static void perform(GitCommand &git, const Settings &settings);
};
