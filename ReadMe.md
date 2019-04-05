# Visual Localizer

If you use this code, please cite the following paper:
>Lin, S.; Cheng, R.; Wang, K.; Yang, K. Visual Localizer: Outdoor Localization Based on ConvNet Descriptor and Global Optimization for Visually Impaired Pedestrians. Sensors 2018, 18, 2476.

The code utilizes the ideas of Visual Localizer, which uses the compressed feature maps of ConvNet as image descriptors and uses Network Flow as the global optiomization tool.

Denpendency: 
+ or-tools_VisualStudio2017-64bit_v6.7.4973
+ OpenCV 4



PlaceRecognitionMain.cpp\
|----read configure file\
|----invoke localization function: VisualLocalization.featureLocalize()\

        VisualLocalization.h/.cpp\
        |----NetworkFlow/BruteForce Matching\
        |----file out results\
