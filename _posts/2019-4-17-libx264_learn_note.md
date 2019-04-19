---
layout:     post
title:      libx264 学习笔记
subtitle:   libx264 的学习笔记
date:       2019-4-19
author:     王鹏程
header-img: img/post-bg-ios10.jpg
catalog: true
tags:
    - x264
    - C++
    - libx264
---

# libx264解码学习笔记
_参考连接：_[libx264源码地址](https://code.videolan.org/videolan/x264);[x264主页](https://www.videolan.org/developers/x264.html);[x264源代码简单分析](https://blog.csdn.net/leixiaohua1020/article/details/45536607);

音视频编码在流媒体和网络领域占有重要地位；流媒体编解码流程大致如下图所示：

![视频编解码流程图](https://img-blog.csdn.net/20180516184837257?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2dvX3N0cg==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

## x264原理解析
_参考链接：_ [x264百度词条](https://baike.baidu.com/item/x264/10787441?fr=aladdin);

### x264简介
H.264从1999年开始，到2003年形成草案，最后在2007年定稿有待核实。在ITU的标准里称为H.264，在MPEG的标准里是MPEG-4的一个组成部分--MPEG-4 Part 10，又叫Advanced Video Codec，因此常常称为MPEG-4 AVC或直接叫AVC。x264主要使用算法为H264编解码算法，下面对H264算法进行解析；

### H264 编解码解析
_参考链接：_ [H264 编解码协议详解](https://blog.csdn.net/qq_19923217/article/details/83348095);[深入浅出理解视频编码H264结构](https://www.jianshu.com/p/9522c4a7818d);[详细文档](http://read.pudn.com/downloads147/ebook/635957/%E6%96%B0%E4%B8%80%E4%BB%A3%E8%A7%86%E9%A2%91%E5%8E%8B%E7%BC%A9%E7%BC%96%E7%A0%81%E6%A0%87%E5%87%86H.264.pdf);[H264编解码框图](https://blog.csdn.net/u014253011/article/details/79967520);

#### H264编码原理：
在音视频传输过程中，视频文件的传输是一个极大的问题；一段分辨率为1920*1080,帧率是25的视频，对于传输带宽的要求是：1920*1080*25/8/1024/1024=6.179MB，意味着视频每秒带宽为6.179MB这基本是不肯能的。因此视频压缩和编码技术应运而生。
对于视频文件来说，视频由单张图片帧所组成，每秒25/32帧，但是图片帧的像素块之间存在相似性，因此视频帧图像可以进行图像压缩；H264采用了16*16的分块大小对，视频帧图像进行相似比较和压缩编码。如下图所示：

![16*16像素块分割](https://img-blog.csdnimg.cn/2019012114350437.png)

除此之外，对于一段连续的帧图像来说，前一帧和后一帧之间的关联度和相似性很高，只记录当前帧和下一帧与当前帧之间的帧数据差，能够快速的帮助我们实现视频压缩编解码。为了方便图像之间的压缩，将原图像当前帧设置为Fn,下一帧设置为Fn-1；对Fn进行帧内相似压缩编码，使用估计函数进行重新映射（图像还原）;再利用当前帧对图和帧差信息对下一帧图片进行估计和还原。具体编码框架如下图：

![H264编码流程图](https://img-blog.csdn.net/20180416220957998?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3UwMTQyNTMwMTE=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

在上图中，输入的帧或场Fn以宏块为单位被编码器处理。首先，按帧内或者帧间预測编码的方法进行处理。假设採用帧间预測编码，其预測值PRED是由当前片中前面已编码的參考图像经运动补偿（MC）后得到，当中參考图像用F'n-1表示。预測值PRED和当前块相减后，产生一个残差块Dn，经块变换、量化后产生一组量化后的变换系数X，再经熵编码，与解码所需的一些头信息一起组成压缩后的码流，经NAL（网络自适应层）供传输和存储用。

在进行编码过后H264也提供了相应的解码方式，解码流程如下图：

![H264 解码流程](https://img-blog.csdn.net/20180416221016950?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3UwMTQyNTMwMTE=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

将编码器的NAL输出的H264比特流经熵解码得到量化后的一组变换系数X，再经反量化、反变换，得到残差D'n。利用从该比特流中解码出的头信息，解码器就产生一个预測块PRED，它和编码器中的原始PRED是同样的。当该解码器产生的PRED与残差D'n相加后，就得到了uF'n，再经滤波后，最后就得到滤波后的解码输出图像F'n

##### H264中的I帧、P帧和B帧
正如上文所说;H264除了使用帧内压缩之外，还使用了帧间压缩；H264采用了独特的I帧、P帧和B帧策略来实现，连续帧之间的压缩；

![I帧、P帧和B帧结构](https://img-blog.csdn.net/20171002114031322?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvcGRzNTc0ODM0NDI0/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast);

如上图所示；

- I帧:自身可以通过视频解压算法解压成一张单独的完整的图片。
- P帧:需要参考前面的I帧或P帧,还原成完整的图片。
- B帧:需要同时参考前面和后面的I帧或P帧,来实现图片的还原。
H.264基准类中，仅使用I帧和P帧以实现低延时，因此是网络摄像机和视频编码器的理想选择。
注意：每当读取到一个新的I帧时，解码器会更新残差信息，删除之前的东西，防止错误。

#### H264编码结构解析

H264除了实现了对视频的压缩处理之外，为了方便网络传输，提供了对应的视频编码和分片策略；类似于网络数据封装成IP帧，在H264中将其称为组(gop)、片（slice）、宏块（Macroblock）这些一起组成了H264的码流分层结构

![H264 码流分层结构](https://gitee.com/kevin1993175/image_resource/raw/master/h264_layer.png)

