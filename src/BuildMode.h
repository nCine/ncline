#pragma once

class Settings;
class CMakeCommand;

class BuildMode
{
  public:
	static void perform(CMakeCommand &cmake, const Settings &settings);
};
