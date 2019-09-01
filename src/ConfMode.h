#pragma once

class Settings;
class CMakeCommand;

class ConfMode
{
  public:
	static void perform(CMakeCommand &cmake, const Settings &settings);
};
