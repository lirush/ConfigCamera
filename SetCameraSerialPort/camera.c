#include "camera.h"
#include <string.h>

struct ControlAndSettings SettingsCamera;

void SetSettingsCamera()
{
    struct ControlAndSettings *ptr = &SettingsCamera;

    memset(ptr,0x00,sizeof(struct ControlAndSettings));

}
