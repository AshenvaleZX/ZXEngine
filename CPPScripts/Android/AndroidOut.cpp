#include "AndroidOut.h"

AndroidOut androidOut("ZXEngine");
std::ostream aOut(&androidOut);

AndroidOutWarn androidOutWarn("ZXEngine");
std::ostream aOutWarn(&androidOutWarn);

AndroidOutError androidOutError("ZXEngine");
std::ostream aOutError(&androidOutError);