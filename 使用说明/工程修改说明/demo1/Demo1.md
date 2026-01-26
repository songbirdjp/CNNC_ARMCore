## 1. 文档说明
- 本demo针对新增片上外设，且不需要设备驱动情况时，工程修改参考
- 本demo拟实现如下功能：
  - 使用TIM6，中断周期为0.1ms，从而提供时间基准供FreeRTOS进行任务运行耗时统计
  - TIM6不占用GPIO，且不被业务需求依赖，故按工程拟定的原则，需要将其初始化放置到**main**函数内部
  - 基础分支hash: 8f895773ff55a79bed5662de1b436ddf827d4cd4
  - demo分支hash: 1b43d79b5e6d06a3eaa587a3097d763b7057e294
  
## 2. 修改方式
- 使用STM32CubeMX构建TIM6配置
![Alt text](demo1_1.png)

- 生成工程，并筛选TIM6相关代码文件
![Alt text](demo1_2.png)

- 将步骤2中文件与原文件对比，差异部分拷贝到相应的原文件中
  > main.c
  ![Alt text](demo1_3.png)
  
  > stm32h7xx_it.c
  ![Alt text](demo1_4.png)
  ![Alt text](demo1_5.png)

  > tim.c
  ![Alt text](demo1_6.png)
  ![Alt text](demo1_7.png)
  ![Alt text](demo1_8.png)
  ![Alt text](demo1_9.png)

  > tim.h
  ![Alt text](demo1_10.png)

- 重新加载运行cmakelist.txt，以更新makefile文件

- 编译文件，结果如下
  ![Alt text](demo1_11.png)
  
- 添加代码：启动TIM6，并于中断中添加打印测试
  ![Alt text](demo1_12.png)
  ![Alt text](demo1_13.png)

- 结果如下
  ![Alt text](demo1_14.png)

- 最后，添加FreeRTOS统计功能函数及实现函数即可
  ![Alt text](demo1_15.png)