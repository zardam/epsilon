#include "apps_container_storage.h"

#include "global_preferences.h"

#ifndef APPS_CONTAINER_SNAPSHOT_CONSTRUCTORS
#error Missing snapshot constructors
#endif

#ifndef APPS_CONTAINER_SNAPSHOT_LIST
#error Missing snapshot list
#endif

#ifndef APPS_CONTAINER_SNAPSHOT_COUNT
#error Missing snapshot count
#endif

AppsContainerStorage::AppsContainerStorage() :
  AppsContainer()
  APPS_CONTAINER_SNAPSHOT_CONSTRUCTORS
{
  size_t numberOfFiles = Ion::Archive::numberOfFiles();

  for (size_t idx = 0; idx < numberOfFiles; idx++) {
    Ion::Archive::File entry;
    Ion::Archive::fileAtIndex(idx, entry);
    if (entry.isExecutable) {
      strlcpy(m_externalAppSnapshots[m_numberOfExternalApps].descriptor()->filename, entry.name, Ion::Archive::MaxNameLength);
      m_numberOfExternalApps++;

      if (m_numberOfExternalApps == k_maxNumberOfExternalApps - 1) {
        break;
      }
    }
  }
}

int AppsContainerStorage::numberOfApps() {
  if (GlobalPreferences::sharedGlobalPreferences()->examMode() == GlobalPreferences::ExamMode::Activate) {
    return k_numberOfCommonApps;
  }
  return k_numberOfCommonApps + m_numberOfExternalApps;
}

App::Snapshot * AppsContainerStorage::appSnapshotAtIndex(int index) {
  if (index < 0) {
    return nullptr;
  }
  App::Snapshot * snapshots[] = {
    homeAppSnapshot()
    APPS_CONTAINER_SNAPSHOT_LIST
  };
  assert(sizeof(snapshots)/sizeof(snapshots[0]) == k_numberOfCommonApps);
  assert(index >= 0 && index < numberOfApps());

  if (index < k_numberOfCommonApps) {
    return snapshots[index];
  }
  return &m_externalAppSnapshots[index - k_numberOfCommonApps];
}
