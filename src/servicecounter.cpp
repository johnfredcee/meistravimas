
#include <unordered_map>
#include <string>
#include <SDL.h>
#include "hashstring.h"
#include "locator.h"
#include "servicecounter.h"

namespace venk
{
    Locator::ServiceName_t ServiceCounter::serviceCount = ServiceCounter::NO_SERVICE;
}
