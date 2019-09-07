#include <cassert>
#include "DownloadMode.h"
#include "GitCommand.h"
#include "Settings.h"
#include "Helpers.h"

namespace {

void downloadLibraries(GitCommand &git)
{
	git.clone(Helpers::nCineLibrariesRepositoryUrl());
}

void downloadEngine(GitCommand &git)
{
	git.clone(Helpers::nCineDataRepositoryUrl());
	git.clone(Helpers::nCineRepositoryUrl());
	std::string version;
	if (git.checkRepositoryVersion(Helpers::nCineSourceDir(), version))
		Helpers::info("Repository at version: ", version.data());
}

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void DownloadMode::perform(GitCommand &git, const Settings &settings)
{
	assert(settings.mode() == Settings::Mode::DOWNLOAD);

	switch (settings.downloadMode())
	{
		case Settings::DownloadMode::LIBS: downloadLibraries(git); break;
		case Settings::DownloadMode::ENGINE: downloadEngine(git); break;
		default: break;
	}
}
