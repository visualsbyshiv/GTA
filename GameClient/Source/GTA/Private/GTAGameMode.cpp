#include "GTAGameMode.h"
#include "GTACharacter.h"
#include "GTAPlayerController.h"

AGTAGameMode::AGTAGameMode()
{
	DefaultPawnClass = AGTACharacter::StaticClass();
	PlayerControllerClass = AGTAPlayerController::StaticClass();
}
