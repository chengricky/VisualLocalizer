#include "picGnssfile.h"
#include "VisualLocalization.h"
#include "GlobalConfig.h"
#include <iostream>
#include <iomanip>
#include <cmath>


GlobalConfig GlobalConfig::config("E:/PreciseLocalization/VisualLocalizer/VisualLocalizer/Config.yaml");

int main()
{
	static GlobalConfig& config = GlobalConfig::instance();

	VisualLocalization vl(config);

	vl.featureLocalize(true);

	system("pause");
	return 0;
}
