#include "BasicPlayFabSettings.h"

UBasicPlayFabSettings* UBasicPlayFabSettings::Get()
{
	return UBasicPlayFabSettings::StaticClass()->GetDefaultObject<UBasicPlayFabSettings>();
}
