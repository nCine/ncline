#include <cassert>
#include "DownloadMode.h"
#include "GitCommand.h"
#include "Settings.h"

namespace {

const char *nCineLibrariesRepositoryUrl = "https://github.com/nCine/nCine-libraries.git";
const char *nCineRepositoryUrl = "https://github.com/nCine/nCine.git";

void downloadLibraries(GitCommand &git)
{
	git.clone(nCineLibrariesRepositoryUrl);
}

void downloadEngine(GitCommand &git)
{
	git.clone(nCineRepositoryUrl);
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
