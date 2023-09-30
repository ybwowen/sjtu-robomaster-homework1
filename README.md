## 装甲板识别

宏参数说明：

`#define DEBUG` 调试模式 方便利用TrackBar调整参数 去掉该宏会直接输出视频

（由于我电脑屏幕比较小，因此在DEBUG模式下，视频画面会缩小一半以方便我调试）

`#define HSV` 是否使用HSV提取的方法进行二值化，否则利用三通道分离的方法进行二值化

`#define ROTATED` 是否使用旋转矩形拟合，否则会利用`approxPolyDp`进行拟合

命名空间说明：

`namespace HSVColor` 封装了利用HSV颜色空间提取颜色进行二值化的方法

`namespace RGBChannels`封装了直接利用三通道分离进行二值化的方法

代码后续可能还要进行进一步的修改