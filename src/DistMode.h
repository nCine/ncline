#pragma once

class Settings;
class CMakeCommand;

class DistMode
{
  public:
	static void perform(CMakeCommand &cmake, const Settings &settings);
};
