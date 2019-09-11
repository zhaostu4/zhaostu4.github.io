#  TensorRT 加速推理实践 -- YOLO 目标检测
_作者：_ 
## TensorRT 简介
高性能深度学习推断库
## 用法：
输入训练好的模型;主要流程如下

![flow](./img/TensorRT_workflow.png "你好")

下面输入YOLOv3网络架构：

![YOLOv3](./img/Yolov3.png)

注意进行视频流处理时，需要及时free内存；

深度学习自动量化:
[深度学习神经网络量化](https://blog.csdn.net/qq_28413479/article/details/77479722)